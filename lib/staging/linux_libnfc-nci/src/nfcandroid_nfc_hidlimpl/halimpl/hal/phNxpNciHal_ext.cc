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
#include <log/log.h>
#include <phDal4Nfc_messageQueueLib.h>
#include <phNxpConfig.h>
#include <phNxpLog.h>
#include <phNxpNciHal.h>
#include <phNxpNciHal_Adaptation.h>
#include "hal_nxpnfc.h"
#include "hal_nxpese.h"
#include <phNxpNciHal_NfcDepSWPrio.h>
#include <phNxpNciHal_ext.h>
#include <phTmlNfc.h>
#include "bt_trace.h"
/* Timeout value to wait for response from PN548AD */
#define HAL_EXTNS_WRITE_RSP_TIMEOUT (1000)

#undef P2P_PRIO_LOGIC_HAL_IMP

/******************* Global variables *****************************************/
extern phNxpNciHal_Control_t nxpncihal_ctrl;
extern phNxpNciProfile_Control_t nxpprofile_ctrl;
extern uint8_t anti_tearing_recovery_success;
extern uint32_t cleanup_timer;
extern bool nfc_debug_enabled;
uint8_t icode_detected = 0x00;
uint8_t icode_send_eof = 0x00;
uint8_t nfcdep_detected = 0x00;
static uint8_t ee_disc_done = 0x00;
uint8_t EnableP2P_PrioLogic = false;
extern bool bEnableMfcExtns;
extern bool bEnableMfcReader;
extern bool bDisableLegacyMfcExtns;
static uint32_t RfDiscID = 1;
static uint32_t RfProtocolType = 4;
/* NFCEE Set mode */
static uint8_t setEEModeDone = 0x00;
/* External global variable to get FW version from NCI response*/
extern uint32_t wFwVerRsp;
/* External global variable to get FW version from FW file*/
extern uint16_t wFwVer;

uint16_t fw_maj_ver;
uint16_t rom_version;

extern uint32_t timeoutTimerId;

/************** HAL extension functions ***************************************/
static void hal_extns_write_rsp_timeout_cb(uint32_t TimerId, void* pContext);

/*Proprietary cmd sent to HAL to send reader mode flag
 * Last byte of 4 byte proprietary cmd data contains ReaderMode flag
 * If this flag is enabled, NFC-DEP protocol is modified to T3T protocol
 * if FrameRF interface is selected. This needs to be done as the FW
 * always sends Ntf for FrameRF with NFC-DEP even though FrameRF with T3T is
 * previously selected with DISCOVER_SELECT_CMD
 */
#define PROPRIETARY_CMD_FELICA_READER_MODE 0xFE
static uint8_t gFelicaReaderMode;

static NFCSTATUS phNxpNciHal_ext_process_nfc_init_rsp(uint8_t* p_ntf,
                                                      uint16_t* p_len);
/*******************************************************************************
**
** Function         phNxpNciHal_ext_init
**
** Description      initialize extension function
**
*******************************************************************************/
void phNxpNciHal_ext_init(void) {
  icode_detected = 0x00;
  icode_send_eof = 0x00;
  setEEModeDone = 0x00;
  EnableP2P_PrioLogic = false;
}

/*******************************************************************************
**
** Function         phNxpNciHal_process_ext_rsp
**
** Description      Process extension function response
**
** Returns          NFCSTATUS_SUCCESS if success
**
*******************************************************************************/
NFCSTATUS phNxpNciHal_process_ext_rsp(uint8_t* p_ntf, uint16_t* p_len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && *p_len < 14) {
    if ((nfcFL.chipType == pn548C2) && (nfcdep_detected)) {
      nfcdep_detected = 0x00;
    }

    if(*p_len <= 6) {
      android_errorWriteLog(0x534e4554, "118152591");
    }
    NXPLOG_NCIHAL_E("RF_INTF_ACTIVATED_NTF length error!");
    status = NFCSTATUS_FAILED;
    return status;
  }

  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && p_ntf[4] == 0x03 &&
      p_ntf[5] == 0x05 && nxpprofile_ctrl.profile_type == EMV_CO_PROFILE) {
    p_ntf[4] = 0xFF;
    p_ntf[5] = 0xFF;
    p_ntf[6] = 0xFF;
    NXPLOG_NCIHAL_D("Nfc-Dep Detect in EmvCo profile - Restart polling");
  }

  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && p_ntf[4] == 0x01 &&
      p_ntf[5] == 0x05 && p_ntf[6] == 0x02 && gFelicaReaderMode) {
    /*If FelicaReaderMode is enabled,Change Protocol to T3T from NFC-DEP
         * when FrameRF interface is selected*/
    p_ntf[5] = 0x03;
    NXPLOG_NCIHAL_D("FelicaReaderMode:Activity 1.1");
  }

#ifdef P2P_PRIO_LOGIC_HAL_IMP
  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && p_ntf[4] == 0x02 &&
      p_ntf[5] == 0x04 && nxpprofile_ctrl.profile_type == NFC_FORUM_PROFILE) {
    EnableP2P_PrioLogic = true;
  }

  NXPLOG_NCIHAL_D("Is EnableP2P_PrioLogic: 0x0%X", EnableP2P_PrioLogic);
  if (phNxpDta_IsEnable() == false) {
    if ((icode_detected != 1) && (EnableP2P_PrioLogic == true)) {
      if (phNxpNciHal_NfcDep_comapre_ntf(p_ntf, *p_len) == NFCSTATUS_FAILED) {
        status = phNxpNciHal_NfcDep_rsp_ext(p_ntf, p_len);
        if (status != NFCSTATUS_INVALID_PARAMETER) {
          return status;
        }
      }
    }
  }
