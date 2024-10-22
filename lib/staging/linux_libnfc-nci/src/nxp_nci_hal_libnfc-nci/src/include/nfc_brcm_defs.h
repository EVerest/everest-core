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

/******************************************************************************
 *
 *  This file contains the Broadcom-specific defintions that are shared
 *  between HAL, nfc stack, adaptation layer and applications.
 *
 ******************************************************************************/

#ifndef NFC_BRCM_DEFS_H
#define NFC_BRCM_DEFS_H
#include "vendor_cfg.h"
/**********************************************
 * NCI Message Proprietary  Group       - F
 **********************************************/
#define NCI_MSG_GET_BUILD_INFO 0x04
#define NCI_MSG_HCI_NETWK 0x05
#define NCI_MSG_POWER_LEVEL 0x08
#define NCI_MSG_UICC_READER_ACTION 0x0A
/* reset HCI network/close all pipes (S,D) register */
#define NCI_MSG_GET_NV_DEVICE 0x24
#define NCI_MSG_LPTD 0x25
#define NCI_MSG_EEPROM_RW 0x29
#define NCI_MSG_GET_PATCH_VERSION 0x2D
#define NCI_MSG_SECURE_PATCH_DOWNLOAD 0x2E

/* Secure Patch Download definitions (patch type definitions) */
#define NCI_SPD_TYPE_HEADER 0x00

/**********************************************
 * NCI Interface Types
 **********************************************/
#define NCI_INTERFACE_VS_MIFARE NCI_PROTOCOL_MIFARE
#define NCI_INTERFACE_VS_T2T_CE 0x82 /* for Card Emulation side */

#define NFC_SNOOZE_MODE_UART 0x01    /* Snooze mode for UART    */

#define NFC_SNOOZE_ACTIVE_LOW 0x00  /* high to low voltage is asserting */

/**********************************************
 * HCI definitions
 **********************************************/
#define NFC_HAL_HCI_SESSION_ID_LEN 8
#define NFC_HAL_HCI_SYNC_ID_LEN 2

/* Card emulation RF Gate A definitions */
#define NFC_HAL_HCI_CE_RF_A_UID_REG_LEN 10
#define NFC_HAL_HCI_CE_RF_A_ATQA_RSP_CODE_LEN 2
#define NFC_HAL_HCI_CE_RF_A_MAX_HIST_DATA_LEN 15
#define NFC_HAL_HCI_CE_RF_A_MAX_DATA_RATE_LEN 3

/* Card emulation RF Gate B definitions */
#define NFC_HAL_HCI_CE_RF_B_PUPI_LEN 4
#define NFC_HAL_HCI_CE_RF_B_ATQB_LEN 4
#define NFC_HAL_HCI_CE_RF_B_HIGHER_LAYER_RSP_LEN 61
#define NFC_HAL_HCI_CE_RF_B_MAX_DATA_RATE_LEN 3

/* Card emulation RF Gate BP definitions */
#define NFC_HAL_HCI_CE_RF_BP_MAX_PAT_IN_LEN 8
#define NFC_HAL_HCI_CE_RF_BP_DATA_OUT_LEN 40

/* Reader RF Gate A definitions */
#define NFC_HAL_HCI_RD_RF_B_HIGHER_LAYER_DATA_LEN 61

/* DH HCI Network command definitions */
#define NFC_HAL_HCI_DH_MAX_DYN_PIPES 20

#endif /* NFC_BRCM_DEFS_H */
