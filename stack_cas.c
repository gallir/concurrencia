// This code implements the right concurrent stack with ABA check
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdatomic.h>


#define NUM_THREADS		 4
#define OPERATIONS		100000

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
	struct node_head old_head;
	struct node_head next;
	do {
		__atomic_load(head, &old_head, __ATOMIC_RELAXED);
		next.aba = old_head.aba + 1;
		next.node = e;
		e->next = old_head.node;
	} while (! atomic_compare_exchange_weak(head, &old_head, next));
}


struct node *pop(struct node_head *head) {
	struct node_head old_head;
	struct node_head next;
	do {
		__atomic_load(head, &old_head, __ATOMIC_RELAXED);
		if (! old_head.node) {
			return NULL;
		}
		next.aba = old_head.aba + 1;
		next.node = old_head.node->next;
	} while (! atomic_compare_exchange_weak(head, &old_head, next));

	return old_head.node;
}

void *add_elements(void *ptr) {
	long i, elems = OPERATIONS/NUM_THREADS;
	int tid = ((struct tdata *) ptr)->tid;
	struct node *e;

	for (i=0; i < elems; i++) {
		e = pop(&free_nodes); // Get an element from the free list
		e->data.tid = tid;
		e->data.c = i;
		push(&stack_head, e);
		// Pop an element and add it to the free list
		e = pop(&stack_head);
		push(&free_nodes, e);
	}

	printf("End %d\n", tid);
	pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int rc, i;
	struct tdata id[NUM_THREADS];
	struct node *e, *tmp;

	// Do a malloc for all possible nodes and add them to a free list
	for (i=0; i < OPERATIONS; i++) {
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

	return 0;
}

