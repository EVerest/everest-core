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
 *  This file contains the Near Field Communication (NFC) Reader/Writer mode
 *  related internal function / definitions.
 *
 ******************************************************************************/

#ifndef RW_INT_H_
#define RW_INT_H_

#include "rw_api.h"
#include "tags_defs.h"
#include "tags_int.h"

/* Proprietary definitions for HR0 and HR1 */
/* TOPAZ96 Tag                                              */
#define RW_T1T_IS_TOPAZ96 0x11
/* TOPAZ512 Tag                                             */
#define RW_T1T_IS_TOPAZ512 0x12
/* Supports dynamic commands on static tag if HR1 > 0x49    */
#define RW_T1T_HR1_MIN 0x49

/* Maximum supported Memory control TLVS in the tag         */
#define RW_T1T_MAX_MEM_TLVS 0x05
/* Maximum supported Lock control TLVS in the tag           */
#define RW_T1T_MAX_LOCK_TLVS 0x05
/* Maximum supported dynamic lock bytes                     */
#define RW_T1T_MAX_LOCK_BYTES 0x1E

/* State of the Tag as interpreted by RW */
/* TAG State is unknown to RW                               */
#define RW_T1_TAG_ATTRB_UNKNOWN 0x00
/* TAG is in INITIALIZED state                              */
#define RW_T1_TAG_ATTRB_INITIALIZED 0x01
/* TAG is in INITIALIZED state and has NDEF tlv with len=0  */
#define RW_T1_TAG_ATTRB_INITIALIZED_NDEF 0x02
/* TAG is in READ ONLY state                                */
#define RW_T1_TAG_ATTRB_READ_ONLY 0x03
/* TAG is in READ WRITE state                               */
#define RW_T1_TAG_ATTRB_READ_WRITE 0x04

/* Lock not yet set as part of SET TAG RO op                */
#define RW_T1T_LOCK_NOT_UPDATED 0x00
/* Sent command to set the Lock bytes                       */
#define RW_T1T_LOCK_UPDATE_INITIATED 0x01
/* Lock bytes are set                                       */
#define RW_T1T_LOCK_UPDATED 0x02
typedef uint8_t tRW_T1T_LOCK_STATUS;

/* States */
/* Tag not activated and or response not received for RID   */
#define RW_T1T_STATE_NOT_ACTIVATED 0x00
/* T1 Tag activated and ready to perform rw operation on Tag*/
#define RW_T1T_STATE_IDLE 0x01
/* waiting rsp for read command sent to tag                 */
#define RW_T1T_STATE_READ 0x02
/* waiting rsp for write command sent to tag                */
#define RW_T1T_STATE_WRITE 0x03
/* performing TLV detection procedure                       */
#define RW_T1T_STATE_TLV_DETECT 0x04
/* performing read NDEF procedure                           */
#define RW_T1T_STATE_READ_NDEF 0x05
/* performing update NDEF procedure                         */
#define RW_T1T_STATE_WRITE_NDEF 0x06
/* Setting Tag as read only tag                             */
#define RW_T1T_STATE_SET_TAG_RO 0x07
/* Check if Tag is still present                            */
#define RW_T1T_STATE_CHECK_PRESENCE 0x08
/* Format T1 Tag                                            */
#define RW_T1T_STATE_FORMAT_TAG 0x09

/* Sub states */
/* Default substate                                         */
#define RW_T1T_SUBSTATE_NONE 0x00

/* Sub states in RW_T1T_STATE_TLV_DETECT state */
/* waiting for the detection of a tlv in a tag              */
#define RW_T1T_SUBSTATE_WAIT_TLV_DETECT 0x01
/* waiting for finding the len field is 1 or 3 bytes long   */
#define RW_T1T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN 0x02
/* waiting for extracting len field value                   */
#define RW_T1T_SUBSTATE_WAIT_READ_TLV_LEN0 0x03
/* waiting for extracting len field value                   */
#define RW_T1T_SUBSTATE_WAIT_READ_TLV_LEN1 0x04
/* waiting for extracting value field in the TLV            */
#define RW_T1T_SUBSTATE_WAIT_READ_TLV_VALUE 0x05
/* waiting for reading dynamic locks in the TLV             */
#define RW_T1T_SUBSTATE_WAIT_READ_LOCKS 0x06

/* Sub states in RW_T1T_STATE_WRITE_NDEF state */
/* waiting for response of reading a block that will be partially updated */
#define RW_T1T_SUBSTATE_WAIT_READ_NDEF_BLOCK 0x07
/* waiting for response of invalidating NDEF Msg                          */
#define RW_T1T_SUBSTATE_WAIT_INVALIDATE_NDEF 0x08
/* waiting for response of writing a part of NDEF Msg                     */
#define RW_T1T_SUBSTATE_WAIT_NDEF_WRITE 0x09
/* waiting for response of writing last part of NDEF Msg                  */
#define RW_T1T_SUBSTATE_WAIT_NDEF_UPDATED 0x0A
/* waiting for response of validating NDEF Msg                            */
#define RW_T1T_SUBSTATE_WAIT_VALIDATE_NDEF 0x0B

/* Sub states in RW_T1T_STATE_SET_TAG_RO state */
/* waiting for response of setting CC-RWA to read only      */
#define RW_T1T_SUBSTATE_WAIT_SET_CC_RWA_RO 0x0C
/* waiting for response of setting all static lock bits     */
#define RW_T1T_SUBSTATE_WAIT_SET_ST_LOCK_BITS 0x0D
/* waiting for response of setting all dynamic lock bits    */
#define RW_T1T_SUBSTATE_WAIT_SET_DYN_LOCK_BITS 0x0E

