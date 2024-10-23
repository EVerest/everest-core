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
 *  This file contains the implementation for Type 2 tag NDEF operation in
 *  Reader/Writer mode.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <log/log.h>

#include "nfc_target.h"

#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "rw_api.h"
#include "rw_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

#if (RW_NDEF_INCLUDED == TRUE)

/* Local static functions */
static void rw_t2t_handle_cc_read_rsp(void);
static void rw_t2t_handle_lock_read_rsp(uint8_t* p_data);
static void rw_t2t_handle_tlv_detect_rsp(uint8_t* p_data);
static void rw_t2t_handle_ndef_read_rsp(uint8_t* p_data);
static void rw_t2t_handle_ndef_write_rsp(uint8_t* p_data);
static void rw_t2t_handle_format_tag_rsp(uint8_t* p_data);
static void rw_t2t_handle_config_tag_readonly(uint8_t* p_data);
static uint8_t rw_t2t_get_tag_size(uint8_t* p_data);
static void rw_t2t_extract_default_locks_info(void);
static void rw_t2t_update_cb(uint16_t block, uint8_t* p_write_block,
                             bool b_update_len);
static uint8_t rw_t2t_get_ndef_flags(void);
static uint16_t rw_t2t_get_ndef_max_size(void);
static tNFC_STATUS rw_t2t_read_locks(void);
static tNFC_STATUS rw_t2t_read_ndef_last_block(void);
static void rw_t2t_update_attributes(void);
static void rw_t2t_update_lock_attributes(void);
static bool rw_t2t_is_lock_res_byte(uint16_t index);
static bool rw_t2t_is_read_only_byte(uint16_t index);
static tNFC_STATUS rw_t2t_write_ndef_first_block(uint16_t msg_len,
                                                 bool b_update_len);
static tNFC_STATUS rw_t2t_write_ndef_next_block(uint16_t block,
                                                uint16_t msg_len,
                                                bool b_update_len);
static tNFC_STATUS rw_t2t_read_ndef_next_block(uint16_t block);
static tNFC_STATUS rw_t2t_add_terminator_tlv(void);
static bool rw_t2t_is_read_before_write_block(uint16_t block,
                                              uint16_t* p_block_to_read);
static tNFC_STATUS rw_t2t_set_cc(uint8_t tms);
static tNFC_STATUS rw_t2t_set_lock_tlv(uint16_t addr, uint8_t num_dyn_lock_bits,
                                       uint16_t locked_area_size);
static tNFC_STATUS rw_t2t_format_tag(void);
static tNFC_STATUS rw_t2t_soft_lock_tag(void);
static tNFC_STATUS rw_t2t_set_dynamic_lock_bits(uint8_t* p_data);
static void rw_t2t_ntf_tlv_detect_complete(tNFC_STATUS status);

const uint8_t rw_t2t_mask_bits[8] = {0x01, 0x02, 0x04, 0x08,
                                     0x10, 0x20, 0x40, 0x80};

/*******************************************************************************
**
** Function         rw_t2t_handle_rsp
**
** Description      This function handles response to command sent during
**                  NDEF and other tlv operation
**
** Returns          None
**
*******************************************************************************/
void rw_t2t_handle_rsp(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_CC) {
    p_t2t->b_read_hdr = true;
    memcpy(p_t2t->tag_hdr, p_data, T2T_READ_DATA_LEN);
  }

  switch (p_t2t->state) {
    case RW_T2T_STATE_DETECT_TLV:
      if (p_t2t->tlv_detect == TAG_LOCK_CTRL_TLV) {
        if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_CC) {
          rw_t2t_handle_cc_read_rsp();
        } else if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_LOCKS) {
          rw_t2t_handle_lock_read_rsp(p_data);
        } else {
          rw_t2t_handle_tlv_detect_rsp(p_data);
        }
      } else if (p_t2t->tlv_detect == TAG_NDEF_TLV) {
        if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_CC) {
          if (p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] == T2T_CC0_NMN) {
            rw_t2t_handle_cc_read_rsp();
          } else {
            LOG(WARNING) << StringPrintf(
                "NDEF Detection failed!, CC[0]: 0x%02x, CC[1]: 0x%02x, CC[3]: "
                "0x%02x",
                p_t2t->tag_hdr[T2T_CC0_NMN_BYTE],
                p_t2t->tag_hdr[T2T_CC1_VNO_BYTE],
                p_t2t->tag_hdr[T2T_CC3_RWA_BYTE]);
            rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
          }
        } else if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_LOCKS) {
          rw_t2t_handle_lock_read_rsp(p_data);
        } else {
          rw_t2t_handle_tlv_detect_rsp(p_data);
        }
      } else {
        if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_CC) {
          rw_t2t_handle_cc_read_rsp();
        } else {
          rw_t2t_handle_tlv_detect_rsp(p_data);
        }
      }
      break;

    case RW_T2T_STATE_SET_TAG_RO:
      rw_t2t_handle_config_tag_readonly(p_data);
      break;

    case RW_T2T_STATE_FORMAT_TAG:
      rw_t2t_handle_format_tag_rsp(p_data);
      break;

    case RW_T2T_STATE_READ_NDEF:
      rw_t2t_handle_ndef_read_rsp(p_data);
      break;

    case RW_T2T_STATE_WRITE_NDEF:
      rw_t2t_handle_ndef_write_rsp(p_data);
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t2t_info_to_event
**
** Description      This function returns RW event code based on the current
**                  state
**
** Returns          RW event code
**
*******************************************************************************/
tRW_EVENT rw_t2t_info_to_event(const tT2T_CMD_RSP_INFO* p_info) {
  tRW_EVENT rw_event;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  switch (p_t2t->state) {
    case RW_T2T_STATE_DETECT_TLV:
      if (p_t2t->tlv_detect == TAG_NDEF_TLV)
        rw_event = RW_T2T_NDEF_DETECT_EVT;
      else
        rw_event = RW_T2T_TLV_DETECT_EVT;

      break;

    case RW_T2T_STATE_READ_NDEF:
      rw_event = RW_T2T_NDEF_READ_EVT;
      break;

    case RW_T2T_STATE_WRITE_NDEF:
      rw_event = RW_T2T_NDEF_WRITE_EVT;
      break;

    case RW_T2T_STATE_SET_TAG_RO:
      rw_event = RW_T2T_SET_TAG_RO_EVT;
      break;

    case RW_T2T_STATE_CHECK_PRESENCE:
      rw_event = RW_T2T_PRESENCE_CHECK_EVT;
      break;

    case RW_T2T_STATE_FORMAT_TAG:
      rw_event = RW_T2T_FORMAT_CPLT_EVT;
      break;

    default:
      rw_event = t2t_info_to_evt(p_info);
      break;
  }
  return rw_event;
}

