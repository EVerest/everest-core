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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include "gki_int.h"
#include "bt_trace.h"
#if (GKI_NUM_TOTAL_BUF_POOLS > 16)
#error Number of pools out of range (16 Max)!
#endif

#if (BTU_STACK_LITE_ENABLED == FALSE)
static void gki_add_to_pool_list(uint8_t pool_id);
static void gki_remove_from_pool_list(uint8_t pool_id);
#endif /*  BTU_STACK_LITE_ENABLED == FALSE */

//using android::base::StringPrintf;

/*******************************************************************************
**
** Function         gki_init_free_queue
**
** Description      Internal function called at startup to initialize a free
**                  queue. It is called once for each free queue.
**
** Returns          void
**
*******************************************************************************/
static void gki_init_free_queue(uint8_t id, uint16_t size, uint16_t total,
                                void* p_mem) {
  uint16_t i;
  uint16_t act_size;
  BUFFER_HDR_T* hdr;
  BUFFER_HDR_T* hdr1 = nullptr;
  uint32_t* magic;
  int32_t tempsize = size;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* Ensure an even number of longwords */
  tempsize = (int32_t)ALIGN_POOL(size);
  act_size = (uint16_t)(tempsize + BUFFER_PADDING_SIZE);

  /* Remember pool start and end addresses */
  if (p_mem) {
    p_cb->pool_start[id] = (uint8_t*)p_mem;
    p_cb->pool_end[id] = (uint8_t*)p_mem + (act_size * total);
  }

  p_cb->pool_size[id] = act_size;

  p_cb->freeq[id].size = (uint16_t)tempsize;
  p_cb->freeq[id].total = total;
  p_cb->freeq[id].cur_cnt = 0;
  p_cb->freeq[id].max_cnt = 0;

  /* Initialize  index table */
  if (p_mem) {
    hdr = (BUFFER_HDR_T*)p_mem;
    p_cb->freeq[id].p_first = hdr;
    for (i = 0; i < total; i++) {
      hdr->task_id = GKI_INVALID_TASK;
      hdr->q_id = id;
      hdr->status = BUF_STATUS_FREE;
      magic = (uint32_t*)((uint8_t*)hdr + BUFFER_HDR_SIZE + tempsize);
      *magic = MAGIC_NO;
      hdr1 = hdr;
      hdr = (BUFFER_HDR_T*)((uint8_t*)hdr + act_size);
      hdr1->p_next = hdr;
    }
    if (hdr1 != nullptr) hdr = hdr1;
    hdr->p_next = nullptr;
    p_cb->freeq[id].p_last = hdr;
  }
  return;
}

static bool gki_alloc_free_queue(uint8_t id) {
  FREE_QUEUE_T* Q;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  Q = &p_cb->freeq[p_cb->pool_list[id]];

  if (Q->p_first == nullptr) {
    void* p_mem = GKI_os_malloc((Q->size + BUFFER_PADDING_SIZE) * Q->total);
    if (p_mem) {
      // re-initialize the queue with allocated memory
      gki_init_free_queue(id, Q->size, Q->total, p_mem);
      return true;
    }
    GKI_exception(GKI_ERROR_BUF_SIZE_TOOBIG,
                  "gki_alloc_free_queue: Not enough memory");
  }
  return false;
}

