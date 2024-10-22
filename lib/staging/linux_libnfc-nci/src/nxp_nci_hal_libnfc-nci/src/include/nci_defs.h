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
 *  This file contains the definition from NCI specification
 *
 ******************************************************************************/

#ifndef NFC_NCI_DEFS_H
#define NFC_NCI_DEFS_H

#include <stdint.h>
#define NCI_DISCOVERY_TYPE_POLL_B_PRIME         0x74
#define NCI_DISCOVERY_TYPE_LISTEN_B_PRIME       0xF4
/**********************************************
 * NCI Interface Types
 **********************************************/
#define NCI_INTERFACE_EE_DIRECT_RF      0
#define NCI_INTERFACE_FRAME             1
#define NCI_INTERFACE_ISO_DEP           2
#define NCI_INTERFACE_NFC_DEP           3

#define NCI_INTERFACE_MAX               NCI_INTERFACE_NFC_DEP

#define NCI_INTERFACE_FIRST_VS          0x80
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#define NCI_INTERFACE_MIFARE            0x80
/* #if (NFC_NXP_CHIP_TYPE != PN547C2) */
#define NCI_INTERFACE_UICC_DIRECT       0x82
#define NCI_INTERFACE_ESE_DIRECT        0x83
#endif
/* Define the message header size for all NCI Commands and Notifications.
*/
#define NCI_MSG_HDR_SIZE 3  /* per NCI spec */
#define NCI_DATA_HDR_SIZE 3 /* per NCI spec */
#define NCI_MAX_PAYLOAD_SIZE 0xFE
#define NCI_CTRL_INIT_SIZE 32  /* initial NFCC control payload size */
#define NCI_MAX_VSC_SIZE 0xFF
#define APPL_DTA_MODE TRUE
/* NCI header (3) + callback function pointer(8; use 8 to be safe) + HCIT (1
 * byte) */
#define NCI_VSC_MSG_HDR_SIZE 12

/* Max frame size (256) - Prologue (1) - Epilogue (2) in ISO-DEP, CID and NAD
 * are not used*/
#define NCI_ISO_DEP_MAX_INFO 253
/* Max payload (254) - Protocol Header (3) in NFC-DEP, DID and NAD are not used
 */
#define NCI_NFC_DEP_MAX_DATA 251

/* NCI Command and Notification Format:
 * 3 byte message header:
 * byte 0: MT PBF GID
 * byte 1: OID
 * byte 2: Message Length */
/* MT: Message Type (byte 0) */
#define NCI_MT_MASK 0xE0
#define NCI_MT_SHIFT 5
#define NCI_MT_DATA 0x00
#define NCI_MT_CMD 1 /* (NCI_MT_CMD << NCI_MT_SHIFT) = 0x20 */
#define NCI_MT_RSP 2 /* (NCI_MT_RSP << NCI_MT_SHIFT) = 0x40 */
#define NCI_MT_NTF 3 /* (NCI_MT_NTF << NCI_MT_SHIFT) = 0x60 */

#define NCI_NTF_BIT 0x80 /* the tNFC_VS_EVT is a notification */
#define NCI_RSP_BIT 0x40 /* the tNFC_VS_EVT is a response     */

/* for internal use only; not from specification */
/* the following 2 flags are used in layer_specific for fragmentation/reassembly
 * of data packets */

/* PBF: Packet Boundary Flag (byte 0) */
#define NCI_PBF_MASK 0x10
#define NCI_PBF_SHIFT 4

/* GID: Group Identifier (byte 0) */
#define NCI_GID_MASK 0x0F
#define NCI_GID_CORE 0x00      /* 0000b NCI Core group */
#define NCI_GID_RF_MANAGE 0x01 /* 0001b RF Management group */
#define NCI_GID_EE_MANAGE 0x02 /* 0010b NFCEE Management group */
#define NCI_GID_PROP 0x0F      /* 1111b Proprietary */
/* 0111b - 1110b RFU */

/* OID: Opcode Identifier (byte 1) */
#define NCI_OID_MASK 0x3F
#define NCI_OID_SHIFT 0

/* For routing */
#define NCI_DH_ID 0 /* for DH */
/* To identify the loopback test */
#define NCI_TEST_ID 0xFE /* for loopback test */

/* Destination Type */
#define NCI_DEST_TYPE_NFCC 1   /* NFCC - loopback */
#define NCI_DEST_TYPE_REMOTE 2 /* Remote NFC Endpoint */
#define NCI_DEST_TYPE_NFCEE 3  /* NFCEE */
#define NCI_DEST_TYPE_T4T_NFCEE 5 /* T4T NFCEE */

