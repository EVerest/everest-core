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
 *  Copyright (C) 2015 NXP Semiconductors
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

#include <stdarg.h>
#include <stdio.h>
#include "OverrideLog.h"
#include "android_logmsg.h"
#include "nfc_target.h"
#include "buildcfg.h"
#include "phNxpLog.h"

extern UINT32 ScrProtocolTraceFlag;
#define MAX_NCI_PACKET_SIZE 259
#define BTE_LOG_BUF_SIZE 1024
#define BTE_LOG_MAX_SIZE (BTE_LOG_BUF_SIZE - 12)
#define MAX_LOGCAT_LINE 4096
//#define PRINT(s) printf (ANDROID_LOG_DEBUG, "BrcmNci", s)
#define PRINT(s) printf (s)
#define UNUSED(X) (void)X
static char log_line [MAX_LOGCAT_LINE];
static const char* sTable = "0123456789abcdef";
static BOOLEAN sIsUseRaw = FALSE;
static void ToHex (const UINT8* data, UINT16 len, char* hexString, UINT16 hexStringSize);
static void dumpbin (const char* data, int size, UINT32 trace_layer, UINT32 trace_type);
static inline void word2hex (const char* data, char** hex);
static inline void byte2char (const char* data, char** str);
static inline void byte2hex (const char* data, char** str);


void BTDISP_LOCK_LOG()
{
}


void BTDISP_UNLOCK_LOG()
{
}


void BTDISP_INIT_LOCK()
{
}


void BTDISP_UNINIT_LOCK()
{
}


void ProtoDispAdapterUseRawOutput (BOOLEAN isUseRaw)
{
    sIsUseRaw = isUseRaw;
}


void ProtoDispAdapterDisplayNciPacket (UINT8 *nciPacket, UINT16 nciPacketLen, BOOLEAN is_recv)
{
    //Protocol decoder is not available, so decode NCI packet into hex numbers.
    //if (!(ScrProtocolTraceFlag & SCR_PROTO_TRACE_NCI))
    //    return;
    char line_buf [(MAX_NCI_PACKET_SIZE*2)+1];
    ToHex (nciPacket, nciPacketLen, line_buf, sizeof(line_buf));
    phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, (is_recv) ? "NfcNciR:": "NfcNciX:", line_buf);
}


void ToHex (const UINT8* data, UINT16 len, char* hexString, UINT16 hexStringSize)
{
    int i=0, j=0;
    for(i = 0, j = 0; i < len && j < hexStringSize-3; i++)
    {
        hexString [j++] = sTable [(*data >> 4) & 0xf];
        hexString [j++] = sTable [*data & 0xf];
        data++;
    }
    hexString [j] = '\0';
}


//Protodisp code calls ScrLog() to print decoded texts.
void ScrLog (UINT32 trace_set_mask, const char *fmt_str, ...)
{
    static char buffer [BTE_LOG_BUF_SIZE];
    va_list ap;
    UNUSED(trace_set_mask);
    va_start (ap, fmt_str);
    vsnprintf (buffer, BTE_LOG_MAX_SIZE, fmt_str, ap);
    va_end (ap);
    phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, "NfcNci:    ", buffer);
}


UINT8 *scru_dump_hex (UINT8 *p, char *pTitle, UINT32 len, UINT32 layer, UINT32 type)
{
    if(pTitle && *pTitle)
        PRINT(pTitle);
    dumpbin((char*) p, len, layer, type);
    return p;
}


