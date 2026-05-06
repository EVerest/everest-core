/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * json_utils.c - JSON parsing and generation utilities
 */

#include "json_utils.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>

/* Base64 encoding/decoding tables */
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int base64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

size_t base64_encode(const uint8_t* input, size_t input_len, char* output, size_t output_size) {
    size_t output_len = ((input_len + 2) / 3) * 4;

    if (output_size < output_len + 1) {
        return 0;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_len; i += 3, j += 4) {
        uint32_t octet_a = i < input_len ? input[i] : 0;
        uint32_t octet_b = i + 1 < input_len ? input[i + 1] : 0;
        uint32_t octet_c = i + 2 < input_len ? input[i + 2] : 0;

        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        output[j] = base64_chars[(triple >> 18) & 0x3F];
        output[j + 1] = base64_chars[(triple >> 12) & 0x3F];
        output[j + 2] = (i + 1 < input_len) ? base64_chars[(triple >> 6) & 0x3F] : '=';
        output[j + 3] = (i + 2 < input_len) ? base64_chars[triple & 0x3F] : '=';
    }

    output[j] = '\0';
    return j;
}

size_t base64_decode(const char* input, size_t input_len, uint8_t* output, size_t output_size) {
    if (input_len % 4 != 0) {
        return 0;
    }

    size_t output_len = input_len / 4 * 3;
    if (input[input_len - 1] == '=') output_len--;
    if (input[input_len - 2] == '=') output_len--;

    if (output_size < output_len) {
        return 0;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_len; i += 4) {
        int a = base64_decode_table[(unsigned char)input[i]];
        int b = base64_decode_table[(unsigned char)input[i + 1]];
        int c = base64_decode_table[(unsigned char)input[i + 2]];
        int d = base64_decode_table[(unsigned char)input[i + 3]];

        if (a < 0 || b < 0) return 0;

        uint32_t triple = (a << 18) + (b << 12);
        if (c >= 0) triple += (c << 6);
        if (d >= 0) triple += d;

        if (j < output_len) output[j++] = (triple >> 16) & 0xFF;
        if (j < output_len) output[j++] = (triple >> 8) & 0xFF;
        if (j < output_len) output[j++] = triple & 0xFF;
    }

    return output_len;
}

/* Hex encoding table */
static const char hex_chars[] = "0123456789abcdef";

size_t hex_encode(const uint8_t* input, size_t input_len, char* output, size_t output_size) {
    size_t output_len = input_len * 2;

    if (output_size < output_len + 1) {
        return 0;
    }

    for (size_t i = 0; i < input_len; i++) {
        output[i * 2] = hex_chars[(input[i] >> 4) & 0x0F];
        output[i * 2 + 1] = hex_chars[input[i] & 0x0F];
    }
    output[output_len] = '\0';

    return output_len;
}

static int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

size_t hex_decode(const char* input, size_t input_len, uint8_t* output, size_t output_size) {
    /* Hex string must have even length */
    if (input_len % 2 != 0) {
        return 0;
    }

    size_t output_len = input_len / 2;

    if (output_size < output_len) {
        return 0;
    }

    for (size_t i = 0; i < output_len; i++) {
        int high = hex_char_to_int(input[i * 2]);
        int low = hex_char_to_int(input[i * 2 + 1]);

        if (high < 0 || low < 0) {
            return 0;  /* Invalid hex character */
        }

        output[i] = (high << 4) | low;
    }

    return output_len;
}

/* Get string from JSON object, returns empty string if not found */
const char* json_get_string(cJSON* obj, const char* key) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (item && cJSON_IsString(item)) {
        return item->valuestring;
    }
    return "";
}

/* Get integer from JSON object, returns 0 if not found */
int json_get_int(cJSON* obj, const char* key) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (item && cJSON_IsNumber(item)) {
        return item->valueint;
    }
    return 0;
}

/* Get boolean from JSON object, returns 0 if not found */
int json_get_bool(cJSON* obj, const char* key) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (item) {
        return cJSON_IsTrue(item) ? 1 : 0;
    }
    return 0;
}

/* Check if key exists in JSON object */
int json_has_key(cJSON* obj, const char* key) {
    return cJSON_GetObjectItemCaseSensitive(obj, key) != NULL;
}

/* Get bytes from base64-encoded JSON string. The input length is bounded
 * by the maximum base64 encoding of the requested output buffer, which
 * prevents an unbounded read on a non-null-terminated string (CWE-126). */
size_t json_get_bytes(cJSON* obj, const char* key, uint8_t* output, size_t output_size) {
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (item && cJSON_IsString(item)) {
        size_t max_b64_len = ((output_size + 2) / 3) * 4;
        size_t vs_len = strnlen(item->valuestring, max_b64_len);
        return base64_decode(item->valuestring, vs_len, output, output_size);
    }
    return 0;
}

/* Add bytes as base64-encoded string to JSON object */
void json_add_bytes(cJSON* obj, const char* key, const uint8_t* data, size_t data_len) {
    char* encoded = malloc(((data_len + 2) / 3) * 4 + 1);
    if (encoded) {
        base64_encode(data, data_len, encoded, ((data_len + 2) / 3) * 4 + 1);
        cJSON_AddStringToObject(obj, key, encoded);
        free(encoded);
    }
}
