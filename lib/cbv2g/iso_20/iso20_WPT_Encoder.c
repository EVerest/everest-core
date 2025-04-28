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
  * @file iso20_WPT_Encoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_basetypes_encoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/iso_20/iso20_WPT_Datatypes.h"
#include "cbv2g/iso_20/iso20_WPT_Encoder.h"



static int encode_iso20_wpt_TransformType(exi_bitstream_t* stream, const struct iso20_wpt_TransformType* TransformType);
static int encode_iso20_wpt_TransformsType(exi_bitstream_t* stream, const struct iso20_wpt_TransformsType* TransformsType);
static int encode_iso20_wpt_DSAKeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_DSAKeyValueType* DSAKeyValueType);
static int encode_iso20_wpt_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType);
static int encode_iso20_wpt_DigestMethodType(exi_bitstream_t* stream, const struct iso20_wpt_DigestMethodType* DigestMethodType);
static int encode_iso20_wpt_RSAKeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_RSAKeyValueType* RSAKeyValueType);
static int encode_iso20_wpt_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethodType);
static int encode_iso20_wpt_WPT_TxRxPulseOrderType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType);
static int encode_iso20_wpt_SignatureMethodType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureMethodType* SignatureMethodType);
static int encode_iso20_wpt_KeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_KeyValueType* KeyValueType);
static int encode_iso20_wpt_WPT_CoordinateXYZType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType);
static int encode_iso20_wpt_ReferenceType(exi_bitstream_t* stream, const struct iso20_wpt_ReferenceType* ReferenceType);
static int encode_iso20_wpt_RetrievalMethodType(exi_bitstream_t* stream, const struct iso20_wpt_RetrievalMethodType* RetrievalMethodType);
static int encode_iso20_wpt_X509DataType(exi_bitstream_t* stream, const struct iso20_wpt_X509DataType* X509DataType);
static int encode_iso20_wpt_PGPDataType(exi_bitstream_t* stream, const struct iso20_wpt_PGPDataType* PGPDataType);
static int encode_iso20_wpt_SPKIDataType(exi_bitstream_t* stream, const struct iso20_wpt_SPKIDataType* SPKIDataType);
static int encode_iso20_wpt_SignedInfoType(exi_bitstream_t* stream, const struct iso20_wpt_SignedInfoType* SignedInfoType);
static int encode_iso20_wpt_SignatureValueType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureValueType* SignatureValueType);
static int encode_iso20_wpt_RationalNumberType(exi_bitstream_t* stream, const struct iso20_wpt_RationalNumberType* RationalNumberType);
static int encode_iso20_wpt_WPT_LF_RxRSSIType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType);
static int encode_iso20_wpt_WPT_LF_RxRSSIListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType);
static int encode_iso20_wpt_WPT_LF_TxDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType);
static int encode_iso20_wpt_WPT_LF_RxDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType);
static int encode_iso20_wpt_WPT_LF_TxDataListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType);
static int encode_iso20_wpt_KeyInfoType(exi_bitstream_t* stream, const struct iso20_wpt_KeyInfoType* KeyInfoType);
static int encode_iso20_wpt_WPT_TxRxSpecDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType);
static int encode_iso20_wpt_WPT_LF_RxDataListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType);
static int encode_iso20_wpt_ObjectType(exi_bitstream_t* stream, const struct iso20_wpt_ObjectType* ObjectType);
static int encode_iso20_wpt_WPT_TxRxPackageSpecDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType);
static int encode_iso20_wpt_WPT_LF_TransmitterDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType);
static int encode_iso20_wpt_AlternativeSECCType(exi_bitstream_t* stream, const struct iso20_wpt_AlternativeSECCType* AlternativeSECCType);
static int encode_iso20_wpt_WPT_LF_ReceiverDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType);
static int encode_iso20_wpt_WPT_LF_DataPackageType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType);
static int encode_iso20_wpt_DetailedCostType(exi_bitstream_t* stream, const struct iso20_wpt_DetailedCostType* DetailedCostType);
static int encode_iso20_wpt_SignatureType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureType* SignatureType);
static int encode_iso20_wpt_DetailedTaxType(exi_bitstream_t* stream, const struct iso20_wpt_DetailedTaxType* DetailedTaxType);
static int encode_iso20_wpt_MessageHeaderType(exi_bitstream_t* stream, const struct iso20_wpt_MessageHeaderType* MessageHeaderType);
static int encode_iso20_wpt_SignaturePropertyType(exi_bitstream_t* stream, const struct iso20_wpt_SignaturePropertyType* SignaturePropertyType);
static int encode_iso20_wpt_DisplayParametersType(exi_bitstream_t* stream, const struct iso20_wpt_DisplayParametersType* DisplayParametersType);
static int encode_iso20_wpt_WPT_FinePositioningMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType);
static int encode_iso20_wpt_EVSEStatusType(exi_bitstream_t* stream, const struct iso20_wpt_EVSEStatusType* EVSEStatusType);
static int encode_iso20_wpt_WPT_PairingMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType);
static int encode_iso20_wpt_MeterInfoType(exi_bitstream_t* stream, const struct iso20_wpt_MeterInfoType* MeterInfoType);
static int encode_iso20_wpt_WPT_AlignmentCheckMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType);
static int encode_iso20_wpt_WPT_LF_DataPackageListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType);
static int encode_iso20_wpt_AlternativeSECCListType(exi_bitstream_t* stream, const struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType);
static int encode_iso20_wpt_ReceiptType(exi_bitstream_t* stream, const struct iso20_wpt_ReceiptType* ReceiptType);
static int encode_iso20_wpt_WPT_LF_SystemSetupDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType);
static int encode_iso20_wpt_WPT_EVPCPowerControlParameterType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType);
static int encode_iso20_wpt_WPT_SPCPowerControlParameterType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType);
static int encode_iso20_wpt_WPT_FinePositioningSetupReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReqType);
static int encode_iso20_wpt_WPT_FinePositioningSetupResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupResType);
static int encode_iso20_wpt_WPT_FinePositioningReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReqType);
static int encode_iso20_wpt_WPT_FinePositioningResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningResType);
static int encode_iso20_wpt_WPT_PairingReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingReqType* WPT_PairingReqType);
static int encode_iso20_wpt_WPT_PairingResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingResType* WPT_PairingResType);
static int encode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReqType);
static int encode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryResType);
static int encode_iso20_wpt_WPT_AlignmentCheckReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReqType);
static int encode_iso20_wpt_WPT_AlignmentCheckResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckResType);
static int encode_iso20_wpt_WPT_ChargeLoopReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReqType);
static int encode_iso20_wpt_WPT_ChargeLoopResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopResType);
static int encode_iso20_wpt_CLReqControlModeType(exi_bitstream_t* stream, const struct iso20_wpt_CLReqControlModeType* CLReqControlModeType);
static int encode_iso20_wpt_CLResControlModeType(exi_bitstream_t* stream, const struct iso20_wpt_CLResControlModeType* CLResControlModeType);
static int encode_iso20_wpt_ManifestType(exi_bitstream_t* stream, const struct iso20_wpt_ManifestType* ManifestType);
static int encode_iso20_wpt_SignaturePropertiesType(exi_bitstream_t* stream, const struct iso20_wpt_SignaturePropertiesType* SignaturePropertiesType);

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int encode_iso20_wpt_TransformType(exi_bitstream_t* stream, const struct iso20_wpt_TransformType* TransformType) {
    int grammar_id = 0;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=1

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 1;
                    }
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
            if (TransformType->XPath_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (XPath, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->XPath.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso20_wpt_XPath_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (TransformType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)TransformType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, TransformType->ANY.bytesLen, TransformType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_TransformsType(exi_bitstream_t* stream, const struct iso20_wpt_TransformsType* TransformsType) {
    int grammar_id = 4;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Transform)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (TransformType); next=5
                error = encode_iso20_wpt_TransformType(stream, &TransformsType->Transform);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 5;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=2; START (Transform), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transform, TransformType); next=2
                    error = encode_iso20_wpt_TransformType(stream, &TransformsType->Transform);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_DSAKeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 6;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 6:
            // Grammar: ID=6; read/write bits=2; START (P), START (G), START (Y)
            if (DSAKeyValueType->P_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (P, base64Binary); next=7
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->P.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->P.bytesLen, DSAKeyValueType->P.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 7;
                                }
                            }
                        }
                    }
                }
            }
            else if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=9
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 9;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Y, base64Binary); next=10
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 10;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 7:
            // Grammar: ID=7; read/write bits=1; START (Q)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=8
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Q.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Q.bytesLen, DSAKeyValueType->Q.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 8;
                            }
                        }
                    }
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (G), START (Y)
            if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=9
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 9;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Y, base64Binary); next=10
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 10;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=1; START (Y)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=10
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 10;
                            }
                        }
                    }
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=2; START (J), START (Seed), END Element
            if (DSAKeyValueType->J_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (J, base64Binary); next=11
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->J.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->J.bytesLen, DSAKeyValueType->J.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 11;
                                }
                            }
                        }
                    }
                }
            }
            else if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 12;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (Seed), END Element
            if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 12;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=2; START (PgenCounter), END Element
            if (DSAKeyValueType->PgenCounter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PgenCounter, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->PgenCounter.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->PgenCounter.bytesLen, DSAKeyValueType->PgenCounter.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 13;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=14

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509IssuerSerialType->X509IssuerName.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso20_wpt_X509IssuerName_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 14;
                            }
                        }
                    }
                }
            }
            break;
        case 14:
            // Grammar: ID=14; read/write bits=1; START (X509SerialNumber)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (decimal); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_signed(stream, &X509IssuerSerialType->X509SerialNumber);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_DigestMethodType(exi_bitstream_t* stream, const struct iso20_wpt_DigestMethodType* DigestMethodType) {
    int grammar_id = 15;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 15:
            // Grammar: ID=15; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=16

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(DigestMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 16;
                    }
                }
            }
            break;
        case 16:
            // Grammar: ID=16; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (DigestMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DigestMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DigestMethodType->ANY.bytesLen, DigestMethodType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_RSAKeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 17;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 17:
            // Grammar: ID=17; read/write bits=1; START (Modulus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=18
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Modulus.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Modulus.bytesLen, RSAKeyValueType->Modulus.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 18;
                            }
                        }
                    }
                }
            }
            break;
        case 18:
            // Grammar: ID=18; read/write bits=1; START (Exponent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Exponent.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Exponent.bytesLen, RSAKeyValueType->Exponent.bytes, iso20_wpt_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 19;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 19:
            // Grammar: ID=19; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=20

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CanonicalizationMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 20;
                    }
                }
            }
            break;
        case 20:
            // Grammar: ID=20; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (CanonicalizationMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CanonicalizationMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, CanonicalizationMethodType->ANY.bytesLen, CanonicalizationMethodType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_TxRxPulseOrderType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType) {
    int grammar_id = 21;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 21:
            // Grammar: ID=21; read/write bits=1; START (IndexNumber)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=22
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_TxRxPulseOrderType->IndexNumber);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 22;
                        }
                    }
                }
            }
            break;
        case 22:
            // Grammar: ID=22; read/write bits=1; START (TxRxIdentifier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, WPT_TxRxPulseOrderType->TxRxIdentifier);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignatureMethodType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 23;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=24

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso20_wpt_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 24;
                    }
                }
            }
            break;
        case 24:
            // Grammar: ID=24; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            if (SignatureMethodType->HMACOutputLength_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (HMACOutputLength, integer); next=25
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 25;
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (SignatureMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (SignatureMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_KeyValueType(exi_bitstream_t* stream, const struct iso20_wpt_KeyValueType* KeyValueType) {
    int grammar_id = 26;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 26:
            // Grammar: ID=26; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            if (KeyValueType->DSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DSAKeyValue, DSAKeyValueType); next=2
                    error = encode_iso20_wpt_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyValueType->RSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RSAKeyValue, RSAKeyValueType); next=2
                    error = encode_iso20_wpt_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyValueType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyValueType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyValueType->ANY.bytesLen, KeyValueType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_CoordinateXYZType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType) {
    int grammar_id = 27;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 27:
            // Grammar: ID=27; read/write bits=1; START (Coord_X)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=28
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, WPT_CoordinateXYZType->Coord_X);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 28;
                        }
                    }
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=1; START (Coord_Y)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=29
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, WPT_CoordinateXYZType->Coord_Y);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 29;
                        }
                    }
                }
            }
            break;
        case 29:
            // Grammar: ID=29; read/write bits=1; START (Coord_Z)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, WPT_CoordinateXYZType->Coord_Z);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_ReferenceType(exi_bitstream_t* stream, const struct iso20_wpt_ReferenceType* ReferenceType) {
    int grammar_id = 30;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 30:
            // Grammar: ID=30; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=31

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 31;
                        }
                    }
                }
            }
            else if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=32

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 32;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=33

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 33;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=34
                    error = encode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 34;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=35
                    error = encode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            break;
        case 31:
            // Grammar: ID=31; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=32

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 32;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=33

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 33;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=34
                    error = encode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 34;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=35
                    error = encode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            break;
        case 32:
            // Grammar: ID=32; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=33

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 33;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=34
                    error = encode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 34;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=35
                    error = encode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            break;
        case 33:
            // Grammar: ID=33; read/write bits=2; START (Transforms), START (DigestMethod)
            if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=34
                    error = encode_iso20_wpt_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 34;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=35
                    error = encode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (DigestMethodType); next=35
                error = encode_iso20_wpt_DigestMethodType(stream, &ReferenceType->DigestMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 35;
                }
            }
            break;
        case 35:
            // Grammar: ID=35; read/write bits=1; START (DigestValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ReferenceType->DigestValue.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, ReferenceType->DigestValue.bytesLen, ReferenceType->DigestValue.bytes, iso20_wpt_DigestValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_RetrievalMethodType(exi_bitstream_t* stream, const struct iso20_wpt_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 36;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            if (RetrievalMethodType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=37

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso20_wpt_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 37;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=38

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 38;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=2; START (URI), START (Transforms), END Element
            if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=38

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_wpt_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 38;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 38:
            // Grammar: ID=38; read/write bits=2; START (Transforms), END Element
            if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_wpt_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_X509DataType(exi_bitstream_t* stream, const struct iso20_wpt_X509DataType* X509DataType) {
    int grammar_id = 39;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 39:
            // Grammar: ID=39; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            if (X509DataType->X509IssuerSerial_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerial, X509IssuerSerialType); next=2
                    error = encode_iso20_wpt_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (X509DataType->X509SKI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SKI, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509SKI.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509SKI.bytesLen, X509DataType->X509SKI.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509SubjectName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SubjectName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509DataType->X509SubjectName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso20_wpt_X509SubjectName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509Certificate_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Certificate, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509Certificate.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509Certificate.bytesLen, X509DataType->X509Certificate.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509CRL_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509CRL, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509CRL.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509CRL.bytesLen, X509DataType->X509CRL.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->ANY.bytesLen, X509DataType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_PGPDataType(exi_bitstream_t* stream, const struct iso20_wpt_PGPDataType* PGPDataType) {
    int grammar_id = 40;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 40:
            // Grammar: ID=40; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyID, base64Binary); next=41
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyID.bytesLen, PGPDataType->choice_1.PGPKeyID.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 41;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=42
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 42;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 41:
            // Grammar: ID=41; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=42
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 42;
                                }
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=43
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 43;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 42:
            // Grammar: ID=42; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=43
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 43;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 43:
            // Grammar: ID=43; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=44
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.PGPKeyPacket.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.PGPKeyPacket.bytesLen, PGPDataType->choice_2.PGPKeyPacket.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 44;
                            }
                        }
                    }
                }
            }
            break;
        case 44:
            // Grammar: ID=44; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (PGPDataType->choice_2_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=43
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.ANY.bytesLen, PGPDataType->choice_2.ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 43;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SPKIDataType(exi_bitstream_t* stream, const struct iso20_wpt_SPKIDataType* SPKIDataType) {
    int grammar_id = 45;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 45:
            // Grammar: ID=45; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=46
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->SPKISexp.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SPKIDataType->SPKISexp.bytesLen, SPKIDataType->SPKISexp.bytes, iso20_wpt_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 46;
                            }
                        }
                    }
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (SPKIDataType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SPKIDataType->ANY.bytesLen, SPKIDataType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignedInfoType(exi_bitstream_t* stream, const struct iso20_wpt_SignedInfoType* SignedInfoType) {
    int grammar_id = 47;
    int done = 0;
    int error = 0;
    uint16_t Reference_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 47:
            // Grammar: ID=47; read/write bits=2; START (Id), START (CanonicalizationMethod)
            if (SignedInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=48

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 48;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType); next=49
                    error = encode_iso20_wpt_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 49;
                    }
                }
            }
            break;
        case 48:
            // Grammar: ID=48; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CanonicalizationMethodType); next=49
                error = encode_iso20_wpt_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 49;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignatureMethodType); next=50
                error = encode_iso20_wpt_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 50;
                }
            }
            break;
        case 50:
            // Grammar: ID=50; read/write bits=1; START (Reference)
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=51
                    error = encode_iso20_wpt_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=2; LOOP (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ReferenceType); next=51
                    error = encode_iso20_wpt_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignatureValueType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureValueType* SignatureValueType) {
    int grammar_id = 52;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=2; START (Id), START (CONTENT)
            if (SignatureValueType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=53

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureValueType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 53;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CONTENT, base64Binary); next=2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso20_wpt_SignatureValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 53:
            // Grammar: ID=53; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso20_wpt_SignatureValueType_BYTES_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_RationalNumberType(exi_bitstream_t* stream, const struct iso20_wpt_RationalNumberType* RationalNumberType) {
    int grammar_id = 54;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 54:
            // Grammar: ID=54; read/write bits=1; START (Exponent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=55
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // type has min_value = -128
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, RationalNumberType->Exponent + -128);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 55;
                        }
                    }
                }
            }
            break;
        case 55:
            // Grammar: ID=55; read/write bits=1; START (Value)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, RationalNumberType->Value);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_RxRSSIType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType) {
    int grammar_id = 56;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 56:
            // Grammar: ID=56; read/write bits=1; START (TxIdentifier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=57
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, WPT_LF_RxRSSIType->TxIdentifier);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 57;
                        }
                    }
                }
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=1; START (RSSI)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_LF_RxRSSIType->RSSI);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_RxRSSIListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType) {
    int grammar_id = 58;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 58:
            // Grammar: ID=58; read/write bits=1; START (RSSIDataList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_LF_RxRSSIType); next=2
                error = encode_iso20_wpt_WPT_LF_RxRSSIType(stream, &WPT_LF_RxRSSIListType->RSSIDataList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_TxDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType) {
    int grammar_id = 59;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 59:
            // Grammar: ID=59; read/write bits=1; START (TxIdentifier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=60
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, WPT_LF_TxDataType->TxIdentifier);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 60;
                        }
                    }
                }
            }
            break;
        case 60:
            // Grammar: ID=60; read/write bits=1; START (EIRP)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_LF_TxDataType->EIRP);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_RxDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType) {
    int grammar_id = 61;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 61:
            // Grammar: ID=61; read/write bits=1; START (RxIdentifier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=62
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, WPT_LF_RxDataType->RxIdentifier);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 62;
                        }
                    }
                }
            }
            break;
        case 62:
            // Grammar: ID=62; read/write bits=1; START (RSSIData)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_LF_RxRSSIListType); next=2
                error = encode_iso20_wpt_WPT_LF_RxRSSIListType(stream, &WPT_LF_RxDataType->RSSIData);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_TxDataListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType) {
    int grammar_id = 63;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 63:
            // Grammar: ID=63; read/write bits=1; START (WPT_LF_TxDataList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_LF_TxDataType); next=2
                error = encode_iso20_wpt_WPT_LF_TxDataType(stream, &WPT_LF_TxDataListType->WPT_LF_TxDataList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_KeyInfoType(exi_bitstream_t* stream, const struct iso20_wpt_KeyInfoType* KeyInfoType) {
    int grammar_id = 64;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 64:
            // Grammar: ID=64; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=65

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 65;
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_wpt_KeyName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=2
                    error = encode_iso20_wpt_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=2
                    error = encode_iso20_wpt_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=2
                    error = encode_iso20_wpt_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=2
                    error = encode_iso20_wpt_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=2
                    error = encode_iso20_wpt_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_wpt_MgmtData_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 65:
            // Grammar: ID=65; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_wpt_KeyName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=2
                    error = encode_iso20_wpt_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=2
                    error = encode_iso20_wpt_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=2
                    error = encode_iso20_wpt_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=2
                    error = encode_iso20_wpt_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=2
                    error = encode_iso20_wpt_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_wpt_MgmtData_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_TxRxSpecDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType) {
    int grammar_id = 66;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 66:
            // Grammar: ID=66; read/write bits=1; START (TxRxIdentifier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=67
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, WPT_TxRxSpecDataType->TxRxIdentifier);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 67;
                        }
                    }
                }
            }
            break;
        case 67:
            // Grammar: ID=67; read/write bits=1; START (TxRxPosition)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_CoordinateXYZType); next=68
                error = encode_iso20_wpt_WPT_CoordinateXYZType(stream, &WPT_TxRxSpecDataType->TxRxPosition);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 68;
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=1; START (TxRxOrientation)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_CoordinateXYZType); next=2
                error = encode_iso20_wpt_WPT_CoordinateXYZType(stream, &WPT_TxRxSpecDataType->TxRxOrientation);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_RxDataListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType) {
    int grammar_id = 69;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 69:
            // Grammar: ID=69; read/write bits=1; START (WPT_LF_RxDataList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_LF_RxDataType); next=2
                error = encode_iso20_wpt_WPT_LF_RxDataType(stream, &WPT_LF_RxDataListType->WPT_LF_RxDataList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_ObjectType(exi_bitstream_t* stream, const struct iso20_wpt_ObjectType* ObjectType) {
    int grammar_id = 70;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 70:
            // Grammar: ID=70; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Encoding_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Encoding, anyURI); next=71

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Encoding.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso20_wpt_Encoding_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 71;
                        }
                    }
                }
            }
            else if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=72

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 72;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=73

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 73;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=3)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=72

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 72;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=73

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 73;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=2)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=73

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_wpt_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 73;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_TxRxPackageSpecDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType) {
    int grammar_id = 74;
    int done = 0;
    int error = 0;
    uint16_t PulseSequenceOrder_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 74:
            // Grammar: ID=74; read/write bits=1; START (PulseSequenceOrder)
            if (PulseSequenceOrder_currentIndex < WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_TxRxPulseOrderType); next=75
                    error = encode_iso20_wpt_WPT_TxRxPulseOrderType(stream, &WPT_TxRxPackageSpecDataType->PulseSequenceOrder.array[PulseSequenceOrder_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 75;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; LOOP (PulseSequenceOrder)
            if (PulseSequenceOrder_currentIndex < WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (WPT_TxRxPulseOrderType); next=75
                    error = encode_iso20_wpt_WPT_TxRxPulseOrderType(stream, &WPT_TxRxPackageSpecDataType->PulseSequenceOrder.array[PulseSequenceOrder_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 75;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=1; START (PulseSeparationTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=77
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_TxRxPackageSpecDataType->PulseSeparationTime);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 77;
                        }
                    }
                }
            }
            break;
        case 77:
            // Grammar: ID=77; read/write bits=1; START (PulseDuration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=78
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_TxRxPackageSpecDataType->PulseDuration);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 78;
                        }
                    }
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=1; START (PackageSeparationTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_TxRxPackageSpecDataType->PackageSeparationTime);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_TransmitterDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType) {
    int grammar_id = 79;
    int done = 0;
    int error = 0;
    uint16_t TxSpecData_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 79:
            // Grammar: ID=79; read/write bits=1; START (NumberOfTransmitters)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=80
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, WPT_LF_TransmitterDataType->NumberOfTransmitters);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 80;
                        }
                    }
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=1; START (SignalFrequency)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=81
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_LF_TransmitterDataType->SignalFrequency);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 81;
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=2; START (TxSpecData), END Element
            if (TxSpecData_currentIndex < WPT_LF_TransmitterDataType->TxSpecData.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_TxRxSpecDataType); next=82
                    error = encode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_TransmitterDataType->TxSpecData.array[TxSpecData_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 82;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 82:
            // Grammar: ID=82; read/write bits=1; LOOP (TxSpecData)
            if (TxSpecData_currentIndex < WPT_LF_TransmitterDataType->TxSpecData.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (WPT_TxRxSpecDataType); next=82
                    error = encode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_TransmitterDataType->TxSpecData.array[TxSpecData_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 82;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (TxPackageSpecData), END Element
            if (WPT_LF_TransmitterDataType->TxPackageSpecData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TxPackageSpecData, WPT_TxRxPackageSpecDataType); next=2
                    error = encode_iso20_wpt_WPT_TxRxPackageSpecDataType(stream, &WPT_LF_TransmitterDataType->TxPackageSpecData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_AlternativeSECCType(exi_bitstream_t* stream, const struct iso20_wpt_AlternativeSECCType* AlternativeSECCType) {
    int grammar_id = 84;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 84:
            // Grammar: ID=84; read/write bits=3; START (SSID), START (BSSID), START (IPAddress), START (Port), END Element
            if (AlternativeSECCType->SSID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SSID, string); next=85

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->SSID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->SSID.charactersLen, AlternativeSECCType->SSID.characters, iso20_wpt_SSID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 85;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->BSSID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BSSID, string); next=86

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->BSSID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->BSSID.charactersLen, AlternativeSECCType->BSSID.characters, iso20_wpt_BSSID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 86;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->IPAddress_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (IPAddress, string); next=87

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->IPAddress.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 87;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->Port_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Port, unsignedInt); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, AlternativeSECCType->Port);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=3; START (BSSID), START (IPAddress), START (Port), END Element
            if (AlternativeSECCType->BSSID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BSSID, string); next=86

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->BSSID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->BSSID.charactersLen, AlternativeSECCType->BSSID.characters, iso20_wpt_BSSID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 86;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->IPAddress_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (IPAddress, string); next=87

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->IPAddress.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 87;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->Port_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Port, unsignedInt); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, AlternativeSECCType->Port);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 86:
            // Grammar: ID=86; read/write bits=2; START (IPAddress), START (Port), END Element
            if (AlternativeSECCType->IPAddress_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (IPAddress, string); next=87

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AlternativeSECCType->IPAddress.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AlternativeSECCType->IPAddress.charactersLen, AlternativeSECCType->IPAddress.characters, iso20_wpt_IPAddress_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 87;
                                }
                            }
                        }
                    }
                }
            }
            else if (AlternativeSECCType->Port_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Port, unsignedInt); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, AlternativeSECCType->Port);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=2; START (Port), END Element
            if (AlternativeSECCType->Port_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Port, unsignedInt); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, AlternativeSECCType->Port);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_ReceiverDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType) {
    int grammar_id = 88;
    int done = 0;
    int error = 0;
    uint16_t RxSpecData_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 88:
            // Grammar: ID=88; read/write bits=1; START (NumberOfReceivers)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=89
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, WPT_LF_ReceiverDataType->NumberOfReceivers);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 89;
                        }
                    }
                }
            }
            break;
        case 89:
            // Grammar: ID=89; read/write bits=2; START (RxSpecData), END Element
            if (RxSpecData_currentIndex < WPT_LF_ReceiverDataType->RxSpecData.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_TxRxSpecDataType); next=90
                    error = encode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_ReceiverDataType->RxSpecData.array[RxSpecData_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 90;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=1; LOOP (RxSpecData)
            if (RxSpecData_currentIndex < WPT_LF_ReceiverDataType->RxSpecData.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (WPT_TxRxSpecDataType); next=90
                    error = encode_iso20_wpt_WPT_TxRxSpecDataType(stream, &WPT_LF_ReceiverDataType->RxSpecData.array[RxSpecData_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 90;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_DataPackageType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType) {
    int grammar_id = 91;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 91:
            // Grammar: ID=91; read/write bits=1; START (PackageIndex)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=92
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, WPT_LF_DataPackageType->PackageIndex);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 92;
                        }
                    }
                }
            }
            break;
        case 92:
            // Grammar: ID=92; read/write bits=2; START (LF_TxData), START (LF_RxData)
            if (WPT_LF_DataPackageType->LF_TxData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_TxData, WPT_LF_TxDataListType); next=2
                    error = encode_iso20_wpt_WPT_LF_TxDataListType(stream, &WPT_LF_DataPackageType->LF_TxData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_RxData, WPT_LF_RxDataListType); next=2
                    error = encode_iso20_wpt_WPT_LF_RxDataListType(stream, &WPT_LF_DataPackageType->LF_RxData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_DetailedCostType(exi_bitstream_t* stream, const struct iso20_wpt_DetailedCostType* DetailedCostType) {
    int grammar_id = 93;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 93:
            // Grammar: ID=93; read/write bits=1; START (Amount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=94
                error = encode_iso20_wpt_RationalNumberType(stream, &DetailedCostType->Amount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 94;
                }
            }
            break;
        case 94:
            // Grammar: ID=94; read/write bits=1; START (CostPerUnit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &DetailedCostType->CostPerUnit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignatureType(exi_bitstream_t* stream, const struct iso20_wpt_SignatureType* SignatureType) {
    int grammar_id = 95;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 95:
            // Grammar: ID=95; read/write bits=2; START (Id), START (SignedInfo)
            if (SignatureType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=96

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 96;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignedInfo, SignedInfoType); next=97
                    error = encode_iso20_wpt_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 97;
                    }
                }
            }
            break;
        case 96:
            // Grammar: ID=96; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedInfoType); next=97
                error = encode_iso20_wpt_SignedInfoType(stream, &SignatureType->SignedInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 97;
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=98
                error = encode_iso20_wpt_SignatureValueType(stream, &SignatureType->SignatureValue);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 98;
                }
            }
            break;
        case 98:
            // Grammar: ID=98; read/write bits=2; START (KeyInfo), START (Object), END Element
            if (SignatureType->KeyInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyInfo, KeyInfoType); next=100
                    error = encode_iso20_wpt_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 100;
                    }
                }
            }
            else if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=99
                    error = encode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 99;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=2
                    error = encode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 100:
            // Grammar: ID=100; read/write bits=2; START (Object), END Element
            if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=101
                    error = encode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 101;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 101:
            // Grammar: ID=101; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=2
                    error = encode_iso20_wpt_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_DetailedTaxType(exi_bitstream_t* stream, const struct iso20_wpt_DetailedTaxType* DetailedTaxType) {
    int grammar_id = 102;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=1; START (TaxRuleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=103
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, DetailedTaxType->TaxRuleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 103;
                        }
                    }
                }
            }
            break;
        case 103:
            // Grammar: ID=103; read/write bits=1; START (Amount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &DetailedTaxType->Amount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_MessageHeaderType(exi_bitstream_t* stream, const struct iso20_wpt_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 104;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 104:
            // Grammar: ID=104; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=105
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MessageHeaderType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MessageHeaderType->SessionID.bytesLen, MessageHeaderType->SessionID.bytes, iso20_wpt_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 105;
                            }
                        }
                    }
                }
            }
            break;
        case 105:
            // Grammar: ID=105; read/write bits=1; START (TimeStamp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=106
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, MessageHeaderType->TimeStamp);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 106;
                        }
                    }
                }
            }
            break;
        case 106:
            // Grammar: ID=106; read/write bits=2; START (Signature), END Element
            if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=2
                    error = encode_iso20_wpt_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignaturePropertyType(exi_bitstream_t* stream, const struct iso20_wpt_SignaturePropertyType* SignaturePropertyType) {
    int grammar_id = 107;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 107:
            // Grammar: ID=107; read/write bits=2; START (Id), START (Target)
            if (SignaturePropertyType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=108

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Id.charactersLen, SignaturePropertyType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 108;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Target, anyURI); next=109

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Target.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_wpt_Target_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 109;
                        }
                    }
                }
            }
            break;
        case 108:
            // Grammar: ID=108; read/write bits=1; START (Target)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=109

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Target.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_wpt_Target_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 109;
                    }
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=1; START (ANY)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignaturePropertyType->ANY.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignaturePropertyType->ANY.bytesLen, SignaturePropertyType->ANY.bytes, iso20_wpt_anyType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_DisplayParametersType(exi_bitstream_t* stream, const struct iso20_wpt_DisplayParametersType* DisplayParametersType) {
    int grammar_id = 110;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 110:
            // Grammar: ID=110; read/write bits=4; START (PresentSOC), START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->PresentSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PresentSOC, byte); next=111
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->PresentSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 111;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->MinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumSOC, byte); next=112
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 112;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=113
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 113;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->MaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumSOC, byte); next=114
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 114;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMinimumSOC, unsignedLong); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 115;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 9);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 10);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 111:
            // Grammar: ID=111; read/write bits=4; START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->MinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumSOC, byte); next=112
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 112;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=113
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 113;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->MaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumSOC, byte); next=114
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 114;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMinimumSOC, unsignedLong); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 115;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 9);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=4; START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=113
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 113;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->MaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumSOC, byte); next=114
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 114;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMinimumSOC, unsignedLong); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 115;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 113:
            // Grammar: ID=113; read/write bits=4; START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->MaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumSOC, byte); next=114
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DisplayParametersType->MaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 114;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMinimumSOC, unsignedLong); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 115;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=3; START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->RemainingTimeToMinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMinimumSOC, unsignedLong); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 115;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 115:
            // Grammar: ID=115; read/write bits=3; START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->RemainingTimeToTargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToTargetSOC, unsignedLong); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToTargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=3; START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->RemainingTimeToMaximumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToMaximumSOC, unsignedLong); next=117
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DisplayParametersType->RemainingTimeToMaximumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 117;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 117:
            // Grammar: ID=117; read/write bits=3; START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->ChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=118
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 118;
                            }
                        }
                    }
                }
            }
            else if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=2; START (BatteryEnergyCapacity), START (InletHot), END Element
            if (DisplayParametersType->BatteryEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BatteryEnergyCapacity, RationalNumberType); next=119
                    error = encode_iso20_wpt_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 119;
                    }
                }
            }
            else if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 119:
            // Grammar: ID=119; read/write bits=2; START (InletHot), END Element
            if (DisplayParametersType->InletHot_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (InletHot, boolean); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DisplayParametersType->InletHot);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_FinePositioningMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType) {
    int grammar_id = 120;
    int done = 0;
    int error = 0;
    uint16_t WPT_FinePositioningMethod_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 120:
            // Grammar: ID=120; read/write bits=1; START (WPT_FinePositioningMethod)
            if (WPT_FinePositioningMethod_currentIndex < WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=121
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.array[WPT_FinePositioningMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 121;
                            }
                        }
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 121:
            // Grammar: ID=121; read/write bits=2; LOOP (WPT_FinePositioningMethod), END Element
            if (WPT_FinePositioningMethod_currentIndex < WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=121
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.array[WPT_FinePositioningMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 121;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_EVSEStatusType(exi_bitstream_t* stream, const struct iso20_wpt_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 122;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 122:
            // Grammar: ID=122; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=123
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, EVSEStatusType->NotificationMaxDelay);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 123;
                        }
                    }
                }
            }
            break;
        case 123:
            // Grammar: ID=123; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, EVSEStatusType->EVSENotification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_PairingMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType) {
    int grammar_id = 124;
    int done = 0;
    int error = 0;
    uint16_t WPT_PairingMethod_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 124:
            // Grammar: ID=124; read/write bits=1; START (WPT_PairingMethod)
            if (WPT_PairingMethod_currentIndex < WPT_PairingMethodListType->WPT_PairingMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=125
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, WPT_PairingMethodListType->WPT_PairingMethod.array[WPT_PairingMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 125;
                            }
                        }
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 125:
            // Grammar: ID=125; read/write bits=2; LOOP (WPT_PairingMethod), END Element
            if (WPT_PairingMethod_currentIndex < WPT_PairingMethodListType->WPT_PairingMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=125
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, WPT_PairingMethodListType->WPT_PairingMethod.array[WPT_PairingMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 125;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_MeterInfoType(exi_bitstream_t* stream, const struct iso20_wpt_MeterInfoType* MeterInfoType) {
    int grammar_id = 126;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 126:
            // Grammar: ID=126; read/write bits=1; START (MeterID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=127

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeterInfoType->MeterID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso20_wpt_MeterID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 127;
                            }
                        }
                    }
                }
            }
            break;
        case 127:
            // Grammar: ID=127; read/write bits=1; START (ChargedEnergyReadingWh)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=128
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->ChargedEnergyReadingWh);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 128;
                        }
                    }
                }
            }
            break;
        case 128:
            // Grammar: ID=128; read/write bits=3; START (BPT_DischargedEnergyReadingWh), START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_DischargedEnergyReadingWh, nonNegativeInteger); next=129
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_DischargedEnergyReadingWh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 129;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->CapacitiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CapacitiveEnergyReadingVARh, nonNegativeInteger); next=130
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->CapacitiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 130;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=131
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 131;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=132
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_wpt_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 132;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=133
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 133;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=3; START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->CapacitiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CapacitiveEnergyReadingVARh, nonNegativeInteger); next=130
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->CapacitiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 130;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=131
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 131;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=132
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_wpt_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 132;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=133
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 133;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 130:
            // Grammar: ID=130; read/write bits=3; START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=131
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 131;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=132
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_wpt_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 132;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=133
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 133;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=3; START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=132
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_wpt_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 132;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=133
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 133;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 132:
            // Grammar: ID=132; read/write bits=2; START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=133
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 133;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 133:
            // Grammar: ID=133; read/write bits=2; START (MeterTimestamp), END Element
            if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_AlignmentCheckMethodListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType) {
    int grammar_id = 134;
    int done = 0;
    int error = 0;
    uint16_t WPT_AlignmentCheckMethod_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 134:
            // Grammar: ID=134; read/write bits=1; START (WPT_AlignmentCheckMethod)
            if (WPT_AlignmentCheckMethod_currentIndex < WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=135
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.array[WPT_AlignmentCheckMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 135;
                            }
                        }
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 135:
            // Grammar: ID=135; read/write bits=2; LOOP (WPT_AlignmentCheckMethod), END Element
            if (WPT_AlignmentCheckMethod_currentIndex < WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=135
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.array[WPT_AlignmentCheckMethod_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 135;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_DataPackageListType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType) {
    int grammar_id = 136;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 136:
            // Grammar: ID=136; read/write bits=1; START (NumPackages)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=137
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, WPT_LF_DataPackageListType->NumPackages);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 137;
                        }
                    }
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (WPT_LF_DataPackage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_LF_DataPackageType); next=2
                error = encode_iso20_wpt_WPT_LF_DataPackageType(stream, &WPT_LF_DataPackageListType->WPT_LF_DataPackage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_AlternativeSECCListType(exi_bitstream_t* stream, const struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType) {
    int grammar_id = 138;
    int done = 0;
    int error = 0;
    uint16_t AlternativeSECC_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 138:
            // Grammar: ID=138; read/write bits=1; START (AlternativeSECC)
            if (AlternativeSECC_currentIndex < AlternativeSECCListType->AlternativeSECC.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AlternativeSECCType); next=139
                    error = encode_iso20_wpt_AlternativeSECCType(stream, &AlternativeSECCListType->AlternativeSECC.array[AlternativeSECC_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 139;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 139:
            // Grammar: ID=139; read/write bits=2; LOOP (AlternativeSECC), END Element
            if (AlternativeSECC_currentIndex < AlternativeSECCListType->AlternativeSECC.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (AlternativeSECCType); next=139
                    error = encode_iso20_wpt_AlternativeSECCType(stream, &AlternativeSECCListType->AlternativeSECC.array[AlternativeSECC_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 139;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_ReceiptType(exi_bitstream_t* stream, const struct iso20_wpt_ReceiptType* ReceiptType) {
    int grammar_id = 140;
    int done = 0;
    int error = 0;
    uint16_t TaxCosts_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 140:
            // Grammar: ID=140; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=141
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, ReceiptType->TimeAnchor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 141;
                        }
                    }
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=3; START (EnergyCosts), START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->EnergyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EnergyCosts, DetailedCostType); next=143
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->EnergyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 143;
                    }
                }
            }
            else if (ReceiptType->OccupancyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OccupancyCosts, DetailedCostType); next=145
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 145;
                    }
                }
            }
            else if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=147
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 147;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=149
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 149;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=142 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 142;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 142:
            // Grammar: ID=142; read/write bits=2; LOOP (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (TaxCosts, DetailedTaxType); next=142 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 142;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=3; START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->OccupancyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OccupancyCosts, DetailedCostType); next=145
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 145;
                    }
                }
            }
            else if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=147
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 147;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=149
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 149;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=144 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 144;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=2; LOOP (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (TaxCosts, DetailedTaxType); next=144 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 144;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 145:
            // Grammar: ID=145; read/write bits=3; START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=147
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 147;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=149
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 149;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=146 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 146;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 146:
            // Grammar: ID=146; read/write bits=2; LOOP (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (TaxCosts, DetailedTaxType); next=146 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 146;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 147:
            // Grammar: ID=147; read/write bits=2; START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=149
                    error = encode_iso20_wpt_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 149;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=148 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 148;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 148:
            // Grammar: ID=148; read/write bits=2; LOOP (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (TaxCosts, DetailedTaxType); next=148 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 148;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 149:
            // Grammar: ID=149; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=150 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 150;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=2; LOOP (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (TaxCosts, DetailedTaxType); next=150 (optional array)
                    error = encode_iso20_wpt_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 150;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_LF_SystemSetupDataType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType) {
    int grammar_id = 151;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=2; START (LF_TransmitterSetupData), START (LF_ReceiverSetupData)
            if (WPT_LF_SystemSetupDataType->LF_TransmitterSetupData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_TransmitterSetupData, WPT_LF_TransmitterDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_TransmitterDataType(stream, &WPT_LF_SystemSetupDataType->LF_TransmitterSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_ReceiverSetupData, WPT_LF_ReceiverDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_ReceiverDataType(stream, &WPT_LF_SystemSetupDataType->LF_ReceiverSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_EVPCPowerControlParameterType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType) {
    int grammar_id = 152;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 152:
            // Grammar: ID=152; read/write bits=1; START (EVPCCoilCurrentRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=153
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCoilCurrentRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 153;
                }
            }
            break;
        case 153:
            // Grammar: ID=153; read/write bits=1; START (EVPCCoilCurrentInformation)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=154
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCoilCurrentInformation);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 154;
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=1; START (EVPCCurrentOutputInformation)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=155
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCCurrentOutputInformation);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 155;
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=1; START (EVPCVoltageOutputInformation)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_EVPCPowerControlParameterType->EVPCVoltageOutputInformation);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_SPCPowerControlParameterType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType) {
    int grammar_id = 156;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 156:
            // Grammar: ID=156; read/write bits=1; START (SPCPrimaryDeviceCoilCurrentInformation)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_SPCPowerControlParameterType->SPCPrimaryDeviceCoilCurrentInformation);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_FinePositioningSetupReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReqType) {
    int grammar_id = 157;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 157:
            // Grammar: ID=157; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=158
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningSetupReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 158;
                }
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=159
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_FinePositioningSetupReqType->EVProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 159;
                        }
                    }
                }
            }
            break;
        case 159:
            // Grammar: ID=159; read/write bits=1; START (EVDeviceFinePositioningMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_FinePositioningMethodListType); next=160
                error = encode_iso20_wpt_WPT_FinePositioningMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDeviceFinePositioningMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 160;
                }
            }
            break;
        case 160:
            // Grammar: ID=160; read/write bits=1; START (EVDevicePairingMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_PairingMethodListType); next=161
                error = encode_iso20_wpt_WPT_PairingMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDevicePairingMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 161;
                }
            }
            break;
        case 161:
            // Grammar: ID=161; read/write bits=1; START (EVDeviceAlignmentCheckMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_AlignmentCheckMethodListType); next=162
                error = encode_iso20_wpt_WPT_AlignmentCheckMethodListType(stream, &WPT_FinePositioningSetupReqType->EVDeviceAlignmentCheckMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 162;
                }
            }
            break;
        case 162:
            // Grammar: ID=162; read/write bits=1; START (NaturalOffset)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=163
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_FinePositioningSetupReqType->NaturalOffset);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 163;
                        }
                    }
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=164 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 164;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 164:
            // Grammar: ID=164; read/write bits=2; LOOP (VendorSpecificDataContainer), START (LF_SystemSetupData), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=165 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 165;
                                }
                            }
                        }
                    }
                }
            }
            else if (WPT_FinePositioningSetupReqType->LF_SystemSetupData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupReqType->LF_SystemSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=2; START (LF_SystemSetupData), END Element
            if (WPT_FinePositioningSetupReqType->LF_SystemSetupData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupReqType->LF_SystemSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_FinePositioningSetupResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupResType) {
    int grammar_id = 166;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 166:
            // Grammar: ID=166; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=167
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningSetupResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 167;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=168
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_FinePositioningSetupResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 168;
                        }
                    }
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=1; START (PrimaryDeviceFinePositioningMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_FinePositioningMethodListType); next=169
                error = encode_iso20_wpt_WPT_FinePositioningMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDeviceFinePositioningMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 169;
                }
            }
            break;
        case 169:
            // Grammar: ID=169; read/write bits=1; START (PrimaryDevicePairingMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_PairingMethodListType); next=170
                error = encode_iso20_wpt_WPT_PairingMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDevicePairingMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 170;
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=1; START (PrimaryDeviceAlignmentCheckMethodList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (WPT_AlignmentCheckMethodListType); next=171
                error = encode_iso20_wpt_WPT_AlignmentCheckMethodListType(stream, &WPT_FinePositioningSetupResType->PrimaryDeviceAlignmentCheckMethodList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 171;
                }
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=1; START (NaturalOffset)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=172
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_FinePositioningSetupResType->NaturalOffset);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 172;
                        }
                    }
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=173 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 173;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 173:
            // Grammar: ID=173; read/write bits=2; LOOP (VendorSpecificDataContainer), START (LF_SystemSetupData), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=174 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningSetupResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 174;
                                }
                            }
                        }
                    }
                }
            }
            else if (WPT_FinePositioningSetupResType->LF_SystemSetupData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupResType->LF_SystemSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=2; START (LF_SystemSetupData), END Element
            if (WPT_FinePositioningSetupResType->LF_SystemSetupData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (LF_SystemSetupData, WPT_LF_SystemSetupDataType); next=2
                    error = encode_iso20_wpt_WPT_LF_SystemSetupDataType(stream, &WPT_FinePositioningSetupResType->LF_SystemSetupData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_FinePositioningReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReqType) {
    int grammar_id = 175;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=176
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 176;
                }
            }
            break;
        case 176:
            // Grammar: ID=176; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=177
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_FinePositioningReqType->EVProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 177;
                        }
                    }
                }
            }
            break;
        case 177:
            // Grammar: ID=177; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=178
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_FinePositioningReqType->EVResultCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 178;
                        }
                    }
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=179 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 179;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 179:
            // Grammar: ID=179; read/write bits=2; LOOP (VendorSpecificDataContainer), START (WPT_LF_DataPackageList), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=180 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 180;
                                }
                            }
                        }
                    }
                }
            }
            else if (WPT_FinePositioningReqType->WPT_LF_DataPackageList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType); next=2
                    error = encode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningReqType->WPT_LF_DataPackageList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 180:
            // Grammar: ID=180; read/write bits=2; START (WPT_LF_DataPackageList), END Element
            if (WPT_FinePositioningReqType->WPT_LF_DataPackageList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType); next=2
                    error = encode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningReqType->WPT_LF_DataPackageList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_FinePositioningResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningResType) {
    int grammar_id = 181;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 181:
            // Grammar: ID=181; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=182
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_FinePositioningResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 182;
                }
            }
            break;
        case 182:
            // Grammar: ID=182; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=183
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_FinePositioningResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 183;
                        }
                    }
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=184
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_FinePositioningResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 184;
                        }
                    }
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=185 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 185;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 185:
            // Grammar: ID=185; read/write bits=2; LOOP (VendorSpecificDataContainer), START (WPT_LF_DataPackageList), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=186 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_FinePositioningResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 186;
                                }
                            }
                        }
                    }
                }
            }
            else if (WPT_FinePositioningResType->WPT_LF_DataPackageList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType); next=2
                    error = encode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningResType->WPT_LF_DataPackageList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=2; START (WPT_LF_DataPackageList), END Element
            if (WPT_FinePositioningResType->WPT_LF_DataPackageList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (WPT_LF_DataPackageList, WPT_LF_DataPackageListType); next=2
                    error = encode_iso20_wpt_WPT_LF_DataPackageListType(stream, &WPT_FinePositioningResType->WPT_LF_DataPackageList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_PairingReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingReqType* WPT_PairingReqType) {
    int grammar_id = 187;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 187:
            // Grammar: ID=187; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=188
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_PairingReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 188;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=189
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_PairingReqType->EVProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 189;
                        }
                    }
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=2; START (ObservedIDCode), START (EVResultCode)
            if (WPT_PairingReqType->ObservedIDCode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ObservedIDCode, unsignedInt); next=190
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, WPT_PairingReqType->ObservedIDCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 190;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVResultCode, string); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_PairingReqType->EVResultCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 191;
                            }
                        }
                    }
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=191
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_PairingReqType->EVResultCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 191;
                        }
                    }
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=192 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 192;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=192 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 192;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_PairingResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_PairingResType* WPT_PairingResType) {
    int grammar_id = 193;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 193:
            // Grammar: ID=193; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=194
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_PairingResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 194;
                }
            }
            break;
        case 194:
            // Grammar: ID=194; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=195
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_PairingResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 195;
                        }
                    }
                }
            }
            break;
        case 195:
            // Grammar: ID=195; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=196
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_PairingResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 196;
                        }
                    }
                }
            }
            break;
        case 196:
            // Grammar: ID=196; read/write bits=3; START (ObservedIDCode), START (AlternativeSECCList), START (VendorSpecificDataContainer), END Element
            if (WPT_PairingResType->ObservedIDCode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ObservedIDCode, unsignedInt); next=198
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, WPT_PairingResType->ObservedIDCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 198;
                            }
                        }
                    }
                }
            }
            else if (WPT_PairingResType->AlternativeSECCList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AlternativeSECCList, AlternativeSECCListType); next=200
                    error = encode_iso20_wpt_AlternativeSECCListType(stream, &WPT_PairingResType->AlternativeSECCList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 200;
                    }
                }
            }
            else if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=197 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 197;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 197:
            // Grammar: ID=197; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=197 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 197;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=2; START (AlternativeSECCList), START (VendorSpecificDataContainer), END Element
            if (WPT_PairingResType->AlternativeSECCList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AlternativeSECCList, AlternativeSECCListType); next=200
                    error = encode_iso20_wpt_AlternativeSECCListType(stream, &WPT_PairingResType->AlternativeSECCList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 200;
                    }
                }
            }
            else if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=199 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 199;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=199 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 199;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=201 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 201;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 201:
            // Grammar: ID=201; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_PairingResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=201 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_PairingResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 201;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReqType) {
    int grammar_id = 202;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 202:
            // Grammar: ID=202; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=203
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeParameterDiscoveryReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 203;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=1; START (EVPCMaxReceivablePower)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=204
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryReqType->EVPCMaxReceivablePower);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 204;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=1; START (SDMaxGroundClearence)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=205
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_ChargeParameterDiscoveryReqType->SDMaxGroundClearence);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 205;
                        }
                    }
                }
            }
            break;
        case 205:
            // Grammar: ID=205; read/write bits=1; START (SDMinGroundClearence)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=206
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_ChargeParameterDiscoveryReqType->SDMinGroundClearence);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 206;
                        }
                    }
                }
            }
            break;
        case 206:
            // Grammar: ID=206; read/write bits=1; START (EVPCNaturalFrequency)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=207
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryReqType->EVPCNaturalFrequency);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 207;
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=1; START (EVPCDeviceLocalControl)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=208
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, WPT_ChargeParameterDiscoveryReqType->EVPCDeviceLocalControl);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 208;
                        }
                    }
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=209 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 209;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 209:
            // Grammar: ID=209; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=209 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 209;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryResType) {
    int grammar_id = 210;
    int done = 0;
    int error = 0;
    uint16_t SDManufacturerSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 210:
            // Grammar: ID=210; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=211
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeParameterDiscoveryResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 211;
                }
            }
            break;
        case 211:
            // Grammar: ID=211; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=212
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_ChargeParameterDiscoveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 212;
                        }
                    }
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=1; START (PDInputPowerClass)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=213
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_ChargeParameterDiscoveryResType->PDInputPowerClass);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 213;
                        }
                    }
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=1; START (SDMinOutputPower)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=214
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->SDMinOutputPower);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 214;
                }
            }
            break;
        case 214:
            // Grammar: ID=214; read/write bits=1; START (SDMaxOutputPower)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=215
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->SDMaxOutputPower);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 215;
                }
            }
            break;
        case 215:
            // Grammar: ID=215; read/write bits=1; START (SDMaxGroundClearanceSupport)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=216
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_ChargeParameterDiscoveryResType->SDMaxGroundClearanceSupport);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 216;
                        }
                    }
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=1; START (SDMinGroundClearanceSupport)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=217
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, WPT_ChargeParameterDiscoveryResType->SDMinGroundClearanceSupport);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 217;
                        }
                    }
                }
            }
            break;
        case 217:
            // Grammar: ID=217; read/write bits=1; START (PDMinCoilCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=218
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->PDMinCoilCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 218;
                }
            }
            break;
        case 218:
            // Grammar: ID=218; read/write bits=1; START (PDMaxCoilCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=219
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeParameterDiscoveryResType->PDMaxCoilCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 219;
                }
            }
            break;
        case 219:
            // Grammar: ID=219; read/write bits=2; START (SDManufacturerSpecificDataContainer), END Element
            if (SDManufacturerSpecificDataContainer_currentIndex < WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SDManufacturerSpecificDataContainer, base64Binary); next=220 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                SDManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 220;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=2; LOOP (SDManufacturerSpecificDataContainer), END Element
            if (SDManufacturerSpecificDataContainer_currentIndex < WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (SDManufacturerSpecificDataContainer, base64Binary); next=220 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.array[SDManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                SDManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 220;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_AlignmentCheckReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReqType) {
    int grammar_id = 221;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 221:
            // Grammar: ID=221; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=222
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_AlignmentCheckReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 222;
                }
            }
            break;
        case 222:
            // Grammar: ID=222; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=223
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckReqType->EVProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 223;
                        }
                    }
                }
            }
            break;
        case 223:
            // Grammar: ID=223; read/write bits=2; START (TargetCoilCurrent), START (EVResultCode)
            if (WPT_AlignmentCheckReqType->TargetCoilCurrent_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetCoilCurrent, RationalNumberType); next=224
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckReqType->TargetCoilCurrent);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 224;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVResultCode, string); next=225
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckReqType->EVResultCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 225;
                            }
                        }
                    }
                }
            }
            break;
        case 224:
            // Grammar: ID=224; read/write bits=1; START (EVResultCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=225
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckReqType->EVResultCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 225;
                        }
                    }
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=226 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 226;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=226 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckReqType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 226;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_AlignmentCheckResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckResType) {
    int grammar_id = 227;
    int done = 0;
    int error = 0;
    uint16_t VendorSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=228
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_AlignmentCheckResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 228;
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=229
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_AlignmentCheckResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 229;
                        }
                    }
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=230
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_AlignmentCheckResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 230;
                        }
                    }
                }
            }
            break;
        case 230:
            // Grammar: ID=230; read/write bits=3; START (PowerTransmitted), START (SupplyDeviceCurrent), START (VendorSpecificDataContainer), END Element
            if (WPT_AlignmentCheckResType->PowerTransmitted_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerTransmitted, RationalNumberType); next=232
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->PowerTransmitted);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 232;
                    }
                }
            }
            else if (WPT_AlignmentCheckResType->SupplyDeviceCurrent_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SupplyDeviceCurrent, RationalNumberType); next=234
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->SupplyDeviceCurrent);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 234;
                    }
                }
            }
            else if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=231 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 231;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=231 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 231;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=2; START (SupplyDeviceCurrent), START (VendorSpecificDataContainer), END Element
            if (WPT_AlignmentCheckResType->SupplyDeviceCurrent_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SupplyDeviceCurrent, RationalNumberType); next=234
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_AlignmentCheckResType->SupplyDeviceCurrent);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 234;
                    }
                }
            }
            else if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=233 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 233;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=233 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 233;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=2; START (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VendorSpecificDataContainer, base64Binary); next=235 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 235;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 235:
            // Grammar: ID=235; read/write bits=2; LOOP (VendorSpecificDataContainer), END Element
            if (VendorSpecificDataContainer_currentIndex < WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (VendorSpecificDataContainer, base64Binary); next=235 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytesLen, WPT_AlignmentCheckResType->VendorSpecificDataContainer.array[VendorSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                VendorSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 235;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_ChargeLoopReqType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReqType) {
    int grammar_id = 236;
    int done = 0;
    int error = 0;
    uint16_t ManufacturerSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 236:
            // Grammar: ID=236; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=237
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeLoopReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 237;
                }
            }
            break;
        case 237:
            // Grammar: ID=237; read/write bits=2; START (DisplayParameters), START (MeterInfoRequested)
            if (WPT_ChargeLoopReqType->DisplayParameters_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DisplayParameters, DisplayParametersType); next=238
                    error = encode_iso20_wpt_DisplayParametersType(stream, &WPT_ChargeLoopReqType->DisplayParameters);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 238;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfoRequested, boolean); next=239
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, WPT_ChargeLoopReqType->MeterInfoRequested);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 239;
                            }
                        }
                    }
                }
            }
            break;
        case 238:
            // Grammar: ID=238; read/write bits=1; START (MeterInfoRequested)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=239
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, WPT_ChargeLoopReqType->MeterInfoRequested);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 239;
                        }
                    }
                }
            }
            break;
        case 239:
            // Grammar: ID=239; read/write bits=1; START (EVPCPowerRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=240
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCPowerRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 240;
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=1; START (EVPCPowerOutput)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=241
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCPowerOutput);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 241;
                }
            }
            break;
        case 241:
            // Grammar: ID=241; read/write bits=1; START (EVPCChargeDiagnostics)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=242
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, WPT_ChargeLoopReqType->EVPCChargeDiagnostics);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 242;
                        }
                    }
                }
            }
            break;
        case 242:
            // Grammar: ID=242; read/write bits=3; START (EVPCOperatingFrequency), START (EVPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            if (WPT_ChargeLoopReqType->EVPCOperatingFrequency_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCOperatingFrequency, RationalNumberType); next=244
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopReqType->EVPCOperatingFrequency);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 244;
                    }
                }
            }
            else if (WPT_ChargeLoopReqType->EVPCPowerControlParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType); next=246
                    error = encode_iso20_wpt_WPT_EVPCPowerControlParameterType(stream, &WPT_ChargeLoopReqType->EVPCPowerControlParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 246;
                    }
                }
            }
            else if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=243 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 243;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=243 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 243;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=2; START (EVPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            if (WPT_ChargeLoopReqType->EVPCPowerControlParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType); next=246
                    error = encode_iso20_wpt_WPT_EVPCPowerControlParameterType(stream, &WPT_ChargeLoopReqType->EVPCPowerControlParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 246;
                    }
                }
            }
            else if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=245 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 245;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 245:
            // Grammar: ID=245; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=245 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 245;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=2; START (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=247 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 247;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 247:
            // Grammar: ID=247; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=247 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 247;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_WPT_ChargeLoopResType(exi_bitstream_t* stream, const struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopResType) {
    int grammar_id = 248;
    int done = 0;
    int error = 0;
    uint16_t ManufacturerSpecificDataContainer_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 248:
            // Grammar: ID=248; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=249
                error = encode_iso20_wpt_MessageHeaderType(stream, &WPT_ChargeLoopResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 249;
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=250
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, WPT_ChargeLoopResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 250;
                        }
                    }
                }
            }
            break;
        case 250:
            // Grammar: ID=250; read/write bits=3; START (EVSEStatus), START (MeterInfo), START (Receipt), START (EVPCPowerRequest)
            if (WPT_ChargeLoopResType->EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEStatus, EVSEStatusType); next=251
                    error = encode_iso20_wpt_EVSEStatusType(stream, &WPT_ChargeLoopResType->EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 251;
                    }
                }
            }
            else if (WPT_ChargeLoopResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=252
                    error = encode_iso20_wpt_MeterInfoType(stream, &WPT_ChargeLoopResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 252;
                    }
                }
            }
            else if (WPT_ChargeLoopResType->Receipt_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Receipt, ReceiptType); next=253
                    error = encode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 253;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCPowerRequest, RationalNumberType); next=254
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=2; START (MeterInfo), START (Receipt), START (EVPCPowerRequest)
            if (WPT_ChargeLoopResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=252
                    error = encode_iso20_wpt_MeterInfoType(stream, &WPT_ChargeLoopResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 252;
                    }
                }
            }
            else if (WPT_ChargeLoopResType->Receipt_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Receipt, ReceiptType); next=253
                    error = encode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 253;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCPowerRequest, RationalNumberType); next=254
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=2; START (Receipt), START (EVPCPowerRequest)
            if (WPT_ChargeLoopResType->Receipt_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Receipt, ReceiptType); next=253
                    error = encode_iso20_wpt_ReceiptType(stream, &WPT_ChargeLoopResType->Receipt);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 253;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPCPowerRequest, RationalNumberType); next=254
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=1; START (EVPCPowerRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=254
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->EVPCPowerRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 254;
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=2; START (SDPowerInput), START (SPCMaxOutputPowerLimit)
            if (WPT_ChargeLoopResType->SDPowerInput_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SDPowerInput, RationalNumberType); next=255
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SDPowerInput);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPCMaxOutputPowerLimit, RationalNumberType); next=256
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMaxOutputPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 256;
                    }
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=1; START (SPCMaxOutputPowerLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=256
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMaxOutputPowerLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 256;
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=1; START (SPCMinOutputPowerLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=257
                error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCMinOutputPowerLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 257;
                }
            }
            break;
        case 257:
            // Grammar: ID=257; read/write bits=1; START (SPCChargeDiagnostics)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=258
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, WPT_ChargeLoopResType->SPCChargeDiagnostics);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 258;
                        }
                    }
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=3; START (SPCOperatingFrequency), START (SPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            if (WPT_ChargeLoopResType->SPCOperatingFrequency_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPCOperatingFrequency, RationalNumberType); next=260
                    error = encode_iso20_wpt_RationalNumberType(stream, &WPT_ChargeLoopResType->SPCOperatingFrequency);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 260;
                    }
                }
            }
            else if (WPT_ChargeLoopResType->SPCPowerControlParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPCPowerControlParameter, WPT_SPCPowerControlParameterType); next=262
                    error = encode_iso20_wpt_WPT_SPCPowerControlParameterType(stream, &WPT_ChargeLoopResType->SPCPowerControlParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 262;
                    }
                }
            }
            else if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=259 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 259;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=259 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 259;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=2; START (SPCPowerControlParameter), START (ManufacturerSpecificDataContainer), END Element
            if (WPT_ChargeLoopResType->SPCPowerControlParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPCPowerControlParameter, WPT_SPCPowerControlParameterType); next=262
                    error = encode_iso20_wpt_WPT_SPCPowerControlParameterType(stream, &WPT_ChargeLoopResType->SPCPowerControlParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 262;
                    }
                }
            }
            else if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=261 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 261;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 261:
            // Grammar: ID=261; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=261 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 261;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=2; START (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ManufacturerSpecificDataContainer, base64Binary); next=263 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 263;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 263:
            // Grammar: ID=263; read/write bits=2; LOOP (ManufacturerSpecificDataContainer), END Element
            if (ManufacturerSpecificDataContainer_currentIndex < WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ManufacturerSpecificDataContainer, base64Binary); next=263 (optional array)
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytesLen, WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.array[ManufacturerSpecificDataContainer_currentIndex].bytes, iso20_wpt_WPT_DataContainerType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ManufacturerSpecificDataContainer_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 263;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_CLReqControlModeType(exi_bitstream_t* stream, const struct iso20_wpt_CLReqControlModeType* CLReqControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)CLReqControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_wpt_CLResControlModeType(exi_bitstream_t* stream, const struct iso20_wpt_CLResControlModeType* CLResControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)CLResControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
static int encode_iso20_wpt_ManifestType(exi_bitstream_t* stream, const struct iso20_wpt_ManifestType* ManifestType) {
    int grammar_id = 264;
    int done = 0;
    int error = 0;
    uint16_t Reference_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 264:
            // Grammar: ID=264; read/write bits=2; START (Id), START (Reference)
            if (ManifestType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=266

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ManifestType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ManifestType->Id.charactersLen, ManifestType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 266;
                        }
                    }
                }
            }
            else
            {
                if (Reference_currentIndex < ManifestType->Reference.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (ReferenceType); next=265
                        error = encode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 265;
                        }
                    }
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=2; LOOP (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ReferenceType); next=265
                    error = encode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 265;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (Reference)
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=267
                    error = encode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 267;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=2; LOOP (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ReferenceType); next=267
                    error = encode_iso20_wpt_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 267;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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
