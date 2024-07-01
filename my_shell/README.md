# My Shell
A simple command-line interpreter (shell) based on Bash.

### Features
- Interactive mode
- Batch mode to execute multiple commands from a file
- Built-in and Linux commands
- Running processes in the background

### Built-in Commands
- ```exit```: Terminates the shell.
- ```cd [dir]```: Changes the present working directory to the specified _subdirectory_. If no directory is given, goes to the home directory.
- ```pwd```: Prints the present working directory.
- ```wait```: Waits for all child processes to finish.

### Building and Running
Build with the included makefile:
```
make
```
Run in interactive mode with:
```
./mysh
```
Run in batch mode with:
```
./mysh filename
```
