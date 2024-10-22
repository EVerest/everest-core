/******************************************************************************
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
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

#ifndef __NATIVE_NFC_HANDOVER_H__
#define __NATIVE_NFC_HANDOVER_H__

#include "data_types.h"
#include "linux_nfc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

extern INT32 nativeNfcHO_registerCallback(nfcHandoverCallback_t *callback);

extern void nativeNfcHO_deregisterCallback();

extern INT32 nativeNfcHO_sendHs(UINT8 *msg, UINT32 length);

extern INT32 nativeNfcHO_sendSelectError(UINT8  error_reason, UINT32 error_data);

#ifdef __cplusplus
}
#endif

#endif // __NATIVE_NFC_HANDOVER_H__