/* Sub states in RW_T1T_STATE_FORMAT_TAG state */
/* waiting for response to format/set capability container  */
#define RW_T1T_SUBSTATE_WAIT_SET_CC 0x0F
/* waiting for response to format/set NULL NDEF             */
#define RW_T1T_SUBSTATE_WAIT_SET_NULL_NDEF 0x10

typedef struct {
  uint16_t offset;  /* Offset of the lock byte in the Tag                   */
  uint8_t num_bits; /* Number of lock bits in the lock byte                 */
  uint8_t bytes_locked_per_bit; /* No. of tag bytes gets locked by a bit in this
                                   byte   */
} tRW_T1T_LOCK_INFO;

typedef struct {
  uint16_t offset;   /* Reserved bytes offset taken from Memory control TLV  */
  uint8_t num_bytes; /* Number of reserved bytes as per the TLV              */
} tRW_T1T_RES_INFO;

typedef struct {
  uint8_t tlv_index;  /* Index of Lock control tlv that points to this address*/
  uint8_t byte_index; /* Index of Lock byte pointed by the TLV                */
  uint8_t lock_byte;  /* Value in the lock byte                               */
  tRW_T1T_LOCK_STATUS
      lock_status;  /* Indicates if it is modifed to set tag as Read only   */
  bool b_lock_read; /* Is the lock byte is already read from tag            */
} tRW_T1T_LOCK;

typedef struct {
  uint8_t addr;    /* ADD/ADD8/ADDS field value                            */
  uint8_t op_code; /* Command sent                                         */
  uint8_t rsp_len; /* expected length of the response                      */
  uint8_t
      pend_retx_rsp; /* Number of pending rsps to retransmission on prev cmd */
} tRW_T1T_PREV_CMD_RSP_INFO;

#if (RW_NDEF_INCLUDED == TRUE)
/* Buffer 0-E block, for easier tlv operation           */
#define T1T_BUFFER_SIZE T1T_STATIC_SIZE
#else
/* Buffer UID                                           */
#define T1T_BUFFER_SIZE T1T_UID_LEN
#endif

/* RW Type 1 Tag control blocks */
typedef struct {
  uint8_t
      hr[T1T_HR_LEN]; /* Header ROM byte 0 - 0x1y,Header ROM byte 1 - 0x00    */
  uint8_t mem[T1T_SEGMENT_SIZE]; /* Tag contents of block 0 or from block 0-E */
  tT1T_CMD_RSP_INFO*
      p_cmd_rsp_info; /* Pointer to Command rsp info of last sent command     */
  uint8_t state;      /* Current state of RW module                           */
  uint8_t tag_attribute; /* Present state of the Tag as interpreted by RW */
  NFC_HDR*
      p_cur_cmd_buf; /* Buffer to hold cur sent command for retransmission   */
  uint8_t addr;      /* ADD/ADD8/ADDS value                                  */
  tRW_T1T_PREV_CMD_RSP_INFO
      prev_cmd_rsp_info; /* Information about previous sent command if retx */
  TIMER_LIST_ENT timer; /* timer to set timelimit for the response to command */
  bool b_update;    /* Tag header updated                                   */
  bool b_rseg;      /* Segment 0 read from tag                              */
  bool b_hard_lock; /* Hard lock the tag as part of config tag to Read only */
#if (RW_NDEF_INCLUDED == TRUE)
  uint8_t segment;  /* Current Tag segment                                  */
  uint8_t substate; /* Current substate of RW module                        */
  uint16_t work_offset;                     /* Working byte offset */
  uint8_t ndef_first_block[T1T_BLOCK_SIZE]; /* Buffer for ndef first block */
  uint8_t ndef_final_block[T1T_BLOCK_SIZE]; /* Buffer for ndef last block */
  uint8_t* p_ndef_buffer;                   /* Buffer to store ndef message */
  uint16_t new_ndef_msg_len; /* Lenght of new updating NDEF Message */
  uint8_t block_read; /* Last read Block                                      */
  uint8_t write_byte; /* Index of last written byte                           */
  uint8_t tlv_detect; /* TLV type under detection                             */
  uint16_t ndef_msg_offset; /* The offset on Tag where first NDEF message is
                               present*/
  uint16_t ndef_msg_len;    /* Lenght of NDEF Message */
  uint16_t
      max_ndef_msg_len; /* Maximum size of NDEF that can be written on the tag
                           */
  uint16_t ndef_header_offset; /* The offset on Tag where first NDEF tlv is
                                  present    */
  uint8_t ndef_block_written;  /* Last block where NDEF bytes are written */
  uint8_t num_ndef_finalblock; /* Block number where NDEF's last byte will be
                                  present  */
  uint8_t num_lock_tlvs;       /* Number of lcok tlvs detected in the tag */
  tRW_T1T_LOCK_INFO lock_tlv[RW_T1T_MAX_LOCK_TLVS]; /* Information retrieved
                                                       from lock control tlv */
  uint8_t num_lockbytes; /* Number of dynamic lock bytes present in the tag */
  tRW_T1T_LOCK
      lockbyte[RW_T1T_MAX_LOCK_BYTES]; /* Dynamic Lock byte information */
  uint8_t num_mem_tlvs; /* Number of memory tlvs detected in the tag */
  tRW_T1T_RES_INFO
      mem_tlv[RW_T1T_MAX_MEM_TLVS]; /* Information retrieved from mem tlv */
  uint8_t attr_seg; /* Tag segment for which attributes are prepared        */
  uint8_t
      lock_attr_seg; /* Tag segment for which lock attributes are prepared   */
  uint8_t
      attr[T1T_BLOCKS_PER_SEGMENT]; /* byte information - Reserved/lock/otp or
                                       data         */
  uint8_t lock_attr
      [T1T_BLOCKS_PER_SEGMENT]; /* byte information - read only or read write */
#endif
} tRW_T1T_CB;

