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

static pthread_mutex_t barrier_lock;
static pthread_cond_var barrier_cond;
static int barrier_countdown = NUM_PHILOSOPHERS;

/* decrease barrier_countdown by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int barrier_point() {
    pthread_mutex_lock(&barrier_lock);
    barrier_countdown--;
    printf("%d\n", pthread_self());
    if (barrier_countdown) {
        pthread_cond_wait(&barrier_lock, &barrier_cond);
    }
    printf("%d\n", pthread_self());
    pthread_cond_signal(&barrier_cond);
    pthread_mutex_unlock(&barrier_lock);
    return 0;
}

void* thread_task(void* param) {
    sleep(1);
    barrier_point();
    pthread_exit(0);
}

int main()
{
    int i;
    pthread_t threads[NUM_PHILOSOPHERS];

    pthread_mutex_init(&barrier_lock, NULL);
    pthread_cond_init(&barrier_cond);

    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        if (pthread_create(&threads[i], NULL, thread_task, NULL) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }

    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("last barrier_countdown: %d\n", barrier_countdown);
    return 0;
}

