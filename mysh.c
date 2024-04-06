#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <glob.h>

#define DELIM " \n"
#define LINE_BUF 512
#define TOKEN_BUF 64

char* parse_command(int input_fd);
char** tokenize(char* line);

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

    while (*line) {
        // check if the current character is a delimiter
        if (strchr(delim, *line) != NULL) {
            // replace the delimiter with a null terminator to end the current token
            *line = '\0';
            // if token is not NULL (i.e., there is content), add it to tokens array
            if (token != NULL) {
                // allocate memory for the token
                tokens[pos] = token;
                pos++;
                // check if we need to reallocate memory for tokens array
                if (pos >= bufSize) {
                    bufSize += TOKEN_BUF;
                    tokens = realloc(tokens, bufSize * sizeof(char*));
                    if (!tokens) {
                        printf("mysh: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
                // reset token pointer to NULL for the next token
                token = NULL;
            }
        } else {
            // if the character is not a delimiter, check if token is NULL
            // if token is NULL, it means we have encountered the start of a new token
            if (token == NULL) {
                // set the token pointer to the current position in the line
                token = line;
            }
        }
        // move to the next character in the line
        line++;
    }
    // add the last token to tokens array if it exists
    if (token != NULL) {
        tokens[pos] = token;
        pos++;
    }
    // add NULL as the last element to indicate the end of tokens array
    tokens[pos] = NULL;
    return tokens;
}


// function used to parse through input lines (either file or standard input)
char *parse_command(int input_fd) {
    int bufSize = LINE_BUF;      // buffer size for command length
    int pos = 0;                 // position of bufSize
    char *buffer = (char*)malloc(sizeof(char) * bufSize);   // malloc buffer for command

    if(!buffer){
        printf("mysh: allocation error \n"); 
        exit(EXIT_FAILURE);
    }

    // parses through input lines (standard input or file)
    while(1) {
        ssize_t bytes_read = read(input_fd, &buffer[pos], 1);  
        if (bytes_read < 0) {
            printf("Error reading command\n");
            exit(EXIT_FAILURE);
        } 
        else if (isatty(input_fd)) {      // if in interactive mode
            if (bytes_read == 0) {
                printf("End of input. Exiting shell program.\n");
                buffer[pos] = '\0';
                return buffer;
            } else buffer[pos] = bytes_read;
            // break; why did i break?
        } 
        else if (!(isatty(input_fd))) {   // if in bash mode
            if (bytes_read == 0 || buffer[pos] == '\n') {
                buffer[pos] = '\0';
                return buffer;
            } else buffer[pos] = bytes_read;
        }
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
void run_shell_loop (int input_fd) { 
    char *command;
    char **args;
    int is_interactive = isatty(STDIN_FILENO);      // is from terminal?

    printf("step 1");
    while (1) {
        printf("step 2");
        

        if (is_interactive) {
            printf("mysh> ");
        }

        // reads line of input
        command = parse_command(input_fd);

        // tokenizes line of input
        args = tokenize(command);

        //wildcard *
        if(strchr("*", args) != NULL){
            
        }



        //exit code part 
        if(strchr("exit", args) != NULL){
            exit(EXIT_SUCCESS); 
        }

        // missing execution
        free(command);
        free(args);

    } 

    if (is_interactive) {
        printf("Goodbye!");
    }

}

int main(int argc, char const *argv[])
{
    // file descriptor for input
    int input_fd;

    // this program takes in at most 2 arguments including the program name. any more results in error
    if (argc > 2) {
        printf("The program, %s, takes in either 0 or 1 argument(s)\n", argv[0]);
        return 1;
    }

    // used to determine whether the program will run in interactive or bash mode
    if (isatty(STDIN_FILENO)) 
        input_fd = STDIN_FILENO;
    else input_fd = open(argv[1], O_RDONLY);

    // check for fd error
    if (input_fd < 0) {
        perror("Error reading from input");
        return EXIT_FAILURE;
    }

    // one major loop that runs the shell in both modes
    run_shell_loop(input_fd);

    return EXIT_SUCCESS;
}










































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
        char searchpath[3][FILENAME_MAX] = {"/usr/local/bin/", "/usr/bin/", "/bin/" };
        for(int i = 0; i < 3; i++) {
            strcat(searchpath[i], command);
            if(access(searchpath[i], X_OK) == 0) {
                strcpy(path, searchpath[i]);
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
            fprintf(stderr, "cd: Error Number of Parameters\n");
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
            fprintf(stderr, "which: must provide one and only one parameter");
            return EXIT_FAILURE;
        }
        char pathname[FILENAME_MAX] = "";
        if(get_command_path_name(argv[1], pathname) == EXIT_FAILURE) {
            fprintf(stderr, "which: command not found\n");
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
    

    if(get_command_path_name(argv[0], command_pathname) == EXIT_FAILURE) {
        restore_std(saved_stdin, saved_stdout);
        return EXIT_FAILURE;
    }

    if(command_pathname[0] == '\0') {
        int ret = execute_built_in_command(argv);
        restore_std(saved_stdin, saved_stdout);
        return ret;
    }
    else {
        // external command
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
        }
        else if (pid == 0) {
            // child progress
        if (execv(argv[0], argv) == -1) {
                perror("execv");
                exit(EXIT_FAILURE);
            }
        }
        else {
            // father progress
            int status;
            waitpid(pid, &status, 0);
            if (!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)) {
                restore_std(saved_stdin, saved_stdout);
                return EXIT_FAILURE;
            }
        }
    }
    restore_std(saved_stdin, saved_stdout);
    return EXIT_SUCCESS;
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

void batch_mode(const char *filename) {
  char command[MAX_COMMAND_LENGTH];
  int last_status = EXIT_SUCCESS;
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("open file error");
    exit(EXIT_FAILURE);
  }
  while (fgets(command, MAX_COMMAND_LENGTH, file)) {
    command[strcspn(command, "\n")] = 0;
    if (strlen(command) == 0) {
        continue;
    }
    else if (strcmp(command, "exit") == 0) {
      printf("mysh: exiting\n");
      break;
    }
    last_status = parse_and_execute(command, last_status);
  }

  fclose(file);
}

void interactive_mode() {
  char command[MAX_COMMAND_LENGTH];
  int last_status = EXIT_SUCCESS;
  printf("welcome to the shell！\n");
  while (1) {
    printf("mysh> ");
    if (!fgets(command, MAX_COMMAND_LENGTH, stdin)) {
      // if get wrong command
      break;
    }

    // remove line break
    command[strcspn(command, "\n")] = 0;

    // if input 'exit', then 'exit'
    if (strcmp(command, "exit") == 0) {
      printf("mysh: exiting\n");
      break;
    }

    last_status = parse_and_execute(command, last_status);
  }
}


int main(int argc, char *argv[]) {
  
  if (argc == 2) {
    // batch_mode
    batch_mode(argv[1]);
  } else {
    // interactive_mode first
    interactive_mode();
  }
  return 0;
}