/* builds byte0 of NCI Command and Notification packet */
#define NCI_MSG_BLD_HDR0(p, mt, gid) \
  *(p)++ = (uint8_t)(((mt) << NCI_MT_SHIFT) | (gid));

/* builds byte1 of NCI Command and Notification packet */
#define NCI_MSG_BLD_HDR1(p, oid) *(p)++ = (uint8_t)(((oid) << NCI_OID_SHIFT));

/* parse byte0 of NCI packet */
#define NCI_MSG_PRS_HDR0(p, mt, pbf, gid)       \
  mt = (*(p)&NCI_MT_MASK) >> NCI_MT_SHIFT;      \
  (pbf) = (*(p)&NCI_PBF_MASK) >> NCI_PBF_SHIFT; \
  (gid) = *(p)++ & NCI_GID_MASK;

/* parse byte1 of NCI Cmd/Ntf */
#define NCI_MSG_PRS_HDR1(p, oid) \
  (oid) = (*(p)&NCI_OID_MASK);   \
  (p)++;

/* NCI Data Format:
 * byte 0: MT(0) PBF CID
 * byte 1: RFU
 * byte 2: Data Length */
/* CID: Connection Identifier (byte 0) 1-0xF Dynamically assigned (by NFCC), 0
 * is predefined  */
#define NCI_CID_MASK 0x0F

#define NCI_DATA_PBLD_HDR(p, pbf, cid, len)             \
  *(p)++ = (uint8_t)(((pbf) << NCI_PBF_SHIFT) | (cid)); \
  *(p)++ = 0;                                           \
  *(p)++ = (len);

#define NCI_DATA_PRS_HDR(p, pbf, cid, len)      \
  (pbf) = (*(p)&NCI_PBF_MASK) >> NCI_PBF_SHIFT; \
  (cid) = (*(p)&NCI_CID_MASK);                  \
  (p)++;                                        \
  (p)++;                                        \
  (len) = *(p)++;

/* Logical target ID 0x01-0xFE */

/* CORE_RESET_NTF reset trigger type*/
#define NCI2_0_RESET_TRIGGER_TYPE_POWERED_ON 0x01
#define NCI2_0_RESET_TRIGGER_TYPE_CORE_RESET_CMD_RECEIVED 0x02

/* Status Codes */
#define NCI_STATUS_OK 0x00
#define NCI_STATUS_REJECTED 0x01
#define NCI_STATUS_MESSAGE_CORRUPTED 0x02
#define NCI_STATUS_BUFFER_FULL 0xE0
#define NCI_STATUS_FAILED 0x03
#define NCI_STATUS_NOT_INITIALIZED 0x04
#define NCI_STATUS_SYNTAX_ERROR 0x05
#define NCI_STATUS_SEMANTIC_ERROR 0x06
#define NCI_STATUS_UNKNOWN_GID 0x07
#define NCI_STATUS_UNKNOWN_OID 0x08
#define NCI_STATUS_INVALID_PARAM 0x09
#define NCI_STATUS_MSG_SIZE_TOO_BIG 0x0A
#define NCI_STATUS_NOT_SUPPORTED 0x0B
/* discovery */
#define NCI_STATUS_ALREADY_STARTED 0xA0
#define NCI_STATUS_ACTIVATION_FAILED 0xA1
#define NCI_STATUS_TEAR_DOWN 0xA2
/* RF Interface */
#define NCI_STATUS_RF_TRANSMISSION_ERR 0xB0
#define NCI_STATUS_RF_PROTOCOL_ERR 0xB1
#define NCI_STATUS_TIMEOUT 0xB2
/* NFCEE Interface */
#define NCI_STATUS_EE_INTF_ACTIVE_FAIL 0xC0
#define NCI_STATUS_EE_TRANSMISSION_ERR 0xC1
#define NCI_STATUS_EE_PROTOCOL_ERR 0xC2
#define NCI_STATUS_EE_TIMEOUT 0xC3

/* RF Technologies */
#define NCI_RF_TECHNOLOGY_A 0x00
#define NCI_RF_TECHNOLOGY_B 0x01
#define NCI_RF_TECHNOLOGY_F 0x02
#define NCI_RF_TECHNOLOGY_V 0x03

/* Bit Rates */
#define NCI_BIT_RATE_106 0x00  /* 106 kbit/s */
#define NCI_BIT_RATE_212 0x01  /* 212 kbit/s */
#define NCI_BIT_RATE_424 0x02  /* 424 kbit/s */
#define NCI_BIT_RATE_848 0x03  /* 848 Kbit/s */
#define NCI_BIT_RATE_1696 0x04 /* 1696 Kbit/s*/
#define NCI_BIT_RATE_3392 0x05 /* 3392 Kbit/s*/
#define NCI_BIT_RATE_6784 0x06 /* 6784 Kbit/s*/

