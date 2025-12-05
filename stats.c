#include "stats.h"
#include <limits.h>   // for LLONG_MAX

stats_t g_stats;

// Initialize stats struct and its mutex.
// Returns 0 on success, -1 on failure.
int stats_init(stats_t *s)
{
    if (!s) return -1;

    s->total_running_time_ms   = 0;

    s->jobs_sum_turnaround_ms  = 0;
    s->jobs_min_turnaround_ms  = LLONG_MAX;  // no jobs yet
    s->jobs_max_turnaround_ms  = 0;
    s->jobs_count              = 0;

    if (pthread_mutex_init(&s->mutex, NULL) != 0) {
        return -1;
    }

    return 0;
}

void stats_destroy(stats_t *s)
{
    if (!s) return;
    pthread_mutex_destroy(&s->mutex);
}

// Called by worker for each completed job.
// Updates sum / min / max / count under mutex.
void stats_record_job(long long turnaround_ms)
{
    pthread_mutex_lock(&g_stats.mutex);

    g_stats.jobs_sum_turnaround_ms += turnaround_ms;
    g_stats.jobs_count++;

    if (turnaround_ms < g_stats.jobs_min_turnaround_ms) {
        g_stats.jobs_min_turnaround_ms = turnaround_ms;
    }
    if (turnaround_ms > g_stats.jobs_max_turnaround_ms) {
        g_stats.jobs_max_turnaround_ms = turnaround_ms;
    }

    pthread_mutex_unlock(&g_stats.mutex);
}

// Called by dispatcher at the very end with total runtime
void stats_set_total_runtime(long long total_ms)
{
    pthread_mutex_lock(&g_stats.mutex);
    g_stats.total_running_time_ms = total_ms;
    pthread_mutex_unlock(&g_stats.mutex);
}
