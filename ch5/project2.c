/*
 * Author: Alvin Mao
 * Date: 8/15/15
 *
 * This file simulates the dining philosophers problem.
 *
 * Prompt:
 * - There are 5 philosophers and 5 chopsticks.
 * - A philosopher can only use the chopstick to her left or right.
 * - Solution must have no dead/livelock and all philosophers mustn't starve.
 * - It is assumed that this is all the philosophers do for all of eternity.
 *
 * Solution:
 * Chandy/Misra Implementation
 *
 * Implementation Overview:
 *   Data Structures -
 *   - Barrier s.t. the initial of handing out forks ensures an acyclic graph.
 *   - Chopstick array of size = NUM_PHILOSOPHERS
 *   - Dirty/clean array that maps to the chopstick array.
 *   - Philosophers represented as an array of threads.
 *   - Philosophers' mail boxes which are represented as a pair of boolean
 *     arrays each of size = NUM_PHILOSOPHERS
 *   - TODO An array of 5 condition variables, 1 for each thread that enables
 *     the thread to sleep when waiting for the chopsticks.
 */

// Must be >= 3
#define NUM_PHILOSOPHERS 5
#define LAST_INDEX      (NUM_PHILOSOPHERS - 1)

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

// Barrier variables
static pthread_mutex_t barrier_lock;
static pthread_cond_t  barrier_cond;
static int             barrier_countdown = NUM_PHILOSOPHERS;
// Chopstick variables
// For reference, the index is assumed to be the left chopstick.
static pthread_mutex_t chopstick_locks[NUM_PHILOSOPHERS];
static bool            chopstick_clean[NUM_PHILOSOPHERS];
// Mailbox variables
static bool            right_mail_box[NUM_PHILOSOPHERS];
static bool            left_mail_box [NUM_PHILOSOPHERS];
static pthread_mutex_t lock_right_mb [NUM_PHILOSOPHERS];
static pthread_mutex_t lock_left_mb  [NUM_PHILOSOPHERS];

void write_mail_box(bool left, int index, bool value) {
    if (left) {
        pthread_mutex_lock(&lock_left_mb[index]);
        left_mail_box[index] = value;
        pthread_mutex_unlock(&lock_left_mb[index]);
    }
    else {
        pthread_mutex_lock(&lock_right_mb[index]);
        right_mail_box[index] = value;
        pthread_mutex_unlock(&lock_right_mb[index]);
    }
}

bool read_mail_box(bool left, int index) {
    bool value;
    if (left) {
        pthread_mutex_lock(&lock_left_mb[index]);
        value = left_mail_box[index];
        pthread_mutex_unlock(&lock_left_mb[index]);
    }
    else {
        pthread_mutex_lock(&lock_right_mb[index]);
        value = right_mail_box[index];
        pthread_mutex_unlock(&lock_right_mb[index]);
    }
    return value;
}

// *mb = right/left mail box
// These are the only handles necessary to manage mail boxes.
bool read_my_lmb(int index) {
    return read_mail_box(true, index);
}
bool read_my_rmb(int index) {
    return read_mail_box(false, index);
}
bool read_lmb(int index) {
    int left_index  = (index + 1) % NUM_PHILOSOPHERS;
    return read_mail_box(false, left_index);
}
bool read_rmb(int index) {
    int right_index = (index + LAST_INDEX) % NUM_PHILOSOPHERS;
    return read_mail_box(true, right_index);
}
void clear_my_lmb(int index) {
    write_mail_box(true, index, false);
}
void clear_my_rmb(int index) {
    write_mail_box(false, index, false);
}
void write_lmb(int index) {
    int left_index  = (index + 1) % NUM_PHILOSOPHERS;
    write_mail_box(false, left_index, true);
}
void write_rmb(int index) {
    int right_index = (index + LAST_INDEX) % NUM_PHILOSOPHERS;
    write_mail_box(true, right_index, true);
}

