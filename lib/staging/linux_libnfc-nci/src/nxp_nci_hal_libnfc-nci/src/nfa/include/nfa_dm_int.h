/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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
 *  This is the private interface file for the NFA device manager.
 *
 ******************************************************************************/
#ifndef NFA_DM_INT_H
#define NFA_DM_INT_H

#include <string>
#include "nfa_api.h"
#include "nfa_sys.h"
#include "nfc_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* DM events */
enum {
  /* device manager local device API events */
  NFA_DM_API_ENABLE_EVT = NFA_SYS_EVT_START(NFA_ID_DM),
  NFA_DM_API_DISABLE_EVT,
  NFA_DM_API_SET_CONFIG_EVT,
  NFA_DM_API_GET_CONFIG_EVT,
  NFA_DM_API_REQUEST_EXCL_RF_CTRL_EVT,
  NFA_DM_API_RELEASE_EXCL_RF_CTRL_EVT,
  NFA_DM_API_ENABLE_POLLING_EVT,
  NFA_DM_API_DISABLE_POLLING_EVT,
  NFA_DM_API_ENABLE_LISTENING_EVT,
  NFA_DM_API_DISABLE_LISTENING_EVT,
  NFA_DM_API_PAUSE_P2P_EVT,
  NFA_DM_API_RESUME_P2P_EVT,
  NFA_DM_API_RAW_FRAME_EVT,
  NFA_DM_API_SET_P2P_LISTEN_TECH_EVT,
  NFA_DM_API_START_RF_DISCOVERY_EVT,
  NFA_DM_API_STOP_RF_DISCOVERY_EVT,
  NFA_DM_API_SET_RF_DISC_DURATION_EVT,
  NFA_DM_API_SELECT_EVT,
  NFA_DM_API_UPDATE_RF_PARAMS_EVT,
  NFA_DM_API_DEACTIVATE_EVT,
  NFA_DM_API_POWER_OFF_SLEEP_EVT,
  NFA_DM_API_REG_NDEF_HDLR_EVT,
  NFA_DM_API_DEREG_NDEF_HDLR_EVT,
  NFA_DM_API_REG_VSC_EVT,
  NFA_DM_API_SEND_VSC_EVT,
  NFA_DM_TIMEOUT_DISABLE_EVT,
  NFA_DM_API_SET_POWER_SUB_STATE_EVT,
  NFA_DM_API_SEND_RAW_VS_EVT,
  NFA_DM_MAX_EVT
};

/* data type for NFA_DM_API_ENABLE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_DM_CBACK* p_dm_cback;
  tNFA_CONN_CBACK* p_conn_cback;
} tNFA_DM_API_ENABLE;

/* data type for NFA_DM_API_DISABLE_EVT */
typedef struct {
  NFC_HDR hdr;
  bool graceful;
} tNFA_DM_API_DISABLE;

/* data type for NFA_DM_API_SET_CONFIG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_PMID param_id;
  uint8_t length;
  uint8_t* p_data;
} tNFA_DM_API_SET_CONFIG;

/* data type for NFA_DM_API_GET_CONFIG_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t num_ids;
  tNFA_PMID* p_pmids;
} tNFA_DM_API_GET_CONFIG;

/* data type for NFA_DM_API_REQ_EXCL_RF_CTRL_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_TECHNOLOGY_MASK poll_mask;
  tNFA_LISTEN_CFG listen_cfg;
  tNFA_CONN_CBACK* p_conn_cback;
  tNFA_NDEF_CBACK* p_ndef_cback;
} tNFA_DM_API_REQ_EXCL_RF_CTRL;

/* data type for NFA_DM_API_ENABLE_POLLING_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_TECHNOLOGY_MASK poll_mask;
} tNFA_DM_API_ENABLE_POLL;

/* data type for NFA_DM_API_SET_P2P_LISTEN_TECH_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_TECHNOLOGY_MASK tech_mask;
} tNFA_DM_API_SET_P2P_LISTEN_TECH;

/* data type for NFA_DM_API_SELECT_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t rf_disc_id;
  tNFA_NFC_PROTOCOL protocol;
  tNFA_INTF_TYPE rf_interface;
} tNFA_DM_API_SELECT;

/* data type for NFA_DM_API_UPDATE_RF_PARAMS_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_RF_COMM_PARAMS params;
} tNFA_DM_API_UPDATE_RF_PARAMS;

/* data type for NFA_DM_API_DEACTIVATE_EVT */
typedef struct {
  NFC_HDR hdr;
  bool sleep_mode;
} tNFA_DM_API_DEACTIVATE;