/**********************************************
 * NCI Core Group Opcode        - 0
 **********************************************/
#define NCI_MSG_CORE_RESET 0
#define NCI_MSG_CORE_INIT 1
#define NCI_MSG_CORE_SET_CONFIG 2
#define NCI_MSG_CORE_GET_CONFIG 3
#define NCI_MSG_CORE_CONN_CREATE 4
#define NCI_MSG_CORE_CONN_CLOSE 5
#define NCI_MSG_CORE_CONN_CREDITS 6
#define NCI_MSG_CORE_GEN_ERR_STATUS 7
#define NCI_MSG_CORE_INTF_ERR_STATUS 8
#define NCI_MSG_CORE_SET_POWER_SUB_STATE 9

/**********************************************
 * RF MANAGEMENT Group Opcode    - 1
 **********************************************/
#define NCI_MSG_RF_DISCOVER_MAP 0
#define NCI_MSG_RF_SET_ROUTING 1
#define NCI_MSG_RF_GET_ROUTING 2
#define NCI_MSG_RF_DISCOVER 3
#define NCI_MSG_RF_DISCOVER_SELECT 4
#define NCI_MSG_RF_INTF_ACTIVATED 5
#define NCI_MSG_RF_DEACTIVATE 6
#define NCI_MSG_RF_FIELD 7
#define NCI_MSG_RF_T3T_POLLING 8
#define NCI_MSG_RF_EE_ACTION 9
#define NCI_MSG_RF_EE_DISCOVERY_REQ 10
#define NCI_MSG_RF_PARAMETER_UPDATE 11
#define NCI_MSG_RF_ISO_DEP_NAK_PRESENCE 16

/**********************************************
 * NFCEE MANAGEMENT Group Opcode - 2
 **********************************************/
#define NCI_MSG_NFCEE_DISCOVER 0
#define NCI_MSG_NFCEE_MODE_SET 1
#define NCI_MSG_NFCEE_STATUS 2
#define NCI_MSG_NFCEE_POWER_LINK_CTRL 3
/**********************************************
 * NCI Proprietary  Group       - F
 **********************************************/

/**********************************************
 * NCI Core Group Params
 **********************************************/
#define NCI_CORE_PARAM_SIZE_RESET 0x01

/**********************************************
 * NCI Feature Bit
 **********************************************/
#define NCI_FEAT_HCI_NETWORK 0x00000008

#define NCI_CORE_PARAM_SIZE_INIT(X) (((X) == NCI_VERSION_2_0) ? (0x02) : (0x00))
#define NCI2_0_CORE_INIT_CMD_BYTE_0 0x00
#define NCI2_0_CORE_INIT_CMD_BYTE_1 0x00

/* Status (1 octet) and number of params */
#define NCI_CORE_PARAM_SIZE_SET_POWER_SUB_STATE 0x01

#define NCI_CORE_PARAM_SIZE_CON_CREATE 0x02 /* handle, num_tlv, (tlv) */
/* status, size, credits, conn_id */
#define NCI_CON_CREATE_TAG_RF_DISC_ID 0x00
#define NCI_CON_CREATE_TAG_NFCEE_VAL 0x01

#define NCI_CORE_PARAM_SIZE_CON_CLOSE 0x01     /* Conn ID (1 octet) */
#if (NXP_EXTNS == TRUE)
/* Keep the NCI configuration (if possible) and perform NCI initialization. */
#define NCI_RESET_TYPE_KEEP_CFG 0x00
#endif

/* Reset the NCI configuration, and perform NCI initialization. */
#define NCI_RESET_TYPE_RESET_CFG 0x01

/* No operating field generated by remote device  */
#define NCI_RF_STS_NO_REMOTE 0x00
/* Operating field generated by remote device  */
#define NCI_RF_STS_REMOTE 0x01

/* Discovery Action (1 octet) */
#define NCI_PARAM_SIZE_DISCOVER_NFCEE(X) \
  (((X) == NCI_VERSION_2_0) ? 0X00 : 0X01)

#define NCI_DISCOVER_ACTION_DISABLE 0
#define NCI_DISCOVER_ACTION_ENABLE 1

#define NCI_RF_PARAM_ID_TECH_N_MODE 0x00 /* RF Technology and Mode   */
#define NCI_RF_PARAM_ID_TX_BIT_RATE 0x01 /* Transmit Bit Rate        */
#define NCI_RF_PARAM_ID_RX_BIT_RATE 0x02 /* Receive Bit Rate         */
#define NCI_RF_PARAM_ID_B_DATA_EX_PARAM \
  0x03 /* B Data Exchange config param  \
          */

