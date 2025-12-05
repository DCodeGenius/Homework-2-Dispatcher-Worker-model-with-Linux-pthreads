#ifndef LOGGING_H
#define LOGGING_H

#include "common.h"

// log_enabled: 1 or 0 from command line
extern int g_log_enabled;

// Dispatcher log file
extern FILE *g_dispatcher_log;

// === Dispatcher logging (Dan) ===
int  logging_open_dispatcher(void);
void logging_close_dispatcher(void);
void logging_dispatcher_read_line(const char *line);

// === Worker logging (Daniel) ===
// Each worker owns its own FILE*
FILE *logging_open_worker_log(int worker_id);
void   logging_close_worker_log(FILE *f);
void   logging_worker_start_job(FILE *f, const char *line);
void   logging_worker_end_job(FILE *f, const char *line, long long end_ms);

#endif // LOGGING_H
