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
 *  This file contains the call-in functions for NFA HCI
 *
 ******************************************************************************/
#include <string.h>
#include "nfa_hci_api.h"
#include "nfa_hci_int.h"
#include "nfa_nv_co.h"

/*******************************************************************************
**
** Function         nfa_nv_ci_read
**
** Description      call-in function for non volatile memory read acess
**
** Returns          none
**
*******************************************************************************/
void nfa_nv_ci_read(uint16_t num_bytes_read, tNFA_NV_CO_STATUS status,
                    uint8_t block) {
  tNFA_HCI_EVENT_DATA* p_msg;

  p_msg = (tNFA_HCI_EVENT_DATA*)GKI_getbuf(sizeof(tNFA_HCI_EVENT_DATA));
  if (p_msg != nullptr) {
    p_msg->nv_read.hdr.event = NFA_HCI_RSP_NV_READ_EVT;

    if ((status == NFA_STATUS_OK) && (num_bytes_read != 0))
      p_msg->nv_read.status = NFA_STATUS_OK;
    else
      p_msg->nv_read.status = NFA_STATUS_FAILED;

    p_msg->nv_read.size = num_bytes_read;
    p_msg->nv_read.block = block;
    nfa_sys_sendmsg(p_msg);
  }
}

/*******************************************************************************
**
** Function         nfa_nv_ci_write
**
** Description      call-in function for non volatile memory write acess
**
** Returns          none
**
*******************************************************************************/
void nfa_nv_ci_write(tNFA_NV_CO_STATUS status) {
  tNFA_HCI_EVENT_DATA* p_msg;

  p_msg = (tNFA_HCI_EVENT_DATA*)GKI_getbuf(sizeof(tNFA_HCI_EVENT_DATA));
  if (p_msg != nullptr) {
    p_msg->nv_write.hdr.event = NFA_HCI_RSP_NV_WRITE_EVT;
    p_msg->nv_write.status =
        (status == NFA_NV_CO_OK) ? NFA_STATUS_OK : NFA_STATUS_FAILED;
    nfa_sys_sendmsg(p_msg);
  }
}
