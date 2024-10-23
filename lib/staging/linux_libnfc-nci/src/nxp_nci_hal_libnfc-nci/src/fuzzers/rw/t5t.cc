#include "fuzz.h"

#define MODULE_NAME "Type5 Read/Write"

enum {
  SUB_TYPE_INVENTORY,
  SUB_TYPE_STAY_QUIET,
  SUB_TYPE_READ_SINGLEBLOCK,
  SUB_TYPE_WRITE_SINGLEBLOCK,
  SUB_TYPE_LOCK_BLOCK,
  SUB_TYPE_READ_MULTIPLEBLOCKS,
  SUB_TYPE_WRITE_MULTIPLEBLOCKS,
  SUB_TYPE_SELECT,
  SUB_TYPE_RESET_TO_READY,
  SUB_TYPE_WRITE_AFI,
  SUB_TYPE_LOCK_AFI,
  SUB_TYPE_WRITE_DSFID,
  SUB_TYPE_LOCK_DSFID,
  SUB_TYPE_GET_SYS_INFO,
  SUB_TYPE_GET_MULTI_BLOCK_SECURITY_STATUS,
  SUB_TYPE_DETECT_NDEF,
  SUB_TYPE_READ_NDEF,
  SUB_TYPE_UPDATE_NDEF,
  SUB_TYPE_FORMAT_NDEF,
  SUB_TYPE_SET_TAG_READONLY,
  SUB_TYPE_PRESENCE_CHECK,

  SUB_TYPE_MAX
};

#define TEST_UID_VALUE \
  { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }
// const uint8_t TEST_UID[] = TEST_UID_VALUE;

static void rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data) {
  FUZZLOG(MODULE_NAME ": rw_cback: event=0x%02x, p_rw_data=%p", event,
          p_rw_data);
  if (event == RW_I93_DATA_EVT || event == RW_I93_NDEF_READ_EVT ||
      event == RW_I93_NDEF_READ_CPLT_EVT) {
    if (p_rw_data->i93_data.p_data) {
      GKI_freebuf(p_rw_data->i93_data.p_data);
      p_rw_data->i93_data.p_data = nullptr;
    }
  } else if (event == RW_I93_RAW_FRAME_EVT) {
    if (p_rw_data->raw_frame.p_data) {
      GKI_freebuf(p_rw_data->raw_frame.p_data);
      p_rw_data->raw_frame.p_data = nullptr;
    }
  }
}

static bool Init(Fuzz_Context& /*ctx*/) {
  tNFC_ACTIVATE_DEVT activate_params = {
      .protocol = static_cast<tNFC_PROTOCOL>(NFC_PROTOCOL_T5T),
      .rf_tech_param = {.mode = NFC_DISCOVERY_TYPE_POLL_V,
                        .param = {.pi93 = {
                                      .uid = TEST_UID_VALUE,
                                  }}}};

  rw_init();
  if (NFC_STATUS_OK != RW_SetActivatedTagType(&activate_params, rw_cback)) {
    FUZZLOG(MODULE_NAME ": RW_SetActivatedTagType failed");
    return false;
  }

  return true;
}

static bool Init_Inventory(Fuzz_Context& /*ctx*/) {
  uint8_t uid[] = TEST_UID_VALUE;
  return NFC_STATUS_OK == RW_I93Inventory(false, 0, uid);
}

static bool Init_StayQuiet(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93StayQuiet();
}

static bool Init_ReadSingleBlock(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93ReadSingleBlock(0);
}

static bool Init_WriteSingleBlock(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_I93WriteSingleBlock(0, scratch);
}

static bool Init_LockBlock(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93LockBlock(0);
}

static bool Init_ReadMultipleBlocks(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93ReadMultipleBlocks(0, 10);
}

static bool Init_WriteMultipleBlocks(Fuzz_Context& ctx) {
  auto scratch = ctx.GetBuffer(16 * 10);
  return NFC_STATUS_OK == RW_I93WriteMultipleBlocks(0, 10, scratch);
}

static bool Init_Select(Fuzz_Context& /*ctx*/) {
  uint8_t uid[] = TEST_UID_VALUE;
  return NFC_STATUS_OK == RW_I93Select(uid);
}

