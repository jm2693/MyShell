// WHAT WORKS: CD, PWD, EXIT, INTERACTIVE, WILDCARDS, BARENAMES, BATCH
// WHAT NEEDS WORK: 
// WHAT DOESN'T WORK: PIPING, REDIRECTING



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


int run_shell(int input_fd) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int num_paths = NUM_PATHS;
    char* paths[] = PATHS;
    int exit_status = 0;
    int prev_status = 0; // Previous command's exit status
    int in_fd = input_fd; // Input file descriptor for the current command
    int out_fd = STDOUT_FILENO; // Output file descriptor for the current command

    while (1) {
        if (isatty(input_fd)) {
            print_prompt();
        }

        // reading input 
        ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
        if (bytes_read <= 0) {
            // End of input stream
            break;
        }
        // Null terminate the string
        command[bytes_read] = '\0';

        // Parse command into arguments
        int num_args = 0;
        char *token = strtok(command, " \n");
        while (token != NULL) {
            args[num_args++] = token;
            token = strtok(NULL, " \n");
        }
        args[num_args] = NULL;

        if (strcmp(args[0], "then") == 0 || strcmp(args[0], "else") == 0) {
            // Conditional command
            if (prev_status == (strcmp(args[0], "then") == 0 ? 0 : 1)) {
                // Skip executing the command
                continue;
            }
            // Remove the conditional command from the argument list
            num_args--;
            memmove(args, args + 1, num_args * sizeof(char *));
            args[num_args] = NULL;
        }

                char *args_command [MAX_ARGS];
        char *args_redirect[MAX_ARGS];
        int args_inc1 = 0;

        if(strchr(*args, '>') != NULL || strchr(*args, '<') || strchr(*args, '|')){
            //redirection detected
            for(int i = 0; i < num_args; i++){
                if(strcmp(args[i], ">") == 0 || strcmp(args[i], "<") || strcmp(args[i], "|")){
                    args_redirect[args_inc1] = args[i];
                    args_command[args_inc1] = " ";
                    args_inc1++;
                }
                else{
                    args_command[args_inc1] = args[i];
                    args_redirect[args_inc1] = " "; 
                    args_inc1++;  
                }
            }

            for(int i = 0; i < args_inc1; i++){
                if(strcmp(args_redirect[i], "<") == 0 || strcmp(args_redirect[i], "|") == 0){
                    int input = open(args_command[i--], O_RDONLY);
                    int output = open(args_command[i++], O_WRONLY, 0640);
                    execute_command(args, input, output); 
                }
                else if(strcmp(args_redirect[i], ">") == 0){
                    int input = open(args_command[i++], O_RDONLY);
                    int output = open(args_command[i--], O_WRONLY, 0640);
                    execute_command(args, input, output); 
                }
            }

        }

        // if (strstr(command, "|") != NULL) {
        //     // Pipeline detected
        //     int pipefd[2];
        //     if (pipe(pipefd) == -1) {
        //         perror("pipe");
        //         return 1;
        //     }

        //     // Fork a child process to execute the first command
        //     pid_t pid1 = fork();
        //     if (pid1 == 0) {
        //         // Child process
        //         close(pipefd[0]); // Close unused read end
        //         dup2(in_fd, STDIN_FILENO); // Redirect stdin to the input file descriptor
        //         dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        //         close(pipefd[1]); // Close write end
        //         exit_status = execute_command(args, in_fd, pipefd[1]);
        //         close(pipefd[1]);
        //         _exit(exit_status); // If execute_command returns, there's an error
        //     } else if (pid1 < 0) {
        //         // Fork failed
        //         perror("fork");
        //         return 1;
        //     }

        //     // Fork another child process to execute the second command
        //     pid_t pid2 = fork();
        //     if (pid2 == 0) {
        //         // Child process
        //         close(pipefd[1]); // Close unused write end
        //         dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
        //         close(pipefd[0]); // Close read end
        //         exit_status = execute_command(args, pipefd[0], out_fd);
        //         close(pipefd[0]);
        //         _exit(exit_status); // If execute_command returns, there's an error
        //     } else if (pid2 < 0) {
        //         // Fork failed
        //         perror("fork");
        //         return 1;
        //     }

        //     // Close pipe ends in parent process
        //     close(pipefd[0]);
        //     close(pipefd[1]);

        //     // Wait for both child processes to finish
        //     waitpid(pid1, NULL, 0);
        //     waitpid(pid2, NULL, 0);
        // } 

        if (strcmp(args[0], "cd") == 0) {
            if (num_args != 2) {
                write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
                continue;
            }
            if (chdir(args[1]) != 0) {
                perror("chdir");
            }
            continue;
        }

        else if (strcmp(args[0], "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                write(STDOUT_FILENO, cwd, strlen(cwd));
                write(STDOUT_FILENO, "\n", 1);
            } else {
                perror("getcwd");
            }
            continue;
        } 

        else if (strcmp(args[0], "which") == 0) {
            if (num_args != 2) {
                write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
                continue;
            }

            if (strcmp(args[1], "which") == 0 || strcmp(args[1], "pwd") == 0 ||
                strcmp(args[1], "cd") == 0 || strcmp(args[1], "exit") == 0) {
                        continue;
                    }

            // Maximum length of the command path
            char cmd_path[MAX_COMMAND_LENGTH];

            // Loop through each directory and check if the program exists
            int found = 0;
            for (int i = 0; i < num_paths; i++) {
                int dir_length = strlen(paths[i]);
                int cmd_length = strlen(args[1]);
                if (dir_length + cmd_length + 2 > MAX_COMMAND_LENGTH) {
                    // Path length exceeds maximum length
                    write(STDERR_FILENO, "which: Path length exceeds maximum\n", 36);
                    continue;
                }
                // Construct the path
                strcpy(cmd_path, paths[i]);
                //strcat(cmd_path, "/");
                strcat(cmd_path, args[1]);
                if (access(cmd_path, X_OK) == 0) {
                    // Print the path and exit the loop
                    write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
                    write(STDOUT_FILENO, "\n", 1);
                    found = 1;
                    break;
                }
            }

            // If the program is not found in any directory, print an error message
            if (!found) {
                write(STDERR_FILENO, "which: Program not found\n", 26);
            }

            continue;
        }
        else if (strcmp(args[0], "exit") == 0) {
            if (num_args > 1) {
                write(STDOUT_FILENO, "Exiting with status: ", 22);
                write(STDOUT_FILENO, args[1], strlen(args[1]));
                write(STDOUT_FILENO, "\n", 1);
                exit_status = atoi(args[1]);
            }
            break;
        } 
        
        // Handle wildcard expansion
        num_args = handle_wildcard(args);

        // Single command
        exit_status = execute_command(args, in_fd, out_fd);

        // Update previous status if the command was executed
        if (strcmp(args[0], "then") != 0 && strcmp(args[0], "else") != 0) {
            prev_status = exit_status == 0 ? 1 : 0;
        }
    }

    return exit_status;
}





