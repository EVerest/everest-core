/******************************************************************************
 *
 *  Copyright 2018-2019 NXP
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

#define LOG_TAG "nxpnfc@1.2-service"
#include <android/hardware/nfc/1.2/INfc.h>
#include <vendor/nxp/nxpnfc/1.0/INxpNfc.h>

#include <hidl/LegacySupport.h>
#include "Nfc.h"
#include "NxpNfc.h"

// Generated HIDL files
using android::hardware::nfc::V1_2::INfc;
using android::hardware::nfc::V1_2::implementation::Nfc;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
using android::status_t;
using android::OK;
using vendor::nxp::nxpnfc::V1_0::INxpNfc;
using vendor::nxp::nxpnfc::V1_0::implementation::NxpNfc;

int main() {
    ALOGD("NFC HAL Service 1.2 is starting.");
    sp<INfc> nfc_service = new Nfc();

    configureRpcThreadpool(1, true /*callerWillJoin*/);
    status_t status = nfc_service->registerAsService();
    if (status != OK) {
        LOG_ALWAYS_FATAL("Could not register service for NFC HAL Iface (%d).", status);
        return -1;
    }
    sp<INxpNfc> nxp_nfc_service = new NxpNfc();
    status = nxp_nfc_service->registerAsService();
    if (status != OK) {
        ALOGD("Could not register service for NXP NFC Extn Iface (%d).", status);
    }
    ALOGD("NFC service is ready");
    joinRpcThreadpool();
    return 1;
}
