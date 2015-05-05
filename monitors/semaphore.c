#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS      4
#define MAX_COUNT 10000000

struct tdata {
    int tid;
};

/* Simulation of semaphores with "monitors" */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notZero = PTHREAD_COND_INITIALIZER;
int sem_value = 1;

void p() {
    pthread_mutex_lock(&mutex);
    while (sem_value == 0) {
        pthread_cond_wait(&notZero, &mutex);
    }
    sem_value--;
    pthread_mutex_unlock(&mutex);
}

void v() {
    pthread_mutex_lock(&mutex);
    sem_value++;
    pthread_cond_signal(&notZero);
    pthread_mutex_unlock(&mutex);
}
/* END semaphore simulation */

int counter = 0;

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        p();
        counter += 1;
        v();
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
