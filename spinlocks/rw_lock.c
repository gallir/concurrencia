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

uint32_t rw_lock = 0; // We use 32 bits, up 2^21 lectores

int counter = 0;

void reader_lock() {
    uint32_t old, new;
    while (1) {
        while(rw_lock & 0x80000000);

        old = rw_lock & 0x7fffffff;
        new = old + 1;

        if (__atomic_compare_exchange_n(&rw_lock, &old, new, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            return;
        }
    }
}

void writer_lock() {
    uint32_t old, new;
    while (1) {
        while(rw_lock & 0x80000000);

        old = rw_lock & 0x7fffffff;
        new = old | 0x80000000;

        if (__atomic_compare_exchange_n(&rw_lock, &old, new, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
            // wait for readers to finish
            while (rw_lock & 0x7fffffff);
            return;
        }
    }
}

void reader_unlock() {
    __atomic_fetch_add(&rw_lock, -1, __ATOMIC_RELEASE);
}

void writer_unlock() {
    __atomic_store_n(&rw_lock, 0, __ATOMIC_RELEASE);
}


void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;
    int c;

    for (i=0; i < max; i++) {
        if (i % 10) {
            // 90% of probability of being a reader
            reader_lock();
            c = counter; // Just copy the value
            reader_unlock();
        } else {
            writer_lock();
            counter += 1;
            writer_unlock();
        }
    }
    printf("End %d counter: %d\n", tid, counter);
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

    printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT/10);
    return 0;
}
