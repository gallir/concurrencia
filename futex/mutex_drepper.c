#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

/* Drepper mutex implementation
 * based on http://www.akkadia.org/drepper/futex.pdf
 * It doesn't overflow, values:
 * 0: unlocked
 * 1: locked, no waiters
 * 2: locked, one or more waiters
 */
void lock(int *futex) {
    int c = 0;

    __atomic_compare_exchange_n(futex, &c, 1, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

    if (c == 0) return; // No contention

    if (c != 2) { // Assign 2 if it was 1 before
        c = __atomic_exchange_n(futex, 2, __ATOMIC_SEQ_CST);
    }

    while (c != 0) { // Wait until is unlocked
        syscall(__NR_futex, futex, FUTEX_WAIT, 2, NULL, 0, 0);
        c = __atomic_exchange_n(futex, 2, __ATOMIC_SEQ_CST);
    }
}

void unlock(int *futex) {
    if (__atomic_fetch_sub(futex, 1, __ATOMIC_SEQ_CST) != 1) {
        // There are waiters, wake one
        *futex = 0;
        syscall(__NR_futex, futex, FUTEX_WAKE, 1, NULL, 0, 0);
    }
}

// It's used as futex
int mutex = 0;

/* END FUTEX */

int counter = 0;

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        lock(&mutex);
        counter += 1;
        unlock(&mutex);
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

    printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT);
    return 0;
}
