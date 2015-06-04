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

#define CPUID_RTM (1 << 11)
#define CPUID_HLE (1 << 4)
static inline int cpu_has_rtm(void) {
    if (__get_cpuid_max(0, NULL) >= 7) {
        unsigned a, b, c, d;
        __cpuid_count(7, 0, a, b, c, d);
        return !!(b & CPUID_RTM);
    }
    return 0;
}

int mutex = 0;
inline void lock() {
    while(__atomic_exchange_n(&mutex, 1, __ATOMIC_SEQ_CST)) {
        PAUSE;
    }
}

inline void unlock() {
    __atomic_store_n(&mutex, 0, __ATOMIC_RELEASE);
}

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;


    for (i=0; i < max; i++) {
        if (_xbegin() == _XBEGIN_STARTED) {
            if (mutex) {
                _xabort(1); /* Lock busy */
            }
            counter[i % ARRAY_SIZE]++;
            _xend();
        } else {
            lock();
            counter[i % ARRAY_SIZE]++;
            unlock();
        }
    }
    printf("End %d counter: %d\n", tid, SUM(counter));
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    if (! cpu_has_rtm) {
        printf("CPU doesn't have RTM extensions\n");
        exit(1);
    }

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
