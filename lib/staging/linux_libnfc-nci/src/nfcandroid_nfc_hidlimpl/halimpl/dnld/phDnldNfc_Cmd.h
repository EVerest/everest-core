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

/*
 * Firmware Download command values
 */

#ifndef PHDNLDNFC_CMD_H
#define PHDNLDNFC_CMD_H

#include <phNfcStatus.h>

/*
 * Enum definition contains Firmware Download Command Ids
 */
typedef enum phDnldNfc_CmdId {
  PH_DL_CMD_NONE = 0x00,           /* Invalid Cmd */
  PH_DL_CMD_RESET = 0xF0,          /* Reset */
  PH_DL_CMD_GETVERSION = 0xF1,     /* Get Version */
  PH_DL_CMD_CHECKINTEGRITY = 0xE0, /* Check Integrity */
  PH_DL_CMD_WRITE = 0xC0,          /* Write */
  PH_DL_CMD_READ = 0xA2,           /* Read */
  PH_DL_CMD_LOG = 0xA7,            /* Log */
  PH_DL_CMD_FORCE = 0xD0,          /* Force */
  PH_DL_CMD_GETSESSIONSTATE = 0xF2 /* Get Session State */
} phDnldNfc_CmdId_t;

#endif /* PHDNLDNFC_CMD_H */
