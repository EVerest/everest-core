/******************************************************************************
 *
 *  Copyright 2019-2021 NXP
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
#if (NXP_EXTNS == TRUE)
#include "NativeT4tNfcee.h"
#include <android-base/stringprintf.h>
#include <base/logging.h>
#ifndef LINUX
#include <nativehelper/ScopedPrimitiveArray.h>
#endif
#include "NfcJniUtil.h"
#include "nci_defs_extns.h"
#include "nfa_nfcee_api.h"
#include "nfa_nfcee_int.h"
#include "nfc_config.h"

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

/*Considering NCI response timeout which is 2s, Timeout set 100ms more*/
#define T4TNFCEE_TIMEOUT 2100
#define T4TOP_TIMEOUT 200
#define FILE_ID_LEN 0x02

extern bool gActivated;

#ifndef LINUX
namespace android {
extern bool isDiscoveryStarted();
extern void startRfDiscovery(bool isStart);
extern bool nfcManager_isNfcActive();
extern tNFA_STATUS getConfig(uint16_t* len, uint8_t* configValue,
                             uint8_t numParam, tNFA_PMID* param);
extern tNFA_STATUS NxpNfc_Write_Cmd_Common(uint8_t retlen, uint8_t* buffer);
extern int nfcManager_doPartialInitialize(JNIEnv* e, jobject o, jint mode);
extern int nfcManager_doPartialDeInitialize(JNIEnv*, jobject);
}  // namespace android

#else
extern Mutex         gSyncMutex;
extern void startRfDiscovery(BOOLEAN isStart);
extern bool isDiscoveryStarted();
extern bool nfcManager_isNfcActive();
#endif

NativeT4tNfcee NativeT4tNfcee::sNativeT4tNfceeInstance;
bool NativeT4tNfcee::sIsNfcOffTriggered = false;

NativeT4tNfcee::NativeT4tNfcee() { mBusy = false; memset (&mReadData, 0x00, sizeof(tNFA_RX_DATA)); mT4tOpStatus = NFA_STATUS_FAILED; }

/*****************************************************************************
**
** Function:        getInstance
**
** Description:     Get the NativeT4tNfcee singleton object.
**
** Returns:         NativeT4tNfcee object.
**
*******************************************************************************/
NativeT4tNfcee& NativeT4tNfcee::getInstance() {
  return sNativeT4tNfceeInstance;
}

/*******************************************************************************
**
** Function:        initialize
**
** Description:     Initialize all member variables.
**
** Returns:         None.
**
*******************************************************************************/
void NativeT4tNfcee::initialize(void) {
  sIsNfcOffTriggered = false;
  mBusy = false;
}

