/*
 * Copyright (C) 2012-2019 NXP Semiconductors
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
#include "NfccPowerTracker.h"
#include "hal_nxpese.h"
#include "hal_nxpnfc.h"
#include "spi_spm.h"
#ifdef ANDROID
#include <EseAdaptation.h>
#include <cutils/properties.h>
#endif
#include <log/log.h>
#include <phDal4Nfc_messageQueueLib.h>
#include <phDnldNfc.h>
#include <phNxpConfig.h>
#include <phNxpLog.h>
#include <phNxpNciHal.h>
#include <phNxpNciHal_Adaptation.h>
#include <phNxpNciHal_Dnld.h>
#include <phNxpNciHal_NfcDepSWPrio.h>
#include <phNxpNciHal_ext.h>
#include <phTmlNfc.h>
#include <sys/stat.h>
#include "types.h"
#include "bt_trace.h"
#include "NfcAdaptation.h"
using namespace android::hardware::nfc::V1_1;
//using namespace android::hardware::nfc::V1_2;
using android::hardware::nfc::V1_1::NfcEvent;

/*********************** Global Variables *************************************/
#define PN547C2_CLOCK_SETTING
#define CORE_RES_STATUS_BYTE 3

bool bEnableMfcExtns = false;
bool bEnableMfcReader = false;
bool bDisableLegacyMfcExtns = true;

/* Processing of ISO 15693 EOF */
extern uint8_t icode_send_eof;
extern uint8_t icode_detected;
static uint8_t cmd_icode_eof[] = {0x00, 0x00, 0x00};

/* FW download success flag */
static uint8_t fw_download_success = 0;
/* Anti-tearing mechanism sucess flag */
uint8_t anti_tearing_recovery_success = 0;

static uint8_t config_access = false;
static uint8_t config_success = true;

static ThreadMutex sHalFnLock;

/* NCI HAL Control structure */
phNxpNciHal_Control_t nxpncihal_ctrl;

/* NXP Poll Profile structure */
phNxpNciProfile_Control_t nxpprofile_ctrl;

/* TML Context */
extern phTmlNfc_Context_t* gpphTmlNfc_Context;
extern void phTmlNfc_set_fragmentation_enabled(
    phTmlNfc_i2cfragmentation_t result);
/* global variable to get FW version from NCI response*/
uint32_t wFwVerRsp;
#ifdef ANDROID
EseAdaptation *gpEseAdapt = NULL;
#endif
/* External global variable to get FW version */
extern uint16_t wFwVer;
extern uint16_t fw_maj_ver;
extern uint16_t rom_version;
extern uint8_t gRecFWDwnld;
static uint8_t gRecFwRetryCount;  // variable to hold dummy FW recovery count
static uint8_t Rx_data[NCI_MAX_DATA_LEN];
extern int phPalEse_spi_ioctl(phPalEse_ControlCode_t eControlCode,void *pDevHandle, long level);
uint8_t discovery_cmd[50] = {0};
uint8_t discovery_cmd_len = 0;
uint32_t timeoutTimerId = 0;
#ifdef LINUX
extern bool nfc_debug_enabled;
#else
bool nfc_debug_enabled = true;
#endif
static bool sIsForceFwDownloadReqd = false;
/*  Used to send Callback Transceive data during Mifare Write.
 *  If this flag is enabled, no need to send response to Upper layer */
bool sendRspToUpperLayer = true;

phNxpNciHal_Sem_t config_data;

phNxpNciClock_t phNxpNciClock = {0, {0}, false};

phNxpNciRfSetting_t phNxpNciRfSet = {false, {0}};

phNxpNciMwEepromArea_t phNxpNciMwEepromArea = {false, {0}};

/**************** local methods used in this file only ************************/
static NFCSTATUS phNxpNciHal_fw_download(void);
static void phNxpNciHal_open_complete(NFCSTATUS status);
static void phNxpNciHal_MinOpen_complete(NFCSTATUS status);
static void phNxpNciHal_write_complete(void* pContext,
                                       phTmlNfc_TransactInfo_t* pInfo);
static void phNxpNciHal_read_complete(void* pContext,
                                      phTmlNfc_TransactInfo_t* pInfo);
static void phNxpNciHal_close_complete(NFCSTATUS status);
static void phNxpNciHal_core_initialized_complete(NFCSTATUS status);
static void phNxpNciHal_power_cycle_complete(NFCSTATUS status);
static void phNxpNciHal_kill_client_thread(
    phNxpNciHal_Control_t* p_nxpncihal_ctrl);
static void* phNxpNciHal_client_thread(void* arg);
static void phNxpNciHal_get_clk_freq(void);
static void phNxpNciHal_set_clock(void);
static void phNxpNciHal_hci_network_reset(void);
static NFCSTATUS phNxpNciHal_do_se_session_reset(void);
static void phNxpNciHal_print_res_status(uint8_t* p_rx_data, uint16_t* p_len);
static NFCSTATUS phNxpNciHal_CheckValidFwVersion(void);
static void phNxpNciHal_enable_i2c_fragmentation();
static NFCSTATUS phNxpNciHal_get_mw_eeprom(void);
static NFCSTATUS phNxpNciHal_set_mw_eeprom(void);
static NFCSTATUS phNxpNciHal_config_t4t_ndef(uint8_t t4tFlag);
static int phNxpNciHal_fw_mw_ver_check();
NFCSTATUS phNxpNciHal_check_clock_config(void);
NFCSTATUS phNxpNciHal_china_tianjin_rf_setting(void);
static void phNxpNciHal_gpio_restore(phNxpNciHal_GpioInfoState state);
static void phNxpNciHal_initialize_debug_enabled_flag();
static void phNxpNciHal_initialize_mifare_flag();
NFCSTATUS phNxpNciHal_nfcc_core_reset_init();
NFCSTATUS phNxpNciHal_getChipInfoInFwDnldMode(void);
static NFCSTATUS phNxpNciHalRFConfigCmdRecSequence();
static NFCSTATUS phNxpNciHal_CheckRFCmdRespStatus();
int check_config_parameter();
#ifdef FactoryOTA
void phNxpNciHal_isFactoryOTAModeActive();
static NFCSTATUS phNxpNciHal_disableFactoryOTAMode(void);
#endif
/******************************************************************************
 * Function         phNxpNciHal_initialize_debug_enabled_flag
 *
 * Description      This function gets the value for nfc_debug_enabled
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpNciHal_initialize_debug_enabled_flag() {
  unsigned long num = 0;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if(GetNxpNumValue(NAME_NFC_DEBUG_ENABLED, &num, sizeof(num))) {
    nfc_debug_enabled = (num == 0) ? false : true;

  }

#if 0
  int len = property_get("nfc.debug_enabled", valueStr, "");
  if (len > 0) {
        // let Android property override .conf variable
    unsigned debug_enabled = 0;
    sscanf(valueStr, "%u", &debug_enabled);
    nfc_debug_enabled = (debug_enabled == 0) ? false : true;
  }
  NXPLOG_NCIHAL_D("nfc_debug_enabled : %d",nfc_debug_enabled);
#endif
}

/******************************************************************************
 * Function         phNxpNciHal_client_thread
 *
 * Description      This function is a thread handler which handles all TML and
 *                  NCI messages.
 *
 * Returns          void
 *
 ******************************************************************************/
static void* phNxpNciHal_client_thread(void* arg) {
  phNxpNciHal_Control_t* p_nxpncihal_ctrl = (phNxpNciHal_Control_t*)arg;
  phLibNfc_Message_t msg;

  NXPLOG_NCIHAL_D("thread started");

  p_nxpncihal_ctrl->thread_running = 1;

  while (p_nxpncihal_ctrl->thread_running == 1) {
    /* Fetch next message from the NFC stack message queue */
    if (phDal4Nfc_msgrcv(p_nxpncihal_ctrl->gDrvCfg.nClientId, &msg, 0, 0) ==
        -1) {
      NXPLOG_NCIHAL_E("NFC client received bad message");
      continue;
    }

    if (p_nxpncihal_ctrl->thread_running == 0) {
      break;
    }

    switch (msg.eMsgType) {
      case PH_LIBNFC_DEFERREDCALL_MSG: {
        phLibNfc_DeferredCall_t* deferCall =
            (phLibNfc_DeferredCall_t*)(msg.pMsgData);

        REENTRANCE_LOCK();
        deferCall->pCallback(deferCall->pParameter);
        REENTRANCE_UNLOCK();

        break;
      }

      case NCI_HAL_OPEN_CPLT_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_OPEN_CPLT_EVT,
                                              HAL_NFC_STATUS_OK);
        }
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_CLOSE_CPLT_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_CLOSE_CPLT_EVT,
                                              HAL_NFC_STATUS_OK);
        }
        phNxpNciHal_kill_client_thread(&nxpncihal_ctrl);
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_POST_INIT_CPLT_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_POST_INIT_CPLT_EVT,
                                              HAL_NFC_STATUS_OK);
        }
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_PRE_DISCOVER_CPLT_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_PRE_DISCOVER_CPLT_EVT,
                                              HAL_NFC_STATUS_OK);
        }
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_HCI_NETWORK_RESET_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)((uint32_t)NfcEvent::HCI_NETWORK_RESET,
                                              HAL_NFC_STATUS_OK);
        }
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_ERROR_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
          /* Send the event */
          (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_ERROR_EVT,
                                              HAL_NFC_STATUS_FAILED);
        }
        REENTRANCE_UNLOCK();
        break;
      }

      case NCI_HAL_RX_MSG: {
        REENTRANCE_LOCK();
        if (nxpncihal_ctrl.p_nfc_stack_data_cback != NULL) {
          (*nxpncihal_ctrl.p_nfc_stack_data_cback)(nxpncihal_ctrl.rsp_len,
                                                   nxpncihal_ctrl.p_rsp_data);
        }
        REENTRANCE_UNLOCK();
        break;
      }
    }
  }

  NXPLOG_NCIHAL_D("NxpNciHal thread stopped");

  return NULL;
}

/******************************************************************************
 * Function         phNxpNciHal_kill_client_thread
 *
 * Description      This function safely kill the client thread and clean all
 *                  resources.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_kill_client_thread(
    phNxpNciHal_Control_t* p_nxpncihal_ctrl) {
  NXPLOG_NCIHAL_D("Terminating phNxpNciHal client thread...");

  p_nxpncihal_ctrl->p_nfc_stack_cback = NULL;
  p_nxpncihal_ctrl->p_nfc_stack_data_cback = NULL;
  p_nxpncihal_ctrl->thread_running = 0;

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_fw_download
 *
 * Description      This function download the PN54X secure firmware to IC. If
 *                  firmware version in Android filesystem and firmware in the
 *                  IC is same then firmware download will return with success
 *                  without downloading the firmware.
 *
 * Returns          NFCSTATUS_SUCCESS if firmware download successful
 *                  NFCSTATUS_FAILED in case of failure
 *                  NFCSTATUS_REJECTED if FW version is invalid or if hardware
 *                                     criteria is not matching
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_fw_download(void) {
  if (NFCSTATUS_SUCCESS != phNxpNciHal_CheckValidFwVersion()) {
     return NFCSTATUS_REJECTED;
  }

#if(NXP_EXTNS != TRUE)
  nfc_nci_IoctlInOutData_t data;
  memset(&data, 0x00, sizeof(nfc_nci_IoctlInOutData_t));
  data.inp.level = 0x03; // ioctl call arg value to get eSE power GPIO value = 0x03
  int ese_gpio_value = phNxpNciHal_ioctl(HAL_NFC_GET_SPM_STATUS, &data);
  NXPLOG_NCIHAL_D("eSE Power GPIO value = %d", ese_gpio_value);
  if (ese_gpio_value != 0) {
    NXPLOG_NCIHAL_E("FW download denied while SPI in use, Continue NFC init");
    return NFCSTATUS_REJECTED;
  }
#endif
  if (!sIsForceFwDownloadReqd) {
    nxpncihal_ctrl.phNxpNciGpioInfo.state = GPIO_UNKNOWN;
    phNxpNciHal_gpio_restore(GPIO_STORE);
  }
  int fw_retry_count = 0;
  NFCSTATUS status = NFCSTATUS_REJECTED;
  NXPLOG_NCIHAL_D("Starting FW update");
  do {
    fw_download_success = 0;
    phNxpNciHal_get_clk_freq();
    status = phTmlNfc_IoCtl(phTmlNfc_e_EnableDownloadMode);
    if (NFCSTATUS_SUCCESS != status) {
      fw_retry_count++;
      NXPLOG_NCIHAL_D("Retrying: FW download");
      continue;
    }
    //patch applied from latest Android code base
    if (sIsForceFwDownloadReqd) {
      status = phNxpNciHal_getChipInfoInFwDnldMode();
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("Unknown chip type, FW can't be upgraded");
        return status;
      }
    }

    /* Set the obtained device handle to download module */
    phDnldNfc_SetHwDevHandle();
    NXPLOG_NCIHAL_D("Calling Seq handler for FW Download \n");
    status = phNxpNciHal_fw_download_seq(nxpprofile_ctrl.bClkSrcVal,
                                         nxpprofile_ctrl.bClkFreqVal);
    if (status != NFCSTATUS_SUCCESS) {
      //patch applied from latest Android code base
      phDnldNfc_ReSetHwDevHandle();
      fw_retry_count++;
      NXPLOG_NCIHAL_D("Retrying: FW download");
    }
  } while((fw_retry_count < 3) && (status != NFCSTATUS_SUCCESS));

  if (status != NFCSTATUS_SUCCESS) {
    if (NFCSTATUS_SUCCESS != phNxpNciHal_fw_mw_ver_check()) {
      NXPLOG_NCIHAL_D("Chip Version Middleware Version mismatch!!!!");
      phOsalNfc_Timer_Cleanup();
      phTmlNfc_Shutdown_CleanUp();
      status = NFCSTATUS_FAILED;
    } else {
      NXPLOG_NCIHAL_E("FW download failed, Continue NFCC init");
    }
  } else {
    status = NFCSTATUS_SUCCESS;
    fw_download_success = 1;
  }

  /*Keep Read Pending on I2C*/
  NFCSTATUS readRestoreStatus = NFCSTATUS_FAILED;
  readRestoreStatus = phTmlNfc_Read(
      nxpncihal_ctrl.p_cmd_data, NCI_MAX_DATA_LEN,
      (pphTmlNfc_TransactCompletionCb_t)&phNxpNciHal_read_complete, NULL);
  if (readRestoreStatus != NFCSTATUS_PENDING) {
    NXPLOG_NCIHAL_E("TML Read status error status = %x", readRestoreStatus);
    readRestoreStatus = phTmlNfc_Shutdown_CleanUp();
    if (readRestoreStatus != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("TML Shutdown failed. Status  = %x", readRestoreStatus);
    }
  }
  phDnldNfc_ReSetHwDevHandle();

  if (status == NFCSTATUS_SUCCESS) {
    status = phNxpNciHal_nfcc_core_reset_init();
    if (status == NFCSTATUS_SUCCESS) {
      phNxpNciHal_gpio_restore(GPIO_RESTORE);
    } else {
      NXPLOG_NCIHAL_D("Failed to restore GPIO values!!!\n");
    }
  }

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_CheckValidFwVersion
 *
 * Description      This function checks the valid FW for Mobile device.
 *                  If the FW doesn't belong the Mobile device it further
 *                  checks nxp config file to override.
 *
 * Returns          NFCSTATUS_SUCCESS if valid fw version found
 *                  NFCSTATUS_NOT_ALLOWED in case of FW not valid for mobile
 *                  device
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_CheckValidFwVersion(void) {
  NFCSTATUS status = NFCSTATUS_NOT_ALLOWED;
  const unsigned char sfw_infra_major_no = 0x02;
  unsigned char ufw_current_major_no = 0x00;
  unsigned long num = 0;
  int isfound = 0;
  //patch applied from latest Android code base
  unsigned char fw_major_no = ((wFwVerRsp >> 8) & 0x000000FF);

  /* extract the firmware's major no */
  ufw_current_major_no = ((0x00FF) & (wFwVer >> 8U));
  if (ufw_current_major_no >= fw_major_no) {
    status = NFCSTATUS_SUCCESS;
  } else if (ufw_current_major_no == sfw_infra_major_no) {
    if (rom_version == FW_MOBILE_ROM_VERSION_PN553 &&
        nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
      NXPLOG_NCIHAL_D(" PN81A  allow Fw download with major number =  0x%x",
                      ufw_current_major_no);
      status = NFCSTATUS_SUCCESS;
    } else {
      /* Check the nxp config file if still want to go for download */
      /* By default NAME_NXP_FW_PROTECION_OVERRIDE will not be defined in config
         file.
         If user really want to override the Infra firmware over mobile
         firmware, please
         put "NXP_FW_PROTECION_OVERRIDE=0x01" in libnfc-nxp.conf file.
         Please note once Infra firmware downloaded to Mobile device, The device
         can never be updated to Mobile firmware*/
      isfound =
          GetNxpNumValue(NAME_NXP_FW_PROTECION_OVERRIDE, &num, sizeof(num));
      if (isfound > 0) {
        if (num == 0x01) {
          NXPLOG_NCIHAL_D("Override Infra FW over Mobile");
          status = NFCSTATUS_SUCCESS;
        } else {
          NXPLOG_NCIHAL_D(
              "Firmware download not allowed (NXP_FW_PROTECION_OVERRIDE "
              "invalid value)");
        }
      } else {
        NXPLOG_NCIHAL_D(
            "Firmware download not allowed (NXP_FW_PROTECION_OVERRIDE not "
            "defined)");
      }
    }
  }
  else if (gRecFWDwnld == TRUE) {
    status = NFCSTATUS_SUCCESS;
  }
  else if (wFwVerRsp == 0) {
    NXPLOG_NCIHAL_E(
        "FW Version not received by NCI command >>> Force Firmware download");
    status = NFCSTATUS_SUCCESS;
  } else {
    NXPLOG_NCIHAL_E("Wrong FW Version >>> Firmware download not allowed");
  }

  return status;
}

