/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
#define NULL 0
#endif

#ifndef false
#define false 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef uint32_t TIME_STAMP;

#ifndef true
#define true (!false)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif
typedef unsigned char UBYTE;

#ifdef __arm
#define PACKED __packed
#define INLINE __inline
#else
#define PACKED
#define INLINE
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN false
#endif

#define UINT16_LOW_BYTE(x) ((x)&0xff)
#define UINT16_HI_BYTE(x) ((x) >> 8)

#endif
