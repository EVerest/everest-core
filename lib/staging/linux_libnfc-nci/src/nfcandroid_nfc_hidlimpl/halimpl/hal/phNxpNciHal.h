/*
 * Copyright (C) 2010-2019 NXP Semiconductors
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
#ifndef _PHNXPNCIHAL_H_
#define _PHNXPNCIHAL_H_

#include "NxpMfcReader.h"
#include "NxpNfcCapability.h"
//#include <hardware/nfc.h>
#include <phNxpNciHal_utils.h>
#include "hal_nxpnfc.h"
#include "phNxpNciHal.h"
#include "nfc_hal_api.h"
#ifndef ANDROID
#include "nfc.h"
#endif
/********************* Definitions and structures *****************************/
#define MAX_RETRY_COUNT 5
#define NCI_MAX_DATA_LEN 300
#define NCI_POLL_DURATION 500
#define HAL_NFC_ENABLE_I2C_FRAGMENTATION_EVT 0x07
#undef P2P_PRIO_LOGIC_HAL_IMP
#define NCI_VERSION_2_0 0x20
#define NCI_VERSION_1_1 0x11
#define NCI_VERSION_1_0 0x10
#define NCI_VERSION_UNKNOWN 0x00
typedef void(phNxpNciHal_control_granted_callback_t)();

/*ROM CODE VERSION FW*/
#define FW_MOBILE_ROM_VERSION_PN548AD 0x10
#define FW_MOBILE_ROM_VERSION_PN551 0x10
#define FW_MOBILE_ROM_VERSION_PN553 0x11
#define FW_MOBILE_ROM_VERSION_PN557 0x12
/* NCI Data */

#define NCI_MT_CMD 0x20
#define NCI_MT_RSP 0x40
#define NCI_MT_NTF 0x60

#define CORE_RESET_TRIGGER_TYPE_CORE_RESET_CMD_RECEIVED 0x02
#define CORE_RESET_TRIGGER_TYPE_POWERED_ON 0x01
#define NCI_MSG_CORE_RESET 0x00
#define NCI_MSG_CORE_INIT 0x01
#define NCI_MT_MASK 0xE0
#define NCI_OID_MASK 0x3F

#define NXP_MAX_CONFIG_STRING_LEN 260
#define NCI_HEADER_SIZE 3

typedef struct nci_data {
  uint16_t len;
  uint8_t p_data[NCI_MAX_DATA_LEN];
} nci_data_t;

typedef enum {
  HAL_STATUS_CLOSE = 0,
  HAL_STATUS_OPEN,
  HAL_STATUS_MIN_OPEN
} phNxpNci_HalStatus;

typedef enum {
  GPIO_UNKNOWN = 0x00,
  GPIO_STORE = 0x01,
  GPIO_STORE_DONE = 0x02,
  GPIO_RESTORE = 0x10,
  GPIO_RESTORE_DONE = 0x20,
  GPIO_CLEAR = 0xFF
} phNxpNciHal_GpioInfoState;

typedef struct phNxpNciGpioInfo {
  phNxpNciHal_GpioInfoState state;
  uint8_t values[2];
} phNxpNciGpioInfo_t;