void dumpbin(const char* data, int size, UINT32 trace_layer, UINT32 trace_type)
{
    char line_buff[256];
    char *line;
    int i, j, addr;
    const int width = 16;
    UNUSED(trace_layer);
    UNUSED(trace_type);
    if(size <= 0)
        return;
    for(i = 0; i < size / width; i++)
    {
        line = line_buff;
        //write address:
        addr = i*width;
        word2hex((const char*)&addr, &line);
        *line++ = ':'; *line++ = ' ';
        //write hex of data
        for(j = 0; j < width; j++)
        {
            byte2hex(&data[j], &line);
            *line++ = ' ';
        }
        //write char of data
        for(j = 0; j < width; j++)
            byte2char(data++, &line);
        //wirte the end of line
        *line = 0;
        //output the line
        PRINT(line_buff);
    }
    //last line of left over if any
    int leftover = size % width;
    if(leftover > 0)
    {
        line = line_buff;
        //write address:
        addr = i*width;
        word2hex((const char*)&addr, &line);
        *line++ = ':'; *line++ = ' ';
        //write hex of data
        for(j = 0; j < leftover; j++)
        {
            byte2hex(&data[j], &line);
            *line++ = ' ';
        }
        //write hex padding
        for(; j < width; j++)
        {
            *line++ = ' ';
            *line++ = ' ';
            *line++ = ' ';
        }
        //write char of data
        for(j = 0; j < leftover; j++)
            byte2char(data++, &line);
        //write the end of line
        *line = 0;
        //output the line
        PRINT(line_buff);
    }
}


inline void word2hex (const char* data, char** hex)
{
    byte2hex(&data[1], hex);
    byte2hex(&data[0], hex);
}


inline void byte2char (const char* data, char** str)
{
    **str = *data < ' ' ? '.' : *data > '~' ? '.' : *data;
    ++(*str);
}


inline void byte2hex (const char* data, char** str)
{
    **str = sTable[(*data >> 4) & 0xf];
    ++*str;
    **str = sTable[*data & 0xf];
    ++*str;
}


    //Decode a few Bluetooth HCI packets into hex numbers.
    void DispHciCmd (BT_HDR *p_buf)
    {
        UINT32 nBytes = ((BT_HDR_SIZE + p_buf->offset + p_buf->len)*2)+1;
        UINT8* data = (UINT8*) p_buf;
        int data_len = BT_HDR_SIZE + p_buf->offset + p_buf->len;

        if (appl_trace_level < BT_TRACE_LEVEL_DEBUG)
            return;

        if (nBytes > sizeof(log_line))
            return;

        ToHex (data, data_len, log_line, sizeof(log_line));
        phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, "NfcHciX:", log_line);
    }


    //Decode a few Bluetooth HCI packets into hex numbers.
    void DispHciEvt (BT_HDR *p_buf)
    {
        UINT32 nBytes = ((BT_HDR_SIZE + p_buf->offset + p_buf->len)*2)+1;
        UINT8* data = (UINT8*) p_buf;
        int data_len = BT_HDR_SIZE + p_buf->offset + p_buf->len;

        if (appl_trace_level < BT_TRACE_LEVEL_DEBUG)
            return;

        if (nBytes > sizeof(log_line))
            return;

        ToHex (data, data_len, log_line, sizeof(log_line));
        //__android_log_write (ANDROID_LOG_DEBUG, "BrcmHciR", log_line);
        phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, "NfcHciR:", log_line);
    }


    /*******************************************************************************
    **
    ** Function         DispLLCP
    **
    ** Description      Log LLCP packet as hex-ascii bytes.
    **
    ** Returns          None.
    **
    *******************************************************************************/
    void DispLLCP (BT_HDR *p_buf, BOOLEAN is_recv)
    {
        UINT32 nBytes = ((BT_HDR_SIZE + p_buf->offset + p_buf->len)*2)+1;
        UINT8 * data = (UINT8*) p_buf;
        int data_len = BT_HDR_SIZE + p_buf->offset + p_buf->len;

        if (appl_trace_level < BT_TRACE_LEVEL_DEBUG)
            return;

        if (nBytes > sizeof(log_line))
            return;

        ToHex (data, data_len, log_line, sizeof(log_line));
        //__android_log_write (ANDROID_LOG_DEBUG, (is_recv) ? "BrcmLlcpR": "BrcmLlcpX", log_line);
        phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, (is_recv) ? "NfcLlcpR:": "NfcLlcpX:", log_line);
    }


    /*******************************************************************************
    **
    ** Function         DispHcp
    **
    ** Description      Log raw HCP packet as hex-ascii bytes
    **
    ** Returns          None.
    **
    *******************************************************************************/
    void DispHcp (UINT8 *data, UINT16 len, BOOLEAN is_recv)
    {
        UINT32 nBytes = (len*2)+1;

        if (appl_trace_level < BT_TRACE_LEVEL_DEBUG)
            return;

        // Only trace HCP if we're tracing HCI as well
        //if (!(ScrProtocolTraceFlag & SCR_PROTO_TRACE_HCI_SUMMARY))
        //    return;

        if (nBytes > sizeof(log_line))
            return;

        ToHex (data, len, log_line, sizeof(log_line));
        //__android_log_write (ANDROID_LOG_DEBUG, (is_recv) ? "BrcmHcpR": "BrcmHcpX", log_line);
        phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, (is_recv) ? "NfcHcpR:": "NfcHcpX:", log_line);
    }


    void DispSNEP (UINT8 local_sap, UINT8 remote_sap, BT_HDR *p_buf, BOOLEAN is_first, BOOLEAN is_rx)
    {
       UNUSED(local_sap);
       UNUSED(remote_sap);
       UNUSED(p_buf);
       UNUSED(is_first);
       UNUSED(is_rx);
    }
    void DispCHO (UINT8 *pMsg, UINT32 MsgLen, BOOLEAN is_rx)
    {
       UNUSED(pMsg);
       UNUSED(MsgLen);
       UNUSED(is_rx);
    }
    void DispT3TagMessage(BT_HDR *p_msg, BOOLEAN is_rx)
    {
       UNUSED(p_msg);
       UNUSED(is_rx);
    }
    void DispRWT4Tags (BT_HDR *p_buf, BOOLEAN is_rx)
    {
       UNUSED(p_buf);
       UNUSED(is_rx);
    }
    void DispCET4Tags (BT_HDR *p_buf, BOOLEAN is_rx)
    {
       UNUSED(p_buf);
       UNUSED(is_rx);
    }
    void DispRWI93Tag (BT_HDR *p_buf, BOOLEAN is_rx, UINT8 command_to_respond)
    {
       UNUSED(p_buf);
       UNUSED(is_rx);
       UNUSED(command_to_respond);
    }
    void DispNDEFMsg (UINT8 *pMsg, UINT32 MsgLen, BOOLEAN is_recv)
    {
       UNUSED(pMsg);
       UNUSED(MsgLen);
       UNUSED(is_recv);
    }


