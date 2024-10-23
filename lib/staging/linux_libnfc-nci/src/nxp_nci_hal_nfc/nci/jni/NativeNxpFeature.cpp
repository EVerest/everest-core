/*
 * Copyright (C) 2012 The Android Open Source Project
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
/******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright 2015-2021 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <errno.h>
#include <semaphore.h>
#include "JavaClassConstants.h"
#include "NfcAdaptation.h"
#include "NfcJniUtil.h"
#include "RoutingManager.h"
#include "SyncEvent.h"
#include "config.h"
#include "nfc_config.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"

#if (NXP_EXTNS == TRUE)
using android::base::StringPrintf;

extern bool nfc_debug_enabled;

typedef struct nxp_feature_data {
  SyncEvent NxpFeatureConfigEvt;
  Mutex mMutex;
  tNFA_STATUS wstatus;
  uint8_t rsp_data[255];
  uint8_t rsp_len;
} Nxp_Feature_Data_t;

typedef enum {
  NCI_OID_SYSTEM_DEBUG_STATE_L1_MESSAGE = 0x35,
  NCI_OID_SYSTEM_DEBUG_STATE_L2_MESSAGE,
  NCI_OID_SYSTEM_DEBUG_STATE_L3_MESSAGE,
} eNciSystemPropOpcodeIdentifier_t;

namespace android {
extern nfc_jni_native_data* getNative(JNIEnv* e, jobject o);
static Nxp_Feature_Data_t gnxpfeature_conf;
void SetCbStatus(tNFA_STATUS status);
tNFA_STATUS GetCbStatus(void);
static void NxpResponse_Cb(uint8_t event, uint16_t param_len, uint8_t* p_param);
}  // namespace android

namespace android {
extern bool suppressLogs;
void SetCbStatus(tNFA_STATUS status) { gnxpfeature_conf.wstatus = status; }

tNFA_STATUS GetCbStatus(void) { return gnxpfeature_conf.wstatus; }

void NxpPropCmd_OnResponseCallback(uint8_t event, uint16_t param_len,
                                   uint8_t *p_param) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
    "NxpPropCmd_OnResponseCallback: Received length data = 0x%x status = "
        "0x%x", param_len, p_param[3]);
  uint8_t oid = p_param[1];
  uint8_t status = NFA_STATUS_FAILED;

  switch (oid) {
  case (0x03):
  /*FALL_THRU*/
  case (0x1A):
  /*FALL_THRU*/
  case (0x1C):
    status = p_param[3];
    break;
  case (0x1B):
    status = p_param[param_len - 1];
    break;
  default:
    LOG(ERROR) << StringPrintf("Propreitary Rsp: OID is not supported");
    break;
  }

  android::SetCbStatus(status);

  android::gnxpfeature_conf.rsp_len = (uint8_t)param_len;
  memcpy(android::gnxpfeature_conf.rsp_data, p_param, param_len);
  SyncEventGuard guard(android::gnxpfeature_conf.NxpFeatureConfigEvt);
  android::gnxpfeature_conf.NxpFeatureConfigEvt.notifyOne();
}

tNFA_STATUS NxpPropCmd_send(uint8_t *pData4Tx, uint8_t dataLen,
                            uint8_t *rsp_len, uint8_t *rsp_buf,
                            uint32_t rspTimeout, tHAL_NFC_ENTRY *halMgr) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  bool retVal = false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: prop cmd being txed", __func__);

  gnxpfeature_conf.mMutex.lock();

  android::SetCbStatus(NFA_STATUS_FAILED);
  SyncEventGuard guard(android::gnxpfeature_conf.NxpFeatureConfigEvt);

  status =
      NFA_SendRawVsCommand(dataLen, pData4Tx, NxpPropCmd_OnResponseCallback);
  if (status == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Success NFA_SendNxpNciCommand", __func__);

    retVal = android::gnxpfeature_conf.NxpFeatureConfigEvt.wait(
        rspTimeout); /* wait for callback */
    if (retVal == false) {
      android::SetCbStatus(NFA_STATUS_TIMEOUT);
      android::gnxpfeature_conf.rsp_len = 0;
      memset(android::gnxpfeature_conf.rsp_data, 0,
             sizeof(android::gnxpfeature_conf.rsp_data));
    }
  } else {
    LOG(ERROR) << StringPrintf("%s: Failed NFA_SendNxpNciCommand", __func__);
  }
  status = android::GetCbStatus();
  if ((android::gnxpfeature_conf.rsp_len > 3) && (rsp_buf != NULL)) {
    *rsp_len = android::gnxpfeature_conf.rsp_len - 3;
    memcpy(rsp_buf, android::gnxpfeature_conf.rsp_data + 3,
           android::gnxpfeature_conf.rsp_len - 3);
  }
  android::gnxpfeature_conf.mMutex.unlock();
  return status;
}

static void NxpResponse_Cb(uint8_t event, uint16_t param_len,
                           uint8_t* p_param) {
  (void)event;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "NxpResponse_Cb Received length data = 0x%x status = 0x%x", param_len,
      p_param[3]);
  if (p_param != NULL) {
    if (p_param[3] == 0x00) {
      SetCbStatus(NFA_STATUS_OK);
    } else {
      SetCbStatus(NFA_STATUS_FAILED);
    }
    gnxpfeature_conf.rsp_len = (uint8_t)param_len;
    if (param_len > 0) {
      memcpy(gnxpfeature_conf.rsp_data, p_param, param_len);
    }
    SyncEventGuard guard(gnxpfeature_conf.NxpFeatureConfigEvt);
    gnxpfeature_conf.NxpFeatureConfigEvt.notifyOne();
  }
}


