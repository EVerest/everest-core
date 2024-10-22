/*
 * Copyright (C) 2015 The Android Open Source Project
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
 * NCI Interface
 */

#ifndef PHNCINFCTYPES_H
#define PHNCINFCTYPES_H

/*
 ################################################################################
 ***************************** Header File Inclusion
 ****************************
 ################################################################################
 */
#include <phNfcStatus.h>

/*
 ################################################################################
 ****************************** Macro Definitions
 *******************************
 ################################################################################
 */
#define PH_NCINFCTYPES_MAX_UID_LENGTH (0x0AU)
/* Maximum length of ATR_RES (General Bytes) length expected */
#define PH_NCINFCTYPES_MAX_ATR_LENGTH (0x30U)
#define PH_NCINFCTYPES_ATQA_LENGTH (0x02U) /* ATQA length */
#define PH_NCINFCTYPES_MAX_HIST_BYTES \
  (0x0FU) /* Max Historical bytes returned by Type A tag */

/*
 * Enum definition contains supported RF Protocols
 */
typedef enum {
  phNciNfc_e_RfProtocolsUnknownProtocol = 0x00, /* Protocol is not known */
  phNciNfc_e_RfProtocolsT1tProtocol = 0x01,     /* Type 1 Tag protocol */
  phNciNfc_e_RfProtocolsT2tProtocol = 0x02,     /* Type 2 Tag protocol */
  phNciNfc_e_RfProtocolsT3tProtocol = 0x03,     /* Type 3 Tag protocol */
  phNciNfc_e_RfProtocolsIsoDepProtocol = 0x04,  /* ISO DEP protocol */
  phNciNfc_e_RfProtocolsNfcDepProtocol = 0x05,  /* NFC DEP protocol */
  phNciNfc_e_RfProtocols15693Protocol = 0x06,   /* 15693 protocol */
  phNciNfc_e_RfProtocolsMifCProtocol = 0x80,    /* Mifare Classic protocol */
  phNciNfc_e_RfProtocolsHidProtocol = 0x81,     /* Hid protocol */
  phNciNfc_e_RfProtocolsEpcGen2Protocol = 0x82, /* EpcGen2 protocol */
  phNciNfc_e_RfProtocolsKovioProtocol = 0x83    /* Kovio protocol */
} phNciNfc_RfProtocols_t;

/*
 * Supported RF Interfaces
 */
typedef enum {
  phNciNfc_e_RfInterfacesNfceeDirect_RF = 0x00, /* Nfcee Direct RF Interface */
  phNciNfc_e_RfInterfacesFrame_RF = 0x01,       /* Frame RF Interface */
  phNciNfc_e_RfInterfacesISODEP_RF = 0x02,      /* ISO DEP RF Interface */
  phNciNfc_e_RfInterfacesNFCDEP_RF = 0x03,      /* NFC DEP RF Interface */
  phNciNfc_e_RfInterfacesTagCmd_RF = 0x80, /* Tag-Cmd RF Interface (Nxp prop) */
  phNciNfc_e_RfInterfacesHID_RF = 0x81     /* Hid RF Interface (Nxp prop) */
} phNciNfc_RfInterfaces_t;

/*
 * Enum definition contains RF technology modes supported.
 * This information is a part of RF_DISCOVER_NTF or RF_INTF_ACTIVATED_NTF.
 */
typedef enum {
  phNciNfc_NFCA_Poll = 0x00,        /* Nfc A Technology in Poll Mode */
  phNciNfc_NFCB_Poll = 0x01,        /* Nfc B Technology in Poll Mode */
  phNciNfc_NFCF_Poll = 0x02,        /* Nfc F Technology in Poll Mode */
  phNciNfc_NFCA_Active_Poll = 0x03, /* Nfc A Technology in Active Poll Mode */
  phNciNfc_NFCF_Active_Poll = 0x05, /* Nfc F Technology in Active Poll Mode */
  phNciNfc_NFCISO15693_Poll = 0x06, /* Nfc ISO15693 Technology in Poll Mode */
  phNciNfc_NxpProp_NFCHID_Poll = 0x70, /* Nfc Hid Technology in Poll Mode */
  phNciNfc_NxpProp_NFCEPFGEN2_Poll =
      0x71, /* Nfc EpcGen2 Technology in Poll Mode */
  phNciNfc_NxpProp_NFCKOVIO_Poll = 0x72, /* Nfc Kovio Technology in Poll Mode */
  phNciNfc_NFCA_Listen = 0x80,           /* Nfc A Technology in Listen Mode */
  phNciNfc_NFCB_Listen = 0x81,           /* Nfc B Technology in Listen Mode */
  phNciNfc_NFCF_Listen = 0x82,           /* Nfc F Technology in Listen Mode */
  phNciNfc_NFCA_Active_Listen =
      0x83, /* Nfc A Technology in Active Listen Mode */
  phNciNfc_NFCF_Active_Listen =
      0x85, /* Nfc F Technology in Active Listen Mode */
  phNciNfc_NFCISO15693_Active_Listen =
      0x86 /* Nfc ISO15693 Technology in Listen Mode */
} phNciNfc_RfTechMode_t;

