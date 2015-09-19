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
int g_available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
// This value will only be written to a single time and never written to again.
int g_maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int g_allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int g_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void printArr(int a[], int len) {
    if (a == NULL)
        return;
    for (int i = 0; i < len; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

// Returns if every element in a >= b.
int cmpArr(int a[], int b[], int len) {
    if ((a == NULL) ^ (b == NULL))
        return false;
    for (int i = 0; i < len; i++) {
        if (a[i] < b[i])
            return false;
    }
    return true;
}

// Requests resources contained in request[] for customer_num
// Given I start in a safe state and requests will only be granted
// if they are safe, I will always release whatever values are
// requested.
int release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        g_available[i] += release[i];
        g_allocation[customer_num][i] -= release[i];
        g_need[customer_num][i] += release[i];
    }
    return 1;
}

// Requests resources contained in request[] for customer_num, positive successful.
// Performs a banker's safety algorithm.
int request_resources(int customer_num, int request[]) {
    int i, j;
    int  work  [NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS];

    // Determine if request can be safely granted.
    // Normally test to see if request exceeds Need(i) but the way the requests
    // are generated, this will never happen.

    // Test if resources are available
    if (!cmpArr(g_available, request, NUMBER_OF_RESOURCES))
        return 0;

    // Temporarily grant request given resources are available
    for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
        g_available[i] -= request[i];
        g_allocation[customer_num][i] += request[i];
        g_need[customer_num][i] -= request[i];
    }

    // User Banker's Algorithm to check if in safe state, if not rewind request.
    // Initialize variables.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = false;
    }
    for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = g_available[i];
    }

    // Find an index i, s.t. Finish[i] == false && Need[i] <= Work
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (cmpArr(work, g_need[i], NUMBER_OF_RESOURCES) && !finish[i]) {
            for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
                work[j] += g_allocation[i][j];
            }
            finish[i] = true;
        }
    }

    // Check if all elements are finished.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) {
            release_resources(customer_num, request);
            return 0;
        }
    }
    return 1;
}

void* customer_thread(void* c_void_args) {
    int result;
    int* as = c_void_args;
    int customer_id = *as;
    int request[NUMBER_OF_RESOURCES];
    while (true) {
        // Generate request random number of resources.
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            request[i] = rand() % (g_need[customer_id][i] + 1);
        }

        // Request a random number of resources. Grant if safe.
        pthread_mutex_lock(&resource_lock);
        printf("\ncustomer: %d requesting:\n", customer_id);
        printArr(request, NUMBER_OF_RESOURCES);
        printf("Before request available:\n");
        printArr(g_available, NUMBER_OF_RESOURCES);
        result = request_resources(customer_id, request);
        if (result) {
            printf("request granted\n");
            printf("After request available:\n");
            printArr(g_available, NUMBER_OF_RESOURCES);
        }
        else {
            printf("request rejected\n");
        }
        pthread_mutex_unlock(&resource_lock);

        sleep((rand() % 2) + 1);

        // Generate release random number of resources.
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            request[i] = rand() % (g_allocation[customer_id][i] + 1);
        }

        // Release a random number of resources.
        pthread_mutex_lock(&resource_lock);
        printf("\ncustomer: %d releasing:\n", customer_id);
        printArr(request, NUMBER_OF_RESOURCES);
        release_resources(customer_id, request);
        printf("Available:\n");
        printArr(g_available, NUMBER_OF_RESOURCES);
        pthread_mutex_unlock(&resource_lock);
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

    printf("Maximum array:\n");
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printArr(g_maximum[i], NUMBER_OF_RESOURCES);
    }

    // Construct and spawn arguments and threads
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        args[i] = i;
        v_args[i] = (void*)(args + i);
        if (pthread_create(&thread[i], NULL, &customer_thread, v_args[i]) != 0) {
            printf("ERROR: Customer thread created incorrectly.\n");
            exit(0);
        }
    }

    // Close and dispose of everything. We won't ever reach this because our threads go on forever.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        pthread_join(thread[i], NULL);
    return 0;
}