static void phNxpNciHal_get_clk_freq(void) {
  unsigned long num = 0;
  int isfound = 0;

  nxpprofile_ctrl.bClkSrcVal = 0;
  nxpprofile_ctrl.bClkFreqVal = 0;
  nxpprofile_ctrl.bTimeout = 0;

  isfound = GetNxpNumValue(NAME_NXP_SYS_CLK_SRC_SEL, &num, sizeof(num));
  if (isfound > 0) {
    nxpprofile_ctrl.bClkSrcVal = num;
  }

  num = 0;
  isfound = 0;
  isfound = GetNxpNumValue(NAME_NXP_SYS_CLK_FREQ_SEL, &num, sizeof(num));
  if (isfound > 0) {
    nxpprofile_ctrl.bClkFreqVal = num;
  }

  num = 0;
  isfound = 0;
  isfound = GetNxpNumValue(NAME_NXP_SYS_CLOCK_TO_CFG, &num, sizeof(num));
  if (isfound > 0) {
    nxpprofile_ctrl.bTimeout = num;
  }

  NXPLOG_FWDNLD_D("gphNxpNciHal_fw_IoctlCtx.bClkSrcVal = 0x%x",
                  nxpprofile_ctrl.bClkSrcVal);
  NXPLOG_FWDNLD_D("gphNxpNciHal_fw_IoctlCtx.bClkFreqVal = 0x%x",
                  nxpprofile_ctrl.bClkFreqVal);
  NXPLOG_FWDNLD_D("gphNxpNciHal_fw_IoctlCtx.bTimeout = 0x%x",
                  nxpprofile_ctrl.bTimeout);

  if ((nxpprofile_ctrl.bClkSrcVal < CLK_SRC_XTAL) ||
      (nxpprofile_ctrl.bClkSrcVal > CLK_SRC_PLL)) {
    NXPLOG_FWDNLD_E(
        "Clock source value is wrong in config file, setting it as default");
    nxpprofile_ctrl.bClkSrcVal = NXP_SYS_CLK_SRC_SEL;
  }
  if (nxpprofile_ctrl.bClkFreqVal == CLK_SRC_PLL &&
      (nxpprofile_ctrl.bClkFreqVal < CLK_FREQ_13MHZ ||
       nxpprofile_ctrl.bClkFreqVal > CLK_FREQ_52MHZ)) {
    NXPLOG_FWDNLD_E(
        "Clock frequency value is wrong in config file, setting it as default");
    nxpprofile_ctrl.bClkFreqVal = NXP_SYS_CLK_FREQ_SEL;
  }
  if ((nxpprofile_ctrl.bTimeout < CLK_TO_CFG_DEF) ||
      (nxpprofile_ctrl.bTimeout > CLK_TO_CFG_MAX)) {
    NXPLOG_FWDNLD_E(
        "Clock timeout value is wrong in config file, setting it as default");
    nxpprofile_ctrl.bTimeout = CLK_TO_CFG_DEF;
  }
}

/******************************************************************************
 * Function         phNxpNciHal_MinOpen
 *
 * Description      This function initializes the least required resources to
 *                  communicate to NFCC.This is mainly used to communicate to
 *                  NFCC when NFC service is not available.
 *
 *
 * Returns          This function return NFCSTATUS_SUCCES (0) in case of success
 *                  In case of failure returns other failure value.
 *
 ******************************************************************************/
int phNxpNciHal_MinOpen (){
  phOsalNfc_Config_t tOsalConfig;
  phTmlNfc_Config_t tTmlConfig;
  char* nfc_dev_node = NULL;
  const uint16_t max_len = 260;
  NFCSTATUS wConfigStatus = NFCSTATUS_SUCCESS;
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  NXPLOG_NCIHAL_D("phNxpNci_MinOpen(): enter");
  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};
  /*NCI2_0_INIT_CMD*/
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};

  AutoThreadMutex a(sHalFnLock);
  if (nxpncihal_ctrl.halStatus == HAL_STATUS_MIN_OPEN) {
    NXPLOG_NCIHAL_D("phNxpNciHal_MinOpen(): already open");
    return NFCSTATUS_SUCCESS;
  }
  /* reset config cache */
  resetNxpConfig();

  /* reset config cache */
  int init_retry_cnt = 0;
  int8_t ret_val = 0x00;
#ifdef ANDROID
  phNxpNciHal_initialize_debug_enabled_flag();
#endif
  /* initialize trace level */
  phNxpLog_InitializeLogLevel();

  /* initialize Mifare flags*/
  phNxpNciHal_initialize_mifare_flag();

  /*Create the timer for extns write response*/
  timeoutTimerId = phOsalNfc_Timer_Create();

  if (phNxpNciHal_init_monitor() == NULL) {
    NXPLOG_NCIHAL_E("Init monitor failed");
    return NFCSTATUS_FAILED;
  }

  CONCURRENCY_LOCK();
  memset(&nxpncihal_ctrl, 0x00, sizeof(nxpncihal_ctrl));
  memset(&tOsalConfig, 0x00, sizeof(tOsalConfig));
  memset(&tTmlConfig, 0x00, sizeof(tTmlConfig));
  memset(&nxpprofile_ctrl, 0, sizeof(phNxpNciProfile_Control_t));

  /*Init binary semaphore for Spi Nfc synchronization*/
  if (0 != sem_init(&nxpncihal_ctrl.syncSpiNfc, 0, 1)) {
    NXPLOG_NCIHAL_E("sem_init() FAiled, errno = 0x%02X", errno);
    goto clean_and_return;
  }

  /* By default HAL status is HAL_STATUS_OPEN */
  nxpncihal_ctrl.halStatus = HAL_STATUS_OPEN;
#ifdef ANDROID
  gpEseAdapt = &EseAdaptation::GetInstance();
  gpEseAdapt->Initialize();
#endif
  /*nci version NCI_VERSION_UNKNOWN version by default*/
  nxpncihal_ctrl.nci_info.nci_version = NCI_VERSION_UNKNOWN;
  /* Read the nfc device node name */
  nfc_dev_node = (char*)malloc(max_len * sizeof(char));
  if (nfc_dev_node == NULL) {
    NXPLOG_NCIHAL_D("malloc of nfc_dev_node failed ");
    goto clean_and_return;
  } else if (!GetNxpStrValue(NAME_NXP_NFC_DEV_NODE, nfc_dev_node,
                             max_len)) {
    NXPLOG_NCIHAL_D(
        "Invalid nfc device node name keeping the default device node "
        "/dev/pn54x");
    strlcpy(nfc_dev_node, "/dev/pn54x", (max_len * sizeof(char)));
  }

  /* Configure hardware link */
  nxpncihal_ctrl.gDrvCfg.nClientId = phDal4Nfc_msgget(0, 0600);
  nxpncihal_ctrl.gDrvCfg.nLinkType = ENUM_LINK_TYPE_I2C; /* For PN54X */
  tTmlConfig.pDevName = (int8_t*)nfc_dev_node;
  tOsalConfig.dwCallbackThreadId = (uintptr_t)nxpncihal_ctrl.gDrvCfg.nClientId;
  tOsalConfig.pLogFile = NULL;
  tTmlConfig.dwGetMsgThreadId = (uintptr_t)nxpncihal_ctrl.gDrvCfg.nClientId;

  if (nfcFL.chipType == pn548C2) {
    memset(discovery_cmd, 0, sizeof(discovery_cmd));
    discovery_cmd_len = 0;
  }

  /* Initialize TML layer */
  wConfigStatus = phTmlNfc_Init(&tTmlConfig);
  if (wConfigStatus != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("phTmlNfc_Init Failed");
    goto clean_and_return;
  } else {
    if (nfc_dev_node != NULL) {
      free(nfc_dev_node);
      nfc_dev_node = NULL;
    }
  }

  /* Create the client thread */
  ret_val = pthread_create(&nxpncihal_ctrl.client_thread, NULL,
                           phNxpNciHal_client_thread, &nxpncihal_ctrl);
  if (ret_val != 0) {
    NXPLOG_NCIHAL_E("pthread_create failed");
    wConfigStatus = phTmlNfc_Shutdown_CleanUp();
    goto clean_and_return;
  }

  CONCURRENCY_UNLOCK();

  /* call read pending */
  status = phTmlNfc_Read(
      nxpncihal_ctrl.p_cmd_data, NCI_MAX_DATA_LEN,
      (pphTmlNfc_TransactCompletionCb_t)&phNxpNciHal_read_complete, NULL);
  if (status != NFCSTATUS_PENDING) {
    NXPLOG_NCIHAL_E("TML Read status error status = %x", status);
    wConfigStatus = phTmlNfc_Shutdown_CleanUp();
    wConfigStatus = NFCSTATUS_FAILED;
    goto clean_and_return;
  }

  phNxpNciHal_ext_init();
//patch applied from latest Android code base
init_retry:
  status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if (status == NFCSTATUS_SUCCESS) {
    sIsForceFwDownloadReqd = false;
  } else if (sIsForceFwDownloadReqd) {
    /* MinOpen can be called from either NFC on or any NFC IOCTL calls from
     * SPI HAL or system/nfc while Minopen is not done/success, which can
     * trigger Force FW update during every Minopen. To avoid multiple Force
     * Force FW upadted return if Force FW update is already done */
    NXPLOG_NCIHAL_E("%s: Failed after Force FW updated. Exit", __func__);
    return NFCSTATUS_FAILED;
  }
  sIsForceFwDownloadReqd = (status != NFCSTATUS_SUCCESS) &&
      ((nxpncihal_ctrl.retry_cnt >= MAX_RETRY_COUNT) ||
      (init_retry_cnt >= MAX_RETRY_COUNT)) ;
 if (sIsForceFwDownloadReqd) {
    NXPLOG_NCIHAL_E("Force FW Download, NFCC not coming out from Standby");
    wConfigStatus = NFCSTATUS_FAILED;
    goto force_download;
  } else if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NCI_CORE_RESET: Failed");
    if (init_retry_cnt < MAX_RETRY_COUNT) {
      init_retry_cnt++;
      (void)phNxpNciHal_power_cycle();
      goto init_retry;
    } else
      init_retry_cnt = 0;
    wConfigStatus = phTmlNfc_Shutdown_CleanUp();
    wConfigStatus = NFCSTATUS_FAILED;
    goto clean_and_return;
  }

  if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
  } else {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
    /*If chipType is pn557 or PN81A(PN553_TC) and if the chip is in 1.0 mode,
      Force it to 2.0 mode. To confirm the PN553_TC/PN81A chip, FW version check
      is also added */
    bool pn81A_pn553_chip = (nfcFL.chipType == pn553) && ((wFwVerRsp >> 8 & 0xFFFF) == 0x1102);
    if ((status == NFCSTATUS_SUCCESS) && ((nfcFL.chipType == pn557) || pn81A_pn553_chip)) {
      NXPLOG_NCIHAL_D("Chip is in NCI1.0 mode reset the chip to 2.0 mode");
      status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
        if (status == NFCSTATUS_SUCCESS) {
          goto init_retry;
        }
      }
    }
  }
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NCI_CORE_INIT : Failed");
    if (init_retry_cnt < 3) {
      init_retry_cnt++;
      (void)phNxpNciHal_power_cycle();
      goto init_retry;
    } else
      init_retry_cnt = 0;
    wConfigStatus = phTmlNfc_Shutdown_CleanUp();
    wConfigStatus = NFCSTATUS_FAILED;
    goto clean_and_return;
  }
  phNxpNciHal_enable_i2c_fragmentation();
  /*Get FW version from device*/
  status = phDnldNfc_InitImgInfo();
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("Image information extraction Failed!!");
  }
  NXPLOG_NCIHAL_D("FW version for FW file = 0x%x", wFwVer);
  NXPLOG_NCIHAL_D("FW version from device = 0x%x", wFwVerRsp);
  if ((wFwVerRsp & 0x0000FFFF) == wFwVer) {
    NXPLOG_NCIHAL_D("FW update not required");
    phDnldNfc_ReSetHwDevHandle();
  } else {
force_download:
    status = phNxpNciHal_fw_download();
    if (NFCSTATUS_FAILED == status){
      wConfigStatus = NFCSTATUS_FAILED;
      NXPLOG_NCIHAL_D("FW download Failed");
      goto clean_and_return;
    } else if (NFCSTATUS_REJECTED == status) {
      wConfigStatus = NFCSTATUS_SUCCESS;
      NXPLOG_NCIHAL_D("FW download Rejected. Continuiing Nfc Init");
    } else {
      wConfigStatus = NFCSTATUS_SUCCESS;
      NXPLOG_NCIHAL_D("FW download Success");
    }
  }
  NfccPowerTracker::getInstance().Initialize();
  /* Call open complete */
  phNxpNciHal_MinOpen_complete(wConfigStatus);
  NXPLOG_NCIHAL_D("phNxpNciHal_MinOpen(): exit");
  return wConfigStatus;

clean_and_return:
  CONCURRENCY_UNLOCK();
  if (nfc_dev_node != NULL) {
    free(nfc_dev_node);
    nfc_dev_node = NULL;
  }
  /* Report error status */
  phNxpNciHal_cleanup_monitor();
  nxpncihal_ctrl.halStatus = HAL_STATUS_CLOSE;
  return NFCSTATUS_FAILED;
}


/******************************************************************************
 * Function         phNxpNciHal_open
 *
 * Description      This function is called by libnfc-nci during the
 *                  initialization of the NFCC. It opens the physical connection
 *                  with NFCC (PN54X) and creates required client thread for
 *                  operation.
 *                  After open is complete, status is informed to libnfc-nci
 *                  through callback function.
 *
 * Returns          This function return NFCSTATUS_SUCCES (0) in case of success
 *                  In case of failure returns other failure value.
 *
 ******************************************************************************/
