/******************************************************************************
 *
 *  Copyright 2019 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 ******************************************************************************/
#pragma once

#define NFA_STATUS_READ_ONLY NCI_STATUS_READ_ONLY
#define NCI_STATUS_READ_ONLY 0xC4

/* below  Errors for  when P2P_ERROR_CONNECT_FAIL_EVT Trigger*/
/* Peer Doesn't support connection-oriented link  */
#define LLCP_STATUS_CO_LINK_NOT_SUPPORTED 3
/* SSAP is not Registered */
#define LLCP_STATUS_SSAP_NOT_REG 4
/* Service Name is too Long */
#define LLCP_STATUS_SN_TOO_LONG 5
/* Data link  MIU shall not be bigger than local link MIU */
#define LLCP_STATUS_INVALID_MIU 6
/* Pending Connecting request on this registered */
#define LLCP_STATUS_BUSY 7

/* Error Type when LLCP connect Failed*/
#define LLCP_ERROR_CONNECT_FAIL_EVT 0x01

enum {
  P2P_ERROR_INVALID_HANDLE_EVT =
      0x00,                    // Event for invalid ID which is not registered
  P2P_ERROR_CONNECT_FAIL_EVT,  // Event when LLCP Connection Failed
  P2P_ERROR_LINK_LOSS_EVT      // Event for lInk Loss.
};

enum {
  P2P_CONNECT_FAIL_CO_LINK_NOT_SUPPORTED =
      0x00,  // Peer Doesn't support connection-oriented link
  P2P_CONNECT_FAIL_SSAP_NOT_REG, /* SSAP is not Registered           */
  P2P_CONNECT_FAIL_SN_TOO_LONG,  /* Service Name is too LOng           */
  P2P_CONNECT_FAIL_INVALID_MIU, /* Data link  MIU shall not be bigger than local
                                   link MIU */
  P2P_CONNECT_FAIL_BUSY /* Pending Connecting request on this registered */
};

enum {
  P2P_LINK_LOSS_LOCAL_INITIATED = 0x00,
  P2P_LINK_LOSS_TIMEOUT,
  P2P_LINK_LOSS_REMOTE_INITIATED,
  P2P_LINK_LOSS_RF_LINK_LOSS_ERR
};
