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
 *  This is the public interface file for NFA, Broadcom's NFC application
 *  layer for mobile phones.
 *
 ******************************************************************************/
#ifndef NFA_API_H
#define NFA_API_H

#include "gki.h"
#include "nci_defs.h"
#include "nfc_api.h"
#include "nfc_hal_api.h"
#include "nfc_target.h"
#include "rw_api.h"
#include "tags_defs.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Max length of Appliction ID in 7816-4 */
#define NFA_MAX_AID_LEN NFC_MAX_AID_LEN
#define NFA_MIN_AID_LEN 5 /* per NCI specification */

/* Command succeeded    */
#define NFA_STATUS_OK NCI_STATUS_OK
/* Command is rejected. */
#define NFA_STATUS_REJECTED NCI_STATUS_REJECTED
#define NFA_STATUS_MSG_CORRUPTED        NCI_STATUS_MESSAGE_CORRUPTED    /* Message is corrupted */
/* buffer full          */
#define NFA_STATUS_BUFFER_FULL NCI_STATUS_BUFFER_FULL
/* failed               */
#define NFA_STATUS_FAILED NCI_STATUS_FAILED
#define NFA_STATUS_NOT_INITIALIZED      NCI_STATUS_NOT_INITIALIZED      /* not initialized      */
#define NFA_STATUS_SYNTAX_ERROR         NCI_STATUS_SYNTAX_ERROR         /* Syntax error         */
/* Optional feature of NCI Spec not supported */
#define NFA_STATUS_NOT_SUPPORTED NCI_STATUS_NOT_SUPPORTED
/* Semantic error       */
#define NFA_STATUS_SEMANTIC_ERROR NCI_STATUS_SEMANTIC_ERROR
/* Unknown NCI Group ID */
#define NFA_STATUS_UNKNOWN_GID NCI_STATUS_UNKNOWN_GID
/* Invalid Parameter    */
#define NFA_STATUS_INVALID_PARAM NCI_STATUS_INVALID_PARAM
/* Already started      */
#define NFA_STATUS_ALREADY_STARTED NCI_STATUS_ALREADY_STARTED
/* RF Timeout           */
#define NFA_STATUS_TIMEOUT NCI_STATUS_TIMEOUT

/* Out of GKI buffers */
#define NFA_STATUS_NO_BUFFERS NFC_STATUS_NO_BUFFERS
/* Protocol mismatch between API and activated one */
#define NFA_STATUS_WRONG_PROTOCOL NFC_STATUS_WRONG_PROTOCOL
/* Another Tag command is already in progress */
#define NFA_STATUS_BUSY NFC_STATUS_BUSY

/* data len exceeds MIU                             */
#define NFA_STATUS_BAD_LENGTH NFC_STATUS_BAD_LENGTH
/* invalid handle                                   */
#define NFA_STATUS_BAD_HANDLE NFC_STATUS_BAD_HANDLE
/* congested                                        */
#define NFA_STATUS_CONGESTED NFC_STATUS_CONGESTED
#define NFA_STATUS_CONTINUE             NFC_STATUS_CONTINUE       /* More NFA_CE_GET_ROUTING_REVT to follow           */
typedef uint8_t tNFA_STATUS;

/* Handle for NFA registrations and connections */
typedef uint16_t tNFA_HANDLE;
#define NFA_HANDLE_INVALID (0xFFFF)
/* NFA Handle definitions */

/* The upper byte of NFA_HANDLE signifies the handle group */
/* NDEF Type Handler handles    */
#define NFA_HANDLE_GROUP_NDEF_HANDLER 0x0200
/* DH Card Emulation handles    */
#define NFA_HANDLE_GROUP_CE 0x0300
/* Handles to identify NFCEE    */
#define NFA_HANDLE_GROUP_EE 0x0400
/* P2P handles                  */
#define NFA_HANDLE_GROUP_P2P 0x0500
#define NFA_HANDLE_GROUP_SNEP           0x0700      /* SNEP handles                 */
/* HCI handles                  */
#define NFA_HANDLE_GROUP_HCI 0x0800
/* Local NDEF message handle    */
#define NFA_HANDLE_GROUP_MASK 0xFF00
#define NFA_HANDLE_MASK 0x00FF

/* NCI Parameter IDs */
typedef uint8_t tNFA_PMID;

/* Definitions for tNFA_TECHNOLOGY_MASK */
#define NFA_TECHNOLOGY_MASK_A 0x01        /* NFC Technology A             */
#define NFA_TECHNOLOGY_MASK_B 0x02        /* NFC Technology B             */
#define NFA_TECHNOLOGY_MASK_F 0x04        /* NFC Technology F             */
/* TECHNOLOGY_MASK_V in NCI2.0 and TECHNOLOGY_MASK_15693 proprietary in NCI1.0*/
#define NFA_TECHNOLOGY_MASK_V 0x08
#define NFA_TECHNOLOGY_MASK_B_PRIME 0x10  /* Proprietary Technology       */
#define NFA_TECHNOLOGY_MASK_KOVIO 0x20    /* Proprietary Technology       */
/* NFC technology NFC-DEP protocol active mode */
#define NFA_TECHNOLOGY_MASK_ACTIVE 0x40
/* NFC Technology A active mode */
#define NFA_TECHNOLOGY_MASK_A_ACTIVE 0x40
/* NFC Technology F active mode */
#define NFA_TECHNOLOGY_MASK_F_ACTIVE 0x80
/* All supported technologies   */
typedef uint8_t tNFA_TECHNOLOGY_MASK;

/* Definitions for NFC protocol for RW, CE and P2P APIs */
/* Type1Tag - NFC-A */
#define NFA_PROTOCOL_T1T NFC_PROTOCOL_T1T
/* MIFARE/Type2Tag - NFC-A */
#define NFA_PROTOCOL_T2T NFC_PROTOCOL_T2T
/* Felica/Type3Tag - NFC-F */
#define NFA_PROTOCOL_T3T NFC_PROTOCOL_T3T
/* Type 4A,4B - NFC-A or NFC-B */
#define NFA_PROTOCOL_ISO_DEP NFC_PROTOCOL_ISO_DEP
/* NFCDEP/LLCP - NFC-A or NFC-F */
#define NFA_PROTOCOL_NFC_DEP NFC_PROTOCOL_NFC_DEP
#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#define NFC_PROTOCOL_MASK_ISO7816   0x20    /*ISO 7816 - Aid Default Route */
#endif
/* NFC_PROTOCOL_T5T in NCI2.0 and NFC_PROTOCOL_ISO15693 proprietary in NCI1.0*/
#define NFA_PROTOCOL_T5T NFC_PROTOCOL_T5T
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#define NFA_PROTOCOL_MIFARE     NFC_PROTOCOL_MIFARE
#define NFA_PROTOCOL_T3BT       NFC_PROTOCOL_T3BT
#endif
/* Discovery Types/Detected Technology and Mode */
#define NCI_DISCOVERY_TYPE_POLL_A               0x00
#define NCI_DISCOVERY_TYPE_POLL_B               0x01
#define NCI_DISCOVERY_TYPE_POLL_F               0x02
#define NCI_DISCOVERY_TYPE_POLL_A_ACTIVE        0x03
#define NCI_DISCOVERY_TYPE_POLL_F_ACTIVE        0x05
#define NCI_DISCOVERY_TYPE_POLL_B_PRIME         0x74
#define NCI_DISCOVERY_TYPE_LISTEN_A             0x80
#define NCI_DISCOVERY_TYPE_LISTEN_B             0x81
#define NCI_DISCOVERY_TYPE_LISTEN_F             0x82
#define NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE      0x83
#define NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE      0x85
#define NCI_DISCOVERY_TYPE_LISTEN_B_PRIME       0xF4
#define NCI_DISCOVERY_TYPE_POLL_ISO15693        0x06
#define NCI_DISCOVERY_TYPE_LISTEN_ISO15693      0x86
#define NCI_DISCOVERY_TYPE_MAX  NCI_DISCOVERY_TYPE_LISTEN_ISO15693

