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

/* Monitor barrier */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t allArrived = PTHREAD_COND_INITIALIZER;
int arrived = 0;

void barrier(int n) {
    pthread_mutex_lock(&mutex);

    arrived++;
    if (arrived == n) {
        arrived = 0;
        pthread_cond_broadcast(&allArrived);
    } else {
        pthread_cond_wait(&allArrived, &mutex);
    }

    pthread_mutex_unlock(&mutex);
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