/* Mifare Ultalight/ Ultralight Family blank tag version block settings */
/* Block where version number of the tag is stored */
#define T2T_MIFARE_VERSION_BLOCK 0x04
/* Blank Ultralight tag - Block 4 (byte 0, byte 1) */
#define T2T_MIFARE_ULTRALIGHT_VER_NO 0xFFFF
/* Blank Ultralight family tag - Block 4 (byte 0, byte 1) */
#define T2T_MIFARE_ULTRALIGHT_FAMILY_VER_NO 0x0200

/* Infineon my-d move / my-d blank tag uid block settings */
#define T2T_INFINEON_VERSION_BLOCK 0x00
#define T2T_INFINEON_MYD_MOVE_LEAN 0x0570
#define T2T_INFINEON_MYD_MOVE 0x0530

#define T2T_BRCM_VERSION_BLOCK 0x00
#define T2T_BRCM_STATIC_MEM 0x2E01
#define T2T_BRCM_DYNAMIC_MEM 0x2E02

#define T2T_NDEF_NOT_DETECTED 0x00
#define T2T_NDEF_DETECTED 0x01
#define T2T_NDEF_READ 0x02

/* Maximum supported Memory control TLVS in the tag         */
#define RW_T2T_MAX_MEM_TLVS 0x05
/* Maximum supported Lock control TLVS in the tag           */
#define RW_T2T_MAX_LOCK_TLVS 0x05
/* Maximum supported dynamic lock bytes                     */
#define RW_T2T_MAX_LOCK_BYTES 0x1E
#define RW_T2T_SEGMENT_BYTES 128
#define RW_T2T_SEGMENT_SIZE 16

/* Lock not yet set as part of SET TAG RO op                */
#define RW_T2T_LOCK_NOT_UPDATED 0x00
/* Sent command to set the Lock bytes                       */
#define RW_T2T_LOCK_UPDATE_INITIATED 0x01
/* Lock bytes are set                                       */
#define RW_T2T_LOCK_UPDATED 0x02
typedef uint8_t tRW_T2T_LOCK_STATUS;

/* States */
/* Tag not activated                                        */
#define RW_T2T_STATE_NOT_ACTIVATED 0x00
/* T1 Tag activated and ready to perform rw operation on Tag*/
#define RW_T2T_STATE_IDLE 0x01
/* waiting response for read command sent to tag            */
#define RW_T2T_STATE_READ 0x02
/* waiting response for write command sent to tag           */
#define RW_T2T_STATE_WRITE 0x03
/* Waiting response for sector select command               */
#define RW_T2T_STATE_SELECT_SECTOR 0x04
/* Detecting Lock/Memory/NDEF/Proprietary TLV in the Tag    */
#define RW_T2T_STATE_DETECT_TLV 0x05
/* Performing NDEF Read procedure                           */
#define RW_T2T_STATE_READ_NDEF 0x06
/* Performing NDEF Write procedure                          */
#define RW_T2T_STATE_WRITE_NDEF 0x07
/* Setting Tag as Read only tag                             */
#define RW_T2T_STATE_SET_TAG_RO 0x08
/* Check if Tag is still present                            */
#define RW_T2T_STATE_CHECK_PRESENCE 0x09
/* Format the tag                                           */
#define RW_T2T_STATE_FORMAT_TAG 0x0A
/* Tag is in HALT State */
#define RW_T2T_STATE_HALT 0x0B

/* rw_t2t_read/rw_t2t_write takes care of sector change if the block to
 * read/write is in a different sector
 * Next Substate should be assigned to control variable 'substate' before
 * calling these function for State Machine to
 * move back to the particular substate after Sector change is completed and
 * read/write command is sent on new sector       */

/* Sub states */
#define RW_T2T_SUBSTATE_NONE 0x00

/* Sub states in RW_T2T_STATE_SELECT_SECTOR state */
/* waiting for response of sector select CMD 1              */
#define RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT 0x01
/* waiting for response of sector select CMD 2              */
#define RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR 0x02

/* Sub states in RW_T1T_STATE_DETECT_XXX state */
/* waiting for the detection of a tlv in a tag              */
#define RW_T2T_SUBSTATE_WAIT_READ_CC 0x03
/* waiting for the detection of a tlv in a tag              */
#define RW_T2T_SUBSTATE_WAIT_TLV_DETECT 0x04
/* waiting for finding the len field is 1 or 3 bytes long   */
#define RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN 0x05
/* waiting for extracting len field value                   */
#define RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0 0x06
/* waiting for extracting len field value                   */
#define RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN1 0x07
/* waiting for extracting value field in the TLV            */
#define RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE 0x08
/* waiting for reading dynamic locks in the TLV             */
#define RW_T2T_SUBSTATE_WAIT_READ_LOCKS 0x09

