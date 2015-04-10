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

/* Simple FUTEX semaphore with spinlocks */
typedef struct futex_sem {
    unsigned char spinlock;
    int futex;
    unsigned waiters;
    unsigned value;
} futex_sem_t;

void spin_lock(futex_sem_t *sem) {
    unsigned char local = 0;
    while (! __atomic_compare_exchange_n(&sem->spinlock, &local, 1, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        local = 0;
        sched_yield();
    }
}

void spin_unlock(futex_sem_t *sem) {
    __atomic_store_n(&sem->spinlock, 0, __ATOMIC_RELEASE);
}


void wait(futex_sem_t *sem) {
    spin_lock(sem);
    if (sem->waiters > 0 || sem->value <= 0) {
        sem->waiters++;
        spin_unlock(sem);
        syscall(__NR_futex, &sem->futex, FUTEX_WAIT, sem->futex, NULL, 0, 0);
    } else {
        sem->value--;
        spin_unlock(sem);
    }
}

void signal(futex_sem_t *sem) {
    spin_lock(sem);
    if (sem->waiters > 0) {
        while (syscall(__NR_futex, &sem->futex, FUTEX_WAKE, 1, NULL, 0, 0) < 1) {
            sched_yield();
        }
        sem->waiters--;
    } else {
        sem->value++;
    }
    spin_unlock(sem);
}
/* END FUTEX */

futex_sem_t mutex = {.value = 1};

int counter = 0;

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
