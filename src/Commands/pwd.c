#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/pwd.h"

int pwd()
{
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        perror("getcwd");
        return EXIT_FAILURE;
    }
    printf("%s\n", cwd);
    free(cwd);
    return EXIT_SUCCESS;
}