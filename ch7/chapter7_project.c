/*
This program implements the banker's algorithm for a given number of customers, modeled as threads competing for a given
number of resources. The customers will requrest a random number of resources at all times. Should the request be
unsafe, the request will be denied and the customer will come back with another randomized request.

Additionally, when a customer has obtained all of their resources, the customer will then release all resources they
have obtained.

TODO Think about a scenario where the program is in a safe state but any request sends it into an unsafe state hence all
requests get denied.
 */

#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <time.h>

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

static pthread_mutex_t resource_lock;

/* the available amount of each resource */
// This definitely requires a mutex.
int g_available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
// This value will only be written to a single time and never written to again.
int g_maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int g_allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int g_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Requests resources contained in request[] for customer_num, positive successful.
// Performs a banker's safety algorithm.
int request_resources(int customer_num, int request[]) {

    return 0;
}

// Requests resources contained in request[] for customer_num
int release_resources(int customer_num, int release[]) {
    pthread_mutex_lock(&resource_lock);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        g_available[i] += release[i];
    }
    pthread_mutex_unlock(&resource_lock);
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        g_allocation[customer_num][i] -= release[i];
        g_need      [customer_num][i] += release[i];
    }
    return 0;
}

// Return 0 if not equal, not 0 if equal
bool cmpArr(int a[], int b[]) {
    if ((a == NULL) ^ (b == NULL))
        return false;
    int arrLenA = sizeof(a)/sizeof(a[0]);
    int arrLenB = sizeof(b)/sizeof(b[0]);
    if (arrLenB != arrLenA)
        return false;
    for (int i = 0; i < arrLenA; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

void printArr(int a[]) {
    if (a == NULL)
        return;
    int lenA = sizeof(a)/sizeof(a[0]);
    for (int i = 0; i < lenA; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void* customer_thread(void* c_void_args) {
    int* as = c_void_args;
    int customer_id = *as;
    int request[NUMBER_OF_RESOURCES];
    printf("thread id: %d\n", customer_id);
    while (true) {
        // Generate request random number of resources.
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            request[i] = rand() % (g_need[customer_id][i] + 1);
        }

        // Request a random number of resources. Grant if safe.
        if (request_resources(customer_id, request)) {
            pthread_mutex_lock(&resource_lock);
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                g_available[i] -= request[i];
            }
            pthread_mutex_unlock(&resource_lock);
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                g_allocation[customer_id][i] += request[i];
                g_need      [customer_id][i] -= request[i];
            }
        }

        sleep(rand() % 2);

        // Generate release random number of resources.
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            request[i] = rand() % (g_allocation[customer_id][i] + 1);
        }

        // Release a random number of resources.
        if (release_resources(customer_id, request)) {
        }
    }
    pthread_exit(0);
}

int main(int nargs, char* argv[]) {
    int i, j;
    // The arguments, in this case simply the customer ID.
    int       args  [NUMBER_OF_CUSTOMERS];
    void*     v_args[NUMBER_OF_CUSTOMERS];
    pthread_t thread[NUMBER_OF_CUSTOMERS];

    pthread_mutex_init(&resource_lock, NULL);

    if (nargs != NUMBER_OF_RESOURCES + 1) {
        printf("ERROR: Needs exactly %d arguments.\n", NUMBER_OF_RESOURCES);
        exit(1);
    }

    // Initialize the available array.
    for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
        g_available[i] = atoi(argv[i + 1]);

        if (g_available[i] < 1) {
	    printf("ERROR: The number of resources must be at least 1.\n");
            exit(0);
        }
    }

    srand(time(NULL));

    // Generate maximum, need, and allocation. The maximum is going to be <= the net number of resources.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
            // The +1 is to make the maximum values inclusive.
            g_maximum[i][j]    = rand() % (g_available[j] + 1);
            g_need   [i][j]    = g_maximum[i][j];
            g_allocation[i][j] = 0;
        }
    }

    // Construct and spawn arguments and threads
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        args[i] = i;
        v_args[i] = (void*)(args + i);
        if (pthread_create(&thread[i], NULL, &customer_thread, v_args[i]) != 0) {
            printf("ERROR: Monte Carlo thread created incorrectly.\n");
            exit(0);
        }
        printf("End of loop iteration.\n");
    }


    printf("About to join.\n");
    // Close and dispose of everything. We won't ever reach this because our threads go on forever.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        pthread_join(thread[i], NULL);
    printf("Completed join.\n");
    return 0;
}