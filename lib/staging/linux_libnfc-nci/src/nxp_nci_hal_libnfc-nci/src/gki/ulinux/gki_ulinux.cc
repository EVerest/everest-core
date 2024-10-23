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
#include <errno.h>
#include <malloc.h>
#include <pthread.h> /* must be 1st header defined  */

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "gki_int.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "bt_trace.h"
//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* Temp android logging...move to android tgt config file */

#ifndef LINUX_NATIVE
#else
#define LOGV(format, ...) fprintf(stdout, LOG_TAG format, ##__VA_ARGS__)
#define LOGE(format, ...) fprintf(stderr, LOG_TAG format, ##__VA_ARGS__)
#define LOGI(format, ...) fprintf(stdout, LOG_TAG format, ##__VA_ARGS__)

#define SCHED_NORMAL 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3

#endif

/* Define the structure that holds the GKI variables
*/
tGKI_CB gki_cb;

#define NANOSEC_PER_MILLISEC (1000000)
#define NSEC_PER_SEC (1000 * NANOSEC_PER_MILLISEC)

/* works only for 1ms to 1000ms heart beat ranges */
#define LINUX_SEC (1000 / TICKS_PER_SEC)
// #define GKI_TICK_TIMER_DEBUG

/* this kind of mutex go into tGKI_OS control block!!!! */
/* static pthread_mutex_t GKI_sched_mutex; */
/*static pthread_mutex_t thread_delay_mutex;
static pthread_cond_t thread_delay_cond;
static pthread_mutex_t gki_timer_update_mutex;
static pthread_cond_t   gki_timer_update_cond;
*/
#ifdef NO_GKI_RUN_RETURN
static pthread_t timer_thread_id = 0;
#endif

typedef struct {
  uint8_t task_id;         /* GKI task id */
  TASKPTR task_entry;      /* Task entry function*/
  uintptr_t params;        /* Extra params to pass to task entry function */
  pthread_cond_t* pCond;   /* for android*/
  pthread_mutex_t* pMutex; /* for android*/
} gki_pthread_info_t;
gki_pthread_info_t gki_pthread_info[GKI_MAX_TASKS];

/*******************************************************************************
**
** Function         gki_task_entry
**
** Description      entry point of GKI created tasks
**
** Returns          void
**
*******************************************************************************/
void* gki_task_entry(void* params) {
  pthread_t thread_id = pthread_self();
  gki_pthread_info_t* p_pthread_info = (gki_pthread_info_t*)params;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "gki_task_entry task_id=%i, thread_id=%lx/%lx, pCond/pMutex=%p/%p",
      p_pthread_info->task_id, gki_cb.os.thread_id[p_pthread_info->task_id],
      pthread_self(), p_pthread_info->pCond, p_pthread_info->pMutex);

  gki_cb.os.thread_id[p_pthread_info->task_id] = thread_id;
  /* Call the actual thread entry point */
  (p_pthread_info->task_entry)(p_pthread_info->params);

  LOG(ERROR) << StringPrintf("gki_task task_id=%i terminating",
                             p_pthread_info->task_id);
  gki_cb.os.thread_id[p_pthread_info->task_id] = 0;

  return nullptr;
}
/* end android */

/*******************************************************************************
**
** Function         GKI_init
**
** Description      This function is called once at startup to initialize
**                  all the timer structures.
**
** Returns          void
**
*******************************************************************************/

void GKI_init(void) {
  pthread_mutexattr_t attr;
  tGKI_OS* p_os;

  gki_buffer_init();
  gki_timers_init();
  gki_cb.com.OSTicks = (uint32_t)times(nullptr);

  pthread_mutexattr_init(&attr);

#ifndef __CYGWIN__
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
  p_os = &gki_cb.os;
  pthread_mutex_init(&p_os->GKI_mutex, &attr);
  /* pthread_mutex_init(&GKI_sched_mutex, NULL); */
  /* pthread_mutex_init(&thread_delay_mutex, NULL); */ /* used in GKI_delay */
  /* pthread_cond_init (&thread_delay_cond, NULL); */

  /* Initialiase GKI_timer_update suspend variables & mutexes to be in running
   * state.
   * this works too even if GKI_NO_TICK_STOP is defined in btld.txt */
  p_os->no_timer_suspend = GKI_TIMER_TICK_RUN_COND;
  pthread_mutex_init(&p_os->gki_timer_mutex, nullptr);
  pthread_cond_init(&p_os->gki_timer_cond, nullptr);
}