/*******************************************************************************
**
** Function         rw_t2t_handle_cc_read_rsp
**
** Description      Handle read cc bytes
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_cc_read_rsp(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  if (((p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] != T2T_CC3_RWA_RW) &&
       (p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] != T2T_CC3_RWA_RO)) ||
      ((p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_LEGACY_VNO) &&
       (p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_VNO) &&
       (p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_NEW_VNO))) {
    /* Invalid Version number or RWA byte */
    rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
    return;
  }

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;

  if (rw_t2t_read((uint16_t)T2T_FIRST_DATA_BLOCK) != NFC_STATUS_OK) {
    rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_ntf_tlv_detect_complete
**
** Description      Notify TLV detection complete to upper layer
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_ntf_tlv_detect_complete(tNFC_STATUS status) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t xx;

  if (p_t2t->tlv_detect == TAG_NDEF_TLV) {
    /* Notify upper layer the result of NDEF detect op */
    tRW_DETECT_NDEF_DATA ndef_data = {};
    ndef_data.status = status;
    ndef_data.protocol = NFC_PROTOCOL_T2T;
    ndef_data.flags = rw_t2t_get_ndef_flags();
    ndef_data.cur_size = p_t2t->ndef_msg_len;

    if (status == NFC_STATUS_OK) ndef_data.flags |= RW_NDEF_FL_FORMATED;

    if (p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] == T2T_CC3_RWA_RW)
      ndef_data.max_size = (uint32_t)rw_t2t_get_ndef_max_size();
    else
      ndef_data.max_size = ndef_data.cur_size;

    if (ndef_data.max_size < ndef_data.cur_size) {
      ndef_data.flags |= RW_NDEF_FL_READ_ONLY;
      ndef_data.max_size = ndef_data.cur_size;
    }

    if (!(ndef_data.flags & RW_NDEF_FL_READ_ONLY)) {
      ndef_data.flags |= RW_NDEF_FL_SOFT_LOCKABLE;
      if (status == NFC_STATUS_OK) ndef_data.flags |= RW_NDEF_FL_HARD_LOCKABLE;
    }

    rw_t2t_handle_op_complete();
    tRW_DATA rw_data;
    rw_data.ndef = ndef_data;
    (*rw_cb.p_cback)(RW_T2T_NDEF_DETECT_EVT, &rw_data);
  } else if (p_t2t->tlv_detect == TAG_PROPRIETARY_TLV) {
    tRW_T2T_DETECT evt_data;
    evt_data.msg_len = p_t2t->prop_msg_len;
    evt_data.status = status;
    rw_t2t_handle_op_complete();
    /* FIXME: Unsafe cast */
    (*rw_cb.p_cback)(RW_T2T_TLV_DETECT_EVT, (tRW_DATA*)&evt_data);
  } else {
    /* Notify upper layer the result of Lock/Mem TLV detect op */
    tRW_DETECT_TLV_DATA tlv_data;
    tlv_data.protocol = NFC_PROTOCOL_T2T;
    if (p_t2t->tlv_detect == TAG_LOCK_CTRL_TLV) {
      tlv_data.num_bytes = p_t2t->num_lockbytes;
    } else {
      tlv_data.num_bytes = 0;
      for (xx = 0; xx < p_t2t->num_mem_tlvs; xx++) {
        tlv_data.num_bytes += p_t2t->mem_tlv[p_t2t->num_mem_tlvs].num_bytes;
      }
    }
    tlv_data.status = status;
    rw_t2t_handle_op_complete();
    tRW_DATA rw_data;
    rw_data.tlv = tlv_data;
    (*rw_cb.p_cback)(RW_T2T_TLV_DETECT_EVT, &rw_data);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_handle_lock_read_rsp
**
** Description      Handle response to reading lock bytes
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_lock_read_rsp(uint8_t* p_data) {
  uint8_t updated_lock_byte;
  uint8_t num_locks;
  uint8_t offset = 0;
  uint16_t lock_offset;
  uint16_t base_lock_offset = 0;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t block;

  /* Prepare NDEF/TLV attributes (based on current op) for sending response to
   * upper layer */

  num_locks = 0;
  updated_lock_byte = 0;

  /*  Extract all lock bytes present in the read 16 bytes
   *  but atleast one lock byte (base lock) should be present in the read 16
   * bytes */

  while (num_locks < p_t2t->num_lockbytes) {
    if (p_t2t->lockbyte[num_locks].b_lock_read == false) {
      lock_offset =
          p_t2t->lock_tlv[p_t2t->lockbyte[num_locks].tlv_index].offset +
          p_t2t->lockbyte[num_locks].byte_index;
      if (updated_lock_byte == 0) {
        /* The offset of the first lock byte present in the 16 bytes read using
         * READ command */
        base_lock_offset = lock_offset;
        /* Block number used to read may not be the block where lock offset is
         * present */
        offset = (uint8_t)(lock_offset - (p_t2t->block_read * T2T_BLOCK_SIZE));
        /* Update the lock byte value in the control block */
        p_t2t->lockbyte[num_locks].lock_byte = p_data[offset];
        p_t2t->lockbyte[num_locks].b_lock_read = true;
        updated_lock_byte++;
      } else if (lock_offset > base_lock_offset) {
        /* Atleast one lock byte will get updated in the control block */
        if ((lock_offset - base_lock_offset + offset) < T2T_READ_DATA_LEN) {
          /* And this lock byte is also present in the read data */
          p_t2t->lockbyte[num_locks].lock_byte =
              p_data[lock_offset - base_lock_offset + offset];
          p_t2t->lockbyte[num_locks].b_lock_read = true;
          updated_lock_byte++;
        } else {
          /* This lock byte is not present in the read data */
          block = (uint16_t)(lock_offset / T2T_BLOCK_LEN);
          block -= block % T2T_READ_BLOCKS;
          /* send READ command to read this lock byte */
          if (NFC_STATUS_OK != rw_t2t_read((uint16_t)block)) {
            /* Unable to send Read command, notify failure status to upper layer
             */
            rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
          }
          break;
        }
      } else {
        /* This Lock byte is not present in the read 16 bytes
         * send READ command to read the lock byte       */
        if (NFC_STATUS_OK !=
            rw_t2t_read((uint16_t)(lock_offset / T2T_BLOCK_LEN))) {
          /* Unable to send Read command, notify failure status to upper layer
           */
          rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
        }
        break;
      }
    }
    num_locks++;
  }
  if (num_locks == p_t2t->num_lockbytes) {
    /* All locks are read, notify upper layer */
    rw_t2t_update_lock_attributes();
    rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_OK);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_handle_tlv_detect_rsp
**
** Description      Handle TLV detection.
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_tlv_detect_rsp(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t offset;
  uint16_t len = 0;
  bool failed = false;
  bool found = false;
  tRW_EVENT event;
  uint8_t index;
  uint8_t count = 0;
  uint8_t xx;
  tNFC_STATUS status;
  tT2T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT2T_CMD_RSP_INFO*)rw_cb.tcb.t2t.p_cmd_rsp_info;
  uint8_t tlvtype = p_t2t->tlv_detect;

  if (p_t2t->work_offset == 0) {
    /* Skip UID,Static Lock block,CC*/
    p_t2t->work_offset = T2T_FIRST_DATA_BLOCK * T2T_BLOCK_LEN;
    p_t2t->b_read_data = true;
    memcpy(p_t2t->tag_data, p_data, T2T_READ_DATA_LEN);
  }

  p_t2t->segment = 0;

  for (offset = 0; offset < T2T_READ_DATA_LEN && !failed && !found;) {
    if (rw_t2t_is_lock_res_byte((uint16_t)(p_t2t->work_offset + offset)) ==
        true) {
      /* Skip locks, reserved bytes while searching for TLV */
      offset++;
      continue;
    }
    switch (p_t2t->substate) {
      case RW_T2T_SUBSTATE_WAIT_TLV_DETECT:
        /* Search for the tlv */
        p_t2t->found_tlv = p_data[offset++];
        switch (p_t2t->found_tlv) {
          case TAG_NULL_TLV: /* May be used for padding. SHALL ignore this */
            break;

          case TAG_NDEF_TLV:
            if (tlvtype == TAG_NDEF_TLV) {
              /* NDEF Detected, now collect NDEF Attributes including NDEF
               * Length */
              index = (offset % T2T_BLOCK_SIZE);
              /* Backup ndef first block */
              memcpy(p_t2t->ndef_first_block, &p_data[offset - index], index);
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN;
            } else if (tlvtype == TAG_PROPRIETARY_TLV) {
              /* Proprietary TLV can exist after NDEF Tlv so we continue
               * searching */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN;
            } else if (((tlvtype == TAG_LOCK_CTRL_TLV) &&
                        (p_t2t->num_lockbytes > 0)) ||
                       ((tlvtype == TAG_MEM_CTRL_TLV) &&
                        (p_t2t->num_mem_tlvs > 0))) {
              /* Lock / Memory control tlv cannot exist after NDEF TLV
               * So when NDEF is found, we stop searching for Lock and Memory
               * control tlv */
              found = true;
            } else {
              /* While searching for Lock / Memory control tlv, if NDEF TLV is
               * found
               * first then our search for Lock /Memory control tlv failed and
               * we stop here */
              failed = true;
            }
            break;

          case TAG_LOCK_CTRL_TLV:
          case TAG_MEM_CTRL_TLV:
            p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0;
            break;

          case TAG_PROPRIETARY_TLV:
            if (tlvtype == TAG_PROPRIETARY_TLV) {
              index = (offset % T2T_BLOCK_SIZE);
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN;
            } else {
              /* NDEF/LOCK/MEM TLV can exist after Proprietary Tlv so we
               * continue searching, skiping proprietary tlv */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN;
            }
            break;

          case TAG_TERMINATOR_TLV: /* Last TLV block in the data area. Must be
                                      no NDEF nessage */
            if (((tlvtype == TAG_LOCK_CTRL_TLV) &&
                 (p_t2t->num_lockbytes > 0)) ||
                ((tlvtype == TAG_MEM_CTRL_TLV) && (p_t2t->num_mem_tlvs > 0))) {
              /* No more Lock/Memory TLV control tlv in the tag, so stop
               * searching */
              found = true;
            } else {
              /* NDEF/Lock/Memory/Proprietary TLV cannot exist after Terminator
               * Tlv */
              failed = true;
            }
            break;
          default:
            failed = true;
        }
        break;

      case RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN:
        len = p_data[offset];
        switch (p_t2t->found_tlv) {
          case TAG_NDEF_TLV:
            p_t2t->ndef_header_offset = offset + p_t2t->work_offset;
            if (len == TAG_LONG_NDEF_LEN_FIELD_BYTE0) {
              /* The next two bytes constitute length bytes */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0;
            } else {
              /* one byte length field */
              p_t2t->ndef_msg_len = len;
              p_t2t->bytes_count = p_t2t->ndef_msg_len;
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE;
            }
            break;

          case TAG_PROPRIETARY_TLV:
            if (len == T2T_LONG_NDEF_LEN_FIELD_BYTE0) {
              /* The next two bytes constitute length bytes */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0;
            } else {
              /* one byte length field */
              p_t2t->prop_msg_len = len;
              p_t2t->bytes_count = p_t2t->prop_msg_len;
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE;
            }
            break;
        }
        offset++;
        break;

      case RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0:
        switch (p_t2t->found_tlv) {
          case TAG_LOCK_CTRL_TLV:
          case TAG_MEM_CTRL_TLV:

            len = p_data[offset];
            if (len == TAG_DEFAULT_TLV_LEN) {
              /* Valid Lock control TLV */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE;
              p_t2t->bytes_count = TAG_DEFAULT_TLV_LEN;
            } else if (((tlvtype == TAG_LOCK_CTRL_TLV) &&
                        (p_t2t->num_lockbytes > 0)) ||
                       ((tlvtype == TAG_MEM_CTRL_TLV) &&
                        (p_t2t->num_mem_tlvs > 0))) {
              /* Stop searching for Lock/ Memory control tlv */
              found = true;
            } else {
              failed = true;
            }
            break;

          case TAG_NDEF_TLV:
          case TAG_PROPRIETARY_TLV:
            /* The first length byte */
            p_t2t->bytes_count = (uint8_t)p_data[offset];
            p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN1;
            break;
        }
        offset++;
        break;

      case RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN1:
        /* Prepare NDEF Message length */
        p_t2t->bytes_count = (p_t2t->bytes_count << 8) + p_data[offset];
        if (p_t2t->found_tlv == TAG_NDEF_TLV) {
          p_t2t->ndef_msg_len = p_t2t->bytes_count;
        } else if (p_t2t->found_tlv == TAG_PROPRIETARY_TLV) {
          p_t2t->prop_msg_len = p_t2t->bytes_count;
        }
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE;
        offset++;
        break;

      case RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE:
        switch (p_t2t->found_tlv) {
          case TAG_NDEF_TLV:
            if ((p_t2t->bytes_count == p_t2t->ndef_msg_len) &&
                (tlvtype == TAG_NDEF_TLV)) {
              /* The first byte offset after length field */
              p_t2t->ndef_msg_offset = offset + p_t2t->work_offset;
            }
            /* Reduce number of NDEF bytes remaining to pass over NDEF TLV */
            if (p_t2t->bytes_count > 0) p_t2t->bytes_count--;

            if (tlvtype == TAG_NDEF_TLV) {
              found = true;
              p_t2t->ndef_status = T2T_NDEF_DETECTED;
            } else if (p_t2t->bytes_count == 0) {
              /* Next byte could be a different TLV */
              p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
            }
            break;

          case TAG_LOCK_CTRL_TLV:
            if (p_t2t->bytes_count > 0) {
              p_t2t->bytes_count--;
            } else {
              LOG(ERROR) << StringPrintf("Underflow p_t2t->bytes_count!");
              android_errorWriteLog(0x534e4554, "120506143");
            }
            if ((tlvtype == TAG_LOCK_CTRL_TLV) || (tlvtype == TAG_NDEF_TLV)) {
              if (p_t2t->num_lockbytes > 0) {
                LOG(ERROR) << StringPrintf("Malformed tag!");
                android_errorWriteLog(0x534e4554, "147309942");
                failed = true;
                break;
              }
              /* Collect Lock TLV */
              p_t2t->tlv_value[2 - p_t2t->bytes_count] = p_data[offset];
              if (p_t2t->bytes_count == 0) {
                /* Lock TLV is collected and buffered in tlv_value, now decode
                 * it */
                p_t2t->lock_tlv[p_t2t->num_lock_tlvs].offset =
                    (p_t2t->tlv_value[0] >> 4) & 0x0F;
                p_t2t->lock_tlv[p_t2t->num_lock_tlvs].offset *=
                    (uint8_t)tags_pow(2, p_t2t->tlv_value[2] & 0x0F);
                p_t2t->lock_tlv[p_t2t->num_lock_tlvs].offset +=
                    p_t2t->tlv_value[0] & 0x0F;
                p_t2t->lock_tlv[p_t2t->num_lock_tlvs].bytes_locked_per_bit =
                    (uint8_t)tags_pow(2, ((p_t2t->tlv_value[2] & 0xF0) >> 4));
                /* Note: 0 value in DLA_NbrLockBits means 256 */
                count = p_t2t->tlv_value[1];
                /* Set it to max value that can be stored in lockbytes */
                if (count == 0) {
#if RW_T2T_MAX_LOCK_BYTES > 0x1F
                  count = UCHAR_MAX;
#else
                  count = RW_T2T_MAX_LOCK_BYTES * TAG_BITS_PER_BYTE;
#endif
                }
                p_t2t->lock_tlv[p_t2t->num_lock_tlvs].num_bits = count;
                count = count / TAG_BITS_PER_BYTE +
                        ((count % TAG_BITS_PER_BYTE != 0) ? 1 : 0);

                /* Extract lockbytes info addressed by this Lock TLV */
                xx = 0;
                if (count > RW_T2T_MAX_LOCK_BYTES) {
                  count = RW_T2T_MAX_LOCK_BYTES;
                  android_errorWriteLog(0x534e4554, "112161557");
                }
                while (xx < count) {
                  p_t2t->lockbyte[p_t2t->num_lockbytes].tlv_index =
                      p_t2t->num_lock_tlvs;
                  p_t2t->lockbyte[p_t2t->num_lockbytes].byte_index = xx;
                  p_t2t->lockbyte[p_t2t->num_lockbytes].b_lock_read = false;
                  xx++;
                  p_t2t->num_lockbytes++;
                }
                p_t2t->num_lock_tlvs++;
                rw_t2t_update_attributes();
                /* Next byte could be a different TLV */
                p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
              }
            } else {
              /* If not looking for lock/ndef tlv, just skip this Lock TLV */
              if (p_t2t->bytes_count == 0) {
                p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
              }
            }
            break;

          case TAG_MEM_CTRL_TLV:
            if (p_t2t->bytes_count > 0) {
              p_t2t->bytes_count--;
            } else {
              LOG(ERROR) << StringPrintf("bytes_count underflow!");
              android_errorWriteLog(0x534e4554, "120506143");
            }
            if ((tlvtype == TAG_MEM_CTRL_TLV) || (tlvtype == TAG_NDEF_TLV)) {
              p_t2t->tlv_value[2 - p_t2t->bytes_count] = p_data[offset];
              if (p_t2t->bytes_count == 0) {
                if (p_t2t->num_mem_tlvs >= RW_T2T_MAX_MEM_TLVS) {
                  LOG(ERROR) << StringPrintf(
                      "rw_t2t_handle_tlv_detect_rsp - Maximum buffer allocated "
                      "for Memory tlv has reached");
                  failed = true;
                } else {
                  /* Extract memory control tlv */
                  p_t2t->mem_tlv[p_t2t->num_mem_tlvs].offset =
                      (p_t2t->tlv_value[0] >> 4) & 0x0F;
                  p_t2t->mem_tlv[p_t2t->num_mem_tlvs].offset *=
                      (uint8_t)tags_pow(2, p_t2t->tlv_value[2] & 0x0F);
                  p_t2t->mem_tlv[p_t2t->num_mem_tlvs].offset +=
                      p_t2t->tlv_value[0] & 0x0F;
                  p_t2t->mem_tlv[p_t2t->num_mem_tlvs].num_bytes =
                      p_t2t->tlv_value[1];
                  p_t2t->num_mem_tlvs++;
                  rw_t2t_update_attributes();
                  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
                }
              }
            } else {
              if (p_t2t->bytes_count == 0) {
                p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
              }
            }
            break;

          case TAG_PROPRIETARY_TLV:
            if (p_t2t->bytes_count > 0) {
              p_t2t->bytes_count--;
            } else {
              LOG(ERROR) << StringPrintf("bytes_count underflow!");
              android_errorWriteLog(0x534e4554, "120506143");
            }
            if (tlvtype == TAG_PROPRIETARY_TLV) {
              found = true;
              p_t2t->prop_msg_len = len;
            } else {
              if (p_t2t->bytes_count == 0) {
                p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
              }
            }
            break;
        }
        offset++;
        break;
    }
  }

  p_t2t->work_offset += T2T_READ_DATA_LEN;

  event = rw_t2t_info_to_event(p_cmd_rsp_info);

  /* If not found and not failed, read next block and search tlv */
  if (!found && !failed) {
    if (p_t2t->work_offset >=
        (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] * T2T_TMS_TAG_FACTOR + T2T_FIRST_DATA_BLOCK * T2T_BLOCK_LEN)) {
      if (((tlvtype == TAG_LOCK_CTRL_TLV) && (p_t2t->num_lockbytes > 0)) ||
          ((tlvtype == TAG_MEM_CTRL_TLV) && (p_t2t->num_mem_tlvs > 0))) {
        found = true;
      } else {
        failed = true;
      }
    } else {
      if (rw_t2t_read((uint16_t)(p_t2t->work_offset / T2T_BLOCK_LEN) ) != NFC_STATUS_OK)
        failed = true;
    }
  }

  if (failed || found) {
    if (tlvtype == TAG_LOCK_CTRL_TLV) {
      /* Incase no Lock control tlv is present then look for default dynamic
       * lock bytes */
      rw_t2t_extract_default_locks_info();

      /* Send command to read the dynamic lock bytes */
      status = rw_t2t_read_locks();

      if (status != NFC_STATUS_CONTINUE) {
        /* If unable to read a lock/all locks read, notify upper layer */
        rw_t2t_update_lock_attributes();
        rw_t2t_ntf_tlv_detect_complete(status);
      }
    } else if (tlvtype == TAG_NDEF_TLV) {
      rw_t2t_extract_default_locks_info();

      if (failed) {
        rw_t2t_ntf_tlv_detect_complete(NFC_STATUS_FAILED);
      } else {
        /* NDEF present,Send command to read the dynamic lock bytes */
        status = rw_t2t_read_locks();
        if (status != NFC_STATUS_CONTINUE) {
          /* If unable to read a lock/all locks read, notify upper layer */
          rw_t2t_update_lock_attributes();
          rw_t2t_ntf_tlv_detect_complete(status);
        }
      }
    } else {
      /* Notify Memory/ Proprietary tlv detect result */
      status = failed ? NFC_STATUS_FAILED : NFC_STATUS_OK;
      rw_t2t_ntf_tlv_detect_complete(status);
    }
  }
}