/*
 * This is used to identify the exact device type
 */
typedef enum {
  phNciNfc_eUnknown_DevType = 0x00U,

  /* Generic PICC Type */
  phNciNfc_ePICC_DevType,
  /* Specific PICC Devices */
  /* This PICC type explains that the card is compliant to the
   * ISO 14443-1 and 2A specification. This type can be used for the
   * cards that is supporting these specifications
   */
  phNciNfc_eISO14443_A_PICC,
  /* This PICC type explains that the card is compliant to the
   * ISO 14443-4A specification
   */
  phNciNfc_eISO14443_4A_PICC,
  /* This PICC type explains that the card is compliant to the
   * ISO 14443-3A specification
   */
  phNciNfc_eISO14443_3A_PICC,
  /* This PICC type explains that the card is Mifare UL/1k/4k and
   * also it is compliant to ISO 14443-3A. There can also be other
   * ISO 14443-3A cards, so the phNciNfc_eISO14443_3A_PICC is also used for
   * PICC detection
   */
  phNciNfc_eMifareUL_PICC,
  phNciNfc_eMifare1k_PICC,
  phNciNfc_eMifare4k_PICC,
  phNciNfc_eMifareMini_PICC,
  /* This PICC type explains that the card is compliant to the
   * ISO 14443-1, 2 and 3B specification
   */
  phNciNfc_eISO14443_B_PICC,
  /* This PICC type explains that the card is compliant to the
   * ISO 14443-4B specification
   */
  phNciNfc_eISO14443_4B_PICC,
  /* This PICC type explains that the card is B-Prime type */
  phNciNfc_eISO14443_BPrime_PICC,
  phNciNfc_eFelica_PICC,
  phNciNfc_eJewel_PICC,
  /* This PICC type explains that the card is ISO15693 type */
  phNciNfc_eISO15693_PICC,
  /* This PICC type explains that the card is EpcGen2 type */
  phNciNfc_eEpcGen_PICC,

  /* NFC-IP1 Device Types */
  phNciNfc_eNfcIP1_Target,
  phNciNfc_eNfcIP1_Initiator,

  /* Other Sources */
  phNciNfc_eInvalid_DevType

} phNciNfc_RFDevType_t;

/*
 * RATS Response Params structure
 */
typedef struct phNciNfc_RATSResp {
  uint8_t bFormatByte;                              /* Format Byte */
  uint8_t bIByteTA;                                 /* Interface Byte TA(1) */
  uint8_t bIByteTB;                                 /* Interface Byte TB(1) */
  uint8_t bIByteTC;                                 /* Interface Byte TC(1) */
  uint8_t bHistByte[PH_NCINFCTYPES_MAX_HIST_BYTES]; /* Historical Bytes - Max
                                                       size 15 */
} phNciNfc_RATSResp_t;

/*
 * The Reader A structure includes the available information
 * related to the discovered ISO14443A remote device. This information
 * is updated for every device discovery.
 */
typedef struct phNciNfc_Iso14443AInfo {
  uint8_t Uid[PH_NCINFCTYPES_MAX_UID_LENGTH]; /* UID information of the TYPE A
                                              Tag Discovered NFCID1 -
                                              Considering max size of NFCID1*/
  uint8_t UidLength; /* UID information length, shall not be greater
                     than PHNCINFC_MAX_UID_LENGTH i.e., 10 */
  uint8_t AppData[PH_NCINFCTYPES_MAX_ATR_LENGTH]; /* Application data
                                              information of the tag discovered
                                              (= Historical bytes for type A) */
  uint8_t AppDataLength;                          /* Application data length */
  uint8_t Sak; /* SAK information of the TYPE ATag Discovered
               Mapped to SEL_RES Response*/
  uint8_t AtqA[PH_NCINFCTYPES_ATQA_LENGTH]; /* ATQA information of the TYPE A
                                       Tag Discovered */
  uint8_t MaxDataRate;           /* Maximum data rate supported by the TYPE A
                                 Tag Discovered */
  uint8_t Fwi_Sfgt;              /* Frame waiting time and start up frame guard
                                 time as defined in ISO/IEC 14443-4[7] for type A */
  uint8_t bSensResResp[2];       /* SENS_RES Response */
  uint8_t bSelResRespLen;        /* SEL_RES Response Length */
  uint8_t bRatsRespLen;          /* Length of RATS Response */
  phNciNfc_RATSResp_t tRatsResp; /* RATS Response Info */
} phNciNfc_Iso14443AInfo_t;