/* data type for NFA_DM_API_SET_RF_DISC_DURATION_EVT */
typedef struct {
  NFC_HDR hdr;
  uint16_t rf_disc_dur_ms;
} tNFA_DM_API_SET_RF_DISC_DUR;

/* data type for NFA_DM_API_REG_NDEF_HDLR_EVT */
#define NFA_NDEF_FLAGS_HANDLE_WHOLE_MESSAGE 0x01
#define NFA_NDEF_FLAGS_WKT_URI 0x02
#define NFA_NDEF_FLAGS_WHOLE_MESSAGE_NOTIFIED 0x04

typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE ndef_type_handle;
  uint8_t flags;
  tNFA_NDEF_CBACK* p_ndef_cback;
  tNFA_TNF tnf; /* Type-name field of record-type that was registered. */
  tNFA_NDEF_URI_ID
      uri_id; /* URI prefix abrieviation (for NFA_RegisterNDefUriHandler) */
  uint8_t name_len; /* Length of type name or absolute URI */
  uint8_t name[1];  /* Type name or absolute URI of record-type that got was
                       registered.    */
} tNFA_DM_API_REG_NDEF_HDLR;

/* data type for NFA_DM_API_DEREG_NDEF_HDLR_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE ndef_type_handle;
} tNFA_DM_API_DEREG_NDEF_HDLR;

/* data type for NFA_DM_API_REG_VSC_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_VSC_CBACK* p_cback;
  bool is_register;
} tNFA_DM_API_REG_VSC;

/* data type for NFA_DM_API_SEND_VSC_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_VSC_CBACK* p_cback;
  uint8_t oid;
  uint8_t cmd_params_len;
  uint16_t pad; /* add padding to ensure the size is big enough for
                   offset=NCI_VSC_MSG_HDR_SIZE */
  uint8_t* p_cmd_params;
} tNFA_DM_API_SEND_VSC;

/* data type for NFA_DM_API_SET_POWER_SUB_STATE_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t screen_state;
} tNFA_DM_API_SET_POWER_SUB_STATE;

/* union of all data types */
typedef union {
  /* GKI event buffer header */
  NFC_HDR hdr;                      /* NFA_DM_API_RAW_FRAME_EVT             */
                                    /* NFA_DM_API_MULTI_TECH_RSP_EVT        */
                                    /* NFA_DM_API_RELEASE_EXCL_RF_CTRL      */
                                    /* NFA_DM_API_DISABLE_POLLING_EVT       */
                                    /* NFA_DM_API_START_RF_DISCOVERY_EVT    */
                                    /* NFA_DM_API_STOP_RF_DISCOVERY_EVT     */
  tNFA_DM_API_ENABLE enable;        /* NFA_DM_API_ENABLE_EVT                */
  tNFA_DM_API_DISABLE disable;      /* NFA_DM_API_DISABLE_EVT               */
  tNFA_DM_API_SET_CONFIG setconfig; /* NFA_DM_API_SET_CONFIG_EVT            */
  tNFA_DM_API_GET_CONFIG getconfig; /* NFA_DM_API_GET_CONFIG_EVT            */
  tNFA_DM_API_SET_RF_DISC_DUR
      disc_duration; /* NFA_DM_API_SET_RF_DISC_DURATION_EVT  */
  tNFA_DM_API_REG_NDEF_HDLR reg_ndef_hdlr; /* NFA_DM_API_REG_NDEF_HDLR_EVT */
  tNFA_DM_API_DEREG_NDEF_HDLR
      dereg_ndef_hdlr; /* NFA_DM_API_DEREG_NDEF_HDLR_EVT       */
  tNFA_DM_API_REQ_EXCL_RF_CTRL
      req_excl_rf_ctrl; /* NFA_DM_API_REQUEST_EXCL_RF_CTRL      */
  tNFA_DM_API_ENABLE_POLL enable_poll; /* NFA_DM_API_ENABLE_POLLING_EVT */
  tNFA_DM_API_SET_P2P_LISTEN_TECH
      set_p2p_listen_tech;   /* NFA_DM_API_SET_P2P_LISTEN_TECH_EVT   */
  tNFA_DM_API_SELECT select; /* NFA_DM_API_SELECT_EVT                */
  tNFA_DM_API_UPDATE_RF_PARAMS
      update_rf_params;              /* NFA_DM_API_UPDATE_RF_PARAMS_EVT      */
  tNFA_DM_API_DEACTIVATE deactivate; /* NFA_DM_API_DEACTIVATE_EVT            */
  tNFA_DM_API_SEND_VSC send_vsc;     /* NFA_DM_API_SEND_VSC_EVT              */
  tNFA_DM_API_REG_VSC reg_vsc;       /* NFA_DM_API_REG_VSC_EVT               */
  /* NFA_DM_API_SET_POWER_SUB_STATE_EVT */
  tNFA_DM_API_SET_POWER_SUB_STATE set_power_state;
} tNFA_DM_MSG;

