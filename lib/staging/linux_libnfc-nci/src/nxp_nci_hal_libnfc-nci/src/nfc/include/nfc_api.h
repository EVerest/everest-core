/******************************************************************************
 *
 *  Copyright (C) 2009-2014 Broadcom Corporation
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
 *  This file contains the Near Field Communication (NFC) API function
 *  external definitions.
 *
 ******************************************************************************/

#ifndef NFC_API_H
#define NFC_API_H

#include "gki.h"
#include "nci_defs.h"
#include "nfc_hal_api.h"
#include "nfc_target.h"
#include "nfc_brcm_defs.h"
#include "vendor_cfg.h"

/* NFC application return status codes */
/* Command succeeded    */
#define NFC_STATUS_OK NCI_STATUS_OK
/* Command is rejected. */
#define NFC_STATUS_REJECTED NCI_STATUS_REJECTED
/* Message is corrupted */
#define NFC_STATUS_MSG_CORRUPTED NCI_STATUS_MESSAGE_CORRUPTED
/* buffer full          */
#define NFC_STATUS_BUFFER_FULL NCI_STATUS_BUFFER_FULL
/* failed               */
#define NFC_STATUS_FAILED NCI_STATUS_FAILED
/* not initialized      */
#define NFC_STATUS_NOT_INITIALIZED NCI_STATUS_NOT_INITIALIZED
/* Syntax error         */
#define NFC_STATUS_SYNTAX_ERROR NCI_STATUS_SYNTAX_ERROR
/* Semantic error       */
#define NFC_STATUS_SEMANTIC_ERROR NCI_STATUS_SEMANTIC_ERROR
/* Unknown NCI Group ID */
#define NFC_STATUS_UNKNOWN_GID NCI_STATUS_UNKNOWN_GID
/* Unknown NCI Opcode   */
#define NFC_STATUS_UNKNOWN_OID NCI_STATUS_UNKNOWN_OID
/* Invalid Parameter    */
#define NFC_STATUS_INVALID_PARAM NCI_STATUS_INVALID_PARAM
/* Message size too big */
#define NFC_STATUS_MSG_SIZE_TOO_BIG NCI_STATUS_MSG_SIZE_TOO_BIG
/* Already started      */
#define NFC_STATUS_ALREADY_STARTED NCI_STATUS_ALREADY_STARTED
/* Activation Failed    */
#define NFC_STATUS_ACTIVATION_FAILED NCI_STATUS_ACTIVATION_FAILED
/* Tear Down Error      */
#define NFC_STATUS_TEAR_DOWN NCI_STATUS_TEAR_DOWN
/* RF transmission error*/
#define NFC_STATUS_RF_TRANSMISSION_ERR NCI_STATUS_RF_TRANSMISSION_ERR
/* RF protocol error    */
#define NFC_STATUS_RF_PROTOCOL_ERR NCI_STATUS_RF_PROTOCOL_ERR
/* RF Timeout           */
#define NFC_STATUS_TIMEOUT NCI_STATUS_TIMEOUT
/* EE Intf activate err */
#define NFC_STATUS_EE_INTF_ACTIVE_FAIL NCI_STATUS_EE_INTF_ACTIVE_FAIL
/* EE transmission error*/
#define NFC_STATUS_EE_TRANSMISSION_ERR NCI_STATUS_EE_TRANSMISSION_ERR
/* EE protocol error    */
#define NFC_STATUS_EE_PROTOCOL_ERR NCI_STATUS_EE_PROTOCOL_ERR
/* EE Timeout           */
#define NFC_STATUS_EE_TIMEOUT NCI_STATUS_EE_TIMEOUT
#if (NXP_EXTNS == TRUE)
/**********************************************
 * NFC Config Parameter IDs defined by NXP NFC
 **********************************************/
#define NXP_NFC_SET_CONFIG_PARAM_EXT \
  ((unsigned char)0xA0) /* NXP NFC set config extension ID 0*/
#define NXP_NFC_SET_CONFIG_PARAM_EXT_ID1 \
  ((unsigned char)0xA1) /* NXP NFC set config extension ID 1*/
#endif
/* 0xE0 ~0xFF are proprietary status codes */
/* Command started successfully                     */
#define NFC_STATUS_CMD_STARTED 0xE3
/* NFCC Timeout in responding to an NCI command     */
#define NFC_STATUS_HW_TIMEOUT 0xE4
/* More (same) event to follow                      */
#define NFC_STATUS_CONTINUE 0xE5
/* API is called to perform illegal function        */
#define NFC_STATUS_REFUSED 0xE6
/* Wrong format of R-APDU, CC file or NDEF file     */
#define NFC_STATUS_BAD_RESP 0xE7
/* 7816 Status Word is not command complete(0x9000) */
#define NFC_STATUS_CMD_NOT_CMPLTD 0xE8
/* Out of GKI buffers                               */
#define NFC_STATUS_NO_BUFFERS 0xE9
/* Protocol mismatch between API and activated one  */
#define NFC_STATUS_WRONG_PROTOCOL 0xEA
/* Another Tag command is already in progress       */
#define NFC_STATUS_BUSY 0xEB

/* Link Loss                  */
#define NFC_STATUS_LINK_LOSS 0xFC
/* data len exceeds MIU       */
#define NFC_STATUS_BAD_LENGTH 0xFD
/* invalid handle             */
#define NFC_STATUS_BAD_HANDLE 0xFE
/* congested                  */
#define NFC_STATUS_CONGESTED 0xFF
typedef uint8_t tNFC_STATUS;

/**********************************************
 * NFC Config Parameter IDs defined by NCI
 **********************************************/
#define NFC_PMID_TOTAL_DURATION NCI_PARAM_ID_TOTAL_DURATION
#define NFC_PMID_PF_RC NCI_PARAM_ID_PF_RC
#define NFC_PMID_ATR_REQ_GEN_BYTES NCI_PARAM_ID_ATR_REQ_GEN_BYTES
#define NFC_PMID_LA_HIST_BY NCI_PARAM_ID_LA_HIST_BY
#define NFC_PMID_LA_NFCID1 NCI_PARAM_ID_LA_NFCID1
#define NFC_PMID_LA_BIT_FRAME_SDD NCI_PARAM_ID_LA_BIT_FRAME_SDD
#define NFC_PMID_LA_PLATFORM_CONFIG NCI_PARAM_ID_LA_PLATFORM_CONFIG
#define NFC_PMID_LA_SEL_INFO NCI_PARAM_ID_LA_SEL_INFO
#define NFC_PMID_LB_SENSB_INFO NCI_PARAM_ID_LB_SENSB_INFO
#define NFC_PMID_LB_H_INFO NCI_PARAM_ID_LB_H_INFO_RSP
#define NFC_PMID_LB_NFCID0 NCI_PARAM_ID_LB_NFCID0
#define NFC_PMID_LB_APPDATA NCI_PARAM_ID_LB_APPDATA
#define NFC_PMID_LB_SFGI NCI_PARAM_ID_LB_SFGI
#define NFC_PMID_LB_ADC_FO NCI_PARAM_ID_LB_ADC_FO
#define NFC_PMID_LF_T3T_ID1 NCI_PARAM_ID_LF_T3T_ID1
#define NFC_PMID_LF_PROTOCOL NCI_PARAM_ID_LF_PROTOCOL
#define NFC_PMID_LF_T3T_PMM NCI_PARAM_ID_LF_T3T_PMM
#define NFC_PMID_LF_T3T_FLAGS2 NCI_PARAM_ID_LF_T3T_FLAGS2
#define NFC_PMID_FWI NCI_PARAM_ID_FWI
#define NFC_PMID_LF_CON_BITR_F NCI_PARAM_ID_LF_CON_BITR_F
#define NFC_PMID_WT NCI_PARAM_ID_WT
#define NFC_PMID_ATR_RES_GEN_BYTES NCI_PARAM_ID_ATR_RES_GEN_BYTES
#define NFC_PMID_ATR_RSP_CONFIG NCI_PARAM_ID_ATR_RSP_CONFIG
#define NFC_PMID_RF_FIELD_INFO NCI_PARAM_ID_RF_FIELD_INFO

