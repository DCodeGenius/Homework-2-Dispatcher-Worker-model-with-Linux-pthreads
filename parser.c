#include "parser.h"
#include "common.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define KW_MSLEEP   "msleep"
#define KW_INC      "increment"
#define KW_DEC      "decrement"
#define KW_REPEAT   "repeat"
#define KW_WORKER   "worker"

// --- small helpers ---

// Trim leading + trailing whitespace, in-place.
// Returns pointer to first non-space char (may be '\0').
static char *trim(char *s) {
    char *start;
    char *end;

    if (!s) return s;

    // skip leading spaces
    start = s;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        return start; // string was all spaces
    }

    // skip trailing spaces
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';

    return start;
}

// case-sensitive "starts with"
static int starts_with(const char *s, const char *prefix) {
    while (*prefix) {
        if (*s != *prefix) {
            return 0;
        }
        s++;
        prefix++;
    }
    return 1;
}

// Parse "<keyword> <number>" into basic_cmd_t.
// s is already trimmed. Returns 0 on success, -1 on error.
static int parse_one_command(char *s, basic_cmd_t *out_cmd) {
    if (!s || !out_cmd) {
        return -1;
    }

    char *p = s;

    // msleep
    if (starts_with(p, KW_MSLEEP)) {
        out_cmd->type = CMD_MSLEEP;
        p += strlen(KW_MSLEEP);
    }
    // increment
    else if (starts_with(p, KW_INC)) {
        out_cmd->type = CMD_INCREMENT;
        p += strlen(KW_INC);
    }
    // decrement
    else if (starts_with(p, KW_DEC)) {
        out_cmd->type = CMD_DECREMENT;
        p += strlen(KW_DEC);
    }
    // repeat
    else if (starts_with(p, KW_REPEAT)) {
        out_cmd->type = CMD_REPEAT;
        p += strlen(KW_REPEAT);
    }
    else {
        out_cmd->type = CMD_INVALID;
        out_cmd->arg  = 0;
        return -1; // unknown command
    }

    // skip spaces before argument
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    if (*p == '\0') {
        // no argument provided
        return -1;
    }

    char *endptr = NULL;
    long long val = strtoll(p, &endptr, 10);
    if (p == endptr) {
        // no digits parsed
        return -1;
    }

    out_cmd->arg = val;
    return 0;
}

// --- API implementation ---

int parser_parse_job_line(const char *line, parsed_job_t *out) {
    if (!line || !out) {
        return -1;
    }

    // Initialize output to safe defaults
    out->commands     = NULL;
    out->num_commands = 0;
    out->repeat_index = -1;

    // Make two working copies of the line because strtok modifies it
    char buf1[MAX_LINE_LEN];
    char buf2[MAX_LINE_LEN];

    strncpy(buf1, line, MAX_LINE_LEN);
    buf1[MAX_LINE_LEN - 1] = '\0';

    strncpy(buf2, line, MAX_LINE_LEN);
    buf2[MAX_LINE_LEN - 1] = '\0';

    // -------- First pass: count commands --------
    int count = 0;
    int seen_worker_prefix = 0;

    char *saveptr = NULL;
    char *token   = strtok_r(buf1, ";", &saveptr);

    while (token != NULL) {
        char *part = trim(token);

        if (*part == '\0') {
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }

        if (!seen_worker_prefix) {
            // This should be "worker <id>", we skip it
            if (!starts_with(part, KW_WORKER)) {
                // malformed line
                return -1;
            }
            seen_worker_prefix = 1;
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }

        // Now part should describe a command
        // We don't fully parse it yet, just detect if it's a valid command keyword
        if (starts_with(part, KW_MSLEEP)    ||
            starts_with(part, KW_INC)       ||
            starts_with(part, KW_DEC)       ||
            starts_with(part, KW_REPEAT)) {
            count++;
        } else {
            // Unknown token – treat as parse error
            return -1;
        }

        token = strtok_r(NULL, ";", &saveptr);
    }

    if (count <= 0) {
        // No commands found after "worker" – treat as error
        return -1;
    }

    // Allocate commands array
    basic_cmd_t *cmds = (basic_cmd_t *)malloc(count * sizeof(basic_cmd_t));
    if (!cmds) {
        return -1;
    }

    // -------- Second pass: fill commands[] and repeat_index --------
    seen_worker_prefix = 0;
    int idx = 0;

    saveptr = NULL;
    token   = strtok_r(buf2, ";", &saveptr);

    while (token != NULL && idx < count) {
        char *part = trim(token);

        if (*part == '\0') {
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }

        if (!seen_worker_prefix) {
            // Skip "worker <id>" again
            if (!starts_with(part, KW_WORKER)) {
                free(cmds);
                return -1;
            }
            seen_worker_prefix = 1;
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }

        basic_cmd_t cmd;
        if (parse_one_command(part, &cmd) != 0) {
            free(cmds);
            return -1;
        }

        cmds[idx] = cmd;
        if (cmd.type == CMD_REPEAT && out->repeat_index == -1) {
            out->repeat_index = idx;
        }

        idx++;
        token = strtok_r(NULL, ";", &saveptr);
    }

    // Final sanity check
    if (idx != count) {
        free(cmds);
        return -1;
    }

    out->commands     = cmds;
    out->num_commands = count;
    // repeat_index already set (or -1 if none)

    return 0;
}

void parser_free_parsed_job(parsed_job_t *job) {
    if (!job) return;

    if (job->commands) {
        free(job->commands);
        job->commands = NULL;
    }
    job->num_commands = 0;
    job->repeat_index = -1;
}
