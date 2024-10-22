/******************************************************************************
 *
 *  Copyright (C) 2017 Google Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef _DEBUG_NFCSNOOP_
#define _DEBUG_NFCSNOOP_

#include <stdint.h>
#include "nfc_target.h"
#include "nfc_types.h"
#include "nfc_hal_api.h"

#define NFCSNOOZ_CURRENT_VERSION 0x01

// The preamble is stored un-encrypted as the first part
// of the file.
typedef struct nfcsnooz_preamble_t {
  uint8_t version;
  uint64_t last_timestamp_ms;
} __attribute__((__packed__)) nfcsnooz_preamble_t;

// One header for each NCI packet
typedef struct nfcsnooz_header_t {
  uint16_t length;
  uint32_t delta_time_ms;
  uint8_t is_received;
} __attribute__((__packed__)) nfcsnooz_header_t;

// Initializes nfcsnoop memory logging and registers
void debug_nfcsnoop_init(void);

// Writes nfcsnoop data base64 encoded to fd
void debug_nfcsnoop_dump(int fd);
// capture the packet
void nfcsnoop_capture(const NFC_HDR* packet, bool is_received);

#endif /* _DEBUG_NFCSNOOP_ */
