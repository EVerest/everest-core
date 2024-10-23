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
 * NFC Ndef Mapping For Different Smart Cards.
 *
 */

#ifndef PHFRINFC_NDEFMAP_H
#define PHFRINFC_NDEFMAP_H

/*include files*/
#include <phFriNfc.h>
#include <phNfcStatus.h>
#include <phNfcTypes.h>
#include <phNfcTypes_Mapping.h>

/*  NDEF Mapping Component
 *
 *  This component implements the read/write/check NDEF functions for remote
 * devices. NDEF data, as defined by the NFC Forum NDEF specification are
 * written to or read from a remote device that can be a smart- or memory card.
 *  Please notice that the NDEF mapping command sequence must
 *  be contiguous (after correct initialization)
 *
 */

/*
 * NDEF Mapping - specifies the different card types
 * These are the only recognized card types in this version.
 *
 */

#define PH_FRINFC_NDEFMAP_MIFARE_STD_1K_CARD 7  /* Mifare Standard */
#define PH_FRINFC_NDEFMAP_MIFARE_STD_4K_CARD 8  /* Mifare Standard */
#define PH_FRINFC_NDEFMAP_MIFARE_STD_2K_CARD 11 /*internal Mifare Standard */
#define PH_FRINFC_NDEFMAP_EMPTY_NDEF_MSG \
  { 0xD0, 0x00, 0x00 }                      /* Empty ndef message */
#define PH_FRINFC_NDEFMAP_MFUL_4BYTES_BUF 4 /* To store 4 bytes after write */

/* Enum represents the different card state*/
typedef enum {
  PH_NDEFMAP_CARD_STATE_INITIALIZED,
  PH_NDEFMAP_CARD_STATE_READ_ONLY,
  PH_NDEFMAP_CARD_STATE_READ_WRITE,
  PH_NDEFMAP_CARD_STATE_INVALID
} phNDEF_CARD_STATE;

/*
 * NDEF Mapping - specifies the Compliant Blocks in the Mifare 1k and 4k card
 * types
 *
 */
#define PH_FRINFC_NDEFMAP_MIFARESTD_1KNDEF_COMPBLOCK \
  45 /* Total Ndef Compliant blocks Mifare 1k */
#define PH_FRINFC_NDEFMAP_MIFARESTD_2KNDEF_COMPBLOCK \
  90 /* Total Ndef Compliant blocks Mifare 2k */
#define PH_FRINFC_NDEFMAP_MIFARESTD_4KNDEF_COMPBLOCK \
  210 /* Total Ndef Compliant blocks Mifare 4k */
#define PH_FRINFC_NDEFMAP_MIFARESTD_RDWR_SIZE \
  16 /* Bytes read/write for one read/write operation*/
#define PH_FRINFC_NDEFMAP_MIFARESTD_TOTALNO_BLK \
  40 /* Total number of sectors in Mifare 4k */
#define PH_FRINFC_NDEFMAP_MIFARESTD_ST15_BYTES \
  15 /* To store 15 bytes after reading a block */

/*
 * Completion Routine Indices
 *
 * These are the indices of the completion routine pointers within the component
 * context. Completion routines belong to upper components.
 *
 */
#define PH_FRINFC_NDEFMAP_CR_CHK_NDEF 0    /* */
#define PH_FRINFC_NDEFMAP_CR_RD_NDEF 1     /* */
#define PH_FRINFC_NDEFMAP_CR_WR_NDEF 2     /* */
#define PH_FRINFC_NDEFMAP_CR_ERASE_NDEF 3  /* */
#define PH_FRINFC_NDEFMAP_CR_INVALID_OPE 4 /* */
#define PH_FRINFC_NDEFMAP_CR 5             /* */

/*
 * File Offset Attributes
 *
 * Following values are used to determine the offset value for Read/Write. This
 * specifies whether the Read/Write operation needs to be restarted/continued
 * from the last offset set.
 *
 */
/* Read/Write operation shall start from the last offset set */
#define PH_FRINFC_NDEFMAP_SEEK_CUR 0 /* */
/* Read/Write operation shall start from the beginning of the file/card */
#define PH_FRINFC_NDEFMAP_SEEK_BEGIN 1 /* */

/* Read operation invalid */
#define PH_FRINFC_NDEFMAP_SEEK_INVALID 0xFF /* */

/*
 * Buffer Size Definitions
 *
 */
/* Minimum size of the TRX buffer required */
#define PH_FRINFC_NDEFMAP_MAX_SEND_RECV_BUF_SIZE 252 /* */
/* The size of s MIFARE block */
#define PH_FRINFC_NDEFMAP_MF_READ_BLOCK_SIZE 16 /* */