/* DM RF discovery state */
enum {
  NFA_DM_RFST_IDLE,               /* idle state                     */
  NFA_DM_RFST_DISCOVERY,          /* discovery state                */
  NFA_DM_RFST_W4_ALL_DISCOVERIES, /* wait for all discoveries state */
  NFA_DM_RFST_W4_HOST_SELECT,     /* wait for host selection state  */
  NFA_DM_RFST_POLL_ACTIVE,        /* poll mode activated state      */
  NFA_DM_RFST_LISTEN_ACTIVE,      /* listen mode activated state    */
  NFA_DM_RFST_LISTEN_SLEEP,       /* listen mode sleep state        */
  NFA_DM_RFST_LP_LISTEN,          /* Listening in Low Power mode    */
  NFA_DM_RFST_LP_ACTIVE           /* Activated in Low Power mode    */
};
typedef uint8_t tNFA_DM_RF_DISC_STATE;

/* DM RF discovery state machine event */
enum {
  NFA_DM_RF_DISCOVER_CMD,        /* start RF discovery                    */
  NFA_DM_RF_DISCOVER_RSP,        /* discover response from NFCC           */
  NFA_DM_RF_DISCOVER_NTF,        /* RF discovery NTF from NFCC            */
  NFA_DM_RF_DISCOVER_SELECT_CMD, /* select discovered target              */
  NFA_DM_RF_DISCOVER_SELECT_RSP, /* select response from NFCC             */
  NFA_DM_RF_INTF_ACTIVATED_NTF,  /* RF interface activation NTF from NFCC */
  NFA_DM_RF_DEACTIVATE_CMD,      /* deactivate RF interface               */
  NFA_DM_RF_DEACTIVATE_RSP,      /* deactivate response from NFCC         */
  NFA_DM_RF_DEACTIVATE_NTF,      /* deactivate RF interface NTF from NFCC */
  NFA_DM_LP_LISTEN_CMD,          /* NFCC is listening in low power mode   */
  NFA_DM_CORE_INTF_ERROR_NTF,    /* RF interface error NTF from NFCC      */
  NFA_DM_DISC_SM_MAX_EVENT
};
typedef uint8_t tNFA_DM_RF_DISC_SM_EVENT;

/* DM RF discovery state machine data */
typedef struct {
  uint8_t rf_disc_id;
  tNFA_NFC_PROTOCOL protocol;
  tNFA_INTF_TYPE rf_interface;
} tNFA_DM_DISC_SELECT_PARAMS;

typedef union {
  tNFC_DISCOVER nfc_discover;        /* discovery data from NFCC    */
  tNFC_DEACT_TYPE deactivate_type;   /* deactivation type           */
  tNFA_DM_DISC_SELECT_PARAMS select; /* selected target information */
} tNFA_DM_RF_DISC_DATA;

/* Callback event from NFA DM RF Discovery to other NFA sub-modules */
enum {
  NFA_DM_RF_DISC_START_EVT, /* discovery started with protocol, technology and
                               mode       */
  NFA_DM_RF_DISC_ACTIVATED_EVT,  /* activated with configured protocol,
                                    technology and mode    */
  NFA_DM_RF_DISC_DEACTIVATED_EVT /* deactivated sleep or idle */
};
typedef uint8_t tNFA_DM_RF_DISC_EVT;

