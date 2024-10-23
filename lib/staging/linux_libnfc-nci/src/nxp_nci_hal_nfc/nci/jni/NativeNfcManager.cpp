/*
 * Copyright (C) 2012,2022 The Android Open Source Project
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
/******************************************************************************
 *
 *  The original Work has been changed by NXP.
 *
 *  Copyright 2020-2021 NXP
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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#ifdef LINUX
#include "nci_config.h"
#include "OverrideLog.h"
#include "nfa_snep_api.h"
#include "nativeNfcManager.h"
#include "nativeNfcSnep.h"
#include <signal.h>
#else
#include <cutils/properties.h>
#endif
#include <errno.h>
#ifndef LINUX
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <nativehelper/ScopedUtfChars.h>
#endif
#include <semaphore.h>
#include "HciEventManager.h"
#include "JavaClassConstants.h"
#include "NfcAdaptation.h"
#include "NfcJniUtil.h"
#include "NfcTag.h"
#include "PeerToPeer.h"
#include "PowerSwitch.h"
#include "RoutingManager.h"
#include "SyncEvent.h"
#include "nfc_config.h"
#if(NXP_EXTNS == TRUE)
#include "nfa_nfcee_int.h"
#include "NativeT4tNfcee.h"

#endif

#include "ce_api.h"
#include "nfa_api.h"
#include "nfa_ee_api.h"
#include "nfa_p2p_api.h"
#include "nfc_brcm_defs.h"
#include "phNxpExtns.h"
#include "rw_api.h"
#ifndef LINUX
using android::base::StringPrintf;
#endif
extern tNFA_DM_DISC_FREQ_CFG* p_nfa_dm_rf_disc_freq_cfg;  // defined in stack
#ifndef LINUX
namespace android {
#endif
extern bool gIsTagDeactivating;
extern bool gIsSelectingRfInterface;
extern void nativeNfcTag_doTransceiveStatus(tNFA_STATUS status, uint8_t* buf,
                                            uint32_t buflen);
extern void nativeNfcTag_notifyRfTimeout();
extern void nativeNfcTag_doConnectStatus(jboolean is_connect_ok);
extern void nativeNfcTag_doDeactivateStatus(int status);
extern void nativeNfcTag_doWriteStatus(jboolean is_write_ok);
extern jboolean nativeNfcTag_doDisconnect(JNIEnv*, jobject);
extern void nativeNfcTag_doCheckNdefResult(tNFA_STATUS status,
                                           uint32_t max_size,
                                           uint32_t current_size,
                                           uint8_t flags);
extern void nativeNfcTag_doMakeReadonlyResult(tNFA_STATUS status);
extern void nativeNfcTag_doPresenceCheckResult(tNFA_STATUS status);
extern void nativeNfcTag_formatStatus(bool is_ok);
extern void nativeNfcTag_resetPresenceCheck();
extern void nativeNfcTag_doReadCompleted(tNFA_STATUS status);
extern void nativeNfcTag_setRfInterface(tNFA_INTF_TYPE rfInterface);
extern void nativeNfcTag_setActivatedRfProtocol(tNFA_INTF_TYPE rfProtocol);
extern void nativeNfcTag_abortWaits();
extern void nativeLlcpConnectionlessSocket_abortWait();
extern void nativeNfcTag_registerNdefTypeHandler();
extern void nativeNfcTag_acquireRfInterfaceMutexLock();
extern void nativeNfcTag_releaseRfInterfaceMutexLock();
extern void nativeLlcpConnectionlessSocket_receiveData(uint8_t* data,
                                                       uint32_t len,
                                                       uint32_t remote_sap);
#ifndef LINUX
}  // namespace android
#endif
#ifdef LINUX
static BOOLEAN                 sSelectNext=false;
static UINT32                  sTech_mask;
static tNFA_TECHNOLOGY_MASK    sP2pListenTechMask; // P2P Listen mask
extern nfcTagCallback_t        *gTagCallback;
Mutex                          gSyncMutex;
extern void nativeNfcTag_releasePresenceCheck();
#endif
/*****************************************************************************
**
** public variables and functions
**
*****************************************************************************/
bool gActivated = false;
SyncEvent gDeactivatedEvent;
SyncEvent sNfaSetPowerSubState;
#ifdef LINUX
bool legacy_mfc_reader = false;
#else
bool legacy_mfc_reader = true;
#endif
#ifndef LINUX
namespace android {
#endif
#ifndef LINUX
jmethodID gCachedNfcManagerNotifyNdefMessageListeners;
jmethodID gCachedNfcManagerNotifyTransactionListeners;
jmethodID gCachedNfcManagerNotifyLlcpLinkActivation;
jmethodID gCachedNfcManagerNotifyLlcpLinkDeactivated;
jmethodID gCachedNfcManagerNotifyLlcpFirstPacketReceived;
jmethodID gCachedNfcManagerNotifyHostEmuActivated;
jmethodID gCachedNfcManagerNotifyHostEmuData;
jmethodID gCachedNfcManagerNotifyHostEmuDeactivated;
jmethodID gCachedNfcManagerNotifyRfFieldActivated;
jmethodID gCachedNfcManagerNotifyRfFieldDeactivated;
jmethodID gCachedNfcManagerNotifyEeUpdated;

const char* gNativeP2pDeviceClassName =
    "com/android/nfc/dhimpl/NativeP2pDevice";
const char* gNativeLlcpServiceSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpServiceSocket";
const char* gNativeLlcpConnectionlessSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpConnectionlessSocket";
const char* gNativeLlcpSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpSocket";
const char* gNativeNfcTagClassName = "com/android/nfc/dhimpl/NativeNfcTag";
const char* gNativeNfcManagerClassName =
    "com/android/nfc/dhimpl/NativeNfcManager";
#endif
void doStartupConfig();
void startStopPolling(bool isStartPolling);
#ifdef LINUX
void startRfDiscovery(BOOLEAN isStart);
bool addT4TNfceeAid();
#else
void startRfDiscovery(bool isStart);
#endif
bool isDiscoveryStarted();
#ifndef LINUX
}  // namespace android
#endif
/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
#ifndef LINUX
namespace android {
#endif
static jint sLastError = ERROR_BUFFER_TOO_SMALL;
static SyncEvent sNfaEnableEvent;                // event for NFA_Enable()
static SyncEvent sNfaDisableEvent;               // event for NFA_Disable()
static SyncEvent sNfaEnableDisablePollingEvent;  // event for
                                                 // NFA_EnablePolling(),
                                                 // NFA_DisablePolling()
static SyncEvent sNfaSetConfigEvent;             // event for Set_Config....
static SyncEvent sNfaGetConfigEvent;             // event for Get_Config....
static bool sIsNfaEnabled = false;
static bool sDiscoveryEnabled = false;  // is polling or listening
static bool sPollingEnabled = false;    // is polling for tag?
static bool sIsDisabling = false;
static bool sRfEnabled = false;   // whether RF discovery is enabled
static bool sSeRfActive = false;  // whether RF with SE is likely active
static bool sReaderModeEnabled =
    false;  // whether we're only reading tags, not allowing P2p/card emu
static bool sP2pEnabled = false;
static bool sP2pActive = false;  // whether p2p was last active
static bool sAbortConnlessWait = false;
static jint sLfT3tMax = 0;
static bool sRoutingInitialized = false;

#define CONFIG_UPDATE_TECH_MASK (1 << 1)
#define DEFAULT_TECH_MASK                                                  \
  (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F | \
   NFA_TECHNOLOGY_MASK_V | NFA_TECHNOLOGY_MASK_B_PRIME |                   \
   NFA_TECHNOLOGY_MASK_A_ACTIVE | NFA_TECHNOLOGY_MASK_F_ACTIVE |           \
   NFA_TECHNOLOGY_MASK_KOVIO)
#define DEFAULT_DISCOVERY_DURATION 500
#define READER_MODE_DISCOVERY_DURATION 200

static void nfaConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
static void nfaDeviceManagementCallback(uint8_t event,
                                        tNFA_DM_CBACK_DATA* eventData);
static bool isPeerToPeer(tNFA_ACTIVATED& activated);
static bool isListenMode(tNFA_ACTIVATED& activated);
static tNFA_STATUS stopPolling_rfDiscoveryDisabled();
static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask);
static void nfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                        jint screen_state_mask);
#ifdef LINUX
static void sig_handler(int signo);
#endif

