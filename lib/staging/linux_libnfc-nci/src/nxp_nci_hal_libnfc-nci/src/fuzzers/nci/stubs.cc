#include "fuzz.h"

bool nfa_dm_p2p_prio_logic(unsigned char, unsigned char*, unsigned char) {
  return true;
}

void rw_init() {}
void ce_init() {}
void llcp_init() {}

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

void rw_t4t_handle_isodep_nak_rsp(uint8_t, bool) {}

void rw_t3t_handle_nci_poll_ntf(uint8_t nci_status, uint8_t num_responses,
                                uint8_t sensf_res_buf_size,
                                uint8_t* p_sensf_res_buf) {
  FUZZLOG("nci_status=%02X, num_responses=%d, p_sensf_res_buf=%s", nci_status,
          num_responses,
          BytesToHex(p_sensf_res_buf, sensf_res_buf_size).c_str());
}

void llcp_process_timeout(TIMER_LIST_ENT*) { abort(); }
void rw_t1t_process_timeout(TIMER_LIST_ENT*) { abort(); }
void rw_t2t_process_timeout() { abort(); }
void rw_t3t_process_timeout(TIMER_LIST_ENT*) { abort(); }
void rw_t4t_process_timeout(TIMER_LIST_ENT*) { abort(); }
void rw_i93_process_timeout(TIMER_LIST_ENT*) { abort(); }
void nfa_dm_p2p_timer_event() { abort(); }
void nfa_dm_p2p_prio_logic_cleanup() { abort(); }
void rw_mfc_process_timeout(TIMER_LIST_ENT*) { abort(); }
void ce_t4t_process_timeout(TIMER_LIST_ENT*) { abort(); }
void llcp_cleanup() {}
void nfa_sys_event(NFC_HDR*) { abort(); }
void nfa_sys_timer_update() { abort(); }
