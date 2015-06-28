#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    int n;
    // Shared memory object set up.
    const int SIZE = 4096;
    const char* name = "OS";

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
        // Open chared memory object
        int shm_fd, count;
        void* ptr;
        int* ptr_start;

        // Create shared memory segment
        shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

        // Configure size of shared memory segment
        ftruncate(shm_fd, SIZE);

        count = 0;
        // Map shared memory segment in the address space of the process
        ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        ptr_start = (int*)(ptr + sizeof(n));
        while (n != 1) {
            if (n & 1)
                n = 3*n + 1;
            else
                n >>= 1;
            *ptr_start = n;
            ptr_start++;
            count++;
        }
        int* int_ptr = (int*)ptr;
        *int_ptr = count;
        exit(0);
    }
    else { /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL);
        int shm_fd;
        void* ptr;
        // Read the data from the shared memory object
        shm_fd = shm_open(name, O_RDONLY, 0666);
        ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        int* count_ptr = (int*)ptr;
        int count = *count_ptr;
        if (count) {
            count_ptr++;
            for (int i = 0; i < count; i++) {
                printf("%d\n", *count_ptr);
                count_ptr++;
            }
        }
        else {
            printf("%d\n", 0);
        }
        shm_unlink(name);
    }

    return 0;
}

