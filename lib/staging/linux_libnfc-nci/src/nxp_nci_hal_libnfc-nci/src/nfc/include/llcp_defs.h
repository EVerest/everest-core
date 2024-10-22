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
 *  This file contains the LLCP definitions
 *
 ******************************************************************************/
#ifndef LLCP_DEFS_H
#define LLCP_DEFS_H

/*
** LLCP PDU Descriptions
*/

#define LLCP_PDU_HEADER_SIZE 2 /* DSAP:PTYPE:SSAP excluding Sequence */

#define LLCP_GET_DSAP(u16) (((u16)&0xFC00) >> 10)
#define LLCP_GET_PTYPE(u16) (((u16)&0x03C0) >> 6)
#define LLCP_GET_SSAP(u16) (((u16)&0x003F))

#define LLCP_GET_PDU_HEADER(dsap, ptype, ssap) \
  (((uint16_t)(dsap) << 10) | ((uint16_t)(ptype) << 6) | (uint16_t)(ssap))

#define LLCP_GET_NS(u8) ((uint8_t)(u8) >> 4)
#define LLCP_GET_NR(u8) ((uint8_t)(u8)&0x0F)
#define LLCP_GET_SEQUENCE(NS, NR) (((uint8_t)(NS) << 4) | (uint8_t)(NR))
#define LLCP_SEQUENCE_SIZE 1

#define LLCP_PDU_SYMM_TYPE 0
#define LLCP_PDU_PAX_TYPE 1
#define LLCP_PDU_AGF_TYPE 2
#define LLCP_PDU_UI_TYPE 3
#define LLCP_PDU_CONNECT_TYPE 4
#define LLCP_PDU_DISC_TYPE 5
#define LLCP_PDU_CC_TYPE 6
#define LLCP_PDU_DM_TYPE 7
#define LLCP_PDU_FRMR_TYPE 8
#define LLCP_PDU_SNL_TYPE 9
#define LLCP_PDU_I_TYPE 12
#define LLCP_PDU_RR_TYPE 13
#define LLCP_PDU_RNR_TYPE 14

#define LLCP_PDU_SYMM_SIZE 2
#define LLCP_PDU_DISC_SIZE 2
#define LLCP_PDU_DM_SIZE 3
#define LLCP_PDU_FRMR_SIZE 6
#define LLCP_PDU_RR_SIZE 3
#define LLCP_PDU_RNR_SIZE 3
#define LLCP_PDU_AGF_LEN_SIZE 2 /* 2 bytes of length in AGF PDU */

#define LLCP_SAP_DM_REASON_RESP_DISC 0x00
#define LLCP_SAP_DM_REASON_NO_ACTIVE_CONNECTION 0x01
#define LLCP_SAP_DM_REASON_NO_SERVICE 0x02
#define LLCP_SAP_DM_REASON_APP_REJECTED 0x03
#define LLCP_SAP_DM_REASON_PERM_REJECT_THIS 0x10
#define LLCP_SAP_DM_REASON_PERM_REJECT_ANY 0x11
#define LLCP_SAP_DM_REASON_TEMP_REJECT_THIS 0x20
#define LLCP_SAP_DM_REASON_TEMP_REJECT_ANY 0x21

#define LLCP_FRMR_W_ERROR_FLAG 0x08 /* Well-formedness Error */
#define LLCP_FRMR_I_ERROR_FLAG 0x04 /* Information Field Error */
#define LLCP_FRMR_R_ERROR_FLAG 0x02 /* Receive Sequence Error */
#define LLCP_FRMR_S_ERROR_FLAG 0x01 /* Send Sequence Error */

/*
** LLCP Parameter Descriptions
*/