/* Sub states in RW_T2T_STATE_WRITE_NDEF state */
/* waiting for rsp to reading the block where NDEF starts   */
#define RW_T2T_SUBSTATE_WAIT_READ_NDEF_FIRST_BLOCK 0x0A
/* waiting for rsp to reading block where new NDEF Msg ends */
#define RW_T2T_SUBSTATE_WAIT_READ_NDEF_LAST_BLOCK 0x0B
/* waiting for rsp to reading block where Trm tlv gets added*/
#define RW_T2T_SUBSTATE_WAIT_READ_TERM_TLV_BLOCK 0x0C
/* waiting for rsp to reading block where nxt NDEF write    */
#define RW_T2T_SUBSTATE_WAIT_READ_NDEF_NEXT_BLOCK 0x0D
/* waiting for rsp to writting NDEF block                   */
#define RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_NEXT_BLOCK 0x0E
/* waiting for rsp to last NDEF block write cmd             */
#define RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LAST_BLOCK 0x0F
/* waiting for rsp to reading NDEF len field block          */
#define RW_T2T_SUBSTATE_WAIT_READ_NDEF_LEN_BLOCK 0x10
/* waiting for rsp of updating first NDEF len field block   */
#define RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_BLOCK 0x11
/* waiting for rsp of updating next NDEF len field block    */
#define RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK 0x12
/* waiting for rsp to writing to Terminator tlv             */
#define RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT 0x13

/* Sub states in RW_T2T_STATE_FORMAT_TAG state */
#define RW_T2T_SUBSTATE_WAIT_READ_VERSION_INFO 0x14
/* waiting for response to format/set capability container  */
#define RW_T2T_SUBSTATE_WAIT_SET_CC 0x15
#define RW_T2T_SUBSTATE_WAIT_SET_LOCK_TLV 0x16
/* waiting for response to format/set NULL NDEF             */
#define RW_T2T_SUBSTATE_WAIT_SET_NULL_NDEF 0x17

/* Sub states in RW_T2T_STATE_SET_TAG_RO state */
/* waiting for response to set CC3 to RO                    */
#define RW_T2T_SUBSTATE_WAIT_SET_CC_RO 0x19
/* waiting for response to read dynamic lock bytes block    */
#define RW_T2T_SUBSTATE_WAIT_READ_DYN_LOCK_BYTE_BLOCK 0x1A
/* waiting for response to set dynamic lock bits            */
#define RW_T2T_SUBSTATE_WAIT_SET_DYN_LOCK_BITS 0x1B
/* waiting for response to set static lock bits             */
#define RW_T2T_SUBSTATE_WAIT_SET_ST_LOCK_BITS 0x1C

typedef struct {
  uint16_t offset;              /* Offset of the lock byte in the Tag */
  uint8_t num_bits;             /* Number of lock bits in the lock byte */
  uint8_t bytes_locked_per_bit; /* No. of tag bytes gets locked by a bit in this
                                   byte       */
} tRW_T2T_LOCK_INFO;

typedef struct {
  uint16_t offset;   /* Reserved bytes offset taken from Memory control TLV */
  uint8_t num_bytes; /* Number of reserved bytes as per the TLV */
} tRW_T2T_RES_INFO;

typedef struct {
  uint8_t tlv_index; /* Index of Lock control tlv that points to this address */
  uint8_t byte_index; /* Index of Lock byte pointed by the TLV */
  uint8_t lock_byte;  /* Value in the lock byte */
  tRW_T2T_LOCK_STATUS
      lock_status;  /* Indicates if it is modifed to set tag as Read only */
  bool b_lock_read; /* Is the lock byte is already read from tag */
} tRW_T2T_LOCK;

