/******************************************************************************
 *
 *  Copyright (C) 1999-2014 Broadcom Corporation
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
 *  This file contains functions that interface with the NFC NCI transport.
 *  On the receive side, it routes events to the appropriate handler
 *  (callback). On the transmit side, it manages the command transmission.
 *
 ******************************************************************************/
#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "bt_types.h"
#include "nfc_api.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         nfc_alloc_conn_cb
**
** Description      This function is called to allocation a control block for
**                  NCI logical connection
**
** Returns          The allocated control block or NULL
**
*******************************************************************************/
tNFC_CONN_CB* nfc_alloc_conn_cb(tNFC_CONN_CBACK* p_cback) {
  int xx, max = NCI_MAX_CONN_CBS;
  tNFC_CONN_CB* p_conn_cb = nullptr;

#if (NXP_EXTNS == TRUE)
/* Since T4T NFCEE connection will be active in parallel with HCI, it should
be still NCI_MAX_CONN_CBS. additional check using NFC_CHECK_MAX_CONN will
reduce the connection number. So it is commented*/
// NFC_CHECK_MAX_CONN();
#else
  NFC_CHECK_MAX_CONN();
#endif
  for (xx = 0; xx < max; xx++) {
    if (nfc_cb.conn_cb[xx].conn_id == NFC_ILLEGAL_CONN_ID) {
      nfc_cb.conn_cb[xx].conn_id =
          NFC_PEND_CONN_ID; /* to indicate this cb is used */
      p_conn_cb = &nfc_cb.conn_cb[xx];
      p_conn_cb->p_cback = p_cback;
      break;
    }
  }
  return p_conn_cb;
}

/*******************************************************************************
**
** Function         nfc_set_conn_id
**
** Description      This function is called to set the connection id to the
**                  connection control block and the id mapping table
**
** Returns          void
**
*******************************************************************************/
void nfc_set_conn_id(tNFC_CONN_CB* p_cb, uint8_t conn_id) {
  uint8_t handle;

  if (p_cb == nullptr) return;

  p_cb->conn_id = conn_id;
  handle = (uint8_t)(p_cb - nfc_cb.conn_cb + 1);
  nfc_cb.conn_id[conn_id] = handle;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_set_conn_id conn_id:%d, handle:%d", conn_id, handle);
}

/*******************************************************************************
**
** Function         nfc_find_conn_cb_by_handle
**
** Description      This function is called to locate the control block for
**                  loopback test.
**
** Returns          The loopback test control block or NULL
**
*******************************************************************************/
tNFC_CONN_CB* nfc_find_conn_cb_by_handle(uint8_t id) {
  int xx;
  tNFC_CONN_CB* p_conn_cb = nullptr;

  for (xx = 0; xx < NCI_MAX_CONN_CBS; xx++) {
    if (nfc_cb.conn_cb[xx].id == id) {
      p_conn_cb = &nfc_cb.conn_cb[xx];
      break;
    }
  }
  return p_conn_cb;
}

/*******************************************************************************
**
** Function         nfc_find_conn_cb_by_conn_id
**
** Description      This function is called to locate the control block for
**                  the given connection id
**
** Returns          The control block or NULL
**
*******************************************************************************/
tNFC_CONN_CB* nfc_find_conn_cb_by_conn_id(uint8_t conn_id) {
  tNFC_CONN_CB* p_conn_cb = nullptr;
  uint8_t handle;
  uint8_t id;
  int xx;

  if (conn_id == NFC_PEND_CONN_ID) {
    for (xx = 0; xx < NCI_MAX_CONN_CBS; xx++) {
      if (nfc_cb.conn_cb[xx].conn_id == NFC_PEND_CONN_ID) {
        p_conn_cb = &nfc_cb.conn_cb[xx];
        break;
      }
    }
  } else {
    id = conn_id & NFC_CONN_ID_ID_MASK;
    if (id < NFC_MAX_CONN_ID) {
      handle = nfc_cb.conn_id[id];
      if (handle > 0) p_conn_cb = &nfc_cb.conn_cb[handle - 1];
    }
  }

  return p_conn_cb;
}

/*******************************************************************************
**
** Function         nfc_free_conn_cb
**
** Description      This function is called to free the control block and
**                  resources and id mapping table
**
** Returns          void
**
*******************************************************************************/
void nfc_free_conn_cb(tNFC_CONN_CB* p_cb) {
  void* p_buf;

  if (p_cb == nullptr) return;

  while ((p_buf = GKI_dequeue(&p_cb->rx_q)) != nullptr) GKI_freebuf(p_buf);

  while ((p_buf = GKI_dequeue(&p_cb->tx_q)) != nullptr) GKI_freebuf(p_buf);

  nfc_cb.conn_id[p_cb->conn_id] = 0;
  p_cb->p_cback = nullptr;
  p_cb->conn_id = NFC_ILLEGAL_CONN_ID;
}

/*******************************************************************************
**
** Function         nfc_reset_all_conn_cbs
**
** Description      This function is called to free all the control blocks and
**                  resources and id mapping table
**
** Returns          void
**
*******************************************************************************/
extern void nfc_reset_all_conn_cbs(void) {
  int xx;
  tNFC_CONN_CB* p_conn_cb = &nfc_cb.conn_cb[0];
  tNFC_DEACTIVATE_DEVT deact = tNFC_DEACTIVATE_DEVT();

  deact.status = NFC_STATUS_NOT_INITIALIZED;
  deact.type = NFC_DEACTIVATE_TYPE_IDLE;
  deact.is_ntf = TRUE;
  for (xx = 0; xx < NCI_MAX_CONN_CBS; xx++, p_conn_cb++) {
    if (p_conn_cb->conn_id != NFC_ILLEGAL_CONN_ID) {
      if (p_conn_cb->p_cback) {
        tNFC_CONN nfc_conn;
        nfc_conn.deactivate = deact;
        (*p_conn_cb->p_cback)(p_conn_cb->conn_id, NFC_DEACTIVATE_CEVT,
                              &nfc_conn);
      }
      nfc_free_conn_cb(p_conn_cb);
    }
  }
}
