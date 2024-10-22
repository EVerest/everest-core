/*
 * Copyright (C) 2010-2021 NXP Semiconductors
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

/*
 * Transport Mapping Layer header files containing APIs related to initializing,
 * reading
 * and writing data into files provided by the driver interface.
 *
 * API listed here encompasses Transport Mapping Layer interfaces required to be
 * mapped
 * to different Interfaces and Platforms.
 *
 */

#ifndef PHTMLNFC_H
#define PHTMLNFC_H

#include <phNfcCommon.h>

/*
 * Message posted by Reader thread upon
 * completion of requested operation
 */
#define PH_TMLNFC_READ_MESSAGE (0xAA)

/*
 * Message posted by Writer thread upon
 * completion of requested operation
 */
#define PH_TMLNFC_WRITE_MESSAGE (0x55)

/*
 * Value indicates to reset device
 */
#define PH_TMLNFC_RESETDEVICE (0x00008001)

/*
***************************Globals,Structure and Enumeration ******************
*/

/*
 * Transaction (Tx/Rx) completion information structure of TML
 *
 * This structure holds the completion callback information of the
 * transaction passed from the TML layer to the Upper layer
 * along with the completion callback.
 *
 * The value of field wStatus can be interpreted as:
 *
 *     - NFCSTATUS_SUCCESS                    Transaction performed
 * successfully.
 *     - NFCSTATUS_FAILED                     Failed to wait on Read/Write
 * operation.
 *     - NFCSTATUS_INSUFFICIENT_STORAGE       Not enough memory to store data in
 * case of read.
 *     - NFCSTATUS_BOARD_COMMUNICATION_ERROR  Failure to Read/Write from the
 * file or timeout.
 */

typedef struct phTmlNfc_TransactInfo {
  NFCSTATUS wStatus;       /* Status of the Transaction Completion*/
  uint8_t* pBuff;          /* Response Data of the Transaction*/
  uint16_t wLength;        /* Data size of the Transaction*/
} phTmlNfc_TransactInfo_t; /* Instance of Transaction structure */

/*nfc state flags*/
enum nfc_state_flags {
  /*nfc in unknown state */
  NFC_STATE_UNKNOWN = 0,
  /*nfc booted in download mode */
  NFC_STATE_FW_DWL = 0x1,
  /*nfc booted in NCI mode */
  NFC_STATE_NCI = 0x2,
};
/*
 * TML transreceive completion callback to Upper Layer
 *
 * pContext - Context provided by upper layer
 * pInfo    - Transaction info. See phTmlNfc_TransactInfo
 */
typedef void (*pphTmlNfc_TransactCompletionCb_t)(
    void* pContext, phTmlNfc_TransactInfo_t* pInfo);

/*
 * TML Deferred callback interface structure invoked by upper layer
 *
 * This could be used for read/write operations
 *
 * dwMsgPostedThread Message source identifier
 * pParams Parameters for the deferred call processing
 */
typedef void (*pphTmlNfc_DeferFuncPointer_t)(uint32_t dwMsgPostedThread,
                                             void* pParams);

/*
 * Enum definition contains  supported ioctl control codes.
 *
 * phTmlNfc_IoCtl
 */
typedef enum {
  phTmlNfc_e_Invalid = 0,
  phTmlNfc_e_ResetDevice = PH_TMLNFC_RESETDEVICE, /* Reset the device */
  phTmlNfc_e_EnableDownloadMode, /* Do the hardware setting to enter into
                                    download mode */
  phTmlNfc_e_EnableNormalMode /* Hardware setting for normal mode of operation
                                 */
} phTmlNfc_ControlCode_t;     /* Control code for IOCTL call */

/*
 * Enable / Disable Re-Transmission of Packets
 *
 * phTmlNfc_ConfigNciPktReTx
 */
typedef enum {
  phTmlNfc_e_EnableRetrans = 0x00, /*Enable retransmission of Nci packet */
  phTmlNfc_e_DisableRetrans = 0x01 /*Disable retransmission of Nci packet */
} phTmlNfc_ConfigRetrans_t;        /* Configuration for Retransmission */

/*
 * Structure containing details related to read and write operations
 *
 */
typedef struct phTmlNfc_ReadWriteInfo {
  volatile uint8_t bEnable; /*This flag shall decide whether to perform
                               Write/Read operation */
  uint8_t
      bThreadBusy; /*Flag to indicate thread is busy on respective operation */
  /* Transaction completion Callback function */
  pphTmlNfc_TransactCompletionCb_t pThread_Callback;
  void* pContext;        /*Context passed while invocation of operation */
  uint8_t* pBuffer;      /*Buffer passed while invocation of operation */
  uint16_t wLength;      /*Length of data read/written */
  NFCSTATUS wWorkStatus; /*Status of the transaction performed */
} phTmlNfc_ReadWriteInfo_t;

