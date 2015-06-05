#define NUM_THREADS      4
#define MAX_COUNT 100000000
#define ARRAY_SIZE  4096

#define SUM(x) ({ int i, sum=0; for (i=0;i<ARRAY_SIZE;i++) sum += x[i]; sum;})
#define PAUSE _mm_pause() /* Abort failed transaction */


