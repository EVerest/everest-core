#include "fuzz_cmn.h"
#include "nfa_api.h"
#include "nfa_dm_int.h"

#define MODULE_NAME "nfc_ndef_fuzzer"

const char fuzzer_name[] = MODULE_NAME;

tNFA_DM_CB nfa_dm_cb = {};
bool ndef_handler_registered = false;

static void ndef_cback(tNFA_NDEF_EVT event, tNFA_NDEF_EVT_DATA* p_data) {
  if (event == NFA_NDEF_REGISTER_EVT) {
    ndef_handler_registered = (p_data->ndef_reg.status == NFC_STATUS_OK);
  } else if (event == NFA_NDEF_DATA_EVT) {
    FUZZLOG("ndef_data, start=%p, len=%d", p_data->ndef_data.p_data,
            p_data->ndef_data.len);

    uint16_t cs = 0;
    for (uint8_t* p = p_data->ndef_data.p_data;
         p < p_data->ndef_data.p_data + p_data->ndef_data.len; p++) {
      cs += *p;
    }

    FUZZLOG("ndef_data, checksum=%04X", cs);
  }
}

tNFA_DM_MSG reg_hdler = {.reg_ndef_hdlr = {
                             .tnf = NFA_TNF_DEFAULT,
                             .p_ndef_cback = ndef_cback,
                         }};

static bool init() {
  if (!ndef_handler_registered) {
    nfa_dm_ndef_reg_hdlr(&reg_hdler);
  }
  return ndef_handler_registered;
}

void Fuzz_FixPackets(std::vector<bytes_t>& /*Packets*/, uint /*Seed*/) {}

void Fuzz_RunPackets(const std::vector<bytes_t>& Packets) {
  if (!init()) {
    return;
  }

  for (auto it = Packets.cbegin(); it != Packets.cend(); ++it) {
    nfa_dm_ndef_handle_message(NFA_STATUS_OK, const_cast<uint8_t*>(it->data()),
                               (uint32_t)it->size());
  }
}
