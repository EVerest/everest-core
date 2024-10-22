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
 *  This file contains definitions for some utility functions to help parse
 *  and build NFC Data Exchange Format (NDEF) messages
 *
 ******************************************************************************/

#ifndef NDEF_UTILS_H
#define NDEF_UTILS_H

#include "bt_types.h"
#define HR_REC_TYPE_LEN     2       /* Handover Request Record Type     */
#define HS_REC_TYPE_LEN     2       /* Handover Select Record Type      */
#define HC_REC_TYPE_LEN     2       /* Handover Carrier recrod Type     */
#define CR_REC_TYPE_LEN     2       /* Collision Resolution Record Type */
#define AC_REC_TYPE_LEN     2       /* Alternative Carrier Record Type  */
#define ERR_REC_TYPE_LEN    3       /* Error Record Type                */
#define BT_OOB_REC_TYPE_LEN 32      /* Bluetooth OOB Data Type          */
#define WIFI_WSC_REC_TYPE_LEN 23    /* Wifi WSC Data Type               */

#define NDEF_MB_MASK 0x80  /* Message Begin */
#define NDEF_ME_MASK 0x40  /* Message End */
#define NDEF_CF_MASK 0x20  /* Chunk Flag */
#define NDEF_SR_MASK 0x10  /* Short Record */
#define NDEF_IL_MASK 0x08  /* ID Length */
#define NDEF_TNF_MASK 0x07 /* Type Name Format */
/* First valid ASCII as per RTD specification */
#define NDEF_RTD_VALID_START 0x20
/* Last valid ASCII as per RTD specification */
#define NDEF_RTD_VALID_END 0x7E

/* NDEF Type Name Format */
#define NDEF_TNF_EMPTY 0     /* Empty (type/id/payload len =0) */
#define NDEF_TNF_WKT 1       /* NFC Forum well-known type/RTD */
#define NDEF_TNF_MEDIA 2     /* Media-type as defined in RFC 2046 */
#define NDEF_TNF_URI 3       /* Absolute URI as defined in RFC 3986 */
#define NDEF_TNF_EXT 4       /* NFC Forum external type/RTD */
#define NDEF_TNF_UNKNOWN 5   /* Unknown (type len =0) */
#define NDEF_TNF_UNCHANGED 6 /* Unchanged (type len =0) */
#define NDEF_TNF_RESERVED 7  /* Reserved */
/* Define the status code returned from the Validate, Parse or Build functions
*/
enum {
  NDEF_OK, /* 0 - OK                                   */

  NDEF_REC_NOT_FOUND,         /* 1 - No record matching the find criteria */
  NDEF_MSG_TOO_SHORT,         /* 2 - Message was too short (< 3 bytes)    */
  NDEF_MSG_NO_MSG_BEGIN,      /* 3 - No 'begin' flag at start of message  */
  NDEF_MSG_NO_MSG_END,        /* 4 - No 'end' flag at end of message      */
  NDEF_MSG_EXTRA_MSG_BEGIN,   /* 5 - 'begin' flag after start of message  */
  NDEF_MSG_UNEXPECTED_CHUNK,  /* 6 - Unexpected chunk found               */
  NDEF_MSG_INVALID_EMPTY_REC, /* 7 - Empty record with non-zero contents  */
  NDEF_MSG_INVALID_CHUNK,     /* 8 - Invalid chunk found                  */
  NDEF_MSG_LENGTH_MISMATCH,   /* 9 - Overall message length doesn't match */
  NDEF_MSG_INSUFFICIENT_MEM,  /* 10 - Insuffiecient memory to add record  */
  NDEF_MSG_INVALID_TYPE       /* 11 - TYPE field contains invalid characters  */
};
typedef uint8_t tNDEF_STATUS;
/* Define prefix for exporting APIs from libraries */
#ifdef  NFC_DLL
#define EXPORT_NDEF_API __declspec(dllexport)       /* Windows DLL export prefix */
#else
#define EXPORT_NDEF_API
#endif

/* Functions to parse a received NDEF Message
*/
/*******************************************************************************
**
** Function         NDEF_MsgValidate
**
** Description      This function validates an NDEF message.
**
** Returns          TRUE if all OK, or FALSE if the message is invalid.
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgValidate(uint8_t* p_msg, uint32_t msg_len,
                                     bool b_allow_chunks);

/*******************************************************************************
**
** Function         NDEF_MsgGetNumRecs
**
** Description      This function gets the number of records in the given NDEF
**                  message.
**
** Returns          The record count, or 0 if the message is invalid.
**
*******************************************************************************/
extern int32_t NDEF_MsgGetNumRecs(uint8_t* p_msg);

/*******************************************************************************
**
** Function         NDEF_MsgGetRecLength
**
** Description      This function returns length of the current record in the
**                  given NDEF message.
**
** Returns          Length of record
**
*******************************************************************************/
extern uint32_t NDEF_MsgGetRecLength(uint8_t* p_cur_rec);

