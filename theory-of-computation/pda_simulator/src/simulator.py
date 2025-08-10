#!/usr/bin/python3

import sys
import re
import time
from threading import Timer
from collections import deque
from enum import Enum
from automaton import Pda

timed_out = False

def timeout():
    global timed_out
    timed_out = True

class Result(Enum):
    ACCEPTED = True
    REJECTED = False

def read_cfg_file(filename):
    with open(filename, "r") as file:
        nonterminals = [re.match(r"(\S+):", line).group(1) for line in file]
        terminals = set()
        production_rules = {}
        start_symbol = nonterminals[0]

        file.seek(0)
        for line in file:
            nonterminal, rhs = line.rstrip().split(": ")
            # skip whitespace and pipe characters
            for symbol in re.findall(r"[^\s\|]+", rhs):
                if symbol == Pda.EMPTY or symbol in nonterminals:
                    continue
                terminals.add(symbol)
            productions = rhs.split(" | ")
            production_rules[nonterminal] = productions

    return (nonterminals, terminals, production_rules, start_symbol)

def convert_to_pda(nonterminals, terminals, production_rules, start_symbol):
    states = ["q0", "q1"]
    input_alphabet = list(terminals)
    stack_alphabet = input_alphabet + nonterminals
    start_state, accept_state = states

    transitions = {}
    transitions[(start_state, Pda.EMPTY, Pda.EMPTY)] = (accept_state, start_symbol)
    for symbol in input_alphabet:
        transitions[(accept_state, symbol, symbol)] = (accept_state, Pda.EMPTY)
    for lhs, rhs in production_rules.items():
        transitions[(accept_state, Pda.EMPTY, lhs)] = (accept_state, rhs)
    
    Pda.states = states
    Pda.input_alphabet = input_alphabet
    Pda.stack_alphabet = stack_alphabet
    Pda.start_state = start_state
    Pda.transitions = transitions
    Pda.accept_state = accept_state

def parse_input(input_string):
    queue = deque()
    if len(input_string) > 0:
        symbol_patterns = [re.escape(s) for s in Pda.input_alphabet]
        symbol_patterns.append(r"(?P<BAD_SYMBOL>\S)")
        for match in re.finditer("|".join(symbol_patterns), input_string):
            value = match.group()
            if match.lastgroup == "BAD_SYMBOL":
                raise RuntimeError(f"Malformed string: unexpected symbol {value!r}.")
            queue.append(value)
    
    return queue

def step(machines):
    for m in machines:
        if not m.done:
            m.compute()
            if m.done and m.accepts():
               return Result.ACCEPTED

    if len(Pda.new_machines) == 0 and all(m.done for m in machines):
        return Result.REJECTED

def main():
    try:
        usage_error_msg = "Usage: simulator.py cfgfilename.cfg inputstring [timeout]"
        num_args = len(sys.argv) - 1
        if not (num_args == 2 or num_args == 3):
            raise RuntimeError(usage_error_msg)

        filename = sys.argv[1]
        if not filename.endswith(".cfg"):
            raise RuntimeError(usage_error_msg)
        
        timer = None
        if num_args == 3:
            duration = float(sys.argv[3])
            if duration <= 0:
                raise OverflowError("Timeout value must be greater than 0.")
            timer = Timer(duration, timeout)

        cfg = read_cfg_file(filename)
        convert_to_pda(*cfg)
        
        queue = parse_input(sys.argv[2])
        machines = [Pda(queue)]
        Pda.current_state = Pda.start_state
        Pda.new_machines = deque()

        result = None
        if timer:
            timer.start()
        while not (result or timed_out):
            while len(Pda.new_machines) > 0:
                machines.append(Pda.new_machines.popleft())
            result = step(machines)
            time.sleep(0.1)
        if timer:
            timer.cancel()
        
        if result is Result.ACCEPTED:
            print("Accepted!")
        elif result is Result.REJECTED:
            print("Rejected.")
        elif timed_out:
            print("Timed out.")
    except Exception as e:
        print(e)
        sys.exit(1)

if __name__ == "__main__":
    main()
