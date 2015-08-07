#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_RESOURCES 5

void* thread_task(void* param);

static pthread_mutex_t mutex;
int available_resources = MAX_RESOURCES;

/* decrease available_resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int count) {
    if (available_resources < count)
        return -1;
    else {
        available_resources -= count;
        return 0;
    }
}

/* increase available_resources by count */
int increase_count(int count) {
    available_resources += count;
    return 0;
}

void* thread_task(void* param) {
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main()
{
    int i;
    int num_threads = 5;
    pthread_t threads[num_threads];
    allocate_map();
    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_task, NULL) != 0) {
            printf("ERROR: Pthread created incorrectly.\n");
            exit(0);
        }
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    print_map();
    free(g_pid_table);
    return 0;
}

