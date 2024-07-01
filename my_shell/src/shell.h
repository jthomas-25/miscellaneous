#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>

extern const int MAX_NUM_ARGS;

void print_error_msg();
void print_command(char *cmdline);
bool strequal(char *str1, char *str2);
void get_args(char *inputline, char *args[], int *num_args);
void parse_command(char *args[], int num_args, bool *done);
void execute_command(char *args[], int num_args);