typedef struct phFriNfc_MifareStdCont {
  /* to store bytes that will be used in the next write/read operation, if any
   */
  uint8_t internalBuf[PH_FRINFC_NDEFMAP_MIFARESTD_ST15_BYTES];
  /* to Store the length of the internalBuf */
  uint16_t internalLength;
  /* holds the block number which is presently been used */
  uint8_t currentBlock;
  /* the number of Ndef compliant blocks written/read */
  uint8_t NdefBlocks;
  /* Total Number of Ndef compliant Blocks */
  uint16_t NoOfNdefCompBlocks;
  /* used in write ndef, to know that internal byte are accessed */
  uint8_t internalBufFlag;
  /* used in write ndef, to know that last 16 bytes are used to write*/
  uint8_t RemainingBufFlag;
  /* indicates that Read has reached the end of the card */
  uint8_t ReadWriteCompleteFlag;
  /* indicates that Read has reached the end of the card */
  uint8_t ReadCompleteFlag;
  /* indicates that Write is possible or not */
  uint8_t WriteFlag;
  /* indicates that Write is possible or not */
  uint8_t ReadFlag;
  /* indicates that Write is possible or not */
  uint8_t RdBeforeWrFlag;
  /* Authentication Flag indicating that a particular sector is authenticated or
   * not */
  uint8_t AuthDone;
  /* to store the last Sector ID in Check Ndef */
  uint8_t SectorIndex;
  /* to read the access bits of each sector */
  uint8_t ReadAcsBitFlag;
  /* Flag to check if Acs bit was written in this call */
  uint8_t WriteAcsBitFlag;
  /* Buffer to store 16 bytes */
  uint8_t Buffer[PH_FRINFC_NDEFMAP_MIFARESTD_RDWR_SIZE];
  /* to store the AIDs of Mifare 1k or 4k */
  uint8_t aid[PH_FRINFC_NDEFMAP_MIFARESTD_TOTALNO_BLK];
  /* flag to write with offset begin */
  uint8_t WrNdefFlag;
  /* flag to read with offset begin */
  uint8_t ReadNdefFlag;
  /* flag to check with offset begin */
  uint8_t ChkNdefFlag;
  /* To store the remaining size of the Mifare 1k or 4k card */
  uint16_t remainingSize;
  /* To update the remaining size when writing to the Mifare 1k or 4k card */
  uint8_t remSizeUpdFlag;
  /* The flag is to know that there is a different AID apart from NFC forum
   * sector AID */
  uint16_t aidCompleteFlag;
  /* The flag is to know that there is a a NFC forum sector exists in the card
   */
  uint16_t NFCforumSectFlag;
  /* The flag is to know that the particular sector is a proprietary NFC forum
   * sector */
  uint16_t ProprforumSectFlag;
  /* The flag is set after reading the MAD sectors */
  uint16_t ChkNdefCompleteFlag;
  /* Flag to store the current block */
  uint8_t TempBlockNo;
  /* Completion routine index */
  uint8_t CRIndex;
  /* Bytes remaining to write for one write procedure */
  uint16_t WrLength;
  /* Flag to read after write */
  uint8_t RdAfterWrFlag;
  /* Flag to say that poll is required before write ndef (authentication) */
  uint8_t PollFlag;
  /* Flag is to know that this is first time the read has been called. This
  is required when read is called after write (especially for the card formatted
  with the 2nd configuration) */
  uint8_t FirstReadFlag;
  /* Flag is to know that this is first time the write has been called. This
  is required when the card formatted with the 3rd configuration */
  uint8_t FirstWriteFlag;
  /* Indicates the sector trailor id  for which the convert
      to read only is currently in progress*/
  uint8_t ReadOnlySectorIndex;
  /* Indicates the total number of sectors on the card  */
  uint8_t TotalNoSectors;
  /* Indicates the block number of the sector trailor on the card  */
  uint8_t SectorTrailerBlockNo;
  /* Secret key B to given by the application */
  uint8_t UserScrtKeyB[6];
} phFriNfc_MifareStdCont_t;

/*
 *  NDEF TLV structure which details the different variables used for TLV.
 *
 */
