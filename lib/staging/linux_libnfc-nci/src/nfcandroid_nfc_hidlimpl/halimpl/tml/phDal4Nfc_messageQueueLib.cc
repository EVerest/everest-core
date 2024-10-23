/*
 * Copyright (C) 2010-2014 NXP Semiconductors
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
 * DAL independent message queue implementation for Android (can be used under
 * Linux too)
 */

#include <errno.h>
#include <linux/ipc.h>
#include <phDal4Nfc_messageQueueLib.h>
#include <phNxpLog.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct phDal4Nfc_message_queue_item {
  phLibNfc_Message_t nMsg;
  struct phDal4Nfc_message_queue_item* pPrev;
  struct phDal4Nfc_message_queue_item* pNext;
} phDal4Nfc_message_queue_item_t;

typedef struct phDal4Nfc_message_queue {
  phDal4Nfc_message_queue_item_t* pItems;
  pthread_mutex_t nCriticalSectionMutex;
  sem_t nProcessSemaphore;

} phDal4Nfc_message_queue_t;

/*******************************************************************************
**
** Function         phDal4Nfc_msgget
**
** Description      Allocates message queue
**
** Parameters       Ignored, included only for Linux queue API compatibility
**
** Returns          (int) value of pQueue if successful
**                  -1, if failed to allocate memory or to init mutex
**
*******************************************************************************/
intptr_t phDal4Nfc_msgget(key_t key, int msgflg) {
  phDal4Nfc_message_queue_t* pQueue;
  UNUSED(key);
  UNUSED(msgflg);
  pQueue =
      (phDal4Nfc_message_queue_t*)malloc(sizeof(phDal4Nfc_message_queue_t));
  if (pQueue == NULL) return -1;
  memset(pQueue, 0, sizeof(phDal4Nfc_message_queue_t));
  if (pthread_mutex_init(&pQueue->nCriticalSectionMutex, NULL) != 0) {
    free(pQueue);
    return -1;
  }
  if (sem_init(&pQueue->nProcessSemaphore, 0, 0) == -1) {
    free(pQueue);
    return -1;
  }

  return ((intptr_t)pQueue);
}

/*******************************************************************************
**
** Function         phDal4Nfc_msgrelease
**
** Description      Releases message queue
**
** Parameters       msqid - message queue handle
**
** Returns          None
**
*******************************************************************************/
void phDal4Nfc_msgrelease(intptr_t msqid) {
  phDal4Nfc_message_queue_t* pQueue = (phDal4Nfc_message_queue_t*)msqid;

  if (pQueue != NULL) {
    sem_post(&pQueue->nProcessSemaphore);
    usleep(3000);
    //patch applied from latest Android code base
    if (sem_destroy(&pQueue->nProcessSemaphore)) {
      NXPLOG_TML_E("Failed to destroy semaphore (errno=0x%08x)", errno);
    }
    pthread_mutex_destroy(&pQueue->nCriticalSectionMutex);

    free(pQueue);
  }

  return;
}

/*******************************************************************************
**
** Function         phDal4Nfc_msgctl
**
** Description      Destroys message queue
**
** Parameters       msqid - message queue handle
**                  cmd, buf - ignored, included only for Linux queue API
**                  compatibility
**
** Returns          0,  if successful
**                  -1, if invalid handle is passed
**
*******************************************************************************/
int phDal4Nfc_msgctl(intptr_t msqid, int cmd, void* buf) {
  phDal4Nfc_message_queue_t* pQueue;
  phDal4Nfc_message_queue_item_t* p;
  UNUSED(cmd);
  UNUSED(buf);
  if (msqid == 0) return -1;

  pQueue = (phDal4Nfc_message_queue_t*)msqid;
  pthread_mutex_lock(&pQueue->nCriticalSectionMutex);
  if (pQueue->pItems != NULL) {
    p = pQueue->pItems;
    while (p->pNext != NULL) {
      p = p->pNext;
    }
    while (p->pPrev != NULL) {
      p = p->pPrev;
      free(p->pNext);
      p->pNext = NULL;
    }
    free(p);
  }
  pQueue->pItems = NULL;
  pthread_mutex_unlock(&pQueue->nCriticalSectionMutex);
  pthread_mutex_destroy(&pQueue->nCriticalSectionMutex);
  free(pQueue);

  return 0;
}

/*******************************************************************************
**
** Function         phDal4Nfc_msgsnd
**
** Description      Sends a message to the queue. The message will be added at
**                  the end of the queue as appropriate for FIFO policy
**
** Parameters       msqid  - message queue handle
**                  msgp   - message to be sent
**                  msgsz  - message size
**                  msgflg - ignored
**
** Returns          0,  if successful
**                  -1, if invalid parameter passed or failed to allocate memory
**
*******************************************************************************/
intptr_t phDal4Nfc_msgsnd(intptr_t msqid, phLibNfc_Message_t* msg, int msgflg) {
  phDal4Nfc_message_queue_t* pQueue;
  phDal4Nfc_message_queue_item_t* p;
  phDal4Nfc_message_queue_item_t* pNew;
  UNUSED(msgflg);
  if ((msqid == 0) || (msg == NULL)) return -1;

  pQueue = (phDal4Nfc_message_queue_t*)msqid;
  pNew = (phDal4Nfc_message_queue_item_t*)malloc(
      sizeof(phDal4Nfc_message_queue_item_t));
  if (pNew == NULL) return -1;
  memset(pNew, 0, sizeof(phDal4Nfc_message_queue_item_t));
  memcpy(&pNew->nMsg, msg, sizeof(phLibNfc_Message_t));
  pthread_mutex_lock(&pQueue->nCriticalSectionMutex);

  if (pQueue->pItems != NULL) {
    p = pQueue->pItems;
    while (p->pNext != NULL) {
      p = p->pNext;
    }
    p->pNext = pNew;
    pNew->pPrev = p;
  } else {
    pQueue->pItems = pNew;
  }
  pthread_mutex_unlock(&pQueue->nCriticalSectionMutex);

  sem_post(&pQueue->nProcessSemaphore);

  return 0;
}

/*******************************************************************************
**
** Function         phDal4Nfc_msgrcv
**
** Description      Gets the oldest message from the queue.
**                  If the queue is empty the function waits (blocks on a mutex)
**                  until a message is posted to the queue with phDal4Nfc_msgsnd
**
** Parameters       msqid  - message queue handle
**                  msgp   - message to be received
**                  msgsz  - message size
**                  msgtyp - ignored
**                  msgflg - ignored
**
** Returns          0,  if successful
**                  -1, if invalid parameter passed
**
*******************************************************************************/
int phDal4Nfc_msgrcv(intptr_t msqid, phLibNfc_Message_t* msg, long msgtyp,
                     int msgflg) {
  phDal4Nfc_message_queue_t* pQueue;
  phDal4Nfc_message_queue_item_t* p;
  UNUSED(msgflg);
  UNUSED(msgtyp);
  if ((msqid == 0) || (msg == NULL)) return -1;

  pQueue = (phDal4Nfc_message_queue_t*)msqid;

  sem_wait(&pQueue->nProcessSemaphore);

  pthread_mutex_lock(&pQueue->nCriticalSectionMutex);

  if (pQueue->pItems != NULL) {
    memcpy(msg, &(pQueue->pItems)->nMsg, sizeof(phLibNfc_Message_t));
    p = pQueue->pItems->pNext;
    free(pQueue->pItems);
    pQueue->pItems = p;
  }
  pthread_mutex_unlock(&pQueue->nCriticalSectionMutex);

  return 0;
}
