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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#ifndef LINUX
#include <nativehelper/ScopedPrimitiveArray.h>
#endif
#include "JavaClassConstants.h"
#include "NativeT4tNfcee.h"
#include "NfcJniUtil.h"
extern bool nfc_debug_enabled;
using android::base::StringPrintf;
#ifndef LINUX
namespace android {
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
jint t4tNfceeManager_doClearNdefT4tData(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  return t4tNfcEe.t4tClearData(e, o);
}
#endif
/*******************************************************************************
 **
 ** Function:        nfcManager_doWriteT4tData
 **
 ** Description:     Write the data into the T4T file of the specific file ID
 **
 ** Returns:         Return the size of data written
 **                  Return negative number of error code
 **
 *******************************************************************************/
#ifdef LINUX
int t4tNfceeManager_doWriteT4tData(UINT8 *fileId,
                                    UINT8 *ndefBuffer, int ndefBufferLength) {
#else
jint t4tNfceeManager_doWriteT4tData(JNIEnv* e, jobject o, jbyteArray fileId,
                                    jbyteArray data, jint length) {
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
#ifdef LINUX
  return t4tNfcEe.t4tWriteData(fileId, ndefBuffer, ndefBufferLength);
#else
  return t4tNfcEe.t4tWriteData(e, o, fileId, data, length);
#endif
}
/*******************************************************************************
**
** Function:        nfcManager_doReadT4tData
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
int t4tNfceeManager_doReadT4tData(UINT8 *fileId,
                                    UINT8 *ndefBuffer, int *ndefBufferLength) {
#else
jbyteArray t4tNfceeManager_doReadT4tData(JNIEnv* e, jobject o,
                                         jbyteArray fileId) {
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
#ifdef LINUX
  return t4tNfcEe.t4tReadData(ndefBuffer, ndefBufferLength, fileId);
#else
  return t4tNfcEe.t4tReadData(e, o, fileId);
#endif
}
#ifndef LINUX
/*******************************************************************************
**
** Function:        t4tNfceeManager_doLockT4tData
**
** Description:     Lock/Unlock the data in the T4T NDEF file.
**
** Parameter:       boolean lock : True(lock) or False(unlock)
**
** Returns:         boolean : Return the Success or fail of the operation.
**                  Return "True" when operation is successful. else "False"
**
*******************************************************************************/
jboolean t4tNfceeManager_doLockT4tData(JNIEnv* e, jobject o, jboolean lock) {
  return t4tNfcEe.doLockT4tData(e, o, lock);
}
/*******************************************************************************
**
** Function:        t4tNfceeManager_doLockT4tData
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
jboolean t4tNfceeManager_isLockedT4tData(JNIEnv* e, jobject o) {
  return t4tNfcEe.isLockedT4tData(e, o);
}
/*****************************************************************************
 **
 ** Description:     JNI functions
 **
 *****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doWriteT4tData", "([B[BI)I", (void*)t4tNfceeManager_doWriteT4tData},
    {"doReadT4tData", "([B)[B", (void*)t4tNfceeManager_doReadT4tData},
    {"doLockT4tData", "(Z)Z", (void*)t4tNfceeManager_doLockT4tData},
    {"isLockedT4tData", "()Z", (void*)t4tNfceeManager_isLockedT4tData},
    {"doClearNdefT4tData", "()Z", (void*)t4tNfceeManager_doClearNdefT4tData},
};

/*******************************************************************************
 **
 ** Function:        register_com_android_nfc_NativeT4tNfcee
 **
 ** Description:     Regisgter JNI functions with Java Virtual Machine.
 **                  e: Environment of JVM.
 **
 ** Returns:         Status of registration.
 **
 *******************************************************************************/
int register_com_android_nfc_NativeT4tNfcee(JNIEnv* e) {
  return jniRegisterNativeMethods(e, gNativeT4tNfceeClassName, gMethods,
                                  NELEM(gMethods));
}
#endif
#ifndef LINUX
}  // namespace android
#endif
#endif
