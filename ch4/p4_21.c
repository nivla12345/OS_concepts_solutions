#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

typedef struct{
  int arr_len;
  int arr[];
} args_struct;

int average = 0;
int minimum = 0;
int maximum = 0;

void* average_function(void* v_as) {
    args_struct* as = v_as;
    int* arr        = (*as).arr;
    int arr_len     = (*as).arr_len;
    for (int i = 0; i < arr_len; i++) {
        average += arr[i];
    }
    average /= arr_len;
    pthread_exit(0);
}

void* minimum_function(void* v_as) {
    args_struct* as = v_as;
    int* arr        = (*as).arr;
    int arr_len     = (*as).arr_len;
    minimum = arr[0];
    for (int i = 0; i < arr_len; i++) {
        if (arr[i] < minimum)
            minimum = arr[i];
    }
    pthread_exit(0);
}

void* maximum_function(void* v_as) {
    args_struct* as = v_as;
    int* arr        = (*as).arr;
    int arr_len     = (*as).arr_len;
    maximum = arr[0];
    for (int i = 0; i < arr_len; i++) {
        if (arr[i] > maximum)
            maximum = arr[i];
    }
    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    int i;
    int num_threads = 3;
    args_struct* args;
    void* v_args;
    pthread_t threads[num_threads];

    if (argc < 2) {
        printf("ERROR: Need at least a single parameter.\n");
        exit(1);
    }

    argc--;
    args = malloc(sizeof(args_struct) + argc * sizeof(int));
    (*args).arr_len = argc;
    for (i = 0; i < argc; i++)
        (*args).arr[i] = atoi(argv[i+1]);

    v_args = (void*)(args);

    if (pthread_create(&threads[0], NULL, &average_function, v_args) != 0) {
        printf("ERROR: Average function thread spawned incorrectly.\n");
        exit(0);
    }

    if (pthread_create(&threads[1], NULL, &minimum_function, v_args) != 0) {
        printf("ERROR: Minimum function thread spawned incorrectly.\n");
        exit(0);
    }

    if (pthread_create(&threads[2], NULL, &maximum_function, v_args) != 0) {
        printf("ERROR: Maximum function thread spawned incorrectly.\n");
        exit(0);
    }

    for (i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    printf("The average value is: %d\n", average);
    printf("The maximum value is: %d\n", maximum);
    printf("The minimum value is: %d\n", minimum);

    return 0;
}

