/******************************************************************************
 *
 *  Copyright 2015-2021 NXP
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

#include <string.h>
#include <malloc.h>
#include "nativeNdef.h"

#include "phNxpLog.h"
#include "ndef_utils.h"
#include "nfa_api.h"
#include "data_types.h"

#define NFC_FORUM_HANDOVER_VERSION             0x12    /* version 1.2 */

#define BT_HANDOVER_TYPE_MAC    0x1B
#define BT_HANDOVER_TYPE_LE_ROLE    0x1C
#define BT_HANDOVER_TYPE_LONG_LOCAL_NAME    0x09
#define BT_HANDOVER_TYPE_SHORT_LOCAL_NAME   0x08
#define BT_HANDOVER_LE_ROLE_CENTRAL_ONLY    0x01

#define WIFI_HANDOVER_SSID_ID   0x1045
#define WIFI_HANDOVER_NETWORK_KEY_ID    0x1027

static UINT8 RTD_TEXT[1] = {'T'};
static UINT8 RTD_URL[1] = {'U'};
static UINT8 RTD_Hs[2] = {'H', 's'};
static UINT8 RTD_Hr[2] = {'H', 'r'};
static UINT8 RTD_Ac[2] = { 'a', 'c' };

/* Bluetooth OOB Data Type */
static UINT8 *BT_OOB_REC_TYPE = (UINT8 *)"application/vnd.bluetooth.ep.oob";

/* BLE OOB Data Type */
static UINT8 *BLE_OOB_REC_TYPE = (UINT8 *)"application/vnd.bluetooth.le.oob";

/* Wifi WSC Data Type */
static UINT8 *WIFI_WSC_REC_TYPE = (UINT8 *)"application/vnd.wfa.wsc";


static char *URI_PREFIX_MAP[] = {
            "", // 0x00
            "http://www.", // 0x01
            "https://www.", // 0x02
            "http://", // 0x03
            "https://", // 0x04
            "tel:", // 0x05
            "mailto:", // 0x06
            "ftp://anonymous:anonymous@", // 0x07
            "ftp://ftp.", // 0x08
            "ftps://", // 0x09
            "sftp://", // 0x0A
            "smb://", // 0x0B
            "nfs://", // 0x0C
            "ftp://", // 0x0D
            "dav://", // 0x0E
            "news:", // 0x0F
            "telnet://", // 0x10
            "imap:", // 0x11
            "rtsp://", // 0x12
            "urn:", // 0x13
            "pop:", // 0x14
            "sip:", // 0x15
            "sips:", // 0x16
            "tftp:", // 0x17
            "btspp://", // 0x18
            "btl2cap://", // 0x19
            "btgoep://", // 0x1A
            "tcpobex://", // 0x1B
            "irdaobex://", // 0x1C
            "file://", // 0x1D
            "urn:epc:id:", // 0x1E
            "urn:epc:tag:", // 0x1F
            "urn:epc:pat:", // 0x20
            "urn:epc:raw:", // 0x21
            "urn:epc:", // 0x22
            "urn:nfc:", // 0x23
    };

#define URI_PREFIX_MAP_LENGTH 24
#define BLUETOOTH_ADDRESS_LENGTH 6

static INT32 parseBluetoothAddress(UINT8* payload, UINT32 payload_length, UINT8 *address)
{
    INT32 xx;
    UINT8 *p;
    if (payload == NULL || payload_length < BLUETOOTH_ADDRESS_LENGTH)
    {
        return -1;
    }
    //get address from little endian
    p = payload + (BLUETOOTH_ADDRESS_LENGTH - 1);
    for (xx = 0; xx < BLUETOOTH_ADDRESS_LENGTH; xx++)
    {
        address[xx] = *p--;
    }
    return 0;
}