typedef struct phFriNfc_NDEFTLVCont {
  /* Flag is to know that the TLV Type Found */
  uint8_t NdefTLVFoundFlag;
  /* Sector number of the next/present available TLV */
  uint8_t NdefTLVSector;
  /* Following two variables are used to store the
      T byte and the Block number in which the T is
      found in Tag */
  /* Byte number of the next/present available TLV */
  uint16_t NdefTLVByte;
  /* Block number of the next/present available TLV */
  uint8_t NdefTLVBlock;
  /* Authentication flag for NDEF TLV Block */
  uint8_t NdefTLVAuthFlag;
  /* if the 16th byte of the last read is type (T) of TLV
      and next read contains length (L) bytes of TLV. This flag
      is set when the type (T) of TLV is found in the last read */
  uint8_t TcheckedinTLVFlag;
  /* if the 16th byte of the last read is Length (L) of TLV
      and next read contains length (L) bytes of TLV. This flag
      is set when the Length (L) of TLV is found in the last read */
  uint8_t LcheckedinTLVFlag;
  /* This flag is set, if Terminator TLV is already written
      and next read contains value (V) bytes of TLV. This flag
      is set when the value (V) of TLV is found in the last read */
  uint8_t SetTermTLVFlag;
  /* To know the number of Length (L) field is present in the
      next block */
  uint8_t NoLbytesinTLV;
  /* The value of 3 bytes length(L) field in TLV. In 3 bytes
      length field, 2 bytes are in one block and other 1 byte
      is in the next block. To store the former block length
      field value, this variable is used */
  uint16_t prevLenByteValue;
  /* The value of length(L) field in TLV. */
  uint16_t BytesRemainLinTLV;
  /* Actual size to read and write. This will be always equal to the
      length (L) of TLV as there is only one NDEF TLV . */
  uint16_t ActualSize;
  /* Flag is to write the length (L) field of the TLV */
  uint8_t WrLenFlag;
  /* Flag is to write the length (L) field of the TLV */
  uint16_t NULLTLVCount;
  /* Buffer to store 4 bytes of data which is written to a block */
  uint8_t NdefTLVBuffer[PH_FRINFC_NDEFMAP_MFUL_4BYTES_BUF];
  /* Buffer to store 4 bytes of data which is written to a next block */
  uint8_t NdefTLVBuffer1[PH_FRINFC_NDEFMAP_MFUL_4BYTES_BUF];
} phFriNfc_NDEFTLVCont_t;

/*
 *  Lock Control TLV structure which stores the Position, Size and PageCntrl
 * details.
 */

typedef struct phFriNfc_LockCntrlTLVCont {
  /* Specifies the Byte Position of the lock cntrl tlv
      in the card memory*/
  uint16_t ByteAddr;

  /* Specifies the Size of the lock area in terms of
      bits/bytes*/
  uint16_t Size;

  /* Specifies the Bytes per Page*/
  uint8_t BytesPerPage;

  /* Specifies the BytesLockedPerLockBit */
  uint8_t BytesLockedPerLockBit;

  /* Specifies the index of Lock cntrl TLV*/
  uint8_t LockTlvBuffIdx;

  /* Store the content of Lock cntrl TLV*/
  uint8_t LockTlvBuff[8];

  /* Specifies the Block number Lock cntrl TLV*/
  uint16_t BlkNum;

  /* Specifies the Byte Number position of Lock cntrl TLV*/
  uint16_t ByteNum;

} phFriNfc_LockCntrlTLVCont_t;

/*
 *  Memory Control TLV structure which stores the Position, Size and PageCntrl
 * details of the reserved byte area.
 */

typedef struct phFriNfc_ResMemCntrlTLVCont {
  /* Specifies the Byte Position of the lock cntrl tlv
      in the card memory */
  uint16_t ByteAddr;

  /* Specifies the Size of the lock area in terms of
      bits/bytes*/
  uint16_t Size;

  /* Store the content of Memory cntrl TLV*/
  uint8_t MemCntrlTlvBuff[8];

  /* Specifies the Bytes per Page*/
  uint8_t BytesPerPage;

  /* Specifies the index of Mem cntrl TLV*/
  uint8_t MemTlvBuffIdx;

  /* Specifies the Block number Lock cntrl TLV*/
  uint16_t BlkNum;

  /* Specifies the Byte Number position of Lock cntrl TLV*/
  uint16_t ByteNum;

} phFriNfc_ResMemCntrlTLVCont_t;

/*
 *  NFC NDEF Mapping Component Context Structure
 *
 *  This structure is used to store the current context information of the
 * instance.
 *
 */
