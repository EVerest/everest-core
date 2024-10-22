/*
 * Copyright (C) 2012-2014 NXP Semiconductors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <phNxpLog.h>
#include <phNxpNciHal.h>
#include <phNxpNciHal_NfcDepSWPrio.h>

/* Timeout value to wait for NFC-DEP detection.*/
#define CUSTOM_POLL_TIMEOUT 160
#define CLEAN_UP_TIMEOUT 250
#define MAX_WRITE_RETRY 5

#define MAX_POLL_CMD_LEN 64
#define NCI_HEADER_SIZE 3
/******************* Global variables *****************************************/
extern phNxpNciHal_Control_t nxpncihal_ctrl;
extern NFCSTATUS phNxpNciHal_send_ext_cmd(uint16_t cmd_len, uint8_t* p_cmd);
static uint8_t cmd_stop_rf_discovery[] = {0x21, 0x06, 0x01, 0x00}; /* IDLE */
static uint8_t cmd_resume_rf_discovery[] = {0x21, 0x06, 0x01,
                                            0x03}; /* RF_DISCOVER */

/*RF_DISCOVER_SELECT_CMD*/
static uint8_t cmd_select_rf_discovery[] = {0x21, 0x04, 0x03, 0x01, 0x04, 0x02};

static uint8_t cmd_poll[MAX_POLL_CMD_LEN];
static uint8_t cmd_poll_len = 0;
int discover_type = 0xFF;
uint32_t cleanup_timer;

/*PRIO LOGIC related dead functions undefined*/
#ifdef P2P_PRIO_LOGIC_HAL_IMP

static int iso_dep_detected = 0x00;
static int poll_timer_fired = 0x00;
static uint8_t bIgnorep2plogic = 0;
static uint8_t* p_iso_ntf_buff = NULL; /* buffer to store second notification */
static uint8_t bIgnoreIsoDep = 0;
static uint32_t custom_poll_timer;

/************** NFC-DEP SW PRIO functions *************************************/

static NFCSTATUS phNxpNciHal_start_polling_loop(void);
static NFCSTATUS phNxpNciHal_stop_polling_loop(void);
static NFCSTATUS phNxpNciHal_resume_polling_loop(void);
static void phNxpNciHal_NfcDep_store_ntf(uint8_t* p_cmd_data, uint16_t cmd_len);

/*******************************************************************************
**
** Function         cleanup_timer_handler
**
** Description      Callback function for cleanup timer.
**
** Returns          None
**
*******************************************************************************/
static void cleanup_timer_handler(uint32_t timerId, void* pContext) {
  NXPLOG_NCIHAL_D(">> cleanup_timer_handler.");

  NXPLOG_NCIHAL_D(
      ">> cleanup_timer_handler. ISO_DEP not detected second time.");

  phOsalNfc_Timer_Delete(cleanup_timer);
  cleanup_timer = 0;
  iso_dep_detected = 0x00;
  EnableP2P_PrioLogic = false;
  return;
}

/*******************************************************************************
**
** Function         custom_poll_timer_handler
**
** Description      Callback function for custom poll timer.
**
** Returns          None
**
*******************************************************************************/
static void custom_poll_timer_handler(uint32_t timerId, void* pContext) {
  NXPLOG_NCIHAL_D(">> custom_poll_timer_handler.");

  NXPLOG_NCIHAL_D(
      ">> custom_poll_timer_handler. NFC_DEP not detected. so giving early "
      "chance to ISO_DEP.");

  phOsalNfc_Timer_Delete(custom_poll_timer);

  if (iso_dep_detected == 0x01) {
    poll_timer_fired = 0x01;

    /*
     * Restart polling loop.
     * When the polling loop is stopped, polling will be restarted.
     */
    NXPLOG_NCIHAL_D(">> custom_poll_timer_handler - restart polling loop.");

    phNxpNciHal_stop_polling_loop();
  } else {
    NXPLOG_NCIHAL_D(
        ">> custom_poll_timer_handler - invalid flag state (iso_dep_detected)");
  }

  return;
}
/*******************************************************************************
**
** Function         phNxpNciHal_stop_polling_loop
**
** Description      Sends stop polling cmd to NFCC
**
** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED
**
*******************************************************************************/
static NFCSTATUS phNxpNciHal_stop_polling_loop() {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  phNxpNciHal_Sem_t cb_data;
  pthread_t pthread;
  discover_type = STOP_POLLING;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&pthread, &attr, tmp_thread, (void*)&discover_type) != 0) {
    NXPLOG_NCIHAL_E("fail to create pthread");
  }
  pthread_attr_destroy(&attr);
  return status;
}

