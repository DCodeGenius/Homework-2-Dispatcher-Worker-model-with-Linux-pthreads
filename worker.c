#include "worker.h"
#include "parser.h"
#include "counters.h"
#include "stats.h"
#include "logging.h"
#include "timing.h"
#include <time.h>

int g_num_threads = 0;
int g_outstanding_jobs = 0;

parsed_job_t g_parsed_jobs[MAX_THREADS]; //One job per thread at max
pthread_mutex_t g_outstanding_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  g_outstanding_cond  = PTHREAD_COND_INITIALIZER;

// Internal array of pthread_t for workers
static pthread_t g_worker_threads[MAX_THREADS];
static job_queue_t *g_queue = NULL;
static worker_arg_t g_worker_args[MAX_THREADS];

//Execute:
static void do_msleep(long long ms)
{
    if (ms <= 0) {
        return;
    }

    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static void run_basic_command(const basic_cmd_t *cmd){
    if (!cmd) {
        return;
    }

    switch (cmd->type) {
        case CMD_MSLEEP:
            do_msleep(cmd->arg);
        break;
        case CMD_INCREMENT:
            // cast to int, assuming parser validated range
                counter_increment((int)cmd->arg);
        break;
        case CMD_DECREMENT:
            counter_decrement((int)cmd->arg);
        break;
        case CMD_REPEAT:
        case CMD_INVALID:
        default:
            // Should not be executed here – REPEAT is handled at job level
            break;
    }
}

static long long execute_job_line(parsed_job_t *job){
    if (!job || job->num_commands <= 0 || job->commands == NULL) {
        return since_start_ms();
    }
    int n = job->num_commands;
    int r = job->repeat_index;

    // Case 1: no repeat (or invalid repeat_index) – just run all commands in order
    if (r < 0 || r >= n) {
        for (int i = 0; i < n; ++i) { //TODO why ++i not i++
            run_basic_command(&job->commands[i]);
        }
    } else {
        // Case 2: there is a repeat at index r

        // run commands BEFORE repeat once
        for (int i = 0; i < r; ++i) {
            run_basic_command(&job->commands[i]);
        }

        // repeat commands AFTER
        long long times = job->commands[r].arg;
        for (long long t = 0; t < times; ++t) {
            for (int i = r + 1; i < n; ++i) {
                run_basic_command(&job->commands[i]);
            }
        }
    }
    // End time after finishing all commands
    return since_start_ms();
}


// === internal thread function ===
static void *worker_thread_main(void *arg) {
    worker_arg_t *worker_arg = (worker_arg_t *) arg;
    FILE* thread_file = logging_open_worker_log(worker_arg->worker_id);
    long long turn_around_ms = 0;
    job_t *job;
    while (1) {
        job = queue_dequeue(g_queue);
        if (job == NULL) {
            break;
        }
        logging_worker_start_job(thread_file, job->cmdline);
        parser_parse_job_line(job->cmdline , &g_parsed_jobs[worker_arg->worker_id]);
        job->end_time_ms = execute_job_line(&g_parsed_jobs[worker_arg->worker_id]);
        logging_worker_end_job(thread_file, job->cmdline, job->end_time_ms);
        job->turn_around_ms = job->end_time_ms - job->read_time_ms;
        stats_record_job(job->turn_around_ms);
        pthread_mutex_lock(&g_outstanding_mutex);
        g_outstanding_jobs--;
        if (g_outstanding_jobs == 0) {
            pthread_cond_signal(&g_outstanding_cond);
        }
        pthread_mutex_unlock(&g_outstanding_mutex);
        parser_free_parsed_job(&g_parsed_jobs[worker_id]); //Only to free the cmds array
        free_job(job);
    }
    // 2. loop: dequeue job, log start, parse + execute, log end
    // 3. compute turnaround time and call stats_record_job
    // 4. update outstanding_jobs and signal cond if needed
    logging_close_worker_log(thread_file);
    return NULL;
}



int worker_start_threads(int num_threads, job_queue_t *queue) {
    if (num_threads <= 0 || num_threads > MAX_THREADS) {
        return -1;
    }
    g_num_threads = num_threads;
    g_queue = queue;

    for (int i = 0; i < g_num_threads; i++) {
      g_worker_args[i].worker_id = i;
      g_worker_args[i].queue = g_queue;
      if (pthread_create(&g_worker_threads[i], NULL, worker_thread_main, &g_worker_args[i]) != 0){
          printf("Error creating worker thread\n");
          //TODO if there is an error i should call for a method to reap all the threads??? maybe it is the join_threads...
          return -1;
      }
    }
    return 0;
}


void worker_join_threads(void) {
    // TODO: join threads, free array
}
