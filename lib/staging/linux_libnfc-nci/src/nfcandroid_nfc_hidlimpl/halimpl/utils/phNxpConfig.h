/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
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

/******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2013-2014,2021 NXP Semiconductors
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

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

int GetNxpStrValue(const char* name, char* p_value, unsigned long len);
int GetNxpNumValue(const char* name, void* p_value, unsigned long len);
int GetNxpByteArrayValue(const char* name, char* pValue, long bufflen,
                         long* len);
void resetNxpConfig(void);
int isNxpConfigModified();
int updateNxpConfigTimestamp();

int isNxpConfigValid(unsigned long type);
#ifdef __cplusplus
};
#endif
#define NAME_TxDO_MEASUREDRANGE_MIN "TxDO_MEASUREDRANGE_MIN"
#define NAME_TxDO_MEASUREDRANGE_MAX                  "TxDO_MEASUREDRANGE_MAX"
#define NAME_TxDO_MEASUREDRANGE_TOLERANCE            "TxDO_MEASUREDRANGE_TOLERANCE"
#define NAME_AGC_VALUE                               "AGC_VALUE"
#define NAME_AGC_VALUE_TOLERANCE                     "AGC_VALUE_TOLERANCE"
/* Agc Values with fixed NFCLD Level*/
#define NAME_AGC_VALUE_WITH_FIXED_NFCLD              "AGC_VALUE_WITH_FIXED_NFCLD"
#define NAME_AGC_VALUE_WITH_FIXED_NFCLD_TOLERANCE    "AGC_VALUE_WITH_FIXED_NFCLD_TOLERANCE"
/* Agc Differential with Open Open/Short RM */
#define NAME_AGC_DIFFERENTIAL_WITH_OPEN_1            "AGC_DIFFERENTIAL_WITH_OPEN_1"
#define NAME_AGC_DIFFERENTIAL_WITH_OPEN_TOLERANCE_1  "AGC_DIFFERENTIAL_WITH_OPEN_TOLERANCE_1"
#define NAME_AGC_DIFFERENTIAL_WITH_OPEN_2            "AGC_DIFFERENTIAL_WITH_OPEN_2"
#define NAME_AGC_DIFFERENTIAL_WITH_OPEN_TOLERANCE_2  "AGC_DIFFERENTIAL_WITH_OPEN_TOLERANCE_2"


#define NAME_NXPLOG_EXTNS_LOGLEVEL "NXPLOG_EXTNS_LOGLEVEL"
#define NAME_NXPLOG_NCIHAL_LOGLEVEL "NXPLOG_NCIHAL_LOGLEVEL"
#define NAME_NXPLOG_NCIX_LOGLEVEL "NXPLOG_NCIX_LOGLEVEL"
#define NAME_NXPLOG_NCIR_LOGLEVEL "NXPLOG_NCIR_LOGLEVEL"
#define NAME_NXPLOG_FWDNLD_LOGLEVEL "NXPLOG_FWDNLD_LOGLEVEL"
#define NAME_NXPLOG_TML_LOGLEVEL "NXPLOG_TML_LOGLEVEL"

#define NXP_CONFIG_TYPE_INIT                   0x0U
#define NXP_CONFIG_TYPE_PN547                  0x1U
#define NXP_CONFIG_TYPE_PN548                  0x2U
#define NXP_CONFIG_TYPE_PN557                  0x3U

#define NAME_NXPLOG_EXTNS_LOGLEVEL             "NXPLOG_EXTNS_LOGLEVEL"
#define NAME_NXPLOG_NCIHAL_LOGLEVEL            "NXPLOG_NCIHAL_LOGLEVEL"
#define NAME_NXPLOG_NCIX_LOGLEVEL              "NXPLOG_NCIX_LOGLEVEL"
#define NAME_NXPLOG_NCIR_LOGLEVEL              "NXPLOG_NCIR_LOGLEVEL"
#define NAME_NXPLOG_FWDNLD_LOGLEVEL            "NXPLOG_FWDNLD_LOGLEVEL"
#define NAME_NXPLOG_TML_LOGLEVEL               "NXPLOG_TML_LOGLEVEL"

