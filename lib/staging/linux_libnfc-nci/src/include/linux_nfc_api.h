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

#ifndef __LINUX_NFC_API__H__
#define __LINUX_NFC_API__H__

#include "linux_nfc_api_compatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Name strings for target types. */
#define TARGET_TYPE_UNKNOWN               -1
#define TARGET_TYPE_ISO14443_3A           1
#define TARGET_TYPE_ISO14443_3B           2
#define TARGET_TYPE_ISO14443_4            3
#define TARGET_TYPE_FELICA                4
#define TARGET_TYPE_ISO15693              5
#define TARGET_TYPE_NDEF                  6
#define TARGET_TYPE_NDEF_FORMATABLE       7
#define TARGET_TYPE_MIFARE_CLASSIC        8
#define TARGET_TYPE_MIFARE_UL             9
#define TARGET_TYPE_KOVIO_BARCODE         10
#define TARGET_TYPE_ISO14443_3A_3B        11

/* Definition for listen modes */
#define MODE_LISTEN_A                     0x80
#define MODE_LISTEN_B                     0x81
#define MODE_LISTEN_F                     0x82

/* Definitions for TECHNOLOGY_MASK */
#define DEFAULT_NFA_TECH_MASK             (-1)
/** NFC Technology A             */
#define NFA_TECHNOLOGY_MASK_A           0x01
/** NFC Technology B             */
#define NFA_TECHNOLOGY_MASK_B           0x02
/** NFC Technology F             */
#define NFA_TECHNOLOGY_MASK_F           0x04
/** Proprietary Technology       */
#define NFA_TECHNOLOGY_MASK_ISO15693    0x08
/** Proprietary Technology       */
#define NFA_TECHNOLOGY_MASK_KOVIO       0x20
/** NFC Technology A active mode */
#define NFA_TECHNOLOGY_MASK_A_ACTIVE    0x40
/** NFC Technology F active mode */
#define NFA_TECHNOLOGY_MASK_F_ACTIVE    0x80
/** All supported technologies   */
#define NFA_TECHNOLOGY_MASK_ALL         0xFF

/* NDEF Type Name Format */
/** Empty (type/id/payload len =0) */
#define NDEF_TNF_EMPTY          0
/** NFC Forum well-known type/RTD */
#define NDEF_TNF_WELLKNOWN      1
/** Media-type as defined in RFC 2046 */
#define NDEF_TNF_MEDIA          2
/** Absolute URI as defined in RFC 3986 */
#define NDEF_TNF_URI            3
/** NFC Forum external type/RTD */
#define NDEF_TNF_EXT            4
/** Unknown (type len =0) */
#define NDEF_TNF_UNKNOWN        5
/** Unchanged (type len =0) */
#define NDEF_TNF_UNCHANGED      6

/* Supported Protocols */
#define NFA_PROTOCOL_UNKNOWN            0x00
#define NFA_PROTOCOL_T1T                0x01
#define NFA_PROTOCOL_T2T                0x02
#define NFA_PROTOCOL_T3T                0x03
#define NFA_PROTOCOL_ISO_DEP            0x04
#define NFA_PROTOCOL_15693              0x06
#define NFA_PROTOCOL_MIFARE             0x80
typedef unsigned char tNFC_PROTOCOL;

/**
 *  \brief setting this flag allows host application ignore
 *  an NDEF check command from reader
 */
#define FLAG_HCE_SKIP_NDEF_CHECK    0x80
/**
 *  \brief setting this flag allows host application enable HCE
 */
#define FLAG_HCE_ENABLE_HCE         0x01
void InitializeLogLevel(void);
/**
 *  \brief friendly NDEF Type Name
 */
typedef enum {
    /**
     * \brief NDEF text: NFC Forum well-known type + RTD: 0x55
     */
    NDEF_FRIENDLY_TYPE_TEXT = 0,
    /*
     * \brief NDEF text: NFC Forum well-known type + RTD: 0x54
     */
    NDEF_FRIENDLY_TYPE_URL = 1,
    /*
     * \brief Handover Select package
     */
    NDEF_FRIENDLY_TYPE_HS = 2,
    /*
     * \brief Handover Request package
     */
    NDEF_FRIENDLY_TYPE_HR = 3,
    /*
     * \brief not able to decode directly
     */
    NDEF_FRIENDLY_TYPE_OTHER = 4,
}nfc_friendly_type_t;

