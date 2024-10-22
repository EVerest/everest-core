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

#include <NfccAltSpiTransport.h>
#include <phNfcStatus.h>
#include <phNxpLog.h>
#include <string.h>
#include "phNxpNciHal_utils.h"
#include <linux/spi/spidev.h>
#define PREFIX_LENGTH 1
#define SPI_BUS_SPEED 5000000
#define SPI_BITS_PER_WORD 8
#define WRITE_PREFIX_ON_WRITE (0x7F)
#define WRITE_PREFIX_ON_READ (0xFF)
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
NFCSTATUS NfccAltSpiTransport::OpenAndConfigure(pphTmlNfc_Config_t pConfig,
                                                void** pLinkHandle) {
  NXPLOG_TML_D("%s Enter", __func__);
  NXPLOG_TML_D("phTmlNfc_spi_open_and_configure Alternative NFC\n");
  NXPLOG_TML_D("NFC - Assign IO pins\n");
  int status_value = -1;
  int Fd = -1;
  unsigned char spi_mode = SPI_MODE_0;
  unsigned char spi_bitsPerWord = 8;
  static uint32_t speed = SPI_BUS_SPEED;
  // Assign IO pins
  status_value = ConfigurePin();
  if(status_value == -1)
    return NFCSTATUS_INVALID_DEVICE;
  NXPLOG_TML_D("NFCHW - open SPI bus - %s\n", SPI_BUS);

  // SPI bus
  Fd = open(SPI_BUS, O_RDWR | O_NOCTTY);
  if (Fd < 0) {
    NXPLOG_TML_E("Could not open SPI bus '%s' (%s)", SPI_BUS, strerror(errno));
    Close(NULL);
    return (NFCSTATUS_INVALID_DEVICE);
  }
  *pLinkHandle = (void*)((intptr_t)Fd);
  NXPLOG_TML_D("NFC - open SPI device\n");

  // set and get spi mode
  status_value = ioctl(Fd, SPI_IOC_WR_MODE, &spi_mode);
  if (status_value < 0) {
    NXPLOG_TML_E("Could not set SPIMode (WR)...ioctl fail (%s)\n",
                 strerror(errno));
    Close(pLinkHandle);
    return (NFCSTATUS_INVALID_DEVICE);
  }
  status_value = ioctl(Fd, SPI_IOC_RD_MODE, &spi_mode);
  if (status_value < 0) {
    NXPLOG_TML_E("Could not get SPIMode (RD)...ioctl fail (%s)\n",
                 strerror(errno));
    Close(pLinkHandle);
    return (NFCSTATUS_INVALID_DEVICE);
  }
  // set and get bits per word
  status_value = ioctl(Fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
  if (status_value < 0) {
    NXPLOG_TML_E("Could not set SPI bitsPerWord (WR)...ioctl fail (%s)\n",
                 strerror(errno));
    Close(pLinkHandle);
    return (NFCSTATUS_INVALID_DEVICE);
  }

  status_value = ioctl(Fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
  if (status_value < 0) {
    NXPLOG_TML_E("Could not set SPI bitsPerWord (RD)...ioctl fail (%s)\n",
                 strerror(errno));
    Close(pLinkHandle);
    return (NFCSTATUS_INVALID_DEVICE);
  }

  // set and get max speed
  status_value = ioctl(Fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (status_value < 0) {
    NXPLOG_TML_E("Could not set SPI max speed (WR)...ioctl fail (%s)\n",
                 strerror(errno));
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
** Function         SpiRead
**
** Description      Reads requested number of bytes from NFCC device into given
**                  buffer
**
** Parameters       pDevHandle       - valid device handle
**                  pBuffer          - buffer for read data
**                  nBytesToRead   - number of bytes requested to be read
**
** Returns          numRead   - number of successfully read bytes
**                  -1        - read operation failure
**
*******************************************************************************/

static int SpiRead(int pDevHandle, uint8_t* pBuffer, int nBytesToRead) {
  NXPLOG_TML_D("%s Enter", __func__);
  int numRead = 0;
  struct spi_ioc_transfer spi[2];
  char buf = WRITE_PREFIX_ON_READ;
  memset(spi, 0x0, sizeof(spi));
  NXPLOG_TML_D("nBytesToRead=%d pBuffer=%p\n", nBytesToRead, pBuffer);
  spi[0].rx_buf = NULL;                 // receive into "data"
  spi[0].tx_buf = (unsigned long)&buf;  // transmit from "data"
  spi[0].len = PREFIX_LENGTH;
  spi[0].delay_usecs = 0;
  spi[0].speed_hz = SPI_BUS_SPEED;
  spi[0].bits_per_word = SPI_BITS_PER_WORD;
  spi[0].cs_change = 0;  // 0=Set CS high after a transfer, 1=leave CS set low
  spi[0].tx_nbits = 0;
  spi[0].rx_nbits = 0;

  spi[1].tx_buf = NULL;                    // transmit from "data"
  spi[1].rx_buf = (unsigned long)pBuffer;  // receive into "data"
  spi[1].len = nBytesToRead;
  spi[1].delay_usecs = 0;
  spi[1].speed_hz = SPI_BUS_SPEED;
  spi[1].bits_per_word = SPI_BITS_PER_WORD;
  spi[1].cs_change = 0;  // 0=Set CS high after a transfer, 1=leave CS set low

  spi[1].tx_nbits = 0;
  spi[1].rx_nbits = 0;
  numRead = ioctl(pDevHandle, SPI_IOC_MESSAGE(2), &spi);
  if (numRead > 0) numRead -= PREFIX_LENGTH;
  NXPLOG_TML_D("%s exit", __func__);
  return numRead;
}

/*******************************************************************************
**
** Function         Read
**
** Description      Calls SpiRead to read number of bytes from NFCC device into
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
*******************************************************************************/
int NfccAltSpiTransport::Read(void* pDevHandle, uint8_t* pBuffer,
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
    NXPLOG_TML_E("spi select() errno : %x", errno);
    return -1;
  } else if (ret_Select == 0) {
    NXPLOG_TML_E("spi select() Timeout");
    return -1;
  } else {
    wait4interrupt();
    ret_Read =
        SpiRead((intptr_t)pDevHandle, pBuffer, totalBtyesToRead - numRead);
    if (ret_Read > 0) {
      if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFF) {
        NXPLOG_TML_E("_spi_read() could be spurious interrupt");
        return -1;
      }
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("_spi_read() [hdr]EOF");
      return -1;
    } else {
      NXPLOG_TML_E("_spi_read() [hdr] errno : %x", errno);
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
          SpiRead((intptr_t)pDevHandle, pBuffer, totalBtyesToRead - numRead);
      if (ret_Read != totalBtyesToRead - numRead) {
        NXPLOG_TML_E("_spi_read() [hdr] errno : %x", errno);
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
    ret_Read = SpiRead((intptr_t)pDevHandle, (pBuffer + numRead),
                       totalBtyesToRead - numRead);
    if (ret_Read > 0) {
      numRead += ret_Read;
    } else if (ret_Read == 0) {
      NXPLOG_TML_E("_spi_read() [pyld] EOF");
      return -1;
    } else {
      if (FALSE == bFwDnldFlag) {
        NXPLOG_TML_E("_spi_read() [hdr] received");
        phNxpNciHal_print_packet("RECV", pBuffer, NORMAL_MODE_HEADER_LEN);
      }
      NXPLOG_TML_E("_spi_read() [pyld] errno : %x", errno);
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
int NfccAltSpiTransport::Write(void* pDevHandle, uint8_t* pBuffer,
                               int nNbBytesToWrite) {
  NXPLOG_TML_D("%s Enter", __func__);
  int ret;
  int numWrote = 0;
  int numBytes = nNbBytesToWrite;
  struct spi_ioc_transfer spi;
  if (NULL == pDevHandle) {
    return -1;
  }
  if (fragmentation_enabled == I2C_FRAGMENATATION_DISABLED &&
      nNbBytesToWrite > FRAGMENTSIZE_MAX) {
    NXPLOG_TML_E(
        "spi_write() data larger than maximum SPI  size,enable SPI "
        "fragmentation");
    return -1;
  }
  int i = 0;
  while (numWrote < nNbBytesToWrite) {
    i++;
    if (fragmentation_enabled == I2C_FRAGMENTATION_ENABLED &&
        nNbBytesToWrite > FRAGMENTSIZE_MAX) {
      if (nNbBytesToWrite - numWrote > FRAGMENTSIZE_MAX) {
        numBytes = numWrote + FRAGMENTSIZE_MAX;
      } else {
        numBytes = nNbBytesToWrite;
      }
    }
    memset(&spi, 0x0, sizeof(spi));
    char buf = WRITE_PREFIX_ON_WRITE;
    char* tx_buf = (char*)malloc(numBytes - numWrote + PREFIX_LENGTH);
    memset(tx_buf, 0x0, numBytes - numWrote + PREFIX_LENGTH);
    char* rx_buf = (char*)malloc(numBytes - numWrote + PREFIX_LENGTH);
    memset(rx_buf, 0x0, numBytes - numWrote + PREFIX_LENGTH);
    memcpy(&tx_buf[0], &buf, PREFIX_LENGTH);
    memcpy(&tx_buf[1], pBuffer + numWrote, numBytes - numWrote);
    spi.tx_buf = (unsigned long)tx_buf;  // transmit from "data"
    spi.rx_buf = (unsigned long)rx_buf;  // receive into "data"
    spi.len = numBytes - numWrote + PREFIX_LENGTH;
    spi.delay_usecs = 0;
    spi.speed_hz = SPI_BUS_SPEED;
    spi.bits_per_word = SPI_BITS_PER_WORD;
    spi.tx_nbits = 0;
    spi.rx_nbits = 0;
    spi.cs_change = 0;  // 0=Set CS high after a transfer, 1=leave CS set low
    char mode = SPI_MODE_0;
    ret = ioctl((intptr_t)pDevHandle, SPI_IOC_MESSAGE(1), &spi);
    if (ret > 0 && rx_buf[0] == 0xFF) {
      numWrote += ret;
      if (fragmentation_enabled == I2C_FRAGMENTATION_ENABLED &&
          numWrote < nNbBytesToWrite) {
        usleep(500);
      }
    } else if (ret == 0) {
      NXPLOG_TML_E("_spi_write() EOF");
      return -1;
    } else {
      NXPLOG_TML_E("_spi_write() errno : %x", errno);
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }
      return -1;
    }
  }
  NXPLOG_TML_D("%s exit", __func__);
  return numWrote - PREFIX_LENGTH;
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
void NfccAltSpiTransport::Close(void* pDevHandle) {
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
