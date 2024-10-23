/******************************************************************************
 *
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
 *  This file contains the action functions for device manager discovery
 *  function.
 *
 ******************************************************************************/
#include <string>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nci_hmsgs.h"
#include "nfa_api.h"
#include "nfa_dm_int.h"
#include "nfa_p2p_int.h"

#if (NFC_NFCEE_INCLUDED == TRUE)
#include "nfa_ee_api.h"
#include "nfa_ee_int.h"
#endif
#include "nfa_rw_int.h"

#include "nfc_int.h"
#include "nfa_sys_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
static BOOLEAN reconnect_in_progress;
static BOOLEAN is_emvco_active;
#endif
/*
**  static functions
*/
static uint8_t nfa_dm_get_rf_discover_config(
    tNFA_DM_DISC_TECH_PROTO_MASK dm_disc_mask,
    tNFC_DISCOVER_PARAMS disc_params[], uint8_t max_params);
static tNFA_STATUS nfa_dm_set_rf_listen_mode_config(
    tNFA_DM_DISC_TECH_PROTO_MASK tech_proto_mask);
static void nfa_dm_set_rf_listen_mode_raw_config(
    tNFA_DM_DISC_TECH_PROTO_MASK* p_disc_mask);
static tNFA_DM_DISC_TECH_PROTO_MASK nfa_dm_disc_get_disc_mask(
    tNFC_RF_TECH_N_MODE tech_n_mode, tNFC_PROTOCOL protocol);
static void nfa_dm_notify_discovery(tNFA_DM_RF_DISC_DATA* p_data);
static tNFA_STATUS nfa_dm_disc_notify_activation(tNFC_DISCOVER* p_data);
static void nfa_dm_disc_notify_deactivation(tNFA_DM_RF_DISC_SM_EVENT sm_event,
                                            tNFC_DISCOVER* p_data);
static void nfa_dm_disc_data_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                                   tNFC_CONN* p_data);
static void nfa_dm_disc_kovio_timeout_cback(TIMER_LIST_ENT* p_tle);
static void nfa_dm_disc_report_kovio_presence_check(tNFC_STATUS status);

static std::string nfa_dm_disc_state_2_str(uint8_t state);
static std::string nfa_dm_disc_event_2_str(uint8_t event);

typedef struct nfa_dm_p2p_prio_logic {
  bool isodep_detected;      /* flag to check if ISO-DEP is detected */
  bool timer_expired;        /* flag to check whether timer is expired */
  TIMER_LIST_ENT timer_list; /*timer structure pointer */
  uint8_t first_tech_mode;
} nfa_dm_p2p_prio_logic_t;

static nfa_dm_p2p_prio_logic_t p2p_prio_logic_data;


/*******************************************************************************
**
** Function         nfa_dm_get_rf_discover_config
**
** Description      Build RF discovery configurations from
**                  tNFA_DM_DISC_TECH_PROTO_MASK
**
** Returns          number of RF discovery configurations
**
*******************************************************************************/
static uint8_t nfa_dm_get_rf_discover_config(
    tNFA_DM_DISC_TECH_PROTO_MASK dm_disc_mask,
    tNFC_DISCOVER_PARAMS disc_params[], uint8_t max_params) {
  uint8_t num_params = 0;

  if (nfa_dm_cb.flags & NFA_DM_FLAGS_LISTEN_DISABLED) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("listen disabled, rm listen from 0x%x", dm_disc_mask);
    dm_disc_mask &= NFA_DM_DISC_MASK_POLL;
  }
  if (nfa_dm_is_p2p_paused()) {
    dm_disc_mask &= ~NFA_DM_DISC_MASK_NFC_DEP;
  }

  /* Check polling A */
  if (dm_disc_mask &
      (NFA_DM_DISC_MASK_PA_T1T | NFA_DM_DISC_MASK_PA_T2T |
       NFA_DM_DISC_MASK_PA_ISO_DEP | NFA_DM_DISC_MASK_PA_NFC_DEP |
       NFA_DM_DISC_MASK_P_LEGACY)) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_A;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pa;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check polling B */
  if (dm_disc_mask & NFA_DM_DISC_MASK_PB_ISO_DEP) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_B;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pb;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check polling F */
  if (dm_disc_mask & (NFA_DM_DISC_MASK_PF_T3T | NFA_DM_DISC_MASK_PF_NFC_DEP)) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_F;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pf;
    num_params++;

    if (num_params >= max_params) return num_params;
  }
  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    /* Check polling Active mode  */
    if (dm_disc_mask & NFA_DM_DISC_MASK_PACM_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_ACTIVE;
      disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pacm;
      num_params++;

      if (num_params >= max_params) return num_params;
    }
  } else {
    /* Check polling A Active mode  */
    if (dm_disc_mask & NFA_DM_DISC_MASK_PAA_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_A_ACTIVE;
      disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->paa;
      num_params++;

      if (num_params >= max_params) return num_params;
    }

    /* Check polling F Active mode  */
    if (dm_disc_mask & NFA_DM_DISC_MASK_PFA_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_F_ACTIVE;
      disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pfa;
      num_params++;

      if (num_params >= max_params) return num_params;
    }
  }
  /* Check listening A */
  if (dm_disc_mask &
      (NFA_DM_DISC_MASK_LA_T1T | NFA_DM_DISC_MASK_LA_T2T |
       NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LA_NFC_DEP)) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_A;
    disc_params[num_params].frequency = 1;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check listening B */
  if (dm_disc_mask & NFA_DM_DISC_MASK_LB_ISO_DEP) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_B;
    disc_params[num_params].frequency = 1;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check listening F */
  if (dm_disc_mask & (NFA_DM_DISC_MASK_LF_T3T | NFA_DM_DISC_MASK_LF_NFC_DEP)) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_F;
    disc_params[num_params].frequency = 1;
    num_params++;

    if (num_params >= max_params) return num_params;
  }
  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    /* Check polling Active mode  */
    if (dm_disc_mask & NFA_DM_DISC_MASK_LACM_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_ACTIVE;
      disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pacm;
      num_params++;
      if (num_params >= max_params) return num_params;
    }
  } else {
    /* Check listening A Active mode */
    if (dm_disc_mask & NFA_DM_DISC_MASK_LAA_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE;
      disc_params[num_params].frequency = 1;
      num_params++;

      if (num_params >= max_params) return num_params;
    }

    /* Check listening F Active mode */
    if (dm_disc_mask & NFA_DM_DISC_MASK_LFA_NFC_DEP) {
      disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE;
      disc_params[num_params].frequency = 1;
      num_params++;

      if (num_params >= max_params) return num_params;
    }
  }

  /* Check polling ISO 15693 */
  if (dm_disc_mask & NFA_DM_DISC_MASK_P_T5T) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_V;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pi93;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check polling B' */
  if (dm_disc_mask & NFA_DM_DISC_MASK_P_B_PRIME) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_B_PRIME;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pbp;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check polling KOVIO */
  if (dm_disc_mask & NFA_DM_DISC_MASK_P_KOVIO) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_POLL_KOVIO;
    disc_params[num_params].frequency = p_nfa_dm_rf_disc_freq_cfg->pk;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check listening ISO 15693 */
  if (dm_disc_mask & NFA_DM_DISC_MASK_L_ISO15693) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_ISO15693;
    disc_params[num_params].frequency = 1;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  /* Check listening B' */
  if (dm_disc_mask & NFA_DM_DISC_MASK_L_B_PRIME) {
    disc_params[num_params].type = NFC_DISCOVERY_TYPE_LISTEN_B_PRIME;
    disc_params[num_params].frequency = 1;
    num_params++;

    if (num_params >= max_params) return num_params;
  }

  return num_params;
}

/*******************************************************************************
**
** Function         nfa_dm_set_rf_listen_mode_config
**
** Description      Update listening protocol to NFCC
**
** Returns          NFA_STATUS_OK if success
**
*******************************************************************************/
static tNFA_STATUS nfa_dm_set_rf_listen_mode_config(
    tNFA_DM_DISC_TECH_PROTO_MASK tech_proto_mask) {
  uint8_t params[40], *p;
  uint8_t platform = 0;
  uint8_t sens_info = 0;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("tech_proto_mask = 0x%08X", tech_proto_mask);

  /*
  ** T1T listen     LA_PROT 0x80, LA_SENS_RES byte1:0x00 byte2:0x0C
  ** T2T listen     LA_PROT 0x00
  ** T3T listen     No bit for T3T in LF_PROT (CE T3T set listen parameters,
  **                system code, NFCID2, etc.)
  ** ISO-DEP listen LA_PROT 0x01, LB_PROT 0x01
  ** NFC-DEP listen LA_PROT 0x02, LF_PROT 0x02
  */

  if (tech_proto_mask & NFA_DM_DISC_MASK_LA_T1T) {
    platform = NCI_PARAM_PLATFORM_T1T;
  } else if (tech_proto_mask & NFA_DM_DISC_MASK_LA_T2T) {
    /* platform = 0 and sens_info = 0 */
  } else {
    if (tech_proto_mask & NFA_DM_DISC_MASK_LA_ISO_DEP) {
      sens_info |= NCI_PARAM_SEL_INFO_ISODEP;
    }

    if (tech_proto_mask & NFA_DM_DISC_MASK_LA_NFC_DEP) {
      sens_info |= NCI_PARAM_SEL_INFO_NFCDEP;
    }
  }

  p = params;

  /*
   * for Listen A
   *
   * Set ATQA 0x0C00 for T1T listen
   * If the ATQA values are 0x0000, then the FW will use 0x0400
   * which works for ISODEP, T2T and NFCDEP.
   *
   * In mode NFCC allowed to manage RF config (NFCC_CONFIG_CONTROL),
   * DH will only add RF parameters for itself.
   * In this case, we must program LA_SEL_INFO for DH techs only
   */
    UINT8_TO_STREAM(p, NFC_PMID_LA_BIT_FRAME_SDD);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_BIT_FRAME_SDD);
    UINT8_TO_STREAM(p, 0x04);
    UINT8_TO_STREAM(p, NFC_PMID_LA_PLATFORM_CONFIG);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_PLATFORM_CONFIG);
    UINT8_TO_STREAM(p, platform);
    UINT8_TO_STREAM(p, NFC_PMID_LA_SEL_INFO);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_SEL_INFO);
    UINT8_TO_STREAM(p, sens_info);

  /* for Listen B */

    UINT8_TO_STREAM(p, NFC_PMID_LB_SENSB_INFO);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LB_SENSB_INFO);
    if (tech_proto_mask & NFA_DM_DISC_MASK_LB_ISO_DEP) {
      UINT8_TO_STREAM(p, NCI_LISTEN_PROTOCOL_ISO_DEP);
    } else {
      UINT8_TO_STREAM(p, 0x00);
    }

  /* for Listen F */
  /* NFCC can support NFC-DEP and T3T listening based on NFCID routing
   * regardless of NFC-F tech routing */
  UINT8_TO_STREAM(p, NFC_PMID_LF_PROTOCOL);
  UINT8_TO_STREAM(p, NCI_PARAM_LEN_LF_PROTOCOL);
  if ((tech_proto_mask & NFA_DM_DISC_MASK_LF_NFC_DEP) &&
      !nfa_dm_is_p2p_paused()) {
    UINT8_TO_STREAM(p, NCI_LISTEN_PROTOCOL_NFC_DEP);
  } else {
    UINT8_TO_STREAM(p, 0x00);
  }

  if (p > params) {
    nfa_dm_check_set_config((uint8_t)(p - params), params, false);
  }

  return NFA_STATUS_OK;
}

