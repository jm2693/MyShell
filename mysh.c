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
        // Check if the current character is a delimiter
        if (strchr(delim, *line) != NULL) {
            // Replace the delimiter with a null terminator to end the current token
            *line = '\0';
            // If token is not NULL (i.e., there is content), add it to tokens array
            if (token != NULL) {
                // Allocate memory for the token
                tokens[pos] = token;
                pos++;
                // Check if we need to reallocate memory for tokens array
                if (pos >= bufSize) {
                    bufSize += TOKEN_BUF;
                    tokens = realloc(tokens, bufSize * sizeof(char*));
                    if (!tokens) {
                        printf("mysh: allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
                // Reset token pointer to NULL for the next token
                token = NULL;
            }
        } else {
            // If the character is not a delimiter, check if token is NULL
            // If token is NULL, it means we have encountered the start of a new token
            if (token == NULL) {
                // Set the token pointer to the current position in the line
                token = line;
            }
        }
        // Move to the next character in the line
        line++;
    }
    // Add the last token to tokens array if it exists
    if (token != NULL) {
        tokens[pos] = token;
        pos++;
    }
    // Add NULL as the last element to indicate the end of tokens array
    tokens[pos] = NULL;
    return tokens;
}

// function used to parse through input lines (either file or standard input)
char *read_line(){
    int bufSize = LINE_BUF;      // buffer size for command length
    int pos = 0;            // position of bufSize
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
            continue;
        } else if (isatty(STDIN_FILENO)) {      // if in interactive mode
            if (bytes_read == 0)
                printf("End of input. Exiting shell program.\n");
            break;
        } else if (!(isatty(STDIN_FILENO))) {   // if in bash mode
            if (bytes_read == 0 || buffer[pos] == '\n') {
                buffer[pos] = '\0';
                return buffer;
            }
        } else pos++;

        // add size of buffer if runs out of memory
        if (pos >= bufSize){
            bufSize += LINE_BUF;
            buffer = realloc(buffer, bufSize);
            if(!buffer){
                printf(stderr, "mysh: allocation error \n"); 
                exit(EXIT_FAILURE);
            }
        }
    }
}

// shell loop that will constantly check for input and parse arguments
void run_shell_loop () {
    char *command;
    char **args;
    int is_interactive = isatty(STDIN_FILENO);      // is from terminal?

    while (1) {
        if (is_interactive) {
            printf("mysh> ");
        }

        command = read_line();
        args = tokenize(read_line);
        // missing execution

        free(command);
        free(args);
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
