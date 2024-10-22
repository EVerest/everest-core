#include "fuzz.h"

#define MODULE_NAME "Type3 Emulator:"

// Copied from ce_t3t.cc
enum {
  CE_T3T_COMMAND_INVALID,
  CE_T3T_COMMAND_NFC_FORUM,
  CE_T3T_COMMAND_FELICA
};

enum {
  SUB_TYPE_READONLY,
  SUB_TYPE_READWRITE,

  SUB_TYPE_MAX
};

static void ce_cback(tCE_EVENT event, tCE_DATA* p_ce_data) {
  FUZZLOG(MODULE_NAME ": event=0x%02x, p_ce_data=%p", event, p_ce_data);
}

#define TEST_NFCID_VALUE \
  { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }
const uint8_t TEST_NFCID[] = TEST_NFCID_VALUE;

static bool Init(Fuzz_Context& /*ctx*/) {
  tNFC_ACTIVATE_DEVT activate_params = {
      .protocol = NFC_PROTOCOL_T3T,
      .rf_tech_param = {.param = {.lf = {
                                      .nfcid2 = TEST_NFCID_VALUE,
                                  }}}};

  ce_init();
  if (NFC_STATUS_OK != CE_SetActivatedTagType(&activate_params,
                                              T3T_SYSTEM_CODE_NDEF, ce_cback)) {
    FUZZLOG(MODULE_NAME ": CE_SetActivatedTagType failed");
    return false;
  }

  return true;
}

static bool Init_ReadOnly(Fuzz_Context& ctx) {
  const uint32_t size_max = 1024;
  const uint32_t size_current = 256;

  auto p_buf = ctx.GetBuffer(size_max);

  return NFC_STATUS_OK ==
         CE_T3tSetLocalNDEFMsg(true, size_max, size_current, p_buf, nullptr);
}

static bool Init_ReadWrite(Fuzz_Context& ctx) {
  const uint32_t size_max = 1024;
  const uint32_t size_current = 256;

  auto p_buf = ctx.GetBuffer(size_max);
  auto p_scratch = ctx.GetBuffer(size_max);

  return NFC_STATUS_OK ==
         CE_T3tSetLocalNDEFMsg(false, size_max, size_current, p_buf, p_scratch);
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  bool result = false;
  switch (ctx.SubType) {
    case SUB_TYPE_READONLY:
      result = Init_ReadOnly(ctx);
      break;
    case SUB_TYPE_READWRITE:
      result = Init_ReadWrite(ctx);
      break;
    default:
      FUZZLOG(MODULE_NAME ": Unknown command %d", ctx.SubType);
      result = false;
      break;
  }

  if (!result) {
    FUZZLOG(MODULE_NAME ": Initializing command %02X failed", ctx.SubType);
  }

  return result;
}

static void Fuzz_Run(Fuzz_Context& ctx) {
  for (auto it = ctx.Data.cbegin(); it != ctx.Data.cend(); ++it) {
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

    FUZZLOG(MODULE_NAME ": SubType=%02X, Response[%zd/%zu]=%s", ctx.SubType,
            it - ctx.Data.cbegin() + 1, ctx.Data.size(),
            BytesToHex(*it).c_str());

    rf_cback(NFC_RF_CONN_ID, NFC_DATA_CEVT, &conn);
  }
}

void Type3_FixPackets(uint8_t /*SubType*/, std::vector<bytes_t>& Packets) {
  for (auto it = Packets.begin() + 1; it != Packets.end(); ++it) {
    if (it->size() < T3T_MSG_CMD_COMMON_HDR_LEN) {
      it->resize(T3T_MSG_CMD_COMMON_HDR_LEN);
      memset(it->data(), 0, it->size());
    }

    auto p = it->data();
    p[0] = it->size();

    if (p[1] != CE_T3T_COMMAND_FELICA) {
      memcpy(&p[2], TEST_NFCID, sizeof(TEST_NFCID));
    }
  }
}

void Type3_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Packets);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }
}
