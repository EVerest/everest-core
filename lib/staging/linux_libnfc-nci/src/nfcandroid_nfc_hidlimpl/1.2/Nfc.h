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

#ifndef ANDROID_HARDWARE_NFC_V1_2_NFC_H
#define ANDROID_HARDWARE_NFC_V1_2_NFC_H

#include <android/hardware/nfc/1.2/INfc.h>
#include <android/hardware/nfc/1.2/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <log/log.h>

namespace android {
namespace hardware {
namespace nfc {
namespace V1_2 {
namespace implementation {

using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::nfc::V1_2::INfc;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;
struct Nfc : public V1_2::INfc, public hidl_death_recipient {
 public:
  // Methods from ::android::hardware::nfc::V1_0::INfc follow.
  Return<V1_0::NfcStatus> open(
      const sp<V1_0::INfcClientCallback>& clientCallback) override;
  Return<V1_0::NfcStatus> open_1_1(
      const sp<V1_1::INfcClientCallback>& clientCallback) override;
  Return<uint32_t> write(const hidl_vec<uint8_t>& data) override;
  Return<V1_0::NfcStatus> coreInitialized(
      const hidl_vec<uint8_t>& data) override;
  Return<V1_0::NfcStatus> prediscover() override;
  Return<V1_0::NfcStatus> close() override;
  Return<V1_0::NfcStatus> controlGranted() override;
  Return<V1_0::NfcStatus> powerCycle() override;

  // Methods from ::android::hardware::nfc::V1_1::INfc follow.
  Return<void> factoryReset();
  Return<V1_0::NfcStatus> closeForPowerOffCase();
  Return<void> getConfig(getConfig_cb config);

  // Methods from ::android::hardware::nfc::V1_2::INfc follow.
  Return<void> getConfig_1_2(getConfig_1_2_cb config);

  // Methods from ::android::hidl::base::V1_0::IBase follow.
  static void eventCallback(uint8_t event, uint8_t status) {
    if (mCallbackV1_1 != nullptr) {
      auto ret = mCallbackV1_1->sendEvent_1_1((V1_1::NfcEvent)event,
                                              (V1_0::NfcStatus)status);
      if (!ret.isOk()) {
        ALOGW("failed to send event!!!");
      }
    } else if (mCallbackV1_0 != nullptr) {
      auto ret = mCallbackV1_0->sendEvent((V1_0::NfcEvent)event,
                                          (V1_0::NfcStatus)status);
      if (!ret.isOk()) {
        ALOGE("failed to send event!!!");
      }
    }
  }

  static void dataCallback(uint16_t data_len, uint8_t* p_data) {
    hidl_vec<uint8_t> data;
    data.setToExternal(p_data, data_len);
    if (mCallbackV1_1 != nullptr) {
      auto ret = mCallbackV1_1->sendData(data);
      if (!ret.isOk()) {
        ALOGW("failed to send data!!!");
      }
    } else if (mCallbackV1_0 != nullptr) {
      auto ret = mCallbackV1_0->sendData(data);
      if (!ret.isOk()) {
        ALOGE("failed to send data!!!");
      }
    }
  }

  virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) {
    close();
  }

 private:
  static sp<V1_1::INfcClientCallback> mCallbackV1_1;
  static sp<V1_0::INfcClientCallback> mCallbackV1_0;
};

}  // namespace implementation
}  // namespace V1_2
}  // namespace nfc
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_NFC_V1_2_NFC_H