/*******************************************************************************
**
** Function         nfa_dm_set_total_duration
**
** Description      Update total duration to NFCC
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_set_total_duration(void) {
  uint8_t params[10], *p;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p = params;

  /* for total duration */
  UINT8_TO_STREAM(p, NFC_PMID_TOTAL_DURATION);
  UINT8_TO_STREAM(p, NCI_PARAM_LEN_TOTAL_DURATION);
  UINT16_TO_STREAM(p, nfa_dm_cb.disc_cb.disc_duration);

  if (p > params) {
    nfa_dm_check_set_config((uint8_t)(p - params), params, false);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_set_rf_listen_mode_raw_config
**
** Description      Set raw listen parameters
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_set_rf_listen_mode_raw_config(
    tNFA_DM_DISC_TECH_PROTO_MASK* p_disc_mask) {
  tNFA_DM_DISC_TECH_PROTO_MASK disc_mask = 0;
  tNFA_LISTEN_CFG* p_cfg = &nfa_dm_cb.disc_cb.excl_listen_config;
  uint8_t params[250], *p, xx;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /*
  ** Discovery Configuration Parameters for Listen A
  */
  if ((nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_A] ==
       NFA_DM_DISC_HOST_ID_DH) &&
      (p_cfg->la_enable)) {
    p = params;

    UINT8_TO_STREAM(p, NFC_PMID_LA_BIT_FRAME_SDD);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_BIT_FRAME_SDD);
    UINT8_TO_STREAM(p, p_cfg->la_bit_frame_sdd);

    UINT8_TO_STREAM(p, NFC_PMID_LA_PLATFORM_CONFIG);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_PLATFORM_CONFIG);
    UINT8_TO_STREAM(p, p_cfg->la_platform_config);

    UINT8_TO_STREAM(p, NFC_PMID_LA_SEL_INFO);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LA_SEL_INFO);
    UINT8_TO_STREAM(p, p_cfg->la_sel_info);

    if (p_cfg->la_platform_config == NCI_PARAM_PLATFORM_T1T) {
      disc_mask |= NFA_DM_DISC_MASK_LA_T1T;
    } else {
      /* If T4T or NFCDEP */
      if (p_cfg->la_sel_info & NCI_PARAM_SEL_INFO_ISODEP) {
        disc_mask |= NFA_DM_DISC_MASK_LA_ISO_DEP;
      }

      if (p_cfg->la_sel_info & NCI_PARAM_SEL_INFO_NFCDEP) {
        disc_mask |= NFA_DM_DISC_MASK_LA_NFC_DEP;
      }

      /* If neither, T4T nor NFCDEP, then its T2T */
      if (disc_mask == 0) {
        disc_mask |= NFA_DM_DISC_MASK_LA_T2T;
      }
    }

    UINT8_TO_STREAM(p, NFC_PMID_LA_NFCID1);
    UINT8_TO_STREAM(p, p_cfg->la_nfcid1_len);
    ARRAY_TO_STREAM(p, p_cfg->la_nfcid1, p_cfg->la_nfcid1_len);

    nfa_dm_check_set_config((uint8_t)(p - params), params, false);
  }

  /*
  ** Discovery Configuration Parameters for Listen B
  */
  if ((nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_B] ==
       NFA_DM_DISC_HOST_ID_DH) &&
      (p_cfg->lb_enable)) {
    p = params;

    UINT8_TO_STREAM(p, NFC_PMID_LB_SENSB_INFO);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LB_SENSB_INFO);
    UINT8_TO_STREAM(p, p_cfg->lb_sensb_info);

    UINT8_TO_STREAM(p, NFC_PMID_LB_NFCID0);
    UINT8_TO_STREAM(p, p_cfg->lb_nfcid0_len);
    ARRAY_TO_STREAM(p, p_cfg->lb_nfcid0, p_cfg->lb_nfcid0_len);

    UINT8_TO_STREAM(p, NFC_PMID_LB_APPDATA);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LB_APPDATA);
    ARRAY_TO_STREAM(p, p_cfg->lb_app_data, NCI_PARAM_LEN_LB_APPDATA);

    UINT8_TO_STREAM(p, NFC_PMID_LB_SFGI);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_cfg->lb_adc_fo);

    UINT8_TO_STREAM(p, NFC_PMID_LB_ADC_FO);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LB_ADC_FO);
    UINT8_TO_STREAM(p, p_cfg->lb_adc_fo);

    nfa_dm_check_set_config((uint8_t)(p - params), params, false);

    if (p_cfg->lb_sensb_info & NCI_LISTEN_PROTOCOL_ISO_DEP) {
      disc_mask |= NFA_DM_DISC_MASK_LB_ISO_DEP;
    }
  }

  /*
  ** Discovery Configuration Parameters for Listen F
  */
  if ((nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_F] ==
       NFA_DM_DISC_HOST_ID_DH) &&
      (p_cfg->lf_enable)) {
    p = params;

    UINT8_TO_STREAM(p, NFC_PMID_LF_CON_BITR_F);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_cfg->lf_con_bitr_f);

    UINT8_TO_STREAM(p, NFC_PMID_LF_PROTOCOL);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LF_PROTOCOL);
    UINT8_TO_STREAM(p, p_cfg->lf_protocol_type);

    UINT8_TO_STREAM(p, NFC_PMID_LF_T3T_FLAGS2);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LF_T3T_FLAGS2);
    UINT16_TO_STREAM(p, p_cfg->lf_t3t_flags);

    /* if the bit at position X is set to 0, SC/NFCID2 with index X shall be
     * ignored */
    for (xx = 0; xx < NFA_LF_MAX_SC_NFCID2; xx++) {
      if ((p_cfg->lf_t3t_flags & (0x0001 << xx)) != 0x0000) {
        UINT8_TO_STREAM(p, NFC_PMID_LF_T3T_ID1 + xx);
        UINT8_TO_STREAM(p, NCI_SYSTEMCODE_LEN + NCI_NFCID2_LEN);
        ARRAY_TO_STREAM(p, p_cfg->lf_t3t_identifier[xx],
                        NCI_SYSTEMCODE_LEN + NCI_NFCID2_LEN);
      }
    }

    UINT8_TO_STREAM(p, NFC_PMID_LF_T3T_PMM);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_LF_T3T_PMM);
    ARRAY_TO_STREAM(p, p_cfg->lf_t3t_pmm, NCI_PARAM_LEN_LF_T3T_PMM);

    nfa_dm_check_set_config((uint8_t)(p - params), params, false);

    if (p_cfg->lf_t3t_flags != NCI_LF_T3T_FLAGS2_ALL_DISABLED) {
      disc_mask |= NFA_DM_DISC_MASK_LF_T3T;
    }
    if (p_cfg->lf_protocol_type & NCI_LISTEN_PROTOCOL_NFC_DEP) {
      disc_mask |= NFA_DM_DISC_MASK_LF_NFC_DEP;
    }
  }

  /*
  ** Discovery Configuration Parameters for Listen ISO-DEP
  */
  if ((disc_mask &
       (NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LB_ISO_DEP)) &&
      (p_cfg->li_enable)) {
    p = params;

    UINT8_TO_STREAM(p, NFC_PMID_FWI);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_FWI);
    UINT8_TO_STREAM(p, p_cfg->li_fwi);

    if (disc_mask & NFA_DM_DISC_MASK_LA_ISO_DEP) {
      UINT8_TO_STREAM(p, NFC_PMID_LA_HIST_BY);
      UINT8_TO_STREAM(p, p_cfg->la_hist_bytes_len);
      ARRAY_TO_STREAM(p, p_cfg->la_hist_bytes, p_cfg->la_hist_bytes_len);
    }

    if (disc_mask & NFA_DM_DISC_MASK_LB_ISO_DEP) {
      UINT8_TO_STREAM(p, NFC_PMID_LB_H_INFO);
      UINT8_TO_STREAM(p, p_cfg->lb_h_info_resp_len);
      ARRAY_TO_STREAM(p, p_cfg->lb_h_info_resp, p_cfg->lb_h_info_resp_len);
    }

    nfa_dm_check_set_config((uint8_t)(p - params), params, false);
  }

  /*
  ** Discovery Configuration Parameters for Listen NFC-DEP
  */
  if ((disc_mask &
       (NFA_DM_DISC_MASK_LA_NFC_DEP | NFA_DM_DISC_MASK_LF_NFC_DEP)) &&
      (p_cfg->ln_enable)) {
    p = params;

    UINT8_TO_STREAM(p, NFC_PMID_WT);
    UINT8_TO_STREAM(p, NCI_PARAM_LEN_WT);
    UINT8_TO_STREAM(p, p_cfg->ln_wt);

    UINT8_TO_STREAM(p, NFC_PMID_ATR_RES_GEN_BYTES);
    UINT8_TO_STREAM(p, p_cfg->ln_atr_res_gen_bytes_len);
    ARRAY_TO_STREAM(p, p_cfg->ln_atr_res_gen_bytes,
                    p_cfg->ln_atr_res_gen_bytes_len);

    UINT8_TO_STREAM(p, NFC_PMID_ATR_RSP_CONFIG);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_cfg->ln_atr_res_config);

    nfa_dm_check_set_config((uint8_t)(p - params), params, false);
  }

  *p_disc_mask = disc_mask;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("disc_mask = 0x%x", disc_mask);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_get_disc_mask
