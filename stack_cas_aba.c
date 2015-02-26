// This code implements an FAULTY concurrent stack
// It's suffers of the ABA problem
// Sometimes (or most) of the times it will generate a Segmentation Fault

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


#define NUM_THREADS	     4
#define OPERATIONS      100000

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
	struct node *tmp;

	do {
		e->next = tmp = *head;
	} while (! __atomic_compare_exchange_n(head, &tmp, e, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
}

struct node *pop(struct node **head) {
	struct node *result, *old_head;
	struct node *tmp;

	do {
		tmp = old_head = *head;
		if (! old_head) {
			return NULL;
		}
	} while (! __atomic_compare_exchange_n(head, &tmp, old_head->next, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));

	return old_head;
}
		


void *add_elements(void *ptr) {
	long i, elems = OPERATIONS/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;
	struct node *e;

	for (i=0; i < elems; i++) {
		e = pop(&free_nodes); // Get an element from the free list
		e->data.tid = tid;
		e->data.c = i;
		push(&head, e);
		// Pop an element and add it to the free list
		e = pop(&head);
		push(&free_nodes, e);
	}

	printf("End %d\n", tid);
	pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc, i;
	struct tdata id[NUM_THREADS];
	struct node *e;

	for (i=0; i < OPERATIONS; i++) {
		// Add an elements
		e = malloc(sizeof(struct node));
		push(&free_nodes, e);
	}
		
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

