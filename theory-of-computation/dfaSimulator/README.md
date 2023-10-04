# DFA Simulator
Simulates a deterministic finite automaton (DFA), given a DFA file and a string.

### DFA File Format
A DFA file has the following format:

- a line containing a list of states, separated by commas
- a line containing the input alphabet, separated by commas
- a line containing the start state
- a line containing a list of accept states, separated by commas
- a list of transitions, one per line
  - Transitions have the form **(currentState,inputSymbol)->(nextState)**

Several examples of valid DFA files are provided.

### Commands
Compile with:
```
javac DFA.java
```
Run with:
```
java DFA dfaFile.dfa inputString
```
