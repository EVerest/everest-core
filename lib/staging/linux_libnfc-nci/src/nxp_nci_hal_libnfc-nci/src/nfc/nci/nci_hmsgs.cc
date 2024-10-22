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
 *  This file contains function of the NCI unit to format and send NCI
 *  commands (for DH).
 *
 ******************************************************************************/
#include <string.h>
#include "nfc_target.h"

#include "nci_defs.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"

/*******************************************************************************
**
** Function         nci_snd_core_reset
**
** Description      compose and send CORE RESET command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_reset(uint8_t reset_type) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_RESET);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_RESET;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_RESET);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_RESET);
  UINT8_TO_STREAM(pp, reset_type);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_core_init
**
** Description      compose and send CORE INIT command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_init(uint8_t nci_version) {
  NFC_HDR* p;
  uint8_t* pp;

  if ((p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_INIT(nci_version))) == nullptr)
    return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_INIT(nci_version);
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_INIT);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_INIT(nci_version));
  if (nfc_cb.nci_version == NCI_VERSION_2_0) {
    UINT8_TO_STREAM(pp, NCI2_0_CORE_INIT_CMD_BYTE_0);
    UINT8_TO_STREAM(pp, NCI2_0_CORE_INIT_CMD_BYTE_1);
  }

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_core_get_config
**
** Description      compose and send CORE GET_CONFIG command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_get_config(uint8_t* param_ids, uint8_t num_ids) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(num_ids);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + num_ids + 1;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_GET_CONFIG);
  UINT8_TO_STREAM(pp, (uint8_t)(num_ids + 1));
  UINT8_TO_STREAM(pp, num_ids);
  ARRAY_TO_STREAM(pp, param_ids, num_ids);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_core_set_config
**
** Description      compose and send CORE SET_CONFIG command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_set_config(uint8_t* p_param_tlvs, uint8_t tlv_size) {
  NFC_HDR* p;
  uint8_t* pp;
  uint8_t num = 0, ulen, len, *pt;

  p = NCI_GET_CMD_BUF(tlv_size + 1);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + tlv_size + 1;
  p->offset = NCI_MSG_OFFSET_SIZE;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_SET_CONFIG);
  UINT8_TO_STREAM(pp, (uint8_t)(tlv_size + 1));
  len = tlv_size;
  pt = p_param_tlvs;
  while (len > 1) {
    len -= 2;
    pt++;
    num++;
    ulen = *pt++;
    pt += ulen;
    if (len >= ulen) {
      len -= ulen;
    } else {
      GKI_freebuf(p);
      return NCI_STATUS_FAILED;
    }
  }

  UINT8_TO_STREAM(pp, num);
  ARRAY_TO_STREAM(pp, p_param_tlvs, tlv_size);
  nfc_ncif_send_cmd(p);

  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_core_conn_create
**
** Description      compose and send CORE CONN_CREATE command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_conn_create(uint8_t dest_type, uint8_t num_tlv,
                                 uint8_t tlv_size, uint8_t* p_param_tlvs) {
  NFC_HDR* p;
  uint8_t* pp;
  uint8_t size = NCI_CORE_PARAM_SIZE_CON_CREATE + tlv_size;

  p = NCI_GET_CMD_BUF(size);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_CON_CREATE;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_CONN_CREATE);
  UINT8_TO_STREAM(pp, size);
  UINT8_TO_STREAM(pp, dest_type);
  UINT8_TO_STREAM(pp, num_tlv);
  if (tlv_size) {
    ARRAY_TO_STREAM(pp, p_param_tlvs, tlv_size);
    p->len += tlv_size;
  }

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_core_conn_close
**
** Description      compose and send CORE CONN_CLOSE command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_conn_close(uint8_t conn_id) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_CON_CLOSE);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_CON_CLOSE;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_CONN_CLOSE);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_CON_CLOSE);
  UINT8_TO_STREAM(pp, conn_id);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)
/*******************************************************************************
**
** Function         nci_snd_nfcee_discover
**
** Description      compose and send NFCEE Management NFCEE_DISCOVER command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_nfcee_discover(uint8_t discover_action) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_PARAM_SIZE_DISCOVER_NFCEE(NFC_GetNCIVersion()));
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len =
      NCI_MSG_HDR_SIZE + NCI_PARAM_SIZE_DISCOVER_NFCEE(NFC_GetNCIVersion());
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_EE_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_NFCEE_DISCOVER);
  UINT8_TO_STREAM(pp, NCI_PARAM_SIZE_DISCOVER_NFCEE(NFC_GetNCIVersion()));
  if (NFC_GetNCIVersion() != NCI_VERSION_2_0) {
    UINT8_TO_STREAM(pp, discover_action);
  }
  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_nfcee_mode_set
**
** Description      compose and send NFCEE Management NFCEE MODE SET command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_nfcee_mode_set(uint8_t nfcee_id, uint8_t nfcee_mode) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_NFCEE_MODE_SET);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_NFCEE_MODE_SET;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_EE_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_NFCEE_MODE_SET);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_NFCEE_MODE_SET);
  UINT8_TO_STREAM(pp, nfcee_id);
  UINT8_TO_STREAM(pp, nfcee_mode);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_iso_dep_nak_presence_check_cmd
**
** Description      compose and send RF Management presence check ISO-DEP NAK
**                  command.
**
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_iso_dep_nak_presence_check_cmd() {
  NFC_HDR* p;
  uint8_t* pp;

  if ((p = NCI_GET_CMD_BUF(0)) == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->len = NCI_MSG_HDR_SIZE + 0;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_ISO_DEP_NAK_PRESENCE);
  UINT8_TO_STREAM(pp, 0x00);
  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}
#endif
#endif

/*******************************************************************************
**
** Function         nci_snd_discover_cmd
**
** Description      compose and send RF Management DISCOVER command to command
**                  queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_discover_cmd(uint8_t num, tNCI_DISCOVER_PARAMS* p_param) {
  NFC_HDR* p;
  uint8_t *pp, *p_size, *p_start;
  int xx;
  int size;

  size = num * sizeof(tNCI_DISCOVER_PARAMS) + 1;
  p = NCI_GET_CMD_BUF(size);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DISCOVER);
  p_size = pp;
  pp++;
  p_start = pp;
  UINT8_TO_STREAM(pp, num);
  for (xx = 0; xx < num; xx++) {
    UINT8_TO_STREAM(pp, p_param[xx].type);
    UINT8_TO_STREAM(pp, p_param[xx].frequency);
  }
  *p_size = (uint8_t)(pp - p_start);
  p->len = NCI_MSG_HDR_SIZE + *p_size;

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_discover_select_cmd
**
** Description      compose and send RF Management DISCOVER SELECT command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_discover_select_cmd(uint8_t rf_disc_id, uint8_t protocol,
                                    uint8_t rf_interface) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_DISCOVER_PARAM_SIZE_SELECT);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_DISCOVER_PARAM_SIZE_SELECT;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DISCOVER_SELECT);
  UINT8_TO_STREAM(pp, NCI_DISCOVER_PARAM_SIZE_SELECT);
  UINT8_TO_STREAM(pp, rf_disc_id);
  UINT8_TO_STREAM(pp, protocol);
  UINT8_TO_STREAM(pp, rf_interface);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_deactivate_cmd
**
** Description      compose and send RF Management DEACTIVATE command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_deactivate_cmd(uint8_t de_act_type) {
  NFC_HDR* p;
  uint8_t* pp;

  nfc_cb.reassembly = true;

  p = NCI_GET_CMD_BUF(NCI_DISCOVER_PARAM_SIZE_DEACT);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_DISCOVER_PARAM_SIZE_DEACT;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DEACTIVATE);
  UINT8_TO_STREAM(pp, NCI_DISCOVER_PARAM_SIZE_DEACT);
  UINT8_TO_STREAM(pp, de_act_type);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_discover_map_cmd
**
** Description      compose and send RF Management DISCOVER MAP command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_discover_map_cmd(uint8_t num, tNCI_DISCOVER_MAPS* p_maps) {
  NFC_HDR* p;
  uint8_t *pp, *p_size, *p_start;
  int xx;
  int size;

  size = num * sizeof(tNCI_DISCOVER_MAPS) + 1;

  p = NCI_GET_CMD_BUF(size);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_DISCOVER_MAP);
  p_size = pp;
  pp++;
  p_start = pp;
  UINT8_TO_STREAM(pp, num);
  for (xx = 0; xx < num; xx++) {
    UINT8_TO_STREAM(pp, p_maps[xx].protocol);
    UINT8_TO_STREAM(pp, p_maps[xx].mode);
    UINT8_TO_STREAM(pp, p_maps[xx].intf_type);
  }
  *p_size = (uint8_t)(pp - p_start);
  p->len = NCI_MSG_HDR_SIZE + *p_size;
  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}
/*******************************************************************************
**
** Function         nci_snd_t3t_polling
**
** Description      compose and send RF Management T3T POLLING command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_t3t_polling(uint16_t system_code, uint8_t rc, uint8_t tsn) {
  NFC_HDR* p;
  uint8_t* pp;

  p = NCI_GET_CMD_BUF(NCI_RF_PARAM_SIZE_T3T_POLLING);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_RF_PARAM_SIZE_T3T_POLLING;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_T3T_POLLING);
  UINT8_TO_STREAM(pp, NCI_RF_PARAM_SIZE_T3T_POLLING);
  UINT16_TO_BE_STREAM(pp, system_code);
  UINT8_TO_STREAM(pp, rc);
  UINT8_TO_STREAM(pp, tsn);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_parameter_update_cmd
**
** Description      compose and send RF Management RF Communication Parameter
**                  Update commandto command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_parameter_update_cmd(uint8_t* p_param_tlvs, uint8_t tlv_size) {
  NFC_HDR* p;
  uint8_t* pp;
  uint8_t num = 0, ulen, len, *pt;

  p = NCI_GET_CMD_BUF(tlv_size + 1);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + tlv_size + 1;
  p->offset = NCI_MSG_OFFSET_SIZE;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_PARAMETER_UPDATE);
  UINT8_TO_STREAM(pp, (uint8_t)(tlv_size + 1));
  len = tlv_size;
  pt = p_param_tlvs;
  while (len > 1) {
    len -= 2;
    pt++;
    num++;
    ulen = *pt++;
    pt += ulen;
    if (len >= ulen) {
      len -= ulen;
    } else {
      GKI_freebuf(p);
      return NCI_STATUS_FAILED;
    }
  }

  UINT8_TO_STREAM(pp, num);
  ARRAY_TO_STREAM(pp, p_param_tlvs, tlv_size);
  nfc_ncif_send_cmd(p);

  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nci_snd_nfcee_power_link_control
**
** Description      compose and send NFCEE Management NFCEE Power and Link
**                  Control command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_nfcee_power_link_control(uint8_t nfcee_id, uint8_t pl_config) {
  uint8_t* pp;
  NFC_HDR* p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_NFCEE_PL_CTRL);
  if (p == nullptr) return NCI_STATUS_FAILED;

  p->event = NFC_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_NFCEE_PL_CTRL;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_EE_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_NFCEE_POWER_LINK_CTRL);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_NFCEE_PL_CTRL);
  UINT8_TO_STREAM(pp, nfcee_id);
  UINT8_TO_STREAM(pp, pl_config);

  nfc_ncif_send_cmd(p);
  return NCI_STATUS_OK;
}

#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)
/*******************************************************************************
**
** Function         nci_snd_set_routing_cmd
**
** Description      compose and send RF Management SET_LISTEN_MODE_ROUTING
**                  command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_set_routing_cmd(bool more, uint8_t num_tlv, uint8_t tlv_size,
                                uint8_t* p_param_tlvs) {
  NFC_HDR* p;
  uint8_t* pp;
  uint8_t size = tlv_size + 2;

  if (tlv_size == 0) {
    /* just to terminate routing table
     * 2 bytes (more=FALSE and num routing entries=0) */
    size = 2;
  }

  p = NCI_GET_CMD_BUF(size);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->len = NCI_MSG_HDR_SIZE + size;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_SET_ROUTING);
  UINT8_TO_STREAM(pp, size);
  UINT8_TO_STREAM(pp, more);
  if (size == 2) {
    UINT8_TO_STREAM(pp, 0);
  } else {
    UINT8_TO_STREAM(pp, num_tlv);
    ARRAY_TO_STREAM(pp, p_param_tlvs, tlv_size);
  }
  nfc_ncif_send_cmd(p);

  return (NCI_STATUS_OK);
}
/*******************************************************************************
**
** Function         nci_snd_set_power_sub_state_cmd
**
** Description      compose and send core CORE_SET_POWER_SUB_STATE command
**                  to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_core_set_power_sub_state(uint8_t screen_state) {
  NFC_HDR* p = NCI_GET_CMD_BUF(NCI_CORE_PARAM_SIZE_SET_POWER_SUB_STATE);
  uint8_t* pp;

  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->len = NCI_MSG_HDR_SIZE + NCI_CORE_PARAM_SIZE_SET_POWER_SUB_STATE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_CORE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_CORE_SET_POWER_SUB_STATE);
  UINT8_TO_STREAM(pp, NCI_CORE_PARAM_SIZE_SET_POWER_SUB_STATE);
  UINT8_TO_STREAM(pp, screen_state);

  nfc_ncif_send_cmd(p);

  return (NCI_STATUS_OK);
}
/*******************************************************************************
**
** Function         nci_snd_get_routing_cmd
**
** Description      compose and send RF Management GET_LISTEN_MODE_ROUTING
**                  command to command queue
**
** Returns          status
**
*******************************************************************************/
uint8_t nci_snd_get_routing_cmd(void) {
  NFC_HDR* p;
  uint8_t* pp;
  uint8_t param_size = 0;

  p = NCI_GET_CMD_BUF(param_size);
  if (p == nullptr) return (NCI_STATUS_FAILED);

  p->event = BT_EVT_TO_NFC_NCI;
  p->len = NCI_MSG_HDR_SIZE + param_size;
  p->offset = NCI_MSG_OFFSET_SIZE;
  p->layer_specific = 0;
  pp = (uint8_t*)(p + 1) + p->offset;

  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_RF_MANAGE);
  NCI_MSG_BLD_HDR1(pp, NCI_MSG_RF_GET_ROUTING);
  UINT8_TO_STREAM(pp, param_size);

  nfc_ncif_send_cmd(p);
  return (NCI_STATUS_OK);
}
#endif
#endif