/* RW Type 2 Tag control block */
typedef struct {
  uint8_t state;    /* Reader/writer state */
  uint8_t substate; /* Reader/write substate in NDEF write state */
  uint8_t
      prev_substate; /* Substate of the tag before moving to different sector */
  uint8_t sector;    /* Sector number that is selected */
  uint8_t select_sector; /* Sector number that is expected to get selected */
  uint8_t tag_hdr[T2T_READ_DATA_LEN];  /* T2T Header blocks */
  uint8_t tag_data[T2T_READ_DATA_LEN]; /* T2T Block 4 - 7 data */
  uint8_t ndef_status;    /* The current status of NDEF Write operation */
  uint16_t block_read;    /* Read block */
  uint16_t block_written; /* Written block */
  tT2T_CMD_RSP_INFO*
      p_cmd_rsp_info;     /* Pointer to Command rsp info of last sent command */
  NFC_HDR* p_cur_cmd_buf; /* Copy of current command, for retx/send after sector
                             change   */
  NFC_HDR* p_sec_cmd_buf; /* Copy of command, to send after sector change */
  TIMER_LIST_ENT t2_timer; /* timeout for each API call */
  bool b_read_hdr;         /* Tag header read from tag */
  bool b_read_data;        /* Tag data block read from tag */
  bool b_hard_lock; /* Hard lock the tag as part of config tag to Read only */
  bool check_tag_halt; /* Resent command after NACK rsp to find tag is in HALT
                          State   */
#if (RW_NDEF_INCLUDED == TRUE)
  bool skip_dyn_locks;   /* Skip reading dynamic lock bytes from the tag */
  uint8_t found_tlv;     /* The Tlv found while searching a particular TLV */
  uint8_t tlv_detect;    /* TLV type under detection */
  uint8_t num_lock_tlvs; /* Number of lcok tlvs detected in the tag */
  uint8_t attr_seg;      /* Tag segment for which attributes are prepared */
  uint8_t
      lock_attr_seg; /* Tag segment for which lock attributes are prepared */
  uint8_t segment;   /* Current operating segment */
  uint8_t ndef_final_block[T2T_BLOCK_SIZE]; /* Buffer for ndef last block */
  uint8_t num_mem_tlvs;  /* Number of memory tlvs detected in the tag */
  uint8_t num_lockbytes; /* Number of dynamic lock bytes present in the tag */
  uint8_t attr
      [RW_T2T_SEGMENT_SIZE]; /* byte information - Reserved/lock/otp or data */
  uint8_t lock_attr[RW_T2T_SEGMENT_SIZE];  /* byte information - read only or
                                              read write                   */
  uint8_t tlv_value[3];                    /* Read value field of TLV */
  uint8_t ndef_first_block[T2T_BLOCK_LEN]; /* NDEF TLV Header block */
  uint8_t ndef_read_block[T2T_BLOCK_LEN];  /* Buffer to hold read before write
                                              block                       */
  uint8_t ndef_last_block[T2T_BLOCK_LEN];  /* Terminator TLV block after NDEF
                                              Write operation              */
  uint8_t terminator_tlv_block[T2T_BLOCK_LEN]; /* Terminator TLV Block */
  uint16_t ndef_last_block_num; /* Block where last byte of updating ndef
                                   message will exist    */
  uint16_t ndef_read_block_num; /* Block read during NDEF Write to avoid
                                   overwritting res bytes */
  uint16_t
      bytes_count; /* No. of bytes remaining to collect during tlv detect */
  uint16_t
      terminator_byte_index; /* The offset of the tag where terminator tlv may
                                be added      */
  uint16_t work_offset;      /* Working byte offset */
  uint16_t ndef_header_offset;
  uint16_t
      ndef_msg_offset;   /* Offset on Tag where first NDEF message is present */
  uint16_t ndef_msg_len; /* Lenght of NDEF Message */
  uint16_t
      max_ndef_msg_len; /* Maximum size of NDEF that can be written on the tag
                           */
  uint16_t new_ndef_msg_len; /* Lenght of new updating NDEF Message */
  uint16_t ndef_write_block;
  uint16_t prop_msg_len;      /* Proprietary tlv length */
  uint8_t* p_new_ndef_buffer; /* Pointer to updating NDEF Message */
  uint8_t* p_ndef_buffer;     /* Pointer to NDEF Message */
  tRW_T2T_LOCK_INFO lock_tlv[RW_T2T_MAX_LOCK_TLVS]; /* Information retrieved
                                                       from lock control tlv */
  tRW_T2T_LOCK
      lockbyte[RW_T2T_MAX_LOCK_BYTES]; /* Dynamic Lock byte information */
  tRW_T2T_RES_INFO
      mem_tlv[RW_T2T_MAX_MEM_TLVS]; /* Information retrieved from mem tlv */
#endif
} tRW_T2T_CB;

/* Type 3 Tag control block */
typedef uint8_t tRW_T3T_RW_STATE;

typedef struct {
  tNFC_STATUS status;
  uint8_t version; /* Ver: peer version */
  uint8_t
      nbr; /* NBr: number of blocks that can be read using one Check command */
  uint8_t nbw;    /* Nbw: number of blocks that can be written using one Update
                     command */
  uint16_t nmaxb; /* Nmaxb: maximum number of blocks available for NDEF data */
  uint8_t writef; /* WriteFlag: 00h if writing data finished; 0Fh if writing
                     data in progress */
  uint8_t
      rwflag;  /* RWFlag: 00h NDEF is read-only; 01h if read/write available */
  uint32_t ln; /* Ln: actual size of stored NDEF data (in bytes) */
} tRW_T3T_DETECT;

/* RW_T3T control block flags */
/* The final command for completing the NDEF read/write */
#define RW_T3T_FL_IS_FINAL_NDEF_SEGMENT 0x01
/* Waiting for POLL response for presence check */
#define RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP 0x02
/* Waiting for POLL response for RW_T3tGetSystemCodes */
#define RW_T3T_FL_W4_GET_SC_POLL_RSP 0x04
/* Waiting for POLL response for RW_T3tDetectNDef */
#define RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP 0x08
/* Waiting for POLL response for RW_T3tFormat */
#define RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP 0x10
/* Waiting for POLL response for RW_T3tSetReadOnly */
#define RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP 0x20

typedef struct {
  uint32_t cur_tout; /* Current command timeout */
  /* check timeout is check_tout_a + n * check_tout_b; X is T/t3t * 4^E */
  uint32_t check_tout_a; /* Check command timeout (A+1)*X */
  uint32_t check_tout_b; /* Check command timeout (B+1)*X */
  /* update timeout is update_tout_a + n * update_tout_b; X is T/t3t * 4^E */
  uint32_t update_tout_a;    /* Update command timeout (A+1)*X */
  uint32_t update_tout_b;    /* Update command timeout (B+1)*X */
  tRW_T3T_RW_STATE rw_state; /* Reader/writer state */
  uint8_t rw_substate;
  uint8_t cur_cmd;           /* Current command being executed */
  NFC_HDR* p_cur_cmd_buf;    /* Copy of current command, for retransmission */
  TIMER_LIST_ENT timer;      /* timeout for waiting for response */
  TIMER_LIST_ENT poll_timer; /* timeout for waiting for response */

  tRW_T3T_DETECT ndef_attrib; /* T3T NDEF attribute information */

  uint32_t ndef_msg_len;        /* Length of ndef message to send */
  uint32_t ndef_msg_bytes_sent; /* Length of ndef message sent so far */
  uint8_t* ndef_msg;            /* Buffer for outgoing NDEF message */
  uint32_t ndef_rx_readlen; /* Number of bytes read in current CHECK command */
  uint32_t ndef_rx_offset;  /* Length of ndef message read so far */

  uint8_t num_system_codes; /* System codes detected */
  uint16_t system_codes[T3T_MAX_SYSTEM_CODES];

  uint8_t peer_nfcid2[NCI_NFCID2_LEN];
  uint8_t cur_poll_rc; /* RC used in current POLL command */

  uint8_t flags; /* Flags see RW_T3T_FL_* */
} tRW_T3T_CB;

