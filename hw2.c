#include "common.h"
#include "queue.h"
#include "worker.h"
#include "counters.h"
#include "stats.h"
#include "timing.h"
#include "logging.h"
#include "parser.h"

// === Globals owned by dispatcher side (Dan) ===
static job_queue_t g_job_queue;

// Simple helper: trim newline at end of line
static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')) {
        s[len-1] = '\0';
    }
}

// TODO (Dan): parse command line args, validate them
static int parse_args(int argc, char *argv[],
                      char **cmdfile,
                      int *num_threads,
                      int *num_counters,
                      int *log_enabled) {
    (void)argc; (void)argv; (void)cmdfile;
    (void)num_threads; (void)num_counters; (void)log_enabled;
    return 0;
}

// TODO (Dan): handle "dispatcher msleep x"
static void dispatcher_handle_msleep(long long ms) {
    (void)ms;
}

// TODO (Dan): wait until all outstanding worker jobs finished
static void dispatcher_handle_wait(void) {
    // use g_outstanding_jobs + cond
}

// TODO (Dan): handle a worker line: create job_t, enqueue, bump outstanding_jobs
static int dispatcher_enqueue_worker_job(const char *line, long long read_time_ms) {
    (void)line;
    (void)read_time_ms;
    return 0;
}

int main(int argc, char *argv[]) {
    char *cmdfile = NULL;
    int   num_threads  = 0;
    int   num_counters = 0;
    int   log_enabled  = 0;

    if (parse_args(argc, argv, &cmdfile, &num_threads, &num_counters, &log_enabled) != 0) {
        // TODO: print usage / error
        return 1;
    }

    g_log_enabled = log_enabled;

    timing_set_start();
    stats_init(&g_stats);
    queue_init(&g_job_queue);
    counters_init(num_counters);
    logging_open_dispatcher();

    worker_start_threads(num_threads, &g_job_queue);

    // === Main dispatcher loop (Dan) ===
    FILE *f = fopen(cmdfile, "r");
    if (!f) {
        // TODO: print error
        return 1;
    }

    char line_buf[MAX_LINE_LEN];

    while (fgets(line_buf, sizeof(line_buf), f)) {
        trim_newline(line_buf);

        long long t_read = since_start_ms();
        logging_dispatcher_read_line(line_buf);

        // TODO (Dan): decide if "dispatcher ..." or "worker ..."
        // If dispatcher msleep / wait -> handle directly
        // else if worker ... -> dispatcher_enqueue_worker_job(...)
    }

    fclose(f);

    // After EOF: wait for all worker jobs (like dispatcher wait)
    dispatcher_handle_wait();

    // Compute total runtime and set stats
    long long total_ms = since_start_ms();
    stats_set_total_runtime(total_ms);

    // TODO (Dan): write stats.txt (using g_stats fields)

    // Cleanup
    logging_close_dispatcher();
    worker_join_threads();
    counters_cleanup();
    queue_destroy(&g_job_queue);
    stats_destroy(&g_stats);

    return 0;
}
