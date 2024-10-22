/******************************************************************************
 *
 *  Copyright 2021 NXP
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

#pragma once
#include <NfccAltTransport.h>
#include <NfccTransport.h>

class NfccAltI2cTransport : public NfccAltTransport {
 public:
  /*****************************************************************************
   **
   ** Function         OpenAndConfigure
   **
   ** Description      Open and configure NFCC device
   **
   ** Parameters       pConfig     - hardware information
   **                  pLinkHandle - device handle
   **
   ** Returns          NFC status:
   **                  NFCSTATUS_SUCCESS - open_and_configure operation success
   **                  NFCSTATUS_INVALID_DEVICE - device open operation failure
   **
   ****************************************************************************/
  NFCSTATUS OpenAndConfigure(pphTmlNfc_Config_t pConfig, void** pLinkHandle);

  /*****************************************************************************
   **
   ** Function         Read
   **
   ** Description      Reads requested number of bytes from NFCC device into
   *given
   **                  buffer
   **
   ** Parameters       pDevHandle       - valid device handle
   **                  pBuffer          - buffer for read data
   **                  nNbBytesToRead   - number of bytes requested to be read
   **
   ** Returns          numRead   - number of successfully read bytes
   **                  -1        - read operation failure
   **
   ****************************************************************************/
  int Read(void* pDevHandle, uint8_t* pBuffer, int nNbBytesToRead);

  /*****************************************************************************
  **
  ** Function         Write
  **
  ** Description      Writes requested number of bytes from given buffer into
  **                  NFCC device
  **
  ** Parameters       pDevHandle       - valid device handle
  **                  pBuffer          - buffer for read data
  **                  nNbBytesToWrite  - number of bytes requested to be written
  **
  ** Returns          numWrote   - number of successfully written bytes
  **                  -1         - write operation failure
  **
  *****************************************************************************/
  int Write(void* pDevHandle, uint8_t* pBuffer, int nNbBytesToWrite);
  void Close(void* pDevHandle);
};
