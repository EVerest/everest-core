/******************************************************************************
 *
 *  Copyright 2015-2021 NXP
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

#ifndef __NATIVE_NFCMANAGER_TAG__H__
#define __NATIVE_NFCMANAGER_TAG__H__

#include "data_types.h"
#include "linux_nfc_api.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

/*******************************************************************************
**
** Function:        nfcManager_isNfcActive
**
** Description:     Used externaly to determine if NFC is active or not.
**
** Returns:         'true' if the NFC stack is running, else 'false'.
**
*******************************************************************************/
BOOLEAN nfcManager_isNfcActive();

/*******************************************************************************
**
** Function:        nfcManager_doInitialize
**
** Description:     Turn on NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool nfcManager_doInitialize ();


/*******************************************************************************
**
** Function:        doDeinitialize
**
** Description:     Turn off NFC.
**
** Returns:         0 if ok.
**
*******************************************************************************/
INT32 nfcManager_doDeinitialize ();


/*******************************************************************************
**
** Function:        nfcManager_enableDiscovery
**
** Description:     Start polling and listening for devices.
**                  technologies_mask: the bitmask of technologies for which to enable discovery
**                  reader mode:
**                  enable_host_routing:
**                  restart:
**
** Returns:         0 if ok, error code otherwise
**
*******************************************************************************/
INT32 nfcManager_enableDiscovery (INT32 technologies_mask,
    BOOLEAN enable_lptd, BOOLEAN reader_mode, INT32 enable_host_routing, BOOLEAN enable_p2p, BOOLEAN restart);


/*******************************************************************************
**
** Function:        disableDiscovery
**
** Description:     Stop polling and listening for devices.
**
** Returns:         0 if ok, error code otherwise
**
*******************************************************************************/
INT32 nfcManager_disableDiscovery ();

void nfcManager_registerTagCallback(nfcTagCallback_t *nfcTagCb);

void nfcManager_deregisterTagCallback();

int nfcManager_selectNextTag();

int nativeNfcManager_checkNextProtocol();

int nfcManager_getNumTags();

void nfcManager_registerHostCallback(nfcHostCardEmulationCallback_t *callback);
void nfcManager_deregisterHostCallback();

/*******************************************************************************
**
** Function:        nativeNfcManager_sendRawFrame
**
** Description:     Send a raw frame.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool nfcManager_sendRawFrame (UINT8 *buf, UINT32 bufLen);

/*******************************************************************************
**
** Function:        nfcManager_doRegisterT3tIdentifier
**
** Description:     Registers LF_T3T_IDENTIFIER for NFC-F.
**                  t3tId: LF_T3T_IDENTIFIER value
**                  t3tIdsize: LF_T3T_IDENTIFIER size (10 or 18 bytes)
**
** Returns:         Handle retrieve from RoutingManager.
**
*******************************************************************************/
int nfcManager_doRegisterT3tIdentifier(UINT8 *t3tId, UINT8 t3tIdsize);

/*******************************************************************************
**
** Function:        nfcManager_doDeregisterT3tIdentifier
**
** Description:     Deregisters LF_T3T_IDENTIFIER for NFC-F.
**
** Returns:         None
**
*******************************************************************************/
void nfcManager_doDeregisterT3tIdentifier(void);

/*******************************************************************************
**
** Function:        nativeNfcManager_setConfig
**
** Description:     NCI SET CONFIG command.
**
** Returns:         0 if ok, error code otherwise.
**
*******************************************************************************/
INT32 nativeNfcManager_setConfig(UINT8 id, UINT8 length, UINT8 *p_data);

int t4tNfceeManager_doWriteT4tData(UINT8 *fileId,
                                    UINT8 *ndefBuffer, int ndefBufferLength);

int t4tNfceeManager_doReadT4tData(UINT8 *fileId,
                                    UINT8 *ndefBuffer, int *ndefBufferLength);
//#ifdef __cplusplus
//}
//#endif

#endif // __NATIVE_NFCMANAGER_TAG__H__
