/*
 * Copyright (C) 2013 The Android Open Source Project
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
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright 2021 NXP
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
/*
 *  Manage the listen-mode routing table.
 */
#pragma once
#include <vector>
#include "NfcJniUtil.h"
#include "RouteDataSet.h"
#include "SyncEvent.h"

#include <map>
#include "nfa_api.h"
#include "nfa_ee_api.h"
#include <iostream>
#ifdef LINUX
#include "linux_nfc_api.h"
#include "nativeNfcManager.h"
#endif

//using namespace std;

class RoutingManager {
 public:
  static RoutingManager& getInstance();
  bool initialize(nfc_jni_native_data* native);
  void deinitialize();
  void enableRoutingToHost();
  void disableRoutingToHost();
#ifdef LINUX
  void registerHostCallback(nfcHostCardEmulationCallback_t *callback);
  void deregisterHostCallback();
#endif
  bool addAidRouting(const uint8_t* aid, uint8_t aidLen, int route,
                     int aidInfo, int power);
  bool removeAidRouting(const uint8_t* aid, uint8_t aidLen);
  bool commitRouting();
  int registerT3tIdentifier(uint8_t* t3tId, uint8_t t3tIdLen);
  void deregisterT3tIdentifier(int handle);
  void onNfccShutdown();
  int registerJniFunctions(JNIEnv* e);
  bool setNfcSecure(bool enable);
  void updateRoutingTable();
  static const int PWR_SWTCH_OFF_MASK = 0x02;
  static const int PWR_BATT_OFF_MASK = 0x04;
 private:
  RoutingManager();
  ~RoutingManager();
  RoutingManager(const RoutingManager&);
  RoutingManager& operator=(const RoutingManager&);

  void handleData(uint8_t technology, const uint8_t* data, uint32_t dataLen,
                  tNFA_STATUS status);
  void notifyActivated(uint8_t technology);
  void notifyDeactivated(uint8_t technology);
  void notifyEeUpdated();
  tNFA_TECHNOLOGY_MASK updateEeTechRouteSetting();
  void updateDefaultProtocolRoute();
  void updateDefaultRoute();

  // See AidRoutingManager.java for corresponding
  // AID_MATCHING_ constants

  // Every routing table entry is matched exact (BCM20793)
  static const int AID_MATCHING_EXACT_ONLY = 0x00;
  // Every routing table entry can be matched either exact or prefix
  static const int AID_MATCHING_EXACT_OR_PREFIX = 0x01;
  // Every routing table entry is matched as a prefix
  static const int AID_MATCHING_PREFIX_ONLY = 0x02;

  static void nfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);
  static void stackCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
  static void nfcFCeCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);

  static int com_android_nfc_cardemulation_doGetDefaultRouteDestination(
      JNIEnv* e);
  static int com_android_nfc_cardemulation_doGetDefaultOffHostRouteDestination(
      JNIEnv* e);
  static jbyteArray com_android_nfc_cardemulation_doGetOffHostUiccDestination(
      JNIEnv* e);
  static jbyteArray com_android_nfc_cardemulation_doGetOffHostEseDestination(
      JNIEnv* e);
  static int com_android_nfc_cardemulation_doGetAidMatchingMode(JNIEnv* e);
  static int com_android_nfc_cardemulation_doGetDefaultIsoDepRouteDestination(
      JNIEnv* e);

  std::vector<uint8_t> mRxDataBuffer;
  map<int, uint16_t> mMapScbrHandle;
  bool mSecureNfcEnabled;

  // Fields below are final after initialize()
  nfc_jni_native_data* mNativeData;
  int mDefaultOffHostRoute;
  vector<uint8_t> mOffHostRouteUicc;
  vector<uint8_t> mOffHostRouteEse;
  int mDefaultFelicaRoute;
  int mDefaultEe;
  int mDefaultIsoDepRoute;
  int mAidMatchingMode;
  int mNfcFOnDhHandle;
  bool mIsScbrSupported;
  uint16_t mDefaultSysCode;
  uint16_t mDefaultSysCodeRoute;
  uint8_t mDefaultSysCodePowerstate;
  uint8_t mOffHostAidRoutingPowerState;
  uint8_t mHostListenTechMask;
  bool mDeinitializing;
  bool mEeInfoChanged;
  bool mReceivedEeInfo;
  bool mAidRoutingConfigured;
  tNFA_EE_CBACK_DATA mCbEventData;
  tNFA_EE_DISCOVER_REQ mEeInfo;
  tNFA_TECHNOLOGY_MASK mSeTechMask;
#ifndef LINUX
  static const JNINativeMethod sMethods[];
#else
   nfcHostCardEmulationCallback_t *mCallback;
#endif
  SyncEvent mEeRegisterEvent;
  SyncEvent mRoutingEvent;
  SyncEvent mEeUpdateEvent;
  SyncEvent mEeInfoEvent;
  SyncEvent mEeSetModeEvent;
};