/*******************************************************************************
**
** Function         gki_buffer_init
**
** Description      Called once internally by GKI at startup to initialize all
**                  buffers and free buffer pools.
**
** Returns          void
**
*******************************************************************************/
void gki_buffer_init(void) {
  uint8_t i, tt, mb;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* Initialize mailboxes */
  for (tt = 0; tt < GKI_MAX_TASKS; tt++) {
    for (mb = 0; mb < NUM_TASK_MBOX; mb++) {
      p_cb->OSTaskQFirst[tt][mb] = nullptr;
      p_cb->OSTaskQLast[tt][mb] = nullptr;
    }
  }

  for (tt = 0; tt < GKI_NUM_TOTAL_BUF_POOLS; tt++) {
    p_cb->pool_start[tt] = nullptr;
    p_cb->pool_end[tt] = nullptr;
    p_cb->pool_size[tt] = 0;

    p_cb->freeq[tt].p_first = nullptr;
    p_cb->freeq[tt].p_last = nullptr;
    p_cb->freeq[tt].size = 0;
    p_cb->freeq[tt].total = 0;
    p_cb->freeq[tt].cur_cnt = 0;
    p_cb->freeq[tt].max_cnt = 0;
  }

  /* Use default from target.h */
  p_cb->pool_access_mask = GKI_DEF_BUFPOOL_PERM_MASK;

#if (GKI_NUM_FIXED_BUF_POOLS > 0)
  gki_init_free_queue(0, GKI_BUF0_SIZE, GKI_BUF0_MAX, p_cb->bufpool0);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 1)
  gki_init_free_queue(1, GKI_BUF1_SIZE, GKI_BUF1_MAX, p_cb->bufpool1);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 2)
  gki_init_free_queue(2, GKI_BUF2_SIZE, GKI_BUF2_MAX, p_cb->bufpool2);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 3)
  gki_init_free_queue(3, GKI_BUF3_SIZE, GKI_BUF3_MAX, p_cb->bufpool3);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 4)
  gki_init_free_queue(4, GKI_BUF4_SIZE, GKI_BUF4_MAX, p_cb->bufpool4);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 5)
  gki_init_free_queue(5, GKI_BUF5_SIZE, GKI_BUF5_MAX, p_cb->bufpool5);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 6)
  gki_init_free_queue(6, GKI_BUF6_SIZE, GKI_BUF6_MAX, p_cb->bufpool6);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 7)
  gki_init_free_queue(7, GKI_BUF7_SIZE, GKI_BUF7_MAX, p_cb->bufpool7);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 8)
  gki_init_free_queue(8, GKI_BUF8_SIZE, GKI_BUF8_MAX, p_cb->bufpool8);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 9)
  gki_init_free_queue(9, GKI_BUF9_SIZE, GKI_BUF9_MAX, p_cb->bufpool9);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 10)
  gki_init_free_queue(10, GKI_BUF10_SIZE, GKI_BUF10_MAX, p_cb->bufpool10);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 11)
  gki_init_free_queue(11, GKI_BUF11_SIZE, GKI_BUF11_MAX, p_cb->bufpool11);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 12)
  gki_init_free_queue(12, GKI_BUF12_SIZE, GKI_BUF12_MAX, p_cb->bufpool12);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 13)
  gki_init_free_queue(13, GKI_BUF13_SIZE, GKI_BUF13_MAX, p_cb->bufpool13);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 14)
  gki_init_free_queue(14, GKI_BUF14_SIZE, GKI_BUF14_MAX, p_cb->bufpool14);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 15)
  gki_init_free_queue(15, GKI_BUF15_SIZE, GKI_BUF15_MAX, p_cb->bufpool15);
#endif

  /* add pools to the pool_list which is arranged in the order of size */
  for (i = 0; i < GKI_NUM_FIXED_BUF_POOLS; i++) {
    p_cb->pool_list[i] = i;
  }

  p_cb->curr_total_no_of_pools = GKI_NUM_FIXED_BUF_POOLS;

  return;
}

/*******************************************************************************
**
** Function         GKI_init_q
**
** Description      Called by an application to initialize a buffer queue.
**
** Returns          void
**
*******************************************************************************/
void GKI_init_q(BUFFER_Q* p_q) {
  p_q->p_first = p_q->p_last = nullptr;
  p_q->count = 0;

  return;
}

