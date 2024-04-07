// #include <stdlib.h>
// #include <stdio.h>
// #include <stddef.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/time.h>

// #define DELIM " \t\r\n\a"
// #define LINE_BUF 512
// #define TOKEN_BUF 64
// #define COMMAND 1024

// char* read_command();
// char** tokenize(char* line);
// int execute_builtin_command(char **args);
// int command_path_name(char* command, char* path);

// int command_path_name(char* command, char* path) {
    
//     if(strcmp(command, "which") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "cd") == 0) {
//         return EXIT_SUCCESS;
//     } 
//     else if(strchr(command, '/') != NULL) {
//         strcpy(path, command);
//         return EXIT_SUCCESS;
//     }
//     else {

//         char searchpath[3][COMMAND] = {"/usr/local/bin/", "/usr/bin/", "/bin/" };
        
//         char* first_search = strcat(searchpath[0], command);
//          if(access(first_search, X_OK) == 0) {
//                 strcpy(path, first_search);
//                 return EXIT_SUCCESS;
//             }
//         char* second_search = strcat(searchpath[1], command);
//         if(access(second_search, X_OK) == 0) {
//                 strcpy(path, second_search);
//                 return EXIT_SUCCESS;
//             }
//         char* third_search = strcat(searchpath[2], command);
//         if(access(third_search, X_OK) == 0) {
//                 strcpy(path, third_search);
//                 return EXIT_SUCCESS;
//             }
//         // for(int i = 0; i < 3; i++) {
//         //     strcat(searchpath[i], command);
//         //     if(access(searchpath[i], X_OK) == 0) {
//         //         strcpy(path, searchpath[i]);
//         //         return EXIT_SUCCESS;

//         //     }
//         // }
//     }
//     return EXIT_FAILURE;

// }

// // built-in cd
// // int mysh_cd(char **args)
// // {
// //   if (args[1] == NULL) {
// //     printf("cd: requires argument for \"cd\"\n");
// //   } else {
// //     if (chdir(args[1]) != 0) {
// //       perror("cd");
// //     }
// //   }
// //   return 1;
// // }

// void print_prompt() {
//     if (isatty(STDIN_FILENO) == 1) {
//         printf("Welcome to my shell!\n");
//     }
// }

// void print_goodbye() {
//     if (isatty(STDIN_FILENO)) {
//         printf("Exiting my shell.\n");
//     }
// }

// int execute_builtin_command(char **args) {
//     if (strcmp(args[0], "cd") == 0) {
//         // cd command
//         if (args[1] == NULL || args[2] != NULL) {
//             printf("cd: Error in parameters\n");
//             return EXIT_FAILURE;
//         } else if (chdir(args[1]) != 0) {
//             perror("cd");
//             return EXIT_FAILURE;
//         }
//     } 
//     else if (strcmp(args[0], "pwd") == 0) {
//         // pwd command
//         char cwd[COMMAND];
//         if (getcwd(cwd, sizeof(cwd)) != NULL) {
//             printf("%s\n", cwd);
//         } 
//         else {
//             perror("pwd");
//             return EXIT_FAILURE;
//         }
//     } 
//     else if(strcmp(args[0], "which") == 0) {
//         if (args[1] == NULL || args[2] != NULL) {
//             printf("which: Only takes one parameter");
//             return EXIT_FAILURE;
//         }
//         char pathname[FILENAME_MAX] = "";
//         if(command_path_name(args[1], pathname) == EXIT_FAILURE) {
//             printf("which: Command not found\n");
//             return EXIT_FAILURE;
//         }
//         else if(pathname[0] != '\0'){
//             printf("%s\n", pathname);
//         }
//     }
//     return EXIT_SUCCESS;
// }

// char **tokenize(char* line) {
//     int bufSize = TOKEN_BUF;
//     int pos = 0;
//     char **tokens = (char**)malloc(bufSize * sizeof(char*));
//     char *token;
//     char *delim = DELIM;

//     if (!tokens) {
//         printf("mysh: allocation error\n");
//         exit(EXIT_FAILURE);
//     }

//     token = strtok(line, delim);
//     while (token != NULL) {
//         tokens[pos] = token;
//         pos++;

//         if (pos >= bufSize) {
//         bufSize += TOKEN_BUF;
//         tokens = realloc(tokens, bufSize * sizeof(char*));
//         if (!tokens) {
//             printf("mysh: allocation error\n");
//             exit(EXIT_FAILURE);
//         }
//         }

//         token = strtok(NULL, delim);
//     }

//     // add NULL as the last element to indicate the end of tokens array
//     tokens[pos] = NULL;
//     return tokens;
// }

// // function used to parse through input lines (either file or standard input)
// char *read_command() {

//     int bufsize = LINE_BUF;
//     int position = 0;
//     char *buffer = (char *)malloc(sizeof(char) * bufsize);
//     int chars_read;

//     if (!buffer) {
//         printf("mysh: allocation error\n");
//         exit(EXIT_FAILURE);
//     }

//     while (1) {
//         // Read characters into buffer
//         chars_read = read(STDIN_FILENO, &buffer[position], 1);

//         if (chars_read == -1) {
//             perror("read");
//             exit(EXIT_FAILURE);
//         } else if (chars_read == 0 || buffer[position] == '\n') {
//             buffer[position] = '\0';
//             return buffer;
//         } else {
//             position++;

//             // If we have exceeded the buffer, reallocate.
//             if (position >= bufsize) {
//                 bufsize += LINE_BUF;
//                 buffer = realloc(buffer, bufsize);
//                 if (!buffer) {
//                     printf("mysh: allocation error\n");
//                     exit(EXIT_FAILURE);
//                 }
//             }
//         }
//     }



//     // char *line = NULL;
//     // size_t bufsize = 1024; // Initial buffer size
//     // ssize_t bytes_read;

//     // line = (char *)malloc(bufsize * sizeof(char));
//     // if (line == NULL) {
//     //     perror("malloc failed");
//     //     exit(EXIT_FAILURE);
//     // }

//     // size_t pos = 0; // Position tracker for appending characters

//     // while ((bytes_read = read(STDIN_FILENO, &line[pos], 1)) != -1) {
//     //     if (bytes_read == 0 || line[pos] == '\n') {
//     //         line[pos] = '\0'; // Replace newline with null terminator
//     //         break;
//     //     }
//     //     pos++;

//     //     // Resize buffer if necessary
//     //     if (pos >= bufsize) {
//     //         bufsize *= 2; // Double the buffer size
//     //         line = (char *)realloc(line, bufsize * sizeof(char));
//     //         if (line == NULL) {
//     //             perror("realloc failed");
//     //             exit(EXIT_FAILURE);
//     //         }
//     //     }
//     // }

//     // if (bytes_read == -1) {
//     //     free(line); /* avoid memory leaks */
//     //     perror("error while reading the line from stdin");
//     //     exit(EXIT_FAILURE);
//     // }

//     // return line;







//     // char *line = NULL;
//     // size_t bufsize = 0;
//     // ssize_t chars_read;

//     // while ((chars_read = getline(&line, &bufsize, stdin)) != -1) {
//     //     if (chars_read > 0 && line[chars_read - 1] == '\n') {
//     //         line[chars_read - 1] = '\0'; // Remove trailing newline
//     //         break;
//     //     }
//     // }

//     // if (chars_read == -1) {
//     //     free(line); /* avoid memory leaks */
//     //     perror("error while reading the line from stdin");
//     //     exit(EXIT_FAILURE);
//     // }

//     // return line;







//     // int bufSize = LINE_BUF;      // buffer size for command length
//     // int pos = 0;                 // position of bufSize
//     // char *buffer = (char*)malloc(sizeof(char) * bufSize);   // malloc buffer for command

//     // if(!buffer){
//     //     printf("mysh: allocation error \n"); 
//     //     exit(EXIT_FAILURE);
//     // }

