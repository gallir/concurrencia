#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "tinySTM/include/stm.h"

#include "defs.h"

// Just used to send the index of the id
struct tdata {
    int tid;
};

#define TM_START(tid, ro)               { stm_tx_attr_t _a = {{.id = tid, .read_only = ro}}; sigjmp_buf *_e = stm_start(_a); if (_e != NULL) sigsetjmp(*_e, 0)
#define TM_LOAD(addr)                   stm_load((stm_word_t *)addr)
#define TM_STORE(addr, value)           stm_store((stm_word_t *)addr, (stm_word_t)value)
#define TM_COMMIT                       stm_commit(); }

#define TM_INIT                         stm_init(); mod_ab_init(0, NULL)
#define TM_EXIT                         stm_exit()
#define TM_INIT_THREAD                  stm_init_thread()
#define TM_EXIT_THREAD                  stm_exit_thread()


int counter[ARRAY_SIZE];

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;
    int c;

    TM_INIT_THREAD;

    for (i=0; i < max; i++) {
        TM_START(0, 1);
        c = TM_LOAD(&counter[i % ARRAY_SIZE]);
        c++;
        TM_STORE(&counter[i % ARRAY_SIZE], c);
        TM_COMMIT;
    }
    printf("End %d counter: %d\n", tid, SUM(counter));
    TM_EXIT_THREAD;
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];

    TM_INIT;

    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, count, (void *) &id[i]);
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d Expected: %d\n", SUM(counter), MAX_COUNT);
    TM_EXIT;
    return 0;
}