**
** Description      Convert RF technology, mode and protocol to bit mask
**
** Returns          tNFA_DM_DISC_TECH_PROTO_MASK
**
*******************************************************************************/
static tNFA_DM_DISC_TECH_PROTO_MASK nfa_dm_disc_get_disc_mask(
    tNFC_RF_TECH_N_MODE tech_n_mode, tNFC_PROTOCOL protocol) {
  /* Set initial disc_mask to legacy poll or listen */
  tNFA_DM_DISC_TECH_PROTO_MASK disc_mask =
      ((tech_n_mode & 0x80) ? NFA_DM_DISC_MASK_L_LEGACY
                            : NFA_DM_DISC_MASK_P_LEGACY);

  if (NFC_DISCOVERY_TYPE_POLL_A == tech_n_mode) {
    switch (protocol) {
      case NFC_PROTOCOL_T1T:
        disc_mask = NFA_DM_DISC_MASK_PA_T1T;
        break;
      case NFC_PROTOCOL_T2T:
        disc_mask = NFA_DM_DISC_MASK_PA_T2T;
        break;
      case NFC_PROTOCOL_ISO_DEP:
        disc_mask = NFA_DM_DISC_MASK_PA_ISO_DEP;
        break;
      case NFC_PROTOCOL_NFC_DEP:
        disc_mask = NFA_DM_DISC_MASK_PA_NFC_DEP;
        break;
    }
  } else if (NFC_DISCOVERY_TYPE_POLL_B == tech_n_mode) {
    if (protocol == NFC_PROTOCOL_ISO_DEP)
      disc_mask = NFA_DM_DISC_MASK_PB_ISO_DEP;
  } else if (NFC_DISCOVERY_TYPE_POLL_F == tech_n_mode) {
    if (protocol == NFC_PROTOCOL_T3T)
      disc_mask = NFA_DM_DISC_MASK_PF_T3T;
    else if (protocol == NFC_PROTOCOL_NFC_DEP)
      disc_mask = NFA_DM_DISC_MASK_PF_NFC_DEP;
  } else if (NFC_DISCOVERY_TYPE_POLL_V == tech_n_mode) {
    disc_mask = NFA_DM_DISC_MASK_P_T5T;
  } else if (NFC_DISCOVERY_TYPE_POLL_B_PRIME == tech_n_mode) {
    disc_mask = NFA_DM_DISC_MASK_P_B_PRIME;
  } else if (NFC_DISCOVERY_TYPE_POLL_KOVIO == tech_n_mode) {
    disc_mask = NFA_DM_DISC_MASK_P_KOVIO;
  } else if (NFC_DISCOVERY_TYPE_LISTEN_A == tech_n_mode) {
    switch (protocol) {
      case NFC_PROTOCOL_T1T:
        disc_mask = NFA_DM_DISC_MASK_LA_T1T;
        break;
      case NFC_PROTOCOL_T2T:
        disc_mask = NFA_DM_DISC_MASK_LA_T2T;
        break;
      case NFC_PROTOCOL_ISO_DEP:
        disc_mask = NFA_DM_DISC_MASK_LA_ISO_DEP;
        break;
      case NFC_PROTOCOL_NFC_DEP:
        disc_mask = NFA_DM_DISC_MASK_LA_NFC_DEP;
        break;
    }
  } else if (NFC_DISCOVERY_TYPE_LISTEN_B == tech_n_mode) {
    if (protocol == NFC_PROTOCOL_ISO_DEP)
      disc_mask = NFA_DM_DISC_MASK_LB_ISO_DEP;
  } else if (NFC_DISCOVERY_TYPE_LISTEN_F == tech_n_mode) {
    if (protocol == NFC_PROTOCOL_T3T)
      disc_mask = NFA_DM_DISC_MASK_LF_T3T;
    else if (protocol == NFC_PROTOCOL_NFC_DEP)
      disc_mask = NFA_DM_DISC_MASK_LF_NFC_DEP;
  } else if (NFC_DISCOVERY_TYPE_LISTEN_ISO15693 == tech_n_mode) {
    disc_mask = NFA_DM_DISC_MASK_L_ISO15693;
  } else if (NFC_DISCOVERY_TYPE_LISTEN_B_PRIME == tech_n_mode) {
    disc_mask = NFA_DM_DISC_MASK_L_B_PRIME;
  }
  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    if (NFC_DISCOVERY_TYPE_POLL_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_PACM_NFC_DEP;
    } else if (NFC_DISCOVERY_TYPE_LISTEN_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_LACM_NFC_DEP;
    }
  } else {
    if (NFC_DISCOVERY_TYPE_POLL_A_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_PAA_NFC_DEP;
    } else if (NFC_DISCOVERY_TYPE_POLL_F_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_PFA_NFC_DEP;
    } else if (NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_LAA_NFC_DEP;
    } else if (NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE == tech_n_mode) {
      disc_mask = NFA_DM_DISC_MASK_LFA_NFC_DEP;
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "tech_n_mode:0x%X, protocol:0x%X, "
      "disc_mask:0x%X",
      tech_n_mode, protocol, disc_mask);
  return (disc_mask);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_discovery_cback
**
** Description      Discovery callback event from NFC
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_discovery_cback(tNFC_DISCOVER_EVT event,
                                        tNFC_DISCOVER* p_data) {
  tNFA_DM_RF_DISC_SM_EVENT dm_disc_event = NFA_DM_DISC_SM_MAX_EVENT;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event:0x%X", event);

  switch (event) {
    case NFC_START_DEVT:
      dm_disc_event = NFA_DM_RF_DISCOVER_RSP;
      break;
    case NFC_RESULT_DEVT:
      dm_disc_event = NFA_DM_RF_DISCOVER_NTF;
      break;
    case NFC_SELECT_DEVT:
      dm_disc_event = NFA_DM_RF_DISCOVER_SELECT_RSP;
      break;
    case NFC_ACTIVATE_DEVT:
      dm_disc_event = NFA_DM_RF_INTF_ACTIVATED_NTF;
      break;
    case NFC_DEACTIVATE_DEVT:
      if (p_data->deactivate.is_ntf) {
        dm_disc_event = NFA_DM_RF_DEACTIVATE_NTF;
        if ((p_data->deactivate.type == NFC_DEACTIVATE_TYPE_IDLE) ||
            (p_data->deactivate.type == NFC_DEACTIVATE_TYPE_DISCOVERY)) {
          NFC_SetReassemblyFlag(true);
          nfa_dm_cb.flags &= ~NFA_DM_FLAGS_RAW_FRAME;
        }
      } else
        dm_disc_event = NFA_DM_RF_DEACTIVATE_RSP;
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected event");
      return;
  }

  tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
  nfa_dm_rf_disc_data.nfc_discover = *p_data;
  nfa_dm_disc_sm_execute(dm_disc_event, &nfa_dm_rf_disc_data);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_notify_started
**
** Description      Report NFA_EXCLUSIVE_RF_CONTROL_STARTED_EVT or
**                  NFA_RF_DISCOVERY_STARTED_EVT, if needed
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_notify_started(tNFA_STATUS status) {
  tNFA_CONN_EVT_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfa_dm_disc_notify_started");
  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_NOTIFY) {
    nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_NOTIFY;

    evt_data.status = status;

    if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use)
      nfa_dm_conn_cback_event_notify(NFA_EXCLUSIVE_RF_CONTROL_STARTED_EVT,
                                     &evt_data);
    else
    {
      nfa_dm_conn_cback_event_notify(NFA_RF_DISCOVERY_STARTED_EVT, &evt_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_conn_event_notify
**
** Description      Notify application of CONN_CBACK event, using appropriate
**                  callback
**
** Returns          nothing
**
*******************************************************************************/
void nfa_dm_disc_conn_event_notify(uint8_t event, tNFA_STATUS status) {
  tNFA_CONN_EVT_DATA evt_data;

  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_NOTIFY) {
    nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_NOTIFY;
    evt_data.status = status;

    if (nfa_dm_cb.flags & NFA_DM_FLAGS_EXCL_RF_ACTIVE) {
      /* Use exclusive RF mode callback */
      if (nfa_dm_cb.p_excl_conn_cback)
        (*nfa_dm_cb.p_excl_conn_cback)(event, &evt_data);
    } else {
      (*nfa_dm_cb.p_conn_cback)(event, &evt_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_force_to_idle
**
** Description      Force NFCC to idle state while waiting for deactivation NTF
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
static tNFC_STATUS nfa_dm_disc_force_to_idle(void) {
  tNFC_STATUS status = NFC_STATUS_SEMANTIC_ERROR;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("disc_flags = 0x%x", nfa_dm_cb.disc_cb.disc_flags);

  /* do not execute more than one */
  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF) {
    nfa_dm_cb.disc_cb.disc_flags &= ~(NFA_DM_DISC_FLAGS_W4_NTF);
    nfa_dm_cb.disc_cb.disc_flags |= (NFA_DM_DISC_FLAGS_W4_RSP);
    nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
    status = NFC_Deactivate(NFC_DEACTIVATE_TYPE_IDLE);
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_deact_ntf_timeout_cback
**
** Description      Timeout while waiting for deactivation NTF
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_deact_ntf_timeout_cback(__attribute__((unused))
                                                TIMER_LIST_ENT* p_tle) {
  LOG(ERROR) << __func__;

  nfa_dm_disc_force_to_idle();
}

/*******************************************************************************
**
** Function         nfa_dm_send_deactivate_cmd
**
** Description      Send deactivate command to NFCC, if needed.
**
** Returns          NFC_STATUS_OK             - deactivate cmd is sent
**                  NCI_STATUS_FAILED         - no buffers
**                  NFC_STATUS_SEMANTIC_ERROR - this function does not attempt
**                                              to send deactivate cmd
**
*******************************************************************************/
static tNFC_STATUS nfa_dm_send_deactivate_cmd(tNFC_DEACT_TYPE deactivate_type) {
  tNFC_STATUS status = NFC_STATUS_SEMANTIC_ERROR;
  tNFA_DM_DISC_FLAGS w4_flags =
      nfa_dm_cb.disc_cb.disc_flags &
      (NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF);

  if (!w4_flags) {
    /* if deactivate CMD was not sent to NFCC */
    nfa_dm_cb.disc_cb.disc_flags |=
        (NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF);

    status = NFC_Deactivate(deactivate_type);

    if (!nfa_dm_cb.disc_cb.tle.in_use) {
      nfa_dm_cb.disc_cb.tle.p_cback =
          (TIMER_CBACK*)nfa_dm_disc_deact_ntf_timeout_cback;
      nfa_sys_start_timer(&nfa_dm_cb.disc_cb.tle, 0,
                          NFA_DM_DISC_TIMEOUT_W4_DEACT_NTF);
    }
  } else {
    if (deactivate_type == NFC_DEACTIVATE_TYPE_SLEEP) {
      status = NFC_STATUS_SEMANTIC_ERROR;
    } else if (nfa_dm_cb.disc_cb.tle.in_use) {
      status = NFC_STATUS_OK;
    } else {
      status = nfa_dm_disc_force_to_idle();
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         nfa_dm_start_rf_discover
**
** Description      Start RF discovery
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_start_rf_discover(void) {
  tNFC_DISCOVER_PARAMS disc_params[NFA_DM_MAX_DISC_PARAMS];
  tNFA_DM_DISC_TECH_PROTO_MASK dm_disc_mask = 0, poll_mask, listen_mask;
  uint8_t config_params[10], *p;
  uint8_t num_params, xx;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  /* Make sure that RF discovery was enabled, or some app has exclusive control
   */
  if ((!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_ENABLED)) &&
      (nfa_dm_cb.disc_cb.excl_disc_entry.in_use == false)) {
    return;
  }

  /* get listen mode routing table for technology */
  nfa_ee_get_tech_route(NFA_EE_PWR_STATE_ON, nfa_dm_cb.disc_cb.listen_RT);

  if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
    nfa_dm_set_rf_listen_mode_raw_config(&dm_disc_mask);
    dm_disc_mask |= (nfa_dm_cb.disc_cb.excl_disc_entry.requested_disc_mask &
                     NFA_DM_DISC_MASK_POLL);
    nfa_dm_cb.disc_cb.excl_disc_entry.selected_disc_mask = dm_disc_mask;
  } else {
    /* Collect RF discovery request from sub-modules */
    for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
      if (nfa_dm_cb.disc_cb.entry[xx].in_use) {
        poll_mask = (nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                     NFA_DM_DISC_MASK_POLL);

        /* clear poll mode technolgies and protocols which are already used by
         * others */
        poll_mask &= ~(dm_disc_mask & NFA_DM_DISC_MASK_POLL);

        listen_mask = 0;

        /*
        ** add listen mode technolgies and protocols if host ID is
        ** matched to listen mode routing table
        */

        /* NFC-A */
        if (nfa_dm_cb.disc_cb.entry[xx].host_id ==
            nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_A]) {
          listen_mask |=
              nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
              (NFA_DM_DISC_MASK_LA_T1T | NFA_DM_DISC_MASK_LA_T2T |
               NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LA_NFC_DEP);
          if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
            listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                           NFA_DM_DISC_MASK_LACM_NFC_DEP;
          } else {
            listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                           NFA_DM_DISC_MASK_LAA_NFC_DEP;
          }
        } else {
          /* host can listen ISO-DEP based on AID routing */
          listen_mask |= (nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                          NFA_DM_DISC_MASK_LA_ISO_DEP);
          /* host can listen NFC-DEP based on protocol routing */
           listen_mask |= (nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                            NFA_DM_DISC_MASK_LA_NFC_DEP);
          if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
            listen_mask |= (nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                            NFA_DM_DISC_MASK_LACM_NFC_DEP);
          } else {
            listen_mask |= (nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                            NFA_DM_DISC_MASK_LAA_NFC_DEP);
          }
        }

        /* NFC-B */
        /* multiple hosts can listen ISO-DEP based on AID routing */
        listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                       NFA_DM_DISC_MASK_LB_ISO_DEP;

        /* NFC-F */
        /* NFCC can support NFC-DEP and T3T listening based on NFCID routing
         * regardless of NFC-F tech routing */
        listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                       (NFA_DM_DISC_MASK_LF_T3T | NFA_DM_DISC_MASK_LF_NFC_DEP);
        if (NFC_GetNCIVersion() != NCI_VERSION_2_0) {
          listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                         NFA_DM_DISC_MASK_LFA_NFC_DEP;
        }
        /* NFC-B Prime */
        if (nfa_dm_cb.disc_cb.entry[xx].host_id ==
            nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_BP]) {
          listen_mask |= nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask &
                         NFA_DM_DISC_MASK_L_B_PRIME;
        }

        /*
        ** clear listen mode technolgies and protocols which are already
        ** used by others
        */

        /* Check if other modules are listening T1T or T2T */
        if (dm_disc_mask &
            (NFA_DM_DISC_MASK_LA_T1T | NFA_DM_DISC_MASK_LA_T2T)) {
          listen_mask &=
              ~(NFA_DM_DISC_MASK_LA_T1T | NFA_DM_DISC_MASK_LA_T2T |
                NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LA_NFC_DEP);
        }

        /* T1T/T2T has priority on NFC-A */
        if ((dm_disc_mask &
             (NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LA_NFC_DEP)) &&
            (listen_mask &
             (NFA_DM_DISC_MASK_LA_T1T | NFA_DM_DISC_MASK_LA_T2T))) {
          dm_disc_mask &=
              ~(NFA_DM_DISC_MASK_LA_ISO_DEP | NFA_DM_DISC_MASK_LA_NFC_DEP);
        }

        /* Don't remove ISO-DEP because multiple hosts can listen ISO-DEP based
         * on AID routing */

        /* Check if other modules are listening NFC-DEP */
        if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
          if (dm_disc_mask &
              (NFA_DM_DISC_MASK_LA_NFC_DEP | NFA_DM_DISC_MASK_LACM_NFC_DEP)) {
            listen_mask &=
                ~(NFA_DM_DISC_MASK_LA_NFC_DEP | NFA_DM_DISC_MASK_LACM_NFC_DEP);
          }
        } else {
          if (dm_disc_mask &
              (NFA_DM_DISC_MASK_LA_NFC_DEP | NFA_DM_DISC_MASK_LAA_NFC_DEP)) {
            listen_mask &=
                ~(NFA_DM_DISC_MASK_LA_NFC_DEP | NFA_DM_DISC_MASK_LAA_NFC_DEP);
          }
        }

        nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask =
            poll_mask | listen_mask;

        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "nfa_dm_cb.disc_cb.entry[%d].selected_disc_mask = 0x%x", xx,
            nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask);

        dm_disc_mask |= nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask;
      }
    }

    /* Let P2P set GEN bytes for LLCP to NFCC */
    if (dm_disc_mask & NFA_DM_DISC_MASK_NFC_DEP) {
      nfa_p2p_set_config(dm_disc_mask);
    }
    if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
      if (dm_disc_mask &
          (NFA_DM_DISC_MASK_PF_NFC_DEP | NFA_DM_DISC_MASK_PF_T3T)) {
        /* According to the NFC Forum Activity spec, controllers must:
         * 1) Poll with RC=0 and SC=FFFF to find NFC-DEP targets
         * 2) Poll with RC=1 and SC=FFFF to find T3T targets
         * Many controllers don't do this yet, and seem to be activating
         * NFC-DEP by default.
         *
         * We can at least fix the scenario where we're not interested
         * in NFC-DEP, by setting RC=1 in that case. Otherwise, keep
         * the default of RC=0. */
        p = config_params;
        UINT8_TO_STREAM(p, NFC_PMID_PF_RC);
        UINT8_TO_STREAM(p, NCI_PARAM_LEN_PF_RC);
        if ((dm_disc_mask & NFA_DM_DISC_MASK_PF_NFC_DEP) &&
            !nfa_dm_is_p2p_paused()) {
          UINT8_TO_STREAM(p, 0x00);  // RC=0
        } else {
          UINT8_TO_STREAM(p, 0x01);  // RC=1
        }
        nfa_dm_check_set_config(p - config_params, config_params, false);
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("dm_disc_mask = 0x%x", dm_disc_mask);

  /* Get Discovery Technology parameters */
  num_params = nfa_dm_get_rf_discover_config(dm_disc_mask, disc_params,
                                             NFA_DM_MAX_DISC_PARAMS);

  if (num_params) {
    /*
    ** NFCC will abort programming personality slots if not available.
    ** NFCC programs the personality slots in the following order of RF
    ** technologies: NFC-A, NFC-B, NFC-BP, NFC-I93
    */

    /* if this is not for exclusive control */
    if (!nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
      /* update listening protocols in each NFC technology */
      nfa_dm_set_rf_listen_mode_config(dm_disc_mask);
    }

    /* Set polling duty cycle */
    nfa_dm_set_total_duration();
    nfa_dm_cb.disc_cb.dm_disc_mask = dm_disc_mask;

    NFC_DiscoveryStart(num_params, disc_params, nfa_dm_disc_discovery_cback);
    /* set flag about waiting for response in IDLE state */
    nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;

    /* register callback to get interface error NTF */
    NFC_SetStaticRfCback(nfa_dm_disc_data_cback);
  } else {
    /* RF discovery is started but there is no valid technology or protocol to
     * discover */
    nfa_dm_disc_notify_started(NFA_STATUS_OK);
  }

  /* if Kovio presence check timer is running, timeout callback will reset the
   * activation information */
  if ((nfa_dm_cb.disc_cb.activated_protocol != NFC_PROTOCOL_KOVIO) ||
      (!nfa_dm_cb.disc_cb.kovio_tle.in_use)) {
    /* reset protocol and hanlde of activated sub-module */
    nfa_dm_cb.disc_cb.activated_protocol = NFA_PROTOCOL_INVALID;
    nfa_dm_cb.disc_cb.activated_handle = NFA_HANDLE_INVALID;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_notify_discovery
**
** Description      Send RF discovery notification to upper layer
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_notify_discovery(tNFA_DM_RF_DISC_DATA* p_data) {
  tNFA_CONN_EVT_DATA conn_evt;

  /* let application select a device */
  conn_evt.disc_result.status = NFA_STATUS_OK;
  memcpy(&(conn_evt.disc_result.discovery_ntf), &(p_data->nfc_discover.result),
         sizeof(tNFC_RESULT_DEVT));

  nfa_dm_conn_cback_event_notify(NFA_DISC_RESULT_EVT, &conn_evt);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_handle_kovio_activation
**
** Description      Handle Kovio activation; whether it's new or repeated
**                  activation
**
** Returns          TRUE if repeated activation. No need to notify activated
**                  event to upper layer
**
*******************************************************************************/
bool nfa_dm_disc_handle_kovio_activation(tNFC_DISCOVER* p_data,
                                         tNFA_DISCOVER_CBACK* p_disc_cback) {
  tNFC_DISCOVER disc_data;

  if (nfa_dm_cb.disc_cb.kovio_tle.in_use) {
    /* if this is new Kovio bar code tag */
    if ((nfa_dm_cb.activated_nfcid_len !=
         p_data->activate.rf_tech_param.param.pk.uid_len) ||
        (memcmp(p_data->activate.rf_tech_param.param.pk.uid,
                nfa_dm_cb.activated_nfcid, nfa_dm_cb.activated_nfcid_len))) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("new Kovio tag is detected");

      /* notify presence check failure for previous tag, if presence check is
       * pending */
      nfa_dm_disc_report_kovio_presence_check(NFA_STATUS_FAILED);

      /* notify deactivation of previous activation before notifying new
       * activation */
      if (p_disc_cback) {
        disc_data.deactivate.type = NFA_DEACTIVATE_TYPE_IDLE;
        (*(p_disc_cback))(NFA_DM_RF_DISC_DEACTIVATED_EVT, &disc_data);
      }

      /* restart timer */
      nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                          NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);
    } else {
      /* notify presence check ok, if presence check is pending */
      nfa_dm_disc_report_kovio_presence_check(NFC_STATUS_OK);

      /* restart timer and do not notify upper layer */
      nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                          NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);
      return true;
    }
  } else {
    /* this is the first activation, so start timer and notify upper layer */
    nfa_dm_cb.disc_cb.kovio_tle.p_cback =
        (TIMER_CBACK*)nfa_dm_disc_kovio_timeout_cback;
    nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                        NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);
  }

  return false;
}

/*******************************************************************************
**
** Function         nfa_dm_disc_notify_activation
**
** Description      Send RF activation notification to sub-module
**
** Returns          NFA_STATUS_OK if success
**
*******************************************************************************/
static tNFA_STATUS nfa_dm_disc_notify_activation(tNFC_DISCOVER* p_data) {
  uint8_t xx, host_id_in_LRT;
  uint8_t iso_dep_t3t__listen = NFA_DM_DISC_NUM_ENTRIES;

  tNFC_RF_TECH_N_MODE tech_n_mode = p_data->activate.rf_tech_param.mode;
  tNFC_PROTOCOL protocol = p_data->activate.protocol;

  tNFA_DM_DISC_TECH_PROTO_MASK activated_disc_mask;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("tech_n_mode:0x%X, proto:0x%X", tech_n_mode, protocol);

  if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
    nfa_dm_cb.disc_cb.activated_tech_mode = tech_n_mode;
    nfa_dm_cb.disc_cb.activated_rf_disc_id = p_data->activate.rf_disc_id;
    nfa_dm_cb.disc_cb.activated_rf_interface = p_data->activate.intf_param.type;
    nfa_dm_cb.disc_cb.activated_protocol = protocol;
    nfa_dm_cb.disc_cb.activated_handle = NFA_HANDLE_INVALID;

    if (protocol == NFC_PROTOCOL_KOVIO) {
      /* check whether it's new or repeated activation */
      if (nfa_dm_disc_handle_kovio_activation(
              p_data, nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback)) {
        /* do not notify activation of Kovio to upper layer */
        return (NFA_STATUS_OK);
      }
    }

    if (nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback)
      (*(nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback))(
          NFA_DM_RF_DISC_ACTIVATED_EVT, p_data);

    return (NFA_STATUS_OK);
  }

  /* if this is NFCEE direct RF interface, notify activation to whoever
   * listening UICC */
  if (p_data->activate.intf_param.type == NFC_INTERFACE_EE_DIRECT_RF) {
    for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
      if ((nfa_dm_cb.disc_cb.entry[xx].in_use) &&
          (nfa_dm_cb.disc_cb.entry[xx].host_id != NFA_DM_DISC_HOST_ID_DH)) {
        nfa_dm_cb.disc_cb.activated_rf_disc_id = p_data->activate.rf_disc_id;
        nfa_dm_cb.disc_cb.activated_rf_interface =
            p_data->activate.intf_param.type;
        nfa_dm_cb.disc_cb.activated_protocol = NFC_PROTOCOL_UNKNOWN;
        nfa_dm_cb.disc_cb.activated_handle = xx;

        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "activated_rf_interface:0x%x, activated_handle: 0x%x",
            nfa_dm_cb.disc_cb.activated_rf_interface,
            nfa_dm_cb.disc_cb.activated_handle);

        if (nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)
          (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
              NFA_DM_RF_DISC_ACTIVATED_EVT, p_data);

        return (NFA_STATUS_OK);
      }
    }
    return (NFA_STATUS_FAILED);
  }

  /* get bit mask of technolgies/mode and protocol */
  activated_disc_mask = nfa_dm_disc_get_disc_mask(tech_n_mode, protocol);

  /* get host ID of technology from listen mode routing table */
  if (tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A) {
    host_id_in_LRT = nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_A];
  } else if (tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_B) {
    host_id_in_LRT = nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_B];
  } else if (tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_F) {
    host_id_in_LRT = nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_F];
  } else if (tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
    host_id_in_LRT = nfa_dm_cb.disc_cb.listen_RT[NFA_DM_DISC_LRT_NFC_BP];
  } else /* DH only */
  {
    host_id_in_LRT = NFA_DM_DISC_HOST_ID_DH;
  }

  if (protocol == NFC_PROTOCOL_NFC_DEP) {
    /* Force NFC-DEP to the host */
    host_id_in_LRT = NFA_DM_DISC_HOST_ID_DH;
  }

  for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
    /* if any matching NFC technology and protocol */
    if (nfa_dm_cb.disc_cb.entry[xx].in_use) {
      if (nfa_dm_cb.disc_cb.entry[xx].host_id == host_id_in_LRT) {
        if (nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask &
            activated_disc_mask)
          break;
      } else {
        /* check ISO-DEP listening even if host in LRT is not matched */
        if (protocol == NFC_PROTOCOL_ISO_DEP) {
          if ((tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A) &&
              (nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask &
               NFA_DM_DISC_MASK_LA_ISO_DEP)) {
            iso_dep_t3t__listen = xx;
          } else if ((tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_B) &&
                     (nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask &
                      NFA_DM_DISC_MASK_LB_ISO_DEP)) {
            iso_dep_t3t__listen = xx;
          }
        }
        /* check T3T listening even if host in LRT is not matched */
        else if (protocol == NFC_PROTOCOL_T3T) {
          if ((tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_F) &&
              (nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask &
               NFA_DM_DISC_MASK_LF_T3T)) {
            iso_dep_t3t__listen = xx;
          }
        }
      }
    }
  }

  if (xx >= NFA_DM_DISC_NUM_ENTRIES) {
    /* if any ISO-DEP or T3T listening even if host in LRT is not matched */
    xx = iso_dep_t3t__listen;
  }
  if (protocol == NFC_PROTOCOL_NFC_DEP &&
      (tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE ||
       tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE ||
       tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A)) {
    if (appl_dta_mode_flag == 1 && tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("DTA Mode Enabled : NFC-A Passive Listen Mode");
    }
  }

  if (xx < NFA_DM_DISC_NUM_ENTRIES) {
    nfa_dm_cb.disc_cb.activated_tech_mode = tech_n_mode;
    nfa_dm_cb.disc_cb.activated_rf_disc_id = p_data->activate.rf_disc_id;
    nfa_dm_cb.disc_cb.activated_rf_interface = p_data->activate.intf_param.type;
    nfa_dm_cb.disc_cb.activated_protocol = protocol;
    nfa_dm_cb.disc_cb.activated_handle = xx;

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("activated_protocol:0x%x, activated_handle: 0x%x",
                        nfa_dm_cb.disc_cb.activated_protocol,
                        nfa_dm_cb.disc_cb.activated_handle);

    if (protocol == NFC_PROTOCOL_KOVIO) {
      /* check whether it's new or repeated activation */
      if (nfa_dm_disc_handle_kovio_activation(
              p_data, nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)) {
        /* do not notify activation of Kovio to upper layer */
        return (NFA_STATUS_OK);
      }
    }

    if (nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)
      (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
          NFA_DM_RF_DISC_ACTIVATED_EVT, p_data);

    return (NFA_STATUS_OK);
  } else {
    nfa_dm_cb.disc_cb.activated_protocol = NFA_PROTOCOL_INVALID;
    nfa_dm_cb.disc_cb.activated_handle = NFA_HANDLE_INVALID;
    return (NFA_STATUS_FAILED);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_notify_deactivation
**
** Description      Send deactivation notification to sub-module
**
** Returns          None
**
*******************************************************************************/
static void nfa_dm_disc_notify_deactivation(tNFA_DM_RF_DISC_SM_EVENT sm_event,
                                            tNFC_DISCOVER* p_data) {
  tNFA_HANDLE xx;
  tNFA_CONN_EVT_DATA evt_data;
  tNFC_DISCOVER disc_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "activated_handle=%d", nfa_dm_cb.disc_cb.activated_handle);

  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_CHECKING) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("for sleep wakeup");
    return;
  }

  if (sm_event == NFA_DM_RF_DEACTIVATE_RSP) {
    /*
    ** Activation has been aborted by upper layer in
    ** NFA_DM_RFST_W4_ALL_DISCOVERIES or NFA_DM_RFST_W4_HOST_SELECT
    ** Deactivation by upper layer or RF link loss in
    ** NFA_DM_RFST_LISTEN_SLEEP
    ** No sub-module is activated at this state.
    */

    if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_LISTEN_SLEEP) {
      if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
        if (nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback) {
          disc_data.deactivate.type = NFA_DEACTIVATE_TYPE_IDLE;
          (*(nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback))(
              NFA_DM_RF_DISC_DEACTIVATED_EVT, &disc_data);
        }
      } else {
        /* let each sub-module handle deactivation */
        for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
          if ((nfa_dm_cb.disc_cb.entry[xx].in_use) &&
              (nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask &
               NFA_DM_DISC_MASK_LISTEN)) {
            disc_data.deactivate.type = NFA_DEACTIVATE_TYPE_IDLE;
            (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
                NFA_DM_RF_DISC_DEACTIVATED_EVT, &disc_data);
          }
        }
      }
    } else if ((!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_STOPPING)) ||
               (nfa_dm_cb.disc_cb.deact_notify_pending)) {
      xx = nfa_dm_cb.disc_cb.activated_handle;

      /* notify event to activated module if failed while reactivation */
      if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
        if (nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback) {
          disc_data.deactivate.type = NFA_DEACTIVATE_TYPE_IDLE;
          (*(nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback))(
              NFA_DM_RF_DISC_DEACTIVATED_EVT, p_data);
        }
      } else if ((xx < NFA_DM_DISC_NUM_ENTRIES) &&
                 (nfa_dm_cb.disc_cb.entry[xx].in_use) &&
                 (nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)) {
        (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
            NFA_DM_RF_DISC_DEACTIVATED_EVT, p_data);
      } else {
        /* notify deactivation to application if there is no activated module */
        evt_data.deactivated.type = NFA_DEACTIVATE_TYPE_IDLE;
        nfa_dm_conn_cback_event_notify(NFA_DEACTIVATED_EVT, &evt_data);
      }
    }
  } else {
    if (nfa_dm_cb.disc_cb.activated_protocol == NFC_PROTOCOL_KOVIO) {
      if (nfa_dm_cb.disc_cb.kovio_tle.in_use) {
        /* restart timer and do not notify upper layer */
        nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                            NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);
        return;
      }
      /* Otherwise, upper layer initiated deactivation. */
    }

    /* notify event to activated module */
    if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
      if (nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback) {
        disc_data.deactivate.type = NFA_DEACTIVATE_TYPE_IDLE;
        (*(nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback))(
            NFA_DM_RF_DISC_DEACTIVATED_EVT, p_data);
      }
    } else {
      xx = nfa_dm_cb.disc_cb.activated_handle;

      if ((xx < NFA_DM_DISC_NUM_ENTRIES) &&
          (nfa_dm_cb.disc_cb.entry[xx].in_use)) {
        if (nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)
          (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
              NFA_DM_RF_DISC_DEACTIVATED_EVT, p_data);
      }
    }
  }

  /* clear activated information */
  nfa_dm_cb.disc_cb.activated_tech_mode = 0;
  nfa_dm_cb.disc_cb.activated_rf_disc_id = 0;
  nfa_dm_cb.disc_cb.activated_rf_interface = 0;
  nfa_dm_cb.disc_cb.activated_protocol = NFA_PROTOCOL_INVALID;
  nfa_dm_cb.disc_cb.activated_handle = NFA_HANDLE_INVALID;
  nfa_dm_cb.disc_cb.deact_notify_pending = false;
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sleep_wakeup
**
** Description      Put tag to sleep, then wake it up. Can be used Perform
**                  legacy presence check or to wake up tag that went to HALT
**                  state
**
** Returns          TRUE if operation started
**
*******************************************************************************/
tNFC_STATUS nfa_dm_disc_sleep_wakeup(void) {
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_POLL_ACTIVE) {
    /* Deactivate to sleep mode */
    status = nfa_dm_send_deactivate_cmd(NFC_DEACTIVATE_TYPE_SLEEP);
    if (status == NFC_STATUS_OK) {
      /* deactivate to sleep is sent on behalf of sleep wakeup.
       * set the sleep wakeup information in control block */
      nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_CHECKING;
      nfa_dm_cb.disc_cb.deact_pending = false;
    }
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_dm_is_raw_frame_session
**
** Description      If NFA_SendRawFrame is called since RF activation,
**                  this function returns TRUE.
**
** Returns          TRUE if NFA_SendRawFrame is called
**
*******************************************************************************/
bool nfa_dm_is_raw_frame_session(void) {
  return ((nfa_dm_cb.flags & NFA_DM_FLAGS_RAW_FRAME) ? true : false);
}

/*******************************************************************************
**
** Function         nfa_dm_is_p2p_paused
**
** Description      If NFA_PauseP2p is called sand still effective,
**                  this function returns TRUE.
**
** Returns          TRUE if NFA_SendRawFrame is called
**
*******************************************************************************/
bool nfa_dm_is_p2p_paused(void) {
  return ((nfa_dm_cb.flags & NFA_DM_FLAGS_P2P_PAUSED) ? true : false);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_end_sleep_wakeup
**
** Description      Sleep Wakeup is complete
**
** Returns          None
**
*******************************************************************************/
static void nfa_dm_disc_end_sleep_wakeup(tNFC_STATUS status) {
  if ((nfa_dm_cb.disc_cb.activated_protocol == NFC_PROTOCOL_KOVIO) &&
      (nfa_dm_cb.disc_cb.kovio_tle.in_use)) {
    /* ignore it while doing Kovio presence check */
    return;
  }

  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_CHECKING) {
    nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_CHECKING;

    /* notify RW module that sleep wakeup is finished */
    nfa_rw_handle_sleep_wakeup_rsp(status);

    if (nfa_dm_cb.disc_cb.deact_pending) {
      nfa_dm_cb.disc_cb.deact_pending = false;
      /* Perform pending deactivate command and on response notfiy deactivation
       */
      nfa_dm_cb.disc_cb.deact_notify_pending = true;
      tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
      nfa_dm_rf_disc_data.deactivate_type =
          nfa_dm_cb.disc_cb.pending_deact_type;
      nfa_dm_disc_sm_execute(NFA_DM_RF_DEACTIVATE_CMD, &nfa_dm_rf_disc_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_kovio_timeout_cback
**
** Description      Timeout for Kovio bar code tag presence check
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_kovio_timeout_cback(__attribute__((unused))
                                            TIMER_LIST_ENT* p_tle) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* notify presence check failure, if presence check is pending */
  nfa_dm_disc_report_kovio_presence_check(NFC_STATUS_FAILED);

  if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_POLL_ACTIVE) {
    /* restart timer in case that upper layer's presence check interval is too
     * long */
    nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                        NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);
  } else {
    /* notify upper layer deactivated event */
    tNFC_DEACTIVATE_DEVT deact;
    deact.status = NFC_STATUS_OK;
    deact.type = NFC_DEACTIVATE_TYPE_DISCOVERY;
    deact.is_ntf = true;
    deact.reason = NFC_DEACTIVATE_REASON_DH_REQ;
    tNFC_DISCOVER nfc_discover;
    nfc_discover.deactivate = deact;
    nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF, &nfc_discover);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_start_kovio_presence_check
**
** Description      Deactivate to discovery mode and wait for activation
**
** Returns          TRUE if operation started
**
*******************************************************************************/
tNFC_STATUS nfa_dm_disc_start_kovio_presence_check(void) {
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if ((nfa_dm_cb.disc_cb.activated_protocol == NFC_PROTOCOL_KOVIO) &&
      (nfa_dm_cb.disc_cb.kovio_tle.in_use)) {
    if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_POLL_ACTIVE) {
      /* restart timer */
      nfa_sys_start_timer(&nfa_dm_cb.disc_cb.kovio_tle, 0,
                          NFA_DM_DISC_TIMEOUT_KOVIO_PRESENCE_CHECK);

      /* Deactivate to discovery mode */
      status = nfa_dm_send_deactivate_cmd(NFC_DEACTIVATE_TYPE_DISCOVERY);

      if (status == NFC_STATUS_OK) {
        /* deactivate to sleep is sent on behalf of sleep wakeup.
         * set the sleep wakeup information in control block */
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_CHECKING;
        nfa_dm_cb.disc_cb.deact_pending = false;
      }
    } else {
      /* wait for next activation */
      nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_CHECKING;
      nfa_dm_cb.disc_cb.deact_pending = false;
      status = NFC_STATUS_OK;
    }
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_dm_disc_report_kovio_presence_check
**
** Description      Report Kovio presence check status
**
** Returns          None
**
*******************************************************************************/
static void nfa_dm_disc_report_kovio_presence_check(tNFC_STATUS status) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_CHECKING) {
    nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_CHECKING;

    /* notify RW module that sleep wakeup is finished */
    nfa_rw_handle_presence_check_rsp(status);

    if (nfa_dm_cb.disc_cb.deact_pending) {
      nfa_dm_cb.disc_cb.deact_pending = false;
      tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
      nfa_dm_rf_disc_data.deactivate_type =
          nfa_dm_cb.disc_cb.pending_deact_type;
      nfa_dm_disc_sm_execute(NFA_DM_RF_DEACTIVATE_CMD, &nfa_dm_rf_disc_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_data_cback
**
** Description      Monitoring interface error through data callback
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_data_cback(__attribute__((unused)) uint8_t conn_id,
                                   tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* if selection failed */
  if (event == NFC_ERROR_CEVT) {
    nfa_dm_disc_sm_execute(NFA_DM_CORE_INTF_ERROR_NTF, nullptr);
  } else if (event == NFC_DATA_CEVT) {
    GKI_freebuf(p_data->data.p_data);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_new_state
**
** Description      Processing discovery events in NFA_DM_RFST_IDLE state
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_disc_new_state(tNFA_DM_RF_DISC_STATE new_state) {
  tNFA_CONN_EVT_DATA evt_data;
  tNFA_DM_RF_DISC_STATE old_state = nfa_dm_cb.disc_cb.disc_state;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "old_state: %s (%d), new_state: %s (%d) "
      "disc_flags: 0x%x",
      nfa_dm_disc_state_2_str(nfa_dm_cb.disc_cb.disc_state).c_str(),
      nfa_dm_cb.disc_cb.disc_state, nfa_dm_disc_state_2_str(new_state).c_str(),
      new_state, nfa_dm_cb.disc_cb.disc_flags);

  nfa_dm_cb.disc_cb.disc_state = new_state;

  /* not error recovering */
  if ((new_state == NFA_DM_RFST_IDLE) &&
      (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP))) {
    if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_STOPPING) {
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_STOPPING;

      /* if exclusive RF control is stopping */
      if (nfa_dm_cb.flags & NFA_DM_FLAGS_EXCL_RF_ACTIVE) {
        if (old_state > NFA_DM_RFST_DISCOVERY) {
          /* notify deactivation to application */
          evt_data.deactivated.type = NFA_DEACTIVATE_TYPE_IDLE;
          nfa_dm_conn_cback_event_notify(NFA_DEACTIVATED_EVT, &evt_data);
        }

        nfa_dm_rel_excl_rf_control_and_notify();
      } else {
        evt_data.status = NFA_STATUS_OK;
        nfa_dm_conn_cback_event_notify(NFA_RF_DISCOVERY_STOPPED_EVT, &evt_data);
      }
    }
    if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_DISABLING) {
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_DISABLING;
      nfa_sys_check_disabled();
    }
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_idle
**
** Description      Processing discovery events in NFA_DM_RFST_IDLE state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_idle(tNFA_DM_RF_DISC_SM_EVENT event,
                                tNFA_DM_RF_DISC_DATA* p_data) {
  uint8_t xx;

  switch (event) {
    case NFA_DM_RF_DISCOVER_CMD:
      nfa_dm_start_rf_discover();
      break;

    case NFA_DM_RF_DISCOVER_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;

      if (p_data->nfc_discover.status == NFC_STATUS_OK) {
        nfa_dm_disc_new_state(NFA_DM_RFST_DISCOVERY);

        /* if RF discovery was stopped while waiting for response */
        if (nfa_dm_cb.disc_cb.disc_flags &
            (NFA_DM_DISC_FLAGS_STOPPING | NFA_DM_DISC_FLAGS_DISABLING)) {
          /* stop discovery */
          nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
          NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
          break;
        }

        if (nfa_dm_cb.disc_cb.excl_disc_entry.in_use) {
          if (nfa_dm_cb.disc_cb.excl_disc_entry.disc_flags &
              NFA_DM_DISC_FLAGS_NOTIFY) {
            nfa_dm_cb.disc_cb.excl_disc_entry.disc_flags &=
                ~NFA_DM_DISC_FLAGS_NOTIFY;

            if (nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback)
              (*(nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback))(
                  NFA_DM_RF_DISC_START_EVT, &p_data->nfc_discover);
          }
        } else {
          /* notify event to each module which is waiting for start */
          for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
            /* if registered module is waiting for starting discovery */
            if ((nfa_dm_cb.disc_cb.entry[xx].in_use) &&
                (nfa_dm_cb.disc_cb.dm_disc_mask &
                 nfa_dm_cb.disc_cb.entry[xx].selected_disc_mask) &&
                (nfa_dm_cb.disc_cb.entry[xx].disc_flags &
                 NFA_DM_DISC_FLAGS_NOTIFY)) {
              nfa_dm_cb.disc_cb.entry[xx].disc_flags &=
                  ~NFA_DM_DISC_FLAGS_NOTIFY;

              if (nfa_dm_cb.disc_cb.entry[xx].p_disc_cback)
                (*(nfa_dm_cb.disc_cb.entry[xx].p_disc_cback))(
                    NFA_DM_RF_DISC_START_EVT, &p_data->nfc_discover);
            }
          }
        }
        nfa_dm_disc_notify_started(p_data->nfc_discover.status);
      } else {
        /* in rare case that the discovery states of NFCC and DH mismatch and
         * NFCC rejects Discover Cmd
         * deactivate idle and then start disvocery when got deactivate rsp */
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;

    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;

      /* if NFCC goes to idle successfully */
      if (p_data->nfc_discover.status == NFC_STATUS_OK) {
        /* if DH forced to go idle while waiting for deactivation NTF */
        if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
          nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                          &(p_data->nfc_discover));

          /* check any pending flags like NFA_DM_DISC_FLAGS_STOPPING or
           * NFA_DM_DISC_FLAGS_DISABLING */
          nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
          /* check if need to restart discovery after resync discovery state
           * with NFCC */
          nfa_dm_start_rf_discover();
        }
        /* Otherwise, deactivating when getting unexpected activation */
      }
      /* Otherwise, wait for deactivation NTF */
      break;

    case NFA_DM_RF_DEACTIVATE_NTF:
      /* if NFCC sent this after NFCC had rejected deactivate CMD to idle while
       * deactivating */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
        if (p_data->nfc_discover.deactivate.type ==
            NFC_DEACTIVATE_TYPE_DISCOVERY) {
          /* stop discovery */
          nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
          NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
        } else {
          nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                          &(p_data->nfc_discover));
          /* check any pending flags like NFA_DM_DISC_FLAGS_STOPPING or
           * NFA_DM_DISC_FLAGS_DISABLING */
          nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
          /* check if need to restart discovery after resync discovery state
           * with NFCC */
          nfa_dm_start_rf_discover();
        }
      }
      /* Otherwise, deactivated when received unexpected activation in idle
       * state */
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_NTF;
      break;

    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      /* unexpected activation, deactivate to idle */
      nfa_dm_cb.disc_cb.disc_flags |=
          (NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF);
      NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      break;

    case NFA_DM_LP_LISTEN_CMD:
      nfa_dm_disc_new_state(NFA_DM_RFST_LP_LISTEN);
      break;

    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_discovery
**
** Description      Processing discovery events in NFA_DM_RFST_DISCOVERY state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_discovery(tNFA_DM_RF_DISC_SM_EVENT event,
                                     tNFA_DM_RF_DISC_DATA* p_data) {
 
    switch (event) {
    case NFA_DM_RF_DEACTIVATE_CMD:
      /* if deactivate CMD was not sent to NFCC */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
        NFC_Deactivate(p_data->deactivate_type);
      }
      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;

      /* if it's not race condition between deactivate CMD and activate NTF */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
        /* do not notify deactivated to idle in RF discovery state
        ** because it is internal or stopping RF discovery
        */

        /* there was no activation while waiting for deactivation RSP */
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      }
      break;
    case NFA_DM_RF_DISCOVER_NTF:
      nfa_dm_disc_new_state(NFA_DM_RFST_W4_ALL_DISCOVERIES);
      nfa_dm_notify_discovery(p_data);
      break;
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("RF Activated while waiting for deactivation RSP");
        /* it's race condition. DH has to wait for deactivation NTF */
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_NTF;
      } else {
        if (p_data->nfc_discover.activate.intf_param.type ==
            NFC_INTERFACE_EE_DIRECT_RF) {
          nfa_dm_disc_new_state(NFA_DM_RFST_LISTEN_ACTIVE);
        } else if (p_data->nfc_discover.activate.rf_tech_param.mode & 0x80) {
          /* Listen mode */
          nfa_dm_disc_new_state(NFA_DM_RFST_LISTEN_ACTIVE);
        } else {
          /* Poll mode */
          nfa_dm_disc_new_state(NFA_DM_RFST_POLL_ACTIVE);
        }

        if (nfa_dm_disc_notify_activation(&(p_data->nfc_discover)) ==
            NFA_STATUS_FAILED) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "Not matched, restart discovery after receiving "
              "deactivate ntf");

          /* after receiving deactivate event, restart discovery */
          nfa_dm_cb.disc_cb.disc_flags |=
              (NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF);
          NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
        }
      }
      break;

    case NFA_DM_RF_DEACTIVATE_NTF:
      /* if there was race condition between deactivate CMD and activate NTF */
      if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF) {
        /* race condition is resolved */
        nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_NTF;

        if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
          /* do not notify deactivated to idle in RF discovery state
          ** because it is internal or stopping RF discovery
          */

          nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
          nfa_dm_start_rf_discover();
        }
      }
      break;
    case NFA_DM_LP_LISTEN_CMD:
      break;
    case NFA_DM_CORE_INTF_ERROR_NTF:
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_w4_all_discoveries
**
** Description      Processing discovery events in
**                  NFA_DM_RFST_W4_ALL_DISCOVERIES state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_w4_all_discoveries(tNFA_DM_RF_DISC_SM_EVENT event,
                                              tNFA_DM_RF_DISC_DATA* p_data) {
  
    switch (event) {
    case NFA_DM_RF_DEACTIVATE_CMD:
      /* if deactivate CMD was not sent to NFCC */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
        /* only IDLE mode is allowed */
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;
      /* notify exiting from w4 all discoverie state */
      nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_RSP,
                                      &(p_data->nfc_discover));

      nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
      nfa_dm_start_rf_discover();
      break;
    case NFA_DM_RF_DISCOVER_NTF:
      /* if deactivate CMD is already sent then ignore discover NTF */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
        /* Notification Type = NCI_DISCOVER_NTF_LAST or
         * NCI_DISCOVER_NTF_LAST_ABORT */
        if (p_data->nfc_discover.result.more != NCI_DISCOVER_NTF_MORE) {
          nfa_dm_disc_new_state(NFA_DM_RFST_W4_HOST_SELECT);
        }
        nfa_dm_notify_discovery(p_data);
      }
      break;
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      /*
      ** This is only for ISO15693.
      ** FW sends activation NTF when all responses are received from tags
      ** without host selecting.
      */
      nfa_dm_disc_new_state(NFA_DM_RFST_POLL_ACTIVE);

      if (nfa_dm_disc_notify_activation(&(p_data->nfc_discover)) ==
          NFA_STATUS_FAILED) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Not matched, restart discovery after receiving deactivate ntf");

        /* after receiving deactivate event, restart discovery */
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_w4_host_select
**
** Description      Processing discovery events in NFA_DM_RFST_W4_HOST_SELECT
**                  state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_w4_host_select(tNFA_DM_RF_DISC_SM_EVENT event,
                                          tNFA_DM_RF_DISC_DATA* p_data) {
  tNFA_CONN_EVT_DATA conn_evt;
  tNFA_DM_DISC_FLAGS old_sleep_wakeup_flag =
      (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_CHECKING);
  bool sleep_wakeup_event = false;
  bool sleep_wakeup_event_processed = false;
  tNFA_STATUS status;

  switch (event) {
    case NFA_DM_RF_DISCOVER_SELECT_CMD:
      /* if not waiting to deactivate */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
        NFC_DiscoverySelect(p_data->select.rf_disc_id, p_data->select.protocol,
                            p_data->select.rf_interface);
      } else {
        nfa_dm_disc_conn_event_notify(NFA_SELECT_RESULT_EVT, NFA_STATUS_FAILED);
      }
      break;

    case NFA_DM_RF_DISCOVER_SELECT_RSP:
      sleep_wakeup_event = true;
      /* notify application status of selection */
      if (p_data->nfc_discover.status == NFC_STATUS_OK) {
        sleep_wakeup_event_processed = true;
        conn_evt.status = NFA_STATUS_OK;
        /* register callback to get interface error NTF */
        NFC_SetStaticRfCback(nfa_dm_disc_data_cback);
      } else
        conn_evt.status = NFA_STATUS_FAILED;

      if (!old_sleep_wakeup_flag) {
        nfa_dm_disc_conn_event_notify(NFA_SELECT_RESULT_EVT,
                                      p_data->nfc_discover.status);
      }
      break;
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      nfa_dm_disc_new_state(NFA_DM_RFST_POLL_ACTIVE);
      /* always call nfa_dm_disc_notify_activation to update protocol/interface
       * information in NFA control blocks */
      status = nfa_dm_disc_notify_activation(&(p_data->nfc_discover));
      if (old_sleep_wakeup_flag) {
        /* Handle sleep wakeup success: notify RW module of sleep wakeup of tag;
         * if deactivation is pending then deactivate  */
        nfa_dm_disc_end_sleep_wakeup(NFC_STATUS_OK);
      } else if (status == NFA_STATUS_FAILED) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Not matched, restart discovery after receiving deactivate ntf");

        /* after receiving deactivate event, restart discovery */
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;
    case NFA_DM_RF_DEACTIVATE_CMD:
      if (old_sleep_wakeup_flag) {
        nfa_dm_cb.disc_cb.deact_pending = true;
        nfa_dm_cb.disc_cb.pending_deact_type = p_data->deactivate_type;
      }
      /* if deactivate CMD was not sent to NFCC */
      else if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP)) {
        nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_W4_RSP;
        /* only IDLE mode is allowed */
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;
      /* notify exiting from host select state */
      nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_RSP,
                                      &(p_data->nfc_discover));

      nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
      nfa_dm_start_rf_discover();
      break;

    case NFA_DM_CORE_INTF_ERROR_NTF:
      sleep_wakeup_event = true;
      if (!old_sleep_wakeup_flag) {
        /* target activation failed, upper layer may deactivate or select again
         */
        conn_evt.status = NFA_STATUS_FAILED;
        nfa_dm_conn_cback_event_notify(NFA_SELECT_RESULT_EVT, &conn_evt);
      }
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }

  if (old_sleep_wakeup_flag && sleep_wakeup_event &&
      !sleep_wakeup_event_processed) {
    /* performing sleep wakeup and exception conditions happened
     * clear sleep wakeup information and report failure */
    nfa_dm_disc_end_sleep_wakeup(NFC_STATUS_FAILED);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_poll_active
**
** Description      Processing discovery events in NFA_DM_RFST_POLL_ACTIVE state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_poll_active(tNFA_DM_RF_DISC_SM_EVENT event,
                                       tNFA_DM_RF_DISC_DATA* p_data) {
  tNFC_STATUS status;
  tNFA_DM_DISC_FLAGS old_sleep_wakeup_flag =
      (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_CHECKING);
  bool sleep_wakeup_event = false;
  bool sleep_wakeup_event_processed = false;

  switch (event) {
    case NFA_DM_RF_DEACTIVATE_CMD:

      if (nfa_dm_cb.disc_cb.activated_protocol == NCI_PROTOCOL_MIFARE) {
        nfa_dm_cb.disc_cb.deact_pending = true;
        nfa_dm_cb.disc_cb.pending_deact_type = p_data->deactivate_type;
        status = nfa_dm_send_deactivate_cmd(p_data->deactivate_type);
        break;
      }

      if (old_sleep_wakeup_flag) {
        /* sleep wakeup is already enabled when deactivate cmd is requested,
         * keep the information in control block to issue it later */
        nfa_dm_cb.disc_cb.deact_pending = true;
        nfa_dm_cb.disc_cb.pending_deact_type = p_data->deactivate_type;
      } else {
        status = nfa_dm_send_deactivate_cmd(p_data->deactivate_type);
      }

      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;
      /* register callback to get interface error NTF */
      NFC_SetStaticRfCback(nfa_dm_disc_data_cback);

      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
        /* it's race condition. received deactivate NTF before receiving RSP */

        tNFC_DEACTIVATE_DEVT deact = tNFC_DEACTIVATE_DEVT();
        deact.status = NFC_STATUS_OK;
        deact.type = NFC_DEACTIVATE_TYPE_IDLE;
        deact.is_ntf = true;
        tNFC_DISCOVER nfc_discover;
        nfc_discover.deactivate = deact;
        nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                        &nfc_discover);

        /* NFCC is in IDLE state */
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      }
      break;
    case NFA_DM_RF_DEACTIVATE_NTF:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_NTF;

      nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.tle);

      if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP) {
        /* it's race condition. received deactivate NTF before receiving RSP */
        /* notify deactivation after receiving deactivate RSP */
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Rx deactivate NTF while waiting for deactivate RSP");
        break;
      }
      if (p_data->nfc_discover.deactivate.reason !=
          NFC_DEACTIVATE_REASON_DH_REQ_FAILED) {
        /* count for number of times deactivate cmd sent */
        nfa_dm_cb.deactivate_cmd_retry_count = 0;

        sleep_wakeup_event = true;
        nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                        &(p_data->nfc_discover));
      }
      if ((p_data->nfc_discover.deactivate.type == NFC_DEACTIVATE_TYPE_SLEEP) ||
          (p_data->nfc_discover.deactivate.type ==
           NFC_DEACTIVATE_TYPE_SLEEP_AF)) {
        if (p_data->nfc_discover.deactivate.reason !=
            NFC_DEACTIVATE_REASON_DH_REQ_FAILED) {

          nfa_dm_disc_new_state(NFA_DM_RFST_W4_HOST_SELECT);
        }
        if (old_sleep_wakeup_flag) {
          sleep_wakeup_event_processed = true;
          /* process pending deactivate request */
          if (nfa_dm_cb.disc_cb.deact_pending) {
            /* notify RW module that sleep wakeup is finished */
            /* if deactivation is pending then deactivate  */
            nfa_dm_disc_end_sleep_wakeup(NFC_STATUS_OK);

            /* Notify NFA RW sub-systems because NFA_DM_RF_DEACTIVATE_RSP will
             * not call this function */
            nfa_rw_proc_disc_evt(NFA_DM_RF_DISC_DEACTIVATED_EVT, nullptr, true);
          } else {
            /* Successfully went to sleep mode for sleep wakeup */
            /* Now wake up the tag to complete the operation */
            NFC_DiscoverySelect(nfa_dm_cb.disc_cb.activated_rf_disc_id,
                                nfa_dm_cb.disc_cb.activated_protocol,
                                nfa_dm_cb.disc_cb.activated_rf_interface);
          }
        }
        if (p_data->nfc_discover.deactivate.reason ==
            NFC_DEACTIVATE_REASON_DH_REQ_FAILED) {
          /* in case deactivation is not sucessfull, NFCC shall send
             RF_DEACTIVATE_NTF with DH Req failed due to error.
             MW shall send deactivation cmd again for 3 three times. if
             deactivation is not successfull 3 times also,
             then MW shall send deacivate cmd with deactivate type is
             discovery */
          if (nfa_dm_cb.deactivate_cmd_retry_count == 3) {
            if ((!old_sleep_wakeup_flag) ||
                (!nfa_dm_cb.disc_cb.deact_pending)) {
              nfa_dm_send_deactivate_cmd(NFA_DEACTIVATE_TYPE_DISCOVERY);
            }
          } else {
            nfa_dm_cb.deactivate_cmd_retry_count++;
            nfa_dm_send_deactivate_cmd(p_data->nfc_discover.deactivate.type);
          }
        }
      } else if (p_data->nfc_discover.deactivate.type ==
                 NFC_DEACTIVATE_TYPE_IDLE) {
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      } else if (p_data->nfc_discover.deactivate.type ==
                 NFC_DEACTIVATE_TYPE_DISCOVERY) {
        nfa_dm_disc_new_state(NFA_DM_RFST_DISCOVERY);
        /* If deactivation type is discovery, reset the counter and notify
         * upper layer.
         */
        nfa_dm_cb.deactivate_cmd_retry_count = 0;
        DLOG_IF(INFO, nfc_debug_enabled)
            << __func__
            << StringPrintf("NFA_DM_RF_DEACTIVATE_NTF to discovery");
        if (p_data->nfc_discover.deactivate.reason ==
            NFC_DEACTIVATE_REASON_DH_REQ_FAILED) {
          nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                          &(p_data->nfc_discover));
        }
        if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_STOPPING) {
          /* stop discovery */
          NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
        }
      }
      break;

    case NFA_DM_CORE_INTF_ERROR_NTF:
      sleep_wakeup_event = true;
      if ((!old_sleep_wakeup_flag) || (!nfa_dm_cb.disc_cb.deact_pending)) {
        nfa_dm_send_deactivate_cmd(NFA_DEACTIVATE_TYPE_DISCOVERY);
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }

  if (old_sleep_wakeup_flag && sleep_wakeup_event &&
      !sleep_wakeup_event_processed) {
    /* performing sleep wakeup and exception conditions happened
     * clear sleep wakeup information and report failure */
    nfa_dm_disc_end_sleep_wakeup(NFC_STATUS_FAILED);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_listen_active
**
** Description      Processing discovery events in NFA_DM_RFST_LISTEN_ACTIVE
**                  state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_listen_active(tNFA_DM_RF_DISC_SM_EVENT event,
                                         tNFA_DM_RF_DISC_DATA* p_data) {
  tNFC_DEACTIVATE_DEVT deact = tNFC_DEACTIVATE_DEVT();

  switch (event) {
    case NFA_DM_RF_DEACTIVATE_CMD:
      nfa_dm_send_deactivate_cmd(p_data->deactivate_type);
      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
        /* it's race condition. received deactivate NTF before receiving RSP */

        deact.status = NFC_STATUS_OK;
        deact.type = NFC_DEACTIVATE_TYPE_IDLE;
        deact.is_ntf = true;
        tNFC_DISCOVER nfc_discover;
        nfc_discover.deactivate = deact;
        nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                        &nfc_discover);

        /* NFCC is in IDLE state */
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      }
      break;
    case NFA_DM_RF_DEACTIVATE_NTF:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_NTF;

      nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.tle);

      if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_RSP) {
        /* it's race condition. received deactivate NTF before receiving RSP */
        /* notify deactivation after receiving deactivate RSP */
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Rx deactivate NTF while waiting for deactivate RSP");
      } else {
        nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                        &(p_data->nfc_discover));

        if (p_data->nfc_discover.deactivate.type == NFC_DEACTIVATE_TYPE_IDLE) {
          nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
          nfa_dm_start_rf_discover();
        } else if ((p_data->nfc_discover.deactivate.type ==
                    NFC_DEACTIVATE_TYPE_SLEEP) ||
                   (p_data->nfc_discover.deactivate.type ==
                    NFC_DEACTIVATE_TYPE_SLEEP_AF)) {
          nfa_dm_disc_new_state(NFA_DM_RFST_LISTEN_SLEEP);
        } else if (p_data->nfc_discover.deactivate.type ==
                   NFC_DEACTIVATE_TYPE_DISCOVERY) {
          /* Discovery */
          if (nfa_dm_cb.pending_power_state != SCREEN_STATE_INVALID) {
            NFC_SetPowerSubState(nfa_dm_cb.pending_power_state);
            nfa_dm_cb.pending_power_state = SCREEN_STATE_INVALID;
          }
          nfa_dm_disc_new_state(NFA_DM_RFST_DISCOVERY);
          if (nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_STOPPING) {
            /* stop discovery */
            NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
          }
        }
      }
      break;

    case NFA_DM_CORE_INTF_ERROR_NTF:
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_listen_sleep
**
** Description      Processing discovery events in NFA_DM_RFST_LISTEN_SLEEP
**                  state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_listen_sleep(tNFA_DM_RF_DISC_SM_EVENT event,
                                        tNFA_DM_RF_DISC_DATA* p_data) {
  
    switch (event) {
    case NFA_DM_RF_DEACTIVATE_CMD:
      nfa_dm_send_deactivate_cmd(p_data->deactivate_type);

      /* if deactivate type is not discovery then NFCC will not sent
       * deactivation NTF */
      if (p_data->deactivate_type != NFA_DEACTIVATE_TYPE_DISCOVERY) {
        nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_NTF;
        nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.tle);
      }
      break;
    case NFA_DM_RF_DEACTIVATE_RSP:
      nfa_dm_cb.disc_cb.disc_flags &= ~NFA_DM_DISC_FLAGS_W4_RSP;
      /* if deactivate type in CMD was IDLE */
      if (!(nfa_dm_cb.disc_cb.disc_flags & NFA_DM_DISC_FLAGS_W4_NTF)) {
        nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_RSP,
                                        &(p_data->nfc_discover));

        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      }
      break;
    case NFA_DM_RF_DEACTIVATE_NTF:
      /* clear both W4_RSP and W4_NTF because of race condition between
       * deactivat CMD and link loss */
      nfa_dm_cb.disc_cb.disc_flags &=
          ~(NFA_DM_DISC_FLAGS_W4_RSP | NFA_DM_DISC_FLAGS_W4_NTF);
      nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.tle);

      /* there is no active protocol in this state, so broadcast to all by using
       * NFA_DM_RF_DEACTIVATE_RSP */
      nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_RSP,
                                      &(p_data->nfc_discover));

      if (p_data->nfc_discover.deactivate.type == NFC_DEACTIVATE_TYPE_IDLE) {
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      } else if (p_data->nfc_discover.deactivate.type ==
                 NFA_DEACTIVATE_TYPE_DISCOVERY) {
        nfa_dm_disc_new_state(NFA_DM_RFST_DISCOVERY);
      } else {
        LOG(ERROR) << StringPrintf("Unexpected deactivation type");
        nfa_dm_disc_new_state(NFA_DM_RFST_IDLE);
        nfa_dm_start_rf_discover();
      }
      break;
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      nfa_dm_disc_new_state(NFA_DM_RFST_LISTEN_ACTIVE);
      if (nfa_dm_disc_notify_activation(&(p_data->nfc_discover)) ==
          NFA_STATUS_FAILED) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Not matched, restart discovery after receiving deactivate ntf");

        /* after receiving deactivate event, restart discovery */
        NFC_Deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      }
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_lp_listen
**
** Description      Processing discovery events in NFA_DM_RFST_LP_LISTEN state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_lp_listen(tNFA_DM_RF_DISC_SM_EVENT event,
                                     tNFA_DM_RF_DISC_DATA* p_data) {
  switch (event) {
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      nfa_dm_disc_new_state(NFA_DM_RFST_LP_ACTIVE);
      if (nfa_dm_disc_notify_activation(&(p_data->nfc_discover)) ==
          NFA_STATUS_FAILED) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Not matched, unexpected activation");
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_lp_active
**
** Description      Processing discovery events in NFA_DM_RFST_LP_ACTIVE state
**
** Returns          void
**
*******************************************************************************/
static void nfa_dm_disc_sm_lp_active(tNFA_DM_RF_DISC_SM_EVENT event,
                                     tNFA_DM_RF_DISC_DATA* p_data) {
  switch (event) {
    case NFA_DM_RF_DEACTIVATE_NTF:
      nfa_dm_disc_new_state(NFA_DM_RFST_LP_LISTEN);
      nfa_dm_disc_notify_deactivation(NFA_DM_RF_DEACTIVATE_NTF,
                                      &(p_data->nfc_discover));
      break;
    default:
      LOG(ERROR) << StringPrintf("Unexpected discovery event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_sm_execute
**
** Description      Processing discovery related events
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_disc_sm_execute(tNFA_DM_RF_DISC_SM_EVENT event,
                            tNFA_DM_RF_DISC_DATA* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "state: %s (%d), event: %s(%d) disc_flags: "
      "0x%x",
      nfa_dm_disc_state_2_str(nfa_dm_cb.disc_cb.disc_state).c_str(),
      nfa_dm_cb.disc_cb.disc_state, nfa_dm_disc_event_2_str(event).c_str(),
      event, nfa_dm_cb.disc_cb.disc_flags);

  switch (nfa_dm_cb.disc_cb.disc_state) {
    /*  RF Discovery State - Idle */
    case NFA_DM_RFST_IDLE:
      nfa_dm_disc_sm_idle(event, p_data);
      break;

    /* RF Discovery State - Discovery */
    case NFA_DM_RFST_DISCOVERY:
      nfa_dm_disc_sm_discovery(event, p_data);
      break;

    /*RF Discovery State - Wait for all discoveries */
    case NFA_DM_RFST_W4_ALL_DISCOVERIES:
      nfa_dm_disc_sm_w4_all_discoveries(event, p_data);
      break;

    /* RF Discovery State - Wait for host selection */
    case NFA_DM_RFST_W4_HOST_SELECT:
      nfa_dm_disc_sm_w4_host_select(event, p_data);
      break;

    /* RF Discovery State - Poll mode activated */
    case NFA_DM_RFST_POLL_ACTIVE:
      nfa_dm_disc_sm_poll_active(event, p_data);
      break;

    /* RF Discovery State - listen mode activated */
    case NFA_DM_RFST_LISTEN_ACTIVE:
      nfa_dm_disc_sm_listen_active(event, p_data);
      break;

    /* RF Discovery State - listen mode sleep */
    case NFA_DM_RFST_LISTEN_SLEEP:
      nfa_dm_disc_sm_listen_sleep(event, p_data);
      break;

    /* Listening in Low Power mode    */
    case NFA_DM_RFST_LP_LISTEN:
      nfa_dm_disc_sm_lp_listen(event, p_data);
      break;

    /* Activated in Low Power mode    */
    case NFA_DM_RFST_LP_ACTIVE:
      nfa_dm_disc_sm_lp_active(event, p_data);
      break;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "new state: %s (%d), disc_flags: 0x%x",
      nfa_dm_disc_state_2_str(nfa_dm_cb.disc_cb.disc_state).c_str(),
      nfa_dm_cb.disc_cb.disc_state, nfa_dm_cb.disc_cb.disc_flags);
}

/*******************************************************************************
**
** Function         nfa_dm_add_rf_discover
**
** Description      Add discovery configuration and callback function
**
** Returns          valid handle if success
**
*******************************************************************************/
tNFA_HANDLE nfa_dm_add_rf_discover(tNFA_DM_DISC_TECH_PROTO_MASK disc_mask,
                                   tNFA_DM_DISC_HOST_ID host_id,
                                   tNFA_DISCOVER_CBACK* p_disc_cback) {
  uint8_t xx;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("disc_mask=0x%x", disc_mask);

  for (xx = 0; xx < NFA_DM_DISC_NUM_ENTRIES; xx++) {
    if (!nfa_dm_cb.disc_cb.entry[xx].in_use) {
      nfa_dm_cb.disc_cb.entry[xx].in_use = true;
      nfa_dm_cb.disc_cb.entry[xx].requested_disc_mask = disc_mask;
      nfa_dm_cb.disc_cb.entry[xx].host_id = host_id;
      nfa_dm_cb.disc_cb.entry[xx].p_disc_cback = p_disc_cback;
      nfa_dm_cb.disc_cb.entry[xx].disc_flags = NFA_DM_DISC_FLAGS_NOTIFY;
      return xx;
    }
  }

  return NFA_HANDLE_INVALID;
}

/*******************************************************************************
**
** Function         nfa_dm_start_excl_discovery
**
** Description      Start exclusive RF discovery
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_start_excl_discovery(tNFA_TECHNOLOGY_MASK poll_tech_mask,
                                 tNFA_LISTEN_CFG* p_listen_cfg,
                                 tNFA_DISCOVER_CBACK* p_disc_cback) {
  tNFA_DM_DISC_TECH_PROTO_MASK poll_disc_mask = 0;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_A) {
    poll_disc_mask |= NFA_DM_DISC_MASK_PA_T1T;
    poll_disc_mask |= NFA_DM_DISC_MASK_PA_T2T;
    poll_disc_mask |= NFA_DM_DISC_MASK_PA_ISO_DEP;
    poll_disc_mask |= NFA_DM_DISC_MASK_PA_NFC_DEP;
    poll_disc_mask |= NFA_DM_DISC_MASK_P_LEGACY;
  }
  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    if (poll_tech_mask & NFA_TECHNOLOGY_MASK_ACTIVE) {
      poll_disc_mask |= NFA_DM_DISC_MASK_PACM_NFC_DEP;
    }
  } else {
    if (poll_tech_mask & NFA_TECHNOLOGY_MASK_A_ACTIVE) {
      poll_disc_mask |= NFA_DM_DISC_MASK_PAA_NFC_DEP;
    }
    if (poll_tech_mask & NFA_TECHNOLOGY_MASK_F_ACTIVE) {
      poll_disc_mask |= NFA_DM_DISC_MASK_PFA_NFC_DEP;
    }
  }

  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_B) {
    poll_disc_mask |= NFA_DM_DISC_MASK_PB_ISO_DEP;
  }
  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_F) {
    poll_disc_mask |= NFA_DM_DISC_MASK_PF_T3T;
    poll_disc_mask |= NFA_DM_DISC_MASK_PF_NFC_DEP;
  }
  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_V) {
    poll_disc_mask |= NFA_DM_DISC_MASK_P_T5T;
  }
  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_B_PRIME) {
    poll_disc_mask |= NFA_DM_DISC_MASK_P_B_PRIME;
  }
  if (poll_tech_mask & NFA_TECHNOLOGY_MASK_KOVIO) {
    poll_disc_mask |= NFA_DM_DISC_MASK_P_KOVIO;
  }

  nfa_dm_cb.disc_cb.excl_disc_entry.in_use = true;
  nfa_dm_cb.disc_cb.excl_disc_entry.requested_disc_mask = poll_disc_mask;
  nfa_dm_cb.disc_cb.excl_disc_entry.host_id = NFA_DM_DISC_HOST_ID_DH;
  nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback = p_disc_cback;
  nfa_dm_cb.disc_cb.excl_disc_entry.disc_flags = NFA_DM_DISC_FLAGS_NOTIFY;

  memcpy(&nfa_dm_cb.disc_cb.excl_listen_config, p_listen_cfg,
         sizeof(tNFA_LISTEN_CFG));

  nfa_dm_disc_sm_execute(NFA_DM_RF_DISCOVER_CMD, nullptr);
}

/*******************************************************************************
**
** Function         nfa_dm_stop_excl_discovery
**
** Description      Stop exclusive RF discovery
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_stop_excl_discovery(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  nfa_dm_cb.disc_cb.excl_disc_entry.in_use = false;
  nfa_dm_cb.disc_cb.excl_disc_entry.p_disc_cback = nullptr;
}

/*******************************************************************************
**
** Function         nfa_dm_delete_rf_discover
**
** Description      Remove discovery configuration and callback function
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_delete_rf_discover(tNFA_HANDLE handle) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("handle=0x%x", handle);

  if (handle < NFA_DM_DISC_NUM_ENTRIES) {
    nfa_dm_cb.disc_cb.entry[handle].in_use = false;
  } else {
    LOG(ERROR) << StringPrintf("Invalid discovery handle");
  }
}

/*******************************************************************************
**
** Function         nfa_dm_rf_discover_select
**
** Description      Select target, protocol and RF interface
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_rf_discover_select(uint8_t rf_disc_id, tNFA_NFC_PROTOCOL protocol,
                               tNFA_INTF_TYPE rf_interface) {
  tNFA_DM_DISC_SELECT_PARAMS select_params;
  tNFA_CONN_EVT_DATA conn_evt;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rf_disc_id:0x%X, protocol:0x%X, rf_interface:0x%X",
                      rf_disc_id, protocol, rf_interface);

  if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_W4_HOST_SELECT) {
    /* state is OK: notify the status when the response is received from NFCC */
    select_params.rf_disc_id = rf_disc_id;
    select_params.protocol = protocol;
    select_params.rf_interface = rf_interface;

    nfa_dm_cb.disc_cb.disc_flags |= NFA_DM_DISC_FLAGS_NOTIFY;
    tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
    nfa_dm_rf_disc_data.select = select_params;
    nfa_dm_disc_sm_execute(NFA_DM_RF_DISCOVER_SELECT_CMD, &nfa_dm_rf_disc_data);
  } else {
    /* Wrong state: notify failed status right away */
    conn_evt.status = NFA_STATUS_FAILED;
    nfa_dm_conn_cback_event_notify(NFA_SELECT_RESULT_EVT, &conn_evt);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_rf_deactivate
**
** Description      Deactivate NFC link
**
** Returns          NFA_STATUS_OK if success
**
*******************************************************************************/
tNFA_STATUS nfa_dm_rf_deactivate(tNFA_DEACTIVATE_TYPE deactivate_type) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("deactivate_type:0x%X", deactivate_type);

  if (deactivate_type == NFA_DEACTIVATE_TYPE_SLEEP) {
    if (nfa_dm_cb.disc_cb.activated_protocol == NFA_PROTOCOL_NFC_DEP)
      deactivate_type = NFC_DEACTIVATE_TYPE_SLEEP_AF;
    else
      deactivate_type = NFC_DEACTIVATE_TYPE_SLEEP;
  }

  if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_IDLE) {
    return NFA_STATUS_FAILED;
  } else if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_DISCOVERY) {
    if (deactivate_type == NFA_DEACTIVATE_TYPE_DISCOVERY) {
      if (nfa_dm_cb.disc_cb.kovio_tle.in_use) {
        nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.kovio_tle);
        nfa_dm_disc_kovio_timeout_cback(&nfa_dm_cb.disc_cb.kovio_tle);
        return NFA_STATUS_OK;
      } else {
        /* it could be race condition. */
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("already in discovery state");
        return NFA_STATUS_FAILED;
      }
    } else if (deactivate_type == NFA_DEACTIVATE_TYPE_IDLE) {
      if (nfa_dm_cb.disc_cb.kovio_tle.in_use) {
        nfa_sys_stop_timer(&nfa_dm_cb.disc_cb.kovio_tle);
        nfa_dm_disc_kovio_timeout_cback(&nfa_dm_cb.disc_cb.kovio_tle);
      }
      tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
      nfa_dm_rf_disc_data.deactivate_type = deactivate_type;
      nfa_dm_disc_sm_execute(NFA_DM_RF_DEACTIVATE_CMD, &nfa_dm_rf_disc_data);
      return NFA_STATUS_OK;
    } else {
      return NFA_STATUS_FAILED;
    }
  } else {
    tNFA_DM_RF_DISC_DATA nfa_dm_rf_disc_data;
    nfa_dm_rf_disc_data.deactivate_type = deactivate_type;
    nfa_dm_disc_sm_execute(NFA_DM_RF_DEACTIVATE_CMD, &nfa_dm_rf_disc_data);
    return NFA_STATUS_OK;
  }
}

