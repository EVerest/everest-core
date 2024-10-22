/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
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
 *  This file contains the implementation for Type 4 tag in Reader/Writer
 *  mode.
 *
 ******************************************************************************/
#include <log/log.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"

#if (NXP_EXTNS == TRUE)
#include "nfa_rw_int.h"
#include "nfa_nfcee_int.h"
#endif

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* main state */
/* T4T is not activated                 */
#define RW_T4T_STATE_NOT_ACTIVATED 0x00
/* waiting for upper layer API          */
#define RW_T4T_STATE_IDLE 0x01
/* performing NDEF detection precedure  */
#define RW_T4T_STATE_DETECT_NDEF 0x02
/* performing read NDEF procedure       */
#define RW_T4T_STATE_READ_NDEF 0x03
/* performing update NDEF procedure     */
#define RW_T4T_STATE_UPDATE_NDEF 0x04
/* checking presence of tag             */
#define RW_T4T_STATE_PRESENCE_CHECK 0x05
/* convert tag to read only             */
#define RW_T4T_STATE_SET_READ_ONLY 0x06

/* performing NDEF format               */
#define RW_T4T_STATE_NDEF_FORMAT 0x07

/* sub state */
/* waiting for response of selecting AID    */
#define RW_T4T_SUBSTATE_WAIT_SELECT_APP 0x00
/* waiting for response of selecting CC     */
#define RW_T4T_SUBSTATE_WAIT_SELECT_CC 0x01
/* waiting for response of reading CC       */
#define RW_T4T_SUBSTATE_WAIT_CC_FILE 0x02
/* waiting for response of selecting NDEF   */
#define RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE 0x03
/* waiting for response of reading NLEN     */
#define RW_T4T_SUBSTATE_WAIT_READ_NLEN 0x04
/* waiting for response of reading file     */
#define RW_T4T_SUBSTATE_WAIT_READ_RESP 0x05
/* waiting for response of updating file    */
#define RW_T4T_SUBSTATE_WAIT_UPDATE_RESP 0x06
/* waiting for response of updating NLEN    */
#define RW_T4T_SUBSTATE_WAIT_UPDATE_NLEN 0x07
/* waiting for response of updating CC      */
#define RW_T4T_SUBSTATE_WAIT_UPDATE_CC 0x08

#define RW_T4T_SUBSTATE_WAIT_GET_HW_VERSION 0x09
#define RW_T4T_SUBSTATE_WAIT_GET_SW_VERSION 0x0A
#define RW_T4T_SUBSTATE_WAIT_GET_UID 0x0B
#define RW_T4T_SUBSTATE_WAIT_CREATE_APP 0x0C
#define RW_T4T_SUBSTATE_WAIT_CREATE_CC 0x0D
#define RW_T4T_SUBSTATE_WAIT_CREATE_NDEF 0x0E
#define RW_T4T_SUBSTATE_WAIT_WRITE_CC 0x0F
#define RW_T4T_SUBSTATE_WAIT_WRITE_NDEF 0x10

static std::string rw_t4t_get_state_name(uint8_t state);
static std::string rw_t4t_get_sub_state_name(uint8_t sub_state);

static bool rw_t4t_send_to_lower(NFC_HDR* p_c_apdu);
static bool rw_t4t_select_file(uint16_t file_id);
static bool rw_t4t_read_file(uint16_t offset, uint16_t length,
                             bool is_continue);
static bool rw_t4t_update_nlen(uint16_t ndef_len);
static bool rw_t4t_update_file(void);
static bool rw_t4t_update_cc_to_readonly(void);
static bool rw_t4t_select_application(uint8_t version);
static bool rw_t4t_validate_cc_file(void);

static bool rw_t4t_get_hw_version(void);
static bool rw_t4t_get_sw_version(void);
static bool rw_t4t_create_app(void);
static bool rw_t4t_select_app(void);
static bool rw_t4t_create_ccfile(void);
static bool rw_t4t_create_ndef(void);
static bool rw_t4t_write_cc(void);
static bool rw_t4t_write_ndef(void);
static void rw_t4t_handle_error(tNFC_STATUS status, uint8_t sw1, uint8_t sw2);
static void rw_t4t_sm_detect_ndef(NFC_HDR* p_r_apdu);
static void rw_t4t_sm_read_ndef(NFC_HDR* p_r_apdu);
static void rw_t4t_sm_update_ndef(NFC_HDR* p_r_apdu);
static void rw_t4t_sm_set_readonly(NFC_HDR* p_r_apdu);
static void rw_t4t_data_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data);
static void rw_t4t_sm_ndef_format(NFC_HDR* p_r_apdu);

/*******************************************************************************
**
** Function         rw_t4t_send_to_lower
**
** Description      Send C-APDU to lower layer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_send_to_lower(NFC_HDR* p_c_apdu) {
  uint8_t conn_id = NFC_RF_CONN_ID;
#if (NXP_EXTNS == TRUE)
  if (nfa_t4tnfcee_is_processing()) conn_id = NFC_T4TNFCEE_CONN_ID;
#endif
  if (NFC_SendData(conn_id, p_c_apdu) != NFC_STATUS_OK) {
    LOG(ERROR) << StringPrintf("failed");
    return false;
  }

  nfc_start_quick_timer(&rw_cb.tcb.t4t.timer, NFC_TTYPE_RW_T4T_RESPONSE,
                        (RW_T4T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_get_hw_version
**
** Description      Send get hw version cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_get_hw_version(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_GET_HW_VERSION);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_FIELD(p, 0x00);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_get_sw_version
**
** Description      Send get sw version cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_get_sw_version(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_ADDI_FRAME_RESP);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_FIELD(p, 0x00);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_update_version_details
**
** Description      Updates the size of the card
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_update_version_details(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t* p;
  uint16_t major_version, minor_version;

  if (p_r_apdu->len < T4T_DES_GET_VERSION_LEN) {
    LOG(ERROR) << StringPrintf("%s incorrect p_r_apdu length", __func__);
    android_errorWriteLog(0x534e4554, "120865977");
    return false;
  }

  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  major_version = *(p + 3);
  minor_version = *(p + 4);

  if ((T4T_DESEV0_MAJOR_VERSION == major_version) &&
      (T4T_DESEV0_MINOR_VERSION == minor_version)) {
    p_t4t->card_size = 0xEDE;
  } else if (major_version >= T4T_DESEV1_MAJOR_VERSION) {
    p_t4t->card_type = T4T_TYPE_DESFIRE_EV1;
    switch (*(p + 5)) {
      case T4T_SIZE_IDENTIFIER_2K:
        p_t4t->card_size = 2048;
        break;
      case T4T_SIZE_IDENTIFIER_4K:
        p_t4t->card_size = 4096;
        break;
      case T4T_SIZE_IDENTIFIER_8K:
        p_t4t->card_size = 7680;
        break;
      default:
        return false;
    }
  } else {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_get_uid_details
**
** Description      Send get uid cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_get_uid_details(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_ADDI_FRAME_RESP);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_FIELD(p, 0x00);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_create_app
**
** Description      Send create application cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_create_app(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;
  uint8_t df_name[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_CREATE_AID);
  UINT16_TO_BE_STREAM(p, 0x0000);
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    UINT8_TO_BE_STREAM(p, (T4T_CMD_MAX_HDR_SIZE + sizeof(df_name) + 2));
    UINT24_TO_BE_STREAM(p, T4T_DES_EV1_NFC_APP_ID);
    UINT16_TO_BE_STREAM(p, 0x0F21); /*Key settings and no.of keys */
    UINT16_TO_BE_STREAM(p, 0x05E1); /* ISO file ID */
    ARRAY_TO_BE_STREAM(p, df_name, (int)sizeof(df_name)); /*DF file name */
    UINT8_TO_BE_STREAM(p, 0x00);                          /* Le */
    p_c_apdu->len = 20;
  } else {
    UINT8_TO_BE_STREAM(p, T4T_CMD_MAX_HDR_SIZE);
    UINT24_TO_BE_STREAM(p, T4T_DES_EV0_NFC_APP_ID);
    UINT16_TO_BE_STREAM(p, 0x0F01); /*Key settings and no.of keys */
    UINT8_TO_BE_STREAM(p, 0x00);    /* Le */
    p_c_apdu->len = 11;
  }

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_select_app
**
** Description      Select application cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_select_app(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_SELECT_APP);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_STREAM(p, 0x03); /* Lc: length of wrapped data */
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    UINT24_TO_BE_STREAM(p, T4T_DES_EV1_NFC_APP_ID);
  } else {
    UINT24_TO_BE_STREAM(p, T4T_DES_EV0_NFC_APP_ID);
  }

  UINT8_TO_BE_STREAM(p, 0x00); /* Le */

  p_c_apdu->len = 9;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_create_ccfile