int main(int argc, char *argv[]) {

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

    exit_status = run_shell(input_fd);

    // while (1) {
    //     if (isatty(input_fd)) {
    //         print_prompt();
    //     }

    //     // Read input using read() instead of fgets()
    //     ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
    //     if (bytes_read <= 0) {
    //         // End of input stream
    //         break;
    //     }
    //     // Null terminate the string
    //     command[bytes_read] = '\0';

    //     // Parse command into arguments
    //     num_args = 0;
    //     char *token = strtok(command, " \n");
    //     while (token != NULL) {
    //         args[num_args++] = token;
    //         token = strtok(NULL, " \n");
    //     }
    //     args[num_args] = NULL;

    //     char *args_command [MAX_ARGS];
    //     char *args_redirect[MAX_ARGS];
    //     int args_inc1 = 0;

    //     if(strchr(*args, '>') != NULL || strchr(*args, '<') || strchr(*args, '|')){
    //         //redirection detected
    //         for(int i = 0; i < num_args; i++){
    //             if(strcmp(args[i], ">") == 0 || strcmp(args[i], "<") || strcmp(args[i], "|")){
    //                 args_redirect[args_inc1] = args[i];
    //                 args_command[args_inc1] = " ";
    //                 args_inc1++;
    //             }
    //             else{
    //                 args_command[args_inc1] = args[i];
    //                 args_redirect[args_inc1] = " "; 
    //                 args_inc1++;  
    //             }
    //         }

    //         for(int i = 0; i < args_inc1; i++){
    //             if(strcmp(args_redirect[i], "<") == 0 || strcmp(args_redirect[i], "|") == 0){
    //                 int input = open(args_command[i--], O_RDONLY);
    //                 int output = open(args_command[i++], O_WRONLY, 0640);
    //                 execute_command(args, input, output); 
    //             }
    //             else if(strcmp(args_redirect[i], ">") == 0){
    //                 int input = open(args_command[i++], O_RDONLY);
    //                 int output = open(args_command[i--], O_WRONLY, 0640);
    //                 execute_command(args, input, output); 
    //             }
    //         }

    //     }
    //     if (strcmp(args[0], "cd") == 0) {
    //         if (num_args != 2) {
    //             write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
    //             continue;
    //         }
    //         if (chdir(args[1]) != 0) {
    //             perror("chdir");
    //         }
    //         continue;
    //     } else if (strcmp(args[0], "pwd") == 0) {
    //         char cwd[1024];
    //         if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //             write(STDOUT_FILENO, cwd, strlen(cwd));
    //             write(STDOUT_FILENO, "\n", 1);
    //         } else {
    //             perror("getcwd");
    //         }
    //         continue;
    //     } else if (strcmp(args[0], "which") == 0) {
    //         if (num_args != 2) {
    //             write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
    //             continue;
    //         }
    //         char *path = getenv("PATH");
    //         if (path != NULL) {
    //             char *token = strtok(path, ":");
    //             while (token != NULL) {
    //                 char cmd_path[1024];
    //                 snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, args[1]);
    //                 if (access(cmd_path, X_OK) == 0) {
    //                     write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
    //                     write(STDOUT_FILENO, "\n", 1);
    //                     break;
    //                 }
    //                 token = strtok(NULL, ":");
    //             }
    //         } else {
    //             write(STDERR_FILENO, "which: PATH environment variable not set\n", 42);
    //         }
    //         continue;
    //     } else if (strcmp(args[0], "exit") == 0) {
    //         if (num_args > 1) {
    //             write(STDOUT_FILENO, "Exiting with status: ", 22);
    //             write(STDOUT_FILENO, args[1], strlen(args[1]));
    //             write(STDOUT_FILENO, "\n", 1);
    //             exit_status = atoi(args[1]);
    //         }
    //         break;
    //     }

    //     // Handle wildcard expansion
    //     num_args = handle_wildcard(args);

    //     // Execute command
    //     exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
    // }

    if (isatty(input_fd)) {
        write(STDOUT_FILENO, "Exiting my shell.\n", 18);
    }

    if (argc > 1) {
        close(input_fd);
    }

    return exit_status;
}