static int encode_iso20_wpt_SignaturePropertiesType(exi_bitstream_t* stream, const struct iso20_wpt_SignaturePropertiesType* SignaturePropertiesType) {
    int grammar_id = 268;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 268:
            // Grammar: ID=268; read/write bits=2; START (Id), START (SignatureProperty)
            if (SignaturePropertiesType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=270

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertiesType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertiesType->Id.charactersLen, SignaturePropertiesType->Id.characters, iso20_wpt_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 270;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=269
                    error = encode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 269;
                    }
                }
            }
            break;
        case 269:
            // Grammar: ID=269; read/write bits=2; START (SignatureProperty), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=2
                    error = encode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 270:
            // Grammar: ID=270; read/write bits=1; START (SignatureProperty)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignaturePropertyType); next=271
                error = encode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 271;
                }
            }
            break;
        case 271:
            // Grammar: ID=271; read/write bits=2; START (SignatureProperty), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=2
                    error = encode_iso20_wpt_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
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


// main function for encoding
int encode_iso20_wpt_exiDocument(exi_bitstream_t* stream, struct iso20_wpt_exiDocument* exiDoc)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        if (exiDoc->CLReqControlMode_isUsed == 1)
        {
            // encode event 0
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_CLReqControlModeType(stream, &exiDoc->CLReqControlMode);
            }
        }
        else if (exiDoc->CLResControlMode_isUsed == 1)
        {
            // encode event 1
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_CLResControlModeType(stream, &exiDoc->CLResControlMode);
            }
        }
        else if (exiDoc->CanonicalizationMethod_isUsed == 1)
        {
            // encode event 2
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_CanonicalizationMethodType(stream, &exiDoc->CanonicalizationMethod);
            }
        }
        else if (exiDoc->DSAKeyValue_isUsed == 1)
        {
            // encode event 3
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 3);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_DSAKeyValueType(stream, &exiDoc->DSAKeyValue);
            }
        }
        else if (exiDoc->DigestMethod_isUsed == 1)
        {
            // encode event 4
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 4);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_DigestMethodType(stream, &exiDoc->DigestMethod);
            }
        }
        // simple type! encode_iso20_wpt_DigestValue;
        else if (exiDoc->KeyInfo_isUsed == 1)
        {
            // encode event 6
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 6);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_KeyInfoType(stream, &exiDoc->KeyInfo);
            }
        }
        // simple type! encode_iso20_wpt_KeyName;
        else if (exiDoc->KeyValue_isUsed == 1)
        {
            // encode event 8
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 8);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_KeyValueType(stream, &exiDoc->KeyValue);
            }
        }
        else if (exiDoc->Manifest_isUsed == 1)
        {
            // encode event 9
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 9);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_ManifestType(stream, &exiDoc->Manifest);
            }
        }
        // simple type! encode_iso20_wpt_MgmtData;
        else if (exiDoc->Object_isUsed == 1)
        {
            // encode event 11
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 11);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_ObjectType(stream, &exiDoc->Object);
            }
        }
        else if (exiDoc->PGPData_isUsed == 1)
        {
            // encode event 12
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 12);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_PGPDataType(stream, &exiDoc->PGPData);
            }
        }
        else if (exiDoc->RSAKeyValue_isUsed == 1)
        {
            // encode event 13
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 13);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_RSAKeyValueType(stream, &exiDoc->RSAKeyValue);
            }
        }
        else if (exiDoc->Reference_isUsed == 1)
        {
            // encode event 14
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 14);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_ReferenceType(stream, &exiDoc->Reference);
            }
        }
        else if (exiDoc->RetrievalMethod_isUsed == 1)
        {
            // encode event 15
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 15);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_RetrievalMethodType(stream, &exiDoc->RetrievalMethod);
            }
        }
        else if (exiDoc->SPKIData_isUsed == 1)
        {
            // encode event 16
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 16);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SPKIDataType(stream, &exiDoc->SPKIData);
            }
        }
        else if (exiDoc->SignatureMethod_isUsed == 1)
        {
            // encode event 17
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 17);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignatureMethodType(stream, &exiDoc->SignatureMethod);
            }
        }
        else if (exiDoc->SignatureProperties_isUsed == 1)
        {
            // encode event 18
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 18);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignaturePropertiesType(stream, &exiDoc->SignatureProperties);
            }
        }
        else if (exiDoc->SignatureProperty_isUsed == 1)
        {
            // encode event 19
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 19);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignaturePropertyType(stream, &exiDoc->SignatureProperty);
            }
        }
        else if (exiDoc->Signature_isUsed == 1)
        {
            // encode event 20
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 20);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignatureType(stream, &exiDoc->Signature);
            }
        }
        else if (exiDoc->SignatureValue_isUsed == 1)
        {
            // encode event 21
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 21);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignatureValueType(stream, &exiDoc->SignatureValue);
            }
        }
        else if (exiDoc->SignedInfo_isUsed == 1)
        {
            // encode event 22
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_SignedInfoType(stream, &exiDoc->SignedInfo);
            }
        }
        else if (exiDoc->Transform_isUsed == 1)
        {
            // encode event 23
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_TransformType(stream, &exiDoc->Transform);
            }
        }
        else if (exiDoc->Transforms_isUsed == 1)
        {
            // encode event 24
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_TransformsType(stream, &exiDoc->Transforms);
            }
        }
        else if (exiDoc->WPT_AlignmentCheckReq_isUsed == 1)
        {
            // encode event 25
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 25);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_AlignmentCheckReqType(stream, &exiDoc->WPT_AlignmentCheckReq);
            }
        }
        else if (exiDoc->WPT_AlignmentCheckRes_isUsed == 1)
        {
            // encode event 26
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 26);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_AlignmentCheckResType(stream, &exiDoc->WPT_AlignmentCheckRes);
            }
        }
        else if (exiDoc->WPT_ChargeLoopReq_isUsed == 1)
        {
            // encode event 27
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 27);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_ChargeLoopReqType(stream, &exiDoc->WPT_ChargeLoopReq);
            }
        }
        else if (exiDoc->WPT_ChargeLoopRes_isUsed == 1)
        {
            // encode event 28
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_ChargeLoopResType(stream, &exiDoc->WPT_ChargeLoopRes);
            }
        }
        else if (exiDoc->WPT_ChargeParameterDiscoveryReq_isUsed == 1)
        {
            // encode event 29
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 29);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(stream, &exiDoc->WPT_ChargeParameterDiscoveryReq);
            }
        }
        else if (exiDoc->WPT_ChargeParameterDiscoveryRes_isUsed == 1)
        {
            // encode event 30
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 30);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_ChargeParameterDiscoveryResType(stream, &exiDoc->WPT_ChargeParameterDiscoveryRes);
            }
        }
        else if (exiDoc->WPT_FinePositioningReq_isUsed == 1)
        {
            // encode event 31
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 31);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_FinePositioningReqType(stream, &exiDoc->WPT_FinePositioningReq);
            }
        }
        else if (exiDoc->WPT_FinePositioningRes_isUsed == 1)
        {
            // encode event 32
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 32);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_FinePositioningResType(stream, &exiDoc->WPT_FinePositioningRes);
            }
        }
        else if (exiDoc->WPT_FinePositioningSetupReq_isUsed == 1)
        {
            // encode event 33
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 33);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_FinePositioningSetupReqType(stream, &exiDoc->WPT_FinePositioningSetupReq);
            }
        }
        else if (exiDoc->WPT_FinePositioningSetupRes_isUsed == 1)
        {
            // encode event 34
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 34);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_FinePositioningSetupResType(stream, &exiDoc->WPT_FinePositioningSetupRes);
            }
        }
        else if (exiDoc->WPT_PairingReq_isUsed == 1)
        {
            // encode event 35
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 35);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_PairingReqType(stream, &exiDoc->WPT_PairingReq);
            }
        }
        else if (exiDoc->WPT_PairingRes_isUsed == 1)
        {
            // encode event 36
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 36);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_WPT_PairingResType(stream, &exiDoc->WPT_PairingRes);
            }
        }
        else if (exiDoc->X509Data_isUsed == 1)
        {
            // encode event 37
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 37);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_wpt_X509DataType(stream, &exiDoc->X509Data);
            }
        }
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }
    }

    return error;
}