/*******************************************************************************
**
** Function         GKI_get_os_tick_count
**
** Description      This function is called to retrieve the native OS system
**                  tick.
**
** Returns          Tick count of native OS.
**
*******************************************************************************/
uint32_t GKI_get_os_tick_count(void) {
  /* TODO - add any OS specific code here
  **/
  return (gki_cb.com.OSTicks);
}

/*******************************************************************************
**
** Function         GKI_create_task
**
** Description      This function is called to create a new OSS task.
**
** Parameters:      task_entry  - (input) pointer to the entry function of the
**                                        task
**                  task_id     - (input) Task id is mapped to priority
**                  taskname    - (input) name given to the task
**                  stack       - (input) pointer to the top of the stack
**                                        (highest memory location)
**                  stacksize   - (input) size of the stack allocated for the
**                                        task
**
** Returns          GKI_SUCCESS if all OK, GKI_FAILURE if any problem
**
** NOTE             This function take some parameters that may not be needed
**                  by your particular OS. They are here for compatability
**                  of the function prototype.
**
*******************************************************************************/
uint8_t GKI_create_task(TASKPTR task_entry, uint8_t task_id, int8_t* taskname,
                        uint16_t* stack, uint16_t stacksize, void* pCondVar,
                        void* pMutex) {
  struct sched_param param;
  int policy, ret = 0;
  pthread_condattr_t attr;
  pthread_attr_t attr1;

  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "GKI_create_task func=0x%p  id=%d  name=%s  stack=0x%p  stackSize=%d",
      task_entry, task_id, taskname, stack, stacksize);

  if (task_id >= GKI_MAX_TASKS) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("Error! task ID > max task allowed");
    return (GKI_FAILURE);
  }

  gki_cb.com.OSRdyTbl[task_id] = TASK_READY;
  gki_cb.com.OSTName[task_id] = taskname;
  gki_cb.com.OSWaitTmr[task_id] = 0;
  gki_cb.com.OSWaitEvt[task_id] = 0;

  /* Initialize mutex and condition variable objects for events and timeouts */
  pthread_mutex_init(&gki_cb.os.thread_evt_mutex[task_id], nullptr);
  pthread_cond_init(&gki_cb.os.thread_evt_cond[task_id], &attr);
  pthread_mutex_init(&gki_cb.os.thread_timeout_mutex[task_id], nullptr);
  pthread_cond_init(&gki_cb.os.thread_timeout_cond[task_id], &attr);

  pthread_attr_init(&attr1);
/* by default, pthread creates a joinable thread */
#if (FALSE == GKI_PTHREAD_JOINABLE)
  pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "GKI creating task %i, pCond/pMutex=%p/%p", task_id, pCondVar, pMutex);
#else
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI creating JOINABLE task %i", task_id);
#endif

  /* On Android, the new tasks starts running before
   * 'gki_cb.os.thread_id[task_id]' is initialized */
  /* Pass task_id to new task so it can initialize gki_cb.os.thread_id[task_id]
   * for it calls GKI_wait */
  gki_pthread_info[task_id].task_id = task_id;
  gki_pthread_info[task_id].task_entry = task_entry;
  gki_pthread_info[task_id].params = 0;
  gki_pthread_info[task_id].pCond = (pthread_cond_t*)pCondVar;
  gki_pthread_info[task_id].pMutex = (pthread_mutex_t*)pMutex;

  ret = pthread_create(&gki_cb.os.thread_id[task_id], &attr1, gki_task_entry,
                       &gki_pthread_info[task_id]);

  if (ret != 0) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("pthread_create failed(%d), %s!", ret, taskname);
    return GKI_FAILURE;
  }

  if (pthread_getschedparam(gki_cb.os.thread_id[task_id], &policy, &param) ==
      0) {
#if (PBS_SQL_TASK == TRUE)
    if (task_id == PBS_SQL_TASK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("PBS SQL lowest priority task");
      policy = SCHED_NORMAL;
    } else
#endif
    {
      policy = SCHED_RR;
      param.sched_priority = 30 - task_id - 2;
    }
    pthread_setschedparam(gki_cb.os.thread_id[task_id], policy, &param);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "Leaving GKI_create_task %p %d %lx %s %p %d", task_entry, task_id,
      gki_cb.os.thread_id[task_id], taskname, stack, stacksize);

  return (GKI_SUCCESS);
}