**
** Description      create capability container file cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_create_ccfile(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_CREATE_DATAFILE);
  UINT16_TO_BE_STREAM(p, 0x0000);
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    UINT8_TO_BE_STREAM(p, 0x09);    /* Lc: length of wrapped data */
    UINT8_TO_BE_STREAM(p, 0x01);    /* EV1 CC file id             */
    UINT16_TO_BE_STREAM(p, 0x03E1); /* ISO file id                */
  } else {
    UINT8_TO_BE_STREAM(p, 0x07); /* Lc: length of wrapped data */
    UINT8_TO_BE_STREAM(p, 0x03); /* DESFire CC file id         */
  }

  UINT8_TO_BE_STREAM(p, 0x00);      /* COMM settings              */
  UINT16_TO_BE_STREAM(p, 0xEEEE);   /* Access rights              */
  UINT24_TO_BE_STREAM(p, 0x0F0000); /* Set file size              */
  UINT8_TO_BE_STREAM(p, 0x00);      /* Le                         */

  p_c_apdu->len = (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) ? 15 : 13;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_create_ndef
**
** Description      creates an ndef file cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_create_ndef(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_CREATE_DATAFILE);
  UINT16_TO_BE_STREAM(p, 0x0000);
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    UINT8_TO_BE_STREAM(p, 0x09);    /* Lc: length of wrapped data */
    UINT8_TO_BE_STREAM(p, 0x02);    /* DESFEv1 NDEF file id       */
    UINT16_TO_BE_STREAM(p, 0x04E1); /* ISO file id                */
  } else {
    UINT8_TO_BE_STREAM(p, 0x07);
    UINT8_TO_BE_STREAM(p, 0x04); /* DESF4 NDEF file id        */
  }

  UINT8_TO_BE_STREAM(p, 0x00);    /* COMM settings              */
  UINT16_TO_BE_STREAM(p, 0xEEEE); /* Access rights              */
  UINT16_TO_STREAM(p, p_t4t->card_size);
  UINT8_TO_BE_STREAM(p, 0x00); /* Set card size              */
  UINT8_TO_BE_STREAM(p, 0x00); /* Le                         */

  p_c_apdu->len = (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) ? 15 : 13;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_write_cc
**
** Description      sends write cc file cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_write_cc(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;
  uint8_t CCFileBytes[] = {0x00, 0x0F, 0x10, 0x00, 0x3B, 0x00, 0x34, 0x04,
                           0x06, 0xE1, 0x04, 0x04, 0x00, 0x00, 0x00};

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_WRITE);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_STREAM(p, 0x16); /* Lc: length of wrapped data  */
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    CCFileBytes[2] = 0x20;
    CCFileBytes[11] = p_t4t->card_size >> 8;
    CCFileBytes[12] = (uint8_t)p_t4t->card_size;
    UINT8_TO_BE_STREAM(p, 0x01); /* CC file id                  */
  } else {
    UINT8_TO_BE_STREAM(p, 0x03);
  }

  UINT24_TO_BE_STREAM(p, 0x000000); /* Set the offset              */
  UINT24_TO_BE_STREAM(p, 0x0F0000); /* Set available length        */
  ARRAY_TO_BE_STREAM(p, CCFileBytes, (int)sizeof(CCFileBytes));
  UINT8_TO_BE_STREAM(p, 0x00); /* Le                         */

  p_c_apdu->len = 28;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_write_ndef
**
** Description      sends write ndef file cmd to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_write_ndef(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_DES_WRITE);
  UINT16_TO_BE_STREAM(p, 0x0000);
  UINT8_TO_BE_STREAM(p, 0x09); /* Lc: length of wrapped data  */
  if (p_t4t->card_type == T4T_TYPE_DESFIRE_EV1) {
    UINT8_TO_BE_STREAM(p, 0x02); /* DESFEv1 Ndef file id        */
  } else {
    UINT8_TO_BE_STREAM(p, 0x04);
  }

  UINT24_TO_BE_STREAM(p, 0x000000); /* Set the offset              */
  UINT24_TO_BE_STREAM(p, 0x020000); /* Set available length        */
  UINT16_TO_BE_STREAM(p, 0x0000);   /* Ndef file bytes             */
  UINT8_TO_BE_STREAM(p, 0x00);      /* Le                          */

  p_c_apdu->len = 15;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_select_file
**
** Description      Send Select Command (by File ID) to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_select_file(uint16_t file_id) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("File ID:0x%04X", file_id);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_SELECT);
  UINT8_TO_BE_STREAM(p, T4T_CMD_P1_SELECT_BY_FILE_ID);

  /* if current version mapping is V2.0 */
  if (rw_cb.tcb.t4t.version == T4T_VERSION_2_0) {
    UINT8_TO_BE_STREAM(p, T4T_CMD_P2_FIRST_OR_ONLY_0CH);
  } else /* version 1.0 */
  {
    UINT8_TO_BE_STREAM(p, T4T_CMD_P2_FIRST_OR_ONLY_00H);
  }

  UINT8_TO_BE_STREAM(p, T4T_FILE_ID_SIZE);
  UINT16_TO_BE_STREAM(p, file_id);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + T4T_FILE_ID_SIZE;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_read_file
**
** Description      Send ReadBinary Command to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_read_file(uint16_t offset, uint16_t length,
                             bool is_continue) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "offset:%d, length:%d, is_continue:%d, ", offset, length, is_continue);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  /* if this is the first reading */
  if (is_continue == false) {
    /* initialise starting offset and total length */
    /* these will be updated when receiving response */
    p_t4t->rw_offset = offset;
    p_t4t->rw_length = length;
  }

  /* adjust reading length if payload is bigger than max size per single command
   */
  if (length > p_t4t->max_read_size) {
    length = (uint8_t)(p_t4t->max_read_size);
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, (T4T_CMD_CLASS | rw_cb.tcb.t4t.channel));
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_READ_BINARY);
  UINT16_TO_BE_STREAM(p, offset);
  UINT8_TO_BE_STREAM(p, length); /* Le */

  p_c_apdu->len = T4T_CMD_MIN_HDR_SIZE + 1; /* adding Le */

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_update_nlen
**
** Description      Send UpdateBinary Command to update NLEN to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_update_nlen(uint16_t ndef_len) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("NLEN:%d", ndef_len);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_UPDATE_BINARY);
  UINT16_TO_BE_STREAM(p, 0x0000); /* offset for NLEN */
  UINT8_TO_BE_STREAM(p, T4T_FILE_LENGTH_SIZE);
  UINT16_TO_BE_STREAM(p, ndef_len);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + T4T_FILE_LENGTH_SIZE;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_update_file
