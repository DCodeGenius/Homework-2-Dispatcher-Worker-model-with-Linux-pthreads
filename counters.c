#include "counters.h"
#include <pthread.h>

int g_num_counters = 0;

// One mutex per counter file
static pthread_mutex_t g_counter_mutexes[MAX_COUNTERS];

// === Implementations (Daniel) ===

static void build_counter_filename(int idx, char *buf, size_t buf_size)
{
    // spec: countxx.txt, xx = 00..99 (two digits)
    snprintf(buf, buf_size, "count%02d.txt", idx);
}


int counters_init(int num_counters)
{
    if (num_counters < 0 || num_counters > MAX_COUNTERS) {
        return -1;
    }

    g_num_counters = num_counters;

    // Init mutexes and create/initialize files
    for (int i = 0; i < g_num_counters; ++i) {
        if (pthread_mutex_init(&g_counter_mutexes[i], NULL) != 0) {
            return -1;
        }

        char fname[32];
        build_counter_filename(i, fname, sizeof(fname));

        FILE *f = fopen(fname, "w");
        if (!f) {
            return -1;
        }

        long long zero = 0;
        // spec says decimal, %lld, single line "0"
        fprintf(f, "%lld\n", zero);
        fclose(f);
    }

    return 0;
}

void counters_cleanup(void)
{
    for (int i = 0; i < g_num_counters; ++i) {
        pthread_mutex_destroy(&g_counter_mutexes[i]);
    }
    g_num_counters = 0;
}

// Internal helper to read-modify-write one counter
static int counter_update(int idx, long long delta)
{
    if (idx < 0 || idx >= g_num_counters) {
        return -1;
    }

    pthread_mutex_t *mtx = &g_counter_mutexes[idx];
    pthread_mutex_lock(mtx);

    char fname[32];
    build_counter_filename(idx, fname, sizeof(fname));

    FILE *f = fopen(fname, "r+");
    if (!f) {
        pthread_mutex_unlock(mtx);
        return -1;
    }

    long long val = 0;
    if (fscanf(f, "%lld", &val) != 1) {
        // if file is corrupted, treat as 0
        val = 0;
    }

    val += delta;

    // rewind to beginning and overwrite
    rewind(f);
    fprintf(f, "%lld\n", val);
    fflush(f);
    fclose(f);

    pthread_mutex_unlock(mtx);
    return 0;
}

int counter_increment(int idx)
{
    return counter_update(idx, +1);
}

int counter_decrement(int idx)
{
    return counter_update(idx, -1);
}