#define NAME_MIFARE_READER_ENABLE              "MIFARE_READER_ENABLE"
#define NAME_NXP_NFC_CHIP                      "NXP_NFC_CHIP"
#define NAME_NXP_NFC_DEV_NODE                  "NXP_NFC_DEV_NODE"
#define NAME_NXP_FW_PATH                       "NXP_NFC_FW_PATH"
#define NAME_NXP_FW_NAME                       "NXP_NFC_FW_NAME"
#define NAME_NXP_FW_PROTECION_OVERRIDE         "NXP_FW_PROTECION_OVERRIDE"
#define NAME_NXP_SYS_CLK_SRC_SEL               "NXP_SYS_CLK_SRC_SEL"
#define NAME_NXP_SYS_CLK_FREQ_SEL              "NXP_SYS_CLK_FREQ_SEL"
#define NAME_NXP_SYS_CLOCK_TO_CFG              "NXP_SYS_CLOCK_TO_CFG"
#define NAME_NXP_ACT_PROP_EXTN                 "NXP_ACT_PROP_EXTN"
#define NAME_NXP_EXT_TVDD_CFG                  "NXP_EXT_TVDD_CFG"
#define NAME_NXP_EXT_TVDD_CFG_1                "NXP_EXT_TVDD_CFG_1"
#define NAME_NXP_EXT_TVDD_CFG_2                "NXP_EXT_TVDD_CFG_2"
#define NAME_NXP_EXT_TVDD_CFG_3                "NXP_EXT_TVDD_CFG_3"
#define NAME_NXP_RF_CONF_BLK_1                 "NXP_RF_CONF_BLK_1"
#define NAME_NXP_RF_CONF_BLK_2                 "NXP_RF_CONF_BLK_2"
#define NAME_NXP_RF_CONF_BLK_3                 "NXP_RF_CONF_BLK_3"
#define NAME_NXP_RF_CONF_BLK_4                 "NXP_RF_CONF_BLK_4"
#define NAME_NXP_RF_CONF_BLK_5                 "NXP_RF_CONF_BLK_5"
#define NAME_NXP_RF_CONF_BLK_6                 "NXP_RF_CONF_BLK_6"
#define NAME_NXP_CORE_CONF_EXTN                "NXP_CORE_CONF_EXTN"
#define NAME_NXP_CORE_CONF                     "NXP_CORE_CONF"
#define NAME_NXP_CORE_MFCKEY_SETTING           "NXP_CORE_MFCKEY_SETTING"
#define NAME_NXP_CORE_STANDBY                  "NXP_CORE_STANDBY"
#define NAME_NXP_NFC_PROFILE_EXTN              "NXP_NFC_PROFILE_EXTN"
#define NAME_NXP_CHINA_TIANJIN_RF_ENABLED      "NXP_CHINA_TIANJIN_RF_ENABLED"
#define NAME_NXP_SWP_SWITCH_TIMEOUT            "NXP_SWP_SWITCH_TIMEOUT"
#define NAME_NXP_SWP_FULL_PWR_ON               "NXP_SWP_FULL_PWR_ON"
#define NAME_NXP_CORE_RF_FIELD                 "NXP_CORE_RF_FIELD"
#define NAME_NXP_NFC_MERGE_RF_PARAMS           "NXP_NFC_MERGE_RF_PARAMS"
#define NAME_NXP_I2C_FRAGMENTATION_ENABLED     "NXP_I2C_FRAGMENTATION_ENABLED"
#define NAME_NXP_NFC_PROPRIETARY_CFG           "NXP_NFC_PROPRIETARY_CFG"
#define NAME_NXP_NFC_MAX_EE_SUPPORTED          "NXP_NFC_MAX_EE_SUPPORTED"
#define NAME_AID_MATCHING_PLATFORM             "AID_MATCHING_PLATFORM"

#define NAME_TxDO_RAWVALUE                           "TxDO_RAWVALUE"