#endif

  status = NFCSTATUS_SUCCESS;

  if (bDisableLegacyMfcExtns && bEnableMfcExtns && p_ntf[0] == 0) {
    uint16_t extlen;
    extlen = *p_len - NCI_HEADER_SIZE;
    NxpMfcReaderInstance.AnalyzeMfcResp(&p_ntf[3], &extlen);
    p_ntf[2] = extlen;
    *p_len = extlen + NCI_HEADER_SIZE;
  }

  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05) {
    bEnableMfcExtns = false;
    if (bDisableLegacyMfcExtns && p_ntf[4] == 0x80 && p_ntf[5] == 0x80) {
      bEnableMfcExtns = true;
      NXPLOG_NCIHAL_D("NxpNci: RF Interface = Mifare Enable MifareExtns");
    }
    switch (p_ntf[4]) {
      case 0x00:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = NFCEE Direct RF");
        break;
      case 0x01:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = Frame RF");
        break;
      case 0x02:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = ISO-DEP");
        break;
      case 0x03:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = NFC-DEP");
        if (nfcFL.chipType == pn548C2) {
          nfcdep_detected = 0x01;
        }
        break;
      case 0x80:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = MIFARE");
        break;
      default:
        NXPLOG_NCIHAL_D("NxpNci: RF Interface = Unknown");
        break;
    }

    switch (p_ntf[5]) {
      case 0x01:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = T1T");
        phNxpDta_T1TEnable();
        break;
      case 0x02:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = T2T");
        break;
      case 0x03:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = T3T");
        break;
      case 0x04:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = ISO-DEP");
        break;
      case 0x05:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = NFC-DEP");
        break;
      case 0x06:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = 15693");
        break;
      case 0x80:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = MIFARE");
        break;
      case 0x81:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = Kovio");
        break;
      default:
        NXPLOG_NCIHAL_D("NxpNci: Protocol = Unknown");
        break;
    }

    switch (p_ntf[6]) {
      case 0x00:
        NXPLOG_NCIHAL_D("NxpNci: Mode = A Passive Poll");
        break;
      case 0x01:
        NXPLOG_NCIHAL_D("NxpNci: Mode = B Passive Poll");
        break;
      case 0x02:
        NXPLOG_NCIHAL_D("NxpNci: Mode = F Passive Poll");
        break;
      case 0x03:
        NXPLOG_NCIHAL_D("NxpNci: Mode = A Active Poll");
        break;
      case 0x05:
        NXPLOG_NCIHAL_D("NxpNci: Mode = F Active Poll");
        break;
      case 0x06:
        NXPLOG_NCIHAL_D("NxpNci: Mode = 15693 Passive Poll");
        break;
      case 0x70:
        NXPLOG_NCIHAL_D("NxpNci: Mode = Kovio");
        break;
      case 0x80:
        NXPLOG_NCIHAL_D("NxpNci: Mode = A Passive Listen");
        break;
      case 0x81:
        NXPLOG_NCIHAL_D("NxpNci: Mode = B Passive Listen");
        break;
      case 0x82:
        NXPLOG_NCIHAL_D("NxpNci: Mode = F Passive Listen");
        break;
      case 0x83:
        NXPLOG_NCIHAL_D("NxpNci: Mode = A Active Listen");
        break;
      case 0x85:
        NXPLOG_NCIHAL_D("NxpNci: Mode = F Active Listen");
        break;
      case 0x86:
        NXPLOG_NCIHAL_D("NxpNci: Mode = 15693 Passive Listen");
        break;
      default:
        NXPLOG_NCIHAL_D("NxpNci: Mode = Unknown");
        break;
    }
  }
  phNxpNciHal_ext_process_nfc_init_rsp(p_ntf, p_len);

  if (p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && p_ntf[2] == 0x15 &&
      p_ntf[4] == 0x01 && p_ntf[5] == 0x06 && p_ntf[6] == 0x06) {
    NXPLOG_NCIHAL_D("> Going through workaround - notification of ISO 15693");
    icode_detected = 0x01;
    p_ntf[21] = 0x01;
    p_ntf[22] = 0x01;
  } else if (icode_detected == 1 && icode_send_eof == 2) {
    icode_send_eof = 3;
  } else if (p_ntf[0] == 0x00 && p_ntf[1] == 0x00 && icode_detected == 1) {
    if (icode_send_eof == 3) {
      icode_send_eof = 0;
    }
    if (nxpncihal_ctrl.nci_info.nci_version != NCI_VERSION_2_0) {
      if (p_ntf[p_ntf[2] + 2] == 0x00) {
        NXPLOG_NCIHAL_D("> Going through workaround - data of ISO 15693");
        p_ntf[2]--;
        (*p_len)--;
      } else {
        p_ntf[p_ntf[2] + 2] |= 0x01;
      }
    }
  } else if (p_ntf[2] == 0x02 && p_ntf[1] == 0x00 && icode_detected == 1) {
    NXPLOG_NCIHAL_D("> ICODE EOF response do not send to upper layer");
  } else if (p_ntf[0] == 0x61 && p_ntf[1] == 0x06 && icode_detected == 1) {
    NXPLOG_NCIHAL_D("> Polling Loop Re-Started");
    icode_detected = 0;
    icode_send_eof = 0;
  } else if (*p_len == 4 && p_ntf[0] == 0x40 && p_ntf[1] == 0x02 &&
             p_ntf[2] == 0x01 && p_ntf[3] == 0x06) {
    NXPLOG_NCIHAL_D("> Deinit workaround for LLCP set_config 0x%x 0x%x 0x%x",
                    p_ntf[21], p_ntf[22], p_ntf[23]);
    p_ntf[0] = 0x40;
    p_ntf[1] = 0x02;
    p_ntf[2] = 0x02;
    p_ntf[3] = 0x00;
    p_ntf[4] = 0x00;
    *p_len = 5;
  }
  // 4200 02 00 01
  else if (p_ntf[0] == 0x42 && p_ntf[1] == 0x00 && ee_disc_done == 0x01) {
    NXPLOG_NCIHAL_D("Going through workaround - NFCEE_DISCOVER_RSP");
    if (p_ntf[4] == 0x01) {
      p_ntf[4] = 0x00;

      ee_disc_done = 0x00;
    }
    NXPLOG_NCIHAL_D("Going through workaround - NFCEE_DISCOVER_RSP - END");

  } else if (p_ntf[0] == 0x61 && p_ntf[1] == 0x03 /*&& cleanup_timer!=0*/) {
    if (cleanup_timer != 0) {
      /* if RF Notification Type of RF_DISCOVER_NTF is Last Notification */
      if (0 == (*(p_ntf + 2 + (*(p_ntf + 2))))) {
        phNxpNciHal_select_RF_Discovery(RfDiscID, RfProtocolType);
        status = NFCSTATUS_FAILED;
        return status;
      } else {
        RfDiscID = p_ntf[3];
        RfProtocolType = p_ntf[4];
      }
      status = NFCSTATUS_FAILED;
      return status;
    }
  } else if (p_ntf[0] == 0x41 && p_ntf[1] == 0x04 && cleanup_timer != 0) {
    status = NFCSTATUS_FAILED;
    return status;
  }
  else if (*p_len == 4 && p_ntf[0] == 0x4F && p_ntf[1] == 0x11 &&
           p_ntf[2] == 0x01) {
    if (p_ntf[3] == 0x00) {
      NXPLOG_NCIHAL_D(
          ">  Workaround for ISO-DEP Presence Check, ignore response and wait "
          "for notification");
      p_ntf[0] = 0x60;
      p_ntf[1] = 0x06;
      p_ntf[2] = 0x03;
      p_ntf[3] = 0x01;
      p_ntf[4] = 0x00;
      p_ntf[5] = 0x01;
      *p_len = 6;
    } else {
      NXPLOG_NCIHAL_D(
          ">  Workaround for ISO-DEP Presence Check, presence check return "
          "failed");
      p_ntf[0] = 0x60;
      p_ntf[1] = 0x08;
      p_ntf[2] = 0x02;
      p_ntf[3] = 0xB2;
      p_ntf[4] = 0x00;
      *p_len = 5;
    }
  } else if (*p_len == 4 && p_ntf[0] == 0x6F && p_ntf[1] == 0x11 &&
             p_ntf[2] == 0x01) {
    if (p_ntf[3] == 0x01) {
      NXPLOG_NCIHAL_D(
          ">  Workaround for ISO-DEP Presence Check - Card still in field");
      p_ntf[0] = 0x00;
      p_ntf[1] = 0x00;
      p_ntf[2] = 0x01;
      p_ntf[3] = 0x7E;
    } else {
      NXPLOG_NCIHAL_D(
          ">  Workaround for ISO-DEP Presence Check - Card not in field");
      p_ntf[0] = 0x60;
      p_ntf[1] = 0x08;
      p_ntf[2] = 0x02;
      p_ntf[3] = 0xB2;
      p_ntf[4] = 0x00;
      *p_len = 5;
    }
  }
  else if(p_ntf[0] == 0x60 && p_ntf[1] == 0x07 && p_ntf[3] == 0xE6)
  {
      NXPLOG_NCIHAL_E("CORE_GENERIC_ERROR_NTF received!");
      /* register recovery success to force applying RF settings */
      anti_tearing_recovery_success = 1;
  }

  if (*p_len == 4 && p_ntf[0] == 0x61 && p_ntf[1] == 0x07 ) {
    unsigned long rf_update_enable = 0;
    if(GetNxpNumValue(NAME_RF_STATUS_UPDATE_ENABLE, &rf_update_enable, sizeof(unsigned long))) {
      NXPLOG_NCIHAL_D(
        "RF_STATUS_UPDATE_ENABLE : %lu",rf_update_enable);
    }
    if(rf_update_enable == 0x01) {
      nfc_nci_IoctlInOutData_t inpOutData;
      uint8_t rf_state_update[] = {0x00};
      memset(&inpOutData, 0x00, sizeof(nfc_nci_IoctlInOutData_t));
      inpOutData.inp.data.nciCmd.cmd_len = sizeof(rf_state_update);
      rf_state_update[0]=p_ntf[3];
      memcpy(inpOutData.inp.data.nciCmd.p_cmd, rf_state_update,sizeof(rf_state_update));
      inpOutData.inp.data_source = 2;
      phNxpNciHal_ioctl(HAL_NFC_IOCTL_RF_STATUS_UPDATE, &inpOutData);
    }
  }
  /*
  else if(p_ntf[0] == 0x61 && p_ntf[1] == 0x05 && p_ntf[4] == 0x01 && p_ntf[5]
  == 0x00 && p_ntf[6] == 0x01)
  {
      NXPLOG_NCIHAL_D("Picopass type 3-B with undefined protocol is not
  supported, disabling");
      p_ntf[4] = 0xFF;
      p_ntf[5] = 0xFF;
      p_ntf[6] = 0xFF;
  }*/

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_ext_process_nfc_init_rsp
 *
 * Description      This function is used to process the HAL NFC core reset rsp
 *                  and ntf and core init rsp of NCI 1.0 or NCI2.0 and update
 *                  NCI version.
 *                  It also handles error response such as core_reset_ntf with
 *                  error status in both NCI2.0 and NCI1.0.
 *
 * Returns          Returns NFCSTATUS_SUCCESS if parsing response is successful
 *                  or returns failure.
 *
 *******************************************************************************/