static tNFA_STATUS getDeviceCps(UINT8 *record, UINT8 *ref_name, UINT8 ref_len, nfc_handover_cps_t *power)
{
    UINT8 *p_ac_record, *p_ac_payload;
    uint32_t ac_payload_len;
    UINT8 cps = HANDOVER_CPS_UNKNOWN;
    UINT8 carrier_ref_name_len;

    NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
    p_ac_record = NDEF_MsgGetFirstRecByType (record, NDEF_TNF_WELLKNOWN, RTD_Ac, sizeof(RTD_Ac));
    while ((p_ac_record))
    {
        NXPLOG_API_D ("%s: find ac record\n", __FUNCTION__);
        /* get payload */
        p_ac_payload = NDEF_RecGetPayload (p_ac_record, &ac_payload_len);

        if ((!p_ac_payload) || (ac_payload_len < 3))
        {
            NXPLOG_API_E ("%s: Failed to get ac payload", __FUNCTION__);
            return NFA_STATUS_FAILED;
        }

        /* Carrier Power State */
        cps = *p_ac_payload++;

        /* Carrier Data Reference Length and Characters */
        carrier_ref_name_len =  *p_ac_payload++;

        ac_payload_len -= 2;

        /* remaining must have carrier data ref and Auxiliary Data Reference Count at least */
        if (ac_payload_len > carrier_ref_name_len)
        {
            if (carrier_ref_name_len > NFA_CHO_MAX_REF_NAME_LEN)
            {
                NXPLOG_API_E ("%s: Too many bytes for carrier_ref_name, len = %d",
                                   __FUNCTION__, carrier_ref_name_len);
                return NFA_STATUS_FAILED;
            }
        }
        else
        {
            NXPLOG_API_E ("%s: Failed to parse carrier_ref_name", __FUNCTION__);
            return NFA_STATUS_FAILED;
        }

        if ((carrier_ref_name_len == ref_len) && (memcmp(p_ac_payload, ref_name, ref_len) == 0))
        {
            *power = (nfc_handover_cps_t)cps;
            return NFA_STATUS_OK;
        }

        /* get next Alternative Carrier record */
        p_ac_record = NDEF_MsgGetNextRecByType (p_ac_record, NDEF_TNF_WELLKNOWN, RTD_Ac, sizeof(RTD_Ac));
    }

    return NFA_STATUS_FAILED;
}

nfc_friendly_type_t nativeNdef_getFriendlyType(UINT8 tnf, UINT8 *type, UINT8 typeLength)
{
    if(tnf == NDEF_TNF_WELLKNOWN
                && (memcmp(type, RTD_TEXT, typeLength) == 0))
    {
        return NDEF_FRIENDLY_TYPE_TEXT;
    }
    if(tnf == NDEF_TNF_URI 
                || (tnf == NDEF_TNF_WELLKNOWN && (memcmp(type, RTD_URL, typeLength) == 0)))
    {
        return NDEF_FRIENDLY_TYPE_URL;
    }
    if (tnf == NDEF_TNF_WELLKNOWN
                && (memcmp(type, RTD_Hs, typeLength) == 0))
    {
        return NDEF_FRIENDLY_TYPE_HS;
    }
    if (tnf == NDEF_TNF_WELLKNOWN
                && (memcmp(type, RTD_Hr, typeLength) == 0))
    {
        return NDEF_FRIENDLY_TYPE_HR;
    }
    return NDEF_FRIENDLY_TYPE_OTHER;
}

INT32 nativeNdef_createUri(char *uri, UINT8*outNdefBuff, UINT32 outBufferLen)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    INT32 uriLength = strlen(uri);
    uint32_t current_size = 0;
    INT32 i, prefixLength;
    NXPLOG_API_D ("%s: enter, uri = %s", __FUNCTION__, uri);

    for (i = 1; i < URI_PREFIX_MAP_LENGTH; i++)
    {
        if (memcmp(URI_PREFIX_MAP[i], uri, strlen(URI_PREFIX_MAP[i])) == 0)
        {
            
            break;
        }
    }
    if (i == URI_PREFIX_MAP_LENGTH)
    {
        i = 0;
    }
    prefixLength = strlen(URI_PREFIX_MAP[i]);
    status = NDEF_MsgAddRec(outNdefBuff, outBufferLen, &current_size, NDEF_TNF_WKT, (UINT8*)RTD_URL, 1, NULL, 0,
                                    (UINT8*)&i, 1);
    status |= NDEF_MsgAppendPayload(outNdefBuff, outBufferLen, &current_size, outNdefBuff,
                                    (UINT8*)(uri + prefixLength), (UINT32)(uriLength - prefixLength));

    if (status != NFA_STATUS_OK )
    {
        NXPLOG_API_E ("%s: couldn't create Ndef record", __FUNCTION__);
        current_size = 0;
        goto END;
    }

END:
    NXPLOG_API_D ("%s: exit", __FUNCTION__);
    return current_size;
}