int phNxpNciHal_open(nfc_stack_callback_t* p_cback,
                     nfc_stack_data_callback_t* p_data_cback) {
  NFCSTATUS wConfigStatus = NFCSTATUS_SUCCESS;
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  if (nxpncihal_ctrl.halStatus == HAL_STATUS_OPEN) {
    NXPLOG_NCIHAL_D("phNxpNciHal_open already open");
    return NFCSTATUS_SUCCESS;
  }else if(nxpncihal_ctrl.halStatus == HAL_STATUS_CLOSE){
    status = phNxpNciHal_MinOpen();
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("phNxpNciHal_MinOpen failed");
      goto clean_and_return;
    }
  }/*else its already in MIN_OPEN state. continue with rest of functionality*/
  nxpncihal_ctrl.p_nfc_stack_cback = p_cback;
  nxpncihal_ctrl.p_nfc_stack_data_cback = p_data_cback;

  /* Call open complete */
  phNxpNciHal_open_complete(wConfigStatus);

  return wConfigStatus;

clean_and_return:
  CONCURRENCY_UNLOCK();
  /* Report error status */
  if (p_cback != NULL) {
    (*p_cback)(HAL_NFC_OPEN_CPLT_EVT,
               HAL_NFC_STATUS_FAILED);
  }

  nxpncihal_ctrl.p_nfc_stack_cback = NULL;
  nxpncihal_ctrl.p_nfc_stack_data_cback = NULL;
  phNxpNciHal_cleanup_monitor();
  nxpncihal_ctrl.halStatus = HAL_STATUS_CLOSE;
  return NFCSTATUS_FAILED;
}

/******************************************************************************
 * Function         phNxpNciHal_fw_mw_check
 *
 * Description      This function inform the status of phNxpNciHal_fw_mw_check
 *                  function to libnfc-nci.
 *
 * Returns          int.
 *
 ******************************************************************************/
int phNxpNciHal_fw_mw_ver_check() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  if (((nfcFL.chipType == pn557) || (nfcFL.chipType == pn81T)) &&
      (rom_version == FW_MOBILE_ROM_VERSION_PN557) &&
      (fw_maj_ver == 0x01)) {
    status = NFCSTATUS_SUCCESS;
  } else if (((nfcFL.chipType == pn553) || (nfcFL.chipType == pn80T)) &&
      (rom_version == FW_MOBILE_ROM_VERSION_PN553) &&
      (fw_maj_ver == 0x01 || fw_maj_ver == 0x02)) {
    status = NFCSTATUS_SUCCESS;
  } else if (((nfcFL.chipType == pn551) || (nfcFL.chipType == pn67T)) &&
             (rom_version == FW_MOBILE_ROM_VERSION_PN551) &&
             (fw_maj_ver == 0x05)) {
    status = NFCSTATUS_SUCCESS;
  } else if ((nfcFL.chipType == pn548C2) &&
             (rom_version == FW_MOBILE_ROM_VERSION_PN548AD) &&
             (fw_maj_ver == 0x01)) {
    status = NFCSTATUS_SUCCESS;
   }
  return status;
}

NFCSTATUS phNxpNciHal_core_reset_recovery() {
  NFCSTATUS status = NFCSTATUS_FAILED;

  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01,
                                    0x00};  // keep configuration
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
  /* reset config cache */
  uint8_t retry_core_init_cnt = 0;

  if (discovery_cmd_len == 0) {
    goto FAILURE;
  }
  NXPLOG_NCIHAL_D("%s: recovery", __func__);

retry_core_init:
  if (retry_core_init_cnt > 3) {
    goto FAILURE;
  }

  status = phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("PN54X Reset - FAILED\n");
    goto FAILURE;
  }
  status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if ((status != NFCSTATUS_SUCCESS) &&
      (nxpncihal_ctrl.retry_cnt >= MAX_RETRY_COUNT)) {
    retry_core_init_cnt++;
    goto retry_core_init;
  } else if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("NCI_CORE_RESET: Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }
  if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
  } else {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
  }
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("NCI_CORE_INIT : Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

  status = phNxpNciHal_send_ext_cmd(discovery_cmd_len, discovery_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("RF_DISCOVERY : Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

  return NFCSTATUS_SUCCESS;
FAILURE:
  abort();
}

void phNxpNciHal_discovery_cmd_ext(uint8_t* p_cmd_data, uint16_t cmd_len) {
  NXPLOG_NCIHAL_D("phNxpNciHal_discovery_cmd_ext");
  if (cmd_len > 0 && cmd_len <= sizeof(discovery_cmd)) {
    memcpy(discovery_cmd, p_cmd_data, cmd_len);
    discovery_cmd_len = cmd_len;
  }
}

/******************************************************************************
 * Function         phNxpNciHal_MinOpen_complete
 *
 * Description      This function updates the status of phNxpNciHal_MinOpen_complete
 *                  to halstatus.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_MinOpen_complete(NFCSTATUS status) {

  if (status == NFCSTATUS_SUCCESS) {
    nxpncihal_ctrl.halStatus = HAL_STATUS_MIN_OPEN;
  }

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_open_complete
 *
 * Description      This function inform the status of phNxpNciHal_open
 *                  function to libnfc-nci.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_open_complete(NFCSTATUS status) {
  static phLibNfc_Message_t msg;
  if (status == NFCSTATUS_SUCCESS) {
    msg.eMsgType = NCI_HAL_OPEN_CPLT_MSG;
    nxpncihal_ctrl.hal_open_status = true;
    nxpncihal_ctrl.halStatus = HAL_STATUS_OPEN;
  } else {
    msg.eMsgType = NCI_HAL_ERROR_MSG;
  }

  msg.pMsgData = NULL;
  msg.Size = 0;

  phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId,
                        (phLibNfc_Message_t*)&msg);

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_write
 *
 * Description      This function write the data to NFCC through physical
 *                  interface (e.g. I2C) using the PN54X driver interface.
 *                  Before sending the data to NFCC, phNxpNciHal_write_ext
 *                  is called to check if there is any extension processing
 *                  is required for the NCI packet being sent out.
 *
 * Returns          It returns number of bytes successfully written to NFCC.
 *
 ******************************************************************************/
int phNxpNciHal_write(uint16_t data_len, const uint8_t* p_data) {
  if (bDisableLegacyMfcExtns && bEnableMfcExtns && p_data[0] == 0x00) {
    return NxpMfcReaderInstance.Write(data_len, p_data);
  }
  return phNxpNciHal_write_internal(data_len, p_data);
}

/******************************************************************************
 * Function         phNxpNciHal_write_internal
 *
 * Description      This function write the data to NFCC through physical
 *                  interface (e.g. I2C) using the PN54X driver interface.
 *                  Before sending the data to NFCC, phNxpNciHal_write_ext
 *                  is called to check if there is any extension processing
 *                  is required for the NCI packet being sent out.
 *
 * Returns          It returns number of bytes successfully written to NFCC.
 *
 ******************************************************************************/
int phNxpNciHal_write_internal(uint16_t data_len, const uint8_t* p_data) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  static phLibNfc_Message_t msg;
  if (nxpncihal_ctrl.halStatus != HAL_STATUS_OPEN) {
    return NFCSTATUS_FAILED;
  }
  if (data_len > NCI_MAX_DATA_LEN) {
    NXPLOG_NCIHAL_E("cmd_len exceeds limit NCI_MAX_DATA_LEN");
    android_errorWriteLog(0x534e4554, "121267042");
    goto clean_and_return;
  }
  /* Create local copy of cmd_data */
  memcpy(nxpncihal_ctrl.p_cmd_data, p_data, data_len);
  nxpncihal_ctrl.cmd_len = data_len;
#ifdef P2P_PRIO_LOGIC_HAL_IMP
  /* Specific logic to block RF disable when P2P priority logic is busy */
  if (data_len < NORMAL_MODE_HEADER_LEN) {
  /* Avoid OOB Read */
    android_errorWriteLog(0x534e4554, "128530069");
  } else if (p_data[0] == 0x21 && p_data[1] == 0x06 && p_data[2] == 0x01 &&
      EnableP2P_PrioLogic == true) {
    NXPLOG_NCIHAL_D("P2P priority logic busy: Disable it.");
    phNxpNciHal_clean_P2P_Prio();
  }
#endif

  /* Check for NXP ext before sending write */
  status =
      phNxpNciHal_write_ext(&nxpncihal_ctrl.cmd_len, nxpncihal_ctrl.p_cmd_data,
                            &nxpncihal_ctrl.rsp_len, nxpncihal_ctrl.p_rsp_data);
  if (status != NFCSTATUS_SUCCESS) {
    /* Do not send packet to PN54X, send response directly */
    msg.eMsgType = NCI_HAL_RX_MSG;
    msg.pMsgData = NULL;
    msg.Size = 0;

    phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId,
                          (phLibNfc_Message_t*)&msg);
    goto clean_and_return;
  }

  CONCURRENCY_LOCK();
  data_len = phNxpNciHal_write_unlocked(nxpncihal_ctrl.cmd_len,
                                        nxpncihal_ctrl.p_cmd_data);
  CONCURRENCY_UNLOCK();

  if (icode_send_eof == 1) {
    usleep(10000);
    icode_send_eof = 2;
    status = phNxpNciHal_send_ext_cmd(3, cmd_icode_eof);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("ICODE end of frame command failed");
    }
  }

clean_and_return:
  /* No data written */
  return data_len;
}

/******************************************************************************
 * Function         phNxpNciHal_write_unlocked
 *
 * Description      This is the actual function which is being called by
 *                  phNxpNciHal_write. This function writes the data to NFCC.
 *                  It waits till write callback provide the result of write
 *                  process.
 *
 * Returns          It returns number of bytes successfully written to NFCC.
 *
 ******************************************************************************/
int phNxpNciHal_write_unlocked(uint16_t data_len, const uint8_t* p_data) {
  NFCSTATUS status = NFCSTATUS_INVALID_PARAMETER;
  phNxpNciHal_Sem_t cb_data;
  nxpncihal_ctrl.retry_cnt = 0;
  static uint8_t reset_ntf[] = {0x60, 0x00, 0x06, 0xA0, 0x00,
                                0xC7, 0xD4, 0x00, 0x00};
  /* Create the local semaphore */
  if (phNxpNciHal_init_cb_data(&cb_data, NULL) != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("phNxpNciHal_write_unlocked Create cb data failed");
    data_len = 0;
    goto clean_and_return;
  }

  /* Create local copy of cmd_data */
  memcpy(nxpncihal_ctrl.p_cmd_data, p_data, data_len);
  nxpncihal_ctrl.cmd_len = data_len;

  /* check for write synchronyztion */
  if(phNxpNciHal_check_ncicmd_write_window(nxpncihal_ctrl.cmd_len,
                         nxpncihal_ctrl.p_cmd_data) != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("phNxpNciHal_write_unlocked check nci write window failed");
    data_len = 0;
    goto clean_and_return;
  }

  NfccPowerTracker::getInstance().ProcessCmd(
      (uint8_t *)nxpncihal_ctrl.p_cmd_data, (uint16_t)nxpncihal_ctrl.cmd_len);

retry:

  data_len = nxpncihal_ctrl.cmd_len;

  status = phTmlNfc_Write(
      (uint8_t*)nxpncihal_ctrl.p_cmd_data, (uint16_t)nxpncihal_ctrl.cmd_len,
      (pphTmlNfc_TransactCompletionCb_t)&phNxpNciHal_write_complete,
      (void*)&cb_data);
  if (status != NFCSTATUS_PENDING) {
    NXPLOG_NCIHAL_E("write_unlocked status error");
    data_len = 0;
    goto clean_and_return;
  }

  /* Wait for callback response */
  if (SEM_WAIT(cb_data)) {
    NXPLOG_NCIHAL_E("write_unlocked semaphore error");
    data_len = 0;
    goto clean_and_return;
  }

  if (cb_data.status != NFCSTATUS_SUCCESS) {
    data_len = 0;
    if (nxpncihal_ctrl.retry_cnt++ < MAX_RETRY_COUNT) {
      NXPLOG_NCIHAL_D(
          "write_unlocked failed - PN54X Maybe in Standby Mode - Retry");
      /* 10ms delay to give NFCC wake up delay */
      usleep(1000 * 10);
      goto retry;
    } else {
      NXPLOG_NCIHAL_E(
          "write_unlocked failed - PN54X Maybe in Standby Mode (max count = "
          "0x%x)",
          nxpncihal_ctrl.retry_cnt);

      sem_post(&(nxpncihal_ctrl.syncSpiNfc));

      status = phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);

      if (NFCSTATUS_SUCCESS == status) {
        NXPLOG_NCIHAL_D("PN54X Reset - SUCCESS\n");
      } else {
        NXPLOG_NCIHAL_D("PN54X Reset - FAILED\n");
      }
      if (nxpncihal_ctrl.p_nfc_stack_data_cback != NULL &&
          nxpncihal_ctrl.p_rx_data != NULL &&
          nxpncihal_ctrl.hal_open_status == true) {
        NXPLOG_NCIHAL_D(
            "Send the Core Reset NTF to upper layer, which will trigger the "
            "recovery\n");
        // Send the Core Reset NTF to upper layer, which will trigger the
        // recovery.
        nxpncihal_ctrl.rx_data_len = sizeof(reset_ntf);
        memcpy(nxpncihal_ctrl.p_rx_data, reset_ntf, sizeof(reset_ntf));
        (*nxpncihal_ctrl.p_nfc_stack_data_cback)(nxpncihal_ctrl.rx_data_len,
                                                 nxpncihal_ctrl.p_rx_data);
      }
    }
  }

clean_and_return:
  phNxpNciHal_cleanup_cb_data(&cb_data);
  return data_len;
}

/******************************************************************************
 * Function         phNxpNciHal_write_complete
 *
 * Description      This function handles write callback.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_write_complete(void* pContext,
                                       phTmlNfc_TransactInfo_t* pInfo) {
  phNxpNciHal_Sem_t* p_cb_data = (phNxpNciHal_Sem_t*)pContext;
  if (pInfo->wStatus == NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("write successful status = 0x%x", pInfo->wStatus);
  } else {
    NXPLOG_NCIHAL_D("write error status = 0x%x", pInfo->wStatus);
  }

  p_cb_data->status = pInfo->wStatus;

  SEM_POST(p_cb_data);

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_read_complete
 *
 * Description      This function is called whenever there is an NCI packet
 *                  received from NFCC. It could be RSP or NTF packet. This
 *                  function provide the received NCI packet to libnfc-nci
 *                  using data callback of libnfc-nci.
 *                  There is a pending read called from each
 *                  phNxpNciHal_read_complete so each a packet received from
 *                  NFCC can be provide to libnfc-nci.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_read_complete(void* pContext,
                                      phTmlNfc_TransactInfo_t* pInfo) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  int sem_val;
  UNUSED(pContext);
  if (nxpncihal_ctrl.read_retry_cnt == 1) {
    nxpncihal_ctrl.read_retry_cnt = 0;
  }
  if (pInfo->wStatus == NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("read successful status = 0x%x", pInfo->wStatus);

    sem_getvalue(&(nxpncihal_ctrl.syncSpiNfc), &sem_val);
    if(((pInfo->pBuff[0] & NCI_MT_MASK) == NCI_MT_RSP)  && sem_val == 0 ) {
        sem_post(&(nxpncihal_ctrl.syncSpiNfc));
    }
    /*Check the Omapi command response and store in dedicated buffer to solve sync issue*/
    if(pInfo->pBuff[0] == 0x4F && pInfo->pBuff[1] == 0x01 && pInfo->pBuff[2] == 0x01) {
        nxpncihal_ctrl.p_rx_ese_data = pInfo->pBuff;
        nxpncihal_ctrl.rx_ese_data_len = pInfo->wLength;
        SEM_POST(&(nxpncihal_ctrl.ext_cb_data));
    }
    else{
        nxpncihal_ctrl.p_rx_data = pInfo->pBuff;
        nxpncihal_ctrl.rx_data_len = pInfo->wLength;
        status = phNxpNciHal_process_ext_rsp(nxpncihal_ctrl.p_rx_data,
                                          &nxpncihal_ctrl.rx_data_len);
    }

    phNxpNciHal_print_res_status(pInfo->pBuff, &pInfo->wLength);
