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

#ifndef __NATIVE_NFC_SNEP_H__
#define __NATIVE_NFC_SNEP_H__

#include "data_types.h"
#include "linux_nfc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef WIN_PLATFORM

extern INT32 nativeNfcSnep_registerClientCallback(nfcSnepClientCallback_t *clientCallback);

extern void nativeNfcSnep_deregisterClientCallback();

extern INT32 nativeNfcSnep_startServer(nfcSnepServerCallback_t *serverCallback);

extern void nativeNfcSnep_stopServer();

extern INT32 nativeNfcSnep_putMessage(UINT8* msg, UINT32 length);

extern void  nativeNfcSnep_handleNfcOnOff (BOOLEAN isOn);

#ifdef __cplusplus
}
#endif

#endif // __NATIVE_NFC_SNEP_H__

