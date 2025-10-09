// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

/**
 * \file Test utilities used to determine that the used types
 * have the same sizes independent of the platforms that we
 * are using. Added to make sure that reinterpret_cast or other
 * types of cast will yield the same types across platform
 */
#pragma once

CB_STATIC_ASSERT(sizeof(AppUDPResponse) == 4, "Wrong AppUDPReponse type size!");
CB_STATIC_ASSERT(sizeof(CbType) == 1, "Wrong CB type size!");
CB_STATIC_ASSERT(sizeof(CbStructType) == 2, "Wrong CB type size!");
CB_STATIC_ASSERT((sizeof(CbFirmwareStart) == 16 + 1 && sizeof(CbFirmwareStart) <= CB_MAX_CB_STRUCT_SIZE),
                 "Wrong CB type size!");
CB_STATIC_ASSERT((sizeof(CbFirmwarePacket) == 1 + 2 + 2 + 1024 && sizeof(CbFirmwarePacket) <= CB_MAX_CB_STRUCT_SIZE),
                 "Wrong CB type size!");
CB_STATIC_ASSERT((sizeof(CbFirmwareEnd) == 4 + 1 + (128 + 1) && sizeof(CbFirmwareEnd) <= CB_MAX_CB_STRUCT_SIZE),
                 "Wrong CB type size!");
CB_STATIC_ASSERT((sizeof(CbHeartbeatPacket) == 118 && sizeof(CbHeartbeatPacket) <= CB_MAX_CB_STRUCT_SIZE),
                 "Wrong CB type size!");
