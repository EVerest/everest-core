/*
 * Copyright (C) 2010-2014 NXP Semiconductors
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
#define LOG_TAG "NxpNfcHal"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#if !defined(NXPLOG__H_INCLUDED)
#include "phNxpConfig.h"
#include "phNxpLog.h"
#endif
//#include <cutils/properties.h>
#include <log/log.h>
#define BTE_LOG_BUF_SIZE 1024
#define BTE_LOG_MAX_SIZE (BTE_LOG_BUF_SIZE - 12)
static pthread_mutex_t cs_mutex;
const char* NXPLOG_ITEM_EXTNS = "NxpExtns";
const char* NXPLOG_ITEM_NCIHAL = "NxpHal";
const char* NXPLOG_ITEM_NCIX = "NxpNciX";
const char* NXPLOG_ITEM_NCIR = "NxpNciR";
const char* NXPLOG_ITEM_FWDNLD = "NxpFwDnld";
const char* NXPLOG_ITEM_TML = "NxpTml";
#ifdef LINUX
//Review comment from Jeremy fixed, "NxpFunc" need to be removed
//since it appears in all the function calls
const char * NXPLOG_ITEM_API     = " ";
#else
const char * NXPLOG_ITEM_API     = "NxpFunc:    ";
#endif


#ifdef NXP_HCI_REQ
const char* NXPLOG_ITEM_HCPX = "NxpHcpX";
const char* NXPLOG_ITEM_HCPR = "NxpHcpR";
#endif /*NXP_HCI_REQ*/

/* global log level structure */
nci_log_level_t gLog_level;

extern bool nfc_debug_enabled;

/*******************************************************************************
 *
 * Function         phNxpLog_SetGlobalLogLevel
 *
 * Description      Sets the global log level for all modules.
 *                  This value is set by Android property
 *nfc.nxp_log_level_global.
 *                  If value can be overridden by module log level.
 *
 * Returns          The value of global log level
 *
 ******************************************************************************/
static uint8_t phNxpLog_SetGlobalLogLevel(void) {
  unsigned long level = NXPLOG_DEFAULT_LOGLEVEL;
  unsigned long num = 0;

#if 0
  int len = property_get(prop_name_nxplog_global_loglevel, valuestr, "");
  if (len > 0) {
    /* let android property override .conf variable */
    sscanf(valuestr, "%lu", &num);
    level = (unsigned char)num;
  }
  memset(&glog_level, level, sizeof(nci_log_level_t));
#endif
  gLog_level.global_log_level = level;
  if (GetNxpNumValue (NAME_NXPLOG_GLOBAL_LOGLEVEL, &num, sizeof(num)))
  {
      gLog_level.global_log_level = (level > (unsigned char) num) ? level : (unsigned char) num;
  }
  else if(nfc_debug_enabled)
  {
      gLog_level.global_log_level = NXPLOG_LOG_DEBUG_LOGLEVEL;
  }
  return level;
}

/*******************************************************************************
 *
 * function         phnxplog_sethalloglevel
 *
 * description      sets the hal layer log level.
 *
 * returns          void
 *
 ******************************************************************************/
