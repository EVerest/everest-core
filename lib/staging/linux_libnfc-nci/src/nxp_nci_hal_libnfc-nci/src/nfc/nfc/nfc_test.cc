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
 *  This file contains functions that interface with the NFC NCI transport.
 *  On the receive side, it routes events to the appropriate handler
 *  (callback). On the transmit side, it manages the command transmission.
 *
 ******************************************************************************/
#include <string.h>
#include "bt_types.h"
#include "gki.h"
#include "nfc_target.h"

#include "nci_hmsgs.h"
#include "nfc_int.h"

/****************************************************************************
** Declarations
****************************************************************************/

/*******************************************************************************
**
** Function         NFC_TestLoopback
**
** Description      This function is called to send the given data packet
**                  to NFCC for loopback test.
**                  When loopback data is received from NFCC, tNFC_TEST_CBACK .
**                  reports a NFC_LOOPBACK_TEVT.
**
** Parameters       p_data - the data packet
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_TestLoopback(NFC_HDR* p_data) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_handle(NCI_TEST_ID);

  if (p_data && p_cb &&
      (p_data->offset >= (NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE))) {
    status = nfc_ncif_send_data(p_cb, p_data);
  }

  if (status != NFC_STATUS_OK) GKI_freebuf(p_data);

  return status;
}
