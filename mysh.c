#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <fcntl.h>


#define MAX_COMMAND_LENGTH 2048
#define MAX_ARGS 512

int get_command_path_name(char* command, char path[]) {
    if(strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0) {
        return EXIT_SUCCESS;
    }
    else if(strchr(command, '/') != NULL) {
        strcpy(path, command);
        return EXIT_SUCCESS;
    }
    else {
        char searchpath[3][FILENAME_MAX] = {"/usr/local/bin/", "/usr/bin/", "/bin/"};
        for(int i = 0; i < 3; i++) {
            strcpy(path, searchpath[i]);
            strcat(path, command); // Concatenate command name with directory path
            if(access(path, X_OK) == 0) {
                return EXIT_SUCCESS;
            }
        }
    }
    return EXIT_FAILURE;
}

int redirect_std(char redirectname[2][FILENAME_MAX], int *saved_stdout, int *saved_stdin) {
    if(redirectname[0][0] != '\0') {
        int fd = open(redirectname[0], O_RDONLY);
        if(fd < 0) {
            return EXIT_FAILURE;
        }

        if(saved_stdin != NULL) {
            *saved_stdin = dup(0);
        }

        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if(redirectname[1][0] != '\0') {
        int fd = open(redirectname[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
        if(fd < 0) {
            return EXIT_FAILURE;
        }

        if(saved_stdout != NULL) {
             *saved_stdout = dup(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    return EXIT_SUCCESS;
}

void restore_std(int saved_stdin, int saved_stdout) {
    if(saved_stdout != -1) {
        dup2(saved_stdout, 1);
        close(saved_stdout);
    }
    if(saved_stdin != -1) {
        dup2(saved_stdin, 0);
        close(saved_stdin);
    }
}

int execute_built_in_command(char *argv[] ) {
    if (strcmp(argv[0], "cd") == 0) {
        // cd command
        if (argv[1] == NULL || argv[2] != NULL) {
            printf("cd: Error Number of Parameters\n");
            return EXIT_FAILURE;
        } else if (chdir(argv[1]) != 0) {
            perror("cd");
            return EXIT_FAILURE;
        }
    } 
    else if (strcmp(argv[0], "pwd") == 0) {
        // pwd command
        char cwd[MAX_COMMAND_LENGTH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } 
        else {
            perror("pwd");
            return EXIT_FAILURE;
        }
    } 
    else if(strcmp(argv[0], "which") == 0) {
        if (argv[1] == NULL || argv[2] != NULL) {
            printf("which: must provide one and only one parameter\n");
            return EXIT_FAILURE;
        }
        char pathname[FILENAME_MAX] = "";
        if(get_command_path_name(argv[1], pathname) == EXIT_FAILURE) {
            printf("which: command not found\n");
            return EXIT_FAILURE;
        }
        else if(pathname[0] != '\0'){
            printf("%s\n", pathname);
        }
    }
    return EXIT_SUCCESS;
}

int execute_pipe_command(char *args1[], char *args2[], char redirectname[2][FILENAME_MAX], char redirectname_pipe[2][FILENAME_MAX]) {
    int pipe_fds[2];
    int status[2];
    pid_t pid1, pid2;

    if (pipe(pipe_fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        // first child
        close(pipe_fds[0]); 
        dup2(pipe_fds[1], STDOUT_FILENO);   //Redirects standard input to the read side of the pipe
        close(pipe_fds[1]);
        
        if(redirect_std(redirectname, NULL, NULL) == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }

        char command_pathname[FILENAME_MAX] = "";
        if(get_command_path_name(args1[0], command_pathname) == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }

        if(command_pathname[0] == '\0') {
            exit(execute_built_in_command(args1));
        }
        else {
            execv(command_pathname, args1);
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }

    pid2 = fork();
    if (pid2 == 0) {
        // second child
        close(pipe_fds[1]); 
        dup2(pipe_fds[0], STDIN_FILENO);  //Redirects standard input to the read side of the pipe
        close(pipe_fds[0]);

        if(redirect_std(redirectname_pipe, NULL, NULL) == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }

        char command_pathname[FILENAME_MAX] = "";
        if(get_command_path_name(args2[0], command_pathname) == EXIT_FAILURE) {
            exit(EXIT_FAILURE);
        }
        if(command_pathname[0] == '\0') {
            exit(execute_built_in_command(args2));
        }

        else {
            execv(command_pathname, args2);
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }

    // father
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    waitpid(pid1, &status[0], 0);
    waitpid(pid2, &status[1], 0);

    for(int i = 0; i < 2; i++) {
        if (!(WIFEXITED(status[i]) && WEXITSTATUS(status[i]) == EXIT_SUCCESS)) {
                return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int execute_command(char *argv[], char redirectname[2][FILENAME_MAX]) {
    int saved_stdout = -1;
    int saved_stdin = -1;
    char command_pathname[FILENAME_MAX] = "";

    if(redirect_std(redirectname, &saved_stdout, &saved_stdin) == EXIT_FAILURE) {
        restore_std(saved_stdin, saved_stdout);
        return EXIT_FAILURE;
    }

    // Attempt to find the full path of the command
    if(get_command_path_name(argv[0], command_pathname) == EXIT_SUCCESS) {
        // If the command is found, execute it
        if (execv(command_pathname, argv) == -1) {
            perror("execv");
            restore_std(saved_stdin, saved_stdout);
            return EXIT_FAILURE;
        }
    } else {
        // If the command is not found, try executing it directly
        if (execvp(argv[0], argv) == -1) {
            perror("execvp");
            restore_std(saved_stdin, saved_stdout);
            return EXIT_FAILURE;
        }
    }

    // Execution should never reach here if execv() or execvp() is successful
    return EXIT_FAILURE;
}

void expand_wildcards(char *arg, char **argv, int *argc) {
  glob_t glob_result;
  int i;

    // GLOB_NOCHECK means that `glob` will return to the original pattern if no matching file is found.
    // GLOB_TILDE allows the wave symbol (`~`) to be used in the pattern to expand to the user's home directory.
  if (glob(arg, GLOB_NOCHECK | GLOB_TILDE, NULL, &glob_result) == 0) {
    for (i = 0; i < glob_result.gl_pathc && *argc < MAX_ARGS - 1; i++) {
      argv[(*argc)++] = strdup(glob_result.gl_pathv[i]);
    }
  }
  globfree(&glob_result);
}

int parse_args(char *args[], int argc, char *token, char redirectname[2][FILENAME_MAX]) {
    int pipe_found = 0;
    while (token != NULL && argc < MAX_ARGS - 1) {
        if (strcmp(token, "|") == 0) {
            pipe_found = 1;         // Set the pipe flag if a pipe is found
            break;
        } 
        else if (strchr(token, '*')) {
            // If a wildcard is found, expand it
            expand_wildcards(token, args, &argc);
        } 
        else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            strcpy(redirectname[0], token);
        }
        else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            strcpy(redirectname[1], token);
        }
        else {
            // Store the argument and increment the argument count
            args[argc++] = strdup(token);
        }
        token = strtok(NULL, " ");  // Get the next token
    }
    args[argc] = NULL;
    return pipe_found;
}

void format_command(char *command, char formatcommand[]) {
    int i;
    int j;
    for(i = 0, j = 0; i < strlen(command); i++, j++) {
        if(command[i] == '|' || command[i] == '<' || command[i] == '>') {
            formatcommand[j] = ' ';
            formatcommand[++j] = command[i];
            formatcommand[++j] = ' ';
        }
        else {
            formatcommand[j] = command[i];
        }
    }
}

int parse_and_execute(char *command, int last_status) {
    char *args[MAX_ARGS];           // Array to store command arguments
    char *args_pipe[MAX_ARGS];      // Array to store arguments after a pipe
    int argc = 0, argc_pipe = 0;    // Argument counters for both command parts
    int pipe_found = 0;             // Flag to check if a pipe is found
    int ret = EXIT_FAILURE;
    char redirectname[2][FILENAME_MAX] = {"", ""};
    char redirectname_pipe[2][FILENAME_MAX] = {"", ""};
    char formatcommand[MAX_COMMAND_LENGTH] = "";
    
    format_command(command, formatcommand);
    char *token = strtok(formatcommand, " "); // Tokenize the command string

    //Conditionals
    if(token != NULL) {
        if(strcmp(token, "then") == 0) {
            if(last_status != EXIT_SUCCESS) {
                return EXIT_SUCCESS;
            }
            else {
                token = strtok(NULL, " ");
            }
        }
        else if(strcmp(token, "else") == 0) {
            if(last_status != EXIT_FAILURE) {
                return EXIT_SUCCESS;
            }
            else {
                token = strtok(NULL, " ");
            }
        }
        
    }

    pipe_found = parse_args(args, argc, token, redirectname);
    
    if (pipe_found) {
        // If a pipe is found, process the arguments after the pipe
        token = strtok(NULL, " ");
        parse_args(args_pipe, argc_pipe, token, redirectname_pipe);

        ret = execute_pipe_command(args, args_pipe, redirectname, redirectname_pipe); // Execute the piped command
        
        // Free dynamically allocated memory for pipe arguments
        for (int i = 0; i < argc_pipe; i++) {
            free(args_pipe[i]);
        }
    } else {
        ret = execute_command(args, redirectname); // Execute the command if no pipe is found
    }

    // Free dynamically allocated memory for arguments
    for (int i = 0; i < argc; i++) {
        free(args[i]);
    }
    return ret;
}

void run_shell_loop(int input_fd) {
    char command[MAX_COMMAND_LENGTH];
    int last_status = EXIT_SUCCESS;

    printf("Welcome to the shell!\n");

    // Main loop for reading and executing commands
    while (1) {
        // Check if standard input is a terminal
        if (isatty(STDIN_FILENO)) {
            printf("mysh> ");  // Print the prompt only in interactive mode
        }

        ssize_t bytes_read;

        // Read command from input file descriptor
        if ((bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH)) == -1) {
            perror("Error reading from input");
            break;
        }

        // Check for end of file
        if (bytes_read == 0) {
            break;
        }

        // Null-terminate the command string
        command[bytes_read] = '\0';

        // Remove newline character
        command[strcspn(command, "\n")] = '\0';

        // Check for exit command
        if (strcmp(command, "exit") == 0) {
            printf("mysh: exiting\n");
            break;
        }

        // Execute command
        last_status = parse_and_execute(command, last_status);
    }

    // Close input file descriptor if not stdin
    if (input_fd != STDIN_FILENO) {
        close(input_fd);
    }
}

int main(int argc, char *argv[]) {
  // file descriptor for input
  int input_fd;

  // this program takes at most 1 argument, which is the filename
  if (argc > 2) {
    printf("Usage: %s [filename]\n", argv[0]);
    return EXIT_FAILURE;
  }

  // determine if the program will run in interactive or batch mode
  if (argc == 2) {
    input_fd = open(argv[1], O_RDONLY);
  } else {
    // if no filename provided, use stdin
    input_fd = STDIN_FILENO;
  }

  // check for fd error
  if (input_fd == -1) {
    perror("Error opening input file");
    return EXIT_FAILURE;
  }

  // run the shell loop
  run_shell_loop(input_fd);

  return EXIT_SUCCESS;
}



























// WHAT WORKS: CD, PWD, EXIT, INTERACTIVE, WILDCARDS, BARENAMES
// WHAT NEEDS WORK: BATCH, 
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

// int execute_command(char **args, int input_fd, int output_fd) {
//     pid_t pid = fork();
//     if (pid == 0) {
//         // Child process
//         if (input_fd != STDIN_FILENO) {
//             dup2(input_fd, STDIN_FILENO);
//             close(input_fd);
//         }
//         if (output_fd != STDOUT_FILENO) {
//             dup2(output_fd, STDOUT_FILENO);
//             close(output_fd);
//         }
//         execvp(args[0], args);
//         // If execvp returns, it means an error occurred
//         perror("execvp");
//         exit(EXIT_FAILURE);
//     } else if (pid < 0) {
//         // Fork failed
//         perror("fork");
//         return -1;
//     } else {
//         // Parent process
//         int status;
//         waitpid(pid, &status, 0);
//         return WEXITSTATUS(status);
//     }
// }



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
        if (input_fd == STDIN_FILENO) {
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

        if (strcmp(args[0], "cd") == 0) {
            if (num_args != 2) {
                write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
                continue;
            }
            if (chdir(args[1]) != 0) {
                perror("chdir");
            }
            continue;
        } else if (strcmp(args[0], "pwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                write(STDOUT_FILENO, cwd, strlen(cwd));
                write(STDOUT_FILENO, "\n", 1);
            } else {
                perror("getcwd");
            }
            continue;
        } else if (strcmp(args[0], "which") == 0) {
            if (num_args != 2) {
                write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
                continue;
            }
            char *path = getenv("PATH");
            if (path != NULL) {
                char *token = strtok(path, ":");
                while (token != NULL) {
                    char cmd_path[1024];
                    snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, args[1]);
                    if (access(cmd_path, X_OK) == 0) {
                        write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
                        write(STDOUT_FILENO, "\n", 1);
                        break;
                    }
                    token = strtok(NULL, ":");
                }
            } else {
                write(STDERR_FILENO, "which: PATH environment variable not set\n", 42);
            }
            continue;
        } else if (strcmp(args[0], "exit") == 0) {
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

        // Execute command
        exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
    }

    if (isatty(input_fd)) {
        write(STDOUT_FILENO, "Exiting my shell.\n", 18);
    }

    if (argc > 1) {
        close(input_fd);
    }

    return exit_status;
}
