/*
 * Copyright (C) 2018 The Android Open Source Project
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
#pragma once

#include "NfcJniUtil.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"

/**
 * This class is used for receiving HCI events from libnfc-nci
 * and forwarding the Transaction Events to NFC Service.
 */
class HciEventManager {
 private:
  nfc_jni_native_data* mNativeData;
  static uint8_t sEsePipe;
  static uint8_t sSimPipe;

  HciEventManager();
  std::vector<uint8_t> getDataFromBerTlv(std::vector<uint8_t> berTlv);
  void notifyTransactionListenersOfAid(std::vector<uint8_t> aid,
                                       std::vector<uint8_t> data,
                                       std::string evtSrc);
  static void nfaHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* eventData);

 public:
  static HciEventManager& getInstance();
  void initialize(nfc_jni_native_data* native);
  void finalize();
};
