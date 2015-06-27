#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    int n;

    // Check inputs
    if (argc != 2) {
        printf("This program expects exactly 1 parameter not: %d\n", argc);
        return 0;
    }

    n = atoi(argv[1]);
    if (n < 1) {
        printf("The collatz conjecture only works for numbers > 0.\n");
        return 0;
    }

    /* fork a child process */
    pid = fork();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0) { /* child process */
        while (n != 1) {
            if (n & 1)
                n = 3*n + 1;
            else
                n >>= 1;
            printf("%d\n", n);
        }
        exit(0);
    }
    else { /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL);
        printf("Child Complete\n");
    }

    return 0;
}

