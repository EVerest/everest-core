// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "log.hpp"
#include <everest/logging.hpp> // for logging
#include <stdarg.h>            // for va_list, va_{start,end}()
#include <stdio.h>             // for v*printf()
#include <stdlib.h>            // for atoi()
#include <string.h>            // for strlen()
#include <sys/time.h>          // for gettimeofday()
#include <time.h>              // for strftime()

dloglevel_t minloglevel_current = DLOG_LEVEL_INFO;

static const char* debug_level_logstring_map[DLOG_LEVEL_NUMLEVELS] = {
    // tailing space, no need to add it later when printing
    // try to keep the strings almost same length, looks better
    "[(LOG)] ", "[ERROR] ", "[WARN]  ", "[INFO]  ", "[DEBUG] ", "[TRACE] "};

const char* debug_level_mqtt_string_map[DLOG_LEVEL_NUMLEVELS] = {"always", "error", "warning",
                                                                 "info",   "debug", "trace"};

// FIXME: inline?
void dlog_func(const dloglevel_t loglevel, const char* filename, const int linenumber, const char* functionname,
               const char* format, ...) {
    // fast exit
    if (loglevel > minloglevel_current) {
        return;
    }
    char* format_copy = NULL;
    FILE* outstream = stderr; // change output target here, if desired

    struct timeval debug_tval;
    struct tm tm;
    char log_datetimestamp[16];      // length due to format [00:00:00.000], rounded up to fit 32-bit alignment
    gettimeofday(&debug_tval, NULL); // ignore return value
    size_t offset =
        strftime(log_datetimestamp, sizeof(log_datetimestamp), "[%H:%M:%S", gmtime_r(&debug_tval.tv_sec, &tm));
    if (offset < 1) {
        // in our use of strftime(), this is an error
        return;
    }

    snprintf(log_datetimestamp + offset, sizeof(log_datetimestamp) - offset, ".%03ld] ", debug_tval.tv_usec / 1000);

    va_list args;
    va_start(args, format);

    // print the user given part
    // strip possible newline character from user-given string
    // FIXME: could be skipped
    if (format) {
        size_t formatlen = std::string(format).size();
        format_copy = static_cast<char*>(calloc(1, formatlen + 1)); // additional byte for terminating \0
        memcpy(format_copy, format, formatlen);
        if ((formatlen >= 1) && (format_copy[formatlen - 1] == '\n')) {
            format_copy[formatlen - 1] = '\0';
        }
    }
    char output[256];
    if (format_copy != NULL) {
        vsnprintf(output, sizeof(output), format_copy, args);
    }
    // force EOL
    fputs("\n", outstream);
    fflush(outstream);
    va_end(args);
    if (format_copy) {
        free(format_copy);
    }

    switch (loglevel) {
    case DLOG_LEVEL_ERROR:
        EVLOG_error << output;
        break;
    case DLOG_LEVEL_WARNING:
        EVLOG_warning << output;
        break;
    case DLOG_LEVEL_INFO:
        EVLOG_info << output;
        break;
    case DLOG_LEVEL_DEBUG:
        EVLOG_debug << output;
        break;
    case DLOG_LEVEL_TRACE:
        EVLOG_verbose << output;
        break;
    default:
        EVLOG_critical << "Unknown log level";
        break;
    }
}

void dlog_level_inc(void) {
    dloglevel_t minloglevel_new = (dloglevel_t)((int)minloglevel_current + 1);
    if (minloglevel_new == DLOG_LEVEL_NUMLEVELS) {
        // wrap to bottom, but not DLOG_LEVEL_ALWAYS
        minloglevel_new = DLOG_LEVEL_ERROR;
    }
    dlog_level_set(minloglevel_new);
}

void dlog_level_set(const dloglevel_t loglevel) {
    // no sanity checks currently
    const dloglevel_t minloglevel_old = minloglevel_current;
    dloglevel_t newloglevel = loglevel;
    if (newloglevel >= DLOG_LEVEL_NUMLEVELS) {
        // set something illegally high
        newloglevel = (dloglevel_t)(int)(DLOG_LEVEL_NUMLEVELS - 1);
    }
    if (newloglevel <= DLOG_LEVEL_ALWAYS) {
        // set something illegally low
        newloglevel = (dloglevel_t)(int)(DLOG_LEVEL_ALWAYS + 1);
    }
    if (newloglevel != minloglevel_current) {
        minloglevel_current = newloglevel;
        dlog(DLOG_LEVEL_ALWAYS, "switched log level from %d (\"%s\") to %d (\"%s\")", minloglevel_old,
             debug_level_logstring_map[minloglevel_old], newloglevel, debug_level_logstring_map[newloglevel]);
    }
}

dloglevel_t dlog_level_get(void) {
    return minloglevel_current;
}

static const char* dlog_level_get_string(const dloglevel_t loglevel) {
    if ((loglevel < 1) || loglevel >= DLOG_LEVEL_NUMLEVELS) {
        return "invalid_level";
    }
    return debug_level_mqtt_string_map[loglevel];
}
