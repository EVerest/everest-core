/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * cbv2g_json_wrapper.h - JSON-based API wrapper for libcbv2g
 *
 * This library provides a JSON-based interface to libcbv2g's EXI encoding/decoding
 * functionality, designed to replace the Java-based EXIficient codec in Josev.
 */

#ifndef CBV2G_JSON_WRAPPER_H
#define CBV2G_JSON_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Public-API export macro. The shared library is compiled with
 * -fvisibility=hidden so that statically-linked third-party symbols
 * (libcbv2g, cJSON) stay private; only functions tagged CBV2G_API are
 * exported. */
#if defined(_WIN32) || defined(__CYGWIN__)
#  define CBV2G_API __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#  define CBV2G_API __attribute__((visibility("default")))
#else
#  define CBV2G_API
#endif

/* Version information */
#define CBV2G_JSON_WRAPPER_VERSION_MAJOR 1
#define CBV2G_JSON_WRAPPER_VERSION_MINOR 0
#define CBV2G_JSON_WRAPPER_VERSION_PATCH 0
#define CBV2G_JSON_WRAPPER_VERSION "1.0.0"

/* Error codes */
#define CBV2G_SUCCESS                    0
#define CBV2G_ERROR_INVALID_PARAM       -1
#define CBV2G_ERROR_BUFFER_TOO_SMALL    -2
#define CBV2G_ERROR_ENCODING_FAILED     -3
#define CBV2G_ERROR_DECODING_FAILED     -4
#define CBV2G_ERROR_UNKNOWN_NAMESPACE   -5
#define CBV2G_ERROR_JSON_PARSE          -6
#define CBV2G_ERROR_JSON_GENERATE       -7
#define CBV2G_ERROR_UNKNOWN_MESSAGE     -8
#define CBV2G_ERROR_INTERNAL            -9

/* Namespace constants matching Josev's Namespace enum.
 * Additional namespaces are added in subsequent PRs together with their
 * converter implementations. */
#define NS_SAP                  "urn:iso:15118:2:2010:AppProtocol"
#define NS_DIN_MSG_DEF          "urn:din:70121:2012:MsgDef"

/**
 * @brief Encode a JSON message to EXI format
 *
 * @param json_message  JSON string containing the message to encode
 * @param ns            Namespace string identifying the protocol/schema.
 *                      Named `ns` (not `namespace`) so the header is safe
 *                      to include from C++ where `namespace` is reserved.
 * @param output_buffer Buffer to receive the EXI-encoded bytes
 * @param buffer_size   Size of the output buffer
 * @param output_length Pointer to receive the actual length of encoded data
 *
 * @return CBV2G_SUCCESS on success, negative error code on failure
 */
CBV2G_API int cbv2g_encode(const char* json_message,
                           const char* ns,
                           uint8_t* output_buffer,
                           size_t buffer_size,
                           size_t* output_length);

/**
 * @brief Decode EXI data to JSON format
 *
 * @param exi_data      EXI-encoded byte array
 * @param exi_length    Length of the EXI data
 * @param ns            Namespace string identifying the protocol/schema.
 *                      Named `ns` to keep the header C++-includable.
 * @param output_json   Buffer to receive the JSON string
 * @param buffer_size   Size of the output buffer
 *
 * @return CBV2G_SUCCESS on success, negative error code on failure
 */
CBV2G_API int cbv2g_decode(const uint8_t* exi_data,
                           size_t exi_length,
                           const char* ns,
                           char* output_json,
                           size_t buffer_size);

/**
 * @brief Get the version string of this library
 *
 * @return Version string (e.g., "1.0.0")
 */
CBV2G_API const char* cbv2g_get_version(void);

/**
 * @brief Get the last error message
 *
 * @return Error message string, or empty string if no error
 */
CBV2G_API const char* cbv2g_get_last_error(void);

/**
 * @brief Clear the last error message
 */
CBV2G_API void cbv2g_clear_error(void);

#ifdef __cplusplus
}
#endif

#endif /* CBV2G_JSON_WRAPPER_H */
