/*
 * This file simulates the dining philosophers problem.
 *
 * Prompt:
 * - 5 philosophers and 5 chopsticks.
 * - Philosopher can only use the chopstick to her left or right.
 * - Solution must work, no dead/livelock and all philosophers must eventually
 *   eat.
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
#include <time.h>
#include <errno.h>

static pthread_mutex_t barrier_lock;
static pthread_cond_t barrier_cond;
static int barrier_countdown = NUM_PHILOSOPHERS;

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

void* philosopher_task() {
    sleep(1);
    barrier_point();
    pthread_exit(0);
}

int main()
{
    int i;
    pthread_t threads[NUM_PHILOSOPHERS];

    // Set up barrier lock + condition var.
    pthread_mutex_init(&barrier_lock, NULL);
    pthread_cond_init(&barrier_cond, NULL);

    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (pthread_create(&threads[i], NULL, philosopher_task, NULL) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }
    return 0;
}