/*******************************************************************************
**
** Function         GKI_shutdown
**
** Description      shutdowns the GKI tasks/threads in from max task id to 0 and
**                  frees pthread resources!
**                  IMPORTANT: in case of join method, GKI_shutdown must be
**                  called outside a GKI thread context!
**
** Returns          void
**
*******************************************************************************/
void GKI_shutdown(void) {
  uint8_t task_id;
  volatile int* p_run_cond = &gki_cb.os.no_timer_suspend;
  int oldCOnd = 0;
#if (FALSE == GKI_PTHREAD_JOINABLE)
  int i = 0;
#else
  int result;
#endif

  /* release threads and set as TASK_DEAD. going from low to high priority fixes
   * GKI_exception problem due to btu->hci sleep request events  */
  for (task_id = GKI_MAX_TASKS; task_id > 0; task_id--) {
    if (gki_cb.com.OSRdyTbl[task_id - 1] != TASK_DEAD) {
      gki_cb.com.OSRdyTbl[task_id - 1] = TASK_DEAD;

      /* paranoi settings, make sure that we do not execute any mailbox events
       */
      gki_cb.com.OSWaitEvt[task_id - 1] &=
          ~(TASK_MBOX_0_EVT_MASK | TASK_MBOX_1_EVT_MASK | TASK_MBOX_2_EVT_MASK |
            TASK_MBOX_3_EVT_MASK);
      GKI_send_event(task_id - 1, EVENT_MASK(GKI_SHUTDOWN_EVT));

#if (FALSE == GKI_PTHREAD_JOINABLE)
      i = 0;

      while ((gki_cb.com.OSWaitEvt[task_id - 1] != 0) && (++i < 10))
        usleep(100 * 1000);
#else
      /* wait for proper Arnold Schwarzenegger task state */
      result = pthread_join(gki_cb.os.thread_id[task_id - 1], NULL);
      if (result < 0) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("FAILED: result: %d", result);
      }
#endif
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("task %s dead", gki_cb.com.OSTName[task_id - 1]);
      GKI_exit_task(task_id - 1);
    }
  }

  /* Destroy mutex and condition variable objects */
  pthread_mutex_destroy(&gki_cb.os.GKI_mutex);
/*    pthread_mutex_destroy(&GKI_sched_mutex); */
/*    pthread_mutex_destroy(&thread_delay_mutex);
 pthread_cond_destroy (&thread_delay_cond); */
#if (FALSE == GKI_PTHREAD_JOINABLE)
  i = 0;
#endif

#ifdef NO_GKI_RUN_RETURN
  shutdown_timer = 1;
#endif
  oldCOnd = *p_run_cond;
  *p_run_cond = GKI_TIMER_TICK_EXIT_COND;
  if (oldCOnd == GKI_TIMER_TICK_STOP_COND)
    pthread_cond_signal(&gki_cb.os.gki_timer_cond);
}

/*******************************************************************************
 **
 ** Function        GKI_run
 **
 ** Description     This function runs a task
 **
 ** Parameters:     start: TRUE start system tick (again), FALSE stop
 **
 ** Returns         void
 **
 ******************************************************************************/
void gki_system_tick_start_stop_cback(bool start) {
  tGKI_OS* p_os = &gki_cb.os;
  volatile int* p_run_cond = &p_os->no_timer_suspend;
  if (start == false) {
    /* this can lead to a race condition. however as we only read this variable
     * in the timer loop
     * we should be fine with this approach. otherwise uncomment below mutexes.
     */
    /* GKI_disable(); */
    *p_run_cond = GKI_TIMER_TICK_STOP_COND;
/* GKI_enable(); */
  } else {
    /* restart GKI_timer_update() loop */
    *p_run_cond = GKI_TIMER_TICK_RUN_COND;
    pthread_mutex_lock(&p_os->gki_timer_mutex);
    pthread_cond_signal(&p_os->gki_timer_cond);
    pthread_mutex_unlock(&p_os->gki_timer_mutex);
  }
}

/*******************************************************************************
**
** Function         timer_thread
**
** Description      Timer thread
**
** Parameters:      id  - (input) timer ID
**
** Returns          void
**
*******************************************************************************/
#ifdef NO_GKI_RUN_RETURN
void timer_thread(signed long id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s enter", __func__);
  struct timespec delay;
  int timeout = 1000; /* 10  ms per system tick  */
  int err;

  while (!shutdown_timer) {
    delay.tv_sec = timeout / 1000;
    delay.tv_nsec = 1000 * 1000 * (timeout % 1000);

    /* [u]sleep can't be used because it uses SIGALRM */

    do {
      err = nanosleep(&delay, &delay);
    } while (err < 0 && errno == EINTR);

    GKI_timer_update(1);
  }
  LOG(ERROR) << StringPrintf("%s exit", __func__);
  return;
}
#endif

