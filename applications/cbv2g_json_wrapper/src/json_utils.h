/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * json_utils.h - JSON parsing and generation utilities
 */

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "cJSON.h"

/* Base64 encoding/decoding */
size_t base64_encode(const uint8_t* input, size_t input_len, char* output, size_t output_size);
size_t base64_decode(const char* input, size_t input_len, uint8_t* output, size_t output_size);

/* Hex encoding/decoding (for hexBinary XSD types) */
size_t hex_encode(const uint8_t* input, size_t input_len, char* output, size_t output_size);
size_t hex_decode(const char* input, size_t input_len, uint8_t* output, size_t output_size);

/* JSON helper functions */
const char* json_get_string(cJSON* obj, const char* key);
int json_get_int(cJSON* obj, const char* key);
int json_get_bool(cJSON* obj, const char* key);
int json_has_key(cJSON* obj, const char* key);
size_t json_get_bytes(cJSON* obj, const char* key, uint8_t* output, size_t output_size);
void json_add_bytes(cJSON* obj, const char* key, const uint8_t* data, size_t data_len);

#endif /* JSON_UTILS_H */
