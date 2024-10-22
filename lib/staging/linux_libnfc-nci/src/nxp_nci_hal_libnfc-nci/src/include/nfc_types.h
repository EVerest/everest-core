/******************************************************************************
 *
 *  Copyright (C) 2012-2014 Broadcom Corporation
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

#ifndef NFC_TYPES_H
#define NFC_TYPES_H

/****************************************************************************
** NFC_HDR header definition for NFC messages
*****************************************************************************/
typedef struct {
  uint16_t event;
  uint16_t len;
  uint16_t offset;
  uint16_t layer_specific;
} NFC_HDR;
#define NFC_HDR_SIZE (sizeof(NFC_HDR))

/* Mask for NFC_HDR event field */
#define NFC_EVT_MASK 0xFF00

/****************************************************************************
** NFC_HAL_TASK  definitions
*****************************************************************************/

/* NCI message for hci persistency data     */
#define NFC_HAL_EVT_HCI 0x0400

/* NCI message for sending to host stack    */
#define NFC_EVT_TO_NFC_NCI 0x4000

/*****************************************************************************
** Macros to get and put bytes to and from a stream (Little Endian format).
*****************************************************************************/

#define UINT32_TO_STREAM(p, u32)     \
  {                                  \
    *(p)++ = (uint8_t)(u32);         \
    *(p)++ = (uint8_t)((u32) >> 8);  \
    *(p)++ = (uint8_t)((u32) >> 16); \
    *(p)++ = (uint8_t)((u32) >> 24); \
  }
#define UINT16_TO_STREAM(p, u16)    \
  {                                 \
    *(p)++ = (uint8_t)(u16);        \
    *(p)++ = (uint8_t)((u16) >> 8); \
  }
#define UINT8_TO_STREAM(p, u8) \
  { *(p)++ = (uint8_t)(u8); }
#define INT8_TO_STREAM(p, u8) \
  { *(p)++ = (int8_t)(u8); }
#define ARRAY8_TO_STREAM(p, a)                                    \
  {                                                               \
    int ijk;                                                      \
    for (ijk = 0; ijk < 8; ijk++) *(p)++ = (uint8_t)(a)[7 - ijk]; \
  }
#define ARRAY_TO_STREAM(p, a, len)                                \
  {                                                               \
    int ijk;                                                      \
    for (ijk = 0; ijk < (len); ijk++) *(p)++ = (uint8_t)(a)[ijk]; \
  }
#define STREAM_TO_UINT8(u8, p) \
  {                            \
    (u8) = (uint8_t)(*(p));    \
    (p) += 1;                  \
  }
#define STREAM_TO_UINT16(u16, p)                                  \
  {                                                               \
    (u16) = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); \
    (p) += 2;                                                     \
  }
#define STREAM_TO_UINT32(u32, p)                                      \
  {                                                                   \
    (u32) = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + \
             ((((uint32_t)(*((p) + 2)))) << 16) +                     \
             ((((uint32_t)(*((p) + 3)))) << 24));                     \
    (p) += 4;                                                         \
  }
#define STREAM_TO_ARRAY8(a, p)                     \
  {                                                \
    int ijk;                                       \
    uint8_t* _pa = (uint8_t*)(a) + 7;              \
    for (ijk = 0; ijk < 8; ijk++) *_pa-- = *(p)++; \
  }
#define STREAM_TO_ARRAY(a, p, len)                                   \
  {                                                                  \
    int ijk;                                                         \
    for (ijk = 0; ijk < (len); ijk++) ((uint8_t*)(a))[ijk] = *(p)++; \
  }

/*****************************************************************************
** Macros to get and put bytes to and from a stream (Big Endian format)
*****************************************************************************/

#define UINT32_TO_BE_STREAM(p, u32)  \
  {                                  \
    *(p)++ = (uint8_t)((u32) >> 24); \
    *(p)++ = (uint8_t)((u32) >> 16); \
    *(p)++ = (uint8_t)((u32) >> 8);  \
    *(p)++ = (uint8_t)(u32);         \
  }
#define UINT24_TO_BE_STREAM(p, u24)  \
  {                                  \
    *(p)++ = (uint8_t)((u24) >> 16); \
    *(p)++ = (uint8_t)((u24) >> 8);  \
    *(p)++ = (uint8_t)(u24);         \
  }
#define UINT16_TO_BE_STREAM(p, u16) \
  {                                 \
    *(p)++ = (uint8_t)((u16) >> 8); \
    *(p)++ = (uint8_t)(u16);        \
  }
#define UINT8_TO_BE_STREAM(p, u8) \
  { *(p)++ = (uint8_t)(u8); }
#define ARRAY_TO_BE_STREAM(p, a, len)                             \
  {                                                               \
    int ijk;                                                      \
    for (ijk = 0; ijk < (len); ijk++) *(p)++ = (uint8_t)(a)[ijk]; \
  }

#define BE_STREAM_TO_UINT8(u8, p) \
  {                               \
    (u8) = (uint8_t)(*(p));       \
    (p) += 1;                     \
  }
#define BE_STREAM_TO_UINT16(u16, p)                                       \
  {                                                                       \
    (u16) = (uint16_t)(((uint16_t)(*(p)) << 8) + (uint16_t)(*((p) + 1))); \
    (p) += 2;                                                             \
  }
#define BE_STREAM_TO_UINT32(u32, p)                                      \
  {                                                                      \
    (u32) = ((uint32_t)(*((p) + 3)) + ((uint32_t)(*((p) + 2)) << 8) +    \
             ((uint32_t)(*((p) + 1)) << 16) + ((uint32_t)(*(p)) << 24)); \
    (p) += 4;                                                            \
  }

/*****************************************************************************
** Macros to get and put bytes to and from a field (Big Endian format).
** These are the same as to stream, except the pointer is not incremented.
*****************************************************************************/

#define UINT8_TO_BE_FIELD(p, u8) \
  { *(uint8_t*)(p) = (uint8_t)(u8); }

#endif /* NFC_TYPES_H */
