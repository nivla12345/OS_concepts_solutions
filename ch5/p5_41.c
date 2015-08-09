#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>

void* thread_task(void* param);

static sem_t cvar, lock, next;
int waiters;
int next_count;
static int num_threads;

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

/* decrease num_threads by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int barrier_point() {
    sem_wait(&lock);
    num_threads--;
    printf("%d\n", pthread_self());
    if (num_threads) {
        condition_wait();
    }
    printf("%d\n", pthread_self());
    condition_signal();
    if (next_count > 0)
        sem_post(&next);
    else
        sem_post(&lock);
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
    num_threads = 5;

    // Initialize all monitor semaphores + counters
    sem_init(&cvar, 0, 0);
    sem_init(&lock, 0, 1); // lock should be free initially
    sem_init(&next, 0, 0);

    next_count = 0;
    waiters = 0;

    // Generate random threads.
    pthread_t threads[num_threads];

    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_task, NULL) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("last num_threads: %d\n", num_threads);
    return 0;
}

