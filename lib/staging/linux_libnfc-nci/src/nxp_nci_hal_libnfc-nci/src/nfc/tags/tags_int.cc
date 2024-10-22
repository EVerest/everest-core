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
 *  This file contains the common data types shared by Reader/Writer mode
 *  and Card Emulation.
 *
 ******************************************************************************/
#include "bt_types.h"
#include "nfc_target.h"

#include "rw_api.h"
#include "rw_int.h"

#define T1T_MAX_NUM_OPCODES 9
#define T1T_STATIC_OPCODES 5
#define T1T_MAX_TAG_MODELS 2

const tT1T_CMD_RSP_INFO t1t_cmd_rsp_infos[] = {
    /* Note: the order of these commands can not be changed.
     * If new events are added, add them after T1T_CMD_WRITE_NE8 */
    /*   opcode         cmd_len,  uid_offset,  rsp_len */
    {T1T_CMD_RID, 7, 3, 6},        {T1T_CMD_RALL, 7, 3, 122},
    {T1T_CMD_READ, 7, 3, 2},       {T1T_CMD_WRITE_E, 7, 3, 2},
    {T1T_CMD_WRITE_NE, 7, 3, 2},   {T1T_CMD_RSEG, 14, 10, 129},
    {T1T_CMD_READ8, 14, 10, 9},    {T1T_CMD_WRITE_E8, 14, 10, 9},
    {T1T_CMD_WRITE_NE8, 14, 10, 9}};

const tT1T_INIT_TAG t1t_init_content[] = {
    /*  Tag Name            CC3,        is dynamic, ltv[0]  ltv[1]  ltv[2]
       mtv[0]  mtv[1]  mtv[2]*/
    {RW_T1T_IS_TOPAZ96, 0x0E, FALSE, {0, 0, 0}, {0, 0, 0}},
    {RW_T1T_IS_TOPAZ512, 0x3F, TRUE, {0xF2, 0x30, 0x33}, {0xF0, 0x02, 0x03}}};

#define T2T_MAX_NUM_OPCODES 3
#define T2T_MAX_TAG_MODELS 7

const tT2T_CMD_RSP_INFO t2t_cmd_rsp_infos[] = {
    /* Note: the order of these commands can not be changed.
     * If new events are added, add them after T2T_CMD_SEC_SEL */
    /*  opcode            cmd_len,   rsp_len, nack_rsp_len */
    {T2T_CMD_READ, 2, 16, 1},
    {T2T_CMD_WRITE, 6, 1, 1},
    {T2T_CMD_SEC_SEL, 2, 1, 1}};

const tT2T_INIT_TAG t2t_init_content[] = {
    /*  Tag Name        is_multi_v  Ver Block                   Ver No
       Vbitmask   to_calc_cc CC3      OTP     BLPB */
    {TAG_MIFARE_MID, true, T2T_MIFARE_VERSION_BLOCK,
     T2T_MIFARE_ULTRALIGHT_VER_NO, 0xFFFF, false, 0x06, false,
     T2T_DEFAULT_LOCK_BLPB},
    {TAG_MIFARE_MID, true, T2T_MIFARE_VERSION_BLOCK,
     T2T_MIFARE_ULTRALIGHT_FAMILY_VER_NO, 0xFFFF, true, 0x00, false,
     T2T_DEFAULT_LOCK_BLPB},
    {TAG_KOVIO_MID, false, 0x00, 0x00, 0x0000, false, 0x1D, true, 0x04},
    {TAG_INFINEON_MID, true, T2T_INFINEON_VERSION_BLOCK,
     T2T_INFINEON_MYD_MOVE_LEAN, 0xFFF0, false, 0x06, false,
     T2T_DEFAULT_LOCK_BLPB},
    {TAG_INFINEON_MID, true, T2T_INFINEON_VERSION_BLOCK, T2T_INFINEON_MYD_MOVE,
     0xFFF0, false, 0x10, false, T2T_DEFAULT_LOCK_BLPB},
    {TAG_BRCM_MID, true, T2T_BRCM_VERSION_BLOCK, T2T_BRCM_STATIC_MEM, 0xFFFF,
     false, 0x06, false, T2T_DEFAULT_LOCK_BLPB},
    {TAG_BRCM_MID, true, T2T_BRCM_VERSION_BLOCK, T2T_BRCM_DYNAMIC_MEM, 0xFFFF,
     false, 0x3C, false, T2T_DEFAULT_LOCK_BLPB}

};

const uint8_t t4t_v10_ndef_tag_aid[T4T_V10_NDEF_TAG_AID_LEN] = {
    0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x00};
