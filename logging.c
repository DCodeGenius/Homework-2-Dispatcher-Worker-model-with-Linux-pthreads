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
static void build_thread_filename(int idx, char *buf, size_t buf_size)
{
    // threadXX.txt, XX = 00..99 (two digits)
    snprintf(buf, buf_size, "thread%02d.txt", idx);
}


FILE *logging_open_worker_log(int worker_id) {
    if (!g_log_enabled) {
      return NULL;
    }
    char fname[32];
    build_thread_filename(worker_id, fname, sizeof(fname));

    FILE *f = fopen(fname, "w");
    if (f == NULL) {
      printf("Failed to open log file %s\n", fname);
      return NULL;
    }
    return f;

}

void logging_close_worker_log(FILE *f) {
    if (f != NULL) {
        fclose(f);
    }
}

void logging_worker_start_job(FILE *f, const char *line) {
    if (!f || !g_log_enabled) return;
    long long start_ms = since_start_ms();
    fprintf(f,"TIME %lld: START job %s\n",start_ms, line);
    fflush(f);
    return;
}

void logging_worker_end_job(FILE *f, const char *line, long long end_ms) {
    if (!f || !g_log_enabled) return;
    fprintf(f,"TIME %lld: END job %s\n",end_ms, line);
    fflush(f);
    return;
}