/* Technology based routing  */
#define NFC_ROUTE_TAG_TECH NCI_ROUTE_TAG_TECH
/* Protocol based routing  */
#define NFC_ROUTE_TAG_PROTO NCI_ROUTE_TAG_PROTO
#define NFC_ROUTE_TAG_AID NCI_ROUTE_TAG_AID /* AID routing */
#define NFC_ROUTE_TAG_SYSCODE NCI_ROUTE_TAG_SYSCODE /* System Code routing*/
/* tag, len, 2 byte value for technology/protocol based routing */

/* For routing */
#define NFC_DH_ID NCI_DH_ID /* for DH */
/* To identify the loopback test */
/* use a proprietary range */
#define NFC_TEST_ID NCI_TEST_ID

#define NFC_TL_SIZE 2
#define NFC_SAVED_CMD_SIZE 2

typedef tNCI_DISCOVER_MAPS tNFC_DISCOVER_MAPS;
typedef tNCI_DISCOVER_PARAMS tNFC_DISCOVER_PARAMS;

/* all NFC Manager Callback functions have prototype like void (cback) (uint8_t
 * event, void *p_data)
 * tNFC_DATA_CBACK uses connection id as the first parameter; range 0x00-0x0F.
 * tNFC_DISCOVER_CBACK uses tNFC_DISCOVER_EVT; range  0x4000 ~
 * tNFC_RESPONSE_CBACK uses tNFC_RESPONSE_EVT; range  0x5000 ~
 */

#define NFC_FIRST_DEVT 0x4000
#define NFC_FIRST_REVT 0x5000
#define NFC_FIRST_CEVT 0x6000
#define NFC_FIRST_TEVT 0x8000

/* the events reported on tNFC_RESPONSE_CBACK */
enum {
  NFC_ENABLE_REVT = NFC_FIRST_REVT, /* 0  Enable event                  */
  NFC_DISABLE_REVT,                 /* 1  Disable event                 */
  NFC_SET_CONFIG_REVT,              /* 2  Set Config Response           */
  NFC_GET_CONFIG_REVT,              /* 3  Get Config Response           */
  NFC_NFCEE_DISCOVER_REVT,          /* 4  Discover NFCEE response       */
  NFC_NFCEE_INFO_REVT,              /* 5  Discover NFCEE Notification   */
  NFC_NFCEE_MODE_SET_REVT,          /* 6  NFCEE Mode Set response       */
  NFC_RF_FIELD_REVT,                /* 7  RF Field information          */
  NFC_EE_ACTION_REVT,               /* 8  EE Action notification        */
  NFC_EE_DISCOVER_REQ_REVT,         /* 9  EE Discover Req notification  */
  NFC_SET_ROUTING_REVT,             /* 10 Configure Routing response    */
  NFC_GET_ROUTING_REVT,             /* 11 Retrieve Routing response     */
  NFC_RF_COMM_PARAMS_UPDATE_REVT,   /* 12 RF Communication Param Update */
  NFC_GEN_ERROR_REVT,               /* 13 generic error notification    */
  NFC_NFCC_RESTART_REVT,            /* 14 NFCC has been re-initialized  */
  NFC_NFCC_TIMEOUT_REVT,            /* 15 NFCC is not responding        */
  NFC_NFCC_TRANSPORT_ERR_REVT,      /* 16 NCI Tranport error            */
  NFC_NFCC_POWER_OFF_REVT,          /* 17 NFCC turned off               */
  NFC_SET_POWER_SUB_STATE_REVT,     /* 18 Set power sub state response  */
  NFC_NFCEE_PL_CONTROL_REVT,        /* NFCEE Power/Link Ctrl response*/
  NFC_NFCEE_STATUS_REVT             /* NFCEE Status Notification     */
                                    /* First vendor-specific rsp event  */
};
typedef uint16_t tNFC_RESPONSE_EVT;

enum {
  NFC_CONN_CREATE_CEVT = NFC_FIRST_CEVT, /* 0  Conn Create Response          */
  NFC_CONN_CLOSE_CEVT,                   /* 1  Conn Close Response           */
  NFC_DEACTIVATE_CEVT,                   /* 2  Deactivate response/notificatn*/
  NFC_DATA_CEVT,                         /* 3  Data                          */
  NFC_ERROR_CEVT,                        /* 4  generic or interface error    */
  NFC_DATA_START_CEVT /* 5  received the first fragment on RF link */
};
typedef uint16_t tNFC_CONN_EVT;

#define NFC_NFCC_INFO_LEN 4
#ifndef NFC_NFCC_MAX_NUM_VS_INTERFACE
#define NFC_NFCC_MAX_NUM_VS_INTERFACE 5
#endif
typedef struct {
  tNFC_STATUS status;                   /* The event status.                */
  uint8_t nci_version;                  /* the NCI version of NFCC          */
  uint8_t max_conn;                     /* max number of connections by NFCC*/
  uint32_t nci_features;                /* the NCI features of NFCC         */
  uint16_t nci_interfaces;              /* the NCI interfaces of NFCC       */
  uint16_t max_ce_table;                /* the max routing table size       */
  uint16_t max_param_size;              /* Max Size for Large Parameters    */
  uint8_t manufacture_id;               /* the Manufacture ID for NFCC      */
  uint8_t nfcc_info[NFC_NFCC_INFO_LEN]; /* the Manufacture Info for NFCC      */
  uint8_t vs_interface
      [NFC_NFCC_MAX_NUM_VS_INTERFACE]; /* the NCI VS interfaces of NFCC    */
  uint8_t hci_packet_size;             /*HCI payload size*/
  uint8_t hci_conn_credits;            /*max number of HCI credits*/
  uint16_t max_nfc_v_size;             /* maximum frame size for NFC-V*/
} tNFC_ENABLE_REVT;

#define NFC_MAX_NUM_IDS 125
/* the data type associated with NFC_SET_CONFIG_REVT */
typedef struct {
  tNFC_STATUS status;                 /* The event status.                */
  uint8_t num_param_id;               /* Number of rejected NCI Param ID  */
  uint8_t param_ids[NFC_MAX_NUM_IDS]; /* NCI Param ID          */
} tNFC_SET_CONFIG_REVT;

/* the data type associated with NFC_GET_CONFIG_REVT */
typedef struct {
  tNFC_STATUS status;    /* The event status.    */
  uint16_t tlv_size;     /* The length of TLV    */
  uint8_t* p_param_tlvs; /* TLV                  */
} tNFC_GET_CONFIG_REVT;

/* the data type associated with NFC_NFCEE_DISCOVER_REVT */
typedef struct {
  tNFC_STATUS status; /* The event status.    */
  uint8_t num_nfcee;  /* The number of NFCEE  */
} tNFC_NFCEE_DISCOVER_REVT;

#define NFC_NFCEE_INTERFACE_APDU NCI_NFCEE_INTERFACE_APDU
#define NFC_NFCEE_INTERFACE_HCI_ACCESS NCI_NFCEE_INTERFACE_HCI_ACCESS
#define NFC_NFCEE_INTERFACE_T3T NCI_NFCEE_INTERFACE_T3T
#define NFC_NFCEE_INTERFACE_TRANSPARENT NCI_NFCEE_INTERFACE_TRANSPARENT
#define NFC_NFCEE_INTERFACE_PROPRIETARY NCI_NFCEE_INTERFACE_PROPRIETARY

