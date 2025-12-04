#include "timing.h"
#include <time.h>

static long long g_start_ms = 0;

static long long to_ms(struct timespec ts) {
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return to_ms(ts);
}

void timing_set_start(void) {
    g_start_ms = now_ms();
}

long long since_start_ms(void) {
    return now_ms() - g_start_ms;
}
