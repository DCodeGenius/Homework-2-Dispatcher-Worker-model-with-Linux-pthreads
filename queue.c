#include "queue.h"

// === Dan: implement these ===

int queue_init(job_queue_t *q) {
    if (q == NULL) {
      fprintf(stderr, "queue is null\n");
      return -1;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    if(pthread_mutex_init(&q->mutex, NULL) != 0) return -1;
    if(pthread_cond_init(&q->not_empty, NULL) != 0 ) return -1;
    q->shutdown = 0;

    return 0;
}

void queue_destroy(job_queue_t *q) {
  pthread_mutex_lock(&q->mutex);
  while( q->head != NULL ) {
    job_node_t *tmp = q->head;
    q->head = tmp->next;
    free(tmp->job);
    free(tmp);
    }
    pthread_mutex_unlock(&q->mutex);


   pthread_cond_destroy(&q->not_empty);
   pthread_mutex_destroy(&q->mutex);
}

int queue_enqueue(job_queue_t *q, job_t *job) {
  if (q == NULL || job == NULL) {
    fprintf(stderr, "job/queue is null, illegal\n");
    return -1;
  }
    job_node_t *new_node = malloc(sizeof(job_node_t));
    if (new_node == NULL) {
      fprintf(stderr, "queue_enqueue failed\n");
      return -1;
    }
    new_node->job = job;
    new_node->next = NULL;
    pthread_mutex_lock(&q->mutex);
    if( q->head == NULL ) {
      q->head = new_node;
      q->tail = new_node;
    }
    else {
      q->tail->next = new_node;
      q->tail = new_node;
    }
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

job_t *queue_dequeue(job_queue_t *q) {
  pthread_mutex_lock(&q->mutex);
  while (q->size == 0 && !q->shutdown)
    pthread_cond_wait(&q->not_empty, &q->mutex);

  if (q->shutdown && q->size == 0) {
    pthread_mutex_unlock(&q->mutex);
    return NULL;
  }
  else{
    q->size--;
    job_t *job = q->head->job;
    job_node_t *tmp = q->head;
    q->head = tmp->next;
    if (q->size == 0) {
      q->tail = NULL;
    }
    pthread_mutex_unlock(&q->mutex);
    free(tmp);
    return job;
    }
}

void queue_shutdown(job_queue_t *q) {
pthread_mutex_lock(&q->mutex);
q->shutdown = 1;
pthread_cond_broadcast(&q->not_empty);
pthread_mutex_unlock(&q->mutex);
};
