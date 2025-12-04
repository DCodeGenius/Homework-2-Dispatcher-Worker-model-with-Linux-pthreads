#include "counters.h"
#include <pthread.h>

int g_num_counters = 0;

// One mutex per counter file
static pthread_mutex_t g_counter_mutexes[MAX_COUNTERS];

// === Implementations (Daniel) ===

int counters_init(int num_counters) {
    // TODO: create countXX.txt files with 0, init mutexes
    (void)num_counters;
    return 0;
}

void counters_cleanup(void) {
    // TODO: destroy mutexes if needed
}

int counter_increment(int idx) {
    // TODO: lock mutex, read file, increment, write back
    (void)idx;
    return 0;
}

int counter_decrement(int idx) {
    // TODO: lock mutex, read file, decrement, write back
    (void)idx;
    return 0;
}
