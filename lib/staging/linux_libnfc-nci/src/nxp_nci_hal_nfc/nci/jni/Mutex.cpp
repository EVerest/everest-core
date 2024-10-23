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

/*
 *  Encapsulate a mutex for thread synchronization.
 */

#include "Mutex.h"
#include "NfcJniUtil.h"

#include <errno.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

using android::base::StringPrintf;

/*******************************************************************************
**
** Function:        Mutex
**
** Description:     Initialize member variables.
**
** Returns:         None.
**
*******************************************************************************/
Mutex::Mutex() {
  memset(&mMutex, 0, sizeof(mMutex));
  int res = pthread_mutex_init(&mMutex, NULL);
  if (res != 0) {
    LOG(ERROR) << StringPrintf("Mutex::Mutex: fail init; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        ~Mutex
**
** Description:     Cleanup all resources.
**
** Returns:         None.
**
*******************************************************************************/
Mutex::~Mutex() {
  int res = pthread_mutex_destroy(&mMutex);
  if (res != 0) {
    LOG(ERROR) << StringPrintf("Mutex::~Mutex: fail destroy; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        lock
**
** Description:     Block the thread and try lock the mutex.
**
** Returns:         None.
**
*******************************************************************************/
void Mutex::lock() {
  int res = pthread_mutex_lock(&mMutex);
  if (res != 0) {
    LOG(ERROR) << StringPrintf("Mutex::lock: fail lock; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        unlock
**
** Description:     Unlock a mutex to unblock a thread.
**
** Returns:         None.
**
*******************************************************************************/
void Mutex::unlock() {
  int res = pthread_mutex_unlock(&mMutex);
  if (res != 0) {
    LOG(ERROR) << StringPrintf("Mutex::unlock: fail unlock; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        tryLock
**
** Description:     Try to lock the mutex.
**
** Returns:         True if the mutex is locked.
**
*******************************************************************************/
bool Mutex::tryLock() {
  int res = pthread_mutex_trylock(&mMutex);
  if ((res != 0) && (res != EBUSY)) {
    LOG(ERROR) << StringPrintf("Mutex::tryLock: error=0x%X", res);
  }
  return res == 0;
}

/*******************************************************************************
**
** Function:        nativeHandle
**
** Description:     Get the handle of the mutex.
**
** Returns:         Handle of the mutex.
**
*******************************************************************************/
pthread_mutex_t* Mutex::nativeHandle() { return &mMutex; }
