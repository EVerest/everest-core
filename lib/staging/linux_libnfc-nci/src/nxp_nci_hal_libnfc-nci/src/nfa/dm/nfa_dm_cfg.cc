/******************************************************************************
 *
 *  Copyright (C) 2011-2014 Broadcom Corporation
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
 *  This file contains compile-time configurable constants for NFA modules
 *
 ******************************************************************************/
#include "nfa_api.h"

/* the SetConfig for CE T3T/T4T */
const uint8_t nfa_dm_ce_cfg[] = {
    13,                  /* total length */
    NFC_PMID_LF_T3T_PMM, /* Type-3 tag default PMM */
    NCI_PARAM_LEN_LF_T3T_PMM,
    0x01, /* This PAD0 is used to identify HCE-F on Android */
    0xFE, /* This PAD0 is used to identify HCE-F on Android */
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    NFC_PMID_FWI, /* FWI for ISO-DEP */
    1,
    CE_T4T_ISO_DEP_FWI};

uint8_t* p_nfa_dm_ce_cfg = (uint8_t*)nfa_dm_ce_cfg;

uint8_t* p_nfa_dm_gen_cfg = nullptr;

/* the RF Discovery Frequency for each technology */
const tNFA_DM_DISC_FREQ_CFG nfa_dm_rf_disc_freq_cfg = {
    1, /* Frequency for NFC Technology A               */
    1, /* Frequency for NFC Technology B               */
    1, /* Frequency for NFC Technology F               */
    1, /* Frequency for Proprietary Technology/15693   */
    1, /* Frequency for Proprietary Technology/B-Prime */
    1, /* Frequency for Proprietary Technology/Kovio   */
    1, /* Frequency for NFC Technology A active mode   */
    1, /* Frequency for NFC Technology F active mode   */
    1  /* Frequency for NFC Technology active mode     */
};

tNFA_DM_DISC_FREQ_CFG* p_nfa_dm_rf_disc_freq_cfg =
    (tNFA_DM_DISC_FREQ_CFG*)&nfa_dm_rf_disc_freq_cfg;

uint8_t nfa_ee_max_ee_cfg = NFA_EE_MAX_EE_SUPPORTED;

/* set to NULL to use the default mapping set by stack */
tNCI_DISCOVER_MAPS* p_nfa_dm_interface_mapping = nullptr;
uint8_t nfa_dm_num_dm_interface_mapping = 0;

tNFA_DM_CFG nfa_dm_cfg = {
    /* Automatic NDEF detection (when not in exclusive RF mode) */
    NFA_DM_AUTO_DETECT_NDEF,
    /* Automatic NDEF read (when not in exclusive RF mode) */
    NFA_DM_AUTO_READ_NDEF,
    /* Automatic presence check */
    NFA_DM_AUTO_PRESENCE_CHECK,
    /* Use sleep/wake(last interface) for ISODEP presence check */
    NFA_DM_PRESENCE_CHECK_OPTION,
    /* Maximum time to wait for presence check response */
    NFA_DM_MAX_PRESENCE_CHECK_TIMEOUT};

tNFA_DM_CFG* p_nfa_dm_cfg = (tNFA_DM_CFG*)&nfa_dm_cfg;

const uint8_t nfa_hci_whitelist[] = {0x02, 0x03, 0x04};

tNFA_HCI_CFG nfa_hci_cfg = {
    /* Max HCI Network IDLE time to wait for EE DISC REQ Ntf(s) */
    NFA_HCI_NETWK_INIT_IDLE_TIMEOUT,
    /* Maximum HCP Response time to any HCP Command */
    NFA_HCI_RESPONSE_TIMEOUT,
    /* Number of host in the whitelist of Terminal host */
    0x03,
    /* Pointer to the Whitelist of Terminal Host */
    (uint8_t*)nfa_hci_whitelist};

tNFA_HCI_CFG* p_nfa_hci_cfg = (tNFA_HCI_CFG*)&nfa_hci_cfg;

bool nfa_poll_bail_out_mode = false;
tNFA_PROPRIETARY_CFG nfa_proprietary_cfg = {
    0x80, /* NCI_PROTOCOL_18092_ACTIVE */
    0x81, /* NCI_PROTOCOL_B_PRIME */
    0x82, /* NCI_PROTOCOL_DUAL */
    0x83, /* NCI_PROTOCOL_15693 */
    0x8A, /* NCI_PROTOCOL_KOVIO */
    0xFF, /* NCI_PROTOCOL_MIFARE */
    0x77, /* NCI_DISCOVERY_TYPE_POLL_KOVIO */
    0x74, /* NCI_DISCOVERY_TYPE_POLL_B_PRIME */
    0xF4, /* NCI_DISCOVERY_TYPE_LISTEN_B_PRIME */
};

tNFA_PROPRIETARY_CFG* p_nfa_proprietary_cfg =
    (tNFA_PROPRIETARY_CFG*)&nfa_proprietary_cfg;
