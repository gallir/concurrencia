#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS	     4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
	int tid;
};

// Struct for MCS spinlock
struct mcs_spinlock {
	struct mcs_spinlock *next;
	unsigned locked;
};

struct mcs_spinlock *mcs_tail = NULL;

int counter = 0;

void lock(struct mcs_spinlock *node) {
	struct mcs_spinlock *predecessor = node;
	node->next = NULL;
	predecessor = __atomic_exchange_n(&mcs_tail, node, __ATOMIC_RELAXED);
	if (predecessor != NULL) {
		node->locked = 1;
		predecessor->next = node;
		while (node->locked);
	}
}

void unlock(struct mcs_spinlock *node) {
	struct mcs_spinlock *last = node;

	if (! node->next) { // I'm he last in the queue
		if (__atomic_compare_exchange_n(&mcs_tail, &last, NULL, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED) ) {
			return;
		} else {
			// Another process executed exchange but
			// didn't asssign next yet, so wait
			while (! node->next);
		}
	}
	node->next->locked = 0;
}

void *count(void *ptr) {
	long i, max = MAX_COUNT/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;

	struct mcs_spinlock node;

	for (i=0; i < max; i++) {
		lock(&node);
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

