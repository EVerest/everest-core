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
#ifndef _PHNXPNCIHAL_NFCDEPSWPRIO_H_
#define _PHNXPNCIHAL_NFCDEPSWPRIO_H_

#include <phNxpNciHal.h>
#include <phTmlNfc.h>
#include <string.h>

#define START_POLLING 0x00
#define RESUME_POLLING 0x01
#define STOP_POLLING 0x02
#define DISCOVER_SELECT 0x03
#define CLEAR_PIPE_RSP 0x04

extern uint8_t EnableP2P_PrioLogic;

extern NFCSTATUS phNxpNciHal_NfcDep_rsp_ext(uint8_t* p_ntf, uint16_t* p_len);
extern void phNxpNciHal_NfcDep_cmd_ext(uint8_t* p_cmd_data, uint16_t* cmd_len);
extern NFCSTATUS phNxpNciHal_NfcDep_comapre_ntf(uint8_t* p_cmd_data,
                                                uint16_t cmd_len);
extern NFCSTATUS phNxpNciHal_select_RF_Discovery(unsigned int RfID,
                                                 unsigned int RfProtocolType);
extern NFCSTATUS phNxpNciHal_clean_P2P_Prio();
extern NFCSTATUS phNxpNciHal_send_clear_pipe_rsp(void);

#endif /* _PHNXPNCIHAL_NFCDEPSWPRIO_H_ */