/*****************************************************************************
**
** Function:        onNfccShutdown
**
** Description:     This api shall be called in NFC OFF case.
**
** Returns:         none.
**
*******************************************************************************/
void NativeT4tNfcee::onNfccShutdown() {
  sIsNfcOffTriggered = true;
  if(mBusy) {
    /* Unblock JNI APIs */
    {
      SyncEventGuard g(mT4tNfcOffEvent);
      if (mT4tNfcOffEvent.wait(T4TOP_TIMEOUT) == false) {
        SyncEventGuard ga(mT4tNfcEeRWEvent);
        mT4tNfcEeRWEvent.notifyOne();
      }
    }
    /* Try to close the connection with t4t nfcee, discard the status */
    (void)closeConnection();
    resetBusy();
  }
}
/*******************************************************************************
**
** Function:        t4tClearData
**
** Description:     This API will set all the T4T NFCEE NDEF data to zero.
**                  This API can be called regardless of NDEF file lock state.
**
** Returns:         boolean : Return the Success or fail of the operation.
**                  Return "True" when operation is successful. else "False"
**
*******************************************************************************/
#ifndef LINUX
jboolean NativeT4tNfcee::t4tClearData(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s:Enter: ", __func__);

  /*Local variable Initalization*/
  uint8_t pFileId[] = {0xE1, 0x04};
  jbyteArray fileIdArray = e->NewByteArray(sizeof(pFileId));
  e->SetByteArrayRegion(fileIdArray, 0, sizeof(pFileId), (jbyte*)pFileId);
  bool clear_status = false;

  /*Validate Precondition*/
  T4TNFCEE_STATUS_t t4tNfceeStatus =
      validatePreCondition(OP_CLEAR, fileIdArray);

  switch (t4tNfceeStatus) {
    case STATUS_SUCCESS:
      /*NFC is ON*/
      clear_status = performT4tClearData(pFileId);
      break;
    case ERROR_NFC_NOT_ON:
      /*NFC is OFF*/
      // if (android::nfcManager_doPartialInitialize(e, o, NFA_MINIMUM_BOOT_MODE) ==
      //     NFA_STATUS_OK) {
        NativeT4tNfcee::getInstance().initialize();
        clear_status = performT4tClearData(pFileId);
      //   android::nfcManager_doPartialDeInitialize(NULL, NULL);
      // }
      break;
    default:
      DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf(
          "%s:Exit: Returnig status : %d", __func__, clear_status);
      break;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s:Exit: ", __func__);
  return clear_status;
}
/*******************************************************************************
**
** Function:        performT4tClearData
**
** Description:     This api clear the T4T Nfcee data
**
** Returns:         boolean : Return the Success or fail of the operation.
**                  Return "True" when operation is successful. else "False"
**
*******************************************************************************/
jboolean NativeT4tNfcee::performT4tClearData(uint8_t* fileId) {
  bool t4tClearReturn = false;
  tNFA_STATUS status = NFA_STATUS_FAILED;

  /*Open connection and stop discovery*/
  if (setup() != NFA_STATUS_OK) return t4tClearReturn;

  /*Clear Ndef data*/
  SyncEventGuard g(mT4tNfcEeClrDataEvent);
  status = NFA_T4tNfcEeClear(fileId);
  if (status == NFA_STATUS_OK) {
    if (mT4tNfcEeClrDataEvent.wait(T4TNFCEE_TIMEOUT) == false)
      t4tClearReturn = false;
    else {
      if (mT4tOpStatus == NFA_STATUS_OK) {
        t4tClearReturn = true;
      }
    }
  }

  /*Close connection and start discovery*/
  cleanup();
  return t4tClearReturn;
}
#endif
/*******************************************************************************
**
** Function:        t4tWriteData
**
** Description:     Write the data into the T4T file of the specific file ID
**
** Returns:         Return the size of data written
**                  Return negative number of error code
**
*******************************************************************************/
#ifdef LINUX
int NativeT4tNfcee::t4tWriteData(uint8_t* fileId, uint8_t* data,
                                     int length) {
  jbyteArray buf = fileId;
  Bytes bytes(buf, FILE_ID_LEN);

  jbyteArray buf1= data;
  Bytes bytesData(buf1, length);
#else
jint NativeT4tNfcee::t4tWriteData(JNIEnv* e, jobject object, jbyteArray fileId,
                                  jbyteArray data, int length) {
#endif
  tNFA_STATUS status = NFA_STATUS_FAILED;

  T4TNFCEE_STATUS_t t4tNfceeStatus =
      validatePreCondition(OP_WRITE, fileId, data);
  if (t4tNfceeStatus != STATUS_SUCCESS) return t4tNfceeStatus;
#ifndef LINUX
  ScopedByteArrayRO bytes(e, fileId);
  if (bytes.size() < FILE_ID_LEN) {
    DLOG_IF(ERROR, nfc_debug_enabled)
        << StringPrintf("%s:Wrong File Id", __func__);
    return ERROR_INVALID_FILE_ID;
  }

  ScopedByteArrayRO bytesData(e, data);
  if (bytesData.size() == 0x00) {
    DLOG_IF(ERROR, nfc_debug_enabled)
        << StringPrintf("%s:Empty Data", __func__);
    return ERROR_EMPTY_PAYLOAD;
  }

  if ((int)bytesData.size() != length) {
    DLOG_IF(ERROR, nfc_debug_enabled)
        << StringPrintf("%s:Invalid Length", __func__);
    return ERROR_INVALID_LENGTH;
  }
#endif
  if (setup() != NFA_STATUS_OK) return ERROR_CONNECTION_FAILED;
#ifdef LINUX
  gSyncMutex.lock();
  uint8_t* pFileId = buf;
  uint8_t* pData = buf1;
#else
  uint8_t* pFileId = NULL;
  pFileId = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));

  uint8_t* pData = NULL;
  pData = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytesData[0]));
