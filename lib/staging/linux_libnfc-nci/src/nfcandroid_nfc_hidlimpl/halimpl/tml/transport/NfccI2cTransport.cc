/******************************************************************************
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

#include "phNxpNciHal_utils.h"
#include <NfccI2cTransport.h>
#include <phNfcStatus.h>
#include <phNxpLog.h>
#include <string.h>

#define CRC_LEN 2
#define NORMAL_MODE_HEADER_LEN 3
#define FW_DNLD_HEADER_LEN 2
#define FW_DNLD_LEN_OFFSET 1
#define NORMAL_MODE_LEN_OFFSET 2
#define FRAGMENTSIZE_MAX PHNFC_I2C_FRAGMENT_SIZE
extern phTmlNfc_i2cfragmentation_t fragmentation_enabled;
extern phTmlNfc_Context_t *gpphTmlNfc_Context;
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
void NfccI2cTransport::Close(void *pDevHandle) {
  if (NULL != pDevHandle) {
    close((intptr_t)pDevHandle);
  }
  sem_destroy(&mTxRxSemaphore);
  return;
}

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
NFCSTATUS NfccI2cTransport::OpenAndConfigure(pphTmlNfc_Config_t pConfig,
                                             void **pLinkHandle) {
  int nHandle;

  NXPLOG_TML_D("%s Opening port=%s\n", __func__, pConfig->pDevName);
  /* open port */
  nHandle = open((const char *)pConfig->pDevName, O_RDWR);
  if (nHandle < 0) {
    NXPLOG_TML_E("_i2c_open() Failed: retval %x", nHandle);
    *pLinkHandle = NULL;
    return NFCSTATUS_INVALID_DEVICE;
  }

  *pLinkHandle = (void *)((intptr_t)nHandle);
  if (0 != sem_init(&mTxRxSemaphore, 0, 1)) {
    NXPLOG_TML_E("%s Failed: reason sem_init : retval %x", __func__, nHandle);
  }
  /*Reset PN54X*/
  NfccReset((void *)((intptr_t)nHandle), MODE_POWER_OFF);
  usleep(10 * 1000);
  NfccReset((void *)((intptr_t)nHandle), MODE_POWER_ON);

  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         Flushdata