/*******************************************************************************
**
** Function         GKI_run
**
** Description      This function runs a task
**
** Parameters:      p_task_id  - (input) pointer to task id
**
** Returns          void
**
** NOTE             This function is only needed for operating systems where
**                  starting a task is a 2-step process. Most OS's do it in
**                  one step, If your OS does it in one step, this function
**                  should be empty.
*******************************************************************************/
void GKI_run(__attribute__((unused)) void* p_task_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s enter", __func__);
  struct timespec delay;
  int err = 0;
  volatile int* p_run_cond = &gki_cb.os.no_timer_suspend;

#ifndef GKI_NO_TICK_STOP
  /* register start stop function which disable timer loop in GKI_run() when no
   * timers are
   * in any GKI/BTA/BTU this should save power when BTLD is idle! */
  GKI_timer_queue_register_callback(gki_system_tick_start_stop_cback);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Start/Stop GKI_timer_update_registered!");
#endif

#ifdef NO_GKI_RUN_RETURN
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_run == NO_GKI_RUN_RETURN");
  pthread_attr_t timer_attr;

  shutdown_timer = 0;

  pthread_attr_init(&timer_attr);
  pthread_attr_setdetachstate(&timer_attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&timer_thread_id, &timer_attr, timer_thread, NULL) != 0) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "GKI_run: pthread_create failed to create timer_thread!");
    return GKI_FAILURE;
  }
#else
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_run, run_cond(%p)=%d ", p_run_cond, *p_run_cond);
  for (; GKI_TIMER_TICK_EXIT_COND != *p_run_cond;) {
    do {
      /* adjust hear bit tick in btld by changning TICKS_PER_SEC!!!!! this
       * formula works only for
       * 1-1000ms heart beat units! */
      delay.tv_sec = LINUX_SEC / 1000;
      delay.tv_nsec = 1000 * 1000 * (LINUX_SEC % 1000);

      /* [u]sleep can't be used because it uses SIGALRM */
      do {
        err = nanosleep(&delay, &delay);
      } while (err < 0 && errno == EINTR);

      if (GKI_TIMER_TICK_RUN_COND != *p_run_cond) break;  // GKI has shutdown

      /* the unit should be alsways 1 (1 tick). only if you vary for some reason
       * heart beat tick
       * e.g. power saving you may want to provide more ticks
       */
      GKI_timer_update(1);
    } while (GKI_TIMER_TICK_RUN_COND == *p_run_cond);

/* currently on reason to exit above loop is no_timer_suspend ==
 * GKI_TIMER_TICK_STOP_COND
 * block timer main thread till re-armed by  */
#ifdef GKI_TICK_TIMER_DEBUG
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(">>> SUSPENDED");
#endif
    if (GKI_TIMER_TICK_EXIT_COND != *p_run_cond) {
      pthread_mutex_lock(&gki_cb.os.gki_timer_mutex);
      pthread_cond_wait(&gki_cb.os.gki_timer_cond, &gki_cb.os.gki_timer_mutex);
      pthread_mutex_unlock(&gki_cb.os.gki_timer_mutex);
    }
/* potentially we need to adjust os gki_cb.com.OSTicks */

#ifdef GKI_TICK_TIMER_DEBUG
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf(">>> RESTARTED run_cond: %d", *p_run_cond);
#endif
  } /* for */
#endif
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s exit", __func__);
}

/*******************************************************************************
**
** Function         GKI_stop
**
** Description      This function is called to stop
**                  the tasks and timers when the system is being stopped
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to use it in your own implementation,
**                  put specific code here.
**
*******************************************************************************/
void GKI_stop(void) {
  uint8_t task_id;

  /*  gki_queue_timer_cback(FALSE); */
  /* TODO - add code here if needed*/

  for (task_id = 0; task_id < GKI_MAX_TASKS; task_id++) {
    if (gki_cb.com.OSRdyTbl[task_id] != TASK_DEAD) {
      GKI_exit_task(task_id);
    }
  }
}