**
** Description      Send UpdateBinary Command to peer
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_update_file(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_c_apdu;
  uint8_t* p;
  uint16_t length;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "rw_offset:%d, rw_length:%d", p_t4t->rw_offset, p_t4t->rw_length);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  /* try to send all of remaining data */
  length = p_t4t->rw_length;

  /* adjust updating length if payload is bigger than max size per single
   * command */
  if (length > p_t4t->max_update_size) {
    length = (uint8_t)(p_t4t->max_update_size);
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_UPDATE_BINARY);
  UINT16_TO_BE_STREAM(p, p_t4t->rw_offset);
  UINT8_TO_BE_STREAM(p, length);

  memcpy(p, p_t4t->p_update_data, length);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + length;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  /* adjust offset, length and pointer for remaining data */
  p_t4t->rw_offset += length;
  p_t4t->rw_length -= length;
  p_t4t->p_update_data += length;

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_update_cc_to_readonly
**
** Description      Send UpdateBinary Command for changing Write access
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_update_cc_to_readonly(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Remove Write access from CC");

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  /* Add Command Header */
  UINT8_TO_BE_STREAM(p, T4T_CMD_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_UPDATE_BINARY);
  UINT16_TO_BE_STREAM(
      p, (T4T_FC_TLV_OFFSET_IN_CC +
          T4T_FC_WRITE_ACCESS_OFFSET_IN_TLV)); /* Offset for Read Write access
                                                  byte of CC */
  UINT8_TO_BE_STREAM(
      p, 1); /* Length of write access field in cc interms of bytes */

  /* Remove Write access */
  UINT8_TO_BE_STREAM(p, T4T_FC_NO_WRITE_ACCESS);

  p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + 1;

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_select_application
**
** Description      Select Application
**
**                  NDEF Tag Application Select - C-APDU
**
**                        CLA INS P1 P2 Lc Data(AID)      Le
**                  V1.0: 00  A4  04 00 07 D2760000850100 -
**                  V2.0: 00  A4  04 00 07 D2760000850101 00
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_select_application(uint8_t version) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("version:0x%X", version);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, T4T_CMD_CLASS);
  UINT8_TO_BE_STREAM(p, T4T_CMD_INS_SELECT);
  UINT8_TO_BE_STREAM(p, T4T_CMD_P1_SELECT_BY_NAME);
  UINT8_TO_BE_STREAM(p, T4T_CMD_P2_FIRST_OR_ONLY_00H);

  if (version == T4T_VERSION_1_0) /* this is for V1.0 */
  {
    UINT8_TO_BE_STREAM(p, T4T_V10_NDEF_TAG_AID_LEN);

    memcpy(p, t4t_v10_ndef_tag_aid, T4T_V10_NDEF_TAG_AID_LEN);

    p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + T4T_V10_NDEF_TAG_AID_LEN;
  } else if (version == T4T_VERSION_2_0) /* this is for V2.0 */
  {
    UINT8_TO_BE_STREAM(p, T4T_V20_NDEF_TAG_AID_LEN);

    memcpy(p, t4t_v20_ndef_tag_aid, T4T_V20_NDEF_TAG_AID_LEN);
    p += T4T_V20_NDEF_TAG_AID_LEN;

    UINT8_TO_BE_STREAM(p, 0x00); /* Le set to 0x00 */

    p_c_apdu->len = T4T_CMD_MAX_HDR_SIZE + T4T_V20_NDEF_TAG_AID_LEN + 1;
  } else {
    return false;
  }

  if (!rw_t4t_send_to_lower(p_c_apdu)) {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_validate_cc_file
**
** Description      Validate CC file and mandatory NDEF TLV
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool rw_t4t_validate_cc_file(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (p_t4t->cc_file.cclen < T4T_CC_FILE_MIN_LEN) {
    LOG(ERROR) << StringPrintf("CCLEN (%d) is too short", p_t4t->cc_file.cclen);
    return false;
  }

  if (T4T_GET_MAJOR_VERSION(p_t4t->cc_file.version) !=
      T4T_GET_MAJOR_VERSION(p_t4t->version)) {
    LOG(ERROR) << StringPrintf(
        "Peer version (0x%02X) is matched to ours "
        "(0x%02X)",
        p_t4t->cc_file.version, p_t4t->version);
    return false;
  }

  if (p_t4t->cc_file.max_le < 0x000F) {
    LOG(ERROR) << StringPrintf("MaxLe (%d) is too small",
                               p_t4t->cc_file.max_le);
    return false;
  }

  if (p_t4t->cc_file.max_lc < 0x0001) {
    LOG(ERROR) << StringPrintf("MaxLc (%d) is too small",
                               p_t4t->cc_file.max_lc);
    return false;
  }

  if ((p_t4t->cc_file.ndef_fc.file_id == T4T_CC_FILE_ID) ||
      (p_t4t->cc_file.ndef_fc.file_id == 0xE102) ||
      (p_t4t->cc_file.ndef_fc.file_id == 0xE103) ||
      ((p_t4t->cc_file.ndef_fc.file_id == 0x0000) &&
       (p_t4t->cc_file.version == 0x20)) ||
      (p_t4t->cc_file.ndef_fc.file_id == 0x3F00) ||
      (p_t4t->cc_file.ndef_fc.file_id == 0x3FFF) ||
      (p_t4t->cc_file.ndef_fc.file_id == 0xFFFF)) {
    LOG(ERROR) << StringPrintf("File ID (0x%04X) is invalid",
                               p_t4t->cc_file.ndef_fc.file_id);
    return false;
  }

  if ((p_t4t->cc_file.ndef_fc.max_file_size < 0x0005) ||
      (p_t4t->cc_file.ndef_fc.max_file_size == 0xFFFF)) {
    LOG(ERROR) << StringPrintf("max_file_size (%d) is reserved",
                               p_t4t->cc_file.ndef_fc.max_file_size);
    return false;
  }

  if (p_t4t->cc_file.ndef_fc.read_access != T4T_FC_READ_ACCESS) {
    LOG(ERROR) << StringPrintf("Read Access (0x%02X) is invalid",
                               p_t4t->cc_file.ndef_fc.read_access);
    return false;
  }

  if ((p_t4t->cc_file.ndef_fc.write_access != T4T_FC_WRITE_ACCESS) &&
      (p_t4t->cc_file.ndef_fc.write_access < T4T_FC_WRITE_ACCESS_PROP_START)) {
    LOG(ERROR) << StringPrintf("Write Access (0x%02X) is invalid",
                               p_t4t->cc_file.ndef_fc.write_access);
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_t4t_handle_error
**
** Description      notify error to application and clean up
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_handle_error(tNFC_STATUS status, uint8_t sw1, uint8_t sw2) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  tRW_DATA rw_data;
  tRW_EVENT event;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "status:0x%02X, sw1:0x%02X, sw2:0x%02X, "
      "state:0x%X",
      status, sw1, sw2, p_t4t->state);

  nfc_stop_quick_timer(&p_t4t->timer);

  if (rw_cb.p_cback) {
    rw_data.status = status;

    rw_data.t4t_sw.sw1 = sw1;
    rw_data.t4t_sw.sw2 = sw2;
    rw_data.ndef.cur_size = 0;
    rw_data.ndef.max_size = 0;

    switch (p_t4t->state) {
      case RW_T4T_STATE_DETECT_NDEF:
        rw_data.ndef.flags = RW_NDEF_FL_UNKNOWN;
        event = RW_T4T_NDEF_DETECT_EVT;
        break;

      case RW_T4T_STATE_READ_NDEF:
        event = RW_T4T_NDEF_READ_FAIL_EVT;
        break;

      case RW_T4T_STATE_UPDATE_NDEF:
        event = RW_T4T_NDEF_UPDATE_FAIL_EVT;
        break;

      case RW_T4T_STATE_PRESENCE_CHECK:
        event = RW_T4T_PRESENCE_CHECK_EVT;
        rw_data.status = NFC_STATUS_FAILED;
        break;

      case RW_T4T_STATE_SET_READ_ONLY:
        event = RW_T4T_SET_TO_RO_EVT;
        break;

      case RW_T4T_STATE_NDEF_FORMAT:
        event = RW_T4T_NDEF_FORMAT_CPLT_EVT;
        rw_data.status = NFC_STATUS_FAILED;
        break;

      default:
        event = RW_T4T_MAX_EVT;
        break;
    }

    p_t4t->state = RW_T4T_STATE_IDLE;

    if (event != RW_T4T_MAX_EVT) {
      (*(rw_cb.p_cback))(event, &rw_data);
    }
  } else {
    p_t4t->state = RW_T4T_STATE_IDLE;
  }
}

/*******************************************************************************
**
** Function         rw_t4t_sm_ndef_format
**
** Description      State machine for NDEF format procedure
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_sm_ndef_format(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t* p;
  uint16_t status_words;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (%d)", rw_t4t_get_sub_state_name(p_t4t->sub_state).c_str(),
      p_t4t->sub_state);

  /* get status words */
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;

  switch (p_t4t->sub_state) {
    case RW_T4T_SUBSTATE_WAIT_GET_HW_VERSION:
      p += (p_r_apdu->len - 1);
      if (*(p) == T4T_ADDI_FRAME_RESP) {
        if (!rw_t4t_get_sw_version()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_GET_SW_VERSION;
        }
      } else {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_GET_SW_VERSION:
      p += (p_r_apdu->len - 1);
      if (*(p) == T4T_ADDI_FRAME_RESP) {
        if (!rw_t4t_update_version_details(p_r_apdu)) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        }

        if (!rw_t4t_get_uid_details()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        }

        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_GET_UID;
      } else {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_GET_UID:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_create_app()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_CREATE_APP;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_CREATE_APP:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words == 0x91DE) /* DUPLICATE_ERROR, file already exist*/
      {
        status_words = 0x9100;
      }

      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_select_app()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_APP;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_SELECT_APP:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_create_ccfile()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_CREATE_CC;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_CREATE_CC:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words == 0x91DE) /* DUPLICATE_ERROR, file already exist*/
      {
        status_words = 0x9100;
      }

      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_create_ndef()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_CREATE_NDEF;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_CREATE_NDEF:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words == 0x91DE) /* DUPLICATE_ERROR, file already exist*/
      {
        status_words = 0x9100;
      }

      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_write_cc()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_WRITE_CC;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_WRITE_CC:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        if (!rw_t4t_write_ndef()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_WRITE_NDEF;
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_WRITE_NDEF:
      p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p);
      if (status_words != 0x9100) {
        rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
      } else {
        p_t4t->state = RW_T4T_STATE_IDLE;
        if (rw_cb.p_cback) {
          rw_data.ndef.status = NFC_STATUS_OK;
          rw_data.ndef.protocol = NFC_PROTOCOL_ISO_DEP;
          rw_data.ndef.max_size = p_t4t->card_size;
          rw_data.ndef.cur_size = 0x00;

          (*(rw_cb.p_cback))(RW_T4T_NDEF_FORMAT_CPLT_EVT, &rw_data);

          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("Sent RW_T4T_NDEF_FORMAT_CPLT_EVT");
        }
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown sub_state=%d", p_t4t->sub_state);
      rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t4t_sm_detect_ndef
**
** Description      State machine for NDEF detection procedure
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_sm_detect_ndef(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t *p, type, length;
  uint16_t status_words, nlen;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (%d)", rw_t4t_get_sub_state_name(p_t4t->sub_state).c_str(),
      p_t4t->sub_state);

  /* get status words */
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
  BE_STREAM_TO_UINT16(status_words, p);

  if (status_words != T4T_RSP_CMD_CMPLTED) {
    /* try V1.0 after failing of V2.0 */
    if ((p_t4t->sub_state == RW_T4T_SUBSTATE_WAIT_SELECT_APP) &&
        (p_t4t->version == T4T_VERSION_2_0)) {
      p_t4t->version = T4T_VERSION_1_0;

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("retry with version=0x%02X", p_t4t->version);

      if (!rw_t4t_select_application(T4T_VERSION_1_0)) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      }
      return;
    }

    p_t4t->ndef_status &= ~(RW_T4T_NDEF_STATUS_NDEF_DETECTED);
    rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
    return;
  }

  switch (p_t4t->sub_state) {
    case RW_T4T_SUBSTATE_WAIT_SELECT_APP:

      /* NDEF Tag application has been selected then select CC file */
      if (!rw_t4t_select_file(T4T_CC_FILE_ID)) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      } else {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_CC;
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_SELECT_CC:

      /* CC file has been selected then read mandatory part of CC file */
      if (!rw_t4t_read_file(0x00, T4T_CC_FILE_MIN_LEN, false)) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      } else {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_CC_FILE;
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_CC_FILE:

      /* CC file has been read then validate and select mandatory NDEF file */
      if (p_r_apdu->len >= T4T_CC_FILE_MIN_LEN + T4T_RSP_STATUS_WORDS_SIZE) {
        p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;

        BE_STREAM_TO_UINT16(p_t4t->cc_file.cclen, p);
        BE_STREAM_TO_UINT8(p_t4t->cc_file.version, p);
        BE_STREAM_TO_UINT16(p_t4t->cc_file.max_le, p);
        BE_STREAM_TO_UINT16(p_t4t->cc_file.max_lc, p);

        BE_STREAM_TO_UINT8(type, p);
        BE_STREAM_TO_UINT8(length, p);

        if ((type == T4T_NDEF_FILE_CONTROL_TYPE) &&
            (length == T4T_FILE_CONTROL_LENGTH)) {
          BE_STREAM_TO_UINT16(p_t4t->cc_file.ndef_fc.file_id, p);
          BE_STREAM_TO_UINT16(p_t4t->cc_file.ndef_fc.max_file_size, p);
          BE_STREAM_TO_UINT8(p_t4t->cc_file.ndef_fc.read_access, p);
          BE_STREAM_TO_UINT8(p_t4t->cc_file.ndef_fc.write_access, p);

          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("Capability Container (CC) file");
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("  CCLEN:  0x%04X", p_t4t->cc_file.cclen);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("  Version:0x%02X", p_t4t->cc_file.version);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("  MaxLe:  0x%04X", p_t4t->cc_file.max_le);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("  MaxLc:  0x%04X", p_t4t->cc_file.max_lc);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("  NDEF File Control TLV");
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "    FileID:      0x%04X", p_t4t->cc_file.ndef_fc.file_id);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "    MaxFileSize: 0x%04X", p_t4t->cc_file.ndef_fc.max_file_size);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "    ReadAccess:  0x%02X", p_t4t->cc_file.ndef_fc.read_access);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "    WriteAccess: 0x%02X", p_t4t->cc_file.ndef_fc.write_access);

          if (rw_t4t_validate_cc_file()) {
            if (!rw_t4t_select_file(p_t4t->cc_file.ndef_fc.file_id)) {
              rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
            } else {
              p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE;
            }
            break;
          }
        }
      }

      /* invalid response or CC file */
      p_t4t->ndef_status &= ~(RW_T4T_NDEF_STATUS_NDEF_DETECTED);
      rw_t4t_handle_error(NFC_STATUS_BAD_RESP, 0, 0);
      break;

    case RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE:

      /* NDEF file has been selected then read the first 2 bytes (NLEN) */
      if (!rw_t4t_read_file(0, T4T_FILE_LENGTH_SIZE, false)) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      } else {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_READ_NLEN;
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_READ_NLEN:

      /* NLEN has been read then report upper layer */
      if (p_r_apdu->len == T4T_FILE_LENGTH_SIZE + T4T_RSP_STATUS_WORDS_SIZE) {
        /* get length of NDEF */
        p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
        BE_STREAM_TO_UINT16(nlen, p);

        if (nlen <=
            p_t4t->cc_file.ndef_fc.max_file_size - T4T_FILE_LENGTH_SIZE) {
          p_t4t->ndef_status = RW_T4T_NDEF_STATUS_NDEF_DETECTED;

          if (p_t4t->cc_file.ndef_fc.write_access != T4T_FC_WRITE_ACCESS) {
            p_t4t->ndef_status |= RW_T4T_NDEF_STATUS_NDEF_READ_ONLY;
          }

          /* Get max bytes to read per command */
          if (p_t4t->cc_file.max_le >= RW_T4T_MAX_DATA_PER_READ) {
            p_t4t->max_read_size = RW_T4T_MAX_DATA_PER_READ;
          } else {
            p_t4t->max_read_size = p_t4t->cc_file.max_le;
          }

          /* Le: valid range is 0x01 to 0xFF */
          if (p_t4t->max_read_size >= T4T_MAX_LENGTH_LE) {
            p_t4t->max_read_size = T4T_MAX_LENGTH_LE;
          }

          /* Get max bytes to update per command */
          if (p_t4t->cc_file.max_lc >= RW_T4T_MAX_DATA_PER_WRITE) {
            p_t4t->max_update_size = RW_T4T_MAX_DATA_PER_WRITE;
          } else {
            p_t4t->max_update_size = p_t4t->cc_file.max_lc;
          }

          /* Lc: valid range is 0x01 to 0xFF */
          if (p_t4t->max_update_size >= T4T_MAX_LENGTH_LC) {
            p_t4t->max_update_size = T4T_MAX_LENGTH_LC;
          }

          p_t4t->ndef_length = nlen;
          p_t4t->state = RW_T4T_STATE_IDLE;

          if (rw_cb.p_cback) {
            rw_data.ndef.status = NFC_STATUS_OK;
            rw_data.ndef.protocol = NFC_PROTOCOL_ISO_DEP;
            rw_data.ndef.max_size =
                (uint32_t)(p_t4t->cc_file.ndef_fc.max_file_size -
                           (uint16_t)T4T_FILE_LENGTH_SIZE);
            rw_data.ndef.cur_size = nlen;
            rw_data.ndef.flags = RW_NDEF_FL_SUPPORTED | RW_NDEF_FL_FORMATED;
            if (p_t4t->cc_file.ndef_fc.write_access != T4T_FC_WRITE_ACCESS) {
              rw_data.ndef.flags |= RW_NDEF_FL_READ_ONLY;
            }

            (*(rw_cb.p_cback))(RW_T4T_NDEF_DETECT_EVT, &rw_data);

            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("Sent RW_T4T_NDEF_DETECT_EVT");
          }
        } else {
          /* NLEN should be less than max file size */
          LOG(ERROR) << StringPrintf(
              "NLEN (%d) + 2 must be <= max file "
              "size (%d)",
              nlen, p_t4t->cc_file.ndef_fc.max_file_size);

          p_t4t->ndef_status &= ~(RW_T4T_NDEF_STATUS_NDEF_DETECTED);
          rw_t4t_handle_error(NFC_STATUS_BAD_RESP, 0, 0);
        }
      } else {
        /* response payload size should be T4T_FILE_LENGTH_SIZE */
        LOG(ERROR) << StringPrintf(
            "Length (%d) of R-APDU must be %d", p_r_apdu->len,
            T4T_FILE_LENGTH_SIZE + T4T_RSP_STATUS_WORDS_SIZE);

        p_t4t->ndef_status &= ~(RW_T4T_NDEF_STATUS_NDEF_DETECTED);
        rw_t4t_handle_error(NFC_STATUS_BAD_RESP, 0, 0);
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown sub_state=%d", p_t4t->sub_state);
      rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t4t_sm_read_ndef
**
** Description      State machine for NDEF read procedure
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_sm_read_ndef(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t* p;
  uint16_t status_words;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (%d)", rw_t4t_get_sub_state_name(p_t4t->sub_state).c_str(),
      p_t4t->sub_state);

  /* get status words */
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
  BE_STREAM_TO_UINT16(status_words, p);

  if (status_words != T4T_RSP_CMD_CMPLTED) {
    rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
    GKI_freebuf(p_r_apdu);
    return;
  }

  switch (p_t4t->sub_state) {
    case RW_T4T_SUBSTATE_WAIT_READ_RESP:

      /* Read partial or complete data */
      p_r_apdu->len -= T4T_RSP_STATUS_WORDS_SIZE;

      if ((p_r_apdu->len > 0) && (p_r_apdu->len <= p_t4t->rw_length)) {
        p_t4t->rw_length -= p_r_apdu->len;
        p_t4t->rw_offset += p_r_apdu->len;

        if (rw_cb.p_cback) {
          rw_data.data.status = NFC_STATUS_OK;
          rw_data.data.p_data = p_r_apdu;

          /* if need to read more data */
          if (p_t4t->rw_length > 0) {
            (*(rw_cb.p_cback))(RW_T4T_NDEF_READ_EVT, &rw_data);

            if (!rw_t4t_read_file(p_t4t->rw_offset, p_t4t->rw_length, true)) {
              rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
            }
          } else {
            p_t4t->state = RW_T4T_STATE_IDLE;

            (*(rw_cb.p_cback))(RW_T4T_NDEF_READ_CPLT_EVT, &rw_data);

            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("Sent RW_T4T_NDEF_READ_CPLT_EVT");
          }

          p_r_apdu = nullptr;
        } else {
          p_t4t->rw_length = 0;
          p_t4t->state = RW_T4T_STATE_IDLE;
        }
      } else {
        LOG(ERROR) << StringPrintf(
            "invalid payload length (%d), rw_length "
            "(%d)",
            p_r_apdu->len, p_t4t->rw_length);
        rw_t4t_handle_error(NFC_STATUS_BAD_RESP, 0, 0);
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown sub_state = %d", p_t4t->sub_state);
      rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      break;
  }

  if (p_r_apdu) GKI_freebuf(p_r_apdu);
}

/*******************************************************************************
**
** Function         rw_t4t_sm_update_ndef
**
** Description      State machine for NDEF update procedure
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_sm_update_ndef(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t* p;
  uint16_t status_words;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (%d)", rw_t4t_get_sub_state_name(p_t4t->sub_state).c_str(),
      p_t4t->sub_state);

  /* Get status words */
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
  BE_STREAM_TO_UINT16(status_words, p);

  if (status_words != T4T_RSP_CMD_CMPLTED) {
    rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
    return;
  }

  switch (p_t4t->sub_state) {
    case RW_T4T_SUBSTATE_WAIT_UPDATE_NLEN:

      /* NLEN has been updated */
      /* if need to update data */
      if (p_t4t->p_update_data) {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_UPDATE_RESP;

        if (!rw_t4t_update_file()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
          p_t4t->p_update_data = nullptr;
        }
      } else {
        p_t4t->state = RW_T4T_STATE_IDLE;

        /* just finished last step of updating (updating NLEN) */
        if (rw_cb.p_cback) {
          rw_data.status = NFC_STATUS_OK;

          (*(rw_cb.p_cback))(RW_T4T_NDEF_UPDATE_CPLT_EVT, &rw_data);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("Sent RW_T4T_NDEF_UPDATE_CPLT_EVT");
        }
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_UPDATE_RESP:

      /* if updating is not completed */
      if (p_t4t->rw_length > 0) {
        if (!rw_t4t_update_file()) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
          p_t4t->p_update_data = nullptr;
        }
      } else {
        p_t4t->p_update_data = nullptr;

        /* update NLEN as last step of updating file */
        if (!rw_t4t_update_nlen(p_t4t->ndef_length)) {
          rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
        } else {
          p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_UPDATE_NLEN;
        }
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown sub_state = %d", p_t4t->sub_state);
      rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t4t_sm_set_readonly
**
** Description      State machine for CC update procedure
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_sm_set_readonly(NFC_HDR* p_r_apdu) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  uint8_t* p;
  uint16_t status_words;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (%d)", rw_t4t_get_sub_state_name(p_t4t->sub_state).c_str(),
      p_t4t->sub_state);

  /* Get status words */
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
  BE_STREAM_TO_UINT16(status_words, p);

  if (status_words != T4T_RSP_CMD_CMPLTED) {
    rw_t4t_handle_error(NFC_STATUS_CMD_NOT_CMPLTD, *(p - 2), *(p - 1));
    return;
  }

  switch (p_t4t->sub_state) {
    case RW_T4T_SUBSTATE_WAIT_SELECT_CC:

      /* CC file has been selected then update write access to read-only in CC
       * file */
      if (!rw_t4t_update_cc_to_readonly()) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      } else {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_UPDATE_CC;
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_UPDATE_CC:
      /* CC Updated, Select NDEF File to allow NDEF operation */
      p_t4t->cc_file.ndef_fc.write_access = T4T_FC_NO_WRITE_ACCESS;
      p_t4t->ndef_status |= RW_T4T_NDEF_STATUS_NDEF_READ_ONLY;

      if (!rw_t4t_select_file(p_t4t->cc_file.ndef_fc.file_id)) {
        rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      } else {
        p_t4t->sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE;
      }
      break;

    case RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE:
      p_t4t->state = RW_T4T_STATE_IDLE;
      /* just finished last step of configuring tag read only (Selecting NDEF
       * file CC) */
      if (rw_cb.p_cback) {
        rw_data.status = NFC_STATUS_OK;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Sent RW_T4T_SET_TO_RO_EVT");
        (*(rw_cb.p_cback))(RW_T4T_SET_TO_RO_EVT, &rw_data);
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown sub_state = %d", p_t4t->sub_state);
      rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t4t_process_timeout
**
** Description      process timeout event
**
** Returns          none
**
*******************************************************************************/
void rw_t4t_process_timeout(TIMER_LIST_ENT* p_tle) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event=%d", p_tle->event);

  if (p_tle->event == NFC_TTYPE_RW_T4T_RESPONSE) {
    rw_t4t_handle_error(NFC_STATUS_TIMEOUT, 0, 0);
  } else {
    LOG(ERROR) << StringPrintf("unknown event=%d", p_tle->event);
  }
}

/*******************************************************************************
**
** Function         rw_t4t_handle_isodep_nak_rsp
**
** Description      This function handles the response and ntf .
**
** Returns          none
**
*******************************************************************************/
void rw_t4t_handle_isodep_nak_rsp(uint8_t status, bool is_ntf) {
  tRW_DATA rw_data;
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rw_t4t_handle_isodep_nak_rsp %d", status);
  if (is_ntf || (status != NFC_STATUS_OK)) {
    rw_data.status = status;
    nfc_stop_quick_timer(&p_t4t->timer);
    p_t4t->state = RW_T4T_STATE_IDLE;
    (*(rw_cb.p_cback))(RW_T4T_PRESENCE_CHECK_EVT, &rw_data);
  }
}

/*******************************************************************************
**
** Function         rw_t4t_data_cback
**
** Description      This callback function receives the data from NFCC.
**
** Returns          none
**
*******************************************************************************/
static void rw_t4t_data_cback(__attribute__((unused)) uint8_t conn_id,
                              tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  NFC_HDR* p_r_apdu;
  tRW_DATA rw_data;

  uint8_t begin_state = p_t4t->state;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event = 0x%X", event);
  nfc_stop_quick_timer(&p_t4t->timer);

  switch (event) {
    case NFC_DEACTIVATE_CEVT:
      NFC_SetStaticRfCback(nullptr);
      p_t4t->state = RW_T4T_STATE_NOT_ACTIVATED;
      return;

    case NFC_ERROR_CEVT:
      if (p_t4t->state == RW_T4T_STATE_PRESENCE_CHECK) {
        p_t4t->state = RW_T4T_STATE_IDLE;
        rw_data.status = NFC_STATUS_FAILED;
        (*(rw_cb.p_cback))(RW_T4T_PRESENCE_CHECK_EVT, &rw_data);
      } else if (p_t4t->state == RW_T4T_STATE_NDEF_FORMAT) {
        p_t4t->state = RW_T4T_STATE_IDLE;
        rw_data.status = NFC_STATUS_FAILED;
        (*(rw_cb.p_cback))(RW_T4T_NDEF_FORMAT_CPLT_EVT, &rw_data);
      } else if (p_t4t->state != RW_T4T_STATE_IDLE) {
        rw_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        rw_t4t_handle_error(rw_data.status, 0, 0);
      } else {
        p_t4t->state = RW_T4T_STATE_IDLE;
        rw_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        (*(rw_cb.p_cback))(RW_T4T_INTF_ERROR_EVT, &rw_data);
      }
      return;

    case NFC_DATA_CEVT:
      p_r_apdu = (NFC_HDR*)p_data->data.p_data;
      break;

    default:
      return;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "RW T4T state: <%s (%d)>", rw_t4t_get_state_name(p_t4t->state).c_str(),
      p_t4t->state);

  if (p_t4t->state != RW_T4T_STATE_IDLE &&
      p_t4t->state != RW_T4T_STATE_PRESENCE_CHECK &&
      p_r_apdu->len < T4T_RSP_STATUS_WORDS_SIZE) {
    LOG(ERROR) << StringPrintf("%s incorrect p_r_apdu length", __func__);
    android_errorWriteLog(0x534e4554, "120865977");
    rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
    GKI_freebuf(p_r_apdu);
    return;
  }

  switch (p_t4t->state) {
    case RW_T4T_STATE_IDLE:
      /* Unexpected R-APDU, it should be raw frame response */
      /* forward to upper layer without parsing */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "RW T4T Raw Frame: Len [0x%X] Status [%s]", p_r_apdu->len,
          NFC_GetStatusName(p_data->data.status).c_str());
      if (rw_cb.p_cback) {
        rw_data.raw_frame.status = p_data->data.status;
        rw_data.raw_frame.p_data = p_r_apdu;
        (*(rw_cb.p_cback))(RW_T4T_RAW_FRAME_EVT, &rw_data);
        p_r_apdu = nullptr;
      } else {
        GKI_freebuf(p_r_apdu);
      }
      break;
    case RW_T4T_STATE_DETECT_NDEF:
      rw_t4t_sm_detect_ndef(p_r_apdu);
      GKI_freebuf(p_r_apdu);
      break;
    case RW_T4T_STATE_READ_NDEF:
      rw_t4t_sm_read_ndef(p_r_apdu);
      /* p_r_apdu may send upper lyaer */
      break;
    case RW_T4T_STATE_UPDATE_NDEF:
      rw_t4t_sm_update_ndef(p_r_apdu);
      GKI_freebuf(p_r_apdu);
      break;
    case RW_T4T_STATE_PRESENCE_CHECK:
      /* if any response, send presence check with ok */
      rw_data.status = NFC_STATUS_OK;
      p_t4t->state = RW_T4T_STATE_IDLE;
      (*(rw_cb.p_cback))(RW_T4T_PRESENCE_CHECK_EVT, &rw_data);
      GKI_freebuf(p_r_apdu);
      break;
    case RW_T4T_STATE_SET_READ_ONLY:
      rw_t4t_sm_set_readonly(p_r_apdu);
      GKI_freebuf(p_r_apdu);
      break;
    case RW_T4T_STATE_NDEF_FORMAT:
      rw_t4t_sm_ndef_format(p_r_apdu);
      GKI_freebuf(p_r_apdu);
      break;
    default:
      LOG(ERROR) << StringPrintf("invalid state=%d", p_t4t->state);
      GKI_freebuf(p_r_apdu);
      break;
  }

  if (begin_state != p_t4t->state) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW T4T state changed:<%s> -> <%s>",
                        rw_t4t_get_state_name(begin_state).c_str(),
                        rw_t4t_get_state_name(p_t4t->state).c_str());
  }
}

#if(NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         rw_t4t_ndefee_select
**
** Description      Initialize T4T
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeInitCb(void) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter ", __func__);
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rw_t4t_ndefee_select ()");

  NFC_SetStaticT4tNfceeCback(rw_t4t_data_cback);

  p_t4t->state = RW_T4T_STATE_IDLE;
  p_t4t->version = T4T_MY_VERSION;
  /* set it min of max R-APDU data size before reading CC file */
  p_t4t->cc_file.max_le = T4T_MIN_MLE;

  /* These will be udated during NDEF detection */
  p_t4t->max_read_size = T4T_MAX_LENGTH_LE - T4T_FILE_LENGTH_SIZE;
  p_t4t->max_update_size = RW_T4TNFCEE_DATA_PER_WRITE;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeUpdateCC
**
** Description      Updates the T4T data structures with CC info
**
** Returns          None
**
*******************************************************************************/
void RW_T4tNfceeUpdateCC(uint8_t *ccInfo) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter", __func__);
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  BE_STREAM_TO_UINT16(p_t4t->cc_file.max_le, ccInfo);
  BE_STREAM_TO_UINT16(p_t4t->cc_file.max_lc, ccInfo);

  /* Get max bytes to read per command */
  if (p_t4t->cc_file.max_le >= RW_T4T_MAX_DATA_PER_READ) {
      p_t4t->max_read_size = RW_T4T_MAX_DATA_PER_READ;
  } else {
    p_t4t->max_read_size = p_t4t->cc_file.max_le;
  }

  /* Le: valid range is 0x01 to 0xFF */
  if (p_t4t->max_read_size >= T4T_MAX_LENGTH_LE) {
      p_t4t->max_read_size = T4T_MAX_LENGTH_LE;
  }

  /* Get max bytes to update per command */
  if (p_t4t->cc_file.max_lc >= RW_T4T_MAX_DATA_PER_WRITE) {
    p_t4t->max_update_size = RW_T4T_MAX_DATA_PER_WRITE;
  } else {
    p_t4t->max_update_size = p_t4t->cc_file.max_lc;
  }
  /* Lc: valid range is 0x01 to 0xFF */
  if (p_t4t->max_update_size >= T4T_MAX_LENGTH_LC) {
    p_t4t->max_update_size = T4T_MAX_LENGTH_LC;
  }


  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s le %d  lc: %d  max_read_size: %d max_update_size: %d", __func__,
         p_t4t->cc_file.max_le, p_t4t->cc_file.max_lc, p_t4t->max_read_size, p_t4t->max_update_size);

}

