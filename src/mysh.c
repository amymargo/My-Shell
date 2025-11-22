#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#include "../include/execute.h"
#include "../include/constants.h"
#include "../include/common.h"
#include "../include/cd.h"
#include "../include/pwd.h"

int main(int argc, char **argv)
{
    char line[BUFFER_SIZE];
    char **args;
    int status = 0;
    int last_status = 0;        // Track the exit status of the last executed command
    FILE *input_stream = stdin; // Default to standard input
    bool interactive_mode = (isatty(fileno(stdin)) && (argc == 1));

    if (argc > 1)
    {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL)
        {
            perror("mysh: unable to open file");
            return -1;
        }
    }
    else if (interactive_mode)
    {
        printf("Welcome to my shell!\n");
    }

    while (1)
    {
        if (interactive_mode)
        {
            printf("mysh> ");
        }

        if (!fgets(line, BUFFER_SIZE, input_stream))
        {
            if (input_stream != stdin)
            {
                fclose(input_stream);
            }
            break;
        }

        args = tokenize_input(line);

        // Check for empty command or command starting with a pipe
        
        // Check for empty command or command starting with a pipe
if (args[0] == NULL || strcmp(args[0], "|") == 0) {
    fprintf(stderr, "mysh: invalid command\n");
    free(args);
    continue; // Skip to the next command
}

int has_then_else = 0;
    for (int i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "then") == 0 || strcmp(args[i], "else") == 0)
        {
            has_then_else = 1;
            break;
        }
    }

// Flag to indicate if an invalid then/else usage is found after a pipe
int invalid_then_else_after_pipe = 0;
int pipe_encountered = 0;

for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], "|") == 0) {
        pipe_encountered = 1; // Mark that a pipe has been encountered
    } else if (pipe_encountered && (strcmp(args[i], "then") == 0 || strcmp(args[i], "else") == 0)) {
        fprintf(stderr, "mysh: 'then' or 'else' not allowed in a piped command sequence\n");
        invalid_then_else_after_pipe = 1;
        break; // Exit the loop as an invalid usage is found
    }
}

if (invalid_then_else_after_pipe) {
    free(args);
    continue; // Skip to the next command
}

if (has_then_else==0){
    // Handle built-in commands
if (strcmp(args[0], "cd") == 0) {
    status = cd(args);
} else if (strcmp(args[0], "pwd") == 0) {
    status = pwd();
} else if (strcmp(args[0], "exit") == 0) {
    status = 0;
    printf("mysh: exiting\n");
    break;
} else {
    // Regular command execution
    status = execute(args);
    last_status = status;
}}
if (has_then_else==1){
    status= execute_then_else(args);
    last_status = status;
}

        free(args);
    }

    return status;
}