/*******************************************************************************
**
** Function         rw_t2t_read_locks
**
** Description      This function will send command to read next unread locks
**
** Returns          NFC_STATUS_OK, if all locks are read successfully
**                  NFC_STATUS_FAILED, if reading locks failed
**                  NFC_STATUS_CONTINUE, if reading locks is in progress
**
*******************************************************************************/
tNFC_STATUS rw_t2t_read_locks(void) {
  uint8_t num_locks = 0;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_CONTINUE;
  uint16_t offset;
  uint16_t block;

  if ((p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] != T2T_CC3_RWA_RW) ||
      (p_t2t->skip_dyn_locks)) {
    /* Skip reading dynamic lock bytes if CC is set as Read only or layer above
     * instructs to skip */
    while (num_locks < p_t2t->num_lockbytes) {
      p_t2t->lockbyte[num_locks].lock_byte = 0x00;
      p_t2t->lockbyte[num_locks].b_lock_read = true;
      num_locks++;
    }
  }

  while (num_locks < p_t2t->num_lockbytes) {
    if (p_t2t->lockbyte[num_locks].b_lock_read == false) {
      /* Send Read command to read the first un read locks */
      offset = p_t2t->lock_tlv[p_t2t->lockbyte[num_locks].tlv_index].offset +
               p_t2t->lockbyte[num_locks].byte_index;

      /* Read 16 bytes where this lock byte is present */
      block = (uint16_t)(offset / T2T_BLOCK_LEN);
      block -= block % T2T_READ_BLOCKS;

      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_LOCKS;
      /* send READ8 command */
      status = rw_t2t_read((uint16_t)block);
      if (status == NFC_STATUS_OK) {
        /* Reading Locks */
        status = NFC_STATUS_CONTINUE;
      } else {
        status = NFC_STATUS_FAILED;
      }
      break;
    }
    num_locks++;
  }
  if (num_locks == p_t2t->num_lockbytes) {
    /* All locks are read */
    status = NFC_STATUS_OK;
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_extract_default_locks_info
**
** Description      This function will prepare lockbytes information for default
**                  locks present in the tag in the absence of lock control tlv.
**                  Adding a virtual lock control tlv for these lock bytes for
**                  easier manipulation.
**
** Returns          None
**
*******************************************************************************/
void rw_t2t_extract_default_locks_info(void) {
  uint8_t num_dynamic_lock_bits;
  uint8_t num_dynamic_lock_bytes;
  uint8_t xx;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  const tT2T_INIT_TAG* p_ret;
  uint8_t bytes_locked_per_lock_bit = T2T_DEFAULT_LOCK_BLPB;

  if ((p_t2t->num_lock_tlvs == 0) &&
      (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] > T2T_CC2_TMS_STATIC)) {
    /* No Lock control tlv is detected. Indicates lock bytes are present in
     * default location */
    /* Add a virtual Lock tlv to map this default lock location */
    p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], false, 0);
    if (p_ret != nullptr) bytes_locked_per_lock_bit = p_ret->default_lock_blpb;

    num_dynamic_lock_bits =
        ((p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] * T2T_TMS_TAG_FACTOR) -
         (T2T_STATIC_SIZE - T2T_HEADER_SIZE)) /
        bytes_locked_per_lock_bit;
    num_dynamic_lock_bytes = num_dynamic_lock_bits / 8;
    num_dynamic_lock_bytes += (num_dynamic_lock_bits % 8 == 0) ? 0 : 1;
    if (num_dynamic_lock_bytes > RW_T2T_MAX_LOCK_BYTES) {
      LOG(ERROR) << StringPrintf(
          "rw_t2t_extract_default_locks_info - buffer size: %u less than "
          "DynLock area sise: %u",
          RW_T2T_MAX_LOCK_BYTES, num_dynamic_lock_bytes);
      num_dynamic_lock_bytes = RW_T2T_MAX_LOCK_BYTES;
      android_errorWriteLog(0x534e4554, "147310721");
    }

    p_t2t->lock_tlv[p_t2t->num_lock_tlvs].offset =
        (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] * T2T_TMS_TAG_FACTOR) +
        (T2T_FIRST_DATA_BLOCK * T2T_BLOCK_LEN);
    p_t2t->lock_tlv[p_t2t->num_lock_tlvs].bytes_locked_per_bit =
        bytes_locked_per_lock_bit;
    p_t2t->lock_tlv[p_t2t->num_lock_tlvs].num_bits = num_dynamic_lock_bits;

    /* Based on tag data size the number of locks present in the default
     * location changes */
    for (xx = 0; xx < num_dynamic_lock_bytes; xx++) {
      p_t2t->lockbyte[xx].tlv_index = p_t2t->num_lock_tlvs;
      p_t2t->lockbyte[xx].byte_index = xx;
      p_t2t->lockbyte[xx].b_lock_read = false;
    }
    p_t2t->num_lockbytes = num_dynamic_lock_bytes;
    p_t2t->num_lock_tlvs = 1;
  }
}