/*
 *Base Context Structure containing members required for entire session
 */
typedef struct phTmlNfc_Context {
  pthread_t readerThread; /*Handle to the thread which handles write and read
                             operations */
  pthread_t writerThread;
  volatile uint8_t
      bThreadDone; /*Flag to decide whether to run or abort the thread */
  phTmlNfc_ConfigRetrans_t
      eConfig;             /*Retransmission of Nci Packet during timeout */
  uint8_t bRetryCount;     /*Number of times retransmission shall happen */
  uint8_t bWriteCbInvoked; /* Indicates whether write callback is invoked during
                              retransmission */
  uint32_t dwTimerId;      /* Timer used to retransmit nci packet */
  phTmlNfc_ReadWriteInfo_t tReadInfo;  /*Pointer to Reader Thread Structure */
  phTmlNfc_ReadWriteInfo_t tWriteInfo; /*Pointer to Writer Thread Structure */
  void* pDevHandle;                    /* Pointer to Device Handle */
  uintptr_t dwCallbackThreadId; /* Thread ID to which message to be posted */
  uint8_t bEnableCrc;           /*Flag to validate/not CRC for input buffer */
  sem_t rxSemaphore;
  sem_t txSemaphore;      /* Lock/Aquire txRx Semaphore */
  sem_t postMsgSemaphore; /* Semaphore to post message atomically by Reader &
                             writer thread */
  pthread_mutex_t readInfoUpdateMutex; /*Mutex to synchronize read Info update*/
} phTmlNfc_Context_t;

/*
 * TML Configuration exposed to upper layer.
 */
typedef struct phTmlNfc_Config {
  /* Port name connected to PN54X
   *
   * Platform specific canonical device name to which PN54X is connected.
   *
   * e.g. On Linux based systems this would be /dev/PN54X
   */
  int8_t* pDevName;
  /* Callback Thread ID
   *
   * This is the thread ID on which the Reader & Writer thread posts message. */
  uintptr_t dwGetMsgThreadId;
  /* Communication speed between DH and PN54X
   *
   * This is the baudrate of the bus for communication between DH and PN54X */
  uint32_t dwBaudRate;
} phTmlNfc_Config_t, *pphTmlNfc_Config_t; /* pointer to phTmlNfc_Config_t */

/*
 * TML Deferred Callback structure used to invoke Upper layer Callback function.
 */
typedef struct {
  /* Deferred callback function to be invoked */
  pphTmlNfc_DeferFuncPointer_t pDef_call;
  /* Source identifier
   *
   * Identifier of the source which posted the message
   */
  uint32_t dwMsgPostedThread;
  /** Actual Message
   *
   * This is passed as a parameter passed to the deferred callback function
   * pDef_call. */
  void* pParams;
} phTmlNfc_DeferMsg_t; /* DeferMsg structure passed to User Thread */

typedef enum {
  I2C_FRAGMENATATION_DISABLED, /*i2c fragmentation_disabled           */
  I2C_FRAGMENTATION_ENABLED    /*i2c_fragmentation_enabled          */
} phTmlNfc_i2cfragmentation_t;
/* Function declarations */
NFCSTATUS phTmlNfc_Init(pphTmlNfc_Config_t pConfig);
NFCSTATUS phTmlNfc_Shutdown(void);
NFCSTATUS phTmlNfc_Shutdown_CleanUp();
void phTmlNfc_CleanUp(void);
NFCSTATUS phTmlNfc_Write(uint8_t* pBuffer, uint16_t wLength,
                         pphTmlNfc_TransactCompletionCb_t pTmlWriteComplete,
                         void* pContext);
NFCSTATUS phTmlNfc_Read(uint8_t* pBuffer, uint16_t wLength,
                        pphTmlNfc_TransactCompletionCb_t pTmlReadComplete,
                        void* pContext);
NFCSTATUS phTmlNfc_WriteAbort(void);
NFCSTATUS phTmlNfc_ReadAbort(void);
NFCSTATUS phTmlNfc_IoCtl(phTmlNfc_ControlCode_t eControlCode);
NFCSTATUS phTmlNfc_UpdateReadCompleteCallback (
    pphTmlNfc_TransactCompletionCb_t pTmlReadComplete);
void phTmlNfc_DeferredCall(uintptr_t dwThreadId,
                           phLibNfc_Message_t* ptWorkerMsg);
void phTmlNfc_ConfigNciPktReTx(phTmlNfc_ConfigRetrans_t eConfig,
                               uint8_t bRetryCount);
void phTmlNfc_set_fragmentation_enabled(phTmlNfc_i2cfragmentation_t enable);
phTmlNfc_i2cfragmentation_t phTmlNfc_get_fragmentation_enabled();
NFCSTATUS phTmlNfc_ConfigTransport();
#endif /*  PHTMLNFC_H  */