// Fork handlers
// Marks both forks dirty
void dirty_forks(int index) {
    chopstick_clean[index] = false;
    chopstick_clean[(index + 1) % NUM_PHILOSOPHERS];
}
void clean_left_fork(int index) {
    chopstick_clean[index] = true;
}
void clean_right_fork(int index) {
    chopstick_clean[(index + 1) % NUM_PHILOSOPHERS] = true;
}

// Threads wait here until all threads have reached this point.
int barrier_point() {
    pthread_mutex_lock(&barrier_lock);
    barrier_countdown--;
    // The last thread to reach this point brings barrier_countdown down to 0
    // and will proceed to signal the next thread which will signal the next
    // one etc...
    if (barrier_countdown)
        pthread_cond_wait(&barrier_cond, &barrier_lock);
    pthread_cond_signal(&barrier_cond);
    pthread_mutex_unlock(&barrier_lock);
    return 0;
}

// Sleep from [1:3] seconds
void sleep_random_123() {
    double raw_rand_sleep;
    int rand_sleep;
    raw_rand_sleep = rand()/(double)RAND_MAX;
    rand_sleep = 1 + ((int)(raw_rand_sleep * 3));
    sleep(rand_sleep);
}

void* philosopher_task(void* p_args) {
    int index = *((int*)p_args);
    int r_chopstick_index = (index + 1) % NUM_PHILOSOPHERS;
    bool has_left  = false;
    bool has_right = false;

    // Get assigned a chopstick
    //   Philosopher 0 gets 2 chopsticks
    if (index == 0) {
        pthread_mutex_unlock(&chopstick_locks[index]);
        has_left  = true;
        pthread_mutex_unlock(&chopstick_locks[LAST_INDEX]);
        has_right = true;
    }
    //   Philosophers [1:3] get 1 chopstick
    else if (index < LAST_INDEX) {
        pthread_mutex_unlock(&chopstick_locks[index]);
        has_left = true;
    }
    // Wait for every thread to get assigned a chopstick
    barrier_point();

    // Loop: think, eat, or wait.
    while(1) {
        // 0) Check if can eat. Message other philosophers and wait if
        //    necessary.
        // TODO After finishing, change this implementation to using
        //      conditional variables.
        if (!has_right) {
            write_rmb(index);
            while (read_rmb(index)) {}
            pthread_mutex_unlock(&chopstick_locks[r_chopstick_index]);
            has_right = true;
        }
        if (!has_left) {
            write_lmb(index);
            while (read_lmb(index)) {}
            pthread_mutex_unlock(&chopstick_locks[index]);
            has_left = true;
        }
        // TODO Mark chopsticks as dirty
        // 1) Eat
        sleep_random_123();
        dirty_forks(index);
        printf("p%d: ate\n", index);

        // 2) Check mail box, checks fork and clean and distribute if there's
        //    mail and if fork is dirty otherwise do nothing.
        if (read_my_rmb(index)) {
            clean_right_fork(index);
            pthread_mutex_unlock(&chopstick_locks[r_chopstick_index]);
            clear_my_rmb(index);
            has_right = false;
        }
        if (read_my_lmb(index)) {
            clean_left_fork(index);
            pthread_mutex_unlock(&chopstick_locks[index]);
            clear_my_lmb(index);
            has_left = false;
        }
    }
    pthread_exit(0);
}

int main() {
    int i;
    void*     p_args   [NUM_PHILOSOPHERS];
    int       p_indexes[NUM_PHILOSOPHERS];
    pthread_t threads  [NUM_PHILOSOPHERS];

    // Set up barrier variables.
    pthread_mutex_init(&barrier_lock, NULL);
    pthread_cond_init(&barrier_cond, NULL);

    // Set up chopsticks
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopstick_locks[i], NULL);
        chopstick_clean[i] = false;
    }

    // Set up mailboxes
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&lock_right_mb[i], NULL);
        pthread_mutex_init(&lock_left_mb [i], NULL);
        right_mail_box[i] = false;
        left_mail_box [i] = false;
    }

    srand(time(NULL));

    // Create philosophers
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