/**
 *  \brief Bluetooth Handover Cofiguration Type Name
 */
typedef enum {
    /**
     * \brief indicates this is not a handover bluetooth record
     */
    HANDOVER_TYPE_UNKNOWN = 0,
    /**
     * \brief NFC Forum handover bluetooth record
     */
    HANDOVER_TYPE_BT,
    /**
     * \brief indicates NFC Forum handover BLE record
     */
    HANDOVER_TYPE_BLE,
}nfc_handover_bt_type_t;

/**
 *  \brief Handover Carrier Power State definitions
 */
typedef enum {
    /**
     *  \brief indicates the carrier is currently off
     */
    HANDOVER_CPS_INACTIVE = 0,
    /**
     *  \brief indicates the carrier is currently on
     */
    HANDOVER_CPS_ACTIVE = 1,
    /**
     *  \brief indicates the carrier is in the process of activation
     */
    HANDOVER_CPS_ACTIVATING = 2,
    /**
     *  \brief indicates the carrier power state is unknown
     */
    HANDOVER_CPS_UNKNOWN = 3,
}nfc_handover_cps_t;

/**
 * \brief NFC tag information structure definition.
 */
typedef struct
{
    /**
     *  \brief indicates the technology of tag
     */
    unsigned int technology;
    /**
     *  \brief the handle of tag
     */
    unsigned int handle;
    /**
     *  \brief the uid of tag
     */
    char uid[32];
    /**
     *  \brief the uid length
     */
    unsigned int uid_length;
    /**
     *  \brief activated protocol
     */
    tNFC_PROTOCOL protocol;

}nfc_tag_info_t;

/**
 * \brief NFC NDEF Message information structure definition.
 */
typedef struct
{
    /**
     *  \brief The flags to indicate if it contains ndef record.
     */
    int is_ndef;

    /**
     *  \brief Existing Ndef message length.
     */
    unsigned int current_ndef_length;

    /**
     *  \brief Max Ndef message length can be written.
     */
    unsigned int max_ndef_length;

    /**
     *  \brief The flags to indicate if writing a Ndef message is supprted.
     */
    int is_writable;
}ndef_info_t;

/**
 *  \brief NFC handover bluetooth record structure definition.
 */
typedef struct
{
    /**
     *  \brief Handover Carrier Power State
     */
    nfc_handover_cps_t power_state;
    /**
     *  \brief Handover bluetooth carrier configuration record type
     */
    nfc_handover_bt_type_t type;
    /**
     *  \brief Handover Carrier Configuration record
     */
    unsigned char *ndef;
    /**
     *  \brief Handover Carrier Configuration record length
     */
    unsigned int ndef_length;
    /**
     *  \brief Bluetooth address
     */
    unsigned char address[6];
    /**
     *  \brief Handover Carrier Data Reference name
     */
    unsigned char *device_name;
    /**
     *  \brief Handover Carrier Data Reference name length
     */
    unsigned int device_name_length;
}nfc_btoob_pairing_t, nfc_btoob_request_t;

/**
 *  \brief NFC wifi handover select record structure definition.
 */
typedef struct
{
    /**
     *  \brief Handover Carrier Power State
     */
    nfc_handover_cps_t power_state;
    /**
     *  \brief Handover Carrier Configuration record
     */
    unsigned char *ndef;
    /**
     *  \brief Handover Carrier Configuration record length
     */
    unsigned int ndef_length;
    /**
     *  \brief Wifi network ssid
     */
    unsigned char *ssid;
    /**
     *  \brief Wifi network ssid length
     */
    unsigned int ssid_length;
    /**
     *  \brief Wifi netwrok key
     */
    unsigned char *key;
    /**
     *  \brief Wifi netwrok key length
     */
    unsigned int key_length;
}nfc_wifi_pairing_t;

/**
 *  \brief NFC wifi handover request record structure definition.
 */
typedef struct
{
    /**
     *  \brief indicates it has wifi request
     */
    int has_wifi;
    /**
     *  \brief Handover Carrier Configuration record
     */
    unsigned char *ndef;
    /**
     *  \brief Handover Carrier Configuration record length
     */
    unsigned int ndef_length;
}nfc_wifi_request_t;

/**
 *  \brief NFC handover request record structure definition.
 */