static NFCSTATUS phNxpNciHal_ext_process_nfc_init_rsp(uint8_t* p_ntf,
                                                      uint16_t* p_len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  /* Parsing CORE_RESET_RSP and CORE_RESET_NTF to update NCI version.*/
  if (p_ntf == NULL || *p_len == 0x00) {
    return NFCSTATUS_FAILED;
  }
  if (p_ntf[0] == NCI_MT_RSP &&
      ((p_ntf[1] & NCI_OID_MASK) == NCI_MSG_CORE_RESET)) {
    if (p_ntf[2] == 0x01 && p_ntf[3] == 0x00) {
      NXPLOG_NCIHAL_D("CORE_RESET_RSP NCI2.0");
      if (nxpncihal_ctrl.hal_ext_enabled == TRUE) {
        nxpncihal_ctrl.nci_info.wait_for_ntf = TRUE;
      }
    } else if (p_ntf[2] == 0x03 && p_ntf[3] == 0x00) {
      NXPLOG_NCIHAL_D("CORE_RESET_RSP NCI1.0");
      nxpncihal_ctrl.nci_info.nci_version = p_ntf[4];
    }
  } else if (p_ntf[0] == NCI_MT_NTF &&
             ((p_ntf[1] & NCI_OID_MASK) == NCI_MSG_CORE_RESET)) {
    if (p_ntf[3] == CORE_RESET_TRIGGER_TYPE_CORE_RESET_CMD_RECEIVED ||
        p_ntf[3] == CORE_RESET_TRIGGER_TYPE_POWERED_ON) {
      NXPLOG_NCIHAL_D("CORE_RESET_NTF NCI2.0 reason CORE_RESET_CMD received !");
      nxpncihal_ctrl.nci_info.nci_version = p_ntf[5];
      NXPLOG_NCIHAL_D("nci_version : 0x%02x",nxpncihal_ctrl.nci_info.nci_version);
      if(!nxpncihal_ctrl.hal_open_status) {
        phNxpNciHal_configFeatureList(p_ntf,*p_len);
      }
      int len = p_ntf[2] + 2; /*include 2 byte header*/
      if(len != *p_len - 1) {
        NXPLOG_NCIHAL_E("phNxpNciHal_ext_process_nfc_init_rsp invalid NTF length");
        android_errorWriteLog(0x534e4554, "121263487");
        return NFCSTATUS_FAILED;
      }
      wFwVerRsp = (((uint32_t)p_ntf[len - 2]) << 16U) |
                  (((uint32_t)p_ntf[len - 1]) << 8U) | p_ntf[len];
      NXPLOG_NCIHAL_D("NxpNci> FW Version: %x.%x.%x", p_ntf[len - 2],
                      p_ntf[len - 1], p_ntf[len]);
      fw_maj_ver = p_ntf[len - 1];
      rom_version = p_ntf[len - 2];
    } else {
      uint32_t i;
      char print_buffer[*p_len * 3 + 1];

      memset(print_buffer, 0, sizeof(print_buffer));
      for (i = 0; i < *p_len; i++) {
        snprintf(&print_buffer[i * 2], 3, "%02X", p_ntf[i]);
      }
      NXPLOG_NCIHAL_D("CORE_RESET_NTF received !");
      NXPLOG_NCIR_E("len = %3d > %s", *p_len, print_buffer);
      if ((nfcFL.chipType == pn548C2) && nfcdep_detected &&
          !(p_ntf[2] == 0x06 && p_ntf[3] == 0xA0 && p_ntf[4] == 0x00 &&
            ((p_ntf[5] == 0xC9 && p_ntf[6] == 0x95 && p_ntf[7] == 0x00 &&
              p_ntf[8] == 0x00) ||
             (p_ntf[5] == 0x07 && p_ntf[6] == 0x39 && p_ntf[7] == 0xF2 &&
              p_ntf[8] == 0x00)))) {
        nfcdep_detected = 0x00;
      }
      phNxpNciHal_emergency_recovery();
      status = NFCSTATUS_FAILED;
    } /* Parsing CORE_INIT_RSP*/
  } else if (p_ntf[0] == NCI_MT_RSP &&
             ((p_ntf[1] & NCI_OID_MASK) == NCI_MSG_CORE_INIT)) {
    if (nxpncihal_ctrl.nci_info.nci_version == NCI_VERSION_2_0) {
      NXPLOG_NCIHAL_D("CORE_INIT_RSP NCI2.0 received !");
    } else {
      NXPLOG_NCIHAL_D("CORE_INIT_RSP NCI1.0 received !");
      if(!nxpncihal_ctrl.hal_open_status) {
        phNxpNciHal_configFeatureList(p_ntf,*p_len);
      }
      int len = p_ntf[2] + 2; /*include 2 byte header*/
      if(len != *p_len - 1) {
        NXPLOG_NCIHAL_E("phNxpNciHal_ext_process_nfc_init_rsp invalid NTF length");
        android_errorWriteLog(0x534e4554, "121263487");
        return NFCSTATUS_FAILED;
      }
      wFwVerRsp = (((uint32_t)p_ntf[len - 2]) << 16U) |
                  (((uint32_t)p_ntf[len - 1]) << 8U) | p_ntf[len];
      if (wFwVerRsp == 0) status = NFCSTATUS_FAILED;
      NXPLOG_NCIHAL_D("NxpNci> FW Version: %x.%x.%x", p_ntf[len - 2],
                      p_ntf[len - 1], p_ntf[len]);
      fw_maj_ver = p_ntf[len - 1];
      rom_version = p_ntf[len - 2];
    }
  }
  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_process_ext_cmd_rsp
 *
 * Description      This function process the extension command response. It
 *                  also checks the received response to expected response.
 *
 * Returns          returns NFCSTATUS_SUCCESS if response is as expected else
 *                  returns failure.
 *
 ******************************************************************************/
static NFCSTATUS phNxpNciHal_process_ext_cmd_rsp(uint16_t cmd_len,
                                                 uint8_t* p_cmd) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint16_t data_written = 0;

  /* Create the local semaphore */
  if (phNxpNciHal_init_cb_data(&nxpncihal_ctrl.ext_cb_data, NULL) !=
      NFCSTATUS_SUCCESS) {
    NXPLOG_NCIHAL_D("Create ext_cb_data failed");
    return NFCSTATUS_FAILED;
  }

  nxpncihal_ctrl.ext_cb_data.status = NFCSTATUS_SUCCESS;

  /* Send ext command */
  data_written = phNxpNciHal_write_unlocked(cmd_len, p_cmd);
  if (data_written != cmd_len) {
    NXPLOG_NCIHAL_D("phNxpNciHal_write failed for hal ext");
    goto clean_and_return;
  }

  /* Start timer */
  status = phOsalNfc_Timer_Start(timeoutTimerId, HAL_EXTNS_WRITE_RSP_TIMEOUT,
                                 &hal_extns_write_rsp_timeout_cb, NULL);
  if (NFCSTATUS_SUCCESS == status) {
    NXPLOG_NCIHAL_D("Response timer started");
  } else {
    NXPLOG_NCIHAL_E("Response timer not started!!!");
    status = NFCSTATUS_FAILED;
    goto clean_and_return;
  }

  /* Wait for rsp */
  NXPLOG_NCIHAL_D("Waiting after ext cmd sent");
  if (SEM_WAIT(nxpncihal_ctrl.ext_cb_data)) {
    NXPLOG_NCIHAL_E("p_hal_ext->ext_cb_data.sem semaphore error");
    goto clean_and_return;
  }

  /* Stop Timer */
  status = phOsalNfc_Timer_Stop(timeoutTimerId);
  if (NFCSTATUS_SUCCESS == status) {
    NXPLOG_NCIHAL_D("Response timer stopped");
  } else {
    NXPLOG_NCIHAL_E("Response timer stop ERROR!!!");
    status = NFCSTATUS_FAILED;
    goto clean_and_return;
  }
  /* No NTF expected for OMAPI command */
  if(p_cmd[0] == 0x2F && p_cmd[1] == 0x1 &&  p_cmd[2] == 0x01) {
    nxpncihal_ctrl.nci_info.wait_for_ntf = FALSE;
  }
  /* Start timer to wait for NTF*/
  if (nxpncihal_ctrl.nci_info.wait_for_ntf == TRUE) {
    status = phOsalNfc_Timer_Start(timeoutTimerId, HAL_EXTNS_WRITE_RSP_TIMEOUT,
                                   &hal_extns_write_rsp_timeout_cb, NULL);
    if (NFCSTATUS_SUCCESS == status) {
      NXPLOG_NCIHAL_D("Response timer started");
    } else {
      NXPLOG_NCIHAL_E("Response timer not started!!!");
      status = NFCSTATUS_FAILED;
      goto clean_and_return;
    }
    if (SEM_WAIT(nxpncihal_ctrl.ext_cb_data)) {
      NXPLOG_NCIHAL_E("p_hal_ext->ext_cb_data.sem semaphore error");
      /* Stop Timer */
      status = phOsalNfc_Timer_Stop(timeoutTimerId);
      goto clean_and_return;
    }
    status = phOsalNfc_Timer_Stop(timeoutTimerId);
    if (NFCSTATUS_SUCCESS == status) {
      NXPLOG_NCIHAL_D("Response timer stopped");
    } else {
      NXPLOG_NCIHAL_E("Response timer stop ERROR!!!");
      status = NFCSTATUS_FAILED;
      goto clean_and_return;
    }
  }

  if (nxpncihal_ctrl.ext_cb_data.status != NFCSTATUS_SUCCESS && p_cmd[0] != 0x2F && p_cmd[1] != 0x1 &&  p_cmd[2] == 0x01) {
    NXPLOG_NCIHAL_E(
        "Callback Status is failed!! Timer Expired!! Couldn't read it! 0x%x",
        nxpncihal_ctrl.ext_cb_data.status);
    status = NFCSTATUS_FAILED;
    goto clean_and_return;
  }

  NXPLOG_NCIHAL_D("Checking response");
  status = NFCSTATUS_SUCCESS;

clean_and_return:
  phNxpNciHal_cleanup_cb_data(&nxpncihal_ctrl.ext_cb_data);
  nxpncihal_ctrl.nci_info.wait_for_ntf = FALSE;
  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_write_ext
 *
 * Description      This function inform the status of phNxpNciHal_open
 *                  function to libnfc-nci.
 *
 * Returns          It return NFCSTATUS_SUCCESS then continue with send else
 *                  sends NFCSTATUS_FAILED direct response is prepared and
 *                  do not send anything to NFCC.
 *
 ******************************************************************************/

NFCSTATUS phNxpNciHal_write_ext(uint16_t* cmd_len, uint8_t* p_cmd_data,
                                uint16_t* rsp_len, uint8_t* p_rsp_data) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phNxpNciHal_NfcDep_cmd_ext(p_cmd_data, cmd_len);

  if (phNxpDta_IsEnable() == true) {
    status = phNxpNHal_DtaUpdate(cmd_len, p_cmd_data, rsp_len, p_rsp_data);
  }

  if (p_cmd_data[0] == PROPRIETARY_CMD_FELICA_READER_MODE &&
      p_cmd_data[1] == PROPRIETARY_CMD_FELICA_READER_MODE &&
      p_cmd_data[2] == PROPRIETARY_CMD_FELICA_READER_MODE) {
    NXPLOG_NCIHAL_D("Received proprietary command to set Felica Reader mode:%d",
                    p_cmd_data[3]);
    gFelicaReaderMode = p_cmd_data[3];
    /* frame the dummy response */
    *rsp_len = 4;
    p_rsp_data[0] = 0x00;
    p_rsp_data[1] = 0x00;
    p_rsp_data[2] = 0x00;
    p_rsp_data[3] = 0x00;
    status = NFCSTATUS_FAILED;
  } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
             p_cmd_data[2] == 0x05 && p_cmd_data[3] == 0x01 &&
             p_cmd_data[4] == 0xA0 && p_cmd_data[5] == 0x44 &&
             p_cmd_data[6] == 0x01 && p_cmd_data[7] == 0x01) {
    nxpprofile_ctrl.profile_type = EMV_CO_PROFILE;
    NXPLOG_NCIHAL_D("EMV_CO_PROFILE mode - Enabled");
    status = NFCSTATUS_SUCCESS;
  } else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 &&
             p_cmd_data[2] == 0x05 && p_cmd_data[3] == 0x01 &&
             p_cmd_data[4] == 0xA0 && p_cmd_data[5] == 0x44 &&
             p_cmd_data[6] == 0x01 && p_cmd_data[7] == 0x00) {
    NXPLOG_NCIHAL_D("NFC_FORUM_PROFILE mode - Enabled");
    nxpprofile_ctrl.profile_type = NFC_FORUM_PROFILE;
    status = NFCSTATUS_SUCCESS;
  }

  if (nxpprofile_ctrl.profile_type == EMV_CO_PROFILE) {
    if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x06 &&
        p_cmd_data[2] == 0x01 && p_cmd_data[3] == 0x03) {
#if 0
            //Needs clarification whether to keep it or not
            NXPLOG_NCIHAL_D ("EmvCo Poll mode - RF Deactivate discard");
            phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
            *rsp_len = 4;
            p_rsp_data[0] = 0x41;
            p_rsp_data[1] = 0x06;
            p_rsp_data[2] = 0x01;
            p_rsp_data[3] = 0x00;
            phNxpNciHal_print_packet("RECV", p_rsp_data, 4);
            status = NFCSTATUS_FAILED;
#endif
    } else if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x03) {
      NXPLOG_NCIHAL_D("EmvCo Poll mode - Discover map only for A and B");
      p_cmd_data[2] = 0x05;
      p_cmd_data[3] = 0x02;
      p_cmd_data[4] = 0x00;
      p_cmd_data[5] = 0x01;
      p_cmd_data[6] = 0x01;
      p_cmd_data[7] = 0x01;
      *cmd_len = 8;
    }
  }

  if (*cmd_len <= (NCI_MAX_DATA_LEN - 3) &&
      bEnableMfcReader && p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x00) {
    NXPLOG_NCIHAL_D("Going through extns - Adding Mifare in RF Discovery");
    p_cmd_data[2] += 3;
    p_cmd_data[3] += 1;
    p_cmd_data[*cmd_len] = 0x80;
    p_cmd_data[*cmd_len + 1] = 0x01;
    p_cmd_data[*cmd_len + 2] = 0x80;
    *cmd_len += 3;
    status = NFCSTATUS_SUCCESS;
    bEnableMfcExtns = false;
    NXPLOG_NCIHAL_D(
        "Going through extns - Adding Mifare in RF Discovery - END");
  } else if (p_cmd_data[3] == 0x81 && p_cmd_data[4] == 0x01 &&
             p_cmd_data[5] == 0x03) {
    if (nxpncihal_ctrl.nci_info.nci_version != NCI_VERSION_2_0) {
      NXPLOG_NCIHAL_D("> Going through workaround - set host list");

      *cmd_len = 8;

      p_cmd_data[2] = 0x05;
      p_cmd_data[6] = 0x02;
      p_cmd_data[7] = 0xC0;

      NXPLOG_NCIHAL_D("> Going through workaround - set host list - END");
      status = NFCSTATUS_SUCCESS;
    }
  } else if (icode_detected) {
    if ((p_cmd_data[3] & 0x40) == 0x40 &&
        (p_cmd_data[4] == 0x21 || p_cmd_data[4] == 0x22 ||
         p_cmd_data[4] == 0x24 || p_cmd_data[4] == 0x27 ||
         p_cmd_data[4] == 0x28 || p_cmd_data[4] == 0x29 ||
         p_cmd_data[4] == 0x2a)) {
      NXPLOG_NCIHAL_D("> Send EOF set");
      icode_send_eof = 1;
    }

    if (p_cmd_data[3] == 0x20 || p_cmd_data[3] == 0x24 ||
        p_cmd_data[3] == 0x60) {
      NXPLOG_NCIHAL_D("> NFC ISO_15693 Proprietary CMD ");
      p_cmd_data[3] += 0x02;
    }
  } else if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x03) {
    NXPLOG_NCIHAL_D("> Polling Loop Started");
    icode_detected = 0;
    icode_send_eof = 0;
    if (nfcFL.chipType == pn548C2) {
      // Cache discovery cmd for recovery
      phNxpNciHal_discovery_cmd_ext(p_cmd_data, *cmd_len);
    }
  }
  // 22000100
  else if (p_cmd_data[0] == 0x22 && p_cmd_data[1] == 0x00 &&
           p_cmd_data[2] == 0x01 && p_cmd_data[3] == 0x00) {
    // ee_disc_done = 0x01;//Reader Over SWP event getting
    *rsp_len = 0x05;
    p_rsp_data[0] = 0x42;
    p_rsp_data[1] = 0x00;
    p_rsp_data[2] = 0x02;
    p_rsp_data[3] = 0x00;
    p_rsp_data[4] = 0x00;
    phNxpNciHal_print_packet("RECV", p_rsp_data, 5);
    status = NFCSTATUS_FAILED;
  }
  // 2002 0904 3000 3100 3200 5000
  else if ((p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02) &&
           ((p_cmd_data[2] == 0x09 && p_cmd_data[3] == 0x04) /*||
            (p_cmd_data[2] == 0x0D && p_cmd_data[3] == 0x04)*/
            )) {
    *cmd_len += 0x01;
    p_cmd_data[2] += 0x01;
    p_cmd_data[9] = 0x01;
    p_cmd_data[10] = 0x40;
    p_cmd_data[11] = 0x50;
    p_cmd_data[12] = 0x00;

    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config ");
    //        phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config - End ");
  }
  //    20020703300031003200
  //    2002 0301 3200
  else if ((p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02) &&
           ((p_cmd_data[2] == 0x07 && p_cmd_data[3] == 0x03) ||
            (p_cmd_data[2] == 0x03 && p_cmd_data[3] == 0x01 &&
             p_cmd_data[4] == 0x32))) {
    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config ");
    phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
    *rsp_len = 5;
    p_rsp_data[0] = 0x40;
    p_rsp_data[1] = 0x02;
    p_rsp_data[2] = 0x02;
    p_rsp_data[3] = 0x00;
    p_rsp_data[4] = 0x00;

    phNxpNciHal_print_packet("RECV", p_rsp_data, 5);
    status = NFCSTATUS_FAILED;
    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config - End ");
  }

  // 2002 0D04 300104 310100 320100 500100
  // 2002 0401 320100
  else if ((p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02) &&
           (
               /*(p_cmd_data[2] == 0x0D && p_cmd_data[3] == 0x04)*/
               (p_cmd_data[2] == 0x04 && p_cmd_data[3] == 0x01 &&
                p_cmd_data[4] == 0x32 && p_cmd_data[5] == 0x00))) {
    //        p_cmd_data[12] = 0x40;

    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config ");
    phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
    p_cmd_data[6] = 0x60;

    phNxpNciHal_print_packet("RECV", p_rsp_data, 5);
    //        status = NFCSTATUS_FAILED;
    NXPLOG_NCIHAL_D("> Going through workaround - Dirty Set Config - End ");
  } else if (*cmd_len <= (NCI_MAX_DATA_LEN - 3) &&
             p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x00) {
    NXPLOG_NCIHAL_D(
        "> Going through workaround - Add Mifare Classic in Discovery Map");
    p_cmd_data[*cmd_len] = 0x80;
    p_cmd_data[*cmd_len + 1] = 0x01;
    p_cmd_data[*cmd_len + 2] = 0x80;
    p_cmd_data[5] = 0x01;
    p_cmd_data[6] = 0x01;
    p_cmd_data[2] += 3;
    p_cmd_data[3] += 1;
    *cmd_len += 3;
  } else if (*cmd_len == 3 && p_cmd_data[0] == 0x00 && p_cmd_data[1] == 0x00 &&
             p_cmd_data[2] == 0x00) {
    NXPLOG_NCIHAL_D("> Going through workaround - ISO-DEP Presence Check ");
    p_cmd_data[0] = 0x2F;
    p_cmd_data[1] = 0x11;
    p_cmd_data[2] = 0x00;
    status = NFCSTATUS_SUCCESS;
    NXPLOG_NCIHAL_D(
        "> Going through workaround - ISO-DEP Presence Check - End");
  }