/*******************************************************************************
**
** Function         rw_t2t_read_ndef_last_block
**
** Description      This function will locate and read the last ndef block.
**                  The last ndef block refers to the tag block where last byte
**                  of new ndef message will reside. Also this function will
**                  locate the offset of Terminator TLV based on the size of
**                  new NDEF Message
**
** Returns          NCI_STATUS_OK, if able to locate last ndef block & read
**                  started. Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_read_ndef_last_block(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t header_len = (p_t2t->new_ndef_msg_len >= T2T_LONG_NDEF_MIN_LEN)
                            ? T2T_LONG_NDEF_LEN_FIELD_LEN
                            : T2T_SHORT_NDEF_LEN_FIELD_LEN;
  uint16_t num_ndef_bytes;
  uint16_t total_ndef_bytes;
  uint16_t last_ndef_byte_offset;
  uint16_t terminator_tlv_byte_index;
  tNFC_STATUS status;
  uint16_t block;

  total_ndef_bytes = header_len + p_t2t->new_ndef_msg_len;
  num_ndef_bytes = 0;
  last_ndef_byte_offset = p_t2t->ndef_header_offset;

  /* Locate NDEF final block based on the size of new NDEF Message */
  while (num_ndef_bytes < total_ndef_bytes) {
    if (rw_t2t_is_lock_res_byte((uint16_t)(last_ndef_byte_offset)) == false)
      num_ndef_bytes++;

    last_ndef_byte_offset++;
  }
  p_t2t->ndef_last_block_num =
      (uint16_t)((last_ndef_byte_offset - 1) / T2T_BLOCK_SIZE);
  block = p_t2t->ndef_last_block_num;

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_NDEF_LAST_BLOCK;
  /* Read NDEF last block before updating */
  status = rw_t2t_read(block);
  if (status == NFC_STATUS_OK) {
    if ((p_t2t->new_ndef_msg_len + 1) <= p_t2t->max_ndef_msg_len) {
      /* Locate Terminator TLV Block */
      total_ndef_bytes++;
      terminator_tlv_byte_index = last_ndef_byte_offset;

      while (num_ndef_bytes < total_ndef_bytes) {
        if (rw_t2t_is_lock_res_byte((uint16_t)terminator_tlv_byte_index) ==
            false)
          num_ndef_bytes++;

        terminator_tlv_byte_index++;
      }

      p_t2t->terminator_byte_index = terminator_tlv_byte_index - 1;
    } else {
      /* No space for Terminator TLV */
      p_t2t->terminator_byte_index = 0x00;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_read_terminator_tlv_block
**
** Description      This function will read the block where terminator tlv will
**                  be added later
**
** Returns          NCI_STATUS_OK, if read was started. Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_read_terminator_tlv_block(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;
  uint16_t block;

  /* Send read command to read base block (Block % 4==0) where this block is
   * also read as part of 16 bytes */
  block = p_t2t->terminator_byte_index / T2T_BLOCK_SIZE;
  block -= block % T2T_READ_BLOCKS;

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_TERM_TLV_BLOCK;
  /* Read the block where Terminator TLV may be added later during NDEF Write
   * operation */
  status = rw_t2t_read(block);
  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_read_ndef_next_block
**
** Description      This function will read the tag block passed as argument
**
** Returns          NCI_STATUS_OK, if read was started. Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_read_ndef_next_block(uint16_t block) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;

  /* Send read command to read base block (Block % 4==0) where this block is
   * also read as part of 16 bytes */
  block -= block % T2T_READ_BLOCKS;

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_NDEF_NEXT_BLOCK;
  /* Read the block */
  status = rw_t2t_read(block);

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_is_read_before_write_block
**
** Description      This function will check if the block has to be read before
**                  writting to avoid over writting in to lock/reserved bytes
**                  present in the block.
**                  If no bytes in the block can be overwritten it moves in to
**                  next block and check. Finally it finds a block where part of
**                  ndef bytes can exist and check if the whole block can be
**                  updated or only part of block can be modified.
**
** Returns          TRUE, if the block returned should be read before writting
**                  FALSE, if the block need not be read as it was already
**                         read or during NDEF write we may completely overwrite
**                         the block and there is no reserved or locked bytes in
**                         that block
**
*******************************************************************************/
static bool rw_t2t_is_read_before_write_block(uint16_t block,
                                              uint16_t* p_block_to_read) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t* p_cc = &p_t2t->tag_hdr[T2T_CC0_NMN_BYTE];
  uint8_t count;
  uint8_t index;
  uint16_t tag_size = p_cc[2] * 2 + T2T_FIRST_DATA_BLOCK;
  bool read_before_write = true;

  if (block == p_t2t->ndef_header_offset / T2T_BLOCK_SIZE) {
    /* First NDEF block is already read */
    read_before_write = false;
    memcpy(p_t2t->ndef_read_block, p_t2t->ndef_first_block, T2T_BLOCK_SIZE);
  } else if (block == p_t2t->ndef_last_block_num) {
    /* Last NDEF block is already read */
    read_before_write = false;
    memcpy(p_t2t->ndef_read_block, p_t2t->ndef_last_block, T2T_BLOCK_SIZE);
  } else if (block == p_t2t->terminator_byte_index / T2T_BLOCK_SIZE) {
    /* Terminator tlv block is already read */
    read_before_write = false;
    memcpy(p_t2t->ndef_read_block, p_t2t->terminator_tlv_block, T2T_BLOCK_SIZE);
  } else {
    count = 0;
    while (block < tag_size) {
      index = 0;

      while (index < T2T_BLOCK_SIZE) {
        /* check if it is a reserved or locked byte */
        if (rw_t2t_is_lock_res_byte(
                (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
          count++;
        }
        index++;
      }
      if (count == T2T_BLOCK_SIZE) {
        /* All the bytes in the block are free to NDEF write  */
        read_before_write = false;
        break;
      } else if (count == 0) {
        /* The complete block is not free for NDEF write  */
        index = 0;
        block++;
      } else {
        /* The block has reseved byte (s) or locked byte (s) or both */
        read_before_write = true;
        break;
      }
    }
  }
  /* Return the block to read next before NDEF write */
  *p_block_to_read = block;
  return read_before_write;
}

/*******************************************************************************
**
** Function         rw_t2t_write_ndef_first_block
**
** Description      This function will write the first NDEF block with Length
**                  field reset to zero.
**                  Also after writting NDEF this function may be called to
**                  update new NDEF length
**
** Returns          NCI_STATUS_OK, if write was started.
**                  Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_write_ndef_first_block(uint16_t msg_len, bool b_update_len) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t new_lengthfield_len;
  uint8_t write_block[4];
  uint8_t block;
  uint8_t* p_cc = &p_t2t->tag_hdr[T2T_CC0_NMN_BYTE];
  uint16_t total_blocks = p_cc[2] * 2 + T2T_FIRST_DATA_BLOCK;
  tNFC_STATUS status;
  uint8_t length_field[3];
  uint8_t index;

  p_t2t->work_offset = 0;
  new_lengthfield_len = p_t2t->new_ndef_msg_len >= T2T_LONG_NDEF_MIN_LEN
                            ? T2T_LONG_NDEF_LEN_FIELD_LEN
                            : T2T_SHORT_NDEF_LEN_FIELD_LEN;
  if (new_lengthfield_len == 3) {
    /* New NDEF is Long NDEF */
    if (msg_len == 0) {
      /* Clear NDEF length field */
      length_field[0] = 0x00;
      length_field[1] = 0x00;
      length_field[2] = 0x00;
    } else {
      /* Update NDEF length field with new NDEF Msg len */
      length_field[0] = T2T_LONG_NDEF_LEN_FIELD_BYTE0;
      length_field[1] = (uint8_t)(msg_len >> 8);
      length_field[2] = (uint8_t)(msg_len);
    }
  } else {
    /* New NDEF is Short NDEF */
    length_field[0] = (uint8_t)(msg_len);
  }

  /* updating ndef_first_block with new ndef message */
  memcpy(write_block, p_t2t->ndef_first_block, T2T_BLOCK_SIZE);

  index = p_t2t->ndef_header_offset % T2T_BLOCK_SIZE;
  block = (uint8_t)(p_t2t->ndef_header_offset / T2T_BLOCK_SIZE);

  while (p_t2t->work_offset == 0 && block < total_blocks) {
    /* update length field */
    while (index < T2T_BLOCK_SIZE &&
           p_t2t->work_offset < p_t2t->new_ndef_msg_len) {
      if (rw_t2t_is_lock_res_byte(
              (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
        write_block[index] = length_field[p_t2t->work_offset];
        p_t2t->work_offset++;
      }
      index++;
      if (p_t2t->work_offset == new_lengthfield_len) {
        break;
      }
    }
    /* If more space in this block then add ndef message */
    while (index < T2T_BLOCK_SIZE &&
           p_t2t->work_offset <
               (p_t2t->new_ndef_msg_len + new_lengthfield_len)) {
      if (rw_t2t_is_lock_res_byte(
              (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
        write_block[index] =
            p_t2t->p_new_ndef_buffer[p_t2t->work_offset - new_lengthfield_len];
        p_t2t->work_offset++;
      }
      index++;
    }
    if (p_t2t->work_offset == 0) {
      /* If no bytes are written move to next block */
      index = 0;
      block++;
      if (block == p_t2t->ndef_last_block_num) {
        memcpy(write_block, p_t2t->ndef_last_block, T2T_BLOCK_SIZE);
      }
    }
  }
  if (p_t2t->work_offset == 0) {
    status = NFC_STATUS_FAILED;
  } else {
    rw_t2t_update_cb(block, write_block, b_update_len);
    /* Update the identified block with newly prepared data */
    status = rw_t2t_write(block, write_block);
    if (status == NFC_STATUS_OK) {
      p_t2t->b_read_data = false;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_write_ndef_next_block
**
** Description      This function can be called to write an NDEF message block
**
** Returns          NCI_STATUS_OK, if write was started.
**                  Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_write_ndef_next_block(uint16_t block, uint16_t msg_len,
                                         bool b_update_len) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t new_lengthfield_len;
  uint8_t write_block[4];
  uint8_t* p_cc = &p_t2t->tag_hdr[T2T_CC0_NMN_BYTE];
  uint16_t total_blocks = p_cc[2] * 2 + T2T_FIRST_DATA_BLOCK;
  uint16_t initial_offset;
  uint8_t length_field[3];
  uint8_t index;
  tNFC_STATUS status;

  /* Write NDEF Message */
  new_lengthfield_len = p_t2t->new_ndef_msg_len >= T2T_LONG_NDEF_MIN_LEN
                            ? T2T_LONG_NDEF_LEN_FIELD_LEN
                            : T2T_SHORT_NDEF_LEN_FIELD_LEN;

  index = 0;

  memcpy(write_block, p_t2t->ndef_read_block, T2T_BLOCK_SIZE);

  if (p_t2t->work_offset >= new_lengthfield_len) {
    /* Length field is updated, write ndef message field */
    initial_offset = p_t2t->work_offset;
    while (p_t2t->work_offset == initial_offset && block < total_blocks) {
      while (index < T2T_BLOCK_SIZE &&
             p_t2t->work_offset <
                 (p_t2t->new_ndef_msg_len + new_lengthfield_len)) {
        if (rw_t2t_is_lock_res_byte(
                (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
          write_block[index] =
              p_t2t
                  ->p_new_ndef_buffer[p_t2t->work_offset - new_lengthfield_len];
          p_t2t->work_offset++;
        }
        index++;
      }
      if (p_t2t->work_offset == initial_offset) {
        index = 0;
        block++;
      }
    }
  } else {
    /* Complete writting Length field and then write ndef message */
    new_lengthfield_len = p_t2t->new_ndef_msg_len >= T2T_LONG_NDEF_MIN_LEN
                              ? T2T_LONG_NDEF_LEN_FIELD_LEN
                              : T2T_SHORT_NDEF_LEN_FIELD_LEN;
    if (new_lengthfield_len == 3) {
      /* New NDEF is Long NDEF */
      if (msg_len == 0) {
        length_field[0] = 0x00;
        length_field[1] = 0x00;
        length_field[2] = 0x00;
      } else {
        length_field[0] = T2T_LONG_NDEF_LEN_FIELD_BYTE0;
        length_field[1] = (uint8_t)(msg_len >> 8);
        length_field[2] = (uint8_t)(msg_len);
      }
    } else {
      /* New NDEF is short NDEF */
      length_field[0] = (uint8_t)(msg_len);
    }
    initial_offset = p_t2t->work_offset;
    while (p_t2t->work_offset == initial_offset && block < total_blocks) {
      /* Update length field */
      while (index < T2T_BLOCK_SIZE &&
             p_t2t->work_offset < p_t2t->new_ndef_msg_len) {
        if (rw_t2t_is_lock_res_byte(
                (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
          write_block[index] = length_field[p_t2t->work_offset];
          p_t2t->work_offset++;
        }
        index++;
        if (p_t2t->work_offset == new_lengthfield_len) {
          break;
        }
      }
      /* Update ndef message field */
      while (index < T2T_BLOCK_SIZE &&
             p_t2t->work_offset <
                 (p_t2t->new_ndef_msg_len + new_lengthfield_len)) {
        if (rw_t2t_is_lock_res_byte(
                (uint16_t)((block * T2T_BLOCK_SIZE) + index)) == false) {
          write_block[index] =
              p_t2t
                  ->p_new_ndef_buffer[p_t2t->work_offset - new_lengthfield_len];
          p_t2t->work_offset++;
        }
        index++;
      }
      if (p_t2t->work_offset == initial_offset) {
        index = 0;
        block++;
      }
    }
  }
  if (p_t2t->work_offset == initial_offset) {
    status = NFC_STATUS_FAILED;
  } else {
    rw_t2t_update_cb(block, write_block, b_update_len);
    /* Write the NDEF Block */
    status = rw_t2t_write(block, write_block);
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_update_cb
**
** Description      This function can be called to write an NDEF message block
**
** Returns          NCI_STATUS_OK, if write was started.
**                  Otherwise, error status.
**
*******************************************************************************/
static void rw_t2t_update_cb(uint16_t block, uint8_t* p_write_block,
                             bool b_update_len) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t new_lengthfield_len;

  /* Write NDEF Message */
  new_lengthfield_len = p_t2t->new_ndef_msg_len >= T2T_LONG_NDEF_MIN_LEN
                            ? T2T_LONG_NDEF_LEN_FIELD_LEN
                            : T2T_SHORT_NDEF_LEN_FIELD_LEN;

  if (block == p_t2t->ndef_header_offset / T2T_BLOCK_SIZE) {
    /* Update ndef first block if the 'block' points to ndef first block */
    memcpy(p_t2t->ndef_first_block, p_write_block, T2T_BLOCK_SIZE);
  }
  if (p_t2t->terminator_byte_index / T2T_BLOCK_SIZE == block) {
    /* Update terminator block if the 'block' points to terminator tlv block */
    memcpy(p_t2t->terminator_tlv_block, p_write_block, T2T_BLOCK_LEN);
  }
  if (b_update_len == false) {
    if (block == p_t2t->ndef_last_block_num) {
      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LAST_BLOCK;
      p_t2t->work_offset = 0;
      /* Update ndef final block if the 'block' points to ndef final block */
      memcpy(p_t2t->ndef_last_block, p_write_block, T2T_BLOCK_SIZE);
    } else {
      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_NEXT_BLOCK;
    }
  } else {
    if (block == p_t2t->ndef_last_block_num) {
      /* Update the backup of Ndef final block TLV block */
      memcpy(p_t2t->ndef_last_block, p_write_block, T2T_BLOCK_SIZE);
    }

    if (p_t2t->work_offset >= new_lengthfield_len) {
      if (p_t2t->terminator_byte_index != 0) {
        /* Add Terminator TLV as part of NDEF Write operation */
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_BLOCK;
      } else {
        /* Skip adding Terminator TLV */
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT;
      }
    } else {
      /* Part of NDEF Message Len should be added in the next block */
      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK;
    }
  }
}

/*******************************************************************************
**
** Function         rw_t2t_get_ndef_flags
**
** Description      Prepare NDEF Flags
**
** Returns          NDEF Flag value
**
*******************************************************************************/
static uint8_t rw_t2t_get_ndef_flags(void) {
  uint8_t flags = 0;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  const tT2T_INIT_TAG* p_ret;

  flags |= RW_NDEF_FL_SUPPORTED;

  if ((p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] == T2T_CC2_TMS_STATIC) ||
      (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] == 0))
    flags |= RW_NDEF_FL_FORMATABLE;

  if ((p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] & T2T_CC3_RWA_RO) == T2T_CC3_RWA_RO)
    flags |= RW_NDEF_FL_READ_ONLY;

  if (((p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], false, 0)) != nullptr) &&
      (p_ret->b_otp)) {
    /* Set otp flag */
    flags |= RW_NDEF_FL_OTP;

    /* Set Read only flag if otp tag already has NDEF Message */
    if (p_t2t->ndef_msg_len) flags |= RW_NDEF_FL_READ_ONLY;
  }
  return flags;
}

/*******************************************************************************
**
** Function         rw_t2t_get_ndef_max_size
**
** Description      Calculate maximum size of NDEF message that can be written
**                  on to the tag
**
** Returns          Maximum size of NDEF Message
**
*******************************************************************************/
static uint16_t rw_t2t_get_ndef_max_size(void) {
  uint16_t offset;
  uint8_t xx;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t tag_size = (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] * T2T_TMS_TAG_FACTOR) +
                      (T2T_FIRST_DATA_BLOCK * T2T_BLOCK_LEN) +
                      p_t2t->num_lockbytes;

  for (xx = 0; xx < p_t2t->num_mem_tlvs; xx++)
    tag_size += p_t2t->mem_tlv[xx].num_bytes;

  offset = p_t2t->ndef_msg_offset;
  p_t2t->max_ndef_msg_len = 0;

  if ((tag_size < T2T_STATIC_SIZE) ||
      (tag_size > (T2T_SECTOR_SIZE * T2T_MAX_SECTOR)) ||
      ((p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] != T2T_CC0_NMN) &&
       (p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] != 0))) {
    /* Tag not formated, assume static tag */
    p_t2t->max_ndef_msg_len = T2T_STATIC_SIZE - T2T_HEADER_SIZE -
                              T2T_TLV_TYPE_LEN - T2T_SHORT_NDEF_LEN_FIELD_LEN;
    return p_t2t->max_ndef_msg_len;
  }

  /* Starting from NDEF Message offset find the first locked data byte */
  while (offset < tag_size) {
    if (rw_t2t_is_lock_res_byte((uint16_t)offset) == false) {
      if (rw_t2t_is_read_only_byte((uint16_t)offset) == true) break;
      p_t2t->max_ndef_msg_len++;
    }
    offset++;
  }
  /* NDEF Length field length changes based on NDEF size */
  if ((p_t2t->max_ndef_msg_len >= T2T_LONG_NDEF_LEN_FIELD_BYTE0) &&
      ((p_t2t->ndef_msg_offset - p_t2t->ndef_header_offset) ==
       T2T_SHORT_NDEF_LEN_FIELD_LEN)) {
    p_t2t->max_ndef_msg_len -=
        (p_t2t->max_ndef_msg_len == T2T_LONG_NDEF_LEN_FIELD_BYTE0)
            ? 1
            : (T2T_LONG_NDEF_LEN_FIELD_LEN - T2T_SHORT_NDEF_LEN_FIELD_LEN);
  }
  return p_t2t->max_ndef_msg_len;
}

/*******************************************************************************
**
** Function         rw_t2t_add_terminator_tlv
**
** Description      This function will add terminator TLV after NDEF Message
**
** Returns          NCI_STATUS_OK, if write was started.
**                  Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS rw_t2t_add_terminator_tlv(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;
  uint16_t block;

  /* Add Terminator TLV after NDEF Message */
  p_t2t->terminator_tlv_block[p_t2t->terminator_byte_index % T2T_BLOCK_LEN] =
      TAG_TERMINATOR_TLV;
  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT;

  block = p_t2t->terminator_byte_index / T2T_BLOCK_LEN;
  status = rw_t2t_write(block, p_t2t->terminator_tlv_block);

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_handle_ndef_read_rsp
**
** Description      This function handles reading an NDEF message.
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_ndef_read_rsp(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tRW_READ_DATA evt_data;
  uint16_t len;
  uint16_t offset;
  bool failed = false;
  bool done = false;

  /* On the first read, adjust for any partial block offset */
  offset = 0;
  len = T2T_READ_DATA_LEN;

  if (p_t2t->work_offset == 0) {
    /* The Ndef Message offset may be present in the read 16 bytes */
    offset = (p_t2t->ndef_msg_offset - (p_t2t->block_read * T2T_BLOCK_SIZE));
  }

  /* Skip all reserved and lock bytes */
  while ((offset < len) && (p_t2t->work_offset < p_t2t->ndef_msg_len))

  {
    if (rw_t2t_is_lock_res_byte(
            (uint16_t)(offset + p_t2t->block_read * T2T_BLOCK_LEN)) == false) {
      /* Collect the NDEF Message */
      p_t2t->p_ndef_buffer[p_t2t->work_offset] = p_data[offset];
      p_t2t->work_offset++;
    }
    offset++;
  }

  if (p_t2t->work_offset >= p_t2t->ndef_msg_len) {
    done = true;
    p_t2t->ndef_status = T2T_NDEF_READ;
  } else {
    /* Read next 4 blocks */
    if (rw_t2t_read((uint16_t)(p_t2t->block_read + T2T_READ_BLOCKS)) !=
        NFC_STATUS_OK)
      failed = true;
  }

  if (failed || done) {
    evt_data.status = failed ? NFC_STATUS_FAILED : NFC_STATUS_OK;
    evt_data.p_data = nullptr;
    rw_t2t_handle_op_complete();
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*rw_cb.p_cback)(RW_T2T_NDEF_READ_EVT, &rw_data);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_handle_ndef_write_rsp
**
** Description      Handle response received to reading (or part of) NDEF
**                  message.
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_ndef_write_rsp(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tRW_READ_DATA evt_data;
  bool failed = false;
  bool done = false;
  uint16_t block;
  uint8_t offset;

  switch (p_t2t->substate) {
    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_FIRST_BLOCK:

      /* Backup the read NDEF first block */
      memcpy(p_t2t->ndef_first_block, p_data, T2T_BLOCK_LEN);
      /* Read ndef final block */
      if (rw_t2t_read_ndef_last_block() != NFC_STATUS_OK) failed = true;
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_LAST_BLOCK:

      offset = (uint8_t)(p_t2t->ndef_last_block_num - p_t2t->block_read) *
               T2T_BLOCK_SIZE;
      /* Backup the read NDEF final block */
      memcpy(p_t2t->ndef_last_block, &p_data[offset], T2T_BLOCK_LEN);
      if ((p_t2t->terminator_byte_index / T2T_BLOCK_SIZE) ==
          p_t2t->ndef_last_block_num) {
        /* If Terminator TLV will reside on the NDEF Final block */
        memcpy(p_t2t->terminator_tlv_block, p_t2t->ndef_last_block,
               T2T_BLOCK_LEN);
        if (rw_t2t_write_ndef_first_block(0x0000, false) != NFC_STATUS_OK)
          failed = true;
      } else if (p_t2t->terminator_byte_index != 0) {
        /* If there is space for Terminator TLV and if it will reside outside
         * NDEF Final block */
        if (rw_t2t_read_terminator_tlv_block() != NFC_STATUS_OK) failed = true;
      } else {
        if (rw_t2t_write_ndef_first_block(0x0000, false) != NFC_STATUS_OK)
          failed = true;
      }
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_TERM_TLV_BLOCK:

      offset = (uint8_t)(((p_t2t->terminator_byte_index / T2T_BLOCK_SIZE) -
                          p_t2t->block_read) *
                         T2T_BLOCK_SIZE);
      /* Backup the read Terminator TLV block */
      memcpy(p_t2t->terminator_tlv_block, &p_data[offset], T2T_BLOCK_LEN);

      /* Write the first block for new NDEF Message */
      if (rw_t2t_write_ndef_first_block(0x0000, false) != NFC_STATUS_OK)
        failed = true;
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_NEXT_BLOCK:

      offset = (uint8_t)(p_t2t->ndef_read_block_num - p_t2t->block_read) *
               T2T_BLOCK_SIZE;
      /* Backup read block */
      memcpy(p_t2t->ndef_read_block, &p_data[offset], T2T_BLOCK_LEN);

      /* Update the block with new NDEF Message */
      if (rw_t2t_write_ndef_next_block(p_t2t->ndef_read_block_num, 0x0000,
                                       false) != NFC_STATUS_OK)
        failed = true;
      break;

    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_NEXT_BLOCK:
    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK:
      if (rw_t2t_is_read_before_write_block(
              (uint16_t)(p_t2t->block_written + 1), &block) == true) {
        p_t2t->ndef_read_block_num = block;
        /* If only part of the block is going to be updated read the block to
           retain previous data for
           unchanged part of the block */
        if (rw_t2t_read_ndef_next_block(block) != NFC_STATUS_OK) failed = true;
      } else {
        if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK) {
          /* Directly write the block with new NDEF contents as whole block is
           * going to be updated */
          if (rw_t2t_write_ndef_next_block(block, p_t2t->new_ndef_msg_len,
                                           true) != NFC_STATUS_OK)
            failed = true;
        } else {
          /* Directly write the block with new NDEF contents as whole block is
           * going to be updated */
          if (rw_t2t_write_ndef_next_block(block, 0x0000, false) !=
              NFC_STATUS_OK)
            failed = true;
        }
      }
      break;

    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LAST_BLOCK:
      /* Write the next block for new NDEF Message */
      p_t2t->ndef_write_block = p_t2t->ndef_header_offset / T2T_BLOCK_SIZE;
      if (rw_t2t_is_read_before_write_block((uint16_t)(p_t2t->ndef_write_block),
                                            &block) == true) {
        /* If only part of the block is going to be updated read the block to
           retain previous data for
           part of the block thats not going to be changed */
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_NDEF_LEN_BLOCK;
        if (rw_t2t_read(block) != NFC_STATUS_OK) failed = true;

      } else {
        /* Update NDEF Message Length in the Tag */
        if (rw_t2t_write_ndef_first_block(p_t2t->new_ndef_msg_len, true) !=
            NFC_STATUS_OK)
          failed = true;
      }
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_LEN_BLOCK:
      /* Backup read block */
      memcpy(p_t2t->ndef_read_block, p_data, T2T_BLOCK_LEN);

      /* Update the block with new NDEF Message */
      if (rw_t2t_write_ndef_next_block(p_t2t->block_read,
                                       p_t2t->new_ndef_msg_len,
                                       true) == NFC_STATUS_OK)
        p_t2t->ndef_write_block = p_t2t->block_read + 1;
      else
        failed = true;

      break;

    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_BLOCK:
      if (rw_t2t_add_terminator_tlv() != NFC_STATUS_OK) failed = true;
      break;

    case RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT:
      done = true;
      break;

    default:
      break;
  }

  if (failed || done) {
    evt_data.p_data = nullptr;
    /* NDEF WRITE Operation is done, inform up the stack */
    evt_data.status = failed ? NFC_STATUS_FAILED : NFC_STATUS_OK;
    if (done) {
      if ((p_t2t->ndef_msg_len >= 0x00FF) &&
          (p_t2t->new_ndef_msg_len < 0x00FF)) {
        p_t2t->ndef_msg_offset -= 2;
      } else if ((p_t2t->new_ndef_msg_len >= 0x00FF) &&
                 (p_t2t->ndef_msg_len < 0x00FF)) {
        p_t2t->ndef_msg_offset += 2;
      }
      p_t2t->ndef_msg_len = p_t2t->new_ndef_msg_len;
    }
    rw_t2t_handle_op_complete();
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*rw_cb.p_cback)(RW_T2T_NDEF_WRITE_EVT, &rw_data);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_get_tag_size
**
** Description      This function calculates tag data area size from data read
**                  from block with version number
**
** Returns          TMS of the tag
**
*******************************************************************************/
static uint8_t rw_t2t_get_tag_size(uint8_t* p_data) {
  uint16_t LchunkSize = 0;
  uint16_t Num_LChuncks = 0;
  uint16_t tms = 0;

  LchunkSize = (uint16_t)p_data[2] << 8 | p_data[3];
  Num_LChuncks = (uint16_t)p_data[4] << 8 | p_data[5];

  tms = (uint16_t)(LchunkSize * Num_LChuncks);

  tms += (T2T_STATIC_SIZE - T2T_HEADER_SIZE);

  tms /= 0x08;

  return (uint8_t)tms;
}

/*******************************************************************************
**
** Function         rw_t2t_handle_config_tag_readonly
**
** Description      This function handles configure type 2 tag as read only
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_config_tag_readonly(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  bool b_notify = false;
  uint8_t write_block[T2T_BLOCK_SIZE];
  bool b_pending = false;
  uint8_t read_lock = 0;
  uint8_t num_locks = 0;
  uint16_t offset;

  switch (p_t2t->substate) {
    case RW_T2T_SUBSTATE_WAIT_READ_CC:

      /* First soft lock the tag */
      rw_t2t_soft_lock_tag();

      break;

    case RW_T2T_SUBSTATE_WAIT_SET_CC_RO:

      /* Successfully soft locked! Update Tag header for future reference */
      p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] = T2T_CC3_RWA_RO;
      if (!p_t2t->b_hard_lock) {
        /* Tag configuration complete */
        status = NFC_STATUS_OK;
        b_notify = true;
        break;
      }
      FALLTHROUGH_INTENDED;

    /* Coverity: [FALSE-POSITIVE error] intended fall through */
    /* Missing break statement between cases in switch statement */
    case RW_T2T_SUBSTATE_WAIT_SET_DYN_LOCK_BITS:

      num_locks = 0;

      while (num_locks < p_t2t->num_lockbytes) {
        if (p_t2t->lockbyte[num_locks].lock_status ==
            RW_T2T_LOCK_UPDATE_INITIATED) {
          /* Update control block as one or more dynamic lock byte (s) are set
           */
          p_t2t->lockbyte[num_locks].lock_status = RW_T2T_LOCK_UPDATED;
        }
        if (!b_pending &&
            p_t2t->lockbyte[num_locks].lock_status == RW_T2T_LOCK_NOT_UPDATED) {
          /* One or more dynamic lock bits are not set */
          b_pending = true;
          read_lock = num_locks;
        }
        num_locks++;
      }

      if (b_pending) {
        /* Read the block where dynamic lock bits are present to avoid writing
         * to NDEF bytes in the same block */
        offset = p_t2t->lock_tlv[p_t2t->lockbyte[read_lock].tlv_index].offset +
                 p_t2t->lockbyte[read_lock].byte_index;
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_DYN_LOCK_BYTE_BLOCK;
        status = rw_t2t_read((uint16_t)(offset / T2T_BLOCK_LEN));
      } else {
        /* Now set Static lock bits as no more dynamic lock bits to set */

        /* Copy the internal bytes */
        memcpy(write_block,
               &p_t2t->tag_hdr[T2T_STATIC_LOCK0 - T2T_INTERNAL_BYTES_LEN],
               T2T_INTERNAL_BYTES_LEN);
        /* Set all Static lock bits */
        write_block[T2T_STATIC_LOCK0 % T2T_BLOCK_SIZE] = 0xFF;
        write_block[T2T_STATIC_LOCK1 % T2T_BLOCK_SIZE] = 0xFF;
        p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_ST_LOCK_BITS;
        status = rw_t2t_write((T2T_STATIC_LOCK0 / T2T_BLOCK_SIZE), write_block);
      }
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_DYN_LOCK_BYTE_BLOCK:
      /* Now set the dynamic lock bits present in the block read now */
      status = rw_t2t_set_dynamic_lock_bits(p_data);
      break;

    case RW_T2T_SUBSTATE_WAIT_SET_ST_LOCK_BITS:
      /* Tag configuration complete */
      status = NFC_STATUS_OK;
      b_notify = true;
      break;
  }

  if (status != NFC_STATUS_OK || b_notify) {
    /* Notify upper layer the result of Configuring Tag as Read only */
    tRW_DATA evt;
    evt.status = status;
    rw_t2t_handle_op_complete();
    (*rw_cb.p_cback)(RW_T2T_SET_TAG_RO_EVT, &evt);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_handle_format_tag_rsp
**
** Description      This function handles formating a type 2 tag
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_handle_format_tag_rsp(uint8_t* p_data) {
  uint8_t* p;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  uint16_t version_no;
  const tT2T_INIT_TAG* p_ret;
  uint8_t tms;
  uint8_t next_block = T2T_FIRST_DATA_BLOCK + 1;
  uint16_t addr, locked_area;
  bool b_notify = false;

  p = p_t2t->ndef_final_block;
  UINT8_TO_BE_STREAM(p, p_t2t->tlv_value[2]);

  switch (p_t2t->substate) {
    case RW_T2T_SUBSTATE_WAIT_READ_CC:
      /* Start format operation */
      status = rw_t2t_format_tag();
      break;

    case RW_T2T_SUBSTATE_WAIT_READ_VERSION_INFO:

      memcpy(p_t2t->tag_data, p_data, T2T_READ_DATA_LEN);
      p_t2t->b_read_data = true;
      version_no = (uint16_t)p_data[0] << 8 | p_data[1];
      p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], true, version_no);
      if (p_ret != nullptr) {
        /* Valid Version Number */
        if (p_ret->b_calc_cc) /* Calculate tag size from Version Information */
          tms = rw_t2t_get_tag_size(p_data);

        else
          /* Tag size from Look up table */
          tms = p_ret->tms;

        /* Set CC with the Tag size from look up table or from calculated value
         */
        status = rw_t2t_set_cc(tms);
      }
      break;

    case RW_T2T_SUBSTATE_WAIT_SET_CC:

      version_no = (uint16_t)p_t2t->tag_data[0] << 8 | p_t2t->tag_data[1];
      if ((version_no == 0) ||
          ((p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], true, version_no)) ==
           nullptr) ||
          (!p_ret->b_multi_version) || (!p_ret->b_calc_cc)) {
        /* Currently Formating a non blank tag or a blank tag with manufacturer
         * has only one variant of tag. Set Null NDEF TLV and complete Format
         * Operation */
        next_block = T2T_FIRST_DATA_BLOCK;
        p = p_t2t->ndef_final_block;
      } else {
        addr = (uint16_t)(
            ((uint16_t)p_t2t->tag_data[2] << 8 | p_t2t->tag_data[3]) *
                ((uint16_t)p_t2t->tag_data[4] << 8 | p_t2t->tag_data[5]) +
            T2T_STATIC_SIZE);
        locked_area = ((uint16_t)p_t2t->tag_data[2] << 8 | p_t2t->tag_data[3]) *
                      ((uint16_t)p_t2t->tag_data[6]);

        status = rw_t2t_set_lock_tlv(addr, p_t2t->tag_data[7], locked_area);
        if (status == NFC_STATUS_REJECTED) {
          /* Cannot calculate Lock TLV. Set Null NDEF TLV and complete Format
           * Operation */
          next_block = T2T_FIRST_DATA_BLOCK;
          p = p_t2t->ndef_final_block;
        } else
          break;
      }
      FALLTHROUGH_INTENDED;

    case RW_T2T_SUBSTATE_WAIT_SET_LOCK_TLV:

      /* Prepare NULL NDEF TLV, TERMINATOR_TLV */
      UINT8_TO_BE_STREAM(p, TAG_NDEF_TLV);
      UINT8_TO_BE_STREAM(p, 0);

      if (((p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], false, 0)) != nullptr) &&
          (!p_ret->b_otp)) {
        UINT8_TO_BE_STREAM(p, TAG_TERMINATOR_TLV);
      } else
        UINT8_TO_BE_STREAM(p, 0);

      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_NULL_NDEF;
      /* send WRITE-E8 command */
      status = rw_t2t_write(next_block, p_t2t->ndef_final_block);
      if (status == NFC_STATUS_OK) p_t2t->b_read_data = false;
      break;

    case RW_T2T_SUBSTATE_WAIT_SET_NULL_NDEF:
      /* Tag Formated successfully */
      status = NFC_STATUS_OK;
      b_notify = true;
      break;

    default:
      break;
  }

  if (status != NFC_STATUS_OK || b_notify) {
    /* Notify upper layer the result of Format op */
    tRW_DATA evt;
    evt.status = status;
    rw_t2t_handle_op_complete();
    (*rw_cb.p_cback)(RW_T2T_FORMAT_CPLT_EVT, &evt);
  }
}

