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

#include "NfcJniUtil.h"

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <errno.h>
#include <log/log.h>
#include <nativehelper/JNIHelp.h>
#include <nativehelper/ScopedLocalRef.h>

#include "RoutingManager.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function:        JNI_OnLoad
**
** Description:     Register all JNI functions with Java Virtual Machine.
**                  jvm: Java Virtual Machine.
**                  reserved: Not used.
**
** Returns:         JNI version.
**
*******************************************************************************/
jint JNI_OnLoad(JavaVM* jvm, void*) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  JNIEnv* e = NULL;

  LOG(INFO) << StringPrintf("NFC Service: loading nci JNI");

  // Check JNI version
  if (jvm->GetEnv((void**)&e, JNI_VERSION_1_6)) return JNI_ERR;

  if (android::register_com_android_nfc_NativeNfcManager(e) == -1)
    return JNI_ERR;
  if (android::register_com_android_nfc_NativeLlcpServiceSocket(e) == -1)
    return JNI_ERR;
  if (android::register_com_android_nfc_NativeLlcpSocket(e) == -1)
    return JNI_ERR;
  if (android::register_com_android_nfc_NativeNfcTag(e) == -1) return JNI_ERR;
  if (android::register_com_android_nfc_NativeLlcpConnectionlessSocket(e) == -1)
    return JNI_ERR;
  if (android::register_com_android_nfc_NativeP2pDevice(e) == -1)
    return JNI_ERR;
  if (RoutingManager::getInstance().registerJniFunctions(e) == -1)
    return JNI_ERR;
  if (android::register_com_android_nfc_NativeT4tNfcee(e) == -1)
    return JNI_ERR;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_VERSION_1_6;
}

namespace android {

/*******************************************************************************
**
** Function:        nfc_jni_cache_object
**
** Description:
**
** Returns:         Status code.
**
*******************************************************************************/
int nfc_jni_cache_object(JNIEnv* e, const char* className, jobject* cachedObj) {
  ScopedLocalRef<jclass> cls(e, e->FindClass(className));
  if (cls.get() == NULL) {
    LOG(ERROR) << StringPrintf("%s: find class error", __func__);
    return -1;
  }

  jmethodID ctor = e->GetMethodID(cls.get(), "<init>", "()V");
  ScopedLocalRef<jobject> obj(e, e->NewObject(cls.get(), ctor));
  if (obj.get() == NULL) {
    LOG(ERROR) << StringPrintf("%s: create object error", __func__);
    return -1;
  }

  *cachedObj = e->NewGlobalRef(obj.get());
  if (*cachedObj == NULL) {
    LOG(ERROR) << StringPrintf("%s: global ref error", __func__);
    return -1;
  }
  return 0;
}

/*******************************************************************************
**
** Function:        nfc_jni_get_nfc_socket_handle
**
** Description:     Get the value of "mHandle" member variable.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Value of mHandle.
**
*******************************************************************************/
int nfc_jni_get_nfc_socket_handle(JNIEnv* e, jobject o) {
  ScopedLocalRef<jclass> c(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(c.get(), "mHandle", "I");
  return e->GetIntField(o, f);
}

/*******************************************************************************
**
** Function:        nfc_jni_get_nat
**
** Description:     Get the value of "mNative" member variable.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Pointer to the value of mNative.
**
*******************************************************************************/
struct nfc_jni_native_data* nfc_jni_get_nat(JNIEnv* e, jobject o) {
  ScopedLocalRef<jclass> c(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(c.get(), "mNative", "J");
  /* Retrieve native structure address */
  return (struct nfc_jni_native_data*)e->GetLongField(o, f);
}

/*******************************************************************************
**
** Function         nfc_jni_cache_object_local
**
** Description      Allocates a java object and calls it's constructor
**
** Returns          -1 on failure, 0 on success
**
*******************************************************************************/
int nfc_jni_cache_object_local(JNIEnv* e, const char* className,
                               jobject* cachedObj) {
  ScopedLocalRef<jclass> cls(e, e->FindClass(className));
  if (cls.get() == NULL) {
    LOG(ERROR) << StringPrintf("%s: find class error", __func__);
    return -1;
  }

  jmethodID ctor = e->GetMethodID(cls.get(), "<init>", "()V");
  jobject obj = e->NewObject(cls.get(), ctor);
  if (obj == NULL) {
    LOG(ERROR) << StringPrintf("%s: create object error", __func__);
    return -1;
  }

  *cachedObj = obj;
  if (*cachedObj == NULL) {
    LOG(ERROR) << StringPrintf("%s: global ref error", __func__);
    return -1;
  }
  return 0;
}

}  // namespace android