/*******************************************************************************
**
** Function         GKI_getbuf
**
** Description      Called by an application to get a free buffer which
**                  is of size greater or equal to the requested size.
**
**                  Note: This routine only takes buffers from public pools.
**                        It will not use any buffers from pools
**                        marked GKI_RESTRICTED_POOL.
**
** Parameters       size - (input) number of bytes needed.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void* GKI_getbuf(uint16_t size) {
  BUFFER_HDR_T* p_hdr;

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  if (size == 0) {
    LOG(ERROR) << StringPrintf("getbuf: Size is zero");
    abort();
  }

  size_t total_sz = size + sizeof(BUFFER_HDR_T);
  p_hdr = (BUFFER_HDR_T*)GKI_os_malloc(total_sz);
  if (!p_hdr) {
    LOG(ERROR) << StringPrintf("unable to allocate buffer!!!!!");
    abort();
  }

  memset(p_hdr, 0, total_sz);

  p_hdr->task_id = GKI_get_taskid();
  p_hdr->status = BUF_STATUS_UNLINKED;
  p_hdr->p_next = nullptr;
  p_hdr->Type = 0;

  p_hdr->q_id = 0;
  p_hdr->size = size;

  UNUSED(gki_alloc_free_queue);

  return (void*)((uint8_t*)p_hdr + BUFFER_HDR_SIZE);
#else
  uint8_t i;
  FREE_QUEUE_T* Q;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  if (size == 0) {
    GKI_exception(GKI_ERROR_BUF_SIZE_ZERO, "getbuf: Size is zero");
    return (nullptr);
  }

  /* Find the first buffer pool that is public that can hold the desired size */
  for (i = 0; i < p_cb->curr_total_no_of_pools; i++) {
    if (size <= p_cb->freeq[p_cb->pool_list[i]].size) break;
  }

  if (i == p_cb->curr_total_no_of_pools) {
    GKI_exception(GKI_ERROR_BUF_SIZE_TOOBIG, "getbuf: Size is too big");
    return (nullptr);
  }

  /* Make sure the buffers aren't disturbed til finished with allocation */
  GKI_disable();

  /* search the public buffer pools that are big enough to hold the size
   * until a free buffer is found */
  for (; i < p_cb->curr_total_no_of_pools; i++) {
    /* Only look at PUBLIC buffer pools (bypass RESTRICTED pools) */
    if (((uint16_t)1 << p_cb->pool_list[i]) & p_cb->pool_access_mask) continue;

    Q = &p_cb->freeq[p_cb->pool_list[i]];
    if (Q->cur_cnt < Q->total) {
      if (Q->p_first == nullptr && gki_alloc_free_queue(i) != true) {
        LOG(ERROR) << StringPrintf("out of buffer");
        GKI_enable();
        return nullptr;
      }

      if (Q->p_first == nullptr) {
        /* gki_alloc_free_queue() failed to alloc memory */
        LOG(ERROR) << StringPrintf("fail alloc free queue");
        GKI_enable();
        return nullptr;
      }

      p_hdr = Q->p_first;
      Q->p_first = p_hdr->p_next;

      if (!Q->p_first) Q->p_last = nullptr;

      if (++Q->cur_cnt > Q->max_cnt) Q->max_cnt = Q->cur_cnt;

      GKI_enable();

      p_hdr->task_id = GKI_get_taskid();

      p_hdr->status = BUF_STATUS_UNLINKED;
      p_hdr->p_next = nullptr;
      p_hdr->Type = 0;
      return ((void*)((uint8_t*)p_hdr + BUFFER_HDR_SIZE));
    }
  }

  LOG(ERROR) << StringPrintf("unable to allocate buffer!!!!!");

  GKI_enable();

  return (nullptr);
#endif
}

/*******************************************************************************
**
** Function         GKI_getpoolbuf
**
** Description      Called by an application to get a free buffer from
**                  a specific buffer pool.
**
**                  Note: If there are no more buffers available from the pool,
**                        the public buffers are searched for an available
**                        buffer.
**
** Parameters       pool_id - (input) pool ID to get a buffer out of.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void* GKI_getpoolbuf(uint8_t pool_id) {
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  uint16_t size = 0;
  switch (pool_id) {
    // NFC_NCI_POOL_ID, NFC_RW_POOL_ID and NFC_CE_POOL_ID are all redefined to
    // GKI_POOL_ID_2.
    case GKI_POOL_ID_2:
      size = GKI_BUF2_SIZE;
      break;

    // LLCP_POOL_ID, GKI_MAX_BUF_SIZE_POOL_ID are redefined to GKI_POOL_ID_3.
    case GKI_POOL_ID_3:
      size = GKI_BUF3_SIZE;
      break;

    default:
      LOG(ERROR) << StringPrintf("Unknown pool ID: %d", pool_id);
      abort();
      break;
  }

  return GKI_getbuf(size);
#else
  FREE_QUEUE_T* Q;
  BUFFER_HDR_T* p_hdr;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) return (nullptr);

  /* Make sure the buffers aren't disturbed til finished with allocation */
  GKI_disable();

  Q = &p_cb->freeq[pool_id];
  if (Q->cur_cnt < Q->total) {
    if (Q->p_first == nullptr && gki_alloc_free_queue(pool_id) != true) return nullptr;

    if (Q->p_first == nullptr) {
      /* gki_alloc_free_queue() failed to alloc memory */
      LOG(ERROR) << StringPrintf("fail alloc free queue");
      return nullptr;
    }

    p_hdr = Q->p_first;
    Q->p_first = p_hdr->p_next;

    if (!Q->p_first) Q->p_last = nullptr;

    if (++Q->cur_cnt > Q->max_cnt) Q->max_cnt = Q->cur_cnt;

    GKI_enable();

    p_hdr->task_id = GKI_get_taskid();

    p_hdr->status = BUF_STATUS_UNLINKED;
    p_hdr->p_next = nullptr;
    p_hdr->Type = 0;

    return ((void*)((uint8_t*)p_hdr + BUFFER_HDR_SIZE));
  }

  /* If here, no buffers in the specified pool */
  GKI_enable();

  /* try for free buffers in public pools */
  return (GKI_getbuf(p_cb->freeq[pool_id].size));
#endif
}

