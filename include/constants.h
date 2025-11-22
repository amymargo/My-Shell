#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 * Buffer size.
 */
#define BUFFER_SIZE 1024

/**
 * Maximum number of allowed commands.
 */
#define MAX_COMMAND_LENGTH 1000

/**
 * Maximum number of allowed commands segments.
 */
#define MAX_SEGMENTS 100

/**
 * Maximum number of arguments.
 */
#define MAX_NUM_ARGUMENTS 100

/**
 * Number of paths to search for commands.
 */
#define NUM_PATHS 6

/**
 * Array of paths to search for commands.
 */
extern const char *PATHS[NUM_PATHS];

#endif