/* Version */
#define LLCP_VERSION_TYPE 0x01
#define LLCP_VERSION_LEN 0x01
#define LLCP_GET_MAJOR_VERSION(u8) (((uint8_t)(u8) >> 4) & 0x0F)
#define LLCP_GET_MINOR_VERSION(u8) ((uint8_t)(u8)&0x0F)
#define LLCP_MIN_MAJOR_VERSION 0x01
#define LLCP_MIN_SNL_MAJOR_VERSION 0x01
#define LLCP_MIN_SNL_MINOR_VERSION 0x01

/* LLCP Version 1.2 */
#define LLCP_VERSION_MAJOR 0x01
#define LLCP_VERSION_MINOR 0x02
#define LLCP_VERSION_VALUE ((LLCP_VERSION_MAJOR << 4) | LLCP_VERSION_MINOR)

/* Maximum Information Unit Extension */
#define LLCP_MIUX_TYPE 0x02
#define LLCP_MIUX_LEN 0x02
#define LLCP_MIUX_MASK 0x07FF /* MIUX bit 10:0 */
#define LLCP_DEFAULT_MIU 128  /* if local LLC doesn't receive MIUX */
#define LLCP_MAX_MIU 2175     /* 2047 (11bits) + 128 */

/* Well-Known Service */
#define LLCP_WKS_TYPE 0x03
#define LLCP_WKS_LEN 0x02

/* Well-Known Service Bitmap */
#define LLCP_WKS_MASK_LM 0x0001  /* Link Management */

/* Well-Known Service Access Points */
#define LLCP_SAP_LM 0x00  /* Link Management */
#define LLCP_SAP_SDP 0x01 /* Service Discovery "urn:nfc:sn:sdp" */

/* Link Timeout, LTO */
#define LLCP_LTO_TYPE 0x04
#define LLCP_LTO_LEN 0x01
/* default 100ms. It should be sufficiently larget than RWT */
#define LLCP_DEFAULT_LTO_IN_MS 100
#define LLCP_LTO_UNIT 10        /* 10 ms */
#define LLCP_MAX_LTO_IN_MS 2550 /* 2550 ms; 8bits * 10ms */

/* Receive Window Size, RW */
#define LLCP_RW_TYPE 0x05
#define LLCP_RW_LEN 0x01
#define LLCP_DEFAULT_RW 1 /* if local LLC doesn't receive RW */

/* Service Name, SN */
#define LLCP_SN_TYPE 0x06

/* Option, OPT */
#define LLCP_OPT_TYPE 0x07
#define LLCP_OPT_LEN 0x01

/* Service Discovery Request, SDREQ */
#define LLCP_SDREQ_TYPE 0x08
/* type(1 byte), length(1 byte), TID(1 byte) */
#define LLCP_SDREQ_MIN_LEN 0x03

/* Service Discovery Response, SDRES */
#define LLCP_SDRES_TYPE 0x09
#define LLCP_SDRES_LEN 0x02

/* Link Service Class */
#define LLCP_LSC_UNKNOWN 0x00
#define LLCP_LSC_1 0x01
#define LLCP_LSC_2 0x02
#define LLCP_LSC_3 0x03

#define LLCP_MAGIC_NUMBER_LEN 3
#define LLCP_MAGIC_NUMBER_BYTE0 0x46
#define LLCP_MAGIC_NUMBER_BYTE1 0x66
#define LLCP_MAGIC_NUMBER_BYTE2 0x6D

#define LLCP_SEQ_MODULO 16

#define LLCP_NUM_SAPS 64
#define LLCP_UPPER_BOUND_WK_SAP 0x0F
#define LLCP_LOWER_BOUND_SDP_SAP 0x10
#define LLCP_UPPER_BOUND_SDP_SAP 0x1F
#define LLCP_LOWER_BOUND_LOCAL_SAP 0x20
#define LLCP_UPPER_BOUND_LOCAL_SAP 0x3F

/* Max Payload */
/* Maximum Payload size, Length Reduction LRi/LRt */
#define LLCP_NCI_MAX_PAYL_SIZE 254

#define LLCP_MAX_GEN_BYTES 48

#endif /* LLCP_DEFS_H */
