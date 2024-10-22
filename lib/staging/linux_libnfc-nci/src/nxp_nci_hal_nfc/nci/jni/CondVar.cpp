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
 *  Encapsulate a condition variable for thread synchronization.
 */

#include "CondVar.h"
#include "NfcJniUtil.h"

#include <errno.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

using android::base::StringPrintf;

/*******************************************************************************
**
** Function:        CondVar
**
** Description:     Initialize member variables.
**
** Returns:         None.
**
*******************************************************************************/
CondVar::CondVar() {
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  memset(&mCondition, 0, sizeof(mCondition));
  int const res = pthread_cond_init(&mCondition, &attr);
  if (res) {
    LOG(ERROR) << StringPrintf("CondVar::CondVar: fail init; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        ~CondVar
**
** Description:     Cleanup all resources.
**
** Returns:         None.
**
*******************************************************************************/
CondVar::~CondVar() {
  int const res = pthread_cond_destroy(&mCondition);
  if (res) {
    LOG(ERROR) << StringPrintf("CondVar::~CondVar: fail destroy; error=0x%X",
                               res);
  }
}

/*******************************************************************************
**
** Function:        wait
**
** Description:     Block the caller and wait for a condition.
**
** Returns:         None.
**
*******************************************************************************/
void CondVar::wait(Mutex& mutex) {
  int const res = pthread_cond_wait(&mCondition, mutex.nativeHandle());
  if (res) {
    LOG(ERROR) << StringPrintf("CondVar::wait: fail wait; error=0x%X", res);
  }
}

/*******************************************************************************
**
** Function:        wait
**
** Description:     Block the caller and wait for a condition.
**                  millisec: Timeout in milliseconds.
**
** Returns:         True if wait is successful; false if timeout occurs.
**
*******************************************************************************/
bool CondVar::wait(Mutex& mutex, long millisec) {
  bool retVal = false;
  struct timespec absoluteTime = {0,0};
#ifdef LINUX
  if (clock_gettime(/*CLOCK_MONOTONIC*/CLOCK_REALTIME, &absoluteTime) == -1) {
#else
  if (clock_gettime(CLOCK_MONOTONIC, &absoluteTime) == -1) {
#endif
    LOG(ERROR) << StringPrintf("CondVar::wait: fail get time; errno=0x%X",
                               errno);
  } else {
    absoluteTime.tv_sec += millisec / 1000;
    long ns = absoluteTime.tv_nsec + ((millisec % 1000) * 1000000);
    if (ns > 1000000000) {
      absoluteTime.tv_sec++;
      absoluteTime.tv_nsec = ns - 1000000000;
    } else
      absoluteTime.tv_nsec = ns;
  }

  int waitResult =
      pthread_cond_timedwait(&mCondition, mutex.nativeHandle(), &absoluteTime);
  if ((waitResult != 0) && (waitResult != ETIMEDOUT))
    LOG(ERROR) << StringPrintf("CondVar::wait: fail timed wait; error=0x%X",
                               waitResult);
  retVal = (waitResult == 0);  // waited successfully
  return retVal;
}

/*******************************************************************************
**
** Function:        notifyOne
**
** Description:     Unblock the waiting thread.
**
** Returns:         None.
**
*******************************************************************************/
void CondVar::notifyOne() {
  int const res = pthread_cond_signal(&mCondition);
  if (res) {
    LOG(ERROR) << StringPrintf("CondVar::notifyOne: fail signal; error=0x%X",
                               res);
  }
}