#define NFC_NFCEE_TAG_HW_ID NCI_NFCEE_TAG_HW_ID
#define NFC_NFCEE_TAG_ATR_BYTES NCI_NFCEE_TAG_ATR_BYTES
#define NFC_NFCEE_TAG_T3T_INFO NCI_NFCEE_TAG_T3T_INFO
#define NFC_NFCEE_TAG_HCI_HOST_ID NCI_NFCEE_TAG_HCI_HOST_ID
typedef uint8_t tNFC_NFCEE_TAG;
/* additional NFCEE Info */
typedef struct {
  tNFC_NFCEE_TAG tag;
  uint8_t len;
  uint8_t info[NFC_MAX_EE_INFO];
} tNFC_NFCEE_TLV;

/* NFCEE unrecoverable error */
#define NFC_NFCEE_STATUS_UNRECOVERABLE_ERROR NCI_NFCEE_STS_UNRECOVERABLE_ERROR
/* NFCEE connected and inactive */
#define NFC_NFCEE_STATUS_INACTIVE NCI_NFCEE_STS_CONN_INACTIVE
/* NFCEE connected and active   */
#define NFC_NFCEE_STATUS_ACTIVE NCI_NFCEE_STS_CONN_ACTIVE
/* NFCEE removed                */
#define NFC_NFCEE_STATUS_REMOVED NCI_NFCEE_STS_REMOVED

/* the data type associated with NFC_NFCEE_INFO_REVT */
typedef struct {
  tNFC_STATUS status;    /* The event status - place holder  */
  uint8_t nfcee_id;      /* NFCEE ID                         */
  uint8_t ee_status;     /* The NFCEE status.                */
  uint8_t num_interface; /* number of NFCEE interfaces       */
  uint8_t ee_interface[NFC_MAX_EE_INTERFACE]; /* NFCEE interface       */
  uint8_t num_tlvs;                       /* number of TLVs                   */
  tNFC_NFCEE_TLV ee_tlv[NFC_MAX_EE_TLVS]; /* The TLVs associated with NFCEE   */
  bool nfcee_power_ctrl; /* 1, if NFCC has control of NFCEE Power Supply */
} tNFC_NFCEE_INFO_REVT;

#define NFC_MODE_ACTIVATE NCI_NFCEE_MD_ACTIVATE
#define NFC_MODE_DEACTIVATE NCI_NFCEE_MD_DEACTIVATE
typedef uint8_t tNFC_NFCEE_MODE;
/* the data type associated with NFC_NFCEE_MODE_SET_REVT */
typedef struct {
  tNFC_STATUS status;   /* The event status.*/
  uint8_t nfcee_id;     /* NFCEE ID         */
  tNFC_NFCEE_MODE mode; /* NFCEE mode       */
} tNFC_NFCEE_MODE_SET_REVT;

#if (APPL_DTA_MODE == TRUE)
/* This data type is for FW Version */
typedef struct {
  uint8_t rom_code_version; /* ROM code Version  */
  uint8_t major_version;    /* Major Version */
  uint8_t minor_version;    /* Minor Version  */
} tNFC_FW_VERSION;
#endif
#define NFC_MAX_AID_LEN NCI_MAX_AID_LEN /* 16 */

/* the data type associated with NFC_CE_GET_ROUTING_REVT */
typedef struct {
  tNFC_STATUS status; /* The event status                 */
  uint8_t nfcee_id;   /* NFCEE ID                         */
  uint8_t num_tlvs;   /* number of TLVs                   */
  uint8_t tlv_size;   /* the total len of all TLVs        */
  uint8_t param_tlvs[NFC_MAX_EE_TLV_SIZE]; /* the TLVs         */
} tNFC_GET_ROUTING_REVT;

/* the data type associated with NFC_CONN_CREATE_CEVT */
typedef struct {
  tNFC_STATUS status; /* The event status                 */
  uint8_t dest_type;  /* the destination type             */
  uint8_t id;         /* NFCEE ID  or RF Discovery ID     */
  uint8_t buff_size;  /* The max buffer size              */
  uint8_t num_buffs;  /* The number of buffers            */
} tNFC_CONN_CREATE_CEVT;

/* the data type associated with NFC_CONN_CLOSE_CEVT */
typedef struct {
  tNFC_STATUS status; /* The event status                 */
} tNFC_CONN_CLOSE_CEVT;

/* the data type associated with NFC_DATA_CEVT */
typedef struct {
  tNFC_STATUS status; /* The event status                 */
  NFC_HDR* p_data;    /* The received Data                */
} tNFC_DATA_CEVT;

/* the data type associated with NFC_NFCEE_PL_CONTROL_REVT */
typedef struct {
  tNFC_STATUS status;              /* The event status */
  uint8_t nfcee_id;                /* NFCEE ID */
  tNCI_NFCEE_PL_CONFIG pl_control; /* Power/Link Control command */
} tNFC_NFCEE_PL_CONTROL_REVT;

/* the data type associated with NFC_NFCEE_STATUS_REVT */
typedef struct {
  tNFC_STATUS status;              /* The event status */
  uint8_t nfcee_id;                /* NFCEE ID */
  tNCI_EE_NTF_STATUS nfcee_status; /* NFCEE status */
} tNFC_NFCEE_STATUS_REVT;
/* RF Field Status */
typedef uint8_t tNFC_RF_STS;

/* RF Field Technologies */
#define NFC_RF_TECHNOLOGY_A NCI_RF_TECHNOLOGY_A
#define NFC_RF_TECHNOLOGY_B NCI_RF_TECHNOLOGY_B
#define NFC_RF_TECHNOLOGY_F NCI_RF_TECHNOLOGY_F
typedef uint8_t tNFC_RF_TECH;

extern uint8_t NFC_GetNCIVersion();

#define NFC_PROTOCOL_15693      NCI_PROTOCOL_15693
/* Supported Protocols */
#define NFC_PROTOCOL_UNKNOWN NCI_PROTOCOL_UNKNOWN /* Unknown */
/* Type1Tag    - NFC-A            */
#define NFC_PROTOCOL_T1T NCI_PROTOCOL_T1T
/* Type2Tag    - NFC-A            */
#define NFC_PROTOCOL_T2T NCI_PROTOCOL_T2T
/* Type3Tag    - NFC-F            */
#define NFC_PROTOCOL_T3T NCI_PROTOCOL_T3T
/* Type5Tag    - NFC-V/ISO15693*/
#define NFC_PROTOCOL_T5T NFC_PROTOCOL_T5T_(NFC_GetNCIVersion())
#define NFC_PROTOCOL_T5T_(x) \
  (((x) == NCI_VERSION_2_0) ? NCI_PROTOCOL_T5T : NCI_PROTOCOL_15693)
/* Type 4A,4B  - NFC-A or NFC-B   */
#define NFC_PROTOCOL_ISO_DEP NCI_PROTOCOL_ISO_DEP
/* NFCDEP/LLCP - NFC-A or NFC-F       */
#define NFC_PROTOCOL_NFC_DEP NCI_PROTOCOL_NFC_DEP
#define NFC_PROTOCOL_MIFARE NCI_PROTOCOL_MIFARE
#define NFC_PROTOCOL_ISO15693 NCI_PROTOCOL_15693

 #define NFC_PROTOCOL_T3BT       NCI_PROTOCOL_T3BT
#define NFC_PROTOCOL_B_PRIME NCI_PROTOCOL_B_PRIME
#define NFC_PROTOCOL_KOVIO NCI_PROTOCOL_KOVIO
typedef uint8_t tNFC_PROTOCOL;

