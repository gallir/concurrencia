#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <errno.h>

#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};


/* Simplest FUTEX semaphore implementation */
typedef struct futex_sem {
    int futex; // Never is modified so FUTEX_WAIT doesn't fail
    int value;
} futex_sem_t;

void wait(futex_sem_t *sem) {
    int value = __atomic_fetch_sub(&sem->value, 1, __ATOMIC_RELAXED);
    if (value <= 0) {
        syscall(__NR_futex, &sem->futex, FUTEX_WAIT, sem->futex, NULL, 0, 0);
    }
}

void signal(futex_sem_t *sem) {
    int value = __atomic_add_fetch(&sem->value, 1, __ATOMIC_RELAXED);
    if (value <= 0) {
        while (syscall(__NR_futex, &sem->futex, FUTEX_WAKE, 1, NULL, 0, 0) < 1) {
            sched_yield();
        }
    }
}
/* END FUTEX */

int counter = 0;
futex_sem_t mutex = {.value = 1}; // Initialized to 1

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        wait(&mutex);
        counter += 1;
        signal(&mutex);
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