INT32 nativeNdef_createText(char *languageCode, char *text, UINT8*outNdefBuff, UINT32 outBufferLen)
{
    static char * DEFAULT_LANGUAGE_CODE = "En";
    tNFA_STATUS status = NFA_STATUS_FAILED;
    UINT32 textLength = strlen(text);
    UINT32 langCodeLength = 0;
    uint32_t current_size = 0;
    char *langCode = (char *)languageCode;
    NXPLOG_API_D ("%s: enter, text = %s", __FUNCTION__, text);

    if (langCode != NULL)
    {
        langCodeLength = strlen(langCode);
    }

    if (langCodeLength > 64)
    {
        NXPLOG_API_E ("%s: language code is too long, must be <64 bytes.", __FUNCTION__);
        return 0;
    }
    if (langCodeLength == 0)
    {
        //set default language to 'EN'
        langCode = DEFAULT_LANGUAGE_CODE;
        langCodeLength = 2;
    }
    memset (outNdefBuff, 0, outBufferLen);
    status = NDEF_MsgAddRec(outNdefBuff, outBufferLen, &current_size, NDEF_TNF_WKT, (UINT8*)RTD_TEXT, 1, NULL, 0,
                                    (UINT8*)(&langCodeLength), 1);
    status |= NDEF_MsgAppendPayload(outNdefBuff, outBufferLen, &current_size, outNdefBuff,
                                    (UINT8*)langCode, langCodeLength);
    status |= NDEF_MsgAppendPayload(outNdefBuff, outBufferLen, &current_size, outNdefBuff,
                                    (UINT8*)text, textLength);

    if (status != NFA_STATUS_OK )
    {
        NXPLOG_API_E ("%s: couldn't create Ndef record", __FUNCTION__);
        current_size =0;
        goto END;
    }

END:
    NXPLOG_API_D ("%s: exit", __FUNCTION__);
    return current_size;
}

INT32 nativeNdef_createMime(char *mimeType, UINT8 *mimeData, UINT32 mimeDataLength,
                                                                UINT8*outNdefBuff, UINT32 outBufferLen)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    uint32_t current_size = 0;
    UINT32 mimeTypeLength = strlen(mimeType);
    NXPLOG_API_D ("%s: enter, mime = %s", __FUNCTION__, mimeType);

    if (mimeTypeLength + mimeDataLength >= (INT32)outBufferLen)
    {
        NXPLOG_API_E ("%s: data too large.", __FUNCTION__);
        return 0;
    }

    status = NDEF_MsgAddRec(outNdefBuff, outBufferLen, &current_size, NDEF_TNF_MEDIA, (UINT8 *)mimeType, mimeTypeLength, NULL, 0,
                                    (UINT8*)mimeData, (UINT32)mimeDataLength);

    if (status != NFA_STATUS_OK )
    {
        NXPLOG_API_E ("%s: couldn't create Ndef record", __FUNCTION__);
        current_size = 0;
        goto END;
    }

END:
    NXPLOG_API_D ("%s: exit", __FUNCTION__);
    return current_size;
}

