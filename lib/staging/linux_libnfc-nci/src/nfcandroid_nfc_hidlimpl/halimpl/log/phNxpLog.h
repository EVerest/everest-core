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

#if !defined(NXPLOG__H_INCLUDED)
#define NXPLOG__H_INCLUDED
#include <log/log.h>
#include "log.h"
#include "logging.h"
#include <stdint.h>
typedef unsigned char uint8_t;
//#define LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_EXTNS, ...) {printf("\n");printf(__VA_ARGS__);printf("\n");}
typedef struct nci_log_level {
  uint8_t global_log_level;
  uint8_t extns_log_level;
  uint8_t hal_log_level;
  uint8_t dnld_log_level;
  uint8_t tml_log_level;
  uint8_t ncix_log_level;
  uint8_t ncir_log_level;
} nci_log_level_t;

/* global log level Ref */
extern nci_log_level_t gLog_level;
extern bool nfc_debug_enabled;
void phNxpLog_LogMsg (uint32_t trace_set_mask, const char *item, const char *fmt_str, ...);
/* define log module included when compile */
#define ENABLE_EXTNS_TRACES TRUE
#define ENABLE_HAL_TRACES TRUE
#define ENABLE_TML_TRACES TRUE
#define ENABLE_FWDNLD_TRACES TRUE
#define ENABLE_NCIX_TRACES TRUE
#define ENABLE_NCIR_TRACES TRUE

#define ENABLE_API_TRACES     TRUE
#define ENABLE_HCPX_TRACES FALSE
#define ENABLE_HCPR_TRACES FALSE

/* ####################### Set the log module name in .conf file
 * ########################## */
#define NAME_NXPLOG_EXTNS_LOGLEVEL "NXPLOG_EXTNS_LOGLEVEL"
#define NAME_NXPLOG_HAL_LOGLEVEL "NXPLOG_NCIHAL_LOGLEVEL"
#define NAME_NXPLOG_NCIX_LOGLEVEL "NXPLOG_NCIX_LOGLEVEL"
#define NAME_NXPLOG_NCIR_LOGLEVEL "NXPLOG_NCIR_LOGLEVEL"
#define NAME_NXPLOG_FWDNLD_LOGLEVEL "NXPLOG_FWDNLD_LOGLEVEL"
#define NAME_NXPLOG_TML_LOGLEVEL "NXPLOG_TML_LOGLEVEL"
#define NAME_NXPLOG_GLOBAL_LOGLEVEL         "NXPLOG_GLOBAL_LOGLEVEL"
/* ####################### Set the log module name by Android property
 * ########################## */
#define PROP_NAME_NXPLOG_GLOBAL_LOGLEVEL "nfc.nxp_log_level_global"
#define PROP_NAME_NXPLOG_EXTNS_LOGLEVEL "nfc.nxp_log_level_extns"
#define PROP_NAME_NXPLOG_HAL_LOGLEVEL "nfc.nxp_log_level_hal"
#define PROP_NAME_NXPLOG_NCI_LOGLEVEL "nfc.nxp_log_level_nci"
#define PROP_NAME_NXPLOG_FWDNLD_LOGLEVEL "nfc.nxp_log_level_dnld"
#define PROP_NAME_NXPLOG_TML_LOGLEVEL "nfc.nxp_log_level_tml"

/* ####################### Set the logging level for EVERY COMPONENT here
 * ######################## :START: */
#define NXPLOG_LOG_SILENT_LOGLEVEL 0x00
#define NXPLOG_LOG_ERROR_LOGLEVEL 0x01
#define NXPLOG_LOG_WARN_LOGLEVEL 0x02
#define NXPLOG_LOG_DEBUG_LOGLEVEL 0x03
/* ####################### Set the default logging level for EVERY COMPONENT
 * here ########################## :END: */

/* The Default log level for all the modules. */
#define NXPLOG_DEFAULT_LOGLEVEL NXPLOG_LOG_SILENT_LOGLEVEL
#define PROPERTY_VALUE_MAX                      100
/* ################################################################################################################
 */
/* ############################################### Component Names
 * ################################################ */
/* ################################################################################################################
 */

