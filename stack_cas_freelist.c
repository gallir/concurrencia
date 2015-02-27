// This code implements an FAULTY concurrent stack
// It's suffers of the ABA problem it will generate a Segmentation Fault
// In ARM the macro translates to load-linked/store-conditional

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS	     4
#define OPERATIONS      1000000

// Just used to send the index of the id
struct tdata {
	int tid;
};

struct node_data {
	int tid;
	int c;
};

struct node {
	struct node *next;
	struct node_data data;
};

struct node *head = NULL;
struct node *free_nodes = NULL;

void push(struct node **head, struct node *e) {
	e->next = *head;
	while (! __atomic_compare_exchange(head, &e->next, &e, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
}

struct node *pop(struct node **head) {
	struct node *result, *old_head;

	old_head = *head;
	if (! old_head) {
		return NULL;
	}
	while (! __atomic_compare_exchange(head, &old_head, &old_head->next, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED));

	return old_head;
}

void *add_elements(void *ptr) {
	long i, elems = OPERATIONS/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;
	struct node *e;

	for (i=0; i < elems; i++) {
		// Get an element from the free list
		e = pop(&free_nodes);
		if (! e) {
			// It was empty, request memory
			e = malloc(sizeof(struct node));
		}
		e->data.tid = tid;
		e->data.c = i;
		push(&head, e);
		// Pop an element and add it to the free list
		e = pop(&head);
		if (e) {
			push(&free_nodes, e);
		}
	}

	printf("End %d\n", tid);
	pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc, i;
	struct tdata id[NUM_THREADS];
	struct node *e;

	for(i=0; i<NUM_THREADS; i++){
		id[i].tid = i;
		rc = pthread_create(&threads[i], NULL, add_elements, (void *) &id[i]);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	for(i=0; i<NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}
}
