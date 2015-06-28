/**
 * Example program demonstrating UNIX pipes.
 *
 * Figures 3.25 & 3.26
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 512
#define READ_END    0
#define WRITE_END   1

int main(int argc, char* argv[])
{
    pid_t pid;
    int fd[2];

    if (argc != 3) {
        printf("ERROR: Incorrect number of parameters.\n");
        return 0;
    }

    if (strlen(argv[1]) > BUFFER_SIZE) {
        printf("ERROR: Incorrect number of parameters.\n");
        return 0;
    }

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr,"Pipe failed");
        return 1;
    }

    /* now fork a child process */
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed");
        return 1;
    }

    if (pid > 0) {  /* parent process */
        /* Open file to read data from */
        FILE* fp;
        char* line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(argv[1], "r");
        if (fp == NULL)
            exit(-1);

        while ((read = getline(&line, &len, fp)) != -1) {
            printf("Retrieved line of length %zu :\n", read);
            printf("%s", line);

            /* write to the pipe */
            write(fd[WRITE_END], line, strlen(line) + 1);
        }

        fclose(fp);
        if (line)
            free(line);

        close(fd[WRITE_END]);
//        wait(NULL);
    }
    else { /* child process */
        FILE* fp;
        char* line;
        int str_len;

        fp = fopen(argv[2], "w");
        do {
            /* read from the pipe */
            read(fd[READ_END], line, BUFFER_SIZE);
            str_len = strlen(line);
            if (!str_len) {
                printf("ERROR: Read line that consisted of 0 chars length.\n");
                exit(-1);
            }
            fputs(line, fp);
        } while(line[str_len - 1] != EOF);
        fclose(fp);

        /* close the wrie end of the pipe */
        close(fd[READ_END]);
    }

    return 0;
}