/* Discovery Types/Detected Technology and Mode */
#define NFC_DISCOVERY_TYPE_POLL_A           NCI_DISCOVERY_TYPE_POLL_A
#define NFC_DISCOVERY_TYPE_POLL_B           NCI_DISCOVERY_TYPE_POLL_B
#define NFC_DISCOVERY_TYPE_POLL_F           NCI_DISCOVERY_TYPE_POLL_F
#define NFC_DISCOVERY_TYPE_POLL_A_ACTIVE    NCI_DISCOVERY_TYPE_POLL_A_ACTIVE
#define NFC_DISCOVERY_TYPE_POLL_F_ACTIVE    NCI_DISCOVERY_TYPE_POLL_F_ACTIVE
#define NFC_DISCOVERY_TYPE_POLL_ISO15693    NCI_DISCOVERY_TYPE_POLL_ISO15693
#define NFC_DISCOVERY_TYPE_POLL_B_PRIME     NCI_DISCOVERY_TYPE_POLL_B_PRIME
#define NFC_DISCOVERY_TYPE_POLL_KOVIO       NCI_DISCOVERY_TYPE_POLL_KOVIO
#define NFC_DISCOVERY_TYPE_LISTEN_A         NCI_DISCOVERY_TYPE_LISTEN_A
#define NFC_DISCOVERY_TYPE_LISTEN_B         NCI_DISCOVERY_TYPE_LISTEN_B
#define NFC_DISCOVERY_TYPE_LISTEN_F         NCI_DISCOVERY_TYPE_LISTEN_F
#define NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE  NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE
#define NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE  NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE
#define NFC_DISCOVERY_TYPE_LISTEN_ISO15693  NCI_DISCOVERY_TYPE_LISTEN_ISO15693
#define NFC_DISCOVERY_TYPE_LISTEN_B_PRIME   NCI_DISCOVERY_TYPE_LISTEN_B_PRIME

#define NFA_PROTOCOL_INVALID 0xFF
typedef uint8_t tNFA_NFC_PROTOCOL;

/* Definitions for tNFA_PROTOCOL_MASK */
#define NFA_PROTOCOL_MASK_T1T 0x01     /* Type 1 tag          */
#define NFA_PROTOCOL_MASK_T2T 0x02     /* MIFARE / Type 2 tag */
#define NFA_PROTOCOL_MASK_T3T 0x04     /* FeliCa / Type 3 tag */
#define NFA_PROTOCOL_MASK_ISO_DEP 0x08 /* ISODEP/4A,4B        */
#define NFA_PROTOCOL_MASK_NFC_DEP 0x10 /* NFCDEP/LLCP         */
typedef uint8_t tNFA_PROTOCOL_MASK;

/* NFA_DM callback events */
/* Result of NFA_Enable             */
#define NFA_DM_ENABLE_EVT 0
/* Result of NFA_Disable            */
#define NFA_DM_DISABLE_EVT 1
/* Result of NFA_SetConfig          */
#define NFA_DM_SET_CONFIG_EVT 2
/* Result of NFA_GetConfig          */
#define NFA_DM_GET_CONFIG_EVT 3
/* Result of NFA_PowerOffSleepMode  */
#define NFA_DM_PWR_MODE_CHANGE_EVT 4
#define NFA_DM_RF_FIELD_EVT 5 /* Status of RF Field               */
/* NFCC is not responding           */
#define NFA_DM_NFCC_TIMEOUT_EVT 6
/* NCI Tranport error               */
#define NFA_DM_NFCC_TRANSPORT_ERR_EVT 7
/* Result of NFA_SetPowerSubStateForScreenState */
#define NFA_DM_SET_POWER_SUB_STATE_EVT 11
/* T1T HR length            */
#define NFA_T1T_HR_LEN T1T_HR_LEN
/* Max UID length of T1/T2  */
#define NFA_MAX_UID_LEN TAG_MAX_UID_LEN
/* UID len for T1T cmds     */
#define NFA_T1T_CMD_UID_LEN T1T_CMD_UID_LEN
/* T2T UID length           */

/* Tag formated for NDEF */
#define NFA_RW_NDEF_FL_FORMATED RW_NDEF_FL_FORMATED
/* NDEF supported by the tag */
#define NFA_RW_NDEF_FL_SUPPORTED RW_NDEF_FL_SUPPORTED
/* Unable to find if tag is ndef capable/formated/read only */
#define NFA_RW_NDEF_FL_UNKNOWN RW_NDEF_FL_UNKNOWN
/* Tag supports format operation */
#define NFA_RW_NDEF_FL_FORMATABLE RW_NDEF_FL_FORMATABLE

typedef uint8_t tNFA_RW_NDEF_FLAG;

/* Data for NFA_DM_SET_CONFIG_EVT */
typedef struct {
  tNFA_STATUS status;                   /* NFA_STATUS_OK if successful  */
  uint8_t num_param_id;                 /* Number of rejected Param ID  */
  tNFA_PMID param_ids[NFC_MAX_NUM_IDS]; /* Rejected Param ID            */
} tNFA_SET_CONFIG;

/* Data for NFA_DM_GET_CONFIG_EVT */
typedef struct {
  tNFA_STATUS status;    /* NFA_STATUS_OK if successful              */
  uint16_t tlv_size;     /* The length of TLV                        */
  uint8_t* param_tlvs;   /* TLV (Parameter ID-Len-Value byte stream) */
} tNFA_GET_CONFIG;

/* Structure to store screen state */
typedef enum screen_state {
  NFA_SCREEN_STATE_UNKNOWN = 0x00,
  NFA_SCREEN_STATE_OFF_UNLOCKED = 0x01,
  NFA_SCREEN_STATE_OFF_LOCKED = 0x02,
  NFA_SCREEN_STATE_ON_LOCKED = 0x04,
  NFA_SCREEN_STATE_ON_UNLOCKED = 0x08
} eScreenState_t;

typedef enum power_substate {
  SCREEN_STATE_ON_UNLOCKED = 0x00,
  SCREEN_STATE_OFF_UNLOCKED,
  SCREEN_STATE_ON_LOCKED,
  SCREEN_STATE_OFF_LOCKED,
  SCREEN_STATE_INVALID = 0xFF
} epower_substate_t;

#define NFA_SCREEN_STATE_MASK 0x0F

/* CONN_DISCOVER_PARAM */
#define NFA_DM_PWR_MODE_FULL 0x04
#define NFA_DM_PWR_MODE_OFF_SLEEP 0x00

typedef uint8_t tNFA_DM_PWR_MODE;

/* Data for NFA_DM_PWR_MODE_CHANGE_EVT */
typedef struct {
  tNFA_STATUS status; /* NFA_STATUS_OK if successful */
  /* NFA_DM_PWR_MODE_FULL or NFA_DM_PWR_MODE_OFF_SLEEP */
  tNFA_DM_PWR_MODE power_mode;
} tNFA_DM_PWR_MODE_CHANGE;

/* Data for NFA_DM_RF_FIELD_EVT */
#define NFA_DM_RF_FIELD_OFF 0x00
#define NFA_DM_RF_FIELD_ON 0x01