#ifndef LINUX
    if ((nxpncihal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_NTF) {
      NfccPowerTracker::getInstance().ProcessNtf(nxpncihal_ctrl.p_rx_data,
                                                 nxpncihal_ctrl.rx_data_len);
    }
#endif
    /* Check if response should go to hal module only */
    if (nxpncihal_ctrl.hal_ext_enabled == TRUE &&
        (nxpncihal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_RSP) {
      if (status == NFCSTATUS_FAILED) {
        NXPLOG_NCIHAL_D("enter into NFCC init recovery");
        nxpncihal_ctrl.ext_cb_data.status = status;
      }
      /* Unlock semaphore only for responses*/
      if ((nxpncihal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_RSP ||
          ((icode_detected == true) && (icode_send_eof == 3))) {
        /* Unlock semaphore */
        SEM_POST(&(nxpncihal_ctrl.ext_cb_data));
      }
    }  // Notification Checking
    else if ((nxpncihal_ctrl.hal_ext_enabled == TRUE) &&
             ((nxpncihal_ctrl.p_rx_data[0x00] & NCI_MT_MASK) == NCI_MT_NTF) &&
             (nxpncihal_ctrl.nci_info.wait_for_ntf == TRUE)) {
      /* Unlock semaphore waiting for only  ntf*/
      SEM_POST(&(nxpncihal_ctrl.ext_cb_data));
      nxpncihal_ctrl.nci_info.wait_for_ntf = FALSE;
    } else if (bDisableLegacyMfcExtns && !sendRspToUpperLayer &&
               (nxpncihal_ctrl.p_rx_data[0x00] == 0x00)) {
      sendRspToUpperLayer = true;
      NFCSTATUS mfcRspStatus = NxpMfcReaderInstance.CheckMfcResponse(
          nxpncihal_ctrl.p_rx_data, nxpncihal_ctrl.rx_data_len);
      NXPLOG_NCIHAL_D("Mfc Response Status = 0x%x", mfcRspStatus);
      SEM_POST(&(nxpncihal_ctrl.ext_cb_data));
    }
    /* Read successful send the event to higher layer */
    else if ((nxpncihal_ctrl.p_nfc_stack_data_cback != NULL) &&
             (status == NFCSTATUS_SUCCESS)) {
      (*nxpncihal_ctrl.p_nfc_stack_data_cback)(nxpncihal_ctrl.rx_data_len,
                                               nxpncihal_ctrl.p_rx_data);
      //workaround for sync issue between SPI and NFC
      if ((nfcFL.chipType == pn557) && nxpncihal_ctrl.p_rx_data[0] == 0x62 &&
          nxpncihal_ctrl.p_rx_data[1] == 0x00 &&
          nxpncihal_ctrl.p_rx_data[3] == 0xC0 &&
          nxpncihal_ctrl.p_rx_data[4] == 0x00) {
        uint8_t nfcee_notifiations[3][9] = {
          {0x61, 0x0A, 0x06, 0x01, 0x00, 0x03, 0xC0, 0x80, 0x04},
          {0x61, 0x0A, 0x06, 0x01, 0x00, 0x03, 0xC0, 0x81, 0x04},
          {0x61, 0x0A, 0x06, 0x01, 0x00, 0x03, 0xC0, 0x82, 0x03},
        };

        for (int i = 0; i < 3; i++) {
          (*nxpncihal_ctrl.p_nfc_stack_data_cback)(sizeof(nfcee_notifiations[i]),
                                               nfcee_notifiations[i]);
        }
      }
    }
  } else {
    NXPLOG_NCIHAL_E("read error status = 0x%x", pInfo->wStatus);
  }

  if (nxpncihal_ctrl.halStatus == HAL_STATUS_CLOSE &&
      nxpncihal_ctrl.nci_info.wait_for_ntf == FALSE) {
    NXPLOG_NCIHAL_D("Ignoring read, HAL close triggered");
    return;
  }
  /* Read again because read must be pending always.*/
  status = phTmlNfc_Read(
      Rx_data, NCI_MAX_DATA_LEN,
      (pphTmlNfc_TransactCompletionCb_t)&phNxpNciHal_read_complete, NULL);
  if (status != NFCSTATUS_PENDING) {
    NXPLOG_NCIHAL_E("read status error status = %x", status);
    /* TODO: Not sure how to handle this ? */
  }

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_core_initialized
 *
 * Description      This function is called by libnfc-nci after successful open
 *                  of NFCC. All proprietary setting for PN54X are done here.
 *                  After completion of proprietary settings notification is
 *                  provided to libnfc-nci through callback function.
 *
 * Returns          Always returns NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_core_initialized(uint8_t* p_core_init_rsp_params) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  static uint8_t p2p_listen_mode_routing_cmd[] = {0x21, 0x01, 0x07, 0x00, 0x01,
                                                  0x01, 0x03, 0x00, 0x01, 0x05};

  uint8_t swp_full_pwr_mode_on_cmd[] = {0x20, 0x02, 0x05, 0x01,
                                        0xA0, 0xF1, 0x01, 0x01};

  static uint8_t cmd_ven_pulld_enable_nci[] = {0x20, 0x02, 0x05, 0x01,
                                               0xA0, 0x07, 0x01, 0x03};

  static uint8_t android_l_aid_matching_mode_on_cmd[] = {
      0x20, 0x02, 0x05, 0x01, 0xA0, 0x91, 0x01, 0x01};
  static uint8_t swp_switch_timeout_cmd[] = {0x20, 0x02, 0x06, 0x01, 0xA0,
                                             0xF3, 0x02, 0x00, 0x00};
  config_success = true;
  uint8_t* buffer = NULL;
  long bufflen = 260;
  long retlen = 0;
  int isfound;
#if (NFC_NXP_HFO_SETTINGS == TRUE)
  /* Temp fix to re-apply the proper clock setting */
  int temp_fix = 1;
#endif
  unsigned long num = 0;
  // initialize dummy FW recovery variables
  gRecFwRetryCount = 0;
  gRecFWDwnld = 0;
  // recovery --start
  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01,
                                    0x00};  // keep configuration
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
  /* reset config cache */
  static uint8_t retry_core_init_cnt;
  if (nxpncihal_ctrl.halStatus != HAL_STATUS_OPEN) {
    return NFCSTATUS_FAILED;
  }
  if ((*p_core_init_rsp_params > 0) &&
      (*p_core_init_rsp_params < 4))  // initializing for recovery.
  {
  retry_core_init:
    config_access = false;
    if (buffer != NULL) {
      free(buffer);
      buffer = NULL;
    }
    if (retry_core_init_cnt > 3) {
      return NFCSTATUS_FAILED;
    }

    status = phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);
    if (NFCSTATUS_SUCCESS == status) {
      NXPLOG_NCIHAL_D("PN54X Reset - SUCCESS\n");
    } else {
      NXPLOG_NCIHAL_D("PN54X Reset - FAILED\n");
    }

    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
    if ((status != NFCSTATUS_SUCCESS) &&
        (nxpncihal_ctrl.retry_cnt >= MAX_RETRY_COUNT)) {
      NXPLOG_NCIHAL_E("Force FW Download, NFCC not coming out from Standby");
      retry_core_init_cnt++;
      goto retry_core_init;
    } else if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NCI_CORE_RESET: Failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    if (*p_core_init_rsp_params == 2) {
      NXPLOG_NCIHAL_E(" Last command is CORE_RESET!!");
      goto invoke_callback;
    }
    if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
      status =
          phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
    } else {
      status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
    }
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NCI_CORE_INIT : Failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    if (*p_core_init_rsp_params == 3) {
      NXPLOG_NCIHAL_E(" Last command is CORE_INIT!!");
      goto invoke_callback;
    }
  }
  // recovery --end

  buffer = (uint8_t*)malloc(bufflen * sizeof(uint8_t));
  if (NULL == buffer) {
    return NFCSTATUS_FAILED;
  }
  config_access = true;
  retlen = 0;
  isfound = GetNxpByteArrayValue(NAME_NXP_ACT_PROP_EXTN, (char*)buffer, bufflen,
                                 &retlen);
  if (retlen > 0) {
    /* NXP ACT Proprietary Ext */
    status = phNxpNciHal_send_ext_cmd(retlen, buffer);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NXP ACT Proprietary Ext failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }
  }

  status = phNxpNciHal_send_ext_cmd(sizeof(cmd_ven_pulld_enable_nci),
                                    cmd_ven_pulld_enable_nci);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("cmd_ven_pulld_enable_nci: Failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

  if (fw_download_success == 1) {
    phNxpNciHal_hci_network_reset();
  }

  // Check if firmware download success
  status = phNxpNciHal_get_mw_eeprom();
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NXP GET MW EEPROM AREA Proprietary Ext failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

  //
  status = phNxpNciHal_check_clock_config();
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("phNxpNciHal_check_clock_config failed");
    retry_core_init_cnt++;
    goto retry_core_init;
  }