/*******************************************************************************
**
** Function         rw_t2t_update_attributes
**
** Description      This function will update attribute for the current segment
**                  based on lock and reserved bytes
**
** Returns          None
**
*******************************************************************************/
static void rw_t2t_update_attributes(void) {
  uint8_t count = 0;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t lower_offset;
  uint16_t upper_offset;
  uint16_t offset;
  uint8_t num_bytes;

  /* Prepare attr for the current segment */
  memset(p_t2t->attr, 0, RW_T2T_SEGMENT_SIZE * sizeof(uint8_t));

  /* calculate offset where the current segment starts in the tag */
  lower_offset = p_t2t->segment * RW_T2T_SEGMENT_BYTES;
  /* calculate offset where the current segment ends in the tag */
  upper_offset = (p_t2t->segment + 1) * RW_T2T_SEGMENT_BYTES;

  /* check offset of lock bytes in the tag and update p_t2t->attr
   * for every lock byte that is present in the current segment */
  count = 0;
  while (count < p_t2t->num_lockbytes) {
    offset = p_t2t->lock_tlv[p_t2t->lockbyte[count].tlv_index].offset +
             p_t2t->lockbyte[count].byte_index;
    if (offset >= lower_offset && offset < upper_offset) {
      /* Calculate offset in the current segment as p_t2t->attr is prepared for
       * one segment only */
      offset %= RW_T2T_SEGMENT_BYTES;
      /* Every bit in p_t2t->attr indicates one byte of the tag is either a
       * lock/reserved byte or not
       * So, each array element in p_t2t->attr covers two blocks in the tag as
       * T2 block size is 4 and array element size is 8
       * Set the corresponding bit in attr to indicate - reserved byte */
      p_t2t->attr[offset / TAG_BITS_PER_BYTE] |=
          rw_t2t_mask_bits[offset % TAG_BITS_PER_BYTE];
    }
    count++;
  }

  /* Search reserved bytes identified by all memory tlvs present in the tag */
  count = 0;
  while (count < p_t2t->num_mem_tlvs) {
    /* check the offset of reserved bytes in the tag and update  p_t2t->attr
     * for every  reserved byte that is present in the current segment */
    num_bytes = 0;
    while (num_bytes < p_t2t->mem_tlv[count].num_bytes) {
      offset = p_t2t->mem_tlv[count].offset + num_bytes;
      if (offset >= lower_offset && offset < upper_offset) {
        /* Let offset represents offset in the current segment as p_t2t->attr is
         * prepared for one segment only */
        offset %= RW_T2T_SEGMENT_BYTES;
        /* Every bit in p_t2t->attr indicates one byte of the tag is either a
         * lock/reserved byte or not
         * So, each array element in p_t2t->attr covers two blocks in the tag as
         * T2 block size is 4 and array element size is 8
         * Set the corresponding bit in attr to indicate - reserved byte */
        p_t2t->attr[offset / TAG_BITS_PER_BYTE] |=
            rw_t2t_mask_bits[offset % TAG_BITS_PER_BYTE];
      }
      num_bytes++;
    }
    count++;
  }
}