/*******************************************************************************
**
** Function         phNxpNciHal_resume_polling_loop
**
** Description      Sends resume polling cmd to NFCC
**
** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED
**
*******************************************************************************/
static NFCSTATUS phNxpNciHal_resume_polling_loop() {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  phNxpNciHal_Sem_t cb_data;
  pthread_t pthread;
  discover_type = RESUME_POLLING;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&pthread, &attr, tmp_thread, (void*)&discover_type) != 0) {
    NXPLOG_NCIHAL_E("fail to create pthread");
  }
  pthread_attr_destroy(&attr);
  return status;
}

/*******************************************************************************
**
** Function         phNxpNciHal_start_polling_loop
**
** Description      Sends start polling cmd to NFCC
**
** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED
**
*******************************************************************************/
NFCSTATUS phNxpNciHal_start_polling_loop() {
  NFCSTATUS status = NFCSTATUS_FAILED;
  phNxpNciHal_Sem_t cb_data;
  pthread_t pthread;
  discover_type = START_POLLING;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&pthread, &attr, tmp_thread, (void*)&discover_type) != 0) {
    NXPLOG_NCIHAL_E("fail to create pthread");
  }
  pthread_attr_destroy(&attr);
  return status;
}

/*******************************************************************************
**
** Function         phNxpNciHal_NfcDep_rsp_ext
**
** Description      Implements algorithm for NFC-DEP protocol priority over
**                  ISO-DEP protocol.
**                  Following the algorithm:
**                  IF ISO-DEP detected first time,set the ISO-DEP detected flag
**                  and resume polling loop with 60ms timeout value.
**                      a) if than NFC-DEP detected than send the response to
**                         libnfc-nci stack and stop the timer.
**                      b) if NFC-DEP not detected with in 60ms, than restart
**                         the polling loop to give early chance to ISO-DEP with
**                         a cleanup timer.
**                      c) if ISO-DEP detected second time send the response to
**                         libnfc-nci stack and stop the cleanup timer.
**                      d) if ISO-DEP not detected with in cleanup timeout, than
**                         clear the ISO-DEP detection flag.
**
** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED
**
*******************************************************************************/
NFCSTATUS phNxpNciHal_NfcDep_rsp_ext(uint8_t* p_ntf, uint16_t* p_len) {
  NFCSTATUS status = NFCSTATUS_INVALID_PARAMETER;

  NXPLOG_NCIHAL_D(">> p_ntf[0]=%02x , p_ntf[1]=%02x", p_ntf[0], p_ntf[1]);

  if (p_ntf[0] == 0x41 && p_ntf[1] == 0x04) {
    // Tag selected, Disable P2P Prio logic.
    bIgnoreIsoDep = 1;
    NXPLOG_NCIHAL_D(">> Tag selected, Disable P2P Prio logic.");

  } else if (((p_ntf[0] == 0x61 && p_ntf[1] == 0x06) ||
              (p_ntf[0] == 0x41 && p_ntf[1] == 0x06)) &&
             bIgnoreIsoDep == 1) {
    // Tag deselected, enable P2P Prio logic.
    bIgnoreIsoDep = 0x00;
    NXPLOG_NCIHAL_D(">> Tag deselected, enable P2P Prio logic.");
  }
  if (bIgnoreIsoDep == 0x00 && p_ntf[0] == 0x61 && p_ntf[1] == 0x05 &&
      *p_len > 5) {
    if (p_ntf[5] == 0x04 && p_ntf[6] < 0x80) {
      NXPLOG_NCIHAL_D(">> ISO DEP detected.");

      if (iso_dep_detected == 0x00) {
        NXPLOG_NCIHAL_D(">> ISO DEP detected first time. Resume polling loop");

        iso_dep_detected = 0x01;
        status = phNxpNciHal_resume_polling_loop();

        custom_poll_timer = phOsalNfc_Timer_Create();
        NXPLOG_NCIHAL_D("custom poll timer started - %d", custom_poll_timer);

        status = phOsalNfc_Timer_Start(custom_poll_timer, CUSTOM_POLL_TIMEOUT,
                                       &custom_poll_timer_handler, NULL);

        if (NFCSTATUS_SUCCESS == status) {
          NXPLOG_NCIHAL_D("custom poll timer started");
        } else {
          NXPLOG_NCIHAL_E("custom poll timer not started!!!");
          status = NFCSTATUS_FAILED;
        }

        status = NFCSTATUS_FAILED;
      } else {
        NXPLOG_NCIHAL_D(">> ISO DEP detected second time.");
        /* Store notification */
        phNxpNciHal_NfcDep_store_ntf(p_ntf, *p_len);

        /* Stop Cleanup_timer */
        phOsalNfc_Timer_Stop(cleanup_timer);
        phOsalNfc_Timer_Delete(cleanup_timer);
        cleanup_timer = 0;
        EnableP2P_PrioLogic = false;
        iso_dep_detected = 0;
        status = NFCSTATUS_SUCCESS;
      }
    } else if (p_ntf[5] == 0x05) {
      NXPLOG_NCIHAL_D(">> NFC-DEP Detected - stopping the custom poll timer");

      phOsalNfc_Timer_Stop(custom_poll_timer);
      phOsalNfc_Timer_Delete(custom_poll_timer);
      EnableP2P_PrioLogic = false;
      iso_dep_detected = 0;
      status = NFCSTATUS_SUCCESS;
    } else {
      NXPLOG_NCIHAL_D(
          ">>  detected other technology- stopping the custom poll timer");
      phOsalNfc_Timer_Stop(custom_poll_timer);
      phOsalNfc_Timer_Delete(custom_poll_timer);
      EnableP2P_PrioLogic = false;
      iso_dep_detected = 0;
      status = NFCSTATUS_INVALID_PARAMETER;
    }
  } else if (bIgnoreIsoDep == 0x00 &&
             ((p_ntf[0] == 0x41 && p_ntf[1] == 0x06) ||
              (p_ntf[0] == 0x61 && p_ntf[1] == 0x06))) {
    NXPLOG_NCIHAL_D(">> RF disabled");
    if (poll_timer_fired == 0x01) {
      poll_timer_fired = 0x00;

      NXPLOG_NCIHAL_D(">>restarting polling loop.");

      /* start polling loop */
      phNxpNciHal_start_polling_loop();
      EnableP2P_PrioLogic = false;
      NXPLOG_NCIHAL_D(
          ">> NFC DEP NOT  detected - custom poll timer expired - RF disabled");

      cleanup_timer = phOsalNfc_Timer_Create();

      /* Start cleanup_timer */
      NFCSTATUS status = phOsalNfc_Timer_Start(cleanup_timer, CLEAN_UP_TIMEOUT,
                                               &cleanup_timer_handler, NULL);

      if (NFCSTATUS_SUCCESS == status) {
        NXPLOG_NCIHAL_D("cleanup timer started");
      } else {
        NXPLOG_NCIHAL_E("cleanup timer not started!!!");
        status = NFCSTATUS_FAILED;
      }

      status = NFCSTATUS_FAILED;
    } else {
      status = NFCSTATUS_SUCCESS;
    }
  }
  if (bIgnoreIsoDep == 0x00 && iso_dep_detected == 1) {
    if ((p_ntf[0] == 0x41 && p_ntf[1] == 0x06) ||
        (p_ntf[0] == 0x61 && p_ntf[1] == 0x06)) {
      NXPLOG_NCIHAL_D(">>iso_dep_detected Disconnect related notification");
      status = NFCSTATUS_FAILED;
    } else {
      NXPLOG_NCIHAL_W("Never come here");
    }
  }

  return status;
}
/*******************************************************************************
**
** Function         phNxpNciHal_NfcDep_store_ntf
**
** Description      Stores the iso dep notification locally.
**
** Returns          None
**
*******************************************************************************/
static void phNxpNciHal_NfcDep_store_ntf(uint8_t* p_cmd_data,
                                         uint16_t cmd_len) {
  p_iso_ntf_buff = NULL;

  p_iso_ntf_buff = malloc(sizeof(uint8_t) * cmd_len);
  if (p_iso_ntf_buff == NULL) {
    NXPLOG_NCIHAL_E("Error allocating memory (p_iso_ntf_buff)");
    return;
  }
  memcpy(p_iso_ntf_buff, p_cmd_data, cmd_len);
  bIgnorep2plogic = 1;
}

