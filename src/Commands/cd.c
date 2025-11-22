#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../../include/common.h"
#include "../../include/cd.h"

int cd(char **args) {
    // Default directory (HOME) when no argument is provided
    char *directory = (args[1] == NULL || args[1][0] == '~') ? getenv("HOME") : args[1];

    // Attempt to change the directory
    if (chdir(directory) != 0) {
        fprintf(stderr, "cd: No such file or directory\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