typedef struct
{
    /**
     *  \brief Bluetooth Handover request
     */
    nfc_btoob_request_t bluetooth;
    /**
     *  \brief Wifi Handover request
     */
    nfc_wifi_request_t wifi;
}nfc_handover_request_t;

/**
 *  \brief NFC handover select record structure definition.
 */
typedef struct
{
    /**
     *  \brief Bluetooth Handover select
     */
    nfc_btoob_pairing_t bluetooth;
    /**
     *  \brief Wifi Handover select
     */
    nfc_wifi_pairing_t  wifi;
}nfc_handover_select_t;

/**
 * \brief NFC Tag callback function structure definition.
 */
typedef struct {
    /**
     * \brief NFC Tag callback function when tag is detected.
     * param pTagInfo       tag infomation
     */
    void (*onTagArrival) (nfc_tag_info_t *pTagInfo);

    /**
     * \brief NFC Tag callback function when tag is removed.
     */
    void (*onTagDeparture) (void);
}nfcTagCallback_t;

/**
 * \brief NFC SNEP server callback function structure definition.
 */
typedef struct {
    /**
     * \brief NFC Peer Device callback function when device is detected.
     */
    void (*onDeviceArrival) ();

    /**
     * \brief NFC Peer Device callback function when device is removed.
     */
    void (*onDeviceDeparture) (void);

    /**
     * \brief NFC Peer Device callback function when NDEF message is received from peer device.
     * \param message    NDEF message
     * \param length     NDEF message length
     */
    void (*onMessageReceived)(unsigned char *message, unsigned int length);
}nfcSnepServerCallback_t;

/**
 * \brief NFC LLCP connectionless server callback function structure definition.
 */
typedef struct {
    /**
     * \brief NFC Peer Device callback function when device is detected.
     */
    void (*onDeviceArrival) (void);

    /**
     * \brief NFC Peer Device callback function when device is removed.
     */
    void (*onDeviceDeparture) (void);

    /**
     * \brief NFC Peer Device callback function when NDEF message is received from peer device.
     */
    void (*onMessageReceived)(void);
}nfcllcpConnlessServerCallback_t;

/**
 * \brief NFC SNEP client callback function structure definition.
 */
typedef struct {
    /*
     * \brief NFC Peer Device callback function when device is detected.
     */
    void (*onDeviceArrival)();

    /**
     * \brief NFC Peer Device callback function when device is removed.
     */
    void (*onDeviceDeparture) (void);
}nfcSnepClientCallback_t, nfcllcpConnlessClientCallback_t;

/**
 *  \brief Host card emulation callback function structure definition.
 */
typedef struct {
    /**
     * \brief Host Card Emulation activation callback function.
     * \param mode      Activation technology
     */
    void (*onHostCardEmulationActivated) (unsigned char mode);

    /**
     * \brief Host Card Emulation de-activation callback function.
     */
    void (*onHostCardEmulationDeactivated) ();

    /**
     * \brief Apdu data callback function.
     * \param data      apdu data received from remote reader
     * \param data_length      apdu data length
     */
    void (*onDataReceived)(unsigned char *data, unsigned int data_length);
}nfcHostCardEmulationCallback_t;

/**
 *  \brief Handover callback functions structure definition.
 */
typedef struct {
    /**
     * \brief Handover Request callback function.\n
     *        Use ndef_readHandoverRequestInfo() to parse the message
     * \param msg      Handover request NDEF message from remote device
     * \param length   Handover request NDEF message length
     */
   void (*onHandoverRequestReceived)(unsigned char *msg, unsigned int length);
    /**
     * \brief Handover Select callback function.\n
     *        Use ndef_readHandoverSelectInfo() to parse the message
     * \param msg      Handover select NDEF message from remote device
     * \param length   Handover select NDEF message length
     */
   void (*onHandoverSelectReceived)(unsigned char *msg, unsigned int length);
}nfcHandoverCallback_t;

/**
* \brief read text message from NDEF data.
* \param ndef_buff:  the buffer with ndef message
* \param ndef_buff_length:  the length of buffer
* \param out_text:  the buffer to fill text in
* \param out_text_length:  the length of out_text buffer
* \return the length of ndef message if success, otherwise -1
*/
extern int ndef_readText(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_text, unsigned int out_text_length);

