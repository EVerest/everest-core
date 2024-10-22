/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
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

/******************************************************************************
 *
 *  Protocol timer services (taken from bta ptim)
 *
 ******************************************************************************/
#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_sys_ptim.h"
#include "nfa_sys.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         nfa_sys_ptim_init
**
** Description      Initialize a protocol timer control block.  Parameter
**                  period is the GKI timer period in milliseconds.  Parameter
**                  timer_id is the GKI timer id.
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_ptim_init(tPTIM_CB* p_cb, uint16_t period, uint8_t timer_id) {
  GKI_init_timer_list(&p_cb->timer_queue);
  p_cb->period = period;
  p_cb->timer_id = timer_id;
}

/*******************************************************************************
**
** Function         nfa_sys_ptim_timer_update
**
** Description      Update the protocol timer list and handle expired timers.
**                  This function is called from the task running the protocol
**                  timers when the periodic GKI timer expires.
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_ptim_timer_update(tPTIM_CB* p_cb) {
  TIMER_LIST_ENT* p_tle;
  NFC_HDR* p_msg;
  uint32_t new_ticks_count;
  int32_t period_in_ticks;

  /* To handle the case when the function is called less frequently than the
     period
     we must convert determine the number of ticks since the last update, then
     convert back to milliseconds before updating timer list */
  new_ticks_count = GKI_get_tick_count();

  /* Check for wrapped condition */
  if (new_ticks_count >= p_cb->last_gki_ticks) {
    period_in_ticks = (int32_t)(new_ticks_count - p_cb->last_gki_ticks);
  } else {
    period_in_ticks = (int32_t)(((uint32_t)0xffffffff - p_cb->last_gki_ticks) +
                                new_ticks_count + 1);
  }

  /* update timer list */
  GKI_update_timer_list(&p_cb->timer_queue, GKI_TICKS_TO_MS(period_in_ticks));

  p_cb->last_gki_ticks = new_ticks_count;

  /* while there are expired timers */
  while ((p_cb->timer_queue.p_first) &&
         (p_cb->timer_queue.p_first->ticks <= 0)) {
    /* removed expired timer from list */
    p_tle = p_cb->timer_queue.p_first;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("nfa_sys_ptim_timer_update expired: %p", p_tle);
    GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

    /* call timer callback */
    if (p_tle->p_cback) {
      (*p_tle->p_cback)(p_tle);
    } else if (p_tle->event) {
      p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR));
      if (p_msg != nullptr) {
        p_msg->event = p_tle->event;
        p_msg->layer_specific = 0;
        nfa_sys_sendmsg(p_msg);
      }
    }
  }

  /* if timer list is empty stop periodic GKI timer */
  if (p_cb->timer_queue.p_first == nullptr) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("ptim timer stop");
    GKI_stop_timer(p_cb->timer_id);
  }
}

/*******************************************************************************
**
** Function         nfa_sys_ptim_start_timer
**
** Description      Start a protocol timer for the specified amount
**                  of time in seconds.
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_ptim_start_timer(tPTIM_CB* p_cb, TIMER_LIST_ENT* p_tle,
                              uint16_t type, int32_t timeout) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_sys_ptim_start_timer %p", p_tle);

  /* if timer list is currently empty, start periodic GKI timer */
  if (p_cb->timer_queue.p_first == nullptr) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("ptim timer start");
    p_cb->last_gki_ticks = GKI_get_tick_count();
    GKI_start_timer(p_cb->timer_id, GKI_MS_TO_TICKS(p_cb->period), true);
  }

  GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

  p_tle->event = type;
  p_tle->ticks = timeout;

  GKI_add_to_timer_list(&p_cb->timer_queue, p_tle);
}

/*******************************************************************************
**
** Function         nfa_sys_ptim_stop_timer
**
** Description      Stop a protocol timer.
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_ptim_stop_timer(tPTIM_CB* p_cb, TIMER_LIST_ENT* p_tle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_sys_ptim_stop_timer %p", p_tle);

  GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

  /* if timer list is empty stop periodic GKI timer */
  if (p_cb->timer_queue.p_first == nullptr) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("ptim timer stop");
    GKI_stop_timer(p_cb->timer_id);
  }
}