/*******************************************************************************
**
** Function         NDEF_MsgGetNextRec
**
** Description      This function gets a pointer to the next record after the
**                  current one.
**
** Returns          Pointer to the start of the record, or NULL if no more
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetNextRec(uint8_t* p_cur_rec);

/*******************************************************************************
**
** Function         NDEF_MsgGetRecByIndex
**
** Description      This function gets a pointer to the record with the given
**                  index (0-based index) in the given NDEF message.
**
** Returns          Pointer to the start of the record, or NULL
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetRecByIndex(uint8_t* p_msg, int32_t index);

/*******************************************************************************
**
** Function         NDEF_MsgGetLastRecInMsg
**
** Description      This function gets a pointer to the last record in the
**                  given NDEF message.
**
** Returns          Pointer to the start of the last record, or NULL if some
**                  problem
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetLastRecInMsg(uint8_t* p_msg);

/*******************************************************************************
**
** Function         NDEF_MsgGetFirstRecByType
**
** Description      This function gets a pointer to the first record with the
**                  given record type in the given NDEF message.
**
** Returns          Pointer to the start of the record, or NULL
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetFirstRecByType(uint8_t* p_msg, uint8_t tnf,
                                          uint8_t* p_type, uint8_t tlen);

/*******************************************************************************
**
** Function         NDEF_MsgGetNextRecByType
**
** Description      This function gets a pointer to the next record with the
**                  given record type in the given NDEF message.
**
** Returns          Pointer to the start of the record, or NULL
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetNextRecByType(uint8_t* p_cur_rec, uint8_t tnf,
                                         uint8_t* p_type, uint8_t tlen);

/*******************************************************************************
**
** Function         NDEF_MsgGetFirstRecById
**
** Description      This function gets a pointer to the first record with the
**                  given record id in the given NDEF message.
**
** Returns          Pointer to the start of the record, or NULL
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetFirstRecById(uint8_t* p_msg, uint8_t* p_id,
                                        uint8_t ilen);

/*******************************************************************************
**
** Function         NDEF_MsgGetNextRecById
**
** Description      This function gets a pointer to the next record with the
**                  given record id in the given NDEF message.
**
** Returns          Pointer to the start of the record, or NULL
**
*******************************************************************************/
extern uint8_t* NDEF_MsgGetNextRecById(uint8_t* p_cur_rec, uint8_t* p_id,
                                       uint8_t ilen);

/*******************************************************************************
**
** Function         NDEF_RecGetType
**
** Description      This function gets a pointer to the record type for the
**                  given NDEF record.
**
** Returns          Pointer to Type (NULL if none). TNF and len are filled in.
**
*******************************************************************************/
extern uint8_t* NDEF_RecGetType(uint8_t* p_rec, uint8_t* p_tnf,
                                uint8_t* p_type_len);

/*******************************************************************************
**
** Function         NDEF_RecGetId
**
** Description      This function gets a pointer to the record id for the given
**                  NDEF record.
**
** Returns          Pointer to Id (NULL if none). ID Len is filled in.
**
*******************************************************************************/
extern uint8_t* NDEF_RecGetId(uint8_t* p_rec, uint8_t* p_id_len);

/*******************************************************************************
**
** Function         NDEF_RecGetPayload
**
** Description      This function gets a pointer to the payload for the given
**                  NDEF record.
**
** Returns          a pointer to the payload (NULL if none). Payload len filled
**                  in.
**
*******************************************************************************/
extern uint8_t* NDEF_RecGetPayload(uint8_t* p_rec, uint32_t* p_payload_len);

/* Functions to build an NDEF Message
*/
/*******************************************************************************
**
** Function         NDEF_MsgInit
**
** Description      This function initializes an NDEF message.
**
** Returns          void
**                  *p_cur_size is initialized to 0
**
*******************************************************************************/
extern void NDEF_MsgInit(uint8_t* p_msg, uint32_t max_size,
                         uint32_t* p_cur_size);