#define NCI_NFCEE_INTERFACE_APDU 0x00
#define NCI_NFCEE_INTERFACE_HCI_ACCESS 0x01
#define NCI_NFCEE_INTERFACE_T3T 0x02
#define NCI_NFCEE_INTERFACE_TRANSPARENT 0x03
#define NCI_NFCEE_INTERFACE_PROPRIETARY 0x80
/****************************************************
 * NCI NFCEE INterface specific status Codes
 ****************************************************/
#define NCI_NFCEE_STS_UNRECOVERABLE_ERROR 0x00
#define NCI_NFCEE_STS_INTF_ACTIVATION_FAILED 0xC0
#define NCI_NFCEE_STS_TRANSMISSION_ERROR 0xC1
#define NCI_NFCEE_STS_PROTOCOL_ERROR 0xC2
#define NCI_NFCEE_STS_TIMEOUT_ERROR 0xC3

#define NCI_NFCEE_STS_CONN_ACTIVE 0x00
#define NCI_NFCEE_STS_CONN_INACTIVE 0x01
#define NCI_NFCEE_STS_REMOVED 0x02

/* Logical Target ID (1 octet)NFCEE Mode (1 octet) */
#define NCI_CORE_PARAM_SIZE_NFCEE_MODE_SET 0x02

/* Deactivate the connected NFCEE */
#define NCI_NFCEE_MD_DEACTIVATE 0x00
/* Activate the connected NFCEE */
#define NCI_NFCEE_MD_ACTIVATE 0x01
#define NCI_NUM_NFCEE_MODE 2

/**********************************************
 * NCI Deactivation Type
 **********************************************/
#define NCI_DEACTIVATE_TYPE_IDLE 0      /* Idle Mode     */
#define NCI_DEACTIVATE_TYPE_SLEEP 1     /* Sleep Mode    */
#define NCI_DEACTIVATE_TYPE_SLEEP_AF 2  /* Sleep_AF Mode */
#define NCI_DEACTIVATE_TYPE_DISCOVERY 3 /* Discovery     */

/**********************************************
 * NCI Deactivation Reasons
 **********************************************/
#define NCI_DEACTIVATE_REASON_DH_REQ 0       /* DH Request       */
#define NCI_DEACTIVATE_REASON_ENDPOINT_REQ 1 /* Endpoint Request */
#define NCI_DEACTIVATE_REASON_RF_LINK_LOSS 2 /* RF Link Loss     */
#define NCI_DEACTIVATE_REASON_NFCB_BAD_AFI 3 /* NFC-B Bad AFI    */
/* DH Request Failed due to error */
#define NCI_DEACTIVATE_REASON_DH_REQ_FAILED 4

/* The NFCEE status in NFCEE Status Notification */
typedef uint8_t tNCI_EE_NTF_STATUS;

/* NFCEE Power and Link Configuration */
typedef uint8_t tNCI_NFCEE_PL_CONFIG;

/**********************************************
* NCI Interface Mode
**********************************************/
#define NCI_INTERFACE_MODE_POLL_N_LISTEN 3

/**********************************************
 * NCI Interface Types
 **********************************************/
#define NCI_INTERFACE_EE_DIRECT_RF 0
#define NCI_INTERFACE_FRAME 1
#define NCI_INTERFACE_ISO_DEP 2
#define NCI_INTERFACE_NFC_DEP 3
#define NCI_INTERFACE_MAX NCI_INTERFACE_NFC_DEP
#define NCI_INTERFACE_EXTENSION_MAX 2
#define NCI_INTERFACE_FIRST_VS 0x80
typedef uint8_t tNCI_INTF_TYPE;

/**********************************************
 * NCI RF Management / DISCOVERY Group Params
 **********************************************/

#define NCI_DISCOVER_PARAM_SIZE_SELECT 0x03     /* ID, protocol, interface */
#define NCI_DISCOVER_PARAM_SIZE_DEACT 0x01      /* type */

/**********************************************
 * Supported Protocols
 **********************************************/
#define NCI_PROTOCOL_UNKNOWN 0x00
#define NCI_PROTOCOL_T1T 0x01
#define NCI_PROTOCOL_T2T 0x02
#define NCI_PROTOCOL_T3T 0x03
#define NCI_PROTOCOL_T5T 0x06
#define NCI_PROTOCOL_ISO_DEP 0x04
#define NCI_PROTOCOL_NFC_DEP 0x05