/*******************************************************************************
**
** Function         GKI_freebuf
**
** Description      Called by an application to return a buffer to the free
**                  pool.
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          void
**
*******************************************************************************/
void GKI_freebuf(void* p_buf) {
  BUFFER_HDR_T* p_hdr;

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_FREEBUF_BUF_LINKED, "Freeing Linked Buf");
    return;
  }

  if (p_hdr->q_id >= GKI_NUM_TOTAL_BUF_POOLS) {
    GKI_exception(GKI_ERROR_FREEBUF_BAD_QID, "Bad Buf QId");
    return;
  }

  GKI_os_free(p_hdr);
#else
  FREE_QUEUE_T* Q;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
  if (!p_buf || gki_chk_buf_damage(p_buf)) {
    GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Free - Buf Corrupted");
    return;
  }
#endif

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_FREEBUF_BUF_LINKED, "Freeing Linked Buf");
    return;
  }

  if (p_hdr->q_id >= GKI_NUM_TOTAL_BUF_POOLS) {
    GKI_exception(GKI_ERROR_FREEBUF_BAD_QID, "Bad Buf QId");
    return;
  }

  GKI_disable();

  /*
  ** Release the buffer
  */
  Q = &gki_cb.com.freeq[p_hdr->q_id];
  if (Q->p_last)
    Q->p_last->p_next = p_hdr;
  else
    Q->p_first = p_hdr;

  Q->p_last = p_hdr;
  p_hdr->p_next = nullptr;
  p_hdr->status = BUF_STATUS_FREE;
  p_hdr->task_id = GKI_INVALID_TASK;
  if (Q->cur_cnt > 0) Q->cur_cnt--;

  GKI_enable();
#endif
}

/*******************************************************************************
**
** Function         GKI_get_buf_size
**
** Description      Called by an application to get the size of a buffer.
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          the size of the buffer
**
*******************************************************************************/
uint16_t GKI_get_buf_size(void* p_buf) {
  BUFFER_HDR_T* p_hdr;

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  return p_hdr->size;
#else
  if ((uintptr_t)p_hdr & 1) return (0);

  if (p_hdr->q_id < GKI_NUM_TOTAL_BUF_POOLS) {
    return (gki_cb.com.freeq[p_hdr->q_id].size);
  }

  return (0);
#endif
}

/*******************************************************************************
**
** Function         gki_chk_buf_damage
**
** Description      Called internally by OSS to check for buffer corruption.
**
** Returns          TRUE if there is a problem, else FALSE
**
*******************************************************************************/
bool gki_chk_buf_damage(void* p_buf) {
#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)

  uint32_t* magic;
  magic = (uint32_t*)((uint8_t*)p_buf + GKI_get_buf_size(p_buf));

  if ((uintptr_t)magic & 1) return true;

  if (*magic == MAGIC_NO) return false;

  return true;

#else
  UNUSED(p_buf);
  return false;

#endif
}

/*******************************************************************************
**
** Function         GKI_send_msg
**
** Description      Called by applications to send a buffer to a task
**
** Returns          Nothing
**
*******************************************************************************/
void GKI_send_msg(uint8_t task_id, uint8_t mbox, void* msg) {
  BUFFER_HDR_T* p_hdr;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* If task non-existant or not started, drop buffer */
  if ((task_id >= GKI_MAX_TASKS) || (mbox >= NUM_TASK_MBOX) ||
      (p_cb->OSRdyTbl[task_id] == TASK_DEAD)) {
    GKI_exception(GKI_ERROR_SEND_MSG_BAD_DEST, "Sending to unknown dest");
    GKI_freebuf(msg);
    return;
  }

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
  if (gki_chk_buf_damage(msg)) {
    GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Send - Buffer corrupted");
    return;
  }
#endif

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)msg - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_SEND_MSG_BUF_LINKED, "Send - buffer linked");
    return;
  }

  GKI_disable();

  if (p_cb->OSTaskQFirst[task_id][mbox])
    p_cb->OSTaskQLast[task_id][mbox]->p_next = p_hdr;
  else
    p_cb->OSTaskQFirst[task_id][mbox] = p_hdr;

  p_cb->OSTaskQLast[task_id][mbox] = p_hdr;

  p_hdr->p_next = nullptr;
  p_hdr->status = BUF_STATUS_QUEUED;
  p_hdr->task_id = task_id;

  GKI_enable();

  GKI_send_event(task_id, (uint16_t)EVENT_MASK(mbox));

  return;
}

