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

#ifndef _PHNXPNCIHAL_DTA_H_
#define _PHNXPNCIHAL_DTA_H_

#include <phNxpNciHal_utils.h>
/* DTA Control structure */
typedef struct phNxpDta_Control {
  uint8_t dta_ctrl_flag;
  uint16_t dta_pattern_no;
  uint8_t dta_t1t_flag;
} phNxpDta_Control_t;

void phNxpEnable_DtaMode(uint16_t pattern_no);
void phNxpDisable_DtaMode(void);
NFCSTATUS phNxpDta_IsEnable(void);
void phNxpDta_T1TEnable(void);
NFCSTATUS phNxpNHal_DtaUpdate(uint16_t* cmd_len, uint8_t* p_cmd_data,
                              uint16_t* rsp_len, uint8_t* p_rsp_data);

#endif /* _PHNXPNICHAL_DTA_H_ */
