/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * converters.h - Internal header for protocol converters
 */

#ifndef CONVERTERS_H
#define CONVERTERS_H

#include "cbv2g_json_wrapper.h"
#include <stdarg.h>

/* Error handling */
void set_error(const char* format, ...);

/* App Handshake converter */
int apphand_encode(const char* json, uint8_t* out, size_t out_size, size_t* out_len);
int apphand_decode(const uint8_t* exi, size_t exi_len, char* out, size_t out_size);

#endif /* CONVERTERS_H */
