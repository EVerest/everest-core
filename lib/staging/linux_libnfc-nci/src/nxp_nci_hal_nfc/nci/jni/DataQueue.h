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
 *  Store data bytes in a variable-size queue.
 */

#pragma once
#include <list>
#include "Mutex.h"
#include "NfcJniUtil.h"
#include "gki.h"

class DataQueue {
 public:
  /*******************************************************************************
  **
  ** Function:        DataQueue
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  DataQueue();

  /*******************************************************************************
  **
  ** Function:        ~DataQueue
  **
  ** Description:      Release all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~DataQueue();

  /*******************************************************************************
  **
  ** Function:        enqueue
  **
  ** Description:     Append data to the queue.
  **                  data: array of bytes
  **                  dataLen: length of the data.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool enqueue(uint8_t* data, uint16_t dataLen);

  /*******************************************************************************
  **
  ** Function:        dequeue
  **
  ** Description:     Retrieve and remove data from the front of the queue.
  **                  buffer: array to store the data.
  **                  bufferMaxLen: maximum size of the buffer.
  **                  actualLen: actual length of the data.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool dequeue(uint8_t* buffer, uint16_t bufferMaxLen, uint16_t& actualLen);

  /*******************************************************************************
  **
  ** Function:        isEmpty
  **
  ** Description:     Whether the queue is empty.
  **
  ** Returns:         True if empty.
  **
  *******************************************************************************/
  bool isEmpty();

 private:
  struct tHeader {
    uint16_t mDataLen;  // number of octets of data
    uint16_t mOffset;   // offset of the first octet of data
  };
  typedef std::list<tHeader*> Queue;

  Queue mQueue;
  Mutex mMutex;
};
