/******************************************************************************
 *
 *  Copyright (C) 2011-2014 Broadcom Corporation
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
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2020 NXP Semiconductors
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

/******************************************************************************
 *
 *  This file contains the implementation for ISO 15693 in Reader/Writer
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

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* Response timeout     */
#define RW_I93_TOUT_RESP 1000
/* stay quiet timeout   */
#define RW_I93_TOUT_STAY_QUIET 200
/* max reading data if read multi block is supported */
#define RW_I93_READ_MULTI_BLOCK_SIZE 128
/* CC, zero length NDEF, Terminator TLV              */
#define RW_I93_FORMAT_DATA_LEN 8
/* max getting lock status if get multi block sec is supported */
#define RW_I93_GET_MULTI_BLOCK_SEC_SIZE 253
/*Capability Container CC Size */
#define RW_I93_CC_SIZE 4

/* main state */
enum {
  RW_I93_STATE_NOT_ACTIVATED, /* ISO15693 is not activated            */
  RW_I93_STATE_IDLE,          /* waiting for upper layer API          */
  RW_I93_STATE_BUSY,          /* waiting for response from tag        */

  RW_I93_STATE_DETECT_NDEF,   /* performing NDEF detection precedure  */
  RW_I93_STATE_READ_NDEF,     /* performing read NDEF procedure       */
  RW_I93_STATE_UPDATE_NDEF,   /* performing update NDEF procedure     */
  RW_I93_STATE_FORMAT,        /* performing format procedure          */
  RW_I93_STATE_SET_READ_ONLY, /* performing set read-only procedure   */

  RW_I93_STATE_PRESENCE_CHECK /* checking presence of tag             */
};

/* sub state */
enum {
  RW_I93_SUBSTATE_WAIT_UID,          /* waiting for response of inventory    */
  RW_I93_SUBSTATE_WAIT_SYS_INFO,     /* waiting for response of get sys info */
  RW_I93_SUBSTATE_WAIT_CC,           /* waiting for reading CC               */
  RW_I93_SUBSTATE_SEARCH_NDEF_TLV,   /* searching NDEF TLV                   */
  RW_I93_SUBSTATE_CHECK_LOCK_STATUS, /* check if any NDEF TLV is locked      */

  RW_I93_SUBSTATE_RESET_LEN,  /* set length to 0 to update NDEF TLV   */
  RW_I93_SUBSTATE_WRITE_NDEF, /* writing NDEF and Terminator TLV      */
  RW_I93_SUBSTATE_UPDATE_LEN, /* set length into NDEF TLV             */

  RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI, /* reset DSFID and AFI */
  RW_I93_SUBSTATE_CHECK_READ_ONLY,   /* check if any block is locked         */
  RW_I93_SUBSTATE_WRITE_CC_NDEF_TLV, /* write CC and empty NDEF/Terminator TLV
                                        */

  RW_I93_SUBSTATE_WAIT_UPDATE_CC, /* updating CC as read-only             */
  RW_I93_SUBSTATE_LOCK_NDEF_TLV,  /* lock blocks of NDEF TLV              */
  RW_I93_SUBSTATE_WAIT_LOCK_CC    /* lock block of CC                     */
};

static std::string rw_i93_get_state_name(uint8_t state);
static std::string rw_i93_get_sub_state_name(uint8_t sub_state);
static std::string rw_i93_get_tag_name(uint8_t product_version);

static void rw_i93_data_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data);
void rw_i93_handle_error(tNFC_STATUS status);
tNFC_STATUS rw_i93_send_cmd_get_sys_info(uint8_t* p_uid, uint8_t extra_flag);
tNFC_STATUS rw_i93_send_cmd_get_ext_sys_info(uint8_t* p_uid);