/*******************************************************************************
**
** Function         phNxpNciHal_NfcDep_comapre_ntf
**
** Description      Compare the notification with previous iso dep notification.
**
** Returns          NFCSTATUS_SUCCESS if successful,otherwise NFCSTATUS_FAILED
**
*******************************************************************************/
NFCSTATUS phNxpNciHal_NfcDep_comapre_ntf(uint8_t* p_cmd_data,
                                         uint16_t cmd_len) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  int32_t ret_val = -1;

  if (bIgnorep2plogic == 1) {
    ret_val = memcmp(p_cmd_data, p_iso_ntf_buff, cmd_len);
    if (ret_val != 0) {
      NXPLOG_NCIHAL_E("Third notification is not equal to last");
    } else {
      NXPLOG_NCIHAL_D(
          "Third notification is equal to last (disable p2p logic)");
      status = NFCSTATUS_SUCCESS;
    }
    bIgnorep2plogic = 0;
  }
  if (p_iso_ntf_buff != NULL) {
    free(p_iso_ntf_buff);
    p_iso_ntf_buff = NULL;
  }

  return status;
}

extern NFCSTATUS phNxpNciHal_clean_P2P_Prio() {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  iso_dep_detected = 0x00;
  EnableP2P_PrioLogic = false;
  poll_timer_fired = 0x00;
  bIgnorep2plogic = 0x00;
  bIgnoreIsoDep = 0x00;

  status = phOsalNfc_Timer_Stop(cleanup_timer);
  status |= phOsalNfc_Timer_Delete(cleanup_timer);

  status |= phOsalNfc_Timer_Stop(custom_poll_timer);
  status |= phOsalNfc_Timer_Delete(custom_poll_timer);
  cleanup_timer = 0;
  return status;
}

