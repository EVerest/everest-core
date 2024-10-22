#include "fuzz.h"

#define MODULE_NAME "Type3 Read/Write"

enum {
  SUB_TYPE_CHECK_NDEF,
  SUB_TYPE_UPDATE_NDEF,
  SUB_TYPE_CHECK,
  SUB_TYPE_UPDATE,
  SUB_TYPE_SEND_RAW_FRAME,

  SUB_TYPE_NCI_CMD_FIRST,
  SUB_TYPE_DETECT_NDEF = SUB_TYPE_NCI_CMD_FIRST,
  SUB_TYPE_PRESENCE_CHECK,
  SUB_TYPE_POLL,
  SUB_TYPE_GET_SYSTEM_CODES,
  SUB_TYPE_FORMAT_NDEF,
  SUB_TYPE_SET_READ_ONLY_SOFT,
  SUB_TYPE_SET_READ_ONLY_HARD,

  SUB_TYPE_MAX
};

// The following definition are copied from rw_t3t.cc
// ============================================================================

/* Default NDEF attribute information block (used when formatting Felica-Lite
 * tags) */
/* NBr (max block reads per cmd)*/
#define RW_T3T_DEFAULT_FELICALITE_NBR 4
/* NBw (max block write per cmd)*/
#define RW_T3T_DEFAULT_FELICALITE_NBW 1
#define RW_T3T_DEFAULT_FELICALITE_NMAXB (T3T_FELICALITE_NMAXB)
#define RW_T3T_DEFAULT_FELICALITE_ATTRIB_INFO_CHECKSUM                       \
  ((T3T_MSG_NDEF_VERSION + RW_T3T_DEFAULT_FELICALITE_NBR +                   \
    RW_T3T_DEFAULT_FELICALITE_NBW + (RW_T3T_DEFAULT_FELICALITE_NMAXB >> 8) + \
    (RW_T3T_DEFAULT_FELICALITE_NMAXB & 0xFF) + T3T_MSG_NDEF_WRITEF_OFF +     \
    T3T_MSG_NDEF_RWFLAG_RW) &                                                \
   0xFFFF)
// ============================================================================

static void rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data) {
  FUZZLOG(MODULE_NAME ": rw_cback: event=0x%02x, p_rw_data=%p", event,
          p_rw_data);

  if (event == RW_T3T_RAW_FRAME_EVT) {
    if (p_rw_data->data.p_data) {
      GKI_freebuf(p_rw_data->data.p_data);
      p_rw_data->data.p_data = nullptr;
    }
  }
}

#define TEST_NFCID_VALUE \
  { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 }
const uint8_t TEST_NFCID[] = TEST_NFCID_VALUE;

static bool Init(Fuzz_Context& /*ctx*/) {
  tNFC_ACTIVATE_DEVT activate_params = {
      .protocol = NFC_PROTOCOL_T3T,
      .rf_tech_param = {.mode = NFC_DISCOVERY_TYPE_POLL_F,
                        .param = {.pf = {
                                      .nfcid2 = TEST_NFCID_VALUE,
                                      .bit_rate = NFC_BIT_RATE_212,
                                      .sensf_res_len = NFC_MAX_SENSF_RES_LEN,
                                      .mrti_check = 1,
                                      .mrti_update = 1,
                                  }}}};

  rw_init();
  if (NFC_STATUS_OK != RW_SetActivatedTagType(&activate_params, rw_cback)) {
    FUZZLOG(MODULE_NAME ": RW_SetActivatedTagType failed");
    return false;
  }

  // A workaround to initialize Type3 tag attribute
  tRW_T3T_DETECT t3t_detect = {
      NFC_STATUS_OK,         // tNFC_STATUS status;
      T3T_MSG_NDEF_VERSION,  // uint8_t version; /* Ver: peer version */
      RW_T3T_DEFAULT_FELICALITE_NBR,  // uint8_t
                                      // nbr; /* NBr: number of blocks that can
                                      // be read using one Check command */
      RW_T3T_DEFAULT_FELICALITE_NBW,  // uint8_t nbw;    /* Nbw: number of
                                      // blocks that can be written using one
                                      // Update command */
      RW_T3T_DEFAULT_FELICALITE_NMAXB,  // uint16_t nmaxb; /* Nmaxb: maximum
                                        // number of blocks available for NDEF
                                        // data */
      T3T_MSG_NDEF_WRITEF_OFF,  // uint8_t writef; /* WriteFlag: 00h if writing
                                // data finished; 0Fh if writing data in
                                // progress */
      T3T_MSG_NDEF_RWFLAG_RW,   // uint8_t
                               // rwflag;  /* RWFlag: 00h NDEF is read-only; 01h
                               // if read/write available */
      0x100 * 16,  // uint32_t ln; /* Ln: actual size of stored NDEF data (in
                   // bytes) */
  };

  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  memcpy(&p_cb->ndef_attrib, &t3t_detect, sizeof(t3t_detect));

  // workaround of issue b/139424089
  p_cb->p_cur_cmd_buf->offset = 1;
  p_cb->p_cur_cmd_buf->len = 0;
  return true;
}

static bool Init_CheckNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tCheckNDef();
}

static bool Init_UpdateNDef(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  auto scratch = ctx.GetBuffer(sizeof(data), data);
  return NFC_STATUS_OK == RW_T3tUpdateNDef(sizeof(data), scratch);
}

