# Enzyme
Sorts a given lowercase string in alphabetical order by using POSIX threads, a.k.a. Pthreads, to swap characters.

The maximum length of the string is defined in ```enzyme.h```.

Note: If the string contains the **'C'** character, then the program sleeps for 5 seconds before exiting. This is to test both canceling an enzyme thread and running an independent sleeper thread.

### Commands
Build with the included makefile:
```
make
```
Run with:
```
./enzyme string
```
