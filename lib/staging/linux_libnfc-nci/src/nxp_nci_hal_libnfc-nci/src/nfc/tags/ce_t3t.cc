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
 *  This file contains the implementation for Type 3 tag in Card Emulation
 *  mode.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <log/log.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "ce_api.h"
#include "ce_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

enum {
  CE_T3T_COMMAND_INVALID,
  CE_T3T_COMMAND_NFC_FORUM,
  CE_T3T_COMMAND_FELICA
};

/* T3T CE states */
enum { CE_T3T_STATE_NOT_ACTIVATED, CE_T3T_STATE_IDLE, CE_T3T_STATE_UPDATING };

/* Bitmasks to indicate type of UPDATE */
#define CE_T3T_UPDATE_FL_NDEF_UPDATE_START 0x01
#define CE_T3T_UPDATE_FL_NDEF_UPDATE_CPLT 0x02
#define CE_T3T_UPDATE_FL_UPDATE 0x04

/*******************************************************************************
* Static constant definitions
*******************************************************************************/
/* Default PMm param */
static const uint8_t CE_DEFAULT_LF_PMM[NCI_T3T_PMM_LEN] = {
    0x01, /* This PAD0 is used to identify HCE-F on Android */
    0xFE, /* This PAD0 is used to identify HCE-F on Android */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*******************************************************************************
**
** Function         ce_t3t_init
**
** Description      Initialize tag-specific fields of ce control block
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_init(void) {
  memcpy(ce_cb.mem.t3t.local_pmm, CE_DEFAULT_LF_PMM, NCI_T3T_PMM_LEN);
  ce_cb.mem.t3t.ndef_info.nbr = CE_T3T_DEFAULT_CHECK_MAXBLOCKS;
  ce_cb.mem.t3t.ndef_info.nbw = CE_T3T_DEFAULT_UPDATE_MAXBLOCKS;
}

/*******************************************************************************
**
** Function         ce_t3t_send_to_lower
**
** Description      Send C-APDU to lower layer
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_send_to_lower(NFC_HDR* p_msg) {
  uint8_t* p;

  /* Set NFC-F SoD field (payload len + 1) */
  p_msg->offset -= 1; /* Point to SoD field */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  UINT8_TO_STREAM(p, (p_msg->len + 1));
  p_msg->len += 1; /* Increment len to include SoD */

  if (NFC_SendData(NFC_RF_CONN_ID, p_msg) != NFC_STATUS_OK) {
    LOG(ERROR) << StringPrintf("failed");
  }
}

/*******************************************************************************
**
** Function         ce_t3t_is_valid_opcode
**
** Description      Valid opcode
**
** Returns          Type of command
**
*******************************************************************************/
uint8_t ce_t3t_is_valid_opcode(uint8_t cmd_id) {
  uint8_t retval = CE_T3T_COMMAND_INVALID;

  if ((cmd_id == T3T_MSG_OPC_CHECK_CMD) || (cmd_id == T3T_MSG_OPC_UPDATE_CMD)) {
    retval = CE_T3T_COMMAND_NFC_FORUM;
  } else if ((cmd_id == T3T_MSG_OPC_POLL_CMD) ||
             (cmd_id == T3T_MSG_OPC_REQ_SERVICE_CMD) ||
             (cmd_id == T3T_MSG_OPC_REQ_RESPONSE_CMD) ||
             (cmd_id == T3T_MSG_OPC_REQ_SYSTEMCODE_CMD)) {
    retval = CE_T3T_COMMAND_FELICA;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         ce_t3t_get_rsp_buf
**
** Description      Get a buffer for sending T3T messages
**
** Returns          NFC_HDR *
**
*****************************************************************************/
NFC_HDR* ce_t3t_get_rsp_buf(void) {
  NFC_HDR* p_cmd_buf;

  p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_CE_POOL_ID);
  if (p_cmd_buf != nullptr) {
    /* Reserve offset for NCI_DATA_HDR and NFC-F Sod (LEN) field */
    p_cmd_buf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + 1;
    p_cmd_buf->len = 0;
  }

  return (p_cmd_buf);
}

/*******************************************************************************
**
** Function         ce_t3t_send_rsp
**
** Description      Send response to reader/writer
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_send_rsp(tCE_CB* p_ce_cb, uint8_t* p_nfcid2, uint8_t opcode,
                     uint8_t status1, uint8_t status2) {
  tCE_T3T_MEM* p_cb = &p_ce_cb->mem.t3t;
  NFC_HDR* p_rsp_msg;
  uint8_t *p_dst, *p_rsp_start;

  /* If p_nfcid2 is NULL, then used activated NFCID2 */
  if (p_nfcid2 == nullptr) {
    p_nfcid2 = p_cb->local_nfcid2;
  }

  p_rsp_msg = ce_t3t_get_rsp_buf();
  if (p_rsp_msg != nullptr) {
    p_dst = p_rsp_start = (uint8_t*)(p_rsp_msg + 1) + p_rsp_msg->offset;

    /* Response Code */
    UINT8_TO_STREAM(p_dst, opcode);

    /* Manufacturer ID */
    ARRAY_TO_STREAM(p_dst, p_nfcid2, NCI_RF_F_UID_LEN);

    /* Status1 and Status2 */
    UINT8_TO_STREAM(p_dst, status1);
    UINT8_TO_STREAM(p_dst, status2);

    p_rsp_msg->len = (uint16_t)(p_dst - p_rsp_start);
    ce_t3t_send_to_lower(p_rsp_msg);
  } else {
    LOG(ERROR) << StringPrintf(
        "CE: Unable to allocat buffer for response message");
  }
}

