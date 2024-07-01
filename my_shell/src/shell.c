#include "shell.h"

const int MAX_NUM_ARGS = 32;    // no more than 32 arguments in a command, minus NULL
char error_message[30] = "An error has occurred\n";

void print_error_msg() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void print_command(char *cmdline) {
    write(STDOUT_FILENO, cmdline, strlen(cmdline));
}

bool strequal(char *str1, char *str2) {
    return strcmp(str1, str2) == 0;
}

void get_args(char *inputline, char *args[], int *num_args) {
    int i = 0;
    // assume arguments are separated by whitespace characters
    char *token = strtok(inputline, " \t\n");
    while (token != NULL) {
        args[i] = token;
        token = strtok(NULL, " \t\n");
        i++;
        // check if arguments list is full (leaves room for NULL)
        if (i == MAX_NUM_ARGS) {
            break;
        }
    }
    // terminate the arguments list with NULL before passing to exec()
    args[i] = NULL;
    // save the number of arguments
    *num_args = i;
}

void parse_command(char *args[], int num_args, bool *done) {
    // check for exit command
    if (strequal(*args, "exit")) {
        if (num_args == 1) {
            *done = true;                           // notify the shell to terminate
        }
        else {
            print_error_msg();                      // too many arguments
        }
    }
    // check for cd command
    else if (strequal(*args, "cd")) {
        if (num_args == 1) {
            chdir(getenv("HOME"));                  // go to the home directory
        }
        else if (num_args == 2) {
            chdir(args[1]);                         // go to the specified directory
        }
        else {
            print_error_msg();                      // too many arguments
        }
    }
    // check for command to get the present working directory
    else if (strequal(*args, "pwd")) {
        if (num_args == 1) {
            char cwd[PATH_MAX];                     // need a buffer to hold the pathname
            printf("%s\n", getcwd(cwd, sizeof(cwd)));
        }
        else {
            print_error_msg();                      // too many arguments
        }
    }
    // check for wait command
    else if (strequal(*args, "wait")) {
        if (num_args == 1) {
            int pid;
            int status;
            while ((pid = wait(&status)) != -1);    // wait for all children to finish
        }
        else {
            print_error_msg();                      // too many arguments
        }
    }
    else {
        execute_command(args, num_args);  // not a built-in command
    }
}

void execute_command(char *args[], int num_args) {
    bool found_ampersand = false;
    if (strequal(args[num_args-1], "&")) {
        args[num_args-1] = NULL;
        found_ampersand = true;
    }
    int pid = fork();
    // failed to fork
    if (pid < 0) {
        print_error_msg();
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // command does not exist or cannot be executed, exit child process
        if (execvp(*args, args) == -1) {
            print_error_msg();
            exit(EXIT_FAILURE);
        }
    }
    else {
        // if there's no trailing ampersand
        if (!found_ampersand) {
            // shell waits for child to finish
            int status;
            while (wait(&status) != pid);
            //waitpid(-1, &status, WNOHANG);
        }
        // otherwise, launch the process in the background, but don't wait for it to finish
    }
}