/* Discovery Types/Detected Technology and Mode */
#define NCI_DISCOVERY_TYPE_POLL_A 0x00
#define NCI_DISCOVERY_TYPE_POLL_B 0x01
#define NCI_DISCOVERY_TYPE_POLL_F 0x02
#define NCI_DISCOVERY_TYPE_POLL_V 0x06
#define NCI_DISCOVERY_TYPE_POLL_A_ACTIVE 0x03
/* NCI2.0 standardizes P2P poll active*/
#define NCI_DISCOVERY_TYPE_POLL_ACTIVE 0x03
#define NCI_DISCOVERY_TYPE_POLL_F_ACTIVE 0x05
#define NCI_DISCOVERY_TYPE_LISTEN_A 0x80
#define NCI_DISCOVERY_TYPE_LISTEN_B 0x81
#define NCI_DISCOVERY_TYPE_LISTEN_F 0x82
#define NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE 0x83
/* NCI2.0 standardizes P2P listen active*/
#define NCI_DISCOVERY_TYPE_LISTEN_ACTIVE 0x83
#define NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE 0x85
#define NCI_DISCOVERY_TYPE_LISTEN_ISO15693 0x86

typedef uint8_t tNCI_DISCOVERY_TYPE;

#define NCI_EE_TRIG_7816_SELECT 0x00
#define NCI_EE_TRIG_RF_PROTOCOL 0x01
#define NCI_EE_TRIG_RF_TECHNOLOGY 0x02
#define NCI_EE_TRIG_APP_INIT 0x10

#define NCI_EE_ACT_TAG_AID 0xC0   /* AID                 */
#define NCI_EE_ACT_TAG_DATA 0xC3  /* hex data for app    */

/* NFCEE ID (1 octet) PL config (1 octet) */
#define NCI_CORE_PARAM_SIZE_NFCEE_PL_CTRL 0x02
/* Status (1 octet) */
/* Technology based routing  */
#define NCI_ROUTE_TAG_TECH 0x00
/* Protocol based routing  */
#define NCI_ROUTE_TAG_PROTO 0x01
#define NCI_ROUTE_TAG_AID 0x02 /* AID routing */
#define NCI_ROUTE_TAG_SYSCODE 0x03 /* SystemCode routing */

#define NCI_ROUTE_PWR_STATE_ON 0x01 /* The device is on */
/* The device is switched off */
#define NCI_ROUTE_PWR_STATE_SWITCH_OFF 0x02
/* The device's battery is removed */
#define NCI_ROUTE_PWR_STATE_BATT_OFF 0x04
/* The device is screen off Unlock mode */
#define NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK() \
  ((NFC_GetNCIVersion() == NCI_VERSION_2_0) ? 0x08 : 0x80)
/* The device is screen on lock mode */
#define NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK() \
  ((NFC_GetNCIVersion() == NCI_VERSION_2_0) ? 0x10 : 0x40)
/* The device is screen off lock mode */
#define NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK() \
  ((NFC_GetNCIVersion() == NCI_VERSION_2_0) ? 0x20 : 0x00)

/* Hardware / Registration Identification  */
#define NCI_NFCEE_TAG_HW_ID 0x00
#define NCI_NFCEE_TAG_ATR_BYTES 0x01 /* ATR Bytes  */
/* T3T Command Set Interface Supplementary Info */
#define NCI_NFCEE_TAG_T3T_INFO 0x02
#define NCI_NFCEE_TAG_HCI_HOST_ID 0xA0 /* HCI host ID */

#define NCI_DISCOVER_NTF_LAST 0x00
#define NCI_DISCOVER_NTF_LAST_ABORT 0x01
#define NCI_DISCOVER_NTF_MORE 0x02

/* NCI RF Management Group Params */
#define NCI_RF_PARAM_SIZE_T3T_POLLING 0x04 /* System Code, RC, TSN */

/**********************************************
 * NCI Parameter IDs
 **********************************************/

#define NCI_PARAM_ID_TOTAL_DURATION 0x00
#define NCI_PARAM_ID_CON_DEVICES_LIMIT 0x01
#define NCI_PARAM_ID_CON_DISCOVERY_PARAM 0x02
#define NCI_PARAM_ID_PA_BAILOUT 0x08
#define NCI_PARAM_ID_PB_AFI 0x10
#define NCI_PARAM_ID_PB_BAILOUT 0x11
#define NCI_PARAM_ID_PB_ATTRIB_PARAM1 0x12
#define NCI_PARAM_ID_PF_BIT_RATE 0x18
#define NCI_PARAM_ID_PF_RC 0x19
#define NCI_PARAM_ID_PB_H_INFO 0x20
#define NCI_PARAM_ID_PI_BIT_RATE 0x21

