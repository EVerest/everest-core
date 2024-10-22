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
#ifndef __BUILDCFG_H
#define __BUILDCFG_H
//#include <cutils/memory.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "data_types.h"

#ifndef NFC_CONTORLLER_ID
#define NFC_CONTORLLER_ID (1)
#endif

#define GKI_BUF1_MAX 0
// 2 is in use
#define GKI_BUF3_MAX 30
#define GKI_BUF4_SIZE 2400
#define GKI_BUF4_MAX 30
#define GKI_BUF5_MAX 0
#define GKI_BUF6_MAX 0
#define GKI_BUF7_MAX 0
#define GKI_BUF8_MAX 0

#define GKI_BUF2_SIZE 660
#define GKI_BUF2_MAX 50

#define GKI_BUF0_SIZE 268
#define GKI_BUF0_MAX 40

#define GKI_NUM_FIXED_BUF_POOLS 4

#endif
