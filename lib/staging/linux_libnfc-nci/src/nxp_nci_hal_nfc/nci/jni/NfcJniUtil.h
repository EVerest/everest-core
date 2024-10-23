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

#pragma once
#ifndef LINUX
#include <jni.h>
#endif
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>
#if (NXP_EXTNS == TRUE)
#include "Nxp_Features.h"
#endif

/* Discovery modes -- keep in sync with NFCManager.DISCOVERY_MODE_* */
#define DISCOVERY_MODE_TAG_READER 0
#define DISCOVERY_MODE_NFCIP1 1
#define DISCOVERY_MODE_CARD_EMULATION 2
#define DISCOVERY_MODE_TABLE_SIZE 3

#define DISCOVERY_MODE_DISABLED 0
#define DISCOVERY_MODE_ENABLED 1

#define MODE_P2P_TARGET 0
#define MODE_P2P_INITIATOR 1

/* Properties values */
#define PROPERTY_LLCP_LTO 0
#define PROPERTY_LLCP_MIU 1
#define PROPERTY_LLCP_WKS 2
#define PROPERTY_LLCP_OPT 3
#define PROPERTY_NFC_DISCOVERY_A 4
#define PROPERTY_NFC_DISCOVERY_B 5
#define PROPERTY_NFC_DISCOVERY_F 6
#define PROPERTY_NFC_DISCOVERY_15693 7
#define PROPERTY_NFC_DISCOVERY_NCFIP 8

/* Error codes */
#define ERROR_BUFFER_TOO_SMALL (-12)
#define ERROR_INSUFFICIENT_RESOURCES (-9)

/* Pre-defined tag type values. These must match the values in
 * Ndef.java in the framework.
 */
#define NDEF_UNKNOWN_TYPE (-1)
#define NDEF_TYPE1_TAG 1
#define NDEF_TYPE2_TAG 2
#define NDEF_TYPE3_TAG 3
#define NDEF_TYPE4_TAG 4
#define NDEF_MIFARE_CLASSIC_TAG 101

/* Pre-defined card read/write state values. These must match the values in
 * Ndef.java in the framework.
 */
#define NDEF_MODE_READ_ONLY 1
#define NDEF_MODE_READ_WRITE 2
#define NDEF_MODE_UNKNOWN 3

/* Name strings for target types. These *must* match the values in
 * TagTechnology.java */
#define TARGET_TYPE_UNKNOWN (-1)
#define TARGET_TYPE_ISO14443_3A 1
#define TARGET_TYPE_ISO14443_3B 2
#define TARGET_TYPE_ISO14443_4 3
#define TARGET_TYPE_FELICA 4
#define TARGET_TYPE_V 5
#define TARGET_TYPE_NDEF 6
#define TARGET_TYPE_NDEF_FORMATABLE 7
#define TARGET_TYPE_MIFARE_CLASSIC 8
#define TARGET_TYPE_MIFARE_UL 9
#define TARGET_TYPE_KOVIO_BARCODE 10
#define JNI_TRUE true
#define JNI_FALSE false
// define a few NXP error codes that NFC service expects;
// see external/libnfc-nxp/src/phLibNfcStatus.h;
// see external/libnfc-nxp/inc/phNfcStatus.h
#define NFCSTATUS_SUCCESS (0x0000)
#define NFCSTATUS_FAILED (0x00FF)
#ifdef LINUX
#include<string>
using namespace std;
struct JavaVM {
};
typedef unsigned char* jobject;
#endif
struct nfc_jni_native_data {
  /* Thread handle */
  pthread_t thread;
  int running;
  /* Our VM */
  JavaVM* vm;
  int env_version;

  /* Reference to the NFCManager instance */
  jobject manager;

  /* Cached objects */
  jobject cached_NfcTag;
  jobject cached_P2pDevice;

  /* Secure Element selected */
  int seId;

  /* LLCP params */
  int lto;
  int miu;
  int wks;
  int opt;

  int tech_mask;
  int discovery_duration;

#ifndef LINUX
  /* Tag detected */
  jobject tag;
#endif
  int tHandle;
  int tProtocols[16];
  int handles[16];
};

#ifndef LINUX
class ScopedAttach {
 public:
  ScopedAttach(JavaVM* vm, JNIEnv** env) : vm_(vm) {
    vm_->AttachCurrentThread(env, NULL);
  }

  ~ScopedAttach() { vm_->DetachCurrentThread(); }

 private:
  JavaVM* vm_;
};

jint JNI_OnLoad(JavaVM* jvm, void* reserved);
#endif
#ifdef LINUX
class Bytes
{
  public:
   Bytes(unsigned char *buf, int Len)
   {
     buffer = buf;
     bufLen = Len;
   }
   int size()
   {
     return (bufLen);
   }
  private:
   unsigned char* buffer;
   int bufLen;
};
struct JNIEnv {
    int bufLen;
};
//struct jobject{
//};
struct jclass {
};
/*typedef struct jbyteArray
{
    unsigned char *buffer;
    int bufLen;
}jbyteArray;*/
typedef unsigned char* jbyteArray;
typedef unsigned char jbyte;
typedef int jintArray;
typedef bool jboolean;
typedef int jint;
typedef unsigned char * jobjectArray;
typedef string jstring;
#endif
namespace android {
int nfc_jni_cache_object(JNIEnv* e, const char* clsname, jobject* cached_obj);
int nfc_jni_cache_object_local(JNIEnv* e, const char* className,
                               jobject* cachedObj);
int nfc_jni_get_nfc_socket_handle(JNIEnv* e, jobject o);
struct nfc_jni_native_data* nfc_jni_get_nat(JNIEnv* e, jobject o);
int register_com_android_nfc_NativeNfcManager(JNIEnv* e);
int register_com_android_nfc_NativeNfcTag(JNIEnv* e);
int register_com_android_nfc_NativeP2pDevice(JNIEnv* e);
int register_com_android_nfc_NativeLlcpConnectionlessSocket(JNIEnv* e);
int register_com_android_nfc_NativeLlcpServiceSocket(JNIEnv* e);
int register_com_android_nfc_NativeLlcpSocket(JNIEnv* e);
int register_com_android_nfc_NativeT4tNfcee(JNIEnv *e);
}  // namespace android