INT32 nativeNdef_createHs(nfc_handover_cps_t cps, char *carrier_data_ref,
                                UINT8 *ndefBuff, UINT32 ndefBuffLen, UINT8 *outBuff, UINT32 outBuffLen)
{
    UINT8          *p_msg_ac = NULL;
    uint32_t          cur_size_ac = 0;
    UINT32  max_size, cur_ndef_size = 0;
    if (ndefBuff == NULL || ndefBuffLen == 0 
            || outBuff == NULL || outBuffLen == 0
            || carrier_data_ref == NULL)
    {
        return -1;
    }
    max_size = outBuffLen - ndefBuffLen;
    p_msg_ac = (UINT8 *) malloc (max_size);

    if (!p_msg_ac)
    {
        NXPLOG_API_E ("%s: Failed to allocate buffer", __FUNCTION__);
        return 0;
    }

    NDEF_MsgInit (p_msg_ac, max_size, &cur_size_ac);
    if (NDEF_OK != NDEF_MsgAddWktAc (p_msg_ac, max_size,(UINT32 *) &cur_size_ac,
                       cps, carrier_data_ref, 0, NULL))
    {
        NXPLOG_API_E ("%s: Failed to create ac message", __FUNCTION__);
        cur_ndef_size = 0;
        goto end_and_return;
    }

    /* Creare Handover Select Record */
    if (NDEF_OK != NDEF_MsgCreateWktHs (outBuff, outBuffLen, &cur_ndef_size,
                              NFC_FORUM_HANDOVER_VERSION))
    {
        NXPLOG_API_E ("%s: Failed to create Hs message", __FUNCTION__);
        cur_ndef_size = 0;
        goto end_and_return;
    }

    /* Append Alternative Carrier Records */
    if (NDEF_OK != NDEF_MsgAppendPayload (outBuff, outBuffLen, (uint32_t *) &cur_ndef_size,
                                        outBuff, p_msg_ac, cur_size_ac))
    {
        NXPLOG_API_E ("%s: Failed to append Alternative Carrier Record", __FUNCTION__);
        cur_ndef_size = 0;
        goto end_and_return;
    }
#if 0
    /* Append Alternative Carrier Reference Data */
    if (NDEF_OK != NDEF_MsgAppendRec (outBuff, outBuffLen, &cur_ndef_size,
                                ndefBuff, ndefBuffLen))
    {
        NXPLOG_API_E ("%s: Failed to append Alternative Carrier Reference Data", __FUNCTION__);
        cur_ndef_size = 0;
        goto end_and_return;
    }
#endif
end_and_return:
    if (p_msg_ac)
    {
        free(p_msg_ac);
    }
    return cur_ndef_size;
}

INT32 nativeNdef_readText( UINT8*ndefBuff, UINT32 ndefBuffLen, char * outText, UINT32 textLen)
{
    int langCodeLen = 0;
    UINT8 *payload = NULL;
    UINT32 payloadLength = 0;
    UINT8 ndef_tnf = 0;
    UINT8 *ndef_type = NULL;
    UINT8 ndef_typeLength = 0;
    nfc_friendly_type_t friendly_type = NDEF_FRIENDLY_TYPE_OTHER;

    ndef_type = NDEF_RecGetType((UINT8*)ndefBuff, &ndef_tnf, &ndef_typeLength);
    friendly_type = nativeNdef_getFriendlyType(ndef_tnf, ndef_type, ndef_typeLength);
    if (friendly_type != NDEF_FRIENDLY_TYPE_TEXT)
    {
        return -1;
    }
    payload = NDEF_RecGetPayload((UINT8*)ndefBuff, (uint32_t*)&payloadLength);
    if (payload == NULL)
    {
        return -1;
    }
    langCodeLen = payload[0];
    if (textLen < (payloadLength - langCodeLen - 1))
    {
        return -1;
    }
    memcpy(outText, payload + langCodeLen + 1, payloadLength - langCodeLen - 1);
    return (payloadLength - langCodeLen - 1);
}

INT32 nativeNdef_readLang( UINT8*ndefBuff, UINT32 ndefBuffLen, char * outLang, UINT32 LangLen)
{
    int langCodeLen = 0;
    UINT8 *payload = NULL;
    uint32_t payloadLength = 0;
    UINT8 ndef_tnf = 0;
    UINT8 *ndef_type = NULL;
    UINT8 ndef_typeLength = 0;
    nfc_friendly_type_t friendly_type = NDEF_FRIENDLY_TYPE_OTHER;

    ndef_type = NDEF_RecGetType((UINT8*)ndefBuff, &ndef_tnf, &ndef_typeLength);
    friendly_type = nativeNdef_getFriendlyType(ndef_tnf, ndef_type, ndef_typeLength);
    if (friendly_type != NDEF_FRIENDLY_TYPE_TEXT)
    {
        return -1;
    }
    payload = NDEF_RecGetPayload((UINT8*)ndefBuff, &payloadLength);
    if (payload == NULL)
    {
        return -1;
    }
    langCodeLen = payload[0];
    if (LangLen < langCodeLen)
    {
        return -1;
    }
    memcpy(outLang, payload + 1, langCodeLen);
    return (langCodeLen);
}

