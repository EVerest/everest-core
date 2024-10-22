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
 *  This file contains the common data types shared by Reader/Writer mode
 *  and Card Emulation.
 *
 ******************************************************************************/

#ifndef TAGS_INT_H
#define TAGS_INT_H

/******************************************************************************
// T1T command and response definitions
******************************************************************************/

typedef struct {
  uint8_t opcode;
  uint8_t cmd_len;
  uint8_t uid_offset;
  uint8_t rsp_len;
} tT1T_CMD_RSP_INFO;

typedef struct {
  uint8_t tag_model;
  uint8_t tms;
  uint8_t b_dynamic;
  uint8_t lock_tlv[3];
  uint8_t mem_tlv[3];
} tT1T_INIT_TAG;

typedef struct {
  uint8_t manufacturer_id;
  bool b_multi_version;
  uint8_t version_block;
  uint16_t version_no;
  uint16_t version_bmask;
  uint8_t b_calc_cc;
  uint8_t tms;
  bool b_otp;
  uint8_t default_lock_blpb;
} tT2T_INIT_TAG;

typedef struct {
  uint8_t opcode;
  uint8_t cmd_len;
  uint8_t rsp_len;
  uint8_t nack_rsp_len;
} tT2T_CMD_RSP_INFO;

extern const uint8_t
    t4t_v10_ndef_tag_aid[]; /* V1.0 Type 4 Tag Applicaiton ID */
extern const uint8_t
    t4t_v20_ndef_tag_aid[]; /* V2.0 Type 4 Tag Applicaiton ID */

extern const tT1T_CMD_RSP_INFO t1t_cmd_rsp_infos[];
extern const tT1T_INIT_TAG t1t_init_content[];
extern const tT1T_CMD_RSP_INFO* t1t_cmd_to_rsp_info(uint8_t opcode);
extern const tT1T_INIT_TAG* t1t_tag_init_data(uint8_t tag_model);
extern uint8_t t1t_info_to_evt(const tT1T_CMD_RSP_INFO* p_info);

extern const tT2T_INIT_TAG* t2t_tag_init_data(uint8_t manufacturer_id,
                                              bool b_valid_ver,
                                              uint16_t version_no);
extern const tT2T_CMD_RSP_INFO t2t_cmd_rsp_infos[];
extern const tT2T_CMD_RSP_INFO* t2t_cmd_to_rsp_info(uint8_t opcode);
extern uint8_t t2t_info_to_evt(const tT2T_CMD_RSP_INFO* p_info);

extern const char* t1t_info_to_str(const tT1T_CMD_RSP_INFO* p_info);
extern const char* t2t_info_to_str(const tT2T_CMD_RSP_INFO* p_info);
extern int tags_pow(int x, int y);
extern unsigned int tags_log2(unsigned int x);

#endif /* TAGS_INT_H */
