#ifndef WORKER_H
#define WORKER_H

#include "common.h"
#include "queue.h"

// Number of worker threads passed from main
extern int g_num_threads;

// job counter used for dispatcher wait (shared)
extern int g_outstanding_jobs;
extern pthread_mutex_t g_outstanding_mutex;
extern pthread_cond_t  g_outstanding_cond;

typedef struct worker_arg {
    int worker_id;   // 0, 1, 2, ...
    job_queue_t *queue;       // pointer to the shared queue
} worker_arg_t;

// === API (Daniel) ===
int  worker_start_threads(int num_threads, job_queue_t *queue);
void worker_join_threads(void);

#endif // WORKER_H
