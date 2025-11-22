#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>

#include "../include/execute.h"
#include "../include/constants.h"
#include "../include/common.h"

const char *PATHS[NUM_PATHS] = {
    "/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin", "/sbin", "/bin"
};

//Helper function to find executable path
char *find_executable_path(const char *name) {
    char path[BUFFER_SIZE];
    for (int i = 0; i < NUM_PATHS; i++) {
        snprintf(path, sizeof(path), "%s/%s", PATHS[i], name);
        if (access(path, X_OK) == 0) {
            return strdup(path); // Duplicate path string
        }
    }
    return NULL;
}

//Helper function to handle forking and execution
int fork_and_execute(char *path, char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execv(path, args);
        perror("mysh: execv");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("mysh: fork");
        return EXIT_FAILURE;
    }
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}

int execute_then_else(char **args) {
    int status = 0;
    int last_status = 0;
    bool execute_next = true; // Flag to decide whether to execute the next command

    // Loop through each argument
    char *segments[100][100]; // Assuming a maximum of 100 segments with each having a maximum of 100 words
    int segment_count = 0;
    int word_count = 0;

        if (strcmp(args[0], "then") == 0 || strcmp(args[0], "else") == 0) {
        fprintf(stderr, "mysh: 'then' or 'else' with no preceding command\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; args[i] != NULL; i++) {
        // Check if the current argument is "then" or "else"
        if (strcmp(args[i], "then") == 0 || strcmp(args[i], "else") == 0) {
            segments[segment_count][word_count] = NULL; // Mark the end of the segment
            segment_count++; // Move to the next segment
            word_count = 0; // Reset word count for the next segment
            segments[segment_count][0] = strdup(args[i]); // Insert "then" or "else" as a new segment
            segment_count++; // Move to the next segment
        } else {
            segments[segment_count][word_count] = strdup(args[i]); // Insert the current word into the current segment
            word_count++; // Move to the next word in the segment
        }
    }

    // Add a null terminator at the end
    segments[segment_count][word_count] = NULL;

    for (int i = 0; segments[i][0] != NULL; i++) {
        if (strcmp(segments[i][0], "then") == 0) {
            execute_next = (last_status == 0); // Execute next command if last command succeeded
        } else if (strcmp(segments[i][0], "else") == 0) {
            execute_next = (last_status != 0); // Execute next command if last command failed
        } else {
            if (execute_next) {
                char *word[100]; // Array of pointers to characters
                memset(word, 0, sizeof(word)); // Initialize word array to NULL
                int j = 0;
                while (segments[i][j] != NULL) {
                    word[j] = strdup(segments[i][j]); // Allocate memory for the string and copy the content
                    j++;
                }
                // Concatenate the current word
                char command[100000] = ""; // Initialize command as an empty string
                for (int k = 0; word[k] != NULL; k++) {
                    strcat(command, word[k]);
                    strcat(command, " "); // Add a space as delimiter between words
                }
                // Execute the command
                status = execute(word);
                last_status = status;
                // Free memory allocated for word
                for (int k = 0; word[k] != NULL; k++) {
                    free(word[k]);
                }
            }
        }
    }

    return status;
}

int execute(char **args) {
    if (args[0] == NULL) {
        // No command entered
        return EXIT_SUCCESS;
    }

    // Handle built-in commands
    if (strcmp(args[0], "cd") == 0) {
        return cd(args); 
    } else if (strcmp(args[0], "pwd") == 0) {
        return pwd(); 
    } else if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS); // Or handle exit appropriately
    }

    // Handle other commands including those with pipes and redirections
    if (count_pipes(args) > 0 || count_redirections(args) > 0) {
        return executeCommand(args);
    }

    // For external commands, find the executable path and execute
    char *path = find_executable_path(args[0]);
    if (path) {
        int status = fork_and_execute(path, args);
        free(path);
        return status;
    }

    printf("mysh: command not found: %s\n", args[0]);
    return EXIT_FAILURE;
}

int executeFromPath(char **args) {
    if (access(args[0], X_OK) != 0) {
        printf("mysh: %s: command not found\n", args[0]);
        return EXIT_FAILURE;
    }
    return fork_and_execute(args[0], args);
}

int executeInPath(char *name, char **args) {
    // Process tokens to expand wildcards
    CommandSegment segments[MAX_SEGMENTS];
    int num_segments;
    processTokens(args, segments, &num_segments);

    if (access(name, X_OK) == 0) {
        return fork_and_execute(name, segments[0].argv);
    }

    char *path = find_executable_path(name);
    if (path) {
        int status = fork_and_execute(path, segments[0].argv);
        free(path);
        return status;
    }

    return EXIT_FAILURE;
}
int executeCommand(char **tokens) {
    CommandSegment segments[MAX_SEGMENTS];
    int num_segments;
    processTokens(tokens, segments, &num_segments);

    // Check for the presence of a pipe
    int num_pipes = count_pipes(tokens);

    // Handle single pipe
    if (num_pipes == 1 && num_segments == 2) {
        int pipefd[2];
        if (pipe(pipefd) < 0) {
            perror("mysh: pipe");
            return EXIT_FAILURE;
        }

        // Fork the first process
        pid_t pid1 = fork();
        if (pid1 == 0) {
            // Child process for the first command
            close(pipefd[0]);  // Close unused read end
            dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to write end of pipe
            close(pipefd[1]);  // Close the write end after duplicating

            executeCommandSegment(&segments[0]);
            exit(EXIT_FAILURE);  // Exit if exec fails
        }

        // Fork the second process
        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Child process for the second command
            close(pipefd[1]);  // Close unused write end
            dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to read end of pipe
            close(pipefd[0]);  // Close the read end after duplicating

            executeCommandSegment(&segments[1]);
            exit(EXIT_FAILURE);  // Exit if exec fails
        }

        // Parent process - close both ends of the pipe and wait for both children
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

        return EXIT_SUCCESS;
    } else if (num_pipes > 1) {
        fprintf(stderr, "mysh: only single pipes are supported\n");
        return EXIT_FAILURE;
    } else {
        // Handle commands without pipes, which could include redirection
        for (int i = 0; i < num_segments; i++) {
            int status = executeCommandSegment(&segments[i]);
            if (status != EXIT_SUCCESS) {
                return status;  // Return the status if command execution fails
            }
        }
    }

    return EXIT_SUCCESS;
}

int executeCommandSegment(CommandSegment *segment) {
    int input_fd = -1, output_fd = -1;

    // Handle input redirection
    if (segment->input_file != NULL) {
        input_fd = open(segment->input_file, O_RDONLY);
        if (input_fd < 0) {
            perror("mysh: open input");
            return EXIT_FAILURE;
        }
    }

    // Handle output redirection
    if (segment->output_file != NULL) {
        output_fd = open(segment->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (output_fd < 0) {
            perror("mysh: open output");
            if (input_fd >= 0) close(input_fd);
            return EXIT_FAILURE;
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (input_fd >= 0) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd >= 0) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        char *path = find_executable_path(segment->argv[0]);
        if (path == NULL) {
            fprintf(stderr, "mysh: command not found: %s\n", segment->argv[0]);
            exit(EXIT_FAILURE);
        }
        execv(path, segment->argv);
        perror("mysh: execv");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("mysh: fork");
        return EXIT_FAILURE;
    }

    // Parent process
    if (input_fd >= 0) close(input_fd);
    if (output_fd >= 0) close(output_fd);
    waitpid(pid, NULL, 0);

    return EXIT_SUCCESS;
}