static bool Init_ResetToReady(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93ResetToReady();
}

static bool Init_WriteAFI(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93WriteAFI(0x11);
}

static bool Init_LockAFI(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93LockAFI();
}

static bool Init_WriteDSFID(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93WriteDSFID(0x22);
}

static bool Init_LockDSFID(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93LockDSFID();
}

static bool Init_GetSysInfo(Fuzz_Context& /*ctx*/) {
  uint8_t uid[] = TEST_UID_VALUE;
  return NFC_STATUS_OK == RW_I93GetSysInfo(uid);
}

static bool Init_GetMultiBlockSecurityStatus(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93GetMultiBlockSecurityStatus(0, 10);
}

static bool Init_DetectNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93DetectNDef();
}

static bool Init_ReadNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93ReadNDef();
}

static bool Init_UpdateNDef(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_I93UpdateNDef(sizeof(data), scratch);
}

static bool Init_FormatNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93FormatNDef();
}

static bool Init_SetTagReadOnly(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93SetTagReadOnly();
}

static bool Init_PresenceCheck(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_I93PresenceCheck();
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  bool result = false;
  switch (ctx.SubType) {
    case SUB_TYPE_INVENTORY:
      result = Init_Inventory(ctx);
      break;

    case SUB_TYPE_STAY_QUIET:
      result = Init_StayQuiet(ctx);
      break;

    case SUB_TYPE_READ_SINGLEBLOCK:
      result = Init_ReadSingleBlock(ctx);
      break;

    case SUB_TYPE_WRITE_SINGLEBLOCK:
      result = Init_WriteSingleBlock(ctx);
      break;

    case SUB_TYPE_LOCK_BLOCK:
      result = Init_LockBlock(ctx);
      break;

    case SUB_TYPE_READ_MULTIPLEBLOCKS:
      result = Init_ReadMultipleBlocks(ctx);
      break;

    case SUB_TYPE_WRITE_MULTIPLEBLOCKS:
      result = Init_WriteMultipleBlocks(ctx);
      break;

    case SUB_TYPE_SELECT:
      result = Init_Select(ctx);
      break;

    case SUB_TYPE_RESET_TO_READY:
      result = Init_ResetToReady(ctx);
      break;

    case SUB_TYPE_WRITE_AFI:
      result = Init_WriteAFI(ctx);
      break;

    case SUB_TYPE_LOCK_AFI:
      result = Init_LockAFI(ctx);
      break;

    case SUB_TYPE_WRITE_DSFID:
      result = Init_WriteDSFID(ctx);
      break;

    case SUB_TYPE_LOCK_DSFID:
      result = Init_LockDSFID(ctx);
      break;

    case SUB_TYPE_GET_SYS_INFO:
      result = Init_GetSysInfo(ctx);
      break;

    case SUB_TYPE_GET_MULTI_BLOCK_SECURITY_STATUS:
      result = Init_GetMultiBlockSecurityStatus(ctx);
      break;

    case SUB_TYPE_DETECT_NDEF:
      result = Init_DetectNDef(ctx);
      break;

    case SUB_TYPE_READ_NDEF:
      result = Init_ReadNDef(ctx);
      break;

    case SUB_TYPE_UPDATE_NDEF:
      result = Init_UpdateNDef(ctx);
      break;

    case SUB_TYPE_FORMAT_NDEF:
      result = Init_FormatNDef(ctx);
      break;

    case SUB_TYPE_SET_TAG_READONLY:
      result = Init_SetTagReadOnly(ctx);
      break;

    case SUB_TYPE_PRESENCE_CHECK:
      result = Init_PresenceCheck(ctx);
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
    tRW_I93_CB* p_i93 = &rw_cb.tcb.i93;
    if (p_i93->p_update_data) {
      GKI_freebuf(p_i93->p_update_data);
      p_i93->p_update_data = nullptr;
    }

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

void Type5_FixPackets(uint8_t /*SubType*/, std::vector<bytes_t>& /*Data*/) {}

void Type5_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Data) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Data);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }

  Fuzz_Deinit(ctx);
}