typedef struct phFriNfc_NdefMap {
  /* The state of the operation. */
  uint8_t State;

  /* Completion Routine Context. */
  phFriNfc_CplRt_t CompletionRoutine[PH_FRINFC_NDEFMAP_CR];

  phNfc_sTransceiveInfo_t* pTransceiveInfo;

  /*Holds the completion routine informations of the Map Layer*/
  phFriNfc_CplRt_t MapCompletionInfo;

  /* Pointer to the Remote Device Information */
  phLibNfc_sRemoteDevInformation_t* psRemoteDevInfo;

  /*Holds the Command Type(read/write)*/
  phNfc_uCmdList_t Cmd;

  /* Pointer to a temporary buffer. Could be
        used for read/write purposes */
  uint8_t* ApduBuffer;

  /* Size allocated to the ApduBuffer. */
  uint32_t ApduBufferSize;

  /* Index to the APDU Buffer. Used for internal calculations */
  uint16_t ApduBuffIndex;

  /* Pointer to the user-provided Data Size to be written trough WrNdef
   * function. */
  uint32_t* WrNdefPacketLength;

  /* Holds the length of the received data. */
  uint16_t* SendRecvLength;

  /*Holds the ack of some initial commands*/
  uint8_t* SendRecvBuf;

  /* Holds the length of the data to be sent. */
  uint16_t SendLength;

  /* Data Byte Count, which gives the offset to the integration.*/
  uint16_t* DataCount;

  /* Holds the previous operation on the card*/
  uint8_t PrevOperation;

  /* Holds the previous read mode*/
  uint8_t bPrevReadMode;

  /* Holds the current read mode*/
  uint8_t bCurrReadMode;

  /* Holds the previous state on the card*/
  uint8_t PrevState;

  /* Stores the type of the smart card. */
  uint8_t CardType;

  /* Stores the card state. */
  uint8_t CardState;

  /* Stores the memory size of the card */
  uint16_t CardMemSize;

  /*to Store the page offset on the mifare ul card*/
  uint8_t Offset;

  /* specifies the desired operation to be performed*/
  uint8_t DespOpFlag;

  /*  Used to remember how many bytes were written, to update
                 the dataCount and the BufferIndex */
  uint16_t NumOfBytesWritten;

  /*used to remember number of L byte Remaining to be written */
  uint16_t NumOfLReminWrite;

  /*  Pointer Used to remember and return how many bytes were read,
                 to update the PacketDataLength in case of Read operation */
  /*  Fix for 0000238: [gk] MAP: Number of bytes actually read out is
      not returned. */
  uint32_t* NumOfBytesRead;

  /*  Flag used to tell the process function that WRITE has
                 requested for an internal READ.*/
  uint8_t ReadingForWriteOperation;

  /*  Buffer of 5 bytes used for the write operation for the
                 Mifare UL card.*/
  uint8_t BufferForWriteOp[5];

  /* Temporary Receive Length to update the Receive Length
                when every time the Overlapped HAL is called. */
  uint16_t TempReceiveLength;

  uint8_t NoOfDevices;

  /* stores operating mode type of the felica smart tag */
  /* phHal_eOpModes_t                OpModeType[2]; */

  /* stores the type of the TLV found */
  uint8_t TLVFoundFlag;

  /* stores the TLV structure related informations  */
  phFriNfc_NDEFTLVCont_t TLVStruct;

  /* stores the Lock Contrl Tlv related informations  */
  phFriNfc_LockCntrlTLVCont_t LockTlv;

  /* stores the Mem Contrl Tlv related informations  */
  phFriNfc_ResMemCntrlTLVCont_t MemTlv;

  /* Pointer to the Mifare Standard capability Container Structure. */
  phFriNfc_MifareStdCont_t StdMifareContainer;

} phFriNfc_NdefMap_t;

/*
 * States of the FSM.
 */
#define PH_FRINFC_NDEFMAP_STATE_RESET_INIT 0    /* Initial state */
#define PH_FRINFC_NDEFMAP_STATE_CR_REGISTERED 1 /* CR has been registered */
#define PH_FRINFC_NDEFMAP_STATE_EOF_CARD 2      /* EOF card reached */

/* Following values specify the previous operation on the card. This value is
   assigned to the context structure variable: PrevOperation. */

/* Previous operation is check */
#define PH_FRINFC_NDEFMAP_CHECK_OPE 1
/* Previous operation is read */
#define PH_FRINFC_NDEFMAP_READ_OPE 2
/* Previous operation is write */
#define PH_FRINFC_NDEFMAP_WRITE_OPE 3
/* Previous operation is Actual size */
#define PH_FRINFC_NDEFMAP_GET_ACTSIZE_OPE 4

/* This flag is set when there is a need of write operation on the odd positions
   ex: 35,5 etc. This is used with MfUlOp Flag */
#define PH_FRINFC_MFUL_INTERNAL_READ 3 /* Read/Write control*/

#endif /* PHFRINFC_NDEFMAP_H */
