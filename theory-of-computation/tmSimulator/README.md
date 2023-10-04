# TM Simulator
Simulates a Turing Machine (TM), given a TM file and a string.

### TM File Format
A TM file has the following format:
    - a line containing a list of states, separated by commas
    - a line containing the input alphabet, separated by commas
    - a line containing the tape alphabet, separated by commas. _ is used to denote the blank symbol.
    - a list of transitions, one per line
      - Transitions have the form **(currentState,readSymbol)->(writeSymbol,nextState,direction)**
    - a line containing the start state
    - a line containing the accept state
    - a line containing the reject state

Several examples of valid TM files are provided.

### Commands
Compile with:
```
javac TM.java
```
Run with:
```
java TM tmFile.tm inputString ["verbose"]
```
If the optional "verbose" argument is included, the tape will be displayed after each step of the computation.