extern const char* NXPLOG_ITEM_EXTNS;  /* Android logging tag for NxpExtns  */
extern const char* NXPLOG_ITEM_NCIHAL; /* Android logging tag for NxpNciHal */
extern const char* NXPLOG_ITEM_NCIX;   /* Android logging tag for NxpNciX   */
extern const char* NXPLOG_ITEM_NCIR;   /* Android logging tag for NxpNciR   */
extern const char* NXPLOG_ITEM_FWDNLD; /* Android logging tag for NxpFwDnld */
extern const char* NXPLOG_ITEM_TML;    /* Android logging tag for NxpTml    */
extern const char * NXPLOG_ITEM_NCIX;    /* Android logging tag for NxpNciX   */
extern const char * NXPLOG_ITEM_NCIR;    /* Android logging tag for NxpNciR   */
extern const char * NXPLOG_ITEM_FWDNLD;  /* Android logging tag for NxpFwDnld */
extern const char * NXPLOG_ITEM_TML;     /* Android logging tag for NxpTml    */
extern const char * NXPLOG_ITEM_API;     /* Android logging tag for NxpFunc    */


#ifdef NXP_HCI_REQ
extern const char* NXPLOG_ITEM_HCPX; /* Android logging tag for NxpHcpX   */
extern const char* NXPLOG_ITEM_HCPR; /* Android logging tag for NxpHcpR   */
#endif                               /*NXP_HCI_REQ*/

/* ######################################## Defines used for Logging data
 * ######################################### */
#ifdef NXP_VRBS_REQ
#define NXPLOG_FUNC_ENTRY(COMP) \
  LOG_PRI(ANDROID_LOG_VERBOSE, (COMP), "+:%s", (__func__))
#define NXPLOG_FUNC_EXIT(COMP) \
  LOG_PRI(ANDROID_LOG_VERBOSE, (COMP), "-:%s", (__func__))
#endif /*NXP_VRBS_REQ*/

/* ################################################################################################################
 */
/* ######################################## Logging APIs of actual modules
 * ######################################## */
/* ################################################################################################################
 */
/* Logging APIs used by NxpExtns module */
#if (ENABLE_EXTNS_TRACES == TRUE)
#define NXPLOG_EXTNS_D(...)                                       \
  {                                                               \
     if ((nfc_debug_enabled) ||                                   \
      (gLog_level.extns_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_EXTNS, __VA_ARGS__); \
  }
#define NXPLOG_EXTNS_W(...)                                      \
  {                                                              \
     if ((nfc_debug_enabled) ||                                  \
      (gLog_level.extns_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_EXTNS, __VA_ARGS__); \
  }
#define NXPLOG_EXTNS_E(...)                                       \
  {                                                               \
    if (gLog_level.extns_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)  \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_EXTNS, __VA_ARGS__); \
  }
#else
#define NXPLOG_EXTNS_D(...)
#define NXPLOG_EXTNS_W(...)
#define NXPLOG_EXTNS_E(...)
#endif /* Logging APIs used by NxpExtns module */
#if (ENABLE_API_TRACES == TRUE )
//Added nfc_debug_enabled flag  to fix logger issue
//To enable all the debug prints when this flag is set from conf file
#    define NXPLOG_API_D(...)      {if(nfc_debug_enabled) phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL,NXPLOG_ITEM_API,__VA_ARGS__); }
#    define NXPLOG_API_W(...)      {if(nfc_debug_enabled) phNxpLog_LogMsg(NXPLOG_LOG_WARN_LOGLEVEL,NXPLOG_ITEM_API,__VA_ARGS__); }
#    define NXPLOG_API_E(...)      {if(nfc_debug_enabled) phNxpLog_LogMsg(NXPLOG_LOG_ERROR_LOGLEVEL,NXPLOG_ITEM_API,__VA_ARGS__); }
#define LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_EXTNS, ...) {phNxpLog_LogMsg(NXPLOG_LOG_ERROR_LOGLEVEL,NXPLOG_ITEM_API,__VA_ARGS__); }
#else
#    define NXPLOG_API_D(...)
#    define NXPLOG_API_W(...)
#    define NXPLOG_API_E(...)
#endif
/* Logging APIs used by NxpNciHal module */
#if (ENABLE_HAL_TRACES == TRUE)
#define NXPLOG_NCIHAL_D(...)                                       \
  {                                                                \
    if ((nfc_debug_enabled) ||                                     \
      (gLog_level.hal_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))     \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_NCIHAL, __VA_ARGS__); \
  }