/*
 * The Remote Device Information Union includes the available Remote Device
 * Information structures. Following the device detected, the corresponding data
 * structure is used.
 */
typedef union phNciNfc_RemoteDevInfo {
  phNciNfc_Iso14443AInfo_t Iso14443A_Info; /* Type A tag Info */
} phNciNfc_RemoteDevInfo_t;

/* Contains Details of Discovered Target */
typedef struct phNciNfc_RemoteDevInformation {
  uint8_t SessionOpened; /* Flag indicating the validity of the handle of the
                            remote device. */
  phNciNfc_RFDevType_t
      RemDevType;                /* Remote device type which says that remote
                                  is Reader A or Reader B or NFCIP or Felica or
                                  Reader B Prime or Jewel*/
  uint8_t bRfDiscId;             /* ID of the Tag */
  phNciNfc_RfInterfaces_t eRfIf; /* RF Interface */
  phNciNfc_RfProtocols_t eRFProtocol; /* RF protocol of the target */
  phNciNfc_RfTechMode_t
      eRFTechMode; /* RF Technology mode of the discovered/activated target */
  uint8_t bMaxPayLoadSize;       /* Max data payload size*/
  uint8_t bInitialCredit;        /* Initial credit*/
  uint8_t bTechSpecificParamLen; /* Technology Specific parameter length, for
                                    Debugging purpose only*/
  phNciNfc_RfTechMode_t
      eDataXchgRFTechMode; /* Data Exchange RF Technology mode of the activated
                              target */
  uint8_t bTransBitRate;   /* Transmit Bit Rate */
  uint8_t bRecvBitRate;    /* Receive Bit Rate */
  phNciNfc_RemoteDevInfo_t
      tRemoteDevInfo; /* Structure object to #phNciNfc_RemoteDevInfo_t*/
} phNciNfc_RemoteDevInformation_t,
    *pphNciNfc_RemoteDevInformation_t; /* Pointer to Remote Dev Info*/

/*
 * Structure contains buffer where payload of the received data packet
 * shall be stored and length of the payload stored in the buffer.
 */
typedef struct phNciNfc_Data {
  uint8_t* pBuff; /* Buffer to store received data packet's payload */
  uint16_t wLen;  /* Length of the payload */
} phNciNfc_Data_t, *pphNciNfc_Data_t;

/*
 * Type 2 tag command list supported by NCI stack
 * It includes command lists applicable to Mifare family cards also
 */
typedef enum phNciNfc_T2TCmdList {
  phNciNfc_eT2TRaw = 0x00, /* Performs Raw communication over T2T Tag*/
  phNciNfc_eT2TWriteN,     /* Write Multiple blocks to T2T tag*/
  phNciNfc_eT2TreadN,      /* Read Multiple blocks to T2T tag*/
  phNciNfc_eT2TSectorSel,  /* Sector Select for MifareStd Cards*/
  phNciNfc_eT2TAuth,       /* Sector Select for MifareStd Cards*/
  phNciNfc_eT2TProxCheck,  /* Proxy Check command for MF+*/
  phNciNfc_eT2TInvalidCmd  /* Invalid Command*/
} phNciNfc_T2TCmdList_t;   /* Type2 Tag and Mifare specicific command list*/

/* All command list for tag operation supported by NCI stack */
typedef union phNciNfc_TagCmdList {
  phNciNfc_T2TCmdList_t T2TCmd; /* T2T Specific command*/
} phNciNfc_TagCmdList_t;        /* Tag specific command */

/* Transceive info */
typedef struct phNciNfc_TransceiveInfo {
  phNciNfc_TagCmdList_t uCmd; /* Technology Specific commands */
  uint8_t bAddr; /* Start address to perform operation,Valid for T1T T2T T3T and
                    some Propriatery tags */
  uint8_t bNumBlock;         /* Number of blocks */
  uint16_t wTimeout;         /* Timeout value to be used during transceive */
  phNciNfc_Data_t tSendData; /* Buffer information for sending data */
  phNciNfc_Data_t tRecvData; /* Buffer information for receiving data */
  /* Details for Felica To be Added if Check and Update supported */
} phNciNfc_TransceiveInfo_t,
    *pphNciNfc_TransceiveInfo_t; /* pointer to struct #phNciNfc_TransceiveInfo_t
                                  */

#endif /* end of #ifndef PHNCINFCTYPES_H */
