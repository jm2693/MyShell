#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <glob.h>

#define PATHS { "/usr/local/bin/", "/usr/bin/", "/bin/" }
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define NUM_PATHS 3

void print_prompt() {
    write(STDOUT_FILENO, "mysh> ", 6);
}

// doesn't use access
int execute_command(char **args, int input_fd, int output_fd) {
    pid_t pid;
    int status;
    char *paths[] = PATHS;
    
    for (int i = 0; i < NUM_PATHS; i++) {
        pid = fork();
        if (pid == 0) {
            // Child process
            if (input_fd != STDIN_FILENO) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            if (output_fd != STDOUT_FILENO) {
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }
            // Construct the full path to the command
            char *full_path = malloc(strlen(paths[i]) + strlen(args[0]) + 1);
            strcpy(full_path, paths[i]);
            strcat(full_path, args[0]);
            //if (access(full_path, X_OK)) hmmmmm
            execv(full_path, args);
            // If execv returns, it means an error occurred
            free(full_path);
            _exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Fork failed
            perror("fork");
            return 1;
        } else {
            // Parent process
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);
                if (exit_status == 0) {
                    return exit_status;
                }
            }
        }
    }
    // If none of the paths were successful
    printf("Command not found: %s\n", args[0]);
    return 1;
}

// Function to handle wildcard expansion
int handle_wildcard(char **args) {
    int i, j, num_args = 0;
    glob_t glob_result;
    int flags = 0; // GLOB_NOCHECK to return pattern itself if no matches found

    // Count the number of arguments
    while (args[num_args] != NULL) {
        num_args++;
    }

    for (i = 0; i < num_args; i++) {
        if (strchr(args[i], '*') != NULL) {
            if (glob(args[i], flags, NULL, &glob_result) == 0) {
                // Replace the wildcard argument with the expanded filenames
                for (j = 0; j < glob_result.gl_pathc; j++) {
                    args[i + j] = strdup(glob_result.gl_pathv[j]);
                }
                // Shift the remaining arguments to accommodate the expansion
                for (j = num_args - 1; j >= i + 1; j--) {
                    args[j + glob_result.gl_pathc - 1] = args[j];
                }
                // Update the number of arguments
                num_args += glob_result.gl_pathc - 1;
                args[num_args] = NULL; // Null-terminate the argument list
                globfree(&glob_result);
            }
        }
    }

    return num_args;
}

int main(int argc, char *argv[]) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int num_args;
    int exit_status = 0;

    int input_fd = STDIN_FILENO;
    if (argc > 1) {
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    if (isatty(input_fd)) {
        write(STDOUT_FILENO, "Welcome to my shell!\n", 21);
    }

    while (1) {
        if (isatty(input_fd)) {
            print_prompt();
        }

        // Read input using read() instead of fgets()
        ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
        if (bytes_read <= 0) {
            // End of input stream
            break;
        }
        // Null terminate the string
        command[bytes_read] = '\0';

        // Parse command into arguments
        num_args = 0;
        char *token = strtok(command, " \n");
        while (token != NULL) {
            args[num_args++] = token;
            token = strtok(NULL, " \n");
        }
        args[num_args] = NULL;

        // Check for redirection and piping
        int redirect_index = -1;
        int pipe_index = -1;
        for (int i = 0; i < num_args; i++) {
            if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0) {
                redirect_index = i;
                break;
            } else if (strcmp(args[i], "|") == 0) {
                pipe_index = i;
                break;
            }
        }

        if (redirect_index != -1 || pipe_index != -1) {
            // Handle redirection or piping
            int input = STDIN_FILENO;
            int output = STDOUT_FILENO;

            if (redirect_index != -1) {
                // Redirect input/output
                if (redirect_index + 1 < num_args) {
                    // Set input file descriptor
                    input = open(args[redirect_index + 1], O_RDONLY);
                    if (input == -1) {
                        perror("open");
                        continue;
                    }
                }
                if (redirect_index - 1 >= 0) {
                    // Set output file descriptor
                    output = open(args[redirect_index - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output == -1) {
                        perror("open");
                        continue;
                    }
                }
            } else if (pipe_index != -1) {
                // Create pipe
                int pipefd[2];
                if (pipe(pipefd) == -1) {
                    perror("pipe");
                    continue;
                }
                // Fork child process
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    continue;
                } else if (pid == 0) {
                    // Child process
                    close(pipefd[0]); // Close read end of pipe
                    dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
                    close(pipefd[1]); // Close write end of pipe
                    // Execute command before pipe
                    execvp(args[0], args);
                    perror("execvp");
                    exit(EXIT_FAILURE);
                } else {
                    // Parent process
                    close(pipefd[1]); // Close write end of pipe
                    input = pipefd[0]; // Set input for next command to read from pipe
                }
            }

            // Execute command
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);

            // Close file descriptors
            if (input != STDIN_FILENO)
                close(input);
            if (output != STDOUT_FILENO)
                close(output);
        } else {
            // No redirection or piping, execute command
            exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
        }
    }

    if (isatty(input_fd)) {
        write(STDOUT_FILENO, "Exiting my shell.\n", 18);
    }

    if (argc > 1) {
        close(input_fd);
    }

    return exit_status;
}