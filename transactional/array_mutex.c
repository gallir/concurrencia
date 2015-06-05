#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "defs.h"

// Just used to send the index of the id
struct tdata {
    int tid;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int padding[64]; /* To avoid false sharing with counter */

int counter[ARRAY_SIZE];


void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        pthread_mutex_lock(&mutex);
        counter[i % ARRAY_SIZE]++;
        pthread_mutex_unlock(&mutex);
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
