#include "fuzz.h"

void hal_inject_event(uint8_t hal_evt, tHAL_NFC_STATUS status) {
  tNFC_HAL_EVT_MSG msg = {};

  msg.hdr.len = 0;
  msg.hdr.event = BT_EVT_TO_NFC_MSGS;
  msg.hdr.offset = 0;
  msg.hdr.layer_specific = 0;
  msg.hal_evt = hal_evt;
  msg.status = status;

  FUZZLOG("Injecting event to NFC code: event=%d, status=%d", hal_evt, status);
  nfc_main_handle_hal_evt(&msg);
}

bool hal_inject_data(const uint8_t* p_data, uint16_t data_len) {
  FUZZLOG("Injecting data to NFC stack: %s",
          BytesToHex(p_data, data_len).c_str());

  // For NCI responses, nfc_ncif_process_event checks the response OID matches
  // the command being sent last time. So mimic this by always copying the first
  // two bytes into last header.
  if (data_len >= sizeof(nfc_cb.last_hdr)) {
    memcpy(nfc_cb.last_hdr, p_data, sizeof(nfc_cb.last_hdr));
  }

  NFC_HDR* p_msg;
  p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + NFC_RECEIVE_MSGS_OFFSET +
                               data_len);
  if (p_msg != nullptr) {
    /* Initialize NFC_HDR */
    p_msg->len = data_len;
    p_msg->event = BT_EVT_TO_NFC_NCI;
    p_msg->offset = NFC_RECEIVE_MSGS_OFFSET;

    uint8_t* p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    memcpy(p, p_data, p_msg->len);

    if (nfc_ncif_process_event(p_msg)) {
      GKI_freebuf(p_msg);
    }
    return true;
  } else {
    LOG(ERROR) << StringPrintf("No buffer");
    return false;
  }
}

static void HalInitialize() { FUZZLOG("HAL_OP: type=initialize"); }

static void HalTerminate() { FUZZLOG("HAL_OP: type=terminate"); }

static void HalOpen(tHAL_NFC_CBACK* /*p_hal_inject_event*/,
                    tHAL_NFC_DATA_CBACK* /*p_data_cback*/) {
  FUZZLOG("HAL_OP, type=open");
  hal_inject_event(HAL_NFC_OPEN_CPLT_EVT, HAL_NFC_STATUS_OK);
}

static void HalClose() {
  FUZZLOG("HAL_OP, type=close");
  hal_inject_event(HAL_NFC_CLOSE_CPLT_EVT, HAL_NFC_STATUS_OK);
}

const uint8_t reset_req[] = {0x20, 0x00, 0x01, 0x01};

const uint8_t reset_rsp[] = {0x40, 0x00, 0x01, 0x00};

const uint8_t reset_ntf[] = {0x60, 0x00, 0x09, 0x02, 0x01, 0x20,
                             0x04, 0x04, 0x51, 0x12, 0x01, 0x90};

const uint8_t init_req[] = {0x20, 0x01, 0x02, 0x00, 0x00};

const uint8_t init_rsp[] = {
    0x40, 0x01, 0x1E, 0x00, 0x1A, 0x7E, 0x06, 0x01, 0x01, 0x5C, 0x03,
    0xFF, 0xFF, 0x01, 0xFF, 0x00, 0x08, 0x00, 0x00, 0x01, 0x00, 0x02,
    0x00, 0x03, 0x00, 0x80, 0x00, 0x82, 0x00, 0x83, 0x00, 0x84, 0x00};

static void HalWrite(uint16_t data_len, uint8_t* p_data) {
  FUZZLOG("HAL_OP: type=write, data=%s", BytesToHex(p_data, data_len).c_str());

  if (data_len == sizeof(reset_req) &&
      memcmp(reset_req, p_data, data_len) == 0) {
    hal_inject_data(reset_rsp, sizeof(reset_rsp));
    hal_inject_data(reset_ntf, sizeof(reset_ntf));
  } else if (data_len == sizeof(init_req) &&
             memcmp(init_req, p_data, data_len) == 0) {
    hal_inject_data(init_rsp, sizeof(init_rsp));
  }
}

static void HalCoreInitialized(uint16_t data_len,
                               uint8_t* p_core_init_rsp_params) {
  FUZZLOG("HAL_OP: type=coreInitialized, data=%s",
          BytesToHex(p_core_init_rsp_params, data_len).c_str());
  hal_inject_event(HAL_NFC_POST_INIT_CPLT_EVT, HAL_NFC_STATUS_OK);
}

static bool HalPrediscover() {
  FUZZLOG("HAL_OP: type=prediscover, return=false");
  return false;
}

static void HalControlGranted() { FUZZLOG("HAL_OP: type=controlGranted"); }

static void HalPowerCycle() { FUZZLOG("HAL_OP: type=powerCycle"); }

// Magic value from the real NFC code.
#define MAX_NFC_EE 2
static uint8_t HalGetMaxNfcee() {
  FUZZLOG("HAL_OP: type=getMaxNfcee, return=%d", MAX_NFC_EE);
  return MAX_NFC_EE;
}

static tHAL_NFC_ENTRY s_halFuncEntries = {
    .initialize = HalInitialize,
    .terminate = HalTerminate,
    .open = HalOpen,
    .close = HalClose,
    .core_initialized = HalCoreInitialized,
    .write = HalWrite,
    .prediscover = HalPrediscover,
    .control_granted = HalControlGranted,
    .power_cycle = HalPowerCycle,
    .get_max_ee = HalGetMaxNfcee,
};

tHAL_NFC_ENTRY* get_hal_func_entries() { return &s_halFuncEntries; }