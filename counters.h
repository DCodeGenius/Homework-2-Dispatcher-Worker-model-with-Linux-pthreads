#ifndef COUNTERS_H
#define COUNTERS_H

#include "common.h"

extern int g_num_counters;

// === API (Daniel) ===
int  counters_init(int num_counters);
void counters_cleanup(void);

int  counter_increment(int idx);
int  counter_decrement(int idx);
// (optionally) add helper to read current value for debugging

#endif // COUNTERS_H
