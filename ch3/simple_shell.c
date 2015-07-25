/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* 80 chars per line, per command */
#define MAX_LINE 80

int get_cmd_input(char* cmd_input) {
    int c;
    int i = 0;
    while (1) { /* skip leading whitespace */
        c = getchar();
        if (c == EOF || c == '\n')
            return 0;
        if (!isspace(c)) {
            ungetc(c, stdin);
            break;
        }
    }

    while (1) {
        c = getchar();
        if (c == '\n' || c == EOF) {/* at end, add terminating zero */
            cmd_input[i] = 0;
            return 1;
        }
        cmd_input[i] = c;

        if (i == MAX_LINE - 1) { /* buffer full */
            cmd_input[i] = 0;
            return 1;
        }
        i++;
    }
}

int main(void)
{
    char *args[MAX_LINE >> 1 + 1];  /* command line (of 80) has max of 40 arguments */
    char cmd_input[MAX_LINE];
    int should_run = 1;
    int i, upper, num_args, is_background, j;
    pid_t pid;

    while (should_run){
        is_background = 0;
        printf("osh> ");
        fflush(stdout);

        /**
         * After reading user input, the steps are:
         * (1) fork a child process
         * (2) the child process will invoke execvp()
         * (3) if command included &, parent will invoke wait()
         */
        if (get_cmd_input(cmd_input)) {
            num_args = 0;
            // split command and args
            char* pch = strtok(cmd_input, " ");
            int status;
            pid_t returned_child;
            while (pch != NULL) {
                args[num_args] = pch;
                pch = strtok(NULL, " ");
                num_args++;
            }

            args[num_args] = NULL;

            is_background = num_args && (strcmp(args[num_args-1], "&") == 0);
            if (is_background)
                args[num_args-1] = NULL;

            pid = fork();

            if (pid < 0 ){
                fprintf(stderr, "Fork Failed\n");
                return 1;
            }
            else if (pid) { // Parent process
                if (is_background) {
                    returned_child = waitpid(pid, NULL, WNOHANG);
                    printf("wait_returned_child: %d, pid: %d\n", returned_child, pid);
                }
                else {
                    returned_child = wait(&status);
                    printf("wait_returned_child: %d, pid: %d\n", returned_child, pid);
//                    waitpid(pid, NULL, 0);
                }
            }
            else { // Child process
                execvp(args[0], args);
                exit(0);
            }
        }
        memset(cmd_input, 0, MAX_LINE);
        memset(args, 0, MAX_LINE >> 1 + 1);
    }

    return 0;
}