/* Discovery Types/Detected Technology and Mode */
#define NFC_DISCOVERY_TYPE_POLL_A NCI_DISCOVERY_TYPE_POLL_A
#define NFC_DISCOVERY_TYPE_POLL_B NCI_DISCOVERY_TYPE_POLL_B
#define NFC_DISCOVERY_TYPE_POLL_F NCI_DISCOVERY_TYPE_POLL_F
#define NFC_DISCOVERY_TYPE_POLL_A_ACTIVE NCI_DISCOVERY_TYPE_POLL_A_ACTIVE
#define NFC_DISCOVERY_TYPE_POLL_F_ACTIVE NCI_DISCOVERY_TYPE_POLL_F_ACTIVE
#define NFC_DISCOVERY_TYPE_POLL_ACTIVE NCI_DISCOVERY_TYPE_POLL_ACTIVE
#define NFC_DISCOVERY_TYPE_POLL_V NCI_DISCOVERY_TYPE_POLL_V
#define NFC_DISCOVERY_TYPE_POLL_B_PRIME NCI_DISCOVERY_TYPE_POLL_B_PRIME
#define NFC_DISCOVERY_TYPE_POLL_KOVIO NCI_DISCOVERY_TYPE_POLL_KOVIO
#define NFC_DISCOVERY_TYPE_LISTEN_A NCI_DISCOVERY_TYPE_LISTEN_A
#define NFC_DISCOVERY_TYPE_LISTEN_B NCI_DISCOVERY_TYPE_LISTEN_B
#define NFC_DISCOVERY_TYPE_LISTEN_F NCI_DISCOVERY_TYPE_LISTEN_F
#define NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE
#define NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE
#define NFC_DISCOVERY_TYPE_LISTEN_ACTIVE NCI_DISCOVERY_TYPE_LISTEN_ACTIVE
#define NFC_DISCOVERY_TYPE_LISTEN_ISO15693 NCI_DISCOVERY_TYPE_LISTEN_ISO15693
#define NFC_DISCOVERY_TYPE_LISTEN_B_PRIME NCI_DISCOVERY_TYPE_LISTEN_B_PRIME
typedef uint8_t tNFC_DISCOVERY_TYPE;
typedef uint8_t tNFC_RF_TECH_N_MODE;

/* Select Response codes */
#define NFC_SEL_RES_NFC_FORUM_T2T 0x00
#define NFC_SEL_RES_MF_CLASSIC 0x08

/* Bit Rates */
#define NFC_BIT_RATE_212 NCI_BIT_RATE_212   /* 212 kbit/s */
#define NFC_BIT_RATE_424 NCI_BIT_RATE_424   /* 424 kbit/s */
typedef uint8_t tNFC_BIT_RATE;

/**********************************************
 * Interface Types
 **********************************************/
#define NFC_INTERFACE_EE_DIRECT_RF NCI_INTERFACE_EE_DIRECT_RF
#define NFC_INTERFACE_FRAME NCI_INTERFACE_FRAME
#define NFC_INTERFACE_ISO_DEP NCI_INTERFACE_ISO_DEP
#define NFC_INTERFACE_NFC_DEP NCI_INTERFACE_NFC_DEP
#define NFC_INTERFACE_MIFARE NCI_INTERFACE_VS_MIFARE
typedef tNCI_INTF_TYPE tNFC_INTF_TYPE;

/**********************************************
 *  Deactivation Type
 **********************************************/
#define NFC_DEACTIVATE_TYPE_IDLE NCI_DEACTIVATE_TYPE_IDLE
#define NFC_DEACTIVATE_TYPE_SLEEP NCI_DEACTIVATE_TYPE_SLEEP
#define NFC_DEACTIVATE_TYPE_SLEEP_AF NCI_DEACTIVATE_TYPE_SLEEP_AF
#define NFC_DEACTIVATE_TYPE_DISCOVERY NCI_DEACTIVATE_TYPE_DISCOVERY
typedef uint8_t tNFC_DEACT_TYPE;

/**********************************************
 *  Deactivation Reasons
 **********************************************/
#define NFC_DEACTIVATE_REASON_DH_REQ_FAILED NCI_DEACTIVATE_REASON_DH_REQ_FAILED
#define NFC_DEACTIVATE_REASON_DH_REQ NCI_DEACTIVATE_REASON_DH_REQ
typedef uint8_t tNFC_DEACT_REASON;

/* the data type associated with NFC_RF_FIELD_REVT */
typedef struct {
  tNFC_STATUS status;   /* The event status - place holder. */
  tNFC_RF_STS rf_field; /* RF Field Status                  */
} tNFC_RF_FIELD_REVT;

#define NFC_MAX_APP_DATA_LEN 40
typedef struct {
  uint8_t len_aid;              /* length of application id */
  uint8_t aid[NFC_MAX_AID_LEN]; /* application id           */
} tNFC_AID;
typedef struct {
  uint8_t len_aid;                    /* length of application id */
  uint8_t aid[NFC_MAX_AID_LEN];       /* application id           */
  uint8_t len_data;                   /* len of application data  */
  uint8_t data[NFC_MAX_APP_DATA_LEN]; /* application data    */
} tNFC_APP_INIT;

/* ISO 7816-4 SELECT command */
#define NFC_EE_TRIG_SELECT NCI_EE_TRIG_7816_SELECT
/* RF Protocol changed       */
#define NFC_EE_TRIG_RF_PROTOCOL NCI_EE_TRIG_RF_PROTOCOL
/* RF Technology changed     */
#define NFC_EE_TRIG_RF_TECHNOLOGY NCI_EE_TRIG_RF_TECHNOLOGY
/* Application initiation    */
#define NFC_EE_TRIG_APP_INIT NCI_EE_TRIG_APP_INIT
typedef uint8_t tNFC_EE_TRIGGER;
typedef struct {
  tNFC_EE_TRIGGER trigger; /* the trigger of this event        */
  union {
    tNFC_PROTOCOL protocol;
    tNFC_RF_TECH technology;
    tNFC_AID aid;
    tNFC_APP_INIT app_init;
  } param; /* Discovery Type specific parameters */
} tNFC_ACTION_DATA;

/* the data type associated with NFC_EE_ACTION_REVT */
typedef struct {
  tNFC_STATUS status;        /* The event status - place holder  */
  uint8_t nfcee_id;          /* NFCEE ID                         */
  tNFC_ACTION_DATA act_data; /* data associated /w the action    */
} tNFC_EE_ACTION_REVT;

#define NFC_EE_DISC_OP_ADD 0
typedef uint8_t tNFC_EE_DISC_OP;
typedef struct {
  tNFC_EE_DISC_OP op;              /* add or remove this entry         */
  uint8_t nfcee_id;                /* NFCEE ID                         */
  tNFC_RF_TECH_N_MODE tech_n_mode; /* Discovery Technology and Mode    */
  tNFC_PROTOCOL protocol;          /* NFC protocol                     */
} tNFC_EE_DISCOVER_INFO;

#ifndef NFC_MAX_EE_DISC_ENTRIES
#define NFC_MAX_EE_DISC_ENTRIES 6
#endif
/* T, L, V(NFCEE ID, TechnMode, Protocol) */
#define NFC_EE_DISCOVER_ENTRY_LEN 5
#define NFC_EE_DISCOVER_INFO_LEN 3 /* NFCEE ID, TechnMode, Protocol */
/* the data type associated with NFC_EE_DISCOVER_REQ_REVT */
typedef struct {
  tNFC_STATUS status; /* The event status - place holder  */
  uint8_t num_info;   /* number of entries in info[]      */
  tNFC_EE_DISCOVER_INFO
      info[NFC_MAX_EE_DISC_ENTRIES]; /* discovery request from NFCEE */
} tNFC_EE_DISCOVER_REQ_REVT;