/*******************************************************************************
 **
 ** Function:        NxpNfc_Write_Cmd()
 **
 ** Description:     Writes the command to NFCC
 **
 ** Returns:         success/failure
 **
 *******************************************************************************/
tNFA_STATUS NxpNfc_Write_Cmd_Common(uint8_t retlen, uint8_t* buffer) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  SetCbStatus(NFA_STATUS_FAILED);
  SyncEventGuard guard(gnxpfeature_conf.NxpFeatureConfigEvt);
  status = NFA_SendRawVsCommand(retlen, buffer, NxpResponse_Cb);
  if (status == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Success NFA_SendRawVsCommand", __func__);
    gnxpfeature_conf.NxpFeatureConfigEvt.wait(); /* wait for callback */
  } else {
    LOG(ERROR) << StringPrintf("%s: Failed NFA_SendRawVsCommand", __func__);
  }
  status = GetCbStatus();
  return status;
}
/*******************************************************************************
 **
 ** Function:        getNumValue()
 **
 ** Description:     get the value from th config file.
 **
 ** Returns:         success/failure
 **
 *******************************************************************************/
uint32_t getNumValue(const char* key ,uint32_t value) {
  return NfcConfig::getUnsigned(key, value);
}

/*******************************************************************************
 **
 ** Function:        send_flush_ram_to_flash
 **
 ** Description:     This is used to update ram to flash command to NFCC.
 **                  This will write the contents of RAM to FLASH.This will
 **                  be sent only one time after NFC init.
 **
 ** Returns:         NFA_STATUS_OK on success
 **www
 *******************************************************************************/
tNFA_STATUS send_flush_ram_to_flash() {
  DLOG_IF(INFO, nfc_debug_enabled)
    << StringPrintf("%s: enter", __func__);
  tNFA_STATUS status = NFA_STATUS_OK;
  uint8_t  cmd[] = {0x2F, 0x21, 0x00};

  status = NxpNfc_Write_Cmd_Common(sizeof(cmd), cmd);
  if(status != NFA_STATUS_OK) {
    DLOG_IF(ERROR, nfc_debug_enabled)
      << StringPrintf("%s: send_flush_ram_to_flash sending status %x", __func__,status);
  }
  return status;
}
/*******************************************************************************
 **
 ** Function:        enableDisableLog(bool type)
 **
 ** Description:     This function is used to enable/disable the
 **                  logging module for cmd/data exchanges.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void enableDisableLog(bool type) {
  // static bool prev_trace_level = nfc_debug_enabled;

  // NfcAdaptation& theInstance = NfcAdaptation::GetInstance();

  // if (android::suppressLogs) {
  //   if (true == type) {
  //     if (nfc_debug_enabled != prev_trace_level) {
  //       nfc_debug_enabled = prev_trace_level;
  //       theInstance.HalSetProperty("nfc.debug_enabled", "1");
  //     }
  //   } else if (false == type) {
  //     if (0 != nfc_debug_enabled) {
  //       nfc_debug_enabled = 0;
  //       theInstance.HalSetProperty("nfc.debug_enabled", "0");
  //     }
  //   }
  // }
}

/*******************************************************************************
**
** Function:        nfaVSCNtfCallback
**
** Description:     Receives LxDebug events from stack.
**                  Event: for which the callback is invoked
**                  param_len: Len of the Parameters passed
**                  p_param: Pointer to the event param
**
** Returns:         None
**
*******************************************************************************/
void nfaVSCNtfCallback(uint8_t event, uint16_t param_len, uint8_t *p_param) {
  (void)event;
  DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: event = 0x%02X", __func__, event);
  uint8_t op_code = (event & ~NCI_NTF_BIT);
  uint32_t len;
  uint8_t nciHdrLen = 3;

  if(!p_param || param_len <= nciHdrLen) {
    LOG(ERROR) << "Invalid Params. returning...";
    return;
  }

  switch(op_code) {
    case NCI_OID_SYSTEM_DEBUG_STATE_L1_MESSAGE:
    break;

    case NCI_OID_SYSTEM_DEBUG_STATE_L2_MESSAGE:
      len = param_len - nciHdrLen;
    {
#ifndef LINUX
      struct nfc_jni_native_data* mNativeData = getNative(NULL, NULL);
      JNIEnv* e = NULL;
      ScopedAttach attach(mNativeData->vm, &e);
      if (e == NULL) {
        LOG(ERROR) << "jni env is null";
        return;
      }

      jbyteArray retArray = e->NewByteArray(len);

      if((uint32_t)e->GetArrayLength(retArray) != len)
      {
        e->DeleteLocalRef(retArray);
        retArray = e->NewByteArray(len);
      }
      e->SetByteArrayRegion(retArray, 0, len, (jbyte*)(p_param + nciHdrLen));

      // e->CallVoidMethod(mNativeData->manager,
      //                 android::gCachedNfcManagerNotifyLxDebugInfo,
      //                 (int)len, retArray);
      if (e->ExceptionCheck()) {
        e->ExceptionClear();
        LOG(ERROR) << "fail notify";
      }
#endif
    }
    break;

    case NCI_OID_SYSTEM_DEBUG_STATE_L3_MESSAGE:
    break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event ????", __func__);
    break;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit", __func__);
}


} /*namespace android*/

#endif
