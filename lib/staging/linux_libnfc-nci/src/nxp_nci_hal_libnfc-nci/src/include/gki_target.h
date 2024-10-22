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
#ifndef GKI_TARGET_H
#define GKI_TARGET_H

#ifdef BUILDCFG
#include "buildcfg.h"
#endif

#include "data_types.h"
/* Operating System Selection */
#ifndef BTE_SIM_APP
#define _GKI_ARM
#define _GKI_STANDALONE
#else
#define _BT_WIN32
#endif

/* define prefix for exporting APIs from libraries */
#define EXPORT_API

#ifndef BTE_BSE_WRAPPER
#ifdef  BTE_SIM_APP
#undef  EXPORT_API
#define EXPORT_API  __declspec(dllexport)
#endif
#endif

#define GKI_API EXPORT_API
#define UDRV_API EXPORT_API


/******************************************************************************
**
** Task configuration
**
******************************************************************************/

/* Definitions of task IDs for inter-task messaging */
#ifndef MMI_TASK
#define MMI_TASK 0
#endif

#ifndef HCISU_TASK
#define HCISU_TASK 1
#endif

#ifndef NCIT_TASK
#define NCIT_TASK 2
#endif

#ifndef NFC_TASK
#define NFC_TASK 3
#endif

#ifndef BTU_TASK
#define BTU_TASK 4
#endif

/* BTE Application, Sample Apps, or Serial port Demo based on JP3 and JP4
 * setting) */
#ifndef BTE_APPL_TASK
#define BTE_APPL_TASK 5
#endif

#ifndef DEV_MGR_TASK
#define DEV_MGR_TASK 6
#endif

#ifndef ISE_SCR_TASK
#define ISE_SCR_TASK 7
#endif

#ifndef UCODEC_TASK
#define UCODEC_TASK 8
#endif

#ifndef RPCT_TASK
#define RPCT_TASK 9
#endif

#ifndef UNV_TASK
#define UNV_TASK 10
#endif

#ifndef BTE_IDLE_TASK
#define BTE_IDLE_TASK 11
#endif

#ifndef UIPC_TASK
#define UIPC_TASK 12
#endif

#ifndef HCISU_AMP_TASK
#define HCISU_AMP_TASK 13
#endif

/* The number of GKI tasks in the software system. */
#ifndef GKI_MAX_TASKS
#define GKI_MAX_TASKS 14
#endif

/******************************************************************************
**
** Timer configuration
**
******************************************************************************/

/* The number of GKI timers in the software system. */
#ifndef GKI_NUM_TIMERS
#define GKI_NUM_TIMERS 3
#endif

/* A conversion value for translating ticks to calculate GKI timer.  */
#ifndef TICKS_PER_SEC
#define TICKS_PER_SEC 100
#endif

/* delay in ticks before stopping system tick. */
#ifndef GKI_DELAY_STOP_SYS_TICK
#define GKI_DELAY_STOP_SYS_TICK 10
#endif

/******************************************************************************
**
** Buffer configuration
**
******************************************************************************/

/* The size of the buffers in pool 0. */
#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE 64
#endif

/* The number of buffers in buffer pool 0. */
#ifndef GKI_BUF0_MAX
#define GKI_BUF0_MAX 48
#endif

/* The ID of buffer pool 0. */
#ifndef GKI_POOL_ID_0
#define GKI_POOL_ID_0 0
#endif

/* The size of the buffers in pool 1. */
#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE 128
#endif

/* The number of buffers in buffer pool 1. */
#ifndef GKI_BUF1_MAX
#define GKI_BUF1_MAX 26
#endif

/* The ID of buffer pool 1. */
#ifndef GKI_POOL_ID_1
#define GKI_POOL_ID_1 1
#endif

/* The size of the buffers in pool 2. */
#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE 660
#endif

/* The number of buffers in buffer pool 2. */
#ifndef GKI_BUF2_MAX
#define GKI_BUF2_MAX 45
#endif

/* The ID of buffer pool 2. */
#ifndef GKI_POOL_ID_2
#define GKI_POOL_ID_2 2
#endif

/* The size of the buffers in pool 3. */
#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE (0xFFB0)
#endif