typedef struct {
  tNFA_STATUS status; /* NFA_STATUS_OK if successful  */
  /* NFA_DM_RF_FIELD_ON if operating field generated by remote */
  uint8_t rf_field_status;
} tNFA_DM_RF_FIELD;

typedef struct {
  tNFA_STATUS status;  /* NFA_STATUS_OK if successful  */
  uint8_t power_state; /* current screen/power state */
} tNFA_DM_POWER_STATE;

/* Union of all DM callback structures */
typedef union {
  tNFA_STATUS status;                 /* NFA_DM_ENABLE_EVT        */
  tNFA_SET_CONFIG set_config;         /* NFA_DM_SET_CONFIG_EVT    */
  tNFA_GET_CONFIG get_config;         /* NFA_DM_GET_CONFIG_EVT    */
  tNFA_DM_PWR_MODE_CHANGE power_mode; /* NFA_DM_PWR_MODE_CHANGE_EVT   */
  tNFA_DM_RF_FIELD rf_field;          /* NFA_DM_RF_FIELD_EVT      */
  void* p_vs_evt_data;                /* Vendor-specific evt data */
  tNFA_DM_POWER_STATE power_sub_state; /* power sub state */
} tNFA_DM_CBACK_DATA;

/* NFA_DM callback */
typedef void(tNFA_DM_CBACK)(uint8_t event, tNFA_DM_CBACK_DATA* p_data);

/* NFA Enable DTA Type Mode */
typedef enum {
  NFA_DTA_DEFAULT_MODE = 0x00000001,
  NFA_DTA_LLCP_MODE = 0x00000002,
  NFA_DTA_HCEF_MODE = 0x00000004,
  NFA_DTA_CR8 = 0x00000080,
} tNFA_eDtaModes;
typedef struct
{
    UINT8 major_version; /* Major Version of MW*/
    UINT8 minor_version; /* Minor Version of Mw */
    UINT8 android_version; /* NXP Android Version*/
}tNFA_MW_VERSION;

/* NFA Connection Callback Events */
#define NFA_POLL_ENABLED_EVT 0  /* Polling enabled event */
#define NFA_POLL_DISABLED_EVT 1 /* Polling disabled event */
/* NFC link/protocol discovery notificaiton */
#define NFA_DISC_RESULT_EVT 2
/* NFC link/protocol discovery select response */
#define NFA_SELECT_RESULT_EVT 3
#define NFA_DEACTIVATE_FAIL_EVT 4 /* NFA_Deactivate failure */
/* NFC link/protocol activated */
#define NFA_ACTIVATED_EVT 5
/* NFC link/protocol deactivated */
#define NFA_DEACTIVATED_EVT 6
#define NFA_TLV_DETECT_EVT 7 /* TLV Detection complete */
/* NDEF Detection complete */
#define NFA_NDEF_DETECT_EVT 8
#define NFA_DATA_EVT 9            /* Data message received */
#define NFA_SELECT_CPLT_EVT 10    /* Select completed */
#define NFA_READ_CPLT_EVT 11      /* Read completed */
#define NFA_WRITE_CPLT_EVT 12     /* Write completed */
#define NFA_LLCP_ACTIVATED_EVT 13 /* LLCP link is activated */
/* LLCP link is deactivated */
#define NFA_LLCP_DEACTIVATED_EVT 14
/* Response to NFA_RwPresenceCheck */
#define NFA_PRESENCE_CHECK_EVT 15
/* Tag Formating completed */
#define NFA_FORMAT_CPLT_EVT 16
/* ISO 15693 command completed */
#define NFA_I93_CMD_CPLT_EVT 17
#define NFA_SET_TAG_RO_EVT 18 /* Tag set as Read only */
/* Result for NFA_RequestExclusiveRfControl */
#define NFA_EXCLUSIVE_RF_CONTROL_STARTED_EVT 19
/* Result for NFA_ReleaseExclusiveRfControl */
#define NFA_EXCLUSIVE_RF_CONTROL_STOPPED_EVT 20
/* DH Card emulation: AID or System code reg'd  */
#define NFA_CE_REGISTERED_EVT 21
/* DH Card emulation: AID or System code dereg'd */
#define NFA_CE_DEREGISTERED_EVT 22
/* DH Card emulation: data received event */
#define NFA_CE_DATA_EVT 23
/* DH Card emulation: activation event */
#define NFA_CE_ACTIVATED_EVT 24
/* DH Card emulation: deactivation event */
#define NFA_CE_DEACTIVATED_EVT 25
/* DH Card emulation: local NDEF configured */
#define NFA_CE_LOCAL_TAG_CONFIGURED_EVT 26
/* DH Card emulation: NDEF write started */
#define NFA_CE_NDEF_WRITE_START_EVT 27
/* DH Card emulation: NDEF write completed */
#define NFA_CE_NDEF_WRITE_CPLT_EVT 28
#define NFA_CE_UICC_LISTEN_CONFIGURED_EVT 29 /* UICC Listen configured */
/* RF Discovery started event */
#define NFA_RF_DISCOVERY_STARTED_EVT 30
/* RF Discovery stopped event */
#define NFA_RF_DISCOVERY_STOPPED_EVT 31
/* status of updating RF communication paramters */
#define NFA_UPDATE_RF_PARAM_RESULT_EVT 32
/* RF Interface error event */
#define NFA_RW_INTF_ERROR_EVT 34
/* status of setting P2P listen technologies */
#define NFA_SET_P2P_LISTEN_TECH_EVT 33
/* First packet received over LLCP link */
#define NFA_LLCP_FIRST_PACKET_RECEIVED_EVT 35
/* Listening enabled event */
#define NFA_LISTEN_ENABLED_EVT 36
/* Listening disabled event */
#define NFA_LISTEN_DISABLED_EVT 37
/* P2P services paused event */
#define NFA_P2P_PAUSED_EVT 38
/* P2P services resumed event */
#define NFA_P2P_RESUMED_EVT 39
#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#define NFA_CE_ESE_LISTEN_CONFIGURED_EVT        40  /* ESE Listen configured                        */
#define NFA_ACTIVATED_UPDATE_EVT                41  /* Activated intf for updating the   tech variables */
#define NFA_RECOVERY_EVT                        42  /*Recovery*/
#endif

/* NFC deactivation type */
#define NFA_DEACTIVATE_TYPE_IDLE NFC_DEACTIVATE_TYPE_IDLE
#define NFA_DEACTIVATE_TYPE_SLEEP NFC_DEACTIVATE_TYPE_SLEEP
#define NFA_DEACTIVATE_TYPE_DISCOVERY NFC_DEACTIVATE_TYPE_DISCOVERY

typedef uint8_t tNFA_DEACTIVATE_TYPE;

/* Data for NFA_DISC_RESULT_EVT */
typedef struct {
  tNFA_STATUS status;             /* NFA_STATUS_OK if successful       */
  tNFC_RESULT_DEVT discovery_ntf; /* RF discovery notification details */
} tNFA_DISC_RESULT;

/* Data for NFA_ACTIVATED_EVT */
typedef struct {
  uint8_t hr[NFA_T1T_HR_LEN];       /* HR of Type 1 tag         */
  uint8_t uid[NFA_T1T_CMD_UID_LEN]; /* UID used in T1T Commands */
} tNFA_T1T_PARAMS;

typedef struct {
  uint8_t uid[NFA_MAX_UID_LEN]; /* UID of T2T tag           */
} tNFA_T2T_PARAMS;

typedef struct {
  uint8_t num_system_codes; /* Number of system codes supporte by tag   */
  uint16_t* p_system_codes; /* Pointer to list of system codes          */
} tNFA_T3T_PARAMS;

