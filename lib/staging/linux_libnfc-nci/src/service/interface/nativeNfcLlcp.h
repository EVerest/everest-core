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

#ifndef __NATIVE_NFC_LLCP_H__
#define __NATIVE_NFC_LLCP_H__

#include "data_types.h"
#include "linux_nfc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef WIN_PLATFORM

#define LLCP_CL_SAP_ID_DEFAULT 0x14
#define LLCP_MAX_DATA_SIZE 0x07FF + 0x0800

extern INT32 nativeNfcLlcp_ConnLessRegisterClientCallback(nfcllcpConnlessClientCallback_t *clientCallback);

extern void nativeNfcLlcp_ConnLessDeregisterClientCallback();

extern INT32 nativeNfcLlcp_ConnLessStartServer(nfcllcpConnlessServerCallback_t *serverCallback);

extern void nativeNfcLlcp_ConnLessStopServer();

extern INT32 nativeNfcLlcp_ConnLessSendMessage(UINT8* msg, UINT32 length);

extern INT32 nativeNfcLlcp_ConnLessReceiveMessage(UINT8* msg, UINT32 *length);

#ifdef __cplusplus
}
#endif

#endif // __NATIVE_NFC_LLCP_H__

