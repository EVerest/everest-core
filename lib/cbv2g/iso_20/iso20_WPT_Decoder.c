/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2022 - 2023 chargebyte GmbH
 * Copyright (C) 2022 - 2023 Contributors to EVerest
 */

/*****************************************************
 *
 * @author
 * @version
 *
 * The Code is generated! Changes may be overwritten.
 *
 *****************************************************/

/**
  * @file iso20_WPT_Decoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_types_decoder.h"
#include "cbv2g/common/exi_basetypes_decoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/iso_20/iso20_WPT_Datatypes.h"
#include "cbv2g/iso_20/iso20_WPT_Decoder.h"



static int decode_iso20_wpt_TransformType(exi_bitstream_t* stream, struct iso20_wpt_TransformType* TransformType);
static int decode_iso20_wpt_TransformsType(exi_bitstream_t* stream, struct iso20_wpt_TransformsType* TransformsType);
static int decode_iso20_wpt_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_wpt_DSAKeyValueType* DSAKeyValueType);
static int decode_iso20_wpt_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType);
static int decode_iso20_wpt_DigestMethodType(exi_bitstream_t* stream, struct iso20_wpt_DigestMethodType* DigestMethodType);
static int decode_iso20_wpt_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_wpt_RSAKeyValueType* RSAKeyValueType);
static int decode_iso20_wpt_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethodType);
static int decode_iso20_wpt_WPT_TxRxPulseOrderType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType);
static int decode_iso20_wpt_SignatureMethodType(exi_bitstream_t* stream, struct iso20_wpt_SignatureMethodType* SignatureMethodType);
static int decode_iso20_wpt_KeyValueType(exi_bitstream_t* stream, struct iso20_wpt_KeyValueType* KeyValueType);
static int decode_iso20_wpt_WPT_CoordinateXYZType(exi_bitstream_t* stream, struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType);
static int decode_iso20_wpt_ReferenceType(exi_bitstream_t* stream, struct iso20_wpt_ReferenceType* ReferenceType);
static int decode_iso20_wpt_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_wpt_RetrievalMethodType* RetrievalMethodType);
static int decode_iso20_wpt_X509DataType(exi_bitstream_t* stream, struct iso20_wpt_X509DataType* X509DataType);
static int decode_iso20_wpt_PGPDataType(exi_bitstream_t* stream, struct iso20_wpt_PGPDataType* PGPDataType);
static int decode_iso20_wpt_SPKIDataType(exi_bitstream_t* stream, struct iso20_wpt_SPKIDataType* SPKIDataType);
static int decode_iso20_wpt_SignedInfoType(exi_bitstream_t* stream, struct iso20_wpt_SignedInfoType* SignedInfoType);
static int decode_iso20_wpt_SignatureValueType(exi_bitstream_t* stream, struct iso20_wpt_SignatureValueType* SignatureValueType);
static int decode_iso20_wpt_RationalNumberType(exi_bitstream_t* stream, struct iso20_wpt_RationalNumberType* RationalNumberType);
static int decode_iso20_wpt_WPT_LF_RxRSSIType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType);
static int decode_iso20_wpt_WPT_LF_RxRSSIListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType);
static int decode_iso20_wpt_WPT_LF_TxDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType);
static int decode_iso20_wpt_WPT_LF_RxDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType);
static int decode_iso20_wpt_WPT_LF_TxDataListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType);
static int decode_iso20_wpt_KeyInfoType(exi_bitstream_t* stream, struct iso20_wpt_KeyInfoType* KeyInfoType);
static int decode_iso20_wpt_WPT_TxRxSpecDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType);
static int decode_iso20_wpt_WPT_LF_RxDataListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType);
static int decode_iso20_wpt_ObjectType(exi_bitstream_t* stream, struct iso20_wpt_ObjectType* ObjectType);
static int decode_iso20_wpt_WPT_TxRxPackageSpecDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType);
static int decode_iso20_wpt_WPT_LF_TransmitterDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType);
static int decode_iso20_wpt_AlternativeSECCType(exi_bitstream_t* stream, struct iso20_wpt_AlternativeSECCType* AlternativeSECCType);
static int decode_iso20_wpt_WPT_LF_ReceiverDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType);
static int decode_iso20_wpt_WPT_LF_DataPackageType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType);
static int decode_iso20_wpt_DetailedCostType(exi_bitstream_t* stream, struct iso20_wpt_DetailedCostType* DetailedCostType);
static int decode_iso20_wpt_SignatureType(exi_bitstream_t* stream, struct iso20_wpt_SignatureType* SignatureType);
static int decode_iso20_wpt_DetailedTaxType(exi_bitstream_t* stream, struct iso20_wpt_DetailedTaxType* DetailedTaxType);
static int decode_iso20_wpt_MessageHeaderType(exi_bitstream_t* stream, struct iso20_wpt_MessageHeaderType* MessageHeaderType);
static int decode_iso20_wpt_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_wpt_SignaturePropertyType* SignaturePropertyType);
static int decode_iso20_wpt_DisplayParametersType(exi_bitstream_t* stream, struct iso20_wpt_DisplayParametersType* DisplayParametersType);
static int decode_iso20_wpt_WPT_FinePositioningMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType);
static int decode_iso20_wpt_EVSEStatusType(exi_bitstream_t* stream, struct iso20_wpt_EVSEStatusType* EVSEStatusType);
static int decode_iso20_wpt_WPT_PairingMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType);
static int decode_iso20_wpt_MeterInfoType(exi_bitstream_t* stream, struct iso20_wpt_MeterInfoType* MeterInfoType);
static int decode_iso20_wpt_WPT_AlignmentCheckMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType);
static int decode_iso20_wpt_WPT_LF_DataPackageListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType);
static int decode_iso20_wpt_AlternativeSECCListType(exi_bitstream_t* stream, struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType);
static int decode_iso20_wpt_ReceiptType(exi_bitstream_t* stream, struct iso20_wpt_ReceiptType* ReceiptType);
static int decode_iso20_wpt_WPT_LF_SystemSetupDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType);
static int decode_iso20_wpt_WPT_EVPCPowerControlParameterType(exi_bitstream_t* stream, struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType);
static int decode_iso20_wpt_WPT_SPCPowerControlParameterType(exi_bitstream_t* stream, struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType);
static int decode_iso20_wpt_WPT_FinePositioningSetupReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReqType);
static int decode_iso20_wpt_WPT_FinePositioningSetupResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupResType);
static int decode_iso20_wpt_WPT_FinePositioningReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReqType);
static int decode_iso20_wpt_WPT_FinePositioningResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningResType);
static int decode_iso20_wpt_WPT_PairingReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingReqType* WPT_PairingReqType);
static int decode_iso20_wpt_WPT_PairingResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingResType* WPT_PairingResType);
static int decode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReqType);
static int decode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryResType);
static int decode_iso20_wpt_WPT_AlignmentCheckReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReqType);
static int decode_iso20_wpt_WPT_AlignmentCheckResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckResType);
static int decode_iso20_wpt_WPT_ChargeLoopReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReqType);
static int decode_iso20_wpt_WPT_ChargeLoopResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopResType);
static int decode_iso20_wpt_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_wpt_CLReqControlModeType* CLReqControlModeType);
static int decode_iso20_wpt_CLResControlModeType(exi_bitstream_t* stream, struct iso20_wpt_CLResControlModeType* CLResControlModeType);
static int decode_iso20_wpt_ManifestType(exi_bitstream_t* stream, struct iso20_wpt_ManifestType* ManifestType);
static int decode_iso20_wpt_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_wpt_SignaturePropertiesType* SignaturePropertiesType);

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int decode_iso20_wpt_TransformType(exi_bitstream_t* stream, struct iso20_wpt_TransformType* TransformType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_TransformType(TransformType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=1
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &TransformType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (TransformType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            TransformType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 1;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (XPath, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &TransformType->XPath.charactersLen);
                            if (error == 0)
                            {
                                if (TransformType->XPath.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    TransformType->XPath.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso20_wpt_XPath_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                TransformType->XPath_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &TransformType->ANY.bytesLen, &TransformType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        TransformType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
static int decode_iso20_wpt_TransformsType(exi_bitstream_t* stream, struct iso20_wpt_TransformsType* TransformsType) {
    int grammar_id = 4;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_TransformsType(TransformsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Transform)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=5
                    // decode: element
                    error = decode_iso20_wpt_TransformType(stream, &TransformsType->Transform);
                    if (error == 0)
                    {
                        grammar_id = 5;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=2; START (Transform), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=2
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
static int decode_iso20_wpt_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_wpt_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 6;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_DSAKeyValueType(DSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 6:
            // Grammar: ID=6; read/write bits=2; START (P), START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (P, CryptoBinary (base64Binary)); next=7
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->P.bytesLen, &DSAKeyValueType->P.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->P_isUsed = 1u;
                        grammar_id = 7;
                    }
                    break;
                case 1:
                    // Event: START (G, CryptoBinary (base64Binary)); next=9
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 2:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 10;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 7:
            // Grammar: ID=7; read/write bits=1; START (Q)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Q, CryptoBinary (base64Binary)); next=8
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Q.bytesLen, &DSAKeyValueType->Q.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Q_isUsed = 1u;
                        grammar_id = 8;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (G, CryptoBinary (base64Binary)); next=9
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 1:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 10;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=1; START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 10;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=2; START (J), START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (J, CryptoBinary (base64Binary)); next=11
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->J.bytesLen, &DSAKeyValueType->J.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->J_isUsed = 1u;
                        grammar_id = 11;
                    }
                    break;
                case 1:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 12;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 12;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=2; START (PgenCounter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PgenCounter, CryptoBinary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->PgenCounter.bytesLen, &DSAKeyValueType->PgenCounter.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->PgenCounter_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
static int decode_iso20_wpt_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 13;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_X509IssuerSerialType(X509IssuerSerialType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerName, string (string)); next=14
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &X509IssuerSerialType->X509IssuerName.charactersLen);
                            if (error == 0)
                            {
                                if (X509IssuerSerialType->X509IssuerName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    X509IssuerSerialType->X509IssuerName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso20_wpt_X509IssuerName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 14;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 14:
            // Grammar: ID=14; read/write bits=1; START (X509SerialNumber)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509SerialNumber, integer (decimal)); next=2
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &X509IssuerSerialType->X509SerialNumber);
                        if (error == 0)
                        {
                            grammar_id = 2;
                        }
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_DigestMethodType(exi_bitstream_t* stream, struct iso20_wpt_DigestMethodType* DigestMethodType) {
    int grammar_id = 15;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_DigestMethodType(DigestMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 15:
            // Grammar: ID=15; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=16
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &DigestMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (DigestMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            DigestMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 16;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 16:
            // Grammar: ID=16; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DigestMethodType->ANY.bytesLen, &DigestMethodType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        DigestMethodType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
static int decode_iso20_wpt_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_wpt_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 17;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_RSAKeyValueType(RSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 17:
            // Grammar: ID=17; read/write bits=1; START (Modulus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Modulus, CryptoBinary (base64Binary)); next=18
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Modulus.bytesLen, &RSAKeyValueType->Modulus.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 18;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 18:
            // Grammar: ID=18; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Exponent, CryptoBinary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Exponent.bytesLen, &RSAKeyValueType->Exponent.bytes[0], iso20_wpt_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 19;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_CanonicalizationMethodType(CanonicalizationMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 19:
            // Grammar: ID=19; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=20
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CanonicalizationMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (CanonicalizationMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CanonicalizationMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 20;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 20:
            // Grammar: ID=20; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CanonicalizationMethodType->ANY.bytesLen, &CanonicalizationMethodType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        CanonicalizationMethodType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}PulseSequenceOrder; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxPulseOrderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: IndexNumber, unsignedShort (1, 1); TxRxIdentifier, numericIDType (1, 1);
static int decode_iso20_wpt_WPT_TxRxPulseOrderType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType) {
    int grammar_id = 21;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_TxRxPulseOrderType(WPT_TxRxPulseOrderType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 21:
            // Grammar: ID=21; read/write bits=1; START (IndexNumber)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (IndexNumber, unsignedShort (unsignedInt)); next=22
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_TxRxPulseOrderType->IndexNumber);
                    if (error == 0)
                    {
                        grammar_id = 22;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 22:
            // Grammar: ID=22; read/write bits=1; START (TxRxIdentifier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxRxIdentifier, numericIDType (unsignedInt)); next=2
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_TxRxPulseOrderType->TxRxIdentifier);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_SignatureMethodType(exi_bitstream_t* stream, struct iso20_wpt_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 23;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignatureMethodType(SignatureMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=24
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 24;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 24:
            // Grammar: ID=24; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (HMACOutputLength, HMACOutputLengthType (integer)); next=25
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == 0)
                        {
                            SignatureMethodType->HMACOutputLength_isUsed = 1u;
                            grammar_id = 25;
                        }
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SignatureMethodType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SignatureMethodType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_KeyValueType(exi_bitstream_t* stream, struct iso20_wpt_KeyValueType* KeyValueType) {
    int grammar_id = 26;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_KeyValueType(KeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 26:
            // Grammar: ID=26; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DSAKeyValue, DSAKeyValueType (DSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->DSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (RSAKeyValue, RSAKeyValueType (RSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->RSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyValueType->ANY.bytesLen, &KeyValueType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyValueType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxRxPosition; type={urn:iso:std:iso:15118:-20:WPT}WPT_CoordinateXYZType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Coord_X, short (1, 1); Coord_Y, short (1, 1); Coord_Z, short (1, 1);
static int decode_iso20_wpt_WPT_CoordinateXYZType(exi_bitstream_t* stream, struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType) {
    int grammar_id = 27;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_CoordinateXYZType(WPT_CoordinateXYZType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 27:
            // Grammar: ID=27; read/write bits=1; START (Coord_X)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Coord_X, short (int)); next=28
                    // decode: short
                    error = decode_exi_type_integer16(stream, &WPT_CoordinateXYZType->Coord_X);
                    if (error == 0)
                    {
                        grammar_id = 28;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=1; START (Coord_Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Coord_Y, short (int)); next=29
                    // decode: short
                    error = decode_exi_type_integer16(stream, &WPT_CoordinateXYZType->Coord_Y);
                    if (error == 0)
                    {
                        grammar_id = 29;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 29:
            // Grammar: ID=29; read/write bits=1; START (Coord_Z)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Coord_Z, short (int)); next=2
                    // decode: short
                    error = decode_exi_type_integer16(stream, &WPT_CoordinateXYZType->Coord_Z);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
static int decode_iso20_wpt_ReferenceType(exi_bitstream_t* stream, struct iso20_wpt_ReferenceType* ReferenceType) {
    int grammar_id = 30;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_ReferenceType(ReferenceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 30:
            // Grammar: ID=30; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=31
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Id_isUsed = 1u;
                    grammar_id = 31;
                    break;
                case 1:
                    // Event: START (Type, anyURI (anyURI)); next=32
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 32;
                    break;
                case 2:
                    // Event: START (URI, anyURI (anyURI)); next=33
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 33;
                    break;
                case 3:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=34
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 34;
                    }
                    break;
                case 4:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=35
                    // decode: element
                    error = decode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 35;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 31:
            // Grammar: ID=31; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=32
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 32;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=33
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 33;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=34
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 34;
                    }
                    break;
                case 3:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=35
                    // decode: element
                    error = decode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 35;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 32:
            // Grammar: ID=32; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=33
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 33;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=34
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 34;
                    }
                    break;
                case 2:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=35
                    // decode: element
                    error = decode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 35;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 33:
            // Grammar: ID=33; read/write bits=2; START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=34
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 34;
                    }
                    break;
                case 1:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=35
                    // decode: element
                    error = decode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 35;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=35
                    // decode: element
                    error = decode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 35;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 35:
            // Grammar: ID=35; read/write bits=1; START (DigestValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestValue, DigestValueType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ReferenceType->DigestValue.bytesLen, &ReferenceType->DigestValue.bytes[0], iso20_wpt_DigestValueType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
static int decode_iso20_wpt_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_wpt_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 36;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_RetrievalMethodType(RetrievalMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=37
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->Type_isUsed = 1u;
                    grammar_id = 37;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=38
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 38;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=2; START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=38
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 38;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 38:
            // Grammar: ID=38; read/write bits=2; START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_X509DataType(exi_bitstream_t* stream, struct iso20_wpt_X509DataType* X509DataType) {
    int grammar_id = 39;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_X509DataType(X509DataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 39:
            // Grammar: ID=39; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerSerial, X509IssuerSerialType (X509IssuerSerialType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == 0)
                    {
                        X509DataType->X509IssuerSerial_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (X509SKI, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509SKI.bytesLen, &X509DataType->X509SKI.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509SKI_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (X509SubjectName, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &X509DataType->X509SubjectName.charactersLen);
                            if (error == 0)
                            {
                                if (X509DataType->X509SubjectName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    X509DataType->X509SubjectName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso20_wpt_X509SubjectName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                X509DataType->X509SubjectName_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (X509Certificate, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509Certificate.bytesLen, &X509DataType->X509Certificate.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509Certificate_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509CRL, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509CRL.bytesLen, &X509DataType->X509CRL.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509CRL_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->ANY.bytesLen, &X509DataType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_PGPDataType(exi_bitstream_t* stream, struct iso20_wpt_PGPDataType* PGPDataType) {
    int grammar_id = 40;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_PGPDataType(PGPDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 40:
            // Grammar: ID=40; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyID, base64Binary (base64Binary)); next=41
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyID.bytesLen, &PGPDataType->choice_1.PGPKeyID.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 41;
                    }
                    break;
                case 1:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=42
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 42;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 41:
            // Grammar: ID=41; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=42
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 42;
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 43;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 42:
            // Grammar: ID=42; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 43;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 43:
            // Grammar: ID=43; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=44
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.PGPKeyPacket.bytesLen, &PGPDataType->choice_2.PGPKeyPacket.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 44;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 44:
            // Grammar: ID=44; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=43
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.ANY.bytesLen, &PGPDataType->choice_2.ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_2.ANY_isUsed = 1u;
                        grammar_id = 43;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SPKIData; type={http://www.w3.org/2000/09/xmldsig#}SPKIDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPKISexp, base64Binary (1, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_SPKIDataType(exi_bitstream_t* stream, struct iso20_wpt_SPKIDataType* SPKIDataType) {
    int grammar_id = 45;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SPKIDataType(SPKIDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 45:
            // Grammar: ID=45; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPKISexp, base64Binary (base64Binary)); next=46
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->SPKISexp.bytesLen, &SPKIDataType->SPKISexp.bytes[0], iso20_wpt_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 46;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->ANY.bytesLen, &SPKIDataType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SPKIDataType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
static int decode_iso20_wpt_SignedInfoType(exi_bitstream_t* stream, struct iso20_wpt_SignedInfoType* SignedInfoType) {
    int grammar_id = 47;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignedInfoType(SignedInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 47:
            // Grammar: ID=47; read/write bits=2; START (Id), START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=48
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignedInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignedInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignedInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignedInfoType->Id_isUsed = 1u;
                    grammar_id = 48;
                    break;
                case 1:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=49
                    // decode: element
                    error = decode_iso20_wpt_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 49;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 48:
            // Grammar: ID=48; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=49
                    // decode: element
                    error = decode_iso20_wpt_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 49;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureMethod, SignatureMethodType (SignatureMethodType)); next=50
                    // decode: element
                    error = decode_iso20_wpt_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                    if (error == 0)
                    {
                        grammar_id = 50;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 50:
            // Grammar: ID=50; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=51
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 51;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=51
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 51;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
static int decode_iso20_wpt_SignatureValueType(exi_bitstream_t* stream, struct iso20_wpt_SignatureValueType* SignatureValueType) {
    int grammar_id = 52;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignatureValueType(SignatureValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=2; START (Id), START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=53
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureValueType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureValueType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureValueType->Id_isUsed = 1u;
                    grammar_id = 53;
                    break;
                case 1:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=2
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_wpt_SignatureValueType_BYTES_SIZE);
                        if (error == 0)
                        {
                            grammar_id = 2;
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 53:
            // Grammar: ID=53; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=2
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_wpt_SignatureValueType_BYTES_SIZE);
                        if (error == 0)
                        {
                            grammar_id = 2;
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}SignalFrequency; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
static int decode_iso20_wpt_RationalNumberType(exi_bitstream_t* stream, struct iso20_wpt_RationalNumberType* RationalNumberType) {
    int grammar_id = 54;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_RationalNumberType(RationalNumberType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 54:
            // Grammar: ID=54; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Exponent, byte (short)); next=55
                    // decode: byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = -128
                                RationalNumberType->Exponent = (int8_t)(value + -128);
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 55;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 55:
            // Grammar: ID=55; read/write bits=1; START (Value)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Value, short (int)); next=2
                    // decode: short
                    error = decode_exi_type_integer16(stream, &RationalNumberType->Value);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}RSSIDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxRSSIType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxIdentifier, numericIDType (1, 1); RSSI, RationalNumberType (1, 1);
static int decode_iso20_wpt_WPT_LF_RxRSSIType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType) {
    int grammar_id = 56;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_RxRSSIType(WPT_LF_RxRSSIType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 56:
            // Grammar: ID=56; read/write bits=1; START (TxIdentifier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxIdentifier, numericIDType (unsignedInt)); next=57
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_LF_RxRSSIType->TxIdentifier);
                    if (error == 0)
                    {
                        grammar_id = 57;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=1; START (RSSI)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RSSI, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_LF_RxRSSIType->RSSI);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}RSSIData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxRSSIListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RSSIDataList, WPT_LF_RxRSSIType (1, 1);
static int decode_iso20_wpt_WPT_LF_RxRSSIListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType) {
    int grammar_id = 58;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_RxRSSIListType(WPT_LF_RxRSSIListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 58:
            // Grammar: ID=58; read/write bits=1; START (RSSIDataList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RSSIDataList, WPT_LF_RxRSSIType (WPT_LF_RxRSSIType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_RxRSSIType(stream, &WPT_LF_RxRSSIListType->RSSIDataList);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxIdentifier, numericIDType (1, 1); EIRP, RationalNumberType (1, 1);
static int decode_iso20_wpt_WPT_LF_TxDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType) {
    int grammar_id = 59;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_TxDataType(WPT_LF_TxDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 59:
            // Grammar: ID=59; read/write bits=1; START (TxIdentifier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxIdentifier, numericIDType (unsignedInt)); next=60
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_LF_TxDataType->TxIdentifier);
                    if (error == 0)
                    {
                        grammar_id = 60;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 60:
            // Grammar: ID=60; read/write bits=1; START (EIRP)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EIRP, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_LF_TxDataType->EIRP);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RxIdentifier, numericIDType (1, 1); RSSIData, WPT_LF_RxRSSIListType (1, 1);
static int decode_iso20_wpt_WPT_LF_RxDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType) {
    int grammar_id = 61;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_RxDataType(WPT_LF_RxDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 61:
            // Grammar: ID=61; read/write bits=1; START (RxIdentifier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RxIdentifier, numericIDType (unsignedInt)); next=62
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_LF_RxDataType->RxIdentifier);
                    if (error == 0)
                    {
                        grammar_id = 62;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 62:
            // Grammar: ID=62; read/write bits=1; START (RSSIData)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RSSIData, WPT_LF_RxRSSIListType (WPT_LF_RxRSSIListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_RxRSSIListType(stream, &WPT_LF_RxDataType->RSSIData);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_TxData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_LF_TxDataList, WPT_LF_TxDataType (1, 1);
static int decode_iso20_wpt_WPT_LF_TxDataListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType) {
    int grammar_id = 63;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_TxDataListType(WPT_LF_TxDataListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 63:
            // Grammar: ID=63; read/write bits=1; START (WPT_LF_TxDataList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_LF_TxDataList, WPT_LF_TxDataType (WPT_LF_TxDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_TxDataType(stream, &WPT_LF_TxDataListType->WPT_LF_TxDataList);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_KeyInfoType(exi_bitstream_t* stream, struct iso20_wpt_KeyInfoType* KeyInfoType) {
    int grammar_id = 64;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_KeyInfoType(KeyInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 64:
            // Grammar: ID=64; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=65
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (KeyInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            KeyInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    KeyInfoType->Id_isUsed = 1u;
                    grammar_id = 65;
                    break;
                case 1:
                    // Event: START (KeyName, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->KeyName.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->KeyName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->KeyName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_wpt_KeyName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->KeyName_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (KeyValue, KeyValueType (KeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 6:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == 0)
                    {
                        KeyInfoType->SPKIData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 7:
                    // Event: START (MgmtData, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->MgmtData.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->MgmtData.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->MgmtData.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_wpt_MgmtData_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->MgmtData_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 8:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyInfoType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 65:
            // Grammar: ID=65; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyName, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->KeyName.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->KeyName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->KeyName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_wpt_KeyName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->KeyName_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (KeyValue, KeyValueType (KeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == 0)
                    {
                        KeyInfoType->SPKIData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 6:
                    // Event: START (MgmtData, string (string)); next=2
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->MgmtData.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->MgmtData.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->MgmtData.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_wpt_MgmtData_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->MgmtData_isUsed = 1u;
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 7:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyInfoType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxSpecData; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxSpecDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxRxIdentifier, numericIDType (1, 1); TxRxPosition, WPT_CoordinateXYZType (1, 1); TxRxOrientation, WPT_CoordinateXYZType (1, 1);
static int decode_iso20_wpt_WPT_TxRxSpecDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType) {
    int grammar_id = 66;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_TxRxSpecDataType(WPT_TxRxSpecDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 66:
            // Grammar: ID=66; read/write bits=1; START (TxRxIdentifier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxRxIdentifier, numericIDType (unsignedInt)); next=67
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_TxRxSpecDataType->TxRxIdentifier);
                    if (error == 0)
                    {
                        grammar_id = 67;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 67:
            // Grammar: ID=67; read/write bits=1; START (TxRxPosition)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxRxPosition, WPT_CoordinateXYZType (WPT_CoordinateXYZType)); next=68
                    // decode: element
                    error = decode_iso20_wpt_WPT_CoordinateXYZType(stream, &WPT_TxRxSpecDataType->TxRxPosition);
                    if (error == 0)
                    {
                        grammar_id = 68;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=1; START (TxRxOrientation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxRxOrientation, WPT_CoordinateXYZType (WPT_CoordinateXYZType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_CoordinateXYZType(stream, &WPT_TxRxSpecDataType->TxRxOrientation);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_RxData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_LF_RxDataList, WPT_LF_RxDataType (1, 1);
static int decode_iso20_wpt_WPT_LF_RxDataListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType) {
    int grammar_id = 69;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_RxDataListType(WPT_LF_RxDataListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 69:
            // Grammar: ID=69; read/write bits=1; START (WPT_LF_RxDataList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_LF_RxDataList, WPT_LF_RxDataType (WPT_LF_RxDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_RxDataType(stream, &WPT_LF_RxDataListType->WPT_LF_RxDataList);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int decode_iso20_wpt_ObjectType(exi_bitstream_t* stream, struct iso20_wpt_ObjectType* ObjectType) {
    int grammar_id = 70;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_ObjectType(ObjectType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 70:
            // Grammar: ID=70; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Encoding, anyURI (anyURI)); next=71
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Encoding.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Encoding.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Encoding.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso20_wpt_Encoding_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Encoding_isUsed = 1u;
                    grammar_id = 71;
                    break;
                case 1:
                    // Event: START (Id, ID (NCName)); next=72
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 72;
                    break;
                case 2:
                    // Event: START (MimeType, string (string)); next=73
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 73;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 4:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=72
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 72;
                    break;
                case 1:
                    // Event: START (MimeType, string (string)); next=73
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 73;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 4:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MimeType, string (string)); next=73
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 73;
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxPackageSpecData; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxPackageSpecDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PulseSequenceOrder, WPT_TxRxPulseOrderType (2, 255); PulseSeparationTime, unsignedShort (1, 1); PulseDuration, unsignedShort (1, 1); PackageSeparationTime, unsignedShort (1, 1);
static int decode_iso20_wpt_WPT_TxRxPackageSpecDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType) {
    int grammar_id = 74;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_TxRxPackageSpecDataType(WPT_TxRxPackageSpecDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 74:
            // Grammar: ID=74; read/write bits=1; START (PulseSequenceOrder)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PulseSequenceOrder, WPT_TxRxPulseOrderType (WPT_TxRxPulseOrderType)); next=75
                    // decode: element array
                    if (WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen < iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxPulseOrderType(stream, &WPT_TxRxPackageSpecDataType->PulseSequenceOrder.array[WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 75;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; LOOP (PulseSequenceOrder)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (PulseSequenceOrder, WPT_TxRxPulseOrderType (WPT_TxRxPulseOrderType)); next=75
                    // decode: element array
                    if (WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen < iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxPulseOrderType(stream, &WPT_TxRxPackageSpecDataType->PulseSequenceOrder.array[WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen < 255)
                    {
                        grammar_id = 75;
                    }
                    else
                    {
                        grammar_id = -1;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=1; START (PulseSeparationTime)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PulseSeparationTime, unsignedShort (unsignedInt)); next=77
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_TxRxPackageSpecDataType->PulseSeparationTime);
                    if (error == 0)
                    {
                        grammar_id = 77;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 77:
            // Grammar: ID=77; read/write bits=1; START (PulseDuration)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PulseDuration, unsignedShort (unsignedInt)); next=78
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_TxRxPackageSpecDataType->PulseDuration);
                    if (error == 0)
                    {
                        grammar_id = 78;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=1; START (PackageSeparationTime)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PackageSeparationTime, unsignedShort (unsignedInt)); next=2
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_TxRxPackageSpecDataType->PackageSeparationTime);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_TransmitterSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TransmitterDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumberOfTransmitters, unsignedByte (1, 1); SignalFrequency, RationalNumberType (1, 1); TxSpecData, WPT_TxRxSpecDataType (2, 255); TxPackageSpecData, WPT_TxRxPackageSpecDataType (0, 1);
static int decode_iso20_wpt_WPT_LF_TransmitterDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType) {
    int grammar_id = 79;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_TransmitterDataType(WPT_LF_TransmitterDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 79:
            // Grammar: ID=79; read/write bits=1; START (NumberOfTransmitters)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NumberOfTransmitters, unsignedByte (unsignedShort)); next=80
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                WPT_LF_TransmitterDataType->NumberOfTransmitters = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 80;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=1; START (SignalFrequency)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignalFrequency, RationalNumberType (RationalNumberType)); next=81
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_LF_TransmitterDataType->SignalFrequency);
                    if (error == 0)
                    {
                        grammar_id = 81;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=2; START (TxSpecData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxSpecData, WPT_TxRxSpecDataType (WPT_TxRxSpecDataType)); next=82
                    // decode: element array
                    if (WPT_LF_TransmitterDataType->TxSpecData.arrayLen < iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_TransmitterDataType->TxSpecData.array[WPT_LF_TransmitterDataType->TxSpecData.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 82;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 82:
            // Grammar: ID=82; read/write bits=1; LOOP (TxSpecData)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TxSpecData, WPT_TxRxSpecDataType (WPT_TxRxSpecDataType)); next=82
                    // decode: element array
                    if (WPT_LF_TransmitterDataType->TxSpecData.arrayLen < iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_TransmitterDataType->TxSpecData.array[WPT_LF_TransmitterDataType->TxSpecData.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (WPT_LF_TransmitterDataType->TxSpecData.arrayLen < 255)
                    {
                        grammar_id = 82;
                    }
                    else
                    {
                        grammar_id = -1;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (TxPackageSpecData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TxPackageSpecData, WPT_TxRxPackageSpecDataType (WPT_TxRxPackageSpecDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_TxRxPackageSpecDataType(stream, &WPT_LF_TransmitterDataType->TxPackageSpecData);
                    if (error == 0)
                    {
                        WPT_LF_TransmitterDataType->TxPackageSpecData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}AlternativeSECC; type={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SSID, identifierType (0, 1); BSSID, bssidType (0, 1); IPAddress, ipaddressType (0, 1); Port, unsignedShort (0, 1);
static int decode_iso20_wpt_AlternativeSECCType(exi_bitstream_t* stream, struct iso20_wpt_AlternativeSECCType* AlternativeSECCType) {
    int grammar_id = 84;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_AlternativeSECCType(AlternativeSECCType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 84:
            // Grammar: ID=84; read/write bits=3; START (SSID), START (BSSID), START (IPAddress), START (Port), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SSID, identifierType (string)); next=85
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->SSID.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->SSID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->SSID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->SSID.charactersLen, AlternativeSECCType->SSID.characters, iso20_wpt_SSID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->SSID_isUsed = 1u;
                                grammar_id = 85;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (BSSID, bssidType (string)); next=86
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->BSSID.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->BSSID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->BSSID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->BSSID.charactersLen, AlternativeSECCType->BSSID.characters, iso20_wpt_BSSID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->BSSID_isUsed = 1u;
                                grammar_id = 86;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (IPAddress, ipaddressType (string)); next=87
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->IPAddress.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->IPAddress.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->IPAddress.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->IPAddress_isUsed = 1u;
                                grammar_id = 87;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (Port, unsignedShort (unsignedInt)); next=2
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &AlternativeSECCType->Port);
                    if (error == 0)
                    {
                        AlternativeSECCType->Port_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=3; START (BSSID), START (IPAddress), START (Port), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BSSID, bssidType (string)); next=86
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->BSSID.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->BSSID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->BSSID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->BSSID.charactersLen, AlternativeSECCType->BSSID.characters, iso20_wpt_BSSID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->BSSID_isUsed = 1u;
                                grammar_id = 86;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (IPAddress, ipaddressType (string)); next=87
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->IPAddress.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->IPAddress.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->IPAddress.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->IPAddress_isUsed = 1u;
                                grammar_id = 87;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (Port, unsignedShort (unsignedInt)); next=2
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &AlternativeSECCType->Port);
                    if (error == 0)
                    {
                        AlternativeSECCType->Port_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 86:
            // Grammar: ID=86; read/write bits=2; START (IPAddress), START (Port), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (IPAddress, ipaddressType (string)); next=87
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AlternativeSECCType->IPAddress.charactersLen);
                            if (error == 0)
                            {
                                if (AlternativeSECCType->IPAddress.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AlternativeSECCType->IPAddress.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                AlternativeSECCType->IPAddress_isUsed = 1u;
                                grammar_id = 87;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (Port, unsignedShort (unsignedInt)); next=2
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &AlternativeSECCType->Port);
                    if (error == 0)
                    {
                        AlternativeSECCType->Port_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=2; START (Port), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Port, unsignedShort (unsignedInt)); next=2
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &AlternativeSECCType->Port);
                    if (error == 0)
                    {
                        AlternativeSECCType->Port_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_ReceiverSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_ReceiverDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumberOfReceivers, unsignedByte (1, 1); RxSpecData, WPT_TxRxSpecDataType (2, 255);
static int decode_iso20_wpt_WPT_LF_ReceiverDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType) {
    int grammar_id = 88;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_ReceiverDataType(WPT_LF_ReceiverDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 88:
            // Grammar: ID=88; read/write bits=1; START (NumberOfReceivers)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NumberOfReceivers, unsignedByte (unsignedShort)); next=89
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                WPT_LF_ReceiverDataType->NumberOfReceivers = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 89;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 89:
            // Grammar: ID=89; read/write bits=2; START (RxSpecData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RxSpecData, WPT_TxRxSpecDataType (WPT_TxRxSpecDataType)); next=90
                    // decode: element array
                    if (WPT_LF_ReceiverDataType->RxSpecData.arrayLen < iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_ReceiverDataType->RxSpecData.array[WPT_LF_ReceiverDataType->RxSpecData.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 90;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=1; LOOP (RxSpecData)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (RxSpecData, WPT_TxRxSpecDataType (WPT_TxRxSpecDataType)); next=90
                    // decode: element array
                    if (WPT_LF_ReceiverDataType->RxSpecData.arrayLen < iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_ReceiverDataType->RxSpecData.array[WPT_LF_ReceiverDataType->RxSpecData.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (WPT_LF_ReceiverDataType->RxSpecData.arrayLen < 255)
                    {
                        grammar_id = 90;
                    }
                    else
                    {
                        grammar_id = -1;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackage; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PackageIndex, unsignedByte (1, 1); LF_TxData, WPT_LF_TxDataListType (0, 1); LF_RxData, WPT_LF_RxDataListType (0, 1);
static int decode_iso20_wpt_WPT_LF_DataPackageType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType) {
    int grammar_id = 91;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_DataPackageType(WPT_LF_DataPackageType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 91:
            // Grammar: ID=91; read/write bits=1; START (PackageIndex)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PackageIndex, unsignedByte (unsignedShort)); next=92
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                WPT_LF_DataPackageType->PackageIndex = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 92:
            // Grammar: ID=92; read/write bits=2; START (LF_TxData), START (LF_RxData)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (LF_TxData, WPT_LF_TxDataListType (WPT_LF_TxDataListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_TxDataListType(stream, &WPT_LF_DataPackageType->LF_TxData);
                    if (error == 0)
                    {
                        WPT_LF_DataPackageType->LF_TxData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (LF_RxData, WPT_LF_RxDataListType (WPT_LF_RxDataListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_RxDataListType(stream, &WPT_LF_DataPackageType->LF_RxData);
                    if (error == 0)
                    {
                        WPT_LF_DataPackageType->LF_RxData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
static int decode_iso20_wpt_DetailedCostType(exi_bitstream_t* stream, struct iso20_wpt_DetailedCostType* DetailedCostType) {
    int grammar_id = 93;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_DetailedCostType(DetailedCostType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 93:
            // Grammar: ID=93; read/write bits=1; START (Amount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Amount, RationalNumberType (RationalNumberType)); next=94
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DetailedCostType->Amount);
                    if (error == 0)
                    {
                        grammar_id = 94;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 94:
            // Grammar: ID=94; read/write bits=1; START (CostPerUnit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CostPerUnit, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DetailedCostType->CostPerUnit);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
static int decode_iso20_wpt_SignatureType(exi_bitstream_t* stream, struct iso20_wpt_SignatureType* SignatureType) {
    int grammar_id = 95;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignatureType(SignatureType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 95:
            // Grammar: ID=95; read/write bits=2; START (Id), START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=96
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureType->Id_isUsed = 1u;
                    grammar_id = 96;
                    break;
                case 1:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=97
                    // decode: element
                    error = decode_iso20_wpt_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 97;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 96:
            // Grammar: ID=96; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=97
                    // decode: element
                    error = decode_iso20_wpt_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 97;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureValue, SignatureValueType (base64Binary)); next=98
                    // decode: element
                    error = decode_iso20_wpt_SignatureValueType(stream, &SignatureType->SignatureValue);
                    if (error == 0)
                    {
                        grammar_id = 98;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 98:
            // Grammar: ID=98; read/write bits=2; START (KeyInfo), START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyInfo, KeyInfoType (KeyInfoType)); next=100
                    // decode: element
                    error = decode_iso20_wpt_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == 0)
                    {
                        SignatureType->KeyInfo_isUsed = 1u;
                        grammar_id = 100;
                    }
                    break;
                case 1:
                    // Event: START (Object, ObjectType (ObjectType)); next=99
                    // decode: element
                    error = decode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 99;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=2
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 100:
            // Grammar: ID=100; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=101
                    // decode: element
                    error = decode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 101;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 101:
            // Grammar: ID=101; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=2
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
static int decode_iso20_wpt_DetailedTaxType(exi_bitstream_t* stream, struct iso20_wpt_DetailedTaxType* DetailedTaxType) {
    int grammar_id = 102;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_DetailedTaxType(DetailedTaxType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=1; START (TaxRuleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TaxRuleID, numericIDType (unsignedInt)); next=103
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DetailedTaxType->TaxRuleID);
                    if (error == 0)
                    {
                        grammar_id = 103;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 103:
            // Grammar: ID=103; read/write bits=1; START (Amount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Amount, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DetailedTaxType->Amount);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
static int decode_iso20_wpt_MessageHeaderType(exi_bitstream_t* stream, struct iso20_wpt_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 104;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_MessageHeaderType(MessageHeaderType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 104:
            // Grammar: ID=104; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=105
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MessageHeaderType->SessionID.bytesLen, &MessageHeaderType->SessionID.bytes[0], iso20_wpt_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 105;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 105:
            // Grammar: ID=105; read/write bits=1; START (TimeStamp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TimeStamp, unsignedLong (nonNegativeInteger)); next=106
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MessageHeaderType->TimeStamp);
                    if (error == 0)
                    {
                        grammar_id = 106;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 106:
            // Grammar: ID=106; read/write bits=2; START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Signature, SignatureType (SignatureType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == 0)
                    {
                        MessageHeaderType->Signature_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
static int decode_iso20_wpt_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_wpt_SignaturePropertyType* SignaturePropertyType) {
    int grammar_id = 107;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignaturePropertyType(SignaturePropertyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 107:
            // Grammar: ID=107; read/write bits=2; START (Id), START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=108
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Id.charactersLen, SignaturePropertyType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertyType->Id_isUsed = 1u;
                    grammar_id = 108;
                    break;
                case 1:
                    // Event: START (Target, anyURI (anyURI)); next=109
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_wpt_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 109;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 108:
            // Grammar: ID=108; read/write bits=1; START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Target, anyURI (anyURI)); next=109
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_wpt_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 109;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=1; START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignaturePropertyType->ANY.bytesLen, &SignaturePropertyType->ANY.bytes[0], iso20_wpt_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SignaturePropertyType->ANY_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParameters; type={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParametersType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PresentSOC, percentValueType (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); MaximumSOC, percentValueType (0, 1); RemainingTimeToMinimumSOC, unsignedInt (0, 1); RemainingTimeToTargetSOC, unsignedInt (0, 1); RemainingTimeToMaximumSOC, unsignedInt (0, 1); ChargingComplete, boolean (0, 1); BatteryEnergyCapacity, RationalNumberType (0, 1); InletHot, boolean (0, 1);
static int decode_iso20_wpt_DisplayParametersType(exi_bitstream_t* stream, struct iso20_wpt_DisplayParametersType* DisplayParametersType) {
    int grammar_id = 110;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_DisplayParametersType(DisplayParametersType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 110:
            // Grammar: ID=110; read/write bits=4; START (PresentSOC), START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PresentSOC, percentValueType (byte)); next=111
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->PresentSOC = (int8_t)value;
                                DisplayParametersType->PresentSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 111;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=112
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MinimumSOC = (int8_t)value;
                                DisplayParametersType->MinimumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 112;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (TargetSOC, percentValueType (byte)); next=113
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->TargetSOC = (int8_t)value;
                                DisplayParametersType->TargetSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 113;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (MaximumSOC, percentValueType (byte)); next=114
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MaximumSOC = (int8_t)value;
                                DisplayParametersType->MaximumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 114;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=115
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 115;
                    }
                    break;
                case 5:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 6:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 7:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 8:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 9:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 10:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 111:
            // Grammar: ID=111; read/write bits=4; START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=112
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MinimumSOC = (int8_t)value;
                                DisplayParametersType->MinimumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 112;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (TargetSOC, percentValueType (byte)); next=113
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->TargetSOC = (int8_t)value;
                                DisplayParametersType->TargetSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 113;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (MaximumSOC, percentValueType (byte)); next=114
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MaximumSOC = (int8_t)value;
                                DisplayParametersType->MaximumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 114;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=115
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 115;
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 5:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 6:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 7:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 8:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 9:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=4; START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=113
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->TargetSOC = (int8_t)value;
                                DisplayParametersType->TargetSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 113;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (MaximumSOC, percentValueType (byte)); next=114
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MaximumSOC = (int8_t)value;
                                DisplayParametersType->MaximumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 114;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=115
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 115;
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 5:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 6:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 7:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 8:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 113:
            // Grammar: ID=113; read/write bits=4; START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MaximumSOC, percentValueType (byte)); next=114
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->MaximumSOC = (int8_t)value;
                                DisplayParametersType->MaximumSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 114;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=115
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 115;
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 4:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 5:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 6:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 7:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=3; START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=115
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 115;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 3:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 5:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 6:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 115:
            // Grammar: ID=115; read/write bits=3; START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=116
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 116;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 2:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 4:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 5:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=3; START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=117
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 1:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 3:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 117:
            // Grammar: ID=117; read/write bits=3; START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingComplete, boolean (boolean)); next=118
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->ChargingComplete = value;
                                DisplayParametersType->ChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 118;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 2:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=2; START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 1:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 119:
            // Grammar: ID=119; read/write bits=2; START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (InletHot, boolean (boolean)); next=2
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DisplayParametersType->InletHot = value;
                                DisplayParametersType->InletHot_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDeviceFinePositioningMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_FinePositioningMethod, WPT_FinePositioningMethodType (1, 8);
static int decode_iso20_wpt_WPT_FinePositioningMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType) {
    int grammar_id = 120;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_FinePositioningMethodListType(WPT_FinePositioningMethodListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 120:
            // Grammar: ID=120; read/write bits=1; START (WPT_FinePositioningMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_FinePositioningMethod, WPT_FinePositioningMethodType (string)); next=121
                    // decode: enum array
                    if (WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen < iso20_wpt_WPT_FinePositioningMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.array[WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen] = (iso20_wpt_WPT_FinePositioningMethodType)value;
                                    WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 121;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 121:
            // Grammar: ID=121; read/write bits=2; LOOP (WPT_FinePositioningMethod), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (WPT_FinePositioningMethod, WPT_FinePositioningMethodType (string)); next=121
                    // decode: enum array
                    if (WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen < iso20_wpt_WPT_FinePositioningMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.array[WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen] = (iso20_wpt_WPT_FinePositioningMethodType)value;
                                    WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 121;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
static int decode_iso20_wpt_EVSEStatusType(exi_bitstream_t* stream, struct iso20_wpt_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 122;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_EVSEStatusType(EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 122:
            // Grammar: ID=122; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedShort (unsignedInt)); next=123
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 123;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 123:
            // Grammar: ID=123; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, evseNotificationType (string)); next=2
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                            if (error == 0)
                            {
                                EVSEStatusType->EVSENotification = (iso20_wpt_evseNotificationType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 2;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDevicePairingMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_PairingMethod, WPT_PairingMethodType (1, 8);
static int decode_iso20_wpt_WPT_PairingMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType) {
    int grammar_id = 124;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_PairingMethodListType(WPT_PairingMethodListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 124:
            // Grammar: ID=124; read/write bits=1; START (WPT_PairingMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_PairingMethod, WPT_PairingMethodType (string)); next=125
                    // decode: enum array
                    if (WPT_PairingMethodListType->WPT_PairingMethod.arrayLen < iso20_wpt_WPT_PairingMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    WPT_PairingMethodListType->WPT_PairingMethod.array[WPT_PairingMethodListType->WPT_PairingMethod.arrayLen] = (iso20_wpt_WPT_PairingMethodType)value;
                                    WPT_PairingMethodListType->WPT_PairingMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 125;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 125:
            // Grammar: ID=125; read/write bits=2; LOOP (WPT_PairingMethod), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (WPT_PairingMethod, WPT_PairingMethodType (string)); next=125
                    // decode: enum array
                    if (WPT_PairingMethodListType->WPT_PairingMethod.arrayLen < iso20_wpt_WPT_PairingMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    WPT_PairingMethodListType->WPT_PairingMethod.array[WPT_PairingMethodListType->WPT_PairingMethod.arrayLen] = (iso20_wpt_WPT_PairingMethodType)value;
                                    WPT_PairingMethodListType->WPT_PairingMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 125;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
static int decode_iso20_wpt_MeterInfoType(exi_bitstream_t* stream, struct iso20_wpt_MeterInfoType* MeterInfoType) {
    int grammar_id = 126;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_MeterInfoType(MeterInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 126:
            // Grammar: ID=126; read/write bits=1; START (MeterID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterID, meterIDType (string)); next=127
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &MeterInfoType->MeterID.charactersLen);
                            if (error == 0)
                            {
                                if (MeterInfoType->MeterID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    MeterInfoType->MeterID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso20_wpt_MeterID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 127;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 127:
            // Grammar: ID=127; read/write bits=1; START (ChargedEnergyReadingWh)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargedEnergyReadingWh, unsignedLong (nonNegativeInteger)); next=128
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->ChargedEnergyReadingWh);
                    if (error == 0)
                    {
                        grammar_id = 128;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 128:
            // Grammar: ID=128; read/write bits=3; START (BPT_DischargedEnergyReadingWh), START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BPT_DischargedEnergyReadingWh, unsignedLong (nonNegativeInteger)); next=129
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_DischargedEnergyReadingWh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 1u;
                        grammar_id = 129;
                    }
                    break;
                case 1:
                    // Event: START (CapacitiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=130
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->CapacitiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 130;
                    }
                    break;
                case 2:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=131
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 131;
                    }
                    break;
                case 3:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=132
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_wpt_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 132;
                    }
                    break;
                case 4:
                    // Event: START (MeterStatus, short (int)); next=133
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 133;
                    }
                    break;
                case 5:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 6:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=3; START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CapacitiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=130
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->CapacitiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 130;
                    }
                    break;
                case 1:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=131
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 131;
                    }
                    break;
                case 2:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=132
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_wpt_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 132;
                    }
                    break;
                case 3:
                    // Event: START (MeterStatus, short (int)); next=133
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 133;
                    }
                    break;
                case 4:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 130:
            // Grammar: ID=130; read/write bits=3; START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=131
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 131;
                    }
                    break;
                case 1:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=132
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_wpt_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 132;
                    }
                    break;
                case 2:
                    // Event: START (MeterStatus, short (int)); next=133
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 133;
                    }
                    break;
                case 3:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=3; START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=132
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_wpt_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 132;
                    }
                    break;
                case 1:
                    // Event: START (MeterStatus, short (int)); next=133
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 133;
                    }
                    break;
                case 2:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 132:
            // Grammar: ID=132; read/write bits=2; START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterStatus, short (int)); next=133
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 133;
                    }
                    break;
                case 1:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 133:
            // Grammar: ID=133; read/write bits=2; START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterTimestamp, unsignedLong (nonNegativeInteger)); next=2
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterTimestamp);
                    if (error == 0)
                    {
                        MeterInfoType->MeterTimestamp_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDeviceAlignmentCheckMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_AlignmentCheckMethod, WPT_AlignmentCheckMethodType (1, 8);
static int decode_iso20_wpt_WPT_AlignmentCheckMethodListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType) {
    int grammar_id = 134;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_AlignmentCheckMethodListType(WPT_AlignmentCheckMethodListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 134:
            // Grammar: ID=134; read/write bits=1; START (WPT_AlignmentCheckMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_AlignmentCheckMethod, WPT_AlignmentCheckMethodType (string)); next=135
                    // decode: enum array
                    if (WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen < iso20_wpt_WPT_AlignmentCheckMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                                if (error == 0)
                                {
                                    WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.array[WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen] = (iso20_wpt_WPT_AlignmentCheckMethodType)value;
                                    WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 135;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 135:
            // Grammar: ID=135; read/write bits=2; LOOP (WPT_AlignmentCheckMethod), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (WPT_AlignmentCheckMethod, WPT_AlignmentCheckMethodType (string)); next=135
                    // decode: enum array
                    if (WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen < iso20_wpt_WPT_AlignmentCheckMethodType_8_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                                if (error == 0)
                                {
                                    WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.array[WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen] = (iso20_wpt_WPT_AlignmentCheckMethodType)value;
                                    WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 135;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumPackages, unsignedByte (1, 1); WPT_LF_DataPackage, WPT_LF_DataPackageType (1, 1);
static int decode_iso20_wpt_WPT_LF_DataPackageListType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType) {
    int grammar_id = 136;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_DataPackageListType(WPT_LF_DataPackageListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 136:
            // Grammar: ID=136; read/write bits=1; START (NumPackages)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NumPackages, unsignedByte (unsignedShort)); next=137
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                WPT_LF_DataPackageListType->NumPackages = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 137;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (WPT_LF_DataPackage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_LF_DataPackage, WPT_LF_DataPackageType (WPT_LF_DataPackageType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_DataPackageType(stream, &WPT_LF_DataPackageListType->WPT_LF_DataPackage);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCList; type={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AlternativeSECC, AlternativeSECCType (1, 8);
static int decode_iso20_wpt_AlternativeSECCListType(exi_bitstream_t* stream, struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType) {
    int grammar_id = 138;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_AlternativeSECCListType(AlternativeSECCListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 138:
            // Grammar: ID=138; read/write bits=1; START (AlternativeSECC)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AlternativeSECC, AlternativeSECCType (AlternativeSECCType)); next=139
                    // decode: element array
                    if (AlternativeSECCListType->AlternativeSECC.arrayLen < iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_AlternativeSECCType(stream, &AlternativeSECCListType->AlternativeSECC.array[AlternativeSECCListType->AlternativeSECC.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 139;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 139:
            // Grammar: ID=139; read/write bits=2; LOOP (AlternativeSECC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (AlternativeSECC, AlternativeSECCType (AlternativeSECCType)); next=139
                    // decode: element array
                    if (AlternativeSECCListType->AlternativeSECC.arrayLen < iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_AlternativeSECCType(stream, &AlternativeSECCListType->AlternativeSECC.array[AlternativeSECCListType->AlternativeSECC.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (AlternativeSECCListType->AlternativeSECC.arrayLen < 8)
                    {
                        grammar_id = 139;
                    }
                    else
                    {
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
static int decode_iso20_wpt_ReceiptType(exi_bitstream_t* stream, struct iso20_wpt_ReceiptType* ReceiptType) {
    int grammar_id = 140;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_ReceiptType(ReceiptType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 140:
            // Grammar: ID=140; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TimeAnchor, unsignedLong (nonNegativeInteger)); next=141
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &ReceiptType->TimeAnchor);
                    if (error == 0)
                    {
                        grammar_id = 141;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=3; START (EnergyCosts), START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EnergyCosts, DetailedCostType (DetailedCostType)); next=143
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->EnergyCosts);
                    if (error == 0)
                    {
                        ReceiptType->EnergyCosts_isUsed = 1u;
                        grammar_id = 143;
                    }
                    break;
                case 1:
                    // Event: START (OccupancyCosts, DetailedCostType (DetailedCostType)); next=145
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == 0)
                    {
                        ReceiptType->OccupancyCosts_isUsed = 1u;
                        grammar_id = 145;
                    }
                    break;
                case 2:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=147
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 147;
                    }
                    break;
                case 3:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=149
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 149;
                    }
                    break;
                case 4:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=142
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 142;
                    break;
                case 5:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 142:
            // Grammar: ID=142; read/write bits=2; LOOP (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TaxCosts, DetailedTaxType (DetailedTaxType)); next=142
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ReceiptType->TaxCosts.arrayLen < 10)
                    {
                        grammar_id = 142;
                    }
                    else
                    {
                        grammar_id = 143;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=3; START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (OccupancyCosts, DetailedCostType (DetailedCostType)); next=145
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == 0)
                    {
                        ReceiptType->OccupancyCosts_isUsed = 1u;
                        grammar_id = 145;
                    }
                    break;
                case 1:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=147
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 147;
                    }
                    break;
                case 2:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=149
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 149;
                    }
                    break;
                case 3:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=144
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 144;
                    break;
                case 4:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=2; LOOP (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TaxCosts, DetailedTaxType (DetailedTaxType)); next=144
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ReceiptType->TaxCosts.arrayLen < 10)
                    {
                        grammar_id = 144;
                    }
                    else
                    {
                        grammar_id = 145;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 145:
            // Grammar: ID=145; read/write bits=3; START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=147
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 147;
                    }
                    break;
                case 1:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=149
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 149;
                    }
                    break;
                case 2:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=146
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 146;
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 146:
            // Grammar: ID=146; read/write bits=2; LOOP (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TaxCosts, DetailedTaxType (DetailedTaxType)); next=146
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ReceiptType->TaxCosts.arrayLen < 10)
                    {
                        grammar_id = 146;
                    }
                    else
                    {
                        grammar_id = 147;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 147:
            // Grammar: ID=147; read/write bits=2; START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=149
                    // decode: element
                    error = decode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 149;
                    }
                    break;
                case 1:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=148
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 148;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 148:
            // Grammar: ID=148; read/write bits=2; LOOP (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TaxCosts, DetailedTaxType (DetailedTaxType)); next=148
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ReceiptType->TaxCosts.arrayLen < 10)
                    {
                        grammar_id = 148;
                    }
                    else
                    {
                        grammar_id = 149;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 149:
            // Grammar: ID=149; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=150
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 150;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=2; LOOP (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (TaxCosts, DetailedTaxType (DetailedTaxType)); next=150
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_wpt_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_DetailedTaxType_10_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ReceiptType->TaxCosts.arrayLen < 10)
                    {
                        grammar_id = 150;
                    }
                    else
                    {
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_SystemSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_SystemSetupDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: LF_TransmitterSetupData, WPT_LF_TransmitterDataType (0, 1); LF_ReceiverSetupData, WPT_LF_ReceiverDataType (0, 1);
static int decode_iso20_wpt_WPT_LF_SystemSetupDataType(exi_bitstream_t* stream, struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType) {
    int grammar_id = 151;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_LF_SystemSetupDataType(WPT_LF_SystemSetupDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=2; START (LF_TransmitterSetupData), START (LF_ReceiverSetupData)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (LF_TransmitterSetupData, WPT_LF_TransmitterDataType (WPT_LF_TransmitterDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_TransmitterDataType(stream, &WPT_LF_SystemSetupDataType->LF_TransmitterSetupData);
                    if (error == 0)
                    {
                        WPT_LF_SystemSetupDataType->LF_TransmitterSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (LF_ReceiverSetupData, WPT_LF_ReceiverDataType (WPT_LF_ReceiverDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_ReceiverDataType(stream, &WPT_LF_SystemSetupDataType->LF_ReceiverSetupData);
                    if (error == 0)
                    {
                        WPT_LF_SystemSetupDataType->LF_ReceiverSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVPCPowerControlParameter; type={urn:iso:std:iso:15118:-20:WPT}WPT_EVPCPowerControlParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPCCoilCurrentRequest, RationalNumberType (1, 1); EVPCCoilCurrentInformation, RationalNumberType (1, 1); EVPCCurrentOutputInformation, RationalNumberType (1, 1); EVPCVoltageOutputInformation, RationalNumberType (1, 1);
static int decode_iso20_wpt_WPT_EVPCPowerControlParameterType(exi_bitstream_t* stream, struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType) {
    int grammar_id = 152;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_EVPCPowerControlParameterType(WPT_EVPCPowerControlParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 152:
            // Grammar: ID=152; read/write bits=1; START (EVPCCoilCurrentRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCCoilCurrentRequest, RationalNumberType (RationalNumberType)); next=153
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCoilCurrentRequest);
                    if (error == 0)
                    {
                        grammar_id = 153;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 153:
            // Grammar: ID=153; read/write bits=1; START (EVPCCoilCurrentInformation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCCoilCurrentInformation, RationalNumberType (RationalNumberType)); next=154
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCoilCurrentInformation);
                    if (error == 0)
                    {
                        grammar_id = 154;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=1; START (EVPCCurrentOutputInformation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCCurrentOutputInformation, RationalNumberType (RationalNumberType)); next=155
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCurrentOutputInformation);
                    if (error == 0)
                    {
                        grammar_id = 155;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=1; START (EVPCVoltageOutputInformation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCVoltageOutputInformation, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCVoltageOutputInformation);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}SPCPowerControlParameter; type={urn:iso:std:iso:15118:-20:WPT}WPT_SPCPowerControlParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPCPrimaryDeviceCoilCurrentInformation, RationalNumberType (1, 1);
static int decode_iso20_wpt_WPT_SPCPowerControlParameterType(exi_bitstream_t* stream, struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType) {
    int grammar_id = 156;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_SPCPowerControlParameterType(WPT_SPCPowerControlParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 156:
            // Grammar: ID=156; read/write bits=1; START (SPCPrimaryDeviceCoilCurrentInformation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCPrimaryDeviceCoilCurrentInformation, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_SPCPowerControlParameterType->SPCPrimaryDeviceCoilCurrentInformation);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (1, 1); EVDevicePairingMethodList, WPT_PairingMethodListType (1, 1); EVDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (1, 1); NaturalOffset, unsignedShort (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); LF_SystemSetupData, WPT_LF_SystemSetupDataType (0, 1);
static int decode_iso20_wpt_WPT_FinePositioningSetupReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReqType) {
    int grammar_id = 157;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_FinePositioningSetupReqType(WPT_FinePositioningSetupReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 157:
            // Grammar: ID=157; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=158
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningSetupReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 158;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=159
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningSetupReqType->EVProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 159;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 159:
            // Grammar: ID=159; read/write bits=1; START (EVDeviceFinePositioningMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (WPT_FinePositioningMethodListType)); next=160
                    // decode: element
                    error = decode_iso20_wpt_WPT_FinePositioningMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDeviceFinePositioningMethodList);
                    if (error == 0)
                    {
                        grammar_id = 160;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 160:
            // Grammar: ID=160; read/write bits=1; START (EVDevicePairingMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVDevicePairingMethodList, WPT_PairingMethodListType (WPT_PairingMethodListType)); next=161
                    // decode: element
                    error = decode_iso20_wpt_WPT_PairingMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDevicePairingMethodList);
                    if (error == 0)
                    {
                        grammar_id = 161;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 161:
            // Grammar: ID=161; read/write bits=1; START (EVDeviceAlignmentCheckMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (WPT_AlignmentCheckMethodListType)); next=162
                    // decode: element
                    error = decode_iso20_wpt_WPT_AlignmentCheckMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDeviceAlignmentCheckMethodList);
                    if (error == 0)
                    {
                        grammar_id = 162;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 162:
            // Grammar: ID=162; read/write bits=1; START (NaturalOffset)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NaturalOffset, unsignedShort (unsignedInt)); next=163
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_FinePositioningSetupReqType->NaturalOffset);
                    if (error == 0)
                    {
                        grammar_id = 163;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=164
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningSetupReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 164;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 164:
            // Grammar: ID=164; read/write bits=2; LOOP (VendorSpecificDataContainer), START (LF_SystemSetupData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=165
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningSetupReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 165;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType (WPT_LF_SystemSetupDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupReqType->LF_SystemSetupData);
                    if (error == 0)
                    {
                        WPT_FinePositioningSetupReqType->LF_SystemSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=2; START (LF_SystemSetupData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType (WPT_LF_SystemSetupDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupReqType->LF_SystemSetupData);
                    if (error == 0)
                    {
                        WPT_FinePositioningSetupReqType->LF_SystemSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); PrimaryDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (1, 1); PrimaryDevicePairingMethodList, WPT_PairingMethodListType (1, 1); PrimaryDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (1, 1); NaturalOffset, unsignedShort (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); LF_SystemSetupData, WPT_LF_SystemSetupDataType (0, 1);
static int decode_iso20_wpt_WPT_FinePositioningSetupResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupResType) {
    int grammar_id = 166;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_FinePositioningSetupResType(WPT_FinePositioningSetupResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 166:
            // Grammar: ID=166; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=167
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningSetupResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 167;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=168
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningSetupResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 168;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=1; START (PrimaryDeviceFinePositioningMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PrimaryDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (WPT_FinePositioningMethodListType)); next=169
                    // decode: element
                    error = decode_iso20_wpt_WPT_FinePositioningMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDeviceFinePositioningMethodList);
                    if (error == 0)
                    {
                        grammar_id = 169;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 169:
            // Grammar: ID=169; read/write bits=1; START (PrimaryDevicePairingMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PrimaryDevicePairingMethodList, WPT_PairingMethodListType (WPT_PairingMethodListType)); next=170
                    // decode: element
                    error = decode_iso20_wpt_WPT_PairingMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDevicePairingMethodList);
                    if (error == 0)
                    {
                        grammar_id = 170;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=1; START (PrimaryDeviceAlignmentCheckMethodList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PrimaryDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (WPT_AlignmentCheckMethodListType)); next=171
                    // decode: element
                    error = decode_iso20_wpt_WPT_AlignmentCheckMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDeviceAlignmentCheckMethodList);
                    if (error == 0)
                    {
                        grammar_id = 171;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=1; START (NaturalOffset)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NaturalOffset, unsignedShort (unsignedInt)); next=172
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_FinePositioningSetupResType->NaturalOffset);
                    if (error == 0)
                    {
                        grammar_id = 172;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=173
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningSetupResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 173;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 173:
            // Grammar: ID=173; read/write bits=2; LOOP (VendorSpecificDataContainer), START (LF_SystemSetupData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=174
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningSetupResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 174;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType (WPT_LF_SystemSetupDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupResType->LF_SystemSetupData);
                    if (error == 0)
                    {
                        WPT_FinePositioningSetupResType->LF_SystemSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=2; START (LF_SystemSetupData), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType (WPT_LF_SystemSetupDataType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupResType->LF_SystemSetupData);
                    if (error == 0)
                    {
                        WPT_FinePositioningSetupResType->LF_SystemSetupData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); WPT_LF_DataPackageList, WPT_LF_DataPackageListType (0, 1);
static int decode_iso20_wpt_WPT_FinePositioningReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReqType) {
    int grammar_id = 175;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_FinePositioningReqType(WPT_FinePositioningReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=176
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 176;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 176:
            // Grammar: ID=176; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=177
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningReqType->EVProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 177;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 177:
            // Grammar: ID=177; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVResultCode, WPT_EVResultType (string)); next=178
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningReqType->EVResultCode = (iso20_wpt_WPT_EVResultType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 178;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=179
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningReqType->VendorSpecificDataContainer.array[WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningReqType->VendorSpecificDataContainer.array[WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 179;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 179:
            // Grammar: ID=179; read/write bits=2; LOOP (VendorSpecificDataContainer), START (WPT_LF_DataPackageList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=180
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningReqType->VendorSpecificDataContainer.array[WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningReqType->VendorSpecificDataContainer.array[WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 180;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType (WPT_LF_DataPackageListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningReqType->WPT_LF_DataPackageList);
                    if (error == 0)
                    {
                        WPT_FinePositioningReqType->WPT_LF_DataPackageList_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 180:
            // Grammar: ID=180; read/write bits=2; START (WPT_LF_DataPackageList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType (WPT_LF_DataPackageListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningReqType->WPT_LF_DataPackageList);
                    if (error == 0)
                    {
                        WPT_FinePositioningReqType->WPT_LF_DataPackageList_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); WPT_LF_DataPackageList, WPT_LF_DataPackageListType (0, 1);
static int decode_iso20_wpt_WPT_FinePositioningResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningResType) {
    int grammar_id = 181;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_FinePositioningResType(WPT_FinePositioningResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 181:
            // Grammar: ID=181; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=182
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 182;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 182:
            // Grammar: ID=182; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=183
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 183;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=184
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_FinePositioningResType->EVSEProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 184;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=185
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningResType->VendorSpecificDataContainer.array[WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningResType->VendorSpecificDataContainer.array[WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 185;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 185:
            // Grammar: ID=185; read/write bits=2; LOOP (VendorSpecificDataContainer), START (WPT_LF_DataPackageList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=186
                    // decode exi type: base64Binary (Array)
                    if (WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_FinePositioningResType->VendorSpecificDataContainer.array[WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_FinePositioningResType->VendorSpecificDataContainer.array[WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_FinePositioningResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 186;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType (WPT_LF_DataPackageListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningResType->WPT_LF_DataPackageList);
                    if (error == 0)
                    {
                        WPT_FinePositioningResType->WPT_LF_DataPackageList_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=2; START (WPT_LF_DataPackageList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType (WPT_LF_DataPackageListType)); next=2
                    // decode: element
                    error = decode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningResType->WPT_LF_DataPackageList);
                    if (error == 0)
                    {
                        WPT_FinePositioningResType->WPT_LF_DataPackageList_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_PairingReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); ObservedIDCode, numericIDType (0, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_PairingReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingReqType* WPT_PairingReqType) {
    int grammar_id = 187;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_PairingReqType(WPT_PairingReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 187:
            // Grammar: ID=187; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=188
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_PairingReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 188;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=189
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_PairingReqType->EVProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 189;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=2; START (ObservedIDCode), START (EVResultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ObservedIDCode, numericIDType (unsignedInt)); next=190
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_PairingReqType->ObservedIDCode);
                    if (error == 0)
                    {
                        WPT_PairingReqType->ObservedIDCode_isUsed = 1u;
                        grammar_id = 190;
                    }
                    break;
                case 1:
                    // Event: START (EVResultCode, WPT_EVResultType (string)); next=191
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_PairingReqType->EVResultCode = (iso20_wpt_WPT_EVResultType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVResultCode, WPT_EVResultType (string)); next=191
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_PairingReqType->EVResultCode = (iso20_wpt_WPT_EVResultType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=192
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingReqType->VendorSpecificDataContainer.array[WPT_PairingReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingReqType->VendorSpecificDataContainer.array[WPT_PairingReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 192;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=192
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingReqType->VendorSpecificDataContainer.array[WPT_PairingReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingReqType->VendorSpecificDataContainer.array[WPT_PairingReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 192;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_PairingRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); ObservedIDCode, numericIDType (0, 1); AlternativeSECCList, AlternativeSECCListType (0, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_PairingResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_PairingResType* WPT_PairingResType) {
    int grammar_id = 193;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_PairingResType(WPT_PairingResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 193:
            // Grammar: ID=193; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=194
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_PairingResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 194;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 194:
            // Grammar: ID=194; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=195
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_PairingResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 195;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 195:
            // Grammar: ID=195; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=196
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_PairingResType->EVSEProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 196;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 196:
            // Grammar: ID=196; read/write bits=3; START (ObservedIDCode), START (AlternativeSECCList), START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ObservedIDCode, numericIDType (unsignedInt)); next=198
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &WPT_PairingResType->ObservedIDCode);
                    if (error == 0)
                    {
                        WPT_PairingResType->ObservedIDCode_isUsed = 1u;
                        grammar_id = 198;
                    }
                    break;
                case 1:
                    // Event: START (AlternativeSECCList, AlternativeSECCListType (AlternativeSECCListType)); next=200
                    // decode: element
                    error = decode_iso20_wpt_AlternativeSECCListType(stream, &WPT_PairingResType->AlternativeSECCList);
                    if (error == 0)
                    {
                        WPT_PairingResType->AlternativeSECCList_isUsed = 1u;
                        grammar_id = 200;
                    }
                    break;
                case 2:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=197
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 197;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 197:
            // Grammar: ID=197; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=197
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 197;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=2; START (AlternativeSECCList), START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AlternativeSECCList, AlternativeSECCListType (AlternativeSECCListType)); next=200
                    // decode: element
                    error = decode_iso20_wpt_AlternativeSECCListType(stream, &WPT_PairingResType->AlternativeSECCList);
                    if (error == 0)
                    {
                        WPT_PairingResType->AlternativeSECCList_isUsed = 1u;
                        grammar_id = 200;
                    }
                    break;
                case 1:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=199
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 199;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=199
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 199;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=201
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 201;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 201:
            // Grammar: ID=201; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=201
                    // decode exi type: base64Binary (Array)
                    if (WPT_PairingResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_PairingResType->VendorSpecificDataContainer.array[WPT_PairingResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_PairingResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_PairingResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 201;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVPCMaxReceivablePower, RationalNumberType (1, 1); SDMaxGroundClearence, unsignedShort (1, 1); SDMinGroundClearence, unsignedShort (1, 1); EVPCNaturalFrequency, RationalNumberType (1, 1); EVPCDeviceLocalControl, boolean (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReqType) {
    int grammar_id = 202;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(WPT_ChargeParameterDiscoveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 202:
            // Grammar: ID=202; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=203
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeParameterDiscoveryReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 203;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=1; START (EVPCMaxReceivablePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCMaxReceivablePower, RationalNumberType (RationalNumberType)); next=204
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryReqType->EVPCMaxReceivablePower);
                    if (error == 0)
                    {
                        grammar_id = 204;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=1; START (SDMaxGroundClearence)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMaxGroundClearence, unsignedShort (unsignedInt)); next=205
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_ChargeParameterDiscoveryReqType->SDMaxGroundClearence);
                    if (error == 0)
                    {
                        grammar_id = 205;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 205:
            // Grammar: ID=205; read/write bits=1; START (SDMinGroundClearence)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMinGroundClearence, unsignedShort (unsignedInt)); next=206
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_ChargeParameterDiscoveryReqType->SDMinGroundClearence);
                    if (error == 0)
                    {
                        grammar_id = 206;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 206:
            // Grammar: ID=206; read/write bits=1; START (EVPCNaturalFrequency)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCNaturalFrequency, RationalNumberType (RationalNumberType)); next=207
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryReqType->EVPCNaturalFrequency);
                    if (error == 0)
                    {
                        grammar_id = 207;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=1; START (EVPCDeviceLocalControl)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCDeviceLocalControl, boolean (boolean)); next=208
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                WPT_ChargeParameterDiscoveryReqType->EVPCDeviceLocalControl = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 208;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=209
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 209;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 209:
            // Grammar: ID=209; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=209
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 209;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); PDInputPowerClass, WPT_PowerClassType (1, 1); SDMinOutputPower, RationalNumberType (1, 1); SDMaxOutputPower, RationalNumberType (1, 1); SDMaxGroundClearanceSupport, unsignedShort (1, 1); SDMinGroundClearanceSupport, unsignedShort (1, 1); PDMinCoilCurrent, RationalNumberType (1, 1); PDMaxCoilCurrent, RationalNumberType (1, 1); SDManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryResType) {
    int grammar_id = 210;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_ChargeParameterDiscoveryResType(WPT_ChargeParameterDiscoveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 210:
            // Grammar: ID=210; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=211
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeParameterDiscoveryResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 211;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 211:
            // Grammar: ID=211; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=212
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_ChargeParameterDiscoveryResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 212;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=1; START (PDInputPowerClass)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PDInputPowerClass, WPT_PowerClassType (string)); next=213
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_ChargeParameterDiscoveryResType->PDInputPowerClass = (iso20_wpt_WPT_PowerClassType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 213;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=1; START (SDMinOutputPower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMinOutputPower, RationalNumberType (RationalNumberType)); next=214
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->SDMinOutputPower);
                    if (error == 0)
                    {
                        grammar_id = 214;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 214:
            // Grammar: ID=214; read/write bits=1; START (SDMaxOutputPower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMaxOutputPower, RationalNumberType (RationalNumberType)); next=215
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->SDMaxOutputPower);
                    if (error == 0)
                    {
                        grammar_id = 215;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 215:
            // Grammar: ID=215; read/write bits=1; START (SDMaxGroundClearanceSupport)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMaxGroundClearanceSupport, unsignedShort (unsignedInt)); next=216
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_ChargeParameterDiscoveryResType->SDMaxGroundClearanceSupport);
                    if (error == 0)
                    {
                        grammar_id = 216;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=1; START (SDMinGroundClearanceSupport)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDMinGroundClearanceSupport, unsignedShort (unsignedInt)); next=217
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &WPT_ChargeParameterDiscoveryResType->SDMinGroundClearanceSupport);
                    if (error == 0)
                    {
                        grammar_id = 217;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 217:
            // Grammar: ID=217; read/write bits=1; START (PDMinCoilCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PDMinCoilCurrent, RationalNumberType (RationalNumberType)); next=218
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->PDMinCoilCurrent);
                    if (error == 0)
                    {
                        grammar_id = 218;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 218:
            // Grammar: ID=218; read/write bits=1; START (PDMaxCoilCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PDMaxCoilCurrent, RationalNumberType (RationalNumberType)); next=219
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->PDMaxCoilCurrent);
                    if (error == 0)
                    {
                        grammar_id = 219;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 219:
            // Grammar: ID=219; read/write bits=2; START (SDManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=220
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 220;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=2; LOOP (SDManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SDManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=220
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 220;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); TargetCoilCurrent, RationalNumberType (0, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_AlignmentCheckReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReqType) {
    int grammar_id = 221;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_AlignmentCheckReqType(WPT_AlignmentCheckReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 221:
            // Grammar: ID=221; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=222
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_AlignmentCheckReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 222;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 222:
            // Grammar: ID=222; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=223
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_AlignmentCheckReqType->EVProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 223;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 223:
            // Grammar: ID=223; read/write bits=2; START (TargetCoilCurrent), START (EVResultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TargetCoilCurrent, RationalNumberType (RationalNumberType)); next=224
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckReqType->TargetCoilCurrent);
                    if (error == 0)
                    {
                        WPT_AlignmentCheckReqType->TargetCoilCurrent_isUsed = 1u;
                        grammar_id = 224;
                    }
                    break;
                case 1:
                    // Event: START (EVResultCode, WPT_EVResultType (string)); next=225
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_AlignmentCheckReqType->EVResultCode = (iso20_wpt_WPT_EVResultType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 225;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 224:
            // Grammar: ID=224; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVResultCode, WPT_EVResultType (string)); next=225
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_AlignmentCheckReqType->EVResultCode = (iso20_wpt_WPT_EVResultType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 225;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=226
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 226;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=226
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckReqType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 226;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); PowerTransmitted, RationalNumberType (0, 1); SupplyDeviceCurrent, RationalNumberType (0, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_AlignmentCheckResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckResType) {
    int grammar_id = 227;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_AlignmentCheckResType(WPT_AlignmentCheckResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=228
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_AlignmentCheckResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 228;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=229
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_AlignmentCheckResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 229;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=230
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_AlignmentCheckResType->EVSEProcessing = (iso20_wpt_processingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 230;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 230:
            // Grammar: ID=230; read/write bits=3; START (PowerTransmitted), START (SupplyDeviceCurrent), START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PowerTransmitted, RationalNumberType (RationalNumberType)); next=232
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->PowerTransmitted);
                    if (error == 0)
                    {
                        WPT_AlignmentCheckResType->PowerTransmitted_isUsed = 1u;
                        grammar_id = 232;
                    }
                    break;
                case 1:
                    // Event: START (SupplyDeviceCurrent, RationalNumberType (RationalNumberType)); next=234
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->SupplyDeviceCurrent);
                    if (error == 0)
                    {
                        WPT_AlignmentCheckResType->SupplyDeviceCurrent_isUsed = 1u;
                        grammar_id = 234;
                    }
                    break;
                case 2:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=231
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 231;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=231
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 231;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=2; START (SupplyDeviceCurrent), START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SupplyDeviceCurrent, RationalNumberType (RationalNumberType)); next=234
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->SupplyDeviceCurrent);
                    if (error == 0)
                    {
                        WPT_AlignmentCheckResType->SupplyDeviceCurrent_isUsed = 1u;
                        grammar_id = 234;
                    }
                    break;
                case 1:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=233
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 233;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=233
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 233;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=2; START (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=235
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 235;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 235:
            // Grammar: ID=235; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (VendorSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=235
                    // decode exi type: base64Binary (Array)
                    if (WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytesLen, &WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen++;
                            WPT_AlignmentCheckResType->VendorSpecificDataContainer_isUsed = 1u;
                            grammar_id = 235;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); EVPCPowerRequest, RationalNumberType (1, 1); EVPCPowerOutput, RationalNumberType (1, 1); EVPCChargeDiagnostics, WPT_EVPCChargeDiagnosticsType (1, 1); EVPCOperatingFrequency, RationalNumberType (0, 1); EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType (0, 1); ManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_ChargeLoopReqType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReqType) {
    int grammar_id = 236;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_ChargeLoopReqType(WPT_ChargeLoopReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 236:
            // Grammar: ID=236; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=237
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeLoopReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 237;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 237:
            // Grammar: ID=237; read/write bits=2; START (DisplayParameters), START (MeterInfoRequested)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DisplayParameters, DisplayParametersType (DisplayParametersType)); next=238
                    // decode: element
                    error = decode_iso20_wpt_DisplayParametersType(stream, &WPT_ChargeLoopReqType->DisplayParameters);
                    if (error == 0)
                    {
                        WPT_ChargeLoopReqType->DisplayParameters_isUsed = 1u;
                        grammar_id = 238;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfoRequested, boolean (boolean)); next=239
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                WPT_ChargeLoopReqType->MeterInfoRequested = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 239;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 238:
            // Grammar: ID=238; read/write bits=1; START (MeterInfoRequested)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfoRequested, boolean (boolean)); next=239
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                WPT_ChargeLoopReqType->MeterInfoRequested = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 239;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 239:
            // Grammar: ID=239; read/write bits=1; START (EVPCPowerRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCPowerRequest, RationalNumberType (RationalNumberType)); next=240
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCPowerRequest);
                    if (error == 0)
                    {
                        grammar_id = 240;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=1; START (EVPCPowerOutput)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCPowerOutput, RationalNumberType (RationalNumberType)); next=241
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCPowerOutput);
                    if (error == 0)
                    {
                        grammar_id = 241;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 241:
            // Grammar: ID=241; read/write bits=1; START (EVPCChargeDiagnostics)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCChargeDiagnostics, WPT_EVPCChargeDiagnosticsType (string)); next=242
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                WPT_ChargeLoopReqType->EVPCChargeDiagnostics = (iso20_wpt_WPT_EVPCChargeDiagnosticsType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 242;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 242:
            // Grammar: ID=242; read/write bits=3; START (EVPCOperatingFrequency), START (EVPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCOperatingFrequency, RationalNumberType (RationalNumberType)); next=244
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCOperatingFrequency);
                    if (error == 0)
                    {
                        WPT_ChargeLoopReqType->EVPCOperatingFrequency_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                case 1:
                    // Event: START (EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType (WPT_EVPCPowerControlParameterType)); next=246
                    // decode: element
                    error = decode_iso20_wpt_WPT_EVPCPowerControlParameterType(stream, &WPT_ChargeLoopReqType->EVPCPowerControlParameter);
                    if (error == 0)
                    {
                        WPT_ChargeLoopReqType->EVPCPowerControlParameter_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 2:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=243
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 243;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=243
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 243;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=2; START (EVPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType (WPT_EVPCPowerControlParameterType)); next=246
                    // decode: element
                    error = decode_iso20_wpt_WPT_EVPCPowerControlParameterType(stream, &WPT_ChargeLoopReqType->EVPCPowerControlParameter);
                    if (error == 0)
                    {
                        WPT_ChargeLoopReqType->EVPCPowerControlParameter_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 1:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=245
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 245;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 245:
            // Grammar: ID=245; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=245
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 245;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=2; START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=247
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 247;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 247:
            // Grammar: ID=247; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=247
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 247;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVPCPowerRequest, RationalNumberType (1, 1); SDPowerInput, RationalNumberType (0, 1); SPCMaxOutputPowerLimit, RationalNumberType (1, 1); SPCMinOutputPowerLimit, RationalNumberType (1, 1); SPCChargeDiagnostics, WPT_SPCChargeDiagnosticsType (1, 1); SPCOperatingFrequency, RationalNumberType (0, 1); SPCPowerControlParameter, WPT_SPCPowerControlParameterType (0, 1); ManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
static int decode_iso20_wpt_WPT_ChargeLoopResType(exi_bitstream_t* stream, struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopResType) {
    int grammar_id = 248;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_WPT_ChargeLoopResType(WPT_ChargeLoopResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 248:
            // Grammar: ID=248; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=249
                    // decode: element
                    error = decode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeLoopResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 249;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=250
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 6, &value);
                            if (error == 0)
                            {
                                WPT_ChargeLoopResType->ResponseCode = (iso20_wpt_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 250;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 250:
            // Grammar: ID=250; read/write bits=3; START (EVSEStatus), START (MeterInfo), START (Receipt), START (EVPCPowerRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEStatus, EVSEStatusType (EVSEStatusType)); next=251
                    // decode: element
                    error = decode_iso20_wpt_EVSEStatusType(stream, &WPT_ChargeLoopResType->EVSEStatus);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->EVSEStatus_isUsed = 1u;
                        grammar_id = 251;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=252
                    // decode: element
                    error = decode_iso20_wpt_MeterInfoType(stream, &WPT_ChargeLoopResType->MeterInfo);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->MeterInfo_isUsed = 1u;
                        grammar_id = 252;
                    }
                    break;
                case 2:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=253
                    // decode: element
                    error = decode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 3:
                    // Event: START (EVPCPowerRequest, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == 0)
                    {
                        grammar_id = 254;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=2; START (MeterInfo), START (Receipt), START (EVPCPowerRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=252
                    // decode: element
                    error = decode_iso20_wpt_MeterInfoType(stream, &WPT_ChargeLoopResType->MeterInfo);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->MeterInfo_isUsed = 1u;
                        grammar_id = 252;
                    }
                    break;
                case 1:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=253
                    // decode: element
                    error = decode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 2:
                    // Event: START (EVPCPowerRequest, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == 0)
                    {
                        grammar_id = 254;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=2; START (Receipt), START (EVPCPowerRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=253
                    // decode: element
                    error = decode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 1:
                    // Event: START (EVPCPowerRequest, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == 0)
                    {
                        grammar_id = 254;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=1; START (EVPCPowerRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPCPowerRequest, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == 0)
                    {
                        grammar_id = 254;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=2; START (SDPowerInput), START (SPCMaxOutputPowerLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SDPowerInput, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SDPowerInput);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->SDPowerInput_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 1:
                    // Event: START (SPCMaxOutputPowerLimit, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMaxOutputPowerLimit);
                    if (error == 0)
                    {
                        grammar_id = 256;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=1; START (SPCMaxOutputPowerLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCMaxOutputPowerLimit, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMaxOutputPowerLimit);
                    if (error == 0)
                    {
                        grammar_id = 256;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=1; START (SPCMinOutputPowerLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCMinOutputPowerLimit, RationalNumberType (RationalNumberType)); next=257
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMinOutputPowerLimit);
                    if (error == 0)
                    {
                        grammar_id = 257;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 257:
            // Grammar: ID=257; read/write bits=1; START (SPCChargeDiagnostics)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCChargeDiagnostics, WPT_SPCChargeDiagnosticsType (string)); next=258
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                            if (error == 0)
                            {
                                WPT_ChargeLoopResType->SPCChargeDiagnostics = (iso20_wpt_WPT_SPCChargeDiagnosticsType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 258;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=3; START (SPCOperatingFrequency), START (SPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCOperatingFrequency, RationalNumberType (RationalNumberType)); next=260
                    // decode: element
                    error = decode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCOperatingFrequency);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->SPCOperatingFrequency_isUsed = 1u;
                        grammar_id = 260;
                    }
                    break;
                case 1:
                    // Event: START (SPCPowerControlParameter, WPT_SPCPowerControlParameterType (WPT_SPCPowerControlParameterType)); next=262
                    // decode: element
                    error = decode_iso20_wpt_WPT_SPCPowerControlParameterType(stream, &WPT_ChargeLoopResType->SPCPowerControlParameter);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->SPCPowerControlParameter_isUsed = 1u;
                        grammar_id = 262;
                    }
                    break;
                case 2:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=259
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 259;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 3:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=259
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 259;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=2; START (SPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPCPowerControlParameter, WPT_SPCPowerControlParameterType (WPT_SPCPowerControlParameterType)); next=262
                    // decode: element
                    error = decode_iso20_wpt_WPT_SPCPowerControlParameterType(stream, &WPT_ChargeLoopResType->SPCPowerControlParameter);
                    if (error == 0)
                    {
                        WPT_ChargeLoopResType->SPCPowerControlParameter_isUsed = 1u;
                        grammar_id = 262;
                    }
                    break;
                case 1:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=261
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 261;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 261:
            // Grammar: ID=261; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=261
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 261;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=2; START (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=263
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 263;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 263:
            // Grammar: ID=263; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ManufacturerSpecificDataContainer, WPT_DataContainerType (base64Binary)); next=263
                    // decode exi type: base64Binary (Array)
                    if (WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen < iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytesLen, &WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen].bytes[0], iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                        if (error == 0)
                        {
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen++;
                            WPT_ChargeLoopResType->ManufacturerSpecificDataContainer_isUsed = 1u;
                            grammar_id = 263;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int decode_iso20_wpt_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_wpt_CLReqControlModeType* CLReqControlModeType) {
    // Element has no particles, so the function just decodes END Element
    (void)CLReqControlModeType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int decode_iso20_wpt_CLResControlModeType(exi_bitstream_t* stream, struct iso20_wpt_CLResControlModeType* CLResControlModeType) {
    // Element has no particles, so the function just decodes END Element
    (void)CLResControlModeType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
static int decode_iso20_wpt_ManifestType(exi_bitstream_t* stream, struct iso20_wpt_ManifestType* ManifestType) {
    int grammar_id = 264;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_ManifestType(ManifestType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 264:
            // Grammar: ID=264; read/write bits=2; START (Id), START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=266
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ManifestType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ManifestType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ManifestType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ManifestType->Id.charactersLen, ManifestType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ManifestType->Id_isUsed = 1u;
                    grammar_id = 266;
                    break;
                case 1:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=265
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 265;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=265
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 265;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=267
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 267;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=267
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_wpt_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_wpt_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 267;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
static int decode_iso20_wpt_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_wpt_SignaturePropertiesType* SignaturePropertiesType) {
    int grammar_id = 268;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_wpt_SignaturePropertiesType(SignaturePropertiesType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 268:
            // Grammar: ID=268; read/write bits=2; START (Id), START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=270
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertiesType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertiesType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertiesType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertiesType->Id.charactersLen, SignaturePropertiesType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertiesType->Id_isUsed = 1u;
                    grammar_id = 270;
                    break;
                case 1:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=269
                    // decode: element
                    error = decode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
                    {
                        grammar_id = 269;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 269:
            // Grammar: ID=269; read/write bits=2; START (SignatureProperty), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=2
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 270:
            // Grammar: ID=270; read/write bits=1; START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=271
                    // decode: element
                    error = decode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
                    {
                        grammar_id = 271;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 271:
            // Grammar: ID=271; read/write bits=2; START (SignatureProperty), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=2
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}


// main function for decoding
int decode_iso20_wpt_exiDocument(exi_bitstream_t* stream, struct iso20_wpt_exiDocument* exiDoc) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == 0)
    {
        init_iso20_wpt_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
        if (error == 0)
        {
            switch (eventCode)
            {
            case 0:
                error = decode_iso20_wpt_CLReqControlModeType(stream, &exiDoc->CLReqControlMode);
                exiDoc->CLReqControlMode_isUsed = 1u;
                break;
            case 1:
                error = decode_iso20_wpt_CLResControlModeType(stream, &exiDoc->CLResControlMode);
                exiDoc->CLResControlMode_isUsed = 1u;
                break;
            case 2:
                error = decode_iso20_wpt_CanonicalizationMethodType(stream, &exiDoc->CanonicalizationMethod);
                exiDoc->CanonicalizationMethod_isUsed = 1u;
                break;
            case 3:
                error = decode_iso20_wpt_DSAKeyValueType(stream, &exiDoc->DSAKeyValue);
                exiDoc->DSAKeyValue_isUsed = 1u;
                break;
            case 4:
                error = decode_iso20_wpt_DigestMethodType(stream, &exiDoc->DigestMethod);
                exiDoc->DigestMethod_isUsed = 1u;
                break;
            case 5:
                // simple type! decode_iso20_wpt_DigestValue;
                break;
            case 6:
                error = decode_iso20_wpt_KeyInfoType(stream, &exiDoc->KeyInfo);
                exiDoc->KeyInfo_isUsed = 1u;
                break;
            case 7:
                // simple type! decode_iso20_wpt_KeyName;
                break;
            case 8:
                error = decode_iso20_wpt_KeyValueType(stream, &exiDoc->KeyValue);
                exiDoc->KeyValue_isUsed = 1u;
                break;
            case 9:
                error = decode_iso20_wpt_ManifestType(stream, &exiDoc->Manifest);
                exiDoc->Manifest_isUsed = 1u;
                break;
            case 10:
                // simple type! decode_iso20_wpt_MgmtData;
                break;
            case 11:
                error = decode_iso20_wpt_ObjectType(stream, &exiDoc->Object);
                exiDoc->Object_isUsed = 1u;
                break;
            case 12:
                error = decode_iso20_wpt_PGPDataType(stream, &exiDoc->PGPData);
                exiDoc->PGPData_isUsed = 1u;
                break;
            case 13:
                error = decode_iso20_wpt_RSAKeyValueType(stream, &exiDoc->RSAKeyValue);
                exiDoc->RSAKeyValue_isUsed = 1u;
                break;
            case 14:
                error = decode_iso20_wpt_ReferenceType(stream, &exiDoc->Reference);
                exiDoc->Reference_isUsed = 1u;
                break;
            case 15:
                error = decode_iso20_wpt_RetrievalMethodType(stream, &exiDoc->RetrievalMethod);
                exiDoc->RetrievalMethod_isUsed = 1u;
                break;
            case 16:
                error = decode_iso20_wpt_SPKIDataType(stream, &exiDoc->SPKIData);
                exiDoc->SPKIData_isUsed = 1u;
                break;
            case 17:
                error = decode_iso20_wpt_SignatureMethodType(stream, &exiDoc->SignatureMethod);
                exiDoc->SignatureMethod_isUsed = 1u;
                break;
            case 18:
                error = decode_iso20_wpt_SignaturePropertiesType(stream, &exiDoc->SignatureProperties);
                exiDoc->SignatureProperties_isUsed = 1u;
                break;
            case 19:
                error = decode_iso20_wpt_SignaturePropertyType(stream, &exiDoc->SignatureProperty);
                exiDoc->SignatureProperty_isUsed = 1u;
                break;
            case 20:
                error = decode_iso20_wpt_SignatureType(stream, &exiDoc->Signature);
                exiDoc->Signature_isUsed = 1u;
                break;
            case 21:
                error = decode_iso20_wpt_SignatureValueType(stream, &exiDoc->SignatureValue);
                exiDoc->SignatureValue_isUsed = 1u;
                break;
            case 22:
                error = decode_iso20_wpt_SignedInfoType(stream, &exiDoc->SignedInfo);
                exiDoc->SignedInfo_isUsed = 1u;
                break;
            case 23:
                error = decode_iso20_wpt_TransformType(stream, &exiDoc->Transform);
                exiDoc->Transform_isUsed = 1u;
                break;
            case 24:
                error = decode_iso20_wpt_TransformsType(stream, &exiDoc->Transforms);
                exiDoc->Transforms_isUsed = 1u;
                break;
            case 25:
                error = decode_iso20_wpt_WPT_AlignmentCheckReqType(stream, &exiDoc->WPT_AlignmentCheckReq);
                exiDoc->WPT_AlignmentCheckReq_isUsed = 1u;
                break;
            case 26:
                error = decode_iso20_wpt_WPT_AlignmentCheckResType(stream, &exiDoc->WPT_AlignmentCheckRes);
                exiDoc->WPT_AlignmentCheckRes_isUsed = 1u;
                break;
            case 27:
                error = decode_iso20_wpt_WPT_ChargeLoopReqType(stream, &exiDoc->WPT_ChargeLoopReq);
                exiDoc->WPT_ChargeLoopReq_isUsed = 1u;
                break;
            case 28:
                error = decode_iso20_wpt_WPT_ChargeLoopResType(stream, &exiDoc->WPT_ChargeLoopRes);
                exiDoc->WPT_ChargeLoopRes_isUsed = 1u;
                break;
            case 29:
                error = decode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(stream, &exiDoc->WPT_ChargeParameterDiscoveryReq);
                exiDoc->WPT_ChargeParameterDiscoveryReq_isUsed = 1u;
                break;
            case 30:
                error = decode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(stream, &exiDoc->WPT_ChargeParameterDiscoveryRes);
                exiDoc->WPT_ChargeParameterDiscoveryRes_isUsed = 1u;
                break;
            case 31:
                error = decode_iso20_wpt_WPT_FinePositioningReqType(stream, &exiDoc->WPT_FinePositioningReq);
                exiDoc->WPT_FinePositioningReq_isUsed = 1u;
                break;
            case 32:
                error = decode_iso20_wpt_WPT_FinePositioningResType(stream, &exiDoc->WPT_FinePositioningRes);
                exiDoc->WPT_FinePositioningRes_isUsed = 1u;
                break;
            case 33:
                error = decode_iso20_wpt_WPT_FinePositioningSetupReqType(stream, &exiDoc->WPT_FinePositioningSetupReq);
                exiDoc->WPT_FinePositioningSetupReq_isUsed = 1u;
                break;
            case 34:
                error = decode_iso20_wpt_WPT_FinePositioningSetupResType(stream, &exiDoc->WPT_FinePositioningSetupRes);
                exiDoc->WPT_FinePositioningSetupRes_isUsed = 1u;
                break;
            case 35:
                error = decode_iso20_wpt_WPT_PairingReqType(stream, &exiDoc->WPT_PairingReq);
                exiDoc->WPT_PairingReq_isUsed = 1u;
                break;
            case 36:
                error = decode_iso20_wpt_WPT_PairingResType(stream, &exiDoc->WPT_PairingRes);
                exiDoc->WPT_PairingRes_isUsed = 1u;
                break;
            case 37:
                error = decode_iso20_wpt_X509DataType(stream, &exiDoc->X509Data);
                exiDoc->X509Data_isUsed = 1u;
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }
        }
    }

    return error;
}