**
** Description      Reads payload of FW rsp from NFCC device into given buffer
**
** Parameters       pDevHandle - valid device handle
**                  pBuffer    - buffer for read data
**                  numRead    - number of bytes read by calling function
**
** Returns          always returns -1
**
*******************************************************************************/
int NfccI2cTransport::Flushdata(void* pDevHandle, uint8_t* pBuffer, int numRead) {
  int retRead = 0;
  uint16_t totalBtyesToRead = pBuffer[FW_DNLD_LEN_OFFSET] + FW_DNLD_HEADER_LEN + CRC_LEN;
  /* we shall read totalBtyesToRead-1 as one byte is already read by calling function*/
  retRead = read((intptr_t)pDevHandle, pBuffer + numRead, totalBtyesToRead - 1);
  if (retRead > 0) {
    numRead += retRead;
    phNxpNciHal_print_packet("RECV", pBuffer, numRead);
  } else if (retRead == 0) {
    NXPLOG_TML_E("%s _i2c_read() [pyld] EOF", __func__);
  } else {
    if (bFwDnldFlag == false) {
      NXPLOG_TML_D("%s _i2c_read() [hdr] received", __func__);
      phNxpNciHal_print_packet("RECV", pBuffer - numRead, NORMAL_MODE_HEADER_LEN);
    }
    NXPLOG_TML_E("%s _i2c_read() [pyld] errno : %x", __func__, errno);
  }
  SemPost();
  return -1;
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
int NfccI2cTransport::Read(void *pDevHandle, uint8_t *pBuffer,
                           int nNbBytesToRead) {
  int ret_Read;
  int ret_Select;
  int numRead = 0;
  struct timeval tv;
  fd_set rfds;
  uint16_t totalBtyesToRead = 0;

  UNUSED(nNbBytesToRead);
  if (NULL == pDevHandle) {
    return -1;
  }

  if (bFwDnldFlag == false) {
    totalBtyesToRead = NORMAL_MODE_HEADER_LEN;
  } else {
    totalBtyesToRead = FW_DNLD_HEADER_LEN;
  }

  /* Read with 2 second timeout, so that the read thread can be aborted
     when the NFCC does not respond and we need to switch to FW download
     mode. This should be done via a control socket instead. */
  FD_ZERO(&rfds);
  FD_SET((intptr_t)pDevHandle, &rfds);
  tv.tv_sec = 2;
  tv.tv_usec = 1;

  ret_Select =
      select((int)((intptr_t)pDevHandle + (int)1), &rfds, NULL, NULL, &tv);
  if (ret_Select < 0) {
    NXPLOG_TML_D("%s errno : %x", __func__, errno);
    return -1;
  } else if (ret_Select == 0) {
    NXPLOG_TML_D("%s Timeout", __func__);
    return -1;
  } else {
    ret_Read = read((intptr_t)pDevHandle, pBuffer, totalBtyesToRead - numRead);
    if (ret_Read > 0 && !(pBuffer[0] == 0xFF && pBuffer[1] == 0xFF)) {
      SemTimedWait();
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("%s [hdr]EOF", __func__);
      return -1;
    } else {
      NXPLOG_TML_E("%s [hdr] errno : %x", __func__, errno);
      NXPLOG_TML_E(" %s pBuffer[0] = %x pBuffer[1]= %x", __func__, pBuffer[0],
                   pBuffer[1]);
      return -1;
    }

    if (bFwDnldFlag == false) {
      totalBtyesToRead = NORMAL_MODE_HEADER_LEN;
#if(NXP_EXTNS == TRUE)
      if (gpphTmlNfc_Context->tReadInfo.pContext != NULL &&
              !memcmp(gpphTmlNfc_Context->tReadInfo.pContext, "MinOpen", 0x07) &&
              !pBuffer[0] && pBuffer[1]) {
        return Flushdata(pDevHandle, pBuffer, numRead);
      }
#endif
    } else {
      totalBtyesToRead = FW_DNLD_HEADER_LEN;
    }

    if (numRead < totalBtyesToRead) {
      ret_Read = read((intptr_t)pDevHandle, (pBuffer + numRead), totalBtyesToRead - numRead);

      if (ret_Read != totalBtyesToRead - numRead) {
        SemPost();
        NXPLOG_TML_E("%s [hdr] errno : %x", __func__, errno);
        return -1;
      } else {
        numRead += ret_Read;
      }
    }
    if (bFwDnldFlag == true) {
      totalBtyesToRead = pBuffer[FW_DNLD_LEN_OFFSET] + FW_DNLD_HEADER_LEN + CRC_LEN;
    } else {
      totalBtyesToRead = pBuffer[NORMAL_MODE_LEN_OFFSET] + NORMAL_MODE_HEADER_LEN;
    }
    if ((totalBtyesToRead - numRead) != 0) {
      ret_Read = read((intptr_t)pDevHandle, (pBuffer + numRead), totalBtyesToRead - numRead);
      if (ret_Read > 0) {
        numRead += ret_Read;
      } else if (ret_Read == 0) {
        SemPost();
        NXPLOG_TML_E("%s [pyld] EOF", __func__);
        return -1;
      } else {
        if (bFwDnldFlag == false) {
          NXPLOG_TML_D("_i2c_read() [hdr] received");
          phNxpNciHal_print_packet("RECV", pBuffer, NORMAL_MODE_HEADER_LEN);
        }
        SemPost();
        NXPLOG_TML_E("%s [pyld] errno : %x", __func__, errno);
        return -1;
      }
    } else {
      NXPLOG_TML_E("%s _>>>>> Empty packet recieved !!", __func__);
    }
  }
  SemPost();
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
int NfccI2cTransport::Write(void *pDevHandle, uint8_t *pBuffer,
                            int nNbBytesToWrite) {
  int ret;
  int numWrote = 0;
  int numBytes = nNbBytesToWrite;
  if (NULL == pDevHandle) {
    return -1;
  }
  if (fragmentation_enabled == I2C_FRAGMENATATION_DISABLED &&
      nNbBytesToWrite > FRAGMENTSIZE_MAX) {
    NXPLOG_TML_D(
        "%s data larger than maximum I2C  size,enable I2C fragmentation",
        __func__);
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
    SemTimedWait();
    ret = write((intptr_t)pDevHandle, pBuffer + numWrote, numBytes - numWrote);
    SemPost();
    if (ret > 0) {
      numWrote += ret;
      if (fragmentation_enabled == I2C_FRAGMENTATION_ENABLED &&
          numWrote < nNbBytesToWrite) {
        usleep(500);
      }
    } else if (ret == 0) {
      NXPLOG_TML_D("%s EOF", __func__);
      return -1;
    } else {
      NXPLOG_TML_D("%s errno : %x", __func__, errno);
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      return -1;
    }
  }

  return numWrote;
}

/*******************************************************************************
**
** Function         Reset
**
** Description      Reset NFCC device, using VEN pin
**
** Parameters       pDevHandle     - valid device handle
**                  eType          - reset level
**
** Returns           0   - reset operation success
**                  -1   - reset operation failure
**
*******************************************************************************/
int NfccI2cTransport::NfccReset(void *pDevHandle, NfccResetType eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, VEN eType %ld", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }

  ret = ioctl((intptr_t)pDevHandle, NFC_SET_PWR, eType);
  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  //patch applied from latest Android code base
  //which fix firmware download issue
  if ((eType != MODE_FW_DWNLD_WITH_VEN && eType != MODE_FW_DWND_HIGH) &&
      ret == 0) {
    EnableFwDnldMode(false);
  }
  if ((((eType == MODE_FW_DWNLD_WITH_VEN) || (eType == MODE_FW_DWND_HIGH)) && (ret == 0))) {
    EnableFwDnldMode(true);
  }

  return ret;
}

/*******************************************************************************
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
*******************************************************************************/
int NfccI2cTransport::EseReset(void *pDevHandle, EseResetType eType) {
  int ret = -1;
  NXPLOG_TML_D("%s, eType %ld", __func__, eType);

  if (NULL == pDevHandle) {
    return -1;
  }
  ret = ioctl((intptr_t)pDevHandle, ESE_SET_PWR, eType);
  if (ret < 0) {
    NXPLOG_TML_E("%s :failed errno = 0x%x", __func__, errno);
  }
  return ret;
}

/*******************************************************************************
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
*******************************************************************************/
int NfccI2cTransport::EseGetPower(void *pDevHandle, long level) {
  return ioctl((intptr_t)pDevHandle, ESE_GET_PWR, level);
}

/*******************************************************************************
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
*******************************************************************************/
int NfccI2cTransport::GetPlatform(void *pDevHandle) {
  int ret = -1;
  NXPLOG_TML_D("%s ", __func__);
  if (NULL == pDevHandle) {
    return -1;
  }
  ret = ioctl((intptr_t)pDevHandle, NFC_GET_PLATFORM_TYPE);
  NXPLOG_TML_D("%s :platform = %d", __func__, ret);
  return ret;
}

/*******************************************************************************
**
** Function         GetNfcState
**
** Description      Get NFC state
**
** Parameters       pDevHandle     - valid device handle
** Returns           0   - unknown
**                   1   - FW DWL
**                   2 	 - NCI
**
*******************************************************************************/
int NfccI2cTransport::GetNfcState(void *pDevHandle) {
  int ret = NFC_STATE_UNKNOWN;
  NXPLOG_TML_D("%s ", __func__);
  if (NULL == pDevHandle) {
    return ret;
  }
  ret = ioctl((intptr_t)pDevHandle, NFC_GET_NFC_STATE);
  NXPLOG_TML_D("%s :nfc state = %d", __func__, ret);
  return ret;
}
/*******************************************************************************
**
** Function         EnableFwDnldMode
**
** Description      updates the state to Download mode
**
** Parameters       True/False
**
** Returns          None
*******************************************************************************/
void NfccI2cTransport::EnableFwDnldMode(bool mode) { bFwDnldFlag = mode; }

/*******************************************************************************
**
** Function         IsFwDnldModeEnabled
**
** Description      Returns the current mode
**
** Parameters       none
**
** Returns           Current mode download/NCI
*******************************************************************************/
bool_t NfccI2cTransport::IsFwDnldModeEnabled(void) { return bFwDnldFlag; }

/*******************************************************************************
**
** Function         SemPost
**
** Description      sem_post 2c_read / write
**
** Parameters       none
**
** Returns          none
*******************************************************************************/
void NfccI2cTransport::SemPost() {
  int sem_val = 0;
  sem_getvalue(&mTxRxSemaphore, &sem_val);
  if (sem_val == 0) {
    sem_post(&mTxRxSemaphore);
  }
}

/*******************************************************************************
**
** Function         SemTimedWait
**
** Description      Timed sem_wait for avoiding i2c_read & write overlap
**
** Parameters       none
**
** Returns          Sem_wait return status
*******************************************************************************/
int NfccI2cTransport::SemTimedWait() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  long sem_timedout = 500 * 1000 * 1000;
  int s = 0;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 0;
  ts.tv_nsec += sem_timedout;
  while ((s = sem_timedwait(&mTxRxSemaphore, &ts)) == -1 && errno == EINTR) {
    continue; /* Restart if interrupted by handler */
  }
  if (s != -1) {
    status = NFCSTATUS_SUCCESS;
  } else if (errno == ETIMEDOUT && s == -1) {
    NXPLOG_TML_E("%s :timed out errno = 0x%x", __func__, errno);
  }
  return status;
}

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
int NfccI2cTransport::GetIrqState(void *pDevHandle) {
  int ret = -1;

  NXPLOG_TML_D("%s Enter",__func__);
  if (NULL != pDevHandle) {
    ret = ioctl((intptr_t)pDevHandle, NFC_GET_IRQ_STATE);
  }
  NXPLOG_TML_D("%s exit: state = %d", __func__, ret);
  return ret;
}