static uint16_t sCurrentConfigLen;
static uint8_t sConfig[256];
static int prevScreenState = NFA_SCREEN_STATE_OFF_LOCKED;
static int NFA_SCREEN_POLLING_TAG_MASK = 0x10;
static bool gIsDtaEnabled = false;
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#ifdef LINUX
//#define nfc_debug_enabled nfc_debug_enabled1
#endif
bool nfc_debug_enabled = false;
namespace {
void initializeGlobalDebugEnabledFlag() {
  nfc_debug_enabled =
      (NfcConfig::getUnsigned(NAME_NFC_DEBUG_ENABLED, 1) != 0) ? true : false;
#ifndef LINUX
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  int len = property_get("nfc.debug_enabled", valueStr, "");
  if (len > 0) {
    unsigned debug_enabled = 1;
    // let Android property override .conf variable
    sscanf(valueStr, "%u", &debug_enabled);
    nfc_debug_enabled = (debug_enabled == 0) ? false : true;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: level=%u", __func__, nfc_debug_enabled);
#else
  //nfc_debug_enabled  = false;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: level=%u", __func__, nfc_debug_enabled);
#endif
}
void initializeMfcReaderOption() {
  legacy_mfc_reader =
      (NfcConfig::getUnsigned(NAME_LEGACY_MIFARE_READER, 0) != 0) ? true
                                                                  : false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << __func__ <<": mifare reader option=" << legacy_mfc_reader;

}
}  // namespace
#ifndef LINUX
/*******************************************************************************
**
** Function:        getNative
**
** Description:     Get native data
**
** Returns:         Native data structure.
**
*******************************************************************************/
nfc_jni_native_data* getNative(JNIEnv* e, jobject o) {
  static struct nfc_jni_native_data* sCachedNat = NULL;
  if (e) {
    sCachedNat = nfc_jni_get_nat(e, o);
  }
  return sCachedNat;
}
#endif
/*******************************************************************************
**
** Function:        handleRfDiscoveryEvent
**
** Description:     Handle RF-discovery events from the stack.
**                  discoveredDevice: Discovered device.
**
** Returns:         None
**
*******************************************************************************/
static void handleRfDiscoveryEvent(tNFC_RESULT_DEVT* discoveredDevice) {
  NfcTag& natTag = NfcTag::getInstance();
  natTag.setNumDiscNtf(natTag.getNumDiscNtf() + 1);
  if (discoveredDevice->more == NCI_DISCOVER_NTF_MORE) {
    // there is more discovery notification coming
    return;
  }

#ifdef LINUX
  natTag.mNumTags = natTag.getNumDiscNtf();
#endif

  bool isP2p = natTag.isP2pDiscovered();

  if (natTag.getNumDiscNtf() > 1) {
    natTag.setMultiProtocolTagSupport(true);
    if (isP2p) {
      // Remove NFC_DEP NTF count
      // Skip NFC_DEP protocol in MultiProtocolTag select.
      natTag.setNumDiscNtf(natTag.getNumDiscNtf() - 1);
    }
  }

  if (sP2pEnabled && !sReaderModeEnabled && isP2p) {
    // select the peer that supports P2P
    natTag.selectP2p();
  } else {
    natTag.setNumDiscNtf(natTag.getNumDiscNtf() - 1);
    // select the first of multiple tags that is discovered
    natTag.selectFirstTag();
  }
}

/*******************************************************************************
**
** Function:        nfaConnectionCallback
**
** Description:     Receive connection-related events from stack.
**                  connEvent: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
static void nfaConnectionCallback(uint8_t connEvent,
                                  tNFA_CONN_EVT_DATA* eventData) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: event= %u", __func__, connEvent);

  switch (connEvent) {
    case NFA_POLL_ENABLED_EVT:  // whether polling successfully started
    {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_POLL_ENABLED_EVT: status = %u", __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_POLL_DISABLED_EVT:  // Listening/Polling stopped
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_POLL_DISABLED_EVT: status = %u", __func__,
                          eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_RF_DISCOVERY_STARTED_EVT:  // RF Discovery started
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STARTED_EVT: status = %u",
                          __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_RF_DISCOVERY_STOPPED_EVT:  // RF Discovery stopped event
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u",
                          __func__, eventData->status);

      gActivated = false;

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_DISC_RESULT_EVT:  // NFC link/protocol discovery notificaiton
      status = eventData->disc_result.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DISC_RESULT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        NfcTag::getInstance().setNumDiscNtf(0);
        LOG(ERROR) << StringPrintf("%s: NFA_DISC_RESULT_EVT error: status = %d",
                                   __func__, status);
      } else {
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        handleRfDiscoveryEvent(&eventData->disc_result.discovery_ntf);
      }
      break;

    case NFA_SELECT_RESULT_EVT:  // NFC link/protocol discovery select response
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_RESULT_EVT: status = %d, gIsSelectingRfInterface = "
          "%d, "
          "sIsDisabling=%d",
          __func__, eventData->status, gIsSelectingRfInterface, sIsDisabling);

      if (sIsDisabling) break;

      if (eventData->status != NFA_STATUS_OK) {
        if (gIsSelectingRfInterface) {
          nativeNfcTag_doConnectStatus(false);
        }

        LOG(ERROR) << StringPrintf(
            "%s: NFA_SELECT_RESULT_EVT error: status = %d", __func__,
            eventData->status);
        NFA_Deactivate(FALSE);
      }
      break;

    case NFA_DEACTIVATE_FAIL_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DEACTIVATE_FAIL_EVT: status = %d", __func__,
                          eventData->status);
      break;

    case NFA_ACTIVATED_EVT:  // NFC link/protocol activated
    {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_ACTIVATED_EVT: gIsSelectingRfInterface=%d, sIsDisabling=%d",
          __func__, gIsSelectingRfInterface, sIsDisabling);
      uint8_t activatedProtocol =
          (tNFA_INTF_TYPE)eventData->activated.activate_ntf.protocol;

      if ((eventData->activated.activate_ntf.protocol !=
           NFA_PROTOCOL_NFC_DEP) &&
          (!isListenMode(eventData->activated))) {
        nativeNfcTag_setRfInterface(
            (tNFA_INTF_TYPE)eventData->activated.activate_ntf.intf_param.type);
        nativeNfcTag_setActivatedRfProtocol(activatedProtocol);
      }
      if (EXTNS_GetConnectFlag() == TRUE) {
        NfcTag::getInstance().setActivationState();
        nativeNfcTag_doConnectStatus(true);
        break;
      }
      NfcTag::getInstance().setActive(true);
      if (sIsDisabling || !sIsNfaEnabled) break;
      gActivated = true;

      NfcTag::getInstance().setActivationState();
      if (gIsSelectingRfInterface) {
        nativeNfcTag_doConnectStatus(true);
        break;
      }

      nativeNfcTag_resetPresenceCheck();
#ifndef LINUX
      if (!isListenMode(eventData->activated) &&
          (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
           prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED)) {
        NFA_Deactivate(FALSE);
      }
#endif
      if (isPeerToPeer(eventData->activated)) {
        if (sReaderModeEnabled) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: ignoring peer target in reader mode.", __func__);
          NFA_Deactivate(FALSE);
          break;
        }
        sP2pActive = true;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_ACTIVATED_EVT; is p2p", __func__);
        if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
          // Disable RF field events in case of p2p
          uint8_t nfa_disable_rf_events[] = {0x00};
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: Disabling RF field events", __func__);
          status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                 sizeof(nfa_disable_rf_events),
                                 &nfa_disable_rf_events[0]);
          if (status == NFA_STATUS_OK) {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: Disabled RF field events", __func__);
          } else {
            LOG(ERROR) << StringPrintf("%s: Failed to disable RF field events",
                                       __func__);
          }
        }
      } else {
          NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
#ifndef LINUX
          if (NfcTag::getInstance().getNumDiscNtf()) {
              /*If its multiprotocol tag, deactivate tag with current selected
                protocol to sleep . Select tag with next supported protocol after
                deactivation event is received*/
              NFA_Deactivate(true);

          }
#endif

        // We know it is not activating for P2P.  If it activated in
        // listen mode then it is likely for an SE transaction.
        // Send the RF Event.
        if (isListenMode(eventData->activated)) {
          sSeRfActive = true;
        }
      }
    } break;
    case NFA_DEACTIVATED_EVT:  // NFC link/protocol deactivated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DEACTIVATED_EVT   Type: %u, gIsTagDeactivating: %d",
          __func__, eventData->deactivated.type, gIsTagDeactivating);
      NfcTag::getInstance().setDeactivationState(eventData->deactivated);
#ifndef LINUX
      NfcTag::getInstance().selectNextTagIfExists();
#endif
      if (eventData->deactivated.type != NFA_DEACTIVATE_TYPE_SLEEP) {
        {
          SyncEventGuard g(gDeactivatedEvent);
          gActivated = false;  // guard this variable from multi-threaded access
          gDeactivatedEvent.notifyOne();
        }
        nativeNfcTag_resetPresenceCheck();
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        nativeNfcTag_abortWaits();
        NfcTag::getInstance().abort();
      } else if (gIsTagDeactivating) {
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(0);
      } else if (EXTNS_GetDeactivateFlag() == TRUE) {
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(0);
      }
#ifdef LINUX
      if( sSelectNext && NfcTag::getInstance ().mNumTags)
      {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: selectNextTag", __func__);
          nativeNfcTag_releasePresenceCheck();
          {
              SyncEventGuard g (gDeactivatedEvent);
              gActivated = false; //guard this variable from multi-threaded access
              gDeactivatedEvent.notifyOne ();
          }
          //NfcTag::getInstance ().mNumDiscNtf--;

          NXPLOG_API_D("%s: Multiple Tags=%d Select Next", __FUNCTION__
                  , NfcTag::getInstance ().mNumTags);

          NfcTag::getInstance().selectNextTagIfExists();
          sSelectNext = false;
      }

