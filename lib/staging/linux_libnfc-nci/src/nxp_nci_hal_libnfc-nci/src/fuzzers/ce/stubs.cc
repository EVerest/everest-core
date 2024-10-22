#include "fuzz_cmn.h"
#include "rw_int.h"

// These are the functions implemented elsewhere in the NFC code. Our fuzzing
// doesn't need them. To avoid pulling into more source code we simply stub
// them out.

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

tRW_CB rw_cb = {};

tNFA_PROPRIETARY_CFG* p_nfa_proprietary_cfg =
    (tNFA_PROPRIETARY_CFG*)&nfa_proprietary_cfg;

void nfc_start_quick_timer(TIMER_LIST_ENT*, uint16_t, uint32_t) {}
void nfc_stop_timer(TIMER_LIST_ENT*) {}
void nfc_stop_quick_timer(TIMER_LIST_ENT*) {}
uint8_t NFC_GetNCIVersion() { return NCI_VERSION_2_0; }

tNFC_STATUS NFC_SendData(uint8_t conn_id, NFC_HDR* p_data) {
  uint8_t* p = (uint8_t*)(p_data + 1) + p_data->offset;
  uint8_t len = (uint8_t)p_data->len;

  FUZZLOG("conn_id=%d, data=%s", conn_id, BytesToHex(p, len).c_str());
  GKI_freebuf(p_data);
  return NFC_STATUS_OK;
}

uint8_t nci_snd_t3t_polling(uint16_t system_code, uint8_t rc, uint8_t tsn) {
  FUZZLOG("sc=%04X, rc=%02X, tsn=%02X", system_code, rc, tsn);
  return NFC_STATUS_OK;
}

tNFC_CONN_CBACK* rf_cback = nullptr;
void NFC_SetStaticRfCback(tNFC_CONN_CBACK* p_cback) { rf_cback = p_cback; }

tNFC_STATUS NFC_ISODEPNakPresCheck() { return NFC_STATUS_OK; }

std::string NFC_GetStatusName(tNFC_STATUS status) {
  switch (status) {
    case NFC_STATUS_OK:
      return "OK";
    case NFC_STATUS_REJECTED:
      return "REJECTED";
    case NFC_STATUS_MSG_CORRUPTED:
      return "CORRUPTED";
    case NFC_STATUS_BUFFER_FULL:
      return "BUFFER_FULL";
    case NFC_STATUS_FAILED:
      return "FAILED";
    case NFC_STATUS_NOT_INITIALIZED:
      return "NOT_INITIALIZED";
    case NFC_STATUS_SYNTAX_ERROR:
      return "SYNTAX_ERROR";
    case NFC_STATUS_SEMANTIC_ERROR:
      return "SEMANTIC_ERROR";
    case NFC_STATUS_UNKNOWN_GID:
      return "UNKNOWN_GID";
    case NFC_STATUS_UNKNOWN_OID:
      return "UNKNOWN_OID";
    case NFC_STATUS_INVALID_PARAM:
      return "INVALID_PARAM";
    case NFC_STATUS_MSG_SIZE_TOO_BIG:
      return "MSG_SIZE_TOO_BIG";
    case NFC_STATUS_ALREADY_STARTED:
      return "ALREADY_STARTED";
    case NFC_STATUS_ACTIVATION_FAILED:
      return "ACTIVATION_FAILED";
    case NFC_STATUS_TEAR_DOWN:
      return "TEAR_DOWN";
    case NFC_STATUS_RF_TRANSMISSION_ERR:
      return "RF_TRANSMISSION_ERR";
    case NFC_STATUS_RF_PROTOCOL_ERR:
      return "RF_PROTOCOL_ERR";
    case NFC_STATUS_TIMEOUT:
      return "TIMEOUT";
    case NFC_STATUS_EE_INTF_ACTIVE_FAIL:
      return "EE_INTF_ACTIVE_FAIL";
    case NFC_STATUS_EE_TRANSMISSION_ERR:
      return "EE_TRANSMISSION_ERR";
    case NFC_STATUS_EE_PROTOCOL_ERR:
      return "EE_PROTOCOL_ERR";
    case NFC_STATUS_EE_TIMEOUT:
      return "EE_TIMEOUT";
    case NFC_STATUS_CMD_STARTED:
      return "CMD_STARTED";
    case NFC_STATUS_HW_TIMEOUT:
      return "HW_TIMEOUT";
    case NFC_STATUS_CONTINUE:
      return "CONTINUE";
    case NFC_STATUS_REFUSED:
      return "REFUSED";
    case NFC_STATUS_BAD_RESP:
      return "BAD_RESP";
    case NFC_STATUS_CMD_NOT_CMPLTD:
      return "CMD_NOT_CMPLTD";
    case NFC_STATUS_NO_BUFFERS:
      return "NO_BUFFERS";
    case NFC_STATUS_WRONG_PROTOCOL:
      return "WRONG_PROTOCOL";
    case NFC_STATUS_BUSY:
      return "BUSY";
    case NFC_STATUS_LINK_LOSS:
      return "LINK_LOSS";
    case NFC_STATUS_BAD_LENGTH:
      return "BAD_LENGTH";
    case NFC_STATUS_BAD_HANDLE:
      return "BAD_HANDLE";
    case NFC_STATUS_CONGESTED:
      return "CONGESTED";
    default:
      return "UNKNOWN";
  }
}