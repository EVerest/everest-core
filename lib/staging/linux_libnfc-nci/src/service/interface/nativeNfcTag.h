/******************************************************************************
 *
 *  Copyright 2015-2021,2022 NXP
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

#ifndef __NATIVE_NFC_TAG__H__
#define __NATIVE_NFC_TAG__H__

#include "data_types.h"
#include "linux_nfc_api.h"

/*******************************************************************************
**
** Function:        nativeNfcTag_checkNdef
**
** Description:     Does the tag contain a NDEF message?
**                      tagHandle: handle of the tag
**                      maxNdefLength: for return, max NDef message length
**                      isWritable: for return, if write new NDef is allowed
**
** Returns:         TRUE if has NDEF message.
**
*******************************************************************************/
extern BOOLEAN nativeNfcTag_doCheckNdef(UINT32 tagHandle, ndef_info_t *info);

/*******************************************************************************
**
** Function:        nativeNfcTag_doHandleReconnect
**
** Description:     Reconnect to the tag and Reselect
**                  tagHandle: tag handle.
**
** Returns:         0 if ok.
**
******************************************************************************/
extern INT32 nativeNfcTag_doHandleReconnect(UINT32 tagHandle);

/*******************************************************************************
**
** Function:        nativeNfcTag_doRead
**
** Description:     Read the NDEF message on the tag.
**                  tagHandle: tag handle.
**
** Returns:         NDEF message.
**
******************************************************************************/
extern INT32 nativeNfcTag_doRead(UINT8* ndefBuffer,  UINT32 ndefBufferLength, nfc_friendly_type_t *friendly_type);

/*******************************************************************************
**
** Function:        writeNdef
**
** Description:     Write a NDEF message to the tag.
**                  buf: Contains a NDEF message.
**
** Returns:         0 if ok.
**
*******************************************************************************/
extern INT32 nativeNfcTag_doWrite(UINT8* data,  UINT32 dataLength/*ndef message*/);

/*******************************************************************************
**
** Function:        isFormatable
**
** Description:     Can tag be formatted to store NDEF message?
**                  tagHandle: Handle of tag.
**
** Returns:         True if formattable.
**
*******************************************************************************/
extern BOOLEAN nativeNfcTag_isFormatable(UINT32 tagHandle);

extern INT32 nativeNfcTag_doFormatTag(UINT32 tagHandle);

/*******************************************************************************
**
** Function:        nativeNfcTag_doMakeReadonly
**
** Description:     Make the tag read-only.
**
** Returns:         0 if success.
**
*******************************************************************************/
extern INT32 nativeNfcTag_doMakeReadonly (UINT32 tagHandle,UINT8 *key, UINT8 key_size);

extern INT32 nativeNfcTag_switchRF(UINT32 tagHandle, BOOLEAN isFrameRF);

extern INT32 nativeNfcTag_doTransceive (UINT32 handle, UINT8* txBuffer, INT32 txBufferLen, UINT8* rxBuffer, INT32 rxBufferLen, UINT32 timeout);

#endif /* __NATIVE_NFC_TAG__H__ */
