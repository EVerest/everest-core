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
 *  Entry point for NFC_TASK
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "ce_int.h"
#include "gki.h"
#include "nci_hmsgs.h"
#include "nfc_int.h"
#include "rw_int.h"
#if (NFC_RW_ONLY == FALSE)
#include "llcp_int.h"
#else
#define llcp_cleanup()
#endif

#include "nfa_dm_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         nfc_start_timer
**
** Description      Start a timer for the specified amount of time.
**                  NOTE: The timeout resolution is in SECONDS! (Even
**                          though the timer structure field is ticks)
**
** Returns          void
**
*******************************************************************************/
void nfc_start_timer(TIMER_LIST_ENT* p_tle, uint16_t type, uint32_t timeout) {
  NFC_HDR* p_msg;

  /* if timer list is currently empty, start periodic GKI timer */
  if (nfc_cb.timer_queue.p_first == nullptr) {
    /* if timer starts on other than NFC task (scritp wrapper) */
    if (GKI_get_taskid() != NFC_TASK) {
      /* post event to start timer in NFC task */
      p_msg = (NFC_HDR*)GKI_getbuf(NFC_HDR_SIZE);
      if (p_msg != nullptr) {
        p_msg->event = BT_EVT_TO_START_TIMER;
        GKI_send_msg(NFC_TASK, NFC_MBOX_ID, p_msg);
      }
    } else {
      /* Start nfc_task 1-sec resolution timer */
      GKI_start_timer(NFC_TIMER_ID, GKI_SECS_TO_TICKS(1), true);
    }
  }

  GKI_remove_from_timer_list(&nfc_cb.timer_queue, p_tle);

  p_tle->event = type;
  p_tle->ticks = timeout; /* Save the number of seconds for the timer */

  GKI_add_to_timer_list(&nfc_cb.timer_queue, p_tle);
}

/*******************************************************************************
**
** Function         nfc_remaining_time
**
** Description      Return amount of time to expire
**
** Returns          time in second
**
*******************************************************************************/
uint32_t nfc_remaining_time(TIMER_LIST_ENT* p_tle) {
  return (GKI_get_remaining_ticks(&nfc_cb.timer_queue, p_tle));
}

/*******************************************************************************
**
** Function         nfc_process_timer_evt
**
** Description      Process nfc GKI timer event
**
** Returns          void
**
*******************************************************************************/
void nfc_process_timer_evt(void) {
  TIMER_LIST_ENT* p_tle;

  GKI_update_timer_list(&nfc_cb.timer_queue, 1);

  while ((nfc_cb.timer_queue.p_first) && (!nfc_cb.timer_queue.p_first->ticks)) {
    p_tle = nfc_cb.timer_queue.p_first;
    GKI_remove_from_timer_list(&nfc_cb.timer_queue, p_tle);

    switch (p_tle->event) {
      case NFC_TTYPE_NCI_WAIT_RSP:
        nfc_ncif_cmd_timeout();
        break;

      case NFC_TTYPE_WAIT_2_DEACTIVATE:
        nfc_wait_2_deactivate_timeout();
        break;
      case NFC_TTYPE_WAIT_MODE_SET_NTF:
        nfc_mode_set_ntf_timeout();
        break;
      default:
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("nfc_process_timer_evt: timer:0x%p event (0x%04x)",
                            p_tle, p_tle->event);
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "nfc_process_timer_evt: unhandled timer event (0x%04x)",
            p_tle->event);
    }
  }

  /* if timer list is empty stop periodic GKI timer */
  if (nfc_cb.timer_queue.p_first == nullptr) {
    GKI_stop_timer(NFC_TIMER_ID);
  }
}

/*******************************************************************************
**
** Function         nfc_stop_timer
**
** Description      Stop a timer.
**
** Returns          void
**
*******************************************************************************/
void nfc_stop_timer(TIMER_LIST_ENT* p_tle) {
  GKI_remove_from_timer_list(&nfc_cb.timer_queue, p_tle);

  /* if timer list is empty stop periodic GKI timer */
  if (nfc_cb.timer_queue.p_first == nullptr) {
    GKI_stop_timer(NFC_TIMER_ID);
  }
}

