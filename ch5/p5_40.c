#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

int main(int argc, char* argv[])
{
    int i;
    double random_x = 0;
    double random_y = 0;
    double pi = 0;

    if (argc != 2) {
        printf("ERROR: Needs exactly 1 argument.\n");
        exit(1);
    }
    int len = atoi(argv[1]);

    srand(time(NULL));
    #pragma omp parallel for
    for (int i = 0; i < len; i++) {
        double random_x = rand()/(double)RAND_MAX;
        double random_y = rand()/(double)RAND_MAX;
        int add = random_x*random_x + random_y*random_y <= 1;
        #pragma omp critical
        {
           pi += add;
        }
    }

    pi *= 4.0/len;
    printf("Pi is: %f\n", pi);
    return 0;
}
