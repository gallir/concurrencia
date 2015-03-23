#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS      4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
    int tid;
};

unsigned choosing[NUM_THREADS];
unsigned number[NUM_THREADS];

int counter = 0;

unsigned max(unsigned *v, int size) {
    unsigned max = 0, i = 0;
    for (i=0; i<size; i++) {
        if (v[i] > max) {
            max = v[i];
        }
    }
    return max;
}

void lock(int i) {
    int j;

    choosing[i] = 1;
    __atomic_thread_fence(__ATOMIC_RELEASE);
    number[i] = max(number, NUM_THREADS) + 1;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    choosing[i] = 0;

    for (j=0; j<NUM_THREADS; j++) {
        while(choosing[j]);
        while(number[j] > 0 &&
            (number[j] < number[i] ||
            (number[j] == number[i] && j < i)));
    }

}

void unlock(int i) {
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    number[i] = 0;
}

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        lock(tid);
        counter += 1;
        unlock(tid);
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
