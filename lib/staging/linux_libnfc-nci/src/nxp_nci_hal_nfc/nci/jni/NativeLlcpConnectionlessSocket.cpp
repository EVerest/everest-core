/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <errno.h>
#include <malloc.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <semaphore.h>
#include <string.h>
#include "JavaClassConstants.h"
#include "NfcJniUtil.h"
#include "nfa_api.h"
#include "nfa_p2p_api.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

namespace android {

/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
static sem_t sConnlessRecvSem;
static jboolean sConnlessRecvWaitingForData = JNI_FALSE;
static uint8_t* sConnlessRecvBuf = NULL;
static uint32_t sConnlessRecvLen = 0;
static uint32_t sConnlessRecvRemoteSap = 0;

/*******************************************************************************
**
** Function:        nativeLlcpConnectionlessSocket_doSendTo
**
** Description:     Send data to peer.
**                  e: JVM environment.
**                  o: Java object.
**                  nsap: service access point.
**                  data: buffer for data.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpConnectionlessSocket_doSendTo(JNIEnv* e, jobject o,
                                                        jint nsap,
                                                        jbyteArray data) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: nsap = %d", __func__, nsap);

  ScopedLocalRef<jclass> c(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(c.get(), "mHandle", "I");
  jint handle = e->GetIntField(o, f);

  ScopedByteArrayRO bytes(e, data);
  if (bytes.get() == NULL) {
    return JNI_FALSE;
  }
  size_t byte_count = bytes.size();

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("NFA_P2pSendUI: len = %zu", byte_count);
  uint8_t* raw_ptr = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(
      &bytes[0]));  // TODO: API bug; NFA_P2pSendUI should take const*!
  tNFA_STATUS status =
      NFA_P2pSendUI((tNFA_HANDLE)handle, nsap, byte_count, raw_ptr);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: NFA_P2pSendUI done, status = %d", __func__, status);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: NFA_P2pSendUI failed, status = %d",
                               __func__, status);
    return JNI_FALSE;
  }
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nativeLlcpConnectionlessSocket_receiveData
**
** Description:     Receive data from the stack.
**                  data: buffer contains data.
**                  len: length of data.
**                  remoteSap: remote service access point.
**
** Returns:         None
**
*******************************************************************************/
void nativeLlcpConnectionlessSocket_receiveData(uint8_t* data, uint32_t len,
                                                uint32_t remoteSap) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: waiting for data = %d, len = %d", __func__,
                      sConnlessRecvWaitingForData, len);

  // Sanity...
  if (sConnlessRecvLen < len) {
    len = sConnlessRecvLen;
  }

  if (sConnlessRecvWaitingForData) {
    sConnlessRecvWaitingForData = JNI_FALSE;
    sConnlessRecvLen = len;
    memcpy(sConnlessRecvBuf, data, len);
    sConnlessRecvRemoteSap = remoteSap;

    sem_post(&sConnlessRecvSem);
  }
}

/*******************************************************************************
**
** Function:        connectionlessCleanup
**
** Description:     Free resources.
**
** Returns:         None
**
*******************************************************************************/
static jobject connectionlessCleanup() {
  sConnlessRecvWaitingForData = JNI_FALSE;
  sConnlessRecvLen = 0;
  if (sConnlessRecvBuf != NULL) {
    free(sConnlessRecvBuf);
    sConnlessRecvBuf = NULL;
  }
  return NULL;
}

/*******************************************************************************
**
** Function:        nativeLlcpConnectionlessSocket_abortWait
**
** Description:     Abort current operation and unblock threads.
**
** Returns:         None
**
*******************************************************************************/
void nativeLlcpConnectionlessSocket_abortWait() { sem_post(&sConnlessRecvSem); }

