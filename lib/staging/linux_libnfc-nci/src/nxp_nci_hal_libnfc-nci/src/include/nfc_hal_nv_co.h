/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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
 *  This is the interface file for storing nv data
 *
 ******************************************************************************/
#ifndef NFC_HAL_NV_CO_H
#define NFC_HAL_NV_CO_H

#include "nfc_hal_target.h"

#if (NFC_HAL_HCI_INCLUDED == TRUE)

#include <time.h>

/*****************************************************************************
**  Constants and Data Types
*****************************************************************************/

/**************************
**  Common Definitions
***************************/

typedef uint8_t tNFC_HAL_NV_CO_STATUS;

#define DH_NV_BLOCK 0x01
#define HC_F3_NV_BLOCK 0x02
#define HC_F4_NV_BLOCK 0x03
#define HC_F2_NV_BLOCK 0x04
#define HC_F5_NV_BLOCK 0x05

/*****************************************************************************
**  Function Declarations
*****************************************************************************/
/**************************
**  Common Functions
***************************/

#endif /* NFC_HAL_HCI_INCLUDED */
#endif /* NFC_HAL_NV_CO_H */