/**
* \brief read language code from NDEF Text type message
* \param ndef_buff:  the buffer with ndef message
* \param ndef_buff_length:  the length of buffer
* \param out_language_code:  the buffer to fill language code in
* \param out_language_code_length:  the length of out_language_code buffer
* \return the length of language code if success, otherwise -1
*/
extern int ndef_readLanguageCode(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_language_code, unsigned int out_language_code_length);

/**
* \brief read uri message from NDEF data.
* \param ndef_buff:  the buffer with ndef message
* \param ndef_buff_length:  the length of buffer
* \param out_url:  the buffer to fill url in
* \param out_url_length:  the length of out_url buffer
* \return the length of ndef message if success, otherwise -1
*/
extern int ndef_readUrl(unsigned char *ndef_buff, unsigned int ndef_buff_length, char * out_url, unsigned int out_url_length);

/**
* \brief read handover select message from NDEF data.
* \param ndef_buff:  the buffer with handover ndef message
* \param ndef_buff_length:  the length of handover ndef message
* \param info:  handover select information to be filled
* \return 0 if success, otherwise failed.
*/
extern int ndef_readHandoverSelectInfo(unsigned char *ndef_buff, unsigned int ndef_buff_length, nfc_handover_select_t *info);

/**
* \brief read handover request message from NDEF data.
* \param ndef_buff:  the buffer with handover ndef message
* \param ndef_buff_length:  the length of handover ndef message
* \param info:  handover request information to be filled
* \return 0 if success, otherwise failed.
*/
extern int ndef_readHandoverRequestInfo(unsigned char *ndef_buff, unsigned int ndef_buff_length, nfc_handover_request_t *info);

/**
* \brief Create a new NDEF Record containing a URI.
* \param uri:  the uri to be written
* \param out_ndef_buff:  the buffer to store ndef message
* \param out_ndef_buff_length:  the length of ndef buffer
* \return the length of NDEF buffer be used.
*/
extern int ndef_createUri(char *uri, unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length);

/**
* \brief Create a new NDEF Record containing a text.
* \param language_code:  language encoding code
* \param text:  text to be written
* \param out_ndef_buff:  the buffer to store ndef message
* \param out_ndef_buff_length:  the length of ndef buffer
* \return the length of NDEF buffer be used.
*/
extern int ndef_createText(char *language_code, char *text,
                                                    unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length);

/**
* \brief Create a new NDEF Record containing MIME data
* \param mime_type:    a valid MIME type
* \param mime_data:  MIME data as bytes array
* \param mime_data_length:  MIME data length
* \param out_ndef_buff:  the buffer to store ndef message
* \param out_ndef_buff_length:  the length of ndef buffer
* \return the length of NDEF buffer be used.
*/
extern int ndef_createMime(char *mime_type, unsigned char *mime_data, unsigned int mime_data_length,
                                                    unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length);

/**
* \brief Create a new NDEF Record containing Handover Select message
* \param cps:    Carrier power state
* \param carrier_data_ref:  carrier data reference name
* \param ndef_buff:  carrier configuration record
* \param ndef_buff_length:  carrier configuration record length
* \param out_ndef_buff:  the buffer to store handover select message
* \param out_ndef_buff_length:  the length of handover select message
* \return the length of NDEF buffer be used.
*/
extern int ndef_createHandoverSelect(nfc_handover_cps_t cps, char *carrier_data_ref,
                                unsigned char *ndef_buff, unsigned int ndef_buff_length, unsigned char *out_ndef_buff, unsigned int out_ndef_buff_length);

/**
* \brief Check if the tag is Ndef formated.
* \param handle:  handle to the tag.
* \param info:           information about tag to be retreived
* \return 1 with info if it is Ndef tag, otherwise 0.
*
*/
extern int nfcTag_isNdef(unsigned int handle, ndef_info_t *info);


/**
* \reconnect to the tag
* \get protocol parameter sCurrentConnectedTargetProtocol updated.
* \reselect the tag
* \return 0 if reselection is success.
*
*/
extern int nfcTag_doHandleReconnect(unsigned int handle);

/**
* \brief Read ndef message from tag.
* \param handle:  handle to the tag.
* \param ndef_buffer:  the buffer to be filled with ndef message
* \param ndef_buffer_length:  the length of buffer
* \param friendly_ndef_type:  the friendly ndef type of ndef message
* \return the length of ndef message if success, otherwise -1.
*
*/
extern int nfcTag_readNdef(unsigned int handle, unsigned char *ndef_buffer,  unsigned int ndef_buffer_length, nfc_friendly_type_t *friendly_ndef_type);

