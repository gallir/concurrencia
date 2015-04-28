#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

/* Implementation of Lamport's Fast Mutex Algorithm
 * Blame him for the use of GOTOs and labels ;)
 */
unsigned int b[NUM_THREADS];
int y = -1, x = -1;

void lock(int i) {
    int j;

start:
    b[i] = 1;
    x = i;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    if (y >= 0) {
        b[i] = 0;
        while (y >= 0);
        goto start;
    }
    y = i;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    if (x != i) {
        b[i] = 0;
        for (j = 0; j < NUM_THREADS; j++) {
            while (b[j]);
        }
        if (y != i) {
            while (y >= 0);
            goto start;
        }
    }
}

void unlock(int i) {
    //__atomic_thread_fence(__ATOMIC_SEQ_CST);
    y = -1;
    b[i] = 0;
}

int counter = 0;

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        lock(tid);
        counter += 1;
        unlock(tid);
    }

    printf("End %d counter: %d\n", tid, counter);
    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, count, (void *) &id[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT);
    return 0;
}
