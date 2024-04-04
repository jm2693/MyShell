#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 1024

// shell loop that will constantly check for input and parse arguments
void run_shell_loop () {
    char *command;
    char **args;
    int is_interactive = isatty(STDIN_FILENO);      

    while (1) {
        if (is_interactive) {
            printf("mysh> ");
        }

        ssize_t bytes_read = read(STDIN_FILENO, command, MAX_COMMAND_LENGTH);

        ssize_t bytes_read = read(STDIN_FILENO, command, MAX_COMMAND_LENGTH);
        if (bytes_read < 0) {
            printf("Error reading command\n");
            continue;
        } else if (bytes_read == 0) {
            if (is_interactive)
                printf("End of input. Exiting shell program.\n");
            break;
        }
    } 

}




int main(int argc, char const *argv[])
{
    // this program takes in at most 2 arguments including the program name. any more results in error
    if (argc > 2) {
        printf("The program, %s, takes in either 0 or 1 argument(s)\n", argv[0]);
        return 1;
    }

    // one major loop that runs the shell in both modes
    run_shell_loop();

    return EXIT_SUCCESS;
}