//     // // parses through input lines (standard input or file)
//     // while(1) {
//     //     ssize_t bytes_read = read(STDIN_FILENO, &buffer[pos], 1);  
//     //     if (bytes_read < 0) {
//     //         printf("Error reading command\n");
//     //         exit(EXIT_FAILURE);
//     //     } 
//     //     else if (bytes_read == 0 || buffer[pos] == '\n') {
//     //         buffer[pos] = '\0';
//     //         return buffer;
//     //     }
//     //     else buffer[pos] = bytes_read;

//     //     // else if ((isatty(STDIN_FILENO)) == 1) {      // if in interactive mode
//     //     //     if (bytes_read == 0 || buffer[pos] == '\n') {
//     //     //         buffer[pos] = '\0';
//     //     //         return buffer;
//     //     //     } else buffer[pos] = bytes_read;
//     //     // } 
//     //     // else if (isatty(STDIN_FILENO) != 1) {        // if in bash mode
//     //     //     if (bytes_read == 0 || buffer[pos] == '\n') {
//     //     //         buffer[pos] = '\0';
//     //     //         return buffer;
//     //     //     } else buffer[pos] = bytes_read;
//     //     // }
//     //     pos++;

//     //     // add size of buffer if runs out of memory
//     //     if (pos >= bufSize){
//     //         bufSize += LINE_BUF;
//     //         buffer = realloc(buffer, bufSize);
//     //         if(!buffer){
//     //             printf("mysh: allocation error \n"); 
//     //             exit(EXIT_FAILURE);
//     //         }
//     //     }
//     // }
// }

// int process(char** tokens) {
//     pid_t pid;
//     pid_t wpid;
//     int status;

//     pid = fork();
//     if (pid == 0) {
//         // Child process
//         if (execv(tokens[0], tokens) == -1) {
//             perror("mysh");
//         }
//         exit(EXIT_FAILURE);
//     } else if (pid < 0) {
//         // Error forking
//         perror("mysh");
//     } else {
//         // Parent process
//         do {
//             wpid = waitpid(pid, &status, WUNTRACED);
//         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
//     }

//     return 1;
// }

// int execute(char **args) {
//     if (args[0] == NULL) {
//         return 1;
//     }

//     if (execute_builtin_command(args)) return 0;
//     else process(args);

//     return 0;
// }

// // shell loop that will constantly check for input and parse arguments
// void run_shell_loop () { 
//     int is_interactive = isatty(STDIN_FILENO);
//     char *command;
//     char **args;
//     if(is_interactive == 1) printf("Welcome to my shell!\n");
//     //print_prompt();

//     while (1) {
        
//         if (is_interactive == 1)
//             printf("mysh> ");

//         //reads line of input
//         command = read_command();

//         // tokenizes line of input
//         args = tokenize(command);

//         // missing execution
//         execute(args);

//         free(command);
//         free(args);
//     } 

// }

// int main(int argc, char **argv)
// {
//     // this program takes in at most 2 arguments including the program name. any more results in error
//     if (argc > 2) {
//         printf("The program, %s, takes in either 0 or 1 argument(s)\n", argv[0]);
//         return EXIT_FAILURE;
//     } 

//     // check for fd error
//     if (STDIN_FILENO < 0) {
//         perror("Error reading from input");
//         return EXIT_FAILURE;
//     }

//     // one major loop that runs the shell in both modes
//     run_shell_loop();

//     return EXIT_SUCCESS;
// }

































// THIS ONE WORKS kinda




// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <glob.h>

// #define MAX_TOKENS 64
// #define MAX_TOKEN_SIZE 64
// #define MAX_COMMAND_SIZE 256

// int isatty(int fd);

// void print_prompt() {
//     if (isatty(STDIN_FILENO)) {
//         write(STDOUT_FILENO, "Welcome to my shell!\n", strlen("Welcome to my shell!\n"));
//     }
//     write(STDOUT_FILENO, "mysh> ", strlen("mysh> "));
// }

// void print_goodbye() {
//     if (isatty(STDIN_FILENO)) {
//         write(STDOUT_FILENO, "Exiting my shell.\n", strlen("Exiting my shell.\n"));
//     }
// }

// void execute_command(char *tokens[], int input_fd, int output_fd) {
//     if (strcmp(tokens[0], "cd") == 0) {
//         if (tokens[1] == NULL) {
//             write(STDERR_FILENO, "cd: missing argument\n", strlen("cd: missing argument\n"));
//             return;
//         }
//         if (chdir(tokens[1]) != 0) {
//             char msg[] = "cd: failed to change directory\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//         }
//     } else if (strcmp(tokens[0], "pwd") == 0) {
//         char cwd[1024];
//         if (getcwd(cwd, sizeof(cwd)) != NULL) {
//             int len = 0;
//             while (cwd[len] != '\0') {
//                 len++;
//             }
//             write(STDOUT_FILENO, cwd, len);
//             write(STDOUT_FILENO, "\n", 1);
//         } else {
//             char msg[] = "pwd: failed to get current directory\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//         }
//     } else if (strcmp(tokens[0], "which") == 0) {
//         if (tokens[1] == NULL) {
//             write(STDERR_FILENO, "which: missing argument\n", strlen("which: missing argument\n"));
//             return;
//         }
//         char *cmd_path = getenv("PATH");
//         char *cmd = strtok(cmd_path, ":");
//         int found = 0;
//         while (cmd != NULL) {
//             char file_path[256];
//             strcpy(file_path, cmd);
//             strcat(file_path, "/");
//             strcat(file_path, tokens[1]);
//             if (access(file_path, X_OK) == 0) {
//                 int len = 0;
//                 while (file_path[len] != '\0') {
//                     len++;
//                 }
//                 write(STDOUT_FILENO, file_path, len);
//                 write(STDOUT_FILENO, "\n", 1);
//                 found = 1;
//                 break;
//             }
//             cmd = strtok(NULL, ":");
//         }
//         if (!found) {
//             char msg[] = "which: command not found\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//         }
//     } else if (strcmp(tokens[0], "exit") == 0) {
//         if (tokens[1] != NULL) {
//             for (int i = 1; tokens[i] != NULL; ++i) {
//                 write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
//                 write(STDOUT_FILENO, " ", strlen(" "));
//             }
//             write(STDOUT_FILENO, "\n", strlen("\n"));
//         }
//         exit(0);
//     } else {
//         pid_t pid = fork();
//         if (pid < 0) {
//             char msg[] = "fork: failed to create child process\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//             return;
//         } else if (pid == 0) { // Child process
//             if (input_fd != STDIN_FILENO) {
//                 if (dup2(input_fd, STDIN_FILENO) == -1) {
//                     char msg[] = "dup2: failed to duplicate file descriptor\n";
//                     write(STDERR_FILENO, msg, strlen(msg));
//                     exit(1);
//                 }
//                 close(input_fd);
//             }
//             if (output_fd != STDOUT_FILENO) {
//                 if (dup2(output_fd, STDOUT_FILENO) == -1) {
//                     char msg[] = "dup2: failed to duplicate file descriptor\n";
//                     write(STDERR_FILENO, msg, strlen(msg));
//                     exit(1);
//                 }
//                 close(output_fd);
//             }
//             execvp(tokens[0], tokens);
//             char msg[] = "execvp: failed to execute command\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//             exit(127);
//         } else { // Parent process
//             int status;
//             waitpid(pid, &status, 0);
//         }
//     }
// }

// void tokenize(char *command, char *tokens[]) {
//     char *token = strtok(command, " \n");
//     int i = 0;
//     while (token != NULL && i < MAX_TOKENS) {
//         tokens[i++] = token;
//         token = strtok(NULL, " \n");
//     }
//     tokens[i] = NULL;
// }