typedef struct {
  uint8_t uid[I93_UID_BYTE_LEN]; /* UID[0]:MSB, ... UID[7]:LSB */
  uint8_t info_flags;   /* information flags                            */
  uint8_t dsfid;        /* DSFID if I93_INFO_FLAG_DSFID                 */
  uint8_t afi;          /* AFI if I93_INFO_FLAG_AFI                     */
  uint16_t num_block;   /* number of blocks if I93_INFO_FLAG_MEM_SIZE   */
  uint8_t block_size;   /* block size in byte if I93_INFO_FLAG_MEM_SIZE */
  uint8_t IC_reference; /* IC Reference if I93_INFO_FLAG_IC_REF         */
} tNFA_I93_PARAMS;

typedef union {
  tNFA_T1T_PARAMS t1t; /* HR and UID of T1T                */
  tNFA_T2T_PARAMS t2t; /* UID of T2T                       */
  tNFA_T3T_PARAMS t3t; /* System codes                     */
  tNFA_I93_PARAMS i93; /* System Information of ISO 15693  */
} tNFA_TAG_PARAMS;

typedef struct {
  tNFC_ACTIVATE_DEVT activate_ntf; /* RF discovery activation details */
  tNFA_TAG_PARAMS params;          /* additional informaiton of tag   */
} tNFA_ACTIVATED;

/* Data for NFA_DEACTIVATED_EVT */
typedef struct {
  /* NFA_DEACTIVATE_TYPE_IDLE or NFA_DEACTIVATE_TYPE_SLEEP */
  tNFA_DEACTIVATE_TYPE type;
} tNFA_DEACTIVATED;

/* Structure for NFA_NDEF_DETECT_EVT event data */
typedef struct {
  tNFA_STATUS status;         /* Status of the ndef detecton */
  tNFA_NFC_PROTOCOL protocol; /* protocol used to detect NDEF */
  uint32_t max_size;          /* max number of bytes available for NDEF data */
  uint32_t cur_size;          /* current size of stored NDEF data (in bytes) */
  /* Flags to indicate NDEF capability, is formated, soft/hard lockable,
   * formatable, otp and read only */
  tNFA_RW_NDEF_FLAG flags;
} tNFA_NDEF_DETECT;

/* Structure for NFA_TLV_DETECT_EVT event data */
typedef struct {
  tNFA_STATUS status;         /* Status of the tlv detecton        */
  tNFA_NFC_PROTOCOL protocol; /* protocol used to detect TLV       */
  uint8_t num_tlvs;           /* number of tlvs present in the tag */
  uint8_t num_bytes;          /* number of lock/reserved bytes     */
} tNFA_TLV_DETECT;

/* Structure for NFA_DATA_EVT data */
typedef struct {
  tNFA_STATUS status; /* Status of Data received          */
  uint8_t* p_data;    /* Data buffer                      */
  uint16_t len;       /* Length of data                   */
} tNFA_RX_DATA;

/* Structure for NFA_CE_NDEF_WRITE_CPLT_EVT data */
typedef struct {
  tNFA_STATUS status; /* Status of the ndef write op      */
  uint32_t len;       /* Update length of NDEF data       */
  uint8_t* p_data;    /* data buffer                      */
} tNFA_CE_NDEF_WRITE_CPLT;

/* Data for NFA_LLCP_ACTIVATED_EVT */
typedef struct {
  bool is_initiator;        /* TRUE if initiator                */
  uint16_t remote_wks;      /* Well-Known service mask of peer  */
  uint8_t remote_lsc;       /* Link Service Class of peer       */
  uint16_t remote_link_miu; /* Link MIU of peer                 */
  uint16_t local_link_miu;  /* Link MIU of local                */
  uint8_t remote_version;   /* LLCP version of remote           */
} tNFA_LLCP_ACTIVATED;

/* Data for NFA_LLCP_DEACTIVATED_EVT */
typedef struct {
  uint8_t reason; /* reason of deactivation           */
} tNFA_LLCP_DEACTIVATED;

/* Data for NFA_I93_CMD_CPLT_EVT */
typedef struct {
  uint8_t dsfid;                 /* DSFID                       */
  uint8_t uid[I93_UID_BYTE_LEN]; /* UID[0]:MSB, ... UID[7]:LSB  */
} tNFA_I93_INVENTORY;

typedef struct /* RW_I93_SYS_INFO_EVT                          */
{
  uint8_t info_flags; /* information flags                            */
  uint8_t uid[I93_UID_BYTE_LEN]; /* UID */
  uint8_t dsfid;        /* DSFID if I93_INFO_FLAG_DSFID                 */
  uint8_t afi;          /* AFI if I93_INFO_FLAG_AFI                     */
  uint16_t num_block;   /* number of blocks if I93_INFO_FLAG_MEM_SIZE   */
  uint8_t block_size;   /* block size in byte if I93_INFO_FLAG_MEM_SIZE */
  uint8_t IC_reference; /* IC Reference if I93_INFO_FLAG_IC_REF         */
} tNFA_I93_SYS_INFO;

typedef struct {
  tNFA_STATUS status;   /* Status of sending command       */
  uint8_t sent_command; /* sent command to tag             */
  union {
    uint8_t error_code;           /* error code defined in ISO 15693 */
    tNFA_I93_INVENTORY inventory; /* inventory response              */
    tNFA_I93_SYS_INFO sys_info;   /* system information              */
  } params;
} tNFA_I93_CMD_CPLT;

/* Data for NFA_CE_REGISTERED_EVT */
typedef struct {
  tNFA_STATUS status; /* NFA_STATUS_OK if successful                      */
  tNFA_HANDLE handle; /* handle for NFA_CeRegisterFelicaSystemCodeOnDH () */
                      /*            NFA_CeRegisterT4tAidOnDH ()           */
} tNFA_CE_REGISTERED;

/* Data for NFA_CE_DEREGISTERED_EVT */
typedef struct {
  tNFA_HANDLE handle; /* handle from NFA_CE_REGISTERED_EVT   */
} tNFA_CE_DEREGISTERED;

/* Data for NFA_CE_ACTIVATED_EVT */
typedef struct {
  tNFA_STATUS status; /* NFA_STATUS_OK if successful              */
  tNFA_HANDLE handle; /* handle from NFA_CE_REGISTERED_EVT        */
  tNFC_ACTIVATE_DEVT activate_ntf; /* RF discovery activation details */
} tNFA_CE_ACTIVATED;

/* Data for NFA_CE_DEACTIVATED_EVT */
typedef struct {
  tNFA_HANDLE handle; /* handle from NFA_CE_REGISTERED_EVT   */
  /* NFA_DEACTIVATE_TYPE_IDLE or NFA_DEACTIVATE_TYPE_SLEEP */
  tNFA_DEACTIVATE_TYPE type;
} tNFA_CE_DEACTIVATED;

/* Structure for NFA_CE_DATA_EVT data */
typedef struct {
  tNFA_STATUS status; /* NFA_STATUS_OK if complete packet     */
  tNFA_HANDLE handle; /* handle from NFA_CE_REGISTERED_EVT    */
  uint8_t* p_data;    /* Data buffer                          */
  uint16_t len;       /* Length of data                       */
} tNFA_CE_DATA;

