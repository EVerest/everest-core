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

#pragma once
#include <pthread.h>

class Mutex {
 public:
  /*******************************************************************************
  **
  ** Function:        Mutex
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  Mutex();

  /*******************************************************************************
  **
  ** Function:        ~Mutex
  **
  ** Description:     Cleanup all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~Mutex();

  /*******************************************************************************
  **
  ** Function:        lock
  **
  ** Description:     Block the thread and try lock the mutex.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void lock();

  /*******************************************************************************
  **
  ** Function:        unlock
  **
  ** Description:     Unlock a mutex to unblock a thread.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void unlock();

  /*******************************************************************************
  **
  ** Function:        tryLock
  **
  ** Description:     Try to lock the mutex.
  **
  ** Returns:         True if the mutex is locked.
  **
  *******************************************************************************/
  bool tryLock();

  /*******************************************************************************
  **
  ** Function:        nativeHandle
  **
  ** Description:     Get the handle of the mutex.
  **
  ** Returns:         Handle of the mutex.
  **
  *******************************************************************************/
  pthread_mutex_t* nativeHandle();

  class Autolock {
   public:
    inline Autolock(Mutex& mutex) : mLock(mutex) { mLock.lock(); }
    inline Autolock(Mutex* mutex) : mLock(*mutex) { mLock.lock(); }
    inline ~Autolock() { mLock.unlock(); }

   private:
    Mutex& mLock;
  };

 private:
  pthread_mutex_t mMutex;
};

typedef Mutex::Autolock AutoMutex;