/*******************************************************************************
**
** Function         GKI_read_mbox
**
** Description      Called by applications to read a buffer from one of
**                  the task mailboxes.  A task can only read its own mailbox.
**
** Parameters:      mbox  - (input) mailbox ID to read (0, 1, 2, or 3)
**
** Returns          NULL if the mailbox was empty, else the address of a buffer
**
*******************************************************************************/
void* GKI_read_mbox(uint8_t mbox) {
  uint8_t task_id = GKI_get_taskid();
  void* p_buf = nullptr;
  BUFFER_HDR_T* p_hdr;

  if ((task_id >= GKI_MAX_TASKS) || (mbox >= NUM_TASK_MBOX)) return (nullptr);

  GKI_disable();

  if (gki_cb.com.OSTaskQFirst[task_id][mbox]) {
    p_hdr = gki_cb.com.OSTaskQFirst[task_id][mbox];
    gki_cb.com.OSTaskQFirst[task_id][mbox] = p_hdr->p_next;

    p_hdr->p_next = nullptr;
    p_hdr->status = BUF_STATUS_UNLINKED;

    p_buf = (uint8_t*)p_hdr + BUFFER_HDR_SIZE;
  }

  GKI_enable();

  return (p_buf);
}

/*******************************************************************************
**
** Function         GKI_enqueue
**
** Description      Enqueue a buffer at the tail of the queue
**
** Parameters:      p_q  -  (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          void
**
*******************************************************************************/
void GKI_enqueue(BUFFER_Q* p_q, void* p_buf) {
  BUFFER_HDR_T* p_hdr;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
  if (gki_chk_buf_damage(p_buf)) {
    GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Enqueue - Buffer corrupted");
    return;
  }
#endif

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_ENQUEUE_BUF_LINKED, "Eneueue - buf already linked");
    return;
  }

  GKI_disable();

  /* Since the queue is exposed (C vs C++), keep the pointers in exposed format
   */
  if (p_q->p_first) {
    BUFFER_HDR_T* p_last_hdr =
        (BUFFER_HDR_T*)((uint8_t*)p_q->p_last - BUFFER_HDR_SIZE);
    p_last_hdr->p_next = p_hdr;
  } else
    p_q->p_first = p_buf;

  p_q->p_last = p_buf;
  p_q->count++;

  p_hdr->p_next = nullptr;
  p_hdr->status = BUF_STATUS_QUEUED;

  GKI_enable();

  return;
}

/*******************************************************************************
**
** Function         GKI_enqueue_head
**
** Description      Enqueue a buffer at the head of the queue
**
** Parameters:      p_q  -  (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          void
**
*******************************************************************************/
void GKI_enqueue_head(BUFFER_Q* p_q, void* p_buf) {
  BUFFER_HDR_T* p_hdr;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
  if (gki_chk_buf_damage(p_buf)) {
    GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Enqueue - Buffer corrupted");
    return;
  }
#endif

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_ENQUEUE_BUF_LINKED,
                  "Eneueue head - buf already linked");
    return;
  }

  GKI_disable();

  if (p_q->p_first) {
    p_hdr->p_next = (BUFFER_HDR_T*)((uint8_t*)p_q->p_first - BUFFER_HDR_SIZE);
    p_q->p_first = p_buf;
  } else {
    p_q->p_first = p_buf;
    p_q->p_last = p_buf;
    p_hdr->p_next = nullptr;
  }
  p_q->count++;

  p_hdr->status = BUF_STATUS_QUEUED;

  GKI_enable();

  return;
}

/*******************************************************************************
**
** Function         GKI_dequeue
**
** Description      Dequeues a buffer from the head of a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void* GKI_dequeue(BUFFER_Q* p_q) {
  BUFFER_HDR_T* p_hdr;

  GKI_disable();

  if (!p_q || !p_q->count) {
    GKI_enable();
    return (nullptr);
  }

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_q->p_first - BUFFER_HDR_SIZE);

  /* Keep buffers such that GKI header is invisible
  */
  if (p_hdr->p_next)
    p_q->p_first = ((uint8_t*)p_hdr->p_next + BUFFER_HDR_SIZE);
  else {
    p_q->p_first = nullptr;
    p_q->p_last = nullptr;
  }

  p_q->count--;

  p_hdr->p_next = nullptr;
  p_hdr->status = BUF_STATUS_UNLINKED;

  GKI_enable();

  return ((uint8_t*)p_hdr + BUFFER_HDR_SIZE);
}

/*******************************************************************************
**
** Function         GKI_remove_from_queue
**
** Description      Dequeue a buffer from the middle of the queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void* GKI_remove_from_queue(BUFFER_Q* p_q, void* p_buf) {
  BUFFER_HDR_T* p_prev;
  BUFFER_HDR_T* p_buf_hdr;

  GKI_disable();

  if (p_buf == p_q->p_first) {
    GKI_enable();
    return (GKI_dequeue(p_q));
  }

  p_buf_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);
  p_prev = (BUFFER_HDR_T*)((uint8_t*)p_q->p_first - BUFFER_HDR_SIZE);

  for (; p_prev; p_prev = p_prev->p_next) {
    /* If the previous points to this one, move the pointers around */
    if (p_prev->p_next == p_buf_hdr) {
      p_prev->p_next = p_buf_hdr->p_next;

      /* If we are removing the last guy in the queue, update p_last */
      if (p_buf == p_q->p_last) p_q->p_last = p_prev + 1;

      /* One less in the queue */
      p_q->count--;

      /* The buffer is now unlinked */
      p_buf_hdr->p_next = nullptr;
      p_buf_hdr->status = BUF_STATUS_UNLINKED;

      GKI_enable();
      return (p_buf);
    }
  }

  GKI_enable();
  return (nullptr);
}

