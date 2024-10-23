/******************************************************************************
 *
 *  Copyright (C) 2012 Broadcom Corporation
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
 *  Copyright (C) 2013-2014 NXP Semiconductors
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
/******************************************************************************
 *
 *  Override the Android logging macro(s) from
 *  /system/core/include/cutils/log.h. This header must be the first header
 *  included by a *.cpp file so the original Android macro can be replaced.
 *  Do not include this header in another header, because that will create
 *  unnecessary dependency.
 *
 ******************************************************************************/
#pragma once

#include "bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif


extern unsigned char appl_trace_level;
#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
extern unsigned char appl_dta_mode_flag; //defined for run time DTA mode selection
#endif


/*******************************************************************************
**
** Function:        initializeGlobalAppLogLevel
**
** Description:     Initialize and get global logging level from .conf or
**                  Android property nfc.app_log_level.  The Android property
**                  overrides .conf variable.
**
** Returns:         Global log level:
**                  BT_TRACE_LEVEL_NONE    0        * No trace messages to be generated
**                  BT_TRACE_LEVEL_ERROR   1        * Error condition trace messages
**                  BT_TRACE_LEVEL_WARNING 2        * Warning condition trace messages
**                  BT_TRACE_LEVEL_API     3        * API traces
**                  BT_TRACE_LEVEL_EVENT   4        * Debug messages for events
**                  BT_TRACE_LEVEL_DEBUG   5        * Debug messages (general)
**
*******************************************************************************/
unsigned char initializeGlobalAppLogLevel ();


#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
/*******************************************************************************
**
** Function:        initializeGlobalDtaMode
**
** Description:     Initialize and get global DTA mode from .conf
**
** Returns:         none:
**
*******************************************************************************/
void initializeGlobalAppDtaMode ();
#endif
#ifdef __cplusplus
}
#endif