/**
* \brief Write ndef message to tag.
* \param handle:  handle to the tag.
* \param ndef_buffer:  the buffer with ndef message
* \param ndef_buffer_length:  the length of buffer
* \return if success, otherwise failed.
*/
extern int nfcTag_writeNdef(unsigned int handle, unsigned char *ndef_buffer, unsigned int ndef_buffer_length);

/**
* \brief Check if the tag is Ndef formatable.
* \param handle:  handle to the tag.
* \return 1 with info if it is Ndef tag Formattable, otherwise 0.
*
*/
extern int nfcTag_isFormatable(unsigned int handle);

/**
* \brief Format the NDEF Formatable Tag.
* \param handle:  handle to the tag.
* \return if success, otherwise failed.
*
*/
extern int nfcTag_formatTag(unsigned int handle);


/**
* \brief Make the tag read-only.
* \param handle:  handle to the tag.
* \return 0 if success, otherwise failed.
*/
extern int nfcTag_makeReadOnly(unsigned int handle, unsigned char *key, unsigned char key_length);

/**
* \brief Switch RF interface for ISO-DEP and MifareClassic tag.
* \param handle:  handle to the tag.
* \param is_frame_rf:  indicates if the target RF interface is Frame RF or not.
* \return 0 if success, otherwise failed.
*/
extern int nfcTag_switchRF(unsigned int handle, int is_frame_rf);

/**
* \brief Send raw command to tag.
* \param handle:  handle to the tag.
* \param tx_buffer:  the buffer to be sent
* \param tx_buffer_length:  the length of send buffer
* \param rx_buffer:  the receive buffer to be filled
* \param rx_buffer_length:  the length of receive buffer
* \param timeout:  the timeout value in milliseconds
* \return the real length of data received or 0 if failed.
*/
extern int nfcTag_transceive (unsigned int handle, unsigned char *tx_buffer, int tx_buffer_length, unsigned char* rx_buffer, int rx_buffer_length, unsigned int timeout);



/**
* \brief initialize nfc stack.
* \return 0 if success, otherwise failed.
*/
extern int doInitialize ();

/**
* \brief de-initialize nfc stack.
* \return 0 if success, otherwise failed.
*/
extern int doDeinitialize ();

/*
* \brief Check if nfc stack is running or not.
* \return 1 if running, otherwise 0.
*/
extern int isNfcActive();

/**
* \brief Start nfc discovery.
* \param technologies_masks:  Nfc technology mask.
* \param reader_only_mode:  indicates if enable reader only mode. (Means no P2P or HCE)
* \param enable_host_routing:  indicates if enable host card emualtion
* \param restart:  indicates if force restart discovery
* \return 0 if success, otherwise failed.
*/
extern void doEnableDiscovery (int technologies_masks,
                        int reader_only_mode, int enable_host_routing, int restart);

/**
* \brief Stop polling and listening for devices.
* \return None
*/
extern void disableDiscovery ();

/**
* \brief Register a tag callback functions.
* \param callback:  tag callback functions.
* \return None
*/
extern void registerTagCallback(nfcTagCallback_t *callback);

/**
* \brief Deregister a tag callback functions.
* \return None
*/
extern void deregisterTagCallback();

/**
* \brief Select the next Tag present in the Field.
* \return 0 if success, otherwise failed.
*/
extern int selectNextTag(void);

/**
* \brief To get the number of Tags Discovered.
* \return Number of Tags Discovered
*/
extern int getNumTags(void);

/**
* \brief check the next valid protocol.
* \return Index of the next tag protocol,
*           -1 if no valid index
*/
extern int checkNextProtocol(void);

/**
* \brief Return FW version.
* \return FW version on chip, return 0 if fails.
*/
extern int getFwVersion();

/**
* \brief Register a callback functions for snep client.
* \param client_callback:  snep client callback functions.
* \return 0 if success
*/
extern int nfcSnep_registerClientCallback(nfcSnepClientCallback_t *client_callback);

/**
* \brief Deregister a callback functions for snep client.
* \return None
*/
extern void nfcSnep_deregisterClientCallback();