#endif
      // If RF is activated for what we think is a Secure Element transaction
      // and it is deactivated to either IDLE or DISCOVERY mode, notify w/event.
      if ((eventData->deactivated.type == NFA_DEACTIVATE_TYPE_IDLE) ||
          (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_DISCOVERY)) {
        if (sSeRfActive) {
          sSeRfActive = false;
        } else if (sP2pActive) {
          sP2pActive = false;
          // Make sure RF field events are re-enabled
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: NFA_DEACTIVATED_EVT; is p2p", __func__);
          if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
            // Disable RF field events in case of p2p
            uint8_t nfa_enable_rf_events[] = {0x01};

            if (!sIsDisabling && sIsNfaEnabled) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: Enabling RF field events", __func__);
              status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                     sizeof(nfa_enable_rf_events),
                                     &nfa_enable_rf_events[0]);
              if (status == NFA_STATUS_OK) {
                DLOG_IF(INFO, nfc_debug_enabled)
                    << StringPrintf("%s: Enabled RF field events", __func__);
              } else {
                LOG(ERROR) << StringPrintf(
                    "%s: Failed to enable RF field events", __func__);
              }
            }
          }
        }
      }

      break;

    case NFA_TLV_DETECT_EVT:  // TLV Detection complete
      status = eventData->tlv_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_TLV_DETECT_EVT: status = %d, protocol = %d, num_tlvs = %d, "
          "num_bytes = %d",
          __func__, status, eventData->tlv_detect.protocol,
          eventData->tlv_detect.num_tlvs, eventData->tlv_detect.num_bytes);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_TLV_DETECT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_NDEF_DETECT_EVT:  // NDEF Detection complete;
      // if status is failure, it means the tag does not contain any or valid
      // NDEF data;  pass the failure status to the NFC Service;
      status = eventData->ndef_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = %u, "
          "max_size = %u, cur_size = %u, flags = 0x%X",
          __func__, status, eventData->ndef_detect.protocol,
          eventData->ndef_detect.max_size, eventData->ndef_detect.cur_size,
          eventData->ndef_detect.flags);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      nativeNfcTag_doCheckNdefResult(status, eventData->ndef_detect.max_size,
                                     eventData->ndef_detect.cur_size,
                                     eventData->ndef_detect.flags);
      break;

    case NFA_DATA_EVT:  // Data message received (for non-NDEF reads)
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DATA_EVT: status = 0x%X, len = %d", __func__,
                          eventData->status, eventData->data.len);
      nativeNfcTag_doTransceiveStatus(eventData->status, eventData->data.p_data,
                                      eventData->data.len);
      break;
    case NFA_RW_INTF_ERROR_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFC_RW_INTF_ERROR_EVT", __func__);
      nativeNfcTag_notifyRfTimeout();
      nativeNfcTag_doReadCompleted(NFA_STATUS_TIMEOUT);
      break;
    case NFA_SELECT_CPLT_EVT:  // Select completed
      status = eventData->status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_CPLT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SELECT_CPLT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_READ_CPLT_EVT:  // NDEF-read or tag-specific-read completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_READ_CPLT_EVT: status = 0x%X", __func__, eventData->status);
      nativeNfcTag_doReadCompleted(eventData->status);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      break;

    case NFA_WRITE_CPLT_EVT:  // Write completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_WRITE_CPLT_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doWriteStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_SET_TAG_RO_EVT:  // Tag set as Read only
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SET_TAG_RO_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doMakeReadonlyResult(eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_START_EVT:  // NDEF write started
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_NDEF_WRITE_START_EVT: status: %d",
                          __func__, eventData->status);

      if (eventData->status != NFA_STATUS_OK)
        LOG(ERROR) << StringPrintf(
            "%s: NFA_CE_NDEF_WRITE_START_EVT error: status = %d", __func__,
            eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_CPLT_EVT:  // NDEF write completed
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: FA_CE_NDEF_WRITE_CPLT_EVT: len = %u", __func__,
                          eventData->ndef_write_cplt.len);
      break;

    case NFA_LLCP_ACTIVATED_EVT:  // LLCP link is activated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_LLCP_ACTIVATED_EVT: is_initiator: %d  remote_wks: %d, "
          "remote_lsc: %d, remote_link_miu: %d, local_link_miu: %d",
          __func__, eventData->llcp_activated.is_initiator,
          eventData->llcp_activated.remote_wks,
          eventData->llcp_activated.remote_lsc,
          eventData->llcp_activated.remote_link_miu,
          eventData->llcp_activated.local_link_miu);
#ifndef LINUX
      PeerToPeer::getInstance().llcpActivatedHandler(getNative(0, 0),
                                                     eventData->llcp_activated);
#endif
      break;

    case NFA_LLCP_DEACTIVATED_EVT:  // LLCP link is deactivated
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_DEACTIVATED_EVT", __func__);
#ifndef LINUX
      PeerToPeer::getInstance().llcpDeactivatedHandler(
          getNative(0, 0), eventData->llcp_deactivated);
#endif
      break;
    case NFA_LLCP_FIRST_PACKET_RECEIVED_EVT:  // Received first packet over llcp
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_FIRST_PACKET_RECEIVED_EVT", __func__);
#ifndef LINUX
      PeerToPeer::getInstance().llcpFirstPacketHandler(getNative(0, 0));
#endif
      break;
    case NFA_PRESENCE_CHECK_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_PRESENCE_CHECK_EVT", __func__);
      nativeNfcTag_doPresenceCheckResult(eventData->status);
      break;
    case NFA_FORMAT_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_FORMAT_CPLT_EVT: status=0x%X", __func__, eventData->status);
      nativeNfcTag_formatStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_I93_CMD_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_I93_CMD_CPLT_EVT: status=0x%X", __func__, eventData->status);
      break;

    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_UICC_LISTEN_CONFIGURED_EVT : status=0x%X",
                          __func__, eventData->status);
      break;

    case NFA_SET_P2P_LISTEN_TECH_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_SET_P2P_LISTEN_TECH_EVT", __func__);
      PeerToPeer::getInstance().connectionEventHandler(connEvent, eventData);
      break;

    #if (NXP_EXTNS == TRUE)
    case NFA_T4TNFCEE_EVT:
    case NFA_T4TNFCEE_READ_CPLT_EVT:
    case NFA_T4TNFCEE_WRITE_CPLT_EVT:
    case NFA_T4TNFCEE_CLEAR_CPLT_EVT:
      t4tNfcEe.eventHandler(connEvent, eventData);
      break;
    #endif

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event ????", __func__);
      break;
  }
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_initNativeStruc
**
** Description:     Initialize variables.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_initNativeStruc(JNIEnv* e, jobject o) {
  initializeGlobalDebugEnabledFlag();
  initializeMfcReaderOption();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  nfc_jni_native_data* nat =
      (nfc_jni_native_data*)malloc(sizeof(struct nfc_jni_native_data));
  if (nat == NULL) {
    LOG(ERROR) << StringPrintf("%s: fail allocate native data", __func__);
    return JNI_FALSE;
  }

  memset(nat, 0, sizeof(*nat));
  e->GetJavaVM(&(nat->vm));
  nat->env_version = e->GetVersion();
  nat->manager = e->NewGlobalRef(o);

  ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");
  e->SetLongField(o, f, (jlong)nat);

  /* Initialize native cached references */
  gCachedNfcManagerNotifyNdefMessageListeners =
      e->GetMethodID(cls.get(), "notifyNdefMessageListeners",
                     "(Lcom/android/nfc/dhimpl/NativeNfcTag;)V");
  gCachedNfcManagerNotifyLlcpLinkActivation =
      e->GetMethodID(cls.get(), "notifyLlcpLinkActivation",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpLinkDeactivated =
      e->GetMethodID(cls.get(), "notifyLlcpLinkDeactivated",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpFirstPacketReceived =
      e->GetMethodID(cls.get(), "notifyLlcpLinkFirstPacketReceived",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");

  gCachedNfcManagerNotifyHostEmuActivated =
      e->GetMethodID(cls.get(), "notifyHostEmuActivated", "(I)V");

  gCachedNfcManagerNotifyHostEmuData =
      e->GetMethodID(cls.get(), "notifyHostEmuData", "(I[B)V");

  gCachedNfcManagerNotifyHostEmuDeactivated =
      e->GetMethodID(cls.get(), "notifyHostEmuDeactivated", "(I)V");

  gCachedNfcManagerNotifyRfFieldActivated =
      e->GetMethodID(cls.get(), "notifyRfFieldActivated", "()V");
  gCachedNfcManagerNotifyRfFieldDeactivated =
      e->GetMethodID(cls.get(), "notifyRfFieldDeactivated", "()V");

  gCachedNfcManagerNotifyTransactionListeners = e->GetMethodID(
      cls.get(), "notifyTransactionListeners", "([B[BLjava/lang/String;)V");

  gCachedNfcManagerNotifyEeUpdated =
      e->GetMethodID(cls.get(), "notifyEeUpdated", "()V");

  if (nfc_jni_cache_object(e, gNativeNfcTagClassName, &(nat->cached_NfcTag)) ==
      -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeNfcTag", __func__);
    return JNI_FALSE;
  }

  if (nfc_jni_cache_object(e, gNativeP2pDeviceClassName,
                           &(nat->cached_P2pDevice)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeP2pDevice", __func__);
    return JNI_FALSE;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}
#endif
/*******************************************************************************
**
** Function:        nfaDeviceManagementCallback
**
** Description:     Receive device management events from stack.
**                  dmEvent: Device-management event ID.
**                  eventData: Data associated with event ID.
**
** Returns:         None
**
*******************************************************************************/
void nfaDeviceManagementCallback(uint8_t dmEvent,
                                 tNFA_DM_CBACK_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; event=0x%X", __func__, dmEvent);

  switch (dmEvent) {
    case NFA_DM_ENABLE_EVT: /* Result of NFA_Enable */
    {
      SyncEventGuard guard(sNfaEnableEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_ENABLE_EVT; status=0x%X", __func__, eventData->status);
      sIsNfaEnabled = eventData->status == NFA_STATUS_OK;
      sIsDisabling = false;
      sNfaEnableEvent.notifyOne();
    } break;

    case NFA_DM_DISABLE_EVT: /* Result of NFA_Disable */
    {
      SyncEventGuard guard(sNfaDisableEvent);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_DISABLE_EVT", __func__);
      sIsNfaEnabled = false;
      sIsDisabling = false;
      sNfaDisableEvent.notifyOne();
    } break;

    case NFA_DM_SET_CONFIG_EVT:  // result of NFA_SetConfig
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(sNfaSetConfigEvent);
        sNfaSetConfigEvent.notifyOne();
      }
      break;

    case NFA_DM_GET_CONFIG_EVT: /* Result of NFA_GetConfig */
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_GET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(sNfaGetConfigEvent);
        if (eventData->status == NFA_STATUS_OK &&
            eventData->get_config.tlv_size <= sizeof(sConfig)) {
          sCurrentConfigLen = eventData->get_config.tlv_size;
          memcpy(sConfig, eventData->get_config.param_tlvs,
                 eventData->get_config.tlv_size);
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_DM_GET_CONFIG failed", __func__);
          sCurrentConfigLen = 0;
        }
        sNfaGetConfigEvent.notifyOne();
      }
      break;

    case NFA_DM_RF_FIELD_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_RF_FIELD_EVT; status=0x%X; field status=%u", __func__,
          eventData->rf_field.status, eventData->rf_field.rf_field_status);
#if (NFC_AGC_DEBUG_FEATURE == TRUE)
      EXTNS_DebugAgcCfg(eventData->rf_field.rf_field_status);
#endif
#ifndef LINUX
      if (!sP2pActive && eventData->rf_field.status == NFA_STATUS_OK) {
        struct nfc_jni_native_data* nat = getNative(NULL, NULL);
        JNIEnv* e = NULL;
        ScopedAttach attach(nat->vm, &e);
        if (e == NULL) {
          LOG(ERROR) << StringPrintf("jni env is null");
          return;
        }
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON)
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldActivated);
        else
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldDeactivated);
      }
#endif
      break;

    case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
    case NFA_DM_NFCC_TIMEOUT_EVT: {
      if (dmEvent == NFA_DM_NFCC_TIMEOUT_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TIMEOUT_EVT; abort",
                                   __func__);
      else if (dmEvent == NFA_DM_NFCC_TRANSPORT_ERR_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TRANSPORT_ERR_EVT; abort",
                                   __func__);

      nativeNfcTag_abortWaits();
      NfcTag::getInstance().abort();
      sAbortConnlessWait = true;
#ifndef LINUX
      nativeLlcpConnectionlessSocket_abortWait();
#endif
      {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: aborting  sNfaEnableDisablePollingEvent", __func__);
        SyncEventGuard guard(sNfaEnableDisablePollingEvent);
        sNfaEnableDisablePollingEvent.notifyOne();
      }
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: aborting  sNfaEnableEvent", __func__);
        SyncEventGuard guard(sNfaEnableEvent);
        sNfaEnableEvent.notifyOne();
      }
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: aborting  sNfaDisableEvent", __func__);
        SyncEventGuard guard(sNfaDisableEvent);
        sNfaDisableEvent.notifyOne();
      }
      sDiscoveryEnabled = false;
      sPollingEnabled = false;
#ifndef LINUX
      PowerSwitch::getInstance().abort();
#endif
      if (!sIsDisabling && sIsNfaEnabled) {
        EXTNS_Close();
        NFA_Disable(FALSE);
        sIsDisabling = true;
      } else {
        sIsNfaEnabled = false;
        sIsDisabling = false;
      }
#ifndef LINUX
      PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
#endif
      LOG(ERROR) << StringPrintf("%s: crash NFC service", __func__);
      //////////////////////////////////////////////
      // crash the NFC service process so it can restart automatically
      abort();
      //////////////////////////////////////////////
    } break;

    case NFA_DM_PWR_MODE_CHANGE_EVT:
