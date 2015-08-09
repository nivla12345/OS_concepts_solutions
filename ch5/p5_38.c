#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#define MAX_RESOURCES 5

void* thread_task(void* param);

static sem_t cvar, lock, next;
int waiters;
int next_count;
static int available_resources = MAX_RESOURCES;

void condition_signal();
void condition_wait();

/* decrease available_resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int count) {
    sem_wait(&lock);
    while (available_resources < count) {
        condition_wait();
    }
    printf("d curr:%d -%d\n", available_resources, count);
    available_resources -= count;
    if (next_count > 0)
        sem_post(&next);
    else
        sem_post(&lock);
    return 0;
}

/* increase available_resources by count */
int increase_count(int count) {
    sem_wait(&lock);
    printf("i curr:%d +%d\n", available_resources, count);
    available_resources += count;
    condition_signal();
    if (next_count > 0)
        sem_post(&next);
    else
        sem_post(&lock);
    return 0;
}

void* thread_task(void* param) {
    int random = (int)(5*(rand()/(double)RAND_MAX) + 1);
    decrease_count(random);
    increase_count(random);
    pthread_exit(0);
}

// Wait conditional variable call
void condition_wait() {
    waiters++;
    if (next_count > 0)
        sem_post(&next);
    else
        sem_post(&lock);
    sem_wait(&cvar);
    waiters--;
}

// Signal conditional variable call
void condition_signal() {
    if (waiters > 0) {
        next_count++;
        sem_post(&cvar);
        sem_wait(&next);
        next_count--;
    }
}

int main()
{
    int i;
    int num_threads = 10;

    // Initialize all monitor semaphores + counters
    sem_init(&cvar, 0, 0);
    sem_init(&lock, 0, 1); // lock should be free initially
    sem_init(&next, 0, 0);

    next_count = 0;
    waiters = 0;

    // Generate random threads.
    pthread_t threads[num_threads];

    // Must seed time only once.
    srand(time(NULL));

    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_task, NULL) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("last available_resources: %d\n", available_resources);
    return 0;
}

