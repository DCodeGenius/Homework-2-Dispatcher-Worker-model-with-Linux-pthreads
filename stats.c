#include "stats.h"

stats_t g_stats;

// === Implementations (mostly Daniel for record_job), Dan can use set_total_runtime ===

int stats_init(stats_t *s) {
    // TODO: initialize fields and mutex, set min to something large
    return 0;
}

void stats_destroy(stats_t *s) {
    // TODO: destroy mutex
}

void stats_record_job(long long turnaround_ms) {
    // TODO: lock, update sum/min/max/count, unlock
    (void)turnaround_ms;
}

void stats_set_total_runtime(long long total_ms) {
    // TODO: probably just set g_stats.total_running_time_ms = total_ms
    (void)total_ms;
}
