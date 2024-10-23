/******************************************************************************
 *
 *  Copyright (C) 2015,2021 NXP Semiconductors
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
#define LOG_TAG "NxpHal"
#include "NxpNfcCapability.h"
#include <phNxpLog.h>
#include "logging.h"

capability* capability::instance = NULL;
tNFC_chipType capability::chipType = pn81T;
tNfc_featureList nfcFL;

capability::capability() {}

capability* capability::getInstance() {
  if (NULL == instance) {
    instance = new capability();
  }
  return instance;
}

tNFC_chipType capability::getChipType(uint8_t* msg, uint16_t msg_len) {
  if ((msg != NULL) && (msg_len != 0)) {
    uint16_t offsetHwVersion = 0;
    uint16_t offsetFwVersion = 0;

    if (msg[0] == 0x60 && msg[1] == 0x00) {
      /*CORE_RST_NTF*/
      offsetHwVersion = offsetRstHwVersion;
      offsetFwVersion = offsetRstFwVersion;
    } else if (msg[0] == 0x40 && msg[1] == 0x01) {
      /*CORE_INIT_RSP*/
      offsetHwVersion = offsetInitHwVersion;
      offsetFwVersion = offsetInitFwVersion;
    } else if (msg[0] == 0x00 && msg[1] == 0x0A) {
      /*Propreitary Response*/
      offsetHwVersion = offsetPropHwVersion;
      offsetFwVersion = offsetPropFwVersion;
    }
    if ((offsetHwVersion > 0) && (offsetHwVersion < msg_len)) {
      ALOGD("%s HwVersion : 0x%02x", __func__, msg[offsetHwVersion]);
      switch (msg[offsetHwVersion]) {
        case 0x40:  // PN553 A0
        case 0x41:  // PN553 B0
          // NQ310
          if (msg[offsetFwVersion] == 0x12) {
            chipType = pn557;
          } else{
            chipType = pn553;
          }
          break;

        case 0x50:  // PN553 A0 + P73
        case 0x51:  // PN553 B0 + P73 , NQ440
                    // NQ330
                    // PN80T
                    // PN81T
          if (msg[offsetFwVersion] == 0x12) {
            chipType = pn81T;
          } else {
            chipType = pn80T;
          }
          break;

        case 0x61:// PN7160 (no ECP support)
        case 0x71:// PN7161 (ECP support)
          if (msg[offsetFwVersion] == 0x11) {
            chipType = pn553;
          } else if (msg[offsetFwVersion] == 0x12) {
            chipType = pn557;
          }
          break;
        case 0x98:
          chipType = pn551;
          break;

        case 0xA8:
        case 0x08:
          chipType = pn67T;
          break;

        case 0x28:
        case 0x48:  // NQ210
        case 0x88:
          chipType = pn548C2;
          break;

        case 0x18:
        case 0x58:  // NQ220
          chipType = pn66T;
          break;

        default:
          chipType = pn80T;
      }
    } else {
      ALOGD("%s Wrong msg_len. Setting Default ChiptType pn81T", __func__);
      chipType = pn81T;
    }
  }
  ALOGD("%s Product : %s", __func__, product[chipType]);
  return chipType;
}
