#include "fuzz_cmn.h"

// These are the functions implemented elsewhere in the NFC code. Our fuzzing
// doesn't need them. To avoid pulling into more source code we simply stub
// them out.

uint8_t NFC_GetNCIVersion() { return NCI_VERSION_2_0; }

tNFC_STATUS NFC_DiscoveryMap(uint8_t, tNFC_DISCOVER_MAPS*,
                             tNFC_DISCOVER_CBACK*) {
  return NFC_STATUS_OK;
}

void nfa_sys_cback_notify_nfcc_power_mode_proc_complete(uint8_t) {}

void nfa_sys_cback_reg_enable_complete(void*) {}

tNFC_STATUS NFC_Enable(tNFC_RESPONSE_CBACK*) { return NFC_STATUS_OK; }

void nfa_sys_deregister(uint8_t) {}
void nfa_rw_stop_presence_check_timer() {}
void nfa_p2p_deactivate_llcp() {}
void nfa_rw_proc_disc_evt(int, tNFC_DISCOVER*, bool) {}
