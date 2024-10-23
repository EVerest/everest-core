/*****************************************************************************
 * Copyright (C) 2015 ST Microelectronics S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

/******************************************************************************
 *
 *  This file contains the implementation for Mifare Classic tag in
 *  Reader/Writer mode.
 *
 ******************************************************************************/
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <string.h>
#include "bt_types.h"
#include "nfc_target.h"

#include "gki.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"
#include "tags_int.h"

#define MFC_KeyA 0x60
#define MFC_KeyB 0x61
#define MFC_Read 0x30
#define MFC_Write 0xA0

/* main state */
/* Mifare Classic is not activated */
#define RW_MFC_STATE_NOT_ACTIVATED 0x00
/* waiting for upper layer API */
#define RW_MFC_STATE_IDLE 0x01
/* performing NDEF detection precedure */
#define RW_MFC_STATE_DETECT_NDEF 0x02
/* performing read NDEF procedure */
#define RW_MFC_STATE_READ_NDEF 0x03
/* performing update NDEF procedure */
#define RW_MFC_STATE_UPDATE_NDEF 0x04
/* checking presence of tag */
#define RW_MFC_STATE_PRESENCE_CHECK 0x05
/* convert tag to read only */
#define RW_MFC_STATE_SET_READ_ONLY 0x06
/* detect tlv */
#define RW_MFC_STATE_DETECT_TLV 0x7
/* NDef Format */
#define RW_MFC_STATE_NDEF_FORMAT 0x8

#define RW_MFC_SUBSTATE_NONE 0x00
#define RW_MFC_SUBSTATE_IDLE 0x01
#define RW_MFC_SUBSTATE_WAIT_ACK 0x02
#define RW_MFC_SUBSTATE_READ_BLOCK 0x03
#define RW_MFC_SUBSTATE_FORMAT_BLOCK 0x04
#define RW_MFC_SUBSTATE_WRITE_BLOCK 0x05

#define RW_MFC_LONG_TLV_SIZE 4
#define RW_MFC_SHORT_TLV_SIZE 2

#define RW_MFC_4K_Support 0x10

#define RW_MFC_1K_BLOCK_SIZE 16