#ifndef LINUX
      PowerSwitch::getInstance().deviceManagementCallback(dmEvent, eventData);
#endif
      break;

    case NFA_DM_SET_POWER_SUB_STATE_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%X",
                          __FUNCTION__, eventData->power_sub_state.status);
      SyncEventGuard guard(sNfaSetPowerSubState);
      sNfaSetPowerSubState.notifyOne();
    } break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unhandled event", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfcManager_sendRawFrame
**
** Description:     Send a raw frame.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
#ifdef LINUX
bool nfcManager_sendRawFrame(UINT8 *buf, UINT32 buffLen){
  jbyteArray data = buf;
  Bytes bytes(buf,buffLen);
#else
static jboolean nfcManager_sendRawFrame(JNIEnv* e, jobject, jbyteArray data) {
  ScopedByteArrayRO bytes(e, data);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
#endif
  size_t bufLen = bytes.size();
  tNFA_STATUS status = NFA_SendRawFrame(buf, bufLen, 0);

  return (status == NFA_STATUS_OK);
}

#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_routeAid
**
** Description:     Route an AID to an EE
**                  e: JVM environment.
**                  aid: aid to be added to routing table.
**                  route: aid route location. i.e. DH/eSE/UICC
**                  aidInfo: prefix or suffix aid.
**
** Returns:         True if aid is accpted by NFA Layer.
**
*******************************************************************************/
static jboolean nfcManager_routeAid(JNIEnv* e, jobject, jbyteArray aid,
                                    jint route, jint aidInfo, jint power) {
  uint8_t* buf;
  size_t bufLen;

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
    return RoutingManager::getInstance().addAidRouting(buf, bufLen, route,
                                                       aidInfo, power);
  }
  ScopedByteArrayRO bytes(e, aid);
  buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  bufLen = bytes.size();
  #if (NXP_EXTNS == TRUE)
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: check and update AID 1", __func__);
    NativeT4tNfcee::getInstance().checkAndUpdateT4TAid(buf, (uint8_t*)&bufLen);

    RoutingManager::getInstance().removeAidRouting(buf, bufLen);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: check and update AID 2", __func__);
  #endif
  return RoutingManager::getInstance().addAidRouting(buf, bufLen, route,
                                                     aidInfo, power);
}

/*******************************************************************************
**
** Function:        nfcManager_unrouteAid
**
** Description:     Remove a AID routing
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_unrouteAid(JNIEnv* e, jobject, jbyteArray aid) {
  uint8_t* buf;
  size_t bufLen;

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
    return RoutingManager::getInstance().removeAidRouting(buf, bufLen);
  }
  ScopedByteArrayRO bytes(e, aid);
  buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  bufLen = bytes.size();
  return RoutingManager::getInstance().removeAidRouting(buf, bufLen);
}

/*******************************************************************************
**
** Function:        nfcManager_commitRouting
**
** Description:     Sends the AID routing table to the controller
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_commitRouting(JNIEnv* e, jobject) {
  if (sRfEnabled) {
    /*Update routing table only in Idle state.*/
    startRfDiscovery(false);
  }
  jboolean commitStatus = RoutingManager::getInstance().commitRouting();
  startRfDiscovery(true);
  return commitStatus;
}
#endif
/*******************************************************************************
**
** Function:        nfcManager_doRegisterT3tIdentifier
**
** Description:     Registers LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  t3tIdentifier: LF_T3T_IDENTIFIER value (10 or 18 bytes)
**
** Returns:         Handle retrieve from RoutingManager.
**
*******************************************************************************/
#ifdef LINUX
jint nfcManager_doRegisterT3tIdentifier(UINT8 *buf, UINT8 t3tIdsize){
#else
static jint nfcManager_doRegisterT3tIdentifier(JNIEnv* e, jobject,
                                               jbyteArray t3tIdentifier) {
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
#ifdef LINUX
  Bytes bytes(buf,t3tIdsize);
#else
  ScopedByteArrayRO bytes(e, t3tIdentifier);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
#endif
  size_t bufLen = bytes.size();
  int handle = RoutingManager::getInstance().registerT3tIdentifier(buf, bufLen);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: handle=%d", __func__, handle);
  if (handle != NFA_HANDLE_INVALID)
    RoutingManager::getInstance().commitRouting();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return handle;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeregisterT3tIdentifier
**
** Description:     Deregisters LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  handle: Handle retrieve from libnfc-nci.
**
** Returns:         None
**
*******************************************************************************/
#ifdef LINUX
static void nfcManager_doDeregisterT3tIdentifier(jint handle) {
#else
static void nfcManager_doDeregisterT3tIdentifier(JNIEnv*, jobject,
                                                 jint handle) {
#endif
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; handle=%d", __func__, handle);

  RoutingManager::getInstance().deregisterT3tIdentifier(handle);
  RoutingManager::getInstance().commitRouting();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_getLfT3tMax
**
** Description:     Returns LF_T3T_MAX value.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         LF_T3T_MAX value.
**
*******************************************************************************/
static jint nfcManager_getLfT3tMax(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("LF_T3T_MAX=%d", sLfT3tMax);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return sLfT3tMax;
}

#endif
/*******************************************************************************
**
** Function:        nfcManager_doInitialize
**
** Description:     Turn on NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
#ifdef LINUX
bool nfcManager_doInitialize() {
JNIEnv* e;
jobject o;
#else
static jboolean nfcManager_doInitialize(JNIEnv* e, jobject o) {
#endif
  initializeGlobalDebugEnabledFlag();
  tNFA_STATUS stat = NFA_STATUS_OK;
#ifndef LINUX
  PowerSwitch& powerSwitch = PowerSwitch::getInstance();
#endif
  if (sIsNfaEnabled) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already enabled", __func__);
    goto TheEnd;
  }

#ifdef LINUX
if ((signal(SIGABRT, sig_handler) == SIG_ERR) &&
            (signal(SIGSEGV, sig_handler) == SIG_ERR) &&
            (signal(SIGINT, sig_handler) == SIG_ERR) &&
            (signal(SIGHUP, sig_handler) == SIG_ERR))
    {
        NXPLOG_API_E("Failed to register signal handler");
    }
#else
  //powerSwitch.initialize(PowerSwitch::FULL_POWER);
#endif
  {

    NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
    theInstance.Initialize();  // start GKI, NCI task, NFC task

    {
      SyncEventGuard guard(sNfaEnableEvent);
      tHAL_NFC_ENTRY* halFuncEntries = theInstance.GetHalEntryFuncs();

      NFA_Init(halFuncEntries);

      stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
if (stat == NFA_STATUS_OK) {
#ifdef LINUX
    /* The Global App Log already initialised during Adaptation
     *  Initialise */
    int num = initializeGlobalAppLogLevel ();
    CE_SetTraceLevel (num);
    //LLCP_SetTraceLevel (num);
    NFC_SetTraceLevel (num);
    //RW_SetTraceLevel (num);
    //NFA_SetTraceLevel (num);
    NFA_P2pSetTraceLevel (num);
    NFA_SnepSetTraceLevel(num);
#endif
    sNfaEnableEvent.wait();  // wait for NFA command to finish
}
      EXTNS_Init(nfaDeviceManagementCallback, nfaConnectionCallback);
    }

    if (stat == NFA_STATUS_OK) {
      // sIsNfaEnabled indicates whether stack started successfully
      if (sIsNfaEnabled) {
#ifndef LINUX
        sRoutingInitialized =
            RoutingManager::getInstance().initialize(getNative(e, o));
#else

        sRoutingInitialized =
            RoutingManager::getInstance().initialize(NULL);
#endif
        nativeNfcTag_registerNdefTypeHandler();
#ifndef LINUX
        NfcTag::getInstance().initialize(getNative(e, o));
#else
        NfcTag::getInstance().initialize(NULL);
        nativeNfcSnep_handleNfcOnOff(true);
#endif
        PeerToPeer::getInstance().initialize();
#ifndef LINUX
        PeerToPeer::getInstance().handleNfcOnOff(true);
        HciEventManager::getInstance().initialize(getNative(e, o));
#else
        //HciEventManager::getInstance().initialize(NULL);
#endif
        /////////////////////////////////////////////////////////////////////////////////
        // Add extra configuration here (work-arounds, etc.)

        if (gIsDtaEnabled == true) {
          uint8_t configData = 0;
          configData = 0x01; /* Poll NFC-DEP : Highest Available Bit Rates */
          NFA_SetConfig(NCI_PARAM_ID_BITR_NFC_DEP, sizeof(uint8_t),
                        &configData);
          configData = 0x0B; /* Listen NFC-DEP : Waiting Time */
          NFA_SetConfig(NFC_PMID_WT, sizeof(uint8_t), &configData);
          configData = 0x0F; /* Specific Parameters for NFC-DEP RF Interface */
          NFA_SetConfig(NCI_PARAM_ID_NFC_DEP_OP, sizeof(uint8_t), &configData);
        }

#ifndef LINUX
        struct nfc_jni_native_data* nat = getNative(e, o);
        if (nat) {
          nat->tech_mask =
              NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: tag polling tech mask=0x%X", __func__, nat->tech_mask);
        }
        // if this value exists, set polling interval.
        nat->discovery_duration = NfcConfig::getUnsigned(
            NAME_NFA_DM_DISC_DURATION_POLL, DEFAULT_DISCOVERY_DURATION);

        NFA_SetRfDiscoveryDuration(nat->discovery_duration);
#else
        INT32 discovery_duration;
        int num = 0;
        if (GetNumValue(NAME_POLLING_TECH_MASK, &num, sizeof(num)))
        {
            sTech_mask = num;
            NXPLOG_API_D ("%s: NAME_POLLING_TECH_MASK mask=0x%X", __FUNCTION__, sTech_mask);
        }
        else
        {
            sTech_mask = DEFAULT_TECH_MASK;
        }
        NXPLOG_API_D ("%s: tag polling tech mask=0x%X", __FUNCTION__, sTech_mask);
        if (GetNumValue (NAME_P2P_LISTEN_TECH_MASK, &num, sizeof (num)))
        {
            sP2pListenTechMask = num;
        }
        else
        {
            sP2pListenTechMask = (NFA_TECHNOLOGY_MASK_A
                    | NFA_TECHNOLOGY_MASK_F
                    | NFA_TECHNOLOGY_MASK_A_ACTIVE
                    | NFA_TECHNOLOGY_MASK_F_ACTIVE);
        }
        NXPLOG_API_D ("%s: @@P2P listen tech mask=0x%X", __FUNCTION__, sP2pListenTechMask);
        discovery_duration = NfcConfig::getUnsigned(
            NAME_NFA_DM_DISC_DURATION_POLL, DEFAULT_DISCOVERY_DURATION);

        NFA_SetRfDiscoveryDuration(discovery_duration);
#endif
        // get LF_T3T_MAX
        {
          SyncEventGuard guard(sNfaGetConfigEvent);
          tNFA_PMID configParam[1] = {NCI_PARAM_ID_LF_T3T_MAX};
          stat = NFA_GetConfig(1, configParam);
          if (stat == NFA_STATUS_OK) {
            sNfaGetConfigEvent.wait();
            if (sCurrentConfigLen >= 4 ||
                sConfig[1] == NCI_PARAM_ID_LF_T3T_MAX) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: lfT3tMax=%d", __func__, sConfig[3]);
              sLfT3tMax = sConfig[3];
            }
          }
        }

        prevScreenState = NFA_SCREEN_STATE_OFF_LOCKED;

        // Do custom NFCA startup configuration.
        doStartupConfig();
        goto TheEnd;
      }
    }

    LOG(ERROR) << StringPrintf("%s: fail nfa enable; error=0x%X", __func__,
                               stat);

    if (sIsNfaEnabled) {
      EXTNS_Close();
      stat = NFA_Disable(FALSE /* ungraceful */);
    }

    theInstance.Finalize();
  }