/*
**  Type 4 Tag
*/

/* Max data size using a single ReadBinary. 2 bytes are for status bytes */
#define RW_T4T_MAX_DATA_PER_READ                             \
  (NFC_RW_POOL_BUF_SIZE - NFC_HDR_SIZE - NCI_DATA_HDR_SIZE - \
   T4T_RSP_STATUS_WORDS_SIZE)

/* Max data size using a single UpdateBinary. 6 bytes are for CLA, INS, P1, P2,
 * Lc */
#define RW_T4T_MAX_DATA_PER_WRITE                              \
  (NFC_RW_POOL_BUF_SIZE - NFC_HDR_SIZE - NCI_MSG_OFFSET_SIZE - \
   NCI_DATA_HDR_SIZE - T4T_CMD_MAX_HDR_SIZE)

/* Mandatory NDEF file control */
typedef struct {
  uint16_t file_id;       /* File Identifier          */
  uint16_t max_file_size; /* Max NDEF file size       */
  uint8_t read_access;    /* read access condition    */
  uint8_t write_access;   /* write access condition   */
} tRW_T4T_NDEF_FC;

/* Capability Container */
typedef struct {
  uint16_t cclen;          /* the size of this capability container        */
  uint8_t version;         /* the mapping specification version            */
  uint16_t max_le;         /* the max data size by a single ReadBinary     */
  uint16_t max_lc;         /* the max data size by a single UpdateBinary   */
  tRW_T4T_NDEF_FC ndef_fc; /* Mandatory NDEF file control                  */
} tRW_T4T_CC;

typedef uint8_t tRW_T4T_RW_STATE;
typedef uint8_t tRW_T4T_RW_SUBSTATE;

/* Type 4 Tag Control Block */
typedef struct {
  tRW_T4T_RW_STATE state;        /* main state                       */
  tRW_T4T_RW_SUBSTATE sub_state; /* sub state                        */
  uint8_t version;               /* currently effective version      */
  TIMER_LIST_ENT timer;          /* timeout for each API call        */

  uint16_t ndef_length;    /* length of NDEF data              */
  uint8_t* p_update_data;  /* pointer of data to update        */
  uint16_t rw_length;      /* remaining bytes to read/write    */
  uint16_t rw_offset;      /* remaining offset to read/write   */
  NFC_HDR* p_data_to_free; /* GKI buffet to delete after done  */

  tRW_T4T_CC cc_file; /* Capability Container File        */

/* NDEF has been detected   */
#define RW_T4T_NDEF_STATUS_NDEF_DETECTED 0x01
/* NDEF file is read-only   */
#define RW_T4T_NDEF_STATUS_NDEF_READ_ONLY 0x02

  uint8_t ndef_status; /* bitmap for NDEF status           */
  uint8_t channel;     /* channel id: used for read-binary */

  uint16_t max_read_size;   /* max reading size per a command   */
  uint16_t max_update_size; /* max updating size per a command  */
  uint16_t card_size;
  uint8_t card_type;
} tRW_T4T_CB;

/* RW retransmission statistics */
#if (RW_STATS_INCLUDED == TRUE)
typedef struct {
  uint32_t start_tick;     /* System tick count at activation */
  uint32_t bytes_sent;     /* Total bytes sent since activation */
  uint32_t bytes_received; /* Total bytes received since activation */
  uint32_t num_ops;        /* Number of operations since activation */
  uint32_t num_retries;    /* Number of retranmissions since activation */
  uint32_t num_crc;        /* Number of crc failures */
  uint32_t num_trans_err;  /* Number of transmission error notifications */
  uint32_t num_fail;       /* Number of aborts (failures after retries) */
} tRW_STATS;
#endif /* RW_STATS_INCLUDED */

/* Mifare Classic RW Control Block */

typedef struct {
  uint16_t block;
  bool auth;
} tRW_MFC_BLOCK;

#define MFC_NDEF_NOT_DETECTED 0x00
#define MFC_NDEF_DETECTED 0x01
#define MFC_NDEF_READ 0x02