INT32 nativeNdef_readUrl(UINT8*ndefBuff, UINT32 ndefBuffLen, char * outUrl, UINT32 urlBufferLen)
{
    UINT32 prefixIdx = 0;
    UINT32 prefixLen = 0;
    UINT8 *payload = NULL;
    UINT32 payloadLength = 0;
    UINT8 ndef_tnf = 0;
    UINT8 *ndef_type = NULL;
    UINT8 ndef_typeLength = 0;
    nfc_friendly_type_t friendly_type = NDEF_FRIENDLY_TYPE_OTHER;

    ndef_type = NDEF_RecGetType((UINT8*)ndefBuff, &ndef_tnf, &ndef_typeLength);
    friendly_type = nativeNdef_getFriendlyType(ndef_tnf, ndef_type, ndef_typeLength);
    if (friendly_type != NDEF_FRIENDLY_TYPE_URL)
    {
        return -1;
    }
    payload = NDEF_RecGetPayload((UINT8*)ndefBuff, (uint32_t *)&payloadLength);
    if (payload == NULL)
    {
        return -1;
    }

    if( payload[0]  >= URI_PREFIX_MAP_LENGTH )
    {
        prefixIdx = 0;
    }
    else
    {
        prefixIdx = payload[0];
    }
    prefixLen = strlen(URI_PREFIX_MAP[prefixIdx]);
    if (urlBufferLen < payloadLength + prefixLen)
    {
        return -1;
    }
    memcpy(outUrl, URI_PREFIX_MAP[prefixIdx], prefixLen);
    memcpy(outUrl + prefixLen, payload + 1, payloadLength - 1);
    return (payloadLength + prefixLen - 1);
 }

