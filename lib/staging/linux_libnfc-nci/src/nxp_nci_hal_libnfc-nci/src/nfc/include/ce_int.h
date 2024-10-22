/******************************************************************************
 *
 *  Copyright (C) 2009-2014 Broadcom Corporation
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

/******************************************************************************
 *
 *  This file contains the Near Field Communication (NFC) Card Emulation
 *  mode related internal function / definitions.
 *
 ******************************************************************************/

#ifndef CE_INT_H_
#define CE_INT_H_

#include "ce_api.h"

#if (CE_TEST_INCLUDED == FALSE)
#define CE_MIN_SUP_PROTO NCI_PROTOCOL_FELICA
#define CE_MAX_SUP_PROTO NCI_PROTOCOL_ISO4
#else
#define CE_MIN_SUP_PROTO NCI_PROTOCOL_TYPE1
#define CE_MAX_SUP_PROTO NCI_PROTOCOL_MIFARE
#endif

/* CE Type 3 Tag structures */

/* Type 3 Tag NDEF card-emulation */
typedef struct {
  bool initialized;
  uint8_t version; /* Ver: peer version */
  uint8_t
      nbr; /* NBr: number of blocks that can be read using one Check command */
  uint8_t nbw;    /* Nbw: number of blocks that can be written using one Update
                     command */
  uint16_t nmaxb; /* Nmaxb: maximum number of blocks available for NDEF data */
  uint8_t writef; /* WriteFlag: 00h if writing data finished; 0Fh if writing
                     data in progress */
  uint8_t
      rwflag; /* RWFlag: 00h NDEF is read-only; 01h if read/write available */
  uint32_t ln;
  uint8_t* p_buf; /* Current contents for READs */

  /* Scratch NDEF buffer (for update NDEF commands) */
  uint8_t scratch_writef;
  uint32_t scratch_ln;
  uint8_t* p_scratch_buf; /* Scratch buffer for WRITE/readback */
} tCE_T3T_NDEF_INFO;

/* Type 3 Tag current command processing */
typedef struct {
  uint16_t service_code_list[T3T_MSG_SERVICE_LIST_MAX];
  uint8_t* p_block_list_start;
  uint8_t* p_block_data_start;
  uint8_t num_services;
  uint8_t num_blocks;
} tCE_T3T_CUR_CMD;

/* Type 3 Tag control blcok */
typedef struct {
  uint8_t state;
  uint16_t system_code;
  uint8_t local_nfcid2[NCI_RF_F_UID_LEN];
  uint8_t local_pmm[NCI_T3T_PMM_LEN];
  tCE_T3T_NDEF_INFO ndef_info;
  tCE_T3T_CUR_CMD cur_cmd;
} tCE_T3T_MEM;

/* CE Type 4 Tag control blocks */
typedef struct {
  uint8_t aid_len;
  uint8_t aid[NFC_MAX_AID_LEN];
  tCE_CBACK* p_cback;
} tCE_T4T_REG_AID; /* registered AID table */

typedef struct {
  TIMER_LIST_ENT timer; /* timeout for update file              */
  uint8_t cc_file[T4T_FC_TLV_OFFSET_IN_CC + T4T_FILE_CONTROL_TLV_SIZE];
  uint8_t* p_ndef_msg;    /* storage of NDEF message              */
  uint16_t nlen;          /* current size of NDEF message         */
  uint16_t max_file_size; /* size of storage + 2 bytes for NLEN   */
  uint8_t* p_scratch_buf; /* temp storage of NDEF message for update */

/* T4T CE App is selected       */
#define CE_T4T_STATUS_T4T_APP_SELECTED 0x01
/* Registered AID is selected   */
#define CE_T4T_STATUS_REG_AID_SELECTED 0x02
/* CC file is selected          */
#define CE_T4T_STATUS_CC_FILE_SELECTED 0x04
/* NDEF file is selected        */
#define CE_T4T_STATUS_NDEF_SELECTED 0x08
/* NDEF is read-only            */
#define CE_T4T_STATUS_NDEF_FILE_READ_ONLY 0x10
/* NDEF is updating             */
#define CE_T4T_STATUS_NDEF_FILE_UPDATING 0x20
/* Wildcard AID selected        */
#define CE_T4T_STATUS_WILDCARD_AID_SELECTED 0x40

  uint8_t status;

  tCE_CBACK* p_wildcard_aid_cback; /* registered wildcard AID callback */
  tCE_T4T_REG_AID reg_aid[CE_T4T_MAX_REG_AID]; /* registered AID table */
  uint8_t selected_aid_idx;
} tCE_T4T_MEM;

/* CE memory control blocks */
typedef struct {
  tCE_T3T_MEM t3t;
  tCE_T4T_MEM t4t;
} tCE_MEM;

/* CE control blocks */
typedef struct {
  tCE_MEM mem;
  tCE_CBACK* p_cback;
  uint8_t* p_ndef;   /* the memory starting from NDEF */
  uint16_t ndef_max; /* max size of p_ndef */
  uint16_t ndef_cur; /* current size of p_ndef */
  tNFC_RF_TECH tech;
  uint8_t  trace_level;
} tCE_CB;

/*
** CE Type 4 Tag Definition
*/

/* Max data size using a single ReadBinary. 2 bytes are for status bytes */
#define CE_T4T_MAX_LE                                          \
  (NFC_CE_POOL_BUF_SIZE - NFC_HDR_SIZE - NCI_MSG_OFFSET_SIZE - \
   NCI_DATA_HDR_SIZE - T4T_RSP_STATUS_WORDS_SIZE)

/* Max data size using a single UpdateBinary. 6 bytes are for CLA, INS, P1, P2,
 * Lc */
#define CE_T4T_MAX_LC                                        \
  (NFC_CE_POOL_BUF_SIZE - NFC_HDR_SIZE - NCI_DATA_HDR_SIZE - \
   T4T_CMD_MAX_HDR_SIZE)

/*****************************************************************************
**  EXTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

/* Global NFC data */
extern tCE_CB ce_cb;

extern void ce_init(void);

/* ce_t3t internal functions */
void ce_t3t_init(void);
tNFC_STATUS ce_select_t3t(uint16_t system_code,
                          uint8_t nfcid2[NCI_RF_F_UID_LEN]);

/* ce_t4t internal functions */
extern tNFC_STATUS ce_select_t4t(void);
extern void ce_t4t_process_timeout(TIMER_LIST_ENT* p_tle);

#endif /* CE_INT_H_ */
