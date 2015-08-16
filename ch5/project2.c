/*
 * This file simulates the dining philosophers problem.
 *
 * Prompt:
 * - 5 philosophers and 5 chopsticks.
 * - Philosopher can only use the chopstick to her left or right.
 * - Solution must work, no dead/livelock and all philosophers must eventually
 *   eat.
 * - It is assumed that this is all the philosophers do for all of eternity.
 *
 * Solution:
 * Chandy/Misra Implementation
 *
 * Implementation Overview:
 *   Locks + Shared Objects -
 *   - Barrier s.t. the initialization of handing out forks ensures an acyclic
 *     graph.
 *   - 
 */

#define NUM_PHILOSOPHERS 5
#define NUM_FORKS        5

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

static pthread_mutex_t barrier_lock;
static pthread_cond_t  barrier_cond;
static int             barrier_countdown = NUM_PHILOSOPHERS;

// Threads wait here until all threads have reached this point.
int barrier_point() {
    pthread_mutex_lock(&barrier_lock);
    barrier_countdown--;
    if (barrier_countdown)
        pthread_cond_wait(&barrier_cond, &barrier_lock);
    pthread_cond_signal(&barrier_cond);
    pthread_mutex_unlock(&barrier_lock);
    return 0;
}

void* philosopher_task(void* p_args) {
    int index = *((int*)p_args);
    printf("%d\n", index);
    bool has_left  = false;
    bool has_right = false;
    // Get assigned a chopstick
    barrier_point();
    // Loop: think, eat, or wait.
    while(1) {
        // 0) Check if can eat. Message other philosiphers and wait if can't.
//        if (!has_left || !has_right)
        break;
        // 1) Obtain chopstick lock and think
        // 2) Check mail box, clean and distribute if there's mail otherwise
        //    do nothing.
    }
    pthread_exit(0);
}

void sleep_random_123() {
    double raw_rand_sleep;
    int rand_sleep;
    raw_rand_sleep = rand()/(double)RAND_MAX;
    rand_sleep = 1 + ((int)(rand_sleep * 3));
    sleep(rand_sleep);
}

int main()
{
    int i;
    void*     p_args   [NUM_PHILOSOPHERS];
    int       p_indexes[NUM_PHILOSOPHERS];
    pthread_t threads  [NUM_PHILOSOPHERS];

    // Set up barrier lock + condition var.
    pthread_mutex_init(&barrier_lock, NULL);
    pthread_cond_init(&barrier_cond, NULL);

    srand(time(NULL));
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        p_indexes[i] = i;
        p_args   [i] = (void*)(p_indexes + i);
        if (pthread_create(&threads[i], NULL, philosopher_task, p_args[i]) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }

    // The point in joining despite looping forever is to keep the p_args,
    // p_indexes in scope.
    for (i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_join(threads[i], NULL);
    return 0;
}