INT32 nativeNdef_readHr(UINT8*ndefBuff, UINT32 ndefBuffLen, nfc_handover_request_t *hrInfo)
{
    UINT8 *p_hr_record = NULL;
    UINT8 *p_hr_payload = NULL;
    UINT32 hr_payload_len = 0;
    UINT8 *p_record = NULL;
    UINT8 *p_payload = NULL;
    UINT32 record_payload_len = 0;
    UINT8 *p_id = NULL;
    UINT8 id_len = 0;
    UINT8   version = 0;
    UINT32 index = 0;
    UINT8 len = 0;
    UINT8 type = 0xFF;

    (void)ndefBuffLen;
    if (hrInfo == NULL)
    {
        return -1;
    }
    memset(hrInfo, 0, sizeof(nfc_handover_request_t));
    NXPLOG_API_D ("%s: enter", __FUNCTION__);

    /* get Handover Request record */
    p_hr_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_WELLKNOWN, (UINT8*)RTD_Hr, sizeof(RTD_Hr));
    if (p_hr_record)
    {
        NXPLOG_API_E ("%s: Find Hr record", __FUNCTION__);
        p_hr_payload = NDEF_RecGetPayload (p_hr_record,(uint32_t *) &hr_payload_len);

        if ((!p_hr_payload) || (hr_payload_len < 7))
        {
            NXPLOG_API_E ("%s: Failed to get Hr payload (version, cr/ac record)", __FUNCTION__);
            return -1;
        }

        /* Version */
        if (NFC_FORUM_HANDOVER_VERSION != p_hr_payload[0])
        {
            NXPLOG_API_E ("%s: Version (0x%02x) not matched", __FUNCTION__, p_hr_payload[0]);
            return -1;
        }
        p_hr_payload += 1;
        hr_payload_len--;

        /* NDEF message for Collision Resolution record and Alternative Carrier records */
        if (NDEF_OK != NDEF_MsgValidate (p_hr_payload, hr_payload_len, FALSE))
        {
            NXPLOG_API_E ("%s: Failed to validate NDEF message for cr/ac records", __FUNCTION__);
            return -1;
        }
    }

    p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                          BT_OOB_REC_TYPE, BT_OOB_REC_TYPE_LEN);

    if (p_record)
    {
        NXPLOG_API_D ("%s: Found BT OOB record", __FUNCTION__);
        if (p_hr_record)
        {
            p_id = NDEF_RecGetId(p_record, &id_len);
            if (p_id == NULL || id_len == 0)
            {
                NXPLOG_API_E ("%s: Failed to retreive NDEF ID", __FUNCTION__);
                return -1;
            }
        
            if (getDeviceCps(p_hr_payload, p_id, id_len, &hrInfo->bluetooth.power_state)!=NFA_STATUS_OK)
            {
                hrInfo->bluetooth.power_state = HANDOVER_CPS_UNKNOWN;
            }
        }
        p_payload = NDEF_RecGetPayload(p_record, (uint32_t *)&record_payload_len);
        if (p_payload == NULL)
        {
            NXPLOG_API_E ("%s: Failed to retreive NDEF payload", __FUNCTION__);
            return -1;
        }
        hrInfo->bluetooth.type = HANDOVER_TYPE_BT;
        hrInfo->bluetooth.ndef = p_record;
        hrInfo->bluetooth.ndef_length = record_payload_len;
        index = 2;
        if (parseBluetoothAddress(&p_payload[index] , record_payload_len - index, hrInfo->bluetooth.address)!= 0)
        {
            NXPLOG_API_E ("%s: Failed to retreive device address", __FUNCTION__);
            return -1;
        }
        index += BLUETOOTH_ADDRESS_LENGTH;
        while(index < record_payload_len)
        {
            len = p_payload[index++];
            type = p_payload[index++];
            switch (type)
            {
                case BT_HANDOVER_TYPE_SHORT_LOCAL_NAME:
                    hrInfo->bluetooth.device_name = p_payload;
                    hrInfo->bluetooth.device_name_length = len;
                    break;
                case BT_HANDOVER_TYPE_LONG_LOCAL_NAME:
                    if (hrInfo->bluetooth.device_name)
                    {
                        break;  // prefer short name
                    }
                    hrInfo->bluetooth.device_name = p_payload;
                    hrInfo->bluetooth.device_name_length = len;
                    break;
                default:
                    index += (len - 1);
                    break;
            }
        }
    }
    else
    {
        p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                              BLE_OOB_REC_TYPE, BT_OOB_REC_TYPE_LEN);

        if (p_record)
        {
            NXPLOG_API_D ("%s: Found BLE OOB record", __FUNCTION__);
            if (p_hr_record)
            {
                p_id = NDEF_RecGetId(p_record, &id_len);
                if (p_id == NULL || id_len == 0)
                {
                    NXPLOG_API_E ("%s: Failed to retreive NDEF ID", __FUNCTION__);
                    return -1;
                }
            
                if (getDeviceCps(p_hr_payload, p_id, id_len, &hrInfo->bluetooth.power_state)!=NFA_STATUS_OK)
                {
                    hrInfo->bluetooth.power_state = HANDOVER_CPS_UNKNOWN;
                }
            }

            p_payload = NDEF_RecGetPayload(p_record, (uint32_t *)&record_payload_len);
            if (p_payload == NULL)
            {
                NXPLOG_API_E ("%s: Failed to retreive NDEF payload", __FUNCTION__);
                return -1;
            }
            hrInfo->bluetooth.type = HANDOVER_TYPE_BLE;
            hrInfo->bluetooth.ndef = p_record;
            hrInfo->bluetooth.ndef_length = record_payload_len;
            index = 0;
            while(index < record_payload_len)
            {
                len = p_payload[index++];
                type = p_payload[index++];
                switch (type)
                {
                    case BT_HANDOVER_TYPE_MAC:
                        parseBluetoothAddress(&p_payload[index] , record_payload_len - index, hrInfo->bluetooth.address);
                        break;
                    case BT_HANDOVER_TYPE_SHORT_LOCAL_NAME:
                        hrInfo->bluetooth.device_name = p_payload;
                        hrInfo->bluetooth.device_name_length = len;
                        break;
                    case BT_HANDOVER_TYPE_LONG_LOCAL_NAME:
                        if (hrInfo->bluetooth.device_name)
                        {
                            break;  // prefer short name
                        }
                        hrInfo->bluetooth.device_name = p_payload;
                        hrInfo->bluetooth.device_name_length = len;
                        break;
                    default:
                        index += (len - 1);
                        break;
                }
            }        
        }
    }
    p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                          WIFI_WSC_REC_TYPE, WIFI_WSC_REC_TYPE_LEN);

    if (p_record)
    {
        NXPLOG_API_D ("%s: Found WiFi record", __FUNCTION__);
        hrInfo->wifi.has_wifi = TRUE;
        hrInfo->wifi.ndef = p_record;
        hrInfo->wifi.ndef_length = record_payload_len;
    }
    return 0;
}

