#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 1024


void run_shell_loop (int mode_fd) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_COMMAND_LENGTH-1];
    int is_interactive = isatty(STDIN_FILENO);      

    while (1) {
        if (is_interactive) {
            printf("mysh> ");
        }

        ssize_t bytes_read = read(mode_fd, command, MAX_COMMAND_LENGTH);

        ssize_t bytes_read = read(mode_fd, command, MAX_COMMAND_LENGTH);
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
        printf("The program, %s, takes in either 0 or 1 arguments\n", argv[0]);
        return 1;
    }

    // 
    int input_fd = STDIN_FILENO;

    if (argc == 2) {
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            perror("Error opening file");
            return 1;
        }
    }

    run_shell_loop(input_fd);

    if (argc == 2) close(input_fd);

    return 0;
}
