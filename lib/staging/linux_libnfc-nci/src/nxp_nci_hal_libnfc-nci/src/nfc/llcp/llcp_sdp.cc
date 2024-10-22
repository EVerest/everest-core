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
 *  This file contains the LLCP Service Discovery
 *
 ******************************************************************************/

#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "bt_types.h"
#include "gki.h"
#include "llcp_api.h"
#include "llcp_int.h"
#include "nfa_dm_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         llcp_sdp_proc_data
**
** Description      Do nothing
**
**
** Returns          void
**
*******************************************************************************/
void llcp_sdp_proc_data(__attribute__((unused)) tLLCP_SAP_CBACK_DATA* p_data) {
  /*
  ** Do nothing
  ** llcp_sdp_proc_SNL () is called by link layer
  */
}

/*******************************************************************************
**
** Function         llcp_sdp_check_send_snl
**
** Description      Enqueue Service Name Lookup PDU into sig_xmit_q for
**                  transmitting
**
**
** Returns          void
**
*******************************************************************************/
void llcp_sdp_check_send_snl(void) {
  uint8_t* p;

  if (llcp_cb.sdp_cb.p_snl) {
    DLOG_IF(INFO, nfc_debug_enabled) << __func__;

    llcp_cb.sdp_cb.p_snl->len += LLCP_PDU_HEADER_SIZE;
    llcp_cb.sdp_cb.p_snl->offset -= LLCP_PDU_HEADER_SIZE;

    p = (uint8_t*)(llcp_cb.sdp_cb.p_snl + 1) + llcp_cb.sdp_cb.p_snl->offset;
    UINT16_TO_BE_STREAM(
        p, LLCP_GET_PDU_HEADER(LLCP_SAP_SDP, LLCP_PDU_SNL_TYPE, LLCP_SAP_SDP));

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, llcp_cb.sdp_cb.p_snl);
    llcp_cb.sdp_cb.p_snl = nullptr;
  } else {
    /* Notify DTA after sending out SNL with SDRES not to send SNLs in AGF PDU
     */
    if (llcp_cb.p_dta_cback && llcp_cb.dta_snl_resp) {
      llcp_cb.dta_snl_resp = false;
      (*llcp_cb.p_dta_cback)();
    }
  }
}

/*******************************************************************************
**
** Function         llcp_sdp_add_sdreq
**
** Description      Add Service Discovery Request into SNL PDU
**
**
** Returns          void
**
*******************************************************************************/
static void llcp_sdp_add_sdreq(uint8_t tid, char* p_name) {
  uint8_t* p;
  uint16_t name_len = (uint16_t)strlen(p_name);

  p = (uint8_t*)(llcp_cb.sdp_cb.p_snl + 1) + llcp_cb.sdp_cb.p_snl->offset +
      llcp_cb.sdp_cb.p_snl->len;

  UINT8_TO_BE_STREAM(p, LLCP_SDREQ_TYPE);
  UINT8_TO_BE_STREAM(p, (1 + name_len));
  UINT8_TO_BE_STREAM(p, tid);
  ARRAY_TO_BE_STREAM(p, p_name, name_len);

  llcp_cb.sdp_cb.p_snl->len += LLCP_SDREQ_MIN_LEN + name_len;
}