/*******************************************************************************
**
** Function         rw_i93_get_product_version
**
** Description      Get product version from UID
**
** Returns          void
**
*******************************************************************************/
void rw_i93_get_product_version(uint8_t* p_uid) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;

  if (!memcmp(p_i93->uid, p_uid, I93_UID_BYTE_LEN)) {
    return;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  memcpy(p_i93->uid, p_uid, I93_UID_BYTE_LEN);

  if (p_uid[1] == I93_UID_IC_MFG_CODE_NXP) {
    if (p_uid[2] == I93_UID_ICODE_SLI)
      p_i93->product_version = RW_I93_ICODE_SLI;
    else if (p_uid[2] == I93_UID_ICODE_SLI_S)
      p_i93->product_version = RW_I93_ICODE_SLI_S;
    else if (p_uid[2] == I93_UID_ICODE_SLI_L)
      p_i93->product_version = RW_I93_ICODE_SLI_L;
    else
      p_i93->product_version = RW_I93_UNKNOWN_PRODUCT;
  } else if (p_uid[1] == I93_UID_IC_MFG_CODE_TI) {
    if ((p_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
        I93_UID_TAG_IT_HF_I_PLUS_INLAY)
      p_i93->product_version = RW_I93_TAG_IT_HF_I_PLUS_INLAY;
    else if ((p_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
             I93_UID_TAG_IT_HF_I_PLUS_CHIP)
      p_i93->product_version = RW_I93_TAG_IT_HF_I_PLUS_CHIP;
    else if ((p_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
             I93_UID_TAG_IT_HF_I_STD_CHIP_INLAY)
      p_i93->product_version = RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY;
    else if ((p_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
             I93_UID_TAG_IT_HF_I_PRO_CHIP_INLAY)
      p_i93->product_version = RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY;
    else
      p_i93->product_version = RW_I93_UNKNOWN_PRODUCT;
  } else if ((p_uid[1] == I93_UID_IC_MFG_CODE_STM) &&
             (p_i93->info_flags & I93_INFO_FLAG_IC_REF)) {
    if (p_i93->ic_reference == I93_IC_REF_STM_M24LR04E_R)
      p_i93->product_version = RW_I93_STM_M24LR04E_R;
    else if (p_i93->ic_reference == I93_IC_REF_STM_M24LR16E_R)
      p_i93->product_version = RW_I93_STM_M24LR16E_R;
    else if (p_i93->ic_reference == I93_IC_REF_STM_M24LR64E_R)
      p_i93->product_version = RW_I93_STM_M24LR64E_R;
    else if (p_i93->ic_reference == I93_IC_REF_STM_ST25DVHIK)
      p_i93->product_version = RW_I93_STM_ST25DVHIK;
    else if (p_i93->ic_reference == I93_IC_REF_STM_ST25DV04K)
      p_i93->product_version = RW_I93_STM_ST25DV04K;
    else {
      switch (p_i93->ic_reference & I93_IC_REF_STM_MASK) {
        case I93_IC_REF_STM_LRI1K:
          p_i93->product_version = RW_I93_STM_LRI1K;
          break;
        case I93_IC_REF_STM_LRI2K:
          p_i93->product_version = RW_I93_STM_LRI2K;
          break;
        case I93_IC_REF_STM_LRIS2K:
          p_i93->product_version = RW_I93_STM_LRIS2K;
          break;
        case I93_IC_REF_STM_LRIS64K:
          p_i93->product_version = RW_I93_STM_LRIS64K;
          break;
        case I93_IC_REF_STM_M24LR64_R:
          p_i93->product_version = RW_I93_STM_M24LR64_R;
          break;
        default:
          p_i93->product_version = RW_I93_UNKNOWN_PRODUCT;
          break;
      }
    }
  } else if ((p_uid[1] == I93_UID_IC_MFG_CODE_ONS) &&
             (p_i93->info_flags & I93_INFO_FLAG_IC_REF)) {
    switch (p_i93->ic_reference) {
      case I93_IC_REF_ONS_N36RW02:
        p_i93->product_version = RW_I93_ONS_N36RW02;
        break;
      case I93_IC_REF_ONS_N24RF04:
        p_i93->product_version = RW_I93_ONS_N24RF04;
        break;
      case I93_IC_REF_ONS_N24RF04E:
        p_i93->product_version = RW_I93_ONS_N24RF04E;
        break;
      case I93_IC_REF_ONS_N24RF16:
        p_i93->product_version = RW_I93_ONS_N24RF16;
        break;
      case I93_IC_REF_ONS_N24RF16E:
        p_i93->product_version = RW_I93_ONS_N24RF16E;
        break;
      case I93_IC_REF_ONS_N24RF64:
        p_i93->product_version = RW_I93_ONS_N24RF64;
        break;
      case I93_IC_REF_ONS_N24RF64E:
        p_i93->product_version = RW_I93_ONS_N24RF64E;
        break;
      default:
        p_i93->product_version = RW_I93_UNKNOWN_PRODUCT;
        break;
    }
  } else {
    p_i93->product_version = RW_I93_UNKNOWN_PRODUCT;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("product_version = <%s>",
                      rw_i93_get_tag_name(p_i93->product_version).c_str());

  switch (p_i93->product_version) {
    case RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY:
    case RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY:
      /* these don't support Get System Information Command */
      /* these support only Inventory, Stay Quiet, Read Single Block, Write
       * Single Block, Lock Block */
      p_i93->block_size = I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_BLK_SIZE;
      p_i93->num_block = I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_NUM_USER_BLK;
      break;
    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_i93_process_ext_sys_info
**
** Description      Store extended system information of tag
**
** Returns          FALSE if retrying with protocol extension flag
**
*******************************************************************************/
bool rw_i93_process_ext_sys_info(uint8_t* p_data, uint16_t length) {
  uint8_t* p = p_data;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  uint8_t uid[I93_UID_BYTE_LEN], *p_uid;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (length < (I93_UID_BYTE_LEN + 1)) {
    android_errorWriteLog(0x534e4554, "122316913");
    return false;
  }

  STREAM_TO_UINT8(p_i93->info_flags, p);
  length--;

  p_uid = uid;
  STREAM_TO_ARRAY8(p_uid, p);
  length -= I93_UID_BYTE_LEN;

  if (p_i93->info_flags & I93_INFO_FLAG_DSFID) {
    if (length < I93_INFO_DSFID_LEN) {
      android_errorWriteLog(0x534e4554, "122316913");
      return false;
    }
    STREAM_TO_UINT8(p_i93->dsfid, p);
    length--;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_AFI) {
    if (length < I93_INFO_AFI_LEN) {
      android_errorWriteLog(0x534e4554, "122316913");
      return false;
    }
    STREAM_TO_UINT8(p_i93->afi, p);
    length--;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_MEM_SIZE) {
    if (length < I93_INFO_16BIT_NUM_BLOCK_LEN + I93_INFO_BLOCK_SIZE_LEN) {
      android_errorWriteLog(0x534e4554, "122316913");
      return false;
    }
    STREAM_TO_UINT16(p_i93->num_block, p);
    length -= I93_INFO_16BIT_NUM_BLOCK_LEN;

    /* it is one less than actual number of bytes */
    p_i93->num_block += 1;

    STREAM_TO_UINT8(p_i93->block_size, p);
    length--;
    /* it is one less than actual number of blocks */
    p_i93->block_size = (p_i93->block_size & 0x1F) + 1;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_IC_REF) {
    if (length < I93_INFO_IC_REF_LEN) {
      android_errorWriteLog(0x534e4554, "122316913");
      return false;
    }
    STREAM_TO_UINT8(p_i93->ic_reference, p);
    length--;

    /* clear existing UID to set product version */
    p_i93->uid[0] = 0x00;

    /* store UID and get product version */
    rw_i93_get_product_version(p_uid);

    if (p_i93->uid[0] == I93_UID_FIRST_BYTE) {
      if ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_STM) ||
          (p_i93->uid[1] == I93_UID_IC_MFG_CODE_ONS)) {
        /* STM & ONS supports more than 2040 bytes */
        p_i93->intl_flags |= RW_I93_FLAG_EXT_COMMANDS;
      }
    }
  }
  return true;
}

/*******************************************************************************
**
** Function         rw_i93_process_sys_info
**
** Description      Store system information of tag
**
** Returns          FALSE if retrying with protocol extension flag
**
*******************************************************************************/
bool rw_i93_process_sys_info(uint8_t* p_data, uint16_t length) {
  uint8_t* p = p_data;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  uint8_t uid[I93_UID_BYTE_LEN], *p_uid;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (length < (I93_UID_BYTE_LEN + 1)) {
    android_errorWriteLog(0x534e4554, "121259048");
    return false;
  }
  STREAM_TO_UINT8(p_i93->info_flags, p);
  length--;

  p_uid = uid;
  STREAM_TO_ARRAY8(p_uid, p);
  length -= I93_UID_BYTE_LEN;

  if (p_i93->info_flags & I93_INFO_FLAG_DSFID) {
    if (length == 0) {
      android_errorWriteLog(0x534e4554, "121259048");
      return false;
    }
    STREAM_TO_UINT8(p_i93->dsfid, p);
    length--;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_AFI) {
    if (length == 0) {
      android_errorWriteLog(0x534e4554, "121259048");
      return false;
    }
    STREAM_TO_UINT8(p_i93->afi, p);
    length--;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_MEM_SIZE) {
    bool block_16_bit = p_i93->intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK;
    if (block_16_bit && length > 2) {
      STREAM_TO_UINT16(p_i93->num_block, p);
      length -= 2;
    } else if (!block_16_bit && length > 1) {
      STREAM_TO_UINT8(p_i93->num_block, p);
      length--;
    } else {
      android_errorWriteLog(0x534e4554, "121259048");
      return false;
    }
    /* it is one less than actual number of bytes */
    p_i93->num_block += 1;

    STREAM_TO_UINT8(p_i93->block_size, p);
    length--;
    /* it is one less than actual number of blocks */
    p_i93->block_size = (p_i93->block_size & 0x1F) + 1;
  }
  if (p_i93->info_flags & I93_INFO_FLAG_IC_REF) {
    if (length == 0) {
      android_errorWriteLog(0x534e4554, "121259048");
      return false;
    }
    STREAM_TO_UINT8(p_i93->ic_reference, p);

    /* clear existing UID to set product version */
    p_i93->uid[0] = 0x00;

    /* store UID and get product version */
    rw_i93_get_product_version(p_uid);

    if (p_i93->uid[0] == I93_UID_FIRST_BYTE) {
      if ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
          (p_i93->ic_reference == I93_IC_REF_ICODE_SLI_L)) {
        p_i93->num_block = 8;
        p_i93->block_size = 4;
      } else if (p_i93->uid[1] == I93_UID_IC_MFG_CODE_STM) {
        /*
        **  LRI1K:      010000xx(b), blockSize: 4, numberBlocks: 0x20
        **  LRI2K:      001000xx(b), blockSize: 4, numberBlocks: 0x40
        **  LRIS2K:     001010xx(b), blockSize: 4, numberBlocks: 0x40
        **  LRIS64K:    010001xx(b), blockSize: 4, numberBlocks: 0x800
        **  M24LR64-R:  001011xx(b), blockSize: 4, numberBlocks: 0x800
        **  M24LR04E-R: 01011010(b), blockSize: 4, numberBlocks: 0x80
        **  M24LR16E-R: 01001110(b), blockSize: 4, numberBlocks: 0x200
        **  M24LR64E-R: 01011110(b), blockSize: 4, numberBlocks: 0x800
        */
        if ((p_i93->product_version == RW_I93_STM_M24LR16E_R) ||
            (p_i93->product_version == RW_I93_STM_M24LR64E_R)) {
          /*
          ** M24LR16E-R or M24LR64E-R returns system information
          ** without memory size, if option flag is not set.
          ** LRIS64K and M24LR64-R return error if option flag is not
          ** set.
          */
          if (!(p_i93->intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK)) {
            /* get memory size with protocol extension flag */
            if (rw_i93_send_cmd_get_sys_info(nullptr, I93_FLAG_PROT_EXT_YES) ==
                NFC_STATUS_OK) {
              /* STM supports more than 2040 bytes */
              p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;

              return false;
            }
          }
        } else if ((p_i93->product_version == RW_I93_STM_LRI2K) &&
                   (p_i93->ic_reference == 0x21)) {
          /* workaround of byte order in memory size information */
          p_i93->num_block = 64;
          p_i93->block_size = 4;
        } else if (!(p_i93->info_flags & I93_INFO_FLAG_MEM_SIZE)) {
          if (!(p_i93->intl_flags & RW_I93_FLAG_EXT_COMMANDS)) {
            if (rw_i93_send_cmd_get_ext_sys_info(nullptr) == NFC_STATUS_OK) {
              /* STM supports more than 2040 bytes */
              p_i93->intl_flags |= RW_I93_FLAG_EXT_COMMANDS;

              return false;
            }
          }
        }
      } else if (p_i93->uid[1] == I93_UID_IC_MFG_CODE_ONS) {
        /*
        **  N36RW02:  00011010(b), blockSize: 4, numberBlocks: 0x40
        **  N24RF04:  00101110(b), blockSize: 4, numberBlocks: 0x80
        **  N24RF04E: 00101010(b), blockSize: 4, numberBlocks: 0x80
        **  N24RF16:  01001010(b), blockSize: 4, numberBlocks: 0x200
        **  N24RF16E: 01001110(b), blockSize: 4, numberBlocks: 0x200
        **  N24RF64:  01101010(b), blockSize: 4, numberBlocks: 0x800
        **  N24RF64E: 01101110(b), blockSize: 4, numberBlocks: 0x800
        */
        p_i93->block_size = 4;
        switch (p_i93->product_version) {
          case RW_I93_ONS_N36RW02:
            p_i93->num_block = 0x40;
            break;
          case RW_I93_ONS_N24RF04:
          case RW_I93_ONS_N24RF04E:
            p_i93->num_block = 0x80;
            break;
          case RW_I93_ONS_N24RF16:
          case RW_I93_ONS_N24RF16E:
            p_i93->num_block = 0x200;
            p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;
            break;
          case RW_I93_ONS_N24RF64:
          case RW_I93_ONS_N24RF64E:
            p_i93->num_block = 0x800;
            p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;
            break;
          default:
            return false;
        }
      }
    }
  }

  return true;
}

/*******************************************************************************
**
** Function         rw_i93_check_sys_info_prot_ext
**
** Description      Check if need to set protocol extension flag to get system
**                  info
**
** Returns          TRUE if sent Get System Info with protocol extension flag
**
*******************************************************************************/
bool rw_i93_check_sys_info_prot_ext(uint8_t error_code) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (((p_i93->uid[1] == I93_UID_IC_MFG_CODE_STM) ||
       (p_i93->uid[1] == I93_UID_IC_MFG_CODE_ONS)) &&
      (p_i93->sent_cmd == I93_CMD_GET_SYS_INFO) &&
      (error_code == I93_ERROR_CODE_OPTION_NOT_SUPPORTED) &&
      (rw_i93_send_cmd_get_sys_info(nullptr, I93_FLAG_PROT_EXT_YES) ==
       NFC_STATUS_OK)) {
    return true;
  } else {
    return false;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_to_upper
**
** Description      Send response to upper layer
**
** Returns          void
**
*******************************************************************************/
void rw_i93_send_to_upper(NFC_HDR* p_resp) {
  uint8_t *p = (uint8_t*)(p_resp + 1) + p_resp->offset, *p_uid;
  uint16_t length = p_resp->len;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;
  uint8_t event = RW_I93_MAX_EVT;
  uint8_t flags;
  NFC_HDR* p_buff;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (length == 0) {
    android_errorWriteLog(0x534e4554, "121035878");
    rw_data.i93_cmd_cmpl.status = NFC_STATUS_FAILED;
    rw_data.i93_cmd_cmpl.command = p_i93->sent_cmd;
    rw_cb.tcb.i93.sent_cmd = 0;
    (*(rw_cb.p_cback))(RW_I93_CMD_CMPL_EVT, &rw_data);
    return;
  }

  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    if ((length) && (rw_i93_check_sys_info_prot_ext(*p))) {
      /* getting system info with protocol extension flag */
      /* This STM & ONS tag supports more than 2040 bytes */
      p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;
      p_i93->state = RW_I93_STATE_BUSY;
    } else {
      /* notify error to upper layer */
      rw_data.i93_cmd_cmpl.status = NFC_STATUS_FAILED;
      rw_data.i93_cmd_cmpl.command = p_i93->sent_cmd;
      STREAM_TO_UINT8(rw_data.i93_cmd_cmpl.error_code, p);

      rw_cb.tcb.i93.sent_cmd = 0;
      (*(rw_cb.p_cback))(RW_I93_CMD_CMPL_EVT, &rw_data);
    }
    return;
  }

  switch (p_i93->sent_cmd) {
    case I93_CMD_INVENTORY:
      if (length < I93_INFO_DSFID_LEN + I93_UID_BYTE_LEN) return;

      /* forward inventory response */
      rw_data.i93_inventory.status = NFC_STATUS_OK;
      STREAM_TO_UINT8(rw_data.i93_inventory.dsfid, p);

      p_uid = rw_data.i93_inventory.uid;
      STREAM_TO_ARRAY8(p_uid, p);

      /* store UID and get product version */
      rw_i93_get_product_version(p_uid);

      event = RW_I93_INVENTORY_EVT;
      break;

    case I93_CMD_READ_SINGLE_BLOCK:
    case I93_CMD_EXT_READ_SINGLE_BLOCK:
    case I93_CMD_READ_MULTI_BLOCK:
    case I93_CMD_EXT_READ_MULTI_BLOCK:
    case I93_CMD_GET_MULTI_BLK_SEC:
    case I93_CMD_EXT_GET_MULTI_BLK_SEC:

      /* forward tag data or security status */
      p_buff = (NFC_HDR*)GKI_getbuf((uint16_t)(length + NFC_HDR_SIZE));

      if (p_buff) {
        p_buff->offset = 0;
        p_buff->len = length;

        memcpy((p_buff + 1), p, length);

        rw_data.i93_data.status = NFC_STATUS_OK;
        rw_data.i93_data.command = p_i93->sent_cmd;
        rw_data.i93_data.p_data = p_buff;

        event = RW_I93_DATA_EVT;
      } else {
        rw_data.i93_cmd_cmpl.status = NFC_STATUS_NO_BUFFERS;
        rw_data.i93_cmd_cmpl.command = p_i93->sent_cmd;
        rw_data.i93_cmd_cmpl.error_code = 0;

        event = RW_I93_CMD_CMPL_EVT;
      }
      break;

    case I93_CMD_WRITE_SINGLE_BLOCK:
    case I93_CMD_EXT_WRITE_SINGLE_BLOCK:
    case I93_CMD_LOCK_BLOCK:
    case I93_CMD_EXT_LOCK_BLOCK:
    case I93_CMD_WRITE_MULTI_BLOCK:
    case I93_CMD_EXT_WRITE_MULTI_BLOCK:
    case I93_CMD_SELECT:
    case I93_CMD_RESET_TO_READY:
    case I93_CMD_WRITE_AFI:
    case I93_CMD_LOCK_AFI:
    case I93_CMD_WRITE_DSFID:
    case I93_CMD_LOCK_DSFID:

      /* notify the complete of command */
      rw_data.i93_cmd_cmpl.status = NFC_STATUS_OK;
      rw_data.i93_cmd_cmpl.command = p_i93->sent_cmd;
      rw_data.i93_cmd_cmpl.error_code = 0;

      event = RW_I93_CMD_CMPL_EVT;
      break;

    case I93_CMD_GET_SYS_INFO:

      if (rw_i93_process_sys_info(p, length)) {
        rw_data.i93_sys_info.status = NFC_STATUS_OK;
        rw_data.i93_sys_info.info_flags = p_i93->info_flags;
        rw_data.i93_sys_info.dsfid = p_i93->dsfid;
        rw_data.i93_sys_info.afi = p_i93->afi;
        rw_data.i93_sys_info.num_block = p_i93->num_block;
        rw_data.i93_sys_info.block_size = p_i93->block_size;
        rw_data.i93_sys_info.IC_reference = p_i93->ic_reference;

        memcpy(rw_data.i93_sys_info.uid, p_i93->uid, I93_UID_BYTE_LEN);

        event = RW_I93_SYS_INFO_EVT;
      } else {
        /* retrying with protocol extension flag */
        p_i93->state = RW_I93_STATE_BUSY;
        return;
      }
      break;

    case I93_CMD_EXT_GET_SYS_INFO:

      if (rw_i93_process_ext_sys_info(p, length)) {
        rw_data.i93_sys_info.status = NFC_STATUS_OK;
        rw_data.i93_sys_info.info_flags = p_i93->info_flags;
        rw_data.i93_sys_info.dsfid = p_i93->dsfid;
        rw_data.i93_sys_info.afi = p_i93->afi;
        rw_data.i93_sys_info.num_block = p_i93->num_block;
        rw_data.i93_sys_info.block_size = p_i93->block_size;
        rw_data.i93_sys_info.IC_reference = p_i93->ic_reference;

        memcpy(rw_data.i93_sys_info.uid, p_i93->uid, I93_UID_BYTE_LEN);

        event = RW_I93_SYS_INFO_EVT;
      } else {
        /* retrying with protocol extension flag or with extended sys info
         * command */
        p_i93->state = RW_I93_STATE_BUSY;
        return;
      }
      break;

    default:
      break;
  }

  rw_cb.tcb.i93.sent_cmd = 0;
  if (event != RW_I93_MAX_EVT) {
    (*(rw_cb.p_cback))(event, &rw_data);
  } else {
    LOG(ERROR) << StringPrintf("Invalid response");
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_to_lower
**
** Description      Send Request frame to lower layer
**
** Returns          TRUE if success
**
*******************************************************************************/
bool rw_i93_send_to_lower(NFC_HDR* p_msg) {
  /* store command for retransmitting */
  if (rw_cb.tcb.i93.p_retry_cmd) {
    GKI_freebuf(rw_cb.tcb.i93.p_retry_cmd);
    rw_cb.tcb.i93.p_retry_cmd = nullptr;
  }

  rw_cb.tcb.i93.p_retry_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (rw_cb.tcb.i93.p_retry_cmd) {
    memcpy(rw_cb.tcb.i93.p_retry_cmd, p_msg,
           sizeof(NFC_HDR) + p_msg->offset + p_msg->len);
  }

  if (NFC_SendData(NFC_RF_CONN_ID, p_msg) != NFC_STATUS_OK) {
    LOG(ERROR) << StringPrintf("failed");
    return false;
  }

  nfc_start_quick_timer(&rw_cb.tcb.i93.timer, NFC_TTYPE_RW_I93_RESPONSE,
                        (RW_I93_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

  return true;
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_inventory
**
** Description      Send Inventory Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_inventory(uint8_t* p_uid, bool including_afi,
                                      uint8_t afi) {
  NFC_HDR* p_cmd;
  uint8_t *p, flags;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("including_afi:%d, AFI:0x%02X", including_afi, afi);

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 3;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  flags = (I93_FLAG_SLOT_ONE | I93_FLAG_INVENTORY_SET |
           RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE);
  if (including_afi) {
    flags |= I93_FLAG_AFI_PRESENT;
  }

  UINT8_TO_STREAM(p, flags);

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_INVENTORY);

  if (including_afi) {
    /* Parameters */
    UINT8_TO_STREAM(p, afi); /* Optional AFI */
    p_cmd->len++;
  }

  if (p_uid) {
    UINT8_TO_STREAM(p, I93_UID_BYTE_LEN * 8); /* Mask Length */
    ARRAY8_TO_STREAM(p, p_uid);               /* UID */
    p_cmd->len += I93_UID_BYTE_LEN;
  } else {
    UINT8_TO_STREAM(p, 0x00); /* Mask Length */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_INVENTORY;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_stay_quiet
**
** Description      Send Stay Quiet Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_stay_quiet(void) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_STAY_QUIET);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_STAY_QUIET;

    /* restart timer for stay quiet */
    nfc_start_quick_timer(
        &rw_cb.tcb.i93.timer, NFC_TTYPE_RW_I93_RESPONSE,
        (RW_I93_TOUT_STAY_QUIET * QUICK_TIMER_TICKS_PER_SEC) / 1000);
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_read_single_block
**
** Description      Send Read Single Block Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_read_single_block(uint16_t block_number,
                                              bool read_security) {
  NFC_HDR* p_cmd;
  uint8_t *p, flags;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  flags =
      (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE);

  if (read_security) flags |= I93_FLAG_OPTION_SET;

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK)
    flags |= I93_FLAG_PROT_EXT_YES;

  UINT8_TO_STREAM(p, flags);

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, I93_CMD_EXT_READ_SINGLE_BLOCK);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_READ_SINGLE_BLOCK);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK ||
      rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(p, block_number); /* Block number */
    p_cmd->len++;
  } else {
    UINT8_TO_STREAM(p, block_number); /* Block number */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_READ_SINGLE_BLOCK;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_READ_SINGLE_BLOCK;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_write_single_block
**
** Description      Send Write Single Block Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_write_single_block(uint16_t block_number,
                                               uint8_t* p_data) {
  NFC_HDR* p_cmd;
  uint8_t *p, flags;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11 + rw_cb.tcb.i93.block_size;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  if ((rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) {
    /* Option must be set for TI tag */
    flags = (I93_FLAG_ADDRESS_SET | I93_FLAG_OPTION_SET |
             RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE);
  } else {
    flags = (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
             RW_I93_FLAG_DATA_RATE);
  }

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK)
    flags |= I93_FLAG_PROT_EXT_YES;

  UINT8_TO_STREAM(p, flags);

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, I93_CMD_EXT_WRITE_SINGLE_BLOCK);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_WRITE_SINGLE_BLOCK);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK ||
      rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(p, block_number); /* Block number */
    p_cmd->len++;
  } else {
    UINT8_TO_STREAM(p, block_number); /* Block number */
  }

  /* Data */
  ARRAY_TO_STREAM(p, p_data, rw_cb.tcb.i93.block_size);

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_WRITE_SINGLE_BLOCK;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_WRITE_SINGLE_BLOCK;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_lock_block
**
** Description      Send Lock Block Request to VICC
**
**                  STM LRIS64K, M24LR64-R, M24LR04E-R, M24LR16E-R, M24LR64E-R
**                  do not support.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_lock_block(uint8_t block_number) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  if ((rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) {
    /* Option must be set for TI tag */
    UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | I93_FLAG_OPTION_SET |
                        RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE));
  } else {
    UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                        RW_I93_FLAG_DATA_RATE));
  }

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, I93_CMD_EXT_LOCK_BLOCK);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_LOCK_BLOCK);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, block_number); /* Block number */
    p_cmd->len++;
  } else {
    UINT8_TO_STREAM(p, block_number); /* Block number */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_LOCK_BLOCK;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_LOCK_BLOCK;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_read_multi_blocks
**
** Description      Send Read Multiple Blocks Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_read_multi_blocks(uint16_t first_block_number,
                                              uint16_t number_blocks) {
  NFC_HDR* p_cmd;
  uint8_t *p, flags;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 12;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  flags =
      (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE);

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK) {
    flags |= I93_FLAG_PROT_EXT_YES;
  }

  UINT8_TO_STREAM(p, flags);

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, I93_CMD_EXT_READ_MULTI_BLOCK);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_READ_MULTI_BLOCK);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK ||
      rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(p, first_block_number); /* First block number */
    p_cmd->len++;
  } else {
    UINT8_TO_STREAM(p, first_block_number); /* First block number */
  }

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
    p_cmd->len++;
  } else {
    UINT8_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_READ_MULTI_BLOCK;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_READ_MULTI_BLOCK;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_write_multi_blocks
**
** Description      Send Write Multiple Blocks Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_write_multi_blocks(uint16_t first_block_number,
                                               uint16_t number_blocks,
                                               uint8_t* p_data) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 12 + number_blocks * rw_cb.tcb.i93.block_size;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    INT8_TO_STREAM(p, I93_CMD_EXT_WRITE_MULTI_BLOCK);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_WRITE_MULTI_BLOCK);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(p, first_block_number); /* Block number */
    UINT16_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
    p_cmd->len += 2;
  } else {
    UINT8_TO_STREAM(p, first_block_number); /* Block number */
    UINT8_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
  }

  UINT8_TO_STREAM(
      p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */

  /* Data */
  ARRAY_TO_STREAM(p, p_data, number_blocks * rw_cb.tcb.i93.block_size);

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_WRITE_MULTI_BLOCK;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_WRITE_MULTI_BLOCK;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_select
**
** Description      Send Select Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_select(uint8_t* p_uid) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_SELECT);

  /* Parameters */
  ARRAY8_TO_STREAM(p, p_uid); /* UID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_SELECT;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_reset_to_ready
**
** Description      Send Reset to Ready Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_reset_to_ready(void) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_RESET_TO_READY);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_RESET_TO_READY;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_write_afi
**
** Description      Send Write AFI Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_write_afi(uint8_t afi) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_WRITE_AFI);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */
  UINT8_TO_STREAM(p, afi);                /* AFI */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_WRITE_AFI;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_lock_afi