#endif

/*******************************************************************************
 **
 ** Function         tmp_thread
 **
 ** Description      Thread to execute custom poll commands .
 **
 ** Returns          None
 **
 *******************************************************************************/
void* tmp_thread(void* tmp) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint16_t data_len;
  NXPLOG_NCIHAL_W("tmp_thread: enter type=0x0%x", *((int*)tmp));
  usleep(10 * 1000);

  switch (*((int*)tmp)) {
    case START_POLLING: {
      CONCURRENCY_LOCK();
      data_len = phNxpNciHal_write_unlocked(cmd_poll_len, cmd_poll);
      CONCURRENCY_UNLOCK();

      if (data_len != cmd_poll_len) {
        NXPLOG_NCIHAL_E("phNxpNciHal_start_polling_loop: data len mismatch");
        status = NFCSTATUS_FAILED;
      }
    } break;

    case RESUME_POLLING: {
      CONCURRENCY_LOCK();
      data_len = phNxpNciHal_write_unlocked(sizeof(cmd_resume_rf_discovery),
                                            cmd_resume_rf_discovery);
      CONCURRENCY_UNLOCK();

      if (data_len != sizeof(cmd_resume_rf_discovery)) {
        NXPLOG_NCIHAL_E("phNxpNciHal_resume_polling_loop: data len mismatch");
        status = NFCSTATUS_FAILED;
      }
    } break;

    case STOP_POLLING: {
      CONCURRENCY_LOCK();
      data_len = phNxpNciHal_write_unlocked(sizeof(cmd_stop_rf_discovery),
                                            cmd_stop_rf_discovery);
      CONCURRENCY_UNLOCK();

      if (data_len != sizeof(cmd_stop_rf_discovery)) {
        NXPLOG_NCIHAL_E("phNxpNciHal_stop_polling_loop: data len mismatch");
        status = NFCSTATUS_FAILED;
      }
    } break;

    case DISCOVER_SELECT: {
      CONCURRENCY_LOCK();
      data_len = phNxpNciHal_write_unlocked(sizeof(cmd_select_rf_discovery),
                                            cmd_select_rf_discovery);
      CONCURRENCY_UNLOCK();

      if (data_len != sizeof(cmd_resume_rf_discovery)) {
        NXPLOG_NCIHAL_E("phNxpNciHal_resume_polling_loop: data len mismatch");
        status = NFCSTATUS_FAILED;
      }
    } break;

    default:
      NXPLOG_NCIHAL_E("No Matching case");
      status = NFCSTATUS_FAILED;
      break;
  }

  NXPLOG_NCIHAL_W("tmp_thread: exit");
  return NULL;
}
/*******************************************************************************
 **
 ** Function         phNxpNciHal_select_RF_Discovery
 **
 ** Description     Sends RF_DISCOVER_SELECT_CMD
 ** Parameters    RfID ,  RfProtocolType
 ** Returns          NFCSTATUS_PENDING if success
 **
 *******************************************************************************/
