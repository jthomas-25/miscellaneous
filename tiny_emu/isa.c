#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cpu.h"
#include "isa.h"

static char *opcodes[] = {"nothing", "ldr", "str",  "ldx", "stx", "mov", "add", "sub", "mul", "div", "and", "orr", "eor", "cmp", "b"};
static int opcodes_val[] = {0, LDR, STR,  LDX, STX, MOV, ADD, SUB, MUL, DIV, AND, ORR, EOR, CMP, B};
static char *reg_strs[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
static char *branches[] = {"b", "beq", "bne", "ble", "blt", "bge", "bgt"};

char *opcode_str(int opcode) {
    for (int i = 1; i < NUM_OPCODES+1; i++)
        if (opcode == opcodes_val[i])
            return opcodes[i];
    return NULL;
}

char disassembled[100]; // holds a disassembled instruction
char *disassemble(decoded *d) {
    char *p = disassembled;
    if (d->opcode != B) {
        p = strcpy(p, opcode_str(d->opcode));
        p = strcat(p, " ");
        p = strcat(p, reg_strs[d->rd]);
        p = strcat(p, ", ");
    }
    char buf[25];
    switch (d->opcode) {
      case LDR: case STR:
        sprintf(buf, "0x%x", d->address);
        p = strcat(p, buf);
        break;
      case LDX: case STX:
        p = strcat(p, "[");
        p = strcat(p, reg_strs[d->rn]);
        p = strcat(p, ", ");
        sprintf(buf, "#0x%x", d->offset);
        p = strcat(p, buf);
        p = strcat(p, "]");
        break;
      case MOV: case CMP:
        if (d->flag)
            p = strcat(p, reg_strs[d->rn]);
        else {
            sprintf(buf, "#0x%x", d->immediate);
            p = strcat(p, buf);
        }
        break;
      case ADD: case SUB: case MUL: case DIV: case AND: case ORR: case EOR:
        p = strcat(p, reg_strs[d->rm]);
        p = strcat(p, ", ");
        p = strcat(p, reg_strs[d->rn]);
        break;
      case B:
        if (d->condition == 0x80)
            p = strcpy(p, "BL");
        else
            p = strcpy(p, branches[d->condition]);
        p = strcat(p, " ");
        sprintf(buf, "0x%x", d->address);
        p = strcat(p, buf);
        break;
      default:
        p = strcat(p, "BAD");
    }
    return disassembled;
}

decoded *decode(unsigned int inst) {
    decoded *d;
    int opcode;
    int rd;
    int rm;
    int rn;
    int flag;
    int address;
    int immediate;
    int offset;
    int condition;
    opcode = inst>>24;                      //get bits 24-31
    switch (opcode) {
        case LDR: case STR:
            rd = (inst>>16) & 0xff;         //rd gets bits 16-23
            if (rd > R15)
                return NULL;
            address = inst & 0xffff;        //address gets bits 0-15
            if (address > 0x3ff)
                return NULL;
            d = malloc(sizeof(decoded));
            d->opcode = opcode;
            d->rd = rd;
            d->address = address;
            return d;
        case LDX: case STX:
            rd = (inst>>16) & 0xff;         //rd gets bits 16-23
            if (rd > R15)
                return NULL;
            offset = (inst>>8) & 0xff;      //offset gets bits 8-15
            rn = inst & 0xff;               //rn gets bits 0-7
            if (rn > R15)
                return NULL;
            d = malloc(sizeof(decoded));
            d->opcode = opcode;
            d->rd = rd;
            d->offset = offset;
            d->rn = rn;
            return d;
        case MOV: case CMP:
            rd = (inst>>16) & 0xff;         //rd gets bits 16-23
            if (rd > R15)
                return NULL;
            flag = (inst>>15) & 1;          //flag gets bit 15
            if (flag) {                     //test if flag is set
                rn = inst & 0xff;           //rn gets bits 0-7
                if (rn > R15)
                    return NULL;
            } else {
                immediate = inst & 0xffff;  //immediate gets bits 0-15
            }
            d = malloc(sizeof(decoded));
            d->opcode = opcode;
            d->rd = rd;
            d->flag = flag;
            if (d->flag)
                d->rn = rn;
            else
                d->immediate = immediate;
            return d;
        case ADD: case SUB: case MUL: case DIV: case AND: case ORR: case EOR:
            rd = (inst>>16) & 0xff;         //rd gets bits 16-23
            if (rd > R15)
                return NULL;
            rm = (inst>>8) & 0xff;          //rm gets bits 8-15
            if (rm > R15)
                return NULL;
            rn = inst & 0xff;               //rn gets bits 0-7
            if (rn > R15)
                return NULL;
            d = malloc(sizeof(decoded));
            d->opcode = opcode;
            d->rd = rd;
            d->rm = rm;
            d->rn = rn;
            return d;
        case B:
            condition = (inst>>16) & 0xff;  //condition gets bits 16-23
            address = inst & 0xffff;        //address gets bits 0-15
            if (address > 0x3ff)
                return NULL;
            d = malloc(sizeof(decoded));
            d->opcode = opcode;
            d->condition = condition;
            d->address = address;
            return d;
        default:
            return NULL;
    }
}
