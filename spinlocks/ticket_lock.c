#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>


#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

struct tickets {
    uint16_t turn;
    uint16_t number;
};

struct tickets ticket_lock;

int counter = 0;

void lock() {
    uint16_t number;

    number =  __atomic_fetch_add(&ticket_lock.number, 1, __ATOMIC_RELAXED);
    while (number != ticket_lock.turn);
}

void unlock() {
    __atomic_fetch_add(&ticket_lock.turn, 1, __ATOMIC_RELEASE);
}

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        lock();
        counter += 1; // The global variable, i.e. the critical section
        unlock();
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