/*******************************************************************************
**
** Function         nfc_start_quick_timer
**
** Description      Start a timer for the specified amount of time.
**                  NOTE: The timeout resolution depends on including modules.
**                  QUICK_TIMER_TICKS_PER_SEC should be used to convert from
**                  time to ticks.
**
**
** Returns          void
**
*******************************************************************************/
void nfc_start_quick_timer(TIMER_LIST_ENT* p_tle, uint16_t type,
                           uint32_t timeout) {
  NFC_HDR* p_msg;

  /* if timer list is currently empty, start periodic GKI timer */
  if (nfc_cb.quick_timer_queue.p_first == nullptr) {
    /* if timer starts on other than NFC task (scritp wrapper) */
    if (GKI_get_taskid() != NFC_TASK) {
      /* post event to start timer in NFC task */
      p_msg = (NFC_HDR*)GKI_getbuf(NFC_HDR_SIZE);
      if (p_msg != nullptr) {
        p_msg->event = BT_EVT_TO_START_QUICK_TIMER;
        GKI_send_msg(NFC_TASK, NFC_MBOX_ID, p_msg);
      }
    } else {
      /* Quick-timer is required for LLCP */
      GKI_start_timer(NFC_QUICK_TIMER_ID,
                      ((GKI_SECS_TO_TICKS(1) / QUICK_TIMER_TICKS_PER_SEC)),
                      true);
    }
  }

  GKI_remove_from_timer_list(&nfc_cb.quick_timer_queue, p_tle);

  p_tle->event = type;
  p_tle->ticks = timeout; /* Save the number of ticks for the timer */

  GKI_add_to_timer_list(&nfc_cb.quick_timer_queue, p_tle);
}

/*******************************************************************************
**
** Function         nfc_stop_quick_timer
**
** Description      Stop a timer.
**
** Returns          void
**
*******************************************************************************/
void nfc_stop_quick_timer(TIMER_LIST_ENT* p_tle) {
  GKI_remove_from_timer_list(&nfc_cb.quick_timer_queue, p_tle);

  /* if timer list is empty stop periodic GKI timer */
  if (nfc_cb.quick_timer_queue.p_first == nullptr) {
    GKI_stop_timer(NFC_QUICK_TIMER_ID);
  }
}

/*******************************************************************************
**
** Function         nfc_process_quick_timer_evt
**
** Description      Process quick timer event
**
** Returns          void
**
*******************************************************************************/
void nfc_process_quick_timer_evt(void) {
  TIMER_LIST_ENT* p_tle;

  GKI_update_timer_list(&nfc_cb.quick_timer_queue, 1);

  while ((nfc_cb.quick_timer_queue.p_first) &&
         (!nfc_cb.quick_timer_queue.p_first->ticks)) {
    p_tle = nfc_cb.quick_timer_queue.p_first;
    GKI_remove_from_timer_list(&nfc_cb.quick_timer_queue, p_tle);

    switch (p_tle->event) {
#if (NFC_RW_ONLY == FALSE)
      case NFC_TTYPE_LLCP_LINK_MANAGER:
      case NFC_TTYPE_LLCP_LINK_INACT:
      case NFC_TTYPE_LLCP_DATA_LINK:
      case NFC_TTYPE_LLCP_DELAY_FIRST_PDU:
        llcp_process_timeout(p_tle);
        break;
#endif
      case NFC_TTYPE_RW_T1T_RESPONSE:
        rw_t1t_process_timeout(p_tle);
        break;
      case NFC_TTYPE_RW_T2T_RESPONSE:
        rw_t2t_process_timeout();
        break;
      case NFC_TTYPE_RW_T3T_RESPONSE:
        rw_t3t_process_timeout(p_tle);
        break;
      case NFC_TTYPE_RW_T4T_RESPONSE:
        rw_t4t_process_timeout(p_tle);
        break;
      case NFC_TTYPE_RW_I93_RESPONSE:
        rw_i93_process_timeout(p_tle);
        break;
      case NFC_TTYPE_P2P_PRIO_RESPONSE:
        nfa_dm_p2p_timer_event();
        break;
      case NFC_TTYPE_P2P_PRIO_LOGIC_CLEANUP:
        nfa_dm_p2p_prio_logic_cleanup();
        break;
      case NFC_TTYPE_RW_MFC_RESPONSE:
        rw_mfc_process_timeout(p_tle);
        break;

#if (NFC_RW_ONLY == FALSE)
      case NFC_TTYPE_CE_T4T_UPDATE:
        ce_t4t_process_timeout(p_tle);
        break;
#endif
      default:
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "nfc_process_quick_timer_evt: unhandled timer event (0x%04x)",
            p_tle->event);
        break;
    }
  }

  /* if timer list is empty stop periodic GKI timer */
  if (nfc_cb.quick_timer_queue.p_first == nullptr) {
    GKI_stop_timer(NFC_QUICK_TIMER_ID);
  }
}

