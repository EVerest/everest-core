/*
 * Copyright (C) 2010-2014 NXP Semiconductors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * NFC Component ID Values - Used for Function Return Codes
 */

#ifndef PHNFCCOMPID_H
#define PHNFCCOMPID_H

/*
 *  Component IDs
 *
 *  IDs for all NFC components. Combined with the Status Code they build the
 * value (status)
 *  returned by each function.
 *
 *  ID Number Spaces:
 *  - 01..1F: HAL
 *  - 20..3F: NFC-MW (Local Device)
 *  - 40..5F: NFC-MW (Remote Device)
 *  .
 *
 *         The value CID_NFC_NONE does not exist for Component IDs. Do not use
 * this value except
 *         for NFCSTATUS_SUCCESS. The enumeration function uses CID_NFC_NONE
 *         to mark unassigned "References".
 */
/* Unassigned or doesn't apply (see #NFCSTATUS_SUCCESS) */
#define CID_NFC_NONE 0x00
#define CID_NFC_TML 0x01 /* Transport Mapping Layer */
#define CID_NFC_LLC 0x07 /* Logical Link Control Layer */
/* NFC Controller(NFCC) Interface Layer */
#define CID_NFC_NCI 0x08
/* Firmware Download Management Layer */
#define CID_NFC_DNLD 0x09
#define CID_NFC_HAL 0x10 /* Hardware Abstraction Layer */
/* Operating System Abstraction Layer*/
#define CID_NFC_OSAL CID_NFC_NONE
#define CID_FRI_NFC_OVR_HAL 0x20       /* NFC-Device, HAL-based */
#define CID_FRI_NFC_NDEF_RECORD 0x22   /* NDEF Record Tools Library. */
#define CID_FRI_NFC_NDEF_MAP 0x23      /* NDEF Mapping. */
#define CID_FRI_NFC_NDEF_REGISTRY 0x24 /* NDEF_REGISTRY. */
/* Automatic Device Discovery. */
#define CID_FRI_NFC_AUTO_DEV_DIS 0x25
#define CID_FRI_NFC_NDEF_SMTCRDFMT 0x26 /* Smart Card Formatting */
#define CID_NFC_LIB 0x30                /* NFC Library Layer*/
/* The maximum CID value that is defined. */
#define CID_MAX_VALUE 0xF0
/* Logical Link Control Protocol */
#define CID_FRI_NFC_LLCP 0x40
#define CID_FRI_NFC_LLCP_TRANSPORT 0x50
#define CID_FRI_NFC_LLCP_MAC 0x60

#endif /* PHNFCCOMPID_H */