/*******************************************************************************
**
** Function         rw_t2t_get_lock_bits_for_segment
**
** Description      This function returns the offset of lock bits associated for
**                  the specified segment
**
** Parameters:      segment: The segment number to which lock bits are
**                           associated
**                  p_start_byte: The offset of lock byte that contains the
**                                first lock bit for the segment
**                  p_start_bit:  The offset of the lock bit in the lock byte
**
**                  p_end_byte:   The offset of the last bit associcated to the
**                                segment
**
** Returns          Total number of lock bits assigned to the specified segment
**
*******************************************************************************/
static uint8_t rw_t2t_get_lock_bits_for_segment(uint8_t segment,
                                                uint8_t* p_start_byte,
                                                uint8_t* p_start_bit,
                                                uint8_t* p_end_byte) {
  uint8_t total_bits = 0;
  uint16_t byte_count = 0;
  uint16_t lower_offset, upper_offset;
  uint8_t num_dynamic_locks = 0;
  uint8_t bit_count = 0;
  uint8_t bytes_locked_per_bit;
  uint8_t num_bits;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  bool b_all_bits_are_locks = true;
  uint16_t tag_size;
  uint8_t xx;

  tag_size = (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] * T2T_TMS_TAG_FACTOR) +
             (T2T_FIRST_DATA_BLOCK * T2T_BLOCK_SIZE) + p_t2t->num_lockbytes;

  for (xx = 0; xx < p_t2t->num_mem_tlvs; xx++)
    tag_size += p_t2t->mem_tlv[xx].num_bytes;

  lower_offset = segment * RW_T2T_SEGMENT_BYTES;
  if (segment == 0) {
    lower_offset += T2T_STATIC_SIZE;
  }
  upper_offset = (segment + 1) * RW_T2T_SEGMENT_BYTES;

  byte_count = T2T_STATIC_SIZE;
  if (tag_size < upper_offset) {
    upper_offset = tag_size;
  }

  *p_start_byte = num_dynamic_locks;
  *p_start_bit = 0;

  while ((byte_count <= lower_offset) &&
         (num_dynamic_locks < p_t2t->num_lockbytes)) {
    bytes_locked_per_bit =
        p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
            .bytes_locked_per_bit;
    /* Number of bits in the current lock byte */
    b_all_bits_are_locks =
        ((p_t2t->lockbyte[num_dynamic_locks].byte_index + 1) *
             TAG_BITS_PER_BYTE <=
         p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
             .num_bits);
    num_bits =
        b_all_bits_are_locks
            ? TAG_BITS_PER_BYTE
            : p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
                      .num_bits %
                  TAG_BITS_PER_BYTE;

    if (((bytes_locked_per_bit * num_bits) + byte_count) <= lower_offset) {
      /* Skip this lock byte as it covers different segment */
      byte_count += bytes_locked_per_bit * num_bits;
      num_dynamic_locks++;
    } else {
      bit_count = 0;
      while (bit_count < num_bits) {
        byte_count += bytes_locked_per_bit;
        if (byte_count > lower_offset) {
          /* First lock bit that is used to lock this segment */
          *p_start_byte = num_dynamic_locks;
          *p_end_byte = num_dynamic_locks;
          *p_start_bit = bit_count;
          bit_count++;
          total_bits = 1;
          break;
        }
        bit_count++;
      }
    }
  }
  if (num_dynamic_locks == p_t2t->num_lockbytes) {
    return 0;
  }
  while ((byte_count < upper_offset) &&
         (num_dynamic_locks < p_t2t->num_lockbytes)) {
    bytes_locked_per_bit =
        p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
            .bytes_locked_per_bit;
    /* Number of bits in the current lock byte */
    b_all_bits_are_locks =
        ((p_t2t->lockbyte[num_dynamic_locks].byte_index + 1) *
             TAG_BITS_PER_BYTE <=
         p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
             .num_bits);
    num_bits =
        b_all_bits_are_locks
            ? TAG_BITS_PER_BYTE
            : p_t2t->lock_tlv[p_t2t->lockbyte[num_dynamic_locks].tlv_index]
                      .num_bits %
                  TAG_BITS_PER_BYTE;

    if ((bytes_locked_per_bit * (num_bits - bit_count)) + byte_count <
        upper_offset) {
      /* Collect all lock bits that covers the current segment */
      byte_count += bytes_locked_per_bit * (num_bits - bit_count);
      total_bits += num_bits - bit_count;
      bit_count = 0;
      *p_end_byte = num_dynamic_locks;
      num_dynamic_locks++;
    } else {
      /* The last lock byte that covers the current segment */
      bit_count = 0;
      while (bit_count < num_bits) {
        /* The last lock bit that is used to lock this segment */
        byte_count += bytes_locked_per_bit;
        if (byte_count >= upper_offset) {
          *p_end_byte = num_dynamic_locks;
          total_bits += (bit_count + 1);
          break;
        }
        bit_count++;
      }
    }
  }
  return total_bits;
}

/*******************************************************************************
**
** Function         rw_t2t_update_lock_attributes
**
** Description      This function will check if the tag index passed as
**                  argument is a locked byte and return TRUE or FALSE
**
** Parameters:      index, the index of the byte in the tag
**
**
** Returns          TRUE, if the specified index in the tag is a locked or
**                        reserved or otp byte
**                  FALSE, otherwise
**
*******************************************************************************/
static void rw_t2t_update_lock_attributes(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t xx = 0;
  uint8_t num_static_lock_bytes = 0;
  uint8_t num_dyn_lock_bytes = 0;
  uint8_t bits_covered = 0;
  uint8_t bytes_covered = 0;
  uint8_t block_count = 0;
  bool b_all_bits_are_locks = true;
  uint8_t bytes_locked_per_lock_bit;
  uint8_t start_lock_byte;
  uint8_t start_lock_bit;
  uint8_t end_lock_byte;
  uint8_t num_lock_bits;
  uint8_t total_bits;

  /* Prepare lock_attr for the current segment */
  memset(p_t2t->lock_attr, 0, RW_T2T_SEGMENT_SIZE * sizeof(uint8_t));

  block_count = 0;
  if (p_t2t->segment == 0) {
    /* Update lock_attributes based on static lock bytes */
    xx = 0;
    num_static_lock_bytes = 0;
    block_count = 0;
    num_lock_bits =
        TAG_BITS_PER_BYTE - 1; /* the inner while loop increases xx by 2. need
                                  (-1) to avoid coverity overrun error */

    while (num_static_lock_bytes < T2T_NUM_STATIC_LOCK_BYTES) {
      /* Update lock attribute based on 2 static locks */
      while (xx < num_lock_bits) {
        p_t2t->lock_attr[block_count] = 0x00;

        if (p_t2t->tag_hdr[T2T_STATIC_LOCK0 + num_static_lock_bytes] &
            rw_t2t_mask_bits[xx++]) {
          /* If the bit is set then 1 block is locked */
          p_t2t->lock_attr[block_count] = 0x0F;
        }

        if (p_t2t->tag_hdr[T2T_STATIC_LOCK0 + num_static_lock_bytes] &
            rw_t2t_mask_bits[xx++]) {
          /* If the bit is set then 1 block is locked */
          p_t2t->lock_attr[block_count] |= 0xF0;
        }
        block_count++;
      }
      num_static_lock_bytes++;
      xx = 0;
    }
    /* UID is always locked, irrespective of the lock value */
    p_t2t->lock_attr[0x00] = 0xFF;
  }

  /* Get lock bits applicable for the current segment */
  total_bits = rw_t2t_get_lock_bits_for_segment(
      p_t2t->segment, &start_lock_byte, &start_lock_bit, &end_lock_byte);
  if (total_bits != 0) {
    /* update lock_attributes based on current segment using dynamic lock bytes
     */
    xx = start_lock_bit;
    num_dyn_lock_bytes = start_lock_byte;
    bits_covered = 0;
    bytes_covered = 0;
    num_lock_bits = TAG_BITS_PER_BYTE;
    p_t2t->lock_attr[block_count] = 0;

    while (num_dyn_lock_bytes <= end_lock_byte) {
      bytes_locked_per_lock_bit =
          p_t2t->lock_tlv[p_t2t->lockbyte[num_dyn_lock_bytes].tlv_index]
              .bytes_locked_per_bit;
      /* Find number of bits in the byte are lock bits */
      b_all_bits_are_locks =
          ((p_t2t->lockbyte[num_dyn_lock_bytes].byte_index + 1) *
               TAG_BITS_PER_BYTE <=
           p_t2t->lock_tlv[p_t2t->lockbyte[num_dyn_lock_bytes].tlv_index]
               .num_bits);
      num_lock_bits =
          b_all_bits_are_locks
              ? TAG_BITS_PER_BYTE
              : p_t2t->lock_tlv[p_t2t->lockbyte[num_dyn_lock_bytes].tlv_index]
                        .num_bits %
                    TAG_BITS_PER_BYTE;

      while (xx < num_lock_bits) {
        bytes_covered = 0;
        while (bytes_covered < bytes_locked_per_lock_bit) {
          if (p_t2t->lockbyte[num_dyn_lock_bytes].lock_byte &
              rw_t2t_mask_bits[xx]) {
            /* If the bit is set then it is locked */
            if (block_count < RW_T2T_SEGMENT_SIZE)
              p_t2t->lock_attr[block_count] |= 0x01 << bits_covered;
          }
          bytes_covered++;
          bits_covered++;
          if (bits_covered == TAG_BITS_PER_BYTE) {
            /* Move to next 8 bytes */
            bits_covered = 0;
            block_count++;
            /* Assume unlocked before updating using locks */
            if (block_count < RW_T2T_SEGMENT_SIZE)
              p_t2t->lock_attr[block_count] = 0;
          }
        }
        xx++;
      }
      num_dyn_lock_bytes++;
      xx = 0;
    }
  }
}