static bool Init_Check(Fuzz_Context& /*ctx*/) {
  tT3T_BLOCK_DESC t3t_blocks = {0x000B, 5};
  return NFC_STATUS_OK == RW_T3tCheck(1, &t3t_blocks);
}

static bool Init_Update(Fuzz_Context& ctx) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                          0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
  auto scratch = ctx.GetBuffer(sizeof(data), data);
  tT3T_BLOCK_DESC t3t_blocks = {0x000B, 5};
  return NFC_STATUS_OK == RW_T3tUpdate(1, &t3t_blocks, scratch);
}

static bool Init_SendRawFrame(Fuzz_Context& /*ctx*/) {
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04,
                    0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};

  return NFC_STATUS_OK == RW_T3tSendRawFrame(sizeof(data), data);
}

static bool Init_DetectNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tDetectNDef();
}

static bool Init_PresenceCheck(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tPresenceCheck();
}

static bool Init_Poll(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tPoll(0, 1, 0);
}

static bool Init_GetSystemCode(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tGetSystemCodes();
}

static bool Init_FormatNDef(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tFormatNDef();
}

static bool Init_SetReadonly(Fuzz_Context& /*ctx*/) {
  return NFC_STATUS_OK == RW_T3tSetReadOnly(true);
}

static bool Fuzz_Init(Fuzz_Context& ctx) {
  if (!Init(ctx)) {
    FUZZLOG(MODULE_NAME ": initialization failed");
    return false;
  }

  bool result = false;
  switch (ctx.SubType) {
    case SUB_TYPE_CHECK_NDEF:
      result = Init_CheckNDef(ctx);
      break;
    case SUB_TYPE_UPDATE_NDEF:
      result = Init_UpdateNDef(ctx);
      break;
    case SUB_TYPE_CHECK:
      result = Init_Check(ctx);
      break;
    case SUB_TYPE_UPDATE:
      result = Init_Update(ctx);
      break;
    case SUB_TYPE_SEND_RAW_FRAME:
      result = Init_SendRawFrame(ctx);
      break;
    case SUB_TYPE_DETECT_NDEF:
      result = Init_DetectNDef(ctx);
      break;
    case SUB_TYPE_PRESENCE_CHECK:
      result = Init_PresenceCheck(ctx);
      break;
    case SUB_TYPE_POLL:
      result = Init_Poll(ctx);
      break;
    case SUB_TYPE_GET_SYSTEM_CODES:
      result = Init_GetSystemCode(ctx);
      break;
    case SUB_TYPE_FORMAT_NDEF:
      result = Init_FormatNDef(ctx);
      break;
    case SUB_TYPE_SET_READ_ONLY_SOFT:
    case SUB_TYPE_SET_READ_ONLY_HARD:
      result = Init_SetReadonly(ctx);
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

static void t3t_nci_msg(NFC_HDR* p_msg) {
  uint8_t status;
  uint8_t num_responses;

  uint8_t* p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  uint8_t plen = p_msg->len;

  if (plen >= 2) {
    /* Pass result to RW_T3T for processing */
    STREAM_TO_UINT8(status, p);
    STREAM_TO_UINT8(num_responses, p);
    plen -= NFC_TL_SIZE;
    rw_t3t_handle_nci_poll_ntf(status, num_responses, (uint8_t)plen, p);
  }

  GKI_freebuf(p_msg);
}

static void t3t_data_msg(NFC_HDR* p_msg) {
  tNFC_CONN conn = {.data = {
                        .status = NFC_STATUS_OK,
                        .p_data = p_msg,
                    }};

  rf_cback(NFC_RF_CONN_ID, NFC_DATA_CEVT, &conn);
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

    FUZZLOG(MODULE_NAME ": SubType=%02X, Response[%zd/%zd]=%s", ctx.SubType,
            it - ctx.Data.cbegin(), ctx.Data.size() - 1,
            BytesToHex(*it).c_str());

    if (ctx.SubType >= SUB_TYPE_NCI_CMD_FIRST) {
      t3t_nci_msg(p_msg);
    } else {
      t3t_data_msg(p_msg);
    }
  }
}

void Type3_FixPackets(uint8_t SubType, std::vector<bytes_t>& Packets) {
  for (auto it = Packets.begin() + 1; it != Packets.end(); ++it) {
    if (SubType >= SUB_TYPE_NCI_CMD_FIRST) {
      if (it->size() < 3) {
        it->resize(3);
        memset(it->data(), 0, it->size());
      }
    } else {
      if (it->size() <= T3T_MSG_RSP_COMMON_HDR_LEN) {
        it->resize(T3T_MSG_RSP_COMMON_HDR_LEN + 1);
        memset(it->data(), 0, it->size());
      }

      uint8_t* p = it->data();
      p[0] = it->size();
      p[it->size() - 1] = NFC_STATUS_OK;

      auto rsp = &p[1];
      rsp[T3T_MSG_RSP_OFFSET_STATUS1] = T3T_MSG_RSP_STATUS_OK;
      memcpy(&rsp[T3T_MSG_RSP_OFFSET_IDM], TEST_NFCID, sizeof(TEST_NFCID));
    }
  }
}

void Type3_Fuzz(uint8_t SubType, const std::vector<bytes_t>& Packets) {
  Fuzz_Context ctx(SubType % SUB_TYPE_MAX, Packets);
  if (Fuzz_Init(ctx)) {
    Fuzz_Run(ctx);
  }
  Fuzz_Deinit(ctx);
}