/*******************************************************************************
**
** Function:        LogMsg
**
** Description:     Print messages from NFC stack.
**
** Returns:         None.
**
*******************************************************************************/
void LogMsg (UINT32 trace_set_mask, const char *fmt_str, ...)
{
    static char buffer [BTE_LOG_BUF_SIZE];
    va_list ap;
    UINT32 trace_type = trace_set_mask & 0x07; //lower 3 bits contain trace type

    va_start (ap, fmt_str);
    vsnprintf (buffer, BTE_LOG_MAX_SIZE, fmt_str, ap);
    va_end (ap);
    NXPLOG_API_D("Nfc: %s\n", buffer);
    //phNxpLog_LogMsg(NXPLOG_LOG_DEBUG_LOGLEVEL, "Brcm:       ", buffer);
}


void LogMsg_0 (UINT32 maskTraceSet, const char *p_str)
{
    LogMsg (maskTraceSet, p_str);
}


void LogMsg_1 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1)
{
    LogMsg (maskTraceSet, fmt_str, p1);
}


void LogMsg_2 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1, UINT32 p2)
{
    LogMsg (maskTraceSet, fmt_str, p1, p2);
}


void LogMsg_3 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3)
{
    LogMsg (maskTraceSet, fmt_str, p1, p2, p3);
}


void LogMsg_4 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4)
{
    LogMsg (maskTraceSet, fmt_str, p1, p2, p3, p4);
}

void LogMsg_5 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5)
{
    LogMsg (maskTraceSet, fmt_str, p1, p2, p3, p4, p5);
}


void LogMsg_6 (UINT32 maskTraceSet, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5, UINT32 p6)
{
    LogMsg (maskTraceSet, fmt_str, p1, p2, p3, p4, p5, p6);
}