// void process_command(char *command) {
//     char *tokens[MAX_TOKENS];
//     tokenize(command, tokens);

//     int input_fd = STDIN_FILENO;
//     int output_fd = STDOUT_FILENO;
//     int i = 0;

//     while (tokens[i] != NULL) {
//         if (strcmp(tokens[i], "<") == 0) {
//             input_fd = open(tokens[i + 1], O_RDONLY);
//             if (input_fd == -1) {
//                 char msg[] = "open: failed to open file\n";
//                 write(STDERR_FILENO, msg, strlen(msg));
//                 return;
//             }
//             tokens[i] = NULL;
//             tokens[i + 1] = NULL;
//             i += 2;
//         } else if (strcmp(tokens[i], ">") == 0) {
//             output_fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
//             if (output_fd == -1) {
//                 char msg[] = "open: failed to open file\n";
//                 write(STDERR_FILENO, msg, strlen(msg));
//                 return;
//             }
//             tokens[i] = NULL;
//             tokens[i + 1] = NULL;
//             i += 2;
//         } else {
//             i++;
//         }
//     }

//     execute_command(tokens, input_fd, output_fd);

//     if (input_fd != STDIN_FILENO) {
//         close(input_fd);
//     }
//     if (output_fd != STDOUT_FILENO) {
//         close(output_fd);
//     }
// }

// int main(int argc, char *argv[]) {
//     if (argc > 1) { // Batch mode
//         int fd = open(argv[1], O_RDONLY);
//         if (fd == -1) {
//             char msg[] = "open: failed to open file\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//             return 1;
//         }
        
//         char buffer[MAX_COMMAND_SIZE];
//         int bytesRead;
//         int pos = 0;

//         while ((bytesRead = read(fd, buffer + pos, sizeof(buffer) - pos)) > 0) {
//             pos += bytesRead;
//             char *newline;
//             while ((newline = memchr(buffer, '\n', pos)) != NULL) {
//                 *newline = '\0'; // Replace newline with null terminator
//                 process_command(buffer);
//                 pos -= (newline - buffer + 1);
//                 memmove(buffer, newline + 1, pos); // Shift remaining data to the beginning of the buffer
//             }
//         }

//         if (bytesRead == -1) {
//             char msg[] = "read: failed to read from file\n";
//             write(STDERR_FILENO, msg, strlen(msg));
//             return 1;
//         }

//         close(fd);
//     } else { // Interactive mode
//         // Your interactive mode code remains unchanged
//     }
//     return 0;
// }


































// this works the most closely

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <fcntl.h>
// #include <dirent.h>
// #include <errno.h>

// #define MAX_TOKENS 100
// #define MAX_TOKEN_LEN 100
// #define MAX_PATH_LEN 1000

// int last_exit_status = 0;

// // Function to execute a command
// void execute_command(char *tokens[], int num_tokens) {
//     if (num_tokens <= 0) return;

//     // Check for built-in commands

//     if (strcmp(tokens[0], "cd") == 0) {
//         // cd command
//         if (tokens[1] == NULL || tokens[2] != NULL) {
//             printf("cd: Error Number of Parameters\n");
//             last_exit_status = 1;
//             return;
//         } else if (chdir(tokens[1]) != 0) {
//             perror("cd");
//             last_exit_status = 1;
//             return;
//         }
//     } 


//     // if (strcmp(tokens[0], "cd") == 0) {
//     //     if (num_tokens != 2) {
//     //         fprintf(stderr, "cd: Invalid number of arguments\n");
//     //         last_exit_status = 1;
//     //         return;
//     //     }
//     //     if (chdir(tokens[1]) == -1) {
//     //         perror("cd");
//     //         last_exit_status = 1;
//     //     }
//     //     return;
//     // }
//     else if (strcmp(tokens[0], "pwd") == 0) {
//         char cwd[MAX_PATH_LEN];
//         if (getcwd(cwd, sizeof(cwd)) != NULL) {
//             printf("%s\n", cwd);
//         } else {
//             perror("pwd");
//             last_exit_status = 1;
//         }
//         return;
//     }
//     else if (strcmp(tokens[0], "which") == 0) {
//         if (num_tokens != 2) {
//             fprintf(stderr, "which: Invalid number of arguments\n");
//             last_exit_status = 1;
//             return;
//         }
//         char *path = getenv("PATH");
//         char *token = strtok(path, ":");
//         while (token != NULL) {
//             char command_path[MAX_PATH_LEN];
//             snprintf(command_path, sizeof(command_path), "%s/%s", token, tokens[1]);
//             if (access(command_path, F_OK) == 0) {
//                 printf("%s\n", command_path);
//                 return;
//             }
//             token = strtok(NULL, ":");
//         }
//         fprintf(stderr, "which: %s: command not found\n", tokens[1]);
//         last_exit_status = 1;
//         return;
//     }
//     else if (strcmp(tokens[0], "exit") == 0) {
//         // for (int i = 1; i < num_tokens; i++) {
//         //     printf("%s ", tokens[i]);
//         // }
//         // printf("\n");
//         exit(0);
//     }

//     // Execution of non-built-in commands
//     pid_t pid = fork();
//     if (pid < 0) {
//         perror("fork");
//         exit(1);
//     }
//     else if (pid == 0) { // Child process
//         // Execute command
//         execv(tokens[0], tokens);
//         // If execv returns, it must have failed
//         perror(tokens[0]);
//         exit(1);
//     }
//     else { // Parent process
//         int status;
//         waitpid(pid, &status, 0);
//         if (WIFEXITED(status)) {
//             last_exit_status = WEXITSTATUS(status);
//         }
//     }
// }

// // Function to tokenize a command
// void tokenize_command(char *command, char *tokens[], int *num_tokens) {
//     *num_tokens = 0;
//     char *token = strtok(command, " \t\n");
//     while (token != NULL) {
//         tokens[(*num_tokens)++] = token;
//         token = strtok(NULL, " \t\n");
//     }
//     tokens[*num_tokens] = NULL;
// }

// // Function to handle redirection
// void handle_redirection(char *tokens[], int *num_tokens, char *input_file, char *output_file) {
//     int i = 0;
//     while (tokens[i] != NULL) {
//         if (strcmp(tokens[i], "<") == 0) {
//             tokens[i] = NULL;
//             strcpy(input_file, tokens[i + 1]);
//             memmove(&tokens[i], &tokens[i + 2], (*num_tokens - i - 1) * sizeof(char *));
//             (*num_tokens) -= 2;
//         }
//         else if (strcmp(tokens[i], ">") == 0) {
//             tokens[i] = NULL;
//             strcpy(output_file, tokens[i + 1]);
//             memmove(&tokens[i], &tokens[i + 2], (*num_tokens - i - 1) * sizeof(char *));
//             (*num_tokens) -= 2;
//         }
//         else {
//             i++;
//         }
//     }
// }

// // Function to execute commands in a pipeline
// void execute_pipeline(char *pipeline[], int num_commands) {
//     int pipefd[2];
//     pid_t pid;
//     int in = 0; // File descriptor for the input of previous command

//     for (int i = 0; i < num_commands; i++) {
//         if (i < num_commands - 1) {
//             pipe(pipefd);
//         }

//         char *tokens[MAX_TOKENS];
//         int num_tokens;
//         tokenize_command(pipeline[i], tokens, &num_tokens);

//         char input_file[MAX_TOKEN_LEN] = "";
//         char output_file[MAX_TOKEN_LEN] = "";
//         handle_redirection(tokens, &num_tokens, input_file, output_file);

//         pid = fork();
//         if (pid < 0) {
//             perror("fork");
//             exit(1);
//         } else if (pid == 0) { // Child process
//             if (i > 0) {
//                 dup2(in, STDIN_FILENO); // Set input to read from the previous command
//                 close(in); // Close unused file descriptor
//             }
//             if (i < num_commands - 1) {
//                 dup2(pipefd[1], STDOUT_FILENO); // Set output to write to the next command
//                 close(pipefd[1]); // Close unused file descriptor
//             }

