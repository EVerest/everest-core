/******************************************************************************
 *
 *  Copyright 2020-2021 NXP
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
#include <NfccTransport.h>
#define NFC_MAGIC 0xE9
/*
 * NFCC power control via ioctl
 * NFC_SET_PWR(0): power off
 * NFC_SET_PWR(1): power on
 * NFC_SET_PWR(2): reset and power on with firmware download enabled
 */
#define NFC_SET_PWR _IOW(NFC_MAGIC, 0x01, long)
/*
 * 1. SPI Request NFCC to enable ESE power, only in param
 *   Only for SPI
 *   level 1 = Enable power
 *   level 0 = Disable power
 * 2. NFC Request the eSE cold reset, only with MODE_ESE_COLD_RESET
 */
#define ESE_SET_PWR _IOW(NFC_MAGIC, 0x02, long)

/*
 * SPI or DWP can call this ioctl to get the current
 * power state of ESE
 */
#define ESE_GET_PWR _IOR(NFC_MAGIC, 0x03, long)

/*
 * get platform interface type(i2c or i3c) for common MW
 * return 0 - i2c, 1 - i3c
 */
#define NFC_GET_PLATFORM_TYPE _IO(NFC_MAGIC, 0x04)
/*
 * get boot state
 * return unknown, fw dwl, fw teared, nci
 */
#define NFC_GET_NFC_STATE _IO(NFC_MAGIC, 0x05)

/* NFC HAL can call this ioctl to get the current IRQ state */
#define NFC_GET_IRQ_STATE _IO(NFC_MAGIC, 0x06)

extern phTmlNfc_i2cfragmentation_t fragmentation_enabled;

class NfccI2cTransport : public NfccTransport {
 private:
  bool_t bFwDnldFlag = false;
  sem_t mTxRxSemaphore;
  /*****************************************************************************
   **
   ** Function         SemTimedWait
   **
   ** Description      Timed sem_wait for avoiding i2c_read & write overlap
   **
   ** Parameters       none
   **
   ** Returns          Sem_wait return status
   ****************************************************************************/
  int SemTimedWait();

  /*****************************************************************************
   **
   ** Function         SemPost
   **
   ** Description      sem_post 2c_read / write
   **
   ** Parameters       none
   **
   ** Returns          none
   ****************************************************************************/
  void SemPost();

  int Flushdata(void* pDevHandle, uint8_t* pBuffer, int numRead);

 public:
  /*****************************************************************************
  **
  ** Function         Close
  **
  ** Description      Closes NFCC device
  **
  ** Parameters       pDevHandle - device handle
  **
  ** Returns          None
  **
  *****************************************************************************/
  void Close(void *pDevHandle);

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
  NFCSTATUS OpenAndConfigure(pphTmlNfc_Config_t pConfig, void **pLinkHandle);

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
  int Read(void *pDevHandle, uint8_t *pBuffer, int nNbBytesToRead);

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
  int Write(void *pDevHandle, uint8_t *pBuffer, int nNbBytesToWrite);

  /*****************************************************************************
   **
   ** Function         Reset
   **
   ** Description      Reset NFCC device, using VEN pin
   **
   ** Parameters       pDevHandle     - valid device handle
   **                  level          - reset level
   **
   ** Returns           0   - reset operation success
   **                  -1   - reset operation failure
   **
   ****************************************************************************/
  int NfccReset(void *pDevHandle, NfccResetType eType);

  /*****************************************************************************
   **
   ** Function         EseReset
   **
   ** Description      Request NFCC to reset the eSE
   **
   ** Parameters       pDevHandle     - valid device handle
   **                  eType          - EseResetType
   **
   ** Returns           0   - reset operation success
   **                  else - reset operation failure
   **
   ****************************************************************************/
  int EseReset(void *pDevHandle, EseResetType eType);

  /*****************************************************************************
   **
   ** Function         EseGetPower
   **
   ** Description      Request NFCC to reset the eSE
   **
   ** Parameters       pDevHandle     - valid device handle
   **                  level          - reset level
   **
   ** Returns           0   - reset operation success
   **                  else - reset operation failure
   **
   ****************************************************************************/
  int EseGetPower(void *pDevHandle, long level);

  /*****************************************************************************
   **
   ** Function         GetPlatform
   **
   ** Description      Get platform interface type (i2c or i3c) for common mw
   **
   ** Parameters       pDevHandle     - valid device handle
   **
   ** Returns           0   - i2c
   **                   1   - i3c
   **
   ****************************************************************************/
  int GetPlatform(void *pDevHandle);

  /*****************************************************************************
  **
  ** Function         GetNfcState
  **
  ** Description      Get Nfc state
  **
  ** Parameters       pDevHandle     - valid device handle
  ** Returns           0   - unknown
  **                   1   - FW DWL
  **                   2   - NCI
  **
  *****************************************************************************/
  int GetNfcState(void *pDevHandle);

  /*****************************************************************************
   **
   ** Function         EnableFwDnldMode
   **
   ** Description      updates the state to Download mode
   **
   ** Parameters       True/False
   **
   ** Returns          None
   ****************************************************************************/
  void EnableFwDnldMode(bool mode);

  /*****************************************************************************
   **
   ** Function         IsFwDnldModeEnabled
   **
   ** Description      Returns the current mode
   **
   ** Parameters       none
   **
   ** Returns           Current mode download/NCI
   ****************************************************************************/
  bool_t IsFwDnldModeEnabled(void);

  /*******************************************************************************
   **
   ** Function         GetIrqState
   **
   ** Description      Get state of IRQ GPIO
   **
   ** Parameters       pDevHandle - valid device handle
   **
   ** Returns          The state of IRQ line i.e. +ve if read is pending else Zer0.
   **                  In the case of IOCTL error, it returns -ve value.
   **
   *******************************************************************************/
  int GetIrqState(void *pDevHandle);
};