/*******************************************************************************
**
** Function         RW_T4tNfceeSelectApplication
**
** Description      Selects T4T application using T4T AID
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeSelectApplication(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter", __func__);
  if (!rw_t4t_select_application(T4T_VERSION_2_0)) {
    return NFC_STATUS_FAILED;
  } else
    return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeSelectFile
**
** Description      Selects T4T Nfcee File
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeSelectFile(uint16_t fileId) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter", __func__);
  if (!rw_t4t_select_file(fileId)) {
    return NFC_STATUS_FAILED;
  } else
    return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeReadDataLen
**
** Description      Reads proprietary data Len
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeReadDataLen() {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter ", __func__);
  if (!rw_t4t_read_file(0x00, T4T_FILE_LENGTH_SIZE, false)) {
    rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
    return NFC_STATUS_FAILED;
  }
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeReadFile
**
** Description      Reads T4T Nfcee File
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeReadFile(uint16_t offset, uint16_t Readlen) {
  // tNFC_STATUS status = NFC_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s Enter : Readlen : 0x%x", __func__, Readlen);
  if (!rw_t4t_read_file(offset, Readlen, false)) {
    rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
    return NFC_STATUS_FAILED;
  }
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeReadPendingData
**
** Description      Reads pending data from T4T Nfcee File
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeReadPendingData() {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;
  p_t4t->rw_length -= p_t4t->max_read_size;
  p_t4t->rw_offset += p_t4t->max_read_size;
  if (!rw_t4t_read_file(p_t4t->rw_offset, p_t4t->rw_length, true)) {
    rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
    return NFC_STATUS_FAILED;
  }
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeUpdateNlen
**
** Description      writes requested length to the file
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeUpdateNlen(uint16_t len) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter ", __func__);
  if (!rw_t4t_update_nlen(len)) {
    return NFC_STATUS_FAILED;
  }
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tNfceeStartUpdateFile
**
** Description      starts writing data to the currently selected file
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeStartUpdateFile(uint16_t length, uint8_t* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter ", __func__);
  rw_cb.tcb.t4t.p_update_data = p_data;
  rw_cb.tcb.t4t.rw_offset = T4T_FILE_LENGTH_SIZE;
  rw_cb.tcb.t4t.rw_length = length;
  return RW_T4tNfceeUpdateFile();
}

/*******************************************************************************
**
** Function         RW_T4tNfceeUpdateFile
**
** Description      writes requested data to the currently selected file
**
** Returns          NFC_STATUS_OK if success else NFC_STATUS_FAILED
**
*******************************************************************************/
tNFC_STATUS RW_T4tNfceeUpdateFile() {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter ", __func__);
  if (!rw_t4t_update_file()) {
    rw_t4t_handle_error(NFC_STATUS_FAILED, 0, 0);
    rw_cb.tcb.t4t.p_update_data = nullptr;
    return NFC_STATUS_FAILED;
  }
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tIsUpdateComplete
**
** Description      Return true if no more data to write
**
** Returns          true/false
**
*******************************************************************************/
bool RW_T4tIsUpdateComplete(void) { return (rw_cb.tcb.t4t.rw_length == 0); }

/*******************************************************************************
**
** Function         RW_T4tIsReadComplete
**
** Description      Return true if no more data to be read
**
** Returns          true/false
**
*******************************************************************************/
bool RW_T4tIsReadComplete(void) {
  return (rw_cb.tcb.t4t.rw_length <= rw_cb.tcb.t4t.max_read_size);
}
#endif

/*******************************************************************************
**
** Function         RW_T4tFormatNDef
**
** Description      format T4T tag
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS RW_T4tFormatNDef(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.t4t.state);
    return NFC_STATUS_FAILED;
  }

  rw_cb.tcb.t4t.card_type = 0x00;

  if (!rw_t4t_get_hw_version()) {
    return NFC_STATUS_FAILED;
  }

  rw_cb.tcb.t4t.state = RW_T4T_STATE_NDEF_FORMAT;
  rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_GET_HW_VERSION;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         rw_t4t_select
**
** Description      Initialise T4T
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS rw_t4t_select(void) {
  tRW_T4T_CB* p_t4t = &rw_cb.tcb.t4t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  NFC_SetStaticRfCback(rw_t4t_data_cback);

  p_t4t->state = RW_T4T_STATE_IDLE;
  p_t4t->version = T4T_MY_VERSION;

  /* set it min of max R-APDU data size before reading CC file */
  p_t4t->cc_file.max_le = T4T_MIN_MLE;

  /* These will be udated during NDEF detection */
  p_t4t->max_read_size = T4T_MAX_LENGTH_LE;
  p_t4t->max_update_size = T4T_MAX_LENGTH_LC;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tDetectNDef
**
** Description      This function performs NDEF detection procedure
**
**                  RW_T4T_NDEF_DETECT_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if T4T is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_T4tDetectNDef(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.t4t.state);
    return NFC_STATUS_FAILED;
  }

  if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_DETECTED) {
    /* NDEF Tag application has been selected then select CC file */
    if (!rw_t4t_select_file(T4T_CC_FILE_ID)) {
      return NFC_STATUS_FAILED;
    }
    rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_CC;
  } else {
    /* Select NDEF Tag Application */
    if (!rw_t4t_select_application(rw_cb.tcb.t4t.version)) {
      return NFC_STATUS_FAILED;
    }
    rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_APP;
  }

  rw_cb.tcb.t4t.state = RW_T4T_STATE_DETECT_NDEF;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_T4tReadNDef
