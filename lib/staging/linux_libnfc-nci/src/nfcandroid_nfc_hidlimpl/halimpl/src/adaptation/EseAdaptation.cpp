/******************************************************************************
 *
 *
 *  Copyright (C) 2015,2020 NXP Semiconductors
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
#define LOG_TAG "EseAdaptation"
//#include <android/hardware/secure_element/1.0/ISecureElement.h>
//#include <android/hardware/secure_element/1.0/ISecureElementHalCallback.h>
//#include <android/hardware/secure_element/1.0/types.h>
//#include <hwbinder/ProcessState.h>
#include <pthread.h>
#include "EseAdaptation.h"
#include <log/log.h>
#include<iostream>
#include "logging.h"
#if 0
using android::hardware::Return;
using android::hardware::Void;
using android::hardware::secure_element::V1_0::ISecureElement;
using android::hardware::secure_element::V1_0::ISecureElementHalCallback;
using android::hardware::hidl_vec;
using android::sp;

using vendor::nxp::nxpese::V1_0::INxpEse;
#endif
extern bool nfc_debug_enabled;

extern "C" void GKI_shutdown();
extern void resetConfig();
extern "C" void verify_stack_non_volatile_store();
extern "C" void delete_stack_non_volatile_store(bool forceDelete);
using namespace std;
EseAdaptation* EseAdaptation::mpInstance = NULL;
ThreadMutex EseAdaptation::sLock;
ThreadMutex EseAdaptation::sIoctlLock;
//sp<INxpEse> EseAdaptation::mHalNxpEse;
//sp<ISecureElement> EseAdaptation::mHal;
tHAL_ESE_CBACK* EseAdaptation::mHalCallback = NULL;
tHAL_ESE_DATA_CBACK* EseAdaptation::mHalDataCallback = NULL;
ThreadCondVar EseAdaptation::mHalOpenCompletedEvent;
ThreadCondVar EseAdaptation::mHalCloseCompletedEvent;

#if (NXP_EXTNS == TRUE)
ThreadCondVar EseAdaptation::mHalCoreResetCompletedEvent;
ThreadCondVar EseAdaptation::mHalCoreInitCompletedEvent;
ThreadCondVar EseAdaptation::mHalInitCompletedEvent;
#define SIGNAL_NONE 0
#define SIGNAL_SIGNALED 1
#endif
#include <cstring>
//#define ALOGD_IF(nfc_debug_enabled,...) { cout << (__VA_ARGS__); }
//#define ALOGE(...) { cout << (__VA_ARGS__); }
/*******************************************************************************
**
** Function:    EseAdaptation::EseAdaptation()
**
** Description: class constructor
**
** Returns:     none
**
*******************************************************************************/
EseAdaptation::EseAdaptation() {
  //mCurrentIoctlData = NULL;
  memset(&mSpiHalEntryFuncs, 0, sizeof(mSpiHalEntryFuncs));
}

/*******************************************************************************
**
** Function:    EseAdaptation::~EseAdaptation()
**
** Description: class destructor
**
** Returns:     none
**
*******************************************************************************/
EseAdaptation::~EseAdaptation() { mpInstance = NULL; }

/*******************************************************************************
**
** Function:    EseAdaptation::GetInstance()
**
** Description: access class singleton
**
** Returns:     pointer to the singleton object
**
*******************************************************************************/
EseAdaptation& EseAdaptation::GetInstance() {
  AutoThreadMutex a(sLock);

  if (!mpInstance) mpInstance = new EseAdaptation;
  return *mpInstance;
}

/*******************************************************************************
**
** Function:    EseAdaptation::Initialize()
**
** Description: class initializer
**
** Returns:     none
**
*******************************************************************************/
void EseAdaptation::Initialize() {
  const char* func = "EseAdaptation::Initialize";
  uint8_t cmd_ese_nxp[] = {0x2F, 0x01, 0x01, 0x01};
  cout<<endl<<"EseAdaptation::Initialize start..... dbg666"<<endl; 
  ALOGD_IF(nfc_debug_enabled, "%s: enter dbg666", func);

  mHalCallback = NULL;
#if 0
  //ese_nxp_IoctlInOutData_t* pInpOutData;
  pInpOutData =
      (ese_nxp_IoctlInOutData_t*)malloc(sizeof(ese_nxp_IoctlInOutData_t));
  if (!pInpOutData) {
    ALOGE("%s Unable to initialize as memory allocation failed", __func__);
    return;
  }
  memset(pInpOutData, 0x00, sizeof(ese_nxp_IoctlInOutData_t));
  pInpOutData->inp.data.nxpCmd.cmd_len = sizeof(cmd_ese_nxp);
  memcpy(pInpOutData->inp.data.nxpCmd.p_cmd, cmd_ese_nxp, sizeof(cmd_ese_nxp));
  InitializeHalDeviceContext();
  if (pInpOutData != NULL) free(pInpOutData);
#endif     
  ALOGD_IF(nfc_debug_enabled, "%s: exit", func);
}

