#include "fuzz.h"

#define MODULE_NAME "nfc_llcp_fuzzer"

const char fuzzer_name[] = MODULE_NAME;

enum {
  SUB_TYPE_DUMMY,

  SUB_TYPE_MAX
};

static void llcp_cback(uint8_t event, uint8_t reason) {
  FUZZLOG(MODULE_NAME ": : event=0x%02x, reason=0x%02X", event, reason);
}

static bool Init(Fuzz_Context& /*ctx*/) {
  uint8_t LLCP_GEN_BYTES[] = {
      LLCP_MAGIC_NUMBER_BYTE0, LLCP_MAGIC_NUMBER_BYTE1,
      LLCP_MAGIC_NUMBER_BYTE2, LLCP_VERSION_TYPE,
      LLCP_VERSION_LEN,        (LLCP_VERSION_MAJOR << 4) | LLCP_VERSION_MINOR,
  };

  tLLCP_ACTIVATE_CONFIG config = {
      .is_initiator = false,
      .max_payload_size = LLCP_NCI_MAX_PAYL_SIZE,
      .p_gen_bytes = LLCP_GEN_BYTES,
      .gen_bytes_len = sizeof(LLCP_GEN_BYTES),
  };

  GKI_init();
  llcp_init();
  if (NFC_STATUS_OK != LLCP_ActivateLink(config, llcp_cback)) {
    FUZZLOG(MODULE_NAME ": LLCP_ActivateLink failed");
    return false;
  }

  return true;
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  return true;
}

static void Fuzz_Deinit(Fuzz_Context& /*ctx*/) {
  LLCP_DeactivateLink();

  // Explicitly calling llcp_link_deactivate with LLCP_LINK_TIMEOUT to avoid
  // memory leak.
  llcp_link_deactivate(LLCP_LINK_TIMEOUT);

  llcp_cleanup();
  GKI_shutdown();
}

static void Fuzz_Run(Fuzz_Context& ctx) {
  for (auto it = ctx.Data.cbegin(); it != ctx.Data.cend(); ++it) {
    FUZZLOG(MODULE_NAME ": Input[%u/%zu](Payload)=%s",
            (uint)(it - ctx.Data.cbegin() + 1), ctx.Data.size(),
            BytesToHex(*it).c_str());

    NFC_HDR* p_msg;
    p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + it->size());
    if (p_msg == nullptr) {
      FUZZLOG(MODULE_NAME ": GKI_getbuf returns null, size=%zu", it->size());
      return;
    }

    /* Initialize NFC_HDR */
    p_msg->len = it->size();
    p_msg->offset = 0;

    uint8_t* p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    memcpy(p, it->data(), it->size());

    tNFC_CONN conn = {.data = {
                          .status = NFC_STATUS_OK,
                          .p_data = p_msg,
                      }};

    rf_cback(NFC_RF_CONN_ID, NFC_DATA_CEVT, &conn);
  }
}

void Fuzz_FixPackets(std::vector<bytes_t>& Packets, uint /*Seed*/) {
  for (auto it = Packets.begin(); it != Packets.end(); ++it) {
    if (it->size() < LLCP_PDU_HEADER_SIZE) {
      it->resize(LLCP_PDU_HEADER_SIZE);
    }
  }
}

void Fuzz_RunPackets(const std::vector<bytes_t>& Packets) {
  Fuzz_Context ctx(SUB_TYPE_DUMMY, Packets);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }
  Fuzz_Deinit(ctx);
}