TheEnd:
#ifndef LINUX
  if (sIsNfaEnabled)
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return sIsNfaEnabled ? JNI_TRUE : JNI_FALSE;

}

static void nfcManager_doEnableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = true;
}

static void nfcManager_doDisableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = false;
}

static void nfcManager_doFactoryReset(JNIEnv*, jobject) {
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.FactoryReset();
}

static void nfcManager_doShutdown(JNIEnv*, jobject) {
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.DeviceShutdown();
}

static void nfcManager_configNfccConfigControl(bool flag) {
    // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
    if (NFC_GetNCIVersion() != NCI_VERSION_1_0) {
        uint8_t nfa_set_config[] = { 0x00 };

        nfa_set_config[0] = (flag == true ? 1 : 0);

        tNFA_STATUS status = NFA_SetConfig(NCI_PARAM_ID_NFCC_CONFIG_CONTROL,
                                           sizeof(nfa_set_config),
                                           &nfa_set_config[0]);
        if (status != NFA_STATUS_OK) {
            LOG(ERROR) << __func__
            << ": Failed to configure NFCC_CONFIG_CONTROL";
        }
    }
}

/*******************************************************************************
**
** Function:        nfcManager_enableDiscovery
**
** Description:     Start polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**                  technologies_mask: the bitmask of technologies for which to
*enable discovery
**                  enable_lptd: whether to enable low power polling (default:
*false)
**
** Returns:         None
**
*******************************************************************************/
#ifdef LINUX
INT32 nfcManager_enableDiscovery(INT32 technologies_mask,
                                       BOOLEAN enable_lptd,
                                       BOOLEAN reader_mode,
                                       INT32 enable_host_routing,
                                       BOOLEAN enable_p2p, BOOLEAN restart) {

#else
static void nfcManager_enableDiscovery(JNIEnv* e, jobject o,
                                       jint technologies_mask,
                                       jboolean enable_lptd,
                                       jboolean reader_mode,
                                       jboolean enable_host_routing,
                                       jboolean enable_p2p, jboolean restart) {
#endif
  tNFA_TECHNOLOGY_MASK tech_mask = DEFAULT_TECH_MASK;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; tech_mask = %02x", __func__, tech_mask);
#ifdef LINUX
  if (technologies_mask == DEFAULT_NFA_TECH_MASK)
  {
      tech_mask = (tNFA_TECHNOLOGY_MASK)sTech_mask;
  }
  else
  {
      tech_mask = sTech_mask = (tNFA_TECHNOLOGY_MASK) technologies_mask;
  }
  NXPLOG_API_D ("%s: enter; tech_mask = %02x", __FUNCTION__, tech_mask);
#else
  struct nfc_jni_native_data* nat = getNative(e, o);

  if (technologies_mask == -1 && nat)
    tech_mask = (tNFA_TECHNOLOGY_MASK)nat->tech_mask;
  else if (technologies_mask != -1)
    tech_mask = (tNFA_TECHNOLOGY_MASK)technologies_mask;
#endif
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; tech_mask = %02x", __func__, tech_mask);

  if (sDiscoveryEnabled && !restart) {
    LOG(ERROR) << StringPrintf("%s: already discovering", __func__);
#ifdef LINUX
    return NFA_STATUS_FAILED;
#else
    return;
#endif
  }

#ifndef LINUX
  PowerSwitch::getInstance().setLevel(PowerSwitch::FULL_POWER);
#endif
  if (sRfEnabled) {
    // Stop RF discovery to reconfigure
    startRfDiscovery(false);
  }

  // Check polling configuration
  if (tech_mask != 0) {
    stopPolling_rfDiscoveryDisabled();
    startPolling_rfDiscoveryDisabled(tech_mask);

    // Start P2P listening if tag polling was enabled
    if (sPollingEnabled) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Enable p2pListening", __func__);
      if (enable_p2p && !sP2pEnabled) {
        sP2pEnabled = true;
        PeerToPeer::getInstance().enableP2pListening(true);
        NFA_ResumeP2p();
      } else if (!enable_p2p && sP2pEnabled) {
        sP2pEnabled = false;
        PeerToPeer::getInstance().enableP2pListening(false);
        NFA_PauseP2p();
      }
      if (reader_mode && !sReaderModeEnabled) {
        sReaderModeEnabled = true;
        NFA_DisableListening();

        // configure NFCC_CONFIG_CONTROL- NFCC not allowed to manage RF configuration.
        nfcManager_configNfccConfigControl(false);

        NFA_SetRfDiscoveryDuration(READER_MODE_DISCOVERY_DURATION);
      } else if (!reader_mode && sReaderModeEnabled) {
#ifndef LINUX
        struct nfc_jni_native_data* nat = getNative(e, o);
#endif
        sReaderModeEnabled = false;
        NFA_EnableListening();

        // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
        nfcManager_configNfccConfigControl(true);

#ifdef LINUX
        INT32 discovery_duration = 0;
        INT32 num = 0;
        if (GetNumValue(NAME_NFA_DM_DISC_DURATION_POLL, &num, sizeof(num)))
        {
            discovery_duration = num;
        }
        else
        {
            discovery_duration = DEFAULT_DISCOVERY_DURATION;
        }

        NFA_SetRfDiscoveryDuration(discovery_duration);
#else
        NFA_SetRfDiscoveryDuration(nat->discovery_duration);
#endif
      }
    }
  } else {
    // No technologies configured, stop polling
    stopPolling_rfDiscoveryDisabled();
  }

#ifdef LINUX
    if(!addT4TNfceeAid()) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: addT4TNfceeAid Failed", __func__);
    }
