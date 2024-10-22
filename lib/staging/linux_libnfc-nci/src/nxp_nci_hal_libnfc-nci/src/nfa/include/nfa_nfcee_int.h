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
 ******************************************************************************/
#pragma once
#include "nfa_ee_int.h"
#include "nfa_sys.h"
using namespace std;

#define CC_FILE_ID 0xE103
#define NDEF_FILE_ID 0xE104
#define PROP_FILE_ID_1 0xE105
#define PROP_FILE_ID_2 0xE106
#define PROP_FILE_ID_3 0xE107
#define PROP_FILE_ID_4 0xE108

#define T4TNFCEE_TARGET_HANDLE 0x10
#define T4TNFCEE_SIZEOF_LEN_BYTES 0x02
#define T4TNFCEE_SIZEOF_STATUS_BYTES 0x02

/*CLA + INS + P1 + P2 + LC*/
#define CAPDU_TL 0x05
#define RW_T4TNFCEE_DATA_PER_WRITE (T4T_MAX_LENGTH_LC - CAPDU_TL)

/*
POWER_STATE:
bit pos 0 = Switch On
bit pos 1 = Switch Off
bit pos 2 = Battery Off
bit pos 3 = Screen On lock
bit pos 4 = Screen off unlock
bit pos 5 = Screen Off lock
*/
#define T4TNFCEE_AID_POWER_STATE 0x3B

/* Event to notify T4T NFCEE Detection complete*/
#define NFA_T4TNFCEE_EVT 40
/* Event to notify NDEF T4TNFCEE READ complete*/
#define NFA_T4TNFCEE_READ_CPLT_EVT 41
/* Event to notify NDEF T4TNFCEE WRITE complete*/
#define NFA_T4TNFCEE_WRITE_CPLT_EVT 42
/* Event to notify NDEF T4TNFCEE CLEAR complete*/
#define NFA_T4TNFCEE_CLEAR_CPLT_EVT 43

#define T4T_NFCEE_READ_ALLOWED 0x00
#define T4T_NFCEE_WRITE_NOT_ALLOWED 0xFF

/*Staus codes*/
#define NFA_T4T_STATUS_INVALID_FILE_ID   0x05

typedef struct {
  uint16_t capacity;
  uint8_t read_access;
  uint8_t write_access;
} tNFA_T4TNFCEE_FILE_INFO;

enum {
  NFA_T4TNFCEE_OP_OPEN_CONNECTION,
  NFA_T4TNFCEE_OP_READ,
  NFA_T4TNFCEE_OP_WRITE,
  NFA_T4TNFCEE_OP_CLOSE_CONNECTION,
  NFA_T4TNFCEE_OP_CLEAR,
  NFA_T4TNFCEE_OP_MAX
};
typedef uint8_t tNFA_T4TNFCEE_OP;

typedef struct {
  uint32_t len;
  uint8_t* p_data;
} tNFA_T4TNFCEE_OP_PARAMS_WRITE;

/* NDEF EE  events */
enum {
  NFA_T4TNFCEE_OP_REQUEST_EVT = NFA_SYS_EVT_START(NFA_ID_T4TNFCEE),
  NFA_T4TNFCEE_MAX_EVT
};

/* data type for NFA_T4TNFCEE_op_req_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_T4TNFCEE_OP op; /* NFA T4TNFCEE operation */
  uint8_t* p_fileId;
  tNFA_T4TNFCEE_OP_PARAMS_WRITE write;
} tNFA_T4TNFCEE_OPERATION;

/* union of all data types */
typedef union {
  /* GKI event buffer header */
  NFC_HDR hdr;
  tNFA_T4TNFCEE_OPERATION op_req;
} tNFA_T4TNFCEE_MSG;

typedef enum {
  /* NFA T4TNFCEE states */
  NFA_T4TNFCEE_STATE_DISABLED = 0x00, /* T4TNFCEE is disabled  */
  NFA_T4TNFCEE_STATE_TRY_ENABLE,
  NFA_T4TNFCEE_STATE_INITIALIZED,  /* T4TNFCEE is waiting to handle api commands
                                    */
  NFA_T4TNFCEE_STATE_CONNECTED,    /* T4TNFCEE is in open sequence */
  NFA_T4TNFCEE_STATE_DISCONNECTED, /* T4TNFCEE is in closing sequence */
  NFA_T4TNFCEE_STATE_OPEN_FAILED   /* T4TNFCEE OPEN Failed */
} tNFA_T4TNFCEE_STATE;

typedef enum {
  PROP_DISABLED = 0x00,
  WAIT_SELECT_APPLICATION,
  WAIT_SELECT_CC,
  WAIT_READ_CC_DATA_LEN,
  WAIT_READ_CC_FILE,
  WAIT_SELECT_FILE,
  WAIT_READ_DATA_LEN,
  WAIT_READ_FILE,
  WAIT_RESET_NLEN,
  WAIT_WRITE,
  WAIT_WRITE_COMPLETE,
  WAIT_UPDATE_NLEN,
  WAIT_CLEAR_NDEF_DATA,
  OP_COMPLETE = 0x00
} tNFA_T4TNFCEE_RW_STATE;
/* NFA T4TNFCEE control block */
typedef struct {
  tNFA_STATUS status;
  tNFA_T4TNFCEE_STATE t4tnfcee_state;
  tNFA_T4TNFCEE_OP cur_op; /* Current operation */
  tNFA_T4TNFCEE_RW_STATE prop_rw_state;
  tNFA_T4TNFCEE_MSG* p_pending_msg;
  uint8_t* p_dataBuf;
  uint16_t cur_fileId;
  uint16_t rd_offset;
  uint32_t dataLen;
} tNFA_T4TNFCEE_CB;
extern tNFA_T4TNFCEE_CB nfa_t4tnfcee_cb;

/* type definition for action functions */
typedef bool (*tNFA_T4TNFCEE_ACTION)(tNFA_T4TNFCEE_MSG* p_data);

bool nfa_t4tnfcee_handle_op_req(tNFA_T4TNFCEE_MSG* p_data);
bool nfa_t4tnfcee_handle_event(NFC_HDR* p_msg);
void nfa_t4tnfcee_free_rx_buf(void);
bool nfa_t4tnfcee_is_enabled(void);
bool nfa_t4tnfcee_is_processing(void);
void nfa_t4tnfcee_set_ee_cback(tNFA_EE_ECB* p_ecb);