/*******************************************************************************
**
** Function         llcp_sdp_send_sdreq
**
** Description      Send Service Discovery Request
**
**
** Returns          LLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_sdp_send_sdreq(uint8_t tid, char* p_name) {
  tLLCP_STATUS status;
  uint16_t name_len;
  uint16_t available_bytes;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("tid=0x%x, ServiceName=%s", tid, p_name);

  /* if there is no pending SNL */
  if (!llcp_cb.sdp_cb.p_snl) {
    llcp_cb.sdp_cb.p_snl = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

    if (llcp_cb.sdp_cb.p_snl) {
      llcp_cb.sdp_cb.p_snl->offset =
          NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + LLCP_PDU_HEADER_SIZE;
      llcp_cb.sdp_cb.p_snl->len = 0;
    }
  }

  if (llcp_cb.sdp_cb.p_snl) {
    available_bytes = GKI_get_buf_size(llcp_cb.sdp_cb.p_snl) - NFC_HDR_SIZE -
                      llcp_cb.sdp_cb.p_snl->offset - llcp_cb.sdp_cb.p_snl->len;

    name_len = (uint16_t)strlen(p_name);

    /* if SDREQ parameter can be added in SNL */
    if ((available_bytes >= LLCP_SDREQ_MIN_LEN + name_len) &&
        (llcp_cb.sdp_cb.p_snl->len + LLCP_SDREQ_MIN_LEN + name_len <=
         llcp_cb.lcb.effective_miu)) {
      llcp_sdp_add_sdreq(tid, p_name);
      status = LLCP_STATUS_SUCCESS;
    } else {
      /* send pending SNL PDU to LM */
      llcp_sdp_check_send_snl();

      llcp_cb.sdp_cb.p_snl = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

      if (llcp_cb.sdp_cb.p_snl) {
        llcp_cb.sdp_cb.p_snl->offset =
            NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + LLCP_PDU_HEADER_SIZE;
        llcp_cb.sdp_cb.p_snl->len = 0;

        llcp_sdp_add_sdreq(tid, p_name);

        status = LLCP_STATUS_SUCCESS;
      } else {
        status = LLCP_STATUS_FAIL;
      }
    }
  } else {
    status = LLCP_STATUS_FAIL;
  }

  /* if LM is waiting for PDUs from upper layer */
  if ((status == LLCP_STATUS_SUCCESS) &&
      (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_LOCAL_XMIT_NEXT)) {
    llcp_link_check_send_data();
  }

  return status;
}

/*******************************************************************************
**
** Function         llcp_sdp_add_sdres
**
** Description      Add Service Discovery Response into SNL PDU
**
**
** Returns          void
**
*******************************************************************************/
static void llcp_sdp_add_sdres(uint8_t tid, uint8_t sap) {
  uint8_t* p;

  p = (uint8_t*)(llcp_cb.sdp_cb.p_snl + 1) + llcp_cb.sdp_cb.p_snl->offset +
      llcp_cb.sdp_cb.p_snl->len;

  UINT8_TO_BE_STREAM(p, LLCP_SDRES_TYPE);
  UINT8_TO_BE_STREAM(p, LLCP_SDRES_LEN);
  UINT8_TO_BE_STREAM(p, tid);
  UINT8_TO_BE_STREAM(p, sap);

  llcp_cb.sdp_cb.p_snl->len += 2 + LLCP_SDRES_LEN; /* type and length */
}

/*******************************************************************************
**
** Function         llcp_sdp_send_sdres
**
** Description      Send Service Discovery Response
**
**
** Returns          LLCP_STATUS
**
*******************************************************************************/
static tLLCP_STATUS llcp_sdp_send_sdres(uint8_t tid, uint8_t sap) {
  tLLCP_STATUS status;
  uint16_t available_bytes;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("tid=0x%x, SAP=0x%x", tid, sap);

  /* if there is no pending SNL */
  if (!llcp_cb.sdp_cb.p_snl) {
    llcp_cb.sdp_cb.p_snl = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

    if (llcp_cb.sdp_cb.p_snl) {
      llcp_cb.sdp_cb.p_snl->offset =
          NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + LLCP_PDU_HEADER_SIZE;
      llcp_cb.sdp_cb.p_snl->len = 0;
    }
  }

  if (llcp_cb.sdp_cb.p_snl) {
    available_bytes = GKI_get_buf_size(llcp_cb.sdp_cb.p_snl) - NFC_HDR_SIZE -
                      llcp_cb.sdp_cb.p_snl->offset - llcp_cb.sdp_cb.p_snl->len;

    /* if SDRES parameter can be added in SNL */
    if ((available_bytes >= 2 + LLCP_SDRES_LEN) &&
        (llcp_cb.sdp_cb.p_snl->len + 2 + LLCP_SDRES_LEN <=
         llcp_cb.lcb.effective_miu)) {
      llcp_sdp_add_sdres(tid, sap);
      status = LLCP_STATUS_SUCCESS;
    } else {
      /* send pending SNL PDU to LM */
      llcp_sdp_check_send_snl();

      llcp_cb.sdp_cb.p_snl = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

      if (llcp_cb.sdp_cb.p_snl) {
        llcp_cb.sdp_cb.p_snl->offset =
            NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + LLCP_PDU_HEADER_SIZE;
        llcp_cb.sdp_cb.p_snl->len = 0;

        llcp_sdp_add_sdres(tid, sap);

        status = LLCP_STATUS_SUCCESS;
      } else {
        status = LLCP_STATUS_FAIL;
      }
    }
  } else {
    status = LLCP_STATUS_FAIL;
  }

  /* if LM is waiting for PDUs from upper layer */
  if ((status == LLCP_STATUS_SUCCESS) &&
      (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_LOCAL_XMIT_NEXT)) {
    llcp_link_check_send_data();
  }

  return status;
}

