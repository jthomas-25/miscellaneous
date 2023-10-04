#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "memory_system.h"
#include "bit_functions.h"
#include "cpu.h"
#include "isa.h"

static int registers[16];

static int cpsr; // status register

/* For future implementation of breakpoints
 *
#define NUM_BREAKPOINTS 2
static int breakpoints[NUM_BREAKPOINTS]; // allow 2 break points
 */

void set_reg(int reg, int value) {
    registers[reg] =  value;
}

int get_reg(int reg) {
    return registers[reg];
}

int get_cpsr() {
    return cpsr;
}

void update_cpsr(int result) {
    if (result > 0) {
        bit_clear(&cpsr, N);    //clear negative flag
        bit_clear(&cpsr, Z);    //clear zero flag
    } else if (result < 0) {
        bit_set(&cpsr, N);      //set negative flag
        bit_clear(&cpsr, Z);    //clear zero flag
    } else {
        bit_clear(&cpsr, N);    //clear negative flag
        bit_set(&cpsr, Z);      //set zero flag
    }
}

void show_regs() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int reg = i*4+j;
            char *spaces = (reg < R10) ? "  " : " ";
            printf("R%d:%s0x%08x", reg, spaces, get_reg(reg));
            if (j < 3)
                printf(", ");
        }
        printf("\n");
    }
}

void step() {
    int pc = registers[PC];
    int inst;
    system_bus(pc, &inst, READ);
    decoded *d = decode(inst);
    if (d == NULL)
        exit(-1);
    switch (d->opcode) {
        int start_address;
        int result;
        int value;
        case LDR:
            system_bus(d->address, &registers[d->rd], READ);
            set_reg(PC, pc + 4);
            break;
        case STR:
            system_bus(d->address, &registers[d->rd], WRITE);
            set_reg(PC, pc + 4);
            break;
        case LDX:
            start_address = get_reg(d->rn) + d->offset;
            system_bus(start_address, &registers[d->rd], READ);
            set_reg(PC, pc + 4);
            break;
        case STX:
            start_address = get_reg(d->rn) + d->offset;
            system_bus(start_address, &registers[d->rd], WRITE);
            set_reg(PC, pc + 4);
            break;
        case MOV:
            value = (d->flag) ? get_reg(d->rn) : d->immediate;
            set_reg(d->rd, value);
            if (d->rd != PC)
                set_reg(PC, pc + 4);
            break;
        case ADD:
            result = get_reg(d->rm) + get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case SUB:
            result = get_reg(d->rm) - get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case MUL:
            result = get_reg(d->rm) * get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case DIV:
            result = get_reg(d->rm) / get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case AND:
            result = get_reg(d->rm) & get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case ORR:
            result = get_reg(d->rm) | get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case EOR:
            result = get_reg(d->rm) ^ get_reg(d->rn);
            set_reg(d->rd, result);
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case CMP:
            result = (d->flag) ? get_reg(d->rd) - get_reg(d->rn) : get_reg(d->rd) - d->immediate;
            update_cpsr(result);
            set_reg(PC, pc + 4);
            break;
        case B:
            switch (d->condition) {
                case BAL:
                    set_reg(PC, d->address);
                    break;
                case BEQ:
                    if (bit_test(cpsr, Z))  //if zero flag is set
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BNE:
                    if (!bit_test(cpsr, Z)) //if zero flag is clear
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BLE:
                    if (bit_test(cpsr, Z) || (bit_test(cpsr, N) != bit_test(cpsr, V)))   //if zero flag or negative flag is set
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BLT:
                    if (bit_test(cpsr, N) != bit_test(cpsr, V)) //if negative flag is set
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BGE:
                    if (bit_test(cpsr, N) == bit_test(cpsr, V)) //if negative flag is clear
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BGT:
                    if (!bit_test(cpsr, Z) && (bit_test(cpsr, N) == bit_test(cpsr, V))) //if zero flag and negative flag are clear
                        set_reg(PC, d->address);
                    else
                        set_reg(PC, pc + 4);
                    break;
                case BL:
                    set_reg(LR, pc + 4);    //set return address to instruction after branch
                    set_reg(PC, d->address);
                    break;
            }
            break;
    }
    printf("PC: 0x%08x, inst: 0x%x, %s\n", get_reg(PC), inst, disassemble(d));
    printf("CPSR: 0x%08x\n", cpsr);
    free(d);    //clean up decoded instruction
}

void step_n(int n) {
    for (int i = 0; i < n; i++)
        step();
}

void step_show_reg() {
    int old_regs[16];
    for (int i = R0; i <= R15; i++)
        old_regs[i] = get_reg(i);
    step();
    for (int i = R0; i <= R15; i++) {
        int current_reg = get_reg(i);
        if (current_reg != old_regs[i])
            printf("reg[%d]: before: 0x%08x, after: 0x%08x\n", i, old_regs[i], current_reg);
    }
}

void step_show_reg_mem() {
    int words[256];
    for (int p = 0; p < 0x400; p+=4)
        system_bus(p, &words[p/4], READ);
    step_show_reg();
    for (int p = 0; p < 0x400; p+=4) {
        int value;
        system_bus(p, &value, READ);
        if (value != words[p/4])
            printf("addr: 0x%03x, before: 0x%08x, after: 0x%08x\n", p, words[p/4], value);
    }
}