#ifdef PN547C2_CLOCK_SETTING
  if (isNxpConfigModified() || (fw_download_success == 1) ||
      (phNxpNciClock.issetConfig)
#if (NFC_NXP_HFO_SETTINGS == TRUE)
      || temp_fix == 1
#endif
      ) {
    // phNxpNciHal_get_clk_freq();
    phNxpNciHal_set_clock();
    phNxpNciClock.issetConfig = false;
#if (NFC_NXP_HFO_SETTINGS == TRUE)
    if (temp_fix == 1) {
      NXPLOG_NCIHAL_D(
          "Applying Default Clock setting and DPLL register at power on");
      /*
      # A0, 0D, 06, 06, 83, 55, 2A, 04, 00 RF_CLIF_CFG_TARGET CLIF_DPLL_GEAR_REG
      # A0, 0D, 06, 06, 82, 33, 14, 17, 00 RF_CLIF_CFG_TARGET CLIF_DPLL_INIT_REG
      # A0, 0D, 06, 06, 84, AA, 85, 00, 80 RF_CLIF_CFG_TARGET
      CLIF_DPLL_INIT_FREQ_REG
      # A0, 0D, 06, 06, 81, 63, 00, 00, 00 RF_CLIF_CFG_TARGET
      CLIF_DPLL_CONTROL_REG
      */
      static uint8_t cmd_dpll_set_reg_nci[] = {
          0x20, 0x02, 0x25, 0x04, 0xA0, 0x0D, 0x06, 0x06, 0x83, 0x55,
          0x2A, 0x04, 0x00, 0xA0, 0x0D, 0x06, 0x06, 0x82, 0x33, 0x14,
          0x17, 0x00, 0xA0, 0x0D, 0x06, 0x06, 0x84, 0xAA, 0x85, 0x00,
          0x80, 0xA0, 0x0D, 0x06, 0x06, 0x81, 0x63, 0x00, 0x00, 0x00};

      status = phNxpNciHal_send_ext_cmd(sizeof(cmd_dpll_set_reg_nci),
                                        cmd_dpll_set_reg_nci);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("NXP DPLL REG ACT Proprietary Ext failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
      /* reset the NFCC after applying the clock setting and DPLL setting */
      // phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);
      temp_fix = 0;
      goto retry_core_init;
    }
#endif
  }
#endif

  retlen = 0;
  config_access = true;
  isfound = GetNxpByteArrayValue(NAME_NXP_NFC_PROFILE_EXTN, (char*)buffer,
                                 bufflen, &retlen);
  if (retlen > 0) {
    /* NXP ACT Proprietary Ext */
    status = phNxpNciHal_send_ext_cmd(retlen, buffer);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NXP ACT Proprietary Ext failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }
  }

  if (isNxpConfigModified() || (fw_download_success == 1)  || (anti_tearing_recovery_success == 1)) {
    NXPLOG_NCIHAL_D("Applying Settings: isNxpConfigModified()=%d, fw_download_success=%d, anti_tearing_recovery_success=%d",
      isNxpConfigModified(), fw_download_success, anti_tearing_recovery_success);

    retlen = 0;
    fw_download_success = 0;

    NXPLOG_NCIHAL_D("Performing TVDD Settings");
    isfound = GetNxpNumValue(NAME_NXP_EXT_TVDD_CFG, &num, sizeof(num));
    if (isfound > 0) {
      if (num == 1) {
        isfound = GetNxpByteArrayValue(NAME_NXP_EXT_TVDD_CFG_1, (char*)buffer,
                                       bufflen, &retlen);
        if (retlen > 0) {
          status = phNxpNciHal_send_ext_cmd(retlen, buffer);
          if (status != NFCSTATUS_SUCCESS) {
            NXPLOG_NCIHAL_E("EXT TVDD CFG 1 Settings failed");
            retry_core_init_cnt++;
            goto retry_core_init;
          }
        }
      } else if (num == 2) {
        isfound = GetNxpByteArrayValue(NAME_NXP_EXT_TVDD_CFG_2, (char*)buffer,
                                       bufflen, &retlen);
        if (retlen > 0) {
          status = phNxpNciHal_send_ext_cmd(retlen, buffer);
          if (status != NFCSTATUS_SUCCESS) {
            NXPLOG_NCIHAL_E("EXT TVDD CFG 2 Settings failed");
            retry_core_init_cnt++;
            goto retry_core_init;
          }
        }
      } else if (num == 3) {
        isfound = GetNxpByteArrayValue(NAME_NXP_EXT_TVDD_CFG_3, (char*)buffer,
                                       bufflen, &retlen);
        if (retlen > 0) {
          status = phNxpNciHal_send_ext_cmd(retlen, buffer);
          if (status != NFCSTATUS_SUCCESS) {
            NXPLOG_NCIHAL_E("EXT TVDD CFG 3 Settings failed");
            retry_core_init_cnt++;
            goto retry_core_init;
          }
        }
      } else {
        NXPLOG_NCIHAL_E("Wrong Configuration Value %ld", num);
      }
    }
    retlen = 0;
    config_access = false;
    NXPLOG_NCIHAL_D("Performing RF Settings BLK 1");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_1, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 1 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;

    NXPLOG_NCIHAL_D("Performing RF Settings BLK 2");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_2, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 2 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;

    NXPLOG_NCIHAL_D("Performing RF Settings BLK 3");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_3, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 3 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;

    NXPLOG_NCIHAL_D("Performing RF Settings BLK 4");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_4, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 4 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;

    NXPLOG_NCIHAL_D("Performing RF Settings BLK 5");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_5, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 5 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;

    NXPLOG_NCIHAL_D("Performing RF Settings BLK 6");
    isfound = GetNxpByteArrayValue(NAME_NXP_RF_CONF_BLK_6, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("RF Settings BLK 6 failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    retlen = 0;
    config_access = true;
    NXPLOG_NCIHAL_D("Performing NAME_NXP_CORE_CONF_EXTN Settings");
    isfound = GetNxpByteArrayValue(NAME_NXP_CORE_CONF_EXTN, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      /* NXP ACT Proprietary Ext */
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("NXP Core configuration failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }

    retlen = 0;
    config_access = false;
    isfound = GetNxpByteArrayValue(NAME_NXP_CORE_RF_FIELD, (char*)buffer,
                                   bufflen, &retlen);
    if (retlen > 0) {
      /* NXP ACT Proprietary Ext */
      status = phNxpNciHal_send_ext_cmd(retlen, buffer);
      if (status == NFCSTATUS_SUCCESS) {
        status = phNxpNciHal_CheckRFCmdRespStatus();
        /*STATUS INVALID PARAM 0x09*/
        if (status == 0x09) {
          phNxpNciHalRFConfigCmdRecSequence();
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else
          if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("Setting NXP_CORE_RF_FIELD status failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    config_access = true;

    retlen = 0;
    /* NXP SWP switch timeout Setting*/
    if (GetNxpNumValue(NAME_NXP_SWP_SWITCH_TIMEOUT, (void*)&retlen,
                       sizeof(retlen))) {
      // Check the permissible range [0 - 60]
      if (0 <= retlen && retlen <= 60) {
        if (0 < retlen) {
          unsigned int timeout = retlen * 1000;
          unsigned int timeoutHx = 0x0000;

          char tmpbuffer[10] = {0};
          snprintf((char*)tmpbuffer, 10, "%04x", timeout);
          sscanf((char*)tmpbuffer, "%x", &timeoutHx);

          swp_switch_timeout_cmd[7] = (timeoutHx & 0xFF);
          swp_switch_timeout_cmd[8] = ((timeoutHx & 0xFF00) >> 8);
        }

        status = phNxpNciHal_send_ext_cmd(sizeof(swp_switch_timeout_cmd),
                                          swp_switch_timeout_cmd);
        if (status != NFCSTATUS_SUCCESS) {
          NXPLOG_NCIHAL_E("SWP switch timeout Setting Failed");
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      } else {
        NXPLOG_NCIHAL_E("SWP switch timeout Setting Failed - out of range!");
      }
    }

    status = phNxpNciHal_china_tianjin_rf_setting();
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("phNxpNciHal_china_tianjin_rf_setting failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    if (!GetNxpNumValue(NAME_NXP_T4T_NFCEE_ENABLE, (void*)&retlen,
                        sizeof(retlen))) {
      retlen = 0x00;
      NXPLOG_NCIHAL_D(
          "T4T_NFCEE_ENABLE not found. Taking default value : 0x%02lx", retlen);
    }

    // Configure t4t ndef emulation
    status = phNxpNciHal_config_t4t_ndef((uint8_t)retlen);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NXP Update MW EEPROM Proprietary Ext failed");
    }

    // Update eeprom value
    status = phNxpNciHal_set_mw_eeprom();
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NXP Update MW EEPROM Proprietary Ext failed");
    }

    anti_tearing_recovery_success = 0;
  }

  retlen = 0;

  NXPLOG_NCIHAL_D("Performing NAME_NXP_CORE_CONF Settings");
  isfound =
      GetNxpByteArrayValue(NAME_NXP_CORE_CONF, (char*)buffer, bufflen, &retlen);
  NXPLOG_NCIHAL_D("NAME_NXP_CORE_CONF Settings Found - %d Len: %ld", isfound, retlen);
  if (retlen > 0) {
    /* NXP ACT Proprietary Ext */
    status = phNxpNciHal_send_ext_cmd(retlen, buffer);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Core Set Config failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }
  }

  config_access = false;
  // if recovery mode and length of last command is 0 then only reset the P2P
  // listen mode routing.
  if ((*p_core_init_rsp_params > 0) && (*p_core_init_rsp_params < 4) &&
      p_core_init_rsp_params[35] == 0) {
    /* P2P listen mode routing */
    status = phNxpNciHal_send_ext_cmd(sizeof(p2p_listen_mode_routing_cmd),
                                      p2p_listen_mode_routing_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("P2P listen mode routing failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }
  }

  retlen = 0;

  /* SWP FULL PWR MODE SETTING ON */
  if (GetNxpNumValue(NAME_NXP_SWP_FULL_PWR_ON, (void*)&retlen,
                     sizeof(retlen))) {
    if (1 == retlen) {
      status = phNxpNciHal_send_ext_cmd(sizeof(swp_full_pwr_mode_on_cmd),
                                        swp_full_pwr_mode_on_cmd);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("SWP FULL PWR MODE SETTING ON CMD FAILED");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    } else {
      swp_full_pwr_mode_on_cmd[7] = 0x00;
      status = phNxpNciHal_send_ext_cmd(sizeof(swp_full_pwr_mode_on_cmd),
                                        swp_full_pwr_mode_on_cmd);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("SWP FULL PWR MODE SETTING OFF CMD FAILED");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
  }

  /* Android L AID Matching Platform Setting*/
  if ((nfcFL.chipType != pn557) && GetNxpNumValue(NAME_AID_MATCHING_PLATFORM, (void*)&retlen,
                     sizeof(retlen))) {
    if (1 == retlen) {
      status =
          phNxpNciHal_send_ext_cmd(sizeof(android_l_aid_matching_mode_on_cmd),
                                   android_l_aid_matching_mode_on_cmd);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("Android L AID Matching Platform Setting Failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    } else if (2 == retlen) {
      android_l_aid_matching_mode_on_cmd[7] = 0x00;
      status =
          phNxpNciHal_send_ext_cmd(sizeof(android_l_aid_matching_mode_on_cmd),
                                   android_l_aid_matching_mode_on_cmd);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("Android L AID Matching Platform Setting Failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
  }

  if ((*p_core_init_rsp_params > 0) && (*p_core_init_rsp_params < 4)) {
    static phLibNfc_Message_t msg;
    uint16_t tmp_len = 0;
    uint8_t uicc_set_mode[] = {0x22, 0x01, 0x02, 0x02, 0x01};
    uint8_t set_screen_state[] = {0x2F, 0x15, 01, 00};  // SCREEN ON
    uint8_t nfcc_core_conn_create[] = {0x20, 0x04, 0x06, 0x03, 0x01,
                                       0x01, 0x02, 0x01, 0x01};
    uint8_t nfcc_mode_set_on[] = {0x22, 0x01, 0x02, 0x01, 0x01};

    NXPLOG_NCIHAL_W(
        "Sending DH and NFCC core connection command as raw packet!!");
    status = phNxpNciHal_send_ext_cmd(sizeof(nfcc_core_conn_create),
                                      nfcc_core_conn_create);

    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E(
          "Sending DH and NFCC core connection command as raw packet!! Failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    NXPLOG_NCIHAL_W("Sending DH and NFCC mode set as raw packet!!");
    status =
        phNxpNciHal_send_ext_cmd(sizeof(nfcc_mode_set_on), nfcc_mode_set_on);

    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Sending DH and NFCC mode set as raw packet!! Failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    NXPLOG_NCIHAL_W("Sending UICC Select Command as raw packet!!");
    status = phNxpNciHal_send_ext_cmd(sizeof(uicc_set_mode), uicc_set_mode);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Sending UICC Select Command as raw packet!! Failed");
      retry_core_init_cnt++;
      goto retry_core_init;
    }

    if (*(p_core_init_rsp_params + 1) == 1)  // RF state is Discovery!!
    {
      NXPLOG_NCIHAL_W("Sending Set Screen ON State Command as raw packet!!");
      status =
          phNxpNciHal_send_ext_cmd(sizeof(set_screen_state), set_screen_state);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E(
            "Sending Set Screen ON State Command as raw packet!! Failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }

      NXPLOG_NCIHAL_W("Sending discovery as raw packet!!");
      status = phNxpNciHal_send_ext_cmd(p_core_init_rsp_params[2],
                                        (uint8_t*)&p_core_init_rsp_params[3]);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("Sending discovery as raw packet Failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }

    } else {
      NXPLOG_NCIHAL_W("Sending Set Screen OFF State Command as raw packet!!");
      set_screen_state[3] = 0x01;  // Screen OFF
      status =
          phNxpNciHal_send_ext_cmd(sizeof(set_screen_state), set_screen_state);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E(
            "Sending Set Screen OFF State Command as raw packet!! Failed");
        retry_core_init_cnt++;
        goto retry_core_init;
      }
    }
    NXPLOG_NCIHAL_W("Sending last command for Recovery ");

    if (p_core_init_rsp_params[35] > 0) {  // if length of last command is 0
                                           // then it doesn't need to send last
                                           // command.
      if (!(((p_core_init_rsp_params[36] == 0x21) &&
             (p_core_init_rsp_params[37] == 0x03)) &&
            (*(p_core_init_rsp_params + 1) == 1)) &&
          !((p_core_init_rsp_params[36] == 0x21) &&
            (p_core_init_rsp_params[37] == 0x06) &&
            (p_core_init_rsp_params[39] == 0x00) &&
            (*(p_core_init_rsp_params + 1) == 0x00)))
      // if last command is discovery and RF status is also discovery state,
      // then it doesn't need to execute or similarly
      // if the last command is deactivate to idle and RF status is also idle ,
      // no need to execute the command .
      {
        tmp_len = p_core_init_rsp_params[35];

        /* Check for NXP ext before sending write */
        status = phNxpNciHal_write_ext(
            &tmp_len, (uint8_t*)&p_core_init_rsp_params[36],
            &nxpncihal_ctrl.rsp_len, nxpncihal_ctrl.p_rsp_data);
        if (status != NFCSTATUS_SUCCESS) {
          if (buffer) {
            free(buffer);
            buffer = NULL;
          }
          /* Do not send packet to PN54X, send response directly */
          msg.eMsgType = NCI_HAL_RX_MSG;
          msg.pMsgData = NULL;
          msg.Size = 0;

          phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId,
                                (phLibNfc_Message_t*)&msg);
          return NFCSTATUS_SUCCESS;
        }

        p_core_init_rsp_params[35] = (uint8_t)tmp_len;

        status = phNxpNciHal_send_ext_cmd(
            p_core_init_rsp_params[35], (uint8_t*)&p_core_init_rsp_params[36]);
        if (status != NFCSTATUS_SUCCESS) {
          NXPLOG_NCIHAL_E("Sending last command for Recovery Failed");
          retry_core_init_cnt++;
          goto retry_core_init;
        }
      }
    }
  }

  retry_core_init_cnt = 0;

  if (buffer) {
    free(buffer);
    buffer = NULL;
  }
  // initialize dummy FW recovery variables
  gRecFWDwnld = 0;
  gRecFwRetryCount = 0;
  if (!((*p_core_init_rsp_params > 0) && (*p_core_init_rsp_params < 4)))
    phNxpNciHal_core_initialized_complete(status);
  else {
  invoke_callback:
    config_access = false;
    if (nxpncihal_ctrl.p_nfc_stack_data_cback != NULL) {
      *p_core_init_rsp_params = 0;
      NXPLOG_NCIHAL_W("Invoking data callback!!");
      (*nxpncihal_ctrl.p_nfc_stack_data_cback)(nxpncihal_ctrl.rx_data_len,
                                               nxpncihal_ctrl.p_rx_data);
    }
  }

  if (config_success == false) return NFCSTATUS_FAILED;
#ifdef PN547C2_CLOCK_SETTING
  if (isNxpConfigModified()) {
    updateNxpConfigTimestamp();
  }
#endif
  return NFCSTATUS_SUCCESS;
}

#ifdef FactoryOTA
void phNxpNciHal_isFactoryOTAModeActive() {
  uint8_t check_factoryOTA[] = {0x20, 0x03, 0x05, 0x02, 0xA0, 0x08, 0xA0, 0x88};
  NFCSTATUS status = NFCSTATUS_FAILED;
  NXPLOG_NCIHAL_D("check FactoryOTA mode status");

  status = phNxpNciHal_send_ext_cmd(sizeof(check_factoryOTA), check_factoryOTA);

  if (status == NFCSTATUS_SUCCESS) {
    if(nxpncihal_ctrl.p_rx_data[9] == 0x1 && nxpncihal_ctrl.p_rx_data[13] == 0x1) {
      NXPLOG_NCIHAL_D("FactoryOTA mode is active");
    } else {
      NXPLOG_NCIHAL_D("FactoryOTA mode is disabled");
    }
  } else {
    NXPLOG_NCIHAL_E("Fail to get FactoryOTA mode status");
  }
  return;
}

NFCSTATUS phNxpNciHal_disableFactoryOTAMode() {
  // NFCC GPIO output control
  uint8_t nfcc_system_gpio[] = {0x20, 0x02, 0x06, 0x01, 0xA0, 0x08, 0x02, 0x00, 0x00};
  // NFCC automatically sets GPIO once a specific RF pattern is detected
  uint8_t nfcc_gpio_pattern[] = {0x20, 0x02, 0x08, 0x01, 0xA0, 0x88, 0x04, 0x00, 0x96, 0x96, 0x03};

  NFCSTATUS status = NFCSTATUS_SUCCESS;
  NXPLOG_NCIHAL_D("Disable FactoryOTA mode");
  status = phNxpNciHal_send_ext_cmd(sizeof(nfcc_system_gpio), nfcc_system_gpio);
  if (status != NFCSTATUS_SUCCESS ) {
    NXPLOG_NCIHAL_E("Can't disable A008 for FactoryOTA mode");
  }
  status = phNxpNciHal_send_ext_cmd(sizeof(nfcc_gpio_pattern), nfcc_gpio_pattern);
  if (status != NFCSTATUS_SUCCESS ) {
    NXPLOG_NCIHAL_E("Can't disable A088 for FactoryOTA mode");
  }
  return status;
}
#endif

/******************************************************************************
 * Function         phNxpNciHal_CheckRFCmdRespStatus
 *
 * Description      This function is called to check the resp status of
 *                  RF update commands.
 *
 * Returns          NFCSTATUS_SUCCESS           if successful,
 *                  NFCSTATUS_INVALID_PARAMETER if parameter is inavlid
 *                  NFCSTATUS_FAILED            if failed response
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_CheckRFCmdRespStatus() {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  static uint16_t INVALID_PARAM = 0x09;
  if ((nxpncihal_ctrl.rx_data_len > 0) && (nxpncihal_ctrl.p_rx_data[2] > 0)) {
    if (nxpncihal_ctrl.p_rx_data[3] == 0x09) {
      status = INVALID_PARAM;
    } else if (nxpncihal_ctrl.p_rx_data[3] != NFCSTATUS_SUCCESS) {
      status = NFCSTATUS_FAILED;
    }
  }
  return status;
}
/******************************************************************************
 * Function         phNxpNciHalRFConfigCmdRecSequence
 *
 * Description      This function is called to handle dummy FW recovery sequence
 *                  Whenever RF settings are failed to apply with invalid param
 *                  response, recovery mechanism includes dummy firmware
 *download
 *                  followed by firmware download and then config settings. The
 *dummy
 *                  firmware changes the major number of the firmware inside
 *NFCC.
 *                  Then actual firmware dowenload will be successful. This can
 *be
 *                  retried maximum three times.
 *
 * Returns          Always returns NFCSTATUS_SUCCESS
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHalRFConfigCmdRecSequence() {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint16_t recFWState = 1;
  gRecFWDwnld = true;
  gRecFwRetryCount++;
  if (gRecFwRetryCount > 0x03) {
    NXPLOG_NCIHAL_D("Max retry count for RF config FW recovery exceeded ");
    gRecFWDwnld = false;
    return NFCSTATUS_FAILED;
  }
  do {
    status = phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);
    phDnldNfc_InitImgInfo();
    if (NFCSTATUS_SUCCESS == phNxpNciHal_CheckValidFwVersion()) {
      fw_download_success = 0;
      status = phNxpNciHal_fw_download();
      if (status == NFCSTATUS_SUCCESS) {
        fw_download_success = 1;
      }
      status = phTmlNfc_Read(
          nxpncihal_ctrl.p_cmd_data, NCI_MAX_DATA_LEN,
          (pphTmlNfc_TransactCompletionCb_t)&phNxpNciHal_read_complete, NULL);
      if (status != NFCSTATUS_PENDING) {
        NXPLOG_NCIHAL_E("TML Read status error status = %x", status);
        phOsalNfc_Timer_Cleanup();
        phTmlNfc_Shutdown();
        status = NFCSTATUS_FAILED;
      }
      break;
    }
    gRecFWDwnld = false;
  } while (recFWState--);
  gRecFWDwnld = false;
  return status;
}
/******************************************************************************
 * Function         phNxpNciHal_core_initialized_complete
 *
 * Description      This function is called when phNxpNciHal_core_initialized
 *                  complete all proprietary command exchanges. This function
 *                  informs libnfc-nci about completion of core initialize
 *                  and result of that through callback.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_core_initialized_complete(NFCSTATUS status) {
  static phLibNfc_Message_t msg;

  if (status == NFCSTATUS_SUCCESS) {
    msg.eMsgType = NCI_HAL_POST_INIT_CPLT_MSG;
  } else {
    msg.eMsgType = NCI_HAL_ERROR_MSG;
  }
  msg.pMsgData = NULL;
  msg.Size = 0;

  phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId,
                        (phLibNfc_Message_t*)&msg);

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_pre_discover
 *
 * Description      This function is called by libnfc-nci to perform any
 *                  proprietary exchange before RF discovery.
 *
 * Returns          It always returns NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_pre_discover(void) {
  /* Nothing to do here for initial version */
  return NFCSTATUS_SUCCESS;
}

/******************************************************************************
 * Function         phNxpNciHal_close
 *
 * Description      This function close the NFCC interface and free all
 *                  resources.This is called by libnfc-nci on NFC service stop.
 *
 * Returns          Always return NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_close(bool bShutdown) {
  NFCSTATUS status;
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};

  static uint8_t cmd_ven_disable_nci[] = {0x20, 0x02, 0x05, 0x01,
                                         0xA0, 0x07, 0x01, 0x02};

  AutoThreadMutex a(sHalFnLock);
  if (nxpncihal_ctrl.halStatus == HAL_STATUS_CLOSE) {
    NXPLOG_NCIHAL_D("phNxpNciHal_close is already closed, ignoring close");
    return NFCSTATUS_FAILED;
  }

  CONCURRENCY_LOCK();

  int sem_val;
  sem_getvalue(&(nxpncihal_ctrl.syncSpiNfc), &sem_val);
  if(sem_val == 0 ) {
      sem_post(&(nxpncihal_ctrl.syncSpiNfc));
  }
  if(!bShutdown){
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_ven_disable_nci), cmd_ven_disable_nci);
    if(status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("CMD_VEN_DISABLE_NCI: Failed");
    }
  }
#ifdef FactoryOTA
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  bool factoryOTA_terminate = false;
#if 0
  int len = roperty_get("persist.factoryota.reboot", valueStr, "normal");
  if (len > 0) {
    factoryOTA_terminate = (len == 9 && (memcmp(valueStr, "terminate", len) == 0)) ? true : false;:
  }
  NXPLOG_NCIHAL_D("factoryOTA_terminate: %d", factoryOTA_terminate);
  if (factoryOTA_terminate) {
    phNxpNciHal_disableFactoryOTAMode();
    phNxpNciHal_isFactoryOTAModeActive();
  }
#endif
#endif
  nxpncihal_ctrl.halStatus = HAL_STATUS_CLOSE;

  status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NCI_CORE_RESET: Failed");
  }

  sem_destroy(&nxpncihal_ctrl.syncSpiNfc);

  if (NULL != gpphTmlNfc_Context->pDevHandle) {
    phNxpNciHal_close_complete(NFCSTATUS_SUCCESS);
    /* Abort any pending read and write */
    status = phTmlNfc_ReadAbort();
    status = phTmlNfc_WriteAbort();

    phOsalNfc_Timer_Cleanup();

    status = phTmlNfc_Shutdown();

    if (0 != pthread_join(nxpncihal_ctrl.client_thread, (void **)NULL)) {
      NXPLOG_TML_E("Fail to kill client thread!");
    }

    phTmlNfc_CleanUp();

    phDal4Nfc_msgrelease(nxpncihal_ctrl.gDrvCfg.nClientId);

    memset(&nxpncihal_ctrl, 0x00, sizeof(nxpncihal_ctrl));

    NXPLOG_NCIHAL_D("phNxpNciHal_close - phOsalNfc_DeInit completed");
  }
  //NfccPowerTracker::getInstance().Pause();
  CONCURRENCY_UNLOCK();

  phNxpNciHal_cleanup_monitor();

  /* Return success always */
  return NFCSTATUS_SUCCESS;
}

/******************************************************************************
 * Function         phNxpNciHal_close_complete
 *
 * Description      This function inform libnfc-nci about result of
 *                  phNxpNciHal_close.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_close_complete(NFCSTATUS status) {
  static phLibNfc_Message_t msg;

  if (status == NFCSTATUS_SUCCESS) {
    msg.eMsgType = NCI_HAL_CLOSE_CPLT_MSG;
  } else {
    msg.eMsgType = NCI_HAL_ERROR_MSG;
  }
  msg.pMsgData = NULL;
  msg.Size = 0;

  phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId, &msg);

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_configDiscShutdown
 *
 * Description      Enable the CE and VEN config during shutdown.
 *
 * Returns          Always return NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_configDiscShutdown(void) {
  NFCSTATUS status;
  NfccPowerTracker::getInstance().Reset();

  status = phNxpNciHal_close(true);
  if(status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NCI_HAL_CLOSE: Failed");
  }

  /* Return success always */
  return NFCSTATUS_SUCCESS;
}

#if(NXP_EXTNS == TRUE)
/******************************************************************************
 * Function         phNxpNciHal_getNxp
 *
 * Description      This function can be used by HAL to inform
 *                 to update vendor configuration parametres
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_getNxpConfig(nfc_nci_IoctlInOutData_t *pInpOutData) {
  unsigned long num = 0;
  uint8_t *buffer = NULL;
  long bufflen = 260;

  buffer = (uint8_t *)malloc(bufflen * sizeof(uint8_t));
  memset(&pInpOutData->out.data.nxpConfigs, 0x00, sizeof(pInpOutData->out.data.nxpConfigs));
  if (GetNxpNumValue(NAME_NXP_AGC_DEBUG_ENABLE, &num, sizeof(num))) {
    pInpOutData->out.data.nxpConfigs.wAgcDebugEnable = num;
  }else {
    pInpOutData->out.data.nxpConfigs.wAgcDebugEnable = 0x00;
  }

  if (GetNxpNumValue(NAME_NXP_T4T_NFCEE_ENABLE, &num, sizeof(num))) {
    pInpOutData->out.data.nxpConfigs.wT4TNdefEnable = num;
  }else {
    pInpOutData->out.data.nxpConfigs.wT4TNdefEnable = 0x00;
  }

  if (GetNxpNumValue(NAME_DEFAULT_T4TNFCEE_AID_POWER_STATE, &num, sizeof(num))) {
    pInpOutData->out.data.nxpConfigs.wT4TPowerState = num;
  }else {
    pInpOutData->out.data.nxpConfigs.wT4TPowerState = 0x00;
  }

}
#endif
#if 0
/******************************************************************************
 * Function         phNxpNciHal_getVendorConfig
 *
 * Description      This function can be used by HAL to inform
 *                 to update vendor configuration parametres
 *
 * Returns          void.
 *
 ******************************************************************************/

void phNxpNciHal_getVendorConfig(android::hardware::nfc::V1_1::NfcConfig& config) {
  unsigned long num = 0;
  std::array<uint8_t, NXP_MAX_CONFIG_STRING_LEN> buffer;
  buffer.fill(0);
  long retlen = 0;
  memset(&config, 0x00, sizeof(android::hardware::nfc::V1_1::NfcConfig));

  config.nfaPollBailOutMode = true;
  if (GetNxpNumValue(NAME_ISO_DEP_MAX_TRANSCEIVE, &num, sizeof(num))) {
    config.maxIsoDepTransceiveLength = num;
  }
  if (GetNxpNumValue(NAME_DEFAULT_OFFHOST_ROUTE, &num, sizeof(num))) {
    config.defaultOffHostRoute = num;
  }
  if (GetNxpNumValue(NAME_DEFAULT_NFCF_ROUTE, &num, sizeof(num))) {
    config.defaultOffHostRouteFelica = num;
  }
  if (GetNxpNumValue(NAME_DEFAULT_SYS_CODE_ROUTE, &num, sizeof(num))) {
    config.defaultSystemCodeRoute = num;
  }
  if (GetNxpNumValue(NAME_DEFAULT_SYS_CODE_PWR_STATE, &num, sizeof(num))) {
    config.defaultSystemCodePowerState = num;
  }
  if (GetNxpNumValue(NAME_DEFAULT_ROUTE, &num, sizeof(num))) {
    config.defaultRoute = num;
  }
  if (GetNxpByteArrayValue(NAME_DEVICE_HOST_WHITE_LIST, (char*)buffer.data(), buffer.size(), &retlen)) {
    config.hostWhitelist.resize(retlen);
    for(int i=0; i<retlen; i++)
      config.hostWhitelist[i] = buffer[i];
  }
  if (GetNxpNumValue(NAME_OFF_HOST_ESE_PIPE_ID, &num, sizeof(num))) {
    config.offHostESEPipeId = num;
  }
  if (GetNxpNumValue(NAME_OFF_HOST_SIM_PIPE_ID, &num, sizeof(num))) {
    config.offHostSIMPipeId = num;
  }
  if ((GetNxpByteArrayValue(NAME_NFA_PROPRIETARY_CFG, (char*)buffer.data(), buffer.size(), &retlen))
         && (retlen == 9)) {
    config.nfaProprietaryCfg.protocol18092Active = (uint8_t) buffer[0];
    config.nfaProprietaryCfg.protocolBPrime = (uint8_t) buffer[1];
    config.nfaProprietaryCfg.protocolDual = (uint8_t) buffer[2];
    config.nfaProprietaryCfg.protocol15693 = (uint8_t) buffer[3];
    config.nfaProprietaryCfg.protocolKovio = (uint8_t) buffer[4];
    config.nfaProprietaryCfg.protocolMifare = (uint8_t) buffer[5];
    config.nfaProprietaryCfg.discoveryPollKovio = (uint8_t) buffer[6];
    config.nfaProprietaryCfg.discoveryPollBPrime = (uint8_t) buffer[7];
    config.nfaProprietaryCfg.discoveryListenBPrime = (uint8_t) buffer[8];
  } else {
    memset(&config.nfaProprietaryCfg, 0xFF, sizeof(ProtocolDiscoveryConfig));
  }
  if ((GetNxpNumValue(NAME_PRESENCE_CHECK_ALGORITHM, &num, sizeof(num))) && (num <= 2) ) {
      config.presenceCheckAlgorithm = (PresenceCheckAlgorithm)num;
  }
}

/******************************************************************************
 * Function         phNxpNciHal_getVendorConfig_1_2
 *
 * Description      This function can be used by HAL to inform
 *                 to update vendor configuration parametres
 *
 * Returns          void.
 *
 ******************************************************************************/

void phNxpNciHal_getVendorConfig_1_2(android::hardware::nfc::V1_2::NfcConfig& config) {
  unsigned long num = 0;
  std::array<uint8_t, NXP_MAX_CONFIG_STRING_LEN> buffer;
  buffer.fill(0);
  long retlen = 0;
  memset(&config, 0x00, sizeof(android::hardware::nfc::V1_2::NfcConfig));
  phNxpNciHal_getVendorConfig(config.v1_1);

  if (GetNxpByteArrayValue(NAME_OFFHOST_ROUTE_UICC, (char*)buffer.data(), buffer.size(), &retlen)) {
    config.offHostRouteUicc.resize(retlen);
    for(int i=0; i<retlen; i++)
      config.offHostRouteUicc[i] = buffer[i];
  }

  if (GetNxpByteArrayValue(NAME_OFFHOST_ROUTE_ESE, (char*)buffer.data(), buffer.size(), &retlen)) {
    config.offHostRouteEse.resize(retlen);
    for(int i=0; i<retlen; i++)
      config.offHostRouteEse[i] = buffer[i];
  }

  if (GetNxpNumValue(NAME_DEFAULT_ISODEP_ROUTE, &num, sizeof(num))) {
      config.defaultIsoDepRoute = num;
  }

}
#endif
/******************************************************************************
 * Function         phNxpNciHal_notify_i2c_fragmentation
 *
 * Description      This function can be used by HAL to inform
 *                 libnfc-nci that i2c fragmentation is enabled/disabled
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_notify_i2c_fragmentation(void) {
  if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
    /*inform libnfc-nci that i2c fragmentation is enabled/disabled */
    (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_ENABLE_I2C_FRAGMENTATION_EVT,
                                        HAL_NFC_STATUS_OK);
  }
}
/******************************************************************************
 * Function         phNxpNciHal_control_granted
 *
 * Description      Called by libnfc-nci when NFCC control is granted to HAL.
 *
 * Returns          Always returns NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_control_granted(void) {
  /* Take the concurrency lock so no other calls from upper layer
   * will be allowed
   */
  CONCURRENCY_LOCK();

  if (NULL != nxpncihal_ctrl.p_control_granted_cback) {
    (*nxpncihal_ctrl.p_control_granted_cback)();
  }
  /* At the end concurrency unlock so calls from upper layer will
   * be allowed
   */
  CONCURRENCY_UNLOCK();
  return NFCSTATUS_SUCCESS;
}

/******************************************************************************
 * Function         phNxpNciHal_request_control
 *
 * Description      This function can be used by HAL to request control of
 *                  NFCC to libnfc-nci. When control is provided to HAL it is
 *                  notified through phNxpNciHal_control_granted.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_request_control(void) {
  if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
    /* Request Control of NCI Controller from NCI NFC Stack */
    (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_REQUEST_CONTROL_EVT,
                                        HAL_NFC_STATUS_OK);
  }

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_release_control
 *
 * Description      This function can be used by HAL to release the control of
 *                  NFCC back to libnfc-nci.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_release_control(void) {
  if (nxpncihal_ctrl.p_nfc_stack_cback != NULL) {
    /* Release Control of NCI Controller to NCI NFC Stack */
    (*nxpncihal_ctrl.p_nfc_stack_cback)(HAL_NFC_RELEASE_CONTROL_EVT,
                                        HAL_NFC_STATUS_OK);
  }

  return;
}

/******************************************************************************
 * Function         phNxpNciHal_power_cycle
 *
 * Description      This function is called by libnfc-nci when power cycling is
 *                  performed. When processing is complete it is notified to
 *                  libnfc-nci through phNxpNciHal_power_cycle_complete.
 *
 * Returns          Always return NFCSTATUS_SUCCESS (0).
 *
 ******************************************************************************/
int phNxpNciHal_power_cycle(void) {
  NXPLOG_NCIHAL_D("Power Cycle");
  NFCSTATUS status = NFCSTATUS_FAILED;
  if (nxpncihal_ctrl.halStatus != HAL_STATUS_OPEN) {
    NXPLOG_NCIHAL_D("Power Cycle failed due to hal status not open");
    return NFCSTATUS_FAILED;
  }
  status = phTmlNfc_IoCtl(phTmlNfc_e_ResetDevice);

  if (NFCSTATUS_SUCCESS == status) {
    NXPLOG_NCIHAL_D("PN54X Reset - SUCCESS\n");
  } else {
    NXPLOG_NCIHAL_D("PN54X Reset - FAILED\n");
  }
  phNxpNciHal_power_cycle_complete(NFCSTATUS_SUCCESS);
  return NFCSTATUS_SUCCESS;
}

/******************************************************************************
 * Function         phNxpNciHal_power_cycle_complete
 *
 * Description      This function is called to provide the status of
 *                  phNxpNciHal_power_cycle to libnfc-nci through callback.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_power_cycle_complete(NFCSTATUS status) {
  static phLibNfc_Message_t msg;

  if (status == NFCSTATUS_SUCCESS) {
    msg.eMsgType = NCI_HAL_OPEN_CPLT_MSG;
  } else {
    msg.eMsgType = NCI_HAL_ERROR_MSG;
  }
  msg.pMsgData = NULL;
  msg.Size = 0;

  phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId, &msg);

  return;
}
/******************************************************************************
 * Function         phNxpNciHal_check_ncicmd_write_window
 *
 * Description      This function is called to check the write synchroniztion
 *                  status if write already aquired then wait for corresponding
                    read to complete.
 *
 * Returns          return 0 on success and -1 on fail.
 *
 ******************************************************************************/

int phNxpNciHal_check_ncicmd_write_window(uint16_t cmd_len, uint8_t* p_cmd) {
  UNUSED(cmd_len);
  NFCSTATUS status = NFCSTATUS_FAILED;
  int sem_timedout = 2, s;
  struct timespec ts;
  if ((p_cmd[0] & 0xF0) == 0x20) {
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += sem_timedout;

    while ((s = sem_timedwait(&nxpncihal_ctrl.syncSpiNfc, &ts)) == -1 &&
           errno == EINTR)
      continue; /* Restart if interrupted by handler */

    if (s != -1) {
      status = NFCSTATUS_SUCCESS;
    }
  } else {
    /* cmd window check not required for writing data packet */
    status = NFCSTATUS_SUCCESS;
  }
  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_ioctl
 *
 * Description      This function is called by jni when wired mode is
 *                  performed.First Pn54x driver will give the access
 *                  permission whether wired mode is allowed or not
 *                  arg (0):
 * Returns          return 0 on success and -1 on fail, On success
 *                  update the acutual state of operation in arg pointer
 *
 ******************************************************************************/
int phNxpNciHal_ioctl(long arg, void* p_data) {
  NXPLOG_NCIHAL_D("%s : enter - arg = %ld", __func__, arg);
  nfc_nci_IoctlInOutData_t* pInpOutData = (nfc_nci_IoctlInOutData_t*)p_data;
  int ret = -1;
  long level;
  level=pInpOutData->inp.level;
  if(nxpncihal_ctrl.halStatus == HAL_STATUS_CLOSE)
   {
       NFCSTATUS status = NFCSTATUS_FAILED;
       status = phNxpNciHal_MinOpen();
       if(status != NFCSTATUS_SUCCESS )
       {
         pInpOutData->out.data.nciRsp.p_rsp[3]=1;
         return -1;
       }
   }
  switch (arg) {
    case HAL_NFC_IOCTL_SPI_DWP_SYNC:
           {
                  ret = phNxpNciHal_send_ese_hal_cmd(pInpOutData->inp.data.nciCmd.cmd_len,
                                     pInpOutData->inp.data.nciCmd.p_cmd);
                  pInpOutData->out.data.nciRsp.rsp_len = nxpncihal_ctrl.rx_ese_data_len;
                  if ((nxpncihal_ctrl.rx_ese_data_len > 0) &&
                    (nxpncihal_ctrl.rx_ese_data_len <= MAX_IOCTL_TRANSCEIVE_RESP_LEN) &&
                            (nxpncihal_ctrl.p_rx_ese_data != NULL)) {
                  memcpy(pInpOutData->out.data.nciRsp.p_rsp, nxpncihal_ctrl.p_rx_ese_data,
                    nxpncihal_ctrl.rx_ese_data_len);
                  }

                  if(pInpOutData->out.data.nciRsp.p_rsp[0] == 0x4F && pInpOutData->out.data.nciRsp.p_rsp[1] == 0x01
                      && pInpOutData->out.data.nciRsp.p_rsp[2] == 0x01 && pInpOutData->out.data.nciRsp.p_rsp[3] == 0x00
                      && pInpOutData->inp.data.nciCmd.p_cmd[3] == 0x01)
                  {
                      NXPLOG_NCIHAL_D("OMAPI COMMAND for Open SUCCESS : 0x%x",pInpOutData->out.data.nciRsp.p_rsp[3]);
                      ret=pInpOutData->out.data.nciRsp.p_rsp[3];
                  }
                  else if(pInpOutData->out.data.nciRsp.p_rsp[0] == 0x4F && pInpOutData->out.data.nciRsp.p_rsp[1] == 0x01
                      && pInpOutData->out.data.nciRsp.p_rsp[2] == 0x01 && pInpOutData->out.data.nciRsp.p_rsp[3] == 0x00
                      && pInpOutData->inp.data.nciCmd.p_cmd[3] == 0x00)

                  {
                      NXPLOG_NCIHAL_D("OMAPI COMMAND for Close SUCCESS : 0x%x",pInpOutData->out.data.nciRsp.p_rsp[3]);
                      ret=pInpOutData->out.data.nciRsp.p_rsp[3];
                  }
                  else{
                      NXPLOG_NCIHAL_D("OMAPI COMMAND FAILURE : 0x%x",pInpOutData->out.data.nciRsp.p_rsp[3]);
                      ret=pInpOutData->out.data.nciRsp.p_rsp[3]=3; //magic number for omapi failure
                  }
       }
      break;
    case HAL_NFC_SET_SPM_PWR:
        //ret = phPalEse_spi_ioctl(phPalEse_e_ChipRst, gpphTmlNfc_Context->pDevHandle, level);
        if ((nxpncihal_ctrl.halStatus == HAL_STATUS_MIN_OPEN) && (level == 0x01)) {
          NXPLOG_NCIHAL_D(" HAL close after SPI close , while NFC is Off");
          phNxpNciHal_close(false);
        }
        break;
    case HAL_NFC_SET_POWER_SCHEME:
         //ret = phPalEse_spi_ioctl(phPalEse_e_SetPowerScheme,gpphTmlNfc_Context->pDevHandle,level);
         break;
    case HAL_NFC_GET_SPM_STATUS:
         //ret = phPalEse_spi_ioctl(phPalEse_e_GetSPMStatus, gpphTmlNfc_Context->pDevHandle,level);
         break;
    case HAL_NFC_GET_ESE_ACCESS:
         //ret = phPalEse_spi_ioctl(phPalEse_e_GetEseAccess, gpphTmlNfc_Context->pDevHandle, level);
         break;
    case HAL_NFC_SET_DWNLD_STATUS:
         //ret = phPalEse_spi_ioctl(phPalEse_e_SetJcopDwnldState, gpphTmlNfc_Context->pDevHandle, level);
         break;
    case HAL_NFC_INHIBIT_PWR_CNTRL:
         //ret = phPalEse_spi_ioctl(phPalEse_e_DisablePwrCntrl, gpphTmlNfc_Context->pDevHandle, level);
         break;
    case HAL_NFC_IOCTL_RF_STATUS_UPDATE:
        NXPLOG_NCIHAL_D("HAL_NFC_IOCTL_RF_STATUS_UPDATE Enter value is %d: \n",pInpOutData->inp.data.nciCmd.p_cmd[0]);
#ifdef ANDROID
        if(gpEseAdapt !=  NULL)
        ret = gpEseAdapt->HalIoctl(HAL_NFC_IOCTL_RF_STATUS_UPDATE,pInpOutData);
#endif
        break;
#if(NXP_EXTNS == TRUE)
    case HAL_NFC_GET_NXP_CONFIG:
      phNxpNciHal_getNxpConfig(pInpOutData);
      ret = 0;
      break;
#endif
    default:
      NXPLOG_NCIHAL_E("%s : Wrong arg = %ld", __func__, arg);
      break;
    }
  NXPLOG_NCIHAL_D("%s : exit - ret = %d", __func__, ret);
  return ret;
}


/******************************************************************************
 * Function         phNxpNciHal_get_mw_eeprom
 *
 * Description      This function is called to retreive data in mw eeprom area
 *
 * Returns          NFCSTATUS.
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_get_mw_eeprom(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t retry_cnt = 0;
  static uint8_t get_mw_eeprom_cmd[] = {0x20, 0x03, 0x03, 0x01, 0xA0, 0x0F};

retry_send_ext:
  if (retry_cnt > 3) {
    return NFCSTATUS_FAILED;
  }

  phNxpNciMwEepromArea.isGetEepromArea = true;
  status =
      phNxpNciHal_send_ext_cmd(sizeof(get_mw_eeprom_cmd), get_mw_eeprom_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("unable to get the mw eeprom data");
    phNxpNciMwEepromArea.isGetEepromArea = false;
    retry_cnt++;
    goto retry_send_ext;
  }
  phNxpNciMwEepromArea.isGetEepromArea = false;

  if (phNxpNciMwEepromArea.p_rx_data[12]) {
    fw_download_success = 1;
  }
  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_set_mw_eeprom
 *
 * Description      This function is called to update data in mw eeprom area
 *
 * Returns          void.
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_set_mw_eeprom(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t retry_cnt = 0;
  uint8_t set_mw_eeprom_cmd[39] = {0};
  uint8_t cmd_header[] = {0x20, 0x02, 0x24, 0x01, 0xA0, 0x0F, 0x20};

  memcpy(set_mw_eeprom_cmd, cmd_header, sizeof(cmd_header));
  phNxpNciMwEepromArea.p_rx_data[12] = 0;
  memcpy(set_mw_eeprom_cmd + sizeof(cmd_header), phNxpNciMwEepromArea.p_rx_data,
         sizeof(phNxpNciMwEepromArea.p_rx_data));

retry_send_ext:
  if (retry_cnt > 3) {
    return NFCSTATUS_FAILED;
  }

  status =
      phNxpNciHal_send_ext_cmd(sizeof(set_mw_eeprom_cmd), set_mw_eeprom_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("unable to update the mw eeprom data");
    retry_cnt++;
    goto retry_send_ext;
  }
  return status;
}
/******************************************************************************
 * Function         phNxpNciHal_config_t4t_ndef
 *
 * Description      This function is called to configure T4T Ndef emulation
 *
 * Returns          void.
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_config_t4t_ndef(uint8_t t4tFlag) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  NXPLOG_NCIHAL_D("NxpNci phNxpNciHal_enable_ndef");
  uint8_t retry_cnt = 0;
  uint8_t set_mw_eeprom_cmd[8] = {0};
  uint8_t cmd_header[] = {0x20, 0x02, 0x05, 0x01, 0xA0, 0x95, 0x01, t4tFlag};

  memcpy(set_mw_eeprom_cmd, cmd_header, sizeof(cmd_header));

retry_send_ext:
  if (retry_cnt > 3) {
    return NFCSTATUS_FAILED;
  }

  status =
      phNxpNciHal_send_ext_cmd(sizeof(set_mw_eeprom_cmd), set_mw_eeprom_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("unable to update the mw eeprom data");
    retry_cnt++;
    goto retry_send_ext;
  }
  return status;
}
/******************************************************************************
 * Function         phNxpNciHal_set_clock
 *
 * Description      This function is called after successfull download
 *                  to apply the clock setting provided in config file
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_set_clock(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  int retryCount = 0;

retrySetclock:
  phNxpNciClock.isClockSet = true;
  if (nxpprofile_ctrl.bClkSrcVal == CLK_SRC_PLL) {
    static uint8_t set_clock_cmd[] = {0x20, 0x02, 0x09, 0x02, 0xA0, 0x03,
                                      0x01, 0x11, 0xA0, 0x04, 0x01, 0x01};
    uint8_t param_clock_src = 0x00;
    if((nfcFL.chipType != pn553)&&(nfcFL.chipType != pn557)) {
      uint8_t param_clock_src = CLK_SRC_PLL;
      param_clock_src = param_clock_src << 3;
    }

    if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_13MHZ) {
      param_clock_src |= 0x00;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_19_2MHZ) {
      param_clock_src |= 0x01;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_24MHZ) {
      param_clock_src |= 0x02;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_26MHZ) {
      param_clock_src |= 0x03;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_38_4MHZ) {
      param_clock_src |= 0x04;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_52MHZ) {
      param_clock_src |= 0x05;
    } else {
      NXPLOG_NCIHAL_E("Wrong clock freq, send default PLL@19.2MHz");
      if((nfcFL.chipType == pn553) || (nfcFL.chipType == pn557)) {
        param_clock_src = 0x01;
      } else {
        param_clock_src = 0x11;
      }
    }

    set_clock_cmd[7] = param_clock_src;
    set_clock_cmd[11] = nxpprofile_ctrl.bTimeout;
    status = phNxpNciHal_send_ext_cmd(sizeof(set_clock_cmd), set_clock_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("PLL colck setting failed !!");
    }
  } else if (nxpprofile_ctrl.bClkSrcVal == CLK_SRC_XTAL) {
    static uint8_t set_clock_cmd[] = {0x20, 0x02, 0x05, 0x01,
                                      0xA0, 0x03, 0x01, 0x08};
    status = phNxpNciHal_send_ext_cmd(sizeof(set_clock_cmd), set_clock_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("XTAL colck setting failed !!");
    }
  } else {
    NXPLOG_NCIHAL_E("Wrong clock source. Dont apply any modification")
  }

  // Checking for SET CONFG SUCCESS, re-send the command  if not.
  phNxpNciClock.isClockSet = false;
  if (phNxpNciClock.p_rx_data[3] != NFCSTATUS_SUCCESS) {
    if (retryCount++ < 3) {
      NXPLOG_NCIHAL_D("Set-clk failed retry again ");
      goto retrySetclock;
    } else {
      NXPLOG_NCIHAL_E("Set clk  failed -  max count = 0x%x exceeded ",
                      retryCount);
      //            NXPLOG_NCIHAL_E("Set Config is failed for Clock Due to
      //            elctrical disturbances, aborting the NFC process");
      //            abort ();
    }
  }
}

/******************************************************************************
 * Function         phNxpNciHal_check_clock_config
 *
 * Description      This function is called after successfull download
 *                  to check if clock settings in config file and chip
 *                  is same
 *
 * Returns          void.
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_check_clock_config(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t param_clock_src;
  static uint8_t get_clock_cmd[] = {0x20, 0x03, 0x07, 0x03, 0xA0,
                                    0x02, 0xA0, 0x03, 0xA0, 0x04};
  phNxpNciClock.isClockSet = true;
  phNxpNciHal_get_clk_freq();
  status = phNxpNciHal_send_ext_cmd(sizeof(get_clock_cmd), get_clock_cmd);

  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("unable to retrieve get_clk_src_sel");
    return status;
  }
  param_clock_src = check_config_parameter();
  if (phNxpNciClock.p_rx_data[12] == param_clock_src &&
      phNxpNciClock.p_rx_data[16] == nxpprofile_ctrl.bTimeout) {
    phNxpNciClock.issetConfig = false;
  } else {
    phNxpNciClock.issetConfig = true;
  }
  phNxpNciClock.isClockSet = false;

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_china_tianjin_rf_setting
 *
 * Description      This function is called to check RF Setting
 *
 * Returns          Status.
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_china_tianjin_rf_setting(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  int isfound = 0;
  int rf_enable = false;
  int rf_val = 0;
  int send_flag;
  uint8_t retry_cnt = 0;
  int enable_bit = 0;
  static uint8_t get_rf_cmd[] = {0x20, 0x03, 0x03, 0x01, 0xA0, 0x85};

retry_send_ext:
  if (retry_cnt > 3) {
    return NFCSTATUS_FAILED;
  }
  send_flag = true;
  phNxpNciRfSet.isGetRfSetting = true;
  status = phNxpNciHal_send_ext_cmd(sizeof(get_rf_cmd), get_rf_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("unable to get the RF setting");
    phNxpNciRfSet.isGetRfSetting = false;
    retry_cnt++;
    goto retry_send_ext;
  }
  phNxpNciRfSet.isGetRfSetting = false;
  if (phNxpNciRfSet.p_rx_data[3] != 0x00) {
    NXPLOG_NCIHAL_E("GET_CONFIG_RSP is FAILED for CHINA TIANJIN");
    return status;
  }
  rf_val = phNxpNciRfSet.p_rx_data[10];
  isfound = (GetNxpNumValue(NAME_NXP_CHINA_TIANJIN_RF_ENABLED,
                            (void*)&rf_enable, sizeof(rf_enable)));
  if (isfound > 0) {
    enable_bit = rf_val & 0x40;
    if ((enable_bit != 0x40) && (rf_enable == 1)) {
      phNxpNciRfSet.p_rx_data[10] |= 0x40;  // Enable if it is disabled
    } else if ((enable_bit == 0x40) && (rf_enable == 0)) {
      phNxpNciRfSet.p_rx_data[10] &= 0xBF;  // Disable if it is Enabled
    } else {
      send_flag = false;  // No need to change in RF setting
    }

    if (send_flag == true) {
      static uint8_t set_rf_cmd[] = {0x20, 0x02, 0x08, 0x01, 0xA0, 0x85,
                                     0x04, 0x50, 0x08, 0x68, 0x00};
      memcpy(&set_rf_cmd[4], &phNxpNciRfSet.p_rx_data[5], 7);
      status = phNxpNciHal_send_ext_cmd(sizeof(set_rf_cmd), set_rf_cmd);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("unable to set the RF setting");
        retry_cnt++;
        goto retry_send_ext;
      }
    }
  }

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_gpio_restore
 *
 * Description      This function restores the gpio values into eeprom
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpNciHal_gpio_restore(phNxpNciHal_GpioInfoState state) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t get_gpio_values_cmd[] = {0x20, 0x03, 0x03, 0x01, 0xA0, 0x00};
  uint8_t set_gpio_values_cmd[] = {0x20, 0x02, 0x00, 0x01, 0xA0, 0x00, 0x20,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  if(state == GPIO_STORE) {
    nxpncihal_ctrl.phNxpNciGpioInfo.state = GPIO_STORE;
    get_gpio_values_cmd[5] = 0x08;
    status = phNxpNciHal_send_ext_cmd(sizeof(get_gpio_values_cmd), get_gpio_values_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Failed to get GPIO values!!!\n");
      return;
    }

    nxpncihal_ctrl.phNxpNciGpioInfo.state = GPIO_STORE_DONE;
    set_gpio_values_cmd[2] = 0x24;
    set_gpio_values_cmd[5] = 0x14;
    set_gpio_values_cmd[7] = nxpncihal_ctrl.phNxpNciGpioInfo.values[0];
    set_gpio_values_cmd[8] = nxpncihal_ctrl.phNxpNciGpioInfo.values[1];
    status = phNxpNciHal_send_ext_cmd(sizeof(set_gpio_values_cmd), set_gpio_values_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Failed to set GPIO values!!!\n");
      return;
    }
  } else if(state == GPIO_RESTORE) {
    nxpncihal_ctrl.phNxpNciGpioInfo.state = GPIO_RESTORE;
    get_gpio_values_cmd[5] = 0x14;
    status = phNxpNciHal_send_ext_cmd(sizeof(get_gpio_values_cmd), get_gpio_values_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Failed to get GPIO values!!!\n");
      return;
    }

    nxpncihal_ctrl.phNxpNciGpioInfo.state = GPIO_RESTORE_DONE;
    set_gpio_values_cmd[2] = 0x06;
    set_gpio_values_cmd[5] = 0x08; //update TAG
    set_gpio_values_cmd[6] = 0x02; //update length
    set_gpio_values_cmd[7] = nxpncihal_ctrl.phNxpNciGpioInfo.values[0];
    set_gpio_values_cmd[8] = nxpncihal_ctrl.phNxpNciGpioInfo.values[1];
    status = phNxpNciHal_send_ext_cmd(9, set_gpio_values_cmd);
    if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("Failed to set GPIO values!!!\n");
      return;
    }
  } else {
      NXPLOG_NCIHAL_E("GPIO Restore Invalid Option!!!\n");
  }
}

/******************************************************************************
 * Function         phNxpNciHal_nfcc_core_reset_init
 *
 * Description      Helper function to do nfcc core reset & core init
 *
 * Returns          Status
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_nfcc_core_reset_init() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t retry_cnt = 0;
  uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x01};

retry_core_reset:
  status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
  if ((status != NFCSTATUS_SUCCESS) && (retry_cnt < 3)) {
    NXPLOG_NCIHAL_D("Retry: NCI_CORE_RESET");
    retry_cnt++;
    goto retry_core_reset;
  } else if (status != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_E("NCI_CORE_RESET failed!!!\n");
      return status;
  }

  retry_cnt = 0;
  uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
retry_core_init:
  if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
  } else {
    status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
  }

  if ((status != NFCSTATUS_SUCCESS) && (retry_cnt < 3)) {
    NXPLOG_NCIHAL_D("Retry: NCI_CORE_INIT\n");
    retry_cnt++;
    goto retry_core_init;
  } else if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("NCI_CORE_INIT failed!!!\n");
    return status;
  }

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_getChipInfoInFwDnldMode
 *
 * Description      Helper function to get the chip info in download mode
 *
 * Returns          Status
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_getChipInfoInFwDnldMode(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t retry_cnt = 0;
  uint8_t get_chip_info_cmd[] = {0x00, 0x04, 0xF1, 0x00,
                                 0x00, 0x00, 0x6E, 0xEF};
  NXPLOG_NCIHAL_D("%s:enter", __func__);
retry:
  status =
      phNxpNciHal_send_ext_cmd(sizeof(get_chip_info_cmd), get_chip_info_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    if (retry_cnt < 3) {
      NXPLOG_NCIHAL_D("Retry: get chip info");
      retry_cnt++;
      goto retry;
    } else {
      NXPLOG_NCIHAL_E("Failed: get chip info");
    }
  } else {
    phNxpNciHal_configFeatureList(nxpncihal_ctrl.p_rx_data,
                                  nxpncihal_ctrl.rx_data_len);
  }
  NXPLOG_NCIHAL_D("%s:exit  status: 0x%02x", __func__, status);
  return status;
}

int check_config_parameter() {
  uint8_t param_clock_src = CLK_SRC_PLL;
  if (nxpprofile_ctrl.bClkSrcVal == CLK_SRC_PLL) {
    if((nfcFL.chipType != pn553)&&(nfcFL.chipType != pn557)) {
      param_clock_src = param_clock_src << 3;
    }
    if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_13MHZ) {
      param_clock_src |= 0x00;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_19_2MHZ) {
      param_clock_src |= 0x01;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_24MHZ) {
      param_clock_src |= 0x02;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_26MHZ) {
      param_clock_src |= 0x03;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_38_4MHZ) {
      param_clock_src |= 0x04;
    } else if (nxpprofile_ctrl.bClkFreqVal == CLK_FREQ_52MHZ) {
      param_clock_src |= 0x05;
    } else {
      NXPLOG_NCIHAL_E("Wrong clock freq, send default PLL@19.2MHz");
      param_clock_src = 0x11;
    }
  } else if (nxpprofile_ctrl.bClkSrcVal == CLK_SRC_XTAL) {
    param_clock_src = 0x08;

  } else {
    NXPLOG_NCIHAL_E("Wrong clock source. Dont apply any modification")
  }
  return param_clock_src;
}
/******************************************************************************
 * Function         phNxpNciHal_enable_i2c_fragmentation
 *
 * Description      This function is called to process the response status
 *                  and print the status byte.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_enable_i2c_fragmentation() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  static uint8_t fragmentation_enable_config_cmd[] = {0x20, 0x02, 0x05, 0x01,
                                                      0xA0, 0x05, 0x01, 0x10};
  long i2c_status = 0x00;
  long config_i2c_vlaue = 0xff;
  /*NCI_RESET_CMD*/
  static uint8_t cmd_reset_nci[] = {0x20, 0x00, 0x01, 0x00};
  /*NCI_INIT_CMD*/
  static uint8_t cmd_init_nci[] = {0x20, 0x01, 0x00};
  static uint8_t cmd_init_nci2_0[] = {0x20, 0x01, 0x02, 0x00, 0x00};
  static uint8_t get_i2c_fragmentation_cmd[] = {0x20, 0x03, 0x03,
                                                0x01, 0xA0, 0x05};
  if (GetNxpNumValue(NAME_NXP_I2C_FRAGMENTATION_ENABLED, (void*)&i2c_status,
                 sizeof(i2c_status)) == true) {
    NXPLOG_FWDNLD_D("I2C status : %ld",i2c_status);
  } else {
    NXPLOG_FWDNLD_E("I2C status read not succeeded. Default value : %ld",i2c_status);
  }
  status = phNxpNciHal_send_ext_cmd(sizeof(get_i2c_fragmentation_cmd),
                                    get_i2c_fragmentation_cmd);
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("unable to retrieve  get_i2c_fragmentation_cmd");
  } else {
    if (nxpncihal_ctrl.p_rx_data[8] == 0x10) {
      config_i2c_vlaue = 0x01;
      phNxpNciHal_notify_i2c_fragmentation();
      phTmlNfc_set_fragmentation_enabled(I2C_FRAGMENTATION_ENABLED);
    } else if (nxpncihal_ctrl.p_rx_data[8] == 0x00) {
      config_i2c_vlaue = 0x00;
    }
    // if the value already matches, nothing to be done
    if (config_i2c_vlaue != i2c_status) {
      if (i2c_status == 0x01) {
        /* NXP I2C fragmenation enabled*/
        status =
            phNxpNciHal_send_ext_cmd(sizeof(fragmentation_enable_config_cmd),
                                     fragmentation_enable_config_cmd);
        if (status != NFCSTATUS_SUCCESS) {
          NXPLOG_NCIHAL_E("NXP fragmentation enable failed");
        }
      } else if (i2c_status == 0x00 || config_i2c_vlaue == 0xff) {
        fragmentation_enable_config_cmd[7] = 0x00;
        /* NXP I2C fragmentation disabled*/
        status =
            phNxpNciHal_send_ext_cmd(sizeof(fragmentation_enable_config_cmd),
                                     fragmentation_enable_config_cmd);
        if (status != NFCSTATUS_SUCCESS) {
          NXPLOG_NCIHAL_E("NXP fragmentation disable failed");
        }
      }
      status = phNxpNciHal_send_ext_cmd(sizeof(cmd_reset_nci), cmd_reset_nci);
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("NCI_CORE_RESET: Failed");
      }
      if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
        status =
            phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci2_0), cmd_init_nci2_0);
      } else {
        status = phNxpNciHal_send_ext_cmd(sizeof(cmd_init_nci), cmd_init_nci);
      }
      if (status != NFCSTATUS_SUCCESS) {
        NXPLOG_NCIHAL_E("NCI_CORE_INIT : Failed");
      } else if (i2c_status == 0x01) {
        phNxpNciHal_notify_i2c_fragmentation();
        phTmlNfc_set_fragmentation_enabled(I2C_FRAGMENTATION_ENABLED);
      }
    }
  }
}
/******************************************************************************
 * Function         phNxpNciHal_do_se_session_reset
 *
 * Description      This function is called to set the session id to default
 *                  value.
 *
 * Returns          NFCSTATUS.
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_do_se_session_reset(void) {
  static uint8_t reset_se_session_identity_set[] = {
      0x20, 0x02, 0x17, 0x02, 0xA0, 0xEA, 0x08, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA0, 0xEB, 0x08,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  NFCSTATUS status = phNxpNciHal_send_ext_cmd(sizeof(reset_se_session_identity_set),
                                  reset_se_session_identity_set);
  NXPLOG_NCIHAL_D("%s status = %x ",__func__, status);
  return status;
}
/******************************************************************************
 * Function         phNxpNciHal_do_factory_reset
 *
 * Description      This function is called during factory reset to clear/reset
 *                  nfc sub-system persistant data.
 *
 * Returns          void.
 *
 ******************************************************************************/
