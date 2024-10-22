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
#ifndef GKI_H
#define GKI_H

#ifdef BUILDCFG
#include "buildcfg.h"
#endif

#ifndef NFC_STANDALONE
#define NFC_STANDALONE FALSE
#endif

#include <string>

#include "bt_types.h"
#include "gki_target.h"

/* Error codes */
#define GKI_SUCCESS 0x00
#define GKI_FAILURE 0x01
#define GKI_INVALID_TASK 0xF0
#define GKI_INVALID_POOL 0xFF

/************************************************************************
** Mailbox definitions. Each task has 4 mailboxes that are used to
** send buffers to the task.
*/
#define TASK_MBOX_0 0
#define TASK_MBOX_2 2

#define NUM_TASK_MBOX 4

/************************************************************************
** Event definitions.
**
** There are 4 reserved events used to signal messages rcvd in task mailboxes.
** There are 4 reserved events used to signal timeout events.
** There are 8 general purpose events available for applications.
*/

#define TASK_MBOX_0_EVT_MASK 0x0001
#define TASK_MBOX_1_EVT_MASK 0x0002
#define TASK_MBOX_2_EVT_MASK 0x0004
#define TASK_MBOX_3_EVT_MASK 0x0008

#define TIMER_0 0
#define TIMER_1 1
#define TIMER_2 2
#define TIMER_3 3

#define TIMER_0_EVT_MASK 0x0010
#define TIMER_1_EVT_MASK 0x0020
#define TIMER_2_EVT_MASK 0x0040
#define TIMER_3_EVT_MASK 0x0080

#define APPL_EVT_0 8
#define APPL_EVT_7 15

#define EVENT_MASK(evt) ((uint16_t)(0x0001 << (evt)))

/************************************************************************
**  Max Time Queue
**/
#ifndef GKI_MAX_TIMER_QUEUES
#define GKI_MAX_TIMER_QUEUES 3
#endif

/************************************************************************
**  Utility macros for timer conversion
**/
#ifdef TICKS_PER_SEC
#define GKI_MS_TO_TICKS(x) ((x) / (1000 / TICKS_PER_SEC))
#define GKI_SECS_TO_TICKS(x) ((x) * (TICKS_PER_SEC))
#define GKI_TICKS_TO_MS(x) ((x) * (1000 / TICKS_PER_SEC))
#define GKI_TICKS_TO_SECS(x) ((x) * (1 / TICKS_PER_SEC))
#endif

/************************************************************************
**  Macro to determine the pool buffer size based on the GKI POOL ID at compile
**  time. Pool IDs index from 0 to GKI_NUM_FIXED_BUF_POOLS - 1
*/

#if (GKI_NUM_FIXED_BUF_POOLS < 1)

#ifndef GKI_POOL_ID_0
#define GKI_POOL_ID_0 0
#endif /* ifndef GKI_POOL_ID_0 */

#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE 0
#endif /* ifndef GKI_BUF0_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 1 */

#if (GKI_NUM_FIXED_BUF_POOLS < 2)

#ifndef GKI_POOL_ID_1
#define GKI_POOL_ID_1 0
#endif /* ifndef GKI_POOL_ID_1 */

#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE 0
#endif /* ifndef GKI_BUF1_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 2 */

#if (GKI_NUM_FIXED_BUF_POOLS < 3)

#ifndef GKI_POOL_ID_2
#define GKI_POOL_ID_2 0
#endif /* ifndef GKI_POOL_ID_2 */

#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE 0
#endif /* ifndef GKI_BUF2_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 3 */

#if (GKI_NUM_FIXED_BUF_POOLS < 4)

#ifndef GKI_POOL_ID_3
#define GKI_POOL_ID_3 0
#endif /* ifndef GKI_POOL_ID_4 */

#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE 0
#endif /* ifndef GKI_BUF3_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 4 */

#if (GKI_NUM_FIXED_BUF_POOLS < 5)

#ifndef GKI_POOL_ID_4
#define GKI_POOL_ID_4 0
#endif /* ifndef GKI_POOL_ID_4 */

#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE 0
#endif /* ifndef GKI_BUF4_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 5 */

#if (GKI_NUM_FIXED_BUF_POOLS < 6)

#ifndef GKI_POOL_ID_5
#define GKI_POOL_ID_5 0
#endif /* ifndef GKI_POOL_ID_5 */

#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE 0
#endif /* ifndef GKI_BUF5_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 6 */

#if (GKI_NUM_FIXED_BUF_POOLS < 7)

#ifndef GKI_POOL_ID_6
#define GKI_POOL_ID_6 0
#endif /* ifndef GKI_POOL_ID_6 */

#ifndef GKI_BUF6_SIZE
#define GKI_BUF6_SIZE 0
#endif /* ifndef GKI_BUF6_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 7 */

