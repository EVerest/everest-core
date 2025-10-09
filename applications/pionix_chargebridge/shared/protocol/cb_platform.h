// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

/**
 * \file compiler utilities and Pionix defines for 
 * cross-compiling
 */
#pragma once

#if defined(__cplusplus)
  #define CB_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
  #define CB_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#endif

#define CB_COMPILER_ATTR_PACK __attribute__((packed))

// Should be < MTU (defined as #define NX_DRIVER_ETHERNET_MTU 1514)
#define CB_MAX_UDP_PACKET_SIZE (1024 + 256)
// -128 since we might want some non-struct metadata
#define CB_MAX_CB_STRUCT_SIZE (CB_MAX_UDP_PACKET_SIZE - 128)

#define CB_MAX_STRING_SIZE 64

#define cb_string(name) int8_t name[CB_MAX_STRING_SIZE]
