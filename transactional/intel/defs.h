#include "../defs.h"

#include <immintrin.h> // For _mm_pause
#define PAUSE _mm_pause() /* Abort failed transaction */