//             // Handle input redirection
//             if (strlen(input_file) > 0) {
//                 int fd = open(input_file, O_RDONLY);
//                 if (fd == -1) {
//                     perror("open");
//                     exit(1);
//                 }
//                 dup2(fd, STDIN_FILENO);
//                 close(fd);
//             }

//             // Handle output redirection
//             if (strlen(output_file) > 0) {
//                 int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
//                 if (fd == -1) {
//                     perror("open");
//                     exit(1);
//                 }
//                 dup2(fd, STDOUT_FILENO);
//                 close(fd);
//             }

//             execvp(tokens[0], tokens);
//             // If execvp returns, it must have failed
//             perror(tokens[0]);
//             exit(1);
//         } else { // Parent process
//             wait(NULL); // Wait for child to finish
//             if (i > 0) {
//                 close(in); // Close input file descriptor of previous command
//             }
//             if (i < num_commands - 1) {
//                 close(pipefd[1]); // Close write end of pipe
//                 in = pipefd[0]; // Save read end of pipe for the next command
//             }
//         }
//     }
// }

// int main(int argc, char *argv[]) {
//     int interactive_mode = isatty(STDIN_FILENO);

//     if (argc == 2) { // Batch mode
//         interactive_mode = 0;
//         freopen(argv[1], "r", stdin);
//     }

//     if (interactive_mode) {
//         printf("Welcome to my shell!\n");
//     }

//     while (1) {
//         if (interactive_mode) {
//             printf("mysh> ");
//         }

//         char command[MAX_TOKEN_LEN];
//         if (fgets(command, sizeof(command), stdin) == NULL) {
//             break; // End of input stream
//         }

//         // Remove trailing newline character
//         command[strcspn(command, "\n")] = '\0';

//         // Split command into individual commands for pipeline
//         char *pipeline[MAX_TOKENS];
//         int num_commands = 0;
//         char *token = strtok(command, "|");
//         while (token != NULL) {
//             pipeline[num_commands++] = token;
//             token = strtok(NULL, "|");
//         }

//         // Execute commands in pipeline
//         execute_pipeline(pipeline, num_commands);
        
//     }

//     if (interactive_mode) {
//         printf("Exiting my shell.\n");
//     }

//     return 0;
// }




























// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <fcntl.h>

// #define MAX_COMMAND_LENGTH 1024
// #define MAX_ARGS 64

// // Function declarations
// void execute_command(char *cmd);
// void split_command(char *cmd, char **argv);
// void run_interactive_mode();
// void run_batch_mode(char *filename);
// int is_interactive_mode();

// int main(int argc, char *argv[]) {
//     if (argc > 2) {
//         printf("Usage: %s [scriptfile]\n", argv[0]);
//         exit(EXIT_FAILURE);
//     }

//     if (argc == 2) {
//         run_batch_mode(argv[1]);
//     } else {
//         if (is_interactive_mode()) {
//             run_interactive_mode();
//         } else {
//             run_batch_mode(NULL); // Batch mode with input from stdin
//         }
//     }

//     return 0;
// }

// void run_interactive_mode() {
//     char command[MAX_COMMAND_LENGTH];

//     printf("Welcome to my shell!\n");
//     while (1) {
//         printf("mysh> ");

//         if (read(STDIN_FILENO, command, MAX_COMMAND_LENGTH) <= 0) {
//             break;
//         }

//         command[strcspn(command, "\n")] = 0; // Remove newline
//         if (strcmp(command, "exit") == 0) {
//             printf("Exiting my shell.\n");
//             break;
//         }

//         execute_command(command);
//     }
// }

// void run_batch_mode(char *filename) {
//     char command[MAX_COMMAND_LENGTH];
//     int fd = STDIN_FILENO;

//     if (filename != NULL) {
//         fd = open(filename, O_RDONLY);
//         if (fd == -1) {
//             perror("Error opening file");
//             exit(EXIT_FAILURE);
//         }
//     }

//     while (read(fd, command, MAX_COMMAND_LENGTH) > 0) {
//         command[strcspn(command, "\n")] = 0; // Remove newline
//         execute_command(command);
//     }

//     if (filename != NULL) {
//         close(fd);
//     }
// }

// void execute_command(char *cmd) {
//     char *argv[MAX_ARGS];
//     pid_t pid;
//     int status;

//     split_command(cmd, argv);

//     if ((pid = fork()) == 0) {
//         if (execvp(argv[0], argv) == -1) {
//             perror("execvp");
//             exit(EXIT_FAILURE);
//         }
//     } else if (pid > 0) {
//         waitpid(pid, &status, 0);
//     } else {
//         perror("fork");
//         exit(EXIT_FAILURE);
//     }
// }

// void split_command(char *cmd, char **argv) {
//     int argc = 0;
//     char *token = strtok(cmd, " ");
//     while (token != NULL && argc < MAX_ARGS - 1) {
//         argv[argc++] = token;
//         token = strtok(NULL, " ");
//     }
//     argv[argc] = NULL; // Null-terminate the argument list
// }

// int is_interactive_mode() {
//     return isatty(STDIN_FILENO);
// }


























// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <glob.h>
// #include <fcntl.h>


// #define MAX_COMMAND_LENGTH 2048
// #define MAX_ARGS 512

// int get_command_path_name(char* command, char path[]) {
    
//     if(strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "which") == 0) {
//         return EXIT_SUCCESS;
//     }
//     else if(strchr(command, '/') != NULL) {
//         strcpy(path, command);
//         return EXIT_SUCCESS;
//     }
//     else {
//         char searchpath[3][FILENAME_MAX] = {"/usr/local/bin/", "/usr/bin/", "/bin/" };
//         for(int i = 0; i < 3; i++) {
//             strcat(searchpath[i], command);
//             if(access(searchpath[i], X_OK) == 0) {
//                 strcpy(path, searchpath[i]);
//                 return EXIT_SUCCESS;

//             }
//         }
//     }
//     return EXIT_FAILURE;

// }

// int redirect_std(char redirectname[2][FILENAME_MAX], int *saved_stdout, int *saved_stdin) {
//     if(redirectname[0][0] != '\0') {
//         int fd = open(redirectname[0], O_RDONLY);
//         if(fd < 0) {
//             return EXIT_FAILURE;
//         }

//         if(saved_stdin != NULL) {
//             *saved_stdin = dup(0);
//         }

//         dup2(fd, STDIN_FILENO);
//         close(fd);
//     }
//     if(redirectname[1][0] != '\0') {
//         int fd = open(redirectname[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
//         if(fd < 0) {
//             return EXIT_FAILURE;
//         }

//         if(saved_stdout != NULL) {
//              *saved_stdout = dup(1);
//         }

//         dup2(fd, STDOUT_FILENO);
//         close(fd);
//     }
//     return EXIT_SUCCESS;
// }

// void restore_std(int saved_stdin, int saved_stdout) {
//     if(saved_stdout != -1) {
//         dup2(saved_stdout, 1);
//         close(saved_stdout);
//     }
//     if(saved_stdin != -1) {
//         dup2(saved_stdin, 0);
//         close(saved_stdin);
//     }
// }

