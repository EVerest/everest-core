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
 *  This file contains source code for some utility functions to help parse
 *  and build NFC Data Exchange Format (NDEF) messages
 *
 ******************************************************************************/
#include "ndef_utils.h"
#include <log/log.h>
#include <string.h>

/*******************************************************************************
**
**              Static Local Functions
**
*******************************************************************************/

/*******************************************************************************
**
** Function         shiftdown
**
** Description      shift memory down (to make space to insert a record)
**
*******************************************************************************/
static void shiftdown(uint8_t* p_mem, uint32_t len, uint32_t shift_amount) {
  uint8_t* ps = p_mem + len - 1;
  uint8_t* pd = ps + shift_amount;
  uint32_t xx;

  for (xx = 0; xx < len; xx++) *pd-- = *ps--;
}

/*******************************************************************************
**
** Function         shiftup
**
** Description      shift memory up (to delete a record)
**
*******************************************************************************/
static void shiftup(uint8_t* p_dest, uint8_t* p_src, uint32_t len) {
  uint8_t* ps = p_src;
  uint8_t* pd = p_dest;
  uint32_t xx;

  for (xx = 0; xx < len; xx++) *pd++ = *ps++;
}

/*******************************************************************************
**
** Function         NDEF_MsgValidate
**
** Description      This function validates an NDEF message.
**
** Returns          TRUE if all OK, or FALSE if the message is invalid.
**
*******************************************************************************/
tNDEF_STATUS NDEF_MsgValidate(uint8_t* p_msg, uint32_t msg_len,
                              bool b_allow_chunks) {
  uint8_t* p_rec = p_msg;
  uint8_t* p_end = p_msg + msg_len;
  uint8_t* p_new;
  uint8_t rec_hdr = 0, type_len, id_len;
  int count;
  uint32_t payload_len;
  bool bInChunk = false;

  if ((p_msg == nullptr) || (msg_len < 3)) return (NDEF_MSG_TOO_SHORT);

  /* The first record must have the MB bit set */
  if ((*p_msg & NDEF_MB_MASK) == 0) return (NDEF_MSG_NO_MSG_BEGIN);

  /* The first record cannot be a chunk */
  if ((*p_msg & NDEF_TNF_MASK) == NDEF_TNF_UNCHANGED)
    return (NDEF_MSG_UNEXPECTED_CHUNK);

  for (count = 0; p_rec < p_end; count++) {
    /* if less than short record header */
    if (p_rec + 3 > p_end) return (NDEF_MSG_TOO_SHORT);

    rec_hdr = *p_rec++;

    /* header should have a valid TNF */
    if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_MASK)
      return NDEF_MSG_INVALID_CHUNK;

    /* The second and all subsequent records must NOT have the MB bit set */
    if ((count > 0) && (rec_hdr & NDEF_MB_MASK))
      return (NDEF_MSG_EXTRA_MSG_BEGIN);

    /* Type field length */
    type_len = *p_rec++;

    /* If the record is chunked, first record must contain the type unless
     * it's Type Name Format is Unknown */
    if ((rec_hdr & NDEF_CF_MASK) && (rec_hdr & NDEF_MB_MASK) && type_len == 0 &&
        (rec_hdr & NDEF_TNF_MASK) != NDEF_TNF_UNKNOWN)
      return (NDEF_MSG_INVALID_CHUNK);

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else {
      /* if less than 4 bytes payload length */
      if (p_rec + 4 > p_end) return (NDEF_MSG_TOO_SHORT);

      BE_STREAM_TO_UINT32(payload_len, p_rec);
    }

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK) {
      /* if less than 1 byte ID field length */
      if (p_rec + 1 > p_end) return (NDEF_MSG_TOO_SHORT);

      id_len = *p_rec++;
    } else {
      id_len = 0;
      /* Empty record must have the id_len */
      if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_EMPTY)
        return (NDEF_MSG_INVALID_EMPTY_REC);
    }

    /* A chunk must have type "unchanged", and no type or ID fields */
    if (rec_hdr & NDEF_CF_MASK) {
      if (!b_allow_chunks) return (NDEF_MSG_UNEXPECTED_CHUNK);

      /* Inside a chunk, the type must be unchanged and no type or ID field i
       * sallowed */
      if (bInChunk) {
        if ((type_len != 0) || (id_len != 0) ||
            ((rec_hdr & NDEF_TNF_MASK) != NDEF_TNF_UNCHANGED))
          return (NDEF_MSG_INVALID_CHUNK);
      } else {
        /* First record of a chunk must NOT have type "unchanged" */
        if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_UNCHANGED)
          return (NDEF_MSG_INVALID_CHUNK);

        bInChunk = true;
      }
    } else {
      /* This may be the last guy in a chunk. */
      if (bInChunk) {
        if ((type_len != 0) || (id_len != 0) ||
            ((rec_hdr & NDEF_TNF_MASK) != NDEF_TNF_UNCHANGED))
          return (NDEF_MSG_INVALID_CHUNK);

        bInChunk = false;
      } else {
        /* If not in a chunk, the record must NOT have type "unchanged" */
        if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_UNCHANGED)
          return (NDEF_MSG_INVALID_CHUNK);
      }
    }

    /* An empty record must NOT have a type, ID or payload */
    if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_EMPTY) {
      if ((type_len != 0) || (id_len != 0) || (payload_len != 0))
        return (NDEF_MSG_INVALID_EMPTY_REC);
    }

    if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_UNKNOWN) {
      if (type_len != 0) return (NDEF_MSG_LENGTH_MISMATCH);
    }

    /* External type should have non-zero type length */
    if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_EXT) {
      if (type_len == 0) return (NDEF_MSG_LENGTH_MISMATCH);
    }

    /* External type and Well Known types should have valid characters
       in the TYPE field */
    if ((rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_EXT ||
        (rec_hdr & NDEF_TNF_MASK) == NDEF_TNF_WKT) {
      uint8_t* p_rec_type = p_rec;
      if ((p_rec_type + type_len) > p_end) return (NDEF_MSG_TOO_SHORT);

      for (int type_index = 0; type_index < type_len; type_index++) {
        if (p_rec_type[type_index] < NDEF_RTD_VALID_START ||
            p_rec_type[type_index] > NDEF_RTD_VALID_END)
          return (NDEF_MSG_INVALID_TYPE);
      }
    }

    /* Check for OOB */
    if (payload_len + type_len + id_len < payload_len ||
        payload_len + type_len + id_len > msg_len) {
      return (NDEF_MSG_LENGTH_MISMATCH);
    }
    p_new = p_rec + (payload_len + type_len + id_len);
    if (p_rec > p_new || p_end < p_new) {
        android_errorWriteLog(0x534e4554, "126200054");
        return (NDEF_MSG_LENGTH_MISMATCH);
    }

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);

    if (rec_hdr & NDEF_ME_MASK) break;

    rec_hdr = 0;
  }

  /* The last record should have the ME bit set */
  if ((rec_hdr & NDEF_ME_MASK) == 0) return (NDEF_MSG_NO_MSG_END);

  /* p_rec should equal p_end if all the length fields were correct */
  if (p_rec != p_end) return (NDEF_MSG_LENGTH_MISMATCH);

  return (NDEF_OK);
}

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
int32_t NDEF_MsgGetNumRecs(uint8_t* p_msg) {
  uint8_t* p_rec = p_msg;
  uint8_t rec_hdr, type_len, id_len;
  int count;
  uint32_t payload_len;

  for (count = 0;;) {
    count++;

    rec_hdr = *p_rec++;

    if (rec_hdr & NDEF_ME_MASK) break;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);
  }

  /* Return the number of records found */
  return (count);
}

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
uint32_t NDEF_MsgGetRecLength(uint8_t* p_cur_rec) {
  uint8_t rec_hdr, type_len, id_len;
  uint32_t rec_len = 0;
  uint32_t payload_len;

  /* Get the current record's header */
  rec_hdr = *p_cur_rec++;
  rec_len++;

  /* Type field length */
  type_len = *p_cur_rec++;
  rec_len++;

  /* Payload length - can be 1 or 4 bytes */
  if (rec_hdr & NDEF_SR_MASK) {
    payload_len = *p_cur_rec++;
    rec_len++;
  } else {
    BE_STREAM_TO_UINT32(payload_len, p_cur_rec);
    rec_len += 4;
  }

  /* ID field Length */
  if (rec_hdr & NDEF_IL_MASK) {
    id_len = *p_cur_rec++;
    rec_len++;
  } else
    id_len = 0;

  /* Total length of record */
  rec_len += (payload_len + type_len + id_len);

  return (rec_len);
}

