#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define BUF 1024


void run_shell_loop (int mode_fd) {
    char command[BUF];
    char *args[BUF-1];
    int is_interactive = isatty(STDIN_FILENO);      

    while (1) {
        if (is_interactive) {
            printf("mysh> ");
        }

        ssize_t bytes_read = read(mode_fd, command, BUF);

        ssize_t bytes_read = read(mode_fd, command, BUF);
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

char *read_line(void){
    int bufSize = BUF;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * bufSize);
    int a; 

    if(!buffer){
        printf(stderr, "mysh: allocation error \n"); 
        exit(EXIT_FAILURE);
    }

    while(1){
        a = getchar();
        
        if(a == EOF || a == '\n'){
            buffer[pos] = '\0';
            return buffer;
        } 
        else{
            buffer[pos] = a;
        }
        pos++;

        if(pos >= bufSize){
            bufSize += BUF;
            buffer = realloc(buffer, bufSize);
            if(!buffer){
                printf(stderr, "mysh: allocation error \n"); 
                exit(EXIT_FAILURE);
            }
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