// int execute_built_in_command(char *argv[] ) {
//     if (strcmp(argv[0], "cd") == 0) {
//         // cd command
//         if (argv[1] == NULL || argv[2] != NULL) {
//             fprintf(stderr, "cd: Error Number of Parameters\n");
//             return EXIT_FAILURE;
//         } else if (chdir(argv[1]) != 0) {
//             perror("cd");
//             return EXIT_FAILURE;
//         }
//     } 
//     else if (strcmp(argv[0], "pwd") == 0) {
//         // pwd command
//         char cwd[MAX_COMMAND_LENGTH];
//         if (getcwd(cwd, sizeof(cwd)) != NULL) {
//             printf("%s\n", cwd);
//         } 
//         else {
//             perror("pwd");
//             return EXIT_FAILURE;
//         }
//     } 
//     else if(strcmp(argv[0], "which") == 0) {
//         if (argv[1] == NULL || argv[2] != NULL) {
//             fprintf(stderr, "which: must provide one and only one parameter");
//             return EXIT_FAILURE;
//         }
//         char pathname[FILENAME_MAX] = "";
//         if(get_command_path_name(argv[1], pathname) == EXIT_FAILURE) {
//             fprintf(stderr, "which: command not found\n");
//             return EXIT_FAILURE;
//         }
//         else if(pathname[0] != '\0'){
//             printf("%s\n", pathname);
//         }
//     }
//     return EXIT_SUCCESS;
// }

// int execute_pipe_command(char *args1[], char *args2[], char redirectname[2][FILENAME_MAX], char redirectname_pipe[2][FILENAME_MAX]) {
//     int pipe_fds[2];
//     int status[2];
//     pid_t pid1, pid2;

//     if (pipe(pipe_fds) == -1) {
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     }

//     pid1 = fork();
//     if (pid1 == 0) {
//         // first child
//         close(pipe_fds[0]); 
//         dup2(pipe_fds[1], STDOUT_FILENO);   //Redirects standard input to the read side of the pipe
//         close(pipe_fds[1]);
        
//         if(redirect_std(redirectname, NULL, NULL) == EXIT_FAILURE) {
//             exit(EXIT_FAILURE);
//         }

//         char command_pathname[FILENAME_MAX] = "";
//         if(get_command_path_name(args1[0], command_pathname) == EXIT_FAILURE) {
//             exit(EXIT_FAILURE);
//         }

//         if(command_pathname[0] == '\0') {
//             exit(execute_built_in_command(args1));
//         }
//         else {
//             execv(command_pathname, args1);
//             perror("execv");
//             exit(EXIT_FAILURE);
//         }
//     }

//     pid2 = fork();
//     if (pid2 == 0) {
//         // second child
//         close(pipe_fds[1]); 
//         dup2(pipe_fds[0], STDIN_FILENO);  //Redirects standard input to the read side of the pipe
//         close(pipe_fds[0]);

//         if(redirect_std(redirectname_pipe, NULL, NULL) == EXIT_FAILURE) {
//             exit(EXIT_FAILURE);
//         }

//         char command_pathname[FILENAME_MAX] = "";
//         if(get_command_path_name(args2[0], command_pathname) == EXIT_FAILURE) {
//             exit(EXIT_FAILURE);
//         }
//         if(command_pathname[0] == '\0') {
//             exit(execute_built_in_command(args2));
//         }

//         else {
//             execv(command_pathname, args2);
//             perror("execv");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // father
//     close(pipe_fds[0]);
//     close(pipe_fds[1]);
//     waitpid(pid1, &status[0], 0);
//     waitpid(pid2, &status[1], 0);

//     for(int i = 0; i < 2; i++) {
//         if (!(WIFEXITED(status[i]) && WEXITSTATUS(status[i]) == EXIT_SUCCESS)) {
//                 return EXIT_FAILURE;
//         }
//     }
//     return EXIT_SUCCESS;
// }

// int execute_command(char *argv[], char redirectname[2][FILENAME_MAX]) {
//     int saved_stdout = -1;
//     int saved_stdin = -1;
//     char command_pathname[FILENAME_MAX] = "";

//     if(redirect_std(redirectname, &saved_stdout, &saved_stdin) == EXIT_FAILURE) {
//         restore_std(saved_stdin, saved_stdout);
//         return EXIT_FAILURE;
//     }
    

//     if(get_command_path_name(argv[0], command_pathname) == EXIT_FAILURE) {
//         restore_std(saved_stdin, saved_stdout);
//         return EXIT_FAILURE;
//     }

//     if(command_pathname[0] == '\0') {
//         int ret = execute_built_in_command(argv);
//         restore_std(saved_stdin, saved_stdout);
//         return ret;
//     }
//     else {
//         // external command
//         pid_t pid = fork();
//         if (pid == -1) {
//             perror("fork");
//         }
//         else if (pid == 0) {
//             // child progress
//         if (execv(argv[0], argv) == -1) {
//                 perror("execv");
//                 exit(EXIT_FAILURE);
//             }
//         }
//         else {
//             // father progress
//             int status;
//             waitpid(pid, &status, 0);
//             if (!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)) {
//                 restore_std(saved_stdin, saved_stdout);
//                 return EXIT_FAILURE;
//             }
//         }
//     }
//     restore_std(saved_stdin, saved_stdout);
//     return EXIT_SUCCESS;
// }

// void expand_wildcards(char *arg, char **argv, int *argc) {
//   glob_t glob_result;
//   int i;

//     // GLOB_NOCHECK means that `glob` will return to the original pattern if no matching file is found.
//     // GLOB_TILDE allows the wave symbol (`~`) to be used in the pattern to expand to the user's home directory.
//   if (glob(arg, GLOB_NOCHECK | GLOB_TILDE, NULL, &glob_result) == 0) {
//     for (i = 0; i < glob_result.gl_pathc && *argc < MAX_ARGS - 1; i++) {
//       argv[(*argc)++] = strdup(glob_result.gl_pathv[i]);
//     }
//   }
//   globfree(&glob_result);
// }

// int parse_args(char **args, int argc, char *token, char redirectname[2][FILENAME_MAX]) {
//     int pipe_found = 0;
//     while (token != NULL && argc < MAX_ARGS - 1) {
//         if (strcmp(token, "|") == 0) {
//             pipe_found = 1;         // Set the pipe flag if a pipe is found
//             break;
//         } 
//         else if (strchr(token, '*')) {
//             // If a wildcard is found, expand it
//             expand_wildcards(token, args, &argc);
//         } 
//         else if (strcmp(token, "<") == 0) {
//             token = strtok(NULL, " ");
//             strcpy(redirectname[0], token);
//         }
//         else if (strcmp(token, ">") == 0) {
//             token = strtok(NULL, " ");
//             strcpy(redirectname[1], token);
//         }
//         else {
//             // Store the argument and increment the argument count
//             args[argc++] = strdup(token);
//         }
//         token = strtok(NULL, " ");  // Get the next token
//     }
//     args[argc] = NULL;
//     return pipe_found;
// }

// void format_command(char *command, char formatcommand[]) {
//     int i;
//     int j;
//     for(i = 0, j = 0; i < strlen(command); i++, j++) {
//         if(command[i] == '|' || command[i] == '<' || command[i] == '>') {
//             formatcommand[j] = ' ';
//             formatcommand[++j] = command[i];
//             formatcommand[++j] = ' ';
//         }
//         else {
//             formatcommand[j] = command[i];
//         }
//     }
// }

// int parse_and_execute(char *command, int last_status) {
//     char *args[MAX_ARGS];           // Array to store command arguments
//     char *args_pipe[MAX_ARGS];      // Array to store arguments after a pipe
//     int argc = 0, argc_pipe = 0;    // Argument counters for both command parts
//     int pipe_found = 0;             // Flag to check if a pipe is found
//     int ret = EXIT_FAILURE;
//     char redirectname[2][FILENAME_MAX] = {"", ""};
//     char redirectname_pipe[2][FILENAME_MAX] = {"", ""};
//     char formatcommand[MAX_COMMAND_LENGTH] = "";
    
//     format_command(command, formatcommand);
//     char *token = strtok(formatcommand, " "); // Tokenize the command string