#define NCI_PARAM_ID_BITR_NFC_DEP 0x28
#define NCI_PARAM_ID_ATR_REQ_GEN_BYTES 0x29
#define NCI_PARAM_ID_ATR_REQ_CONFIG 0x2A

#define NCI_PARAM_ID_LA_BIT_FRAME_SDD 0x30
#define NCI_PARAM_ID_LA_PLATFORM_CONFIG 0x31
#define NCI_PARAM_ID_LA_SEL_INFO 0x32
#define NCI_PARAM_ID_LA_NFCID1 0x33
#define NCI_PARAM_ID_LB_SENSB_INFO 0x38
#define NCI_PARAM_ID_LB_NFCID0 0x39
#define NCI_PARAM_ID_LB_APPDATA 0x3A
#define NCI_PARAM_ID_LB_SFGI 0x3B
#define NCI_PARAM_ID_LB_ADC_FO 0x3C
#define NCI_PARAM_ID_LB_PROTOCOL NCI_PARAM_ID_LB_SENSB_INFO

#define NCI_PARAM_ID_LF_T3T_ID1 0x40
#define NCI_PARAM_ID_LF_T3T_ID2 0x41
#define NCI_PARAM_ID_LF_T3T_ID3 0x42
#define NCI_PARAM_ID_LF_T3T_ID4 0x43
#define NCI_PARAM_ID_LF_T3T_ID5 0x44
#define NCI_PARAM_ID_LF_T3T_ID6 0x45
#define NCI_PARAM_ID_LF_T3T_ID7 0x46
#define NCI_PARAM_ID_LF_T3T_ID8 0x47
#define NCI_PARAM_ID_LF_T3T_ID9 0x48
#define NCI_PARAM_ID_LF_T3T_ID10 0x49
#define NCI_PARAM_ID_LF_T3T_ID11 0x4A
#define NCI_PARAM_ID_LF_T3T_ID12 0x4B
#define NCI_PARAM_ID_LF_T3T_ID13 0x4C
#define NCI_PARAM_ID_LF_T3T_ID14 0x4D
#define NCI_PARAM_ID_LF_T3T_ID15 0x4E
#define NCI_PARAM_ID_LF_T3T_ID16 0x4F
#define NCI_PARAM_ID_LF_PROTOCOL 0x50
#define NCI_PARAM_ID_LF_T3T_PMM 0x51
/* max num of LF_T3T_ID supported by NFCC (1 for now) */
#define NCI_PARAM_ID_LF_T3T_MAX 0x52
#define NCI_PARAM_ID_LF_T3T_FLAGS2 0x53
#define NCI_PARAM_ID_LF_CON_BITR_F 0x54
#define NCI_PARAM_ID_LF_CON_ADV_FEAT 0x55
/*LF_T3T name changed in NCI2.0*/
#define NCI_PARAM_ID_LF_T3T_RD_ALLOWED 0x55

#define NCI_PARAM_ID_FWI 0x58
#define NCI_PARAM_ID_LA_HIST_BY 0x59
#define NCI_PARAM_ID_LB_H_INFO_RSP 0x5A
#define NCI_PARAM_ID_LI_BIT_RATE 0x5B

#define NCI_PARAM_ID_WT 0x60
#define NCI_PARAM_ID_ATR_RES_GEN_BYTES 0x61
#define NCI_PARAM_ID_ATR_RSP_CONFIG 0x62

#define NCI_PARAM_ID_RF_FIELD_INFO 0x80
#define NCI_PARAM_ID_NFC_DEP_OP 0x82

#define NCI_PARAM_ID_NFCC_CONFIG_CONTROL 0x85

/* Type A Parameters */
#define NCI_PARAM_PLATFORM_T1T 0x0C
#define NCI_PARAM_SEL_INFO_ISODEP 0x20
#define NCI_PARAM_SEL_INFO_NFCDEP 0x40
/**********************************************
 * NCI Parameter ID Lens
 **********************************************/
#define NCI_PARAM_LEN_TOTAL_DURATION 2

#define NCI_PARAM_LEN_CON_DISCOVERY_PARAM 1

#define NCI_PARAM_LEN_PF_RC 1

#define NCI_PARAM_LEN_LA_BIT_FRAME_SDD 1
#define NCI_PARAM_LEN_LA_PLATFORM_CONFIG 1
#define NCI_PARAM_LEN_LA_SEL_INFO 1

#define NCI_PARAM_LEN_LB_SENSB_INFO 1
#define NCI_PARAM_LEN_LB_NFCID0 4
#define NCI_PARAM_LEN_LB_APPDATA 4
#define NCI_PARAM_LEN_LB_ADC_FO 1

