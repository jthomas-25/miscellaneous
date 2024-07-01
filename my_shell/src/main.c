#include "shell.h"

const int MAX_LINE_LENGTH = 512;    // no more than 512 characters of input

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // batch mode
        char *filename = argv[1];
        FILE *batch_file = fopen(filename, "r");
        // file does not exist
        if (batch_file == NULL) {
            print_error_msg();
            exit(EXIT_FAILURE);
        }

        char inputline[MAX_LINE_LENGTH * 2];        // input buffer
        char *args[MAX_NUM_ARGS + 1];               // list of arguments to pass to exec() (must end with NULL)

        bool done = false;
        while (fgets(inputline, sizeof(inputline), batch_file) != NULL) {
            // need a copy of input to print since strtok() modifies the original string
            char buffer[sizeof(inputline)];
            char *copyofinput = strncpy(buffer, inputline, sizeof(buffer));
            print_command(copyofinput);

            inputline[strlen(inputline)-1] = '\0';  // need to remove newline char for exec()
            // check if command line is too long
            if (strlen(inputline) > MAX_LINE_LENGTH) {
                print_error_msg();
                //inputline[strlen(inputline) + 1] = '\n';
                while (getchar() != '\n');          // clear input buffer
                continue;                           // continue processing 
            }
            
            int num_args;
            get_args(inputline, args, &num_args);   // get arguments
            // empty command line or multiple whitespaces on the command line
            if (num_args == 0) {
                continue;                           // continue processing
            }
            parse_command(args, num_args, &done);
            //write(STDOUT_FILENO, "\n", 1);
            
            // exit command encountered, terminate shell
            if (done) {
                fclose(batch_file);
                exit(EXIT_SUCCESS);
            }
        }
        fclose(batch_file);                         // end of file reached
        exit(EXIT_SUCCESS);
    }
    else if (argc == 1) {
        // interactive mode
        char inputline[MAX_LINE_LENGTH * 2];            // input buffer
        char *args[MAX_NUM_ARGS + 1];                   // list of arguments to pass to exec() (must end with NULL)

        bool done = false;
        do {
            printf("mysh> ");                           // shell prompt
            fgets(inputline, sizeof(inputline), stdin); // get user input
            inputline[strlen(inputline) - 1] = '\0';    // need to remove newline char for exec()
            // check if command line is too long
            if (strlen(inputline) > MAX_LINE_LENGTH) {
                print_error_msg();
                //inputline[strlen(inputline) + 1] = '\n';
                while (getchar() != '\n');              // clear input buffer
                continue;                               // continue processing 
            }
            
            int num_args;
            get_args(inputline, args, &num_args);       // get arguments
            // empty command line or multiple whitespaces on the command line
            if (num_args == 0) {
                continue;                               // continue processing
            }
            parse_command(args, num_args, &done);
        } while (!done);
        exit(EXIT_SUCCESS);
    }
    else {
        // invalid number of arguments
        print_error_msg();
        exit(EXIT_FAILURE);
    }

    return 0;
}
