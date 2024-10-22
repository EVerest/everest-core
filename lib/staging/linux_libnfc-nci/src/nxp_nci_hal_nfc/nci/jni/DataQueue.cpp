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

#include "DataQueue.h"

#include <malloc.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

using android::base::StringPrintf;
/*******************************************************************************
**
** Function:        DataQueue
**
** Description:     Initialize member variables.
**
** Returns:         None.
**
*******************************************************************************/
DataQueue::DataQueue() {}

/*******************************************************************************
**
** Function:        ~DataQueue
**
** Description:      Release all resources.
**
** Returns:         None.
**
*******************************************************************************/
DataQueue::~DataQueue() {
  mMutex.lock();
  while (mQueue.empty() == false) {
    tHeader* header = mQueue.front();
    mQueue.pop_front();
    free(header);
  }
  mMutex.unlock();
}

bool DataQueue::isEmpty() {
  mMutex.lock();
  bool retval = mQueue.empty();
  mMutex.unlock();
  return retval;
}

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
bool DataQueue::enqueue(uint8_t* data, uint16_t dataLen) {
  if ((data == NULL) || (dataLen == 0)) return false;

  mMutex.lock();

  bool retval = false;
  tHeader* header = (tHeader*)malloc(sizeof(tHeader) + dataLen);

  if (header) {
    memset(header, 0, sizeof(tHeader));
    header->mDataLen = dataLen;
    memcpy(header + 1, data, dataLen);

    mQueue.push_back(header);

    retval = true;
  } else {
    LOG(ERROR) << StringPrintf("DataQueue::enqueue: out of memory ?????");
  }
  mMutex.unlock();
  return retval;
}

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
bool DataQueue::dequeue(uint8_t* buffer, uint16_t bufferMaxLen,
                        uint16_t& actualLen) {
  mMutex.lock();

  tHeader* header = mQueue.front();
  bool retval = false;

  if (header && buffer && (bufferMaxLen > 0)) {
    if (header->mDataLen <= bufferMaxLen) {
      // caller's buffer is big enough to store all data
      actualLen = header->mDataLen;
      char* src = (char*)(header) + sizeof(tHeader) + header->mOffset;
      memcpy(buffer, src, actualLen);

      mQueue.pop_front();
      free(header);
    } else {
      // caller's buffer is too small
      actualLen = bufferMaxLen;
      char* src = (char*)(header) + sizeof(tHeader) + header->mOffset;
      memcpy(buffer, src, actualLen);
      // adjust offset so the next dequeue() will get the remainder
      header->mDataLen -= actualLen;
      header->mOffset += actualLen;
    }
    retval = true;
  }
  mMutex.unlock();
  return retval;
}