#define NCI_PARAM_LEN_LF_PROTOCOL 1
#define NCI_PARAM_LEN_LF_T3T_FLAGS2 2
#define NCI_PARAM_LEN_LF_T3T_PMM 8
#define NCI_PARAM_LEN_LF_T3T_ID(X) (((X) == NCI_VERSION_2_0) ? (0x12) : (0x0A))
#define NCI_PARAM_LEN_LF_CON_ADV_FEAT 1

#define NCI_PARAM_LEN_LF_T3T_RD_ALLOWED 1  // Listen F NCI2.0 Parameter

#define NCI_PARAM_LEN_FWI 1
#define NCI_PARAM_LEN_WT 1
/* GEN_BYTES - variable */

/* Listen protocol bits - NCI_PARAM_ID_LF_PROTOCOL and
 * NCI_PARAM_ID_LB_SENSB_INFO */
#define NCI_LISTEN_PROTOCOL_ISO_DEP 0x01
#define NCI_LISTEN_PROTOCOL_NFC_DEP 0x02

/* LF_T3T_FLAGS2 listen bits all-disabled definition */
#define NCI_LF_T3T_FLAGS2_ALL_DISABLED 0x0000

/* The DH-NFCEE listen is considered as a enable NFCEE */
#define NCI_LISTEN_DH_NFCEE_ENABLE_MASK 0x00
/* The DH-NFCEE listen is considered as a disable NFCEE */
#define NCI_LISTEN_DH_NFCEE_DISABLE_MASK 0x02
/* The DH polling is considered as a disable NFCEE */
#define NCI_POLLING_DH_DISABLE_MASK 0x00
/* The DH polling is considered as a enable NFCEE */
#define NCI_POLLING_DH_ENABLE_MASK 0x01
/* SCBR support check with Core Init resp OCT1 byte */
#define NCI_SCBR_MASK 0x10

/* AID matching is allowed when the SELECT AID is longer */
#define NCI_ROUTE_QUAL_LONG_SELECT 0x10
/* AID matching is allowed when the SELECT AID is shorter */
#define NCI_ROUTE_QUAL_SHORT_SELECT 0x20
/* AID is blocked in unsupported power mode */
#define NCI_ROUTE_QUAL_BLOCK_ROUTE 0x40

#ifndef NCI_GET_CMD_BUF
#if (HCI_USE_VARIABLE_SIZE_CMD_BUF == FALSE)
/* Allocate fixed-size buffer from HCI_CMD_POOL (default case) */
#define NCI_GET_CMD_BUF(paramlen) ((NFC_HDR*)GKI_getpoolbuf(NFC_NCI_POOL_ID))
#else
/* Allocate smallest possible buffer (for platforms with limited RAM) */
#define NCI_GET_CMD_BUF(paramlen)                                    \
  ((NFC_HDR*)GKI_getbuf((uint16_t)(NFC_HDR_SIZE + NCI_MSG_HDR_SIZE + \
                                   NCI_MSG_OFFSET_SIZE + (paramlen))))
#endif
#endif /* NCI_GET_CMD_BUF */

#define NCI_MAX_AID_LEN 16

typedef struct {
  uint8_t type;
  uint8_t frequency;
} tNCI_DISCOVER_PARAMS;

typedef struct {
  uint8_t protocol;
  uint8_t mode;
  uint8_t intf_type;
} tNCI_DISCOVER_MAPS;

#define NCI_NFCID1_MAX_LEN 10
#define NCI_T1T_HR_LEN 2
typedef struct {
  uint8_t sens_res[2]; /* SENS_RES Response (ATQA). Available after Technology
                          Detection */
  uint8_t nfcid1_len;  /* 4, 7 or 10 */
  uint8_t nfcid1[NCI_NFCID1_MAX_LEN]; /* AKA NFCID1 */
  uint8_t sel_rsp; /* SEL_RSP (SAK) Available after Collision Resolution */
  uint8_t hr_len;  /* 2, if T1T HR0/HR1 is reported */
  uint8_t hr[NCI_T1T_HR_LEN]; /* T1T HR0 is in hr[0], HR1 is in hr[1] */
} tNCI_RF_PA_PARAMS;

#define NCI_MAX_SENSB_RES_LEN 12
typedef struct {
  uint8_t sensb_res_len; /* Length of SENSB_RES Response (Byte 2 - Byte 12 or
                            13) Available after Technology Detection */
  uint8_t sensb_res[NCI_MAX_SENSB_RES_LEN]; /* SENSB_RES Response (ATQ) */
} tNCI_RF_PB_PARAMS;

