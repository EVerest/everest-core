/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * cbv2g_json_wrapper.c - Main wrapper implementation
 */

#include "cbv2g_json_wrapper.h"
#include "converters.h"
#include <string.h>
#include <stdio.h>

/* Portable thread-local storage. Prefer the C11 keyword; fall back to the
 * GCC/Clang __thread extension for pre-C11 compilers. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#  define CBV2G_TLS _Thread_local
#elif defined(__GNUC__) || defined(__clang__)
#  define CBV2G_TLS __thread
#else
#  define CBV2G_TLS
#endif

/* Thread-local error message buffer */
static CBV2G_TLS char g_last_error[1024] = {0};

/* Internal function to set error message */
void set_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(g_last_error, sizeof(g_last_error), format, args);
    va_end(args);
}

const char* cbv2g_get_version(void) {
    return CBV2G_JSON_WRAPPER_VERSION;
}

const char* cbv2g_get_last_error(void) {
    return g_last_error;
}

void cbv2g_clear_error(void) {
    g_last_error[0] = '\0';
}

/* Determine protocol from namespace.
 * Only the App Handshake (SAP) protocol is dispatched in this PR.
 * Subsequent PRs extend this enum and switch with DIN, ISO 15118-2, and
 * ISO 15118-20 cases. */
typedef enum {
    PROTOCOL_UNKNOWN = 0,
    PROTOCOL_SAP
} protocol_t;

static protocol_t get_protocol(const char* ns) {
    if (ns == NULL) {
        return PROTOCOL_UNKNOWN;
    }

    /* Use strncmp + sizeof(literal) to bound reads on potentially
     * non-null-terminated input (CWE-126). */
    if (strncmp(ns, NS_SAP, sizeof(NS_SAP)) == 0) {
        return PROTOCOL_SAP;
    }

    return PROTOCOL_UNKNOWN;
}

int cbv2g_encode(const char* json_message,
                 const char* ns,
                 uint8_t* output_buffer,
                 size_t buffer_size,
                 size_t* output_length) {

    /* Validate parameters */
    if (json_message == NULL || ns == NULL ||
        output_buffer == NULL || output_length == NULL) {
        set_error("Invalid parameter: NULL pointer");
        return CBV2G_ERROR_INVALID_PARAM;
    }

    if (buffer_size == 0) {
        set_error("Invalid parameter: buffer_size is 0");
        return CBV2G_ERROR_INVALID_PARAM;
    }

    cbv2g_clear_error();

    /* Route to appropriate protocol encoder */
    protocol_t protocol = get_protocol(ns);

    switch (protocol) {
        case PROTOCOL_SAP:
            return apphand_encode(json_message, output_buffer, buffer_size, output_length);

        default:
            set_error("Unknown namespace: %s", ns);
            return CBV2G_ERROR_UNKNOWN_NAMESPACE;
    }
}

int cbv2g_decode(const uint8_t* exi_data,
                 size_t exi_length,
                 const char* ns,
                 char* output_json,
                 size_t buffer_size) {

    /* Validate parameters */
    if (exi_data == NULL || ns == NULL || output_json == NULL) {
        set_error("Invalid parameter: NULL pointer");
        return CBV2G_ERROR_INVALID_PARAM;
    }

    if (exi_length == 0 || buffer_size == 0) {
        set_error("Invalid parameter: length is 0");
        return CBV2G_ERROR_INVALID_PARAM;
    }

    cbv2g_clear_error();

    /* Route to appropriate protocol decoder */
    protocol_t protocol = get_protocol(ns);

    switch (protocol) {
        case PROTOCOL_SAP:
            return apphand_decode(exi_data, exi_length, output_json, buffer_size);

        default:
            set_error("Unknown namespace: %s", ns);
            return CBV2G_ERROR_UNKNOWN_NAMESPACE;
    }
}