//     //Conditionals
//     if(token != NULL) {
//         if(strcmp(token, "then") == 0) {
//             if(last_status != EXIT_SUCCESS) {
//                 return EXIT_SUCCESS;
//             }
//             else {
//                 token = strtok(NULL, " ");
//             }
//         }
//         else if(strcmp(token, "else") == 0) {
//             if(last_status != EXIT_FAILURE) {
//                 return EXIT_SUCCESS;
//             }
//             else {
//                 token = strtok(NULL, " ");
//             }
//         }
        
//     }

//     pipe_found = parse_args(args, argc, token, redirectname);
    
//     if (pipe_found) {
//         // If a pipe is found, process the arguments after the pipe
//         token = strtok(NULL, " ");
//         parse_args(args_pipe, argc_pipe, token, redirectname_pipe);

//         ret = execute_pipe_command(args, args_pipe, redirectname, redirectname_pipe); // Execute the piped command
        
//         // Free dynamically allocated memory for pipe arguments
//         for (int i = 0; i < argc_pipe; i++) {
//             free(args_pipe[i]);
//         }
//     } else {
//         ret = execute_command(args, redirectname); // Execute the command if no pipe is found
//     }

//     // Free dynamically allocated memory for arguments
//     for (int i = 0; i < argc; i++) {
//         free(args[i]);
//     }
//     return ret;
// }

// void batch_mode(const char *filename) {
//   char command[MAX_COMMAND_LENGTH];
//   int last_status = EXIT_SUCCESS;
//   FILE *file = fopen(filename, "r");
//   if (!file) {
//     perror("open file error");
//     exit(EXIT_FAILURE);
//   }
//   while (fgets(command, MAX_COMMAND_LENGTH, file)) {
//     command[strcspn(command, "\n")] = 0;
//     if (strlen(command) == 0) {
//         continue;
//     }
//     else if (strcmp(command, "exit") == 0) {
//       printf("mysh: exiting\n");
//       break;
//     }
//     last_status = parse_and_execute(command, last_status);
//   }

//   fclose(file);
// }

// void interactive_mode() {
//   char command[MAX_COMMAND_LENGTH];
//   int last_status = EXIT_SUCCESS;
//   printf("welcome to the shell！\n");
//   while (1) {
//     printf("mysh> ");
//     if (!fgets(command, MAX_COMMAND_LENGTH, stdin)) {
//       // if get wrong command
//       break;
//     }

//     // remove line break
//     command[strcspn(command, "\n")] = 0;

//     // if input 'exit', then 'exit'
//     if (strcmp(command, "exit") == 0) {
//       printf("mysh: exiting\n");
//       break;
//     }

//     last_status = parse_and_execute(command, last_status);
//   }
// }


// int main(int argc, char *argv[]) {
  
//   if (argc == 2) {
//     // batch_mode
//     batch_mode(argv[1]);
//   } else {
//     // interactive_mode first
//     interactive_mode();
//   }
//   return 0;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <fcntl.h>

// #define MAX_COMMAND_LENGTH 1024
// #define MAX_ARGS 64

// void print_prompt() {
//     printf("mysh> ");
//     fflush(stdout);
// }

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

// int main(int argc, char *argv[]) {
//     char command[MAX_COMMAND_LENGTH];
//     char *args[MAX_ARGS];
//     int num_args;
//     int exit_status = 0;

//     FILE *input_stream = stdin;
//     if (argc > 1) {
//         input_stream = fopen(argv[1], "r");
//         if (!input_stream) {
//             perror("fopen");
//             exit(EXIT_FAILURE);
//         }
//     }

//     if (isatty(fileno(input_stream))) {
//         printf("Welcome to my shell!\n");
//     }

//     while (1) {
//         if (input_stream == stdin) {
//             print_prompt();
//         }

//         if (!fgets(command, MAX_COMMAND_LENGTH, input_stream)) {
//             // End of input stream
//             break;
//         }

//         // Remove trailing newline
//         command[strcspn(command, "\n")] = '\0';

//         // Parse command into arguments
//         num_args = 0;
//         char *token = strtok(command, " ");
//         while (token != NULL) {
//             args[num_args++] = token;
//             token = strtok(NULL, " ");
//         }
//         args[num_args] = NULL;

//         // Handle built-in commands
//         if (strcmp(args[0], "cd") == 0) {
//             if (num_args != 2) {
//                 fprintf(stderr, "cd: Invalid number of arguments\n");
//                 continue;
//             }
//             if (chdir(args[1]) != 0) {
//                 perror("chdir");
//             }
//             continue;
//         } else if (strcmp(args[0], "pwd") == 0) {
//             char cwd[1024];
//             if (getcwd(cwd, sizeof(cwd)) != NULL) {
//                 printf("%s\n", cwd);
//             } else {
//                 perror("getcwd");
//             }
//             continue;
//         } else if (strcmp(args[0], "which") == 0) {
//             // Implementation of 'which' command
//             // Not implemented in this basic example
//             printf("which command not implemented\n");
//             continue;
//         } else if (strcmp(args[0], "exit") == 0) {
//             if (num_args > 1) {
//                 printf("Exiting with status: %s\n", args[1]);
//                 exit_status = atoi(args[1]);
//             }
//             break;
//         }

//         // Execute command
//         exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
//     }

//     if (isatty(fileno(input_stream))) {
//         printf("Exiting my shell.\n");
//     }

//     if (argc > 1) {
//         fclose(input_stream);
//     }

//     return exit_status;
// }
























// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <fcntl.h>

// #define MAX_COMMAND_LENGTH 1024
// #define MAX_ARGS 64

// void print_prompt() {
//     write(STDOUT_FILENO, "mysh> ", 6);
// }

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

// int main(int argc, char *argv[]) {
//     char command[MAX_COMMAND_LENGTH];
//     char *args[MAX_ARGS];
//     int num_args;
//     int exit_status = 0;

//     int input_fd = STDIN_FILENO;
//     if (argc > 1) {
//         input_fd = open(argv[1], O_RDONLY);
//         if (input_fd == -1) {
//             perror("open");
//             exit(EXIT_FAILURE);
//         }
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Welcome to my shell!\n", 21);
//     }

//     while (1) {
//         if (input_fd == STDIN_FILENO) {
//             print_prompt();
//         }

//         // Read input using read() instead of fgets()
//         ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
//         if (bytes_read <= 0) {
//             // End of input stream
//             break;
//         }
//         // Null terminate the string
//         command[bytes_read] = '\0';

//         // Parse command into arguments
//         num_args = 0;
//         char *token = strtok(command, " ");
//         while (token != NULL) {
//             args[num_args++] = token;
//             token = strtok(NULL, " ");
//         }
//         args[num_args] = NULL;

//         // Handle built-in commands
//         if (strcmp(args[0], "cd") == 0) {
//             if (num_args != 2) {
//                 write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
//                 continue;
//             }
//             if (chdir(args[1]) != 0) {
//                 perror("chdir");
//             }
//             continue;
//         } else if (strcmp(args[0], "pwd") == 0) {
//             char cwd[1024];
//             if (getcwd(cwd, sizeof(cwd)) != NULL) {
//                 write(STDOUT_FILENO, cwd, strlen(cwd));
//                 write(STDOUT_FILENO, "\n", 1);
//             } else {
//                 perror("getcwd");
//             }
//             continue;
//         } else if (strcmp(args[0], "which") == 0) {
//             // Implementation of 'which' command
//             // Not implemented in this basic example
//             write(STDOUT_FILENO, "which command not implemented\n", 30);
//             continue;
//         } else if (strcmp(args[0], "exit") == 0) {
//             if (num_args > 1) {
//                 write(STDOUT_FILENO, "Exiting with status: ", 22);
//                 write(STDOUT_FILENO, args[1], strlen(args[1]));
//                 write(STDOUT_FILENO, "\n", 1);
//                 exit_status = atoi(args[1]);
//             }
//             break;
//         }

//         // Execute command
//         exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Exiting my shell.\n", 18);
//     }

