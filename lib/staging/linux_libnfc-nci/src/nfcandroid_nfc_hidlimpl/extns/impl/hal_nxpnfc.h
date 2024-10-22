/******************************************************************************
 *
 *  Copyright 2018,2020 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#ifndef ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H
#define ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H

#define NFC_NCI_NXP_PN54X_HARDWARE_MODULE_ID "nfc_nci.pn54x"
#define MAX_IOCTL_TRANSCEIVE_CMD_LEN 256
#define MAX_IOCTL_TRANSCEIVE_RESP_LEN 256
#define MAX_ATR_INFO_LEN 128

enum {
  // HAL_NFC_ENABLE_I2C_FRAGMENTATION_EVT = 0x07,
  HAL_NFC_POST_MIN_INIT_CPLT_EVT = 0x08
};

enum {
  HAL_NFC_GET_NXP_CONFIG = 30,
};

/*
 * Data structures provided below are used of Hal Ioctl calls
 */
/*
 * nfc_nci_ExtnCmd_t shall contain data for commands used for transceive command
 * in ioctl
 */
typedef struct {
  uint16_t cmd_len;
  uint8_t p_cmd[MAX_IOCTL_TRANSCEIVE_CMD_LEN];
} nfc_nci_ExtnCmd_t;

/*
 * nfc_nci_ExtnRsp_t shall contain response for command sent in transceive
 * command
 */
typedef struct {
  uint16_t rsp_len;
  uint8_t p_rsp[MAX_IOCTL_TRANSCEIVE_RESP_LEN];
} nfc_nci_ExtnRsp_t;
/*
 * InputData_t :ioctl has multiple subcommands
 * Each command has corresponding input data which needs to be populated in this
 */
typedef union {
  uint16_t bootMode;
  uint8_t halType;
  nfc_nci_ExtnCmd_t nciCmd;
  uint32_t timeoutMilliSec;
  long nfcServicePid;
} InputData_t;
/*
 * nfc_nci_ExtnInputData_t :Apart from InputData_t, there are context data
 * which is required during callback from stub to proxy.
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, context
 * data
 * is exposed to libnfc (Not encapsulated).
 */
typedef struct {
  /*context to be used/updated only by users of proxy & stub of Nfc.hal
  * i.e, NfcAdaptation & hardware/interface/Nfc.
  */
  void* context;
  InputData_t data;
  uint8_t data_source;
  long level;
} nfc_nci_ExtnInputData_t;

typedef struct {
  uint8_t wAgcDebugEnable;
  uint8_t wT4TNdefEnable;
  uint8_t wT4TPowerState;
} nxp_nfc_config_t;

/*
 * outputData_t :ioctl has multiple commands/responses
 * This contains the output types for each ioctl.
 */
typedef union {
  uint32_t status;
  nfc_nci_ExtnRsp_t nciRsp;
  uint8_t nxpNciAtrInfo[MAX_ATR_INFO_LEN];
  uint32_t p61CurrentState;
  uint16_t fwUpdateInf;
  uint16_t fwDwnldStatus;
  uint16_t fwMwVerStatus;
  uint8_t chipType;
  nxp_nfc_config_t nxpConfigs;
} outputData_t;

/*
 * nfc_nci_ExtnOutputData_t :Apart from outputData_t, there are other
 * information
 * which is required during callback from stub to proxy.
 * For ex (context, result of the operation , type of ioctl which was
 * completed).
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, these data
 * is exposed(Not encapsulated).
 */
typedef struct {
  /*ioctlType, result & context to be used/updated only by users of
   * proxy & stub of Nfc.hal.
   * i.e, NfcAdaptation & hardware/interface/Nfc
   * These fields shall not be used by libnfc or halimplementation*/
  uint64_t ioctlType;
  uint32_t result;
  void* context;
  outputData_t data;
} nfc_nci_ExtnOutputData_t;

/*
 * nfc_nci_IoctlInOutData_t :data structure for input & output
 * to be sent for ioctl command. input is populated by client/proxy side
 * output is provided from server/stub to client/proxy
 */
typedef struct {
  nfc_nci_ExtnInputData_t inp;
  nfc_nci_ExtnOutputData_t out;
} nfc_nci_IoctlInOutData_t;

/*
 * nxpnfc_nci_device_t :data structure for nxp's extended nfc_nci_device
 * Extra features added are
 * -ioctl(manage sync between  and DWP & SPI)
 * -check request for fw download
 */
typedef struct nxpnfc_nci_device {
  // nfc_nci_device_t nci_device;
  /*
  * (*ioctl)() For P61 power management synchronization
  * between NFC Wired and SPI.
  */
  int (*ioctl)(const struct nxpnfc_nci_device* p_dev, long arg, void* p_data);
  /*
  * (*check_fw_dwnld_flag)() Is called to get FW downlaod request.
  */
  int (*check_fw_dwnld_flag)(const struct nxpnfc_nci_device* p_dev,
                             uint8_t* param1);
} nxpnfc_nci_device_t;

#endif  // ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H
