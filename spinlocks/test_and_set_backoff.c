#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

unsigned char mutex = 0;

int counter = 0;

#define FAILURES_BACKOFF_LIMIT 12 // ~ 4096 in nanoseconds
void backoff(int failures) {
    struct timespec deadline = { .tv_sec = 0 };
    unsigned limit;

    if (failures > FAILURES_BACKOFF_LIMIT) {
        limit = 1 << FAILURES_BACKOFF_LIMIT;
    } else {
        limit = 1 << failures;
    }

    deadline.tv_nsec = 1 + rand() % limit;
    clock_nanosleep(CLOCK_REALTIME, 0, &deadline, NULL);
}

void lock() {
    unsigned failures = 0;
    while(mutex || __atomic_test_and_set(&mutex, __ATOMIC_RELAXED)) {
        backoff(++failures);
    }
}

void unlock() {
    mutex = 0;
    // Recomended is __atomic_clear(&mutex,...) but we already have cache coherence
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