/*******************************************************************************
**
** Function         GKI_getfirst
**
** Description      Return a pointer to the first buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void* GKI_getfirst(BUFFER_Q* p_q) { return (p_q->p_first); }

/*******************************************************************************
**
** Function         GKI_getlast
**
** Description      Return a pointer to the last buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void* GKI_getlast(BUFFER_Q* p_q) { return (p_q->p_last); }

/*******************************************************************************
**
** Function         GKI_getnext
**
** Description      Return a pointer to the next buffer in a queue
**
** Parameters:      p_buf - (input) pointer to the buffer to find the next one
**                                  from.
**
** Returns          NULL if no more buffers in the queue, else next buffer
**                  address
**
*******************************************************************************/
void* GKI_getnext(void* p_buf) {
  BUFFER_HDR_T* p_hdr;

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  if (p_hdr->p_next)
    return ((uint8_t*)p_hdr->p_next + BUFFER_HDR_SIZE);
  else
    return (nullptr);
}

/*******************************************************************************
**
** Function         GKI_queue_is_empty
**
** Description      Check the status of a queue.
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          TRUE if queue is empty, else FALSE
**
*******************************************************************************/
bool GKI_queue_is_empty(BUFFER_Q* p_q) { return ((bool)(p_q->count == 0)); }

/*******************************************************************************
**
** Function         GKI_find_buf_start
**
** Description      This function is called with an address inside a buffer,
**                  and returns the start address ofthe buffer.
**
**                  The buffer should be one allocated from one of GKI's pools.
**
** Parameters:      p_user_area - (input) address of anywhere in a GKI buffer.
**
** Returns          void * - Address of the beginning of the specified buffer if
**                           successful, otherwise NULL if unsuccessful
**
*******************************************************************************/
void* GKI_find_buf_start(void* p_user_area) {
  uint16_t xx, size;
  uint32_t yy;
  tGKI_COM_CB* p_cb = &gki_cb.com;
  uint8_t* p_ua = (uint8_t*)p_user_area;

  for (xx = 0; xx < GKI_NUM_TOTAL_BUF_POOLS; xx++) {
    if ((p_ua > p_cb->pool_start[xx]) && (p_ua < p_cb->pool_end[xx])) {
      yy = (uint32_t)(p_ua - p_cb->pool_start[xx]);

      size = p_cb->pool_size[xx];

      yy = (yy / size) * size;

      return ((void*)(p_cb->pool_start[xx] + yy + sizeof(BUFFER_HDR_T)));
    }
  }

  /* If here, invalid address - not in one of our buffers */
  GKI_exception(GKI_ERROR_BUF_SIZE_ZERO, "GKI_get_buf_start:: bad addr");

  return (nullptr);
}

/********************************************************
* The following functions are not needed for light stack
*********************************************************/
#ifndef BTU_STACK_LITE_ENABLED
#define BTU_STACK_LITE_ENABLED FALSE
#endif

#if (BTU_STACK_LITE_ENABLED == FALSE)

/*******************************************************************************
**
** Function         GKI_set_pool_permission
**
** Description      This function is called to set or change the permissions for
**                  the specified pool ID.
**
** Parameters       pool_id - (input) pool ID to be set or changed
**                  permission - (input) GKI_PUBLIC_POOL or GKI_RESTRICTED_POOL
**
** Returns          GKI_SUCCESS if successful
**                  GKI_INVALID_POOL if unsuccessful
**
*******************************************************************************/
uint8_t GKI_set_pool_permission(uint8_t pool_id, uint8_t permission) {
  tGKI_COM_CB* p_cb = &gki_cb.com;

  if (pool_id < GKI_NUM_TOTAL_BUF_POOLS) {
    if (permission == GKI_RESTRICTED_POOL)
      p_cb->pool_access_mask =
          (uint16_t)(p_cb->pool_access_mask | (1 << pool_id));

    else /* mark the pool as public */
      p_cb->pool_access_mask =
          (uint16_t)(p_cb->pool_access_mask & ~(1 << pool_id));

    return (GKI_SUCCESS);
  } else
    return (GKI_INVALID_POOL);
}

