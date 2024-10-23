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
#ifndef __NATIVE_NDEF__H__
#define __NATIVE_NDEF__H__

#include "data_types.h"
#include "linux_nfc_api.h"

extern nfc_friendly_type_t nativeNdef_getFriendlyType(UINT8 tnf, UINT8 *type, UINT8 typeLength);

extern INT32 nativeNdef_readText( UINT8*ndefBuff, UINT32 ndefBuffLen, char * outText, UINT32 textLen);

extern INT32 nativeNdef_readLang( UINT8*ndefBuff, UINT32 ndefBuffLen, char * outLang, UINT32 LangLen);

extern INT32 nativeNdef_readUrl(UINT8*ndefBuff, UINT32 ndefBuffLen, char * outUrl, UINT32 urlBufferLen);

extern INT32 nativeNdef_readHr(UINT8*ndefBuff, UINT32 ndefBuffLen, nfc_handover_request_t *hrInfo);

extern INT32 nativeNdef_readHs(UINT8*ndefBuff, UINT32 ndefBuffLen, nfc_handover_select_t *hsInfo);

extern INT32 nativeNdef_createUri(char *uri, UINT8*outNdefBuff, UINT32 outBufferLen);

extern INT32 nativeNdef_createText(char *languageCode, char *text, UINT8*outNdefBuff, UINT32 outBufferLen);

extern INT32 nativeNdef_createMime(char *mimeType, UINT8 *mimeData, UINT32 mimeDataLength,
                                                                            UINT8*outNdefBuff, UINT32 outBufferLen);

extern INT32 nativeNdef_createHs(nfc_handover_cps_t cps, char *carrier_data_ref,
                                UINT8 *ndefBuff, UINT32 ndefBuffLen, UINT8 *outBuff, UINT32 outBuffLen);

#endif
