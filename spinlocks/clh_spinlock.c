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

// Struct for CLH spinlock
// http://ftp.cs.rochester.edu/u/scott/papers/2001_PPoPP_Timeout.pdf
struct clh_node {
    unsigned char locked;
    struct clh_node *prev;
};

struct clh_node lock_node; // point to an unowned node
struct clh_node *clh_lock = &lock_node; 

int counter = 0;

void lock(struct clh_node *node) {
    struct clh_node *predecessor;

    node->locked = 1;
    node->prev = __atomic_exchange_n(&clh_lock, node, __ATOMIC_SEQ_CST);
    predecessor = node->prev;
    while (predecessor->locked);
}

void unlock(struct clh_node **node) {
    struct clh_node *pred = (*node)->prev;

    (*node)->locked = 0;
    *node = pred;
    __atomic_thread_fence (__ATOMIC_SEQ_CST);
}

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;

    struct clh_node *node;
    node = malloc(sizeof(struct clh_node));

    for (i=0; i < max; i++) {
        lock(node);
        counter += 1; // The global variable, i.e. the critical section
        unlock(&node);
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