#endif
  jint t4tWriteReturn = STATUS_FAILED;
  {
    SyncEventGuard g(mT4tNfcEeRWEvent);
    status = NFA_T4tNfcEeWrite(pFileId, pData, bytesData.size());
    if (status == NFA_STATUS_OK) {
      if (mT4tNfcEeRWEvent.wait(T4TNFCEE_TIMEOUT) == false)
        t4tWriteReturn = STATUS_FAILED;
      else {
        if (mT4tOpStatus == NFA_STATUS_OK) {
          /*if status is success then return length of data written*/
          t4tWriteReturn = mReadData.len;
        } else if (mT4tOpStatus == NFA_STATUS_REJECTED) {
          t4tWriteReturn = ERROR_NDEF_VALIDATION_FAILED;
        } else if (mT4tOpStatus == NFA_T4T_STATUS_INVALID_FILE_ID){
          t4tWriteReturn = ERROR_INVALID_FILE_ID;
        } else if (mT4tOpStatus == NFA_STATUS_READ_ONLY) {
          t4tWriteReturn = ERROR_WRITE_PERMISSION;
        } else {
          t4tWriteReturn = STATUS_FAILED;
        }
      }
    }
  }

  /*Close connection and start discovery*/
  cleanup();
  DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf(
      "%s:Exit: Returnig status : %d", __func__, t4tWriteReturn);
#ifdef LINUX
  gSyncMutex.unlock();
  if((t4tWriteReturn > 0) && (t4tWriteReturn == length)) {
    return NFA_STATUS_OK;
  }
#endif
  return t4tWriteReturn;
}

