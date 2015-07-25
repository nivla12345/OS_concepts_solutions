#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <time.h>

typedef struct{
  int arr_len;
} args_struct;

double g_PI = 0;

void* monte_carlo(void* v_as) {
    args_struct* as = v_as;
    int arr_len     = (*as).arr_len;
    srand(time(NULL));
    for (int i = 0; i < arr_len; i++) {
        double random_x = rand()/(double)RAND_MAX;
        double random_y = rand()/(double)RAND_MAX;
        g_PI += random_x*random_x + random_y*random_y <= 1;
    }
    //g_PI *= 4.0/arr_len;
    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    int i;
    args_struct* args;
    void* v_args;
    pthread_t thread;
    int len = atoi(argv[1]);
    double pi = 0;

    if (argc != 2) {
        printf("ERROR: Needs exactly 1 argument.\n");
        exit(1);
    }

    (*args).arr_len = len;
    v_args = (void*)(args);

    if (pthread_create(&thread, NULL, &monte_carlo, v_args) != 0) {
        printf("ERROR: Monte Carlo thread created incorrectly.\n");
        exit(0);
    }

    pthread_join(thread, NULL);
    pi = g_PI*4.0/len;
    printf("Pi is: %f\n", pi);
    return 0;
}