/*******************************************************************************
**
** Function         ce_t3t_handle_update_cmd
**
** Description      Handle UPDATE command from reader/writer
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_handle_update_cmd(tCE_CB* p_ce_cb, NFC_HDR* p_cmd_msg) {
  tCE_T3T_MEM* p_cb = &p_ce_cb->mem.t3t;
  uint8_t* p_temp;
  uint8_t* p_block_list = p_cb->cur_cmd.p_block_list_start;
  uint8_t* p_block_data = p_cb->cur_cmd.p_block_data_start;
  uint8_t i, j, bl0;
  uint16_t block_number, service_code, checksum, checksum_rx;
  uint32_t newlen_hiword;
  tCE_T3T_NDEF_INFO ndef_info;
  tNFC_STATUS nfc_status = NFC_STATUS_OK;
  uint8_t update_flags = 0;

  /* If in idle state, notify app that update is starting */
  if (p_cb->state == CE_T3T_STATE_IDLE) {
    p_cb->state = CE_T3T_STATE_UPDATING;
  }

  for (i = 0; i < p_cb->cur_cmd.num_blocks; i++) {
    /* Read byte0 of block list */
    STREAM_TO_UINT8(bl0, p_block_list);

    if (bl0 & T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT) {
      STREAM_TO_UINT8(block_number, p_block_list);
    } else {
      STREAM_TO_UINT16(block_number, p_block_list);
    }

    /* Read the block from memory */
    service_code =
        p_cb->cur_cmd.service_code_list[bl0 & T3T_MSG_SERVICE_LIST_MASK];

    /* Reject UPDATE command if service code=T3T_MSG_NDEF_SC_RO */
    if (service_code == T3T_MSG_NDEF_SC_RO) {
      /* Error: invalid block number to update */
      LOG(ERROR) << StringPrintf("CE: UPDATE request using read-only service");
      nfc_status = NFC_STATUS_FAILED;
      break;
    }

    /* Check for NDEF */
    if (service_code == T3T_MSG_NDEF_SC_RW) {
      if (p_cb->cur_cmd.num_blocks > p_cb->ndef_info.nbw) {
        LOG(ERROR) << StringPrintf(
            "CE: Requested too many blocks to update (requested: %i, max: %i)",
            p_cb->cur_cmd.num_blocks, p_cb->ndef_info.nbw);
        nfc_status = NFC_STATUS_FAILED;
        break;
      } else if (p_cb->ndef_info.rwflag == T3T_MSG_NDEF_RWFLAG_RO) {
        LOG(ERROR) << StringPrintf(
            "CE: error: write-request to read-only NDEF message.");
        nfc_status = NFC_STATUS_FAILED;
        break;
      } else if (block_number == 0) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "CE: Update sc 0x%04x block %i.", service_code, block_number);

        /* Special caes: NDEF block0 is the ndef attribute block */
        p_temp = p_block_data;
        STREAM_TO_UINT8(ndef_info.version, p_block_data);
        p_block_data += 8; /* Ignore nbr,nbw,maxb,and reserved (reader/writer
                              not allowed to update this) */
        STREAM_TO_UINT8(ndef_info.writef, p_block_data);
        p_block_data++; /* Ignore rwflag (reader/writer not allowed to update
                           this) */
        STREAM_TO_UINT8(newlen_hiword, p_block_data);
        BE_STREAM_TO_UINT16(ndef_info.ln, p_block_data);
        ndef_info.ln += (newlen_hiword << 16);
        BE_STREAM_TO_UINT16(checksum_rx, p_block_data);

        checksum = 0;
        for (j = 0; j < T3T_MSG_NDEF_ATTR_INFO_SIZE; j++) {
          checksum += p_temp[j];
        }

        /* Compare calcuated checksum with received checksum */
        if (checksum != checksum_rx) {
          LOG(ERROR) << StringPrintf(
              "CE: Checksum failed for NDEF attribute block.");
          nfc_status = NFC_STATUS_FAILED;
        } else {
          /* Update NDEF attribute block (only allowed to update current length
           * and writef fields) */
          p_cb->ndef_info.scratch_ln = ndef_info.ln;
          p_cb->ndef_info.scratch_writef = ndef_info.writef;

          /* If writef=0 indicates completion of NDEF update */
          if (ndef_info.writef == 0) {
            update_flags |= CE_T3T_UPDATE_FL_NDEF_UPDATE_CPLT;
          }
          /* writef=1 indicates start of NDEF update */
          else {
            update_flags |= CE_T3T_UPDATE_FL_NDEF_UPDATE_START;
          }
        }
      } else {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "CE: Udpate sc 0x%04x block %i.", service_code, block_number);

        /* Verify that block_number is within NDEF memory */
        if (block_number > p_cb->ndef_info.nmaxb) {
          /* Error: invalid block number to update */
          LOG(ERROR) << StringPrintf(
              "CE: Requested invalid NDEF block number to update %i (max is "
              "%i).",
              block_number, p_cb->ndef_info.nmaxb);
          nfc_status = NFC_STATUS_FAILED;
          break;
        } else {
          /* Update NDEF memory block */
          STREAM_TO_ARRAY(
              (&p_cb->ndef_info
                    .p_scratch_buf[(block_number - 1) * T3T_MSG_BLOCKSIZE]),
              p_block_data, T3T_MSG_BLOCKSIZE);
        }

        /* Set flag to indicate that this UPDATE contained at least one block */
        update_flags |= CE_T3T_UPDATE_FL_UPDATE;
      }
    } else {
      /* Error: invalid service code */
      LOG(ERROR) << StringPrintf("CE: Requested invalid service code: 0x%04x.",
                                 service_code);
      nfc_status = NFC_STATUS_FAILED;
      break;
    }
  }

  /* Send appropriate response to reader/writer */
  if (nfc_status == NFC_STATUS_OK) {
    ce_t3t_send_rsp(p_ce_cb, nullptr, T3T_MSG_OPC_UPDATE_RSP,
                    T3T_MSG_RSP_STATUS_OK, T3T_MSG_RSP_STATUS_OK);
  } else {
    ce_t3t_send_rsp(p_ce_cb, nullptr, T3T_MSG_OPC_UPDATE_RSP,
                    T3T_MSG_RSP_STATUS_ERROR, T3T_MSG_RSP_STATUS2_ERROR_MEMORY);
    p_cb->state = CE_T3T_STATE_IDLE;
  }

  /* Notify the app of what got updated */
  if (update_flags & CE_T3T_UPDATE_FL_NDEF_UPDATE_START) {
    /* NDEF attribute got updated with WriteF=TRUE */
    p_ce_cb->p_cback(CE_T3T_NDEF_UPDATE_START_EVT, nullptr);
  }

  if (update_flags & CE_T3T_UPDATE_FL_UPDATE) {
    /* UPDATE message contained at least one non-NDEF block */
    p_ce_cb->p_cback(CE_T3T_UPDATE_EVT, nullptr);
  }

  if (update_flags & CE_T3T_UPDATE_FL_NDEF_UPDATE_CPLT) {
    /* NDEF attribute got updated with WriteF=FALSE */
    tCE_DATA ce_data;
    ce_data.update_info.status = nfc_status;
    ce_data.update_info.p_data = p_cb->ndef_info.p_scratch_buf;
    ce_data.update_info.length = p_cb->ndef_info.scratch_ln;
    p_cb->state = CE_T3T_STATE_IDLE;
    p_ce_cb->p_cback(CE_T3T_NDEF_UPDATE_CPLT_EVT, &ce_data);
  }

  GKI_freebuf(p_cmd_msg);
}