/*******************************************************************************
**
** Function         NDEF_MsgGetNextRec
**
** Description      This function gets a pointer to the next record in the given
**                  NDEF message. If the current record pointer is NULL, a
**                  pointer to the first record is returned.
**
** Returns          Pointer to the start of the record, or NULL if no more
**
*******************************************************************************/
uint8_t* NDEF_MsgGetNextRec(uint8_t* p_cur_rec) {
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  /* Get the current record's header */
  rec_hdr = *p_cur_rec++;

  /* If this is the last record, return NULL */
  if (rec_hdr & NDEF_ME_MASK) return (nullptr);

  /* Type field length */
  type_len = *p_cur_rec++;

  /* Payload length - can be 1 or 4 bytes */
  if (rec_hdr & NDEF_SR_MASK)
    payload_len = *p_cur_rec++;
  else
    BE_STREAM_TO_UINT32(payload_len, p_cur_rec);

  /* ID field Length */
  if (rec_hdr & NDEF_IL_MASK)
    id_len = *p_cur_rec++;
  else
    id_len = 0;

  /* Point to next record */
  p_cur_rec += (payload_len + type_len + id_len);

  return (p_cur_rec);
}

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
uint8_t* NDEF_MsgGetRecByIndex(uint8_t* p_msg, int32_t index) {
  uint8_t* p_rec = p_msg;
  uint8_t rec_hdr, type_len, id_len;
  int32_t count;
  uint32_t payload_len;

  for (count = 0;; count++) {
    if (count == index) return (p_rec);

    rec_hdr = *p_rec++;

    if (rec_hdr & NDEF_ME_MASK) return (nullptr);

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);
  }

  /* If here, there is no record of that index */
  return (nullptr);
}

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
uint8_t* NDEF_MsgGetLastRecInMsg(uint8_t* p_msg) {
  uint8_t* p_rec = p_msg;
  uint8_t* pRecStart;
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  for (;;) {
    pRecStart = p_rec;
    rec_hdr = *p_rec++;

    if (rec_hdr & NDEF_ME_MASK) break;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);
  }

  return (pRecStart);
}

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
uint8_t* NDEF_MsgGetFirstRecByType(uint8_t* p_msg, uint8_t tnf, uint8_t* p_type,
                                   uint8_t tlen) {
  uint8_t* p_rec = p_msg;
  uint8_t* pRecStart;
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  for (;;) {
    pRecStart = p_rec;

    rec_hdr = *p_rec++;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* At this point, p_rec points to the start of the type field. We need to */
    /* compare the type of the type, the length of the type and the data     */
    if (((rec_hdr & NDEF_TNF_MASK) == tnf) && (type_len == tlen) &&
        (!memcmp(p_rec, p_type, tlen)))
      return (pRecStart);

    /* If this was the last record, return NULL */
    if (rec_hdr & NDEF_ME_MASK) return (nullptr);

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);
  }

  /* If here, there is no record of that type */
  return (nullptr);
}

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
uint8_t* NDEF_MsgGetNextRecByType(uint8_t* p_cur_rec, uint8_t tnf,
                                  uint8_t* p_type, uint8_t tlen) {
  uint8_t* p_rec;
  uint8_t* pRecStart;
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  /* If this is the last record in the message, return NULL */
  p_rec = NDEF_MsgGetNextRec(p_cur_rec);
  if (p_rec == nullptr) return (nullptr);

  for (;;) {
    pRecStart = p_rec;

    rec_hdr = *p_rec++;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* At this point, p_rec points to the start of the type field. We need to */
    /* compare the type of the type, the length of the type and the data     */
    if (((rec_hdr & NDEF_TNF_MASK) == tnf) && (type_len == tlen) &&
        (!memcmp(p_rec, p_type, tlen)))
      return (pRecStart);

    /* If this was the last record, return NULL */
    if (rec_hdr & NDEF_ME_MASK) break;

    /* Point to next record */
    p_rec += (payload_len + type_len + id_len);
  }

  /* If here, there is no record of that type */
  return (nullptr);
}

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
uint8_t* NDEF_MsgGetFirstRecById(uint8_t* p_msg, uint8_t* p_id, uint8_t ilen) {
  uint8_t* p_rec = p_msg;
  uint8_t* pRecStart;
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  for (;;) {
    pRecStart = p_rec;

    rec_hdr = *p_rec++;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* At this point, p_rec points to the start of the type field. Skip it */
    p_rec += type_len;

    /* At this point, p_rec points to the start of the ID field. Compare length
     * and data */
    if ((id_len == ilen) && (!memcmp(p_rec, p_id, ilen))) return (pRecStart);

    /* If this was the last record, return NULL */
    if (rec_hdr & NDEF_ME_MASK) return (nullptr);

    /* Point to next record */
    p_rec += (id_len + payload_len);
  }

  /* If here, there is no record of that ID */
  return (nullptr);
}

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
uint8_t* NDEF_MsgGetNextRecById(uint8_t* p_cur_rec, uint8_t* p_id,
                                uint8_t ilen) {
  uint8_t* p_rec;
  uint8_t* pRecStart;
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  /* If this is the last record in the message, return NULL */
  p_rec = NDEF_MsgGetNextRec(p_cur_rec);
  if (p_rec == nullptr) return (nullptr);

  for (;;) {
    pRecStart = p_rec;

    rec_hdr = *p_rec++;

    /* Type field length */
    type_len = *p_rec++;

    /* Payload length - can be 1 or 4 bytes */
    if (rec_hdr & NDEF_SR_MASK)
      payload_len = *p_rec++;
    else
      BE_STREAM_TO_UINT32(payload_len, p_rec);

    /* ID field Length */
    if (rec_hdr & NDEF_IL_MASK)
      id_len = *p_rec++;
    else
      id_len = 0;

    /* At this point, p_rec points to the start of the type field. Skip it */
    p_rec += type_len;

    /* At this point, p_rec points to the start of the ID field. Compare length
     * and data */
    if ((id_len == ilen) && (!memcmp(p_rec, p_id, ilen))) return (pRecStart);

    /* If this was the last record, return NULL */
    if (rec_hdr & NDEF_ME_MASK) break;

    /* Point to next record */
    p_rec += (id_len + payload_len);
  }

  /* If here, there is no record of that ID */
  return (nullptr);
}

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
uint8_t* NDEF_RecGetType(uint8_t* p_rec, uint8_t* p_tnf, uint8_t* p_type_len) {
  uint8_t rec_hdr, type_len;

  /* First byte is the record header */
  rec_hdr = *p_rec++;

  /* Next byte is the type field length */
  type_len = *p_rec++;

  /* Skip the payload length */
  if (rec_hdr & NDEF_SR_MASK)
    p_rec += 1;
  else
    p_rec += 4;

  /* Skip ID field Length, if present */
  if (rec_hdr & NDEF_IL_MASK) p_rec++;

  /* At this point, p_rec points to the start of the type field.  */
  *p_type_len = type_len;
  *p_tnf = rec_hdr & NDEF_TNF_MASK;

  if (type_len == 0)
    return (nullptr);
  else
    return (p_rec);
}

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
uint8_t* NDEF_RecGetId(uint8_t* p_rec, uint8_t* p_id_len) {
  uint8_t rec_hdr, type_len;

  /* First byte is the record header */
  rec_hdr = *p_rec++;

  /* Next byte is the type field length */
  type_len = *p_rec++;

  /* Skip the payload length */
  if (rec_hdr & NDEF_SR_MASK)
    p_rec++;
  else
    p_rec += 4;

  /* ID field Length */
  if (rec_hdr & NDEF_IL_MASK)
    *p_id_len = *p_rec++;
  else
    *p_id_len = 0;

  /* p_rec now points to the start of the type field. The ID field follows it */
  if (*p_id_len == 0)
    return (nullptr);
  else
    return (p_rec + type_len);
}