/* Macros to enable and disable extensions */
#define HAL_ENABLE_EXT() (nxpncihal_ctrl.hal_ext_enabled = 1)
#define HAL_DISABLE_EXT() (nxpncihal_ctrl.hal_ext_enabled = 0)
typedef struct phNxpNciInfo {
  uint8_t nci_version;
  bool_t wait_for_ntf;
} phNxpNciInfo_t;
/* NCI Control structure */
typedef struct phNxpNciHal_Control {
  tNFC_chipType nfcChipType;
  phNxpNci_HalStatus halStatus; /* Indicate if hal is open or closed */
  pthread_t client_thread;      /* Integration thread handle */
  uint8_t thread_running;       /* Thread running if set to 1, else set to 0 */
  phLibNfc_sConfig_t gDrvCfg;   /* Driver config data */

  /* Rx data */
  uint8_t* p_rx_data;
  uint16_t rx_data_len;

  /* Rx data */
  uint8_t* p_rx_ese_data;
  uint16_t rx_ese_data_len;

  /* libnfc-nci callbacks */
  nfc_stack_callback_t* p_nfc_stack_cback;
  nfc_stack_data_callback_t* p_nfc_stack_data_cback;

  /* control granted callback */
  phNxpNciHal_control_granted_callback_t* p_control_granted_cback;

  /* HAL open status */
  bool_t hal_open_status;

  /* HAL extensions */
  uint8_t hal_ext_enabled;

  /* Waiting semaphore */
  phNxpNciHal_Sem_t ext_cb_data;
  sem_t syncSpiNfc;

  uint16_t cmd_len;
  uint8_t p_cmd_data[NCI_MAX_DATA_LEN];
  uint16_t rsp_len;
  uint8_t p_rsp_data[NCI_MAX_DATA_LEN];

  /* retry count used to force download */
  uint16_t retry_cnt;
  uint8_t read_retry_cnt;
  phNxpNciInfo_t nci_info;

  /* to store and restore gpio values */
  phNxpNciGpioInfo_t phNxpNciGpioInfo;
  bool bIsForceFwDwnld;
} phNxpNciHal_Control_t;

typedef struct phNxpNciClock {
  bool_t isClockSet;
  uint8_t p_rx_data[20];
  bool_t issetConfig;
} phNxpNciClock_t;

typedef struct phNxpNciRfSetting {
  bool_t isGetRfSetting;
  uint8_t p_rx_data[20];
} phNxpNciRfSetting_t;

typedef struct phNxpNciMwEepromArea {
  bool_t isGetEepromArea;
  uint8_t p_rx_data[32];
} phNxpNciMwEepromArea_t;

typedef enum {
  NFC_FORUM_PROFILE,
  EMV_CO_PROFILE,
  INVALID_PROFILe
} phNxpNciProfile_t;
/* NXP Poll Profile control structure */
typedef struct phNxpNciProfile_Control {
  phNxpNciProfile_t profile_type;
  uint8_t bClkSrcVal; /* Holds the System clock source read from config file */
  uint8_t
      bClkFreqVal;  /* Holds the System clock frequency read from config file */
  uint8_t bTimeout; /* Holds the Timeout Value */
} phNxpNciProfile_Control_t;

/* Internal messages to handle callbacks */
#define NCI_HAL_OPEN_CPLT_MSG 0x411
#define NCI_HAL_CLOSE_CPLT_MSG 0x412
#define NCI_HAL_POST_INIT_CPLT_MSG 0x413
#define NCI_HAL_PRE_DISCOVER_CPLT_MSG 0x414
#define NCI_HAL_ERROR_MSG 0x415
#define NCI_HAL_HCI_NETWORK_RESET_MSG 0x416
#define NCI_HAL_RX_MSG 0xF01

#define NCIHAL_CMD_CODE_LEN_BYTE_OFFSET (2U)
#define NCIHAL_CMD_CODE_BYTE_LEN (3U)

/******************** NCI HAL exposed functions *******************************/
tNFC_chipType phNxpNciHal_getChipType(void);
int phNxpNciHal_check_ncicmd_write_window(uint16_t cmd_len, uint8_t* p_cmd);
void phNxpNciHal_request_control(void);
void phNxpNciHal_release_control(void);
int phNxpNciHal_write_unlocked(uint16_t data_len, const uint8_t* p_data);
NFCSTATUS phNxpNciHal_core_reset_recovery();
void phNxpNciHal_discovery_cmd_ext(uint8_t* p_cmd_data, uint16_t cmd_len);
/*******************************************************************************
**
** Function         phNxpNciHal_configFeatureList
**
** Description      Configures the featureList based on chip type

** Returns          none
*******************************************************************************/
void phNxpNciHal_configFeatureList(uint8_t* init_rsp, uint16_t rsp_len);
#if(NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         phNxpNciHal_getNxpConfig
**
** Description      Read vendor configuration macro values
**
** Parameters       ioctl input/output struct.
**
** Returns          none
*******************************************************************************/
void phNxpNciHal_getNxpConfig(nfc_nci_IoctlInOutData_t *pInpOutData);
#endif
#endif /* _PHNXPNCIHAL_H_ */