**
** Description      This function performs NDEF read procedure
**                  Note: RW_T4tDetectNDef () must be called before using this
**
**                  The following event will be returned
**                      RW_T4T_NDEF_READ_EVT for each segmented NDEF message
**                      RW_T4T_NDEF_READ_CPLT_EVT for the last segment or
**                      complete NDEF
**                      RW_T4T_NDEF_READ_FAIL_EVT for failure
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if T4T is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_T4tReadNDef(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.t4t.state);
    return NFC_STATUS_FAILED;
  }

  /* if NDEF has been detected */
  if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_DETECTED) {
    /* start reading NDEF */
    if (!rw_t4t_read_file(T4T_FILE_LENGTH_SIZE, rw_cb.tcb.t4t.ndef_length,
                          false)) {
      return NFC_STATUS_FAILED;
    }

    rw_cb.tcb.t4t.state = RW_T4T_STATE_READ_NDEF;
    rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_READ_RESP;

    return NFC_STATUS_OK;
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         RW_T4tUpdateNDef
**
** Description      This function performs NDEF update procedure
**                  Note: RW_T4tDetectNDef () must be called before using this
**                        Updating data must not be removed until returning
**                        event
**
**                  The following event will be returned
**                      RW_T4T_NDEF_UPDATE_CPLT_EVT for complete
**                      RW_T4T_NDEF_UPDATE_FAIL_EVT for failure
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if T4T is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_T4tUpdateNDef(uint16_t length, uint8_t* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("length:%d", length);

  if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.t4t.state);
    return NFC_STATUS_FAILED;
  }

  /* if NDEF has been detected */
  if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_DETECTED) {
    /* if read-only */
    if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_READ_ONLY) {
      LOG(ERROR) << StringPrintf("NDEF is read-only");
      return NFC_STATUS_FAILED;
    }

    if (rw_cb.tcb.t4t.cc_file.ndef_fc.max_file_size <
        length + T4T_FILE_LENGTH_SIZE) {
      LOG(ERROR) << StringPrintf(
          "data (%d bytes) plus NLEN is more than max file "
          "size (%d)",
          length, rw_cb.tcb.t4t.cc_file.ndef_fc.max_file_size);
      return NFC_STATUS_FAILED;
    }

    /* store NDEF length and data */
    rw_cb.tcb.t4t.ndef_length = length;
    rw_cb.tcb.t4t.p_update_data = p_data;

    rw_cb.tcb.t4t.rw_offset = T4T_FILE_LENGTH_SIZE;
    rw_cb.tcb.t4t.rw_length = length;

    /* set NLEN to 0x0000 for the first step */
    if (!rw_t4t_update_nlen(0x0000)) {
      return NFC_STATUS_FAILED;
    }

    rw_cb.tcb.t4t.state = RW_T4T_STATE_UPDATE_NDEF;
    rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_UPDATE_NLEN;

    return NFC_STATUS_OK;
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }
}

