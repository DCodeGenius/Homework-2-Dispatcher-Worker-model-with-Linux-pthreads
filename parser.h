#ifndef PARSER_H
#define PARSER_H

#include "common.h"

// Basic command types inside a worker job line
typedef enum {
    CMD_INVALID = 0,
    CMD_MSLEEP,
    CMD_INCREMENT,
    CMD_DECREMENT,
    CMD_REPEAT
} basic_cmd_type_t;

typedef struct {
    basic_cmd_type_t type;
    long long        arg;      // x in msleep x / increment x etc.
} basic_cmd_t;

// Represents a parsed job line
typedef struct parsed_job {
    basic_cmd_t *commands;
    int          num_commands;
    int          repeat_index;   // index of 'repeat' command or -1 if none
} parsed_job_t;

// === API (Daniel) ===
int   parser_parse_job_line(const char *line, parsed_job_t *out);
void  parser_free_parsed_job(parsed_job_t *job);

#endif // PARSER_H