//     if (argc > 1) {
//         close(input_fd);
//     }

//     return exit_status;
// }






















// WHAT WORKS: CD, PWD, EXIT, INTERACTIVE
// WHAT NEEDS WORK: BATCH
// WHAT DOESN'T WORK: WILDCARDS, PIPING, REDIRECTING


// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <fcntl.h>
// #include <errno.h>

// #define MAX_COMMAND_LENGTH 1024
// #define MAX_ARGS 64

// void print_prompt() {
//     write(STDOUT_FILENO, "mysh> ", 6);
// }

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

// int main(int argc, char *argv[]) {
//     char command[MAX_COMMAND_LENGTH];
//     char *args[MAX_ARGS];
//     int num_args;
//     int exit_status = 0;

//     int input_fd = STDIN_FILENO;
//     if (argc > 1) {
//         input_fd = open(argv[1], O_RDONLY);
//         if (input_fd == -1) {
//             perror("open");
//             exit(EXIT_FAILURE);
//         }
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Welcome to my shell!\n", 21);
//     }

//     while (1) {
//         if (input_fd == STDIN_FILENO) {
//             print_prompt();
//         }

//         // Read input using read() instead of fgets()
//         ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
//         if (bytes_read <= 0) {
//             // End of input stream
//             break;
//         }
//         // Null terminate the string
//         command[bytes_read] = '\0';

//         // Parse command into arguments
//         num_args = 0;
//         char *token = strtok(command, " \n");
//         while (token != NULL) {
//             args[num_args++] = token;
//             token = strtok(NULL, " \n");
//         }
//         args[num_args] = NULL;

//         // Handle built-in commands
//         if (strcmp(args[0], "cd") == 0) {
//             if (num_args != 2) {
//                 write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
//                 continue;
//             }
//             if (chdir(args[1]) != 0) {
//                 perror("chdir");
//             }
//             continue;
//         } else if (strcmp(args[0], "pwd") == 0) {
//             char cwd[1024];
//             if (getcwd(cwd, sizeof(cwd)) != NULL) {
//                 write(STDOUT_FILENO, cwd, strlen(cwd));
//                 write(STDOUT_FILENO, "\n", 1);
//             } else {
//                 perror("getcwd");
//             }
//             continue;
//         } else if (strcmp(args[0], "which") == 0) {
//             if (num_args != 2) {
//                 write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
//                 continue;
//             }
//             char *path = getenv("PATH");
//             if (path != NULL) {
//                 char *token = strtok(path, ":");
//                 while (token != NULL) {
//                     char cmd_path[1024];
//                     snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, args[1]);
//                     if (access(cmd_path, X_OK) == 0) {
//                         write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
//                         write(STDOUT_FILENO, "\n", 1);
//                         break;
//                     }
//                     token = strtok(NULL, ":");
//                 }
//             } else {
//                 write(STDERR_FILENO, "which: PATH environment variable not set\n", 42);
//             }
//             continue;
//         } else if (strcmp(args[0], "exit") == 0) {
//             if (num_args > 1) {
//                 write(STDOUT_FILENO, "Exiting with status: ", 22);
//                 write(STDOUT_FILENO, args[1], strlen(args[1]));
//                 write(STDOUT_FILENO, "\n", 1);
//                 exit_status = atoi(args[1]);
//             }
//             break;
//         }

//         // Execute command
//         exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Exiting my shell.\n", 18);
//     }

//     if (argc > 1) {
//         close(input_fd);
//     }

//     return exit_status;
// }































// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <glob.h>

// #define MAX_COMMAND_LENGTH 1024
// #define MAX_ARGS 64

// void print_prompt() {
//     write(STDOUT_FILENO, "mysh> ", 6);
// }

// int execute_command(char **args, int input_fd, int output_fd, int *prev_exit_status) {
//     pid_t pid;
//     int fd[2];
//     int status;

//     if (strcmp(args[0], "cd") == 0) {
//         // Change directory
//         if (args[1] == NULL) {
//             // No directory specified
//             fprintf(stderr, "cd: No directory specified\n");
//             return -1;
//         }
//         if (chdir(args[1]) != 0) {
//             // Failed to change directory
//             perror("cd");
//             return -1;
//         }
//         return 0; // Success
//     } else if (strcmp(args[0], "pwd") == 0) {
//         // Print current directory
//         char cwd[1024];
//         if (getcwd(cwd, sizeof(cwd)) == NULL) {
//             perror("getcwd");
//             return -1;
//         }
//         printf("%s\n", cwd);
//         return 0; // Success
//     } else if (strcmp(args[0], "which") == 0) {
//         // Find executable path
//         if (args[1] == NULL) {
//             // No program specified
//             fprintf(stderr, "which: No program specified\n");
//             return -1;
//         }
//         // Search for program in PATH
//         char *path = getenv("PATH");
//         if (path == NULL) {
//             fprintf(stderr, "which: PATH environment variable not set\n");
//             return -1;
//         }
//         char *token = strtok(path, ":");
//         while (token != NULL) {
//             char program_path[1024];
//             snprintf(program_path, sizeof(program_path), "%s/%s", token, args[1]);
//             if (access(program_path, X_OK) == 0) {
//                 printf("%s\n", program_path);
//                 return 0; // Success
//             }
//             token = strtok(NULL, ":");
//         }
//         // Program not found in PATH
//         fprintf(stderr, "which: %s not found in PATH\n", args[1]);
//         return -1;
//     } else if (strcmp(args[0], "exit") == 0) {
//         // Exit shell
//         if (args[1] != NULL) {
//             // Print exit arguments
//             for (int i = 1; args[i] != NULL; i++) {
//                 printf("%s ", args[i]);
//             }
//             printf("\n");
//         }
//         exit(EXIT_SUCCESS);
//     }
//     else {
//         if (strcmp(args[0], "then") == 0 || strcmp(args[0], "else") == 0) {
//             // Conditional command
//             if (*prev_exit_status == 0 && strcmp(args[0], "else") == 0) {
//                 // Skip this command
//                 return 0;
//             } else if (*prev_exit_status != 0 && strcmp(args[0], "then") == 0) {
//                 // Skip this command
//                 return 0;
//             } else {
//                 // Execute the command
//                 pid = fork();
//                 if (pid == 0) {
//                     // Child process
//                     if (input_fd != STDIN_FILENO) {
//                         dup2(input_fd, STDIN_FILENO);
//                         close(input_fd);
//                     }
//                     if (output_fd != STDOUT_FILENO) {
//                         dup2(output_fd, STDOUT_FILENO);
//                         close(output_fd);
//                     }
//                     execvp(args[1], &args[1]);
//                     // If execvp returns, it means an error occurred
//                     perror("execvp");
//                     exit(EXIT_FAILURE);
//                 } else if (pid < 0) {
//                     // Fork failed
//                     perror("fork");
//                     return -1;
//                 } else {
//                     // Parent process
//                     waitpid(pid, &status, 0);
//                     *prev_exit_status = WEXITSTATUS(status);
//                     return *prev_exit_status;
//                 }
//             }
//         }

//         if (pipe(fd) == -1) {
//             perror("pipe");
//             return -1;
//         }

//         pid = fork();
//         if (pid == 0) {
//             // Child process
//             if (input_fd != STDIN_FILENO) {
//                 dup2(input_fd, STDIN_FILENO);
//                 close(input_fd);
//             }
//             close(fd[0]); // Close unused read end of the pipe
//             dup2(fd[1], STDOUT_FILENO);
//             close(fd[1]);

//             execvp(args[0], args);
//             // If execvp returns, it means an error occurred
//             perror("execvp");
//             exit(EXIT_FAILURE);
//         } else if (pid < 0) {
//             // Fork failed
//             perror("fork");
//             return -1;
//         }