uint8_t KeyNDEF[6] = {0xD3, 0XF7, 0xD3, 0XF7, 0xD3, 0XF7};
uint8_t KeyMAD[6] = {0xA0, 0XA1, 0xA2, 0XA3, 0xA4, 0XA5};
uint8_t KeyDefault[6] = {0xFF, 0XFF, 0xFF, 0XFF, 0xFF, 0XFF};
uint8_t access_permission_nfc[4] = {0x7F, 0x07, 0x88, 0x40};
uint8_t access_permission_mad[4] = {0x78, 0x77, 0x88, 0xC1};
uint8_t MAD_B1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                      0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
uint8_t MAD_B2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                      0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
uint8_t MAD_B64[16] = {0xE8, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                       0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
uint8_t NFC_B0[16] = {0x03, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static bool rw_mfc_send_to_lower(NFC_HDR* p_c_apdu);
static bool rw_mfc_authenticate(int sector, bool KeyA);
static tNFC_STATUS rw_mfc_readBlock(int block);
static void rw_mfc_handle_tlv_detect_rsp(uint8_t* p_data);
static tNFC_STATUS rw_MfcLocateTlv(uint8_t tlv_type);
static void rw_mfc_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data);
static void rw_mfc_resume_op();
static bool rw_nfc_decodeTlv(uint8_t* p_data);
static void rw_mfc_ntf_tlv_detect_complete(tNFC_STATUS status);
static void rw_mfc_handle_read_op(uint8_t* data);
static void rw_mfc_handle_op_complete(void);
static void rw_mfc_handle_ndef_read_rsp(uint8_t* p_data);
static void rw_mfc_process_error();

static tNFC_STATUS rw_mfc_formatBlock(int block);
static void rw_mfc_handle_format_rsp(uint8_t* p_data);
static void rw_mfc_handle_format_op();
static tNFC_STATUS rw_mfc_writeBlock(int block);
static void rw_mfc_handle_write_rsp(uint8_t* p_data);
static void rw_mfc_handle_write_op();

//using android::base::StringPrintf;
extern bool nfc_debug_enabled;

/*****************************************************************************
**
** Function         RW_MfcFormatNDef
**
** Description
**      Format Tag content
**
** Returns
**      NFC_STATUS_OK, Command sent to format Tag
**      NFC_STATUS_REJECTED: cannot format the tag
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_MfcFormatNDef(void) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tNFC_STATUS status = NFC_STATUS_OK;

  if (p_mfc->state != RW_MFC_STATE_IDLE) {
    LOG(ERROR) << StringPrintf
    ("%s: Mifare Classic tag not activated or Busy State : %d",
     __func__, p_mfc->state);
    return NFC_STATUS_BUSY;
  }

  p_mfc->state = RW_MFC_STATE_NDEF_FORMAT;
  p_mfc->substate = RW_MFC_SUBSTATE_NONE;
  p_mfc->last_block_accessed.block = 1;
  p_mfc->next_block.block = 1;

  status = rw_mfc_formatBlock(p_mfc->next_block.block);
  if (status == NFC_STATUS_OK) {
    p_mfc->state = RW_MFC_STATE_NDEF_FORMAT;
  } else {
    p_mfc->substate = RW_MFC_SUBSTATE_NONE;
  }

  return status;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_formatBlock
 **
 ** Description      This function format a given block.
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static tNFC_STATUS rw_mfc_formatBlock(int block) {
  NFC_HDR* mfcbuf;
  uint8_t* p;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  int sectorlength = block / 4;
  tNFC_STATUS status = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: block : %d", __func__, block);

  if (block > 128) {
    sectorlength = (p_mfc->next_block.block - 128) / 16 + 32;
  }

  if (sectorlength != p_mfc->sector_authentified) {
    if (rw_mfc_authenticate(block, true) == true) {
      return NFC_STATUS_OK;
    }
    return NFC_STATUS_FAILED;
  }

  mfcbuf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!mfcbuf) {
    LOG(ERROR) << StringPrintf("%s: Cannot allocate buffer",__func__);
    return NFC_STATUS_REJECTED;
  }

  mfcbuf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(mfcbuf + 1) + mfcbuf->offset;

  UINT8_TO_BE_STREAM(p, MFC_Write);
  UINT8_TO_BE_STREAM(p, block);

  if (block == 1) {
    ARRAY_TO_BE_STREAM(p, MAD_B1, 16);
  } else if (block == 2 || block == 65 || block == 66) {
    ARRAY_TO_BE_STREAM(p, MAD_B2, 16);
  } else if (block == 3 || block == 67) {
    ARRAY_TO_BE_STREAM(p, KeyMAD, 6);
    ARRAY_TO_BE_STREAM(p, access_permission_mad, 4);
    ARRAY_TO_BE_STREAM(p, KeyDefault, 6);
  } else if (block == 4) {
    ARRAY_TO_BE_STREAM(p, NFC_B0, 16);
  } else if (block == 64) {
    ARRAY_TO_BE_STREAM(p, MAD_B64, 16);
  } else {
    ARRAY_TO_BE_STREAM(p, KeyNDEF, 6);
    ARRAY_TO_BE_STREAM(p, access_permission_nfc, 4);
    ARRAY_TO_BE_STREAM(p, KeyDefault, 6);
  }
  mfcbuf->len = 18;

  if (!rw_mfc_send_to_lower(mfcbuf)) {
    return NFC_STATUS_REJECTED;
  }
  p_mfc->current_block = block;
  p_mfc->substate = RW_MFC_SUBSTATE_FORMAT_BLOCK;

  return status;
}

static void rw_mfc_handle_format_rsp(uint8_t* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  NFC_HDR* mfc_data;
  uint8_t* p;

  mfc_data = (NFC_HDR*)p_data;
  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;

  switch (p_mfc->substate) {
    case RW_MFC_SUBSTATE_WAIT_ACK:
      p_mfc->last_block_accessed.block = p_mfc->current_block;

      if (p[0] == 0x0) {
        p_mfc->next_block.auth = true;
        p_mfc->last_block_accessed.auth = true;

        if (p_mfc->next_block.block < 128) {
          p_mfc->sector_authentified = p_mfc->next_block.block / 4;
        } else {
          p_mfc->sector_authentified =
              (p_mfc->next_block.block - 128) / 16 + 32;
        }
        rw_mfc_resume_op();
      } else {
        p_mfc->next_block.auth = false;
        p_mfc->last_block_accessed.auth = false;
        nfc_stop_quick_timer(&p_mfc->timer);
        rw_mfc_process_error();
      }
      break;

    case RW_MFC_SUBSTATE_FORMAT_BLOCK:
      if (p[0] == 0x0) {
        rw_mfc_handle_format_op();
      } else {
        nfc_stop_quick_timer(&p_mfc->timer);
        rw_mfc_process_error();
      }
      break;
  }
}

static void rw_mfc_handle_format_op() {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tRW_READ_DATA evt_data;
  int num_of_blocks = 0;

  /* Total blockes of Mifare 1k/4K */
  if (p_mfc->selres & RW_MFC_4K_Support)
    num_of_blocks = 256;
  else
    num_of_blocks = 64;

  p_mfc->last_block_accessed.block = p_mfc->current_block;

  // Find next block needed to format
  if (p_mfc->current_block < 4) {
    p_mfc->next_block.block = p_mfc->current_block + 1;
  } else if (p_mfc->current_block == 4) {
    p_mfc->next_block.block = 7;
  } else if (p_mfc->current_block >= 63 && p_mfc->current_block < 67) {
    p_mfc->next_block.block = p_mfc->current_block + 1;
  } else if (p_mfc->current_block < 127) {
    p_mfc->next_block.block = p_mfc->current_block + 4;
  } else {
    p_mfc->next_block.block = p_mfc->current_block + 16;
  }

  if (p_mfc->next_block.block < num_of_blocks) {
    /* Format next blocks */
    if (rw_mfc_formatBlock(p_mfc->next_block.block) != NFC_STATUS_OK) {
      evt_data.status = NFC_STATUS_FAILED;
      evt_data.p_data = NULL;
      (*rw_cb.p_cback)(RW_MFC_NDEF_FORMAT_CPLT_EVT, (tRW_DATA*)&evt_data);
    }
  } else {
    evt_data.status = NFC_STATUS_OK;
    evt_data.p_data = NULL;
    rw_mfc_handle_op_complete();
    (*rw_cb.p_cback)(RW_MFC_NDEF_FORMAT_CPLT_EVT, (tRW_DATA*)&evt_data);
  }
}