NFCSTATUS phNxpNciHal_select_RF_Discovery(unsigned int RfID,
                                          unsigned int RfProtocolType) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  pthread_t pthread;
  discover_type = DISCOVER_SELECT;
  cmd_select_rf_discovery[3] = RfID;
  cmd_select_rf_discovery[4] = RfProtocolType;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (pthread_create(&pthread, &attr, tmp_thread, (void*)&discover_type) != 0) {
    NXPLOG_NCIHAL_E("fail to create pthread");
  }
  pthread_attr_destroy(&attr);
  return status;
}
/*******************************************************************************
**
** Function         phNxpNciHal_NfcDep_cmd_ext
**
** Description      Stores the polling loop configuration locally.
**
** Returns          None
**
*******************************************************************************/
void phNxpNciHal_NfcDep_cmd_ext(uint8_t* p_cmd_data, uint16_t* cmd_len) {
  if (*cmd_len < NCI_HEADER_SIZE) return;
  if (p_cmd_data[0] == 0x21 && p_cmd_data[1] == 0x03) {
    if (*cmd_len == 6 && p_cmd_data[3] == 0x01 && p_cmd_data[4] == 0x02 &&
        p_cmd_data[5] == 0x01) {
      /* DO NOTHING */
    } else {
      if (*cmd_len > MAX_POLL_CMD_LEN) {
        NXPLOG_NCIHAL_E("invalid cmd_len");
        return;
      }
      /* Store the polling loop configuration */
      cmd_poll_len = *cmd_len;
      memset(&cmd_poll, 0, cmd_poll_len);
      memcpy(&cmd_poll, p_cmd_data, cmd_poll_len);
    }
  }

  return;
}
