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

#pragma once
#include <pthread.h>
#include "Mutex.h"

class CondVar {
 public:
  /*******************************************************************************
  **
  ** Function:        CondVar
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  CondVar();

  /*******************************************************************************
  **
  ** Function:        ~CondVar
  **
  ** Description:     Cleanup all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~CondVar();

  /*******************************************************************************
  **
  ** Function:        wait
  **
  ** Description:     Block the caller and wait for a condition.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void wait(Mutex& mutex);

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
  bool wait(Mutex& mutex, long millisec);

  /*******************************************************************************
  **
  ** Function:        notifyOne
  **
  ** Description:     Unblock the waiting thread.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void notifyOne();

 private:
  pthread_cond_t mCondition;
};