#endif
  // Check listen configuration
  if (enable_host_routing) {
    RoutingManager::getInstance().enableRoutingToHost();
    RoutingManager::getInstance().commitRouting();
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: nfcManager_enableDiscovery debug31", __func__);
  } else {
    RoutingManager::getInstance().disableRoutingToHost();
    RoutingManager::getInstance().commitRouting();
  }
  // Actually start discovery.
  startRfDiscovery(true);
  sDiscoveryEnabled = true;
#ifndef LINUX
  PowerSwitch::getInstance().setModeOn(PowerSwitch::DISCOVERY);
#endif

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return NFA_STATUS_OK;
}

/*******************************************************************************
**
** Function:        nfcManager_disableDiscovery
**
** Description:     Stop polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None
**
*******************************************************************************/
#ifdef LINUX
INT32 nfcManager_disableDiscovery() {
#else
void nfcManager_disableDiscovery(JNIEnv* e, jobject o) {
#endif
  tNFA_STATUS status = NFA_STATUS_OK;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  if (sDiscoveryEnabled == false) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already disabled", __func__);
    goto TheEnd;
  }

  // Stop RF Discovery.
  startRfDiscovery(false);

  if (sPollingEnabled) status = stopPolling_rfDiscoveryDisabled();

  PeerToPeer::getInstance().enableP2pListening(false);
  sP2pEnabled = false;
  sDiscoveryEnabled = false;
#ifndef LINUX
  // if nothing is active after this, then tell the controller to power down
  if (!PowerSwitch::getInstance().setModeOff(PowerSwitch::DISCOVERY))
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
#endif
TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
#ifdef LINUX
  return status;
#endif
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpServiceSocket
**
** Description:     Create a new LLCP server socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpServiceSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpServiceSocket(JNIEnv* e, jobject,
                                                    jint nSap, jstring sn,
                                                    jint miu, jint rw,
                                                    jint linearBufferLength) {
  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();

  ScopedUtfChars serviceName(e, sn);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter: sap=%i; name=%s; miu=%i; rw=%i; buffLen=%i", __func__, nSap,
      serviceName.c_str(), miu, rw, linearBufferLength);

  /* Create new NativeLlcpServiceSocket object */
  jobject serviceSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpServiceSocketClassName,
                                 &(serviceSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: Llcp socket object creation error",
                               __func__);
    return NULL;
  }

  /* Get NativeLlcpServiceSocket class object */
  ScopedLocalRef<jclass> clsNativeLlcpServiceSocket(
      e, e->GetObjectClass(serviceSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: Llcp Socket get object class error",
                               __func__);
    return NULL;
  }

  if (!PeerToPeer::getInstance().registerServer(jniHandle,
                                                serviceName.c_str())) {
    LOG(ERROR) << StringPrintf("%s: RegisterServer error", __func__);
    return NULL;
  }

  jfieldID f;

  /* Set socket handle to be the same as the NfaHandle*/
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mHandle", "I");
  e->SetIntField(serviceSocket, f, (jint)jniHandle);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: socket Handle = 0x%X", __func__, jniHandle);

  /* Set socket linear buffer length */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(),
                    "mLocalLinearBufferLength", "I");
  e->SetIntField(serviceSocket, f, (jint)linearBufferLength);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: buffer length = %d", __func__, linearBufferLength);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalMiu", "I");
  e->SetIntField(serviceSocket, f, (jint)miu);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: MIU = %d", __func__, miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalRw", "I");
  e->SetIntField(serviceSocket, f, (jint)rw);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s:  RW = %d", __func__, rw);

  sLastError = 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return serviceSocket;
}
#endif
/*******************************************************************************
**
** Function:        nfcManager_doGetLastError
**
** Description:     Get the last error code.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Last error code.
**
*******************************************************************************/
static jint nfcManager_doGetLastError(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: last error=%i", __func__, sLastError);
  return sLastError;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeinitialize
**
** Description:     Turn off NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
#ifdef LINUX
INT32 nfcManager_doDeinitialize() {
#else
INT32 nfcManager_doDeinitialize(JNIEnv*, jobject) {
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  sIsDisabling = true;

  RoutingManager::getInstance().onNfccShutdown();
#ifndef LINUX
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  HciEventManager::getInstance().finalize();
#endif
  if (sIsNfaEnabled) {
    SyncEventGuard guard(sNfaDisableEvent);
    EXTNS_Close();
    tNFA_STATUS stat = NFA_Disable(TRUE /* graceful */);
    if (stat == NFA_STATUS_OK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: wait for completion", __func__);
      sNfaDisableEvent.wait();  // wait for NFA command to finish
#ifndef LINUX
      PeerToPeer::getInstance().handleNfcOnOff(false);
#endif
    } else {
      LOG(ERROR) << StringPrintf("%s: fail disable; error=0x%X", __func__,
                                 stat);
    }
  }
  nativeNfcTag_abortWaits();
  NfcTag::getInstance().abort();
  sAbortConnlessWait = true;
#ifndef LINUX
  nativeLlcpConnectionlessSocket_abortWait();
#endif
  sIsNfaEnabled = false;
  sDiscoveryEnabled = false;
  sPollingEnabled = false;
  sIsDisabling = false;
  sP2pEnabled = false;
  gActivated = false;
  sLfT3tMax = 0;

  {
    // unblock NFA_EnablePolling() and NFA_DisablePolling()
    SyncEventGuard guard(sNfaEnableDisablePollingEvent);
    sNfaEnableDisablePollingEvent.notifyOne();
  }

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Finalize();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
#ifdef LINUX
  return (INT32)NFA_STATUS_OK;
#else
  return JNI_TRUE;
#endif
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpSocket
**
** Description:     Create a LLCP connection-oriented socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpSocket(JNIEnv* e, jobject, jint nSap,
                                             jint miu, jint rw,
                                             jint linearBufferLength) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; sap=%d; miu=%d; rw=%d; buffer len=%d",
                      __func__, nSap, miu, rw, linearBufferLength);

  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();
  PeerToPeer::getInstance().createClient(jniHandle, miu, rw);

  /* Create new NativeLlcpSocket object */
  jobject clientSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpSocketClassName,
                                 &(clientSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail Llcp socket creation", __func__);
    return clientSocket;
  }

  /* Get NativeConnectionless class object */
  ScopedLocalRef<jclass> clsNativeLlcpSocket(e,
                                             e->GetObjectClass(clientSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: fail get class object", __func__);
    return clientSocket;
  }

  jfieldID f;

  /* Set socket SAP */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mSap", "I");
  e->SetIntField(clientSocket, f, (jint)nSap);

  /* Set socket handle */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mHandle", "I");
  e->SetIntField(clientSocket, f, (jint)jniHandle);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalMiu", "I");
  e->SetIntField(clientSocket, f, (jint)miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalRw", "I");
  e->SetIntField(clientSocket, f, (jint)rw);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return clientSocket;
}

/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpConnectionlessSocket
**
** Description:     Create a connection-less socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name.
**
** Returns:         NativeLlcpConnectionlessSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpConnectionlessSocket(JNIEnv*, jobject,
                                                           jint nSap,
                                                           jstring /*sn*/) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: nSap=0x%X", __func__, nSap);
  return NULL;
}
#endif
/*******************************************************************************
**
** Function:        isPeerToPeer
**
** Description:     Whether the activation data indicates the peer supports
*NFC-DEP.
**                  activated: Activation data.
**
** Returns:         True if the peer supports NFC-DEP.
**
*******************************************************************************/
static bool isPeerToPeer(tNFA_ACTIVATED& activated) {
  return activated.activate_ntf.protocol == NFA_PROTOCOL_NFC_DEP;
}

/*******************************************************************************
**
** Function:        isListenMode
**
** Description:     Indicates whether the activation data indicates it is
**                  listen mode.
**
** Returns:         True if this listen mode.
**
*******************************************************************************/
static bool isListenMode(tNFA_ACTIVATED& activated) {
  return (
      (NFC_DISCOVERY_TYPE_LISTEN_A ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_B ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_F ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_ISO15693 ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_B_PRIME ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_INTERFACE_EE_DIRECT_RF == activated.activate_ntf.intf_param.type));
}

/*******************************************************************************
**
** Function:        nfcManager_doCheckLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean nfcManager_doCheckLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doActivateLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean nfcManager_doActivateLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doAbort
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doAbort(JNIEnv* e, jobject, jstring msg) {
#ifndef LINUX
  ScopedUtfChars message = {e, msg};
  e->FatalError(message.c_str());
#endif
  abort();  // <-- Unreachable
}

/*******************************************************************************
**
** Function:        nfcManager_doDownload
**
** Description:     Download firmware patch files.  Do not turn on NFC.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_doDownload(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  bool result = JNI_FALSE;
  theInstance.Initialize();  // start GKI, NCI task, NFC task
  result = theInstance.DownloadFirmware();
  theInstance.Finalize();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return result;
}

/*******************************************************************************
**
** Function:        nfcManager_doResetTimeouts
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doResetTimeouts(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  NfcTag::getInstance().resetAllTransceiveTimeouts();
}

/*******************************************************************************
**
** Function:        nfcManager_doSetTimeout
**
** Description:     Set timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**                  timeout: Timeout value.
**
** Returns:         True if ok.
**
*******************************************************************************/
static bool nfcManager_doSetTimeout(JNIEnv*, jobject, jint tech, jint timeout) {
  if (timeout <= 0) {
    LOG(ERROR) << StringPrintf("%s: Timeout must be positive.", __func__);
    return false;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  NfcTag::getInstance().setTransceiveTimeout(tech, timeout);
  return true;
}

/*******************************************************************************
**
** Function:        nfcManager_doGetTimeout
**
** Description:     Get timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**
** Returns:         Timeout value.
**
*******************************************************************************/
static jint nfcManager_doGetTimeout(JNIEnv*, jobject, jint tech) {
  int timeout = NfcTag::getInstance().getTransceiveTimeout(tech);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  return timeout;
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        nfcManager_doDump
**
** Description:     Get libnfc-nci dump
**                  e: JVM environment.
**                  obj: Java object.
**                  fdobj: File descriptor to be used
**
** Returns:         Void
**
*******************************************************************************/
static void nfcManager_doDump(JNIEnv* e, jobject obj, jobject fdobj) {
  int fd = jniGetFDFromFileDescriptor(e, fdobj);
  if (fd < 0) return;

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Dump(fd);
}
#endif
static jint nfcManager_doGetNciVersion(JNIEnv*, jobject) {
  return NFC_GetNCIVersion();
}
#ifndef LINUX
static void nfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                        jint screen_state_mask) {
  tNFA_STATUS status = NFA_STATUS_OK;
  uint8_t state = (screen_state_mask & NFA_SCREEN_STATE_MASK);
  uint8_t discovry_param =
      NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: state = %d prevScreenState= %d, discovry_param = %d",
                      __FUNCTION__, state, prevScreenState, discovry_param);

  if (prevScreenState == state) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "New screen state is same as previous state. No action taken");
    return;
  }

  if (sIsDisabling || !sIsNfaEnabled ||
      (NFC_GetNCIVersion() != NCI_VERSION_2_0)) {
    prevScreenState = state;
    return;
  }
  if (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
      prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED ||
      prevScreenState == NFA_SCREEN_STATE_ON_LOCKED) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
      return;
    } else {
      sNfaSetPowerSubState.wait();
    }
  }

  if (state == NFA_SCREEN_STATE_OFF_LOCKED ||
      state == NFA_SCREEN_STATE_OFF_UNLOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
  }

  if (state == NFA_SCREEN_STATE_ON_LOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        (screen_state_mask & NFA_SCREEN_POLLING_TAG_MASK)
            ? (NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK)
            : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
  }

  if (state == NFA_SCREEN_STATE_ON_UNLOCKED) {
    // enable both poll and listen on DH 0x01
    discovry_param =
        NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;
  }

  SyncEventGuard guard(sNfaSetConfigEvent);
  status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
                         NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovry_param);
  if (status == NFA_STATUS_OK) {
    sNfaSetConfigEvent.wait();
  } else {
    LOG(ERROR) << StringPrintf("%s: Failed to update CON_DISCOVER_PARAM",
                               __FUNCTION__);
    return;
  }

  if (prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
    } else {
      sNfaSetPowerSubState.wait();
    }
  }
  if ((state == NFA_SCREEN_STATE_OFF_LOCKED ||
       state == NFA_SCREEN_STATE_OFF_UNLOCKED) &&
      prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED && (!sP2pActive) &&
      (!sSeRfActive)) {
    // screen turns off, disconnect tag if connected
    nativeNfcTag_doDisconnect(NULL, NULL);
  }

  prevScreenState = state;
}

