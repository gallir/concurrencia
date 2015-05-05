#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <limits.h>
#include <semaphore.h>

#define NUM_THREADS     10
#define PHASES          20

// Just used to send the index of the id
struct tdata {
    int tid;
};

/* Semaphore barrier */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t arrival, departure;
int counter = 0, arrived = 0;

void barrier(int n) {
    sem_wait(&arrival);
    pthread_mutex_lock(&mutex);
    counter++;
    pthread_mutex_unlock(&mutex);
    if (counter < n) {
        sem_post(&arrival);
    } else {
        sem_post(&departure);
    }
    sem_wait(&departure);
    pthread_mutex_lock(&mutex);
    counter--;
    pthread_mutex_unlock(&mutex);
    if (counter > 0) {
        sem_post(&departure);
    } else {
        sem_post(&arrival);
    }
}

/* END BARRIER */

void *run(void *ptr) {
    int tid = ((struct tdata *) ptr)->tid;
    int i;

    for (i=0; i < PHASES; i++) {
        usleep(random() % 1000000);
        barrier(NUM_THREADS);
        printf("%d finished phase %d\n", tid, i);
    }

    barrier(NUM_THREADS);
    printf("Finished thread %d\n", tid);
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    // Semaphore initialization
    sem_init(&arrival, 0, 1);
    sem_init(&departure, 0, 0);

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, run, (void *) &id[i]);
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}
