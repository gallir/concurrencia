#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "../tinySTM/include/stm.h"

#include "defs.h"

// Just used to send the index of the id
struct tdata {
    int tid;
};

#define TM_START(tid, ro)               { stm_tx_attr_t _a = {{.id = tid, .read_only = ro}}; sigjmp_buf *_e = stm_start(_a); if (_e != NULL) sigsetjmp(*_e, 0)
#define TM_COMMIT                       stm_commit(); }


int counter[ARRAY_SIZE];
int in_stm = 0;

void *count(void *ptr) {
    long i, max = MAX_COUNT/NUM_THREADS;
    int tid = ((struct tdata *) ptr)->tid;
    int c, position;

    stm_init_thread();

    for (i=0; i < max; i++) {
        position = i % ARRAY_SIZE;
        if (_xbegin() == _XBEGIN_STARTED) {
            if (in_stm) {
                _xabort(1); /* In STM */
            }
            counter[position]++;
            _xend();
        } else {
            __atomic_fetch_add(&in_stm, 1, __ATOMIC_RELEASE);
            TM_START(0, 0);
            c = stm_load_int(&counter[position]);
            c++;
            stm_store_int(&counter[position], c);
            TM_COMMIT;
            __atomic_fetch_sub(&in_stm, 1, __ATOMIC_RELEASE);
        }
    }
    printf("End %d counter: %d\n", tid, SUM(counter));
    stm_exit_thread();
}

int main (int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, i;
    struct tdata id[NUM_THREADS];


    for(i=0; i<NUM_THREADS; i++){
        id[i].tid = i;
        rc = pthread_create(&threads[i], NULL, count, (void *) &id[i]);
    }

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Counter value: %d Expected: %d\n", SUM(counter), MAX_COUNT);
    stm_exit();
    return 0;
}
