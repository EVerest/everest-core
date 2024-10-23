/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
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
 *  This file contains functions that interface with the NFCEEs.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "gki.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

/*******************************************************************************
**
** Function         NFC_NfceeDiscover
**
** Description      This function is called to enable or disable NFCEE
**                  Discovery. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_NFCEE_DISCOVER_REVT.
**                  The notification from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_NFCEE_INFO_REVT.
**
** Parameters       discover - 1 to enable discover, 0 to disable.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_NfceeDiscover(bool discover) {
  return nci_snd_nfcee_discover((uint8_t)(
      discover ? NCI_DISCOVER_ACTION_ENABLE : NCI_DISCOVER_ACTION_DISABLE));
}

/*******************************************************************************
**
** Function         NFC_NfceeModeSet
**
** Description      This function is called to activate or de-activate an NFCEE
**                  connected to the NFCC.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_NFCEE_MODE_SET_REVT.
**
** Parameters       nfcee_id - the NFCEE to activate or de-activate.
**                  mode - NFC_MODE_ACTIVATE to activate NFCEE,
**                         NFC_MODE_DEACTIVATE to de-activate.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_NfceeModeSet(uint8_t nfcee_id, tNFC_NFCEE_MODE mode) {
  tNFC_STATUS status = NCI_STATUS_OK;
  if (mode >= NCI_NUM_NFCEE_MODE || nfcee_id == NCI_DH_ID) {
    LOG(ERROR) << StringPrintf("%s invalid parameter:%d", __func__, mode);
    return NFC_STATUS_FAILED;
  }
  if (nfc_cb.nci_version != NCI_VERSION_2_0)
    status = nci_snd_nfcee_mode_set(nfcee_id, mode);
  else {
    if (nfc_cb.flags & NFC_FL_WAIT_MODE_SET_NTF)
      status = NFC_STATUS_REFUSED;
    else {
      status = nci_snd_nfcee_mode_set(nfcee_id, mode);
      if (status == NCI_STATUS_OK) {
        /* Mode set command is successfully queued or sent.
         * do not allow another Mode Set command until NTF is received */
        nfc_cb.flags |= NFC_FL_WAIT_MODE_SET_NTF;
        nfc_start_timer(&nfc_cb.nci_mode_set_ntf_timer,
                        (uint16_t)(NFC_TTYPE_WAIT_MODE_SET_NTF),
                        NFC_MODE_SET_NTF_TIMEOUT);
      }
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         NFC_SetRouting
**
** Description      This function is called to configure the CE routing table.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_SET_ROUTING_REVT.
**
** Parameters
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SetRouting(bool more, uint8_t num_tlv, uint8_t tlv_size,
                           uint8_t* p_param_tlvs) {
  return nci_snd_set_routing_cmd(more, num_tlv, tlv_size, p_param_tlvs);
}

/*******************************************************************************
**
** Function         NFC_GetRouting
**
** Description      This function is called to retrieve the CE routing table
**                  from NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_GET_ROUTING_REVT.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_GetRouting(void) { return nci_snd_get_routing_cmd(); }

/*******************************************************************************
**
** Function         NFC_NfceePLConfig
**
** Description      This function is called to set the Power and Link Control to
**                  an NFCEE connected to the NFCC.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_NFCEE_PL_CONTROL_REVT.
**
** Parameters       nfcee_id - the NFCEE to activate or de-activate.
**                  pl_config -
**                   NFCEE_PL_CONFIG_NFCC_DECIDES  NFCC decides (default)
**                   NFCEE_PL_CONFIG_PWR_ALWAYS_ON  NFCEE power supply is
**                                                          always on
**                   NFCEE_PL_CONFIG_LNK_ON_WHEN_PWR_ON  communication link is
**                                       always active when NFCEE is powered on
**                   NFCEE_PL_CONFIG_PWR_LNK_ALWAYS_ON  power supply and
**                                       communication link are always on
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_NfceePLConfig(uint8_t nfcee_id,
                              tNCI_NFCEE_PL_CONFIG pl_config) {
  return nci_snd_nfcee_power_link_control(nfcee_id, pl_config);
}
