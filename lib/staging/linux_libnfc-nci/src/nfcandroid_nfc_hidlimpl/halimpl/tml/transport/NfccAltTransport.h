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
#include <NfccTransport.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <poll.h>

#define I2C_ADDRESS 0x28
#define I2C_BUS "/dev/i2c-1"
#define SPI_BUS "/dev/spidev1.1"
#define PIN_INT 535
#define PIN_ENABLE 536
#define PIN_FWDNLD 537
#define EDGE_NONE 0
#define EDGE_RISING 1
#define EDGE_FALLING 2
#define EDGE_BOTH 3
#define CRC_LEN 2
#define NORMAL_MODE_HEADER_LEN 3
#define FW_DNLD_HEADER_LEN 2
#define FW_DNLD_LEN_OFFSET 1
#define NORMAL_MODE_LEN_OFFSET 2
#define FRAGMENTSIZE_MAX PHNFC_I2C_FRAGMENT_SIZE
extern phTmlNfc_i2cfragmentation_t fragmentation_enabled;

class NfccAltTransport : public NfccTransport {
 public:
  NfccAltTransport();
  bool_t bFwDnldFlag = false;
  sem_t mTxRxSemaphore;
  int iEnableFd;
  int iInterruptFd;
  int iFwDnldFd;

 public:
  void gpio_set_ven(int value);
  void gpio_set_fwdl(int value);
  int verifyPin(int pin, int isoutput, int edge);
  void wait4interrupt(void);
  int SemTimedWait();
  void SemPost();
  int Flushdata(void* pDevHandle, uint8_t* pBuffer, int numRead);
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
  int NfccReset(void* pDevHandle, NfccResetType eType);

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
   ** Returns          The state of IRQ line i.e. +ve if read is pending else
   *Zer0.
   **                  In the case of IOCTL error, it returns -ve value.
   **
   *******************************************************************************/
  int GetIrqState(void* pDevHandle);
  int GetNfcState(void* pDevHandle);
  /*****************************************************************************
   **
   ** Function         ConfigurePin
   **
   ** Description      Configure Pins such as IRQ, VEN, Firmware Download
   **
   ** Parameters       none
   **
   ** Returns           NFCSTATUS_SUCCESS - on Success/ -1 on Failure
   ****************************************************************************/
  int ConfigurePin();
};
