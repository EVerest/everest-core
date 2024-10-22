/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _PHNXPEXTNS_CUSTOM_H_
#define _PHNXPEXTNS_CUSTOM_H_

#include <errno.h>
#include <nfa_api.h>
#include <phNfcStatus.h>
#include <sys/types.h>

/*
 * TLV structure
 * For simple TLV, type[0] == 0x00
 * For extended TLV, type[0] == 0xA0
 */
typedef struct {
  uint8_t type[2];
  uint8_t len;
  uint8_t* val;
} tlv_t;

typedef enum {
  passive_106 = 0x01,
  passive_212 = 0x02,
  passive_424 = 0x04,
  active_106 = 0x10,
  active_212 = 0x20,
  active_424 = 0x40,
} p2p_speed_t;

typedef enum {
  NO_SE,
  UICC,
  eSE,
} SE_t;

typedef enum {
  ReaderMode = 0x01,
  P2PMode = 0x02,
  CEMode = 0x04,
} PollMode_t;

/*******************************************************************************
 **
 ** Function         phNxpExtns_get_version
 **
 ** Description      Function to get the HW, FW and SW versions.
 **
 ** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function will use phNxpNciHal_get_version from HAL.
 *******************************************************************************/

NFCSTATUS phNxpExtns_get_version(uint32_t* hw_ver, uint32_t* fw_ver,
                                 uint32_t* sw_ver);

/*******************************************************************************
 **
 ** Function         phNxpNciHal_read_tlv
 **
 ** Description      Function to read simple TLV and extended TLV.
 **                  Memory for TLV and fields are allocated and freed by
 *calling
 **                  function. Input is type and len. Response is provied in
 **val.
 **
 ** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function will use NFA_GetConfig for simple TLV.
 **       For extended TLV, it will use NFA_SendRawFrame.
 *******************************************************************************/

NFCSTATUS phNxpNciHal_read_tlv(tlv_t* tlv);

/*******************************************************************************
 **
 ** Function         phNxpNciHal_write_tlv
 **
 ** Description      Function to write simple TLV and extended TLV.
 **                  Memory for TLV and fields are allocated and freed by
 *calling
 **                  function. Input is type, len, *val.
 **
 ** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function will use NFA_SetConfig for simple TLV.
 **       For extended TLV, it will use NFA_SendRawFrame.
 *******************************************************************************/

NFCSTATUS phNxpNciHal_write_tlv(tlv_t* tlv);

/*******************************************************************************
 **
 ** Function        phNxpExtns_select_poll_tech
 **
 ** Description     This function selects the polling technology for starting
 **                 polling loop. This function does not start polling loop.
 **                 It is just a setting for polling technology.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function is using NFA_EnablePolling function.
 *******************************************************************************/

NFCSTATUS phNxpExtns_select_poll_tech(tNFA_TECHNOLOGY_MASK tech_mask);

/*******************************************************************************
 **
 ** Function        phNxpExtns_select_ce_listen_tech
 **
 ** Description     This function set the listen tech for card emulation.
 **                 This function does not include routing.
 **                 This function does not start polling loop.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function is using NFA_CeConfigureUiccListenTech.
 **       Not sure which handle to use, from UICC or eSE.
 *******************************************************************************/

NFCSTATUS phNxpExtns_select_ce_listen_tech(tNFA_TECHNOLOGY_MASK tech_mask);

/*******************************************************************************
 **
 ** Function        phNxpExtns_select_p2p_poll_speed
 **
 ** Description     This function will select the P2P polling speed.
 **                 phNxpExtns_select_poll_tech overwrite the settings of poll
 *if
 **                 reader mode is enabled.
 **                 There is only one active poll phase but device can use one
 **                 active speed and can move to higher speed if target
 *supports.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function will use NFA_EnablePolling and NFA_SetConfig
 *******************************************************************************/

NFCSTATUS phNxpExtns_select_p2p_poll_speed(p2p_speed_t p2p_initiator_speed);

/*******************************************************************************
 **
 ** Function        phNxpExtns_select_p2p_listen_speed
 **
 ** Description     This function will select the listen mode
 **                 This function does not include routing.
 **                 This function does not start polling loop.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function will use NFA_SetP2pListenTech and
 *NFA_SetConfig
 *******************************************************************************/

NFCSTATUS phNxpExtns_select_p2p_listen_speed(p2p_speed_t p2p_target_speed);

/*******************************************************************************
 **
 ** Function        phNxpExtns_select_se
 **
 ** Description     This function will set the routing of the traffic to
 *selected
 **                 SE. This function also does not start polling loop.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE:
 *******************************************************************************/

NFCSTATUS phNxpExtns_select_se(SE_t se);

/*******************************************************************************
 **
 ** Function        phNxpExtns_set_poll_mode
 **
 ** Description     This function selects which mode to enable for polling loop.
 **                 This function do not start polling loop.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE:
 *******************************************************************************/

NFCSTATUS phNxpExtns_set_poll_mode(PollMode_t poll_mode);

/*******************************************************************************
 **
 ** Function        phNxpExtns_start_poll
 **
 ** Description     This function starts polling loop based on the configuration
 **                 of the previous calls. If no configuration done through
 *other
 **                 function call then it uses the default configuration from
 **                 configuration files.
 **                 This function internally stops the polling loop if it is
 **                 already running.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE: Internally this function uses NFA_StartRfDiscovery.
 *******************************************************************************/

NFCSTATUS phNxpExtns_start_poll(void);

/*******************************************************************************
 **
 ** Function        phNxpExtns_stop_poll
 **
 ** Description     This function stops the polling loop if it is running.
 **
 ** Returns          NFCSTATUS_SUCCESS if operation successful,
 **                  otherwise NFCSTATUS_FAILED.
 **
 ** NOTE:Internally this function uses NFA_StopRfDiscovery.
 *******************************************************************************/

NFCSTATUS phNxpExtns_stop_poll(void);

/*******************************************************************************
 **
 ** Function        phNxpExtns_enable_Felica_CLT
 **
 ** Description     This function enables or disable Felica CLT feature.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE:
 *******************************************************************************/

NFCSTATUS phNxpExtns_enable_Felica_CLT(bool enable);

/*******************************************************************************
 **
 ** Function        phNxpExtns_enable_Mifare_CLT
 **
 ** Description     This function enables or disable Mifare CLT feature.
 **
 ** Returns         NFCSTATUS_SUCCESS if operation successful,
 **                 otherwise NFCSTATUS_FAILED.
 **
 ** NOTE:
 *******************************************************************************/

NFCSTATUS phNxpExtns_enable_Mifare_CLT(bool enable);

#endif /* _PHNXPEXTNS_CUSTOM_H_ */
