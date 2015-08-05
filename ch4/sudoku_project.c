#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

#define SUDOKU_BOARD_DIM  9
#define SUDOKU_BOARD_SQRT 3
#define NUM_THREADS (3 * SUDOKU_BOARD_DIM)

typedef struct{
    unsigned x;
    unsigned y;
    unsigned status_index;
} struct_parameters;

const static unsigned g_SUDOKU_BOARD[SUDOKU_BOARD_DIM][SUDOKU_BOARD_DIM] =
{
    {1,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

// The g_SUDOKU_STATUS acts as a bit map as follows:
// g_SUDOKU_STATUS[8 :0 ] : columns 8:0
// g_SUDOKU_STATUS[17:9 ] : rows    8:0
// g_SUDOKU_STATUS[26:18] : boxes   8:0
static unsigned g_SUDOKU_STATUS = 0;
const static unsigned g_COL_START   = 0;
const static unsigned g_ROW_START   = 9;
const static unsigned g_BLOCK_START = 18;

static inline void modify_status(unsigned index) {
    g_SUDOKU_STATUS |= 1 << index;
}

static inline unsigned modify_tmp(unsigned index, unsigned* p_track) {
    unsigned occupied;
    unsigned track = *p_track;
    index--;
    occupied = ((track >> index) & 1);
    *p_track |= 1 << index;
    return occupied;
}

// Checks a column to see if it is a valid sudoku entry.
void* col(void* p) {
    struct_parameters* params = p;
    unsigned x     = (*params).x;
    unsigned index = (*params).status_index;
    free(p);
    unsigned track = 0;
    for (int i = 0; i < SUDOKU_BOARD_DIM; i++) {
        if (modify_tmp(g_SUDOKU_BOARD[i][x], &track)) {
            modify_status(index);
            pthread_exit(0);
        }
    }
    pthread_exit(0);
}

// Checks a row to see if it is a valid sudoku entry.
void* row(void* p) {
    struct_parameters* params = p;
    unsigned y     = (*params).y;
    unsigned index = (*params).status_index;
    free(p);
    unsigned track = 0;
    for (int i = 0; i < SUDOKU_BOARD_DIM; i++) {
        if (modify_tmp(g_SUDOKU_BOARD[y][i], &track)) {
            modify_status(index);
            pthread_exit(0);
        }
    }
    pthread_exit(0);
}

// Checks a block to see if it is a valid sudoku entry.
void* block(void* p) {
    struct_parameters* params = p;
    int x = (*params).x;
    int y = (*params).y;
    int index = (*params).status_index;
    free(p);
    unsigned track = 0;
    for (int i = x; i < SUDOKU_BOARD_SQRT; i++) {
        for (int j = y; j < SUDOKU_BOARD_SQRT; j++) {
            if (modify_tmp(g_SUDOKU_BOARD[y][i], &track)) {
                modify_status(index);
                pthread_exit(0);
            }
        }
    }
    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    unsigned i;
    pthread_t thread[NUM_THREADS];

    // Test columns
    for (i = 0; i < SUDOKU_BOARD_DIM; i++) {
        struct_parameters* args =
            (struct_parameters*)malloc(sizeof(struct_parameters));
        void* v_args;
        args -> status_index = g_COL_START + i;
        args -> x = i;
        args -> y = 0;
        v_args = (void*)(args);

        if (pthread_create(&thread[i], NULL, &col, v_args) != 0) {
            printf("ERROR: Column thread created incorrectly.\n");
            exit(0);
        }
    }

    // Test rows
    for (i = SUDOKU_BOARD_DIM; i < (SUDOKU_BOARD_DIM << 1); i++) {
        struct_parameters* args =
            (struct_parameters*)malloc(sizeof(struct_parameters));
        void* v_args;
        args -> status_index = i;
        args -> y = i - SUDOKU_BOARD_DIM;
        args -> x = 0;
        v_args = (void*)(args);

        if (pthread_create(&thread[i], NULL, &row, v_args) != 0) {
            printf("ERROR: Column thread created incorrectly.\n");
            exit(0);
        }
    }
    for (i = 0; i < (SUDOKU_BOARD_DIM << 1); i++)
        pthread_join(thread[i], NULL);

    printf("g_SUDOKU_STATUS is: %d\n", g_SUDOKU_STATUS);
    return 0;
}