#define NCI_MAX_SENSF_RES_LEN 18
#define NCI_NFCID2_LEN 8
#define NCI_T3T_PMM_LEN 8
#define NCI_SYSTEMCODE_LEN 2
#define NCI_RF_F_UID_LEN NCI_NFCID2_LEN
#define NCI_MRTI_CHECK_INDEX 13
#define NCI_MRTI_UPDATE_INDEX 14
typedef struct {
  uint8_t bit_rate;      /* NFC_BIT_RATE_212 or NFC_BIT_RATE_424 */
  uint8_t sensf_res_len; /* Length of SENSF_RES Response (Byte 2 - Byte 17 or
                            19) Available after Technology Detection */
  uint8_t sensf_res[NCI_MAX_SENSF_RES_LEN]; /* SENSB_RES Response */
} tNCI_RF_PF_PARAMS;

typedef struct {
  uint8_t nfcid2[NCI_NFCID2_LEN]; /* NFCID2 generated by the Local NFCC for
                                     NFC-DEP Protocol.Available for Frame
                                     Interface  */
} tNCI_RF_LF_PARAMS;

#ifndef NCI_MAX_ATS_LEN
#define NCI_MAX_ATS_LEN 64
#endif
#ifndef NCI_MAX_HIS_BYTES_LEN
#define NCI_MAX_HIS_BYTES_LEN 50
#endif
#ifndef NCI_MAX_GEN_BYTES_LEN
#define NCI_MAX_GEN_BYTES_LEN 48
#endif

#define NCI_ATS_T0_INDEX 0
#define NCI_ATS_TC_MASK 0x40
#define NCI_ATS_TB_MASK 0x20
#define NCI_ATS_TA_MASK 0x10
typedef struct {
  uint8_t ats_res_len;              /* Length of ATS RES */
  uint8_t ats_res[NCI_MAX_ATS_LEN]; /* ATS RES defined in [DIGPROT] */
} tNCI_INTF_PA_ISO_DEP;

typedef struct { uint8_t rats; /* RATS */ } tNCI_INTF_LA_ISO_DEP;

#define NCI_P_GEN_BYTE_INDEX 15
#define NCI_L_GEN_BYTE_INDEX 14
#define NCI_L_NFC_DEP_TO_INDEX 13
typedef struct {
  uint8_t atr_res_len;              /* Length of ATR_RES */
  uint8_t atr_res[NCI_MAX_ATS_LEN]; /* ATR_RES (Byte 3 - Byte 17+n) as defined
                                       in [DIGPROT] */
} tNCI_INTF_PA_NFC_DEP;

/* Note: keep tNCI_INTF_PA_NFC_DEP data member in the same order as
 * tNCI_INTF_LA_NFC_DEP */
typedef struct {
  uint8_t atr_req_len;              /* Length of ATR_REQ */
  uint8_t atr_req[NCI_MAX_ATS_LEN]; /* ATR_REQ (Byte 3 - Byte 18+n) as defined
                                       in [DIGPROT] */
} tNCI_INTF_LA_NFC_DEP;
typedef tNCI_INTF_LA_NFC_DEP tNCI_INTF_LF_NFC_DEP;
typedef tNCI_INTF_PA_NFC_DEP tNCI_INTF_PF_NFC_DEP;

#define NCI_MAX_ATTRIB_LEN (10 + NCI_MAX_GEN_BYTES_LEN)

typedef struct {
  uint8_t attrib_res_len; /* Length of ATTRIB RES */
  uint8_t
      attrib_res[NCI_MAX_ATTRIB_LEN]; /* ATTRIB RES  as defined in [DIGPROT] */
} tNCI_INTF_PB_ISO_DEP;

typedef struct {
  uint8_t attrib_req_len;                 /* Length of ATTRIB REQ */
  uint8_t attrib_req[NCI_MAX_ATTRIB_LEN]; /* ATTRIB REQ (Byte 2 - Byte 10+k) as
                                             defined in [DIGPROT] */
} tNCI_INTF_LB_ISO_DEP;

typedef struct {
  uint8_t atr_res_len;                      /* Length of ATR_RES            */
  uint8_t atr_res[NCI_MAX_ATS_LEN];         /* ATR_RES (Byte 3 - Byte 17+n) */
  uint8_t max_payload_size;                 /* 64, 128, 192 or 254          */
  uint8_t gen_bytes_len;                    /* len of general bytes         */
  uint8_t gen_bytes[NCI_MAX_GEN_BYTES_LEN]; /* general bytes                */
  uint8_t waiting_time;                     /* WT -> Response Waiting Time
                                               RWT = (256 x 16/fC) x 2WT    */
} tNCI_RF_ACM_P_PARAMS;

#endif /* NFC_NCI_DEFS_H */