/*******************************************************************************
**
** Function:        t4tReadData
**
** Description:     Read the data from the T4T file of the specific file ID.
**
** Returns:         byte[] : all the data previously written to the specific
**                  file ID.
**                  Return one byte '0xFF' if the data was never written to the
**                  specific file ID,
**                  Return null if reading fails.
**
*******************************************************************************/
#ifdef LINUX
int NativeT4tNfcee::t4tReadData(uint8_t* ndefBuffer,  int *ndefBufferLength,
                            uint8_t* fileId) {
  jbyteArray buf = fileId;
  Bytes bytes(buf, FILE_ID_LEN);
#else
jbyteArray NativeT4tNfcee::t4tReadData(JNIEnv* e, jobject object,
                                       jbyteArray fileId) {
#endif
  tNFA_STATUS status = NFA_STATUS_FAILED;

  T4TNFCEE_STATUS_t t4tNfceeStatus = validatePreCondition(OP_READ, fileId);
  #ifdef LINUX
  gSyncMutex.lock();
  if (t4tNfceeStatus != STATUS_SUCCESS) {
    gSyncMutex.unlock();
    return NFA_STATUS_FAILED;
  }

  #else
  if (t4tNfceeStatus != STATUS_SUCCESS) return NULL;

  ScopedByteArrayRO bytes(e, fileId);
  ScopedLocalRef<jbyteArray> result(e, NULL);

  if (bytes.size() < FILE_ID_LEN) {
    DLOG_IF(ERROR, nfc_debug_enabled)
        << StringPrintf("%s:Wrong File Id", __func__);
    return NULL;
  }
  #endif
  if (setup() != NFA_STATUS_OK) return NULL;

  #ifdef LINUX
  uint8_t* pFileId = buf;
  #else
  uint8_t* pFileId = NULL;
  pFileId = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  #endif
  { /*syncEvent code section*/
    SyncEventGuard g(mT4tNfcEeRWEvent);
    sRxDataBuffer.clear();
    status = NFA_T4tNfcEeRead(pFileId);
    if ((status != NFA_STATUS_OK) ||
        (mT4tNfcEeRWEvent.wait(T4TNFCEE_TIMEOUT) == false)) {
      DLOG_IF(ERROR, nfc_debug_enabled)
          << StringPrintf("%s:Read Failed, status = 0x%X", __func__, status);
      cleanup();
#ifdef LINUX
      gSyncMutex.unlock();
      return NFA_STATUS_FAILED;
#else
      return NULL;
#endif
    }
  }

  if (sRxDataBuffer.size() > 0) {
#ifdef LINUX
   if (*ndefBufferLength > sRxDataBuffer.size()) {
      *ndefBufferLength = sRxDataBuffer.size();
    }
    memcpy(ndefBuffer, sRxDataBuffer.data(), *ndefBufferLength);
#else
    result.reset(e->NewByteArray(sRxDataBuffer.size()));
    if (result.get() != NULL) {
      e->SetByteArrayRegion(result.get(), 0, sRxDataBuffer.size(),
            (const jbyte*)sRxDataBuffer.data());
    } else {
      char data[1] = {0xFF};
      result.reset(e->NewByteArray(0x01));
      e->SetByteArrayRegion(result.get(), 0, 0x01, (jbyte*)data);
      LOG(ERROR) << StringPrintf("%s: Failed to allocate java byte array",
               __func__);
    }
#endif
    sRxDataBuffer.clear();
  } else if (mT4tOpStatus == NFA_T4T_STATUS_INVALID_FILE_ID){
#ifndef LINUX
    char data[1] = {0xFF};
    result.reset(e->NewByteArray(0x01));
    e->SetByteArrayRegion(result.get(), 0, 0x01, (jbyte*)data);
#endif
  }
  /*Close connection and start discovery*/
  cleanup();
  #ifdef LINUX
  gSyncMutex.unlock();
  return NFA_STATUS_OK;
  #else
  return result.release();
  #endif
}

/*******************************************************************************
**
** Function:        openConnection
**
** Description:     Open T4T Nfcee Connection
**
** Returns:         Status
**
*******************************************************************************/
tNFA_STATUS NativeT4tNfcee::openConnection() {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
  SyncEventGuard g(mT4tNfcEeEvent);
  status = NFA_T4tNfcEeOpenConnection();
  if (status == NFA_STATUS_OK) {
    if (mT4tNfcEeEvent.wait(T4TNFCEE_TIMEOUT) == false)
      status = NFA_STATUS_FAILED;
    else
      status = mT4tNfcEeEventStat;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Exit status = 0x%02x", __func__, status);
  return status;
}

/*******************************************************************************
**
** Function:        closeConnection
**
** Description:     Close T4T Nfcee Connection
**
** Returns:         Status
**
*******************************************************************************/
tNFA_STATUS NativeT4tNfcee::closeConnection() {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
  {
    SyncEventGuard g(mT4tNfcEeEvent);
    status = NFA_T4tNfcEeCloseConnection();
    if (status == NFA_STATUS_OK) {
      if (mT4tNfcEeEvent.wait(T4TNFCEE_TIMEOUT) == false)
        status = NFA_STATUS_FAILED;
      else
        status = mT4tNfcEeEventStat;
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Exit status = 0x%02x", __func__, status);
  return status;
}

/*******************************************************************************
**
** Function:        setup
**
** Description:     stops Discovery and opens T4TNFCEE connection
**
** Returns:         Status
**
*******************************************************************************/
tNFA_STATUS NativeT4tNfcee::setup(void) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  setBusy();

#ifndef LINUX
  if (android::isDiscoveryStarted()) {
    android::startRfDiscovery(false);
  }
#else
  if(isDiscoveryStarted()) {
    startRfDiscovery(false);
  }
#endif

  status = openConnection();
  if (status != NFA_STATUS_OK) {
    DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf(
        "%s: openConnection Failed, status = 0x%X", __func__, status);
#ifndef LINUX
    if (!android::isDiscoveryStarted()) android::startRfDiscovery(true);
#else
    if (!isDiscoveryStarted()) startRfDiscovery(true);
  #endif
    resetBusy();
  }
  return status;
}
/*******************************************************************************
**
** Function:        cleanup
**
** Description:     closes connection and starts discovery
**
** Returns:         Status
**
*******************************************************************************/
void NativeT4tNfcee::cleanup(void) {

  if(sIsNfcOffTriggered) {
    SyncEventGuard g(mT4tNfcOffEvent);
    mT4tNfcOffEvent.notifyOne();
    DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf("%s: Nfc Off triggered", __func__);
    return;
  }
  if (closeConnection() != NFA_STATUS_OK) {
    DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf("%s: closeConnection Failed", __func__);
  }
#ifndef LINUX
  if (!android::isDiscoveryStarted() ) {
    android::startRfDiscovery(true);
  }
#else
  if (!isDiscoveryStarted() ) {
    startRfDiscovery(true);
  }
#endif
  resetBusy();
}

/*******************************************************************************
**
** Function:        validatePreCondition
**
** Description:     Runs precondition checks for requested operation
**
** Returns:         Status
**
*******************************************************************************/
T4TNFCEE_STATUS_t NativeT4tNfcee::validatePreCondition(T4TNFCEE_OPERATIONS_t op,
                                                       jbyteArray fileId,
                                                       jbyteArray data) {
  T4TNFCEE_STATUS_t t4tNfceeStatus = STATUS_SUCCESS;
#ifndef LINUX
  if (!android::nfcManager_isNfcActive()) {
#else
  if (!nfcManager_isNfcActive()) {
#endif
    t4tNfceeStatus = ERROR_NFC_NOT_ON;
  } else if (sIsNfcOffTriggered) {
    t4tNfceeStatus = ERROR_NFC_OFF_TRIGGERED;
  }else if (gActivated) {
    t4tNfceeStatus = ERROR_RF_ACTIVATED;
  } else if (fileId == NULL) {
    DLOG_IF(ERROR, nfc_debug_enabled)
        << StringPrintf("%s:Invalid File Id", __func__);
    t4tNfceeStatus = ERROR_INVALID_FILE_ID;
  }

  switch (op) {
    case OP_READ:
      break;
    case OP_WRITE:
      if (data == NULL) {
        DLOG_IF(ERROR, nfc_debug_enabled)
            << StringPrintf("%s:Empty data", __func__);
        t4tNfceeStatus = ERROR_EMPTY_PAYLOAD;
      }
      break;
    case OP_LOCK:
      if (t4tNfceeStatus != STATUS_SUCCESS) break;
      if (!isNdefWritePermission()) t4tNfceeStatus = ERROR_WRITE_PERMISSION;
      break;
    case OP_CLEAR:
    [[fallthrough]];
    default:
      break;
  }
  return t4tNfceeStatus;
}

/*******************************************************************************
**
** Function:        t4tReadComplete
**
** Description:     Updates read data to the waiting READ API
**
** Returns:         none
**
*******************************************************************************/
void NativeT4tNfcee::t4tReadComplete(tNFA_STATUS status, tNFA_RX_DATA data) {
  mT4tOpStatus = status;
  if (status == NFA_STATUS_OK) {
    if(data.len > 0) {
      sRxDataBuffer.append(data.p_data, data.len);
      DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Read Data len new: %d ", __func__, data.len);
    }
  }
  SyncEventGuard g(mT4tNfcEeRWEvent);
  mT4tNfcEeRWEvent.notifyOne();
}

/*******************************************************************************
 **
 ** Function:        t4tWriteComplete
 **
 ** Description:     Returns write complete information
 **
 ** Returns:         none
 **
 *******************************************************************************/
void NativeT4tNfcee::t4tWriteComplete(tNFA_STATUS status, tNFA_RX_DATA data) {
  mReadData.len = 0x00;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
  if (status == NFA_STATUS_OK) mReadData.len = data.len;
  mT4tOpStatus = status;
  SyncEventGuard g(mT4tNfcEeRWEvent);
  mT4tNfcEeRWEvent.notifyOne();
}
/*******************************************************************************
 **
 ** Function:        t4tClearComplete
 **
 ** Description:     Update T4T clear data status, waiting T4tClearData API.
 **
 ** Returns:         none
 **
 *******************************************************************************/
void NativeT4tNfcee::t4tClearComplete(tNFA_STATUS status) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
  mT4tOpStatus = status;
  SyncEventGuard g(mT4tNfcEeClrDataEvent);
  mT4tNfcEeClrDataEvent.notifyOne();
}
/*******************************************************************************
**
** Function:        t4tNfceeEventHandler
**
** Description:     Handles callback events received from lower layer
**
** Returns:         none
**
*******************************************************************************/
void NativeT4tNfcee::eventHandler(uint8_t event,
                                  tNFA_CONN_EVT_DATA* eventData) {
  switch (event) {
    case NFA_T4TNFCEE_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_T4TNFCEE_EVT", __func__);
      {
        SyncEventGuard guard(mT4tNfcEeEvent);
        mT4tNfcEeEventStat = eventData->status;
        mT4tNfcEeEvent.notifyOne();
      }
      break;

    case NFA_T4TNFCEE_READ_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_T4TNFCEE_READ_CPLT_EVT", __func__);
      t4tReadComplete(eventData->status, eventData->data);
      break;

    case NFA_T4TNFCEE_WRITE_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_T4TNFCEE_WRITE_CPLT_EVT", __func__);
      t4tWriteComplete(eventData->status, eventData->data);
      break;

    case NFA_T4TNFCEE_CLEAR_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_T4TNFCEE_CLEAR_CPLT_EVT", __func__);
      t4tClearComplete(eventData->status);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown Event", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        doChangeT4tFileWritePerm
**
** Description:     Set/Reset the lock bit for contact or/and contact less NDEF files.
**
** Parameter:       param_val: Reference to a value which shall be modified by this API
**                  const bool& lock : Informs about how to modify the param_val
**
** Returns:         boolean : "True" if param_val is modified else "False"
**
*******************************************************************************/
bool NativeT4tNfcee::doChangeT4tFileWritePerm(uint8_t& param_val, const bool& lock) {
  bool status = false;
  if (lock) { /* Disable the lock bit*/
    if (param_val & (1 << MASK_LOCK_BIT)) {
      param_val &= ~(1 << MASK_LOCK_BIT); /* Reset bit6 to disable write permission */
      status = true;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Lock bit is already disable",__func__);
    }
  } else { /* Enable the lock bit*/
    if (!(param_val & (1 << MASK_LOCK_BIT))) {
      param_val |= (1 << MASK_LOCK_BIT); /* Set bit6 to enable write permission */
      status = true;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Lock bit is already set", __func__);
    }
  }
  return status;
}

#ifndef LINUX
/*******************************************************************************
**
** Function:        doGetT4tConfVals
**
** Description:     This function gets the T4T config values from NFCC.
**
** Parameter:       uint8_t& clNdefFileValue: reference variable to hold value of
**                                        contactless (A095) tag
**                  uint8_t& cNdefFileValue : reference variable to hold value of
**                                        contact (A110) tag
** Returns:         "TRUE" if value is successfully retrieved
**                  "FALSE" if error occurred or T4T feature is disabled
**
*******************************************************************************/
bool NativeT4tNfcee::doGetT4tConfVals(uint8_t& clNdefFileValue, uint8_t& cNdefFileValue) {
  tNFA_PMID t4tNfcEeNdef[] = { NXP_NFC_SET_CONFIG_PARAM_EXT, NXP_NFC_CLPARAM_ID_T4T_NFCEE,
          NXP_NFC_SET_CONFIG_PARAM_EXT_ID1, NXP_NFC_CPARAM_ID_T4T_NFCEE };
  uint8_t configValue[MAX_CONFIG_VALUE_LEN] = {0};
  tNFA_STATUS status = NFA_STATUS_FAILED;
  uint16_t rspLen = 0;

  status = android::getConfig(&rspLen, configValue, NXP_NFC_NUM_PARAM_T4T_NFCEE, t4tNfcEeNdef);
  if(rspLen == 0x0A) { /* Payload len of Get config for A095 & A110 */
    clNdefFileValue = *(configValue + NXP_PARAM_GET_CONFIG_INDEX);
    cNdefFileValue = *(configValue + NXP_PARAM_GET_CONFIG_INDEX1);
  }
  if ((status != NFA_STATUS_OK) || !(clNdefFileValue & MASK_T4T_FEATURE_BIT)) {
    return false;
  }
  return true;
}

/*******************************************************************************
**
** Function:        doLockT4tData
**
** Description:     Lock/Unlock the data in the T4T NDEF file.
**
** Parameter:       boolean lock : True(lock) or False(unlock)
**
** Returns:         boolean : Return the Success or fail of the operation.
**                  Return "True" when operation is successful. else "False"
**
*******************************************************************************/
bool NativeT4tNfcee::doLockT4tData(JNIEnv* e, jobject o, bool lock) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter %d", __func__, lock);

  uint8_t ndef_fileId[] = {0xE1, 0x04};
  jbyteArray fileIdArray = e->NewByteArray(2);
  e->SetByteArrayRegion(fileIdArray, 0, 2, (jbyte*)ndef_fileId);

  T4TNFCEE_STATUS_t t4tNfceeStatus = validatePreCondition(OP_LOCK, fileIdArray);
  if (t4tNfceeStatus != STATUS_SUCCESS) return false;

  tNFA_STATUS status = NFA_STATUS_FAILED;

  uint8_t clNdefFileValue = 0, cNdefFileValue = 0;
  if(!doGetT4tConfVals(clNdefFileValue, cNdefFileValue))
    return false;

#ifndef LINUX
  if (android::isDiscoveryStarted()) {
    android::startRfDiscovery(false);
  }
#else
  if (isDiscoveryStarted()) {
    startRfDiscovery(false);
  }
#endif
  std::vector<uint8_t> cNdefcmd = {0x20,
                                   0x02,
                                   0x05,
                                   0x01,
                                   NXP_NFC_SET_CONFIG_PARAM_EXT_ID1,
                                   NXP_NFC_CPARAM_ID_T4T_NFCEE,
                                   NXP_PARAM_LEN_T4T_NFCEE};

  if (doChangeT4tFileWritePerm(cNdefFileValue, lock)) {
    cNdefcmd.push_back(cNdefFileValue);
    if ((NfcConfig::getUnsigned(NAME_NXP_T4T_NFCEE_ENABLE, 0x00) &
         (1 << MASK_LOCK_BIT)) &&
        doChangeT4tFileWritePerm(clNdefFileValue, lock)) {
      std::vector<uint8_t> clNdefcmd = {NXP_NFC_SET_CONFIG_PARAM_EXT,
                                        NXP_NFC_CLPARAM_ID_T4T_NFCEE,
                                        NXP_PARAM_LEN_T4T_NFCEE};
      int setConfigindex = 2;
      cNdefcmd.at(setConfigindex) = NXP_PARAM_SET_CONFIG_LEN;
      cNdefcmd.at(++setConfigindex) = NXP_PARAM_SET_CONFIG_PARAM;
      clNdefcmd.push_back(clNdefFileValue);
      cNdefcmd.insert(cNdefcmd.end(), &clNdefcmd[0],
                      &clNdefcmd[0] + clNdefcmd.size());
    }
    status = android::NxpNfc_Write_Cmd_Common(cNdefcmd.size(), &cNdefcmd[0]);
  }

#ifndef LINUX
  if (!android::isDiscoveryStarted()) android::startRfDiscovery(true);
#else
  if (!isDiscoveryStarted()) startRfDiscovery(true);
#endif
  if (status != NFA_STATUS_OK) return false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit", __func__);
  return true;
}

/*******************************************************************************
**
** Function:        isLockedT4tData
**
** Description:     Check Lock status of the T4T NDEF file.
**
** Parameter:       NULL
**
** Returns:         Return T4T NDEF lock status.
**                  Return "True" when T4T data is locked (un-writable).
**                  Otherwise, "False" shall be returned.
**
*******************************************************************************/
bool NativeT4tNfcee::isLockedT4tData(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  uint8_t ndef_fileId[] = {0xE1, 0x04};
  jbyteArray fileIdArray = e->NewByteArray(2);
  e->SetByteArrayRegion(fileIdArray, 0, 2, (jbyte*)ndef_fileId);

  T4TNFCEE_STATUS_t t4tNfceeStatus = validatePreCondition(OP_LOCK, fileIdArray);
  if (t4tNfceeStatus != STATUS_SUCCESS) return false;

  uint8_t clNdefFileValue = 0, cNdefFileValue = 0;
  if(!doGetT4tConfVals(clNdefFileValue, cNdefFileValue))
    return false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit", __func__);

  return (((cNdefFileValue & (1 << MASK_LOCK_BIT)) == 0) ? true : false);
}
#endif
/*******************************************************************************
**
** Function:        isNdefWritePermission
**
** Description:     Read from config file for write permission
**
** Parameter:       NULL
**
** Returns:         Return T4T NDEF write permission status.
**                  Return "True" when T4T write permission allow to change.
**                  Otherwise, "False" shall be returned.
**
*******************************************************************************/
bool NativeT4tNfcee::isNdefWritePermission() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  unsigned long num = 0x00;
  bool isNdefWriteAccess = false;
  if (NfcConfig::hasKey(NAME_NXP_T4T_NFCEE_ENABLE))
    num = NfcConfig::getUnsigned(NAME_NXP_T4T_NFCEE_ENABLE);

  if ((num & MASK_T4T_FEATURE_BIT) && (num & (1 << MASK_PROP_NDEF_FILE_BIT)))
    isNdefWriteAccess = true;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit 0x%lx", __func__, num);
  return isNdefWriteAccess;
}
/*******************************************************************************
 **
 ** Function:        isT4tNfceeBusy
 **
 ** Description:     Returns True if T4tNfcee operation is ongoing else false
 **
 ** Returns:         true/false
 **
 *******************************************************************************/
bool NativeT4tNfcee::isT4tNfceeBusy(void) { return mBusy; }

/*******************************************************************************
 **
 ** Function:        setBusy
 **
 ** Description:     Sets busy flag indicating T4T operation is ongoing
 **
 ** Returns:         none
 **
 *******************************************************************************/
void NativeT4tNfcee::setBusy() { mBusy = true; }

/*******************************************************************************
 **
 ** Function:        resetBusy
 **
 ** Description:     Resets busy flag indicating T4T operation is completed
 **
 ** Returns:         none
 **
 *******************************************************************************/
void NativeT4tNfcee::resetBusy() { mBusy = false; }
/*******************************************************************************
**
** Function:        getT4TNfceeAid
**
** Description:     Get the T4T Nfcee AID.
**
** Returns:         T4T AID: vector<uint8_t>
**
*******************************************************************************/
vector<uint8_t> NativeT4tNfcee::getT4TNfceeAid() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s:enter", __func__);

  std::vector<uint8_t> t4tNfceeAidBuf{0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};

  if (NfcConfig::hasKey(NAME_NXP_T4T_NDEF_NFCEE_AID)) {
    t4tNfceeAidBuf = NfcConfig::getBytes(NAME_NXP_T4T_NDEF_NFCEE_AID);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s:Exit", __func__);

  return t4tNfceeAidBuf;
}

/*******************************************************************************
**
** Function:        isFwSupportNonStdT4TAid
**
** Description:     Check FW supports Non-standard AID or not.
**
** Returns:         true: FW support NON-STD AID
**                  false: FW not support NON-STD AID
**
*******************************************************************************/
bool NativeT4tNfcee::isFwSupportNonStdT4TAid() {
  tNFC_FW_VERSION nfc_native_fw_version;
  jboolean isFwSupport = false;
  memset(&nfc_native_fw_version, 0, sizeof(nfc_native_fw_version));
  const uint8_t FW_ROM_VERSION = 0x01;
  const uint8_t FW_MAJOR_VERSION_SN1XX = 0x10;
  const uint8_t FW_MAJOR_VERSION_SN2XX = 0x01;
  const uint8_t FW_MINOR_VERSION_SN1XX = 0x54;
  nfc_native_fw_version = nfc_ncif_getFWVersion();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "FW Version: %x.%x.%x", nfc_native_fw_version.rom_code_version,
      nfc_native_fw_version.major_version, nfc_native_fw_version.minor_version);

  if (nfc_native_fw_version.rom_code_version == FW_ROM_VERSION) {
    if ((nfc_native_fw_version.major_version == FW_MAJOR_VERSION_SN1XX &&
         nfc_native_fw_version.minor_version >= FW_MINOR_VERSION_SN1XX) ||
        (nfc_native_fw_version.major_version == FW_MAJOR_VERSION_SN2XX)) {
      isFwSupport = true;
    }
  }
  LOG(INFO) << StringPrintf(
      "nfcManager_isFwSupportNonStdT4TAid Enter isFwSupport = %d", isFwSupport);
  return isFwSupport;
}
/*******************************************************************************
**
** Function:        checkAndUpdateT4TAid
**
** Description:     Check and update T4T Ndef Nfcee AID.
**
** Returns:         void
**
*******************************************************************************/
void NativeT4tNfcee::checkAndUpdateT4TAid(uint8_t* t4tNdefAid,
                                          uint8_t* t4tNdefAidLen) {
  if (!isFwSupportNonStdT4TAid()) {
    uint8_t stdT4tAid[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
    *t4tNdefAidLen = sizeof(stdT4tAid);
    memcpy(t4tNdefAid, stdT4tAid, *t4tNdefAidLen);
  } else {
    vector<uint8_t> t4tNfceeAidBuf = getT4TNfceeAid();
    uint8_t* t4tAidBuf = t4tNfceeAidBuf.data();
    *t4tNdefAidLen = t4tNfceeAidBuf.size();
    memcpy(t4tNdefAid, t4tAidBuf, *t4tNdefAidLen);
  }
}
#endif
