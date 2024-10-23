/******************************************************************************
 *
 *  Copyright 2018 NXP
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

#ifndef VENDOR_NXP_NXPNFC_V1_0_NXPNFC_H
#define VENDOR_NXP_NXPNFC_V1_0_NXPNFC_H

#include <vendor/nxp/nxpnfc/1.0/INxpNfc.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "hal_nxpnfc.h"

namespace vendor {
namespace nxp {
namespace nxpnfc {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::IBase;
using ::vendor::nxp::nxpnfc::V1_0::INxpNfc;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct NxpNfc : public INxpNfc {
  Return<void> ioctl(uint64_t ioctlType, const hidl_vec<uint8_t>& inOutData,
                     ioctl_cb _hidl_cb) override;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace nxpnfc
}  // namespace nxp
}  // namespace vendor

#endif  // VENDOR_NXP_NXPNFC_V1_0_NXPNFC_H
