#include "parser.h"

// === Daniel: implement the parsing logic here ===

int parser_parse_job_line(const char *line, parsed_job_t *out) {
    // TODO:
    // 1. split by ';'
    // 2. trim spaces
    // 3. detect "msleep x", "increment x", "decrement x", "repeat x"
    // 4. fill out->commands, out->num_commands, out->repeat_index
    (void)line;
    (void)out;
    return 0;
}

void parser_free_parsed_job(parsed_job_t *job) {
    // TODO: free commands array, etc.
    (void)job;
}
