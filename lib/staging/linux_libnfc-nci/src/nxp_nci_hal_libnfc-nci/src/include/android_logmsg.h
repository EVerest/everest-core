/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
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
 * Decode NFC packets and print them to ADB log.
 * If protocol decoder is not present, then decode packets into hex numbers.
 ******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif


#include "data_types.h"


#define DISP_NCI    ProtoDispAdapterDisplayNciPacket
void ProtoDispAdapterDisplayNciPacket (UINT8* nciPacket, UINT16 nciPacketLen, BOOLEAN is_recv);
void ProtoDispAdapterUseRawOutput (BOOLEAN isUseRaw);
void ScrLog (UINT32 trace_set_mask, const char* fmt_str, ...);
void LogMsg (UINT32 trace_set_mask, const char *fmt_str, ...);
void LogMsg_0 (UINT32 trace_set_mask, const char *p_str);
void LogMsg_1 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1);
void LogMsg_2 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2);
void LogMsg_3 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3);
void LogMsg_4 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4);
void LogMsg_5 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5);
void LogMsg_6 (UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5, UINT32 p6);
UINT8* scru_dump_hex (UINT8* p, char* pTitle, UINT32 len, UINT32 layer, UINT32 type);
void BTDISP_LOCK_LOG();
void BTDISP_UNLOCK_LOG();
void BTDISP_INIT_LOCK();
void BTDISP_UNINIT_LOCK();
void DispHciCmd (BT_HDR* p_buf);
void DispHciEvt (BT_HDR* p_buf);
void DispLLCP (BT_HDR *p_buf, BOOLEAN is_recv);
void DispHcp (UINT8 *data, UINT16 len, BOOLEAN is_recv);
void DispSNEP (UINT8 local_sap, UINT8 remote_sap, BT_HDR *p_buf, BOOLEAN is_first, BOOLEAN is_rx);
void DispCHO (UINT8 *pMsg, UINT32 MsgLen, BOOLEAN is_rx);
void DispT3TagMessage(BT_HDR *p_msg, BOOLEAN is_rx);
void DispRWT4Tags (BT_HDR *p_buf, BOOLEAN is_rx);
void DispCET4Tags (BT_HDR *p_buf, BOOLEAN is_rx);
void DispRWI93Tag (BT_HDR *p_buf, BOOLEAN is_rx, UINT8 command_to_respond);
void DispNDEFMsg (UINT8 *pMsg, UINT32 MsgLen, BOOLEAN is_recv);



#ifdef __cplusplus
};
#endif