/*******************************************************************************
**
** Function         rw_t2t_is_lock_res_byte
**
** Description      This function will check if the tag index passed as
**                  argument is a lock or reserved or otp byte and return
**                  TRUE or FALSE
**
** Parameters:      index, the index of the byte in the tag
**
**
** Returns          TRUE, if the specified index in the tag is a locked or
**                        reserved or otp byte
**                  FALSE, otherwise
**
*******************************************************************************/
static bool rw_t2t_is_lock_res_byte(uint16_t index) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->segment = (uint8_t)(index / RW_T2T_SEGMENT_BYTES);

  if (p_t2t->attr_seg != p_t2t->segment) {
    /* Update attributes for the current segment */
    rw_t2t_update_attributes();
    p_t2t->attr_seg = p_t2t->segment;
  }

  index = index % RW_T2T_SEGMENT_BYTES;
  /* Every bit in p_t2t->attr indicates one specific byte of the tag is either a
   * lock/reserved byte or not
   * So, each array element in p_t2t->attr covers two blocks in the tag as T2
   * block size is 4 and array element size is 8
   * Find the block and offset for the index (passed as argument) and Check if
   * the offset bit in the
   * p_t2t->attr[block/2] is set or not. If the bit is set then it is a
   * lock/reserved byte, otherwise not */

  return ((p_t2t->attr[index / 8] & rw_t2t_mask_bits[index % 8]) == 0) ? false
                                                                       : true;
}

/*******************************************************************************
**
** Function         rw_t2t_is_read_only_byte
**
** Description      This function will check if the tag index passed as
**                  argument is a locked and return
**                  TRUE or FALSE
**
** Parameters:      index, the index of the byte in the tag
**
**
** Returns          TRUE, if the specified index in the tag is a locked or
**                        reserved or otp byte
**                  FALSE, otherwise
**
*******************************************************************************/
static bool rw_t2t_is_read_only_byte(uint16_t index) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->segment = (uint8_t)(index / RW_T2T_SEGMENT_BYTES);

  if (p_t2t->lock_attr_seg != p_t2t->segment) {
    /* Update lock attributes for the current segment */
    rw_t2t_update_lock_attributes();
    p_t2t->lock_attr_seg = p_t2t->segment;
  }

  index = index % RW_T2T_SEGMENT_BYTES;
  /* Every bit in p_t2t->lock_attr indicates one specific byte of the tag is a
   * read only byte or read write byte
   * So, each array element in p_t2t->lock_attr covers two blocks of the tag as
   * T2 block size is 4 and array element size is 8
   * Find the block and offset for the index (passed as argument) and Check if
   * the offset bit in
   * p_t2t->lock_attr[block/2] is set or not. If the bit is set then it is a
   * read only byte, otherwise read write byte */

  return ((p_t2t->lock_attr[index / 8] & rw_t2t_mask_bits[index % 8]) == 0)
             ? false
             : true;
}

/*******************************************************************************
**
** Function         rw_t2t_set_dynamic_lock_bits
**
** Description      This function will set dynamic lock bits as part of
**                  configuring tag as read only
**
** Returns
**                  NFC_STATUS_OK, Command sent to set dynamic lock bits
**                  NFC_STATUS_FAILED: otherwise
**
*******************************************************************************/
tNFC_STATUS rw_t2t_set_dynamic_lock_bits(uint8_t* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t write_block[T2T_BLOCK_SIZE];
  uint16_t offset;
  uint16_t next_offset;
  uint8_t num_bits;
  uint8_t next_num_bits;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  uint8_t num_locks;
  uint8_t lock_count;
  bool b_all_bits_are_locks = true;

  num_locks = 0;

  memcpy(write_block, p_data, T2T_BLOCK_SIZE);
  while (num_locks < p_t2t->num_lockbytes) {
    if (p_t2t->lockbyte[num_locks].lock_status == RW_T2T_LOCK_NOT_UPDATED) {
      offset = p_t2t->lock_tlv[p_t2t->lockbyte[num_locks].tlv_index].offset +
               p_t2t->lockbyte[num_locks].byte_index;

      /* Check if all bits are lock bits in the byte */
      b_all_bits_are_locks =
          ((p_t2t->lockbyte[num_locks].byte_index + 1) * TAG_BITS_PER_BYTE <=
           p_t2t->lock_tlv[p_t2t->lockbyte[num_locks].tlv_index].num_bits);
      num_bits =
          b_all_bits_are_locks
              ? TAG_BITS_PER_BYTE
              : p_t2t->lock_tlv[p_t2t->lockbyte[num_locks].tlv_index].num_bits %
                    TAG_BITS_PER_BYTE;

      write_block[(uint8_t)(offset % T2T_BLOCK_SIZE)] |=
          tags_pow(2, num_bits) - 1;
      lock_count = num_locks + 1;

      /* Set all the lock bits in the block using a sing block write command */
      while (lock_count < p_t2t->num_lockbytes) {
        next_offset =
            p_t2t->lock_tlv[p_t2t->lockbyte[lock_count].tlv_index].offset +
            p_t2t->lockbyte[lock_count].byte_index;

        /* Check if all bits are lock bits in the byte */
        b_all_bits_are_locks =
            ((p_t2t->lockbyte[lock_count].byte_index + 1) * TAG_BITS_PER_BYTE <=
             p_t2t->lock_tlv[p_t2t->lockbyte[lock_count].tlv_index].num_bits);
        next_num_bits =
            b_all_bits_are_locks
                ? TAG_BITS_PER_BYTE
                : p_t2t->lock_tlv[p_t2t->lockbyte[lock_count].tlv_index]
                          .num_bits %
                      TAG_BITS_PER_BYTE;

        if (next_offset / T2T_BLOCK_SIZE == offset / T2T_BLOCK_SIZE) {
          write_block[(uint8_t)(next_offset % T2T_BLOCK_SIZE)] |=
              tags_pow(2, next_num_bits) - 1;
        } else
          break;
        lock_count++;
      }

      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_DYN_LOCK_BITS;
      /* send WRITE command to set dynamic lock bits */
      status = rw_t2t_write((uint16_t)(offset / T2T_BLOCK_SIZE), write_block);
      if (status == NFC_STATUS_OK) {
        while (lock_count > num_locks) {
          /* Set update initiated flag to indicate a write command is sent to
           * set dynamic lock bits of the block */
          p_t2t->lockbyte[lock_count - 1].lock_status =
              RW_T2T_LOCK_UPDATE_INITIATED;
          lock_count--;
        }
      } else
        status = NFC_STATUS_FAILED;

      break;
    }
    num_locks++;
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_set_lock_tlv
**
** Description      This function will set lock control tlv on the blank
**                  activated type 2 tag based on values read from version block
**
** Parameters:      TAG data memory size
**
** Returns
**                  NFC_STATUS_OK, Command sent to set Lock TLV
**                  NFC_STATUS_FAILED: otherwise
**
*******************************************************************************/
tNFC_STATUS rw_t2t_set_lock_tlv(uint16_t addr, uint8_t num_dyn_lock_bits,
                                uint16_t locked_area_size) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  int8_t PageAddr = 0;
  int8_t BytePerPage = 0;
  int8_t ByteOffset = 0;
  uint8_t a;
  uint8_t data_block[T2T_BLOCK_SIZE];
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t* p;
  uint8_t xx;

  for (xx = 15; xx > 0; xx--) {
    a = (uint8_t)(addr / xx);
    a += (addr % xx) ? 1 : 0;

    BytePerPage = (int8_t)tags_log2(a);
    ByteOffset = (int8_t)(addr - xx * tags_pow(2, BytePerPage));

    if (ByteOffset < 16) {
      PageAddr = xx;
      break;
    }
  }

  if ((ByteOffset < 16) && (BytePerPage < 16) && (PageAddr < 16)) {
    memset(data_block, 0, T2T_BLOCK_SIZE);
    p = data_block;
    UINT8_TO_BE_STREAM(p, T2T_TLV_TYPE_LOCK_CTRL);
    UINT8_TO_BE_STREAM(p, T2T_TLEN_LOCK_CTRL_TLV);
    UINT8_TO_BE_STREAM(p, (PageAddr << 4 | ByteOffset));
    UINT8_TO_BE_STREAM(p, num_dyn_lock_bits);

    p_t2t->tlv_value[0] = PageAddr << 4 | ByteOffset;
    p_t2t->tlv_value[1] = num_dyn_lock_bits;
    p_t2t->tlv_value[2] =
        (uint8_t)(BytePerPage << 4 | tags_log2(locked_area_size));

    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_LOCK_TLV;

    /* send WRITE-E8 command */
    status = rw_t2t_write(T2T_FIRST_DATA_BLOCK, data_block);
    if (status == NFC_STATUS_OK) {
      p_t2t->b_read_data = false;
    } else
      p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  } else
    status = NFC_STATUS_REJECTED;

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_set_cc
**
** Description      This function will set Capability Container on the activated
**                  type 2 tag with default values of CC0, CC1, CC4 and
**                  specified CC3 value
**
** Parameters:      CC3 value of the tag
**
** Returns
**                  NFC_STATUS_OK, Command sent to set CC
**                  NFC_STATUS_FAILED: otherwise
**
*******************************************************************************/
tNFC_STATUS rw_t2t_set_cc(uint8_t tms) {
  uint8_t cc_block[T2T_BLOCK_SIZE];
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  uint8_t* p;

  memset(cc_block, 0, T2T_BLOCK_SIZE);
  memset(p_t2t->ndef_final_block, 0, T2T_BLOCK_SIZE);
  p = cc_block;

  /* Prepare Capability Container */
  UINT8_TO_BE_STREAM(p, T2T_CC0_NMN);
  UINT8_TO_BE_STREAM(p, T2T_CC1_VNO);
  UINT8_TO_BE_STREAM(p, tms);
  UINT8_TO_BE_STREAM(p, T2T_CC3_RWA_RW);

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_CC;

  /* send WRITE-E8 command */
  status = rw_t2t_write(T2T_CC_BLOCK, cc_block);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_FORMAT_TAG;
    p_t2t->b_read_hdr = false;
  } else
    p_t2t->substate = RW_T2T_SUBSTATE_NONE;

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_format_tag
**
** Description      This function will format tag based on Manufacturer ID
**
** Returns
**                  NFC_STATUS_OK, Command sent to format Tag
**                  NFC_STATUS_FAILED: otherwise
**
*******************************************************************************/
tNFC_STATUS rw_t2t_format_tag(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  const tT2T_INIT_TAG* p_ret;
  uint8_t tms;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  bool b_blank_tag = true;

  p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], false, 0);
  if (p_ret == nullptr) {
    LOG(WARNING) << StringPrintf(
        "rw_t2t_format_tag - Unknown Manufacturer ID: %u, Cannot Format the "
        "tag!",
        p_t2t->tag_hdr[0]);
    return (NFC_STATUS_FAILED);
  }

  if (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] != 0) {
    /* If OTP tag has valid NDEF Message, cannot format the tag */
    if ((p_t2t->ndef_msg_len > 0) && (p_ret->b_otp)) {
      LOG(WARNING) << StringPrintf(
          "rw_t2t_format_tag - Cannot Format a OTP tag with NDEF Message!");
      return (NFC_STATUS_FAILED);
    }

    if (((p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] != 0) &&
         (p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] != T2T_CC0_NMN)) ||
        ((p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != 0) &&
         (p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_LEGACY_VNO) &&
         (p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_VNO) &&
         (p_t2t->tag_hdr[T2T_CC1_VNO_BYTE] != T2T_CC1_NEW_VNO))) {
      LOG(WARNING) << StringPrintf(
          "rw_t2t_format_tag - Tag not blank to Format!");
      return (NFC_STATUS_FAILED);
    } else {
      tms = p_t2t->tag_hdr[T2T_CC2_TMS_BYTE];
      b_blank_tag = false;
    }
  } else
    tms = p_ret->tms;

  memset(p_t2t->tag_data, 0, T2T_READ_DATA_LEN);

  if (!b_blank_tag || !p_ret->b_multi_version) {
    status = rw_t2t_set_cc(tms);
  } else if (p_ret->version_block != 0) {
    /* If Version number is not read, READ it now */
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_VERSION_INFO;

    status = rw_t2t_read(p_ret->version_block);
    if (status == NFC_STATUS_OK)
      p_t2t->state = RW_T2T_STATE_FORMAT_TAG;
    else
      p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  } else {
    /* UID block is the version block */
    p_t2t->state = RW_T2T_STATE_FORMAT_TAG;
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_VERSION_INFO;
    rw_t2t_handle_format_tag_rsp(p_t2t->tag_hdr);
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_soft_lock_tag
**
** Description      This function will soft lock the tag after validating CC.
**
** Returns
**                  NFC_STATUS_OK, Command sent to soft lock the tag
**                  NFC_STATUS_FAILED: otherwise
**
*******************************************************************************/
tNFC_STATUS rw_t2t_soft_lock_tag(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  uint8_t write_block[T2T_BLOCK_SIZE];
  uint8_t num_locks;

  /* If CC block is read and cc3 is soft locked, reject the command */
  if ((p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] & T2T_CC3_RWA_RO) == T2T_CC3_RWA_RO) {
    LOG(ERROR) << StringPrintf(
        "rw_t2t_soft_lock_tag: Error: Type 2 tag is in Read only state, CC3: "
        "%u",
        p_t2t->tag_hdr[T2T_CC3_RWA_BYTE]);
    return (NFC_STATUS_FAILED);
  }

  if (p_t2t->b_hard_lock) {
    /* Should have performed NDEF Detection on dynamic memory structure tag,
     * before permanently converting to Read only
     * Even when no lock control tlv is present, default lock bytes should be
     * present */

    if ((p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] != T2T_CC2_TMS_STATIC) &&
        (p_t2t->num_lockbytes == 0)) {
      LOG(ERROR) << StringPrintf(
          "rw_t2t_soft_lock_tag: Error: Lock TLV not detected! Cannot hard "
          "lock the tag");
      return (NFC_STATUS_FAILED);
    }

    /* On dynamic memory structure tag, reset all lock bytes status to 'Not
     * Updated' if not in Updated status */
    num_locks = 0;
    while (num_locks < p_t2t->num_lockbytes) {
      if (p_t2t->lockbyte[num_locks].lock_status != RW_T2T_LOCK_UPDATED)
        p_t2t->lockbyte[num_locks].lock_status = RW_T2T_LOCK_NOT_UPDATED;
      num_locks++;
    }
  }

  memcpy(write_block, &p_t2t->tag_hdr[T2T_CC0_NMN_BYTE], T2T_BLOCK_SIZE);
  write_block[(T2T_CC3_RWA_BYTE % T2T_BLOCK_SIZE)] = T2T_CC3_RWA_RO;

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SET_CC_RO;
  /* First Soft lock the tag */
  status = rw_t2t_write(T2T_CC_BLOCK, write_block);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_SET_TAG_RO;
    p_t2t->b_read_hdr = false;
  } else {
    p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  }
  return status;
}