**
** Description      Send Lock AFI Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_lock_afi(void) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_LOCK_AFI);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_LOCK_AFI;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_write_dsfid
**
** Description      Send Write DSFID Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_write_dsfid(uint8_t dsfid) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_WRITE_DSFID);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */
  UINT8_TO_STREAM(p, dsfid);              /* DSFID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_WRITE_DSFID;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_lock_dsfid
**
** Description      Send Lock DSFID Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_lock_dsfid(void) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_LOCK_DSFID);

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_LOCK_DSFID;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_get_ext_sys_info
**
** Description      Send Get Extended System Information Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_get_ext_sys_info(uint8_t* p_uid) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    DLOG_IF(INFO, nfc_debug_enabled) << __func__ << "Cannot allocate buffer";
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 11;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_EXT_GET_SYS_INFO);

  /* Parameters request field */
  UINT8_TO_STREAM(p,
                  (I93_INFO_FLAG_MOI | I93_INFO_FLAG_DSFID | I93_INFO_FLAG_AFI |
                   I93_INFO_FLAG_MEM_SIZE | I93_INFO_FLAG_IC_REF));

  /* Parameters */
  if (p_uid) {
    ARRAY8_TO_STREAM(p, p_uid); /* UID */
  } else {
    ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_GET_SYS_INFO;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_get_sys_info
**
** Description      Send Get System Information Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_get_sys_info(uint8_t* p_uid, uint8_t extra_flags) {
  NFC_HDR* p_cmd;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 10;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  UINT8_TO_STREAM(p, (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER |
                      RW_I93_FLAG_DATA_RATE | extra_flags));

  /* Command Code */
  UINT8_TO_STREAM(p, I93_CMD_GET_SYS_INFO);

  /* Parameters */
  if (p_uid) {
    ARRAY8_TO_STREAM(p, p_uid); /* UID */
  } else {
    ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    rw_cb.tcb.i93.sent_cmd = I93_CMD_GET_SYS_INFO;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_send_cmd_get_multi_block_sec
**
** Description      Send Get Multiple Block Security Status Request to VICC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_send_cmd_get_multi_block_sec(uint16_t first_block_number,
                                                uint16_t number_blocks) {
  NFC_HDR* p_cmd;
  uint8_t *p, flags;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cmd = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_cmd) {
    LOG(ERROR) << StringPrintf("Cannot allocate buffer");
    return NFC_STATUS_NO_BUFFERS;
  }

  p_cmd->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p_cmd->len = 12;
  p = (uint8_t*)(p_cmd + 1) + p_cmd->offset;

  /* Flags */
  flags =
      (I93_FLAG_ADDRESS_SET | RW_I93_FLAG_SUB_CARRIER | RW_I93_FLAG_DATA_RATE);

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK)
    flags |= I93_FLAG_PROT_EXT_YES;

  UINT8_TO_STREAM(p, flags);

  /* Command Code */
  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT8_TO_STREAM(p, I93_CMD_EXT_GET_MULTI_BLK_SEC);
  } else {
    UINT8_TO_STREAM(p, I93_CMD_GET_MULTI_BLK_SEC);
  }

  /* Parameters */
  ARRAY8_TO_STREAM(p, rw_cb.tcb.i93.uid); /* UID */

  if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_16BIT_NUM_BLOCK ||
      rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS) {
    UINT16_TO_STREAM(p, first_block_number); /* First block number */
    UINT16_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
    p_cmd->len += 2;
  } else {
    UINT8_TO_STREAM(p, first_block_number); /* First block number */
    UINT8_TO_STREAM(
        p, number_blocks - 1); /* Number of blocks, 0x00 to read one block */
  }

  if (rw_i93_send_to_lower(p_cmd)) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_EXT_COMMANDS)
      rw_cb.tcb.i93.sent_cmd = I93_CMD_EXT_GET_MULTI_BLK_SEC;
    else
      rw_cb.tcb.i93.sent_cmd = I93_CMD_GET_MULTI_BLK_SEC;
    return NFC_STATUS_OK;
  } else {
    return NFC_STATUS_FAILED;
  }
}

/*******************************************************************************
**
** Function         rw_i93_get_next_blocks
**
** Description      Read as many blocks as possible (up to
**                  RW_I93_READ_MULTI_BLOCK_SIZE)
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_get_next_blocks(uint16_t offset) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  uint16_t first_block;
  uint16_t num_block;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  first_block = offset / p_i93->block_size;

  /* more blocks, more efficent but more error rate */

  if (p_i93->intl_flags & RW_I93_FLAG_READ_MULTI_BLOCK) {
    num_block = RW_I93_READ_MULTI_BLOCK_SIZE / p_i93->block_size;

    if (num_block + first_block > p_i93->num_block)
      num_block = p_i93->num_block - first_block;

    if (p_i93->uid[1] == I93_UID_IC_MFG_CODE_STM) {
      /* LRIS64K, M24LR64-R, M24LR04E-R, M24LR16E-R, M24LR64E-R requires
      ** - The max number of blocks is 32 and they are all located in the
      **   same sector.
      ** - The sector is 32 blocks of 4 bytes.
      */
      if ((p_i93->product_version == RW_I93_STM_LRIS64K) ||
          (p_i93->product_version == RW_I93_STM_M24LR64_R) ||
          (p_i93->product_version == RW_I93_STM_M24LR04E_R) ||
          (p_i93->product_version == RW_I93_STM_M24LR16E_R) ||
          (p_i93->product_version == RW_I93_STM_M24LR64E_R)) {
        if (num_block > I93_STM_MAX_BLOCKS_PER_READ)
          num_block = I93_STM_MAX_BLOCKS_PER_READ;

        if ((first_block / I93_STM_BLOCKS_PER_SECTOR) !=
            ((first_block + num_block - 1) / I93_STM_BLOCKS_PER_SECTOR)) {
          num_block = I93_STM_BLOCKS_PER_SECTOR -
                      (first_block % I93_STM_BLOCKS_PER_SECTOR);
        }
      }
    }

    if (p_i93->uid[1] == I93_UID_IC_MFG_CODE_ONS) {
      /* N24RF04, N24RF04E, N24RF16, N24RF16E, N24RF64, N24RF64E requires
      ** - The max number of blocks is 32 and they are all located in the
      **   same sector.
      ** - The sector is 32 blocks of 4 bytes.
      */
      if ((p_i93->product_version == RW_I93_ONS_N36RW02) ||
          (p_i93->product_version == RW_I93_ONS_N24RF04) ||
          (p_i93->product_version == RW_I93_ONS_N24RF04E) ||
          (p_i93->product_version == RW_I93_ONS_N24RF16) ||
          (p_i93->product_version == RW_I93_ONS_N24RF16E) ||
          (p_i93->product_version == RW_I93_ONS_N24RF64) ||
          (p_i93->product_version == RW_I93_ONS_N24RF64E)) {
        if (num_block > I93_ONS_MAX_BLOCKS_PER_READ)
          num_block = I93_ONS_MAX_BLOCKS_PER_READ;

        if ((first_block / I93_ONS_BLOCKS_PER_SECTOR) !=
            ((first_block + num_block - 1) / I93_ONS_BLOCKS_PER_SECTOR)) {
          num_block = I93_ONS_BLOCKS_PER_SECTOR -
                      (first_block % I93_ONS_BLOCKS_PER_SECTOR);
        }
      }
    }

    return rw_i93_send_cmd_read_multi_blocks(first_block, num_block);
  } else {
    return rw_i93_send_cmd_read_single_block(first_block, false);
  }
}

