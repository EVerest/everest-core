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
#include "NfcJniUtil.h"
#include "SyncEvent.h"
#include "nfa_api.h"
#include "linux_nfc_api.h"
#ifndef LINUX
#include <nativehelper/ScopedLocalRef.h>
#endif
#define t4tNfcEe (NativeT4tNfcee::getInstance())

typedef enum { OP_READ = 0, OP_WRITE, OP_LOCK, OP_CLEAR } T4TNFCEE_OPERATIONS_t;

typedef enum {
  STATUS_SUCCESS = 0,
  STATUS_FAILED = -1,
  ERROR_RF_ACTIVATED = -2,
  ERROR_MPOS_ON = -3,
  ERROR_NFC_NOT_ON = -4,
  ERROR_INVALID_FILE_ID = -5,
  ERROR_INVALID_LENGTH = -6,
  ERROR_CONNECTION_FAILED = -7,
  ERROR_EMPTY_PAYLOAD = -8,
  ERROR_NDEF_VALIDATION_FAILED = -9,
  ERROR_WRITE_PERMISSION = -10,
  ERROR_NFC_OFF_TRIGGERED = -11,
} T4TNFCEE_STATUS_t;

class NativeT4tNfcee {
 public:
  SyncEvent mT4tNfceeMPOSEvt;
  /*****************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the NativeT4tNfcee singleton object.
  **
  ** Returns:         NativeT4tNfcee object.
  **
  *******************************************************************************/
  static NativeT4tNfcee& getInstance();

  /*******************************************************************************
  **
  ** Function:        initialize
  **
  ** Description:     Initialize all member variables.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void initialize(void);
  /*****************************************************************************
  **
  ** Function:        onNfccShutdown
  **
  ** Description:     This api shall be called in NFC OFF case.
  **
  ** Returns:         none.
  **
  *******************************************************************************/
  void onNfccShutdown();

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
  int32_t t4tWriteData(uint8_t* fileId, uint8_t* ndefBuffer,  int ndefBufferLength);
  #else
  int t4tWriteData(JNIEnv* e, jobject o, jbyteArray fileId, jbyteArray data,
                   int length);
  #endif
  #ifndef LINUX
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
  jboolean t4tClearData(JNIEnv* e, jobject o);
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
  jboolean performT4tClearData(uint8_t* fileId);
  #endif
  /*******************************************************************************
  **
  ** Function:        t4tReadData
  **
  ** Description:     Read the data from the T4T file of the specific file ID.
  **
  ** Returns:         byte[] : all the data previously written to the specific
  **                  file ID.
  **                  Return one byte '0xFF' if the data was never written to
  *the
  **                  specific file ID,
  **                  Return null if reading fails.
  **
  *******************************************************************************/
  #ifdef LINUX
  int32_t t4tReadData(uint8_t* ndefBuffer,  int *ndefBufferLength,
                            uint8_t* fileId);
  #else
  jbyteArray t4tReadData(JNIEnv* e, jobject o, jbyteArray fileId);
  #endif

  /*******************************************************************************
  **
  ** Function:        t4tReadComplete
  **
  ** Description:     Updates read data to the waiting READ API
  **
  ** Returns:         none
  **
  *******************************************************************************/
  void t4tReadComplete(tNFA_STATUS status, tNFA_RX_DATA data);

  /*******************************************************************************
   **
   ** Function:        t4tWriteComplete
   **
   ** Description:     Returns write complete information
   **
   ** Returns:         none
   **
   *******************************************************************************/
  void t4tWriteComplete(tNFA_STATUS status, tNFA_RX_DATA data);
  /*******************************************************************************
   **
   ** Function:        t4tClearComplete
   **
   ** Description:     Update T4T clear data status, waiting T4tClearData API.
   **
   ** Returns:         none
   **
   *******************************************************************************/
  void t4tClearComplete(tNFA_STATUS status);
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
  bool doChangeT4tFileWritePerm(uint8_t& param_val, const bool& lock);
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
  bool doGetT4tConfVals(uint8_t& clNdefFileValue, uint8_t& cNdefFileValue);
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
  bool doLockT4tData(JNIEnv* e, jobject o, bool lock);
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
  bool isLockedT4tData(JNIEnv* e, jobject o);
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
  bool isNdefWritePermission();
  /*******************************************************************************
   **
   ** Function:        isT4tNfceeBusy
   **
   ** Description:     Returns True if T4tNfcee operation is ongoing else false
   **
   ** Returns:         true/false
   **
   *******************************************************************************/
  bool isT4tNfceeBusy(void);

