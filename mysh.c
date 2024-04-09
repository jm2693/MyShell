#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <glob.h>
#include <signal.h>

#define PATHS { "/usr/local/bin/", "/usr/bin/", "/bin/" }
#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 1024
#define NUM_PATHS 3


void print_prompt() {
    write(STDOUT_FILENO, "mysh> ", 6);
}

int execute_command(char **args, int input_fd, int output_fd) {
    pid_t pid;
    int status;
    char *paths[] = PATHS;

    if (strchr(args[0], '/') != NULL) {
        // If the first argument contains a slash character, assume it's a path to an executable file
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

            execv(args[0], args);
            // If execv returns, it means an error occurred
            perror("execv");
            exit(EXIT_FAILURE);
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
    } else {
    
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
    }
    // If none of the paths were successful
    printf("Command not found: %s\n", args[0]);
    return 1;
}

// Function to handle wildcard expansion
int handle_wildcard(char **args) {
    int i;
    int j;
    int num_args = 0;
    glob_t glob_result;
    int flags = 0; // GLOB_NOCHECK to return pattern itself if no matches found

    // Count the number of arguments
    while (args[num_args] != NULL) {
        num_args++;
    }

    // glob took way too long to figure out
    // there was probably a better way to do this
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

void handle_piping(char *args[]) {
    char* paths[] = PATHS;
    int num_paths = NUM_PATHS;
	int fds[2]; 
	int fdt[2];
    int i = 0;
    int j = 0;
    int k = 0;
    int r = 0;
	
	int num_commands = 0;
	
	char *command[256];
	
	pid_t pid;
	
	int err = -1;
	int end = 0;
	
	// variables used for the different loops
	
	
	// first we calculate the number of commands separated by '|'
	while (args[r] != NULL){
		if (strcmp(args[r],"|") == 0){
			num_commands++;
		}
		r++;
	}
	num_commands++;
	
	// main loop of this method. for each command between '|', the
	// pipes will be configured and standard input and/or output will
	// be replaced

	while (args[j] != NULL && end != 1){
		k = 0;

		while (strcmp(args[j],"|") != 0){
			command[k] = args[j];
			j++;	
			if (args[j] == NULL){
				// 'end' variable used to keep the program from entering
				// again in the loop when no more arguments are found
				end = 1;
				k++;
				break;
			}
			k++;
		}

		command[k] = NULL;
		j++;		
		
		if (i % 2 != 0){
			pipe(fds); 
		}else{
			pipe(fdt); 
		}
		
		pid=fork();
		
		if(pid == -1){			
			if (i != num_commands - 1){
				if (i % 2 != 0){
					close(fds[1]); 
				}else{
					close(fdt[1]); 
				} 
			}			
			printf("Child process could not be created\n");
			return;
		}
		if(pid == 0){
			// If we are in the first command
			if (i == 0){
				dup2(fdt[1], STDOUT_FILENO);
			}
			// we will replace the standard input for one pipe or another
            // the standard output will be untouched because we want to see the 
			// output in the terminal
			else if (i == num_commands - 1){
				if (num_commands % 2 != 0){ // for odd number of commands
					dup2(fds[0],STDIN_FILENO);
				}else{ // for even number of commands
					dup2(fdt[0],STDIN_FILENO);
				}
			}
			
            // attempt to execute the command with each path
            for (int p = 0; p < num_paths; p++) {
                // construct the full path to the command
                char *full_path = malloc(strlen(paths[p]) + strlen(command[0]) + 1);
                strcpy(full_path, paths[p]);
                strcat(full_path, command[0]);
                if (execv(full_path, command) != err) {
                    // if execv succeeds, break the loop
                    free(full_path);
                    break;
                }
                free(full_path);
            }
			kill(getpid(),SIGTERM);	
		}
				
		// CLOSING DESCRIPTORS ON PARENT
		if (i == 0){
			close(fdt[1]);
		}
		else if (i == num_commands - 1){
			if (num_commands % 2 != 0){					
				close(fds[0]);
			}else{					
				close(fdt[0]);
			}
		}else{
			if (i % 2 != 0){					
				close(fdt[0]);
				close(fds[1]);
			}else{					
				close(fds[0]);
				close(fdt[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}

int run_shell(int in_fd) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int num_paths = NUM_PATHS;
    char* paths[] = PATHS;
    int exit_status = 0;
    int prev_status = 0; // Previous command's exit status
    int input_fd = in_fd; // Input file descriptor for the current command
    int output_fd = STDOUT_FILENO; // Output file descriptor for the current command

    while (1) {
        if (isatty(input_fd)) {
            print_prompt();
        }

        // reading input 
        ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
        if (bytes_read <= 0) {
            break;
        }

        // null terminate the string
        command[bytes_read] = '\0';

        // parse command into tokens
        int num_args = 0;
        char *token = strtok(command, " \n");
        while (token != NULL) {
            args[num_args++] = token;
            token = strtok(NULL, " \n");
        }
        args[num_args] = NULL;

        // looking for then else conditionals
        if (strcmp(args[0], "then") == 0 || strcmp(args[0], "else") == 0) {

            // essentiall skip if it's "then" when true, omit command when false
            if (prev_status == (strcmp(args[0], "then") == 0 ? 0 : 1)) {
                continue;
            }
            
            // remove the command from the argument list
            num_args--;
            memmove(args, args + 1, num_args * sizeof(char *));
            args[num_args] = NULL;
        }

        // handles redirection, piping
        int input_redirect = 0; 
        int output_redirect = 0;
        for (int i = 0; i < num_args; i++) {
            if (strcmp(args[i], "<") == 0) {
                input_redirect = i;
                input_fd = open(args[i + 1], O_RDONLY);
                if (input_fd < 0) {
                    perror("open");
                    return 1;
                }
                args[i] = NULL;
            } else if (strcmp(args[i], ">") == 0) {
                output_redirect = i;
                output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0640);
                if (output_fd < 0) {
                    perror("open");
                    return 1;
                }
                args[i] = NULL;
            }
            else if (strcmp(args[i],"|") == 0){
				handle_piping(args);
                num_args = 0;
                continue;
				//return 1;
            }
        }

        // built-ins
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

            // maximum length of the command path
            char cmd_path[MAX_COMMAND_LENGTH];

            // loop through each directory and check if the program exists
            int found = 0;
            for (int i = 0; i < num_paths; i++) {
                int dir_length = strlen(paths[i]);
                int cmd_length = strlen(args[1]);
                if (dir_length + cmd_length + 2 > MAX_COMMAND_LENGTH) {
                    // path length exceeds maximum length
                    write(STDERR_FILENO, "which: Path length exceeds maximum\n", 36);
                    continue;
                }
                // Construct the path
                strcpy(cmd_path, paths[i]);
                //strcat(cmd_path, "/");
                strcat(cmd_path, args[1]);
                if (access(cmd_path, X_OK) == 0) {
                    // print the path and exit the loop
                    write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
                    write(STDOUT_FILENO, "\n", 1);
                    found = 1;
                    break;
                }
            }

            // if the program is not found in any directory, print an error message
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
        
        // handle wildcard expansion
        num_args = handle_wildcard(args);

        // execute command with redirection
        if (input_redirect || output_redirect) {
            exit_status = execute_command(args, input_fd, output_fd);
            if (input_fd != STDIN_FILENO) {
                close(input_fd);
            }
            if (output_fd != STDOUT_FILENO) {
                close(output_fd);
            }
        } 
        else {
            exit_status = execute_command(args, input_fd, output_fd);
        }

        // Update previous status if the command was executed
        if (strcmp(args[0], "then") != 0 && strcmp(args[0], "else") != 0) {
            prev_status = exit_status == 0 ? 1 : 0;
        }
    }

    return exit_status;
}

int main(int argc, char *argv[]) {

    int exit_status = 0;

    if (argc > 2) {
        printf("The following program only takes in 1 additional argument: %s\n", argv[0]);
        return 1;
    }

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

    if (isatty(input_fd)) {
        write(STDOUT_FILENO, "Exiting my shell.\n", 18);
    }

    if (argc > 1) {
        close(input_fd);
    }

    return exit_status;
}


