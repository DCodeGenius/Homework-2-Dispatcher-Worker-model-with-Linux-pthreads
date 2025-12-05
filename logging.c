#include "logging.h"
#include "timing.h"

int g_log_enabled = 0;
FILE *g_dispatcher_log = NULL;

// === Dispatcher side (Dan) ===

int logging_open_dispatcher(void) {
    if (!g_log_enabled) {
        return 0; // logging disabled
    }

    g_dispatcher_log = fopen("dispatcher.txt", "w");
    if (!g_dispatcher_log) {
        fprintf(stderr, "Failed to open dispatcher.txt for writing\n");
        return -1;
    }
    return 0;
}

void logging_close_dispatcher(void) {
    if (g_dispatcher_log) {
        fclose(g_dispatcher_log);
        g_dispatcher_log = NULL;
    }
}

void logging_dispatcher_read_line(const char *line) {
    if (!g_log_enabled || !g_dispatcher_log) {
        return;
    }
    long long t = since_start_ms();
    fprintf(g_dispatcher_log, "TIME %lld: read cmd line: %s\n", t, line);
    fflush(g_dispatcher_log); // optional but safe
}

// === Worker side (Daniel) ===

FILE *logging_open_worker_log(int worker_id) {
    // TODO: open threadXX.txt if g_log_enabled
    (void)worker_id;
    return NULL;
}

void logging_close_worker_log(FILE *f) {
    // TODO: close if non-NULL
    (void)f;
}

void logging_worker_start_job(FILE *f, const char *line) {
    // TODO: TIME %lld: START job %s
    (void)f;
    (void)line;
}

void logging_worker_end_job(FILE *f, const char *line) {
    // TODO: TIME %lld: END job %s
    (void)f;
    (void)line;
}