/*******************************************************************************
**
** Function         RW_MfcWriteNDef
**
** Description      This function can be called to write an NDEF message to the
**                  tag.
**
** Parameters:      buf_len:    The length of the buffer
**                  p_buffer:   The NDEF message to write
**
** Returns          NCI_STATUS_OK, if write was started. Otherwise, error
**                  status.
**
*******************************************************************************/
tNFC_STATUS RW_MfcWriteNDef(uint16_t buf_len, uint8_t* p_buffer) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tNFC_STATUS status = NFC_STATUS_OK;

  if (p_mfc->state != RW_MFC_STATE_IDLE) {
    return NFC_STATUS_BUSY;
  }

  p_mfc->state = RW_MFC_STATE_UPDATE_NDEF;
  p_mfc->substate = RW_MFC_SUBSTATE_NONE;
  p_mfc->last_block_accessed.block = 4;
  p_mfc->next_block.block = 4;

  p_mfc->p_ndef_buffer = p_buffer;
  p_mfc->ndef_length = buf_len;
  p_mfc->work_offset = 0;

  status = rw_mfc_writeBlock(p_mfc->next_block.block);
  if (status == NFC_STATUS_OK) {
    p_mfc->state = RW_MFC_STATE_UPDATE_NDEF;
  } else {
    p_mfc->substate = RW_MFC_SUBSTATE_NONE;
  }

  return status;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_writeBlock
 **
 ** Description      This function write a given block.
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static tNFC_STATUS rw_mfc_writeBlock(int block) {
  NFC_HDR* mfcbuf;
  uint8_t* p;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  int sectorlength = block / 4;
  tNFC_STATUS status = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: block : %d", __func__, block);

  if (block > 128) {
    sectorlength = (p_mfc->next_block.block - 128) / 16 + 32;
  }

  if (sectorlength != p_mfc->sector_authentified) {
    if (rw_mfc_authenticate(block, true) == true) {
      return NFC_STATUS_OK;
    }
    return NFC_STATUS_FAILED;
  }

  mfcbuf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!mfcbuf) {
    LOG(ERROR) << StringPrintf("%s: Cannot allocate buffer",__func__);
    return NFC_STATUS_REJECTED;
  }

  mfcbuf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(mfcbuf + 1) + mfcbuf->offset;

  UINT8_TO_BE_STREAM(p, MFC_Write);
  UINT8_TO_BE_STREAM(p, block);
  int index = 0;
  while (index < RW_MFC_1K_BLOCK_SIZE) {
    if (p_mfc->work_offset == 0) {
      if (p_mfc->ndef_length < 0xFF) {
        UINT8_TO_BE_STREAM(p, 0x03);
        UINT8_TO_BE_STREAM(p, p_mfc->ndef_length);
        index = index + 2;
      } else {
        UINT8_TO_BE_STREAM(p, 0x03);
        UINT8_TO_BE_STREAM(p, 0xFF);
        UINT8_TO_BE_STREAM(p, (uint8_t)(p_mfc->ndef_length >>8));
        UINT8_TO_BE_STREAM(p, (uint8_t)(p_mfc->ndef_length & 0xFF));
        index = index + 4;
      }
    }

    if (p_mfc->work_offset == p_mfc->ndef_length) {
      UINT8_TO_BE_STREAM(p, 0xFE);
    } else if (p_mfc->work_offset > p_mfc->ndef_length) {
      UINT8_TO_BE_STREAM(p, 0x00);
    } else {
      UINT8_TO_BE_STREAM(p, p_mfc->p_ndef_buffer[p_mfc->work_offset]);
    }
    p_mfc->work_offset++;
    index++;
  }
  mfcbuf->len = 18;

  if (!rw_mfc_send_to_lower(mfcbuf)) {
    return NFC_STATUS_REJECTED;
  }
  p_mfc->current_block = block;
  p_mfc->substate = RW_MFC_SUBSTATE_WRITE_BLOCK;

  return status;
}

static void rw_mfc_handle_write_rsp(uint8_t* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  NFC_HDR* mfc_data;
  uint8_t* p;

  mfc_data = (NFC_HDR*)p_data;
  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;

  switch (p_mfc->substate) {
    case RW_MFC_SUBSTATE_WAIT_ACK:
      p_mfc->last_block_accessed.block = p_mfc->current_block;

      if (p[0] == 0x0) {
        p_mfc->next_block.auth = true;
        p_mfc->last_block_accessed.auth = true;

        if (p_mfc->next_block.block < 128) {
          p_mfc->sector_authentified = p_mfc->next_block.block / 4;
        } else {
          p_mfc->sector_authentified =
              (p_mfc->next_block.block - 128) / 16 + 32;
        }
        rw_mfc_resume_op();
      } else {
        p_mfc->next_block.auth = false;
        p_mfc->last_block_accessed.auth = false;
        nfc_stop_quick_timer(&p_mfc->timer);
        rw_mfc_process_error();
      }
      break;

    case RW_MFC_SUBSTATE_WRITE_BLOCK:
      if (p[0] == 0x0) {
        rw_mfc_handle_write_op();
      } else {
        nfc_stop_quick_timer(&p_mfc->timer);
        rw_mfc_process_error();
      }
      break;
  }
}

static void rw_mfc_handle_write_op() {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tRW_READ_DATA evt_data;

  if (p_mfc->work_offset >= p_mfc->ndef_length) {
    evt_data.status = NFC_STATUS_OK;
    evt_data.p_data = NULL;
    (*rw_cb.p_cback)(RW_MFC_NDEF_WRITE_CPLT_EVT, (tRW_DATA*)&evt_data);
  } else {
    p_mfc->last_block_accessed.block = p_mfc->current_block;

    if (p_mfc->current_block % 4 == 2) {
      p_mfc->next_block.block = p_mfc->current_block + 2;
    } else {
      p_mfc->next_block.block = p_mfc->current_block + 1;
    }

    /* Do not read block 16 (MAD2) - Mifare Classic4 k */
    if (p_mfc->next_block.block == 64) {
      p_mfc->next_block.block += 4;
    }

    if ((p_mfc->selres & RW_MFC_4K_Support) &&
        (p_mfc->next_block.block >= 128)) {
      if (p_mfc->current_block % 16 == 14) {
        p_mfc->next_block.block = p_mfc->current_block + 2;
      } else {
        p_mfc->next_block.block = p_mfc->current_block + 1;
      }
    }

    /* Write next blocks */
    if (rw_mfc_writeBlock(p_mfc->next_block.block) != NFC_STATUS_OK) {
      evt_data.status = NFC_STATUS_FAILED;
      evt_data.p_data = NULL;
      (*rw_cb.p_cback)(RW_MFC_NDEF_WRITE_FAIL_EVT, (tRW_DATA*)&evt_data);
    }
  }
}