/*******************************************************************************
**
** Function         rw_i93_get_next_block_sec
**
** Description      Get as many security of blocks as possible from
**                  p_i93->rw_offset (up to RW_I93_GET_MULTI_BLOCK_SEC_SIZE)
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_i93_get_next_block_sec(void) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  uint16_t num_blocks;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (p_i93->num_block <= p_i93->rw_offset) {
    LOG(ERROR) << StringPrintf(
        "rw_offset(0x%x) must be less than num_block(0x%x)", p_i93->rw_offset,
        p_i93->num_block);
    return NFC_STATUS_FAILED;
  }

  num_blocks = p_i93->num_block - p_i93->rw_offset;

  if (num_blocks > RW_I93_GET_MULTI_BLOCK_SEC_SIZE)
    num_blocks = RW_I93_GET_MULTI_BLOCK_SEC_SIZE;
#ifdef ANDROID
  DLOG_IF(INFO, nfc_debug_enabled)
      << __func__ << std::hex << rw_cb.tcb.i93.intl_flags;
#endif
  return rw_i93_send_cmd_get_multi_block_sec(p_i93->rw_offset, num_blocks);
}

/*******************************************************************************
**
** Function         rw_i93_sm_detect_ndef
**
** Description      Process NDEF detection procedure
**
**                  1. Get UID if not having yet
**                  2. Get System Info if not having yet
**                  3. Read first block for CC
**                  4. Search NDEF Type and length
**                  5. Get block status to get max NDEF size and read-only
**                     status
**
** Returns          void
**
*******************************************************************************/
void rw_i93_sm_detect_ndef(NFC_HDR* p_resp) {
  uint8_t *p = (uint8_t*)(p_resp + 1) + p_resp->offset, *p_uid;
  uint8_t flags, u8 = 0, cc[4];
  uint16_t length = p_resp->len, xx, block, first_block, last_block, num_blocks;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (0x%x)",
      rw_i93_get_sub_state_name(p_i93->sub_state).c_str(), p_i93->sub_state);

  if (length == 0) {
    android_errorWriteLog(0x534e4554, "121260197");
    rw_i93_handle_error(NFC_STATUS_FAILED);
    return;
  }
  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    if ((length) && (rw_i93_check_sys_info_prot_ext(*p))) {
      /* getting system info with protocol extension flag */
      /* This STM & ONS tag supports more than 2040 bytes */
      p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Got error flags (0x%02x)", flags);
      rw_i93_handle_error(NFC_STATUS_FAILED);
    }
    return;
  }

  switch (p_i93->sub_state) {
    case RW_I93_SUBSTATE_WAIT_UID:

      if (length < (I93_UID_BYTE_LEN + 1)) {
        android_errorWriteLog(0x534e4554, "121260197");
        rw_i93_handle_error(NFC_STATUS_FAILED);
        return;
      }
      STREAM_TO_UINT8(u8, p); /* DSFID */
      p_uid = p_i93->uid;
      STREAM_TO_ARRAY8(p_uid, p);

      if (u8 != I93_DFS_UNSUPPORTED) {
        /* if Data Storage Format is unknown */
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Got unknown DSFID (0x%02x)", u8);
        rw_i93_handle_error(NFC_STATUS_FAILED);
      } else {
        /* get system information to get memory size */
        if (rw_i93_send_cmd_get_sys_info(nullptr, I93_FLAG_PROT_EXT_NO) ==
            NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_SYS_INFO;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      }
      break;

    case RW_I93_SUBSTATE_WAIT_SYS_INFO:

      p_i93->block_size = 0;
      p_i93->num_block = 0;

      if (!rw_i93_process_sys_info(p, length)) {
        /* retrying with protocol extension flag */
        break;
      }

      if ((p_i93->block_size == 0) || (p_i93->num_block == 0)) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Unable to get tag memory size");
        rw_i93_handle_error(status);
      } else {
        /* read CC in the first block */
        if (rw_i93_send_cmd_read_single_block(0x0000, false) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_CC;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      }
      break;

    case RW_I93_SUBSTATE_WAIT_CC:

      if (length < RW_I93_CC_SIZE) {
        android_errorWriteLog(0x534e4554, "139188579");
        rw_i93_handle_error(NFC_STATUS_FAILED);
        return;
      }

      /* assume block size is more than RW_I93_CC_SIZE 4 */
      STREAM_TO_ARRAY(cc, p, RW_I93_CC_SIZE);

      status = NFC_STATUS_FAILED;

      /*
      ** Capability Container (CC)
      **
      ** CC[0] : magic number (0xE1)
      ** CC[1] : Bit 7-6:Major version number
      **       : Bit 5-4:Minor version number
      **       : Bit 3-2:Read access condition (00b: read access granted
      **         without any security)
      **       : Bit 1-0:Write access condition (00b: write access granted
      **         without any security)
      ** CC[2] : Memory size in 8 bytes (Ex. 0x04 is 32 bytes) [STM, ONS set
      **         to 0xFF if more than 2040bytes]
      ** CC[3] : Bit 0:Read multiple blocks is supported [NXP, STM, ONS]
      **       : Bit 1:Inventory page read is supported [NXP]
      **       : Bit 2:More than 2040 bytes are supported [STM, ONS]
      */

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "cc: 0x%02X 0x%02X 0x%02X 0x%02X", cc[0], cc[1], cc[2], cc[3]);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Total blocks:0x%04X, Block size:0x%02X",
                          p_i93->num_block, p_i93->block_size);

      if ((cc[0] == I93_ICODE_CC_MAGIC_NUMER_E1) ||
          (cc[0] == I93_ICODE_CC_MAGIC_NUMER_E2)) {
        if ((cc[1] & I93_ICODE_CC_READ_ACCESS_MASK) ==
            I93_ICODE_CC_READ_ACCESS_GRANTED) {
          if ((cc[1] & I93_ICODE_CC_WRITE_ACCESS_MASK) !=
              I93_ICODE_CC_WRITE_ACCESS_GRANTED) {
            /* read-only or password required to write */
            p_i93->intl_flags |= RW_I93_FLAG_READ_ONLY;
          }
          if (cc[3] & I93_ICODE_CC_MBREAD_MASK) {
            /* tag supports read multi blocks command */
            p_i93->intl_flags |= RW_I93_FLAG_READ_MULTI_BLOCK;
          }
          if (cc[0] == I93_ICODE_CC_MAGIC_NUMER_E2) {
            p_i93->intl_flags |= RW_I93_FLAG_EXT_COMMANDS;
          }
          status = NFC_STATUS_OK;
        }
      }

      if (status == NFC_STATUS_OK) {
        /* seach NDEF TLV from offset 4 when CC file coded on 4 bytes NFC Forum
         */
        if (cc[2] != 0)
          p_i93->rw_offset = 4;
        else
          p_i93->rw_offset = 8;

        if (rw_i93_get_next_blocks(p_i93->rw_offset) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_SEARCH_NDEF_TLV;
          p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_TYPE;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_SEARCH_NDEF_TLV:

      /* search TLV within read blocks */
      for (xx = 0; xx < length; xx++) {
        /* if looking for type */
        if (p_i93->tlv_detect_state == RW_I93_TLV_DETECT_STATE_TYPE) {
          if (*(p + xx) == I93_ICODE_TLV_TYPE_NULL) {
            continue;
          } else if ((*(p + xx) == I93_ICODE_TLV_TYPE_NDEF) ||
                     (*(p + xx) == I93_ICODE_TLV_TYPE_PROP)) {
            /* store found type and get length field */
            p_i93->tlv_type = *(p + xx);
            p_i93->ndef_tlv_start_offset = p_i93->rw_offset + xx;

            p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_LENGTH_1;
          } else if (*(p + xx) == I93_ICODE_TLV_TYPE_TERM) {
            /* no NDEF TLV found */
            p_i93->tlv_type = I93_ICODE_TLV_TYPE_TERM;
            break;
          } else {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("Invalid type: 0x%02x", *(p + xx));
            rw_i93_handle_error(NFC_STATUS_FAILED);
            return;
          }
        } else if (p_i93->tlv_detect_state ==
                   RW_I93_TLV_DETECT_STATE_LENGTH_1) {
          /* if 3 bytes length field */
          if (*(p + xx) == 0xFF) {
            /* need 2 more bytes for length field */
            p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_LENGTH_2;
          } else {
            p_i93->tlv_length = *(p + xx);
            p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_VALUE;

            if (p_i93->tlv_type == I93_ICODE_TLV_TYPE_NDEF) {
              p_i93->ndef_tlv_last_offset =
                  p_i93->ndef_tlv_start_offset + 1 + p_i93->tlv_length;
              break;
            }
          }
        } else if (p_i93->tlv_detect_state ==
                   RW_I93_TLV_DETECT_STATE_LENGTH_2) {
          /* the second byte of 3 bytes length field */
          p_i93->tlv_length = *(p + xx);
          p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_LENGTH_3;
        } else if (p_i93->tlv_detect_state ==
                   RW_I93_TLV_DETECT_STATE_LENGTH_3) {
          /* the last byte of 3 bytes length field */
          p_i93->tlv_length = (p_i93->tlv_length << 8) + *(p + xx);
          p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_VALUE;

          if (p_i93->tlv_type == I93_ICODE_TLV_TYPE_NDEF) {
            p_i93->ndef_tlv_last_offset =
                p_i93->ndef_tlv_start_offset + 3 + p_i93->tlv_length;
            break;
          }
        } else if (p_i93->tlv_detect_state == RW_I93_TLV_DETECT_STATE_VALUE) {
          /* this is other than NDEF TLV */
          if (p_i93->tlv_length <= length - xx) {
            /* skip value field */
            xx += (uint8_t)p_i93->tlv_length;
            p_i93->tlv_detect_state = RW_I93_TLV_DETECT_STATE_TYPE;
          } else {
            /* read more data */
            p_i93->tlv_length -= (length - xx);
            break;
          }
        }
      }

      /* found NDEF TLV and read length field */
      if ((p_i93->tlv_type == I93_ICODE_TLV_TYPE_NDEF) &&
          (p_i93->tlv_detect_state == RW_I93_TLV_DETECT_STATE_VALUE)) {
        p_i93->ndef_length = p_i93->tlv_length;

        /* get lock status to see if read-only */
        if ((p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
            (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY) ||
            ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
             (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK))) {
          /* these doesn't support GetMultiBlockSecurityStatus */

          p_i93->rw_offset = p_i93->ndef_tlv_start_offset;
          first_block = p_i93->ndef_tlv_start_offset / p_i93->block_size;

          /* read block to get lock status */
          rw_i93_send_cmd_read_single_block(first_block, true);
          p_i93->sub_state = RW_I93_SUBSTATE_CHECK_LOCK_STATUS;
        } else {
          /* block offset for read-only check */
          p_i93->rw_offset = 0;

          if (rw_i93_get_next_block_sec() == NFC_STATUS_OK) {
            p_i93->sub_state = RW_I93_SUBSTATE_CHECK_LOCK_STATUS;
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        }
      } else {
        /* read more data */
        p_i93->rw_offset += length;

        if (p_i93->rw_offset >= p_i93->block_size * p_i93->num_block) {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        } else if (rw_i93_get_next_blocks(p_i93->rw_offset) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_SEARCH_NDEF_TLV;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      }
      break;

    case RW_I93_SUBSTATE_CHECK_LOCK_STATUS:

      if ((p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
          (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY) ||
          ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
           (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK))) {
        /* these doesn't support GetMultiBlockSecurityStatus */

        block = (p_i93->rw_offset / p_i93->block_size);
        last_block = (p_i93->ndef_tlv_last_offset / p_i93->block_size);

        if (length == 0) {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
        if ((*p) & I93_BLOCK_LOCKED) {
          if (block <= last_block) {
            p_i93->intl_flags |= RW_I93_FLAG_READ_ONLY;
          }
        } else {
          /* if we need to check more user blocks */
          if (block + 1 < p_i93->num_block) {
            p_i93->rw_offset += p_i93->block_size;

            /* read block to get lock status */
            rw_i93_send_cmd_read_single_block(
                (uint16_t)(p_i93->rw_offset / p_i93->block_size), true);
            break;
          }
        }

        p_i93->max_ndef_length =
            p_i93->ndef_length
            /* add available bytes including the last block of NDEF TLV */
            + (p_i93->block_size * (block - last_block) + 1) -
            (p_i93->ndef_tlv_last_offset % p_i93->block_size) - 1;
      } else {
        if (p_i93->rw_offset == 0) {
          p_i93->max_ndef_length =
              p_i93->ndef_length
              /* add available bytes in the last block of NDEF TLV */
              + p_i93->block_size -
              (p_i93->ndef_tlv_last_offset % p_i93->block_size) - 1;

          first_block = (p_i93->ndef_tlv_start_offset / p_i93->block_size);
        } else {
          first_block = 0;
        }

        last_block = (p_i93->ndef_tlv_last_offset / p_i93->block_size);
        num_blocks = length;

        for (block = first_block; block < num_blocks; block++) {
          /* if any block of NDEF TLV is locked */
          if ((block + p_i93->rw_offset) <= last_block) {
            if (*(p + block) & I93_BLOCK_LOCKED) {
              p_i93->intl_flags |= RW_I93_FLAG_READ_ONLY;
              break;
            }
          } else {
            if (*(p + block) & I93_BLOCK_LOCKED) {
              /* no more consecutive unlocked block */
              break;
            } else {
              /* add block size if not locked */
              p_i93->max_ndef_length += p_i93->block_size;
            }
          }
        }

        /* update next security of block to check */
        p_i93->rw_offset += num_blocks;

        /* if need to check more */
        if (p_i93->num_block > p_i93->rw_offset) {
          if (rw_i93_get_next_block_sec() != NFC_STATUS_OK) {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
          break;
        }
      }

      /* check if need to adjust max NDEF length */
      if ((p_i93->ndef_length < 0xFF) && (p_i93->max_ndef_length >= 0xFF)) {
        /* 3 bytes length field must be used */
        p_i93->max_ndef_length -= 2;
      }

      rw_data.ndef.status = NFC_STATUS_OK;
      rw_data.ndef.protocol = NFC_PROTOCOL_T5T;
      rw_data.ndef.flags = 0;
      rw_data.ndef.flags |= RW_NDEF_FL_SUPPORTED;
      rw_data.ndef.flags |= RW_NDEF_FL_FORMATED;
      rw_data.ndef.flags |= RW_NDEF_FL_FORMATABLE;
      rw_data.ndef.cur_size = p_i93->ndef_length;

      if (p_i93->intl_flags & RW_I93_FLAG_READ_ONLY) {
        rw_data.ndef.flags |= RW_NDEF_FL_READ_ONLY;
        rw_data.ndef.max_size = p_i93->ndef_length;
      } else {
        rw_data.ndef.flags |= RW_NDEF_FL_HARD_LOCKABLE;
        rw_data.ndef.max_size = p_i93->max_ndef_length;
      }

      p_i93->state = RW_I93_STATE_IDLE;
      p_i93->sent_cmd = 0;

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "NDEF cur_size(%d),max_size (%d), flags (0x%x)",
          rw_data.ndef.cur_size, rw_data.ndef.max_size, rw_data.ndef.flags);

      (*(rw_cb.p_cback))(RW_I93_NDEF_DETECT_EVT, &rw_data);
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_i93_sm_read_ndef
**
** Description      Process NDEF read procedure
**
** Returns          void
**
*******************************************************************************/
void rw_i93_sm_read_ndef(NFC_HDR* p_resp) {
  uint8_t* p = (uint8_t*)(p_resp + 1) + p_resp->offset;
  uint8_t flags;
  uint16_t offset, length = p_resp->len;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (length == 0) {
    android_errorWriteLog(0x534e4554, "122035770");
    rw_i93_handle_error(NFC_STATUS_FAILED);
    return;
  }

  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("Got error flags (0x%02x)", flags);
    rw_i93_handle_error(NFC_STATUS_FAILED);
    return;
  }

  /* if this is the first block */
  if (p_i93->rw_length == 0) {
    /* get start of NDEF in the first block */
    offset = p_i93->ndef_tlv_start_offset % p_i93->block_size;

    if (p_i93->ndef_length < 0xFF) {
      offset += 2;
    } else {
      offset += 4;
    }

    /* adjust offset if read more blocks because the first block doesn't have
     * NDEF */
    offset -= (p_i93->rw_offset - p_i93->ndef_tlv_start_offset);
  } else {
    offset = 0;
  }

  /* if read enough data to skip type and length field for the beginning */
  if (offset < length) {
    offset++; /* flags */
    p_resp->offset += offset;
    p_resp->len -= offset;

    rw_data.data.status = NFC_STATUS_OK;
    rw_data.data.p_data = p_resp;

    p_i93->rw_length += p_resp->len;
  } else {
    /* in case of no Ndef data included */
    p_resp->len = 0;
  }

  /* if read all of NDEF data */
  if (p_i93->rw_length >= p_i93->ndef_length) {
    /* remove extra btyes in the last block */
    p_resp->len -= (p_i93->rw_length - p_i93->ndef_length);

    p_i93->state = RW_I93_STATE_IDLE;
    p_i93->sent_cmd = 0;

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("NDEF read complete read (%d)/total (%d)", p_resp->len,
                        p_i93->ndef_length);

    (*(rw_cb.p_cback))(RW_I93_NDEF_READ_CPLT_EVT, &rw_data);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("NDEF read segment read (%d)/total (%d)", p_resp->len,
                        p_i93->ndef_length);

    if (p_resp->len > 0) {
      (*(rw_cb.p_cback))(RW_I93_NDEF_READ_EVT, &rw_data);
    }

    /* this will make read data from next block */
    p_i93->rw_offset += length;

    if (rw_i93_get_next_blocks(p_i93->rw_offset) != NFC_STATUS_OK) {
      rw_i93_handle_error(NFC_STATUS_FAILED);
    }
  }
}

/*******************************************************************************
**
** Function         rw_i93_sm_update_ndef
**
** Description      Process NDEF update procedure
**
**                  1. Set length field to zero
**                  2. Write NDEF and Terminator TLV
**                  3. Set length field to NDEF length
**
** Returns          void
**
*******************************************************************************/
void rw_i93_sm_update_ndef(NFC_HDR* p_resp) {
  uint8_t* p = (uint8_t*)(p_resp + 1) + p_resp->offset;
  uint8_t flags, xx, length_offset, buff[I93_MAX_BLOCK_LENGH];
  uint16_t length = p_resp->len, block_number;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (0x%x)",
      rw_i93_get_sub_state_name(p_i93->sub_state).c_str(), p_i93->sub_state);

  if (length == 0 || p_i93->block_size > I93_MAX_BLOCK_LENGH) {
    android_errorWriteLog(0x534e4554, "122320256");
    rw_i93_handle_error(NFC_STATUS_FAILED);
    return;
  }

  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    if (((p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) &&
        (*p == I93_ERROR_CODE_BLOCK_FAIL_TO_WRITE)) {
      /* ignore error */
    } else {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Got error flags (0x%02x)", flags);
      rw_i93_handle_error(NFC_STATUS_FAILED);
      return;
    }
  }

  switch (p_i93->sub_state) {
    case RW_I93_SUBSTATE_RESET_LEN:

      /* get offset of length field */
      length_offset = (p_i93->ndef_tlv_start_offset + 1) % p_i93->block_size;

      if (length < length_offset) {
        android_errorWriteLog(0x534e4554, "122320256");
        rw_i93_handle_error(NFC_STATUS_FAILED);
        return;
      }

      /* set length to zero */
      *(p + length_offset) = 0x00;

      if (p_i93->ndef_length > 0) {
        /* if 3 bytes length field is needed */
        if (p_i93->ndef_length >= 0xFF) {
          xx = length_offset + 3;
        } else {
          xx = length_offset + 1;
        }

        /* write the first part of NDEF in the same block */
        for (; xx < p_i93->block_size; xx++) {
          if (xx > length || p_i93->rw_length > p_i93->ndef_length) {
            android_errorWriteLog(0x534e4554, "122320256");
            rw_i93_handle_error(NFC_STATUS_FAILED);
            return;
          }
          if (p_i93->rw_length < p_i93->ndef_length) {
            *(p + xx) = *(p_i93->p_update_data + p_i93->rw_length++);
          } else {
            *(p + xx) = I93_ICODE_TLV_TYPE_NULL;
          }
        }
      }

      block_number = (p_i93->ndef_tlv_start_offset + 1) / p_i93->block_size;

      if (length < p_i93->block_size) {
        android_errorWriteLog(0x534e4554, "143109193");
        rw_i93_handle_error(NFC_STATUS_FAILED);
      } else if (rw_i93_send_cmd_write_single_block(block_number, p) ==
                 NFC_STATUS_OK) {
        /* update next writing offset */
        p_i93->rw_offset = (block_number + 1) * p_i93->block_size;
        p_i93->sub_state = RW_I93_SUBSTATE_WRITE_NDEF;
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_WRITE_NDEF:

      /* if it's not the end of tag memory */
      if (p_i93->rw_offset < p_i93->block_size * p_i93->num_block) {
        block_number = p_i93->rw_offset / p_i93->block_size;

        /* if we have more data to write */
        if (p_i93->rw_length < p_i93->ndef_length) {
          p = p_i93->p_update_data + p_i93->rw_length;

          p_i93->rw_offset += p_i93->block_size;
          p_i93->rw_length += p_i93->block_size;

          /* if this is the last block of NDEF TLV */
          if (p_i93->rw_length > p_i93->ndef_length) {
            /* length of NDEF TLV in the block */
            xx = (uint8_t)(p_i93->block_size -
                           (p_i93->rw_length - p_i93->ndef_length));

            /* set NULL TLV in the unused part of block */
            memset(buff, I93_ICODE_TLV_TYPE_NULL, p_i93->block_size);
            memcpy(buff, p, xx);
            p = buff;

            /* if it's the end of tag memory */
            if ((p_i93->rw_offset >= p_i93->block_size * p_i93->num_block) &&
                (xx < p_i93->block_size)) {
              buff[xx] = I93_ICODE_TLV_TYPE_TERM;
            }

            p_i93->ndef_tlv_last_offset =
                p_i93->rw_offset - p_i93->block_size + xx - 1;
          }

          if (rw_i93_send_cmd_write_single_block(block_number, p) !=
              NFC_STATUS_OK) {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        } else {
          /* if this is the very next block of NDEF TLV */
          if (block_number ==
              (p_i93->ndef_tlv_last_offset / p_i93->block_size) + 1) {
            p_i93->rw_offset += p_i93->block_size;

            /* write Terminator TLV and NULL TLV */
            memset(buff, I93_ICODE_TLV_TYPE_NULL, p_i93->block_size);
            buff[0] = I93_ICODE_TLV_TYPE_TERM;
            p = buff;

            if (rw_i93_send_cmd_write_single_block(block_number, p) !=
                NFC_STATUS_OK) {
              rw_i93_handle_error(NFC_STATUS_FAILED);
            }
          } else {
            /* finished writing NDEF and Terminator TLV */
            /* read length field to update length       */
            block_number =
                (p_i93->ndef_tlv_start_offset + 1) / p_i93->block_size;

            if (rw_i93_send_cmd_read_single_block(block_number, false) ==
                NFC_STATUS_OK) {
              /* set offset to length field */
              p_i93->rw_offset = p_i93->ndef_tlv_start_offset + 1;

              /* get size of length field */
              if (p_i93->ndef_length >= 0xFF) {
                p_i93->rw_length = 3;
              } else if (p_i93->ndef_length > 0) {
                p_i93->rw_length = 1;
              } else {
                p_i93->rw_length = 0;
              }

              p_i93->sub_state = RW_I93_SUBSTATE_UPDATE_LEN;
            } else {
              rw_i93_handle_error(NFC_STATUS_FAILED);
            }
          }
        }
      } else {
        /* if we have no more data to write */
        if (p_i93->rw_length >= p_i93->ndef_length) {
          /* finished writing NDEF and Terminator TLV */
          /* read length field to update length       */
          block_number = (p_i93->ndef_tlv_start_offset + 1) / p_i93->block_size;

          if (rw_i93_send_cmd_read_single_block(block_number, false) ==
              NFC_STATUS_OK) {
            /* set offset to length field */
            p_i93->rw_offset = p_i93->ndef_tlv_start_offset + 1;

            /* get size of length field */
            if (p_i93->ndef_length >= 0xFF) {
              p_i93->rw_length = 3;
            } else if (p_i93->ndef_length > 0) {
              p_i93->rw_length = 1;
            } else {
              p_i93->rw_length = 0;
            }

            p_i93->sub_state = RW_I93_SUBSTATE_UPDATE_LEN;
            break;
          }
        }
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_UPDATE_LEN:

      /* if we have more length field to write */
      if (p_i93->rw_length > 0) {
        /* if we got ack for writing, read next block to update rest of length
         * field */
        if (length == 0) {
          block_number = p_i93->rw_offset / p_i93->block_size;

          if (rw_i93_send_cmd_read_single_block(block_number, false) !=
              NFC_STATUS_OK) {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        } else {
          length_offset = p_i93->rw_offset % p_i93->block_size;

          /* update length field within the read block */
          for (xx = length_offset; xx < p_i93->block_size; xx++) {
            if (xx > length) {
              android_errorWriteLog(0x534e4554, "122320256");
              rw_i93_handle_error(NFC_STATUS_FAILED);
              return;
            }

            if (p_i93->rw_length == 3)
              *(p + xx) = 0xFF;
            else if (p_i93->rw_length == 2)
              *(p + xx) = (uint8_t)((p_i93->ndef_length >> 8) & 0xFF);
            else if (p_i93->rw_length == 1)
              *(p + xx) = (uint8_t)(p_i93->ndef_length & 0xFF);

            p_i93->rw_length--;
            if (p_i93->rw_length == 0) break;
          }

          block_number = (p_i93->rw_offset / p_i93->block_size);

          if (length < p_i93->block_size) {
            android_errorWriteLog(0x534e4554, "143155861");
            rw_i93_handle_error(NFC_STATUS_FAILED);
          } else if (rw_i93_send_cmd_write_single_block(block_number, p) ==
                     NFC_STATUS_OK) {
            /* set offset to the beginning of next block */
            p_i93->rw_offset +=
                p_i93->block_size - (p_i93->rw_offset % p_i93->block_size);
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        }
      } else {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "NDEF update complete, %d bytes, (%d-%d)", p_i93->ndef_length,
            p_i93->ndef_tlv_start_offset, p_i93->ndef_tlv_last_offset);

        p_i93->state = RW_I93_STATE_IDLE;
        p_i93->sent_cmd = 0;
        p_i93->p_update_data = nullptr;

        rw_data.status = NFC_STATUS_OK;
        (*(rw_cb.p_cback))(RW_I93_NDEF_UPDATE_CPLT_EVT, &rw_data);
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_i93_sm_format
**
** Description      Process format procedure
**
**                  1. Get UID
**                  2. Get sys info for memory size (reset AFI/DSFID)
**                  3. Get block status to get read-only status
**                  4. Write CC and empty NDEF
**
** Returns          void
**
*******************************************************************************/
void rw_i93_sm_format(NFC_HDR* p_resp) {
  uint8_t *p = (uint8_t*)(p_resp + 1) + p_resp->offset, *p_uid;
  uint8_t flags;
  uint16_t length = p_resp->len, xx, block_number;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (0x%x)",
      rw_i93_get_sub_state_name(p_i93->sub_state).c_str(), p_i93->sub_state);

  if (length == 0) {
    android_errorWriteLog(0x534e4554, "122323053");
    return;
  }
  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    if (((p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) &&
        (*p == I93_ERROR_CODE_BLOCK_FAIL_TO_WRITE)) {
      /* ignore error */
    } else if ((length) && (rw_i93_check_sys_info_prot_ext(*p))) {
      /* getting system info with protocol extension flag */
      /* This STM & ONS tag supports more than 2040 bytes */
      p_i93->intl_flags |= RW_I93_FLAG_16BIT_NUM_BLOCK;
      return;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Got error flags (0x%02x)", flags);
      rw_i93_handle_error(NFC_STATUS_FAILED);
      return;
    }
  }

  switch (p_i93->sub_state) {
    case RW_I93_SUBSTATE_WAIT_UID:

      if (length < (I93_UID_BYTE_LEN + 1)) {
        android_errorWriteLog(0x534e4554, "122323053");
        return;
      }
      p++; /* skip DSFID */
      p_uid = p_i93->uid;
      STREAM_TO_ARRAY8(p_uid, p); /* store UID */

      /* get system information to get memory size */
      if (rw_i93_send_cmd_get_sys_info(nullptr, I93_FLAG_PROT_EXT_NO) ==
          NFC_STATUS_OK) {
        p_i93->sub_state = RW_I93_SUBSTATE_WAIT_SYS_INFO;
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_WAIT_SYS_INFO:

      p_i93->block_size = 0;
      p_i93->num_block = 0;

      if (!rw_i93_process_sys_info(p, length)) {
        /* retrying with protocol extension flag */
        break;
      }

      if (p_i93->info_flags & I93_INFO_FLAG_DSFID) {
        /* DSFID, if any DSFID then reset */
        if (p_i93->dsfid != I93_DFS_UNSUPPORTED) {
          p_i93->intl_flags |= RW_I93_FLAG_RESET_DSFID;
        }
      }
      if (p_i93->info_flags & I93_INFO_FLAG_AFI) {
        /* AFI, reset to 0 */
        if (p_i93->afi != 0x00) {
          p_i93->intl_flags |= RW_I93_FLAG_RESET_AFI;
        }
      }

      if ((p_i93->block_size == 0) || (p_i93->num_block == 0)) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Unable to get tag memory size");
        rw_i93_handle_error(status);
      } else if (p_i93->intl_flags & RW_I93_FLAG_RESET_DSFID) {
        if (rw_i93_send_cmd_write_dsfid(I93_DFS_UNSUPPORTED) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else if (p_i93->intl_flags & RW_I93_FLAG_RESET_AFI) {
        if (rw_i93_send_cmd_write_afi(0x00) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else {
        /* get lock status to see if read-only */
        if ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
            (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK)) {
          /* these doesn't support GetMultiBlockSecurityStatus */

          rw_cb.tcb.i93.rw_offset = 0;

          /* read blocks with option flag to get block security status */
          if (rw_i93_send_cmd_read_single_block(0x0000, true) ==
              NFC_STATUS_OK) {
            p_i93->sub_state = RW_I93_SUBSTATE_CHECK_READ_ONLY;
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        } else {
          /* block offset for read-only check */
          p_i93->rw_offset = 0;

          if (rw_i93_get_next_block_sec() == NFC_STATUS_OK) {
            p_i93->sub_state = RW_I93_SUBSTATE_CHECK_READ_ONLY;
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        }
      }

      break;

    case RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI:

      if (p_i93->sent_cmd == I93_CMD_WRITE_DSFID) {
        p_i93->intl_flags &= ~RW_I93_FLAG_RESET_DSFID;
      } else if (p_i93->sent_cmd == I93_CMD_WRITE_AFI) {
        p_i93->intl_flags &= ~RW_I93_FLAG_RESET_AFI;
      }

      if (p_i93->intl_flags & RW_I93_FLAG_RESET_DSFID) {
        if (rw_i93_send_cmd_write_dsfid(I93_DFS_UNSUPPORTED) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else if (p_i93->intl_flags & RW_I93_FLAG_RESET_AFI) {
        if (rw_i93_send_cmd_write_afi(0x00) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else {
        /* get lock status to see if read-only */
        if ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
            (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK)) {
          /* these doesn't support GetMultiBlockSecurityStatus */

          rw_cb.tcb.i93.rw_offset = 0;

          /* read blocks with option flag to get block security status */
          if (rw_i93_send_cmd_read_single_block(0x0000, true) ==
              NFC_STATUS_OK) {
            p_i93->sub_state = RW_I93_SUBSTATE_CHECK_READ_ONLY;
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        } else {
          /* block offset for read-only check */
          p_i93->rw_offset = 0;

          if (rw_i93_get_next_block_sec() == NFC_STATUS_OK) {
            p_i93->sub_state = RW_I93_SUBSTATE_CHECK_READ_ONLY;
          } else {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
        }
      }
      break;

    case RW_I93_SUBSTATE_CHECK_READ_ONLY:

      if ((p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
          (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY) ||
          ((p_i93->uid[1] == I93_UID_IC_MFG_CODE_NXP) &&
           (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK))) {
        if (length == 0 || ((*p) & I93_BLOCK_LOCKED)) {
          rw_i93_handle_error(NFC_STATUS_FAILED);
          break;
        }

        /* if we checked all of user blocks */
        if ((p_i93->rw_offset / p_i93->block_size) + 1 == p_i93->num_block) {
          if ((p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
              (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) {
            /* read the block which has AFI */
            p_i93->rw_offset = I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_AFI_LOCATION;
            rw_i93_send_cmd_read_single_block(
                (uint16_t)(p_i93->rw_offset / p_i93->block_size), true);
            break;
          }
        } else if (p_i93->rw_offset ==
                   I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_AFI_LOCATION) {
          /* no block is locked */
        } else {
          p_i93->rw_offset += p_i93->block_size;
          rw_i93_send_cmd_read_single_block(
              (uint16_t)(p_i93->rw_offset / p_i93->block_size), true);
          break;
        }
      } else {
        /* if any block is locked, we cannot format it */
        for (xx = 0; xx < length; xx++) {
          if (*(p + xx) & I93_BLOCK_LOCKED) {
            rw_i93_handle_error(NFC_STATUS_FAILED);
            break;
          }
        }

        /* update block offset for read-only check */
        p_i93->rw_offset += length;

        /* if need to get more lock status of blocks */
        if (p_i93->num_block > p_i93->rw_offset) {
          if (rw_i93_get_next_block_sec() != NFC_STATUS_OK) {
            rw_i93_handle_error(NFC_STATUS_FAILED);
          }
          break;
        }
      }

      /* get buffer to store CC, zero length NDEF TLV and Terminator TLV */
      /* Block size could be either 4 or 8 or 16 or 32 bytes */
      /* Get buffer for the largest block size I93_MAX_BLOCK_LENGH */
      p_i93->p_update_data = (uint8_t*)GKI_getbuf(I93_MAX_BLOCK_LENGH);

      if (!p_i93->p_update_data) {
        LOG(ERROR) << StringPrintf("Cannot allocate buffer");
        rw_i93_handle_error(NFC_STATUS_FAILED);
        break;
      } else if (p_i93->block_size > RW_I93_FORMAT_DATA_LEN) {
        /* Possible leaking information from previous NFC transactions */
        /* Clear previous values */
        memset(p_i93->p_update_data, I93_ICODE_TLV_TYPE_NULL,
               I93_MAX_BLOCK_LENGH);
        android_errorWriteLog(0x534e4554, "139738828");
      }

      p = p_i93->p_update_data;

      /* Capability Container */
      *(p++) = I93_ICODE_CC_MAGIC_NUMER_E1; /* magic number */
      *(p++) = 0x40;                     /* version 1.0, read/write */

      /* if memory size is less than 2048 bytes */
      if (((p_i93->num_block * p_i93->block_size) / 8) < 0x100)
        *(p++) = (uint8_t)((p_i93->num_block * p_i93->block_size) /
                           8); /* memory size */
      else
        *(p++) = 0xFF;

      if ((p_i93->product_version == RW_I93_ICODE_SLI) ||
          (p_i93->product_version == RW_I93_ICODE_SLI_S) ||
          (p_i93->product_version == RW_I93_ICODE_SLI_L)) {
        if (p_i93->ic_reference & I93_ICODE_IC_REF_MBREAD_MASK)
          *(p++) = I93_ICODE_CC_IPREAD_MASK; /* IPREAD */
        else
          *(p++) = I93_ICODE_CC_MBREAD_MASK; /* MBREAD, read multi block command
                                                supported */
      } else if ((p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
                 (p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP)) {
        *(p++) = I93_ICODE_CC_MBREAD_MASK; /* MBREAD, read multi block command
                                              supported */
      } else if ((p_i93->product_version ==
                  RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
                 (p_i93->product_version ==
                  RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) {
        *(p++) = 0;
      } else {
        /* STM except LRIS2K, ONS, Broadcom supports read multi block command */

        /* if memory size is more than 2040 bytes (which is not LRIS2K) */
        if (((p_i93->num_block * p_i93->block_size) / 8) > 0xFF)
          *(p++) = (I93_ICODE_CC_MBREAD_MASK | I93_STM_CC_OVERFLOW_MASK);
        else if (p_i93->product_version == RW_I93_STM_LRIS2K)
          *(p++) = 0x00;
        else
          *(p++) = I93_ICODE_CC_MBREAD_MASK;
      }

      /* zero length NDEF and Terminator TLV */
      *(p++) = I93_ICODE_TLV_TYPE_NDEF;
      *(p++) = 0x00;
      *(p++) = I93_ICODE_TLV_TYPE_TERM;
      *(p++) = I93_ICODE_TLV_TYPE_NULL;

      /* start from block 0 */
      p_i93->rw_offset = 0;

      if (rw_i93_send_cmd_write_single_block(0, p_i93->p_update_data) ==
          NFC_STATUS_OK) {
        p_i93->sub_state = RW_I93_SUBSTATE_WRITE_CC_NDEF_TLV;
        p_i93->rw_offset += p_i93->block_size;
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_WRITE_CC_NDEF_TLV:

      /* if we have more data to write */
      if (p_i93->rw_offset < RW_I93_FORMAT_DATA_LEN) {
        block_number = (p_i93->rw_offset / p_i93->block_size);
        p = p_i93->p_update_data + p_i93->rw_offset;

        if (rw_i93_send_cmd_write_single_block(block_number, p) ==
            NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WRITE_CC_NDEF_TLV;
          p_i93->rw_offset += p_i93->block_size;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else {
        GKI_freebuf(p_i93->p_update_data);
        p_i93->p_update_data = nullptr;

        p_i93->state = RW_I93_STATE_IDLE;
        p_i93->sent_cmd = 0;

        rw_data.status = NFC_STATUS_OK;
        (*(rw_cb.p_cback))(RW_I93_FORMAT_CPLT_EVT, &rw_data);
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_i93_sm_set_read_only
**
** Description      Process read-only procedure
**
**                  1. Update CC as read-only
**                  2. Lock all block of NDEF TLV
**                  3. Lock block of CC
**
** Returns          void
**
*******************************************************************************/
void rw_i93_sm_set_read_only(NFC_HDR* p_resp) {
  uint8_t* p = (uint8_t*)(p_resp + 1) + p_resp->offset;
  uint8_t flags, block_number;
  uint16_t length = p_resp->len;
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "sub_state:%s (0x%x)",
      rw_i93_get_sub_state_name(p_i93->sub_state).c_str(), p_i93->sub_state);

  if (length == 0) {
    android_errorWriteLog(0x534e4554, "122322613");
    rw_i93_handle_error(NFC_STATUS_FAILED);
    return;
  }

  STREAM_TO_UINT8(flags, p);
  length--;

  if (flags & I93_FLAG_ERROR_DETECTED) {
    if (((p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PLUS_CHIP) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
         (p_i93->product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) &&
        (*p == I93_ERROR_CODE_BLOCK_FAIL_TO_WRITE)) {
      /* ignore error */
    } else {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Got error flags (0x%02x)", flags);
      rw_i93_handle_error(NFC_STATUS_FAILED);
      return;
    }
  }

  switch (p_i93->sub_state) {
    case RW_I93_SUBSTATE_WAIT_CC:

      if (length < RW_I93_CC_SIZE) {
        android_errorWriteLog(0x534e4554, "139188579");
        rw_i93_handle_error(NFC_STATUS_FAILED);
        return;
      }

      /* mark CC as read-only */
      *(p + 1) |= I93_ICODE_CC_READ_ONLY;

      if (length < p_i93->block_size) {
        android_errorWriteLog(0x534e4554, "143106535");
        rw_i93_handle_error(NFC_STATUS_FAILED);
      } else if (rw_i93_send_cmd_write_single_block(0, p) == NFC_STATUS_OK) {
        p_i93->sub_state = RW_I93_SUBSTATE_WAIT_UPDATE_CC;
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_WAIT_UPDATE_CC:

      /* successfully write CC then lock all blocks of NDEF TLV */
      p_i93->rw_offset = p_i93->ndef_tlv_start_offset;
      block_number = (uint8_t)(p_i93->rw_offset / p_i93->block_size);

      if (rw_i93_send_cmd_lock_block(block_number) == NFC_STATUS_OK) {
        p_i93->rw_offset += p_i93->block_size;
        p_i93->sub_state = RW_I93_SUBSTATE_LOCK_NDEF_TLV;
      } else {
        rw_i93_handle_error(NFC_STATUS_FAILED);
      }
      break;

    case RW_I93_SUBSTATE_LOCK_NDEF_TLV:

      /* if we need to lock more blocks */
      if (p_i93->rw_offset < p_i93->ndef_tlv_last_offset) {
        /* get the next block of NDEF TLV */
        block_number = (uint8_t)(p_i93->rw_offset / p_i93->block_size);

        if (rw_i93_send_cmd_lock_block(block_number) == NFC_STATUS_OK) {
          p_i93->rw_offset += p_i93->block_size;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      }
      /* if the first block of NDEF TLV is different from block of CC */
      else if (p_i93->ndef_tlv_start_offset / p_i93->block_size != 0) {
        /* lock block of CC */
        if (rw_i93_send_cmd_lock_block(0) == NFC_STATUS_OK) {
          p_i93->sub_state = RW_I93_SUBSTATE_WAIT_LOCK_CC;
        } else {
          rw_i93_handle_error(NFC_STATUS_FAILED);
        }
      } else {
        p_i93->intl_flags |= RW_I93_FLAG_READ_ONLY;
        p_i93->state = RW_I93_STATE_IDLE;
        p_i93->sent_cmd = 0;

        rw_data.status = NFC_STATUS_OK;
        (*(rw_cb.p_cback))(RW_I93_SET_TAG_RO_EVT, &rw_data);
      }
      break;

    case RW_I93_SUBSTATE_WAIT_LOCK_CC:

      p_i93->intl_flags |= RW_I93_FLAG_READ_ONLY;
      p_i93->state = RW_I93_STATE_IDLE;
      p_i93->sent_cmd = 0;

      rw_data.status = NFC_STATUS_OK;
      (*(rw_cb.p_cback))(RW_I93_SET_TAG_RO_EVT, &rw_data);
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_i93_handle_error
**
** Description      notify error to application and clean up
**
** Returns          none
**
*******************************************************************************/
void rw_i93_handle_error(tNFC_STATUS status) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  tRW_DATA rw_data;
  tRW_EVENT event;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("status:0x%02X, state:0x%X", status, p_i93->state);

  nfc_stop_quick_timer(&p_i93->timer);

  if (rw_cb.p_cback) {
    rw_data.status = status;

    switch (p_i93->state) {
      case RW_I93_STATE_IDLE: /* in case of RawFrame */
        event = RW_I93_INTF_ERROR_EVT;
        break;

      case RW_I93_STATE_BUSY:
        if (p_i93->sent_cmd == I93_CMD_STAY_QUIET) {
          /* There is no response to Stay Quiet command */
          rw_data.i93_cmd_cmpl.status = NFC_STATUS_OK;
          rw_data.i93_cmd_cmpl.command = I93_CMD_STAY_QUIET;
          rw_data.i93_cmd_cmpl.error_code = 0;
          event = RW_I93_CMD_CMPL_EVT;
        } else {
          event = RW_I93_INTF_ERROR_EVT;
        }
        break;

      case RW_I93_STATE_DETECT_NDEF:
        rw_data.ndef.protocol = NFC_PROTOCOL_T5T;
        rw_data.ndef.cur_size = 0;
        rw_data.ndef.max_size = 0;
        rw_data.ndef.flags = 0;
        rw_data.ndef.flags |= RW_NDEF_FL_FORMATABLE;
        rw_data.ndef.flags |= RW_NDEF_FL_UNKNOWN;
        event = RW_I93_NDEF_DETECT_EVT;
        break;

      case RW_I93_STATE_READ_NDEF:
        event = RW_I93_NDEF_READ_FAIL_EVT;
        break;

      case RW_I93_STATE_UPDATE_NDEF:
        p_i93->p_update_data = nullptr;
        event = RW_I93_NDEF_UPDATE_FAIL_EVT;
        break;

      case RW_I93_STATE_FORMAT:
        if (p_i93->p_update_data) {
          GKI_freebuf(p_i93->p_update_data);
          p_i93->p_update_data = nullptr;
        }
        event = RW_I93_FORMAT_CPLT_EVT;
        break;

      case RW_I93_STATE_SET_READ_ONLY:
        event = RW_I93_SET_TAG_RO_EVT;
        break;

      case RW_I93_STATE_PRESENCE_CHECK:
        event = RW_I93_PRESENCE_CHECK_EVT;
        break;

      default:
        event = RW_I93_MAX_EVT;
        break;
    }

    p_i93->state = RW_I93_STATE_IDLE;
    p_i93->sent_cmd = 0;

    if (event != RW_I93_MAX_EVT) {
      (*(rw_cb.p_cback))(event, &rw_data);
    }
  } else {
    p_i93->state = RW_I93_STATE_IDLE;
  }
}

/*******************************************************************************
**
** Function         rw_i93_process_timeout
**
** Description      process timeout event
**
** Returns          none
**
*******************************************************************************/
void rw_i93_process_timeout(TIMER_LIST_ENT* p_tle) {
  NFC_HDR* p_buf;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event=%d", p_tle->event);

  if (p_tle->event == NFC_TTYPE_RW_I93_RESPONSE) {
    if ((rw_cb.tcb.i93.retry_count < RW_MAX_RETRIES) &&
        (rw_cb.tcb.i93.p_retry_cmd) &&
        (rw_cb.tcb.i93.sent_cmd != I93_CMD_STAY_QUIET)) {
      rw_cb.tcb.i93.retry_count++;
      LOG(ERROR) << StringPrintf("retry_count = %d", rw_cb.tcb.i93.retry_count);

      p_buf = rw_cb.tcb.i93.p_retry_cmd;
      rw_cb.tcb.i93.p_retry_cmd = nullptr;

      if (rw_i93_send_to_lower(p_buf)) {
        return;
      }
    }

    /* all retrial is done or failed to send command to lower layer */
    if (rw_cb.tcb.i93.p_retry_cmd) {
      GKI_freebuf(rw_cb.tcb.i93.p_retry_cmd);
      rw_cb.tcb.i93.p_retry_cmd = nullptr;
      rw_cb.tcb.i93.retry_count = 0;
    }
    rw_i93_handle_error(NFC_STATUS_TIMEOUT);
  } else {
    LOG(ERROR) << StringPrintf("unknown event=%d", p_tle->event);
  }
}

/*******************************************************************************
**
** Function         rw_i93_data_cback
**
** Description      This callback function receives the data from NFCC.
**
** Returns          none
**
*******************************************************************************/
static void rw_i93_data_cback(__attribute__((unused)) uint8_t conn_id,
                              tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  NFC_HDR* p_resp;
  tRW_DATA rw_data;

  uint8_t begin_state = p_i93->state;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event = 0x%X", event);

  if ((event == NFC_DEACTIVATE_CEVT) || (event == NFC_ERROR_CEVT) ||
      ((event == NFC_DATA_CEVT) && (p_data->status != NFC_STATUS_OK))) {
    nfc_stop_quick_timer(&p_i93->timer);

    if (event == NFC_ERROR_CEVT || (p_data->status != NFC_STATUS_OK)) {
      if ((p_i93->retry_count < RW_MAX_RETRIES) && (p_i93->p_retry_cmd)) {
        p_i93->retry_count++;

        LOG(ERROR) << StringPrintf("retry_count = %d", p_i93->retry_count);

        p_resp = p_i93->p_retry_cmd;
        p_i93->p_retry_cmd = nullptr;
        if (rw_i93_send_to_lower(p_resp)) {
          if (event == NFC_DATA_CEVT) {
            p_resp = (NFC_HDR*)p_data->data.p_data;
            GKI_freebuf(p_resp);
          }
          return;
        }
      }

      /* all retrial is done or failed to send command to lower layer */
      if (p_i93->p_retry_cmd) {
        GKI_freebuf(p_i93->p_retry_cmd);
        p_i93->p_retry_cmd = nullptr;
        p_i93->retry_count = 0;
      }

      rw_i93_handle_error((tNFC_STATUS)(*(uint8_t*)p_data));
    } else {
      /* free retry buffer */
      if (p_i93->p_retry_cmd) {
        GKI_freebuf(p_i93->p_retry_cmd);
        p_i93->p_retry_cmd = nullptr;
        p_i93->retry_count = 0;
      }
      NFC_SetStaticRfCback(nullptr);
      p_i93->state = RW_I93_STATE_NOT_ACTIVATED;
    }
    if ((event == NFC_DATA_CEVT) && (p_data->status != NFC_STATUS_OK)) {
      p_resp = (NFC_HDR*)p_data->data.p_data;
      GKI_freebuf(p_resp);
    }
    return;
  }

  if (event != NFC_DATA_CEVT) {
    return;
  }

  p_resp = (NFC_HDR*)p_data->data.p_data;

  nfc_stop_quick_timer(&p_i93->timer);

  /* free retry buffer */
  if (p_i93->p_retry_cmd) {
    GKI_freebuf(p_i93->p_retry_cmd);
    p_i93->p_retry_cmd = nullptr;
    p_i93->retry_count = 0;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "RW I93 state: <%s (%d)>", rw_i93_get_state_name(p_i93->state).c_str(),
      p_i93->state);

  switch (p_i93->state) {
    case RW_I93_STATE_IDLE:
      /* Unexpected Response from VICC, it should be raw frame response */
      /* forward to upper layer without parsing */
      p_i93->sent_cmd = 0;
      if (rw_cb.p_cback) {
        rw_data.raw_frame.status = p_data->data.status;
        rw_data.raw_frame.p_data = p_resp;
        (*(rw_cb.p_cback))(RW_I93_RAW_FRAME_EVT, &rw_data);
        p_resp = nullptr;
      } else {
        GKI_freebuf(p_resp);
      }
      break;
    case RW_I93_STATE_BUSY:
      p_i93->state = RW_I93_STATE_IDLE;
      rw_i93_send_to_upper(p_resp);
      GKI_freebuf(p_resp);
      break;

    case RW_I93_STATE_DETECT_NDEF:
      rw_i93_sm_detect_ndef(p_resp);
      GKI_freebuf(p_resp);
      break;

    case RW_I93_STATE_READ_NDEF:
      rw_i93_sm_read_ndef(p_resp);
      /* p_resp may send upper lyaer */
      break;

    case RW_I93_STATE_UPDATE_NDEF:
      rw_i93_sm_update_ndef(p_resp);
      GKI_freebuf(p_resp);
      break;

    case RW_I93_STATE_FORMAT:
      rw_i93_sm_format(p_resp);
      GKI_freebuf(p_resp);
      break;

    case RW_I93_STATE_SET_READ_ONLY:
      rw_i93_sm_set_read_only(p_resp);
      GKI_freebuf(p_resp);
      break;

    case RW_I93_STATE_PRESENCE_CHECK:
      p_i93->state = RW_I93_STATE_IDLE;
      p_i93->sent_cmd = 0;

      /* depending of response length, send presence check with ok or failed */
      if (p_resp->len > 1) {
        rw_data.status  = NFC_STATUS_OK;
      } else {
        rw_data.status  = NFC_STATUS_FAILED;
      }
      (*(rw_cb.p_cback))(RW_I93_PRESENCE_CHECK_EVT, &rw_data);
      GKI_freebuf(p_resp);
      break;

    default:
      LOG(ERROR) << StringPrintf("invalid state=%d", p_i93->state);
      GKI_freebuf(p_resp);
      break;
  }

  if (begin_state != p_i93->state) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW I93 state changed:<%s> -> <%s>",
                        rw_i93_get_state_name(begin_state).c_str(),
                        rw_i93_get_state_name(p_i93->state).c_str());
  }
}

/*******************************************************************************
**
** Function         rw_i93_select
**
** Description      Initialise ISO 15693 / T5T RW
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS rw_i93_select(uint8_t* p_uid) {
  tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
  uint8_t uid[I93_UID_BYTE_LEN], *p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  NFC_SetStaticRfCback(rw_i93_data_cback);

  p_i93->state = RW_I93_STATE_IDLE;

  /* convert UID to big endian format - MSB(0xE0) in first byte */
  p = uid;
  STREAM_TO_ARRAY8(p, p_uid);

  rw_i93_get_product_version(uid);

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_I93Inventory
**
** Description      This function send Inventory command with/without AFI
**                  If UID is provided then set UID[0]:MSB, ... UID[7]:LSB
**
**                  RW_I93_RESPONSE_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93Inventory(bool including_afi, uint8_t afi, uint8_t* p_uid) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf(", including_afi:%d, AFI:0x%02X", including_afi, afi);

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_inventory(p_uid, including_afi, afi);

  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return (status);
}

/*******************************************************************************
**
** Function         RW_I93StayQuiet
**
** Description      This function send Inventory command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93StayQuiet(void) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_stay_quiet();
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93ReadSingleBlock
**
** Description      This function send Read Single Block command
**
**                  RW_I93_RESPONSE_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93ReadSingleBlock(uint16_t block_number) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("block_number:0x%02X", block_number);

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_read_single_block(block_number, false);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93WriteSingleBlock
**
** Description      This function send Write Single Block command
**                  Application must get block size first by calling
**                  RW_I93GetSysInfo().
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93WriteSingleBlock(uint16_t block_number, uint8_t* p_data) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  if (rw_cb.tcb.i93.block_size == 0) {
    LOG(ERROR) << StringPrintf("Block size is unknown");
    return NFC_STATUS_FAILED;
  }

  status = rw_i93_send_cmd_write_single_block(block_number, p_data);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93LockBlock
**
** Description      This function send Lock Block command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93LockBlock(uint8_t block_number) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_lock_block(block_number);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93ReadMultipleBlocks
**
** Description      This function send Read Multiple Blocks command
**
**                  RW_I93_RESPONSE_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93ReadMultipleBlocks(uint16_t first_block_number,
                                     uint16_t number_blocks) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_read_multi_blocks(first_block_number, number_blocks);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93WriteMultipleBlocks
**
** Description      This function send Write Multiple Blocks command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93WriteMultipleBlocks(uint16_t first_block_number,
                                      uint16_t number_blocks, uint8_t* p_data) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  if (rw_cb.tcb.i93.block_size == 0) {
    LOG(ERROR) << StringPrintf("Block size is unknown");
    return NFC_STATUS_FAILED;
  }

  status = rw_i93_send_cmd_write_multi_blocks(first_block_number, number_blocks,
                                              p_data);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93Select
**
** Description      This function send Select command
**
**                  UID[0]: 0xE0, MSB
**                  UID[1]: IC Mfg Code
**                  ...
**                  UID[7]: LSB
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93Select(uint8_t* p_uid) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  if (p_uid) {
    status = rw_i93_send_cmd_select(p_uid);
    if (status == NFC_STATUS_OK) {
      rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
    }
  } else {
    LOG(ERROR) << StringPrintf("UID shall be provided");
    status = NFC_STATUS_FAILED;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93ResetToReady
**
** Description      This function send Reset To Ready command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93ResetToReady(void) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_reset_to_ready();
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93WriteAFI
**
** Description      This function send Write AFI command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93WriteAFI(uint8_t afi) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_write_afi(afi);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93LockAFI
**
** Description      This function send Lock AFI command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93LockAFI(void) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_lock_afi();
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93WriteDSFID
**
** Description      This function send Write DSFID command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93WriteDSFID(uint8_t dsfid) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_write_dsfid(dsfid);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93LockDSFID
**
** Description      This function send Lock DSFID command
**
**                  RW_I93_CMD_CMPL_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93LockDSFID(void) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status = rw_i93_send_cmd_lock_dsfid();
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93GetSysInfo
**
** Description      This function send Get System Information command
**
**                  RW_I93_RESPONSE_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93GetSysInfo(uint8_t* p_uid) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  if (p_uid) {
    status = rw_i93_send_cmd_get_sys_info(p_uid, I93_FLAG_PROT_EXT_NO);
  } else {
    status = rw_i93_send_cmd_get_sys_info(nullptr, I93_FLAG_PROT_EXT_NO);
  }

  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93GetMultiBlockSecurityStatus
**
** Description      This function send Get Multiple Block Security Status
**                  command
**
**                  RW_I93_RESPONSE_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_NO_BUFFERS if out of buffer
**                  NFC_STATUS_BUSY if busy
**                  NFC_STATUS_FAILED if other error
**
*******************************************************************************/
tNFC_STATUS RW_I93GetMultiBlockSecurityStatus(uint16_t first_block_number,
                                              uint16_t number_blocks) {
  tNFC_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Unable to start command at state "
        "(0x%X)",
        rw_cb.tcb.i93.state);
    return NFC_STATUS_BUSY;
  }

  status =
      rw_i93_send_cmd_get_multi_block_sec(first_block_number, number_blocks);
  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_BUSY;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_I93DetectNDef
**
** Description      This function performs NDEF detection procedure
**
**                  RW_I93_NDEF_DETECT_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_I93DetectNDef(void) {
  tNFC_STATUS status;
  tRW_I93_RW_SUBSTATE sub_state;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_FAILED;
  }

  if (rw_cb.tcb.i93.uid[0] != I93_UID_FIRST_BYTE) {
    status = rw_i93_send_cmd_inventory(nullptr, false, 0x00);
    sub_state = RW_I93_SUBSTATE_WAIT_UID;
  } else if ((rw_cb.tcb.i93.num_block == 0) ||
             (rw_cb.tcb.i93.block_size == 0)) {
    status =
        rw_i93_send_cmd_get_sys_info(rw_cb.tcb.i93.uid, I93_FLAG_PROT_EXT_NO);
    sub_state = RW_I93_SUBSTATE_WAIT_SYS_INFO;

    /* clear all flags */
    rw_cb.tcb.i93.intl_flags = 0;
  } else {
    /* read CC in the first block */
    status = rw_i93_send_cmd_read_single_block(0x0000, false);
    sub_state = RW_I93_SUBSTATE_WAIT_CC;
  }

  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_DETECT_NDEF;
    rw_cb.tcb.i93.sub_state = sub_state;

    /* clear flags except flag for 2 bytes of number of blocks */
    rw_cb.tcb.i93.intl_flags &= RW_I93_FLAG_16BIT_NUM_BLOCK;
  }

  return (status);
}

/*******************************************************************************
**
** Function         RW_I93ReadNDef
**
** Description      This function performs NDEF read procedure
**                  Note: RW_I93DetectNDef () must be called before using this
**
**                  The following event will be returned
**                      RW_I93_NDEF_READ_EVT for each segmented NDEF message
**                      RW_I93_NDEF_READ_CPLT_EVT for the last segment or
**                      complete NDEF
**                      RW_I93_NDEF_READ_FAIL_EVT for failure
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if I93 is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_I93ReadNDef(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_FAILED;
  }

  if ((rw_cb.tcb.i93.tlv_type == I93_ICODE_TLV_TYPE_NDEF) &&
      (rw_cb.tcb.i93.ndef_length > 0)) {
    rw_cb.tcb.i93.rw_offset = rw_cb.tcb.i93.ndef_tlv_start_offset;
    rw_cb.tcb.i93.rw_length = 0;

    if (rw_i93_get_next_blocks(rw_cb.tcb.i93.rw_offset) == NFC_STATUS_OK) {
      rw_cb.tcb.i93.state = RW_I93_STATE_READ_NDEF;
    } else {
      return NFC_STATUS_FAILED;
    }
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_I93UpdateNDef
**
** Description      This function performs NDEF update procedure
**                  Note: RW_I93DetectNDef () must be called before using this
**                        Updating data must not be removed until returning
**                        event
**
**                  The following event will be returned
**                      RW_I93_NDEF_UPDATE_CPLT_EVT for complete
**                      RW_I93_NDEF_UPDATE_FAIL_EVT for failure
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if I93 is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_I93UpdateNDef(uint16_t length, uint8_t* p_data) {
  uint16_t block_number;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("length:%d", length);

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_FAILED;
  }

  if (rw_cb.tcb.i93.tlv_type == I93_ICODE_TLV_TYPE_NDEF) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_READ_ONLY) {
      LOG(ERROR) << StringPrintf("NDEF is read-only");
      return NFC_STATUS_FAILED;
    }
    if (rw_cb.tcb.i93.max_ndef_length < length) {
      LOG(ERROR) << StringPrintf(
          "data (%d bytes) is more than max NDEF length "
          "(%d)",
          length, rw_cb.tcb.i93.max_ndef_length);
      return NFC_STATUS_FAILED;
    }

    rw_cb.tcb.i93.ndef_length = length;
    rw_cb.tcb.i93.p_update_data = p_data;

    /* read length field */
    rw_cb.tcb.i93.rw_offset = rw_cb.tcb.i93.ndef_tlv_start_offset + 1;
    rw_cb.tcb.i93.rw_length = 0;

    block_number = rw_cb.tcb.i93.rw_offset / rw_cb.tcb.i93.block_size;

    if (rw_i93_send_cmd_read_single_block(block_number, false) ==
        NFC_STATUS_OK) {
      rw_cb.tcb.i93.state = RW_I93_STATE_UPDATE_NDEF;
      rw_cb.tcb.i93.sub_state = RW_I93_SUBSTATE_RESET_LEN;
    } else {
      return NFC_STATUS_FAILED;
    }
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         RW_I93FormatNDef
**
** Description      This function performs formatting procedure
**
**                  RW_I93_FORMAT_CPLT_EVT will be returned
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_I93FormatNDef(void) {
  tNFC_STATUS status;
  tRW_I93_RW_SUBSTATE sub_state;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_FAILED;
  }

  if ((rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY) ||
      (rw_cb.tcb.i93.product_version == RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY)) {
    /* These don't support GetSystemInformation and GetMultiBlockSecurityStatus
     */
    rw_cb.tcb.i93.rw_offset = 0;

    /* read blocks with option flag to get block security status */
    status = rw_i93_send_cmd_read_single_block(0x0000, true);
    sub_state = RW_I93_SUBSTATE_CHECK_READ_ONLY;
  } else {
    status = rw_i93_send_cmd_inventory(rw_cb.tcb.i93.uid, false, 0x00);
    sub_state = RW_I93_SUBSTATE_WAIT_UID;
  }

  if (status == NFC_STATUS_OK) {
    rw_cb.tcb.i93.state = RW_I93_STATE_FORMAT;
    rw_cb.tcb.i93.sub_state = sub_state;
    rw_cb.tcb.i93.intl_flags = 0;
  }

  return (status);
}

/*******************************************************************************
**
** Function         RW_I93SetTagReadOnly
**
** Description      This function performs NDEF read-only procedure
**                  Note: RW_I93DetectNDef () must be called before using this
**                        Updating data must not be removed until returning
**                        event
**
**                  The RW_I93_SET_TAG_RO_EVT event will be returned.
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if I93 is busy or other error
**
*******************************************************************************/
tNFC_STATUS RW_I93SetTagReadOnly(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Unable to start command at state (0x%X)",
                               rw_cb.tcb.i93.state);
    return NFC_STATUS_FAILED;
  }

  if (rw_cb.tcb.i93.tlv_type == I93_ICODE_TLV_TYPE_NDEF) {
    if (rw_cb.tcb.i93.intl_flags & RW_I93_FLAG_READ_ONLY) {
      LOG(ERROR) << StringPrintf("NDEF is already read-only");
      return NFC_STATUS_FAILED;
    }

    /* get CC in the first block */
    if (rw_i93_send_cmd_read_single_block(0, false) == NFC_STATUS_OK) {
      rw_cb.tcb.i93.state = RW_I93_STATE_SET_READ_ONLY;
      rw_cb.tcb.i93.sub_state = RW_I93_SUBSTATE_WAIT_CC;
    } else {
      return NFC_STATUS_FAILED;
    }
  } else {
    LOG(ERROR) << StringPrintf("No NDEF detected");
    return NFC_STATUS_FAILED;
  }

  return NFC_STATUS_OK;
}

/*****************************************************************************
**
** Function         RW_I93PresenceCheck
**
** Description      Check if the tag is still in the field.
**
**                  The RW_I93_PRESENCE_CHECK_EVT w/ status is used to indicate
**                  presence or non-presence.
**
** Returns          NFC_STATUS_OK, if raw data frame sent
**                  NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this
**                  operation
**                  NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_I93PresenceCheck(void) {
  tNFC_STATUS status;
  tRW_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (!rw_cb.p_cback) {
    return NFC_STATUS_FAILED;
  } else if (rw_cb.tcb.i93.state == RW_I93_STATE_NOT_ACTIVATED) {
    evt_data.status = NFC_STATUS_FAILED;
    (*rw_cb.p_cback)(RW_T4T_PRESENCE_CHECK_EVT, &evt_data);

    return NFC_STATUS_OK;
  } else if (rw_cb.tcb.i93.state != RW_I93_STATE_IDLE) {
    return NFC_STATUS_BUSY;
  } else {
    /* The support of AFI by the VICC is optional, so do not include AFI */
    status = rw_i93_send_cmd_inventory(rw_cb.tcb.i93.uid, false, 0x00);

    if (status == NFC_STATUS_OK) {
      /* do not retry during presence check */
      rw_cb.tcb.i93.retry_count = RW_MAX_RETRIES;
      rw_cb.tcb.i93.state = RW_I93_STATE_PRESENCE_CHECK;
    }
  }

  return (status);
}

/*******************************************************************************
**
** Function         rw_i93_get_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_i93_get_state_name(uint8_t state) {
  switch (state) {
    case RW_I93_STATE_NOT_ACTIVATED:
      return "NOT_ACTIVATED";
    case RW_I93_STATE_IDLE:
      return "IDLE";
    case RW_I93_STATE_BUSY:
      return "BUSY";
    case RW_I93_STATE_DETECT_NDEF:
      return "NDEF_DETECTION";
    case RW_I93_STATE_READ_NDEF:
      return "READ_NDEF";
    case RW_I93_STATE_UPDATE_NDEF:
      return "UPDATE_NDEF";
    case RW_I93_STATE_FORMAT:
      return "FORMAT";
    case RW_I93_STATE_SET_READ_ONLY:
      return "SET_READ_ONLY";
    case RW_I93_STATE_PRESENCE_CHECK:
      return "PRESENCE_CHECK";
    default:
      return "???? UNKNOWN STATE";
  }
}

/*******************************************************************************
**
** Function         rw_i93_get_sub_state_name
**
** Description      This function returns the sub_state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_i93_get_sub_state_name(uint8_t sub_state) {
  switch (sub_state) {
    case RW_I93_SUBSTATE_WAIT_UID:
      return "WAIT_UID";
    case RW_I93_SUBSTATE_WAIT_SYS_INFO:
      return "WAIT_SYS_INFO";
    case RW_I93_SUBSTATE_WAIT_CC:
      return "WAIT_CC";
    case RW_I93_SUBSTATE_SEARCH_NDEF_TLV:
      return "SEARCH_NDEF_TLV";
    case RW_I93_SUBSTATE_CHECK_LOCK_STATUS:
      return "CHECK_LOCK_STATUS";
    case RW_I93_SUBSTATE_RESET_LEN:
      return "RESET_LEN";
    case RW_I93_SUBSTATE_WRITE_NDEF:
      return "WRITE_NDEF";
    case RW_I93_SUBSTATE_UPDATE_LEN:
      return "UPDATE_LEN";
    case RW_I93_SUBSTATE_WAIT_RESET_DSFID_AFI:
      return "WAIT_RESET_DSFID_AFI";
    case RW_I93_SUBSTATE_CHECK_READ_ONLY:
      return "CHECK_READ_ONLY";
    case RW_I93_SUBSTATE_WRITE_CC_NDEF_TLV:
      return "WRITE_CC_NDEF_TLV";
    case RW_I93_SUBSTATE_WAIT_UPDATE_CC:
      return "WAIT_UPDATE_CC";
    case RW_I93_SUBSTATE_LOCK_NDEF_TLV:
      return "LOCK_NDEF_TLV";
    case RW_I93_SUBSTATE_WAIT_LOCK_CC:
      return "WAIT_LOCK_CC";
    default:
      return "???? UNKNOWN SUBSTATE";
  }
}

/*******************************************************************************
**
** Function         rw_i93_get_tag_name
**
** Description      This function returns the tag name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_i93_get_tag_name(uint8_t product_version) {
  switch (product_version) {
    case RW_I93_ICODE_SLI:
      return "SLI/SLIX";
    case RW_I93_ICODE_SLI_S:
      return "SLI-S/SLIX-S";
    case RW_I93_ICODE_SLI_L:
      return "SLI-L/SLIX-L";
    case RW_I93_TAG_IT_HF_I_PLUS_INLAY:
      return "Tag-it HF-I Plus Inlay";
    case RW_I93_TAG_IT_HF_I_PLUS_CHIP:
      return "Tag-it HF-I Plus Chip";
    case RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY:
      return "Tag-it HF-I Standard Chip/Inlyas";
    case RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY:
      return "Tag-it HF-I Pro Chip/Inlays";
    case RW_I93_STM_LRI1K:
      return "LRi1K";
    case RW_I93_STM_LRI2K:
      return "LRi2K";
    case RW_I93_STM_LRIS2K:
      return "LRiS2K";
    case RW_I93_STM_LRIS64K:
      return "LRiS64K";
    case RW_I93_STM_M24LR64_R:
      return "M24LR64";
    case RW_I93_STM_M24LR04E_R:
      return "M24LR04E";
    case RW_I93_STM_M24LR16E_R:
      return "M24LR16E";
    case RW_I93_STM_M24LR64E_R:
      return "M24LR64E";
    case RW_I93_STM_ST25DV04K:
      return "ST25DV04";
    case RW_I93_STM_ST25DVHIK:
      return "ST25DV";
    case RW_I93_ONS_N36RW02:
      return ("N36RW02");
    case RW_I93_ONS_N24RF04:
      return ("N24RF04");
    case RW_I93_ONS_N24RF04E:
      return ("N24RF04E");
    case RW_I93_ONS_N24RF16:
      return ("N24RF16");
    case RW_I93_ONS_N24RF16E:
      return ("N24RF16E");
    case RW_I93_ONS_N24RF64:
      return ("N24RF64");
    case RW_I93_ONS_N24RF64E:
      return ("N24RF64E");
    case RW_I93_UNKNOWN_PRODUCT:
    default:
      return "UNKNOWN";
  }
}
