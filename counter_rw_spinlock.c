#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>


#define NUM_THREADS	     4
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
	int tid;
};

// Used for swapping in __sync_bool_compare_and_swap
unsigned int rw_lock = 0; // We use 32 bits

int counter = 0;

void reader_lock() {
	while (1) {
		while(rw_lock & 0x80000000) {
			sched_yield(); // Release the CPU
		}

		unsigned int old = rw_lock & 0x7fffffff;
		unsigned int new = old + 1;

		if (__sync_bool_compare_and_swap(&rw_lock, old, new)) {
			return;
		}
	}
}

void writer_lock() {
	while (1) {
		while(rw_lock & 0x80000000) {
			sched_yield(); // Release the CPU
		}

		unsigned int old = rw_lock & 0x7fffffff;
		unsigned int new = old | 0x80000000;

		if (__sync_bool_compare_and_swap(&rw_lock, old, new)) {
			// wait for readers
			while (rw_lock & 0x7fffffff) {
				sched_yield(); // Release the CPU
			}
			return;
		}
	}
}

void reader_unlock() {
	__sync_fetch_and_add(&rw_lock, -1);
}

void writer_unlock() {
	assert(rw_lock == 0x80000000); // It should be so
	rw_lock = 0;
}


void *count(void *ptr) {
	long i, max = MAX_COUNT/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;

	for (i=0; i < max; i++) {
		writer_lock();
		counter += 1; // The global variable, i.e. the critical section
		writer_unlock();

		// now we check the reader_lock works
		reader_lock();
		int tmp = counter;
		assert(tmp == counter); // Should still be the same value
		reader_unlock();
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

