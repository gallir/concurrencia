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

struct ticket_rw {
    uint8_t number;
    union {
        uint16_t both;
        struct {
            uint8_t write;
            uint8_t read;
        };
    };
};

struct ticket_rw rw_lock;

int counter = 0;

void writer_lock() {
    uint32_t number = __atomic_fetch_add(&rw_lock.number, 1, __ATOMIC_RELAXED);

    while (number != rw_lock.write);
}

void writer_unlock() {
    struct ticket_rw tmp;
    tmp.write = rw_lock.write + 1;
    tmp.read = rw_lock.read + 1;
    rw_lock.both = tmp.both; // It's a 16 bits copy, atomic
    __atomic_thread_fence (__ATOMIC_RELEASE);
}

void reader_lock() {
    uint32_t number = __atomic_fetch_add(&rw_lock.number, 1, __ATOMIC_RELAXED);

    while (number != rw_lock.read);
    rw_lock.read++;
}

void reader_unlock() {
    __atomic_add_fetch(&rw_lock.write, 1, __ATOMIC_RELAXED);
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

    printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT/10);
    return 0;
}