/* The number of buffers in buffer pool 3. */
#ifndef GKI_BUF3_MAX
#define GKI_BUF3_MAX 30
#endif

/* The ID of buffer pool 3. */
#ifndef GKI_POOL_ID_3
#define GKI_POOL_ID_3 3
#endif

/* The size of the largest PUBLIC fixed buffer in system. */
#ifndef GKI_MAX_BUF_SIZE
#define GKI_MAX_BUF_SIZE GKI_BUF3_SIZE
#endif

/* The pool ID of the largest PUBLIC fixed buffer in system. */
#ifndef GKI_MAX_BUF_SIZE_POOL_ID
#define GKI_MAX_BUF_SIZE_POOL_ID GKI_POOL_ID_3
#endif

/* RESERVED buffer pool for OBX */
/* Ideally there should be 1 buffer for each instance for RX data, and some
number
of TX buffers based on active instances. OBX will only use these if packet size
requires it. In most cases the large packets are used in only one direction so
the other direction will use smaller buffers.
Devices with small amount of RAM should limit the number of active obex objects.
*/
/* The size of the buffers in pool 4. */
#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE 0x2000
#endif

/* The number of buffers in buffer pool 4. */
#ifndef GKI_BUF4_MAX
/* (OBX_NUM_SERVERS + OBX_NUM_CLIENTS) */
#define GKI_BUF4_MAX 96
#endif

/* The ID of buffer pool 4. */
#ifndef GKI_POOL_ID_4
#define GKI_POOL_ID_4 4
#endif

/* The number of fixed GKI buffer pools.
If L2CAP_FCR_INCLUDED is FALSE, Pool ID 5 is unnecessary
If BTM_SCO_HCI_INCLUDED is FALSE, Pool ID 6 is unnecessary, otherwise set to 7
If BTA_HL_INCLUDED is FALSE then Pool ID 7 is uncessary and set the following to
7, otherwise set to 8
If GATT_SERVER_ENABLED is FALSE then Pool ID 8 is uncessary and set the
following to 8, otherwise set to 9
*/
#ifndef GKI_NUM_FIXED_BUF_POOLS
#define GKI_NUM_FIXED_BUF_POOLS 9
#endif

/* The buffer pool usage mask. */
#ifndef GKI_DEF_BUFPOOL_PERM_MASK
#define GKI_DEF_BUFPOOL_PERM_MASK 0xfff0
#endif

/* The number of fixed and dynamic buffer pools.
If L2CAP_FCR_INCLUDED is FALSE, Pool ID 4 is unnecessary */
#ifndef GKI_NUM_TOTAL_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS 10
#endif

/* The following is intended to be a reserved pool for L2CAP
Flow control and retransmissions and intentionally kept out
of order */

/* The number of buffers in buffer pool 5. */
#ifndef GKI_BUF5_MAX
#define GKI_BUF5_MAX 64
#endif

/* The ID of buffer pool 5. */
#ifndef GKI_POOL_ID_5
#define GKI_POOL_ID_5 5
#endif

/* The size of the buffers in pool 5
** Special pool used by l2cap retransmissions only. This size based on segment
** that will fit into both DH5 and 2-DH3 packet types after accounting for GKI
** header. 13 bytes of max headers allows us a 339 payload max.
** (in btui_app.txt)
** Note: 748 used for insight scriptwrapper with CAT-2 scripts.
*/
#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE 748
#endif

/* The buffer corruption check flag. */
#ifndef GKI_ENABLE_BUF_CORRUPTION_CHECK
#define GKI_ENABLE_BUF_CORRUPTION_CHECK TRUE
#endif

/* The GKI severe error macro. */
#ifndef GKI_SEVERE
#define GKI_SEVERE(code)
#endif

/* Maximum number of exceptions logged. */
#ifndef GKI_MAX_EXCEPTION
#define GKI_MAX_EXCEPTION 8
#endif

/* Maximum number of chars stored for each exception message. */
#ifndef GKI_MAX_EXCEPTION_MSGLEN
#define GKI_MAX_EXCEPTION_MSGLEN 64
#endif

#ifndef GKI_SEND_MSG_FROM_ISR
#define GKI_SEND_MSG_FROM_ISR FALSE
#endif

/* The following is intended to be a reserved pool for SCO
over HCI data and intentionally kept out of order */