/*******************************************************************************
**
** Function:        nfcManager_doSetP2pInitiatorModes
**
** Description:     Set P2P initiator's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.  The values are
*specified
**                          in external/libnfc-nxp/inc/phNfcTypes.h.  See
**                          enum phNfc_eP2PMode_t.
**
** Returns:         None.
**
*******************************************************************************/
static void nfcManager_doSetP2pInitiatorModes(JNIEnv* e, jobject o,
                                              jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  struct nfc_jni_native_data* nat = getNative(e, o);

  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08) mask |= NFA_TECHNOLOGY_MASK_A_ACTIVE;
  if (modes & 0x10) mask |= NFA_TECHNOLOGY_MASK_F_ACTIVE;
  if (modes & 0x20) mask |= NFA_TECHNOLOGY_MASK_F_ACTIVE;
  nat->tech_mask = mask;
}

/*******************************************************************************
**
** Function:        nfcManager_doSetP2pTargetModes
**
** Description:     Set P2P target's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.
**
** Returns:         None.
**
*******************************************************************************/
static void nfcManager_doSetP2pTargetModes(JNIEnv*, jobject, jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  // Map in the right modes
  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08)
    mask |= NFA_TECHNOLOGY_MASK_A_ACTIVE | NFA_TECHNOLOGY_MASK_F_ACTIVE;

  PeerToPeer::getInstance().setP2pListenMask(mask);
}

static void nfcManager_doEnableScreenOffSuspend(JNIEnv* e, jobject o) {
#ifndef LINUX
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_FULL);
#endif
}

static void nfcManager_doDisableScreenOffSuspend(JNIEnv* e, jobject o) {
#ifndef LINUX
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_OFF);
#endif
}
#endif
/*******************************************************************************
**
** Function:        nfcManager_getIsoDepMaxTransceiveLength
**
** Description:     Get maximum ISO DEP Transceive Length supported by the NFC
**                  chip. Returns default 261 bytes if the property is not set.
**
** Returns:         max value.
**
*******************************************************************************/
static jint nfcManager_getIsoDepMaxTransceiveLength(JNIEnv*, jobject) {
  /* Check if extended APDU is supported by the chip.
   * If not, default value is returned.
   * The maximum length of a default IsoDep frame consists of:
   * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
   */
  return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE, 261);
}

/*******************************************************************************
 **
 ** Function:        nfcManager_getAidTableSize
 ** Description:     Get the maximum supported size for AID routing table.
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **
 *******************************************************************************/
static jint nfcManager_getAidTableSize(JNIEnv*, jobject) {
  return NFA_GetAidTableSize();
}

