/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
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
#ifndef GKI_INT_H
#define GKI_INT_H

#include <pthread.h>
#include "gki_common.h"

/**********************************************************************
** OS specific definitions
*/
#ifdef ANDROID
#include <sys/times.h>
#endif

typedef struct {
  pthread_mutex_t GKI_mutex;
  pthread_t thread_id[GKI_MAX_TASKS];
  pthread_mutex_t thread_evt_mutex[GKI_MAX_TASKS];
  pthread_cond_t thread_evt_cond[GKI_MAX_TASKS];
  pthread_mutex_t thread_timeout_mutex[GKI_MAX_TASKS];
  pthread_cond_t thread_timeout_cond[GKI_MAX_TASKS];
  int no_timer_suspend; /* 1: no suspend, 0 stop calling GKI_timer_update() */
  pthread_mutex_t gki_timer_mutex;
  pthread_cond_t gki_timer_cond;
} tGKI_OS;

/* condition to exit or continue GKI_run() timer loop */
#define GKI_TIMER_TICK_RUN_COND 1
#define GKI_TIMER_TICK_STOP_COND 0
#define GKI_TIMER_TICK_EXIT_COND 2

extern void gki_system_tick_start_stop_cback(bool start);

/* Contains common control block as well as OS specific variables */
typedef struct {
  tGKI_OS os;
  tGKI_COM_CB com;
} tGKI_CB;

extern tGKI_CB gki_cb;

#endif