/*****************************************************************************
 **
 ** Function         RW_MfcDetectNDef
 **
 ** Description
 **      This function is used to perform NDEF detection on a Mifare Classic
 **      tag, and retrieve the tag's NDEF attribute information.
 **      Before using this API, the application must call RW_SelectTagType to
 **      indicate that a Type 1 tag has been activated.
 **
 ** Returns
 **      NFC_STATUS_OK: ndef detection procedure started
 **      NFC_STATUS_WRONG_PROTOCOL: type 1 tag not activated
 **      NFC_STATUS_BUSY: another command is already in progress
 **      NFC_STATUS_FAILED: other error
 **
 *****************************************************************************/
tNFC_STATUS RW_MfcDetectNDef(void) {
  LOG(ERROR) << __func__;
  return rw_MfcLocateTlv(TAG_NDEF_TLV);
}

/*******************************************************************************
 **
 ** Function         rw_mfc_select
 **
 ** Description      This function will set the callback function to
 **                  receive data from lower layers.
 **
 ** Returns          tNFC_STATUS
 **
 *******************************************************************************/
tNFC_STATUS rw_mfc_select(uint8_t selres, uint8_t uid[MFC_UID_LEN]) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;

  p_mfc->state = RW_MFC_STATE_NOT_ACTIVATED;

  /* Alloc cmd buf for retransmissions */
  if (p_mfc->p_cur_cmd_buf == NULL) {
    DLOG_IF(INFO, nfc_debug_enabled) << __func__;
    p_mfc->p_cur_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_mfc->p_cur_cmd_buf == NULL) {
      LOG(ERROR) << StringPrintf
      ("%s: unable to allocate buffer for retransmission",__func__);

      return NFC_STATUS_FAILED;
    }
  }
  p_mfc->selres = selres;
  memcpy(p_mfc->uid, uid, MFC_UID_LEN);

  NFC_SetStaticRfCback(rw_mfc_conn_cback);

  p_mfc->state = RW_MFC_STATE_IDLE;
  p_mfc->substate = RW_MFC_SUBSTATE_IDLE;
  p_mfc->last_block_accessed.block = -1;
  p_mfc->last_block_accessed.auth = false;
  p_mfc->next_block.block = 4;
  p_mfc->next_block.auth = false;
  p_mfc->sector_authentified = -1;

  return NFC_STATUS_OK;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_send_to_lower
 **
 ** Description      Send C-APDU to lower layer
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static bool rw_mfc_send_to_lower(NFC_HDR* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  /* Indicate first attempt to send command, back up cmd buffer in case needed
   * for retransmission */
  rw_cb.cur_retry = 0;
  memcpy(p_mfc->p_cur_cmd_buf, p_data,
         sizeof(NFC_HDR) + p_data->offset + p_data->len);

  if (NFC_SendData(NFC_RF_CONN_ID, p_data) != NFC_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: NFC_SendData () failed",__func__);
    return false;
  }

  nfc_start_quick_timer(&rw_cb.tcb.mfc.timer, NFC_TTYPE_RW_MFC_RESPONSE,
                        (RW_MFC_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

  return true;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_process_timeout
 **
 ** Description      handles timeout event
 **
 ** Returns          none
 **
 *******************************************************************************/
void rw_mfc_process_timeout(TIMER_LIST_ENT* p_tle) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: event = %d", __func__, p_tle->event);

  if (p_tle->event == NFC_TTYPE_RW_MFC_RESPONSE) {
    rw_mfc_process_error();
  } else {
    LOG(ERROR) << StringPrintf("%s: unknown event=%d",__func__, p_tle->event);
  }
}

/*******************************************************************************
 **
 ** Function         rw_mfc_conn_cback
 **
 ** Description      This callback function receives the events/data from NFCC.
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tRW_READ_DATA evt_data;
  NFC_HDR* mfc_data = nullptr;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s conn_id=%i, evt=0x%x", __func__, conn_id, event);
  /* Only handle static conn_id */
  if (conn_id != NFC_RF_CONN_ID) {
    LOG(ERROR) << StringPrintf("%s: Not static connection id =%d",__func__, conn_id);
    return;
  }

  switch (event) {
    case NFC_CONN_CREATE_CEVT:
    case NFC_CONN_CLOSE_CEVT:
      break;

    case NFC_DEACTIVATE_CEVT:

      /* Stop mfc timer (if started) */
      nfc_stop_quick_timer(&p_mfc->timer);
      /* Free cmd buf for retransmissions */
      if (p_mfc->p_cur_cmd_buf) {
        GKI_freebuf(p_mfc->p_cur_cmd_buf);
        p_mfc->p_cur_cmd_buf = NULL;
      }

      p_mfc->state = RW_MFC_STATE_NOT_ACTIVATED;
      NFC_SetStaticRfCback(NULL);
      break;

    case NFC_DATA_CEVT:
      if ((p_data != NULL) && (p_data->data.status == NFC_STATUS_OK)) {
        mfc_data = (NFC_HDR*)p_data->data.p_data;
        break;
      }
      /* Data event with error status...fall through to NFC_ERROR_CEVT case */
      FALLTHROUGH_INTENDED;
    case NFC_ERROR_CEVT:

      if ((p_mfc->state == RW_MFC_STATE_NOT_ACTIVATED) ||
          (p_mfc->state == RW_MFC_STATE_IDLE)) {
        if (event == NFC_ERROR_CEVT) {
          evt_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        } else if (p_data) {
          evt_data.status = p_data->status;
        } else {
          evt_data.status = NFC_STATUS_FAILED;
        }

        evt_data.p_data = NULL;
        (*rw_cb.p_cback)(RW_MFC_INTF_ERROR_EVT, (tRW_DATA*)&evt_data);
        break;
      }
      nfc_stop_quick_timer(&p_mfc->timer);
      break;

    default:
      break;
  }

  switch (p_mfc->state) {
    case RW_MFC_STATE_IDLE:
      /* Unexpected R-APDU, it should be raw frame response */
      /* forward to upper layer without parsing */
      if (rw_cb.p_cback) {
        rw_data.raw_frame.status = p_data->data.status;
        rw_data.raw_frame.p_data = mfc_data;
        (*(rw_cb.p_cback))(RW_MFC_RAW_FRAME_EVT, &rw_data);
        mfc_data = NULL;
      } else {
        GKI_freebuf(mfc_data);
      }
      break;
    case RW_MFC_STATE_DETECT_TLV:
      rw_mfc_handle_tlv_detect_rsp((uint8_t*)mfc_data);
      GKI_freebuf(mfc_data);
      break;

    case RW_MFC_STATE_READ_NDEF:
      rw_mfc_handle_ndef_read_rsp((uint8_t*)mfc_data);
      GKI_freebuf(mfc_data);
      break;
    case RW_MFC_STATE_NOT_ACTIVATED:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s RW_MFC_STATE_NOT_ACTIVATED", __func__);
      /* p_r_apdu may send upper layer */
      break;
    case RW_MFC_STATE_NDEF_FORMAT:
      rw_mfc_handle_format_rsp((uint8_t*)mfc_data);
      GKI_freebuf(mfc_data);
      break;
    case RW_MFC_STATE_UPDATE_NDEF:
      rw_mfc_handle_write_rsp((uint8_t*)mfc_data);
      GKI_freebuf(mfc_data);
      break;
    default:
      LOG(ERROR) << StringPrintf("%s: invalid state=%d",__func__, p_mfc->state);
      break;
  }
}

/*******************************************************************************
 **
 ** Function         rw_MfcLocateTlv
 **
 ** Description      This function performs NDEF detection procedure
 **
 **                  RW_MFC_NDEF_DETECT_EVT will be returned
 **
 ** Returns          NFC_STATUS_OK if success
 **                  NFC_STATUS_FAILED if Mifare classic tag is busy or other
 **                  error
 **
 *******************************************************************************/
static tNFC_STATUS rw_MfcLocateTlv(uint8_t tlv_type) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tNFC_STATUS success = NFC_STATUS_OK;

  if (p_mfc->state != RW_MFC_STATE_IDLE) {
    LOG(ERROR) << StringPrintf
    ("%s: Mifare Classic tag not activated or Busy - State:%d",
     __func__, p_mfc->state);
    return NFC_STATUS_BUSY;
  }

  if ((tlv_type != TAG_LOCK_CTRL_TLV) && (tlv_type != TAG_MEM_CTRL_TLV) &&
      (tlv_type != TAG_NDEF_TLV) && (tlv_type != TAG_PROPRIETARY_TLV)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s - Cannot search TLV: 0x%02x", __func__, tlv_type);
    return NFC_STATUS_FAILED;
  }
  if (tlv_type == TAG_NDEF_TLV) {
    p_mfc->ndef_length = 0;
    p_mfc->ndef_start_pos = 0;
    p_mfc->ndef_first_block = 0;
    p_mfc->work_offset = 0;
    p_mfc->ndef_status = MFC_NDEF_NOT_DETECTED;
  }

  p_mfc->substate = RW_MFC_SUBSTATE_READ_BLOCK;
  p_mfc->state = RW_MFC_STATE_DETECT_TLV;

  success = rw_mfc_readBlock(p_mfc->next_block.block);
  if (success == NFC_STATUS_OK) {
    p_mfc->state = RW_MFC_STATE_DETECT_TLV;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: RW_MFC_STATE_DETECT_TLV state=  %d", __func__, p_mfc->state);
  } else {
    p_mfc->substate = RW_MFC_SUBSTATE_NONE;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: rw_MfcLocateTlv state= %d", __func__, p_mfc->state);
  }

  return NFC_STATUS_OK;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_authenticate
 **
 ** Description      This function performs the authentication of a given
 **                  block.
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static bool rw_mfc_authenticate(int block, bool KeyA) {
  NFC_HDR* mfcbuf;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: block: %d", __func__, block);

  uint8_t* KeyToUse;

  mfcbuf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!mfcbuf) {
    LOG(ERROR) << StringPrintf("%s: Cannot allocate buffer",__func__);
    return false;
  }

  mfcbuf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(mfcbuf + 1) + mfcbuf->offset;

  if (KeyA) {
    UINT8_TO_BE_STREAM(p, MFC_KeyA);
  } else {
    UINT8_TO_BE_STREAM(p, MFC_KeyB);
  }

  UINT8_TO_BE_STREAM(p, block);
  ARRAY_TO_BE_STREAM(p, p_mfc->uid, 4);

  if (p_mfc->state == RW_MFC_STATE_NDEF_FORMAT)
    KeyToUse = KeyDefault;
  else {
    if (block >= 0 && block < 4) {
      KeyToUse = KeyMAD;
    } else {
      KeyToUse = KeyNDEF;
    }
  }
  ARRAY_TO_BE_STREAM(p, KeyToUse, 6);

  mfcbuf->len = 12;

  if (!rw_mfc_send_to_lower(mfcbuf)) {
    return false;
  }
  /* Backup the current substate to move back to this substate after changing
   * sector */
  p_mfc->prev_substate = p_mfc->substate;
  p_mfc->substate = RW_MFC_SUBSTATE_WAIT_ACK;
  return true;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_readBlock
 **
 ** Description      This function read a given block.
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static tNFC_STATUS rw_mfc_readBlock(int block) {
  NFC_HDR* mfcbuf;
  uint8_t* p;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  int sectorlength = block / 4;
  tNFC_STATUS status = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: block : %d", __func__,block);

  if (block > 128) {
    sectorlength = (p_mfc->next_block.block - 128) / 16 + 32;
  }

  if (sectorlength != p_mfc->sector_authentified) {
    if (rw_mfc_authenticate(block, true) == true) {
      LOG(ERROR) << StringPrintf("%s: RW_MFC_SUBSTATE_WAIT_ACK",__func__);
      return NFC_STATUS_OK;
    }
    return NFC_STATUS_FAILED;
  }

  mfcbuf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!mfcbuf) {
    LOG(ERROR) << StringPrintf("%s: Cannot allocate buffer",__func__);
    return NFC_STATUS_REJECTED;
  }

  mfcbuf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(mfcbuf + 1) + mfcbuf->offset;

  UINT8_TO_BE_STREAM(p, MFC_Read);
  UINT8_TO_BE_STREAM(p, block);

  mfcbuf->len = 2;

  if (!rw_mfc_send_to_lower(mfcbuf)) {
    return NFC_STATUS_REJECTED;
  }
  p_mfc->current_block = block;
  p_mfc->substate = RW_MFC_SUBSTATE_READ_BLOCK;

  return status;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_handle_tlv_detect_rsp
 **
 ** Description      Handle TLV detection.
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_handle_tlv_detect_rsp(uint8_t* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  NFC_HDR* mfc_data;
  uint8_t* p;

  mfc_data = (NFC_HDR*)p_data;
  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;

  p_mfc->last_block_accessed.block = p_mfc->next_block.block;
  switch (p_mfc->substate) {
    case RW_MFC_SUBSTATE_WAIT_ACK:
      /* Search for the tlv */
      if (p[0] == 0x0) {
        p_mfc->next_block.auth = true;
        p_mfc->last_block_accessed.auth = true;
        p_mfc->sector_authentified = p_mfc->next_block.block / 4;

        rw_mfc_resume_op();
      } else {
        p_mfc->next_block.auth = false;
        p_mfc->last_block_accessed.auth = false;
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: status= %d", __func__, p[0]);
        nfc_stop_quick_timer(&p_mfc->timer);
        rw_mfc_process_error();
      }
      break;

    case RW_MFC_SUBSTATE_READ_BLOCK:
      /* Search for the tlv */
      if (mfc_data->len == 0x10) {
        p_mfc->last_block_accessed.block = p_mfc->next_block.block;
        p_mfc->next_block.block += 1;
        p_mfc->next_block.auth = false;
        rw_mfc_handle_read_op((uint8_t*)mfc_data);
      }
      break;
  }
}
/*******************************************************************************
 **
 ** Function         rw_mfc_resume_op
 **
 ** Description      This function will continue operation after moving to new
 **                  sector
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_resume_op() {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  bool status = true;

  switch (p_mfc->state) {
    case RW_MFC_STATE_DETECT_TLV:
      status = rw_mfc_readBlock(p_mfc->next_block.block);
      break;
    case RW_MFC_STATE_READ_NDEF:
      status = rw_mfc_readBlock(p_mfc->next_block.block);
      break;
    case RW_MFC_STATE_NDEF_FORMAT:
      status = rw_mfc_formatBlock(p_mfc->next_block.block);
      break;
    case RW_MFC_STATE_UPDATE_NDEF:
      status = rw_mfc_writeBlock(p_mfc->next_block.block);
      break;
  }
}

/*******************************************************************************
 **
 ** Function         rw_mfc_handle_read_op
 **
 ** Description      This function handles all the read operation.
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_handle_read_op(uint8_t* data) {
  uint8_t* p;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  bool tlv_found = false;
  NFC_HDR* mfc_data;
  uint16_t len;
  uint16_t offset;
  bool failed = false;
  bool done = false;
  tRW_READ_DATA evt_data;

  mfc_data = (NFC_HDR*)data;
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;

  switch (p_mfc->state) {
    case RW_MFC_STATE_DETECT_TLV:
      tlv_found = rw_nfc_decodeTlv(data);
      if (tlv_found) {
        p_mfc->ndef_status = MFC_NDEF_DETECTED;
        p_mfc->ndef_first_block = p_mfc->last_block_accessed.block;
        rw_mfc_ntf_tlv_detect_complete(tlv_found);
      }
      break;

    case RW_MFC_STATE_READ_NDEF:
      /* On the first read, adjust for any partial block offset */
      offset = 0;
      len = RW_MFC_1K_BLOCK_SIZE;

      if (p_mfc->work_offset == 0) {
        /* The Ndef Message offset may be present in the read 16 bytes */
        offset = p_mfc->ndef_start_pos;

        if (!rw_nfc_decodeTlv(data)) {
          failed = true;
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf
          ("%s FAILED finding TLV", __func__);
        }
      }

      /* Skip all reserved and lock bytes */
      while ((offset < len) && (p_mfc->work_offset < p_mfc->ndef_length))

      {
        /* Collect the NDEF Message */
        p_mfc->p_ndef_buffer[p_mfc->work_offset] = p[offset];
        p_mfc->work_offset++;
        offset++;
      }

      if (p_mfc->work_offset >= p_mfc->ndef_length) {
        done = true;
        p_mfc->ndef_status = MFC_NDEF_READ;
      } else {
        /* Read next  blocks */
        if (rw_mfc_readBlock(p_mfc->next_block.block) != NFC_STATUS_OK) {
          failed = true;
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: FAILED reading next", __func__);
        }
      }

      if (failed || done) {
        evt_data.status = failed ? NFC_STATUS_FAILED : NFC_STATUS_OK;
        evt_data.p_data = NULL;
        rw_mfc_handle_op_complete();
        (*rw_cb.p_cback)(RW_MFC_NDEF_READ_EVT, (tRW_DATA*)&evt_data);
      }
      break;
  }
}
/*******************************************************************************
 **
 ** Function         rw_nfc_decodeTlv
 **
 ** Description      Decode the NDEF data length from the Mifare TLV
 **                  Leading null TLVs (0x0) are skipped
 **
 ** Returns          true if success
 **
 *******************************************************************************/