/*****************************************************************************
**
** Function         RW_T4tPresenceCheck
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_T4T_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**
**      option is RW_T4T_CHK_EMPTY_I_BLOCK, use empty I block for presence check
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T4tPresenceCheck(uint8_t option) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;
  bool status;
  NFC_HDR* p_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%d", option);

  /* If RW_SelectTagType was not called (no conn_callback) return failure */
  if (!rw_cb.p_cback) {
    retval = NFC_STATUS_FAILED;
  }
  /* If we are not activated, then RW_T4T_PRESENCE_CHECK_EVT with
     NFC_STATUS_FAILED */
  else if (rw_cb.tcb.t4t.state == RW_T4T_STATE_NOT_ACTIVATED) {
    evt_data.status = NFC_STATUS_FAILED;
    (*rw_cb.p_cback)(RW_T4T_PRESENCE_CHECK_EVT, &evt_data);
  }
  /* If command is pending, assume tag is still present */
  else if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    evt_data.status = NFC_STATUS_OK;
    (*rw_cb.p_cback)(RW_T4T_PRESENCE_CHECK_EVT, &evt_data);
  } else {
    status = false;
    if (option == RW_T4T_CHK_EMPTY_I_BLOCK) {
      /* use empty I block for presence check */
      p_data = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + NCI_MSG_OFFSET_SIZE +
                                    NCI_DATA_HDR_SIZE);
      if (p_data != nullptr) {
        p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
        p_data->len = 0;
        if (NFC_SendData(NFC_RF_CONN_ID, (NFC_HDR*)p_data) == NFC_STATUS_OK)
          status = true;
      }
    } else if (option == RW_T4T_CHK_ISO_DEP_NAK_PRES_CHK) {
      if (NFC_ISODEPNakPresCheck() == NFC_STATUS_OK) status = true;
    }

    if (status == true) {
      rw_cb.tcb.t4t.state = RW_T4T_STATE_PRESENCE_CHECK;
    } else {
      retval = NFC_STATUS_NO_BUFFERS;
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T4tSetNDefReadOnly
**
** Description      This function performs NDEF read-only procedure
**                  Note: RW_T4tDetectNDef() must be called before using this
**
**                  The RW_T4T_SET_TO_RO_EVT event will be returned.
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if T4T is busy or other error
**
*****************************************************************************/
tNFC_STATUS RW_T4tSetNDefReadOnly(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.t4t.state != RW_T4T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.t4t.state);
    return NFC_STATUS_FAILED;
  }

  /* if NDEF has been detected */
  if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_DETECTED) {
    /* if read-only */
    if (rw_cb.tcb.t4t.ndef_status & RW_T4T_NDEF_STATUS_NDEF_READ_ONLY) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NDEF is already read-only");

      evt_data.status = NFC_STATUS_OK;
      (*rw_cb.p_cback)(RW_T4T_SET_TO_RO_EVT, &evt_data);
      return (retval);
    }

    /* NDEF Tag application has been selected then select CC file */
    if (!rw_t4t_select_file(T4T_CC_FILE_ID)) {
      return NFC_STATUS_FAILED;
    }

    rw_cb.tcb.t4t.state = RW_T4T_STATE_SET_READ_ONLY;
    rw_cb.tcb.t4t.sub_state = RW_T4T_SUBSTATE_WAIT_SELECT_CC;

    return NFC_STATUS_OK;
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }
  return (retval);
}