/*******************************************************************************
**
** Function         ce_t3t_handle_check_cmd
**
** Description      Handle CHECK command from reader/writer
**
** Returns          Nothing
**
*******************************************************************************/
void ce_t3t_handle_check_cmd(tCE_CB* p_ce_cb, NFC_HDR* p_cmd_msg) {
  tCE_T3T_MEM* p_cb = &p_ce_cb->mem.t3t;
  NFC_HDR* p_rsp_msg;
  uint8_t* p_rsp_start;
  uint8_t *p_dst, *p_temp, *p_status;
  uint8_t* p_src = p_cb->cur_cmd.p_block_list_start;
  uint8_t i, bl0;
  uint8_t ndef_writef;
  uint32_t ndef_len;
  uint16_t block_number, service_code, checksum;

  p_rsp_msg = ce_t3t_get_rsp_buf();
  if (p_rsp_msg != nullptr) {
    p_dst = p_rsp_start = (uint8_t*)(p_rsp_msg + 1) + p_rsp_msg->offset;

    /* Response Code */
    UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_CHECK_RSP);

    /* Manufacturer ID */
    ARRAY_TO_STREAM(p_dst, p_cb->local_nfcid2, NCI_RF_F_UID_LEN);

    /* Save pointer to start of status field */
    p_status = p_dst;

    /* Status1 and Status2 (assume success initially */
    UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS_OK);
    UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS_OK);
    UINT8_TO_STREAM(p_dst, p_cb->cur_cmd.num_blocks);

    for (i = 0; i < p_cb->cur_cmd.num_blocks; i++) {
      /* Read byte0 of block list */
      STREAM_TO_UINT8(bl0, p_src);

      if (bl0 & T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT) {
        STREAM_TO_UINT8(block_number, p_src);
      } else {
        STREAM_TO_UINT16(block_number, p_src);
      }

      /* Read the block from memory */
      service_code =
          p_cb->cur_cmd.service_code_list[bl0 & T3T_MSG_SERVICE_LIST_MASK];

      /* Check for NDEF */
      if ((service_code == T3T_MSG_NDEF_SC_RO) ||
          (service_code == T3T_MSG_NDEF_SC_RW)) {
        /* Verify Nbr (NDEF only) */
        if (p_cb->cur_cmd.num_blocks > p_cb->ndef_info.nbr) {
          /* Error: invalid number of blocks to check */
          LOG(ERROR) << StringPrintf(
              "CE: Requested too many blocks to check (requested: %i, max: %i)",
              p_cb->cur_cmd.num_blocks, p_cb->ndef_info.nbr);

          p_dst = p_status;
          UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS_ERROR);
          UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS2_ERROR_MEMORY);
          break;
        } else if (block_number == 0) {
          /* Special caes: NDEF block0 is the ndef attribute block */
          p_temp = p_dst;

          /* For rw ndef, use scratch buffer's attributes (in case reader/writer
           * had previously updated NDEF) */
          if ((p_cb->ndef_info.rwflag == T3T_MSG_NDEF_RWFLAG_RW) &&
              (p_cb->ndef_info.p_scratch_buf)) {
            ndef_writef = p_cb->ndef_info.scratch_writef;
            ndef_len = p_cb->ndef_info.scratch_ln;
          } else {
            ndef_writef = p_cb->ndef_info.writef;
            ndef_len = p_cb->ndef_info.ln;
          }

          UINT8_TO_STREAM(p_dst, p_cb->ndef_info.version);
          UINT8_TO_STREAM(p_dst, p_cb->ndef_info.nbr);
          UINT8_TO_STREAM(p_dst, p_cb->ndef_info.nbw);
          UINT16_TO_BE_STREAM(p_dst, p_cb->ndef_info.nmaxb);
          UINT32_TO_STREAM(p_dst, 0);
          UINT8_TO_STREAM(p_dst, ndef_writef);
          UINT8_TO_STREAM(p_dst, p_cb->ndef_info.rwflag);
          UINT8_TO_STREAM(p_dst, (ndef_len >> 16 & 0xFF));
          UINT16_TO_BE_STREAM(p_dst, (ndef_len & 0xFFFF));

          checksum = 0;
          for (int j = 0; j < T3T_MSG_NDEF_ATTR_INFO_SIZE; j++) {
            checksum += p_temp[j];
          }
          UINT16_TO_BE_STREAM(p_dst, checksum);
        } else {
          /* Verify that block_number is within NDEF memory */
          if (block_number > p_cb->ndef_info.nmaxb) {
            /* Invalid block number */
            p_dst = p_status;

            LOG(ERROR) << StringPrintf(
                "CE: Requested block number to check %i.", block_number);

            /* Error: invalid number of blocks to check */
            UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS_ERROR);
            UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS2_ERROR_MEMORY);
            break;
          } else {
            /* If card is RW, then read from the scratch buffer (so reader/write
             * can read back what it had just written */
            if ((p_cb->ndef_info.rwflag == T3T_MSG_NDEF_RWFLAG_RW) &&
                (p_cb->ndef_info.p_scratch_buf)) {
              ARRAY_TO_STREAM(
                  p_dst,
                  (&p_cb->ndef_info
                        .p_scratch_buf[(block_number - 1) * T3T_MSG_BLOCKSIZE]),
                  T3T_MSG_BLOCKSIZE);
            } else {
              ARRAY_TO_STREAM(
                  p_dst, (&p_cb->ndef_info
                               .p_buf[(block_number - 1) * T3T_MSG_BLOCKSIZE]),
                  T3T_MSG_BLOCKSIZE);
            }
          }
        }
      } else {
        /* Error: invalid service code */
        LOG(ERROR) << StringPrintf(
            "CE: Requested invalid service code: 0x%04x.", service_code);

        p_dst = p_status;
        UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS_ERROR);
        UINT8_TO_STREAM(p_dst, T3T_MSG_RSP_STATUS2_ERROR_MEMORY);
        break;
      }
    }

    p_rsp_msg->len = (uint16_t)(p_dst - p_rsp_start);
    ce_t3t_send_to_lower(p_rsp_msg);
  } else {
    LOG(ERROR) << StringPrintf(
        "CE: Unable to allocat buffer for response message");
  }

  GKI_freebuf(p_cmd_msg);
}

