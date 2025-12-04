#include "worker.h"
#include "parser.h"
#include "counters.h"
#include "stats.h"
#include "logging.h"
#include "timing.h"

int g_num_threads = 0;

int g_outstanding_jobs = 0;
pthread_mutex_t g_outstanding_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_outstanding_cond  = PTHREAD_COND_INITIALIZER;

// Internal array of pthread_t for workers
static pthread_t *g_worker_threads = NULL;
static job_queue_t *g_queue = NULL;

// === internal thread function ===
static void *worker_thread_main(void *arg) {
    // TODO:
    // 1. open threadXX.txt via logging_open_worker_log
    // 2. loop: dequeue job, log start, parse + execute, log end
    // 3. compute turnaround time and call stats_record_job
    // 4. update outstanding_jobs and signal cond if needed
    (void)arg;
    return NULL;
}

int worker_start_threads(int num_threads, job_queue_t *queue) {
    // TODO: allocate g_worker_threads, create threads
    (void)num_threads;
    (void)queue;
    return 0;
}

void worker_join_threads(void) {
    // TODO: join threads, free array
}