/*******************************************************************************
**
** Function         llcp_sdp_get_sap_by_name
**
** Description      Search SAP by service name
**
**
** Returns          SAP if success
**
*******************************************************************************/
uint8_t llcp_sdp_get_sap_by_name(char* p_name, uint8_t length) {
  uint8_t sap;
  tLLCP_APP_CB* p_app_cb;

  for (sap = LLCP_SAP_SDP; sap <= LLCP_UPPER_BOUND_SDP_SAP; sap++) {
    p_app_cb = llcp_util_get_app_cb(sap);

    if ((p_app_cb) && (p_app_cb->p_app_cback) &&
        (strlen((char*)p_app_cb->p_service_name) == length) &&
        (!strncmp((char*)p_app_cb->p_service_name, p_name, length))) {
      /* if device is under LLCP DTA testing */
      if (llcp_cb.p_dta_cback && (!strncmp((char*)p_app_cb->p_service_name,
                                           "urn:nfc:sn:cl-echo-in", length))) {
        llcp_cb.dta_snl_resp = true;
      }

      return (sap);
    }
  }
  return 0;
}

/*******************************************************************************
**
** Function         llcp_sdp_return_sap
**
** Description      Report TID and SAP to requester
**
**
** Returns          void
**
*******************************************************************************/
static void llcp_sdp_return_sap(uint8_t tid, uint8_t sap) {
  uint8_t i;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("tid=0x%x, SAP=0x%x", tid, sap);

  for (i = 0; i < LLCP_MAX_SDP_TRANSAC; i++) {
    if ((llcp_cb.sdp_cb.transac[i].p_cback) &&
        (llcp_cb.sdp_cb.transac[i].tid == tid)) {
      (*llcp_cb.sdp_cb.transac[i].p_cback)(tid, sap);

      llcp_cb.sdp_cb.transac[i].p_cback = nullptr;
    }
  }
}

/*******************************************************************************
**
** Function         llcp_sdp_proc_deactivation
**
** Description      Report SDP failure for any pending request because of
**                  deactivation
**
**
** Returns          void
**
*******************************************************************************/
void llcp_sdp_proc_deactivation(void) {
  uint8_t i;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  for (i = 0; i < LLCP_MAX_SDP_TRANSAC; i++) {
    if (llcp_cb.sdp_cb.transac[i].p_cback) {
      (*llcp_cb.sdp_cb.transac[i].p_cback)(llcp_cb.sdp_cb.transac[i].tid, 0x00);

      llcp_cb.sdp_cb.transac[i].p_cback = nullptr;
    }
  }

  /* free any pending SNL PDU */
  if (llcp_cb.sdp_cb.p_snl) {
    GKI_freebuf(llcp_cb.sdp_cb.p_snl);
    llcp_cb.sdp_cb.p_snl = nullptr;
  }

  llcp_cb.sdp_cb.next_tid = 0;
  llcp_cb.dta_snl_resp = false;
}