/*******************************************************************************
**
** Function         GKI_wait
**
** Description      This function is called by tasks to wait for a specific
**                  event or set of events. The task may specify the duration
**                  that it wants to wait for, or 0 if infinite.
**
** Parameters:      flag -    (input) the event or set of events to wait for
**                  timeout - (input) the duration that the task wants to wait
**                                    for the specific events (in system ticks)
**
**
** Returns          the event mask of received events or zero if timeout
**
*******************************************************************************/
uint16_t GKI_wait(uint16_t flag, uint32_t timeout) {
  uint16_t evt;
  uint8_t rtask;
  struct timespec abstime = {0, 0};
  int sec;
  int nano_sec;

  rtask = GKI_get_taskid();
  if (rtask >= GKI_MAX_TASKS) {
    LOG(ERROR) << StringPrintf("%s() Exiting thread; rtask %d >= %d", __func__,
                               rtask, GKI_MAX_TASKS);
    return EVENT_MASK(GKI_SHUTDOWN_EVT);
  }

  gki_pthread_info_t* p_pthread_info = &gki_pthread_info[rtask];
  if (p_pthread_info->pCond != nullptr && p_pthread_info->pMutex != nullptr) {
    int ret;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("GKI_wait task=%i, pCond/pMutex = %p/%p", rtask,
                        p_pthread_info->pCond, p_pthread_info->pMutex);
    ret = pthread_mutex_lock(p_pthread_info->pMutex);
    ret = pthread_cond_signal(p_pthread_info->pCond);
    ret = pthread_mutex_unlock(p_pthread_info->pMutex);
    p_pthread_info->pMutex = nullptr;
    p_pthread_info->pCond = nullptr;
  }
  gki_cb.com.OSWaitForEvt[rtask] = flag;

  /* protect OSWaitEvt[rtask] from modification from an other thread */
  pthread_mutex_lock(&gki_cb.os.thread_evt_mutex[rtask]);

#if 0 /* for clean scheduling we probably should always call \
         pthread_cond_wait() */
    /* Check if anything in any of the mailboxes. There is a potential race condition where OSTaskQFirst[rtask]
     has been modified. however this should only result in addtional call to  pthread_cond_wait() but as
     the cond is met, it will exit immediately (depending on schedulling) */
    if (gki_cb.com.OSTaskQFirst[rtask][0])
    gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_0_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][1])
    gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_1_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][2])
    gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_2_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][3])
    gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_3_EVT_MASK;
#endif

  if (!(gki_cb.com.OSWaitEvt[rtask] & flag)) {
    if (timeout) {
      //            timeout = GKI_MS_TO_TICKS(timeout);     /* convert from
      //            milliseconds to ticks */

      /* get current system time */
      //            clock_gettime(CLOCK_MONOTONIC, &currSysTime);
      //            abstime.tv_sec = currSysTime.time;
      //            abstime.tv_nsec = NANOSEC_PER_MILLISEC *
      //            currSysTime.millitm;
      clock_gettime(CLOCK_MONOTONIC, &abstime);

      /* add timeout */
      sec = timeout / 1000;
      nano_sec = (timeout % 1000) * NANOSEC_PER_MILLISEC;
      abstime.tv_nsec += nano_sec;
      if (abstime.tv_nsec > NSEC_PER_SEC) {
        abstime.tv_sec += (abstime.tv_nsec / NSEC_PER_SEC);
        abstime.tv_nsec = abstime.tv_nsec % NSEC_PER_SEC;
      }
      abstime.tv_sec += sec;

      pthread_cond_timedwait(&gki_cb.os.thread_evt_cond[rtask],
                             &gki_cb.os.thread_evt_mutex[rtask], &abstime);

    } else {
      pthread_cond_wait(&gki_cb.os.thread_evt_cond[rtask],
                        &gki_cb.os.thread_evt_mutex[rtask]);
    }

    /* TODO: check, this is probably neither not needed depending on
     phtread_cond_wait() implmentation,
     e.g. it looks like it is implemented as a counter in which case multiple
     cond_signal
     should NOT be lost! */
    // we are waking up after waiting for some events, so refresh variables
    // no need to call GKI_disable() here as we know that we will have some
    // events as we've been waking up after condition pending or timeout
    if (gki_cb.com.OSTaskQFirst[rtask][0])
      gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_0_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][1])
      gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_1_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][2])
      gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_2_EVT_MASK;
    if (gki_cb.com.OSTaskQFirst[rtask][3])
      gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_3_EVT_MASK;

    if (gki_cb.com.OSRdyTbl[rtask] == TASK_DEAD) {
      gki_cb.com.OSWaitEvt[rtask] = 0;
      /* unlock thread_evt_mutex as pthread_cond_wait() does auto lock when cond
       * is met */
      pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[rtask]);
      LOG(ERROR) << StringPrintf("GKI TASK_DEAD received. exit thread %d...",
                                 rtask);

      gki_cb.os.thread_id[rtask] = 0;
      return (EVENT_MASK(GKI_SHUTDOWN_EVT));
    }
  }

  /* Clear the wait for event mask */
  gki_cb.com.OSWaitForEvt[rtask] = 0;

  /* Return only those bits which user wants... */
  evt = gki_cb.com.OSWaitEvt[rtask] & flag;

  /* Clear only those bits which user wants... */
  gki_cb.com.OSWaitEvt[rtask] &= ~flag;

  /* unlock thread_evt_mutex as pthread_cond_wait() does auto lock mutex when
   * cond is met */
  pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[rtask]);
  return (evt);
}

