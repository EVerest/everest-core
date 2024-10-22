/*
 *
 *  Copyright (C) 2013-2014 NXP Semiconductors
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

#ifndef _PHNXPNCIHAL_UTILS_H_
#define _PHNXPNCIHAL_UTILS_H_

#include <assert.h>
#include <phNfcStatus.h>
#include <pthread.h>
#include <semaphore.h>

/********************* Definitions and structures *****************************/

/* List structures */
struct listNode {
  void* pData;
  struct listNode* pNext;
};

struct listHead {
  struct listNode* pFirst;
  pthread_mutex_t mutex;
};

/* Semaphore handling structure */
typedef struct phNxpNciHal_Sem {
  /* Semaphore used to wait for callback */
  sem_t sem;

  /* Used to store the status sent by the callback */
  NFCSTATUS status;

  /* Used to provide a local context to the callback */
  void* pContext;

} phNxpNciHal_Sem_t;

/* Semaphore helper macros */
#define SEM_WAIT(cb_data) sem_wait(&((cb_data).sem))
#define SEM_POST(p_cb_data) sem_post(&((p_cb_data)->sem))

/* Semaphore and mutex monitor */
typedef struct phNxpNciHal_Monitor {
  /* Mutex protecting native library against reentrance */
  pthread_mutex_t reentrance_mutex;

  /* Mutex protecting native library against concurrency */
  pthread_mutex_t concurrency_mutex;

  /* List used to track pending semaphores waiting for callback */
  struct listHead sem_list;

} phNxpNciHal_Monitor_t;

/************************ Exposed functions ***********************************/
/* List functions */
int listInit(struct listHead* pList);
int listDestroy(struct listHead* pList);
int listAdd(struct listHead* pList, void* pData);
int listRemove(struct listHead* pList, void* pData);
int listGetAndRemoveNext(struct listHead* pList, void** ppData);
void listDump(struct listHead* pList);

/* NXP NCI HAL utility functions */
phNxpNciHal_Monitor_t* phNxpNciHal_init_monitor(void);
void phNxpNciHal_cleanup_monitor(void);
phNxpNciHal_Monitor_t* phNxpNciHal_get_monitor(void);
NFCSTATUS phNxpNciHal_init_cb_data(phNxpNciHal_Sem_t* pCallbackData,
                                   void* pContext);
void phNxpNciHal_cleanup_cb_data(phNxpNciHal_Sem_t* pCallbackData);
void phNxpNciHal_releaseall_cb_data(void);
void phNxpNciHal_print_packet(const char* pString, const uint8_t* p_data,
                              uint16_t len);
void phNxpNciHal_emergency_recovery(void);

/* Lock unlock helper macros */
/* Lock unlock helper macros */
#define REENTRANCE_LOCK()        \
  if (phNxpNciHal_get_monitor()) \
  pthread_mutex_lock(&phNxpNciHal_get_monitor()->reentrance_mutex)
#define REENTRANCE_UNLOCK()      \
  if (phNxpNciHal_get_monitor()) \
  pthread_mutex_unlock(&phNxpNciHal_get_monitor()->reentrance_mutex)
#define CONCURRENCY_LOCK()       \
  if (phNxpNciHal_get_monitor()) \
  pthread_mutex_lock(&phNxpNciHal_get_monitor()->concurrency_mutex)
#define CONCURRENCY_UNLOCK()     \
  if (phNxpNciHal_get_monitor()) \
  pthread_mutex_unlock(&phNxpNciHal_get_monitor()->concurrency_mutex)

#endif /* _PHNXPNCIHAL_UTILS_H_ */
