/******************************************************************************
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

#include <errno.h>
#include <fcntl.h>
#ifdef ANDROID
#include <hardware/nfc.h>
#endif
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <NfccAltI2cTransport.h>
#include <phNfcStatus.h>
#include <phNxpLog.h>
#include <string.h>
#include "phNxpNciHal_utils.h"

extern phTmlNfc_Context_t* gpphTmlNfc_Context;

/*******************************************************************************
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
*******************************************************************************/
NFCSTATUS NfccAltI2cTransport::OpenAndConfigure(pphTmlNfc_Config_t pConfig,
                                                void** pLinkHandle) {
  NXPLOG_TML_D("%s Enter", __func__);
  NXPLOG_TML_D("phTmlNfc_i2c_open_and_configure Alternative NFC\n");
  NXPLOG_TML_D("NFC - Assign IO pins\n");
  int status_value = -1;
  int Fd = -1;
  // Assign IO pins
  status_value = ConfigurePin();
  if(status_value == -1)
    return NFCSTATUS_INVALID_DEVICE;
  NXPLOG_TML_D("NFCHW - open I2C bus - %s\n", I2C_BUS);

  // I2C bus
  Fd = open(I2C_BUS, O_RDWR | O_NOCTTY);
  if (Fd < 0) {
    NXPLOG_TML_E("Could not open I2C bus '%s' (%s)", I2C_BUS, strerror(errno));
    Close(NULL);
    return (NFCSTATUS_INVALID_DEVICE);
  }
  *pLinkHandle = (void*)((intptr_t)Fd);
  NXPLOG_TML_D("NFC - open I2C device - 0x%02x\n", I2C_ADDRESS);

  // I2C slave address
  if (ioctl(Fd, I2C_SLAVE, I2C_ADDRESS) < 0) {
    NXPLOG_TML_E("Cannot select I2C address (%s)\n", strerror(errno));
    Close(pLinkHandle);
    return (NFCSTATUS_INVALID_DEVICE);
  }

  NfccReset((void*)((intptr_t)Fd), MODE_POWER_OFF);
  NfccReset((void*)((intptr_t)Fd), MODE_POWER_ON);

  NXPLOG_TML_D("%s exit", __func__);
  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         Read
**
** Description      Reads requested number of bytes from NFCC device into given
**                  buffer
**
** Parameters       pDevHandle       - valid device handle
**                  pBuffer          - buffer for read data
**                  nNbBytesToRead   - number of bytes requested to be read
**
** Returns          numRead   - number of successfully read bytes
**                  -1        - read operation failure
**
*******************************************************************************/
int NfccAltI2cTransport::Read(void* pDevHandle, uint8_t* pBuffer,
                              int nNbBytesToRead) {
  NXPLOG_TML_D("%s Enter", __func__);
  int ret_Read;
  int numRead = 0;
  uint16_t totalBtyesToRead = 0;

  int ret_Select;
  struct timeval tv;
  fd_set rfds;

  UNUSED(nNbBytesToRead);
  if (NULL == pDevHandle) {
    NXPLOG_TML_E("%s devive handle is NULL", __func__);
    return -1;
  }

  if (FALSE == bFwDnldFlag) {
    totalBtyesToRead = NORMAL_MODE_HEADER_LEN;
  } else {
    totalBtyesToRead = FW_DNLD_HEADER_LEN;
  }

  /* Read with 2 second timeout, so that the read thread can be aborted
     when the PN54X does not respond and we need to switch to FW download
     mode. This should be done via a control socket instead. */
  FD_ZERO(&rfds);
  FD_SET((intptr_t)pDevHandle, &rfds);
  tv.tv_sec = 2;
  tv.tv_usec = 1;

  ret_Select =
      select((int)((intptr_t)pDevHandle + (int)1), &rfds, NULL, NULL, &tv);
  if (ret_Select < 0) {
    NXPLOG_TML_E("i2c select() errno : %x", errno);
    return -1;
  } else if (ret_Select == 0) {
    NXPLOG_TML_E("i2c select() Timeout");
    return -1;
  } else {
    wait4interrupt();
    ret_Read = read((intptr_t)pDevHandle, pBuffer, totalBtyesToRead - numRead);
    if (ret_Read > 0) {
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("_i2c_read() [hdr]EOF");
      return -1;
    } else {
      NXPLOG_TML_E("_i2c_read() [hdr] errno : %x", errno);
      return -1;
    }

    if (FALSE == bFwDnldFlag) {
      totalBtyesToRead = NORMAL_MODE_HEADER_LEN;
    } else {
      totalBtyesToRead = FW_DNLD_HEADER_LEN;
    }

    if (numRead < totalBtyesToRead) {
      wait4interrupt();
      ret_Read =
          read((intptr_t)pDevHandle, pBuffer, totalBtyesToRead - numRead);
      if (ret_Read != totalBtyesToRead - numRead) {
        NXPLOG_TML_E("_i2c_read() [hdr] errno : %x", errno);
        return -1;
      } else {
        numRead += ret_Read;
      }
    }
    if (TRUE == bFwDnldFlag) {
      totalBtyesToRead =
          pBuffer[FW_DNLD_LEN_OFFSET] + FW_DNLD_HEADER_LEN + CRC_LEN;
    } else {
      totalBtyesToRead =
          pBuffer[NORMAL_MODE_LEN_OFFSET] + NORMAL_MODE_HEADER_LEN;
    }
    wait4interrupt();
    ret_Read = read((intptr_t)pDevHandle, (pBuffer + numRead),
                    totalBtyesToRead - numRead);
    if (ret_Read > 0) {
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("_i2c_read() [pyld] EOF");
      return -1;
    } else {
      if (FALSE == bFwDnldFlag) {
        NXPLOG_TML_E("_i2c_read() [hdr] received");
        phNxpNciHal_print_packet("RECV", pBuffer, NORMAL_MODE_HEADER_LEN);
      }
      NXPLOG_TML_E("_i2c_read() [pyld] errno : %x", errno);
      return -1;
    }
  }
  NXPLOG_TML_D("%s exit", __func__);
  return numRead;
}

/*******************************************************************************
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
*******************************************************************************/
int NfccAltI2cTransport::Write(void* pDevHandle, uint8_t* pBuffer,
                               int nNbBytesToWrite) {
  NXPLOG_TML_D("%s Enter", __func__);
  int ret;
  int numWrote = 0;
  int numBytes = nNbBytesToWrite;
  if (NULL == pDevHandle) {
    return -1;
  }
  if (fragmentation_enabled == I2C_FRAGMENATATION_DISABLED &&
      nNbBytesToWrite > FRAGMENTSIZE_MAX) {
    NXPLOG_TML_E(
        "i2c_write() data larger than maximum I2C  size,enable I2C "
        "fragmentation");
    return -1;
  }
  while (numWrote < nNbBytesToWrite) {
    if (fragmentation_enabled == I2C_FRAGMENTATION_ENABLED &&
        nNbBytesToWrite > FRAGMENTSIZE_MAX) {
      if (nNbBytesToWrite - numWrote > FRAGMENTSIZE_MAX) {
        numBytes = numWrote + FRAGMENTSIZE_MAX;
      } else {
        numBytes = nNbBytesToWrite;
      }
    }
    ret = write((intptr_t)pDevHandle, pBuffer + numWrote, numBytes - numWrote);
    if (ret > 0) {
      numWrote += ret;
      if (fragmentation_enabled == I2C_FRAGMENTATION_ENABLED &&
          numWrote < nNbBytesToWrite) {
        usleep(500);
      }
    } else if (ret == 0) {
      NXPLOG_TML_E("_i2c_write() EOF");
      return -1;
    } else {
      NXPLOG_TML_E("_i2c_write() errno : %x", errno);
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      return -1;
    }
  }
  NXPLOG_TML_D("%s exit", __func__);
  return numWrote;
}

/*******************************************************************************
**
** Function         Close
**
** Description      Closes NFCC device
**
** Parameters       pDevHandle - device handle
**
** Returns          None
**
*******************************************************************************/
void NfccAltI2cTransport::Close(void* pDevHandle) {
  NXPLOG_TML_D("%s Enter", __func__);
  if (NULL != pDevHandle) {
    close((intptr_t)pDevHandle);
  }
  if (iEnableFd) close(iEnableFd);
  if (iInterruptFd) close(iInterruptFd);
  if (iFwDnldFd) close(iFwDnldFd);
  NXPLOG_TML_D("%s exit", __func__);
  return;
}