typedef uint8_t tRW_MFC_RW_STATE;
typedef uint8_t tRW_MFC_RW_SUBSTATE;
typedef struct {
  tRW_MFC_RW_STATE state;       /* main state */
  tRW_MFC_RW_SUBSTATE substate; /* Reader/write substate in NDEF write state*/
  tRW_MFC_RW_SUBSTATE
      prev_substate;    /* Reader/write substate in NDEF write state*/
  TIMER_LIST_ENT timer; /* timeout for each API call */
  uint8_t uid[4];
  uint8_t selres;
  uint8_t tlv_detect;       /* TLV type under detection */
  uint16_t ndef_length;     /* length of NDEF data */
  uint16_t ndef_start_pos;   /* NDEF start position */
  uint16_t ndef_first_block; /* Frst block containing the NDEF */
  uint8_t* p_update_data;   /* pointer of data to update */
  uint16_t rw_length;       /* remaining bytes to read/write */
  uint16_t rw_offset;       /* remaining offset to read/write */
  NFC_HDR* p_data_to_free;  /* GKI buffer to delete after done */
  tRW_MFC_BLOCK last_block_accessed;
  tRW_MFC_BLOCK next_block;
  uint8_t sector_authentified;
  TIMER_LIST_ENT mfc_timer; /* timeout for each API call */
  uint16_t work_offset;     /* Working byte offset */
  uint8_t* p_ndef_buffer;   /* Buffer to store ndef message */
  uint16_t current_block;
  NFC_HDR* p_cur_cmd_buf; /* Copy of current command, for retx/send after sector
                             change */

  uint8_t ndef_status; /* bitmap for NDEF status */
} tRW_MFC_CB;

/* ISO 15693 RW Control Block */
typedef uint8_t tRW_I93_RW_STATE;
typedef uint8_t tRW_I93_RW_SUBSTATE;

/* tag is read-only                        */
#define RW_I93_FLAG_READ_ONLY 0x01
/* tag supports read multi block           */
#define RW_I93_FLAG_READ_MULTI_BLOCK 0x02
/* need to reset DSFID for formatting      */
#define RW_I93_FLAG_RESET_DSFID 0x04
/* need to reset AFI for formatting        */
#define RW_I93_FLAG_RESET_AFI 0x08
/* use 2 bytes for number of blocks        */
#define RW_I93_FLAG_16BIT_NUM_BLOCK 0x10
/* use extended commands */
#define RW_I93_FLAG_EXT_COMMANDS 0x20

/* searching for type                      */
#define RW_I93_TLV_DETECT_STATE_TYPE 0x01
/* searching for the first byte of length  */
#define RW_I93_TLV_DETECT_STATE_LENGTH_1 0x02
/* searching for the second byte of length */
#define RW_I93_TLV_DETECT_STATE_LENGTH_2 0x03
/* searching for the third byte of length  */
#define RW_I93_TLV_DETECT_STATE_LENGTH_3 0x04
/* reading value field                     */
#define RW_I93_TLV_DETECT_STATE_VALUE 0x05

enum {
  RW_I93_ICODE_SLI,                  /* ICODE SLI, SLIX                  */
  RW_I93_ICODE_SLI_S,                /* ICODE SLI-S, SLIX-S              */
  RW_I93_ICODE_SLI_L,                /* ICODE SLI-L, SLIX-L              */
  RW_I93_TAG_IT_HF_I_PLUS_INLAY,     /* Tag-it HF-I Plus Inlay           */
  RW_I93_TAG_IT_HF_I_PLUS_CHIP,      /* Tag-it HF-I Plus Chip            */
  RW_I93_TAG_IT_HF_I_STD_CHIP_INLAY, /* Tag-it HF-I Standard Chip/Inlyas */
  RW_I93_TAG_IT_HF_I_PRO_CHIP_INLAY, /* Tag-it HF-I Pro Chip/Inlyas      */
  RW_I93_STM_LRI1K,                  /* STM LRI1K                        */
  RW_I93_STM_LRI2K,                  /* STM LRI2K                        */
  RW_I93_STM_LRIS2K,                 /* STM LRIS2K                       */
  RW_I93_STM_LRIS64K,                /* STM LRIS64K                      */
  RW_I93_STM_M24LR64_R,              /* STM M24LR64-R                    */
  RW_I93_STM_M24LR04E_R,             /* STM M24LR04E-R                   */
  RW_I93_STM_M24LR16E_R,             /* STM M24LR16E-R                   */
  RW_I93_STM_M24LR64E_R,             /* STM M24LR64E-R                   */
  RW_I93_STM_ST25DV04K,              /* STM ST25DV04K                    */
  RW_I93_STM_ST25DVHIK,              /* STM ST25DV 16K OR 64K            */
  RW_I93_ONS_N36RW02,                /* ONS N36RW02                      */
  RW_I93_ONS_N24RF04,                /* ONS N24RF04                      */
  RW_I93_ONS_N24RF04E,               /* ONS N24RF04E                     */
  RW_I93_ONS_N24RF16,                /* ONS N24RF16                      */
  RW_I93_ONS_N24RF16E,               /* ONS N24RF16E                     */
  RW_I93_ONS_N24RF64,                /* ONS N24RF64                      */
  RW_I93_ONS_N24RF64E,               /* ONS N24RF64E                     */
  RW_I93_UNKNOWN_PRODUCT             /* Unknwon product version          */
};

typedef struct {
  tRW_I93_RW_STATE state;        /* main state                       */
  tRW_I93_RW_SUBSTATE sub_state; /* sub state                        */
  TIMER_LIST_ENT timer;          /* timeout for each sent command    */
  uint8_t sent_cmd;              /* last sent command                */
  uint8_t retry_count;           /* number of retry                  */
  NFC_HDR* p_retry_cmd;          /* buffer to store cmd sent last    */

  uint8_t info_flags;            /* information flags                */
  uint8_t uid[I93_UID_BYTE_LEN]; /* UID of currently activated       */
  uint8_t dsfid;                 /* DSFID if I93_INFO_FLAG_DSFID     */
  uint8_t afi;                   /* AFI if I93_INFO_FLAG_AFI         */
  uint8_t block_size;            /* block size of tag, in bytes      */
  uint16_t num_block;            /* number of blocks in tag          */
  uint8_t ic_reference;          /* IC Reference of tag              */
  uint8_t product_version;       /* tag product version              */

  uint8_t intl_flags; /* flags for internal information   */

  uint8_t tlv_detect_state; /* TLV detecting state              */
  uint8_t tlv_type;         /* currently detected type          */
  uint16_t tlv_length;      /* currently detected length        */

  uint16_t ndef_tlv_start_offset; /* offset of first byte of NDEF TLV */
  uint16_t ndef_tlv_last_offset;  /* offset of last byte of NDEF TLV  */
  uint16_t max_ndef_length;       /* max NDEF length the tag contains */
  uint16_t ndef_length;           /* length of NDEF data              */

  uint8_t* p_update_data; /* pointer of data to update        */
  uint16_t rw_length;     /* bytes to read/write              */
  uint16_t rw_offset;     /* offset to read/write             */
} tRW_I93_CB;

