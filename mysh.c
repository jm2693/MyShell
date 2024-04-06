#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define DELIM " \t\r\n\a"
#define LINE_BUF 512
#define TOKEN_BUF 64

char* read_command();
char** tokenize(char* line);

void print_prompt() {
    if (isatty(STDIN_FILENO)) {
        printf("Welcome to my shell!\n");
    }
}

void print_goodbye() {
    if (isatty(STDIN_FILENO)) {
        printf("Exiting my shell.\n");
    }
}

char **tokenize(char* line) {
    int bufSize = TOKEN_BUF;
    int pos = 0;
    char **tokens = (char**)malloc(bufSize * sizeof(char*));
    char *token;
    char *delim = DELIM;

    if (!tokens) {
        printf("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, delim);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= bufSize) {
        bufSize += TOKEN_BUF;
        tokens = realloc(tokens, bufSize * sizeof(char*));
        if (!tokens) {
            fprintf(stderr, "lsh: allocation error\n");
            exit(EXIT_FAILURE);
        }
        }

        token = strtok(NULL, delim);
    }

    // add NULL as the last element to indicate the end of tokens array
    tokens[pos] = NULL;
    return tokens;
}

// function used to parse through input lines (either file or standard input)
char *read_command() {
    int bufSize = LINE_BUF;      // buffer size for command length
    int pos = 0;                 // position of bufSize
    char *buffer = (char*)malloc(sizeof(char) * bufSize);   // malloc buffer for command

    if(!buffer){
        printf("mysh: allocation error \n"); 
        exit(EXIT_FAILURE);
    }

    // parses through input lines (standard input or file)
    while(1) {
        ssize_t bytes_read = read(STDIN_FILENO, &buffer[pos], 1);  
        if (bytes_read < 0) {
            printf("Error reading command\n");
            exit(EXIT_FAILURE);
        } 
        else if (bytes_read == 0 || buffer[pos] == '\n') {
            buffer[pos] = '\0';
            return buffer;
        }
        else buffer[pos] = bytes_read;
        // else if ((isatty(STDIN_FILENO)) == 1) {      // if in interactive mode
        //     if (bytes_read == 0 || buffer[pos] == '\n') {
        //         buffer[pos] = '\0';
        //         return buffer;
        //     } else buffer[pos] = bytes_read;
        // } 
        // else if (isatty(STDIN_FILENO) != 1) {        // if in bash mode
        //     if (bytes_read == 0 || buffer[pos] == '\n') {
        //         buffer[pos] = '\0';
        //         return buffer;
        //     } else buffer[pos] = bytes_read;
        // }
        pos++;

        // add size of buffer if runs out of memory
        if (pos >= bufSize){
            bufSize += LINE_BUF;
            buffer = realloc(buffer, bufSize);
            if(!buffer){
                printf("mysh: allocation error \n"); 
                exit(EXIT_FAILURE);
            }
        }
    }
}

// shell loop that will constantly check for input and parse arguments
void run_shell_loop (int is_interactive) { 
    char *command;
    char **args;

    if (is_interactive == 1) {
            printf("mysh> ");
    } 
    else if (is_interactive == 0) printf("goodbye ");

    while (1) {
        
        if (is_interactive == 1)
            printf("mysh> ");

        // reads line of input
        command = read_command();

        // tokenizes line of input
        args = tokenize(command);

        // missing execution

        free(command);
        free(args);
    } 

}

int main(int argc, char **argv)
{
    // interactive or batch
    int is_interactive = isatty(STDIN_FILENO);
    printf("%d\n", is_interactive);

    // this program takes in at most 2 arguments including the program name. any more results in error
    if (argc > 2) {
        printf("The program, %s, takes in either 0 or 1 argument(s)\n", argv[0]);
        return 1;
    } 

    // welcome prompt if interactive
    print_prompt();

    // check for fd error
    if (STDIN_FILENO < 0) {
        perror("Error reading from input");
        return EXIT_FAILURE;
    }

    // one major loop that runs the shell in both modes
    run_shell_loop(is_interactive);

    return EXIT_SUCCESS;
}














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