/*******************************************************************************
**
** Function         nfc_task_shutdown_nfcc
**
** Description      Handle NFC shutdown
**
** Returns          nothing
**
*******************************************************************************/
void nfc_task_shutdown_nfcc(void) {
  NFC_HDR* p_msg;

  /* Free any messages still in the mbox */
  while ((p_msg = (NFC_HDR*)GKI_read_mbox(NFC_MBOX_ID)) != nullptr) {
    GKI_freebuf(p_msg);
  }

  nfc_gen_cleanup();

  if (nfc_cb.flags & NFC_FL_POWER_OFF_SLEEP) {
    nfc_set_state(NFC_STATE_W4_HAL_CLOSE);
    nfc_cb.p_hal->close();
  } else if (nfc_cb.flags & NFC_FL_POWER_CYCLE_NFCC) {
    nfc_set_state(NFC_STATE_W4_HAL_OPEN);
    nfc_cb.p_hal->power_cycle();
  } else {
    nfc_set_state(NFC_STATE_W4_HAL_CLOSE);
    nfc_cb.p_hal->close();

    /* Perform final clean up */
    llcp_cleanup();

    /* Stop the timers */
    GKI_stop_timer(NFC_TIMER_ID);
    GKI_stop_timer(NFC_QUICK_TIMER_ID);
    GKI_stop_timer(NFA_TIMER_ID);
  }
}

/*******************************************************************************
**
** Function         nfc_task
**
** Description      NFC event processing task
**
** Returns          nothing
**
*******************************************************************************/
uint32_t nfc_task(__attribute__((unused)) uint32_t arg) {
  uint16_t event;
  NFC_HDR* p_msg;
  bool free_buf;

  /* Initialize the nfc control block */
  memset(&nfc_cb, 0, sizeof(tNFC_CB));

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("NFC_TASK started.");

  /* main loop */
  while (true) {
    event = GKI_wait(0xFFFF, 0);
    if (event == EVENT_MASK(GKI_SHUTDOWN_EVT)) {
      break;
    }
    /* Handle NFC_TASK_EVT_TRANSPORT_READY from NFC HAL */
    if (event & NFC_TASK_EVT_TRANSPORT_READY) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFC_TASK got NFC_TASK_EVT_TRANSPORT_READY.");

      /* Reset the NFC controller. */
      nfc_set_state(NFC_STATE_CORE_INIT);
      #if (NXP_EXTNS == TRUE)
            nci_snd_core_reset(NCI_RESET_TYPE_KEEP_CFG);
      #else
            nci_snd_core_reset(NCI_RESET_TYPE_RESET_CFG);
      #endif
    }

    if (event & NFC_MBOX_EVT_MASK) {
      /* Process all incoming NCI messages */
      while ((p_msg = (NFC_HDR*)GKI_read_mbox(NFC_MBOX_ID)) != nullptr) {
        free_buf = true;

        /* Determine the input message type. */
        switch (p_msg->event & NFC_EVT_MASK) {
          case BT_EVT_TO_NFC_NCI:
            free_buf = nfc_ncif_process_event(p_msg);
            break;

          case BT_EVT_TO_START_TIMER:
            /* Start nfc_task 1-sec resolution timer */
            GKI_start_timer(NFC_TIMER_ID, GKI_SECS_TO_TICKS(1), true);
            break;

          case BT_EVT_TO_START_QUICK_TIMER:
            /* Quick-timer is required for LLCP */
            GKI_start_timer(
                NFC_QUICK_TIMER_ID,
                ((GKI_SECS_TO_TICKS(1) / QUICK_TIMER_TICKS_PER_SEC)), true);
            break;

          case BT_EVT_TO_NFC_MSGS:
            nfc_main_handle_hal_evt((tNFC_HAL_EVT_MSG*)p_msg);
            break;

          default:
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "nfc_task: unhandle mbox message, event=%04x", p_msg->event);
            break;
        }

        if (free_buf) {
          GKI_freebuf(p_msg);
        }
      }
    }

    /* Process gki timer tick */
    if (event & NFC_TIMER_EVT_MASK) {
      nfc_process_timer_evt();
    }

    /* Process quick timer tick */
    if (event & NFC_QUICK_TIMER_EVT_MASK) {
      nfc_process_quick_timer_evt();
    }

    if (event & NFA_MBOX_EVT_MASK) {
      while ((p_msg = (NFC_HDR*)GKI_read_mbox(NFA_MBOX_ID)) != nullptr) {
        nfa_sys_event(p_msg);
      }
    }

    if (event & NFA_TIMER_EVT_MASK) {
      nfa_sys_timer_update();
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfc_task terminated");

  GKI_exit_task(GKI_get_taskid());
  return 0;
}
