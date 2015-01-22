// Compiled with gcc -O -pthread -o counter counter.c 

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS	 2
#define MAX_COUNT 10000000

// Just used to send the index of the id
struct tdata {
	int tid;
};

int counter = 0;


void *count(void *ptr) {
	long i, max = MAX_COUNT/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;
	int local_counter = 0;

	for (i=0; i < max; i++) {
		local_counter += 1; // The global variable, i.e. the critical section
	}

	printf("End %d counter: %d\n", tid, local_counter);
	counter += local_counter;
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