#if (GKI_NUM_FIXED_BUF_POOLS < 8)

#ifndef GKI_POOL_ID_7
#define GKI_POOL_ID_7 0
#endif /* ifndef GKI_POOL_ID_7 */

#ifndef GKI_BUF7_SIZE
#define GKI_BUF7_SIZE 0
#endif /* ifndef GKI_BUF7_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 8 */

#if (GKI_NUM_FIXED_BUF_POOLS < 9)

#ifndef GKI_POOL_ID_8
#define GKI_POOL_ID_8 0
#endif /* ifndef GKI_POOL_ID_8 */

#ifndef GKI_BUF8_SIZE
#define GKI_BUF8_SIZE 0
#endif /* ifndef GKI_BUF8_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 9 */

#if (GKI_NUM_FIXED_BUF_POOLS < 10)

#ifndef GKI_POOL_ID_9
#define GKI_POOL_ID_9 0
#endif /* ifndef GKI_POOL_ID_9 */

#ifndef GKI_BUF9_SIZE
#define GKI_BUF9_SIZE 0
#endif /* ifndef GKI_BUF9_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 10 */

#if (GKI_NUM_FIXED_BUF_POOLS < 11)

#ifndef GKI_POOL_ID_10
#define GKI_POOL_ID_10 0
#endif /* ifndef GKI_POOL_ID_10 */

#ifndef GKI_BUF10_SIZE
#define GKI_BUF10_SIZE 0
#endif /* ifndef GKI_BUF10_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 11 */

#if (GKI_NUM_FIXED_BUF_POOLS < 12)

#ifndef GKI_POOL_ID_11
#define GKI_POOL_ID_11 0
#endif /* ifndef GKI_POOL_ID_11 */

#ifndef GKI_BUF11_SIZE
#define GKI_BUF11_SIZE 0
#endif /* ifndef GKI_BUF11_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 12 */

#if (GKI_NUM_FIXED_BUF_POOLS < 13)

#ifndef GKI_POOL_ID_12
#define GKI_POOL_ID_12 0
#endif /* ifndef GKI_POOL_ID_12 */

#ifndef GKI_BUF12_SIZE
#define GKI_BUF12_SIZE 0
#endif /* ifndef GKI_BUF12_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 13 */

#if (GKI_NUM_FIXED_BUF_POOLS < 14)

#ifndef GKI_POOL_ID_13
#define GKI_POOL_ID_13 0
#endif /* ifndef GKI_POOL_ID_13 */

#ifndef GKI_BUF13_SIZE
#define GKI_BUF13_SIZE 0
#endif /* ifndef GKI_BUF13_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 14 */

#if (GKI_NUM_FIXED_BUF_POOLS < 15)

#ifndef GKI_POOL_ID_14
#define GKI_POOL_ID_14 0
#endif /* ifndef GKI_POOL_ID_14 */

#ifndef GKI_BUF14_SIZE
#define GKI_BUF14_SIZE 0
#endif /* ifndef GKI_BUF14_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 15 */

#if (GKI_NUM_FIXED_BUF_POOLS < 16)

#ifndef GKI_POOL_ID_15
#define GKI_POOL_ID_15 0
#endif /* ifndef GKI_POOL_ID_15 */

#ifndef GKI_BUF15_SIZE
#define GKI_BUF15_SIZE 0
#endif /* ifndef GKI_BUF15_SIZE */

#endif /* GKI_NUM_FIXED_BUF_POOLS < 16 */

#ifndef GKI_SHUTDOWN_EVT
#define GKI_SHUTDOWN_EVT APPL_EVT_7
#endif

/* Timer list entry callback type
*/
struct TIMER_LIST_ENT;
typedef void(TIMER_CBACK)(TIMER_LIST_ENT* p_tle);

/* Define a timer list entry
*/
struct TIMER_LIST_ENT {
  TIMER_LIST_ENT* p_next;
  TIMER_LIST_ENT* p_prev;
  TIMER_CBACK* p_cback;
  int32_t ticks;
  uintptr_t param;
  uint16_t event;
  uint8_t in_use;
};

/* Define a timer list queue
*/
typedef struct {
  TIMER_LIST_ENT* p_first;
  TIMER_LIST_ENT* p_last;
  int32_t last_ticks;
} TIMER_LIST_Q;

/***********************************************************************
** This queue is a general purpose buffer queue, for application use.
*/
typedef struct {
  void* p_first;
  void* p_last;
  uint16_t count;
} BUFFER_Q;

/* Task constants
*/
#ifndef TASKPTR
typedef uint32_t (*TASKPTR)(uint32_t);
#endif