const uint8_t t4t_v20_ndef_tag_aid[T4T_V20_NDEF_TAG_AID_LEN] = {
    0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

const char* const t1t_cmd_str[] = {
    "T1T_RID",  "T1T_RALL",  "T1T_READ",     "T1T_WRITE_E",  "T1T_WRITE_NE",
    "T1T_RSEG", "T1T_READ8", "T1T_WRITE_E8", "T1T_WRITE_NE8"};

const char* const t2t_cmd_str[] = {"T2T_CMD_READ", "T2T_CMD_WRITE",
                                   "T2T_CMD_SEC_SEL"};

static unsigned int tags_ones32(unsigned int x);

/*******************************************************************************
**
** Function         t1t_cmd_to_rsp_info
**
** Description      This function maps the given opcode to tT1T_CMD_RSP_INFO.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
const tT1T_CMD_RSP_INFO* t1t_cmd_to_rsp_info(uint8_t opcode) {
  const tT1T_CMD_RSP_INFO *p_ret = nullptr, *p;
  int xx;

  for (xx = 0, p = &t1t_cmd_rsp_infos[0]; xx < T1T_MAX_NUM_OPCODES; xx++, p++) {
    if (opcode == p->opcode) {
      if ((xx < T1T_STATIC_OPCODES) || (rw_cb.tcb.t1t.hr[0] != T1T_STATIC_HR0))
        p_ret = p;
      break;
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         t1t_tag_init_data
**
** Description      This function maps the given opcode to tT1T_INIT_TAG.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
const tT1T_INIT_TAG* t1t_tag_init_data(uint8_t tag_model) {
  const tT1T_INIT_TAG *p_ret = nullptr, *p;
  int xx;

  for (xx = 0, p = &t1t_init_content[0]; xx < T1T_MAX_TAG_MODELS; xx++, p++) {
    if (tag_model == p->tag_model) {
      p_ret = p;
      break;
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         t2t_tag_init_data
**
** Description      This function maps the given manufacturer id and version to
**                  tT2T_INIT_TAG.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
const tT2T_INIT_TAG* t2t_tag_init_data(uint8_t manufacturer_id,
                                       bool b_valid_ver, uint16_t version_no) {
  const tT2T_INIT_TAG *p_ret = nullptr, *p;
  int xx;

  for (xx = 0, p = &t2t_init_content[0]; xx < T2T_MAX_TAG_MODELS; xx++, p++) {
    if (manufacturer_id == p->manufacturer_id) {
      if ((!p->b_multi_version) || (!b_valid_ver) ||
          (p->version_no == (version_no & p->version_bmask))) {
        p_ret = p;
        break;
      }
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         t2t_cmd_to_rsp_info
**
** Description      This function maps the given opcode to tT2T_CMD_RSP_INFO.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
const tT2T_CMD_RSP_INFO* t2t_cmd_to_rsp_info(uint8_t opcode) {
  const tT2T_CMD_RSP_INFO *p_ret = nullptr, *p;
  int xx;

  for (xx = 0, p = &t2t_cmd_rsp_infos[0]; xx < T2T_MAX_NUM_OPCODES; xx++, p++) {
    if (opcode == p->opcode) {
      p_ret = p;
      break;
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         t1t_info_to_evt
**
** Description      This function maps the given tT1T_CMD_RSP_INFO to RW/CE
**                  event code
**
** Returns          RW/CE event code
**
*******************************************************************************/
uint8_t t1t_info_to_evt(const tT1T_CMD_RSP_INFO* p_info) {
  return ((uint8_t)(p_info - t1t_cmd_rsp_infos) + RW_T1T_FIRST_EVT);
}

/*******************************************************************************
**
** Function         t2t_info_to_evt
**
** Description      This function maps the given tT2T_CMD_RSP_INFO to RW/CE
**                  event code
**
** Returns          RW/CE event code
**
*******************************************************************************/
uint8_t t2t_info_to_evt(const tT2T_CMD_RSP_INFO* p_info) {
  return ((uint8_t)(p_info - t2t_cmd_rsp_infos) + RW_T2T_FIRST_EVT);
}

/*******************************************************************************
**
** Function         t1t_info_to_str
**
** Description      This function maps the given tT1T_CMD_RSP_INFO to T1T cmd
**                  str
**
** Returns          T1T cmd str
**
*******************************************************************************/
const char* t1t_info_to_str(const tT1T_CMD_RSP_INFO* p_info) {
  int ind = (int)(p_info - t1t_cmd_rsp_infos);
  if (ind < T1T_MAX_NUM_OPCODES)
    return (const char*)t1t_cmd_str[ind];
  else
    return "";
}

/*******************************************************************************
**
** Function         t2t_info_to_str
**
** Description      This function maps the given tT2T_CMD_RSP_INFO to T2T cmd
**                  str
**
** Returns          T2T cmd str
**
*******************************************************************************/
const char* t2t_info_to_str(const tT2T_CMD_RSP_INFO* p_info) {
  int ind = (int)(p_info - t2t_cmd_rsp_infos);
  if (ind < T2T_MAX_NUM_OPCODES)
    return (const char*)t2t_cmd_str[ind];
  else
    return "";
}

/*******************************************************************************
**
** Function         tags_pow
**
** Description      This function calculates x(base) power of y.
**
** Returns          int
**
*******************************************************************************/
int tags_pow(int x, int y) {
  int i, ret = 1;
  for (i = 0; i < y; i++) {
    ret *= x;
  }
  return ret;
}

/*******************************************************************************
**
** Function         ones32
**
** Description      This function returns number of bits set in an unsigned
**                  integer variable
**
** Returns          int
**
*******************************************************************************/
static unsigned int tags_ones32(unsigned int x) {
  /* 32-bit recursive reduction using SWAR...
     but first step is mapping 2-bit values
     into sum of 2 1-bit values in sneaky way
  */
  x -= ((x >> 1) & 0x55555555);
  x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
  x = (((x >> 4) + x) & 0x0f0f0f0f);
  x += (x >> 8);
  x += (x >> 16);
  return (x & 0x0000003f);
}

/*******************************************************************************
**
** Function         tags_log2
**
** Description      This function calculates log to the base  2.
**
** Returns          int
**
*******************************************************************************/
unsigned int tags_log2(unsigned int x) {
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);

  return (tags_ones32(x) - 1);
}