INT32 nativeNdef_readHs(UINT8*ndefBuff, UINT32 ndefBuffLen, nfc_handover_select_t *hsInfo)
{
    UINT8 *p_hs_record = NULL;
    UINT8 *p_hs_payload = NULL;
    UINT32 hs_payload_len = 0;
    UINT8 *p_record = NULL;
    UINT8 *p_payload = NULL;
    UINT32 record_payload_len = 0;
    UINT8 *p_id = NULL;
    UINT8 id_len = 0;
    UINT8  version = 0;
    UINT32 index = 0;
    UINT8 bt_len = 0;
    UINT8 bt_type = 0xFF;
    UINT16 wifi_len = 0;
    UINT16 wifi_type = 0xFFFF;
    UINT8 status = -1;

    (void)ndefBuffLen;
    if (hsInfo == NULL)
    {
        return -1;
    }
    memset(hsInfo, 0, sizeof(nfc_handover_select_t));

    /* get Handover Request record */
    p_hs_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_WELLKNOWN, (UINT8*)RTD_Hs, sizeof(RTD_Hs));
    if (p_hs_record)
    {
        p_hs_payload = NDEF_RecGetPayload (p_hs_record, (uint32_t *)&hs_payload_len);

        if ((!p_hs_payload) || (hs_payload_len < 7))
        {
            NXPLOG_API_E ("%s: Failed to get Hs payload (version, cr/ac record)", __FUNCTION__);
            return -1;
        }

        /* Version */
        if (NFC_FORUM_HANDOVER_VERSION != p_hs_payload[0])
        {
            NXPLOG_API_E ("%s: Version (0x%02x) not matched", __FUNCTION__, p_hs_payload[0]);
            return -1;
        }
        p_hs_payload += 1;
        hs_payload_len--;

        /* NDEF message for Collision Resolution record and Alternative Carrier records */
        if (NDEF_OK != NDEF_MsgValidate (p_hs_payload, hs_payload_len, FALSE))
        {
            NXPLOG_API_E ("%s: Failed to validate NDEF message for cr/ac records", __FUNCTION__);
            return -1;
        }
    }
    else
    {
        NXPLOG_API_E ("%s: Hs record not found", __FUNCTION__);
        return -1;
    }

    p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                          BT_OOB_REC_TYPE, BT_OOB_REC_TYPE_LEN);

    if (p_record)
    {
        status = 0;
        NXPLOG_API_D ("%s: Found BT OOB record");
        if (p_hs_record)
        {
            p_id = NDEF_RecGetId(p_record, &id_len);
            if (p_id == NULL || id_len == 0)
            {
                NXPLOG_API_E ("%s: Failed to retreive NDEF ID", __FUNCTION__);
                return -1;
            }
        
            if (getDeviceCps(p_hs_payload, p_id, id_len, &hsInfo->bluetooth.power_state)!=NFA_STATUS_OK)
            {
                hsInfo->bluetooth.power_state = HANDOVER_CPS_UNKNOWN;
            }
        }
        p_payload = NDEF_RecGetPayload(p_record, (uint32_t *)&record_payload_len);
        if (p_payload == NULL)
        {
            NXPLOG_API_E ("%s: Failed to retreive NDEF payload", __FUNCTION__);
            return -1;
        }
        hsInfo->bluetooth.type = HANDOVER_TYPE_BT;
        hsInfo->bluetooth.ndef = p_record;
        hsInfo->bluetooth.ndef_length = record_payload_len;
        index = 2;
        if (parseBluetoothAddress(&p_payload[index] , record_payload_len - index, hsInfo->bluetooth.address)!= 0)
        {
            NXPLOG_API_E ("%s: Failed to retreive device address", __FUNCTION__);
            return -1;
        }
        index += BLUETOOTH_ADDRESS_LENGTH;
        while(index < record_payload_len)
        {
            bt_len = p_payload[index++];
            bt_type = p_payload[index++];
            switch (bt_type)
            {
                case BT_HANDOVER_TYPE_SHORT_LOCAL_NAME:
                    hsInfo->bluetooth.device_name = &p_payload[index];
                    hsInfo->bluetooth.device_name_length = bt_len-1;
                    break;
                case BT_HANDOVER_TYPE_LONG_LOCAL_NAME:
                    if (hsInfo->bluetooth.device_name)
                    {
                        break;  // prefer short name
                    }
                    hsInfo->bluetooth.device_name = &p_payload[index];
                    hsInfo->bluetooth.device_name_length = bt_len-1;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                              BLE_OOB_REC_TYPE, BT_OOB_REC_TYPE_LEN);

        if (p_record)
        {
            status = 0;
            NXPLOG_API_D ("%s: Found BLE OOB record", __FUNCTION__);
            if (p_hs_record)
            {
                p_id = NDEF_RecGetId(p_record, &id_len);
                if (p_id == NULL || id_len == 0)
                {
                    NXPLOG_API_E ("%s: Failed to retreive NDEF ID", __FUNCTION__);
                    return -1;
                }
            
                if (getDeviceCps(p_hs_payload, p_id, id_len, &hsInfo->bluetooth.power_state)!=NFA_STATUS_OK)
                {
                    hsInfo->bluetooth.power_state = HANDOVER_CPS_UNKNOWN;
                }
            }
            p_payload = NDEF_RecGetPayload(p_record, (uint32_t *)&record_payload_len);
            if (p_payload == NULL)
            {
                NXPLOG_API_E ("%s: Failed to retreive NDEF payload", __FUNCTION__);
                return -1;
            }
            hsInfo->bluetooth.type = HANDOVER_TYPE_BLE;
            hsInfo->bluetooth.ndef = p_record;
            hsInfo->bluetooth.ndef_length = record_payload_len;
            index = 0;
            while(index < record_payload_len)
            {
                bt_len = p_payload[index++];
                bt_type = p_payload[index++];
                switch (bt_type)
                {
                    case BT_HANDOVER_TYPE_MAC:
                        parseBluetoothAddress(&p_payload[index] , record_payload_len - index, hsInfo->bluetooth.address);
                        break;
                    case BT_HANDOVER_TYPE_SHORT_LOCAL_NAME:
                        hsInfo->bluetooth.device_name = &p_payload[index];
                        hsInfo->bluetooth.device_name_length = bt_len-1;
                        break;
                    case BT_HANDOVER_TYPE_LONG_LOCAL_NAME:
                        if (hsInfo->bluetooth.device_name)
                        {
                            break;  // prefer short name
                        }
                        hsInfo->bluetooth.device_name = &p_payload[index];
                        hsInfo->bluetooth.device_name_length = bt_len-1;
                        break;
                    default:
                        break;
                }
            }        
        }
    }
    p_record = NDEF_MsgGetFirstRecByType (ndefBuff, NDEF_TNF_MEDIA,
                                          WIFI_WSC_REC_TYPE, WIFI_WSC_REC_TYPE_LEN);

    if (p_record)
    {
        status = 0;
        NXPLOG_API_D ("%s: Found WiFi record", __FUNCTION__);
        if (p_hs_record)
        {
            p_id = NDEF_RecGetId(p_record, &id_len);
            if (p_id == NULL || id_len == 0)
            {
                NXPLOG_API_E ("%s: Failed to retreive NDEF ID", __FUNCTION__);
                return -1;
            }
        
            if (getDeviceCps(p_hs_payload, p_id, id_len, &hsInfo->wifi.power_state)!=NFA_STATUS_OK)
            {
                hsInfo->wifi.power_state = HANDOVER_CPS_UNKNOWN;
            }
        }
        p_payload = NDEF_RecGetPayload(p_record, (uint32_t *)&record_payload_len);
        if (p_payload == NULL)
        {
            NXPLOG_API_E ("%s: Failed to retreive NDEF payload", __FUNCTION__);
            return -1;
        }
        hsInfo->wifi.ndef = p_record;
        hsInfo->wifi.ndef_length = record_payload_len;
        index = 0;
        while(index < record_payload_len)
        {
            /* wifi type is a 2 byte field*/
            wifi_type = p_payload[index++];
            wifi_type = wifi_type << 8;
            wifi_type += p_payload[index++];

            /* wifi len is a 2 byte field */
            wifi_len = p_payload[index++];
            wifi_len = wifi_len << 8;
            wifi_len += p_payload[index++];

            switch (wifi_type)
            {
                case WIFI_HANDOVER_SSID_ID:
                    hsInfo->wifi.ssid_length = wifi_len;
                    hsInfo->wifi.ssid = &p_payload[index];
                    index += hsInfo->wifi.ssid_length;
                    break;
                case WIFI_HANDOVER_NETWORK_KEY_ID:
                    hsInfo->wifi.key_length = wifi_len;
                    hsInfo->wifi.key = &p_payload[index];
                    index += hsInfo->wifi.key_length;
                    break;
                default:
                    break;
            }
        }
    }
    return status;
}

