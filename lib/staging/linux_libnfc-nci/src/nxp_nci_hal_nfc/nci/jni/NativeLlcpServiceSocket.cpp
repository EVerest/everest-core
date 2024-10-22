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
#include <nativehelper/JNIHelp.h>

#include "JavaClassConstants.h"
#include "NfcAdaptation.h"
#include "NfcJniUtil.h"
#include "PeerToPeer.h"
#include "nfa_api.h"
#include "nfa_p2p_api.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

namespace android {

/*******************************************************************************
**
** Function:        nativeLlcpServiceSocket_doAccept
**
** Description:     Accept a connection request from a peer.
**                  e: JVM environment.
**                  o: Java object.
**                  miu: Maximum information unit.
**                  rw: Receive window.
**                  linearBufferLength: Not used.
**
** Returns:         LlcpSocket Java object.
**
*******************************************************************************/
static jobject nativeLlcpServiceSocket_doAccept(JNIEnv* e, jobject o, jint miu,
                                                jint rw,
                                                jint /*linearBufferLength*/) {
  jobject clientSocket = NULL;
  jclass clsNativeLlcpSocket = NULL;
  jfieldID f = 0;
  PeerToPeer::tJNI_HANDLE serverHandle;  // handle of the local server
  bool stat = false;
  PeerToPeer::tJNI_HANDLE connHandle =
      PeerToPeer::getInstance().getNewJniHandle();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  serverHandle = (PeerToPeer::tJNI_HANDLE)nfc_jni_get_nfc_socket_handle(e, o);

  stat = PeerToPeer::getInstance().accept(serverHandle, connHandle, miu, rw);

  if (!stat) {
    LOG(ERROR) << StringPrintf("%s: fail accept", __func__);
    goto TheEnd;
  }

  /* Create new LlcpSocket object */
  if (nfc_jni_cache_object_local(e, gNativeLlcpSocketClassName,
                                 &(clientSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail create NativeLlcpSocket", __func__);
    goto TheEnd;
  }

  /* Get NativeConnectionOriented class object */
  clsNativeLlcpSocket = e->GetObjectClass(clientSocket);
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: get class object error", __func__);
    goto TheEnd;
  }

  /* Set socket handle */
  f = e->GetFieldID(clsNativeLlcpSocket, "mHandle", "I");
  e->SetIntField(clientSocket, f, (jint)connHandle);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpSocket, "mLocalMiu", "I");
  e->SetIntField(clientSocket, f, (jint)miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpSocket, "mLocalRw", "I");
  e->SetIntField(clientSocket, f, (jint)rw);

TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return clientSocket;
}

/*******************************************************************************
**
** Function:        nativeLlcpServiceSocket_doClose
**
** Description:     Close a server socket.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeLlcpServiceSocket_doClose(JNIEnv* e, jobject o) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  PeerToPeer::tJNI_HANDLE jniServerHandle = 0;
  bool stat = false;

  jniServerHandle = nfc_jni_get_nfc_socket_handle(e, o);

  stat = PeerToPeer::getInstance().deregisterServer(jniServerHandle);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}

/*****************************************************************************
**
** Description:     JNI functions
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doAccept", "(III)Lcom/android/nfc/dhimpl/NativeLlcpSocket;",
     (void*)nativeLlcpServiceSocket_doAccept},
    {"doClose", "()Z", (void*)nativeLlcpServiceSocket_doClose},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeLlcpServiceSocket
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeLlcpServiceSocket(JNIEnv* e) {
  return jniRegisterNativeMethods(e, gNativeLlcpServiceSocketClassName,
                                  gMethods, NELEM(gMethods));
}

}  // namespace android