typedef union {
  tNFC_STATUS status; /* The event status. */
  tNFC_ENABLE_REVT enable;
  tNFC_SET_CONFIG_REVT set_config;
  tNFC_GET_CONFIG_REVT get_config;
  tNFC_NFCEE_DISCOVER_REVT nfcee_discover;
  tNFC_NFCEE_INFO_REVT nfcee_info;
  tNFC_NFCEE_MODE_SET_REVT mode_set;
  tNFC_NFCEE_PL_CONTROL_REVT pl_control;
  tNFC_NFCEE_STATUS_REVT nfcee_status;
  tNFC_RF_FIELD_REVT rf_field;
  tNFC_STATUS cfg_routing;
  tNFC_GET_ROUTING_REVT get_routing;
  tNFC_EE_ACTION_REVT ee_action;
  tNFC_EE_DISCOVER_REQ_REVT ee_discover_req;
  void* p_vs_evt_data;
} tNFC_RESPONSE;

/*************************************
**  RESPONSE Callback Functions
**************************************/
typedef void(tNFC_RESPONSE_CBACK)(tNFC_RESPONSE_EVT event,
                                  tNFC_RESPONSE* p_data);

/* The events reported on tNFC_VS_CBACK */
/* The event is (NCI_RSP_BIT|oid) for response and (NCI_NTF_BIT|oid) for
 * notification*/

typedef uint8_t tNFC_VS_EVT;

/*************************************
**  Proprietary (Vendor Specific) Callback Functions
**************************************/
typedef void(tNFC_VS_CBACK)(tNFC_VS_EVT event, uint16_t data_len,
                            uint8_t* p_data);

/* the events reported on tNFC_DISCOVER_CBACK */
enum {
  NFC_START_DEVT = NFC_FIRST_DEVT, /* Status of NFC_DiscoveryStart     */
  NFC_MAP_DEVT,                    /* Status of NFC_DiscoveryMap       */
  NFC_RESULT_DEVT,                 /* The responses from remote device */
  NFC_SELECT_DEVT,                 /* Status of NFC_DiscoverySelect    */
  NFC_ACTIVATE_DEVT,               /* RF interface is activated        */
  NFC_DEACTIVATE_DEVT              /* Status of RF deactivation        */
};
typedef uint16_t tNFC_DISCOVER_EVT;

/* the data type associated with NFC_START_DEVT */
typedef tNFC_STATUS tNFC_START_DEVT;

typedef tNCI_RF_PA_PARAMS tNFC_RF_PA_PARAMS;
#define NFC_MAX_SENSB_RES_LEN NCI_MAX_SENSB_RES_LEN
#define NFC_NFCID0_MAX_LEN 4
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  #define NFC_PUPIID_MAX_LEN          8
#endif
typedef struct {
  uint8_t sensb_res_len; /* Length of SENSB_RES Response (Byte 2 - Byte 12 or
                            13) Available after Technology Detection */
  uint8_t sensb_res[NFC_MAX_SENSB_RES_LEN]; /* SENSB_RES Response (ATQ) */
  uint8_t nfcid0[NFC_NFCID0_MAX_LEN];
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  uint8_t       pupiid_len;
  uint8_t       pupiid[NFC_PUPIID_MAX_LEN];
#endif


} tNFC_RF_PB_PARAMS;

#define NFC_MAX_SENSF_RES_LEN NCI_MAX_SENSF_RES_LEN
#define NFC_NFCID2_LEN NCI_NFCID2_LEN
typedef struct {
  uint8_t bit_rate;      /* NFC_BIT_RATE_212 or NFC_BIT_RATE_424 */
  uint8_t sensf_res_len; /* Length of SENSF_RES Response (Byte 2 - Byte 17 or
                            19) Available after Technology Detection */
  uint8_t sensf_res[NFC_MAX_SENSF_RES_LEN]; /* SENSB_RES Response */
  uint8_t nfcid2[NFC_NFCID2_LEN]; /* NFCID2 generated by the Local NFCC for
                                     NFC-DEP Protocol.Available for Frame
                                     Interface  */
  uint8_t mrti_check;
  uint8_t mrti_update;
} tNFC_RF_PF_PARAMS;

typedef tNCI_RF_LF_PARAMS tNFC_RF_LF_PARAMS;

#define NFC_ISO15693_UID_LEN 8
typedef struct {
  uint8_t flag;
  uint8_t dsfid;
  uint8_t uid[NFC_ISO15693_UID_LEN];
} tNFC_RF_PISO15693_PARAMS;

#ifndef NFC_KOVIO_MAX_LEN
#define NFC_KOVIO_MAX_LEN 32
#endif
typedef struct {
  uint8_t uid_len;
  uint8_t uid[NFC_KOVIO_MAX_LEN];
} tNFC_RF_PKOVIO_PARAMS;

typedef tNCI_RF_ACM_P_PARAMS tNFC_RF_ACM_P_PARAMS;

typedef union {
  tNFC_RF_PA_PARAMS pa;
  tNFC_RF_PB_PARAMS pb;
  tNFC_RF_PF_PARAMS pf;
  tNFC_RF_LF_PARAMS lf;
  tNFC_RF_PISO15693_PARAMS pi93;
  tNFC_RF_PKOVIO_PARAMS pk;
  tNFC_RF_ACM_P_PARAMS acm_p;
} tNFC_RF_TECH_PARAMU;

typedef struct {
  tNFC_DISCOVERY_TYPE mode;
  tNFC_RF_TECH_PARAMU param;
} tNFC_RF_TECH_PARAMS;

/* the data type associated with NFC_RESULT_DEVT */
typedef struct {
  tNFC_STATUS status;                /* The event status - place holder.  */
  uint8_t rf_disc_id;                /* RF Discovery ID                   */
  uint8_t protocol;                  /* supported protocol                */
  tNFC_RF_TECH_PARAMS rf_tech_param; /* RF technology parameters          */
  uint8_t more;                      /* 0: last, 1: last (limit), 2: more */
} tNFC_RESULT_DEVT;

/* the data type associated with NFC_SELECT_DEVT */
typedef tNFC_STATUS tNFC_SELECT_DEVT;

/* the data type associated with NFC_STOP_DEVT */
typedef tNFC_STATUS tNFC_STOP_DEVT;

#define NFC_MAX_ATS_LEN NCI_MAX_ATS_LEN
#define NFC_MAX_HIS_BYTES_LEN NCI_MAX_HIS_BYTES_LEN
#define NFC_MAX_GEN_BYTES_LEN NCI_MAX_GEN_BYTES_LEN

typedef struct {
  uint8_t ats_res_len;              /* Length of ATS RES                */
  uint8_t ats_res[NFC_MAX_ATS_LEN]; /* ATS RES                          */
  bool nad_used;                    /* NAD is used or not               */
  uint8_t fwi;                      /* Frame Waiting time Integer       */
  uint8_t sfgi;                     /* Start-up Frame Guard time Integer*/
  uint8_t his_byte_len;             /* len of historical bytes          */
  uint8_t his_byte[NFC_MAX_HIS_BYTES_LEN]; /* historical bytes             */
} tNFC_INTF_PA_ISO_DEP;

typedef struct { uint8_t rats; /* RATS */ } tNFC_INTF_LA_ISO_DEP;

