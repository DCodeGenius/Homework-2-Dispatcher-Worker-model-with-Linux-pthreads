#include "common.h"
#include "queue.h"
#include "worker.h"
#include "counters.h"
#include "stats.h"
#include "timing.h"
#include "logging.h"
#include "parser.h"

#define  MAX_NUM_THREADS 4096
#define MAX_NUM_COUNTERS 100

// === Globals owned by dispatcher side (Dan) ===
static job_queue_t g_job_queue;

// Simple helper: trim newline at end of line
static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')) {
        s[len-1] = '\0';
    }
}
static char* ltrim(char *s) {
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    return s;
}



static int parse_args(int argc, char *argv[],
                      char **cmdfile,
                      int *num_threads,
                      int *num_counters,
                      int *log_enabled) {
    if (argc != 5){
      fprintf(stderr, "Wrong number of arguments\n");
      return -1;
      }
      *cmdfile = argv[1];
      *num_threads = atoi(argv[2]);
      *num_counters = atoi(argv[3]);
      *log_enabled = atoi(argv[4]);
      if (*num_threads < 1 || *num_threads > MAX_NUM_THREADS) {
        fprintf(stderr, "Wrong number of threads\n");
        return -1;
      }
      if (*num_counters < 1 || *num_counters > MAX_NUM_COUNTERS) {
        fprintf(stderr, "Wrong number of counters\n");
        return -1;
      }

      if (*log_enabled < 0 || *log_enabled > 1) {
        fprintf(stderr, "Wrong log enabled\n");
        return -1;
      }
    return 0;
}


static void dispatcher_handle_msleep(long long ms) {
    usleep(ms * 1000);
}

static void dispatcher_handle_wait(void) {
    pthread_mutex_lock(&g_outstanding_mutex);
    while (g_outstanding_jobs > 0 ) {
      pthread_cond_wait(&g_outstanding_cond, &g_outstanding_mutex);
    }
    pthread_mutex_unlock(&g_outstanding_mutex);
}

static int dispatcher_enqueue_worker_job(const char *line, long long read_time_ms) {
  job_t *job = (job_t *)malloc(sizeof(job_t));
  if (job == NULL) {
    fprintf(stderr, "malloc failed!\n");
    return -1;
  }
  strncpy(job->cmdline, line, MAX_LINE_LEN);
  job->cmdline[MAX_LINE_LEN] = '\0';
  job->read_time_ms = read_time_ms;
  if (queue_enqueue(&g_job_queue, job) != 0){
    fprintf(stderr, "queue_enqueue failed!\n");
    free(job);
    return -1;
    }

  pthread_mutex_lock(&g_outstanding_mutex);
  g_outstanding_jobs++;
  pthread_mutex_unlock(&g_outstanding_mutex);
    return 0;
}

int main(int argc, char *argv[]) {
    char *cmdfile = NULL;
    int   num_threads  = 0;
    int   num_counters = 0;
    int   log_enabled  = 0;

    if (parse_args(argc, argv, &cmdfile, &num_threads, &num_counters, &log_enabled) != 0) {
        printf("Error in parsing cmd args, see stderr for more info, exiting\n");
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
        fprintf(stderr, "Failed to open file %s\n", cmdfile);
        return 1;
    }

    char line_buf[MAX_LINE_LEN];
    char* parse_buf;

    while (fgets(line_buf, sizeof(line_buf), f)) {
        trim_newline(line_buf);

        parse_buf = ltrim(line_buf);
        long long t_read = since_start_ms();
        logging_dispatcher_read_line(line_buf);

        if (strncmp(parse_buf, "dispatcher", 10) == 0) {
            if (strncmp(parse_buf, "dispatcher_msleep", 17) == 0) {
              char* q = parse_buf + 17;
              q = ltrim(q);
              long long msleep = atoll(q);
              dispatcher_handle_msleep(msleep);
            }
            else if (strncmp(parse_buf, "dispatcher_wait", 15) == 0) {
              dispatcher_handle_wait();
            }
            else{
              fprintf(stderr, "Unknown dispatcher command\n");
              continue;
              }
        }
        else{
          dispatcher_enqueue_worker_job(line_buf, t_read);
        }
    }

    fclose(f);

    // After EOF: wait for all worker jobs (like dispatcher wait)
    dispatcher_handle_wait();

    pthread_mutex_lock(&g_job_queue.mutex);
    g_job_queue.shutdown = 1;
    pthread_cond_broadcast(&g_job_queue.not_empty);
    pthread_mutex_unlock(&g_job_queue.mutex);

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
