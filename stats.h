#ifndef STATS_H
#define STATS_H

#include "common.h"

typedef struct stats {
    long long total_running_time_ms;

    long long jobs_sum_turnaround_ms;
    long long jobs_min_turnaround_ms;
    long long jobs_max_turnaround_ms;
    long long jobs_count;

    pthread_mutex_t mutex;
} stats_t;

// global stats object
extern stats_t g_stats;

// === API ===
int  stats_init(stats_t *s);
void stats_destroy(stats_t *s);

// Called by worker (Daniel) for each completed job
void stats_record_job(long long turnaround_ms);

// Called by dispatcher (Dan) at the end
void stats_set_total_runtime(long long total_ms);

#endif // STATS_H
