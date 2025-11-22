#ifndef COMMON_H
#define COMMON_H

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/**
 * @struct CommandSegment
 * @brief Represents a single command segment in a pipeline.
 * This struct contains the command arguments and any redirection files from the segment.
 */
typedef struct {
    char **argv;
    char *input_file;
    char *output_file;
} CommandSegment;

/**
 * This an external input from the commmand line.
 */
extern const char *commandLineInput;

/**
 * The function that parses the user input into tokens.
 * @param line  The line of user input to parse.
 * @return      An array of tokens, with a NULL terminator.
 */
char **tokenize_input(char *line);

// Helper function to process tokens and identify wildcards, redirections, and other special cases
void processTokens(char **tokens, CommandSegment *segments, int *num_segments);

/**
 * Prints an array of strings, which in this case is each individual token, followed by a newline character.
 * @param tokens The array of strings to print.
 */
void print_tokens(char **tokens);

/**
 * Counts the number of pipe symbols "|" in a given array of strings.
 * @param args  The array of tokens to search for pipe symbols.
 * @return      The number of pipe symbols found.
 */

int count_pipes(char **args);
/**
 * Counts the number of input/output redirection symbols "<" and ">" in a given array of strings.
 * @param args  The array of tokens to search for redirection symbols.
 * @return      The number of redirection symbols found.
 */
int count_redirections(char **args);

/**
 * Check if a given executable file is in the system PATH environment variable.
 * @param name The name of the executable file.
 * @return 1 if the executable file is found in the PATH, 0 otherwise.
 */
int isInPath(char *name);

/**
 * Remove quotes from a token if it has them.
 * @param token The token to remove quotes from.
 */
void remove_quotes(char *token);

#endif
