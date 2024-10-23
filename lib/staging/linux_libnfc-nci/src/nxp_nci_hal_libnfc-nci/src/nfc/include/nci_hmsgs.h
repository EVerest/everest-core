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
 *  defines NCI interface messages (for DH)
 *
 ******************************************************************************/
#ifndef NFC_NCI_HMSGS_H
#define NFC_NCI_HMSGS_H

#include "nci_defs.h"
#include "nfc_types.h"
#include "nfc_hal_api.h"
#include <stdbool.h>

bool nci_proc_core_rsp(NFC_HDR* p_msg);
void nci_proc_rf_management_rsp(NFC_HDR* p_msg);
void nci_proc_ee_management_rsp(NFC_HDR* p_msg);
void nci_proc_core_ntf(NFC_HDR* p_msg);
void nci_proc_rf_management_ntf(NFC_HDR* p_msg);
void nci_proc_ee_management_ntf(NFC_HDR* p_msg);
void nci_proc_prop_rsp(NFC_HDR* p_msg);
void nci_proc_prop_raw_vs_rsp(NFC_HDR* p_msg);
void nci_proc_prop_ntf(NFC_HDR* p_msg);

uint8_t nci_snd_core_reset(uint8_t reset_type);
uint8_t nci_snd_core_init(uint8_t nci_version);
uint8_t nci_snd_core_get_config(uint8_t* param_ids, uint8_t num_ids);
uint8_t nci_snd_core_set_config(uint8_t* p_param_tlvs, uint8_t tlv_size);

uint8_t nci_snd_core_conn_create(uint8_t dest_type, uint8_t num_tlv,
                                 uint8_t tlv_size, uint8_t* p_param_tlvs);
uint8_t nci_snd_core_conn_close(uint8_t conn_id);

uint8_t nci_snd_discover_cmd(uint8_t num, tNCI_DISCOVER_PARAMS* p_param);

uint8_t nci_snd_discover_select_cmd(uint8_t rf_disc_id, uint8_t protocol,
                                    uint8_t rf_interface);
uint8_t nci_snd_deactivate_cmd(uint8_t de_act_type);
uint8_t nci_snd_discover_map_cmd(uint8_t num, tNCI_DISCOVER_MAPS* p_maps);
uint8_t nci_snd_t3t_polling(uint16_t system_code, uint8_t rc, uint8_t tsn);
uint8_t nci_snd_parameter_update_cmd(uint8_t* p_param_tlvs, uint8_t tlv_size);
uint8_t nci_snd_iso_dep_nak_presence_check_cmd();
uint8_t nci_snd_core_set_power_sub_state(uint8_t screen_state);

#if (NFC_NFCEE_INCLUDED == TRUE && NFC_RW_ONLY == FALSE)
uint8_t nci_snd_nfcee_discover(uint8_t discover_action);
uint8_t nci_snd_nfcee_mode_set(uint8_t nfcee_id, uint8_t nfcee_mode);
uint8_t nci_snd_set_routing_cmd(bool more, uint8_t num_tlv, uint8_t tlv_size,
                                uint8_t* p_param_tlvs);
uint8_t nci_snd_get_routing_cmd(void);
uint8_t nci_snd_nfcee_power_link_control(uint8_t nfcee_id, uint8_t pl_config);
#endif

#endif /* NFC_NCI_MSGS_H */
