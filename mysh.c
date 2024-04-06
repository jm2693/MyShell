#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

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
        else if(strchr("<", *line) != NULL || strchr(">", *line) || strchr("|", *line)){
            token[pos] = *line;
            pos++;
            if (pos >= bufSize) {
                    bufSize += TOKEN_BUF;
                    tokens = realloc(tokens, bufSize * sizeof(char*));
                    if (!tokens) {
                        printf("mysh: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
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

        // missing execution
        free(command);
        free(args);

        if (is_interactive) {
            printf("Goodbye!");
        }
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
