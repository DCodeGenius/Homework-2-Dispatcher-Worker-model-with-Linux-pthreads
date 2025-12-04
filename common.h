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
    char       *cmdline;        // full original line from cmd file
    long long   read_time_ms;   // when dispatcher read this line
    // TODO (Daniel): add fields for parsed representation if needed
} job_t;

#endif // COMMON_H