/* Combined NFC Technology and protocol bit mask */
#define NFA_DM_DISC_MASK_PA_T1T 0x00000001
#define NFA_DM_DISC_MASK_PA_T2T 0x00000002
#define NFA_DM_DISC_MASK_PA_ISO_DEP 0x00000004
#define NFA_DM_DISC_MASK_PA_NFC_DEP 0x00000008
#define NFA_DM_DISC_MASK_PB_ISO_DEP 0x00000010
#define NFA_DM_DISC_MASK_PF_T3T 0x00000020
#define NFA_DM_DISC_MASK_PF_NFC_DEP 0x00000040
#define NFA_DM_DISC_MASK_P_T5T 0x00000100
#define NFA_DM_DISC_MASK_P_B_PRIME 0x00000200
#define NFA_DM_DISC_MASK_P_KOVIO 0x00000400
#define NFA_DM_DISC_MASK_PAA_NFC_DEP 0x00000800
#define NFA_DM_DISC_MASK_PACM_NFC_DEP 0x00000800
#define NFA_DM_DISC_MASK_PFA_NFC_DEP 0x00001000
/* Legacy/proprietary/non-NFC Forum protocol (e.g Shanghai transit card) */
#define NFA_DM_DISC_MASK_P_LEGACY 0x00002000
#define NFA_DM_DISC_MASK_PA_MIFARE 0x00004000
#define NFA_DM_DISC_MASK_POLL 0x0000FFFF

#define NFA_DM_DISC_MASK_LA_T1T 0x00010000
#define NFA_DM_DISC_MASK_LA_T2T 0x00020000
#define NFA_DM_DISC_MASK_LA_ISO_DEP 0x00040000
#define NFA_DM_DISC_MASK_LA_NFC_DEP 0x00080000
#define NFA_DM_DISC_MASK_LB_ISO_DEP 0x00100000
#define NFA_DM_DISC_MASK_LF_T3T 0x00200000
#define NFA_DM_DISC_MASK_LF_NFC_DEP 0x00400000
#define NFA_DM_DISC_MASK_L_ISO15693 0x01000000
#define NFA_DM_DISC_MASK_L_B_PRIME 0x02000000
#define NFA_DM_DISC_MASK_LACM_NFC_DEP 0x04000000
#define NFA_DM_DISC_MASK_LAA_NFC_DEP 0x04000000
#define NFA_DM_DISC_MASK_LFA_NFC_DEP 0x08000000
#define NFA_DM_DISC_MASK_L_LEGACY 0x10000000
#define NFA_DM_DISC_MASK_LISTEN 0xFFFF0000

#define NFA_DM_DISC_MASK_NFC_DEP 0x0C481848

typedef uint32_t tNFA_DM_DISC_TECH_PROTO_MASK;

/* DM RF discovery host ID */
#define NFA_DM_DISC_HOST_ID_DH NFC_DH_ID
typedef uint8_t tNFA_DM_DISC_HOST_ID;

/* DM deactivation callback type */
typedef void(tNFA_DISCOVER_CBACK)(tNFA_DM_RF_DISC_EVT event,
                                  tNFC_DISCOVER* p_data);

/* DM RF discovery action flags */
/* RF discovery process has been started        */
#define NFA_DM_DISC_FLAGS_ENABLED 0x0001
/* Stop RF discovery is pending                 */
#define NFA_DM_DISC_FLAGS_STOPPING 0x0002
/* Disable NFA is pending                       */
#define NFA_DM_DISC_FLAGS_DISABLING 0x0004
/* Sleep wakeup in progress                     */
#define NFA_DM_DISC_FLAGS_CHECKING 0x0008
/* Notify sub-module that discovery is starting */
#define NFA_DM_DISC_FLAGS_NOTIFY 0x0010
/* command has been sent to NFCC in the state   */
#define NFA_DM_DISC_FLAGS_W4_RSP 0x0020
/* wait for NTF before changing discovery state */
#define NFA_DM_DISC_FLAGS_W4_NTF 0x0040

typedef uint16_t tNFA_DM_DISC_FLAGS;

/* DM Discovery control block */
typedef struct {
  bool in_use;                       /* TRUE if used          */
  tNFA_DISCOVER_CBACK* p_disc_cback; /* discovery callback    */

  tNFA_DM_DISC_FLAGS disc_flags; /* specific action flags */
  tNFA_DM_DISC_HOST_ID host_id;  /* DH or UICC1/UICC2     */
  tNFA_DM_DISC_TECH_PROTO_MASK
      requested_disc_mask; /* technology and protocol requested              */
  tNFA_DM_DISC_TECH_PROTO_MASK
      selected_disc_mask; /* technology and protocol waiting for activation */
} tNFA_DM_DISC_ENTRY;

/* polling, raw listen, P2P listen, NDEF CE, 2xVSE, 2xUICC */
#define NFA_DM_DISC_NUM_ENTRIES 8

/* max discovery technology parameters */
#define NFA_DM_MAX_DISC_PARAMS 16

/* index of listen mode routing table for technologies */
enum {
  NFA_DM_DISC_LRT_NFC_A,
  NFA_DM_DISC_LRT_NFC_B,
  NFA_DM_DISC_LRT_NFC_F,
  NFA_DM_DISC_LRT_NFC_BP
};

