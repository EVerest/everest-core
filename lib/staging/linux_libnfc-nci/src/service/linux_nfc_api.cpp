/******************************************************************************
 *
 *  Copyright 2015-2021,2022 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
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

#include "linux_nfc_api.h"
#include "nativeNfcManager.h"
#include "nativeNfcSnep.h"
#include "nativeNfcHandover.h"
#include "nfa_api.h"
#include "nativeNdef.h"
#include "nativeNfcTag.h"
#include "ndef_utils.h"
#include "nativeNfcLlcp.h"
#include "phNxpLog.h"
#include "NativeT4tNfcee.h"
int ndef_readText(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_text, unsigned int out_text_length)
{
    return nativeNdef_readText(ndef_buff, ndef_buff_length, out_text, out_text_length);
}

int ndef_readLanguageCode(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_language_code, unsigned int out_language_code_length)
{
    return nativeNdef_readLang(ndef_buff, ndef_buff_length, out_language_code, out_language_code_length);
}

int ndef_readUrl(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_url, unsigned int out_url_length)
{
    return nativeNdef_readUrl(ndef_buff, ndef_buff_length, out_url, out_url_length);
}

int ndef_readHandoverSelectInfo(unsigned char *ndef_buff, unsigned int ndef_buff_length, nfc_handover_select_t *info)
{
    return nativeNdef_readHs(ndef_buff, ndef_buff_length, info);
}

int ndef_readHandoverRequestInfo(unsigned char *ndef_buff, unsigned int ndef_buff_length, nfc_handover_request_t *info)
{
    return nativeNdef_readHr(ndef_buff, ndef_buff_length, info);
}

int ndef_createUri(char *uri, unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length)
{
    int size;
    if (uri == NULL || out_ndef_buff == NULL || out_ndef_buff_length <= 0)
    {
        return 0;
    }
    size = nativeNdef_createUri(uri, out_ndef_buff, out_ndef_buff_length);
    return size;
}

int ndef_createText(char *language_code, char *text,
                                                    unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length)
{
    int size;
    if (text == NULL || out_ndef_buff == NULL || out_ndef_buff_length <= 0)
    {
        return 0;
    }
    size = nativeNdef_createText(language_code, text, out_ndef_buff, out_ndef_buff_length);
    return size;
}

int ndef_createMime(char *mime_type, unsigned char *mime_data, unsigned int mime_data_length,
                                                    unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length)
{
    int size;
    if (mime_type == NULL || mime_data == NULL || out_ndef_buff == NULL || out_ndef_buff_length == 0)
    {
        return 0;
    }
    size = nativeNdef_createMime(mime_type, mime_data, mime_data_length, out_ndef_buff, out_ndef_buff_length);
    return size;
}

int ndef_createHandoverSelect(nfc_handover_cps_t cps, char *carrier_data_ref,
                                unsigned char *ndef_buff, unsigned int ndef_buff_length, unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length)
{
    int size;
    if (ndef_buff == NULL || ndef_buff_length == 0
            || carrier_data_ref == NULL
            || out_ndef_buff == NULL || out_ndef_buff_length == 0)
    {
        return 0;
    }
    size = nativeNdef_createHs(cps, carrier_data_ref, ndef_buff, ndef_buff_length, out_ndef_buff, out_ndef_buff_length);
    return size;
}

int nfcTag_isNdef(unsigned int handle, ndef_info_t *info)
{
    int ret;
    ret = nativeNfcTag_doCheckNdef(handle, info);
    return ret;
}

int nfcTag_doHandleReconnect(unsigned int handle)
{
    int ret;
    ret = nativeNfcTag_doHandleReconnect(handle);
    return ret;
}

int nfcTag_readNdef(unsigned int handle, unsigned char *ndef_buffer,  unsigned int ndef_buffer_length, nfc_friendly_type_t *friendly_ndef_type)
{
    int ret = 0;
    if (ndef_buffer == NULL || ndef_buffer_length <= 0)
    {
        NXPLOG_API_E ("%s: invalide buffer!", __FUNCTION__);
        return -1;
    }

    ret = nativeNfcTag_doRead(ndef_buffer, ndef_buffer_length, friendly_ndef_type);
    return ret;
}

int nfcTag_writeNdef(unsigned int handle, unsigned char *ndef_buffer, unsigned int ndef_buffer_length)
{
    int ret;
    if (ndef_buffer == NULL || ndef_buffer_length <= 0)
    {
        return -1;
    }
    NXPLOG_API_D ("%s: enter; len = %zu", __FUNCTION__, ndef_buffer_length);
    if (NFA_STATUS_OK != NDEF_MsgValidate(ndef_buffer, ndef_buffer_length,  FALSE))
    {
        NXPLOG_API_E ("%s: not NDEF message!\n)", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    ret = nativeNfcTag_doWrite(ndef_buffer, ndef_buffer_length);
    return ret;
}

int nfcTag_isFormatable(unsigned int handle)
{
    int ret;
    ret = nativeNfcTag_isFormatable(handle);
    return ret;
}

int nfcTag_formatTag(unsigned int handle)
{
    int ret;
    ret = nativeNfcTag_doFormatTag(handle);
    return ret;
}

int nfcTag_makeReadOnly(unsigned int handle, unsigned char *key, unsigned char key_length)
{
    int ret;
    ret = nativeNfcTag_doMakeReadonly(handle, key, key_length);
    return ret;
}

int nfcTag_switchRF(unsigned int handle, int is_frame_rf)
{
    int ret;
    ret = nativeNfcTag_switchRF(handle, is_frame_rf);
    return ret;
}

int nfcTag_transceive (unsigned int handle, unsigned char *tx_buffer, int tx_buffer_length, unsigned char* rx_buffer, int rx_buffer_length, unsigned int timeout)
{
    int ret;
    ret = nativeNfcTag_doTransceive(handle, tx_buffer, tx_buffer_length, rx_buffer, rx_buffer_length, timeout);
    return ret;
}

int doInitialize ()
{
    int ret;
    bool status = nfcManager_doInitialize();
    ret = (status)?NFA_STATUS_OK:NFA_STATUS_FAILED;
    return ret;
}

int doDeinitialize ()
{
    int ret;
    ret = nfcManager_doDeinitialize();
    return ret;
}

int isNfcActive()
{
    int ret;
    ret = nfcManager_isNfcActive();
    return ret;
}

void doEnableDiscovery (int technologies_mask,
                        int reader_only_mode, int enable_host_routing, int restart)
{
    nfcManager_enableDiscovery(technologies_mask, 1, reader_only_mode, enable_host_routing, 1, restart);
}

void disableDiscovery ()
{
    nfcManager_disableDiscovery();
}

void registerTagCallback(nfcTagCallback_t *callback)
{
    nfcManager_registerTagCallback(callback);
}

void deregisterTagCallback()
{
    nfcManager_deregisterTagCallback();
}


int selectNextTag()
{
    return nfcManager_selectNextTag();
}

int getNumTags(void)
{
    return nfcManager_getNumTags();
}

int getFwVersion()
{
#if (APPL_DTA_MODE == TRUE)
    tNFC_FW_VERSION fwVer = {0};
    fwVer = nfc_ncif_getFWVersion();
    return ((fwVer.rom_code_version & 0xFF ) << 16) | ((fwVer.major_version & 0xFF ) << 8) | (fwVer.minor_version & 0xFF);
#endif
}

#if 1 //def SNEP_ENABLED
int nfcSnep_registerClientCallback(nfcSnepClientCallback_t *client_callback)
{
    return nativeNfcSnep_registerClientCallback(client_callback);
}

void nfcSnep_deregisterClientCallback()
{
    nativeNfcSnep_deregisterClientCallback();
}
int nfcSnep_startServer(nfcSnepServerCallback_t *server_callback)
{
    return nativeNfcSnep_startServer(server_callback);
}

void nfcSnep_stopServer()
{
    nativeNfcSnep_stopServer();
}

int nfcSnep_putMessage(unsigned char* msg, unsigned int length)
{
    return nativeNfcSnep_putMessage(msg, length);
}

#endif
void nfcHce_registerHceCallback(nfcHostCardEmulationCallback_t *callback)
{
    nfcManager_registerHostCallback(callback);
}

void nfcHce_deregisterHceCallback()
{
    nfcManager_deregisterHostCallback();
}

int nfcHce_registerT3tIdentifier (UINT8 *Id, UINT8 Idsize)
{
    if(Idsize > 0)
    {
        return nfcManager_doRegisterT3tIdentifier(Id, Idsize);
    }
}

int nfcHce_sendCommand(unsigned char* command, unsigned int command_length)
{
    if(nfcManager_sendRawFrame(command, command_length)) {
        return NFA_STATUS_OK;
    }else {
        return NFA_STATUS_FAILED;
    }
}

int nfcHo_registerCallback(nfcHandoverCallback_t *callback)
{
    return nativeNfcHO_registerCallback(callback);
}

void nfcHo_deregisterCallback()
{
    nativeNfcHO_deregisterCallback();
}

int nfcHo_sendSelectRecord(unsigned char *message, unsigned int length)
{
    if (message == NULL || length == 0)
    {
        return -1;
    }
    return nativeNfcHO_sendHs(message, length);
}

int nfcHo_sendSelectError(unsigned int reason, unsigned int data)
{
    return nativeNfcHO_sendSelectError((unsigned char)reason, data);
}

int nfcLlcp_ConnLessRegisterClientCallback(nfcllcpConnlessClientCallback_t *client_callback)
{
    return nativeNfcLlcp_ConnLessRegisterClientCallback(client_callback);
}
void nfcLlcp_ConnLessDeregisterClientCallback()
{
    nativeNfcLlcp_ConnLessDeregisterClientCallback();

}
int nfcLlcp_ConnLessStartServer(nfcllcpConnlessServerCallback_t *server_callback)
{
    return nativeNfcLlcp_ConnLessStartServer(server_callback);
}
void nfcLlcp_ConnLessStopServer()
{
    nativeNfcLlcp_ConnLessStopServer();

}

int nfcLlcp_ConnLessSendMessage(unsigned char* msg, unsigned int length)
{
    return nativeNfcLlcp_ConnLessSendMessage(msg, length);

}


int nfcLlcp_ConnLessReceiveMessage(unsigned char* msg, unsigned int *length)
{
    return nativeNfcLlcp_ConnLessReceiveMessage(msg, (UINT32 *)length);
}
void InitializeLogLevel()
{
    phNxpLog_InitializeLogLevel();
    return;
}
int checkNextProtocol()
{
    return nativeNfcManager_checkNextProtocol();
}

int nfcManager_setConfig(unsigned char id, unsigned char length, unsigned char* p_data)
{
    return nativeNfcManager_setConfig(id, length, p_data);
}

int doWriteT4tData(unsigned char *command, unsigned char *ndef_buffer, int ndef_buffer_length)
{
    int ret = 0;
    if (ndef_buffer == NULL || ndef_buffer_length <= 0) {
      NXPLOG_API_E ("%s: invalide buffer!", __FUNCTION__);
      return NFA_STATUS_FAILED;
    }
    ret = t4tNfceeManager_doWriteT4tData(command, ndef_buffer, ndef_buffer_length);
    return ret;
}
int doReadT4tData(unsigned char *command, unsigned char *ndef_buffer, int *ndef_buffer_length)
{
    int ret = 0;
    if (ndef_buffer == NULL || *ndef_buffer_length <= 0) {
      NXPLOG_API_E ("%s: invalide buffer!", __FUNCTION__);
      return NFA_STATUS_FAILED;
    }
    ret = t4tNfceeManager_doReadT4tData(command, ndef_buffer, ndef_buffer_length);
    return ret;
}