/* The ID of buffer pool 6. */
#ifndef GKI_POOL_ID_6
#define GKI_POOL_ID_6 6
#endif

/* The size of the buffers in pool 6,
  BUF_SIZE = max SCO data 255 + sizeof(NFC_HDR) = 8 + SCO packet header 3 +
  padding 2 = 268 */
#ifndef GKI_BUF6_SIZE
#define GKI_BUF6_SIZE 268
#endif

/* The number of buffers in buffer pool 6. */
#ifndef GKI_BUF6_MAX
#define GKI_BUF6_MAX 60
#endif

/* The following pool is a dedicated pool for HDP
   If a shared pool is more desirable then
   1. set BTA_HL_LRG_DATA_POOL_ID to the desired Gki Pool ID
   2. make sure that the shared pool size is larger than 9472
   3. adjust GKI_NUM_FIXED_BUF_POOLS accordingly since
      POOL ID 7 is not needed
*/

/* The ID of buffer pool 7. */
#ifndef GKI_POOL_ID_7
#define GKI_POOL_ID_7 7
#endif

/* The size of the buffers in pool 7 */
#ifndef GKI_BUF7_SIZE
#define GKI_BUF7_SIZE 9472
#endif

/* The number of buffers in buffer pool 7. */
#ifndef GKI_BUF7_MAX
#define GKI_BUF7_MAX 2
#endif

/* The following pool is a dedicated pool for GATT
   If a shared pool is more desirable then
   1. set GATT_DB_POOL_ID to the desired Gki Pool ID
   2. make sure that the shared pool size fit a common GATT database needs
   3. adjust GKI_NUM_FIXED_BUF_POOLS accordingly since
      POOL ID 8 is not needed
*/

/* The ID of buffer pool 8. */
#ifndef GKI_POOL_ID_8
#define GKI_POOL_ID_8 8
#endif

/* The size of the buffers in pool 8 */
#ifndef GKI_BUF8_SIZE
#define GKI_BUF8_SIZE 128
#endif

/* The number of buffers in buffer pool 8. */
#ifndef GKI_BUF8_MAX
#define GKI_BUF8_MAX 30
#endif
#if defined(GKI_DEBUG) && (GKI_DEBUG == TRUE)
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "GKI_LINUX"
/* GKI Trace Macros */
#define GKI_TRACE_0(m)                          LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m)
#define GKI_TRACE_1(m,p1)                       LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1)
#define GKI_TRACE_2(m,p1,p2)                    LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2)
#define GKI_TRACE_3(m,p1,p2,p3)                 LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3)
#define GKI_TRACE_4(m,p1,p2,p3,p4)              LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4)
#define GKI_TRACE_5(m,p1,p2,p3,p4,p5)           LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4,p5)
#define GKI_TRACE_6(m,p1,p2,p3,p4,p5,p6)        LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4,p5,p6)
#else
#define GKI_TRACE_0(m)
#define GKI_TRACE_1(m,p1)
#define GKI_TRACE_2(m,p1,p2)
#define GKI_TRACE_3(m,p1,p2,p3)
#define GKI_TRACE_4(m,p1,p2,p3,p4)
#define GKI_TRACE_5(m,p1,p2,p3,p4,p5)
#define GKI_TRACE_6(m,p1,p2,p3,p4,p5,p6)

#endif

#define GKI_TRACE_ERROR_0(m)                    LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m)
#define GKI_TRACE_ERROR_1(m,p1)                 LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1)
#define GKI_TRACE_ERROR_2(m,p1,p2)              LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2)
#define GKI_TRACE_ERROR_3(m,p1,p2,p3)           LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3)
#define GKI_TRACE_ERROR_4(m,p1,p2,p3,p4)        LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4)
#define GKI_TRACE_ERROR_5(m,p1,p2,p3,p4,p5)     LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5)
#define GKI_TRACE_ERROR_6(m,p1,p2,p3,p4,p5,p6)  LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6)

#ifdef __cplusplus
extern "C"
{
#endif

EXPORT_API extern void LogMsg (UINT32 trace_set_mask, const char *fmt_str, ...);

#ifdef __cplusplus
}
#endif


#endif /* GKI_TARGET_H */