/* NFA_EE_MAX_TECH_ROUTE. only A, B, F, Bprime are supported by UICC now */
#define NFA_DM_MAX_TECH_ROUTE 4

/* timeout for waiting deactivation NTF,
** possible delay to send deactivate CMD if all credit wasn't returned
** transport delay (1sec) and max RWT (5sec)
*/
#define NFA_DM_DISC_TIMEOUT_W4_DEACT_NTF (NFC_DEACTIVATE_TIMEOUT * 1000 + 6000)

typedef struct {
  uint16_t disc_duration; /* Disc duration                                    */
  tNFA_DM_DISC_FLAGS disc_flags;    /* specific action flags */
  tNFA_DM_RF_DISC_STATE disc_state; /* RF discovery state */

  tNFC_RF_TECH_N_MODE activated_tech_mode; /* activated technology and mode */
  uint8_t activated_rf_disc_id;            /* activated RF discovery ID */
  tNFA_INTF_TYPE activated_rf_interface;   /* activated RF interface */
  tNFA_NFC_PROTOCOL activated_protocol;    /* activated protocol */
  tNFA_HANDLE activated_handle;            /* handle of activated sub-module */
  uint8_t activated_sel_res; /* activated tag's SEL_RES response */

  tNFA_DM_DISC_ENTRY entry[NFA_DM_DISC_NUM_ENTRIES];

  tNFA_DM_DISC_ENTRY excl_disc_entry; /* exclusive RF discovery */
  tNFA_LISTEN_CFG excl_listen_config; /* listen cfg for exclusive-rf mode */

  uint8_t listen_RT[NFA_DM_MAX_TECH_ROUTE]; /* Host ID for A, B, F, B'
                                               technology routing*/
  tNFA_DM_DISC_TECH_PROTO_MASK
      dm_disc_mask; /* technology and protocol waiting for activation   */

  TIMER_LIST_ENT tle; /* timer for waiting deactivation NTF               */
  TIMER_LIST_ENT kovio_tle; /* timer for Kovio bar code tag presence check */

  bool deact_pending; /* TRUE if deactivate while checking presence       */
  bool deact_notify_pending; /* TRUE if notify DEACTIVATED EVT while Stop rf
                                discovery*/
  tNFA_DEACTIVATE_TYPE pending_deact_type; /* pending deactivate type */

} tNFA_DM_DISC_CB;

/* NDEF Type Handler Definitions */
/* Default handler entry in ndef_handler table      */
#define NFA_NDEF_DEFAULT_HANDLER_IDX 0

/* Maximum number of pending SetConfigs */
#define NFA_DM_SETCONFIG_PENDING_MAX 32