/*******************************************************************************
**
** Function         rw_t4t_get_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_t4t_get_state_name(uint8_t state) {
  switch (state) {
    case RW_T4T_STATE_NOT_ACTIVATED:
      return "NOT_ACTIVATED";
    case RW_T4T_STATE_IDLE:
      return "IDLE";
    case RW_T4T_STATE_DETECT_NDEF:
      return "NDEF_DETECTION";
    case RW_T4T_STATE_READ_NDEF:
      return "READ_NDEF";
    case RW_T4T_STATE_UPDATE_NDEF:
      return "UPDATE_NDEF";
    case RW_T4T_STATE_PRESENCE_CHECK:
      return "PRESENCE_CHECK";
    case RW_T4T_STATE_SET_READ_ONLY:
      return "SET_READ_ONLY";
    default:
      return "???? UNKNOWN STATE";
  }
}

/*******************************************************************************
**
** Function         rw_t4t_get_sub_state_name
**
** Description      This function returns the sub_state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_t4t_get_sub_state_name(uint8_t sub_state) {
  switch (sub_state) {
    case RW_T4T_SUBSTATE_WAIT_SELECT_APP:
      return "WAIT_SELECT_APP";
    case RW_T4T_SUBSTATE_WAIT_SELECT_CC:
      return "WAIT_SELECT_CC";
    case RW_T4T_SUBSTATE_WAIT_CC_FILE:
      return "WAIT_CC_FILE";
    case RW_T4T_SUBSTATE_WAIT_SELECT_NDEF_FILE:
      return "WAIT_SELECT_NDEF_FILE";
    case RW_T4T_SUBSTATE_WAIT_READ_NLEN:
      return "WAIT_READ_NLEN";
    case RW_T4T_SUBSTATE_WAIT_READ_RESP:
      return "WAIT_READ_RESP";
    case RW_T4T_SUBSTATE_WAIT_UPDATE_RESP:
      return "WAIT_UPDATE_RESP";
    case RW_T4T_SUBSTATE_WAIT_UPDATE_NLEN:
      return "WAIT_UPDATE_NLEN";
    case RW_T4T_SUBSTATE_WAIT_GET_HW_VERSION:
      return "WAIT_GET_HW_VERSION";
    case RW_T4T_SUBSTATE_WAIT_GET_SW_VERSION:
      return "WAIT_GET_SW_VERSION";
    case RW_T4T_SUBSTATE_WAIT_GET_UID:
      return "WAIT_GET_UID";
    case RW_T4T_SUBSTATE_WAIT_CREATE_APP:
      return "WAIT_CREATE_APP";
    case RW_T4T_SUBSTATE_WAIT_CREATE_CC:
      return "WAIT_CREATE_CC";
    case RW_T4T_SUBSTATE_WAIT_CREATE_NDEF:
      return "WAIT_CREATE_NDEF";
    case RW_T4T_SUBSTATE_WAIT_WRITE_CC:
      return "WAIT_WRITE_CC";
    case RW_T4T_SUBSTATE_WAIT_WRITE_NDEF:
      return "WAIT_WRITE_NDEF";
    default:
      return "???? UNKNOWN SUBSTATE";
  }
}