//         // Parent process
//         waitpid(pid, &status, 0);
//         close(fd[1]); // Close write end of the pipe

//         // Execute the next command
//         pid = fork();
//         if (pid == 0) {
//             // Child process
//             dup2(fd[0], STDIN_FILENO);
//             close(fd[0]);

//             execvp(args[2], &args[2]);
//             // If execvp returns, it means an error occurred
//             perror("execvp");
//             exit(EXIT_FAILURE);
//         } else if (pid < 0) {
//             // Fork failed
//             perror("fork");
//             return -1;
//         }

//         close(fd[0]); // Close read end of the pipe
//         waitpid(pid, &status, 0);

//     }


//     return WEXITSTATUS(status);
// }

// int main(int argc, char *argv[]) {
//     char command[MAX_COMMAND_LENGTH];
//     char *args[MAX_ARGS];
//     int num_args;
//     int exit_status = 0;
//     int prev_exit_status = 0;

//     int input_fd = STDIN_FILENO;
//     if (argc > 1) {
//         input_fd = open(argv[1], O_RDONLY);
//         if (input_fd == -1) {
//             perror("open");
//             exit(EXIT_FAILURE);
//         }
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Welcome to my shell!\n", 21);
//     }

//     while (1) {
//         if (input_fd == STDIN_FILENO) {
//             print_prompt();
//         }

//         // Read input using read() instead of fgets()
//         ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
//         if (bytes_read <= 0) {
//             // End of input stream
//             break;
//         }
//         // Null terminate the string
//         command[bytes_read] = '\0';

//         // Parse command into arguments
//         num_args = 0;
//         char *token = strtok(command, " \n");
//         while (token != NULL) {
//             args[num_args++] = token;
//             token = strtok(NULL, " \n");
//         }
//         args[num_args] = NULL;

//         // Handle wildcards
//         int wildcard_start_index = num_args;
//         for (int i = 0; i < num_args; ++i) {
//             glob_t glob_result;
//             if (strstr(args[i], "*") != NULL) {
//                 if (glob(args[i], GLOB_NOCHECK, NULL, &glob_result) == 0) {
//                     for (int j = 0; j < glob_result.gl_pathc; ++j) {
//                         args[num_args++] = strdup(glob_result.gl_pathv[j]);
//                     }
//                     globfree(&glob_result);
//                     args[num_args] = NULL; // Terminate the args array
//                     break;
//                 }
//             }
//         }

//         // Execute command
//         exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO, &prev_exit_status);

//         // Free memory allocated for dynamically allocated strings after wildcard expansion
//         for (int i = wildcard_start_index; i < num_args; ++i) {
//             free(args[i]);
//         }
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Exiting my shell.\n", 18);
//     }

//     if (argc > 1) {
//         close(input_fd);
//     }

//     return exit_status;
// }
















// WHAT WORKS: CD, PWD, EXIT, INTERACTIVE, WILDCARDS, BARENAMES, BATCH, WHICH
// WHAT NEEDS WORK: 
// WHAT DOESN'T WORK: PIPING, REDIRECTING, CONDITIONALS, PATH-TO-PROGRAM
// OTHER ISSUES: PRESSING BLANK ENTER IN INTERACTIVE MODE => READ MEMORY ACCESS ERROR 



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

// useses execvp

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

// main loop for both batch and interactive
int run_shell(int input_fd) {
    char command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    char *paths[] = PATHS;
    int num_paths = NUM_PATHS;
    int num_args;
    int exit_status = 0;

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










        
        // else if (strcmp(args[0], "which") == 0) {
        //     if (num_args != 2) {
        //         write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
        //         continue;
        //     }
        //     char *path = getenv("PATH");
        //     if (path != NULL) {
        //         char *token = strtok(path, ":");
        //         while (token != NULL) {
        //             char cmd_path[1024];
        //             snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, args[1]);
        //             if (access(cmd_path, X_OK) == 0) {
        //                 write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
        //                 write(STDOUT_FILENO, "\n", 1);
        //                 break;
        //             }
        //             token = strtok(NULL, ":");
        //         }
        //     } else {
        //         write(STDERR_FILENO, "which: PATH environment variable not set\n", 42);
        //     }
        //     continue;
        // } 
        
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

        // Execute command
        exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
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



//ORIGINAL MAIN

// int main(int argc, char *argv[]) {
//     char command[MAX_COMMAND_LENGTH];
//     char *args[MAX_ARGS];
//     int num_args;
//     int exit_status = 0;

//     if (argc > 2) {
//         printf("The following program only takes in 1 additional argument: %s\n", argv[0]);
//         return 1;
//     }

//     int input_fd = STDIN_FILENO;
//     if (argc > 1) {
//         input_fd = open(argv[1], O_RDONLY);
//         if (input_fd == -1) {
//             perror("open");
//             exit(EXIT_FAILURE);
//         }
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Welcome to my shell!\n", 21);
//     }

//     while (1) {
//         if (isatty(input_fd)) {
//             print_prompt();
//         }

//         // reading input 
//         ssize_t bytes_read = read(input_fd, command, MAX_COMMAND_LENGTH);
//         if (bytes_read <= 0) {
//             // End of input stream
//             break;
//         }
//         // Null terminate the string
//         command[bytes_read] = '\0';

//         // Parse command into arguments
//         num_args = 0;
//         char *token = strtok(command, " \n");
//         while (token != NULL) {
//             args[num_args++] = token;
//             token = strtok(NULL, " \n");
//         }
//         args[num_args] = NULL;

//         if (strcmp(args[0], "cd") == 0) {
//             if (num_args != 2) {
//                 write(STDERR_FILENO, "cd: Invalid number of arguments\n", 32);
//                 continue;
//             }
//             if (chdir(args[1]) != 0) {
//                 perror("chdir");
//             }
//             continue;
//         } else if (strcmp(args[0], "pwd") == 0) {
//             char cwd[1024];
//             if (getcwd(cwd, sizeof(cwd)) != NULL) {
//                 write(STDOUT_FILENO, cwd, strlen(cwd));
//                 write(STDOUT_FILENO, "\n", 1);
//             } else {
//                 perror("getcwd");
//             }
//             continue;
//         } else if (strcmp(args[0], "which") == 0) {
//             if (num_args != 2) {
//                 write(STDERR_FILENO, "which: Invalid number of arguments\n", 36);
//                 continue;
//             }
//             char *path = getenv("PATH");
//             if (path != NULL) {
//                 char *token = strtok(path, ":");
//                 while (token != NULL) {
//                     char cmd_path[1024];
//                     snprintf(cmd_path, sizeof(cmd_path), "%s/%s", token, args[1]);
//                     if (access(cmd_path, X_OK) == 0) {
//                         write(STDOUT_FILENO, cmd_path, strlen(cmd_path));
//                         write(STDOUT_FILENO, "\n", 1);
//                         break;
//                     }
//                     token = strtok(NULL, ":");
//                 }
//             } else {
//                 write(STDERR_FILENO, "which: PATH environment variable not set\n", 42);
//             }
//             continue;
//         } else if (strcmp(args[0], "exit") == 0) {
//             if (num_args > 1) {
//                 write(STDOUT_FILENO, "Exiting with status: ", 22);
//                 write(STDOUT_FILENO, args[1], strlen(args[1]));
//                 write(STDOUT_FILENO, "\n", 1);
//                 exit_status = atoi(args[1]);
//             }
//             break;
//         }

//         // Handle wildcard expansion
//         num_args = handle_wildcard(args);

//         // Execute command
//         exit_status = execute_command(args, STDIN_FILENO, STDOUT_FILENO);
//     }

//     if (isatty(input_fd)) {
//         write(STDOUT_FILENO, "Exiting my shell.\n", 18);
//     }

//     if (argc > 1) {
//         close(input_fd);
//     }

//     return exit_status;
// }