/* NFA_DM flags */
/* DM is enabled                                                        */
#define NFA_DM_FLAGS_DM_IS_ACTIVE 0x00000001
/* Exclusive RF mode is active                                          */
#define NFA_DM_FLAGS_EXCL_RF_ACTIVE 0x00000002
/* Polling is enabled (while not in exclusive RF mode                   */
#define NFA_DM_FLAGS_POLLING_ENABLED 0x00000004
/* send poll stop event                                                 */
#define NFA_DM_FLAGS_SEND_POLL_STOP_EVT 0x00000008
/* auto reading of NDEF in progress                                     */
#define NFA_DM_FLAGS_AUTO_READING_NDEF 0x00000010
/* NFA_DM_ENABLE_EVT is not reported yet                                */
#define NFA_DM_FLAGS_ENABLE_EVT_PEND 0x00000020
/* Send NFA_DEACTIVATED_EVT when deactivated                            */
#define NFA_DM_FLAGS_SEND_DEACTIVATED_EVT 0x00000040
/* NFCC is restoring after back to full power mode                      */
#define NFA_DM_FLAGS_NFCC_IS_RESTORING 0x00000100
/* NFCC power mode is updating                                          */
#define NFA_DM_FLAGS_SETTING_PWR_MODE 0x00000200
/* NFA DM is disabling NFC                                              */
#define NFA_DM_FLAGS_DM_DISABLING_NFC 0x00000400
/* NFA_SendRawFrame() is called since RF activation                     */
#define NFA_DM_FLAGS_RAW_FRAME 0x00000800
/* NFA_DisableListening() is called and engaged                         */
#define NFA_DM_FLAGS_LISTEN_DISABLED 0x00001000
/* NFA_PauseP2p() is called and engaged                         */
#define NFA_DM_FLAGS_P2P_PAUSED 0x00002000
/* Power Off Sleep                                                      */
#define NFA_DM_FLAGS_POWER_OFF_SLEEP 0x00008000
/* stored parameters */
typedef struct {
  uint8_t total_duration[NCI_PARAM_LEN_TOTAL_DURATION];

  uint8_t la_bit_frame_sdd[NCI_PARAM_LEN_LA_BIT_FRAME_SDD];
  uint8_t la_bit_frame_sdd_len;
  uint8_t la_platform_config[NCI_PARAM_LEN_LA_PLATFORM_CONFIG];
  uint8_t la_platform_config_len;
  uint8_t la_sel_info[NCI_PARAM_LEN_LA_SEL_INFO];
  uint8_t la_sel_info_len;
  uint8_t la_nfcid1[NCI_NFCID1_MAX_LEN];
  uint8_t la_nfcid1_len;
  uint8_t la_hist_by[NCI_MAX_HIS_BYTES_LEN];
  uint8_t la_hist_by_len;

  uint8_t lb_sensb_info[NCI_PARAM_LEN_LB_SENSB_INFO];
  uint8_t lb_sensb_info_len;
  uint8_t lb_nfcid0[NCI_PARAM_LEN_LB_NFCID0];
  uint8_t lb_nfcid0_len;
  uint8_t lb_appdata[NCI_PARAM_LEN_LB_APPDATA];
  uint8_t lb_appdata_len;
  uint8_t lb_adc_fo[NCI_PARAM_LEN_LB_ADC_FO];
  uint8_t lb_adc_fo_len;
  uint8_t lb_h_info[NCI_MAX_ATTRIB_LEN];
  uint8_t lb_h_info_len;

  uint8_t lf_protocol[NCI_PARAM_LEN_LF_PROTOCOL];
  uint8_t lf_protocol_len;
  uint8_t lf_t3t_flags2[NCI_PARAM_LEN_LF_T3T_FLAGS2];
  uint8_t lf_t3t_flags2_len;
  uint8_t lf_t3t_pmm[NCI_PARAM_LEN_LF_T3T_PMM];
  uint8_t lf_t3t_id[NFA_CE_LISTEN_INFO_MAX]
                   [NCI_PARAM_LEN_LF_T3T_ID(NCI_VERSION_2_0)];

  uint8_t fwi[NCI_PARAM_LEN_FWI];
  uint8_t wt[NCI_PARAM_LEN_WT];
  uint8_t atr_req_gen_bytes[NCI_MAX_GEN_BYTES_LEN];
  uint8_t atr_req_gen_bytes_len;
  uint8_t atr_res_gen_bytes[NCI_MAX_GEN_BYTES_LEN];
  uint8_t atr_res_gen_bytes_len;

  uint8_t pf_rc[NCI_PARAM_LEN_PF_RC];
} tNFA_DM_PARAMS;

/*
**  NFA_NDEF CHO callback
**  It returns TRUE if NDEF is handled by connection handover module.
*/
typedef bool(tNFA_NDEF_CHO_CBACK)(uint32_t ndef_len, uint8_t* p_ndef_data);

/* DM control block */
typedef struct {
  uint32_t flags; /* NFA_DM flags (see definitions for NFA_DM_FLAGS_*)    */
  tNFA_DM_CBACK* p_dm_cback; /* NFA DM callback */
  TIMER_LIST_ENT tle;

  /* NFC link connection management */
  tNFA_CONN_CBACK* p_conn_cback;  /* callback for connection events       */
  tNFA_TECHNOLOGY_MASK poll_mask; /* technologies being polled            */

  tNFA_CONN_CBACK* p_excl_conn_cback; /* exclusive RF mode callback           */
  tNFA_NDEF_CBACK* p_excl_ndef_cback; /* ndef callback for exclusive RF mdoe  */

  tNFA_NDEF_CHO_CBACK*
      p_ndef_cho_cback; /* NDEF callback for static connection handover */

  tNFA_HANDLE poll_disc_handle; /* discovery handle for polling         */

  uint8_t* p_activate_ntf; /* temp holding activation notfication  */
  tHAL_API_GET_MAX_NFCEE* get_max_ee;

  tNFC_RF_TECH_N_MODE
      activated_tech_mode; /* previous activated technology and mode */
  uint8_t activated_nfcid[NFC_KOVIO_MAX_LEN]; /* NFCID 0/1/2 or UID of
                                                 ISO15694/Kovio  */
  uint8_t activated_nfcid_len; /* length of NFCID or UID               */

  /* NFC link discovery management */
  tNFA_DM_DISC_CB disc_cb;

  /* NDEF Type handler */
  tNFA_DM_API_REG_NDEF_HDLR*
      p_ndef_handler[NFA_NDEF_MAX_HANDLERS]; /* ndef handler table */

  /* stored parameters */
  tNFA_DM_PARAMS params;

  /* SetConfig management */
  uint32_t setcfg_pending_mask; /* Mask of to indicate whether pending
                                   SET_CONFIGs require NFA_DM_SET_CONFIG_EVT.
                                   LSB=oldest pending */
  uint8_t setcfg_pending_num;   /* Number of setconfigs pending */

  /* NFCC power mode */
  uint8_t nfcc_pwr_mode; /* NFA_DM_PWR_MODE_FULL or NFA_DM_PWR_MODE_OFF_SLEEP */

  uint8_t deactivate_cmd_retry_count; /*number of times the deactivation cmd
                                         sent in case of error scenerio */

  uint8_t power_state; /* current screen/power  state */
  uint32_t eDtaMode;   /* To enable the DTA type modes. */
  uint8_t pending_power_state; /* pending screen state change received in
                                  LISTEN_ACTIVE state which needs to be applied
                                  after current transaction is completed*/
} tNFA_DM_CB;