/*******************************************************************************
**
** Function         ce_t3t_handle_non_nfc_forum_cmd
**
** Description      Handle POLL command from reader/writer
**
** Returns          Nothing
**
*******************************************************************************/
void ce_t3t_handle_non_nfc_forum_cmd(tCE_CB* p_mem_cb, uint8_t cmd_id,
                                     NFC_HDR* p_cmd_msg) {
  tCE_T3T_MEM* p_cb = &p_mem_cb->mem.t3t;
  NFC_HDR* p_rsp_msg;
  uint8_t* p_rsp_start;
  uint8_t* p_dst;
  uint8_t* p = (uint8_t*)(p_cmd_msg + 1) + p_cmd_msg->offset;
  uint16_t sc;
  uint8_t rc;
  bool send_response = true;

  p_rsp_msg = ce_t3t_get_rsp_buf();
  if (p_rsp_msg != nullptr) {
    p_dst = p_rsp_start = (uint8_t*)(p_rsp_msg + 1) + p_rsp_msg->offset;

    switch (cmd_id) {
      case T3T_MSG_OPC_POLL_CMD:
        if (p_cmd_msg->len < 5) {
          LOG(ERROR) << "Received invalid T3t message";
          android_errorWriteLog(0x534e4554, "121150966");
          send_response = false;
          break;
        }
        /* Get system code and RC */
        /* Skip over sod and cmd_id */
        p += 2;
        BE_STREAM_TO_UINT16(sc, p);
        STREAM_TO_UINT8(rc, p);

        /* If requesting wildcard system code, or specifically our system code,
         * then send POLL response */
        if ((sc == 0xFFFF) || (sc == p_cb->system_code)) {
          /* Response Code */
          UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_POLL_RSP);

          /* Manufacturer ID */
          ARRAY_TO_STREAM(p_dst, p_cb->local_nfcid2, NCI_RF_F_UID_LEN);

          /* Manufacturer Parameter PMm */
          ARRAY_TO_STREAM(p_dst, p_cb->local_pmm, NCI_T3T_PMM_LEN);

          /* If requesting system code */
          if (rc == T3T_POLL_RC_SC) {
            UINT16_TO_BE_STREAM(p_dst, p_cb->system_code);
          }
        } else {
          send_response = false;
        }
        break;

      case T3T_MSG_OPC_REQ_RESPONSE_CMD:
        /* Response Code */
        UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_REQ_RESPONSE_RSP);

        /* Manufacturer ID */
        ARRAY_TO_STREAM(p_dst, p_cb->local_nfcid2, NCI_RF_F_UID_LEN);

        /* Mode */
        UINT8_TO_STREAM(p_dst, 0);
        break;

      case T3T_MSG_OPC_REQ_SYSTEMCODE_CMD:
        /* Response Code */
        UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_REQ_SYSTEMCODE_RSP);

        /* Manufacturer ID */
        ARRAY_TO_STREAM(p_dst, p_cb->local_nfcid2, NCI_RF_F_UID_LEN);

        /* Number of system codes */
        UINT8_TO_STREAM(p_dst, 1);

        /* system codes */
        UINT16_TO_BE_STREAM(p_dst, T3T_SYSTEM_CODE_NDEF);
        break;

      case T3T_MSG_OPC_REQ_SERVICE_CMD:
      default:
        /* Unhandled command */
        LOG(ERROR) << StringPrintf("Unhandled CE opcode: %02x", cmd_id);
        send_response = false;
        break;
    }

    if (send_response) {
      p_rsp_msg->len = (uint16_t)(p_dst - p_rsp_start);
      ce_t3t_send_to_lower(p_rsp_msg);
    } else {
      GKI_freebuf(p_rsp_msg);
    }
  } else {
    LOG(ERROR) << StringPrintf(
        "CE: Unable to allocat buffer for response message");
  }
  GKI_freebuf(p_cmd_msg);
}