void phNxpNciHal_do_factory_reset(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  if (nxpncihal_ctrl.halStatus == HAL_STATUS_CLOSE) {
    status = phNxpNciHal_MinOpen();
    if (status != NFCSTATUS_SUCCESS ) {
      NXPLOG_NCIHAL_E("%s: NXP Nfc Open failed", __func__);
      return;
    }
  }
  status = phNxpNciHal_do_se_session_reset();
  if (status != NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_E("%s failed. status = %x ",__func__, status);
  }
}
/******************************************************************************
 * Function         phNxpNciHal_hci_network_reset
 *
 * Description      This function resets the session id's of all the se's
 *                  in the HCI network and notify to HCI_NETWORK_RESET event to
 *                  NFC HAL Client.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_hci_network_reset(void) {
  static phLibNfc_Message_t msg;
  msg.pMsgData = NULL;
  msg.Size = 0;

  NFCSTATUS status = phNxpNciHal_do_se_session_reset();

  if (status != NFCSTATUS_SUCCESS) {
    msg.eMsgType = NCI_HAL_ERROR_MSG;
  } else {
    msg.eMsgType = NCI_HAL_HCI_NETWORK_RESET_MSG;
  }
  phTmlNfc_DeferredCall(gpphTmlNfc_Context->dwCallbackThreadId, &msg);
}

/*******************************************************************************
**
** Function         phNxpNciHal_configFeatureList
**
** Description      Configures the featureList based on chip type
**                  HW Version information number will provide chipType.
**                  HW Version can be obtained from CORE_INIT_RESPONSE(NCI 1.0)
**                  or CORE_RST_NTF(NCI 2.0) or PROPREITARY RSP (FW download
*                   mode)
**
** Parameters       CORE_INIT_RESPONSE/CORE_RST_NTF/PROPREITARY RSP, len
**
** Returns          none
*******************************************************************************/
void phNxpNciHal_configFeatureList(uint8_t* msg, uint16_t msg_len) {
  tNFC_chipType chipType = pConfigFL->getChipType(msg, msg_len);
  CONFIGURE_FEATURELIST(chipType);
  NXPLOG_NCIHAL_D("%s chipType = %d", __func__, chipType);
}

