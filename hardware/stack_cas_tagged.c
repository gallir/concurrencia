// This code implements the right concurrent stack with ABA check
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>


#define NUM_THREADS		 4
#define OPERATIONS       10000000

// Just used to send the index of the id
struct tdata {
	int tid;
};

struct node_data {
	int tid;
	int c;
};

struct node_head {
	struct node *node;
	uintptr_t aba;
};

struct node {
	struct node_data data;
	struct node *next;
};

struct node_head stack_head;
struct node_head free_nodes;

void push(struct node_head *head, struct node *e) {
	struct node_head old_head, next;

	// The structure is not an "atomic register", we must force the atomic load
	__atomic_load(head, &old_head, __ATOMIC_RELAXED);
	do {
		next.aba = old_head.aba + 1;
		next.node = e;
		e->next = old_head.node;
	} while (! __atomic_compare_exchange(head, &old_head, &next, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
}


struct node *pop(struct node_head *head) {
	struct node_head old_head, next;

	// The structure is not an "atomic register", we must force the atomic load
	__atomic_load(head, &old_head, __ATOMIC_SEQ_CST);
	do {
		if (! old_head.node) {
			return NULL;
		}
		next.aba = old_head.aba + 1;
		next.node = old_head.node->next;
	} while (! __atomic_compare_exchange(head, &old_head, &next, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));

	return old_head.node;
}

void *add_elements(void *ptr) {
	long i, elems = OPERATIONS/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;
	struct node *e;

	for (i=0; i < elems; i++) {
		e = pop(&free_nodes); // Get an element from the free list
		if (! e) { // If there is none, reserve memory
			e = malloc(sizeof(struct node));
			 printf("%d malloc\n", tid);
		}
		e->data.tid = tid;
		e->data.c = i;
		push(&stack_head, e);
		// Pop an element and add it to the free list
		e = pop(&stack_head);
		if (e) {
			push(&free_nodes, e);
		} else {
			printf("Error in %d it shouldn't be empty\n", tid);
		}
	}

	printf("End %d\n", tid);
	pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc, i;
	struct tdata id[NUM_THREADS];
	struct node *e, *tmp;

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

	return 0;
}
