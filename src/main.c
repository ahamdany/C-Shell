#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <pwd.h>
#include "sfish.h"

#include "sfish.h"
#include "debug.h"


int main(int argc, char *argv[], char* envp[]) {
    char* input;
    bool exited = false;


    if(!isatty(STDIN_FILENO)) {
        // If your shell is reading from a piped file
        // Don't have readline write anything to that file.
        // Such as the prompt or "user input"
        if((rl_outstream = fopen("/dev/null", "w")) == NULL){
            perror("Failed trying to open DEVNULL");
            exit(EXIT_FAILURE);
        }
    }

    do {
        char* dir_structure = calloc(512,sizeof(char*));
        dir_structure = create_current_directory();
        char* prompt = calloc(512,sizeof(char*));
        strcpy(prompt,dir_structure);
        strcat(prompt," :: ahamdany >>");
        input = readline(prompt);
        
        // If EOF is read (aka ^D) readline returns NULL
        if(input == NULL) {
            exit(3);
        }else if(*input == '\n'){
            continue;
        }else if(strcmp(input,"")==0){
            continue;
        }else if(redirect(input)){
            continue;
        }else if(check_and_execute_builtin_requests(input)){
            continue;
        }else if(execute(input)){
            continue;
        }
        
        printf(EXEC_ERROR, input);

        // You should change exit to a "builtin" for your hw.
        exited = strcmp(input, "exit") == 0;

        // Readline mallocs the space for input. You must free it.
        rl_free(input);

    } while(!exited);

    debug("%s", "user entered 'exit'");

    return EXIT_SUCCESS;
}