/******************************************************************************
 * Function         phNxpNciHal_print_res_status
 *
 * Description      This function is called to process the response status
 *                  and print the status byte.
 *
 * Returns          void.
 *
 ******************************************************************************/
static void phNxpNciHal_print_res_status(uint8_t* p_rx_data, uint16_t* p_len) {
  static uint8_t response_buf[][30] = {"STATUS_OK",
                                       "STATUS_REJECTED",
                                       "STATUS_RF_FRAME_CORRUPTED",
                                       "STATUS_FAILED",
                                       "STATUS_NOT_INITIALIZED",
                                       "STATUS_SYNTAX_ERROR",
                                       "STATUS_SEMANTIC_ERROR",
                                       "RFU",
                                       "RFU",
                                       "STATUS_INVALID_PARAM",
                                       "STATUS_MESSAGE_SIZE_EXCEEDED",
                                       "STATUS_UNDEFINED"};
  int status_byte;
  if (p_rx_data[0] == 0x40 && (p_rx_data[1] == 0x02 || p_rx_data[1] == 0x03)) {
    if (p_rx_data[2] && p_rx_data[3] <= 10) {
      status_byte = p_rx_data[CORE_RES_STATUS_BYTE];
      NXPLOG_NCIHAL_D("%s: response status =%s", __func__,
                      response_buf[status_byte]);
    } else {
      NXPLOG_NCIHAL_D("%s: response status =%s", __func__, response_buf[11]);
    }
    if (phNxpNciClock.isClockSet) {
      int i;
      for (i = 0; i < *p_len; i++) {
        phNxpNciClock.p_rx_data[i] = p_rx_data[i];
      }
    }

    else if (phNxpNciRfSet.isGetRfSetting) {
      int i;
      for (i = 0; i < *p_len; i++) {
        phNxpNciRfSet.p_rx_data[i] = p_rx_data[i];
        // NXPLOG_NCIHAL_D("%s: response status =0x%x",__func__,p_rx_data[i]);
      }
    } else if (phNxpNciMwEepromArea.isGetEepromArea) {
      int i;
      for (i = 8; i < *p_len; i++) {
        phNxpNciMwEepromArea.p_rx_data[i - 8] = p_rx_data[i];
      }
    } else if (nxpncihal_ctrl.phNxpNciGpioInfo.state == GPIO_STORE) {
        NXPLOG_NCIHAL_D("%s: Storing GPIO Values...", __func__);
        nxpncihal_ctrl.phNxpNciGpioInfo.values[0] = p_rx_data[9];
        nxpncihal_ctrl.phNxpNciGpioInfo.values[1] = p_rx_data[8];
    } else if (nxpncihal_ctrl.phNxpNciGpioInfo.state == GPIO_RESTORE) {
        NXPLOG_NCIHAL_D("%s: Restoring GPIO Values...", __func__);
        nxpncihal_ctrl.phNxpNciGpioInfo.values[0] = p_rx_data[9];
        nxpncihal_ctrl.phNxpNciGpioInfo.values[1] = p_rx_data[8];
    }
}

  if (p_rx_data[2] && (config_access == true)) {
    if (p_rx_data[3] != NFCSTATUS_SUCCESS) {
      NXPLOG_NCIHAL_W("Invalid Data from config file.");
      config_success = false;
    }
  }
}