#if 0
    else if ( (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02 ) &&
                 ((p_cmd_data[2] == 0x09 && p_cmd_data[3] == 0x04) ||
                     (p_cmd_data[2] == 0x0B && p_cmd_data[3] == 0x05) ||
                     (p_cmd_data[2] == 0x07 && p_cmd_data[3] == 0x02) ||
                     (p_cmd_data[2] == 0x0A && p_cmd_data[3] == 0x03) ||
                     (p_cmd_data[2] == 0x0A && p_cmd_data[3] == 0x04) ||
                     (p_cmd_data[2] == 0x05 && p_cmd_data[3] == 0x02))
             )
    {
        NXPLOG_NCIHAL_D ("> Going through workaround - Dirty Set Config ");
        phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
        *rsp_len = 5;
        p_rsp_data[0] = 0x40;
        p_rsp_data[1] = 0x02;
        p_rsp_data[2] = 0x02;
        p_rsp_data[3] = 0x00;
        p_rsp_data[4] = 0x00;

        phNxpNciHal_print_packet("RECV", p_rsp_data, 5);
        status = NFCSTATUS_FAILED;
        NXPLOG_NCIHAL_D ("> Going through workaround - Dirty Set Config - End ");
    }

    else if((p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02) &&
           ((p_cmd_data[3] == 0x00) ||
           ((*cmd_len >= 0x06) && (p_cmd_data[5] == 0x00)))) /*If the length of the first param id is zero don't allow*/
    {
        NXPLOG_NCIHAL_D ("> Going through workaround - Dirty Set Config ");
        phNxpNciHal_print_packet("SEND", p_cmd_data, *cmd_len);
        *rsp_len = 5;
        p_rsp_data[0] = 0x40;
        p_rsp_data[1] = 0x02;
        p_rsp_data[2] = 0x02;
        p_rsp_data[3] = 0x00;
        p_rsp_data[4] = 0x00;

        phNxpNciHal_print_packet("RECV", p_rsp_data, 5);
        status = NFCSTATUS_FAILED;
        NXPLOG_NCIHAL_D ("> Going through workaround - Dirty Set Config - End ");
    }