typedef struct {
  uint8_t atr_res_len;                      /* Length of ATR_RES            */
  uint8_t atr_res[NFC_MAX_ATS_LEN];         /* ATR_RES (Byte 3 - Byte 17+n) */
  uint8_t max_payload_size;                 /* 64, 128, 192 or 254          */
  uint8_t gen_bytes_len;                    /* len of general bytes         */
  uint8_t gen_bytes[NFC_MAX_GEN_BYTES_LEN]; /* general bytes           */
  uint8_t
      waiting_time; /* WT -> Response Waiting Time RWT = (256 x 16/fC) x 2WT */
} tNFC_INTF_PA_NFC_DEP;

/* Note: keep tNFC_INTF_PA_NFC_DEP data member in the same order as
 * tNFC_INTF_LA_NFC_DEP */
typedef struct {
  uint8_t atr_req_len;                      /* Length of ATR_REQ            */
  uint8_t atr_req[NFC_MAX_ATS_LEN];         /* ATR_REQ (Byte 3 - Byte 18+n) */
  uint8_t max_payload_size;                 /* 64, 128, 192 or 254          */
  uint8_t gen_bytes_len;                    /* len of general bytes         */
  uint8_t gen_bytes[NFC_MAX_GEN_BYTES_LEN]; /* general bytes           */
} tNFC_INTF_LA_NFC_DEP;
typedef tNFC_INTF_LA_NFC_DEP tNFC_INTF_LF_NFC_DEP;
typedef tNFC_INTF_PA_NFC_DEP tNFC_INTF_PF_NFC_DEP;

#define NFC_MAX_ATTRIB_LEN NCI_MAX_ATTRIB_LEN

typedef struct {
  uint8_t attrib_res_len;                 /* Length of ATTRIB RES      */
  uint8_t attrib_res[NFC_MAX_ATTRIB_LEN]; /* ATTRIB RES                */
  uint8_t hi_info_len;                    /* len of Higher layer Info  */
  uint8_t hi_info[NFC_MAX_GEN_BYTES_LEN]; /* Higher layer Info         */
  uint8_t mbli;                           /* Maximum buffer length.    */
} tNFC_INTF_PB_ISO_DEP;

typedef struct {
  uint8_t attrib_req_len;                 /* Length of ATTRIB REQ      */
  uint8_t attrib_req[NFC_MAX_ATTRIB_LEN]; /* ATTRIB REQ (Byte 2 - 10+k)*/
  uint8_t hi_info_len;                    /* len of Higher layer Info  */
  uint8_t hi_info[NFC_MAX_GEN_BYTES_LEN]; /* Higher layer Info         */
  uint8_t nfcid0[NFC_NFCID0_MAX_LEN];     /* NFCID0                    */
} tNFC_INTF_LB_ISO_DEP;

#ifndef NFC_MAX_RAW_PARAMS
#define NFC_MAX_RAW_PARAMS 16
#endif
#define NFC_MAX_RAW_PARAMS 16
typedef struct {
  uint8_t param_len;
  uint8_t param[NFC_MAX_RAW_PARAMS];
} tNFC_INTF_FRAME;

typedef struct {
  tNFC_INTF_TYPE type; /* Interface Type  1 Byte  See Table 67 */
  union {
    tNFC_INTF_LA_ISO_DEP la_iso;
    tNFC_INTF_PA_ISO_DEP pa_iso;
    tNFC_INTF_LB_ISO_DEP lb_iso;
    tNFC_INTF_PB_ISO_DEP pb_iso;
    tNFC_INTF_LA_NFC_DEP la_nfc;
    tNFC_INTF_PA_NFC_DEP pa_nfc;
    tNFC_INTF_LF_NFC_DEP lf_nfc;
    tNFC_INTF_PF_NFC_DEP pf_nfc;
    tNFC_INTF_FRAME frame;
  } intf_param; /* Activation Parameters   0 - n Bytes */
} tNFC_INTF_PARAMS;

/* the data type associated with NFC_ACTIVATE_DEVT */
typedef struct {
  uint8_t rf_disc_id;                /* RF Discovery ID          */
  tNFC_PROTOCOL protocol;            /* supported protocol       */
  tNFC_RF_TECH_PARAMS rf_tech_param; /* RF technology parameters */
  tNFC_DISCOVERY_TYPE data_mode;     /* for future Data Exchange */
  tNFC_BIT_RATE tx_bitrate;          /* Data Exchange Tx Bitrate */
  tNFC_BIT_RATE rx_bitrate;          /* Data Exchange Rx Bitrate */
  tNFC_INTF_PARAMS intf_param;       /* interface type and params*/
} tNFC_ACTIVATE_DEVT;

/* the data type associated with NFC_DEACTIVATE_DEVT and NFC_DEACTIVATE_CEVT */
typedef struct {
  tNFC_STATUS status;   /* The event status.        */
  tNFC_DEACT_TYPE type; /* De-activate type         */
  bool is_ntf;          /* TRUE, if deactivate notif*/
  tNFC_DEACT_REASON reason; /* De-activate reason    */
} tNFC_DEACTIVATE_DEVT;

typedef union {
  tNFC_STATUS status; /* The event status.        */
  tNFC_START_DEVT start;
  tNFC_RESULT_DEVT result;
  tNFC_SELECT_DEVT select;
  tNFC_STOP_DEVT stop;
  tNFC_ACTIVATE_DEVT activate;
  tNFC_DEACTIVATE_DEVT deactivate;
} tNFC_DISCOVER;

typedef struct {
  bool include_rf_tech_mode; /* TRUE if including RF Tech and Mode update    */
  tNFC_RF_TECH_N_MODE rf_tech_n_mode; /* RF tech and mode */
  bool include_tx_bit_rate;  /* TRUE if including Tx bit rate update         */
  tNFC_BIT_RATE tx_bit_rate; /* Transmit Bit Rate                            */
  bool include_rx_bit_rate;  /* TRUE if including Rx bit rate update         */
  tNFC_BIT_RATE rx_bit_rate; /* Receive Bit Rate                             */
  bool include_nfc_b_config; /* TRUE if including NFC-B data exchange config */
  uint8_t min_tr0;           /* Minimun TR0                                  */
  uint8_t min_tr1;           /* Minimun TR1                                  */
  uint8_t suppression_eos;   /* Suppression of EoS                           */
  uint8_t suppression_sos;   /* Suppression of SoS                           */
  uint8_t min_tr2;           /* Minimun TR1                                  */
} tNFC_RF_COMM_PARAMS;

/*************************************
**  DISCOVER Callback Functions
**************************************/
typedef void(tNFC_DISCOVER_CBACK)(tNFC_DISCOVER_EVT event,
                                  tNFC_DISCOVER* p_data);

typedef uint16_t tNFC_TEST_EVT;

/* the data type associated with NFC_LOOPBACK_TEVT */
typedef struct {
  tNFC_STATUS status; /* The event status.            */
  NFC_HDR* p_data;    /* The loop back data from NFCC */
} tNFC_LOOPBACK_TEVT;

/* the data type associated with NFC_RF_CONTROL_TEVT */
typedef tNFC_STATUS tNFC_RF_CONTROL_TEVT;

typedef union {
  tNFC_STATUS status; /* The event status.            */
  tNFC_LOOPBACK_TEVT loop_back;
  tNFC_RF_CONTROL_TEVT rf_control;
} tNFC_TEST;

/*************************************
**  TEST Callback Functions
**************************************/
typedef void(tNFC_TEST_CBACK)(tNFC_TEST_EVT event, tNFC_TEST* p_data);

typedef tNFC_DEACTIVATE_DEVT tNFC_DEACTIVATE_CEVT;
typedef union {
  tNFC_STATUS status; /* The event status. */
  tNFC_CONN_CREATE_CEVT conn_create;
  tNFC_CONN_CLOSE_CEVT conn_close;
  tNFC_DEACTIVATE_CEVT deactivate;
  tNFC_DATA_CEVT data;
} tNFC_CONN;