/* Internal function prototypes */
void nfa_dm_ndef_handle_message(tNFA_STATUS status, uint8_t* p_msg_buf,
                                uint32_t len);
void nfa_dm_ndef_dereg_all(void);
void nfa_dm_act_conn_cback_notify(uint8_t event, tNFA_CONN_EVT_DATA* p_data);
void nfa_dm_notify_activation_status(tNFA_STATUS status,
                                     tNFA_TAG_PARAMS* p_params);

bool nfa_dm_act_send_raw_vs(tNFA_DM_MSG* p_data);

void nfa_dm_disable_complete(void);

/* Internal functions from nfa_rw */
void nfa_rw_init(void);
void nfa_rw_proc_disc_evt(tNFA_DM_RF_DISC_EVT event, tNFC_DISCOVER* p_data,
                          bool excl_rf_not_active);
tNFA_STATUS nfa_rw_send_raw_frame(NFC_HDR* p_data);

/* Internal functions from nfa_ce */
void nfa_ce_init(void);

/* Pointer to compile-time configuration structure */
extern tNFA_DM_DISC_FREQ_CFG* p_nfa_dm_rf_disc_freq_cfg;
extern tNFA_HCI_CFG* p_nfa_hci_cfg;
extern tNFA_DM_CFG* p_nfa_dm_cfg;
extern uint8_t* p_nfa_dm_ce_cfg;
extern uint8_t* p_nfa_dm_gen_cfg;
extern uint8_t nfa_ee_max_ee_cfg;
extern tNCI_DISCOVER_MAPS* p_nfa_dm_interface_mapping;
extern uint8_t nfa_dm_num_dm_interface_mapping;
extern bool nfa_poll_bail_out_mode;

void nfa_dm_poll_disc_cback_dta_wrapper(tNFA_DM_RF_DISC_EVT event,
                                        tNFC_DISCOVER* p_data);
extern unsigned char appl_dta_mode_flag;

/* NFA device manager control block */
extern tNFA_DM_CB nfa_dm_cb;

void nfa_dm_init(void);
void nfa_p2p_init(void);
#if (NFA_SNEP_INCLUDED == TRUE)
void nfa_snep_init(bool is_dta_mode);
#else
#define nfa_snep_init(is_dta_mode)
#endif

#if (NFC_NFCEE_INCLUDED == TRUE)
void nfa_ee_init(void);
void nfa_hci_init(void);
#else
#define nfa_ee_init()
#define nfa_hci_init()
#endif