/* Union of all connection callback structures */
typedef union {
  tNFA_STATUS status;           /* NFA_POLL_ENABLED_EVT                 */
  tNFA_DISC_RESULT disc_result; /* NFA_DISC_RESULT_EVT                  */
  tNFA_ACTIVATED activated;     /* NFA_ACTIVATED_EVT                    */
  tNFA_DEACTIVATED deactivated; /* NFA_DEACTIVATED_EVT                  */
  tNFA_NDEF_DETECT ndef_detect; /* NFA_NDEF_DETECT_EVT                  */
  tNFA_TLV_DETECT tlv_detect;   /* NFA_TLV_DETECT_EVT                   */
  tNFA_RX_DATA data;            /* NFA_DATA_EVT                         */
  tNFA_CE_NDEF_WRITE_CPLT ndef_write_cplt; /* NFA_CE_NDEF_WRITE_CPLT_EVT */
  tNFA_LLCP_ACTIVATED llcp_activated; /* NFA_LLCP_ACTIVATED_EVT               */
  tNFA_LLCP_DEACTIVATED llcp_deactivated; /* NFA_LLCP_DEACTIVATED_EVT */
  tNFA_I93_CMD_CPLT i93_cmd_cplt;   /* NFA_I93_CMD_CPLT_EVT                 */
  tNFA_CE_REGISTERED ce_registered; /* NFA_CE_REGISTERED_EVT                */
  tNFA_CE_DEREGISTERED ce_deregistered; /* NFA_CE_DEREGISTERED_EVT */
  tNFA_CE_ACTIVATED ce_activated;     /* NFA_CE_ACTIVATED_EVT                 */
  tNFA_CE_DEACTIVATED ce_deactivated; /* NFA_CE_DEACTIVATED_EVT               */
  tNFA_CE_DATA ce_data;               /* NFA_CE_DATA_EVT                      */

} tNFA_CONN_EVT_DATA;

/* NFA Connection Callback */
typedef void(tNFA_CONN_CBACK)(uint8_t event, tNFA_CONN_EVT_DATA* p_data);

#ifndef NFA_DM_NUM_INTERFACE_MAP
#define NFA_DM_NUM_INTERFACE_MAP 3
#endif

/* compile-time configuration structure for the RF Discovery Frequency for each
 * technology */
typedef struct {
  uint8_t pa;   /* Frequency for NFC Technology A               */
  uint8_t pb;   /* Frequency for NFC Technology B               */
  uint8_t pf;   /* Frequency for NFC Technology F               */
  uint8_t pi93; /* Frequency for Proprietary Technology/15693   */
  uint8_t pbp;  /* Frequency for Proprietary Technology/B-Prime */
  uint8_t pk;   /* Frequency for Proprietary Technology/Kovio   */
  uint8_t paa;  /* Frequency for NFC Technology A active mode   */
  uint8_t pfa;  /* Frequency for NFC Technology F active mode   */
  uint8_t pacm; /* Frequency for NFC Technology active mode     */
} tNFA_DM_DISC_FREQ_CFG;

/* definitions for tNFA_DM_CFG.presence_check_option */
/* if NDEF is not supported by the tag, use sleep/wake(last interface) */
#define NFA_DM_PCO_ISO_SLEEP_WAKE 0x01
/* NFA_SendRawFrame() has been used, use empty I block for presence check
 * if this bit is not set, use read-binary on channel 3 for presence check */
#define NFA_DM_PCO_EMPTY_I_BLOCK 0x02

/* compile-time configuration structure */
typedef struct {
  /* Automatic NDEF detection (when not in exclusive RF mode) */
  bool auto_detect_ndef;
  /* Automatic NDEF read (when not in exclusive RF mode)      */
  bool auto_read_ndef;
  /* Automatic presence check                                 */
  bool auto_presence_check;
  /* Use sleep/wake(last interface) for ISODEP presence check */
  uint8_t presence_check_option;
  /* Maximum time to wait for presence check response         */
  uint16_t presence_check_timeout;
} tNFA_DM_CFG;

/* compile-time configuration structure for HCI */
typedef struct {
  /* Maximum idle(no HCP Pkt) time to wait for EE DISC REQ Ntf(s) */
  uint16_t hci_netwk_enable_timeout;
  /* Maximum time to wait for EE DISC REQ NTF(s) after HOT PLUG EVT(s) */
  uint16_t hcp_response_timeout;
  /* Number of host in the whitelist of Terminal host */
  uint8_t num_whitelist_host;
  /* Whitelist of Terminal Host */
  uint8_t* p_whitelist;
} tNFA_HCI_CFG;

/*
** Exclusive RF mode listen configuration
*/

#define NFA_LB_MAX_NFCID0_LEN 4
#define NFA_LF_MAX_SC_NFCID2 1
#define NFA_LA_MAX_HIST_BYTES 15
#define NFA_LB_MAX_H_INFO_LEN 15

typedef struct {
  /*
  ** Discovery Configuration Parameters for Listen A
  */
  bool la_enable;             /* TRUE if listening A                      */
  uint8_t la_bit_frame_sdd;   /* Bit Frame SDD in Byte 1 of SENS_RES      */
  uint8_t la_platform_config; /* Platform Config in Byte 2 of SENS_RES    */
  uint8_t la_sel_info;        /* Byte of SEL_RES                          */
  uint8_t la_nfcid1_len;      /* NFCID1 (0, 4, 7 or 10 bytes)             */
  uint8_t la_nfcid1[NCI_NFCID1_MAX_LEN]; /*        if empty, NFCC will decide */

  /*
  ** Discovery Configuration Parameters for Listen B
  */
  bool lb_enable;        /* TRUE if listening B                      */
  uint8_t lb_sensb_info; /* Byte 2 of Protocol Info within SENSB_RES */
  uint8_t lb_nfcid0_len; /* NFCID0 (0, 1 or 4 bytes)                 */
  uint8_t
      lb_nfcid0[NFA_LB_MAX_NFCID0_LEN]; /*         if empty, NFCC will decide */
  uint8_t lb_app_data[NCI_PARAM_LEN_LB_APPDATA]; /* Bytes 6 - 9 in SENSB_RES */
  uint8_t lb_sfgi;   /* Start-Up Frame Guard Time                */
  uint8_t lb_adc_fo; /* Byte 12 in SENSB_RES                     */

  /*
  ** Discovery Configuration Parameters for Listen F
  */
  bool lf_enable;           /* TRUE if listening F          */
  uint8_t lf_con_bitr_f;    /* bit rate to listen           */
  uint8_t lf_protocol_type; /* Supported Protocols          */
  /* bit field indicating which lf_t3t_identifier are enabled */
  uint16_t lf_t3t_flags;
  uint8_t lf_t3t_identifier[NFA_LF_MAX_SC_NFCID2]
                           [NCI_SYSTEMCODE_LEN + NCI_NFCID2_LEN];
  /* System Code and NFCID2       */
  uint8_t lf_t3t_pmm[NCI_T3T_PMM_LEN]; /* Bytes 10 - 17 in SENSF_RES   */

  /*
  ** Discovery Configuration Parameters for Listen ISO-DEP
  */
  bool li_enable;            /* TRUE if listening ISO-DEP            */
  uint8_t li_fwi;            /* Frame Waiting Time Integer           */
  uint8_t la_hist_bytes_len; /* historical bytes for Listen-A        */
  uint8_t la_hist_bytes[NFA_LA_MAX_HIST_BYTES];
  uint8_t lb_h_info_resp_len; /* higher layer response for Listen-B   */
  uint8_t lb_h_info_resp[NFA_LB_MAX_H_INFO_LEN];

  /*
  ** Discovery Configuration Parameters for Listen NFC-DEP
  */
  bool ln_enable;                   /* TRUE if listening NFC-DEP            */
  uint8_t ln_wt;                    /* Waiting Time Integer                 */
  uint8_t ln_atr_res_gen_bytes_len; /* General bytes in ATR_RES             */
  uint8_t ln_atr_res_gen_bytes[NCI_MAX_GEN_BYTES_LEN];
  uint8_t ln_atr_res_config; /* Optional parameters (PPt) in ATR_RES */
} tNFA_LISTEN_CFG;

