#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "defs.h"

// Just used to send the index of the id
struct tdata {
    int tid;
};

int counter[ARRAY_SIZE];

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;
    int j = 0;

    for (i=0; i < max; i++) {
        j = (j+1) % ARRAY_SIZE;
        __transaction_atomic {
            counter[i % ARRAY_SIZE]++;
        }
    }
    printf("End %d counter: %d\n", tid, SUM(counter));
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, count, (void *) &id[i]);
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d Expected: %d\n", SUM(counter), MAX_COUNT);
    return 0;
}