static void phNxpLog_SetHALLogLevel(uint8_t level) {
  unsigned long num = 0;
  int len;
#if 0
  char valueStr[PROPERTY_VALUE_MAX] = {0};

  if (GetNxpNumValue(NAME_NXPLOG_HAL_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.hal_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }

  len = property_get(PROP_NAME_NXPLOG_HAL_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.hal_log_level = (unsigned char)num;
  }
#endif
  if (GetNxpNumValue (NAME_NXPLOG_HAL_LOGLEVEL, &num, sizeof(num)))
    {
        gLog_level.hal_log_level = (level > (unsigned char) num) ? level : (unsigned char) num;;
    }

}

/*******************************************************************************
 *
 * Function         phNxpLog_SetExtnsLogLevel
 *
 * Description      Sets the Extensions layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpLog_SetExtnsLogLevel(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (GetNxpNumValue(NAME_NXPLOG_EXTNS_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.extns_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }
#if 0
  len = property_get(PROP_NAME_NXPLOG_EXTNS_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.extns_log_level = (unsigned char)num;
  }
#endif
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetTmlLogLevel
 *
 * Description      Sets the Tml layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpLog_SetTmlLogLevel(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (GetNxpNumValue(NAME_NXPLOG_TML_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.tml_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }
#if 0
  len = property_get(PROP_NAME_NXPLOG_TML_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.tml_log_level = (unsigned char)num;
  }
#endif
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetDnldLogLevel
 *
 * Description      Sets the FW download layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpLog_SetDnldLogLevel(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (GetNxpNumValue(NAME_NXPLOG_FWDNLD_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.dnld_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }
#if 0
  len = property_get(PROP_NAME_NXPLOG_FWDNLD_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.dnld_log_level = (unsigned char)num;
  }
#endif
}

/*******************************************************************************
 *
 * Function         phNxpLog_SetNciTxLogLevel
 *
 * Description      Sets the NCI transaction layer log level.
 *
 * Returns          void
 *
 ******************************************************************************/
static void phNxpLog_SetNciTxLogLevel(uint8_t level) {
  unsigned long num = 0;
  int len;
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  if (GetNxpNumValue(NAME_NXPLOG_NCIX_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.ncix_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
  }
  if (GetNxpNumValue(NAME_NXPLOG_NCIR_LOGLEVEL, &num, sizeof(num))) {
    gLog_level.ncir_log_level =
        (level > (unsigned char)num) ? level : (unsigned char)num;
    ;
  }
#if 0
  len = property_get(PROP_NAME_NXPLOG_NCI_LOGLEVEL, valueStr, "");
  if (len > 0) {
    /* let Android property override .conf variable */
    sscanf(valueStr, "%lu", &num);
    gLog_level.ncix_log_level = (unsigned char)num;
    gLog_level.ncir_log_level = (unsigned char)num;
  }
#endif
}

/******************************************************************************
 * Function         phNxpLog_InitializeLogLevel
 *
 * Description      Initialize and get log level of module from libnfc-nxp.conf
 *or
 *                  Android runtime properties.
 *                  The Android property nfc.nxp_global_log_level is to
 *                  define log level for all modules. Modules log level will
 *overwide global level.
 *                  The Android property will overwide the level
 *                  in libnfc-nxp.conf
 *
 *                  Android property names:
 *                      nfc.nxp_log_level_global    * defines log level for all
 *modules
 *                      nfc.nxp_log_level_extns     * extensions module log
 *                      nfc.nxp_log_level_hal       * Hal module log
 *                      nfc.nxp_log_level_dnld      * firmware download module
 *log
 *                      nfc.nxp_log_level_tml       * TML module log
 *                      nfc.nxp_log_level_nci       * NCI transaction log
 *
 *                  Log Level values:
 *                      NXPLOG_LOG_SILENT_LOGLEVEL  0        * No trace to show
 *                      NXPLOG_LOG_ERROR_LOGLEVEL   1        * Show Error trace
 *only
 *                      NXPLOG_LOG_WARN_LOGLEVEL    2        * Show Warning
 *trace and Error trace
 *                      NXPLOG_LOG_DEBUG_LOGLEVEL   3        * Show all traces
 *
 * Returns          void
 *
 ******************************************************************************/
void phNxpLog_InitializeLogLevel(void) {
  uint8_t level = phNxpLog_SetGlobalLogLevel();

  phNxpLog_SetHALLogLevel(level);
  phNxpLog_SetExtnsLogLevel(level);
  phNxpLog_SetTmlLogLevel(level);
  phNxpLog_SetDnldLogLevel(level);
  phNxpLog_SetNciTxLogLevel(level);

  NXPLOG_API_D ("%s: global =%u, Fwdnld =%u, extns =%u, \
                    hal =%u, tml =%u, ncir =%u, \
                    ncix =%u", \
                    __FUNCTION__, gLog_level.global_log_level, gLog_level.dnld_log_level,
                        gLog_level.extns_log_level, gLog_level.hal_log_level, gLog_level.tml_log_level,
                        gLog_level.ncir_log_level, gLog_level.ncix_log_level);

}

void phNxpLog_LogMsg (uint32_t trace_set_mask, const char *item, const char *fmt_str, ...)
{
    static char buffer [BTE_LOG_BUF_SIZE];
    va_list ap;
    uint32_t trace_type = trace_set_mask & 0x07; //lower 3 bits contain trace type
    time_t timer;
    char buf[26];
    int millisec;
    struct tm* tm_info;
    struct timeval tv;
    int i;

    pthread_mutex_lock( &cs_mutex );

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    strftime(buf, 26, "%Y:%m:%d-%H:%M:%S", tm_info);
    fprintf (stderr, "%s.%03d\t", buf, millisec);

    fprintf (stderr, "%s", item);

    va_start (ap, fmt_str);
    i = vsnprintf (buffer, BTE_LOG_BUF_SIZE, fmt_str, ap);
    if(buffer[i-1] == '\n') buffer[i-1] = '\0';
    vfprintf (stderr, buffer, ap);
    va_end (ap);

    fprintf (stderr, "\n");

    pthread_mutex_unlock( &cs_mutex );
}