#endif
  else if ((wFwVerRsp & 0x0000FFFF) == wFwVer) {
    /* skip CORE_RESET and CORE_INIT from Brcm */
    if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x00 &&
        p_cmd_data[2] == 0x01 && p_cmd_data[3] == 0x01) {
      //            *rsp_len = 6;
      //
      //            NXPLOG_NCIHAL_D("> Going - core reset optimization");
      //
      //            p_rsp_data[0] = 0x40;
      //            p_rsp_data[1] = 0x00;
      //            p_rsp_data[2] = 0x03;
      //            p_rsp_data[3] = 0x00;
      //            p_rsp_data[4] = 0x10;
      //            p_rsp_data[5] = 0x01;
      //
      //            status = NFCSTATUS_FAILED;
      //            NXPLOG_NCIHAL_D("> Going - core reset optimization - END");
    }
    /* CORE_INIT */
    else if (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x01 &&
             p_cmd_data[2] == 0x00) {
    }
  }

  if (nfcFL.chipType == pn548C2 && p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02) {
    uint8_t temp;
    uint8_t* p = p_cmd_data + 4;
    uint8_t* end = p_cmd_data + *cmd_len;
    while (p < end) {
      if (*p == 0x53)  // LF_T3T_FLAGS
      {
        NXPLOG_NCIHAL_D("> Going through workaround - LF_T3T_FLAGS swap");
        temp = *(p + 3);
        *(p + 3) = *(p + 2);
        *(p + 2) = temp;
        NXPLOG_NCIHAL_D("> Going through workaround - LF_T3T_FLAGS - End");
        status = NFCSTATUS_SUCCESS;
        break;
      }
      if (*p == 0xA0) {
        p += *(p + 2) + 3;
      } else {
        p += *(p + 1) + 2;
      }
    }
  }

  if ((nfcFL.chipType == pn548C2) &&
          (p_cmd_data[0] == 0x20 && p_cmd_data[1] == 0x02)) {
      uint8_t temp;
      uint8_t* p = p_cmd_data + 4;
      uint8_t* end = p_cmd_data + *cmd_len;
      while (p < end) {
          if (*p == 0x53)  // LF_T3T_FLAGS
          {
              NXPLOG_NCIHAL_D("> Going through workaround - LF_T3T_FLAGS swap");
              temp = *(p + 3);
              *(p + 3) = *(p + 2);
              *(p + 2) = temp;
              NXPLOG_NCIHAL_D("> Going through workaround - LF_T3T_FLAGS - End");
              status = NFCSTATUS_SUCCESS;
              break;
          }
          if (*p == 0xA0) {
              p += *(p + 2) + 3;
          } else {
              p += *(p + 1) + 2;
          }
      }
  }

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_send_ext_cmd
 *
 * Description      This function send the extension command to NFCC. No
 *                  response is checked by this function but it waits for
 *                  the response to come.
 *
 * Returns          Returns NFCSTATUS_SUCCESS if sending cmd is successful and
 *                  response is received.
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_send_ext_cmd(uint16_t cmd_len, uint8_t* p_cmd) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  HAL_ENABLE_EXT();
  nxpncihal_ctrl.cmd_len = cmd_len;
  memcpy(nxpncihal_ctrl.p_cmd_data, p_cmd, cmd_len);
  status = phNxpNciHal_process_ext_cmd_rsp(nxpncihal_ctrl.cmd_len,
                                           nxpncihal_ctrl.p_cmd_data);
  HAL_DISABLE_EXT();

  return status;
}