/*******************************************************************************
**
** Function         ce_t3t_data_cback
**
** Description      This callback function receives the data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_data_cback(tNFC_DATA_CEVT* p_data) {
  tCE_CB* p_ce_cb = &ce_cb;
  tCE_T3T_MEM* p_cb = &p_ce_cb->mem.t3t;
  NFC_HDR* p_msg = p_data->p_data;
  tCE_DATA ce_data;
  uint8_t cmd_id, bl0, entry_len, i;
  uint8_t* p_nfcid2 = nullptr;
  uint8_t* p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  uint8_t cmd_nfcid2[NCI_RF_F_UID_LEN];
  uint16_t block_list_start_offset, remaining;
  bool msg_processed = false;
  bool block_list_ok;
  uint8_t sod;
  uint8_t cmd_type;

  /* If activate system code is not NDEF, or if no local NDEF contents was set,
   * then pass data up to the app */
  if ((p_cb->system_code != T3T_SYSTEM_CODE_NDEF) ||
      (!p_cb->ndef_info.initialized)) {
    ce_data.raw_frame.status = p_data->status;
    ce_data.raw_frame.p_data = p_msg;
    p_ce_cb->p_cback(CE_T3T_RAW_FRAME_EVT, &ce_data);
    return;
  }

  /* Verify that message contains at least Sod and cmd_id */
  if (p_msg->len < 2) {
    LOG(ERROR) << StringPrintf(
        "CE: received invalid T3t message (invalid length: %i)", p_msg->len);
  } else {
    /* Get and validate command opcode */
    STREAM_TO_UINT8(sod, p);
    STREAM_TO_UINT8(cmd_id, p);

    /* Valid command and message length */
    cmd_type = ce_t3t_is_valid_opcode(cmd_id);
    if (cmd_type == CE_T3T_COMMAND_INVALID) {
      LOG(ERROR) << StringPrintf(
          "CE: received invalid T3t message (invalid command: 0x%02X)", cmd_id);
    } else if (cmd_type == CE_T3T_COMMAND_FELICA) {
      ce_t3t_handle_non_nfc_forum_cmd(p_ce_cb, cmd_id, p_msg);
      msg_processed = true;
    } else {
      /* Verify that message contains at least NFCID2 and NUM services */
      if (p_msg->len < T3T_MSG_CMD_COMMON_HDR_LEN) {
        LOG(ERROR) << StringPrintf(
            "CE: received invalid T3t message (invalid length: %i)",
            p_msg->len);
      } else {
        /* Handle NFC_FORUM command (UPDATE or CHECK) */
        STREAM_TO_ARRAY(cmd_nfcid2, p, NCI_RF_F_UID_LEN);
        STREAM_TO_UINT8(p_cb->cur_cmd.num_services, p);

        /* Validate num_services */
        if (p_cb->cur_cmd.num_services > T3T_MSG_SERVICE_LIST_MAX) {
          LOG(ERROR) << StringPrintf(
              "CE: recieved num_services (%i) exceeds maximum (%i)",
              p_cb->cur_cmd.num_services, T3T_MSG_SERVICE_LIST_MAX);
        } else {
          /* Calculate offset of block-list-start */
          block_list_start_offset =
              T3T_MSG_CMD_COMMON_HDR_LEN + 2 * p_cb->cur_cmd.num_services + 1;

          if (p_cb->state == CE_T3T_STATE_NOT_ACTIVATED) {
            LOG(ERROR) << StringPrintf(
                "CE: received command 0x%02X while in bad state (%i))", cmd_id,
                p_cb->state);
          } else if (memcmp(cmd_nfcid2, p_cb->local_nfcid2, NCI_RF_F_UID_LEN) !=
                     0) {
            LOG(ERROR) << StringPrintf(
                "CE: received invalid T3t message (invalid NFCID2)");
            p_nfcid2 =
                cmd_nfcid2; /* respond with ERROR using the NFCID2 from the
                               command message */
          } else if (p_msg->len < block_list_start_offset) {
            /* Does not have minimum (including number_of_blocks field) */
            LOG(ERROR) << StringPrintf("CE: incomplete message");
          } else {
            /* Parse service code list */
            for (i = 0; i < p_cb->cur_cmd.num_services; i++) {
              STREAM_TO_UINT16(p_cb->cur_cmd.service_code_list[i], p);
            }

            /* Verify that block list */
            block_list_ok = true;
            STREAM_TO_UINT8(p_cb->cur_cmd.num_blocks, p);
            remaining = p_msg->len - block_list_start_offset;
            p_cb->cur_cmd.p_block_list_start = p;
            for (i = 0; i < p_cb->cur_cmd.num_blocks; i++) {
              /* Each entry is at lease 2 bytes long */
              if (remaining < 2) {
                /* Unexpected end of message (while reading block-list) */
                LOG(ERROR) << StringPrintf(
                    "CE: received invalid T3t message (unexpected end of "
                    "block-list)");
                block_list_ok = false;
                break;
              }

              /* Get byte0 of block-list entry */
              bl0 = *p;

              /* Validate service code index and size of block-list */
              if ((bl0 & T3T_MSG_SERVICE_LIST_MASK) >=
                  p_cb->cur_cmd.num_services) {
                /* Invalid service code */
                LOG(ERROR) << StringPrintf(
                    "CE: received invalid T3t message (invalid service index: "
                    "%i)",
                    (bl0 & T3T_MSG_SERVICE_LIST_MASK));
                block_list_ok = false;
                break;
              } else if ((!(bl0 & T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT)) &&
                         (remaining < 3)) {
                /* Unexpected end of message (while reading 3-byte entry) */
                LOG(ERROR) << StringPrintf(
                    "CE: received invalid T3t message (unexpected end of "
                    "block-list)");
                block_list_ok = false;
                break;
              }

              /* Advance pointers to next block-list entry */
              entry_len =
                  (bl0 & T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT) ? 2 : 3;
              p += entry_len;
              remaining -= entry_len;
            }

            /* Block list is verified. Call CHECK or UPDATE handler */
            if (block_list_ok) {
              p_cb->cur_cmd.p_block_data_start = p;
              if (cmd_id == T3T_MSG_OPC_CHECK_CMD) {
                /* This is a CHECK command. Sanity check: there shouldn't be any
                 * more data remaining after reading block list */
                if (remaining) {
                  LOG(ERROR) << StringPrintf(
                      "CE: unexpected data after after CHECK command (%u "
                      "bytes)",
                      (unsigned int)remaining);
                }
                ce_t3t_handle_check_cmd(p_ce_cb, p_msg);
                msg_processed = true;
              } else {
                /* This is an UPDATE command. See if message contains all the
                 * expected block data */
                if (remaining < p_cb->cur_cmd.num_blocks * T3T_MSG_BLOCKSIZE) {
                  LOG(ERROR)
                      << StringPrintf("CE: unexpected end of block-data");
                } else {
                  ce_t3t_handle_update_cmd(p_ce_cb, p_msg);
                  msg_processed = true;
                }
              }
            }
          }
        }
      }
    }
  }

  if (!msg_processed) {
    ce_t3t_send_rsp(p_ce_cb, p_nfcid2, T3T_MSG_OPC_CHECK_RSP,
                    T3T_MSG_RSP_STATUS_ERROR,
                    T3T_MSG_RSP_STATUS2_ERROR_PROCESSING);
    GKI_freebuf(p_msg);
  }
}