/*******************************************************************************
**
** Function         gki_add_to_pool_list
**
** Description      Adds pool to the pool list which is arranged in the
**                  order of size
**
** Returns          void
**
*******************************************************************************/
static void gki_add_to_pool_list(uint8_t pool_id) {
  int32_t i, j;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* Find the position where the specified pool should be inserted into the list
   */
  for (i = 0; i < p_cb->curr_total_no_of_pools; i++) {
    if (p_cb->freeq[pool_id].size <= p_cb->freeq[p_cb->pool_list[i]].size)
      break;
  }

  /* Insert the new buffer pool ID into the list of pools */
  for (j = p_cb->curr_total_no_of_pools; j > i; j--) {
    p_cb->pool_list[j] = p_cb->pool_list[j - 1];
  }

  p_cb->pool_list[i] = pool_id;

  return;
}

/*******************************************************************************
**
** Function         gki_remove_from_pool_list
**
** Description      Removes pool from the pool list. Called when a pool is
**                  deleted
**
** Returns          void
**
*******************************************************************************/
static void gki_remove_from_pool_list(uint8_t pool_id) {
  tGKI_COM_CB* p_cb = &gki_cb.com;
  uint8_t i;

  for (i = 0; i < p_cb->curr_total_no_of_pools; i++) {
    if (pool_id == p_cb->pool_list[i]) break;
  }

  while (i < (p_cb->curr_total_no_of_pools - 1)) {
    p_cb->pool_list[i] = p_cb->pool_list[i + 1];
    i++;
  }

  return;
}

/*******************************************************************************
**
** Function         GKI_poolcount
**
** Description      Called by an application to get the total number of buffers
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          the total number of buffers in the pool
**
*******************************************************************************/
uint16_t GKI_poolcount(uint8_t pool_id) {
  if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) return (0);

  return (gki_cb.com.freeq[pool_id].total);
}

/*******************************************************************************
**
** Function         GKI_poolfreecount
**
** Description      Called by an application to get the number of free buffers
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          the number of free buffers in the pool
**
*******************************************************************************/
uint16_t GKI_poolfreecount(uint8_t pool_id) {
  FREE_QUEUE_T* Q;

  if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) return (0);

  Q = &gki_cb.com.freeq[pool_id];

  return ((uint16_t)(Q->total - Q->cur_cnt));
}

/*******************************************************************************
**
** Function         GKI_change_buf_owner
**
** Description      Called to change the task ownership of a buffer.
**
** Parameters:      p_buf   - (input) pointer to the buffer
**                  task_id - (input) task id to change ownership to
**
** Returns          void
**
*******************************************************************************/
void GKI_change_buf_owner(void* p_buf, uint8_t task_id) {
  BUFFER_HDR_T* p_hdr = (BUFFER_HDR_T*)((uint8_t*)p_buf - BUFFER_HDR_SIZE);

  p_hdr->task_id = task_id;

  return;
}

#if (GKI_SEND_MSG_FROM_ISR == TRUE)
/*******************************************************************************
**
** Function         GKI_isend_msg
**
** Description      Called from interrupt context to send a buffer to a task
**
** Returns          Nothing
**
*******************************************************************************/
void GKI_isend_msg(uint8_t task_id, uint8_t mbox, void* msg) {
  BUFFER_HDR_T* p_hdr;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* If task non-existant or not started, drop buffer */
  if ((task_id >= GKI_MAX_TASKS) || (mbox >= NUM_TASK_MBOX) ||
      (p_cb->OSRdyTbl[task_id] == TASK_DEAD)) {
    GKI_exception(GKI_ERROR_SEND_MSG_BAD_DEST, "Sending to unknown dest");
    GKI_freebuf(msg);
    return;
  }

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
  if (gki_chk_buf_damage(msg)) {
    GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Send - Buffer corrupted");
    return;
  }
#endif

#if (GKI_ENABLE_OWNER_CHECK == TRUE)
  if (gki_chk_buf_owner(msg)) {
    GKI_exception(GKI_ERROR_NOT_BUF_OWNER, "Send by non-owner");
    return;
  }
#endif

  p_hdr = (BUFFER_HDR_T*)((uint8_t*)msg - BUFFER_HDR_SIZE);

  if (p_hdr->status != BUF_STATUS_UNLINKED) {
    GKI_exception(GKI_ERROR_SEND_MSG_BUF_LINKED, "Send - buffer linked");
    return;
  }

  if (p_cb->OSTaskQFirst[task_id][mbox])
    p_cb->OSTaskQLast[task_id][mbox]->p_next = p_hdr;
  else
    p_cb->OSTaskQFirst[task_id][mbox] = p_hdr;

  p_cb->OSTaskQLast[task_id][mbox] = p_hdr;

  p_hdr->p_next = NULL;
  p_hdr->status = BUF_STATUS_QUEUED;
  p_hdr->task_id = task_id;

  GKI_isend_event(task_id, (uint16_t)EVENT_MASK(mbox));

  return;
}
#endif

