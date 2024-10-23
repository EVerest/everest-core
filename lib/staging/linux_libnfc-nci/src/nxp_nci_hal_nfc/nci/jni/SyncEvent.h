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
 *  Synchronize two or more threads using a condition variable and a mutex.
 */
#pragma once
#include "CondVar.h"
#include "Mutex.h"

class SyncEvent {
 public:
  /*******************************************************************************
  **
  ** Function:        ~SyncEvent
  **
  ** Description:     Cleanup all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~SyncEvent() {}

  /*******************************************************************************
  **
  ** Function:        start
  **
  ** Description:     Start a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void start() { mMutex.lock(); }

  /*******************************************************************************
  **
  ** Function:        wait
  **
  ** Description:     Block the thread and wait for the event to occur.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void wait() { mCondVar.wait(mMutex); }

  /*******************************************************************************
  **
  ** Function:        wait
  **
  ** Description:     Block the thread and wait for the event to occur.
  **                  millisec: Timeout in milliseconds.
  **
  ** Returns:         True if wait is successful; false if timeout occurs.
  **
  *******************************************************************************/
  bool wait(long millisec) {
    bool retVal = mCondVar.wait(mMutex, millisec);
    return retVal;
  }

  /*******************************************************************************
  **
  ** Function:        notifyOne
  **
  ** Description:     Notify a blocked thread that the event has occured.
  *Unblocks it.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void notifyOne() { mCondVar.notifyOne(); }

  /*******************************************************************************
  **
  ** Function:        end
  **
  ** Description:     End a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void end() { mMutex.unlock(); }

 private:
  CondVar mCondVar;
  Mutex mMutex;
};

/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
**
**  Name:           SyncEventGuard
**
**  Description:    Automatically start and end a synchronization event.
**
*****************************************************************************/
class SyncEventGuard {
 public:
  /*******************************************************************************
  **
  ** Function:        SyncEventGuard
  **
  ** Description:     Start a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  SyncEventGuard(SyncEvent& event) : mEvent(event) {
    event.start();  // automatically start operation
  };

  /*******************************************************************************
  **
  ** Function:        ~SyncEventGuard
  **
  ** Description:     End a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~SyncEventGuard() {
    mEvent.end();  // automatically end operation
  };

 private:
  SyncEvent& mEvent;
};