/*******************************************************************************
**
** Function         ce_t3t_conn_cback
**
** Description      This callback function receives the events/data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void ce_t3t_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                       tNFC_CONN* p_data) {
  tCE_T3T_MEM* p_cb = &ce_cb.mem.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("ce_t3t_conn_cback: conn_id=%i, evt=%i", conn_id, event);

  switch (event) {
    case NFC_CONN_CREATE_CEVT:
      break;

    case NFC_CONN_CLOSE_CEVT:
      p_cb->state = CE_T3T_STATE_NOT_ACTIVATED;
      break;

    case NFC_DATA_CEVT:
      if (p_data->data.status == NFC_STATUS_OK) {
        ce_t3t_data_cback(&p_data->data);
      }
      break;

    case NFC_DEACTIVATE_CEVT:
      p_cb->state = CE_T3T_STATE_NOT_ACTIVATED;
      NFC_SetStaticRfCback(nullptr);
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         ce_select_t3t
**
** Description      Select Type 3 Tag
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS ce_select_t3t(uint16_t system_code,
                          uint8_t nfcid2[NCI_RF_F_UID_LEN]) {
  tCE_T3T_MEM* p_cb = &ce_cb.mem.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cb->state = CE_T3T_STATE_IDLE;
  p_cb->system_code = system_code;
  memcpy(p_cb->local_nfcid2, nfcid2, NCI_RF_F_UID_LEN);

  NFC_SetStaticRfCback(ce_t3t_conn_cback);
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         CE_T3tSetLocalNDEFMsg
**
** Description      Initialise CE Type 3 Tag with mandatory NDEF message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS CE_T3tSetLocalNDEFMsg(bool read_only, uint32_t size_max,
                                  uint32_t size_current, uint8_t* p_buf,
                                  uint8_t* p_scratch_buf) {
  tCE_T3T_MEM* p_cb = &ce_cb.mem.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("CE_T3tSetContent: ro=%i, size_max=%i, size_current=%i",
                      read_only, size_max, size_current);

  /* Verify scratch buffer was provided if NDEF message is read/write */
  if ((!read_only) && (!p_scratch_buf)) {
    LOG(ERROR) << StringPrintf(
        "p_scratch_buf cannot be NULL if not "
        "read-only");
    return NFC_STATUS_FAILED;
  }

  /* Check if disabling the local NDEF */
  if (!p_buf) {
    p_cb->ndef_info.initialized = false;
  }
  /* Save ndef attributes */
  else {
    p_cb->ndef_info.initialized = true;
    p_cb->ndef_info.ln = size_current; /* Current length */
    p_cb->ndef_info.nmaxb = (uint16_t)(
        (size_max + 15) / T3T_MSG_BLOCKSIZE); /* Max length (in blocks) */
    p_cb->ndef_info.rwflag =
        (read_only) ? T3T_MSG_NDEF_RWFLAG_RO : T3T_MSG_NDEF_RWFLAG_RW;
    p_cb->ndef_info.writef = T3T_MSG_NDEF_WRITEF_OFF;
    p_cb->ndef_info.version = 0x10;
    p_cb->ndef_info.p_buf = p_buf;
    p_cb->ndef_info.p_scratch_buf = p_scratch_buf;

    /* Initiate scratch buffer with same contents as read-buffer */
    if (p_scratch_buf) {
      p_cb->ndef_info.scratch_ln = p_cb->ndef_info.ln;
      p_cb->ndef_info.scratch_writef = T3T_MSG_NDEF_WRITEF_OFF;
      memcpy(p_scratch_buf, p_buf, p_cb->ndef_info.ln);
    }
  }

  return (NFC_STATUS_OK);
}