/*******************************************************************************
**
** Function         GKI_create_pool
**
** Description      Called by applications to create a buffer pool.
**
** Parameters:      size - (input) length (in bytes) of each buffer in the pool
**                  count - (input) number of buffers to allocate for the pool
**                  permission - (input) restricted or public access?
**                                      (GKI_PUBLIC_POOL or GKI_RESTRICTED_POOL)
**                  p_mem_pool - (input) pointer to an OS memory pool, NULL if
**                                       not provided
**
** Returns          the buffer pool ID, which should be used in calls to
**                  GKI_getpoolbuf(). If a pool could not be created, this
**                  function returns 0xff.
**
*******************************************************************************/
uint8_t GKI_create_pool(uint16_t size, uint16_t count, uint8_t permission,
                        void* p_mem_pool) {
  uint8_t xx;
  uint32_t mem_needed;
  int32_t tempsize = size;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  /* First make sure the size of each pool has a valid size with room for the
   * header info */
  if (size > MAX_USER_BUF_SIZE) return (GKI_INVALID_POOL);

  /* First, look for an unused pool */
  for (xx = 0; xx < GKI_NUM_TOTAL_BUF_POOLS; xx++) {
    if (!p_cb->pool_start[xx]) break;
  }

  if (xx == GKI_NUM_TOTAL_BUF_POOLS) return (GKI_INVALID_POOL);

  /* Ensure an even number of longwords */
  tempsize = (int32_t)ALIGN_POOL(size);

  mem_needed = (tempsize + BUFFER_PADDING_SIZE) * count;

  if (!p_mem_pool) p_mem_pool = GKI_os_malloc(mem_needed);

  if (p_mem_pool) {
    /* Initialize the new pool */
    gki_init_free_queue(xx, size, count, p_mem_pool);
    gki_add_to_pool_list(xx);
    (void)GKI_set_pool_permission(xx, permission);
    p_cb->curr_total_no_of_pools++;

    return (xx);
  } else
    return (GKI_INVALID_POOL);
}

/*******************************************************************************
**
** Function         GKI_delete_pool
**
** Description      Called by applications to delete a buffer pool.  The
**                  function calls the operating specific function to free the
**                  actual memory. An exception is generated if an error is
**                  detected.
**
** Parameters:      pool_id - (input) Id of the poll being deleted.
**
** Returns          void
**
*******************************************************************************/
void GKI_delete_pool(uint8_t pool_id) {
  FREE_QUEUE_T* Q;
  tGKI_COM_CB* p_cb = &gki_cb.com;

  if ((pool_id >= GKI_NUM_TOTAL_BUF_POOLS) || (!p_cb->pool_start[pool_id]))
    return;

  GKI_disable();
  Q = &p_cb->freeq[pool_id];

  if (!Q->cur_cnt) {
    Q->size = 0;
    Q->total = 0;
    Q->cur_cnt = 0;
    Q->max_cnt = 0;
    Q->p_first = nullptr;
    Q->p_last = nullptr;

    GKI_os_free(p_cb->pool_start[pool_id]);

    p_cb->pool_start[pool_id] = nullptr;
    p_cb->pool_end[pool_id] = nullptr;
    p_cb->pool_size[pool_id] = 0;

    gki_remove_from_pool_list(pool_id);
    p_cb->curr_total_no_of_pools--;
  } else
    GKI_exception(GKI_ERROR_DELETE_POOL_BAD_QID, "Deleting bad pool");

  GKI_enable();

  return;
}

#endif /*  BTU_STACK_LITE_ENABLED == FALSE */

/*******************************************************************************
**
** Function         GKI_get_pool_bufsize
**
** Description      Called by an application to get the size of buffers in a
**                  pool
**
** Parameters       Pool ID.
**
** Returns          the size of buffers in the pool
**
*******************************************************************************/
uint16_t GKI_get_pool_bufsize(uint8_t pool_id) {
  if (pool_id < GKI_NUM_TOTAL_BUF_POOLS)
    return (gki_cb.com.freeq[pool_id].size);

  return (0);
}

/*******************************************************************************
**
** Function         GKI_poolutilization
**
** Description      Called by an application to get the buffer utilization
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          % of buffers used from 0 to 100
**
*******************************************************************************/
uint16_t GKI_poolutilization(uint8_t pool_id) {
  FREE_QUEUE_T* Q;

  if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) return (100);

  Q = &gki_cb.com.freeq[pool_id];

  if (Q->total == 0) return (100);

  return ((Q->cur_cnt * 100) / Q->total);
}