/*******************************************************************************
**
** Function:    EseAdaptation::signal()
**
** Description: signal the CondVar to release the thread that is waiting
**
** Returns:     none
**
*******************************************************************************/
void EseAdaptation::signal() { mCondVar.signal(); }

/*******************************************************************************
**
** Function:    EseAdaptation::Thread()
**
** Description: Creates work threads
**
** Returns:     none
**
*******************************************************************************/
uint32_t EseAdaptation::Thread(uint32_t arg) {
  const char* func = "EseAdaptation::Thread";
  ALOGD_IF(nfc_debug_enabled, "%s: enter", func);
  arg = 0;
  { ThreadCondVar CondVar; }

  EseAdaptation::GetInstance().signal();

  ALOGD_IF(nfc_debug_enabled, "%s: exit", func);
  return 0;
}

/*******************************************************************************
**
** Function:    EseAdaptation::GetHalEntryFuncs()
**
** Description: Get the set of HAL entry points.
**
** Returns:     Functions pointers for HAL entry points.
**
*******************************************************************************/
tHAL_ESE_ENTRY* EseAdaptation::GetHalEntryFuncs() {
  ALOGD_IF(nfc_debug_enabled, "GetHalEntryFuncs: enter");
  return &mSpiHalEntryFuncs;
}

/*******************************************************************************
**
** Function:    EseAdaptation::InitializeHalDeviceContext
**
** Description: Ask the generic Android HAL to find the Broadcom-specific HAL.
**
** Returns:     None.
**
*******************************************************************************/

void EseAdaptation::InitializeHalDeviceContext() {
  const char* func = "EseAdaptation::InitializeHalDeviceContext";
  ALOGD_IF(nfc_debug_enabled, "%s: enter", func);
  ALOGD_IF(nfc_debug_enabled, "%s: INxpEse::tryGetService()", func);
#if 0
  mHalNxpEse = INxpEse::tryGetService();
  ALOGD_IF(mHalNxpEse == nullptr, "%s: Failed to retrieve the NXP ESE HAL!", func);
  if(mHalNxpEse != nullptr) {
    ALOGD_IF(nfc_debug_enabled, "%s: INxpEse::getService() returned %p (%s)",
             func, mHalNxpEse.get(),
             (mHalNxpEse->isRemote() ? "remote" : "local"));
  }
#endif
  /*Transceive NCI_INIT_CMD*/
  ALOGD_IF(nfc_debug_enabled, "%s: exit", func);
}
/*******************************************************************************
**
** Function:    EseAdaptation::HalDeviceContextDataCallback
**
** Description: Translate generic Android HAL's callback into Broadcom-specific
**              callback function.
**
** Returns:     None.
**
*******************************************************************************/
void EseAdaptation::HalDeviceContextDataCallback(uint16_t data_len,
                                                 uint8_t* p_data) {
  const char* func = "EseAdaptation::HalDeviceContextDataCallback";
  ALOGD_IF(nfc_debug_enabled, "%s: len=%u", func, data_len);
  if (mHalDataCallback) mHalDataCallback(data_len, p_data);
}
#if 0
/*******************************************************************************
**
** Function:    IoctlCallback
**
** Description: Callback from HAL stub for IOCTL api invoked.
**              Output data for IOCTL is sent as argument
**
** Returns:     None.
**
*******************************************************************************/
void IoctlCallback(hidl_vec<uint8_t> outputData) {
  const char* func = "IoctlCallback";
  ese_nxp_ExtnOutputData_t* pOutData =
      (ese_nxp_ExtnOutputData_t*)&outputData[0];
  ALOGD_IF(nfc_debug_enabled, "%s Ioctl Type=%lu", func,
           (unsigned long)pOutData->ioctlType);
  EseAdaptation* pAdaptation = (EseAdaptation*)pOutData->context;
  /*Output Data from stub->Proxy is copied back to output data
   * This data will be sent back to libese*/
  //memcpy(&pAdaptation->mCurrentIoctlData->out, &outputData[0],
  //       sizeof(ese_nxp_ExtnOutputData_t));
}
#endif
/*******************************************************************************
**
** Function:    EseAdaptation::HalIoctl
**
** Description: Calls ioctl to the Ese driver.
**              If called with a arg value of 0x01 than wired access requested,
**              status of the requst would be updated to p_data.
**              If called with a arg value of 0x00 than wired access will be
**              released, status of the requst would be updated to p_data.
**              If called with a arg value of 0x02 than current p61 state would
*be
**              updated to p_data.
**
** Returns:     -1 or 0.
**
*******************************************************************************/
int EseAdaptation::HalIoctl(long arg, void* p_data) {
  const char* func = "EseAdaptation::HalIoctl";
  //hidl_vec<uint8_t> data;
  AutoThreadMutex a(sIoctlLock);
  //ese_nxp_IoctlInOutData_t* pInpOutData = (ese_nxp_IoctlInOutData_t*)p_data;
  ALOGD_IF(nfc_debug_enabled, "%s arg=%ld", func, arg);
  //pInpOutData->inp.context = &EseAdaptation::GetInstance();
  //EseAdaptation::GetInstance().mCurrentIoctlData = pInpOutData;
  //data.setToExternal((uint8_t*)pInpOutData, sizeof(ese_nxp_IoctlInOutData_t));
  //if (mHalNxpEse != nullptr) mHalNxpEse->ioctl(arg, data, IoctlCallback);
  //ALOGD_IF(nfc_debug_enabled, "%s Ioctl Completed for Type=%lu", func,
  //         (unsigned long)pInpOutData->out.ioctlType);
  return (1);//pInpOutData->out.result);
}
#if 0
/*******************************************************************************
**
** Function:    ThreadMutex::ThreadMutex()
**
** Description: class constructor
**
** Returns:     none
**
*******************************************************************************/
ThreadMutex::ThreadMutex() {
  pthread_mutexattr_t mutexAttr;

  pthread_mutexattr_init(&mutexAttr);
  pthread_mutex_init(&mMutex, &mutexAttr);
  pthread_mutexattr_destroy(&mutexAttr);
}