/*******************************************************************************
**
** Function         GKI_delay
**
** Description      This function is called by tasks to sleep unconditionally
**                  for a specified amount of time. The duration is in
**                  milliseconds
**
** Parameters:      timeout -    (input) the duration in milliseconds
**
** Returns          void
**
*******************************************************************************/

void GKI_delay(uint32_t timeout) {
  uint8_t rtask = GKI_get_taskid();
  struct timespec delay;
  int err;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_delay %d %d", rtask, timeout);

  delay.tv_sec = timeout / 1000;
  delay.tv_nsec = 1000 * 1000 * (timeout % 1000);

  /* [u]sleep can't be used because it uses SIGALRM */

  do {
    err = nanosleep(&delay, &delay);
  } while (err < 0 && errno == EINTR);

  /* Check if task was killed while sleeping */
  /* NOTE
  **      if you do not implement task killing, you do not
  **      need this check.
  */
  if (rtask && gki_cb.com.OSRdyTbl[rtask] == TASK_DEAD) {
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_delay %d %d done", rtask, timeout);
  return;
}

/*******************************************************************************
**
** Function         GKI_send_event
**
** Description      This function is called by tasks to send events to other
**                  tasks. Tasks can also send events to themselves.
**
** Parameters:      task_id -  (input) The id of the task to which the event has
**                                     to be sent
**                  event   -  (input) The event that has to be sent
**
**
** Returns          GKI_SUCCESS if all OK, else GKI_FAILURE
**
*******************************************************************************/
uint8_t GKI_send_event(uint8_t task_id, uint16_t event) {
  /* use efficient coding to avoid pipeline stalls */
  if (task_id < GKI_MAX_TASKS) {
    /* protect OSWaitEvt[task_id] from manipulation in GKI_wait() */
    pthread_mutex_lock(&gki_cb.os.thread_evt_mutex[task_id]);

    /* Set the event bit */
    gki_cb.com.OSWaitEvt[task_id] |= event;

    pthread_cond_signal(&gki_cb.os.thread_evt_cond[task_id]);

    pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[task_id]);

    return (GKI_SUCCESS);
  }
  return (GKI_FAILURE);
}

/*******************************************************************************
**
** Function         GKI_isend_event
**
** Description      This function is called from ISRs to send events to other
**                  tasks. The only difference between this function and
**                  GKI_send_event is that this function assumes interrupts are
**                  already disabled.
**
** Parameters:      task_id -  (input) The destination task Id for the event.
**                  event   -  (input) The event flag
**
** Returns          GKI_SUCCESS if all OK, else GKI_FAILURE
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to use it in your own implementation,
**                  put your code here, otherwise you can delete the entire
**                  body of the function.
**
*******************************************************************************/
uint8_t GKI_isend_event(uint8_t task_id, uint16_t event) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_isend_event %d %x", task_id, event);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_isend_event %d %x done", task_id, event);
  return GKI_send_event(task_id, event);
}

/*******************************************************************************
**
** Function         GKI_get_taskid
**
** Description      This function gets the currently running task ID.
**
** Returns          task ID
**
** NOTE             The Widcomm upper stack and profiles may run as a single
**                  task. If you only have one GKI task, then you can hard-code
**                  this function to return a '1'. Otherwise, you should have
**                  some OS-specific method to determine the current task.
**
*******************************************************************************/
uint8_t GKI_get_taskid(void) {
  int i;
  pthread_t thread_id = pthread_self();
  for (i = 0; i < GKI_MAX_TASKS; i++) {
    if (gki_cb.os.thread_id[i] == thread_id) {
      return (i);
    }
  }
  return (-1);
}

/*******************************************************************************
**
** Function         GKI_map_taskname
**
** Description      This function gets the task name of the taskid passed as
**                  arg. If GKI_MAX_TASKS is passed as arg the currently running
**                  task name is returned
**
** Parameters:      task_id -  (input) The id of the task whose name is being
**                  sought. GKI_MAX_TASKS is passed to get the name of the
**                  currently running task.
**
** Returns          pointer to task name
**
** NOTE             this function needs no customization
**
*******************************************************************************/
int8_t* GKI_map_taskname(uint8_t task_id) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_map_taskname %d", task_id);

  if (task_id < GKI_MAX_TASKS) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "GKI_map_taskname %d %s done", task_id, gki_cb.com.OSTName[task_id]);
    return (gki_cb.com.OSTName[task_id]);
  } else if (task_id == GKI_MAX_TASKS) {
    return (gki_cb.com.OSTName[GKI_get_taskid()]);
  } else {
    return (int8_t*)"BAD";
  }
}

