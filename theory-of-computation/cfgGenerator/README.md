# CFG Generator
Randomly generates strings from a context-free grammar (CFG), given a CFG file and the number of strings to generate.

### CFG File Format
A CFG file has the following format:

- a line containing a list of non-terminal symbols, separated by commas
- a line containing a list of terminal symbols, separated by commas
- a list of production rules, one per line
  - Production rules have the form **LHS -> RHS**, where LHS is a non-terminal symbol and RHS is a string of symbols, separated by spaces. **\_EMPTY\_** is used to denote the empty string.
- a line containing the start symbol

Several examples of valid CFG files are provided.

### Commands
Compile with:
```
javac CFG.java
```
Run with:
```
java CFG cfgFile.cfg numRandomStrings
```