/******************************************************************************
 * Function         phNxpNciHal_initialize_mifare_flag
 *
 * Description      This function gets the value for Mfc flags.
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpNciHal_initialize_mifare_flag() {
  unsigned long num = 0;
  bEnableMfcReader = false;
  bDisableLegacyMfcExtns = true;
  //1: Enable Mifare Classic protocol in RF Discovery.
  //0: Remove Mifare Classic protocol in RF Discovery.
  if(GetNxpNumValue(NAME_MIFARE_READER_ENABLE, &num, sizeof(num))) {
    bEnableMfcReader = (num == 0) ? false : true;
  }
  num = 0;
  //1: Use legacy JNI MFC extns.
  //0: Disable legacy JNI MFC extns, use hal MFC Extns instead.
  if(GetNxpNumValue(NAME_LEGACY_MIFARE_READER, &num, sizeof(num))) {
    bDisableLegacyMfcExtns = (num == 0) ? true : false;
  }
}
/*******************************************************************************
**
** Function         phNxpNciHal_getChipType
**
** Description      Gets the nfcChipType which is configured during bootup
**
** Parameters       none
**
** Returns          nfcChipType
*******************************************************************************/
tNFC_chipType phNxpNciHal_getChipType() {
    return nxpncihal_ctrl.nfcChipType;
}
