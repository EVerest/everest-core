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
#include <nativehelper/ScopedPrimitiveArray.h>
#include <nativehelper/ScopedUtfChars.h>

#include "JavaClassConstants.h"
#include "PeerToPeer.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

namespace android {

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doConnect
**
** Description:     Establish a connection to the peer.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpSocket_doConnect(JNIEnv* e, jobject o, jint nSap) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; sap=%d", __func__, nSap);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  bool stat = PeerToPeer::getInstance().connectConnOriented(jniHandle, nSap);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return stat ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doConnectBy
**
** Description:     Establish a connection to the peer.
**                  e: JVM environment.
**                  o: Java object.
**                  sn: Service name.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpSocket_doConnectBy(JNIEnv* e, jobject o, jstring sn) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);

  ScopedUtfChars serviceName(e, sn);
  if (serviceName.c_str() == NULL) {
    return JNI_FALSE;
  }
  bool stat = PeerToPeer::getInstance().connectConnOriented(
      jniHandle, serviceName.c_str());

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return stat ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doClose
**
** Description:     Close socket.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpSocket_doClose(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  bool stat = PeerToPeer::getInstance().disconnectConnOriented(jniHandle);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return stat ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doSend
**
** Description:     Send data to peer.
**                  e: JVM environment.
**                  o: Java object.
**                  data: Buffer of data.
**
** Returns:         True if sent ok.
**
*******************************************************************************/
static jboolean nativeLlcpSocket_doSend(JNIEnv* e, jobject o, jbyteArray data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  ScopedByteArrayRO bytes(e, data);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  uint8_t* raw_ptr = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(
      &bytes[0]));  // TODO: API bug: send should take const*!
  bool stat = PeerToPeer::getInstance().send(jniHandle, raw_ptr, bytes.size());

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return stat ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doReceive
**
** Description:     Receive data from peer.
**                  e: JVM environment.
**                  o: Java object.
**                  origBuffer: Buffer to put received data.
**
** Returns:         Number of bytes received.
**
*******************************************************************************/
static jint nativeLlcpSocket_doReceive(JNIEnv* e, jobject o,
                                       jbyteArray origBuffer) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  ScopedByteArrayRW bytes(e, origBuffer);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  uint16_t actualLen = 0;
  bool stat = PeerToPeer::getInstance().receive(
      jniHandle, reinterpret_cast<uint8_t*>(&bytes[0]), bytes.size(),
      actualLen);

  jint retval = 0;
  if (stat && (actualLen > 0)) {
    retval = actualLen;
  } else
    retval = -1;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; actual len=%d", __func__, retval);
  return retval;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doGetRemoteSocketMIU
**
** Description:     Get peer's maximum information unit.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Peer's maximum information unit.
**
*******************************************************************************/
static jint nativeLlcpSocket_doGetRemoteSocketMIU(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  jint miu = PeerToPeer::getInstance().getRemoteMaxInfoUnit(jniHandle);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return miu;
}

/*******************************************************************************
**
** Function:        nativeLlcpSocket_doGetRemoteSocketRW
**
** Description:     Get peer's receive window size.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Peer's receive window size.
**
*******************************************************************************/
static jint nativeLlcpSocket_doGetRemoteSocketRW(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  PeerToPeer::tJNI_HANDLE jniHandle =
      (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);
  jint rw = PeerToPeer::getInstance().getRemoteRecvWindow(jniHandle);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return rw;
}

/*****************************************************************************
**
** Description:     JNI functions
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doConnect", "(I)Z", (void*)nativeLlcpSocket_doConnect},
    {"doConnectBy", "(Ljava/lang/String;)Z",
     (void*)nativeLlcpSocket_doConnectBy},
    {"doClose", "()Z", (void*)nativeLlcpSocket_doClose},
    {"doSend", "([B)Z", (void*)nativeLlcpSocket_doSend},
    {"doReceive", "([B)I", (void*)nativeLlcpSocket_doReceive},
    {"doGetRemoteSocketMiu", "()I",
     (void*)nativeLlcpSocket_doGetRemoteSocketMIU},
    {"doGetRemoteSocketRw", "()I", (void*)nativeLlcpSocket_doGetRemoteSocketRW},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeLlcpSocket
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeLlcpSocket(JNIEnv* e) {
  return jniRegisterNativeMethods(e, gNativeLlcpSocketClassName, gMethods,
                                  NELEM(gMethods));
}

}  // namespace android