/******************************************************************************
 * Function         phNxpNciHal_send_ese_hal_cmd
 *
 * Description      This function send the extension command to NFCC. No
 *                  response is checked by this function but it waits for
 *                  the response to come.
 *
 * Returns          Returns NFCSTATUS_SUCCESS if sending cmd is successful and
 *                  response is received.
 *
 ******************************************************************************/
NFCSTATUS phNxpNciHal_send_ese_hal_cmd(uint16_t cmd_len, uint8_t* p_cmd) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  if (cmd_len > NCI_MAX_DATA_LEN) {
    NXPLOG_NCIHAL_E("cmd_len exceeds limit NCI_MAX_DATA_LEN");
    return status;
  }
  nxpncihal_ctrl.cmd_len = cmd_len;
  memcpy(nxpncihal_ctrl.p_cmd_data, p_cmd, cmd_len);
  status = phNxpNciHal_process_ext_cmd_rsp(nxpncihal_ctrl.cmd_len,
                                              nxpncihal_ctrl.p_cmd_data);
  return status;
}

/******************************************************************************
 * Function         hal_extns_write_rsp_timeout_cb
 *
 * Description      Timer call back function
 *
 * Returns          None
 *
 ******************************************************************************/
static void hal_extns_write_rsp_timeout_cb(uint32_t timerId, void* pContext) {
  UNUSED(timerId);
  UNUSED(pContext);
  NXPLOG_NCIHAL_D("hal_extns_write_rsp_timeout_cb - write timeout!!!");
  nxpncihal_ctrl.ext_cb_data.status = NFCSTATUS_FAILED;
  usleep(1);
  sem_post(&(nxpncihal_ctrl.syncSpiNfc));
  SEM_POST(&(nxpncihal_ctrl.ext_cb_data));

  return;
}