/*******************************************************************************
**
** Function         nfa_dm_disc_state_2_str
**
** Description      convert nfc discovery state to string
**
*******************************************************************************/
static std::string nfa_dm_disc_state_2_str(uint8_t state) {
  switch (state) {
    case NFA_DM_RFST_IDLE:
      return "IDLE";

    case NFA_DM_RFST_DISCOVERY:
      return "DISCOVERY";

    case NFA_DM_RFST_W4_ALL_DISCOVERIES:
      return "W4_ALL_DISCOVERIES";

    case NFA_DM_RFST_W4_HOST_SELECT:
      return "W4_HOST_SELECT";

    case NFA_DM_RFST_POLL_ACTIVE:
      return "POLL_ACTIVE";

    case NFA_DM_RFST_LISTEN_ACTIVE:
      return "LISTEN_ACTIVE";

    case NFA_DM_RFST_LISTEN_SLEEP:
      return "LISTEN_SLEEP";

    case NFA_DM_RFST_LP_LISTEN:
      return "LP_LISTEN";

    case NFA_DM_RFST_LP_ACTIVE:
      return "LP_ACTIVE";
  }
  return "Unknown";
}

/*******************************************************************************
**
** Function         nfa_dm_disc_event_2_str
**
** Description      convert nfc discovery RSP/NTF to string
**
*******************************************************************************/
static std::string nfa_dm_disc_event_2_str(uint8_t event) {
  switch (event) {
    case NFA_DM_RF_DISCOVER_CMD:
      return "DISCOVER_CMD";
    case NFA_DM_RF_DISCOVER_RSP:
      return "DISCOVER_RSP";
    case NFA_DM_RF_DISCOVER_NTF:
      return "DISCOVER_NTF";
    case NFA_DM_RF_DISCOVER_SELECT_CMD:
      return "SELECT_CMD";
    case NFA_DM_RF_DISCOVER_SELECT_RSP:
      return "SELECT_RSP";
    case NFA_DM_RF_INTF_ACTIVATED_NTF:
      return "ACTIVATED_NTF";
    case NFA_DM_RF_DEACTIVATE_CMD:
      return "DEACTIVATE_CMD";
    case NFA_DM_RF_DEACTIVATE_RSP:
      return "DEACTIVATE_RSP";
    case NFA_DM_RF_DEACTIVATE_NTF:
      return "DEACTIVATE_NTF";
    case NFA_DM_LP_LISTEN_CMD:
      return "NFA_DM_LP_LISTEN_CMD";
    case NFA_DM_CORE_INTF_ERROR_NTF:
      return "INTF_ERROR_NTF";
    default:
      return "Unknown";
  }
}

