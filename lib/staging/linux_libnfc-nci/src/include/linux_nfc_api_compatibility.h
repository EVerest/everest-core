/******************************************************************************
 *
 *  Copyright 2021 NXP
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

#ifndef __LINUX_NFC_API_COMPATIBILITY_H__
#define __LINUX_NFC_API_COMPATIBILITY_H__

#ifndef NXP_NEW_MW_API_COMPATIBILITY

#ifdef __cplusplus
extern "C" {
#endif

#ifndef nfcManager_doInitialize
#define nfcManager_doInitialize doInitialize
#endif

#ifndef nfcManager_doDeinitialize
#define nfcManager_doDeinitialize doDeinitialize
#endif

#ifndef nfcManager_isNfcActive
#define nfcManager_isNfcActive isNfcActive
#endif

#ifndef nfcManager_enableDiscovery
#define nfcManager_enableDiscovery doEnableDiscovery
#endif

#ifndef nfcManager_disableDiscovery
#define nfcManager_disableDiscovery disableDiscovery
#endif

#ifndef nfcManager_registerTagCallback
#define nfcManager_registerTagCallback registerTagCallback
#endif

#ifndef nfcManager_deregisterTagCallback
#define nfcManager_deregisterTagCallback deregisterTagCallback
#endif

#ifndef nfcManager_selectNextTag
#define nfcManager_selectNextTag selectNextTag
#endif

#ifndef nfcManager_getNumTags
#define nfcManager_getNumTags getNumTags
#endif

#ifndef nfcManager_checkNextProtocol
#define nfcManager_checkNextProtocol checkNextProtocol
#endif

#ifndef nfcManager_getFwVersion
#define nfcManager_getFwVersion getFwVersion
#endif

#ifdef __cplusplus
}
#endif

#endif //NXP_NEW_MW_API_COMPATIBILITY

#endif //__LINUX_NFC_API_COMPATIBILITY_H__