/*******************************************************************************
**
** Function         NDEF_RecGetPayload
**
** Description      This function gets a pointer to the payload for the given
**                  NDEF record.
**
** Returns          a pointer to the payload (or NULL none). Payload len filled
**                  in.
**
*******************************************************************************/
uint8_t* NDEF_RecGetPayload(uint8_t* p_rec, uint32_t* p_payload_len) {
  uint8_t rec_hdr, type_len, id_len;
  uint32_t payload_len;

  /* First byte is the record header */
  rec_hdr = *p_rec++;

  /* Next byte is the type field length */
  type_len = *p_rec++;

  /* Next is the payload length (1 or 4 bytes) */
  if (rec_hdr & NDEF_SR_MASK)
    payload_len = *p_rec++;
  else
    BE_STREAM_TO_UINT32(payload_len, p_rec);

  *p_payload_len = payload_len;

  /* ID field Length */
  if (rec_hdr & NDEF_IL_MASK)
    id_len = *p_rec++;
  else
    id_len = 0;

  /* p_rec now points to the start of the type field. The ID field follows it,
   * then the payload */
  if (payload_len == 0)
    return (nullptr);
  else
    return (p_rec + type_len + id_len);
}

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
void NDEF_MsgInit(uint8_t* p_msg, uint32_t max_size, uint32_t* p_cur_size) {
  *p_cur_size = 0;
  memset(p_msg, 0, max_size);
}

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
                                   uint8_t* p_payload, uint32_t payload_len) {
  uint8_t* p_rec = p_msg + *p_cur_size;
  uint32_t recSize;
  int plen = (payload_len < 256) ? 1 : 4;
  int ilen = (id_len == 0) ? 0 : 1;

  if (tnf > NDEF_TNF_RESERVED) {
    tnf = NDEF_TNF_UNKNOWN;
    type_len = 0;
  }

  /* First, make sure the record will fit. we need at least 2 bytes for header
   * and type length */
  recSize = payload_len + 2 + type_len + plen + ilen + id_len;

  if ((*p_cur_size + recSize) > max_size) return (NDEF_MSG_INSUFFICIENT_MEM);

  /* Construct the record header. For the first record, set both begin and end
   * bits */
  if (*p_cur_size == 0)
    *p_rec = tnf | NDEF_MB_MASK | NDEF_ME_MASK;
  else {
    /* Find the previous last and clear his 'Message End' bit */
    uint8_t* pLast = NDEF_MsgGetLastRecInMsg(p_msg);

    if (!pLast) return (NDEF_MSG_NO_MSG_END);

    *pLast &= ~NDEF_ME_MASK;
    *p_rec = tnf | NDEF_ME_MASK;
  }

  if (plen == 1) *p_rec |= NDEF_SR_MASK;

  if (ilen != 0) *p_rec |= NDEF_IL_MASK;

  p_rec++;

  /* The next byte is the type field length */
  *p_rec++ = type_len;

  /* Payload length - can be 1 or 4 bytes */
  if (plen == 1)
    *p_rec++ = (uint8_t)payload_len;
  else
    UINT32_TO_BE_STREAM(p_rec, payload_len);

  /* ID field Length (optional) */
  if (ilen > 0) *p_rec++ = id_len;

  /* Next comes the type */
  if (type_len) {
    if (p_type) memcpy(p_rec, p_type, type_len);

    p_rec += type_len;
  }

  /* Next comes the ID */
  if (id_len) {
    if (p_id) memcpy(p_rec, p_id, id_len);

    p_rec += id_len;
  }

  /* And lastly the payload. If NULL, the app just wants to reserve memory */
  if (p_payload) memcpy(p_rec, p_payload, payload_len);

  *p_cur_size += recSize;

  return (NDEF_OK);
}

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
tNDEF_STATUS NDEF_MsgAppendPayload(uint8_t* p_msg, uint32_t max_size,
                                   uint32_t* p_cur_size, uint8_t* p_rec,
                                   uint8_t* p_add_pl, uint32_t add_pl_len) {
  uint32_t prev_paylen, new_paylen;
  uint8_t *p_prev_pl, *pp;
  uint8_t incr_lenfld = 0;
  uint8_t type_len, id_len;

  /* Skip header */
  pp = p_rec + 1;

  /* Next byte is the type field length */
  type_len = *pp++;

  /* Next is the payload length (1 or 4 bytes) */
  if (*p_rec & NDEF_SR_MASK)
    prev_paylen = *pp++;
  else
    BE_STREAM_TO_UINT32(prev_paylen, pp);

  /* ID field Length */
  if (*p_rec & NDEF_IL_MASK)
    id_len = *pp++;
  else
    id_len = 0;

  p_prev_pl = pp + type_len + id_len;

  new_paylen = prev_paylen + add_pl_len;

  /* Previous payload may be < 256, and this addition may make it larger than
   * 256 */
  /* If that were to happen, the payload length field goes from 1 byte to 4
   * bytes */
  if ((prev_paylen < 256) && (new_paylen > 255)) incr_lenfld = 3;

  /* Check that it all fits */
  if ((*p_cur_size + add_pl_len + incr_lenfld) > max_size)
    return (NDEF_MSG_INSUFFICIENT_MEM);

  /* Point to payload length field */
  pp = p_rec + 2;

  /* If we need to increase the length field from 1 to 4 bytes, do it first */
  if (incr_lenfld) {
    shiftdown(pp + 1, (uint32_t)(*p_cur_size - (pp - p_msg) - 1), 3);
    p_prev_pl += 3;
  }

  /* Store in the new length */
  if (new_paylen > 255) {
    *p_rec &= ~NDEF_SR_MASK;
    UINT32_TO_BE_STREAM(pp, new_paylen);
  } else
    *pp = (uint8_t)new_paylen;

  /* Point to the end of the previous payload */
  pp = p_prev_pl + prev_paylen;

  /* If we are not the last record, make space for the extra payload */
  if ((*p_rec & NDEF_ME_MASK) == 0)
    shiftdown(pp, (uint32_t)(*p_cur_size - (pp - p_msg)), add_pl_len);

  /* Now copy in the additional payload data */
  memcpy(pp, p_add_pl, add_pl_len);

  *p_cur_size += add_pl_len + incr_lenfld;

  return (NDEF_OK);
}

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
tNDEF_STATUS NDEF_MsgReplacePayload(uint8_t* p_msg, uint32_t max_size,
                                    uint32_t* p_cur_size, uint8_t* p_rec,
                                    uint8_t* p_new_pl, uint32_t new_pl_len) {
  uint32_t prev_paylen;
  uint8_t *p_prev_pl, *pp;
  uint32_t paylen_delta;
  uint8_t type_len, id_len;

  /* Skip header */
  pp = p_rec + 1;

  /* Next byte is the type field length */
  type_len = *pp++;

  /* Next is the payload length (1 or 4 bytes) */
  if (*p_rec & NDEF_SR_MASK)
    prev_paylen = *pp++;
  else
    BE_STREAM_TO_UINT32(prev_paylen, pp);

  /* ID field Length */
  if (*p_rec & NDEF_IL_MASK)
    id_len = *pp++;
  else
    id_len = 0;

  p_prev_pl = pp + type_len + id_len;

  /* Point to payload length field again */
  pp = p_rec + 2;

  if (new_pl_len > prev_paylen) {
    /* New payload is larger than the previous */
    paylen_delta = new_pl_len - prev_paylen;

    /* If the previous payload length was < 256, and new is > 255 */
    /* the payload length field goes from 1 byte to 4 bytes       */
    if ((prev_paylen < 256) && (new_pl_len > 255)) {
      if ((*p_cur_size + paylen_delta + 3) > max_size)
        return (NDEF_MSG_INSUFFICIENT_MEM);

      shiftdown(pp + 1, (uint32_t)(*p_cur_size - (pp - p_msg) - 1), 3);
      p_prev_pl += 3;
      *p_cur_size += 3;
      *p_rec &= ~NDEF_SR_MASK;
    } else if ((*p_cur_size + paylen_delta) > max_size)
      return (NDEF_MSG_INSUFFICIENT_MEM);

    /* Store in the new length */
    if (new_pl_len > 255) {
      UINT32_TO_BE_STREAM(pp, new_pl_len);
    } else
      *pp = (uint8_t)new_pl_len;

    /* Point to the end of the previous payload */
    pp = p_prev_pl + prev_paylen;

    /* If we are not the last record, make space for the extra payload */
    if ((*p_rec & NDEF_ME_MASK) == 0)
      shiftdown(pp, (uint32_t)(*p_cur_size - (pp - p_msg)), paylen_delta);

    *p_cur_size += paylen_delta;
  } else if (new_pl_len < prev_paylen) {
    /* New payload is smaller than the previous */
    paylen_delta = prev_paylen - new_pl_len;

    /* If the previous payload was > 256, and new is less than 256 */
    /* the payload length field goes from 4 bytes to 1 byte        */
    if ((prev_paylen > 255) && (new_pl_len < 256)) {
      shiftup(pp + 1, pp + 4, (uint32_t)(*p_cur_size - (pp - p_msg) - 3));
      p_prev_pl -= 3;
      *p_cur_size -= 3;
      *p_rec |= NDEF_SR_MASK;
    }

    /* Store in the new length */
    if (new_pl_len > 255) {
      UINT32_TO_BE_STREAM(pp, new_pl_len);
    } else
      *pp = (uint8_t)new_pl_len;

    /* Point to the end of the previous payload */
    pp = p_prev_pl + prev_paylen;

    /* If we are not the last record, remove the extra space from the previous
     * payload */
    if ((*p_rec & NDEF_ME_MASK) == 0)
      shiftup(pp - paylen_delta, pp, (uint32_t)(*p_cur_size - (pp - p_msg)));

    *p_cur_size -= paylen_delta;
  }

  /* Now copy in the new payload data */
  if (p_new_pl) memcpy(p_prev_pl, p_new_pl, new_pl_len);

  return (NDEF_OK);
}

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
tNDEF_STATUS NDEF_MsgReplaceType(uint8_t* p_msg, uint32_t max_size,
                                 uint32_t* p_cur_size, uint8_t* p_rec,
                                 uint8_t* p_new_type, uint8_t new_type_len) {
  uint8_t typelen_delta;
  uint8_t *p_prev_type, prev_type_len;
  uint8_t* pp;

  /* Skip header */
  pp = p_rec + 1;

  /* Next byte is the type field length */
  prev_type_len = *pp++;

  /* Skip the payload length */
  if (*p_rec & NDEF_SR_MASK)
    pp += 1;
  else
    pp += 4;

  if (*p_rec & NDEF_IL_MASK) pp++;

  /* Save pointer to the start of the type field */
  p_prev_type = pp;

  if (new_type_len > prev_type_len) {
    /* New type is larger than the previous */
    typelen_delta = new_type_len - prev_type_len;

    if ((*p_cur_size + typelen_delta) > max_size)
      return (NDEF_MSG_INSUFFICIENT_MEM);

    /* Point to the end of the previous type, and make space for the extra data
     */
    pp = p_prev_type + prev_type_len;
    shiftdown(pp, (uint32_t)(*p_cur_size - (pp - p_msg)), typelen_delta);

    *p_cur_size += typelen_delta;
  } else if (new_type_len < prev_type_len) {
    /* New type field is smaller than the previous */
    typelen_delta = prev_type_len - new_type_len;

    /* Point to the end of the previous type, and shift up to fill the the
     * unused space */
    pp = p_prev_type + prev_type_len;
    shiftup(pp - typelen_delta, pp, (uint32_t)(*p_cur_size - (pp - p_msg)));

    *p_cur_size -= typelen_delta;
  }

  /* Save in new type length */
  p_rec[1] = new_type_len;

  /* Now copy in the new type field data */
  if (p_new_type) memcpy(p_prev_type, p_new_type, new_type_len);

  return (NDEF_OK);
}

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
tNDEF_STATUS NDEF_MsgReplaceId(uint8_t* p_msg, uint32_t max_size,
                               uint32_t* p_cur_size, uint8_t* p_rec,
                               uint8_t* p_new_id, uint8_t new_id_len) {
  uint8_t idlen_delta;
  uint8_t *p_prev_id, *p_idlen_field;
  uint8_t prev_id_len, type_len;
  uint8_t* pp;

  /* Skip header */
  pp = p_rec + 1;

  /* Next byte is the type field length */
  type_len = *pp++;

  /* Skip the payload length */
  if (*p_rec & NDEF_SR_MASK)
    pp += 1;
  else
    pp += 4;

  p_idlen_field = pp;

  if (*p_rec & NDEF_IL_MASK)
    prev_id_len = *pp++;
  else
    prev_id_len = 0;

  /* Save pointer to the start of the ID field (right after the type field) */
  p_prev_id = pp + type_len;

  if (new_id_len > prev_id_len) {
    /* New ID field is larger than the previous */
    idlen_delta = new_id_len - prev_id_len;

    /* If the previous ID length was 0, we need to add a 1-byte ID length */
    if (prev_id_len == 0) {
      if ((*p_cur_size + idlen_delta + 1) > max_size)
        return (NDEF_MSG_INSUFFICIENT_MEM);

      shiftdown(p_idlen_field,
                (uint32_t)(*p_cur_size - (p_idlen_field - p_msg)), 1);
      p_prev_id += 1;
      *p_cur_size += 1;
      *p_rec |= NDEF_IL_MASK;
    } else if ((*p_cur_size + idlen_delta) > max_size)
      return (NDEF_MSG_INSUFFICIENT_MEM);

    /* Point to the end of the previous ID field, and make space for the extra
     * data */
    pp = p_prev_id + prev_id_len;
    shiftdown(pp, (uint32_t)(*p_cur_size - (pp - p_msg)), idlen_delta);

    *p_cur_size += idlen_delta;
  } else if (new_id_len < prev_id_len) {
    /* New ID field is smaller than the previous */
    idlen_delta = prev_id_len - new_id_len;

    /* Point to the end of the previous ID, and shift up to fill the the unused
     * space */
    pp = p_prev_id + prev_id_len;
    shiftup(pp - idlen_delta, pp, (uint32_t)(*p_cur_size - (pp - p_msg)));

    *p_cur_size -= idlen_delta;

    /* If removing the ID, make sure that length field is also removed */
    if (new_id_len == 0) {
      shiftup(p_idlen_field, p_idlen_field + 1,
              (uint32_t)(*p_cur_size - (p_idlen_field - p_msg - (uint32_t)1)));
      *p_rec &= ~NDEF_IL_MASK;
      *p_cur_size -= 1;
    }
  }

  /* Save in new ID length and data */
  if (new_id_len) {
    *p_idlen_field = new_id_len;

    if (p_new_id) memcpy(p_prev_id, p_new_id, new_id_len);
  }

  return (NDEF_OK);
}