  /*******************************************************************************
  **
  ** Function:        t4tNfceeEventHandler
  **
  ** Description:     Handles callback events received from lower layer
  **
  ** Returns:         none
  **
  *******************************************************************************/
  void eventHandler(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
  /*******************************************************************************
  **
  ** Function:        checkAndUpdateT4TAid
  **
  ** Description:     Check and update T4T Ndef Nfcee AID.
  **
  ** Returns:         void
  **
  *******************************************************************************/
  void checkAndUpdateT4TAid(uint8_t* t4tAid, uint8_t* t4tNdefAidLen);

 private:
  bool mBusy;
  static const int NXP_NFC_CLPARAM_ID_T4T_NFCEE = 0x95;
  static const int NXP_NFC_CPARAM_ID_T4T_NFCEE = 0x10;
  static const int NXP_NFC_NUM_PARAM_T4T_NFCEE = 0x02;
  static const int NXP_PARAM_LEN_T4T_NFCEE = 0x01;
  static const int MASK_T4T_FEATURE_BIT = 1;
  static const int MASK_LOCK_BIT = 6;
  static const int MASK_PROP_NDEF_FILE_BIT = 7;
  static const int MAX_CONFIG_VALUE_LEN = 0x16;
  static const int NXP_PARAM_GET_CONFIG_INDEX = 4;
  static const int NXP_PARAM_GET_CONFIG_INDEX1 = 8;
  static const int NXP_PARAM_SET_CONFIG_LEN = 0x09;
  static const int NXP_PARAM_SET_CONFIG_PARAM = 0x02;
  static NativeT4tNfcee sNativeT4tNfceeInstance;
  static bool sIsNfcOffTriggered;
  SyncEvent mT4tNfcOffEvent;
  SyncEvent mT4tNfcEeRWEvent;
  SyncEvent mT4tNfcEeWriteEvent;
  SyncEvent mT4tNfcEeEvent;
  SyncEvent mT4tNfcEeClrDataEvent;
  tNFA_RX_DATA mReadData;
  tNFA_STATUS mT4tOpStatus = NFA_STATUS_FAILED;
  tNFA_STATUS mT4tNfcEeEventStat = NFA_STATUS_FAILED;
  std::basic_string<uint8_t> sRxDataBuffer;
  NativeT4tNfcee();

  /*******************************************************************************
  **
  ** Function:        openConnection
  **
  ** Description:     Open T4T Nfcee Connection
  **
  ** Returns:         Status
  **
  *******************************************************************************/
  tNFA_STATUS openConnection();

  /*******************************************************************************
  **
  ** Function:        closeConnection
  **
  ** Description:     Close T4T Nfcee Connection
  **
  ** Returns:         Status
  **
  *******************************************************************************/
  tNFA_STATUS closeConnection();

/*******************************************************************************
**
** Function:        setup
**
** Description:     stops Discovery and opens T4TNFCEE connection
**
** Returns:         Status
**
*******************************************************************************/
tNFA_STATUS setup(void);

  /*******************************************************************************
  **
  ** Function:        cleanup
  **
  ** Description:     closes connection and starts discovery
  **
  ** Returns:         Status
  **
  *******************************************************************************/
  void cleanup(void);

  /*******************************************************************************
  **
  ** Function:        validatePreCondition
  **
  ** Description:     Runs precondition checks for requested operation
  **
  ** Returns:         Status
  **
  *******************************************************************************/
  T4TNFCEE_STATUS_t validatePreCondition(T4TNFCEE_OPERATIONS_t op,
                                         jbyteArray fileId,
                                         jbyteArray data = nullptr);

  /*******************************************************************************
   **
   ** Function:        setBusy
   **
   ** Description:     Sets busy flag indicating T4T operation is ongoing
   **
   ** Returns:         none
   **
   *******************************************************************************/
  void setBusy();

  /*******************************************************************************
   **
   ** Function:        resetBusy
   **
   ** Description:     Resets busy flag indicating T4T operation is completed
   **
   ** Returns:         none
   **
   *******************************************************************************/
  void resetBusy();
  /*******************************************************************************
  **
  ** Function:        getT4TNfceeAid
  **
  ** Description:     Get the T4T Nfcee AID.
  **
  ** Returns:         T4T AID: vector<uint8_t>
  **
  *******************************************************************************/
  vector<uint8_t> getT4TNfceeAid();
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
  bool isFwSupportNonStdT4TAid();
};
#endif