static bool rw_nfc_decodeTlv(uint8_t* data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  NFC_HDR* mfc_data;
  uint8_t* p;

  mfc_data = (NFC_HDR*)data;
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;
  int i = 0;

  do {
    if (p[i] == 0x0) {
      // do nothing, skip
    } else if (p[i] == 0x3) {
      p_mfc->tlv_detect = TAG_NDEF_TLV;
      break;

    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Unknown TLV", __func__);
      p_mfc->tlv_detect = TAG_PROPRIETARY_TLV;
      break;
    }
    i++;
  } while (i < mfc_data->len);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: i= %d", __func__, i);

  if ((i + 1) >= mfc_data->len || i < 0 || p[i] != 0x3) {
    LOG(ERROR) << StringPrintf("%s: Can't decode message length",__func__);
  } else {
    if (p[i + 1] != 0xFF) {
      p_mfc->ndef_length = p[i + 1];
      p_mfc->ndef_start_pos = i + RW_MFC_SHORT_TLV_SIZE;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: short NDEF SIZE = %d", __func__, p_mfc->ndef_length);
      return true;
    } else if ((i + 3) < mfc_data->len) {
      p_mfc->ndef_length = (((uint16_t)p[i + 2]) << 8) | ((uint16_t)(p[i + 3]));
      p_mfc->ndef_start_pos = i + RW_MFC_LONG_TLV_SIZE;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s:long NDEF SIZE = %d", __func__, p_mfc->ndef_length);
      return true;
    } else {
      LOG(ERROR) << StringPrintf("%s: Can't decode ndef length", __func__);
    }
  }
  return false;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_ntf_tlv_detect_complete
 **
 ** Description      Notify TLV detection complete to upper layer
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_ntf_tlv_detect_complete(tNFC_STATUS status) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tRW_DETECT_NDEF_DATA ndef_data = {};

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  if (p_mfc->tlv_detect == TAG_NDEF_TLV) {
    /* Notify upper layer the result of NDEF detect op */
    ndef_data.status = NFC_STATUS_OK;  // status;
    ndef_data.protocol = NFC_PROTOCOL_MIFARE;
    ndef_data.flags = 0;
    ndef_data.cur_size = p_mfc->ndef_length;

    if (status == NFC_STATUS_OK) {
      ndef_data.flags |= RW_NDEF_FL_FORMATED;
    }

    // TODO - calculate max size based on MAD sectr NFC_AID condition
    // Set max size as format condition
    if (p_mfc->selres & RW_MFC_4K_Support)
      ndef_data.max_size = 3356;
    else
      ndef_data.max_size = 716;

    rw_mfc_handle_op_complete();
    (*rw_cb.p_cback)(RW_MFC_NDEF_DETECT_EVT, (tRW_DATA*)&ndef_data);
  }
}