/*******************************************************************************
**
** Function         NDEF_MsgRemoveRec
**
** Description      This function removes the record at the given
**                  index in the given NDEF message.
**
** Returns          TRUE if OK, FALSE if the index was invalid
**                  *p_cur_size is updated
**
*******************************************************************************/
tNDEF_STATUS NDEF_MsgRemoveRec(uint8_t* p_msg, uint32_t* p_cur_size,
                               int32_t index) {
  uint8_t* p_rec = NDEF_MsgGetRecByIndex(p_msg, index);
  uint8_t *pNext, *pPrev;

  if (!p_rec) return (NDEF_REC_NOT_FOUND);

  /* If this is the first record in the message... */
  if (*p_rec & NDEF_MB_MASK) {
    /* Find the second record (if any) and set his 'Message Begin' bit */
    pNext = NDEF_MsgGetRecByIndex(p_msg, 1);
    if (pNext != nullptr) {
      *pNext |= NDEF_MB_MASK;

      *p_cur_size -= (uint32_t)(pNext - p_msg);

      shiftup(p_msg, pNext, *p_cur_size);
    } else
      *p_cur_size = 0; /* No more records, lenght must be zero */

    return (NDEF_OK);
  }

  /* If this is the last record in the message... */
  if (*p_rec & NDEF_ME_MASK) {
    if (index > 0) {
      /* Find the previous record and set his 'Message End' bit */
      pPrev = NDEF_MsgGetRecByIndex(p_msg, index - 1);
      if (pPrev == nullptr) return false;

      *pPrev |= NDEF_ME_MASK;
    }
    *p_cur_size = (uint32_t)(p_rec - p_msg);

    return (NDEF_OK);
  }

  /* Not the first or the last... get the address of the next record */
  pNext = NDEF_MsgGetNextRec(p_rec);
  if (pNext == nullptr) return false;

  /* We are removing p_rec, so shift from pNext to the end */
  shiftup(p_rec, pNext, (uint32_t)(*p_cur_size - (pNext - p_msg)));

  *p_cur_size -= (uint32_t)(pNext - p_rec);

  return (NDEF_OK);
}

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
tNDEF_STATUS NDEF_MsgCopyAndDechunk(uint8_t* p_src, uint32_t src_len,
                                    uint8_t* p_dest, uint32_t* p_out_len) {
  uint32_t out_len, max_out_len;
  uint8_t* p_rec;
  uint8_t* p_prev_rec = p_dest;
  uint8_t *p_type, *p_id, *p_pay;
  uint8_t type_len, id_len, tnf;
  uint32_t pay_len;
  tNDEF_STATUS status;

  /* First, validate the source */
  status = NDEF_MsgValidate(p_src, src_len, true);
  if (status != NDEF_OK) return (status);

  /* The output buffer must be at least as large as the input buffer */
  max_out_len = src_len;

  /* Initialize output */
  NDEF_MsgInit(p_dest, max_out_len, &out_len);

  p_rec = p_src;

  /* Now, copy record by record */
  while ((p_rec != nullptr) && (status == NDEF_OK)) {
    p_type = NDEF_RecGetType(p_rec, &tnf, &type_len);
    p_id = NDEF_RecGetId(p_rec, &id_len);
    p_pay = NDEF_RecGetPayload(p_rec, &pay_len);

    /* If this is the continuation of a chunk, append the payload to the
     * previous */
    if (tnf == NDEF_TNF_UNCHANGED) {
      if (p_pay) {
        status = NDEF_MsgAppendPayload(p_dest, max_out_len, &out_len,
                                       p_prev_rec, p_pay, pay_len);
      }
    } else {
      p_prev_rec = p_dest + out_len;

      status = NDEF_MsgAddRec(p_dest, max_out_len, &out_len, tnf, p_type,
                              type_len, p_id, id_len, p_pay, pay_len);
    }

    p_rec = NDEF_MsgGetNextRec(p_rec);
  }

  *p_out_len = out_len;

  return (status);
}