/*******************************************************************************
**
** Function         P2P_Prio_Logic
**
** Description      Implements algorithm for NFC-DEP protocol priority over
**                  ISO-DEP protocol.
**
** Returns          True if success
**
*******************************************************************************/
bool nfa_dm_p2p_prio_logic(uint8_t event, uint8_t* p, uint8_t event_type) {
  if (!nfa_poll_bail_out_mode) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("p2p priority is running under bail out mode ONLY.");
    return true;
  }

  if ((nfa_dm_cb.flags & NFA_DM_FLAGS_P2P_PAUSED) &&
      (nfa_dm_cb.flags & NFA_DM_FLAGS_LISTEN_DISABLED)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "returning from nfa_dm_p2p_prio_logic  Disable p2p_prio_logic");
    return true;
  }
  if (appl_dta_mode_flag == 0x01) {
    /*Disable the P2P Prio Logic when DTA is running*/
    return TRUE;
  }
  if (event == NCI_MSG_RF_DISCOVER &&
      p2p_prio_logic_data.timer_expired == true &&
      event_type == NFA_DM_P2P_PRIO_RSP) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_dm_p2p_prio_logic starting a timer for next rf intf activated "
        "ntf");
    nfc_start_quick_timer(&p2p_prio_logic_data.timer_list,
                          NFC_TTYPE_P2P_PRIO_LOGIC_CLEANUP,
                          ((uint32_t)nfa_dm_act_get_rf_disc_duration() *
                           QUICK_TIMER_TICKS_PER_SEC) /
                              1000);
    return true;
  }

  if (event == NCI_MSG_RF_INTF_ACTIVATED &&
      p2p_prio_logic_data.timer_expired == true) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_dm_p2p_prio_logic stopping a timer for next rf intf activated "
        "ntf");
    nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
  }

  if (nfa_dm_cb.disc_cb.disc_state == NFA_DM_RFST_DISCOVERY) {
    uint8_t rf_disc_id = 0xFF;
    uint8_t type = 0xFF;
    uint8_t protocol = 0xFF;
    uint8_t tech_mode = 0xFF;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("P2P_Prio_Logic");

    if (event == NCI_MSG_RF_INTF_ACTIVATED) {
      rf_disc_id = *p++;
      type = *p++;
      protocol = *p++;
      tech_mode = *p++;
    }
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("nfa_dm_p2p_prio_logic event_type = 0x%x", event_type);

    if (event == NCI_MSG_RF_INTF_ACTIVATED && tech_mode >= 0x80) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_dm_p2p_prio_logic listen mode activated reset all the "
          "nfa_dm_p2p_prio_logic variables ");
      if (p2p_prio_logic_data.timer_list.in_use) {
        nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
      }
      nfa_dm_p2p_prio_logic_cleanup();
    }

    if ((tech_mode < 0x80) && event == NCI_MSG_RF_INTF_ACTIVATED &&
        protocol == NCI_PROTOCOL_ISO_DEP &&
        p2p_prio_logic_data.isodep_detected == false) {
      nfa_dm_p2p_prio_logic_cleanup();
      p2p_prio_logic_data.isodep_detected = true;
      p2p_prio_logic_data.first_tech_mode = tech_mode;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "ISO-DEP Detected First Time  Resume the Polling Loop");
      nci_snd_deactivate_cmd(NFA_DEACTIVATE_TYPE_DISCOVERY);
      return false;
    }

    else if (event == NCI_MSG_RF_INTF_ACTIVATED &&
             protocol == NCI_PROTOCOL_ISO_DEP &&
             p2p_prio_logic_data.isodep_detected == true &&
             p2p_prio_logic_data.first_tech_mode != tech_mode) {
      p2p_prio_logic_data.isodep_detected = true;
      p2p_prio_logic_data.timer_expired = false;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "ISO-DEP Detected Second Time Other Techmode  Resume the Polling "
          "Loop");
      nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
      nci_snd_deactivate_cmd(NFA_DEACTIVATE_TYPE_DISCOVERY);
      return false;
    }

    else if (event == NCI_MSG_RF_INTF_ACTIVATED &&
             protocol == NCI_PROTOCOL_ISO_DEP &&
             p2p_prio_logic_data.isodep_detected == true &&
             p2p_prio_logic_data.timer_expired == true) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "ISO-DEP Detected TimerExpired, Final Notifying the Event");
      nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
      nfa_dm_p2p_prio_logic_cleanup();
    }

    else if (event == NCI_MSG_RF_INTF_ACTIVATED &&
             protocol == NCI_PROTOCOL_ISO_DEP &&
             p2p_prio_logic_data.isodep_detected == true &&
             p2p_prio_logic_data.first_tech_mode == tech_mode) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "ISO-DEP Detected Same Techmode, Final Notifying the Event");
      nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("P2P_Stop_Timer");
      nfa_dm_p2p_prio_logic_cleanup();
    }

    else if (event == NCI_MSG_RF_INTF_ACTIVATED &&
             protocol != NCI_PROTOCOL_ISO_DEP &&
             p2p_prio_logic_data.isodep_detected == true) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "ISO-DEP Not Detected  Giving Priority for other Technology");
      nfc_stop_quick_timer(&p2p_prio_logic_data.timer_list);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("P2P_Stop_Timer");
      nfa_dm_p2p_prio_logic_cleanup();
    }

    else if (event == NCI_MSG_RF_DEACTIVATE &&
             p2p_prio_logic_data.isodep_detected == true &&
             p2p_prio_logic_data.timer_expired == false &&
             event_type == NFA_DM_P2P_PRIO_RSP) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFA_DM_RF_DEACTIVATE_RSP");
      return false;
    }

    else if (event == NCI_MSG_RF_DEACTIVATE &&
             p2p_prio_logic_data.isodep_detected == true &&
             p2p_prio_logic_data.timer_expired == false &&
             event_type == NFA_DM_P2P_PRIO_NTF) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFA_DM_RF_DEACTIVATE_NTF");

      nfc_start_quick_timer(&p2p_prio_logic_data.timer_list,
                            NFC_TTYPE_P2P_PRIO_RESPONSE,
                            ((uint32_t)160 * QUICK_TIMER_TICKS_PER_SEC) / 1000);

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("P2P_Start_Timer");

      return false;
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("returning TRUE");
  return true;
}

/*******************************************************************************
**
** Function         p2p_prio_logic_timeout
**
** Description      Callback function for p2p timer
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_p2p_timer_event() {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("P2P_Timer_timeout NFC-DEP Not Discovered!!");

  p2p_prio_logic_data.timer_expired = true;

  if (p2p_prio_logic_data.isodep_detected == true) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("Deactivate and Restart RF discovery");
    nci_snd_deactivate_cmd(NFC_DEACTIVATE_TYPE_IDLE);
  }
}

/*******************************************************************************
**
** Function         nfa_dm_p2p_prio_logic_cleanup
**
** Description      Callback function for p2p prio logic cleanup timer
**
** Returns          void
**
*******************************************************************************/
void nfa_dm_p2p_prio_logic_cleanup() {
  memset(&p2p_prio_logic_data, 0x00, sizeof(nfa_dm_p2p_prio_logic_t));
}

void NFA_SetReconnectState (BOOLEAN flag)
{
    reconnect_in_progress = flag;
    //NFA_TRACE_DEBUG1("NFA_SetReconnectState = 0x%x", reconnect_in_progress);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("NFA_SetReconnectState = 0x%x",reconnect_in_progress);
}