/*******************************************************************************
**
** Function         GKI_enable
**
** Description      This function enables interrupts.
**
** Returns          void
**
*******************************************************************************/
void GKI_enable(void) {
  pthread_mutex_unlock(&gki_cb.os.GKI_mutex);
  /* 	pthread_mutex_xx is nesting save, no need for this: already_disabled =
   * 0; */
  return;
}

/*******************************************************************************
**
** Function         GKI_disable
**
** Description      This function disables interrupts.
**
** Returns          void
**
*******************************************************************************/

void GKI_disable(void) {
  // DLOG_IF(INFO, nfc_debug_enabled) <<
  // StringPrintf("GKI_disable");

  /*	pthread_mutex_xx is nesting save, no need for this: if
     (!already_disabled) {
      already_disabled = 1; */
  pthread_mutex_lock(&gki_cb.os.GKI_mutex);
  /*  } */
  // DLOG_IF(INFO, nfc_debug_enabled) <<
  // StringPrintf("Leaving GKI_disable");
  return;
}

/*******************************************************************************
**
** Function         GKI_exception
**
** Description      This function throws an exception.
**                  This is normally only called for a nonrecoverable error.
**
** Parameters:      code    -  (input) The code for the error
**                  msg     -  (input) The message that has to be logged
**
** Returns          void
**
*******************************************************************************/

void GKI_exception(uint16_t code, std::string msg) {
  uint8_t task_id;

  LOG(ERROR) << StringPrintf("Task State Table");

  for (task_id = 0; task_id < GKI_MAX_TASKS; task_id++) {
    LOG(ERROR) << StringPrintf("TASK ID [%d] task name [%s] state [%d]",
                               task_id, gki_cb.com.OSTName[task_id],
                               gki_cb.com.OSRdyTbl[task_id]);
  }

  LOG(ERROR) << StringPrintf("%d %s", code, msg.c_str());
  LOG(ERROR) << StringPrintf(
      "********************************************************************");
  LOG(ERROR) << StringPrintf("* %d %s", code, msg.c_str());
  LOG(ERROR) << StringPrintf(
      "********************************************************************");

  LOG(ERROR) << StringPrintf("%d %s done", code, msg.c_str());

  return;
}

/*******************************************************************************
**
** Function         GKI_get_time_stamp
**
** Description      This function formats the time into a user area
**
** Parameters:      tbuf -  (output) the address to the memory containing the
**                  formatted time
**
** Returns          the address of the user area containing the formatted time
**                  The format of the time is ????
**
** NOTE             This function is only called by OBEX.
**
*******************************************************************************/
int8_t* GKI_get_time_stamp(int8_t* tbuf) {
  uint32_t ms_time;
  uint32_t s_time;
  uint32_t m_time;
  uint32_t h_time;
  int8_t* p_out = tbuf;

  gki_cb.com.OSTicks = times(nullptr);
  ms_time = GKI_TICKS_TO_MS(gki_cb.com.OSTicks);
  s_time = ms_time / 100; /* 100 Ticks per second */
  m_time = s_time / 60;
  h_time = m_time / 60;

  ms_time -= s_time * 100;
  s_time -= m_time * 60;
  m_time -= h_time * 60;

  *p_out++ = (int8_t)((h_time / 10) + '0');
  *p_out++ = (int8_t)((h_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (int8_t)((m_time / 10) + '0');
  *p_out++ = (int8_t)((m_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (int8_t)((s_time / 10) + '0');
  *p_out++ = (int8_t)((s_time % 10) + '0');
  *p_out++ = ':';
  *p_out++ = (int8_t)((ms_time / 10) + '0');
  *p_out++ = (int8_t)((ms_time % 10) + '0');
  *p_out++ = ':';
  *p_out = 0;

  return (tbuf);
}

/*******************************************************************************
**
** Function         GKI_register_mempool
**
** Description      This function registers a specific memory pool.
**
** Parameters:      p_mem -  (input) pointer to the memory pool
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If your OS has different memory pools, you
**                  can tell GKI the pool to use by calling this function.
**
*******************************************************************************/
void GKI_register_mempool(void* p_mem) {
  gki_cb.com.p_user_mempool = p_mem;

  return;
}

/*******************************************************************************
**
** Function         GKI_os_malloc
**
** Description      This function allocates memory
**
** Parameters:      size -  (input) The size of the memory that has to be
**                  allocated
**
** Returns          the address of the memory allocated, or NULL if failed
**
** NOTE             This function is called by the Widcomm stack when
**                  dynamic memory allocation is used.
**
*******************************************************************************/
void* GKI_os_malloc(uint32_t size) { return (malloc(size)); }

/*******************************************************************************
**
** Function         GKI_os_free
**
** Description      This function frees memory
**
** Parameters:      size -  (input) The address of the memory that has to be
**                  freed
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. It is only called from within GKI if dynamic
**
*******************************************************************************/
void GKI_os_free(void* p_mem) {
  if (p_mem != nullptr) free(p_mem);
  return;
}

/*******************************************************************************
**
** Function         GKI_suspend_task()
**
** Description      This function suspends the task specified in the argument.
**
** Parameters:      task_id  - (input) the id of the task that has to suspended
**
** Returns          GKI_SUCCESS if all OK, else GKI_FAILURE
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to implement task suspension
**                  capability, put specific code here.
**
*******************************************************************************/
uint8_t GKI_suspend_task(uint8_t task_id) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_suspend_task %d - NOT implemented", task_id);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_suspend_task %d done", task_id);

  return (GKI_SUCCESS);
}

/*******************************************************************************
**
** Function         GKI_resume_task()
**
** Description      This function resumes the task specified in the argument.
**
** Parameters:      task_id  - (input) the id of the task that has to resumed
**
** Returns          GKI_SUCCESS if all OK
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to implement task suspension
**                  capability, put specific code here.
**
*******************************************************************************/
uint8_t GKI_resume_task(uint8_t task_id) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_resume_task %d - NOT implemented", task_id);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_resume_task %d done", task_id);

  return (GKI_SUCCESS);
}

