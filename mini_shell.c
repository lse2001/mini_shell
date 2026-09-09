// Name: Lucas Seitz
// CECS 326 - Programming Assignment 1

#include <stdio.h>      // printf(), fgets()
#include <stdlib.h>     // exit()
#include <string.h>     // strcmp(), strtok(), strlen()
#include <unistd.h>     // fork(), execvp()
#include <sys/wait.h>   // waitpid(), WIFEXITED(), etc.
#include <errno.h>      // errno

#define MAX_ARGS 64

int main(void) {
    char input[1024];

    while (1) {
        printf("osh> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = '\0';

        // Ignore blank lines
        if (strlen(input) == 0) {
            continue;
        }

        // Built-in exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // Parse command into arguments
        char *args[MAX_ARGS];
        int argCount = 0;

        char *token = strtok(input, " \t");

        while (token != NULL && argCount < MAX_ARGS - 1) {
            args[argCount] = token;
            argCount++;

            token = strtok(NULL, " \t");
        }

        args[argCount] = NULL;

        if (argCount == 0) {
            continue;
        }

        // Create a child process
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            execvp(args[0], args);

            // execvp only returns if it fails
            fprintf(stderr, "mini_shell: %s: %s\n", args[0], strerror(errno));
            exit(127);
        }

        else if (pid > 0) {
            int status;

            printf("[parent] created child PID %d\n", pid);

            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                continue;
            }

            if (WIFEXITED(status)) {
                printf("[parent] child %d exited with status %d\n",
                       pid, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status)) {
                printf("[parent] child %d terminated by signal %d\n",
                       pid, WTERMSIG(status));
            }
        }

        else {
            // fork() failed
            perror("fork");
        }
    }

    return 0;
}
