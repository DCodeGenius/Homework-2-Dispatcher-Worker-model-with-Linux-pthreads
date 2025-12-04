#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

// Node for linked list queue
typedef struct job_node {
    job_t             *job;
    struct job_node   *next;
} job_node_t;

// Shared job queue
typedef struct job_queue {
    job_node_t      *head;
    job_node_t      *tail;
    int              size;
    int              shutdown;        // set when program is shutting down
    pthread_mutex_t  mutex;
    pthread_cond_t   not_empty;
} job_queue_t;

// === queue API to implemented queue.c ===
int  queue_init(job_queue_t *q);
void queue_destroy(job_queue_t *q);
int  queue_enqueue(job_queue_t *q, job_t *job);
job_t *queue_dequeue(job_queue_t *q);    // blocks if empty (unless shutdown)

#endif // QUEUE_H
