/******************************************************************************
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

#include <NfccTransport.h>

int NfccTransport::NfccReset(__attribute__((unused)) void *pDevHandle,
                             __attribute__((unused)) NfccResetType eType) {
  return NFCSTATUS_SUCCESS;
}

int NfccTransport::EseReset(__attribute__((unused)) void *pDevHandle,
                            __attribute__((unused)) EseResetType eType) {
  return NFCSTATUS_SUCCESS;
}
int NfccTransport::EseGetPower(__attribute__((unused)) void *pDevHandle,
                               __attribute__((unused)) long level) {
  return NFCSTATUS_SUCCESS;
}

int NfccTransport::GetPlatform(__attribute__((unused)) void *pDevHandle) {
  return 0x00;
}

int NfccTransport::GetNfcState(__attribute__((unused)) void *pDevHandle) {
  return NFC_STATE_UNKNOWN;
}

void NfccTransport::EnableFwDnldMode(__attribute__((unused)) bool mode) {
  return;
}
int NfccTransport::GetIrqState(__attribute__((unused)) void *pDevHandle) {
  return -1;
}

bool_t NfccTransport::IsFwDnldModeEnabled(void) { return false; }
