#include "logging.h"
#include "timing.h"

int g_log_enabled = 0;
FILE *g_dispatcher_log = NULL;

// === Dispatcher side (Dan) ===

int logging_open_dispatcher(void) {
    // TODO: if g_log_enabled, open dispatcher.txt
    return 0;
}

void logging_close_dispatcher(void) {
    // TODO: close if open
}

void logging_dispatcher_read_line(const char *line) {
    // TODO: TIME %lld: read cmd line: %s
    (void)line;
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
