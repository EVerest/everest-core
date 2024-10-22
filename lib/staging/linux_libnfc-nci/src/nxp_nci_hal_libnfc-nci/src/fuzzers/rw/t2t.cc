#include "fuzz.h"

#define MODULE_NAME "Type2 Read/Write"

enum {
  SUB_TYPE_PRESENCE_CHECK,
  SUB_TYPE_READ,
  SUB_TYPE_WRITE,
  SUB_TYPE_SECTOR_SELECT,
  SUB_TYPE_SET_TAG_READONLY,
  SUB_TYPE_WRITE_NDEF,
  SUB_TYPE_READ_NDEF,
  SUB_TYPE_DETECT_NDEF,
  SUB_TYPE_LOCATE_TLV,
  SUB_TYPE_FORMAT_NDEF,
  SUB_TYPE_MAX
};

static void rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data) {
  FUZZLOG(MODULE_NAME ": rw_cback: event=0x%02x, p_rw_data=%p", event,
          p_rw_data);

  if (event == RW_T2T_READ_CPLT_EVT || event == RW_T2T_RAW_FRAME_EVT) {
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
      .protocol = NFC_PROTOCOL_T2T,
      .rf_tech_param = {.mode = NFC_DISCOVERY_TYPE_POLL_A,
                        .param = {.pa = {
                                      .sel_rsp = NFC_SEL_RES_NFC_FORUM_T2T,
                                  }}}};

  rw_init();
  if (NFC_STATUS_OK != RW_SetActivatedTagType(&activate_params, rw_cback)) {
    FUZZLOG(MODULE_NAME ": RW_SetActivatedTagType failed");
    return false;
  }

  return true;
}

static bool Init_PresenceCheck(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tPresenceCheck();
}

static bool Init_Read(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tRead(0);
}

static bool Init_Write(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_T2tWrite(0, scratch);
}

static bool Init_SectorSelect(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tSectorSelect(0);
}

static bool Init_SetTagReadOnly(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tSetTagReadOnly(true);
}

static bool Init_WriteNDef(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->ndef_status = T2T_NDEF_DETECTED;
  return NFC_STATUS_OK == RW_T2tWriteNDef(sizeof(data), scratch);
}

static bool Init_ReadNDef(Fuzz_Context& ctx) {
  auto scratch = ctx.GetBuffer(256);
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->ndef_status = T2T_NDEF_DETECTED;
  p_t2t->ndef_msg_len = 128;
  p_t2t->bytes_count = 128;
  return NFC_STATUS_OK == RW_T2tReadNDef(scratch, 256);
}

static bool Init_DetectNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tDetectNDef(true);
}

static bool Init_LocateTlv(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tLocateTlv(TAG_LOCK_CTRL_TLV);
}

static bool Init_FormatNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T2tFormatNDef();
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
    case SUB_TYPE_READ:
      result = Init_Read(ctx);
      break;
    case SUB_TYPE_WRITE:
      result = Init_Write(ctx);
      break;
    case SUB_TYPE_SECTOR_SELECT:
      result = Init_SectorSelect(ctx);
      break;
    case SUB_TYPE_SET_TAG_READONLY:
      result = Init_SetTagReadOnly(ctx);
      break;
    case SUB_TYPE_WRITE_NDEF:
      result = Init_WriteNDef(ctx);
      break;
    case SUB_TYPE_READ_NDEF:
      result = Init_ReadNDef(ctx);
      break;
    case SUB_TYPE_DETECT_NDEF:
      result = Init_DetectNDef(ctx);
      break;
    case SUB_TYPE_LOCATE_TLV:
      result = Init_LocateTlv(ctx);
      break;
    case SUB_TYPE_FORMAT_NDEF:
      result = Init_FormatNDef(ctx);
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

void Type2_FixPackets(uint8_t /*SubType*/, std::vector<bytes_t>& /*Data*/) {}

void Type2_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Data) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Data);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }

  Fuzz_Deinit(ctx);
}