/*******************************************************************************
**
** Function         GKI_exit_task
**
** Description      This function is called to stop a GKI task.
**
** Parameters:      task_id  - (input) the id of the task that has to be stopped
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to use it in your own implementation,
**                  put specific code here to kill a task.
**
*******************************************************************************/
void GKI_exit_task(uint8_t task_id) {
  if (task_id >= GKI_MAX_TASKS) {
    return;
  }
  GKI_disable();
  gki_cb.com.OSRdyTbl[task_id] = TASK_DEAD;

  /* Destroy mutex and condition variable objects */
  pthread_mutex_destroy(&gki_cb.os.thread_evt_mutex[task_id]);
  pthread_cond_destroy(&gki_cb.os.thread_evt_cond[task_id]);
  pthread_mutex_destroy(&gki_cb.os.thread_timeout_mutex[task_id]);
  pthread_cond_destroy(&gki_cb.os.thread_timeout_cond[task_id]);

  GKI_enable();

  // GKI_send_event(task_id, EVENT_MASK(GKI_SHUTDOWN_EVT));

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("GKI_exit_task %d done", task_id);
  return;
}

/*******************************************************************************
**
** Function         GKI_sched_lock
**
** Description      This function is called by tasks to disable scheduler
**                  task context switching.
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to use it in your own implementation,
**                  put code here to tell the OS to disable context switching.
**
*******************************************************************************/
void GKI_sched_lock(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("GKI_sched_lock");
  GKI_disable();
  return;
}

/*******************************************************************************
**
** Function         GKI_sched_unlock
**
** Description      This function is called by tasks to enable scheduler
**                  switching.
**
** Returns          void
**
** NOTE             This function is NOT called by the Widcomm stack and
**                  profiles. If you want to use it in your own implementation,
**                  put code here to tell the OS to re-enable context switching.
**
*******************************************************************************/
void GKI_sched_unlock(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("GKI_sched_unlock");
  GKI_enable();
}

/*******************************************************************************
**
** Function         GKI_shiftdown
**
** Description      shift memory down (to make space to insert a record)
**
*******************************************************************************/
void GKI_shiftdown(uint8_t* p_mem, uint32_t len, uint32_t shift_amount) {
  uint8_t* ps = p_mem + len - 1;
  uint8_t* pd = ps + shift_amount;
  uint32_t xx;

  for (xx = 0; xx < len; xx++) *pd-- = *ps--;
}

/*******************************************************************************
**
** Function         GKI_shiftup
**
** Description      shift memory up (to delete a record)
**
*******************************************************************************/
void GKI_shiftup(uint8_t* p_dest, uint8_t* p_src, uint32_t len) {
  uint8_t* ps = p_src;
  uint8_t* pd = p_dest;
  uint32_t xx;

  for (xx = 0; xx < len; xx++) *pd++ = *ps++;
}