/*****************************************************************************
**
** Function         RW_T2tFormatNDef
**
** Description
**      Format Tag content
**
** Returns
**      NFC_STATUS_OK, Command sent to format Tag
**      NFC_STATUS_FAILED: otherwise
**
*****************************************************************************/
tNFC_STATUS RW_T2tFormatNDef(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf(
        "RW_T2tFormatNDef - Tag not initialized/ Busy! State: %u",
        p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  if (!p_t2t->b_read_hdr) {
    /* If UID is not read, READ it now */
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_CC;

    status = rw_t2t_read(0);
    if (status == NFC_STATUS_OK)
      p_t2t->state = RW_T2T_STATE_FORMAT_TAG;
    else
      p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  } else {
    status = rw_t2t_format_tag();
    if (status != NFC_STATUS_OK) p_t2t->b_read_hdr = false;
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T2tLocateTlv
**
** Description      This function is used to perform TLV detection on a Type 2
**                  tag, and retrieve the tag's TLV attribute information.
**
**                  Before using this API, the application must call
**                  RW_SelectTagType to indicate that a Type 2 tag has been
**                  activated.
**
** Parameters:      tlv_type : TLV to detect
**
** Returns          NCI_STATUS_OK, if detection was started. Otherwise, error
**                  status.
**
*******************************************************************************/
tNFC_STATUS RW_T2tLocateTlv(uint8_t tlv_type) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;
  uint16_t block;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_BUSY);
  }

  if ((tlv_type != TAG_LOCK_CTRL_TLV) && (tlv_type != TAG_MEM_CTRL_TLV) &&
      (tlv_type != TAG_NDEF_TLV) && (tlv_type != TAG_PROPRIETARY_TLV)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "RW_T2tLocateTlv - Cannot search TLV: 0x%02x", tlv_type);
    return (NFC_STATUS_FAILED);
  }

  if ((tlv_type == TAG_LOCK_CTRL_TLV) && (p_t2t->b_read_hdr) &&
      (p_t2t->tag_hdr[T2T_CC2_TMS_BYTE] == T2T_CC2_TMS_STATIC)) {
    p_t2t->b_read_hdr = false;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "RW_T2tLocateTlv - No Lock tlv in static structure tag, CC[0]: 0x%02x",
        p_t2t->tag_hdr[T2T_CC2_TMS_BYTE]);
    return (NFC_STATUS_FAILED);
  }

  if ((tlv_type == TAG_NDEF_TLV) && (p_t2t->b_read_hdr) &&
      (p_t2t->tag_hdr[T2T_CC0_NMN_BYTE] != T2T_CC0_NMN)) {
    p_t2t->b_read_hdr = false;
    LOG(WARNING) << StringPrintf(
        "RW_T2tLocateTlv - Invalid NDEF Magic Number!, CC[0]: 0x%02x, CC[1]: "
        "0x%02x, CC[3]: 0x%02x",
        p_t2t->tag_hdr[T2T_CC0_NMN_BYTE], p_t2t->tag_hdr[T2T_CC1_VNO_BYTE],
        p_t2t->tag_hdr[T2T_CC3_RWA_BYTE]);
    return (NFC_STATUS_FAILED);
  }

  p_t2t->work_offset = 0;
  p_t2t->tlv_detect = tlv_type;

  /* Reset control block variables based on type of tlv to detect */
  if (tlv_type == TAG_LOCK_CTRL_TLV) {
    p_t2t->num_lockbytes = 0;
    p_t2t->num_lock_tlvs = 0;
  } else if (tlv_type == TAG_MEM_CTRL_TLV) {
    p_t2t->num_mem_tlvs = 0;
  } else if (tlv_type == TAG_NDEF_TLV) {
    p_t2t->ndef_msg_offset = 0;
    p_t2t->num_lockbytes = 0;
    p_t2t->num_lock_tlvs = 0;
    p_t2t->num_mem_tlvs = 0;
    p_t2t->ndef_msg_len = 0;
    p_t2t->ndef_status = T2T_NDEF_NOT_DETECTED;
  } else {
    p_t2t->prop_msg_len = 0;
  }

  if (!p_t2t->b_read_hdr) {
    /* First read CC block */
    block = 0;
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_CC;
  } else {
    /* Read first data block */
    block = T2T_FIRST_DATA_BLOCK;
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_TLV_DETECT;
  }

  /* Start reading tag, looking for the specified TLV */
  status = rw_t2t_read((uint16_t)block);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_DETECT_TLV;
  } else {
    p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  }
  return (status);
}

/*******************************************************************************
**
** Function         RW_T2tDetectNDef
**
** Description      This function is used to perform NDEF detection on a Type 2
**                  tag, and retrieve the tag's NDEF attribute information.
**
**                  Before using this API, the application must call
**                  RW_SelectTagType to indicate that a Type 2 tag has been
**                  activated.
**
** Parameters:      none
**
** Returns          NCI_STATUS_OK,if detect op started.Otherwise,error status.
**
*******************************************************************************/
tNFC_STATUS RW_T2tDetectNDef(bool skip_dyn_locks) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->skip_dyn_locks = skip_dyn_locks;

  return RW_T2tLocateTlv(TAG_NDEF_TLV);
}

/*******************************************************************************
**
** Function         RW_T2tReadNDef
**
** Description      Retrieve NDEF contents from a Type2 tag.
**
**                  The RW_T2T_NDEF_READ_EVT event is used to notify the
**                  application after reading the NDEF message.
**
**                  Before using this API, the RW_T2tDetectNDef function must
**                  be called to verify that the tag contains NDEF data, and to
**                  retrieve the NDEF attributes.
**
**                  Internally, this command will be separated into multiple
**                  Tag2 Read commands (if necessary) - depending on the NDEF
**                  Msg size
**
** Parameters:      p_buffer:   The buffer into which to read the NDEF message
**                  buf_len:    The length of the buffer
**
** Returns          NCI_STATUS_OK, if read was started. Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS RW_T2tReadNDef(uint8_t* p_buffer, uint16_t buf_len) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status = NFC_STATUS_OK;
  uint16_t block;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  if (p_t2t->ndef_status == T2T_NDEF_NOT_DETECTED) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tReadNDef - Error: NDEF detection not performed yet");
    return (NFC_STATUS_FAILED);
  }

  if (buf_len < p_t2t->ndef_msg_len) {
    LOG(WARNING) << StringPrintf(
        "RW_T2tReadNDef - buffer size: %u  less than NDEF msg sise: %u",
        buf_len, p_t2t->ndef_msg_len);
    return (NFC_STATUS_FAILED);
  }

  if (!p_t2t->ndef_msg_len) {
    LOG(WARNING) << StringPrintf(
        "RW_T2tReadNDef - NDEF Message length is zero");
    return (NFC_STATUS_NOT_INITIALIZED);
  }

  p_t2t->p_ndef_buffer = p_buffer;
  p_t2t->work_offset = 0;

  block = (uint16_t)(p_t2t->ndef_msg_offset / T2T_BLOCK_LEN);
  block -= block % T2T_READ_BLOCKS;

  p_t2t->substate = RW_T2T_SUBSTATE_NONE;

  if ((block == T2T_FIRST_DATA_BLOCK) && (p_t2t->b_read_data)) {
    p_t2t->state = RW_T2T_STATE_READ_NDEF;
    p_t2t->block_read = T2T_FIRST_DATA_BLOCK;
    rw_t2t_handle_ndef_read_rsp(p_t2t->tag_data);
  } else {
    /* Start reading NDEF Message */
    status = rw_t2t_read(block);
    if (status == NFC_STATUS_OK) {
      p_t2t->state = RW_T2T_STATE_READ_NDEF;
    }
  }

  return (status);
}

/*******************************************************************************
**
** Function         RW_T2tWriteNDef
**
** Description      Write NDEF contents to a Type2 tag.
**
**                  Before using this API, the RW_T2tDetectNDef
**                  function must be called to verify that the tag contains
**                  NDEF data, and to retrieve the NDEF attributes.
**
**                  The RW_T2T_NDEF_WRITE_EVT callback event will be used to
**                  notify the application of the response.
**
**                  Internally, this command will be separated into multiple
**                  Tag2 Write commands (if necessary) - depending on the NDEF
**                  Msg size
**
** Parameters:      msg_len:    The length of the buffer
**                  p_msg:      The NDEF message to write
**
** Returns          NCI_STATUS_OK,if write was started. Otherwise, error status
**
*******************************************************************************/
tNFC_STATUS RW_T2tWriteNDef(uint16_t msg_len, uint8_t* p_msg) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint16_t block;
  const tT2T_INIT_TAG* p_ret;

  tNFC_STATUS status = NFC_STATUS_OK;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  if (p_t2t->ndef_status == T2T_NDEF_NOT_DETECTED) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tWriteNDef - Error: NDEF detection not performed!");
    return (NFC_STATUS_FAILED);
  }

  if (p_t2t->tag_hdr[T2T_CC3_RWA_BYTE] != T2T_CC3_RWA_RW) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tWriteNDef - Write access not granted - CC3: %u",
        p_t2t->tag_hdr[T2T_CC3_RWA_BYTE]);
    return (NFC_STATUS_REFUSED);
  }

  /* Check if there is enough memory on the tag */
  if (msg_len > p_t2t->max_ndef_msg_len) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tWriteNDef - Cannot write NDEF of size greater than %u bytes",
        p_t2t->max_ndef_msg_len);
    return (NFC_STATUS_FAILED);
  }

  /* If OTP tag and tag has valid NDEF Message, stop writting new NDEF Message
   * as it may corrupt the tag */
  if ((p_t2t->ndef_msg_len > 0) &&
      ((p_ret = t2t_tag_init_data(p_t2t->tag_hdr[0], false, 0)) != nullptr) &&
      (p_ret->b_otp)) {
    LOG(WARNING) << StringPrintf(
        "RW_T2tWriteNDef - Cannot Overwrite NDEF Message on a OTP tag!");
    return (NFC_STATUS_FAILED);
  }
  p_t2t->p_new_ndef_buffer = p_msg;
  p_t2t->new_ndef_msg_len = msg_len;
  p_t2t->work_offset = 0;

  p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_NDEF_FIRST_BLOCK;
  /* Read first NDEF Block before updating NDEF */

  block = (uint16_t)(p_t2t->ndef_header_offset / T2T_BLOCK_LEN);

  if ((block < (T2T_FIRST_DATA_BLOCK + T2T_READ_BLOCKS)) &&
      (p_t2t->b_read_data)) {
    p_t2t->state = RW_T2T_STATE_WRITE_NDEF;
    p_t2t->block_read = block;
    rw_t2t_handle_ndef_write_rsp(
        &p_t2t->tag_data[(block - T2T_FIRST_DATA_BLOCK) * T2T_BLOCK_LEN]);
  } else {
    status = rw_t2t_read(block);
    if (status == NFC_STATUS_OK)
      p_t2t->state = RW_T2T_STATE_WRITE_NDEF;
    else
      p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_T2tSetTagReadOnly
**
** Description      This function can be called to set T2 tag as read only.
**
** Parameters:      b_hard_lock:   To indicate hard lock the tag or not
**
** Returns          NCI_STATUS_OK, if setting tag as read only was started.
**                  Otherwise, error status.
**
*******************************************************************************/
tNFC_STATUS RW_T2tSetTagReadOnly(bool b_hard_lock) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tSetTagReadOnly: Error: Type 2 tag not activated or Busy - "
        "State: %u",
        p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  p_t2t->b_hard_lock = b_hard_lock;

  if (!p_t2t->b_read_hdr) {
    /* Read CC block before configuring tag as Read only */
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_READ_CC;
    status = rw_t2t_read((uint16_t)0);
    if (status == NFC_STATUS_OK) {
      p_t2t->state = RW_T2T_STATE_SET_TAG_RO;
    } else
      p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  } else {
    status = rw_t2t_soft_lock_tag();
    if (status != NFC_STATUS_OK) p_t2t->b_read_hdr = false;
  }

  return status;
}

#endif /* (RW_NDEF_INCLUDED == TRUE) */