/*******************************************************************************
 **
 ** Function         RW_MfcReadNDef
 **
 ** Description      Retrieve NDEF contents from a Mifare Classic tag..
 **
 **                  The RW_MFC_NDEF_READ_EVT event is used to notify the
 **                  application after reading the NDEF message.
 **
 **                  Before using this API, the RW_MfcReadNDef function must
 **                  be called to verify that the tag contains NDEF data, and to
 **                  retrieve the NDEF attributes.
 **
 **                  Internally, this command will be separated into multiple
 **                  Mifare Classic Read commands (if necessary) - depending
 **                  on the NDEF Msg size.
 **
 ** Parameters:      p_buffer:   The buffer into which to read the NDEF message
 **                  buf_len:    The length of the buffer
 **
 ** Returns          NCI_STATUS_OK, if read was started. Otherwise, error
 **                  status.
 **
 *******************************************************************************/
tNFC_STATUS RW_MfcReadNDef(uint8_t* p_buffer, uint16_t buf_len) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tNFC_STATUS status = NFC_STATUS_OK;

  if (p_mfc->state != RW_MFC_STATE_IDLE) {
    LOG(ERROR) << StringPrintf
    ("%s: Mifare Classic tag not activated or Busy - State=%d",
     __func__, p_mfc->state);
    return NFC_STATUS_FAILED;
  }

  if (p_mfc->ndef_status == MFC_NDEF_NOT_DETECTED) {
    LOG(ERROR) << StringPrintf("%s: NDEF detection not performed yet",__func__);
    return NFC_STATUS_FAILED;
  }

  if (buf_len < p_mfc->ndef_length) {
    LOG(ERROR) << StringPrintf
    ("%s:  buffer size==%d less than NDEF msg sise=%d",
     __func__, buf_len, p_mfc->ndef_length);
    return NFC_STATUS_FAILED;
  }

  if (!p_mfc->ndef_length) {
    LOG(ERROR) << StringPrintf("%s: NDEF Message length is zero ",__func__);
    return NFC_STATUS_NOT_INITIALIZED;
  }

  p_mfc->p_ndef_buffer = p_buffer;
  p_mfc->work_offset = 0;

  p_mfc->last_block_accessed.block = 0;
  p_mfc->next_block.block = p_mfc->ndef_first_block;
  p_mfc->substate = RW_MFC_SUBSTATE_NONE;

  /* Start reading NDEF Message */
  status = rw_mfc_readBlock(p_mfc->next_block.block);
  if (status == NFC_STATUS_OK) {
    p_mfc->state = RW_MFC_STATE_READ_NDEF;
  }

  return status;
}

