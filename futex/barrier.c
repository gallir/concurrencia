#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <limits.h>

#define NUM_THREADS     10
#define PHASES          20

// Just used to send the index of the id
struct tdata {
    int tid;
};



/* BARRIER by Drepper http://www.akkadia.org/drepper/futex.pdf */

typedef struct barrier {
    int lock;
    unsigned phase;
    unsigned processes;
    unsigned to_arrive;
} barrier_t;

/* We use Drepper mutex */
void _barrier_lock(int *futex) {
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

void _barrier_unlock(int *futex) {
    if (__atomic_fetch_sub(futex, 1, __ATOMIC_SEQ_CST) != 1) {
        // There are waiters, wake one
        *futex = 0;
        syscall(__NR_futex, futex, FUTEX_WAKE, 1, NULL, 0, 0);
    }
}

void barrier_wait(barrier_t *barrier) {
    unsigned phase;

    _barrier_lock(&barrier->lock);
    barrier->to_arrive--;
    if (barrier->to_arrive > 0) {
        phase = barrier->phase;
        _barrier_unlock(&barrier->lock);
        do {
            syscall(__NR_futex, &barrier->phase, FUTEX_WAIT, phase, NULL, 0, 0);
        } while (barrier->phase == phase);
    } else {
        barrier->phase++;
        barrier->to_arrive = barrier->processes;
        syscall(__NR_futex, &barrier->phase, FUTEX_WAKE, INT_MAX, NULL, 0, 0);
        _barrier_unlock(&barrier->lock);
    }
}

/* END BARRIER */

// Initialized to NUM_THREADS, this can be done with a macro or initialization function
barrier_t barrier = {.processes = NUM_THREADS, .to_arrive = NUM_THREADS};

void *run(void *ptr) {
    int tid = ((struct tdata *) ptr)->tid;
    int i;

    for (i=0; i < PHASES; i++) {
        usleep(random() % 1000000);
        barrier_wait(&barrier);
        printf("%d finished phase %d\n", tid, i);
    }

    barrier_wait(&barrier);
    printf("Finished thread %d\n", tid);
    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, run, (void *) &id[i]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}