#define NXPLOG_NCIHAL_W(...)                                      \
  {                                                               \
    if ((nfc_debug_enabled) ||                                    \
      (gLog_level.hal_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))     \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_NCIHAL, __VA_ARGS__); \
  }
#define NXPLOG_NCIHAL_E(...)                                       \
  {                                                                \
    if ((nfc_debug_enabled) ||                                     \
      (gLog_level.hal_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL))     \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_NCIHAL, __VA_ARGS__); \
  }
#else
#define NXPLOG_NCIHAL_D(...)
#define NXPLOG_NCIHAL_W(...)
#define NXPLOG_NCIHAL_E(...)
#endif /* Logging APIs used by HAL module */

/* Logging APIs used by NxpNciX module */
#if (ENABLE_NCIX_TRACES == TRUE)
#define NXPLOG_NCIX_D(...)                                       \
  {                                                              \
    if ((nfc_debug_enabled) ||                                   \
      (gLog_level.ncix_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_NCIX, __VA_ARGS__); \
  }
#define NXPLOG_NCIX_W(...)                                      \
  {                                                             \
     if ((nfc_debug_enabled) ||                                 \
      (gLog_level.ncix_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_NCIX, __VA_ARGS__); \
  }
#define NXPLOG_NCIX_E(...)                                       \
  {                                                              \
    if (gLog_level.ncix_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)  \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_NCIX, __VA_ARGS__); \
  }
#else
#define NXPLOG_NCIX_D(...)
#define NXPLOG_NCIX_W(...)
#define NXPLOG_NCIX_E(...)
#endif /* Logging APIs used by NCIx module */

/* Logging APIs used by NxpNciR module */
#if (ENABLE_NCIR_TRACES == TRUE)
#define NXPLOG_NCIR_D(...)                                       \
  {                                                              \
     if ((nfc_debug_enabled) ||                                  \
      (gLog_level.ncir_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_NCIR, __VA_ARGS__); \
  }
#define NXPLOG_NCIR_W(...)                                      \
  {                                                             \
     if ((nfc_debug_enabled) ||                                 \
      (gLog_level.ncir_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_NCIR, __VA_ARGS__); \
  }
#define NXPLOG_NCIR_E(...)                                       \
  {                                                              \
    if (gLog_level.ncir_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)  \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_NCIR, __VA_ARGS__); \
  }
#else
#define NXPLOG_NCIR_D(...)
#define NXPLOG_NCIR_W(...)
#define NXPLOG_NCIR_E(...)
#endif /* Logging APIs used by NCIR module */

/* Logging APIs used by NxpFwDnld module */
#if (ENABLE_FWDNLD_TRACES == TRUE)
#define NXPLOG_FWDNLD_D(...)                                       \
  {                                                                \
     if ((nfc_debug_enabled) ||                                    \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_FWDNLD_W(...)                                      \
  {                                                               \
     if ((nfc_debug_enabled) ||                                   \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_FWDNLD_E(...)                                       \
  {                                                                \
    if (gLog_level.dnld_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)    \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#else
#define NXPLOG_FWDNLD_D(...)
#define NXPLOG_FWDNLD_W(...)
#define NXPLOG_FWDNLD_E(...)
#endif /* Logging APIs used by NxpFwDnld module */

/* Logging APIs used by NxpTml module */
#if (ENABLE_TML_TRACES == TRUE)
#define NXPLOG_TML_D(...)                                       \
  {                                                             \
     if ((nfc_debug_enabled) ||                                 \
      (gLog_level.tml_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_TML, __VA_ARGS__); \
  }
#define NXPLOG_TML_W(...)                                      \
  {                                                            \
     if ((nfc_debug_enabled) ||                                \
      (gLog_level.tml_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))  \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_TML, __VA_ARGS__); \
  }
#define NXPLOG_TML_E(...)                                       \
  {                                                             \
    if (gLog_level.tml_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)  \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_TML, __VA_ARGS__); \
  }
#else
#define NXPLOG_TML_D(...)
#define NXPLOG_TML_W(...)
#define NXPLOG_TML_E(...)
#endif /* Logging APIs used by NxpTml module */

#ifdef NXP_HCI_REQ
/* Logging APIs used by NxpHcpX module */
#if (ENABLE_HCPX_TRACES == TRUE)
#define NXPLOG_HCPX_D(...)                                         \
  {                                                                \
     if ((nfc_debug_enabled) ||                                    \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_HCPX_W(...)                                        \
  {                                                               \
     if ((nfc_debug_enabled) ||                                   \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_HCPX_E(...)                                         \
  {                                                                \
    if (gLog_level.dnld_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)    \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#else
#define NXPLOG_HCPX_D(...)
#define NXPLOG_HCPX_W(...)
#define NXPLOG_HCPX_E(...)
#endif /* Logging APIs used by NxpHcpX module */

/* Logging APIs used by NxpHcpR module */
#if (ENABLE_HCPR_TRACES == TRUE)
#define NXPLOG_HCPR_D(...)                                         \
  {                                                                \
     if ((nfc_debug_enabled) ||                                    \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_DEBUG_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_DEBUG, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_HCPR_W(...)                                        \
  {                                                               \
     if ((nfc_debug_enabled) ||                                   \
      (gLog_level.dnld_log_level >= NXPLOG_LOG_WARN_LOGLEVEL))    \
      LOG_PRI(ANDROID_LOG_WARN, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#define NXPLOG_HCPR_E(...)                                         \
  {                                                                \
    if (gLog_level.dnld_log_level >= NXPLOG_LOG_ERROR_LOGLEVEL)    \
      LOG_PRI(ANDROID_LOG_ERROR, NXPLOG_ITEM_FWDNLD, __VA_ARGS__); \
  }
#else
#define NXPLOG_HCPR_D(...)
#define NXPLOG_HCPR_W(...)
#define NXPLOG_HCPR_E(...)
#endif /* Logging APIs used by NxpHcpR module */
#endif /* NXP_HCI_REQ */

#ifdef NXP_VRBS_REQ
#if (ENABLE_EXTNS_TRACES == TRUE)
#define NXPLOG_EXTNS_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_EXTNS)
#define NXPLOG_EXTNS_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_EXTNS)
#else
#define NXPLOG_EXTNS_ENTRY()
#define NXPLOG_EXTNS_EXIT()
#endif

#if (ENABLE_HAL_TRACES == TRUE)
#define NXPLOG_NCIHAL_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_NCIHAL)
#define NXPLOG_NCIHAL_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_NCIHAL)
#else
#define NXPLOG_NCIHAL_ENTRY()
#define NXPLOG_NCIHAL_EXIT()
#endif

#if (ENABLE_NCIX_TRACES == TRUE)
#define NXPLOG_NCIX_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_NCIX)
#define NXPLOG_NCIX_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_NCIX)
#else
#define NXPLOG_NCIX_ENTRY()
#define NXPLOG_NCIX_EXIT()
#endif

#if (ENABLE_NCIR_TRACES == TRUE)
#define NXPLOG_NCIR_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_NCIR)
#define NXPLOG_NCIR_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_NCIR)
#else
#define NXPLOG_NCIR_ENTRY()
#define NXPLOG_NCIR_EXIT()
#endif

#ifdef NXP_HCI_REQ

#if (ENABLE_HCPX_TRACES == TRUE)
#define NXPLOG_HCPX_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_HCPX)
#define NXPLOG_HCPX_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_HCPX)
#else
#define NXPLOG_HCPX_ENTRY()
#define NXPLOG_HCPX_EXIT()
#endif

#if (ENABLE_HCPR_TRACES == TRUE)
#define NXPLOG_HCPR_ENTRY() NXPLOG_FUNC_ENTRY(NXPLOG_ITEM_HCPR)
#define NXPLOG_HCPR_EXIT() NXPLOG_FUNC_EXIT(NXPLOG_ITEM_HCPR)
#else
#define NXPLOG_HCPR_ENTRY()
#define NXPLOG_HCPR_EXIT()
#endif
#endif /* NXP_HCI_REQ */

#endif /* NXP_VRBS_REQ */

void phNxpLog_InitializeLogLevel(void);

#endif /* NXPLOG__H_INCLUDED */
