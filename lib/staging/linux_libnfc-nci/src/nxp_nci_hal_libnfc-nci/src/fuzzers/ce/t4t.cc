#include "fuzz.h"

#define MODULE_NAME "Type4 Emulator:"

enum {
  SUB_TYPE_READONLY,
  SUB_TYPE_READWRITE,

  SUB_TYPE_MAX
};

static void ce_cback(tCE_EVENT event, tCE_DATA* p_ce_data) {
  FUZZLOG(MODULE_NAME ": event=0x%02x, p_ce_data=%p", event, p_ce_data);

  if (event == CE_T4T_RAW_FRAME_EVT) {
    if (p_ce_data->raw_frame.p_data) {
      GKI_freebuf(p_ce_data->raw_frame.p_data);
      p_ce_data->raw_frame.p_data = nullptr;
    }
  }
}

static bool Init(Fuzz_Context& /*ctx*/) {
  tNFC_ACTIVATE_DEVT activate_params = {
      .protocol = NFC_PROTOCOL_ISO_DEP,
  };

  ce_init();
  if (NFC_STATUS_OK != CE_SetActivatedTagType(&activate_params, 0, ce_cback)) {
    FUZZLOG(MODULE_NAME ": CE_SetActivatedTagType failed");
    return false;
  }

  if (CE_T4T_AID_HANDLE_INVALID == CE_T4tRegisterAID(0, nullptr, ce_cback)) {
    return false;
  }

  uint8_t AID[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (CE_T4T_AID_HANDLE_INVALID ==
      CE_T4tRegisterAID(sizeof(AID), AID, ce_cback)) {
    return false;
  }

  return true;
}

static bool Init_ReadOnly(Fuzz_Context& ctx) {
  const uint16_t size_max = 1024;
  const uint16_t ndef_len = 256;

  auto ndef_msg = ctx.GetBuffer(size_max);

  return NFC_STATUS_OK ==
         CE_T4tSetLocalNDEFMsg(true, size_max, ndef_len, ndef_msg, nullptr);
}

static bool Init_ReadWrite(Fuzz_Context& ctx) {
  const uint16_t size_max = 1024;
  const uint16_t ndef_len = 256;

  auto ndef_msg = ctx.GetBuffer(size_max);
  auto scratch_buf = ctx.GetBuffer(size_max);

  return NFC_STATUS_OK == CE_T4tSetLocalNDEFMsg(false, size_max, ndef_len,
                                                ndef_msg, scratch_buf);
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  bool result = true;
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

void Type4_FixPackets(uint8_t /*SubType*/, std::vector<bytes_t>& Packets) {
  const uint8_t valid_cmds[] = {T4T_CMD_INS_SELECT, T4T_CMD_INS_READ_BINARY,
                                T4T_CMD_INS_UPDATE_BINARY};

  for (auto it = Packets.begin() + 1; it != Packets.end(); ++it) {
    if (it->size() < T4T_CMD_MIN_HDR_SIZE) {
      it->resize(T4T_CMD_MIN_HDR_SIZE);
    }

    auto p = it->data();
    p[0] = T4T_CMD_CLASS;
    p[1] = valid_cmds[p[1] % sizeof(valid_cmds)];
  }
}

void Type4_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Packets);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }
}
