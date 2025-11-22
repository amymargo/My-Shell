#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <glob.h>

#include "../include/common.h"
#include "../include/constants.h"

#define DELIMITERS " \t\r\n\a|<>"

const char *commandLineInput;

char **tokenize_input(char *input) {
    char **tokens = malloc(256 * sizeof(char *));
    int num_tokens = 0;

    char *start = input;
    char *end = input;
    int escaped = 0;

    while (*end != '\0') {
        if (!escaped && strchr(DELIMITERS, *end) != NULL) {
            if (start != end) {
                // Handling multi-character tokens
                char *token = malloc((end - start + 1) * sizeof(char));
                char *token_ptr = token;
                for (char *ptr = start; ptr < end; ptr++) {
                    *token_ptr++ = *ptr;
                }
                *token_ptr = '\0';
                
                // Check for wildcard
                if (strchr(token, '*') != NULL) {
                    glob_t globbuf;
                    int result = glob(token, GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);
                    if (result == 0) {
                        //num_tokens--;
                        // Add all matching filenames to the tokens array
                        for (size_t j = 0; j < globbuf.gl_pathc; j++) {
                            tokens[num_tokens] = strdup(globbuf.gl_pathv[j]);
                            num_tokens++;
                        }
                        globfree(&globbuf);
                        free(token);
                    }
                }else{
                    tokens[num_tokens++] = token;
                }
            }

            
            // Handling single-character tokens (e.g., '|')
            if (*end != '\"' && strchr("|<>", *end) != NULL) { // Check if the character is not a quote
                char *token = malloc(2 * sizeof(char));
                token[0] = *end;
                token[1] = '\0';
                tokens[num_tokens++] = token;
            }
            // Skip whitespace after a single-character token
            do {
                end++;
            } while (*end == ' ' || *end == '\t' || *end == '\"'); // Add check for quote here too
            start = end;
            continue;
        } else if (*end == '\\') {
            escaped = !escaped;
            if (escaped) {
                end++;
                continue;
            }
        } else {
            escaped = 0;
        }
        end++;
    }

    if (start != end) {
        // Handling the last token
        char *token = malloc((end - start + 1) * sizeof(char));
        char *token_ptr = token;
        for (char *ptr = start; ptr < end; ptr++) {
            if (*ptr != '\\') {
                *token_ptr++ = *ptr;
            } else if (*(ptr + 1) == '\\') {
                *token_ptr++ = *ptr;
                ptr++;
            }
        }
        *token_ptr = '\0';
        tokens[num_tokens++] = token;
    }

    tokens[num_tokens] = NULL;
    /*for (int i; i<num_tokens;i++){
        printf("%s\n",tokens[i]);
    }*/
    return tokens;
}

void processTokens(char **tokens, CommandSegment *segments, int *num_segments)
{
    int seg_idx = 0;
    int arg_idx = 0;
    CommandSegment *cur_seg = &segments[seg_idx];
    cur_seg->argv = (char **)malloc(sizeof(char *) * MAX_NUM_ARGUMENTS);
    cur_seg->input_file = NULL;
    cur_seg->output_file = NULL;

    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "|") == 0)
        {
            cur_seg->argv[arg_idx] = NULL;
            seg_idx++;
            cur_seg = &segments[seg_idx];
            cur_seg->argv = (char **)malloc(sizeof(char *) * MAX_NUM_ARGUMENTS);
            cur_seg->input_file = NULL;
            cur_seg->output_file = NULL;
            arg_idx = 0;
        }
        else if (strcmp(tokens[i], ">") == 0)
        {
            i++;
            cur_seg->output_file = tokens[i];
        }
        else if (strcmp(tokens[i], "<") == 0)
        {
            i++;
            cur_seg->input_file = tokens[i];
        }
        else
        {
            // Check if the token contains a wildcard
            glob_t globbuf;
            int result = glob(tokens[i], GLOB_NOCHECK | GLOB_TILDE, NULL, &globbuf);
            if (result == 0)
            {
                // Add all matching filenames to the current segment's argv
                for (size_t j = 0; j < globbuf.gl_pathc; j++)
                {
                    cur_seg->argv[arg_idx++] = strdup(globbuf.gl_pathv[j]);
                }
                globfree(&globbuf);
            }
            else
            {
                cur_seg->argv[arg_idx++] = tokens[i];
            }
        }
    }
    cur_seg->argv[arg_idx] = NULL;
    *num_segments = seg_idx + 1;
}



void print_tokens(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        printf("Token %d: %s\n", i, tokens[i]);
    }
}

int count_pipes(char **args)
{
    int count = 0;
    for (int i = 0; args[i] != NULL; i++)
    {
        if (args[i][0] == '|')
        {
            count++;
        }
    }
    return count;
}

int count_redirections(char **args)
{
    int count = 0;
    for (int i = 0; args[i] != NULL; i++)
    {
        if (args[i][0] == '<' || args[i][0] == '>')
        {
            count++;
        }
    }
    return count;
}

int isInPath(char *name)
{
    char path[BUFFER_SIZE];
    int i;
    for (i = 0; i < NUM_PATHS; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", PATHS[i], name);
        if (access(path, X_OK) == 0)
        {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

void remove_quotes(char *token) {
    int len = strlen(token);
    if (len >= 2) {
        // Check if token starts and ends with the same type of quote
        if ((token[0] == '\'' && token[len-1] == '\'') ||
            (token[0] == '"' && token[len-1] == '"')) {
            // Token has matching quotes, so remove them
            memmove(token, token + 1, len - 2);
            token[len-2] = '\0';
        }
    }
}