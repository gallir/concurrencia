#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <semaphore.h>


#define NUM_THREADS      4
#define MAX_COUNT 10000000

struct tdata {
    int tid;
};

int counter = 0;
sem_t mutex; // Used for a unnamed POSIX semaphore

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    for (i=0; i < max; i++) {
        sem_wait(&mutex);
        counter += 1;
        sem_post(&mutex);
    }

    printf("End %d counter: %d\n", tid, counter);
    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    sem_init(&mutex, 0, 1); // Semaphore initialización
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