/**
* \brief Start a snep server to receive snep message.
* \param server_callback:  snep server callback functions.
* \return 0 if success
*/
extern int nfcSnep_startServer(nfcSnepServerCallback_t *server_callback);

/**
* \brief Stop senp server.
* \return None
*/
extern void nfcSnep_stopServer();

/**
* \brief put a snep message to remote snep server.
* \param msg:  snep message.
* \param length:  snep message length.
* \return 0 if success, otherwise failed.
*/
extern int nfcSnep_putMessage(unsigned char* msg, unsigned int length);

/**
* \brief Register a callback functions for host card emulation.
* \param callback:  host card emualtion callback functions.
* \return None
*/
extern void nfcHce_registerHceCallback(nfcHostCardEmulationCallback_t *callback);

/**
* \brief Deregister the host card emulation callback.
* \return None
*/
extern void nfcHce_deregisterHceCallback();

/**
* \brief Send Apdu to remote reader.
* \param command:  apdu package to be sent.
* \param command_length: apdu package length
* \return 0 if success, otherwise failed.
*/
extern int nfcHce_sendCommand(unsigned char* command, unsigned int command_length);

/**
* \brief register T3T identifier
* \param id:  T3T identifier value.
* \param length:  T3T identifier size.
* \return 0 if success, otherwise failed.
*/
extern int nfcHce_registerT3tIdentifier (unsigned char *Id, unsigned char Idsize);

/**
* \brief Register the handover callback.
* \param callback:  handover callback functions.
* \return 0 if success, otherwise failed.
*/
extern int nfcHo_registerCallback(nfcHandoverCallback_t *callback);

/**
* \brief Deregister the handover callback.
* \return None
*/
extern void nfcHo_deregisterCallback();

/**
* \brief Send Handover Select Message to remote device.
* \param message:  handover Select message.
* \param length:  handover Select message length.
* \return 0 if success, otherwise failed.
*/
extern int nfcHo_sendSelectRecord(unsigned char *message, unsigned int length);

/**
* \brief Send Handover select error to remote device.
* \param reason:  error reason.
* \param data:  error data.
* \return 0 if success, otherwise failed.
*/
extern int nfcHo_sendSelectError(unsigned int reason, unsigned int data);

/**
* \brief Register a callback functions for LLCP client.
* \param client_callback:  LLCP client callback functions.
* \return 0 if success
*/
extern int nfcLlcp_ConnLessRegisterClientCallback(nfcSnepClientCallback_t *client_callback);

/**
* \brief Deregister a callback functions for LLCP client.
* \return None
*/
extern void nfcLlcp_ConnLessDeregisterClientCallback();

/**
* \brief Start a LLCP Connection Less server to receive LLCP message.
* \param server_callback:  LLCP connectionless server callback functions.
* \return 0 if success
*/
extern int nfcLlcp_ConnLessStartServer(nfcllcpConnlessServerCallback_t *server_callback);

/**
* \brief Stop LLCP server.
* \return None
*/
extern void nfcLlcp_ConnLessStopServer();

/**
* \brief Send a LLCP message to remote LLCP server.
* \param msg:  LLCP message.
* \param length:  LLCP message length.
* \return 0 if success, otherwise failed.
*/
extern int nfcLlcp_ConnLessSendMessage(unsigned char* msg, unsigned int length);

/**
* \brief Receive a LLCP message from remote LLCP server.
* \param msg:  LLCP message.
* \param length:  LLCP message length.
* \return 0 if success, otherwise failed.
*/
extern int nfcLlcp_ConnLessReceiveMessage(unsigned char* msg, unsigned int *length);

/**
* \brief Set NCI parameter
* \return return 0 if success.
*/
extern int nfcManager_setConfig(unsigned char id, unsigned char length, unsigned char* p_data);

/**
* \brief Write NDEF message in T4T NFCC in Wired Mode
* \return return 0 if success.
*/
extern int doWriteT4tData(unsigned char* command, unsigned char *ndef_buffer, int ndef_buffer_length);
/**
* \brief Read NDEF message in T4T NFCC in Wired Mode
* \return return 0 if success.
*/
extern int doReadT4tData(unsigned char* command, unsigned char *ndef_buffer, int *ndef_buffer_length);
#ifdef __cplusplus
}
#endif

#endif //__LINUX_NFC_API__H__