/*******************************************************************************
**
** Function:        nfcManager_getT4TNfceePowerState
**
** Description:     Get the T4T Nfcee power state supported.
**                  e: JVM environment.
**                  o: Java object.
**                  mode: Not used.
**
** Returns:         None
**
*******************************************************************************/
#ifndef LINUX
static jint nfcManager_getT4TNfceePowerState(JNIEnv* e, jobject o) {
#else
static jint nfcManager_getT4TNfceePowerState() {
#endif
  RoutingManager& routingManager = RoutingManager::getInstance();
  int defaultPowerState = ~(routingManager.PWR_SWTCH_OFF_MASK |
          routingManager.PWR_BATT_OFF_MASK);

  return NfcConfig::getUnsigned(NAME_DEFAULT_T4TNFCEE_AID_POWER_STATE,
          defaultPowerState);
}

static jboolean nfcManager_doSetNfcSecure(JNIEnv* e, jobject o,
                                          jboolean enable) {
  RoutingManager& routingManager = RoutingManager::getInstance();
  routingManager.setNfcSecure(enable);
  bool rfEnabled = sRfEnabled;
  if (sRoutingInitialized) {
    routingManager.disableRoutingToHost();
    if (rfEnabled) startRfDiscovery(false);
    routingManager.updateRoutingTable();
    routingManager.enableRoutingToHost();
    routingManager.commitRouting();
    if (rfEnabled) startRfDiscovery(true);
  }
  return true;
}

#ifndef LINUX
static jstring nfcManager_doGetNfaStorageDir(JNIEnv* e, jobject o) {
  string nfaStorageDir = NfcConfig::getString(NAME_NFA_STORAGE, "/data/nfc");
  return e->NewStringUTF(nfaStorageDir.c_str());
}
/*****************************************************************************
**
** JNI functions for android-4.0.1_r1
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doDownload", "()Z", (void*)nfcManager_doDownload},

    {"initializeNativeStructure", "()Z", (void*)nfcManager_initNativeStruc},

    {"doInitialize", "()Z", (void*)nfcManager_doInitialize},

    {"doDeinitialize", "()Z", (void*)nfcManager_doDeinitialize},

    {"sendRawFrame", "([B)Z", (void*)nfcManager_sendRawFrame},

    {"routeAid", "([BIII)Z", (void*)nfcManager_routeAid},

    {"unrouteAid", "([B)Z", (void*)nfcManager_unrouteAid},

    {"commitRouting", "()Z", (void*)nfcManager_commitRouting},

    {"doRegisterT3tIdentifier", "([B)I",
     (void*)nfcManager_doRegisterT3tIdentifier},

    {"doDeregisterT3tIdentifier", "(I)V",
     (void*)nfcManager_doDeregisterT3tIdentifier},

    {"getLfT3tMax", "()I", (void*)nfcManager_getLfT3tMax},

    {"doEnableDiscovery", "(IZZZZZ)V", (void*)nfcManager_enableDiscovery},

    {"doCheckLlcp", "()Z", (void*)nfcManager_doCheckLlcp},

    {"doActivateLlcp", "()Z", (void*)nfcManager_doActivateLlcp},

    {"doCreateLlcpConnectionlessSocket",
     "(ILjava/lang/String;)Lcom/android/nfc/dhimpl/"
     "NativeLlcpConnectionlessSocket;",
     (void*)nfcManager_doCreateLlcpConnectionlessSocket},

    {"doCreateLlcpServiceSocket",
     "(ILjava/lang/String;III)Lcom/android/nfc/dhimpl/NativeLlcpServiceSocket;",
     (void*)nfcManager_doCreateLlcpServiceSocket},

    {"doCreateLlcpSocket", "(IIII)Lcom/android/nfc/dhimpl/NativeLlcpSocket;",
     (void*)nfcManager_doCreateLlcpSocket},

    {"doGetLastError", "()I", (void*)nfcManager_doGetLastError},

    {"disableDiscovery", "()V", (void*)nfcManager_disableDiscovery},

    {"doSetTimeout", "(II)Z", (void*)nfcManager_doSetTimeout},

    {"doGetTimeout", "(I)I", (void*)nfcManager_doGetTimeout},

    {"doResetTimeouts", "()V", (void*)nfcManager_doResetTimeouts},

    {"doAbort", "(Ljava/lang/String;)V", (void*)nfcManager_doAbort},

    {"doSetP2pInitiatorModes", "(I)V",
     (void*)nfcManager_doSetP2pInitiatorModes},

    {"doSetP2pTargetModes", "(I)V", (void*)nfcManager_doSetP2pTargetModes},

    {"doEnableScreenOffSuspend", "()V",
     (void*)nfcManager_doEnableScreenOffSuspend},

    {"doSetScreenState", "(I)V", (void*)nfcManager_doSetScreenState},

    {"doDisableScreenOffSuspend", "()V",
     (void*)nfcManager_doDisableScreenOffSuspend},

    {"doDump", "(Ljava/io/FileDescriptor;)V", (void*)nfcManager_doDump},

    {"getNciVersion", "()I", (void*)nfcManager_doGetNciVersion},
    {"doEnableDtaMode", "()V", (void*)nfcManager_doEnableDtaMode},
    {"doDisableDtaMode", "()V", (void*)nfcManager_doDisableDtaMode},
    {"doFactoryReset", "()V", (void*)nfcManager_doFactoryReset},
    {"doShutdown", "()V", (void*)nfcManager_doShutdown},

    {"getIsoDepMaxTransceiveLength", "()I",
     (void*)nfcManager_getIsoDepMaxTransceiveLength},

    {"getAidTableSize", "()I", (void*)nfcManager_getAidTableSize},

    {"doSetNfcSecure", "(Z)Z", (void*)nfcManager_doSetNfcSecure},

    {"getNfaStorageDir", "()Ljava/lang/String;",
     (void*)nfcManager_doGetNfaStorageDir},
    {"getT4TNfceePowerState", "()I",
            (void*) nfcManager_getT4TNfceePowerState},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeNfcManager
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeNfcManager(JNIEnv* e) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return jniRegisterNativeMethods(e, gNativeNfcManagerClassName, gMethods,
                                  NELEM(gMethods));
}
#endif
/*******************************************************************************
**
** Function:        startRfDiscovery
**
** Description:     Ask stack to start polling and listening for devices.
**                  isStart: Whether to start.
**
** Returns:         None
**
*******************************************************************************/
#ifdef LINUX
void startRfDiscovery(BOOLEAN isStart) {
#else
void startRfDiscovery(bool isStart) {
#endif
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: is start=%d", __func__, isStart);
  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  status = isStart ? NFA_StartRfDiscovery() : NFA_StopRfDiscovery();
  if (status == NFA_STATUS_OK) {
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_RF_DISCOVERY_xxxx_EVT
    sRfEnabled = isStart;
  } else {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to start/stop RF discovery; error=0x%X", __func__, status);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();
}

/*******************************************************************************
**
** Function:        isDiscoveryStarted
**
** Description:     Indicates whether the discovery is started.
**
** Returns:         True if discovery is started
**
*******************************************************************************/
bool isDiscoveryStarted() { return sRfEnabled; }

/*******************************************************************************
**
** Function:        doStartupConfig
**
** Description:     Configure the NFC controller.
**
** Returns:         None
**
*******************************************************************************/
void doStartupConfig() {
  // configure RF polling frequency for each technology
  static tNFA_DM_DISC_FREQ_CFG nfa_dm_disc_freq_cfg;
  // values in the polling_frequency[] map to members of nfa_dm_disc_freq_cfg
  std::vector<uint8_t> polling_frequency;
  if (NfcConfig::hasKey(NAME_POLL_FREQUENCY))
    polling_frequency = NfcConfig::getBytes(NAME_POLL_FREQUENCY);
  if (polling_frequency.size() == 8) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: polling frequency", __func__);
    memset(&nfa_dm_disc_freq_cfg, 0, sizeof(nfa_dm_disc_freq_cfg));
    nfa_dm_disc_freq_cfg.pa = polling_frequency[0];
    nfa_dm_disc_freq_cfg.pb = polling_frequency[1];
    nfa_dm_disc_freq_cfg.pf = polling_frequency[2];
    nfa_dm_disc_freq_cfg.pi93 = polling_frequency[3];
    nfa_dm_disc_freq_cfg.pbp = polling_frequency[4];
    nfa_dm_disc_freq_cfg.pk = polling_frequency[5];
    nfa_dm_disc_freq_cfg.paa = polling_frequency[6];
    nfa_dm_disc_freq_cfg.pfa = polling_frequency[7];
    p_nfa_dm_rf_disc_freq_cfg = &nfa_dm_disc_freq_cfg;
  }

  // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
  nfcManager_configNfccConfigControl(true);

}

/*******************************************************************************
**
** Function:        nfcManager_isNfcActive
**
** Description:     Used externaly to determine if NFC is active or not.
**
** Returns:         'true' if the NFC stack is running, else 'false'.
**
*******************************************************************************/
#ifdef LINUX
BOOLEAN nfcManager_isNfcActive() { return sIsNfaEnabled; }
#else
bool nfcManager_isNfcActive() { return sIsNfaEnabled; }
#endif
/*******************************************************************************
**
** Function:        startStopPolling
**
** Description:     Start or stop polling.
**                  isStartPolling: true to start polling; false to stop
*polling.
**
** Returns:         None.
**
*******************************************************************************/
void startStopPolling(bool isStartPolling) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; isStart=%u", __func__, isStartPolling);
  startRfDiscovery(false);

  if (isStartPolling)
    startPolling_rfDiscoveryDisabled(0);
  else
    stopPolling_rfDiscoveryDisabled();

  startRfDiscovery(true);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask) {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  if (tech_mask == 0)
    tech_mask =
        NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enable polling", __func__);
  stat = NFA_EnablePolling(tech_mask);
  if (stat == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: wait for enable event", __func__);
    sPollingEnabled = true;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_ENABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail enable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}

static tNFA_STATUS stopPolling_rfDiscoveryDisabled() {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: disable polling", __func__);
  stat = NFA_DisablePolling();
  if (stat == NFA_STATUS_OK) {
    sPollingEnabled = false;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_DISABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail disable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}
/*******************************************************************************
 **
 ** Function:        getConfig
 **
 ** Description:     read the config values from NFC controller.
 **
 ** Returns:         SUCCESS/FAILURE
 **
 *******************************************************************************/
tNFA_STATUS getConfig(uint16_t* rspLen, uint8_t* configValue, uint8_t numParam,
                      tNFA_PMID* param) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  if (rspLen == NULL || configValue == NULL || param == NULL)
    return NFA_STATUS_FAILED;
  SyncEventGuard guard(sNfaGetConfigEvent);
  status = NFA_GetConfig(numParam, param);
  if (status == NFA_STATUS_OK) {
    if (sNfaGetConfigEvent.wait(2000) == false) {
      *rspLen = 0;
    } else {
      *rspLen = sCurrentConfigLen;
      memcpy(configValue, sConfig, sCurrentConfigLen);
    }
  } else {
    *rspLen = 0;
  }
  return status;
}
#ifdef LINUX
int nfcManager_selectNextTag()
{
    int status = NFA_STATUS_FAILED;
    if(NfcTag::getInstance ().mNumTags > 1)
    {
        NXPLOG_API_W("%s: Deactivating Selected Tag to Select Next ", __FUNCTION__
                        , NfcTag::getInstance ().mNumTags);
        sSelectNext = true;
        NFA_Deactivate (TRUE);
        status = NFA_STATUS_OK;
    }
    return status;
}
void nfcManager_registerTagCallback(nfcTagCallback_t *nfcTagCb)
{
    gTagCallback = nfcTagCb;
}
void nfcManager_deregisterTagCallback()
{
    gTagCallback = NULL;
}
void nfcManager_registerHostCallback(nfcHostCardEmulationCallback_t *callback)
{
    RoutingManager::getInstance().registerHostCallback(callback);
}
void nfcManager_deregisterHostCallback()
{
    RoutingManager::getInstance().deregisterHostCallback();
}

int nfcManager_getNumTags()
{
    return NfcTag::getInstance ().mNumTags;
}

int nativeNfcManager_checkNextProtocol()
{
    return NfcTag::getInstance ().checkNextValidProtocol();
}

#ifdef LINUX
INT32 nativeNfcManager_setConfig(UINT8 id, UINT8 length, UINT8 *p_data)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_D ("%s: Nfc not initialized.", __FUNCTION__);
        goto End;
    }
    status = NFA_SetConfig(id, length, p_data);
End:
    gSyncMutex.unlock();
    return status;
}

bool addT4TNfceeAid() {
  uint8_t stdT4tAid[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
  uint8_t t4tNdefAidLen = sizeof(stdT4tAid);
  int t4tNfceeRouteId = 0x10;
  int aidInfo = 0x02;
  int power = nfcManager_getT4TNfceePowerState();
  return RoutingManager::getInstance().addAidRouting(stdT4tAid, t4tNdefAidLen, t4tNfceeRouteId, aidInfo, power);
}
#endif

/*******************************************************************************
**
** Function         sig_handler
**
** Description      This function is used to handle the different types of
**                  signal events.
**
** Returns          None
**
*******************************************************************************/
void sig_handler(int signo)
{
    switch (signo)
    {
        case SIGINT:
            NXPLOG_API_D("received SIGINT\n");
            break;
        case SIGABRT:
            NXPLOG_API_D("received SIGABRT\n");
            NFA_HciW4eSETransaction_Complete(Wait);
            break;
        case SIGSEGV:
            NXPLOG_API_D("received SIGSEGV\n");
            break;
        case SIGHUP:
            NXPLOG_API_D("received SIGHUP\n");
            break;
    }
}

#endif

#ifndef LINUX
} /* namespace android */
#endif