/* General pool accessible to GKI_getbuf() */
#define GKI_RESTRICTED_POOL 1 /* Inaccessible pool to GKI_getbuf() */

/***********************************************************************
** Function prototypes
*/

/* Task management
*/
extern uint8_t GKI_create_task(TASKPTR, uint8_t, int8_t*, uint16_t*, uint16_t,
                               void*, void*);
extern void GKI_exit_task(uint8_t);
extern uint8_t GKI_get_taskid(void);
extern void GKI_init(void);
extern int8_t* GKI_map_taskname(uint8_t);
extern uint8_t GKI_resume_task(uint8_t);
extern void GKI_run(void*);
extern void GKI_stop(void);
extern uint8_t GKI_suspend_task(uint8_t);

/* memory management
*/
extern void GKI_shiftdown(uint8_t* p_mem, uint32_t len, uint32_t shift_amount);
extern void GKI_shiftup(uint8_t* p_dest, uint8_t* p_src, uint32_t len);

/* To send buffers and events between tasks
*/
extern uint8_t GKI_isend_event(uint8_t, uint16_t);
extern void GKI_isend_msg(uint8_t, uint8_t, void*);
extern void* GKI_read_mbox(uint8_t);
extern void GKI_send_msg(uint8_t, uint8_t, void*);
extern uint8_t GKI_send_event(uint8_t, uint16_t);

/* To get and release buffers, change owner and get size
*/
extern void GKI_change_buf_owner(void*, uint8_t);
extern uint8_t GKI_create_pool(uint16_t, uint16_t, uint8_t, void*);
extern void GKI_delete_pool(uint8_t);
extern void* GKI_find_buf_start(void*);
extern void GKI_freebuf(void*);
extern void* GKI_getbuf(uint16_t);
extern uint16_t GKI_get_buf_size(void*);
extern void* GKI_getpoolbuf(uint8_t);

extern uint16_t GKI_poolcount(uint8_t);
extern uint16_t GKI_poolfreecount(uint8_t);
extern uint16_t GKI_poolutilization(uint8_t);
extern void GKI_register_mempool(void* p_mem);
extern uint8_t GKI_set_pool_permission(uint8_t, uint8_t);

/* User buffer queue management
*/
extern void* GKI_dequeue(BUFFER_Q*);
extern void GKI_enqueue(BUFFER_Q*, void*);
extern void GKI_enqueue_head(BUFFER_Q*, void*);
extern void* GKI_getfirst(BUFFER_Q*);
extern void* GKI_getlast(BUFFER_Q*);
extern void* GKI_getnext(void*);
extern void GKI_init_q(BUFFER_Q*);
extern bool GKI_queue_is_empty(BUFFER_Q*);
extern void* GKI_remove_from_queue(BUFFER_Q*, void*);
extern uint16_t GKI_get_pool_bufsize(uint8_t);

/* Timer management
*/
extern void GKI_add_to_timer_list(TIMER_LIST_Q*, TIMER_LIST_ENT*);
extern void GKI_delay(uint32_t);
extern uint32_t GKI_get_tick_count(void);
extern int8_t* GKI_get_time_stamp(int8_t*);
extern void GKI_init_timer_list(TIMER_LIST_Q*);
extern void GKI_init_timer_list_entry(TIMER_LIST_ENT*);
extern int32_t GKI_ready_to_sleep(void);
extern void GKI_remove_from_timer_list(TIMER_LIST_Q*, TIMER_LIST_ENT*);
extern void GKI_start_timer(uint8_t, int32_t, bool);
extern void GKI_stop_timer(uint8_t);
extern void GKI_timer_update(int32_t);
extern uint16_t GKI_update_timer_list(TIMER_LIST_Q*, int32_t);
extern uint32_t GKI_get_remaining_ticks(TIMER_LIST_Q*, TIMER_LIST_ENT*);
extern uint16_t GKI_wait(uint16_t, uint32_t);

/* Start and Stop system time tick callback
 * true for start system tick if time queue is not empty
 * false to stop system tick if time queue is empty
*/
typedef void(SYSTEM_TICK_CBACK)(bool);

/* Time queue management for system ticks
*/
extern bool GKI_timer_queue_empty(void);
extern void GKI_timer_queue_register_callback(SYSTEM_TICK_CBACK*);

/* Disable Interrupts, Enable Interrupts
*/
extern void GKI_enable(void);
extern void GKI_disable(void);
extern void GKI_sched_lock(void);
extern void GKI_sched_unlock(void);

/* Allocate (Free) memory from an OS
*/
extern void* GKI_os_malloc(uint32_t);
extern void GKI_os_free(void*);

/* os timer operation */
extern uint32_t GKI_get_os_tick_count(void);

/* Exception handling
*/
extern void GKI_exception(uint16_t, std::string);

#endif