/*******************************************************************************
**
** Function:        nativeLlcpConnectionlessSocket_doReceiveFrom
**
** Description:     Receive data from a peer.
**                  e: JVM environment.
**                  o: Java object.
**                  linkMiu: max info unit
**
** Returns:         LlcpPacket Java object.
**
*******************************************************************************/
static jobject nativeLlcpConnectionlessSocket_doReceiveFrom(JNIEnv* e, jobject,
                                                            jint linkMiu) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: linkMiu = %d", __func__, linkMiu);
  jobject llcpPacket = NULL;
  ScopedLocalRef<jclass> clsLlcpPacket(e, NULL);

  if (sConnlessRecvWaitingForData != JNI_FALSE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Already waiting for incoming data", __func__);
    return NULL;
  }

  sConnlessRecvBuf = (uint8_t*)malloc(linkMiu);
  if (sConnlessRecvBuf == NULL) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Failed to allocate %d bytes memory buffer", __func__, linkMiu);
    return NULL;
  }
  sConnlessRecvLen = linkMiu;

  // Create the write semaphore
  if (sem_init(&sConnlessRecvSem, 0, 0) == -1) {
    LOG(ERROR) << StringPrintf("%s: semaphore creation failed (errno=0x%08x)",
                               __func__, errno);
    return connectionlessCleanup();
  }

  sConnlessRecvWaitingForData = JNI_TRUE;

  // Wait for sConnlessRecvSem completion status
  if (sem_wait(&sConnlessRecvSem)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to wait for write semaphore (errno=0x%08x)", __func__,
        errno);
    goto TheEnd;
  }

  // Create new LlcpPacket object
  if (nfc_jni_cache_object_local(e, "com/android/nfc/LlcpPacket",
                                 &(llcpPacket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: Find LlcpPacket class error", __func__);
    return connectionlessCleanup();
  }

  // Get NativeConnectionless class object
  clsLlcpPacket.reset(e->GetObjectClass(llcpPacket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: Get Object class error", __func__);
    return connectionlessCleanup();
  }

  // Set Llcp Packet remote SAP
  jfieldID f;
  f = e->GetFieldID(clsLlcpPacket.get(), "mRemoteSap", "I");
  e->SetIntField(llcpPacket, f, (jbyte)sConnlessRecvRemoteSap);

  // Set Llcp Packet Buffer
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Received Llcp packet buffer size = %d\n", __func__,
                      sConnlessRecvLen);
  f = e->GetFieldID(clsLlcpPacket.get(), "mDataBuffer", "[B");

  {
    ScopedLocalRef<jbyteArray> receivedData(e,
                                            e->NewByteArray(sConnlessRecvLen));
    e->SetByteArrayRegion(receivedData.get(), 0, sConnlessRecvLen,
                          (jbyte*)sConnlessRecvBuf);
    e->SetObjectField(llcpPacket, f, receivedData.get());
  }

TheEnd:  // TODO: should all the "return connectionlessCleanup()"s in this
         // function jump here instead?
  connectionlessCleanup();
  if (sem_destroy(&sConnlessRecvSem)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to destroy sConnlessRecvSem semaphore (errno=0x%08x)",
        __func__, errno);
  }
  return llcpPacket;
}

/*******************************************************************************
**
** Function:        nativeLlcpConnectionlessSocket_doClose
**
** Description:     Close socket.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpConnectionlessSocket_doClose(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);

  ScopedLocalRef<jclass> c(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(c.get(), "mHandle", "I");
  jint handle = e->GetIntField(o, f);

  tNFA_STATUS status = NFA_P2pDisconnect((tNFA_HANDLE)handle, FALSE);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: disconnect failed, status = %d", __func__,
                               status);
    return JNI_FALSE;
  }
  return JNI_TRUE;
}

/*****************************************************************************
**
** Description:     JNI functions
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doSendTo", "(I[B)Z", (void*)nativeLlcpConnectionlessSocket_doSendTo},
    {"doReceiveFrom", "(I)Lcom/android/nfc/LlcpPacket;",
     (void*)nativeLlcpConnectionlessSocket_doReceiveFrom},
    {"doClose", "()Z", (void*)nativeLlcpConnectionlessSocket_doClose},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeLlcpConnectionlessSocket
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeLlcpConnectionlessSocket(JNIEnv* e) {
  return jniRegisterNativeMethods(e, gNativeLlcpConnectionlessSocketClassName,
                                  gMethods, NELEM(gMethods));
}

}  // namespace android
