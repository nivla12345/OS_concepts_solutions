/**
 * Example program demonstrating UNIX pipes.
 *
 * Figures 3.25 & 3.26
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 64
#define READ_END    0
#define WRITE_END   1

int main(int argc, char* argv[])
{
    pid_t pid;
    int fd[2];

    if (argc != 2) {
        printf("ERROR: Incorrect number of parameters.\n");
        return 0;
    }

    if (strlen(argv[1]) > 63) {
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
        char parent_write_msg[BUFFER_SIZE] = "";
        char parent_read_msg[BUFFER_SIZE] = "";

        strcpy(parent_write_msg, argv[1]);

        /* write to the pipe */
        write(fd[WRITE_END], parent_write_msg, strlen(parent_write_msg)+1);

        close(fd[WRITE_END]);

        wait(NULL);
        read(fd[READ_END], parent_read_msg, BUFFER_SIZE);

        printf("%s\n", parent_read_msg);
        close(fd[READ_END]);
    }
    else { /* child process */
        char child_write_msg[BUFFER_SIZE] = "";
        char child_read_msg[BUFFER_SIZE] = "";
        int  str_len, i;

        /* read from the pipe */
        read(fd[READ_END], child_read_msg, BUFFER_SIZE);
        /* close the write end of the pipe */
        close(fd[READ_END]);

        str_len = strlen(child_read_msg);
        for (i = 0; i < str_len; i++) {
            char c = child_read_msg[i];
            if (!isalpha(c))
                child_write_msg[i] = c;
            else {
                if (isupper(c))
                    child_write_msg[i] = tolower(c);
                else
                    child_write_msg[i] = toupper(c);
            }
        }
        child_write_msg[i] = '\0';

        write(fd[WRITE_END], child_write_msg, strlen(child_write_msg) + 1);

        /* close the unused end of the pipe */
        close(fd[WRITE_END]);
    }

    return 0;
}