/*************************************
**  Data Callback Functions
**************************************/
typedef void(tNFC_CONN_CBACK)(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data);
#define NFC_MAX_CONN_ID 15
#define NFC_ILLEGAL_CONN_ID 0xFF
/* the static connection ID for RF traffic */
#define NFC_RF_CONN_ID 0
/* the static connection ID for HCI transport */
#define NFC_HCI_CONN_ID 1

 #if (NXP_EXTNS == TRUE)
 #define NFC_T4TNFCEE_CONN_ID 0x05
 #endif

/*****************************************************************************
**  EXTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

/*******************************************************************************
**
** Function         NFC_Enable
**
** Description      This function enables NFC. Prior to calling NFC_Enable:
**                  - the NFCC must be powered up, and ready to receive
**                    commands.
**                  - GKI must be enabled
**                  - NFC_TASK must be started
**                  - NCIT_TASK must be started (if using dedicated NCI
**                    transport)
**
**                  This function opens the NCI transport (if applicable),
**                  resets the NFC controller, and initializes the NFC
**                  subsystems.
**
**                  When the NFC startup procedure is completed, an
**                  NFC_ENABLE_REVT is returned to the application using the
**                  tNFC_RESPONSE_CBACK.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_Enable(tNFC_RESPONSE_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_Disable
**
** Description      This function performs clean up routines for shutting down
**                  NFC and closes the NCI transport (if using dedicated NCI
**                  transport).
**
**                  When the NFC shutdown procedure is completed, an
**                  NFC_DISABLED_REVT is returned to the application using the
**                  tNFC_RESPONSE_CBACK.
**
** Returns          nothing
**
*******************************************************************************/
extern void NFC_Disable(void);

/*******************************************************************************
**
** Function         NFC_Init
**
** Description      This function initializes control blocks for NFC
**
** Returns          nothing
**
*******************************************************************************/
extern void NFC_Init(tHAL_NFC_ENTRY* p_hal_entry_tbl);

/*******************************************************************************
**
** Function         NFC_GetLmrtSize
**
** Description      Called by application wto query the Listen Mode Routing
**                  Table size supported by NFCC
**
** Returns          Listen Mode Routing Table size
**
*******************************************************************************/
extern uint16_t NFC_GetLmrtSize(void);

/*******************************************************************************
**
** Function         NFC_SetConfig
**
** Description      This function is called to send the configuration parameter
**                  TLV to NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_SET_CONFIG_REVT.
**
** Parameters       tlv_size - the length of p_param_tlvs.
**                  p_param_tlvs - the parameter ID/Len/Value list
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SetConfig(uint8_t tlv_size, uint8_t* p_param_tlvs);

/*******************************************************************************
**
** Function         NFC_GetConfig
**
** Description      This function is called to retrieve the parameter TLV from
**                  NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_GET_CONFIG_REVT.
**
** Parameters       num_ids - the number of parameter IDs
**                  p_param_ids - the parameter ID list.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_GetConfig(uint8_t num_ids, uint8_t* p_param_ids);

/*******************************************************************************
**
** Function         NFC_NfceeDiscover
**
** Description      This function is called to enable or disable NFCEE
**                  Discovery. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_NFCEE_DISCOVER_REVT.
**                  The notification from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_NFCEE_INFO_REVT.
**
** Parameters       discover - 1 to enable discover, 0 to disable.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_NfceeDiscover(bool discover);

/*******************************************************************************
**
** Function         NFC_NfceeModeSet
**
** Description      This function is called to activate or de-activate an NFCEE
**                  connected to the NFCC.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_NFCEE_MODE_SET_REVT.
**
** Parameters       nfcee_id - the NFCEE to activate or de-activate.
**                  mode - 0 to activate NFCEE, 1 to de-activate.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_NfceeModeSet(uint8_t nfcee_id, tNFC_NFCEE_MODE mode);
/*******************************************************************************
**
** Function         NFC_DiscoveryMap
**
** Description      This function is called to set the discovery interface
**                  mapping. The response from NFCC is reported by
**                  tNFC_DISCOVER_CBACK as. NFC_MAP_DEVT.
**
** Parameters       num - the number of items in p_params.
**                  p_maps - the discovery interface mappings
**                  p_cback - the discovery callback function
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_DiscoveryMap(uint8_t num, tNFC_DISCOVER_MAPS* p_maps,
                                    tNFC_DISCOVER_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_DiscoveryStart
**
** Description      This function is called to start Polling and/or Listening.
**                  The response from NFCC is reported by tNFC_DISCOVER_CBACK
**                  as NFC_START_DEVT. The notification from NFCC is reported by
**                  tNFC_DISCOVER_CBACK as NFC_RESULT_DEVT.
**
** Parameters       num_params - the number of items in p_params.
**                  p_params - the discovery parameters
**                  p_cback - the discovery callback function
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_DiscoveryStart(uint8_t num_params,
                                      tNFC_DISCOVER_PARAMS* p_params,
                                      tNFC_DISCOVER_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_DiscoverySelect
**
** Description      If tNFC_DISCOVER_CBACK reports status=NFC_MULTIPLE_PROT,
**                  the application needs to use this function to select the
**                  the logical endpoint to continue. The response from NFCC is
**                  reported by tNFC_DISCOVER_CBACK as NFC_SELECT_DEVT.
**
** Parameters       rf_disc_id - The ID identifies the remote device.
**                  protocol - the logical endpoint on the remote devide
**                  rf_interface - the RF interface to communicate with NFCC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_DiscoverySelect(uint8_t rf_disc_id, uint8_t protocol,
                                       uint8_t rf_interface);

/*******************************************************************************
**
** Function         NFC_ConnCreate
**
** Description      This function is called to create a logical connection with
**                  NFCC for data exchange.
**                  The response from NFCC is reported in tNFC_CONN_CBACK
**                  as NFC_CONN_CREATE_CEVT.
**
** Parameters       dest_type - the destination type
**                  id   - the NFCEE ID or RF Discovery ID .
**                  protocol - the protocol
**                  p_cback - the data callback function to receive data from
**                  NFCC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_ConnCreate(uint8_t dest_type, uint8_t id,
                                  uint8_t protocol, tNFC_CONN_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_ConnClose
**
** Description      This function is called to close a logical connection with
**                  NFCC.
**                  The response from NFCC is reported in tNFC_CONN_CBACK
**                  as NFC_CONN_CLOSE_CEVT.
**
** Parameters       conn_id - the connection id.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_ConnClose(uint8_t conn_id);

/*******************************************************************************
**
** Function         NFC_SetStaticRfCback
**
** Description      This function is called to update the data callback function
**                  to receive the data for the given connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
extern void NFC_SetStaticRfCback(tNFC_CONN_CBACK* p_cback);

#if (NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         NFC_SetStaticT4tNfceeCback
**
** Description      This function is called to update the data callback function
**                  to receive the data for the given connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
void NFC_SetStaticT4tNfceeCback(tNFC_CONN_CBACK* p_cback);
#endif

/*******************************************************************************
**
** Function         NFC_SetReassemblyFlag
**
** Description      This function is called to set if nfc will reassemble
**                  nci packet as much as its buffer can hold or it should not
**                  reassemble but forward the fragmented nci packet to layer
**                  above. If nci data pkt is fragmented, nfc may send multiple
**                  NFC_DATA_CEVT with status NFC_STATUS_CONTINUE before sending
**                  NFC_DATA_CEVT with status NFC_STATUS_OK based on reassembly
**                  configuration and reassembly buffer size
**
** Parameters       reassembly - flag to indicate if nfc may reassemble or not
**
** Returns          Nothing
**
*******************************************************************************/
extern void NFC_SetReassemblyFlag(bool reassembly);

