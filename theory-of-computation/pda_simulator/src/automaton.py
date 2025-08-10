import copy

"""
Pushdown Automaton (PDA)

This implementation is based largely on the description given by Critchlow
and Eck in their textbook, Foundations of Computation.

A PDA M is a 6-tuple (Q, Σ, Λ, q0, ∂, F) where:
Q - set of states
Σ - input alphabet
Λ - stack alphabet
q0 - start state
∂ - set of transitions
F - set of accepting states

M accepts a string if and only if it reads all of the string and finishes
in the accept state with an empty stack.

A transition can have one of the following forms, where ε denotes
the empty string:

1. (start_state, ε, ε) -> (accept_state, start_symbol)
    - This is the first step for M. It moves to the accept state and pushes
      the start symbol on the stack, without reading any input.

2. (accept_state, terminal, terminal) -> (accept_state, ε)
    - If a terminal symbol at the top of the stack matches the next symbol in
      the string, M can read that symbol from the string and pop it off the
      stack at the same time.

3. (accept_state, ε, nonterminal) -> (accept_state, rhs)
    - If a nonterminal symbol appears at the top of the stack, M replaces the
      symbol with the right-hand side of the production rule for that symbol.
      - If the right-hand side is the empty string, don't push anything on the stack.
      - Otherwise, push symbols in reverse order (e.g., if 'abc', push 'c', 'b', 'a').
    - For each n-way decision (i.e. if multiple transition rules exist),
      create n - 1 copies of the machine/stack. Every machine will
      apply one of the production rules.
    - M performs this step without reading any input.
"""

class Pda:
    states = None
    input_alphabet = None
    stack_alphabet = None
    start_state = None
    transitions = None
    accept_state = None
    current_state = None
    new_machines = None
    EMPTY = "_EMPTY_"

    def __init__(self, input_string, stack=[]):
        self.input_string = input_string
        self.stack = stack
        self.done = False

    def stop(self):
        self.done = True

    def accepts(self):
        return len(self.input_string) == 0 and \
            Pda.current_state == Pda.accept_state and \
            len(self.stack) == 0

    def push_on_stack(self, rhs):
        if rhs != Pda.EMPTY:
            self.stack.extend(reversed(rhs.split()))

    def handle_terminal_symbol(self, top):
        if len(self.input_string) > 0 and top == self.input_string[0]:
            self.stack.pop()
            self.input_string.popleft()
        else:
            self.stop()

    def handle_nonterminal_symbol(self, top):
        transition = Pda.transitions.get((Pda.accept_state, Pda.EMPTY, top))
        self.stack.pop()
        
        productions = transition[1]
        for i in range(1, len(productions)):
            clone = Pda(copy.deepcopy(self.input_string), copy.deepcopy(self.stack))
            clone.push_on_stack(productions[i])
            Pda.new_machines.append(clone)

        self.push_on_stack(productions[0])

    def compute(self):
        if Pda.current_state == Pda.start_state:
            transition = Pda.transitions.get((Pda.start_state, Pda.EMPTY, Pda.EMPTY))
            Pda.current_state = Pda.accept_state
            start_symbol = transition[1]
            self.stack.append(start_symbol)
        elif len(self.stack) > 0:
            top = self.stack[-1]
            if top in Pda.input_alphabet:
                self.handle_terminal_symbol(top)
            else:
                self.handle_nonterminal_symbol(top)
        else:
            self.stop()