/* Data for NFA_UpdateRFCommParams () */
typedef tNFC_RF_COMM_PARAMS tNFA_RF_COMM_PARAMS;

/* RF Interface type */
#define NFA_INTERFACE_FRAME NFC_INTERFACE_FRAME
#define NFA_INTERFACE_ISO_DEP NFC_INTERFACE_ISO_DEP
#define NFA_INTERFACE_NFC_DEP NFC_INTERFACE_NFC_DEP
#define NFA_PROTOCOL_ISO15693   NFC_PROTOCOL_15693
#define NFA_INTERFACE_MIFARE NFC_INTERFACE_MIFARE
typedef tNFC_INTF_TYPE tNFA_INTF_TYPE;

/*******************************************************************************
** NDEF Definitions
*******************************************************************************/

/* Definitions for tNFA_TNF (NDEF type name format ID) */
/* Empty or no type specified                       */
#define NFA_TNF_EMPTY NDEF_TNF_EMPTY
/* NFC Forum well-known type [NFC RTD]              */
#define NFA_TNF_WKT NDEF_TNF_WKT
/* Media-type as defined in RFC 2046 [RFC 2046]     */
#define NFA_TNF_RFC2046_MEDIA NDEF_TNF_MEDIA
/* Absolute URI as defined in RFC 3986 [RFC 3986]   */
#define NFA_TNF_RFC3986_URI NDEF_TNF_URI
/* NFC Forum external type [NFC RTD]                */
#define NFA_TNF_EXTERNAL NDEF_TNF_EXT
/* Unknown                                          */
#define NFA_TNF_UNKNOWN NDEF_TNF_UNKNOWN
/* Unchanged                                        */
#define NFA_TNF_UNCHANGED NDEF_TNF_UNCHANGED
/* Reserved                                         */
#define NFA_TNF_RESERVED NDEF_TNF_RESERVED
/* Used to register default NDEF type handler       */
#define NFA_TNF_DEFAULT 0xFF
typedef uint8_t tNFA_TNF;

/* Definitions for tNFA_NDEF_URI_ID (Frequently used prefixes. For additional
 * values, see [NFC RTD URI] */
#define NFA_NDEF_URI_ID_ABSOLUTE 0x00 /* Unabridged URI.  */
#define NFA_NDEF_URI_ID_HTTP 0x03     /* http://          */
#define NFA_NDEF_URI_ID_HTTPS 0x04    /* https://         */
#define NFA_NDEF_URI_ID_TEL 0x05      /* tel:             */
#define NFA_NDEF_URI_ID_MAILTO 0x06   /* mailto:          */
#define NFA_NDEF_URI_ID_FTP 0x0D      /* ftp://           */
#define NFA_NDEF_URI_ID_FILE 0x1D     /* file://          */

typedef uint8_t tNFA_NDEF_URI_ID;

/* Events for tNFA_NDEF_CBACK */
/* NDEF record type registered. (In response to NFA_RegisterNDefTypeHandler) */
#define NFA_NDEF_REGISTER_EVT 0
/* Received an NDEF message with the registered type. See [tNFA_NDEF_DATA] */
#define NFA_NDEF_DATA_EVT 1
typedef uint8_t tNFA_NDEF_EVT;

/* Structure for NFA_NDEF_REGISTER_EVT event data */
typedef struct {
  tNFA_STATUS status;           /* Status of the registration               */
  tNFA_HANDLE ndef_type_handle; /* Handle for this NDEF type registration.  */
} tNFA_NDEF_REGISTER;

/* Structure for NFA_NDEF_DATA_EVT event data */
typedef struct {
  tNFA_HANDLE ndef_type_handle; /* Handle for NDEF type registration.   */
  uint8_t* p_data;              /* Data buffer                          */
  uint32_t len;                 /* Length of data                       */
} tNFA_NDEF_DATA;

/* Union of all NDEF callback structures */
typedef union {
  /* Structure for NFA_NDEF_REGISTER_EVT event data */
  tNFA_NDEF_REGISTER ndef_reg;
  /* Structure for NFA_NDEF_DATA_EVT event data */
  tNFA_NDEF_DATA ndef_data;
} tNFA_NDEF_EVT_DATA;

/* NFA_NDEF callback */
typedef void(tNFA_NDEF_CBACK)(tNFA_NDEF_EVT event, tNFA_NDEF_EVT_DATA* p_data);

/* NFA VSC Callback */
typedef void(tNFA_VSC_CBACK)(uint8_t event, uint16_t param_len,
                             uint8_t* p_param);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         NFA_Init
**
** Description      This function initializes control blocks for NFA
**
**                  p_hal_entry_tbl points to a table of HAL entry points
**
**                  NOTE: the buffer that p_hal_entry_tbl points must be
**                  persistent until NFA is disabled.
**
**
** Returns          none
**
*******************************************************************************/
extern void NFA_Init(tHAL_NFC_ENTRY* p_hal_entry_tbl);

