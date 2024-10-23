#include "fuzz.h"

#define MODULE_NAME "Type1 Read/Write"

enum {
  SUB_TYPE_PRESENCE_CHECK,
  SUB_TYPE_RID,
  SUB_TYPE_READ_ALL,
  SUB_TYPE_READ,
  SUB_TYPE_WRITE_ERASE,
  SUB_TYPE_WRITE_NO_ERASE,
  SUB_TYPE_READ_SEG,
  SUB_TYPE_READ_8,
  SUB_TYPE_WRITE_ERASE_8,
  SUB_TYPE_WRITE_NO_ERASE_8,
  SUB_TYPE_FORMAT_NDEF,
  SUB_TYPE_LOCATE_TLV,
  SUB_TYPE_READ_NDEF,
  SUB_TYPE_WRITE_NDEF,
  SUB_TYPE_SET_READONLY,

  SUB_TYPE_MAX
};

static void rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data) {
  FUZZLOG(MODULE_NAME ": rw_cback: event=0x%02x, p_rw_data=%p", event,
          p_rw_data);

  if (event == RW_T1T_RAW_FRAME_EVT || event == RW_T1T_RID_EVT ||
      event == RW_T1T_RALL_CPLT_EVT || event == RW_T1T_READ_CPLT_EVT ||
      event == RW_T1T_RSEG_CPLT_EVT || event == RW_T1T_READ8_CPLT_EVT) {
    if (p_rw_data->data.p_data) {
      GKI_freebuf(p_rw_data->data.p_data);
      p_rw_data->data.p_data = nullptr;
    }
  }
}

#define TEST_NFCID_VALUE \
  { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }

static bool Init(Fuzz_Context& /*ctx*/) {
  tNFC_ACTIVATE_DEVT activate_params = {
      .protocol = NFC_PROTOCOL_T1T,
      .rf_tech_param = {.mode = NFC_DISCOVERY_TYPE_POLL_A,
                        .param = {.pa = {
                                      .hr = {T1T_NDEF_SUPPORTED, 0x01},
                                      .nfcid1 = TEST_NFCID_VALUE,
                                  }}}};

  rw_init();
  if (NFC_STATUS_OK != RW_SetActivatedTagType(&activate_params, rw_cback)) {
    FUZZLOG(MODULE_NAME ": RW_SetActivatedTagType failed");
    return false;
  }

  return true;
}

static bool Init_PresenceCheck(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tPresenceCheck();
}

static bool Init_Rid(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tRid();
}

static bool Init_ReadAll(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tReadAll();
}

static bool Init_Read(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tRead(0, 0x10);
}

static bool Init_WriteErase(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tWriteErase(0, 0x10, 0x20);
}

static bool Init_WriteNoErase(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tWriteNoErase(0, 0x10, 0x20);
}

static bool Init_ReadSeg(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tReadSeg(0);
}

static bool Init_Read8(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tRead8(0);
}

static bool Init_WriteErase8(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_T1tWriteErase8(0, scratch);
}

static bool Init_WriteNoErase8(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_T1tWriteNoErase8(0, scratch);
}

static bool Init_FormatNdef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tFormatNDef();
}
static bool Init_LocateTlv(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tLocateTlv(TAG_NDEF_TLV);
}
static bool Init_ReadNdef(Fuzz_Context& ctx) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  p_t1t->tag_attribute = RW_T1_TAG_ATTRB_READ_WRITE;
  p_t1t->ndef_msg_len = 256;

  auto scratch = ctx.GetBuffer(4096);
  return NFC_STATUS_OK == RW_T1tReadNDef(scratch, 4096);
}
static bool Init_WriteNdef(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_T1tWriteNDef(sizeof(data), scratch);
}
static bool Init_SetReadOnly(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T1tSetTagReadOnly(true);
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  bool result = false;
  switch (ctx.SubType) {
    case SUB_TYPE_PRESENCE_CHECK:
      result = Init_PresenceCheck(ctx);
      break;
    case SUB_TYPE_RID:
      result = Init_Rid(ctx);
      break;
    case SUB_TYPE_READ_ALL:
      result = Init_ReadAll(ctx);
      break;
    case SUB_TYPE_READ:
      result = Init_Read(ctx);
      break;
    case SUB_TYPE_WRITE_ERASE:
      result = Init_WriteErase(ctx);
      break;
    case SUB_TYPE_WRITE_NO_ERASE:
      result = Init_WriteNoErase(ctx);
      break;
    case SUB_TYPE_READ_SEG:
      result = Init_ReadSeg(ctx);
      break;
    case SUB_TYPE_READ_8:
      result = Init_Read8(ctx);
      break;
    case SUB_TYPE_WRITE_ERASE_8:
      result = Init_WriteErase8(ctx);
      break;
    case SUB_TYPE_WRITE_NO_ERASE_8:
      result = Init_WriteNoErase8(ctx);
      break;
    case SUB_TYPE_FORMAT_NDEF:
      result = Init_FormatNdef(ctx);
      break;
    case SUB_TYPE_LOCATE_TLV:
      result = Init_LocateTlv(ctx);
      break;
    case SUB_TYPE_READ_NDEF:
      result = Init_ReadNdef(ctx);
      break;
    case SUB_TYPE_WRITE_NDEF:
      result = Init_WriteNdef(ctx);
      break;
    case SUB_TYPE_SET_READONLY:
      result = Init_SetReadOnly(ctx);
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

static void Fuzz_Deinit(Fuzz_Context& /*ctx*/) {
  if (rf_cback) {
    tNFC_CONN conn = {
        .deactivate = {.status = NFC_STATUS_OK,
                       .type = NFC_DEACTIVATE_TYPE_IDLE,
                       .is_ntf = true,
                       .reason = NFC_DEACTIVATE_REASON_DH_REQ_FAILED}};

    rf_cback(NFC_RF_CONN_ID, NFC_DEACTIVATE_CEVT, &conn);
  }
}

static void Fuzz_Run(Fuzz_Context& ctx) {
  for (auto it = ctx.Data.cbegin() + 1; it != ctx.Data.cend(); ++it) {
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

    FUZZLOG(MODULE_NAME ": SubType=%02X, Response[%zd/%zd]=%s", ctx.SubType,
            it - ctx.Data.cbegin(), ctx.Data.size() - 1,
            BytesToHex(*it).c_str());

    rf_cback(NFC_RF_CONN_ID, NFC_DATA_CEVT, &conn);
  }
}

void Type1_FixPackets(uint8_t /*SubType*/, std::vector<bytes_t>& /*Data*/) {}

void Type1_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Data) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Data);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }
  Fuzz_Deinit(ctx);
}