/*****************************************************************************
 **
 ** Function         rw_mfc_handle_op_complete
 **
 ** Description      Reset to IDLE state
 **
 ** Returns          none
 **
 *****************************************************************************/
static void rw_mfc_handle_op_complete(void) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;

  p_mfc->last_block_accessed.auth = false;
  p_mfc->next_block.auth = false;
  p_mfc->state = RW_MFC_STATE_IDLE;
  p_mfc->substate = RW_MFC_SUBSTATE_NONE;
  return;
}

/*******************************************************************************
 **
 ** Function         rw_mfc_handle_ndef_read_rsp
 **
 ** Description      Handle TLV detection.
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_handle_ndef_read_rsp(uint8_t* p_data) {
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  NFC_HDR* mfc_data;
  uint8_t* p;

  mfc_data = (NFC_HDR*)p_data;
  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(mfc_data + 1) + mfc_data->offset;

  switch (p_mfc->substate) {
    case RW_MFC_SUBSTATE_WAIT_ACK:
      /* Search for the tlv */
      p_mfc->last_block_accessed.block = p_mfc->current_block;

      if (p[0] == 0x0) {
        p_mfc->next_block.auth = true;
        p_mfc->last_block_accessed.auth = true;

        if (p_mfc->current_block < 128) {
          p_mfc->sector_authentified = p_mfc->next_block.block / 4;
        }

        else
          p_mfc->sector_authentified =
              (p_mfc->next_block.block - 128) / 16 + 32;

        rw_mfc_resume_op();
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "rw_mfc_handle_ndef_read_rsp () sector authentified: %d",
            p_mfc->sector_authentified);
      } else {
        p_mfc->next_block.auth = false;
        p_mfc->last_block_accessed.auth = false;
      }
      break;

    case RW_MFC_SUBSTATE_READ_BLOCK:
      /* Search for the tlv */

      if (mfc_data->len == 0x10) {
        p_mfc->last_block_accessed.block = p_mfc->current_block;

        if (p_mfc->current_block % 4 == 2) {
          p_mfc->next_block.block = p_mfc->current_block + 2;
        } else {
          p_mfc->next_block.block = p_mfc->current_block + 1;
        }

        /* Do not read block 16 (MAD2) - Mifare Classic4 k */
        if (p_mfc->next_block.block == 64) {
          p_mfc->next_block.block += 4;
        }

        if ((p_mfc->selres & RW_MFC_4K_Support) &&
            (p_mfc->next_block.block >= 128)) {
          if (p_mfc->current_block % 16 == 14) {
            p_mfc->next_block.block = p_mfc->current_block + 2;
          } else {
            p_mfc->next_block.block = p_mfc->current_block + 1;
          }
        }

        p_mfc->next_block.auth = false;
        rw_mfc_handle_read_op((uint8_t*)mfc_data);
      }
      break;
  }
}