/*******************************************************************************
**
** Function         llcp_sdp_proc_snl
**
** Description      Process SDREQ and SDRES in SNL
**
**
** Returns          LLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_sdp_proc_snl(uint16_t sdu_length, uint8_t* p) {
  uint8_t type, length, tid, sap, *p_value;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if ((llcp_cb.lcb.agreed_major_version < LLCP_MIN_SNL_MAJOR_VERSION) ||
      ((llcp_cb.lcb.agreed_major_version == LLCP_MIN_SNL_MAJOR_VERSION) &&
       (llcp_cb.lcb.agreed_minor_version < LLCP_MIN_SNL_MINOR_VERSION))) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "version number less than 1.1, SNL not "
        "supported.");
    return LLCP_STATUS_FAIL;
  }
  while (sdu_length >= 2) /* at least type and length */
  {
    BE_STREAM_TO_UINT8(type, p);
    BE_STREAM_TO_UINT8(length, p);

    switch (type) {
      case LLCP_SDREQ_TYPE:
        if ((length > 1) /* TID and sevice name */
            &&
            (sdu_length >= 2 + length)) /* type, length, TID and service name */
        {
          p_value = p;
          BE_STREAM_TO_UINT8(tid, p_value);
          sap = llcp_sdp_get_sap_by_name((char*)p_value, (uint8_t)(length - 1));
          /* fix to pass TC_CTO_TAR_BI_03_x (x=5) test case
           * As per the LLCP test specification v1.2.00 by receiving erroneous
           * SNL PDU i'e with improper service name "urn:nfc:sn:dta-co-echo-in",
           * the IUT should not send any PDU except SYMM PDU */
          if (appl_dta_mode_flag == 1 && sap == 0x00) {
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "%s: In dta mode sap == 0x00 p_value = %s", __func__, p_value);
            if ((length - 1) == strlen((const char*)p_value)) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: Strings are not equal", __func__);
              llcp_sdp_send_sdres(tid, sap);
            }
          } else {
            llcp_sdp_send_sdres(tid, sap);
          }
        } else {
          /*For P2P in LLCP mode TC_CTO_TAR_BI_03_x(x=3) fix*/
          if (appl_dta_mode_flag == 1 &&
              ((nfa_dm_cb.eDtaMode & 0x0F) == NFA_DTA_LLCP_MODE)) {
            LOG(ERROR) << StringPrintf("%s: Calling llcp_sdp_send_sdres",
                                       __func__);
            tid = 0x01;
            sap = 0x00;
            llcp_sdp_send_sdres(tid, sap);
          }
          LOG(ERROR) << StringPrintf("bad length (%d) in LLCP_SDREQ_TYPE",
                                     length);
        }
        break;

      case LLCP_SDRES_TYPE:
        if ((length == LLCP_SDRES_LEN)     /* TID and SAP */
            && (sdu_length >= 2 + length)) /* type, length, TID and SAP */
        {
          p_value = p;
          BE_STREAM_TO_UINT8(tid, p_value);
          BE_STREAM_TO_UINT8(sap, p_value);
          llcp_sdp_return_sap(tid, sap);
        } else {
          LOG(ERROR) << StringPrintf("bad length (%d) in LLCP_SDRES_TYPE",
                                     length);
        }
        break;

      default:
        LOG(WARNING) << StringPrintf("Unknown type (0x%x) is ignored", type);
        break;
    }

    if (sdu_length >= 2 + length) /* type, length, value */
    {
      sdu_length -= 2 + length;
      p += length;
    } else {
      break;
    }
  }

  if (sdu_length) {
    LOG(ERROR) << StringPrintf("Bad format of SNL");
    return LLCP_STATUS_FAIL;
  } else {
    return LLCP_STATUS_SUCCESS;
  }
}