/*******************************************************************************
**
** Function         NFA_Enable
**
** Description      This function enables NFC. Prior to calling NFA_Enable,
**                  the NFCC must be powered up, and ready to receive commands.
**                  This function enables the tasks needed by NFC, opens the NCI
**                  transport, resets the NFC controller, downloads patches to
**                  the NFCC (if necessary), and initializes the NFC subsystems.
**
**                  This function should only be called once - typically when
**                  NFC is enabled during boot-up, or when NFC is enabled from a
**                  settings UI. Subsequent calls to NFA_Enable while NFA is
**                  enabling or enabled will be ignored. When the NFC startup
**                  procedure is completed, an NFA_DM_ENABLE_EVT is returned to
**                  the application using the tNFA_DM_CBACK.
**
**                  The tNFA_CONN_CBACK parameter is used to register a callback
**                  for polling, p2p and card emulation events.
**
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_Enable(tNFA_DM_CBACK* p_dm_cback,
                              tNFA_CONN_CBACK* p_conn_cback);

/*******************************************************************************
**
** Function         NFA_Disable
**
** Description      This function is called to shutdown NFC. The tasks for NFC
**                  are terminated, and clean up routines are performed. This
**                  function is typically called during platform shut-down, or
**                  when NFC is disabled from a settings UI. When the NFC
**                  shutdown procedure is completed, an NFA_DM_DISABLE_EVT is
**                  returned to the application using the tNFA_DM_CBACK.
**
**                  The platform should wait until the NFC_DISABLE_REVT is
**                  received before powering down the NFC chip and NCI
**                  transport. This is required to so that NFA can gracefully
**                  shut down any open connections.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_Disable(bool graceful);

/*******************************************************************************
**
** Function         NFA_SetConfig
**
** Description      Set the configuration parameters to NFCC. The result is
**                  reported with an NFA_DM_SET_CONFIG_EVT in the tNFA_DM_CBACK
**                  callback.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function. Most Configuration
**                  parameters are related to RF discovery.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BUSY if previous setting is on-going
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_SetConfig(tNFA_PMID param_id, uint8_t length,
                                 uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_GetConfig
**
** Description      Get the configuration parameters from NFCC. The result is
**                  reported with an NFA_DM_GET_CONFIG_EVT in the tNFA_DM_CBACK
**                  callback.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_GetConfig(uint8_t num_ids, tNFA_PMID* p_param_ids);

/*******************************************************************************
**
** Function         NFA_RequestExclusiveRfControl
**
** Description      Request exclusive control of NFC.
**                  - Previous behavior (polling/tag reading, DH card emulation)
**                    will be suspended .
**                  - Polling and listening will be done based on the specified
**                    params
**
**                  The NFA_EXCLUSIVE_RF_CONTROL_STARTED_EVT event of
**                  tNFA_CONN_CBACK indicates the status of the operation.
**
**                  NFA_ACTIVATED_EVT and NFA_DEACTIVATED_EVT indicates link
**                  activation/deactivation.
**
**                  NFA_SendRawFrame is used to send data to the peer.
**                  NFA_DATA_EVT indicates data from the peer.
**
**                  If a tag is activated, then the NFA_RW APIs may be used to
**                  send commands to the tag. Incoming NDEF messages are sent to
**                  the NDEF callback.
**
**                  Once exclusive RF control has started, NFA will not activate
**                  LLCP internally. The application has exclusive control of
**                  the link.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RequestExclusiveRfControl(tNFA_TECHNOLOGY_MASK poll_mask,
                                                 tNFA_LISTEN_CFG* p_listen_cfg,
                                                 tNFA_CONN_CBACK* p_conn_cback,
                                                 tNFA_NDEF_CBACK* p_ndef_cback);

/*******************************************************************************
**
** Function         NFA_ReleaseExclusiveRfControl
**
** Description      Release exclusive control of NFC. Once released, behavior
**                  prior to obtaining exclusive RF control will resume.
**
Note??
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_ReleaseExclusiveRfControl(void);

/*******************************************************************************
**
** Function         NFA_EnablePolling
**
** Description      Enable polling for technologies specified by poll_mask.
**
**                  The following events (notified using the connection
**                  callback registered with NFA_Enable) are generated during
**                  polling:
**
**                  - NFA_POLL_ENABLED_EVT indicates whether or not polling
**                    successfully enabled.
**                  - NFA_DISC_RESULT_EVT indicates there are more than one
**                    devices, so application must select one of tags by calling
**                    NFA_Select().
**                  - NFA_SELECT_RESULT_EVT indicates whether previous selection
**                    was successful or not. If it was failed then application
**                    must select again or deactivate by calling
**                    NFA_Deactivate().
**                  - NFA_ACTIVATED_EVT is generated when an NFC link is
**                    activated.
**                  - NFA_NDEF_DETECT_EVT is generated if tag is activated
**                  - NFA_LLCP_ACTIVATED_EVT/NFA_LLCP_DEACTIVATED_EVT is
**                    generated if NFC-DEP is activated
**                  - NFA_DEACTIVATED_EVT will be returned after deactivating
**                    NFC link.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_EnablePolling(tNFA_TECHNOLOGY_MASK poll_mask);

/*******************************************************************************
**
** Function         NFA_DisablePolling
**
** Description      Disable polling
**                  NFA_POLL_DISABLED_EVT will be returned after stopping
**                  polling.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_DisablePolling(void);

/*******************************************************************************
**
** Function         NFA_EnableListening
**
** Description      Enable listening.
**                  NFA_LISTEN_ENABLED_EVT will be returned after listening is
**                  allowed.
**
**                  The actual listening technologies are specified by other NFA
**                  API functions. Such functions include (but not limited to)
**                  NFA_CeConfigureUiccListenTech.
**                  If NFA_DisableListening () is called to ignore the listening
**                  technologies, NFA_EnableListening () is called to restore
**                  the listening technologies set by these functions.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_EnableListening(void);

/*******************************************************************************
**
** Function         NFA_DisableListening
**
** Description      Disable listening
**                  NFA_LISTEN_DISABLED_EVT will be returned after stopping
**                  listening. This function is called to exclude listen at RF
**                  discovery.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_DisableListening(void);

/*******************************************************************************
**
** Function         NFA_PauseP2p
**
** Description      Pause P2P services.
**                  NFA_P2P_PAUSED_EVT will be returned after P2P services are
**                  disabled.
**
**                  The P2P services enabled by NFA_P2p* API functions are not
**                  available. NFA_ResumeP2p() is called to resume the P2P
**                  services.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_PauseP2p(void);

/*******************************************************************************
**
** Function         NFA_ResumeP2p
**
** Description      Resume P2P services.
**                  NFA_P2P_RESUMED_EVT will be returned after P2P services are.
**                  enables again.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_ResumeP2p(void);

/*******************************************************************************
**
** Function         NFA_SetP2pListenTech
**
** Description      This function is called to set listen technology for
**                  NFC-DEP. This funtion may be called before or after starting
**                  any server on NFA P2P/CHO/SNEP.
**                  If there is no technology for NFC-DEP, P2P listening will be
**                  stopped.
**
**                  NFA_SET_P2P_LISTEN_TECH_EVT without data will be returned.
**
** Note:            If RF discovery is started,
**                  NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT should
**                  happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_SetP2pListenTech(tNFA_TECHNOLOGY_MASK tech_mask);

/*******************************************************************************
**
** Function         NFA_StartRfDiscovery
**
** Description      Start RF discovery
**                  RF discovery parameters shall be set by other APIs.
**
**                  An NFA_RF_DISCOVERY_STARTED_EVT indicates whether starting
**                  was successful or not.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_StartRfDiscovery(void);

/*******************************************************************************
**
** Function         NFA_StopRfDiscovery
**
** Description      Stop RF discovery
**
**                  An NFA_RF_DISCOVERY_STOPPED_EVT indicates whether stopping
**                  was successful or not.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_StopRfDiscovery(void);

/*******************************************************************************
**
** Function         NFA_SetRfDiscoveryDuration
**
** Description      Set the duration of the single discovery period in [ms].
**                  Allowable range: 0 ms to 0xFFFF ms.
**
** Note:            If discovery is already started, the application should
**                  call NFA_StopRfDiscovery prior to calling
**                  NFA_SetRfDiscoveryDuration, and then call
**                  NFA_StartRfDiscovery afterwards to restart discovery using
**                  the new duration.
**
** Returns:
**                  NFA_STATUS_OK, if command accepted
**                  NFA_STATUS_FAILED: otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_SetRfDiscoveryDuration(uint16_t discovery_period_ms);

/*******************************************************************************
**
** Function         NFA_Select
**
** Description      Select one from detected devices by NFA_DISC_RESULT_EVT
**                  after the last discovery result is received.
**                  An NFA_SELECT_RESULT_EVT indicates whether selection was
**                  successful or not. If failed then application must select
**                  again or deactivate by NFA_Deactivate ().
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_INVALID_PARAM if RF interface is not matched
**                  protocol
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_Select(uint8_t rf_disc_id, tNFA_NFC_PROTOCOL protocol,
                              tNFA_INTF_TYPE rf_interface);

/*******************************************************************************
**
** Function         NFA_UpdateRFCommParams
**
** Description      This function is called to update RF Communication
**                  parameters once the Frame RF Interface has been activated.
**
**                  An NFA_UPDATE_RF_PARAM_RESULT_EVT indicates whether updating
**                  was successful or not.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_UpdateRFCommParams(tNFA_RF_COMM_PARAMS* p_params);

/*******************************************************************************
**
** Function         NFA_Deactivate
**
** Description
**                  If sleep_mode=TRUE:
**                      Deselect the activated device by deactivating into sleep
**                      mode.
**
**                      An NFA_DEACTIVATE_FAIL_EVT indicates that selection was
**                      not successful. Application can select another
**                      discovered device or deactivate by NFA_Deactivate ()
**                      after receiving NFA_DEACTIVATED_EVT.
**
**                      Deactivating to sleep mode is not allowed when NFCC is
**                      in wait-for-host-select mode, or in listen-sleep states;
**                      NFA will deactivate to idle or discovery state for these
**                      cases respectively.
**
**
**                  If sleep_mode=FALSE:
**                      Deactivate the connection (e.g. as a result of presence
**                      check failure) NFA_DEACTIVATED_EVT will indicate that
**                      link is deactivated. Polling/listening will resume
**                      (unless the nfcc is in wait_for-all-discoveries state)
**
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_Deactivate(bool sleep_mode);

/*******************************************************************************
**
** Function         NFA_SendRawFrame
**
** Description      Send a raw frame over the activated interface with the NFCC.
**                  This function can only be called after NFC link is
**                  activated.
**
**                  If the activated interface is a tag and auto-presence check
**                  is enabled then presence_check_start_delay can be used to
**                  indicate the delay in msec after which the next auto
**                  presence check command can be sent.
**                  NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY can be used as the
**                  default value for the delay.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_SendRawFrame(uint8_t* p_raw_data, uint16_t data_len,
                                    uint16_t presence_check_start_delay);

/*******************************************************************************
** NDEF APIs
*******************************************************************************/