#define NAME_MIFARE_READER_ENABLE "MIFARE_READER_ENABLE"
#define NAME_LEGACY_MIFARE_READER "LEGACY_MIFARE_READER"
#define NAME_FW_STORAGE "FW_STORAGE"
#define NAME_NXP_NFC_DEV_NODE "NXP_NFC_DEV_NODE"
#define NAME_NXP_FW_TYPE "NXP_FW_TYPE"
#define NAME_NXP_FW_PROTECION_OVERRIDE "NXP_FW_PROTECION_OVERRIDE"
#define NAME_NXP_SYS_CLK_SRC_SEL "NXP_SYS_CLK_SRC_SEL"
#define NAME_NXP_SYS_CLK_FREQ_SEL "NXP_SYS_CLK_FREQ_SEL"
#define NAME_NXP_SYS_CLOCK_TO_CFG "NXP_SYS_CLOCK_TO_CFG"
#define NAME_NXP_ACT_PROP_EXTN "NXP_ACT_PROP_EXTN"
#define NAME_NXP_EXT_TVDD_CFG "NXP_EXT_TVDD_CFG"
#define NAME_NXP_EXT_TVDD_CFG_1 "NXP_EXT_TVDD_CFG_1"
#define NAME_NXP_EXT_TVDD_CFG_2 "NXP_EXT_TVDD_CFG_2"
#define NAME_NXP_EXT_TVDD_CFG_3 "NXP_EXT_TVDD_CFG_3"
#define NAME_NXP_RF_CONF_BLK_1 "NXP_RF_CONF_BLK_1"
#define NAME_NXP_RF_CONF_BLK_2 "NXP_RF_CONF_BLK_2"
#define NAME_NXP_RF_CONF_BLK_3 "NXP_RF_CONF_BLK_3"
#define NAME_NXP_RF_CONF_BLK_4 "NXP_RF_CONF_BLK_4"
#define NAME_NXP_RF_CONF_BLK_5 "NXP_RF_CONF_BLK_5"
#define NAME_NXP_RF_CONF_BLK_6 "NXP_RF_CONF_BLK_6"
#define NAME_NXP_CORE_CONF_EXTN "NXP_CORE_CONF_EXTN"
#define NAME_NXP_CORE_CONF "NXP_CORE_CONF"
#define NAME_NXP_NFC_PROFILE_EXTN "NXP_NFC_PROFILE_EXTN"
#define NAME_NXP_CHINA_TIANJIN_RF_ENABLED "NXP_CHINA_TIANJIN_RF_ENABLED"
#define NAME_NXP_SWP_SWITCH_TIMEOUT "NXP_SWP_SWITCH_TIMEOUT"
#define NAME_NXP_SWP_FULL_PWR_ON "NXP_SWP_FULL_PWR_ON"
#define NAME_NXP_CORE_RF_FIELD "NXP_CORE_RF_FIELD"
#define NAME_NXP_I2C_FRAGMENTATION_ENABLED "NXP_I2C_FRAGMENTATION_ENABLED"
#define NAME_ISO_DEP_MAX_TRANSCEIVE "ISO_DEP_MAX_TRANSCEIVE"
#define NAME_AID_MATCHING_PLATFORM "AID_MATCHING_PLATFORM"
#define NAME_NFC_DEBUG_ENABLED "NFC_DEBUG_ENABLED"
#define NAME_RF_STATUS_UPDATE_ENABLE "RF_STATUS_UPDATE_ENABLE"
#define NAME_DEFAULT_ROUTE "DEFAULT_ROUTE"
#define NAME_DEFAULT_OFFHOST_ROUTE "DEFAULT_OFFHOST_ROUTE"
#define NAME_DEFAULT_NFCF_ROUTE "DEFAULT_NFCF_ROUTE"
#define NAME_DEFAULT_SYS_CODE_ROUTE "DEFAULT_SYS_CODE_ROUTE"
#define NAME_DEFAULT_SYS_CODE_PWR_STATE "DEFAULT_SYS_CODE_PWR_STATE"
#define NAME_DEVICE_HOST_WHITE_LIST "DEVICE_HOST_WHITE_LIST"
#define NAME_OFF_HOST_ESE_PIPE_ID "OFF_HOST_ESE_PIPE_ID"
#define NAME_OFF_HOST_SIM_PIPE_ID "OFF_HOST_SIM_PIPE_ID"
#define NAME_NFA_PROPRIETARY_CFG "NFA_PROPRIETARY_CFG"
#define NAME_PRESENCE_CHECK_ALGORITHM "PRESENCE_CHECK_ALGORITHM"
#define NAME_OFFHOST_ROUTE_ESE "OFFHOST_ROUTE_ESE"
#define NAME_OFFHOST_ROUTE_UICC "OFFHOST_ROUTE_UICC"
#define NAME_DEFAULT_ISODEP_ROUTE "DEFAULT_ISODEP_ROUTE"
#if(NXP_EXTNS == TRUE)
#define NAME_NXP_TRANSPORT "NXP_TRANSPORT"
#define NAME_NXP_AGC_DEBUG_ENABLE "NXP_AGC_DEBUG_ENABLE"
#define NAME_NXP_T4T_NFCEE_ENABLE "NXP_T4T_NFCEE_ENABLE"
#define NAME_DEFAULT_T4TNFCEE_AID_POWER_STATE "DEFAULT_T4TNFCEE_AID_POWER_STATE"
#endif

/* default configuration */
#define default_storage_location "/data/vendor/nfc"

#endif