/*******************************************************************************
**
** Function:    ThreadMutex::~ThreadMutex()
**
** Description: class destructor
**
** Returns:     none
**
*******************************************************************************/
ThreadMutex::~ThreadMutex() { pthread_mutex_destroy(&mMutex); }

/*******************************************************************************
**
** Function:    ThreadMutex::lock()
**
** Description: lock kthe mutex
**
** Returns:     none
**
*******************************************************************************/
void ThreadMutex::lock() { pthread_mutex_lock(&mMutex); }

/*******************************************************************************
**
** Function:    ThreadMutex::unblock()
**
** Description: unlock the mutex
**
** Returns:     none
**
*******************************************************************************/
void ThreadMutex::unlock() { pthread_mutex_unlock(&mMutex); }

/*******************************************************************************
**
** Function:    ThreadCondVar::ThreadCondVar()
**
** Description: class constructor
**
** Returns:     none
**
*******************************************************************************/
ThreadCondVar::ThreadCondVar() {
  pthread_condattr_t CondAttr;

  pthread_condattr_init(&CondAttr);
  pthread_cond_init(&mCondVar, &CondAttr);

  pthread_condattr_destroy(&CondAttr);
}

/*******************************************************************************
**
** Function:    ThreadCondVar::~ThreadCondVar()
**
** Description: class destructor
**
** Returns:     none
**
*******************************************************************************/
ThreadCondVar::~ThreadCondVar() { pthread_cond_destroy(&mCondVar); }

/*******************************************************************************
**
** Function:    ThreadCondVar::wait()
**
** Description: wait on the mCondVar
**
** Returns:     none
**
*******************************************************************************/
void ThreadCondVar::wait() {
  pthread_cond_wait(&mCondVar, *this);
  pthread_mutex_unlock(*this);
}

/*******************************************************************************
**
** Function:    ThreadCondVar::signal()
**
** Description: signal the mCondVar
**
** Returns:     none
**
*******************************************************************************/
void ThreadCondVar::signal() {
  AutoThreadMutex a(*this);
  pthread_cond_signal(&mCondVar);
}

/*******************************************************************************
**
** Function:    AutoThreadMutex::AutoThreadMutex()
**
** Description: class constructor, automatically lock the mutex
**
** Returns:     none
**
*******************************************************************************/
AutoThreadMutex::AutoThreadMutex(ThreadMutex& m) : mm(m) { mm.lock(); }

/*******************************************************************************
**
** Function:    AutoThreadMutex::~AutoThreadMutex()
**
** Description: class destructor, automatically unlock the mutex
**
** Returns:     none
**
*******************************************************************************/
AutoThreadMutex::~AutoThreadMutex() { mm.unlock(); }
#endif
