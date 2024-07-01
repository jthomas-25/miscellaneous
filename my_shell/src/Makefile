CC = gcc
CFLAGS = -g -Wall

default: mysh

mysh: main.o shell.o
	$(CC) $(CFLAGS) -o mysh main.o shell.o

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f mysh *.o