/*******************************************************************************
 **
 ** Function         rw_mfc_process_error
 **
 ** Description      Process error including Timeout, Frame error. This function
 **                  will retry atleast till RW_MAX_RETRIES before give up and
 **                  sending negative notification to upper layer
 **
 ** Returns          none
 **
 *******************************************************************************/
static void rw_mfc_process_error() {
  tRW_READ_DATA evt_data = tRW_READ_DATA();
  tRW_EVENT rw_event = RW_MFC_NDEF_DETECT_EVT;
  NFC_HDR* p_cmd_buf;
  tRW_MFC_CB* p_mfc = &rw_cb.tcb.mfc;
  tRW_DETECT_NDEF_DATA ndef_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: State = %d",\
          __func__, p_mfc->state);
  evt_data.status = NFC_STATUS_FAILED;

  /* Retry sending command if retry-count < max */
  if (rw_cb.cur_retry < RW_MAX_RETRIES) {
    /* retry sending the command */
    rw_cb.cur_retry++;

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Mifare Classic retransmission attempt  %d  of %d",\
                __func__, rw_cb.cur_retry, RW_MAX_RETRIES );

    /* allocate a new buffer for message */
    p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_cmd_buf != NULL) {
      memcpy(p_cmd_buf, p_mfc->p_cur_cmd_buf,
             sizeof(NFC_HDR) + p_mfc->p_cur_cmd_buf->offset +
                 p_mfc->p_cur_cmd_buf->len);

      if (NFC_SendData(NFC_RF_CONN_ID, p_cmd_buf) == NFC_STATUS_OK) {
        /* Start timer for waiting for response */
        nfc_start_quick_timer(
            &p_mfc->timer, NFC_TTYPE_RW_MFC_RESPONSE,
            (RW_MFC_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

        return;
      }
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s:MFC maximum retransmission attempts reached  %d",\
                __func__, RW_MAX_RETRIES);
  }

  if (p_mfc->state == RW_MFC_STATE_DETECT_TLV) {
    rw_event = RW_MFC_NDEF_DETECT_EVT;
  } else if (p_mfc->state == RW_MFC_STATE_READ_NDEF) {
    rw_event = RW_MFC_NDEF_READ_EVT;
  } else if (p_mfc->state == RW_MFC_STATE_UPDATE_NDEF) {
    rw_event = RW_MFC_NDEF_WRITE_FAIL_EVT;
  } else if (p_mfc->state == RW_MFC_STATE_NDEF_FORMAT) {
    rw_event = RW_MFC_NDEF_FORMAT_CPLT_EVT;
  }

  if (rw_event == RW_MFC_NDEF_DETECT_EVT) {
    ndef_data.status = evt_data.status;
    ndef_data.protocol = NFC_PROTOCOL_MIFARE;
    ndef_data.flags = RW_NDEF_FL_UNKNOWN;
    ndef_data.max_size = 0;
    ndef_data.cur_size = 0;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: status= %d", __func__, evt_data.status);
    /* If not Halt move to idle state */
    rw_mfc_handle_op_complete();

    (*rw_cb.p_cback)(rw_event, (tRW_DATA*)&ndef_data);
  } else {
    evt_data.p_data = NULL;
    /* If activated and not Halt move to idle state */
    if (p_mfc->state != RW_MFC_STATE_NOT_ACTIVATED) {
      rw_mfc_handle_op_complete();
    }

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: status= %d", __func__, evt_data.status);
    p_mfc->substate = RW_MFC_SUBSTATE_NONE;
    (*rw_cb.p_cback)(rw_event, (tRW_DATA*)&evt_data);
  }
}
