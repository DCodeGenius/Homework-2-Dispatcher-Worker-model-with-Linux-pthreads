#include "queue.h"

// === Dan: implement these ===

int queue_init(job_queue_t *q) {
    // TODO: initialize fields, mutex, cond
    return 0;
}

void queue_destroy(job_queue_t *q) {
    // TODO: free remaining nodes, destroy mutex & cond
}

int queue_enqueue(job_queue_t *q, job_t *job) {
    // TODO: push node to tail, signal not_empty
    return 0;
}

job_t *queue_dequeue(job_queue_t *q) {
    // TODO: wait while empty and !shutdown, then pop head
    return NULL;
}