/*******************************************************************************
**
** Function         CE_T3tSetLocalNDefParams
**
** Description      Sets T3T-specific NDEF parameters. (Optional - if not
**                  called, then CE will use default parameters)
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS CE_T3tSetLocalNDefParams(uint8_t nbr, uint8_t nbw) {
  tCE_T3T_MEM* p_cb = &ce_cb.mem.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("CE_T3tSetLocalNDefParams: nbr=%i, nbw=%i", nbr, nbw);

  /* Validate */
  if ((nbr > T3T_MSG_NUM_BLOCKS_CHECK_MAX) ||
      (nbw > T3T_MSG_NUM_BLOCKS_UPDATE_MAX) || (nbr < 1) || (nbw < 1)) {
    LOG(ERROR) << StringPrintf("CE_T3tSetLocalNDefParams: invalid params");
    return NFC_STATUS_FAILED;
  }

  p_cb->ndef_info.nbr = nbr;
  p_cb->ndef_info.nbw = nbw;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         CE_T3tSendCheckRsp
**
** Description      Send CHECK response message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS CE_T3tSendCheckRsp(uint8_t status1, uint8_t status2,
                               uint8_t num_blocks, uint8_t* p_block_data) {
  tCE_T3T_MEM* p_cb = &ce_cb.mem.t3t;
  tNFC_STATUS retval = NFC_STATUS_OK;
  NFC_HDR* p_rsp_msg;
  uint8_t *p_dst, *p_rsp_start;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "CE_T3tCheckRsp: status1=0x%02X, status2=0x%02X, num_blocks=%i", status1,
      status2, num_blocks);

  /* Validate num_blocks */
  if (num_blocks > T3T_MSG_NUM_BLOCKS_CHECK_MAX) {
    LOG(ERROR) << StringPrintf(
        "CE_T3tCheckRsp num_blocks (%i) exceeds maximum (%i)", num_blocks,
        T3T_MSG_NUM_BLOCKS_CHECK_MAX);
    return (NFC_STATUS_FAILED);
  }

  p_rsp_msg = ce_t3t_get_rsp_buf();
  if (p_rsp_msg != nullptr) {
    p_dst = p_rsp_start = (uint8_t*)(p_rsp_msg + 1) + p_rsp_msg->offset;

    /* Response Code */
    UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_CHECK_RSP);

    /* Manufacturer ID */
    ARRAY_TO_STREAM(p_dst, p_cb->local_nfcid2, NCI_RF_F_UID_LEN);

    /* Status1 and Status2 */
    UINT8_TO_STREAM(p_dst, status1);
    UINT8_TO_STREAM(p_dst, status2);

    if (status1 == T3T_MSG_RSP_STATUS_OK) {
      UINT8_TO_STREAM(p_dst, num_blocks);
      ARRAY_TO_STREAM(p_dst, p_block_data, (num_blocks * T3T_MSG_BLOCKSIZE));
    }

    p_rsp_msg->len = (uint16_t)(p_dst - p_rsp_start);
    ce_t3t_send_to_lower(p_rsp_msg);
  } else {
    LOG(ERROR) << StringPrintf(
        "CE: Unable to allocate buffer for response message");
  }

  return (retval);
}

/*******************************************************************************
**
** Function         CE_T3tSendUpdateRsp
**
** Description      Send UPDATE response message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS CE_T3tSendUpdateRsp(uint8_t status1, uint8_t status2) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tCE_CB* p_ce_cb = &ce_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "CE_T3tUpdateRsp: status1=0x%02X, status2=0x%02X", status1, status2);
  ce_t3t_send_rsp(p_ce_cb, nullptr, T3T_MSG_OPC_UPDATE_RSP, status1, status2);

  return (retval);
}
