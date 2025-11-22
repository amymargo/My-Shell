#ifndef EXECUTE_H
#define EXECUTE_H

#include "common.h"


/**
 * Helper function to help find the executable file for a given command.
 * @param name  The name of the command you want to find the executable path.
 * @return      A pointer to a string contianing the full path to the executable or NULL if it can't be found
 */
char *find_executable_path(const char *name);

/**
 * Helper function to help create a process of forking and executing a command in it.
 * @param path  The full path to the executable file
 * @param args  An array of arguments for the command
 * @return      The exit status of the executed command if it exits normally, or EXIT_FAILURE if it fails.
 */
int fork_and_execute(char *path, char **args);

/**
 * Executes a command with the given arguments with help from the helper functions.
 * @param args  An array of arguments, with the first argument being the command to execute.
 * @return      0 if the command executed successfully, or 1 if an error occurred.
 */
int execute(char **args);


/**
 * Executes a command with the given arguments on the path and searches for an executable file.
 * If found, forks a child process and executes the command.
 * @param args  An array of arguments, with the first argument being the command to execute.
 * @return      0 if the command executed successfully, or 1 if an error occurred.
 */
int executeInPath(char *name, char **args);


/**
 * Executes a command with the given arguments.
 * @param args  An array of strings representing the command and its arguments.
 * @return      The function returns 0 on success and -1 on failure.
 */
int executeCommand(char **tokens);

/**
 * Executes a command segment with input/output redirection.
 * @param segment   A CommandSegment struct containing the command segment to execute.
 * @return          The function returns 0 on success and -1 on failure.
 */
int executeCommandSegment(CommandSegment *segment);

/**
 * Executes a command with a given path.
 * @param args  An array of arguments, with the first argument being the path to execute.
 * @return      The function returns 0 if the command executed successfully, or 1 if an error occurred.
 */
int executeFromPath(char **args);

#endif