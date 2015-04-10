#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <limits.h>

#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

int counter = 0;


struct simple_futex {
    unsigned number;
    unsigned turn;
};

struct simple_futex mutex;


/* Simple fair FUTEX mutex implementation */
void lock(struct simple_futex *futex) {
    unsigned number = __atomic_fetch_add(&futex->number, 1, __ATOMIC_SEQ_CST);
    unsigned turn = futex->turn;

    while (number != turn) {
        syscall(__NR_futex, &futex->turn, FUTEX_WAIT, turn, NULL, 0, 0);
        turn = futex->turn;
    }
}

void unlock(struct simple_futex *futex) {
    __atomic_fetch_add(&futex->turn, 1, __ATOMIC_RELEASE);
    if (futex->number >= futex->turn) {
        syscall(__NR_futex, &futex->turn, FUTEX_WAKE, INT_MAX, NULL, 0, 0);
    }
}
/* END FUTEX */

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        lock(&mutex);
        counter += 1;
        unlock(&mutex);
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