/*******************************************************************************
**
** Function         NFC_SendData
**
** Description      This function is called to send the given data packet
**                  to the connection identified by the given connection id.
**
** Parameters       conn_id - the connection id.
**                  p_data - the data packet
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SendData(uint8_t conn_id, NFC_HDR* p_data);

/*******************************************************************************
**
** Function         NFC_FlushData
**
** Description      This function is called to discard the tx data queue of
**                  the given connection id.
**
** Parameters       conn_id - the connection id.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_FlushData(uint8_t conn_id);

/*******************************************************************************
**
** Function         NFC_Deactivate
**
** Description      This function is called to stop the discovery process or
**                  put the listen device in sleep mode or terminate the NFC
**                  link.
**
**                  The response from NFCC is reported by tNFC_DISCOVER_CBACK
**                  as NFC_DEACTIVATE_DEVT.
**
** Parameters       deactivate_type - NFC_DEACTIVATE_TYPE_IDLE, to IDLE mode.
**                                    NFC_DEACTIVATE_TYPE_SLEEP to SLEEP mode.
**                                    NFC_DEACTIVATE_TYPE_SLEEP_AF to SLEEP_AF
**                                    mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_Deactivate(tNFC_DEACT_TYPE deactivate_type);

/*******************************************************************************
**
** Function         NFC_UpdateRFCommParams
**
** Description      This function is called to update RF Communication
**                  parameters once the Frame RF Interface has been activated.
**
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_RF_COMM_PARAMS_UPDATE_REVT.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_UpdateRFCommParams(tNFC_RF_COMM_PARAMS* p_params);

/*******************************************************************************
**
** Function         NFC_SetPowerOffSleep
**
** Description      This function closes/opens transport and turns off/on NFCC.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SetPowerOffSleep(bool enable);

/*******************************************************************************
**
** Function         NFC_SetPowerSubState
**
** Description      This function is called to send the power sub state(screen
**                  state) to NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_SET_POWER_STATE_REVT.
**
** Parameters       scree_state
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SetPowerSubState(uint8_t screen_state);

/*******************************************************************************
**
** Function         NFC_PowerCycleNFCC
**
** Description      This function turns off and then on NFCC.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_PowerCycleNFCC(void);

/*******************************************************************************
**
** Function         NFC_SetRouting
**
** Description      This function is called to configure the CE routing table.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_SET_ROUTING_REVT.
**
** Parameters
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SetRouting(bool more, uint8_t num_tlv, uint8_t tlv_size,
                                  uint8_t* p_param_tlvs);

/*******************************************************************************
**
** Function         NFC_GetRouting
**
** Description      This function is called to retrieve the CE routing table
**                  from NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_GET_ROUTING_REVT.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_GetRouting(void);

/*******************************************************************************
**
** Function         NFC_RegVSCback
**
** Description      This function is called to register or de-register a
**                  callback function to receive Proprietary NCI response and
**                  notification events.
**                  The maximum number of callback functions allowed is
**                  NFC_NUM_VS_CBACKS
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_RegVSCback(bool is_register, tNFC_VS_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_SendVsCommand
**
** Description      This function is called to send the given vendor specific
**                  command to NFCC. The response from NFCC is reported to the
**                  given tNFC_VS_CBACK as (oid).
**
** Parameters       oid - The opcode of the VS command.
**                  p_data - The parameters for the VS command
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SendVsCommand(uint8_t oid, NFC_HDR* p_data,
                                     tNFC_VS_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_SendRawVsCommand
**
** Description      This function is called to send the given raw command to
**                  NFCC. The response from NFCC is reported to the given
**                  tNFC_VS_CBACK.
**
** Parameters       p_data - The command buffer
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_SendRawVsCommand(NFC_HDR* p_data,
                                        tNFC_VS_CBACK* p_cback);

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
extern tNFC_STATUS NFC_TestLoopback(NFC_HDR* p_data);

/*******************************************************************************
**
** Function         NFC_ISODEPNakPresCheck
**
** Description      This function is called to send the ISO DEP nak presence
**                  check cmd to check that the remote end point in RF field.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_ISODEPNakPresCheck();

#if (APPL_DTA_MODE == TRUE)
/*******************************************************************************
**
** Function         nfc_ncif_getFWVersion
**
** Description      This function sets the trace level for NFC.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
extern tNFC_FW_VERSION nfc_ncif_getFWVersion();
#endif

/*******************************************************************************
**
** Function         NFC_NfceePLConfig
**
** Description      This function is called to set the Power and Link Control
**                  to an NFCEE connected to the NFCC.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_NFCEE_PL_CONTROL_REVT.
**
** Parameters       nfcee_id - the NFCEE to activate or de-activate.
**                 pl_config -
**                 NFCEE_PL_CONFIG_NFCC_DECIDES -NFCC decides (default)
**                 NFCEE_PL_CONFIG_PWR_ALWAYS_ON -NFCEE power supply always on
**                 NFCEE_PL_CONFIG_LNK_ON_WHEN_PWR_ON -
**                                     communication link is always active
**                                     when NFCEE is powered on
**                 NFCEE_PL_CONFIG_PWR_LNK_ALWAYS_ON -
**                                     power supply and communication
**                                     link are always on
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_NfceePLConfig(uint8_t nfcee_id,
                                     tNCI_NFCEE_PL_CONFIG pl_config);

/*******************************************************************************
**
** Function         NFC_SetStaticHciCback
**
** Description      This function to update the data callback function
**                  to receive the data for the static Hci connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
extern void NFC_SetStaticHciCback(tNFC_CONN_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_GetStatusName
**
** Description      This function returns the status name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
extern std::string NFC_GetStatusName(tNFC_STATUS status);

/*******************************************************************************
**
** Function         NFC_SetTraceLevel
**
** Description      This function sets the trace level for NFC.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
NFC_API extern UINT8 NFC_SetTraceLevel (UINT8 new_level);

/*******************************************************************************
**
** Function         NFC_ISODEPNakPresCheck
**
** Description      This function is called to send the ISO DEP nak presence
**                  check cmd to check that the remote end point in RF field.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_ISODEPNakPresCheck();



/*******************************************************************************
**
** Function         NFC_NfceePLConfig
**
** Description      This function is called to set the Power and Link Control
**                  to an NFCEE connected to the NFCC.
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_NFCEE_PL_CONTROL_REVT.
**
** Parameters       nfcee_id - the NFCEE to activate or de-activate.
**                 pl_config -
**                 NFCEE_PL_CONFIG_NFCC_DECIDES -NFCC decides (default)
**                 NFCEE_PL_CONFIG_PWR_ALWAYS_ON -NFCEE power supply always on
**                 NFCEE_PL_CONFIG_LNK_ON_WHEN_PWR_ON -
**                                     communication link is always active
**                                     when NFCEE is powered on
**                 NFCEE_PL_CONFIG_PWR_LNK_ALWAYS_ON -
**                                     power supply and communication
**                                     link are always on
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS NFC_NfceePLConfig(uint8_t nfcee_id,
                                     tNCI_NFCEE_PL_CONFIG pl_config);

/*******************************************************************************
**
** Function         NFC_SetStaticHciCback
**
** Description      This function to update the data callback function
**                  to receive the data for the static Hci connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
extern void NFC_SetStaticHciCback(tNFC_CONN_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFC_GetStatusName
**
** Description      This function returns the status name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
extern std::string NFC_GetStatusName(tNFC_STATUS status);


#endif /* NFC_API_H */
