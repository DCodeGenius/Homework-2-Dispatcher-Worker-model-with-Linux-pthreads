#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LINE_LEN    1024
#define MAX_THREADS     4096
#define MAX_COUNTERS    100

// === Job structure (Dan + Daniel) ===
// Dispatcher fills cmdline + read_time_ms.
// Worker uses it (and later can extend with parsed representation).
typedef struct job {
    char       cmdline[MAX_LINE_LEN+1];        // full original line from cmd file
    long long   read_time_ms;   // when dispatcher read this line
    long long   end_time_ms; // When worker finish it
    long long   turn_around_ms // end - read
    // TODO (Daniel): add fields for parsed representation if needed
} job_t;

#endif // COMMON_H