/*******************************************************************************
**
** Function         NFA_RegisterNDefTypeHandler
**
** Description      This function allows the applications to register for
**                  specific types of NDEF records. When NDEF records are
**                  received, NFA will parse the record-type field, and pass
**                  the record to the registered tNFA_NDEF_CBACK.
**
**                  For records types which were not registered, the record will
**                  be sent to the default handler. A default type-handler may
**                  be registered by calling this NFA_RegisterNDefTypeHandler
**                  with tnf=NFA_TNF_DEFAULT. In this case, all un-registered
**                  record types will be sent to the callback. Only one default
**                  handler may be registered at a time.
**
**                  An NFA_NDEF_REGISTER_EVT will be sent to the tNFA_NDEF_CBACK
**                  to indicate that registration was successful, and provide a
**                  handle for this record type.
**
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RegisterNDefTypeHandler(bool handle_whole_message,
                                               tNFA_TNF tnf,
                                               uint8_t* p_type_name,
                                               uint8_t type_name_len,
                                               tNFA_NDEF_CBACK* p_ndef_cback);

/*******************************************************************************
**
** Function         NFA_RegisterNDefUriHandler
**
** Description      This API is a special-case of NFA_RegisterNDefTypeHandler
**                  with TNF=NFA_TNF_WKT, and type_name='U' (URI record); and
**                  allows registering for specific URI types (e.g. 'tel:' or
**                  'mailto:').
**
**                  An NFA_NDEF_REGISTER_EVT will be sent to the tNFA_NDEF_CBACK
**                  to indicate that registration was successful, and provide a
**                  handle for this registration.
**
**                  If uri_id=NFA_NDEF_URI_ID_ABSOLUTE, then p_abs_uri contains
**                  the unabridged URI. For all other uri_id values, the
**                  p_abs_uri parameter is ignored (i.e the URI prefix is
**                  implied by uri_id).
**
**                  See [NFC RTD URI] for more information.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RegisterNDefUriHandler(bool handle_whole_message,
                                              tNFA_NDEF_URI_ID uri_id,
                                              uint8_t* p_abs_uri,
                                              uint8_t uri_id_len,
                                              tNFA_NDEF_CBACK* p_ndef_cback);

/*******************************************************************************
**
** Function         NFA_DeregisterNDefTypeHandler
**
** Description      Deregister NDEF record type handler.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_DeregisterNDefTypeHandler(tNFA_HANDLE ndef_type_handle);

/*******************************************************************************
**
** Function         NFA_PowerOffSleepMode
**
** Description      This function is called to enter or leave NFCC Power Off
**                  Sleep mode
**                  NFA_DM_PWR_MODE_CHANGE_EVT will be sent to indicate status.
**
**                  start_stop : TRUE if entering Power Off Sleep mode
**                               FALSE if leaving Power Off Sleep mode
**
Note??
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_PowerOffSleepMode(bool start_stop);

/*******************************************************************************
**
** Function         NFA_RegVSCback
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
extern tNFC_STATUS NFA_RegVSCback(bool is_register, tNFA_VSC_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFA_SendVsCommand
**
** Description      This function is called to send an NCI Vendor Specific
**                  command to NFCC.
**
**                  oid             - The opcode of the VS command.
**                  cmd_params_len  - The command parameter len
**                  p_cmd_params    - The command parameter
**                  p_cback         - The callback function to receive the
**                                    command status
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_SendVsCommand(uint8_t oid, uint8_t cmd_params_len,
                                     uint8_t* p_cmd_params,
                                     tNFA_VSC_CBACK* p_cback);

/*******************************************************************************
**
** Function         NFA_SendRawVsCommand
**
** Description      This function is called to send raw vendor specific
**                  command to NFCC.
**
**                  cmd_params_len  - The command parameter len
**                  p_cmd_params    - The command parameter
**                  p_cback         - The callback function to receive the
**                                    command
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/

extern tNFA_STATUS NFA_SendRawVsCommand(uint8_t cmd_params_len,
                                        uint8_t* p_cmd_params,
                                        tNFA_VSC_CBACK* p_cback);

/*******************************************************************************
** Function:        NFA_EnableDTA_TypeMode
**
** Description:     Initialize and get global DTA type mode from .conf
**
** Returns:         none:
**
*******************************************************************************/
extern void NFA_EnableDtamode(tNFA_eDtaModes eDtaMode);

/*******************************************************************************
** Function         NFA_GetNCIVersion
**
** Description      Called by higher layer to get the current nci
**                  version of nfc.
**
** Returns          NCI version NCI2.0 / NCI1.0
**
*******************************************************************************/
extern uint8_t NFA_GetNCIVersion();

/*******************************************************************************
** Function:        NFA_SetPowerSubStateForScreenState
**
** Description:     This function send the current screen state
**
** Returns:        NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
*******************************************************************************/
extern tNFA_STATUS NFA_SetPowerSubStateForScreenState(uint8_t ScreenState);

#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         FA_SendNxpNciCommand
**
** Description      This function is called to send NXP NCI Vendor Specific
**                  command to NFCC.
**
**                  cmd_params_len  - The command parameter len
**                  p_cmd_params    - The command parameter
**                  p_cback         - The callback function to receive the command
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/

NFC_API extern tNFA_STATUS NFA_SendNxpNciCommand (UINT8            cmd_params_len,
                                                  UINT8            *p_cmd_params,
                                                  tNFA_VSC_CBACK   *p_cback);

#endif

/*******************************************************************************
**
** Function         NFA_SetTraceLevel
**
** Description      This function sets the trace level for NFA.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
NFC_API extern UINT8 NFA_SetTraceLevel (UINT8 new_level);

#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         NFA_SetReaderMode
**
** Description:
**      This function enable/disable  reader mode. In reader mode, even though if
**      P2P & CE from UICC is detected, Priority will be given to TypeF UICC read.
**      Its currently implemented for TypeF
**
**      ReaderModeFlag - Enable/Disable Reader Mode
**      Technologies   - Type of technologies to be set for Reader mode
**                       Currently not used and reader mode is enabled for TypeF Only
**
** Returns:
**      void
*******************************************************************************/
NFC_API extern void NFA_SetReaderMode (BOOLEAN ReaderModeFlag, UINT32 Technologies);

NFC_API extern void  NFA_EnableDtamode (tNFA_eDtaModes eDtaMode);

/*******************************************************************************
**
** Function:        NFA_GetMwVersion
**
** Description:     This function gets the Middleware Version
**
** Returns:         First 8 bit Major Version
**                  Last 8 bit Minor Version
**
*******************************************************************************/
NFC_API extern tNFA_MW_VERSION NFA_GetMwVersion ();
#endif

#endif /* NFA_API_H */
