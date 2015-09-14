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
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <time.h>

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

typedef struct{
  int customer_id;
} args_struct;

/* the available amount of each resource */
// TODO This may be the only resource that requires a mutex.
int g_available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
// This value will only be written to a single time and never written to again.
int g_maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
// This variable needs to be grouped with the g_available mutex
int g_allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
// This variable needs to be grouped with the g_available mutex
int g_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// Requests resources contained in request[] for customer_num
int request_resources(int customer_num, int request[]) {
	
}

// Requests resources contained in request[] for customer_num
int release_resources(int customer_num, int request[]) {
	
}

void* customer_thread(void* c_void_args) {
	while (true) {
		
	}
	pthread_exit(0);
}

int main(int nargs, char* argv[]) {
	int i, j;
    args_struct* args[NUMBER_OF_CUSTOMERS] = malloc(sizeof(args_struct) * NUMBER_OF_CUSTOMERS);
    pthread_t thread[NUMBER_OF_CUSTOMERS];	

	if (nargs != NUMBER_OF_RESOURCES + 1) {
        printf("ERROR: Needs exactly %d arguments.\n", NUMBER_OF_RESOURCES);
        exit(1);				
	}

	// Initialize the available array.
	for (i = 0; i < NUMBER_OF_RESOURCES; i++) {
		g_available[i] = atoi(argv[i + 1]);
		if (g_available < 1) {
			printf("ERROR: The number of resources must be at least 1.\n");
			exit(0);
		}
	}
	
	srand(time(NULL));
	// TODO Set up argument.
	
	// Generate maximum, need, and allocation. The maximum is going to be <= the net number of resources.
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		for (j = 0; j < NUMBER_OF_RESOURCES; j++) {
			// The +1 is to make the maximum values inclusive.
			g_maximum[i][j]    = rand() % (g_available[j] + 1);
			g_need   [i][j]    = g_maximum[i][j];
			g_allocation[i][j] = 0;
		}
	}

	// Construct and spawn threads
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (pthread_create(&thread[i], NULL, &customer_thread, (void*)args[i]) != 0) {
            printf("ERROR: Monte Carlo thread created incorrectly.\n");
            exit(0);
        }
    }

	// Close and dispose of everything. We won't ever reach this because our threads go on forever.
    for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        pthread_join(thread[i], NULL);
    free(args);
    return 0;
}