/* RW memory control blocks */
typedef union {
  tRW_T1T_CB t1t;
  tRW_T2T_CB t2t;
  tRW_T3T_CB t3t;
  tRW_T4T_CB t4t;
  tRW_I93_CB i93;
  tRW_MFC_CB mfc;
} tRW_TCB;

/* RW control blocks */
typedef struct {
  tRW_TCB tcb;
  tRW_CBACK* p_cback;
  uint32_t cur_retry; /* Retry count for the current operation */
#if (RW_STATS_INCLUDED == TRUE)
  tRW_STATS stats;
#endif /* RW_STATS_INCLUDED */
    UINT8               trace_level;
} tRW_CB;

/*****************************************************************************
**  EXTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

/* Global NFC data */
extern tRW_CB rw_cb;

/* from .c */

#if (RW_NDEF_INCLUDED == TRUE)
extern tRW_EVENT rw_t1t_handle_rsp(const tT1T_CMD_RSP_INFO* p_info,
                                   bool* p_notify, uint8_t* p_data,
                                   tNFC_STATUS* p_status);
extern tRW_EVENT rw_t1t_info_to_event(const tT1T_CMD_RSP_INFO* p_info);
#else
#define rw_t1t_handle_rsp(p, a, b, c) t1t_info_to_evt(p)
#define rw_t1t_info_to_event(p) t1t_info_to_evt(p)
#endif

extern void rw_init(void);
extern tNFC_STATUS rw_t1t_select(uint8_t hr[T1T_HR_LEN],
                                 uint8_t uid[T1T_CMD_UID_LEN]);
extern tNFC_STATUS rw_t1t_send_dyn_cmd(uint8_t opcode, uint8_t add,
                                       uint8_t* p_dat);
extern tNFC_STATUS rw_t1t_send_static_cmd(uint8_t opcode, uint8_t add,
                                          uint8_t dat);
extern void rw_t1t_process_timeout(TIMER_LIST_ENT* p_tle);
extern void rw_t1t_handle_op_complete(void);

#if (NXP_EXTNS == TRUE)
extern tNFC_STATUS RW_T4tNfceeInitCb(void);
#endif
#if (RW_NDEF_INCLUDED == TRUE)
extern tRW_EVENT rw_t2t_info_to_event(const tT2T_CMD_RSP_INFO* p_info);
extern void rw_t2t_handle_rsp(uint8_t* p_data);
#else
#define rw_t2t_info_to_event(p) t2t_info_to_evt(p)
#define rw_t2t_handle_rsp(p)
#endif

extern tNFC_STATUS rw_t2t_sector_change(uint8_t sector);
extern tNFC_STATUS rw_t2t_read(uint16_t block);
extern tNFC_STATUS rw_t2t_write(uint16_t block, uint8_t* p_write_data);
extern void rw_t2t_process_timeout();
extern tNFC_STATUS rw_t2t_select(void);
void rw_t2t_handle_op_complete(void);

extern void rw_t3t_process_timeout(TIMER_LIST_ENT* p_tle);
extern tNFC_STATUS rw_t3t_select(uint8_t peer_nfcid2[NCI_RF_F_UID_LEN],
                                 uint8_t mrti_check, uint8_t mrti_update);
void rw_t3t_handle_nci_poll_ntf(uint8_t nci_status, uint8_t num_responses,
                                uint8_t sensf_res_buf_size,
                                uint8_t* p_sensf_res_buf);

extern tNFC_STATUS rw_t4t_select(void);
extern void rw_t4t_process_timeout(TIMER_LIST_ENT* p_tle);

extern tNFC_STATUS rw_i93_select(uint8_t* p_uid);
extern void rw_i93_process_timeout(TIMER_LIST_ENT* p_tle);
extern void rw_t4t_handle_isodep_nak_rsp(uint8_t status, bool is_ntf);

extern tNFC_STATUS rw_mfc_select(uint8_t selres, uint8_t uid[T1T_CMD_UID_LEN]);
extern void rw_mfc_process_timeout(TIMER_LIST_ENT* p_tle);
#if (RW_STATS_INCLUDED == TRUE)
/* Internal fcns for statistics (from rw_main.c) */
void rw_main_reset_stats(void);
void rw_main_update_tx_stats(uint32_t bytes_tx, bool is_retry);
void rw_main_update_rx_stats(uint32_t bytes_rx);
void rw_main_update_crc_error_stats(void);
void rw_main_update_trans_error_stats(void);
void rw_main_update_fail_stats(void);
void rw_main_log_stats(void);
#endif /* RW_STATS_INCLUDED */

#endif /* RW_INT_H_ */