/* Action function prototypes */
bool nfa_dm_enable(tNFA_DM_MSG* p_data);
bool nfa_dm_disable(tNFA_DM_MSG* p_data);
bool nfa_dm_set_config(tNFA_DM_MSG* p_data);
bool nfa_dm_get_config(tNFA_DM_MSG* p_data);
bool nfa_dm_act_request_excl_rf_ctrl(tNFA_DM_MSG* p_data);
bool nfa_dm_act_release_excl_rf_ctrl(tNFA_DM_MSG* p_data);
bool nfa_dm_act_enable_polling(tNFA_DM_MSG* p_data);
bool nfa_dm_act_disable_polling(tNFA_DM_MSG* p_data);
bool nfa_dm_act_enable_listening(tNFA_DM_MSG* p_data);
bool nfa_dm_act_disable_listening(tNFA_DM_MSG* p_data);
bool nfa_dm_act_pause_p2p(tNFA_DM_MSG* p_data);
bool nfa_dm_act_resume_p2p(tNFA_DM_MSG* p_data);
bool nfa_dm_act_send_raw_frame(tNFA_DM_MSG* p_data);
bool nfa_dm_set_p2p_listen_tech(tNFA_DM_MSG* p_data);
bool nfa_dm_act_start_rf_discovery(tNFA_DM_MSG* p_data);
bool nfa_dm_act_stop_rf_discovery(tNFA_DM_MSG* p_data);
bool nfa_dm_act_set_rf_disc_duration(tNFA_DM_MSG* p_data);
bool nfa_dm_act_select(tNFA_DM_MSG* p_data);
bool nfa_dm_act_update_rf_params(tNFA_DM_MSG* p_data);
bool nfa_dm_act_deactivate(tNFA_DM_MSG* p_data);
bool nfa_dm_act_power_off_sleep(tNFA_DM_MSG* p_data);
bool nfa_dm_ndef_reg_hdlr(tNFA_DM_MSG* p_data);
bool nfa_dm_ndef_dereg_hdlr(tNFA_DM_MSG* p_data);

bool nfa_dm_act_reg_vsc(tNFA_DM_MSG* p_data);
bool nfa_dm_act_send_vsc(tNFA_DM_MSG* p_data);
uint16_t nfa_dm_act_get_rf_disc_duration();
bool nfa_dm_act_disable_timeout(tNFA_DM_MSG* p_data);

bool nfa_dm_set_power_sub_state(tNFA_DM_MSG* p_data);

void nfa_dm_proc_nfcc_power_mode(uint8_t nfcc_power_mode);

/* Main function prototypes */
bool nfa_dm_evt_hdlr(NFC_HDR* p_msg);
void nfa_dm_sys_enable(void);
void nfa_dm_sys_disable(void);
tNFA_STATUS nfa_dm_check_set_config(uint8_t tlv_list_len, uint8_t* p_tlv_list,
                                    bool app_init);

void nfa_dm_conn_cback_event_notify(uint8_t event, tNFA_CONN_EVT_DATA* p_data);

/* Discovery function prototypes */
void nfa_dm_disc_sm_execute(tNFA_DM_RF_DISC_SM_EVENT event,
                            tNFA_DM_RF_DISC_DATA* p_data);
tNFA_HANDLE nfa_dm_add_rf_discover(tNFA_DM_DISC_TECH_PROTO_MASK disc_mask,
                                   tNFA_DM_DISC_HOST_ID host_id,
                                   tNFA_DISCOVER_CBACK* p_disc_cback);
void nfa_dm_delete_rf_discover(tNFA_HANDLE handle);
void nfa_dm_start_excl_discovery(tNFA_TECHNOLOGY_MASK poll_tech_mask,
                                 tNFA_LISTEN_CFG* p_listen_cfg,
                                 tNFA_DISCOVER_CBACK* p_disc_cback);
void nfa_dm_rel_excl_rf_control_and_notify(void);
void nfa_dm_stop_excl_discovery(void);
void nfa_dm_disc_new_state(tNFA_DM_RF_DISC_STATE new_state);

void nfa_dm_start_rf_discover(void);
void nfa_dm_rf_discover_select(uint8_t rf_disc_id, tNFA_NFC_PROTOCOL protocol,
                               tNFA_INTF_TYPE rf_interface);
tNFA_STATUS nfa_dm_rf_deactivate(tNFA_DEACTIVATE_TYPE deactivate_type);
bool nfa_dm_is_protocol_supported(tNFA_NFC_PROTOCOL protocol, uint8_t sel_res);
bool nfa_dm_is_active(void);
tNFC_STATUS nfa_dm_disc_sleep_wakeup(void);
tNFC_STATUS nfa_dm_disc_start_kovio_presence_check(void);
bool nfa_dm_is_raw_frame_session(void);
bool nfa_dm_is_p2p_paused(void);

#if (NFC_NFCEE_INCLUDED == FALSE)
#define nfa_ee_get_tech_route(ps, ha) \
  memset(ha, NFC_DH_ID, NFA_DM_MAX_TECH_ROUTE);
#endif

std::string nfa_dm_nfc_revt_2_str(tNFC_RESPONSE_EVT event);

#endif /* NFA_DM_INT_H */
