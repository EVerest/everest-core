/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PHNFCTYPES_MAPPING_H_
#define PHNFCTYPES_MAPPING_H_

typedef phNfc_sData_t phHal_sData_t;
typedef phNfc_sSupProtocol_t phHal_sSupProtocol_t;
#define phHal_eMifareRaw phNfc_eMifareRaw
#define phHal_eMifareAuthentA phNfc_eMifareAuthentA
#define phHal_eMifareAuthentB phNfc_eMifareAuthentB
#define phHal_eMifareRead16 phNfc_eMifareRead16
#define phHal_eMifareRead phNfc_eMifareRead
#define phHal_eMifareWrite16 phNfc_eMifareWrite16
#define phHal_eMifareWrite4 phNfc_eMifareWrite4
#define phHal_eMifareInc phNfc_eMifareInc
#define phHal_eMifareDec phNfc_eMifareDec
#define phHal_eMifareTransfer phNfc_eMifareTransfer
#define phHal_eMifareRestore phNfc_eMifareRestore
#define phHal_eMifareReadSector phNfc_eMifareReadSector
#define phHal_eMifareWriteSector phNfc_eMifareWriteSector
#define phHal_eMifareReadN phNfc_eMifareReadN
#define phHal_eMifareWriteN phNfc_eMifareWriteN
#define phHal_eMifareSectorSel phNfc_eMifareSectorSel
#define phHal_eMifareAuth phNfc_eMifareAuth
#define phHal_eMifareProxCheck phNfc_eMifareProxCheck
#define phHal_eMifareInvalidCmd phNfc_eMifareInvalidCmd

typedef phNfc_eMifareCmdList_t phHal_eMifareCmdList_t;

typedef phNfc_uCmdList_t phHal_uCmdList_t;
typedef phNfc_sRemoteDevInformation_t phHal_sRemoteDevInformation_t;
typedef phNfc_sRemoteDevInformation_t phLibNfc_sRemoteDevInformation_t;
#endif /* PHNFCTYPES_MAPPING_H_ */