/*******************************************************************************
**
** Function         NDEF_MsgAddRec
**
** Description      This function adds an NDEF record to the end of an NDEF
**                  message.
**
** Returns          OK, or error if the record did not fit
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgAddRec(uint8_t* p_msg, uint32_t max_size,
                                   uint32_t* p_cur_size, uint8_t tnf,
                                   uint8_t* p_type, uint8_t type_len,
                                   uint8_t* p_id, uint8_t id_len,
                                   uint8_t* p_payload, uint32_t payload_len);

/*******************************************************************************
**
** Function         NDEF_MsgAppendPayload
**
** Description      This function appends extra payload to a specific record in
**                  the given NDEF message
**
** Returns          OK, or error if the extra payload did not fit
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgAppendPayload(uint8_t* p_msg, uint32_t max_size,
                                          uint32_t* p_cur_size, uint8_t* p_rec,
                                          uint8_t* p_add_pl,
                                          uint32_t add_pl_len);

/*******************************************************************************
**
** Function         NDEF_MsgReplacePayload
**
** Description      This function replaces the payload of a specific record in
**                  the given NDEF message
**
** Returns          OK, or error if the new payload did not fit
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgReplacePayload(uint8_t* p_msg, uint32_t max_size,
                                           uint32_t* p_cur_size, uint8_t* p_rec,
                                           uint8_t* p_new_pl,
                                           uint32_t new_pl_len);

/*******************************************************************************
**
** Function         NDEF_MsgReplaceType
**
** Description      This function replaces the type field of a specific record
**                  in the given NDEF message
**
** Returns          OK, or error if the new type field did not fit
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgReplaceType(uint8_t* p_msg, uint32_t max_size,
                                        uint32_t* p_cur_size, uint8_t* p_rec,
                                        uint8_t* p_new_type,
                                        uint8_t new_type_len);

/*******************************************************************************
**
** Function         NDEF_MsgReplaceId
**
** Description      This function replaces the ID field of a specific record in
**                  the given NDEF message
**
** Returns          OK, or error if the new ID field did not fit
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgReplaceId(uint8_t* p_msg, uint32_t max_size,
                                      uint32_t* p_cur_size, uint8_t* p_rec,
                                      uint8_t* p_new_id, uint8_t new_id_len);

/*******************************************************************************
**
** Function         NDEF_MsgRemoveRec
**
** Description      This function removes the record at the given
**                  index in the given NDEF message.
**
** Returns          OK, or error if the index was invalid
**                  *p_cur_size is updated
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgRemoveRec(uint8_t* p_msg, uint32_t* p_cur_size,
                                      int32_t index);

/*******************************************************************************
**
** Function         NDEF_MsgCopyAndDechunk
**
** Description      This function copies and de-chunks an NDEF message.
**                  It is assumed that the destination is at least as large
**                  as the source, since the source may not actually contain
**                  any chunks.
**
** Returns          The output byte count
**
*******************************************************************************/
extern tNDEF_STATUS NDEF_MsgCopyAndDechunk(uint8_t* p_src, uint32_t src_len,
                                           uint8_t* p_dest,
                                           uint32_t* p_out_len);

/*******************************************************************************
**
** Function         NDEF_MsgCreateWktHr
**
** Description      This function creates Handover Request Record with version.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgCreateWktHr (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                 UINT8 version );

/*******************************************************************************
**
** Function         NDEF_MsgCreateWktHs
**
** Description      This function creates Handover Select Record with version.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgCreateWktHs (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                 UINT8 version );

/*******************************************************************************
**
** Function         NDEF_MsgAddWktHc
**
** Description      This function adds Handover Carrier Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddWktHc (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                              char  *p_id_str, UINT8 ctf,
                                              UINT8 carrier_type_len, UINT8 *p_carrier_type,
                                              UINT8 carrier_data_len, UINT8 *p_carrier_data);

/*******************************************************************************
**
** Function         NDEF_MsgAddWktAc
**
** Description      This function adds Alternative Carrier Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddWktAc (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                              UINT8 cps, char *p_carrier_data_ref_str,
                                              UINT8 aux_data_ref_count, char *p_aux_data_ref_str[]);

/*******************************************************************************
**
** Function         NDEF_MsgAddWktCr
**
** Description      This function adds Collision Resolution Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddWktCr (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                              UINT16 random_number );

/*******************************************************************************
**
** Function         NDEF_MsgAddWktErr
**
** Description      This function adds Error Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddWktErr (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                               UINT8 error_reason, UINT32 error_data );

/*******************************************************************************
**
** Function         NDEF_MsgAddMediaBtOob
**
** Description      This function adds BT OOB Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddMediaBtOob (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                   char *p_id_str, BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         NDEF_MsgAppendMediaBtOobCod
**
** Description      This function appends COD EIR data at the end of BT OOB Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAppendMediaBtOobCod (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                         char *p_id_str, DEV_CLASS cod);

/*******************************************************************************
**
** Function         NDEF_MsgAppendMediaBtOobName
**
** Description      This function appends Bluetooth Local Name EIR data
**                  at the end of BT OOB Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAppendMediaBtOobName (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                          char *p_id_str, BOOLEAN is_complete,
                                                          UINT8 name_len, UINT8 *p_name);

/*******************************************************************************
**
** Function         NDEF_MsgAppendMediaBtOobHashCRandR
**
** Description      This function appends Hash C and Rand R at the end of BT OOB Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAppendMediaBtOobHashCRandR (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                                char *p_id_str, UINT8 *p_hash_c, UINT8 *p_rand_r);

/*******************************************************************************
**
** Function         NDEF_MsgAppendMediaBtOobEirData
**
** Description      This function appends EIR Data at the end of BT OOB Record.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAppendMediaBtOobEirData (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                                             char *p_id_str,
                                                             UINT8 eir_type, UINT8 data_len, UINT8 *p_data);

/*******************************************************************************
**
** Function         NDEF_MsgAddMediaWifiWsc
**
** Description      This function adds Wifi Wsc Record header.
**
** Returns          NDEF_OK if all OK
**
*******************************************************************************/
EXPORT_NDEF_API extern tNDEF_STATUS NDEF_MsgAddMediaWifiWsc (UINT8 *p_msg, UINT32 max_size, UINT32 *p_cur_size,
                                    char *p_id_str, UINT8 *p_payload, UINT32 payload_len);


#endif /* NDEF_UTILS_H */
