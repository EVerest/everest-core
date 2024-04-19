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
  * @file iso20_DC_Decoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "common/exi_basetypes.h"
#include "common/exi_types_decoder.h"
#include "common/exi_basetypes_decoder.h"
#include "common/exi_error_codes.h"
#include "common/exi_header.h"
#include "iso20_DC_Datatypes.h"
#include "iso20_DC_Decoder.h"



static int decode_iso20_dc_TransformType(exi_bitstream_t* stream, struct iso20_dc_TransformType* TransformType);
static int decode_iso20_dc_TransformsType(exi_bitstream_t* stream, struct iso20_dc_TransformsType* TransformsType);
static int decode_iso20_dc_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_dc_DSAKeyValueType* DSAKeyValueType);
static int decode_iso20_dc_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_dc_X509IssuerSerialType* X509IssuerSerialType);
static int decode_iso20_dc_DigestMethodType(exi_bitstream_t* stream, struct iso20_dc_DigestMethodType* DigestMethodType);
static int decode_iso20_dc_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_dc_RSAKeyValueType* RSAKeyValueType);
static int decode_iso20_dc_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_dc_CanonicalizationMethodType* CanonicalizationMethodType);
static int decode_iso20_dc_SignatureMethodType(exi_bitstream_t* stream, struct iso20_dc_SignatureMethodType* SignatureMethodType);
static int decode_iso20_dc_KeyValueType(exi_bitstream_t* stream, struct iso20_dc_KeyValueType* KeyValueType);
static int decode_iso20_dc_ReferenceType(exi_bitstream_t* stream, struct iso20_dc_ReferenceType* ReferenceType);
static int decode_iso20_dc_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_dc_RetrievalMethodType* RetrievalMethodType);
static int decode_iso20_dc_X509DataType(exi_bitstream_t* stream, struct iso20_dc_X509DataType* X509DataType);
static int decode_iso20_dc_PGPDataType(exi_bitstream_t* stream, struct iso20_dc_PGPDataType* PGPDataType);
static int decode_iso20_dc_SPKIDataType(exi_bitstream_t* stream, struct iso20_dc_SPKIDataType* SPKIDataType);
static int decode_iso20_dc_SignedInfoType(exi_bitstream_t* stream, struct iso20_dc_SignedInfoType* SignedInfoType);
static int decode_iso20_dc_SignatureValueType(exi_bitstream_t* stream, struct iso20_dc_SignatureValueType* SignatureValueType);
static int decode_iso20_dc_KeyInfoType(exi_bitstream_t* stream, struct iso20_dc_KeyInfoType* KeyInfoType);
static int decode_iso20_dc_ObjectType(exi_bitstream_t* stream, struct iso20_dc_ObjectType* ObjectType);
static int decode_iso20_dc_RationalNumberType(exi_bitstream_t* stream, struct iso20_dc_RationalNumberType* RationalNumberType);
static int decode_iso20_dc_DetailedCostType(exi_bitstream_t* stream, struct iso20_dc_DetailedCostType* DetailedCostType);
static int decode_iso20_dc_SignatureType(exi_bitstream_t* stream, struct iso20_dc_SignatureType* SignatureType);
static int decode_iso20_dc_DetailedTaxType(exi_bitstream_t* stream, struct iso20_dc_DetailedTaxType* DetailedTaxType);
static int decode_iso20_dc_MessageHeaderType(exi_bitstream_t* stream, struct iso20_dc_MessageHeaderType* MessageHeaderType);
static int decode_iso20_dc_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_dc_SignaturePropertyType* SignaturePropertyType);
static int decode_iso20_dc_DC_CPDReqEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_DC_CPDReqEnergyTransferModeType* DC_CPDReqEnergyTransferModeType);
static int decode_iso20_dc_DisplayParametersType(exi_bitstream_t* stream, struct iso20_dc_DisplayParametersType* DisplayParametersType);
static int decode_iso20_dc_DC_CPDResEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_DC_CPDResEnergyTransferModeType* DC_CPDResEnergyTransferModeType);
static int decode_iso20_dc_EVSEStatusType(exi_bitstream_t* stream, struct iso20_dc_EVSEStatusType* EVSEStatusType);
static int decode_iso20_dc_MeterInfoType(exi_bitstream_t* stream, struct iso20_dc_MeterInfoType* MeterInfoType);
static int decode_iso20_dc_Scheduled_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_Scheduled_DC_CLReqControlModeType* Scheduled_DC_CLReqControlModeType);
static int decode_iso20_dc_Dynamic_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_Dynamic_DC_CLReqControlModeType* Dynamic_DC_CLReqControlModeType);
static int decode_iso20_dc_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_CLReqControlModeType* CLReqControlModeType);
static int decode_iso20_dc_ReceiptType(exi_bitstream_t* stream, struct iso20_dc_ReceiptType* ReceiptType);
static int decode_iso20_dc_Scheduled_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_Scheduled_DC_CLResControlModeType* Scheduled_DC_CLResControlModeType);
static int decode_iso20_dc_Dynamic_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_Dynamic_DC_CLResControlModeType* Dynamic_DC_CLResControlModeType);
static int decode_iso20_dc_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_CLResControlModeType* CLResControlModeType);
static int decode_iso20_dc_DC_CableCheckReqType(exi_bitstream_t* stream, struct iso20_dc_DC_CableCheckReqType* DC_CableCheckReqType);
static int decode_iso20_dc_DC_CableCheckResType(exi_bitstream_t* stream, struct iso20_dc_DC_CableCheckResType* DC_CableCheckResType);
static int decode_iso20_dc_DC_PreChargeReqType(exi_bitstream_t* stream, struct iso20_dc_DC_PreChargeReqType* DC_PreChargeReqType);
static int decode_iso20_dc_DC_PreChargeResType(exi_bitstream_t* stream, struct iso20_dc_DC_PreChargeResType* DC_PreChargeResType);
static int decode_iso20_dc_DC_WeldingDetectionReqType(exi_bitstream_t* stream, struct iso20_dc_DC_WeldingDetectionReqType* DC_WeldingDetectionReqType);
static int decode_iso20_dc_DC_WeldingDetectionResType(exi_bitstream_t* stream, struct iso20_dc_DC_WeldingDetectionResType* DC_WeldingDetectionResType);
static int decode_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType* BPT_DC_CPDReqEnergyTransferModeType);
static int decode_iso20_dc_DC_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeParameterDiscoveryReqType* DC_ChargeParameterDiscoveryReqType);
static int decode_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType* BPT_DC_CPDResEnergyTransferModeType);
static int decode_iso20_dc_DC_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeParameterDiscoveryResType* DC_ChargeParameterDiscoveryResType);
static int decode_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType* BPT_Scheduled_DC_CLReqControlModeType);
static int decode_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType* BPT_Scheduled_DC_CLResControlModeType);
static int decode_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType* BPT_Dynamic_DC_CLReqControlModeType);
static int decode_iso20_dc_DC_ChargeLoopReqType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeLoopReqType* DC_ChargeLoopReqType);
static int decode_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType* BPT_Dynamic_DC_CLResControlModeType);
static int decode_iso20_dc_DC_ChargeLoopResType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeLoopResType* DC_ChargeLoopResType);
static int decode_iso20_dc_ManifestType(exi_bitstream_t* stream, struct iso20_dc_ManifestType* ManifestType);
static int decode_iso20_dc_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_dc_SignaturePropertiesType* SignaturePropertiesType);

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int decode_iso20_dc_TransformType(exi_bitstream_t* stream, struct iso20_dc_TransformType* TransformType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_TransformType(TransformType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso20_dc_Algorithm_CHARACTER_SIZE);
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
                switch(eventCode)
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
                                    error = exi_basetypes_decoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso20_dc_XPath_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &TransformType->ANY.bytesLen, &TransformType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
// Particle: Transform, TransformType (1, 1);
static int decode_iso20_dc_TransformsType(exi_bitstream_t* stream, struct iso20_dc_TransformsType* TransformsType) {
    int grammar_id = 4;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_TransformsType(TransformsType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Transform)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=5
                    // decode: element
                    error = decode_iso20_dc_TransformType(stream, &TransformsType->Transform);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=2
                    // decode: element
                    error = decode_iso20_dc_TransformType(stream, &TransformsType->Transform);
                    if (error == 0)
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
                switch(eventCode)
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
// Particle: P, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); Q, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']);
static int decode_iso20_dc_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_dc_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 6;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DSAKeyValueType(DSAKeyValueType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 6:
            // Grammar: ID=6; read/write bits=2; START (P), START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (P, CryptoBinary (base64Binary)); next=7
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->P.bytesLen, &DSAKeyValueType->P.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->P_isUsed = 1u;
                        grammar_id = 7;
                    }
                    break;
                case 1:
                    // Event: START (G, CryptoBinary (base64Binary)); next=9
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 2:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (Q, CryptoBinary (base64Binary)); next=8
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Q.bytesLen, &DSAKeyValueType->Q.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (G, CryptoBinary (base64Binary)); next=9
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 1:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (J, CryptoBinary (base64Binary)); next=11
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->J.bytesLen, &DSAKeyValueType->J.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->J_isUsed = 1u;
                        grammar_id = 11;
                    }
                    break;
                case 1:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (PgenCounter, CryptoBinary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->PgenCounter.bytesLen, &DSAKeyValueType->PgenCounter.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_dc_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 13;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_X509IssuerSerialType(X509IssuerSerialType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                                    error = exi_basetypes_decoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso20_dc_X509IssuerName_CHARACTER_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (X509SerialNumber, integer (decimal)); next=2
                    // decode: int
                    error = decode_exi_type_integer32(stream, &X509IssuerSerialType->X509SerialNumber);
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
                switch(eventCode)
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
static int decode_iso20_dc_DigestMethodType(exi_bitstream_t* stream, struct iso20_dc_DigestMethodType* DigestMethodType) {
    int grammar_id = 15;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DigestMethodType(DigestMethodType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 15:
            // Grammar: ID=15; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso20_dc_Algorithm_CHARACTER_SIZE);
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
                switch(eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &DigestMethodType->ANY.bytesLen, &DigestMethodType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_dc_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 17;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_RSAKeyValueType(RSAKeyValueType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 17:
            // Grammar: ID=17; read/write bits=1; START (Modulus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Modulus, CryptoBinary (base64Binary)); next=18
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Modulus.bytesLen, &RSAKeyValueType->Modulus.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (Exponent, CryptoBinary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Exponent.bytesLen, &RSAKeyValueType->Exponent.bytes[0], iso20_dc_CryptoBinary_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_dc_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 19;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_CanonicalizationMethodType(CanonicalizationMethodType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 19:
            // Grammar: ID=19; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso20_dc_Algorithm_CHARACTER_SIZE);
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
                switch(eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &CanonicalizationMethodType->ANY.bytesLen, &CanonicalizationMethodType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_SignatureMethodType(exi_bitstream_t* stream, struct iso20_dc_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 21;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignatureMethodType(SignatureMethodType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 21:
            // Grammar: ID=21; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=22
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso20_dc_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 22;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 22:
            // Grammar: ID=22; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (HMACOutputLength, HMACOutputLengthType (integer)); next=23
                    // decode: int
                    error = decode_exi_type_integer32(stream, &SignatureMethodType->HMACOutputLength);
                    if (error == 0)
                    {
                        SignatureMethodType->HMACOutputLength_isUsed = 1u;
                        grammar_id = 23;
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
        case 23:
            // Grammar: ID=23; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_KeyValueType(exi_bitstream_t* stream, struct iso20_dc_KeyValueType* KeyValueType) {
    int grammar_id = 24;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_KeyValueType(KeyValueType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 24:
            // Grammar: ID=24; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DSAKeyValue, DSAKeyValueType (DSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_dc_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->DSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (RSAKeyValue, RSAKeyValueType (RSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->RSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyValueType->ANY.bytesLen, &KeyValueType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_ReferenceType(exi_bitstream_t* stream, struct iso20_dc_ReferenceType* ReferenceType) {
    int grammar_id = 25;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_ReferenceType(ReferenceType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 25:
            // Grammar: ID=25; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=26
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Id_isUsed = 1u;
                    grammar_id = 26;
                    break;
                case 1:
                    // Event: START (Type, anyURI (anyURI)); next=27
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_dc_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 27;
                    break;
                case 2:
                    // Event: START (URI, anyURI (anyURI)); next=28
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_dc_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 28;
                    break;
                case 3:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=29
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 4:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_dc_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 30;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 26:
            // Grammar: ID=26; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=27
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_dc_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 27;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=28
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_dc_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 28;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=29
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 3:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_dc_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 30;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 27:
            // Grammar: ID=27; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=28
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_dc_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 28;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=29
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 2:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_dc_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 30;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=2; START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=29
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 1:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_dc_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 30;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 29:
            // Grammar: ID=29; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_dc_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 30;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 30:
            // Grammar: ID=30; read/write bits=1; START (DigestValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DigestValue, DigestValueType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ReferenceType->DigestValue.bytesLen, &ReferenceType->DigestValue.bytes[0], iso20_dc_DigestValueType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_dc_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 31;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_RetrievalMethodType(RetrievalMethodType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 31:
            // Grammar: ID=31; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=32
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso20_dc_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->Type_isUsed = 1u;
                    grammar_id = 32;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=33
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_dc_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 33;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 32:
            // Grammar: ID=32; read/write bits=2; START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=33
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_dc_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 33;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 33:
            // Grammar: ID=33; read/write bits=2; START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_dc_TransformsType(stream, &RetrievalMethodType->Transforms);
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
                switch(eventCode)
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
static int decode_iso20_dc_X509DataType(exi_bitstream_t* stream, struct iso20_dc_X509DataType* X509DataType) {
    int grammar_id = 34;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_X509DataType(X509DataType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 34:
            // Grammar: ID=34; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (X509IssuerSerial, X509IssuerSerialType (X509IssuerSerialType)); next=2
                    // decode: element
                    error = decode_iso20_dc_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == 0)
                    {
                        X509DataType->X509IssuerSerial_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (X509SKI, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509SKI.bytesLen, &X509DataType->X509SKI.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
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
                                    error = exi_basetypes_decoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso20_dc_X509SubjectName_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509Certificate.bytesLen, &X509DataType->X509Certificate.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509Certificate_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509CRL, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509CRL.bytesLen, &X509DataType->X509CRL.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509CRL_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->ANY.bytesLen, &X509DataType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_PGPDataType(exi_bitstream_t* stream, struct iso20_dc_PGPDataType* PGPDataType) {
    int grammar_id = 35;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_PGPDataType(PGPDataType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 35:
            // Grammar: ID=35; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (PGPKeyID, base64Binary (base64Binary)); next=36
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyID.bytesLen, &PGPDataType->choice_1.PGPKeyID.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 36;
                    }
                    break;
                case 1:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=37
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 37;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 36:
            // Grammar: ID=36; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=37
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 37;
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=38
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=38
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 38;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=38
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
                    // Event: START (ANY, anyType (base64Binary)); next=38
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 38;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 38:
            // Grammar: ID=38; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=39
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.PGPKeyPacket.bytesLen, &PGPDataType->choice_2.PGPKeyPacket.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 39;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=38
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=38
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.ANY.bytesLen, &PGPDataType->choice_2.ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_2.ANY_isUsed = 1u;
                        grammar_id = 38;
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
                switch(eventCode)
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
static int decode_iso20_dc_SPKIDataType(exi_bitstream_t* stream, struct iso20_dc_SPKIDataType* SPKIDataType) {
    int grammar_id = 40;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SPKIDataType(SPKIDataType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 40:
            // Grammar: ID=40; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SPKISexp, base64Binary (base64Binary)); next=41
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->SPKISexp.bytesLen, &SPKIDataType->SPKISexp.bytes[0], iso20_dc_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 41;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 41:
            // Grammar: ID=41; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->ANY.bytesLen, &SPKIDataType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4);
static int decode_iso20_dc_SignedInfoType(exi_bitstream_t* stream, struct iso20_dc_SignedInfoType* SignedInfoType) {
    int grammar_id = 42;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignedInfoType(SignedInfoType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 42:
            // Grammar: ID=42; read/write bits=2; START (Id), START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=43
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignedInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignedInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignedInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignedInfoType->Id_isUsed = 1u;
                    grammar_id = 43;
                    break;
                case 1:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=44
                    // decode: element
                    error = decode_iso20_dc_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
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
        case 43:
            // Grammar: ID=43; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=44
                    // decode: element
                    error = decode_iso20_dc_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
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
            // Grammar: ID=44; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignatureMethod, SignatureMethodType (SignatureMethodType)); next=45
                    // decode: element
                    error = decode_iso20_dc_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                    if (error == 0)
                    {
                        grammar_id = 45;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 45:
            // Grammar: ID=45; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=46
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 46;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=47
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 47;
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
        case 47:
            // Grammar: ID=47; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=48
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 48;
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
        case 48:
            // Grammar: ID=48; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=49
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 49;
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
        case 49:
            // Grammar: ID=49; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=2
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
                switch(eventCode)
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
static int decode_iso20_dc_SignatureValueType(exi_bitstream_t* stream, struct iso20_dc_SignatureValueType* SignatureValueType) {
    int grammar_id = 50;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignatureValueType(SignatureValueType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 50:
            // Grammar: ID=50; read/write bits=2; START (Id), START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=51
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureValueType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureValueType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureValueType->Id_isUsed = 1u;
                    grammar_id = 51;
                    break;
                case 1:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=2
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_dc_SignatureValueType_BYTES_SIZE);
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
        case 51:
            // Grammar: ID=51; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=2
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_dc_SignatureValueType_BYTES_SIZE);
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
                switch(eventCode)
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
static int decode_iso20_dc_KeyInfoType(exi_bitstream_t* stream, struct iso20_dc_KeyInfoType* KeyInfoType) {
    int grammar_id = 52;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_KeyInfoType(KeyInfoType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=53
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (KeyInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            KeyInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    KeyInfoType->Id_isUsed = 1u;
                    grammar_id = 53;
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_dc_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso20_dc_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 6:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_dc_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
        case 53:
            // Grammar: ID=53; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_dc_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso20_dc_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_dc_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_dc_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1)(old 1, 1);
static int decode_iso20_dc_ObjectType(exi_bitstream_t* stream, struct iso20_dc_ObjectType* ObjectType) {
    int grammar_id = 54;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_ObjectType(ObjectType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 54:
            // Grammar: ID=54; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Encoding, anyURI (anyURI)); next=55
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Encoding.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Encoding.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Encoding.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso20_dc_Encoding_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Encoding_isUsed = 1u;
                    grammar_id = 55;
                    break;
                case 1:
                    // Event: START (Id, ID (NCName)); next=56
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 56;
                    break;
                case 2:
                    // Event: START (MimeType, string (string)); next=57
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_dc_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 57;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
        case 55:
            // Grammar: ID=55; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=56
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 56;
                    break;
                case 1:
                    // Event: START (MimeType, string (string)); next=57
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_dc_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 57;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
        case 56:
            // Grammar: ID=56; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MimeType, string (string)); next=57
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_dc_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 57;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
        case 57:
            // Grammar: ID=57; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}EVMaximumChargePower; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
static int decode_iso20_dc_RationalNumberType(exi_bitstream_t* stream, struct iso20_dc_RationalNumberType* RationalNumberType) {
    int grammar_id = 58;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_RationalNumberType(RationalNumberType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 58:
            // Grammar: ID=58; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Exponent, byte (short)); next=59
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
                                grammar_id = 59;
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
        case 59:
            // Grammar: ID=59; read/write bits=1; START (Value)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                switch(eventCode)
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
static int decode_iso20_dc_DetailedCostType(exi_bitstream_t* stream, struct iso20_dc_DetailedCostType* DetailedCostType) {
    int grammar_id = 60;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DetailedCostType(DetailedCostType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 60:
            // Grammar: ID=60; read/write bits=1; START (Amount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Amount, RationalNumberType (RationalNumberType)); next=61
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DetailedCostType->Amount);
                    if (error == 0)
                    {
                        grammar_id = 61;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 61:
            // Grammar: ID=61; read/write bits=1; START (CostPerUnit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (CostPerUnit, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DetailedCostType->CostPerUnit);
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
                switch(eventCode)
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
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1);
static int decode_iso20_dc_SignatureType(exi_bitstream_t* stream, struct iso20_dc_SignatureType* SignatureType) {
    int grammar_id = 62;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignatureType(SignatureType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 62:
            // Grammar: ID=62; read/write bits=2; START (Id), START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=63
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureType->Id_isUsed = 1u;
                    grammar_id = 63;
                    break;
                case 1:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=64
                    // decode: element
                    error = decode_iso20_dc_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 64;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 63:
            // Grammar: ID=63; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=64
                    // decode: element
                    error = decode_iso20_dc_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 64;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 64:
            // Grammar: ID=64; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignatureValue, SignatureValueType (base64Binary)); next=65
                    // decode: element
                    error = decode_iso20_dc_SignatureValueType(stream, &SignatureType->SignatureValue);
                    if (error == 0)
                    {
                        grammar_id = 65;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 65:
            // Grammar: ID=65; read/write bits=2; START (KeyInfo), START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (KeyInfo, KeyInfoType (KeyInfoType)); next=67
                    // decode: element
                    error = decode_iso20_dc_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == 0)
                    {
                        SignatureType->KeyInfo_isUsed = 1u;
                        grammar_id = 67;
                    }
                    break;
                case 1:
                    // Event: START (Object, ObjectType (ObjectType)); next=66
                    // decode: element
                    error = decode_iso20_dc_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 66;
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
        case 66:
            // Grammar: ID=66; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=2
                    // decode: element
                    error = decode_iso20_dc_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
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
        case 67:
            // Grammar: ID=67; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=68
                    // decode: element
                    error = decode_iso20_dc_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 68;
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
        case 68:
            // Grammar: ID=68; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=2
                    // decode: element
                    error = decode_iso20_dc_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
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
                switch(eventCode)
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
static int decode_iso20_dc_DetailedTaxType(exi_bitstream_t* stream, struct iso20_dc_DetailedTaxType* DetailedTaxType) {
    int grammar_id = 69;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DetailedTaxType(DetailedTaxType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 69:
            // Grammar: ID=69; read/write bits=1; START (TaxRuleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxRuleID, numericIDType (unsignedInt)); next=70
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DetailedTaxType->TaxRuleID);
                    if (error == 0)
                    {
                        grammar_id = 70;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 70:
            // Grammar: ID=70; read/write bits=1; START (Amount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Amount, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DetailedTaxType->Amount);
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
                switch(eventCode)
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
static int decode_iso20_dc_MessageHeaderType(exi_bitstream_t* stream, struct iso20_dc_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 71;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_MessageHeaderType(MessageHeaderType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 71:
            // Grammar: ID=71; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=72
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MessageHeaderType->SessionID.bytesLen, &MessageHeaderType->SessionID.bytes[0], iso20_dc_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=1; START (TimeStamp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TimeStamp, unsignedLong (nonNegativeInteger)); next=73
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MessageHeaderType->TimeStamp);
                    if (error == 0)
                    {
                        grammar_id = 73;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=2; START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Signature, SignatureType (SignatureType)); next=2
                    // decode: element
                    error = decode_iso20_dc_SignatureType(stream, &MessageHeaderType->Signature);
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
                switch(eventCode)
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
static int decode_iso20_dc_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_dc_SignaturePropertyType* SignaturePropertyType) {
    int grammar_id = 74;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignaturePropertyType(SignaturePropertyType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 74:
            // Grammar: ID=74; read/write bits=2; START (Id), START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=75
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Id.charactersLen, SignaturePropertyType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertyType->Id_isUsed = 1u;
                    grammar_id = 75;
                    break;
                case 1:
                    // Event: START (Target, anyURI (anyURI)); next=76
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_dc_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 76;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Target, anyURI (anyURI)); next=76
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_dc_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 76;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=1; START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignaturePropertyType->ANY.bytesLen, &SignaturePropertyType->ANY.bytes[0], iso20_dc_anyType_BYTES_SIZE);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}DC_CPDReqEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMinimumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); TargetSOC, percentValueType (0, 1);
static int decode_iso20_dc_DC_CPDReqEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_DC_CPDReqEnergyTransferModeType* DC_CPDReqEnergyTransferModeType) {
    int grammar_id = 77;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_CPDReqEnergyTransferModeType(DC_CPDReqEnergyTransferModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=1; START (EVMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=78
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMaximumChargePower);
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
            // Grammar: ID=78; read/write bits=1; START (EVMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=79
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 79;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=1; START (EVMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=80
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 80;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=1; START (EVMinimumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargeCurrent, RationalNumberType (RationalNumberType)); next=81
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMinimumChargeCurrent);
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
            // Grammar: ID=81; read/write bits=1; START (EVMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=82
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 82;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 82:
            // Grammar: ID=82; read/write bits=1; START (EVMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=83
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDReqEnergyTransferModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 83;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (TargetSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=2
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
                                DC_CPDReqEnergyTransferModeType->TargetSOC = (int8_t)value;
                                DC_CPDReqEnergyTransferModeType->TargetSOC_isUsed = 1u;
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
                switch(eventCode)
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
static int decode_iso20_dc_DisplayParametersType(exi_bitstream_t* stream, struct iso20_dc_DisplayParametersType* DisplayParametersType) {
    int grammar_id = 84;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DisplayParametersType(DisplayParametersType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 84:
            // Grammar: ID=84; read/write bits=4; START (PresentSOC), START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (PresentSOC, percentValueType (byte)); next=85
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
                    // Event: START (MinimumSOC, percentValueType (byte)); next=86
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
                    // Event: START (TargetSOC, percentValueType (byte)); next=87
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
                    // Event: START (MaximumSOC, percentValueType (byte)); next=88
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
                                grammar_id = 88;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=89
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 89;
                    }
                    break;
                case 5:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 6:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 7:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 8:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 85:
            // Grammar: ID=85; read/write bits=4; START (MinimumSOC), START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=86
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
                    // Event: START (TargetSOC, percentValueType (byte)); next=87
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
                    // Event: START (MaximumSOC, percentValueType (byte)); next=88
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
                                grammar_id = 88;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=89
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 89;
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 5:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 6:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 7:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 86:
            // Grammar: ID=86; read/write bits=4; START (TargetSOC), START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=87
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
                    // Event: START (MaximumSOC, percentValueType (byte)); next=88
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
                                grammar_id = 88;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=89
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 89;
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 4:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 5:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 6:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 87:
            // Grammar: ID=87; read/write bits=4; START (MaximumSOC), START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MaximumSOC, percentValueType (byte)); next=88
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
                                grammar_id = 88;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=89
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 89;
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 3:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 4:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 5:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 88:
            // Grammar: ID=88; read/write bits=3; START (RemainingTimeToMinimumSOC), START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToMinimumSOC, unsignedInt (unsignedLong)); next=89
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMinimumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 1u;
                        grammar_id = 89;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 2:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 3:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 89:
            // Grammar: ID=89; read/write bits=3; START (RemainingTimeToTargetSOC), START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToTargetSOC, unsignedInt (unsignedLong)); next=90
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToTargetSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 1u;
                        grammar_id = 90;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 2:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 90:
            // Grammar: ID=90; read/write bits=3; START (RemainingTimeToMaximumSOC), START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToMaximumSOC, unsignedInt (unsignedLong)); next=91
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DisplayParametersType->RemainingTimeToMaximumSOC);
                    if (error == 0)
                    {
                        DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 1u;
                        grammar_id = 91;
                    }
                    break;
                case 1:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 91:
            // Grammar: ID=91; read/write bits=3; START (ChargingComplete), START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ChargingComplete, boolean (boolean)); next=92
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
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 92:
            // Grammar: ID=92; read/write bits=2; START (BatteryEnergyCapacity), START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BatteryEnergyCapacity, RationalNumberType (RationalNumberType)); next=93
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DisplayParametersType->BatteryEnergyCapacity);
                    if (error == 0)
                    {
                        DisplayParametersType->BatteryEnergyCapacity_isUsed = 1u;
                        grammar_id = 93;
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
        case 93:
            // Grammar: ID=93; read/write bits=2; START (InletHot), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}DC_CPDResEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMinimumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1);
static int decode_iso20_dc_DC_CPDResEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_DC_CPDResEnergyTransferModeType* DC_CPDResEnergyTransferModeType) {
    int grammar_id = 94;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_CPDResEnergyTransferModeType(DC_CPDResEnergyTransferModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 94:
            // Grammar: ID=94; read/write bits=1; START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=95
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 95;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 95:
            // Grammar: ID=95; read/write bits=1; START (EVSEMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=96
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 96;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 96:
            // Grammar: ID=96; read/write bits=1; START (EVSEMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=97
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMaximumChargeCurrent);
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
            // Grammar: ID=97; read/write bits=1; START (EVSEMinimumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargeCurrent, RationalNumberType (RationalNumberType)); next=98
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMinimumChargeCurrent);
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
            // Grammar: ID=98; read/write bits=1; START (EVSEMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=99
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 99;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=1; START (EVSEMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=100
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 100;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 100:
            // Grammar: ID=100; read/write bits=2; START (EVSEPowerRampLimitation), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPowerRampLimitation, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation);
                    if (error == 0)
                    {
                        DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 1u;
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
                switch(eventCode)
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
static int decode_iso20_dc_EVSEStatusType(exi_bitstream_t* stream, struct iso20_dc_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 101;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_EVSEStatusType(EVSEStatusType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 101:
            // Grammar: ID=101; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedShort (unsignedInt)); next=102
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 102;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 102:
            // Grammar: ID=102; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                                EVSEStatusType->EVSENotification = (iso20_dc_evseNotificationType)value;
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
                switch(eventCode)
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
static int decode_iso20_dc_MeterInfoType(exi_bitstream_t* stream, struct iso20_dc_MeterInfoType* MeterInfoType) {
    int grammar_id = 103;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_MeterInfoType(MeterInfoType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 103:
            // Grammar: ID=103; read/write bits=1; START (MeterID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MeterID, meterIDType (string)); next=104
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
                                    error = exi_basetypes_decoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso20_dc_MeterID_CHARACTER_SIZE);
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
                                grammar_id = 104;
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
        case 104:
            // Grammar: ID=104; read/write bits=1; START (ChargedEnergyReadingWh)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ChargedEnergyReadingWh, unsignedLong (nonNegativeInteger)); next=105
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->ChargedEnergyReadingWh);
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
            // Grammar: ID=105; read/write bits=3; START (BPT_DischargedEnergyReadingWh), START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_DischargedEnergyReadingWh, unsignedLong (nonNegativeInteger)); next=106
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_DischargedEnergyReadingWh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 1u;
                        grammar_id = 106;
                    }
                    break;
                case 1:
                    // Event: START (CapacitiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=107
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->CapacitiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 107;
                    }
                    break;
                case 2:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=108
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 108;
                    }
                    break;
                case 3:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=109
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_dc_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 109;
                    }
                    break;
                case 4:
                    // Event: START (MeterStatus, short (int)); next=110
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 110;
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
        case 106:
            // Grammar: ID=106; read/write bits=3; START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (CapacitiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=107
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->CapacitiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 107;
                    }
                    break;
                case 1:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=108
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 108;
                    }
                    break;
                case 2:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=109
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_dc_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 109;
                    }
                    break;
                case 3:
                    // Event: START (MeterStatus, short (int)); next=110
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 110;
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
        case 107:
            // Grammar: ID=107; read/write bits=3; START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_InductiveEnergyReadingVARh, unsignedLong (nonNegativeInteger)); next=108
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->BPT_InductiveEnergyReadingVARh);
                    if (error == 0)
                    {
                        MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 1u;
                        grammar_id = 108;
                    }
                    break;
                case 1:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=109
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_dc_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 109;
                    }
                    break;
                case 2:
                    // Event: START (MeterStatus, short (int)); next=110
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 110;
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
        case 108:
            // Grammar: ID=108; read/write bits=3; START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MeterSignature, meterSignatureType (base64Binary)); next=109
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->MeterSignature.bytesLen, &MeterInfoType->MeterSignature.bytes[0], iso20_dc_meterSignatureType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->MeterSignature_isUsed = 1u;
                        grammar_id = 109;
                    }
                    break;
                case 1:
                    // Event: START (MeterStatus, short (int)); next=110
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 110;
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
        case 109:
            // Grammar: ID=109; read/write bits=2; START (MeterStatus), START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MeterStatus, short (int)); next=110
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 110;
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
        case 110:
            // Grammar: ID=110; read/write bits=2; START (MeterTimestamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLReqControlModeType; base type=Scheduled_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVTargetCurrent, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMaximumChargeCurrent, RationalNumberType (0, 1); EVMaximumVoltage, RationalNumberType (0, 1); EVMinimumVoltage, RationalNumberType (0, 1);
static int decode_iso20_dc_Scheduled_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_Scheduled_DC_CLReqControlModeType* Scheduled_DC_CLReqControlModeType) {
    int grammar_id = 111;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_Scheduled_DC_CLReqControlModeType(Scheduled_DC_CLReqControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 111:
            // Grammar: ID=111; read/write bits=3; START (EVTargetEnergyRequest), START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=112
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 1u;
                        grammar_id = 112;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=113
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 1u;
                        grammar_id = 113;
                    }
                    break;
                case 2:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=114
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 114;
                    }
                    break;
                case 3:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=115
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 115;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=2; START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=113
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 1u;
                        grammar_id = 113;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=114
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 114;
                    }
                    break;
                case 2:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=115
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 115;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 113:
            // Grammar: ID=113; read/write bits=2; START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=114
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 114;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=115
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 115;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=115
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 115;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 115:
            // Grammar: ID=115; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, RationalNumberType (RationalNumberType)); next=116
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVTargetVoltage);
                    if (error == 0)
                    {
                        grammar_id = 116;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=3; START (EVMaximumChargePower), START (EVMinimumChargePower), START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=117
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumChargePower_isUsed = 1u;
                        grammar_id = 117;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=118
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 1u;
                        grammar_id = 118;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 3:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=120
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 120;
                    }
                    break;
                case 4:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
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
        case 117:
            // Grammar: ID=117; read/write bits=3; START (EVMinimumChargePower), START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=118
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 1u;
                        grammar_id = 118;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=120
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 120;
                    }
                    break;
                case 3:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
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
        case 118:
            // Grammar: ID=118; read/write bits=3; START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=119
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 119;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=120
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 120;
                    }
                    break;
                case 2:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
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
        case 119:
            // Grammar: ID=119; read/write bits=2; START (EVMaximumVoltage), START (EVMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=120
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 120;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
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
        case 120:
            // Grammar: ID=120; read/write bits=2; START (EVMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLReqControlModeType; base type=Dynamic_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_Dynamic_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_Dynamic_DC_CLReqControlModeType* Dynamic_DC_CLReqControlModeType) {
    int grammar_id = 121;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_Dynamic_DC_CLReqControlModeType(Dynamic_DC_CLReqControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 121:
            // Grammar: ID=121; read/write bits=2; START (DepartureTime), START (EVTargetEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=122
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &Dynamic_DC_CLReqControlModeType->DepartureTime);
                    if (error == 0)
                    {
                        Dynamic_DC_CLReqControlModeType->DepartureTime_isUsed = 1u;
                        grammar_id = 122;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=123
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVTargetEnergyRequest);
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
        case 122:
            // Grammar: ID=122; read/write bits=1; START (EVTargetEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=123
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVTargetEnergyRequest);
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
            // Grammar: ID=123; read/write bits=1; START (EVMaximumEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=124
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 124;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 124:
            // Grammar: ID=124; read/write bits=1; START (EVMinimumEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=125
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 125;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 125:
            // Grammar: ID=125; read/write bits=1; START (EVMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=126
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 126;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 126:
            // Grammar: ID=126; read/write bits=1; START (EVMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=127
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 127;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 127:
            // Grammar: ID=127; read/write bits=1; START (EVMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=128
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMaximumChargeCurrent);
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
            // Grammar: ID=128; read/write bits=1; START (EVMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=129
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 129;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=1; START (EVMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLReqControlModeType->EVMinimumVoltage);
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
                switch(eventCode)
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
static int decode_iso20_dc_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_CLReqControlModeType* CLReqControlModeType) {
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
static int decode_iso20_dc_ReceiptType(exi_bitstream_t* stream, struct iso20_dc_ReceiptType* ReceiptType) {
    int grammar_id = 130;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_ReceiptType(ReceiptType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 130:
            // Grammar: ID=130; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TimeAnchor, unsignedLong (nonNegativeInteger)); next=131
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &ReceiptType->TimeAnchor);
                    if (error == 0)
                    {
                        grammar_id = 131;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=3; START (EnergyCosts), START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EnergyCosts, DetailedCostType (DetailedCostType)); next=141
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->EnergyCosts);
                    if (error == 0)
                    {
                        ReceiptType->EnergyCosts_isUsed = 1u;
                        grammar_id = 141;
                    }
                    break;
                case 1:
                    // Event: START (OccupancyCosts, DetailedCostType (DetailedCostType)); next=151
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == 0)
                    {
                        ReceiptType->OccupancyCosts_isUsed = 1u;
                        grammar_id = 151;
                    }
                    break;
                case 2:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=161
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 161;
                    }
                    break;
                case 3:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=171
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 171;
                    }
                    break;
                case 4:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=132
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 132;
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
        case 132:
            // Grammar: ID=132; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=133
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 133;
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
        case 133:
            // Grammar: ID=133; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=134
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 134;
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
        case 134:
            // Grammar: ID=134; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=135
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 135;
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
        case 135:
            // Grammar: ID=135; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=136
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 136;
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
        case 136:
            // Grammar: ID=136; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=137
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 137;
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
        case 137:
            // Grammar: ID=137; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=138
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 138;
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
        case 138:
            // Grammar: ID=138; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=139
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 139;
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
        case 139:
            // Grammar: ID=139; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=140
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 140;
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
        case 140:
            // Grammar: ID=140; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=2
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
        case 141:
            // Grammar: ID=141; read/write bits=3; START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (OccupancyCosts, DetailedCostType (DetailedCostType)); next=151
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == 0)
                    {
                        ReceiptType->OccupancyCosts_isUsed = 1u;
                        grammar_id = 151;
                    }
                    break;
                case 1:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=161
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 161;
                    }
                    break;
                case 2:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=171
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 171;
                    }
                    break;
                case 3:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=142
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 142;
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
        case 142:
            // Grammar: ID=142; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=143
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 143;
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
            // Grammar: ID=143; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=144
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 144;
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
        case 144:
            // Grammar: ID=144; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=145
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 145;
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
            // Grammar: ID=145; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=146
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 146;
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
        case 146:
            // Grammar: ID=146; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=147
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 147;
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
            // Grammar: ID=147; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=148
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 148;
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
        case 148:
            // Grammar: ID=148; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=149
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 149;
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
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=150
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
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
            // Grammar: ID=150; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=2
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
        case 151:
            // Grammar: ID=151; read/write bits=3; START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AdditionalServicesCosts, DetailedCostType (DetailedCostType)); next=161
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == 0)
                    {
                        ReceiptType->AdditionalServicesCosts_isUsed = 1u;
                        grammar_id = 161;
                    }
                    break;
                case 1:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=171
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 171;
                    }
                    break;
                case 2:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=152
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 152;
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
        case 152:
            // Grammar: ID=152; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=153
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 153;
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
        case 153:
            // Grammar: ID=153; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=154
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 154;
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
        case 154:
            // Grammar: ID=154; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=155
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 155;
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
        case 155:
            // Grammar: ID=155; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=156
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 156;
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
        case 156:
            // Grammar: ID=156; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=157
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 157;
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
        case 157:
            // Grammar: ID=157; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=158
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 158;
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
        case 158:
            // Grammar: ID=158; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=159
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 159;
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
        case 159:
            // Grammar: ID=159; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=160
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 160;
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
        case 160:
            // Grammar: ID=160; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=2
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
        case 161:
            // Grammar: ID=161; read/write bits=2; START (OverstayCosts), START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (OverstayCosts, DetailedCostType (DetailedCostType)); next=171
                    // decode: element
                    error = decode_iso20_dc_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == 0)
                    {
                        ReceiptType->OverstayCosts_isUsed = 1u;
                        grammar_id = 171;
                    }
                    break;
                case 1:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=162
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 162;
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
        case 162:
            // Grammar: ID=162; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=163
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 163;
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
        case 163:
            // Grammar: ID=163; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=164
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 164;
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
            // Grammar: ID=164; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=165
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 165;
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
        case 165:
            // Grammar: ID=165; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=166
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 166;
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
        case 166:
            // Grammar: ID=166; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=167
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 167;
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
        case 167:
            // Grammar: ID=167; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=168
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 168;
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
        case 168:
            // Grammar: ID=168; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=169
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 169;
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
        case 169:
            // Grammar: ID=169; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=170
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 170;
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
        case 170:
            // Grammar: ID=170; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=2
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
        case 171:
            // Grammar: ID=171; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=172
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 172;
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
        case 172:
            // Grammar: ID=172; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=173
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 173;
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
            // Grammar: ID=173; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=174
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 174;
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
        case 174:
            // Grammar: ID=174; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=175
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 175;
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
        case 175:
            // Grammar: ID=175; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=176
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 176;
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
        case 176:
            // Grammar: ID=176; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=177
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 177;
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
        case 177:
            // Grammar: ID=177; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=178
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 178;
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
        case 178:
            // Grammar: ID=178; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=179
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 179;
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
            // Grammar: ID=179; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=180
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 180;
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
        case 180:
            // Grammar: ID=180; read/write bits=2; START (TaxCosts), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TaxCosts, DetailedTaxType (DetailedTaxType)); next=2
                    // decode: element array
                    if (ReceiptType->TaxCosts.arrayLen < iso20_dc_DetailedTaxType_10_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[ReceiptType->TaxCosts.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLResControlModeType; base type=Scheduled_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (0, 1); EVSEMaximumChargeCurrent, RationalNumberType (0, 1); EVSEMaximumVoltage, RationalNumberType (0, 1);
static int decode_iso20_dc_Scheduled_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_Scheduled_DC_CLResControlModeType* Scheduled_DC_CLResControlModeType) {
    int grammar_id = 181;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_Scheduled_DC_CLResControlModeType(Scheduled_DC_CLResControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 181:
            // Grammar: ID=181; read/write bits=3; START (EVSEMaximumChargePower), START (EVSEMinimumChargePower), START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=182
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower_isUsed = 1u;
                        grammar_id = 182;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=183
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 1u;
                        grammar_id = 183;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=184
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 184;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
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
        case 182:
            // Grammar: ID=182; read/write bits=3; START (EVSEMinimumChargePower), START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=183
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 1u;
                        grammar_id = 183;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=184
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 184;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
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
        case 183:
            // Grammar: ID=183; read/write bits=2; START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=184
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 184;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
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
        case 184:
            // Grammar: ID=184; read/write bits=2; START (EVSEMaximumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLResControlModeType; base type=Dynamic_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_Dynamic_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_Dynamic_DC_CLResControlModeType* Dynamic_DC_CLResControlModeType) {
    int grammar_id = 185;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_Dynamic_DC_CLResControlModeType(Dynamic_DC_CLResControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 185:
            // Grammar: ID=185; read/write bits=3; START (DepartureTime), START (MinimumSOC), START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=186
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &Dynamic_DC_CLResControlModeType->DepartureTime);
                    if (error == 0)
                    {
                        Dynamic_DC_CLResControlModeType->DepartureTime_isUsed = 1u;
                        grammar_id = 186;
                    }
                    break;
                case 1:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=187
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
                                Dynamic_DC_CLResControlModeType->MinimumSOC = (int8_t)value;
                                Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 1u;
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
                                grammar_id = 187;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (TargetSOC, percentValueType (byte)); next=188
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
                                Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 188;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=189
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=190
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 190;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=3; START (MinimumSOC), START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=187
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
                                Dynamic_DC_CLResControlModeType->MinimumSOC = (int8_t)value;
                                Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 1u;
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
                                grammar_id = 187;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (TargetSOC, percentValueType (byte)); next=188
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
                                Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 188;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=189
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=190
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 190;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 187:
            // Grammar: ID=187; read/write bits=2; START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=188
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
                                Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 188;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=189
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=190
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 190;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=2; START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=189
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=190
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 190;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=1; START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=190
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 190;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=1; START (EVSEMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=191
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 191;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=1; START (EVSEMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=192
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 192;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=1; START (EVSEMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &Dynamic_DC_CLResControlModeType->EVSEMaximumVoltage);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int decode_iso20_dc_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_CLResControlModeType* CLResControlModeType) {
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CableCheckReq; type={urn:iso:std:iso:15118:-20:DC}DC_CableCheckReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1);
static int decode_iso20_dc_DC_CableCheckReqType(exi_bitstream_t* stream, struct iso20_dc_DC_CableCheckReqType* DC_CableCheckReqType) {
    int grammar_id = 193;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_CableCheckReqType(DC_CableCheckReqType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 193:
            // Grammar: ID=193; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=2
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_CableCheckReqType->Header);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CableCheckRes; type={urn:iso:std:iso:15118:-20:DC}DC_CableCheckResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1);
static int decode_iso20_dc_DC_CableCheckResType(exi_bitstream_t* stream, struct iso20_dc_DC_CableCheckResType* DC_CableCheckResType) {
    int grammar_id = 194;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_CableCheckResType(DC_CableCheckResType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 194:
            // Grammar: ID=194; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=195
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_CableCheckResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 195;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 195:
            // Grammar: ID=195; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=196
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
                                DC_CableCheckResType->ResponseCode = (iso20_dc_responseCodeType)value;
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
            // Grammar: ID=196; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=2
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
                                DC_CableCheckResType->EVSEProcessing = (iso20_dc_processingType)value;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_PreChargeReq; type={urn:iso:std:iso:15118:-20:DC}DC_PreChargeReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVPresentVoltage, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_DC_PreChargeReqType(exi_bitstream_t* stream, struct iso20_dc_DC_PreChargeReqType* DC_PreChargeReqType) {
    int grammar_id = 197;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_PreChargeReqType(DC_PreChargeReqType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 197:
            // Grammar: ID=197; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=198
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_PreChargeReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 198;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=199
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
                                DC_PreChargeReqType->EVProcessing = (iso20_dc_processingType)value;
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
                                grammar_id = 199;
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
        case 199:
            // Grammar: ID=199; read/write bits=1; START (EVPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVPresentVoltage, RationalNumberType (RationalNumberType)); next=200
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_PreChargeReqType->EVPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 200;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_PreChargeReqType->EVTargetVoltage);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_PreChargeRes; type={urn:iso:std:iso:15118:-20:DC}DC_PreChargeResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_DC_PreChargeResType(exi_bitstream_t* stream, struct iso20_dc_DC_PreChargeResType* DC_PreChargeResType) {
    int grammar_id = 201;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_PreChargeResType(DC_PreChargeResType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 201:
            // Grammar: ID=201; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=202
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_PreChargeResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 202;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 202:
            // Grammar: ID=202; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=203
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
                                DC_PreChargeResType->ResponseCode = (iso20_dc_responseCodeType)value;
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
                                grammar_id = 203;
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
        case 203:
            // Grammar: ID=203; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_PreChargeResType->EVSEPresentVoltage);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionReq; type={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1);
static int decode_iso20_dc_DC_WeldingDetectionReqType(exi_bitstream_t* stream, struct iso20_dc_DC_WeldingDetectionReqType* DC_WeldingDetectionReqType) {
    int grammar_id = 204;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_WeldingDetectionReqType(DC_WeldingDetectionReqType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 204:
            // Grammar: ID=204; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=205
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_WeldingDetectionReqType->Header);
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
            // Grammar: ID=205; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVProcessing, processingType (string)); next=2
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
                                DC_WeldingDetectionReqType->EVProcessing = (iso20_dc_processingType)value;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionRes; type={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_DC_WeldingDetectionResType(exi_bitstream_t* stream, struct iso20_dc_DC_WeldingDetectionResType* DC_WeldingDetectionResType) {
    int grammar_id = 206;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_WeldingDetectionResType(DC_WeldingDetectionResType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 206:
            // Grammar: ID=206; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=207
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_WeldingDetectionResType->Header);
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
            // Grammar: ID=207; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=208
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
                                DC_WeldingDetectionResType->ResponseCode = (iso20_dc_responseCodeType)value;
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
            // Grammar: ID=208; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_WeldingDetectionResType->EVSEPresentVoltage);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDReqEnergyTransferModeType; base type=DC_CPDReqEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMinimumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); TargetSOC, percentValueType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMaximumDischargeCurrent, RationalNumberType (1, 1); EVMinimumDischargeCurrent, RationalNumberType (1, 1);
static int decode_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType* BPT_DC_CPDReqEnergyTransferModeType) {
    int grammar_id = 209;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(BPT_DC_CPDReqEnergyTransferModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 209:
            // Grammar: ID=209; read/write bits=1; START (EVMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=210
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 210;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 210:
            // Grammar: ID=210; read/write bits=1; START (EVMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=211
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMinimumChargePower);
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
            // Grammar: ID=211; read/write bits=1; START (EVMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=212
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 212;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=1; START (EVMinimumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargeCurrent, RationalNumberType (RationalNumberType)); next=213
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMinimumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 213;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=1; START (EVMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=214
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumVoltage);
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
            // Grammar: ID=214; read/write bits=1; START (EVMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=215
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMinimumVoltage);
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
            // Grammar: ID=215; read/write bits=2; START (TargetSOC), START (EVMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=216
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
                                BPT_DC_CPDReqEnergyTransferModeType->TargetSOC = (int8_t)value;
                                BPT_DC_CPDReqEnergyTransferModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 216;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=217
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumDischargePower);
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
        case 216:
            // Grammar: ID=216; read/write bits=1; START (EVMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=217
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumDischargePower);
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
            // Grammar: ID=217; read/write bits=1; START (EVMinimumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=218
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMinimumDischargePower);
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
            // Grammar: ID=218; read/write bits=1; START (EVMaximumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=219
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMaximumDischargeCurrent);
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
            // Grammar: ID=219; read/write bits=1; START (EVMinimumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDReqEnergyTransferModeType->EVMinimumDischargeCurrent);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); BPT_DC_CPDReqEnergyTransferMode, BPT_DC_CPDReqEnergyTransferModeType (0, 1); DC_CPDReqEnergyTransferMode, DC_CPDReqEnergyTransferModeType (0, 1);
static int decode_iso20_dc_DC_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeParameterDiscoveryReqType* DC_ChargeParameterDiscoveryReqType) {
    int grammar_id = 220;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_ChargeParameterDiscoveryReqType(DC_ChargeParameterDiscoveryReqType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 220:
            // Grammar: ID=220; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=221
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_ChargeParameterDiscoveryReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 221;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 221:
            // Grammar: ID=221; read/write bits=2; START (BPT_DC_CPDReqEnergyTransferMode), START (DC_CPDReqEnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_DC_CPDReqEnergyTransferMode, BPT_DC_CPDReqEnergyTransferModeType (DC_CPDReqEnergyTransferModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(stream, &DC_ChargeParameterDiscoveryReqType->BPT_DC_CPDReqEnergyTransferMode);
                    if (error == 0)
                    {
                        DC_ChargeParameterDiscoveryReqType->BPT_DC_CPDReqEnergyTransferMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (DC_CPDReqEnergyTransferMode, DC_CPDReqEnergyTransferModeType (DC_CPDReqEnergyTransferModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_DC_CPDReqEnergyTransferModeType(stream, &DC_ChargeParameterDiscoveryReqType->DC_CPDReqEnergyTransferMode);
                    if (error == 0)
                    {
                        DC_ChargeParameterDiscoveryReqType->DC_CPDReqEnergyTransferMode_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDResEnergyTransferModeType; base type=DC_CPDResEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMinimumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargeCurrent, RationalNumberType (1, 1); EVSEMinimumDischargeCurrent, RationalNumberType (1, 1);
static int decode_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType* BPT_DC_CPDResEnergyTransferModeType) {
    int grammar_id = 222;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(BPT_DC_CPDResEnergyTransferModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 222:
            // Grammar: ID=222; read/write bits=1; START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=223
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 223;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 223:
            // Grammar: ID=223; read/write bits=1; START (EVSEMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=224
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 224;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 224:
            // Grammar: ID=224; read/write bits=1; START (EVSEMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=225
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 225;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=1; START (EVSEMinimumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargeCurrent, RationalNumberType (RationalNumberType)); next=226
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMinimumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 226;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=1; START (EVSEMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=227
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 227;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 227:
            // Grammar: ID=227; read/write bits=1; START (EVSEMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=228
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMinimumVoltage);
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
            // Grammar: ID=228; read/write bits=2; START (EVSEPowerRampLimitation), START (EVSEMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPowerRampLimitation, RationalNumberType (RationalNumberType)); next=229
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation);
                    if (error == 0)
                    {
                        BPT_DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 1u;
                        grammar_id = 229;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=230
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 230;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=1; START (EVSEMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=230
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 230;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 230:
            // Grammar: ID=230; read/write bits=1; START (EVSEMinimumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=231
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 231;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=1; START (EVSEMaximumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=232
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 232;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=1; START (EVSEMinimumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_DC_CPDResEnergyTransferModeType->EVSEMinimumDischargeCurrent);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); BPT_DC_CPDResEnergyTransferMode, BPT_DC_CPDResEnergyTransferModeType (0, 1); DC_CPDResEnergyTransferMode, DC_CPDResEnergyTransferModeType (0, 1);
static int decode_iso20_dc_DC_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeParameterDiscoveryResType* DC_ChargeParameterDiscoveryResType) {
    int grammar_id = 233;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_ChargeParameterDiscoveryResType(DC_ChargeParameterDiscoveryResType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 233:
            // Grammar: ID=233; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=234
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_ChargeParameterDiscoveryResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 234;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=235
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
                                DC_ChargeParameterDiscoveryResType->ResponseCode = (iso20_dc_responseCodeType)value;
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
                                grammar_id = 235;
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
        case 235:
            // Grammar: ID=235; read/write bits=2; START (BPT_DC_CPDResEnergyTransferMode), START (DC_CPDResEnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_DC_CPDResEnergyTransferMode, BPT_DC_CPDResEnergyTransferModeType (DC_CPDResEnergyTransferModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(stream, &DC_ChargeParameterDiscoveryResType->BPT_DC_CPDResEnergyTransferMode);
                    if (error == 0)
                    {
                        DC_ChargeParameterDiscoveryResType->BPT_DC_CPDResEnergyTransferMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (DC_CPDResEnergyTransferMode, DC_CPDResEnergyTransferModeType (DC_CPDResEnergyTransferModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_DC_CPDResEnergyTransferModeType(stream, &DC_ChargeParameterDiscoveryResType->DC_CPDResEnergyTransferMode);
                    if (error == 0)
                    {
                        DC_ChargeParameterDiscoveryResType->DC_CPDResEnergyTransferMode_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLReqControlModeType; base type=Scheduled_DC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVTargetCurrent, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMaximumChargeCurrent, RationalNumberType (0, 1); EVMaximumVoltage, RationalNumberType (0, 1); EVMinimumVoltage, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (0, 1); EVMaximumDischargeCurrent, RationalNumberType (0, 1);
static int decode_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType* BPT_Scheduled_DC_CLReqControlModeType) {
    int grammar_id = 236;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(BPT_Scheduled_DC_CLReqControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 236:
            // Grammar: ID=236; read/write bits=3; START (EVTargetEnergyRequest), START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=237
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 1u;
                        grammar_id = 237;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=238
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 1u;
                        grammar_id = 238;
                    }
                    break;
                case 2:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=239
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 239;
                    }
                    break;
                case 3:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=240
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
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
        case 237:
            // Grammar: ID=237; read/write bits=2; START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=238
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 1u;
                        grammar_id = 238;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=239
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 239;
                    }
                    break;
                case 2:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=240
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
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
        case 238:
            // Grammar: ID=238; read/write bits=2; START (EVMinimumEnergyRequest), START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=239
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 1u;
                        grammar_id = 239;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=240
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
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
        case 239:
            // Grammar: ID=239; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, RationalNumberType (RationalNumberType)); next=240
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetCurrent);
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
            // Grammar: ID=240; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, RationalNumberType (RationalNumberType)); next=241
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVTargetVoltage);
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
            // Grammar: ID=241; read/write bits=4; START (EVMaximumChargePower), START (EVMinimumChargePower), START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=242
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargePower_isUsed = 1u;
                        grammar_id = 242;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=243
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 1u;
                        grammar_id = 243;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=244
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                case 3:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=245
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 245;
                    }
                    break;
                case 4:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=246
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 5:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 6:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 7:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 2;
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
        case 242:
            // Grammar: ID=242; read/write bits=4; START (EVMinimumChargePower), START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=243
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 1u;
                        grammar_id = 243;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=244
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=245
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 245;
                    }
                    break;
                case 3:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=246
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 4:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 5:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 6:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 2;
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
        case 243:
            // Grammar: ID=243; read/write bits=3; START (EVMaximumChargeCurrent), START (EVMaximumVoltage), START (EVMinimumVoltage), START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=244
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=245
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 245;
                    }
                    break;
                case 2:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=246
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 3:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 4:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 5:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
        case 244:
            // Grammar: ID=244; read/write bits=3; START (EVMaximumVoltage), START (EVMinimumVoltage), START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=245
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 1u;
                        grammar_id = 245;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=246
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 3:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 4:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
        case 245:
            // Grammar: ID=245; read/write bits=3; START (EVMinimumVoltage), START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=246
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 1u;
                        grammar_id = 246;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 2:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 3:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
        case 246:
            // Grammar: ID=246; read/write bits=3; START (EVMaximumDischargePower), START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=247
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
        case 247:
            // Grammar: ID=247; read/write bits=2; START (EVMinimumDischargePower), START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=248
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 1u;
                        grammar_id = 248;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
        case 248:
            // Grammar: ID=248; read/write bits=2; START (EVMaximumDischargeCurrent), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLResControlModeType; base type=Scheduled_DC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (0, 1); EVSEMaximumChargeCurrent, RationalNumberType (0, 1); EVSEMaximumVoltage, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (0, 1); EVSEMinimumDischargePower, RationalNumberType (0, 1); EVSEMaximumDischargeCurrent, RationalNumberType (0, 1); EVSEMinimumVoltage, RationalNumberType (0, 1);
static int decode_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType* BPT_Scheduled_DC_CLResControlModeType) {
    int grammar_id = 249;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(BPT_Scheduled_DC_CLResControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 249:
            // Grammar: ID=249; read/write bits=4; START (EVSEMaximumChargePower), START (EVSEMinimumChargePower), START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), START (EVSEMaximumDischargePower), START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=250
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower_isUsed = 1u;
                        grammar_id = 250;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=251
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 1u;
                        grammar_id = 251;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=252
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 252;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=253
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 5:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 6:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 7:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
                        grammar_id = 2;
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
        case 250:
            // Grammar: ID=250; read/write bits=4; START (EVSEMinimumChargePower), START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), START (EVSEMaximumDischargePower), START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=251
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 1u;
                        grammar_id = 251;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=252
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 252;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=253
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 5:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 6:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
                        grammar_id = 2;
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
        case 251:
            // Grammar: ID=251; read/write bits=3; START (EVSEMaximumChargeCurrent), START (EVSEMaximumVoltage), START (EVSEMaximumDischargePower), START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=252
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 1u;
                        grammar_id = 252;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=253
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 5:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
        case 252:
            // Grammar: ID=252; read/write bits=3; START (EVSEMaximumVoltage), START (EVSEMaximumDischargePower), START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=253
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
        case 253:
            // Grammar: ID=253; read/write bits=3; START (EVSEMaximumDischargePower), START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=254
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
        case 254:
            // Grammar: ID=254; read/write bits=3; START (EVSEMinimumDischargePower), START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=255
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
        case 255:
            // Grammar: ID=255; read/write bits=2; START (EVSEMaximumDischargeCurrent), START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=256
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 1u;
                        grammar_id = 256;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
        case 256:
            // Grammar: ID=256; read/write bits=2; START (EVSEMinimumVoltage), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage);
                    if (error == 0)
                    {
                        BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLReqControlModeType; base type=Dynamic_DC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMaximumDischargeCurrent, RationalNumberType (1, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
static int decode_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType* BPT_Dynamic_DC_CLReqControlModeType) {
    int grammar_id = 257;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(BPT_Dynamic_DC_CLReqControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 257:
            // Grammar: ID=257; read/write bits=2; START (DepartureTime), START (EVTargetEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=258
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &BPT_Dynamic_DC_CLReqControlModeType->DepartureTime);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLReqControlModeType->DepartureTime_isUsed = 1u;
                        grammar_id = 258;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=259
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVTargetEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 259;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=1; START (EVTargetEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVTargetEnergyRequest, RationalNumberType (RationalNumberType)); next=259
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVTargetEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 259;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=1; START (EVMaximumEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType (RationalNumberType)); next=260
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 260;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=1; START (EVMinimumEnergyRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType (RationalNumberType)); next=261
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumEnergyRequest);
                    if (error == 0)
                    {
                        grammar_id = 261;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 261:
            // Grammar: ID=261; read/write bits=1; START (EVMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargePower, RationalNumberType (RationalNumberType)); next=262
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 262;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=1; START (EVMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumChargePower, RationalNumberType (RationalNumberType)); next=263
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 263;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 263:
            // Grammar: ID=263; read/write bits=1; START (EVMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=264
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 264;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 264:
            // Grammar: ID=264; read/write bits=1; START (EVMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltage, RationalNumberType (RationalNumberType)); next=265
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 265;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=1; START (EVMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumVoltage, RationalNumberType (RationalNumberType)); next=266
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 266;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (EVMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargePower, RationalNumberType (RationalNumberType)); next=267
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 267;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (EVMinimumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumDischargePower, RationalNumberType (RationalNumberType)); next=268
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 268;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 268:
            // Grammar: ID=268; read/write bits=1; START (EVMaximumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=269
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumDischargeCurrent);
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
            // Grammar: ID=269; read/write bits=2; START (EVMaximumV2XEnergyRequest), START (EVMinimumV2XEnergyRequest), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMaximumV2XEnergyRequest, RationalNumberType (RationalNumberType)); next=270
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMaximumV2XEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLReqControlModeType->EVMaximumV2XEnergyRequest_isUsed = 1u;
                        grammar_id = 270;
                    }
                    break;
                case 1:
                    // Event: START (EVMinimumV2XEnergyRequest, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumV2XEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLReqControlModeType->EVMinimumV2XEnergyRequest_isUsed = 1u;
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
        case 270:
            // Grammar: ID=270; read/write bits=2; START (EVMinimumV2XEnergyRequest), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVMinimumV2XEnergyRequest, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLReqControlModeType->EVMinimumV2XEnergyRequest);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLReqControlModeType->EVMinimumV2XEnergyRequest_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); EVPresentVoltage, RationalNumberType (1, 1); BPT_Dynamic_DC_CLReqControlMode, BPT_Dynamic_DC_CLReqControlModeType (0, 1); BPT_Scheduled_DC_CLReqControlMode, BPT_Scheduled_DC_CLReqControlModeType (0, 1); CLReqControlMode, CLReqControlModeType (0, 1); Dynamic_DC_CLReqControlMode, Dynamic_DC_CLReqControlModeType (0, 1); Scheduled_DC_CLReqControlMode, Scheduled_DC_CLReqControlModeType (0, 1);
static int decode_iso20_dc_DC_ChargeLoopReqType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeLoopReqType* DC_ChargeLoopReqType) {
    int grammar_id = 271;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_ChargeLoopReqType(DC_ChargeLoopReqType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 271:
            // Grammar: ID=271; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=272
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_ChargeLoopReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 272;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 272:
            // Grammar: ID=272; read/write bits=2; START (DisplayParameters), START (MeterInfoRequested)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DisplayParameters, DisplayParametersType (DisplayParametersType)); next=273
                    // decode: element
                    error = decode_iso20_dc_DisplayParametersType(stream, &DC_ChargeLoopReqType->DisplayParameters);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->DisplayParameters_isUsed = 1u;
                        grammar_id = 273;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfoRequested, boolean (boolean)); next=274
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
                                DC_ChargeLoopReqType->MeterInfoRequested = value;
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
                                grammar_id = 274;
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
        case 273:
            // Grammar: ID=273; read/write bits=1; START (MeterInfoRequested)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MeterInfoRequested, boolean (boolean)); next=274
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
                                DC_ChargeLoopReqType->MeterInfoRequested = value;
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
                                grammar_id = 274;
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
        case 274:
            // Grammar: ID=274; read/write bits=1; START (EVPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVPresentVoltage, RationalNumberType (RationalNumberType)); next=275
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopReqType->EVPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 275;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 275:
            // Grammar: ID=275; read/write bits=3; START (BPT_Dynamic_DC_CLReqControlMode), START (BPT_Scheduled_DC_CLReqControlMode), START (CLReqControlMode), START (Dynamic_DC_CLReqControlMode), START (Scheduled_DC_CLReqControlMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_Dynamic_DC_CLReqControlMode, BPT_Dynamic_DC_CLReqControlModeType (Dynamic_DC_CLReqControlModeType)); next=276
                    // decode: element
                    error = decode_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->BPT_Dynamic_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->BPT_Dynamic_DC_CLReqControlMode_isUsed = 1u;
                        grammar_id = 276;
                    }
                    break;
                case 1:
                    // Event: START (BPT_Scheduled_DC_CLReqControlMode, BPT_Scheduled_DC_CLReqControlModeType (Scheduled_DC_CLReqControlModeType)); next=276
                    // decode: element
                    error = decode_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->BPT_Scheduled_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->BPT_Scheduled_DC_CLReqControlMode_isUsed = 1u;
                        grammar_id = 276;
                    }
                    break;
                case 2:
                    // Abstract element or type: CLReqControlMode, CLReqControlModeType (CLReqControlModeType)
                    // decode: element
                    error = decode_iso20_dc_CLReqControlModeType(stream, &DC_ChargeLoopReqType->CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->CLReqControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (Dynamic_DC_CLReqControlMode, Dynamic_DC_CLReqControlModeType (Dynamic_CLReqControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Dynamic_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->Dynamic_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->Dynamic_DC_CLReqControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (Scheduled_DC_CLReqControlMode, Scheduled_DC_CLReqControlModeType (Scheduled_CLReqControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Scheduled_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->Scheduled_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->Scheduled_DC_CLReqControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 276:
            // Grammar: ID=276; read/write bits=2; START (CLReqControlMode), START (Dynamic_DC_CLReqControlMode), START (Scheduled_DC_CLReqControlMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Abstract element or type: CLReqControlMode, CLReqControlModeType (CLReqControlModeType)
                    // decode: element
                    error = decode_iso20_dc_CLReqControlModeType(stream, &DC_ChargeLoopReqType->CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->CLReqControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (Dynamic_DC_CLReqControlMode, Dynamic_DC_CLReqControlModeType (Dynamic_CLReqControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Dynamic_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->Dynamic_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->Dynamic_DC_CLReqControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (Scheduled_DC_CLReqControlMode, Scheduled_DC_CLReqControlModeType (Scheduled_CLReqControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Scheduled_DC_CLReqControlModeType(stream, &DC_ChargeLoopReqType->Scheduled_DC_CLReqControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopReqType->Scheduled_DC_CLReqControlMode_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLResControlModeType; base type=Dynamic_DC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargeCurrent, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1);
static int decode_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(exi_bitstream_t* stream, struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType* BPT_Dynamic_DC_CLResControlModeType) {
    int grammar_id = 277;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(BPT_Dynamic_DC_CLResControlModeType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 277:
            // Grammar: ID=277; read/write bits=3; START (DepartureTime), START (MinimumSOC), START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=278
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &BPT_Dynamic_DC_CLResControlModeType->DepartureTime);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLResControlModeType->DepartureTime_isUsed = 1u;
                        grammar_id = 278;
                    }
                    break;
                case 1:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=279
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
                                BPT_Dynamic_DC_CLResControlModeType->MinimumSOC = (int8_t)value;
                                BPT_Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 1u;
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
                                grammar_id = 279;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (TargetSOC, percentValueType (byte)); next=280
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
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 280;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=281
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 281;
                    }
                    break;
                case 4:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=282
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 282;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 278:
            // Grammar: ID=278; read/write bits=3; START (MinimumSOC), START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MinimumSOC, percentValueType (byte)); next=279
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
                                BPT_Dynamic_DC_CLResControlModeType->MinimumSOC = (int8_t)value;
                                BPT_Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 1u;
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
                                grammar_id = 279;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (TargetSOC, percentValueType (byte)); next=280
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
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 280;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=281
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 281;
                    }
                    break;
                case 3:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=282
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 282;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 279:
            // Grammar: ID=279; read/write bits=2; START (TargetSOC), START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (TargetSOC, percentValueType (byte)); next=280
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
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC = (int8_t)value;
                                BPT_Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 1u;
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
                                grammar_id = 280;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=281
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 281;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=282
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 282;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 280:
            // Grammar: ID=280; read/write bits=2; START (AckMaxDelay), START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AckMaxDelay, unsignedShort (unsignedInt)); next=281
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay);
                    if (error == 0)
                    {
                        BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 1u;
                        grammar_id = 281;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=282
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 282;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 281:
            // Grammar: ID=281; read/write bits=1; START (EVSEMaximumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargePower, RationalNumberType (RationalNumberType)); next=282
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 282;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 282:
            // Grammar: ID=282; read/write bits=1; START (EVSEMinimumChargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumChargePower, RationalNumberType (RationalNumberType)); next=283
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMinimumChargePower);
                    if (error == 0)
                    {
                        grammar_id = 283;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 283:
            // Grammar: ID=283; read/write bits=1; START (EVSEMaximumChargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumChargeCurrent, RationalNumberType (RationalNumberType)); next=284
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumChargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 284;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 284:
            // Grammar: ID=284; read/write bits=1; START (EVSEMaximumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltage, RationalNumberType (RationalNumberType)); next=285
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumVoltage);
                    if (error == 0)
                    {
                        grammar_id = 285;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 285:
            // Grammar: ID=285; read/write bits=1; START (EVSEMaximumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargePower, RationalNumberType (RationalNumberType)); next=286
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 286;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 286:
            // Grammar: ID=286; read/write bits=1; START (EVSEMinimumDischargePower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumDischargePower, RationalNumberType (RationalNumberType)); next=287
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMinimumDischargePower);
                    if (error == 0)
                    {
                        grammar_id = 287;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 287:
            // Grammar: ID=287; read/write bits=1; START (EVSEMaximumDischargeCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumDischargeCurrent, RationalNumberType (RationalNumberType)); next=288
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMaximumDischargeCurrent);
                    if (error == 0)
                    {
                        grammar_id = 288;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 288:
            // Grammar: ID=288; read/write bits=1; START (EVSEMinimumVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltage, RationalNumberType (RationalNumberType)); next=2
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &BPT_Dynamic_DC_CLResControlModeType->EVSEMinimumVoltage);
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
                switch(eventCode)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVSEPresentCurrent, RationalNumberType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); BPT_Dynamic_DC_CLResControlMode, BPT_Dynamic_DC_CLResControlModeType (0, 1); BPT_Scheduled_DC_CLResControlMode, BPT_Scheduled_DC_CLResControlModeType (0, 1); CLResControlMode, CLResControlModeType (0, 1); Dynamic_DC_CLResControlMode, Dynamic_DC_CLResControlModeType (0, 1); Scheduled_DC_CLResControlMode, Scheduled_DC_CLResControlModeType (0, 1);
static int decode_iso20_dc_DC_ChargeLoopResType(exi_bitstream_t* stream, struct iso20_dc_DC_ChargeLoopResType* DC_ChargeLoopResType) {
    int grammar_id = 289;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_DC_ChargeLoopResType(DC_ChargeLoopResType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 289:
            // Grammar: ID=289; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=290
                    // decode: element
                    error = decode_iso20_dc_MessageHeaderType(stream, &DC_ChargeLoopResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 290;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 290:
            // Grammar: ID=290; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=291
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
                                DC_ChargeLoopResType->ResponseCode = (iso20_dc_responseCodeType)value;
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
                                grammar_id = 291;
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
        case 291:
            // Grammar: ID=291; read/write bits=3; START (EVSEStatus), START (MeterInfo), START (Receipt), START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEStatus, EVSEStatusType (EVSEStatusType)); next=292
                    // decode: element
                    error = decode_iso20_dc_EVSEStatusType(stream, &DC_ChargeLoopResType->EVSEStatus);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->EVSEStatus_isUsed = 1u;
                        grammar_id = 292;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=293
                    // decode: element
                    error = decode_iso20_dc_MeterInfoType(stream, &DC_ChargeLoopResType->MeterInfo);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->MeterInfo_isUsed = 1u;
                        grammar_id = 293;
                    }
                    break;
                case 2:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=294
                    // decode: element
                    error = decode_iso20_dc_ReceiptType(stream, &DC_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 294;
                    }
                    break;
                case 3:
                    // Event: START (EVSEPresentCurrent, RationalNumberType (RationalNumberType)); next=295
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 295;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 292:
            // Grammar: ID=292; read/write bits=2; START (MeterInfo), START (Receipt), START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=293
                    // decode: element
                    error = decode_iso20_dc_MeterInfoType(stream, &DC_ChargeLoopResType->MeterInfo);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->MeterInfo_isUsed = 1u;
                        grammar_id = 293;
                    }
                    break;
                case 1:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=294
                    // decode: element
                    error = decode_iso20_dc_ReceiptType(stream, &DC_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 294;
                    }
                    break;
                case 2:
                    // Event: START (EVSEPresentCurrent, RationalNumberType (RationalNumberType)); next=295
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 295;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 293:
            // Grammar: ID=293; read/write bits=2; START (Receipt), START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Receipt, ReceiptType (ReceiptType)); next=294
                    // decode: element
                    error = decode_iso20_dc_ReceiptType(stream, &DC_ChargeLoopResType->Receipt);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Receipt_isUsed = 1u;
                        grammar_id = 294;
                    }
                    break;
                case 1:
                    // Event: START (EVSEPresentCurrent, RationalNumberType (RationalNumberType)); next=295
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 295;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 294:
            // Grammar: ID=294; read/write bits=1; START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentCurrent, RationalNumberType (RationalNumberType)); next=295
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 295;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 295:
            // Grammar: ID=295; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, RationalNumberType (RationalNumberType)); next=296
                    // decode: element
                    error = decode_iso20_dc_RationalNumberType(stream, &DC_ChargeLoopResType->EVSEPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 296;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 296:
            // Grammar: ID=296; read/write bits=1; START (EVSEPowerLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEPowerLimitAchieved, boolean (boolean)); next=297
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
                                DC_ChargeLoopResType->EVSEPowerLimitAchieved = value;
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
                                grammar_id = 297;
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
        case 297:
            // Grammar: ID=297; read/write bits=1; START (EVSECurrentLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSECurrentLimitAchieved, boolean (boolean)); next=298
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
                                DC_ChargeLoopResType->EVSECurrentLimitAchieved = value;
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
                                grammar_id = 298;
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
        case 298:
            // Grammar: ID=298; read/write bits=1; START (EVSEVoltageLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (EVSEVoltageLimitAchieved, boolean (boolean)); next=299
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
                                DC_ChargeLoopResType->EVSEVoltageLimitAchieved = value;
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
                                grammar_id = 299;
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
        case 299:
            // Grammar: ID=299; read/write bits=3; START (BPT_Dynamic_DC_CLResControlMode), START (BPT_Scheduled_DC_CLResControlMode), START (CLResControlMode), START (Dynamic_DC_CLResControlMode), START (Scheduled_DC_CLResControlMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (BPT_Dynamic_DC_CLResControlMode, BPT_Dynamic_DC_CLResControlModeType (Dynamic_DC_CLResControlModeType)); next=300
                    // decode: element
                    error = decode_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->BPT_Dynamic_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->BPT_Dynamic_DC_CLResControlMode_isUsed = 1u;
                        grammar_id = 300;
                    }
                    break;
                case 1:
                    // Event: START (BPT_Scheduled_DC_CLResControlMode, BPT_Scheduled_DC_CLResControlModeType (Scheduled_DC_CLResControlModeType)); next=300
                    // decode: element
                    error = decode_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->BPT_Scheduled_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->BPT_Scheduled_DC_CLResControlMode_isUsed = 1u;
                        grammar_id = 300;
                    }
                    break;
                case 2:
                    // Abstract element or type: CLResControlMode, CLResControlModeType (CLResControlModeType)
                    // decode: element
                    error = decode_iso20_dc_CLResControlModeType(stream, &DC_ChargeLoopResType->CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->CLResControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (Dynamic_DC_CLResControlMode, Dynamic_DC_CLResControlModeType (Dynamic_CLResControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Dynamic_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->Dynamic_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Dynamic_DC_CLResControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (Scheduled_DC_CLResControlMode, Scheduled_DC_CLResControlModeType (Scheduled_CLResControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Scheduled_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->Scheduled_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Scheduled_DC_CLResControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 300:
            // Grammar: ID=300; read/write bits=2; START (CLResControlMode), START (Dynamic_DC_CLResControlMode), START (Scheduled_DC_CLResControlMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Abstract element or type: CLResControlMode, CLResControlModeType (CLResControlModeType)
                    // decode: element
                    error = decode_iso20_dc_CLResControlModeType(stream, &DC_ChargeLoopResType->CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->CLResControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (Dynamic_DC_CLResControlMode, Dynamic_DC_CLResControlModeType (Dynamic_CLResControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Dynamic_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->Dynamic_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Dynamic_DC_CLResControlMode_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (Scheduled_DC_CLResControlMode, Scheduled_DC_CLResControlModeType (Scheduled_CLResControlModeType)); next=2
                    // decode: element
                    error = decode_iso20_dc_Scheduled_DC_CLResControlModeType(stream, &DC_ChargeLoopResType->Scheduled_DC_CLResControlMode);
                    if (error == 0)
                    {
                        DC_ChargeLoopResType->Scheduled_DC_CLResControlMode_isUsed = 1u;
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
                switch(eventCode)
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4);
static int decode_iso20_dc_ManifestType(exi_bitstream_t* stream, struct iso20_dc_ManifestType* ManifestType) {
    int grammar_id = 301;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_ManifestType(ManifestType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 301:
            // Grammar: ID=301; read/write bits=2; START (Id), START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=306
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ManifestType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ManifestType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ManifestType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ManifestType->Id.charactersLen, ManifestType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ManifestType->Id_isUsed = 1u;
                    grammar_id = 306;
                    break;
                case 1:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=302
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 302;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 302:
            // Grammar: ID=302; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=303
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 303;
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
        case 303:
            // Grammar: ID=303; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=304
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 304;
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
        case 304:
            // Grammar: ID=304; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=305
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 305;
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
        case 305:
            // Grammar: ID=305; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=2
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
        case 306:
            // Grammar: ID=306; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=307
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 307;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 307:
            // Grammar: ID=307; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=308
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 308;
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
        case 308:
            // Grammar: ID=308; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=309
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 309;
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
        case 309:
            // Grammar: ID=309; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=310
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 310;
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
        case 310:
            // Grammar: ID=310; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=2
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_dc_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_dc_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 2;
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
                switch(eventCode)
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
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1);
static int decode_iso20_dc_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_dc_SignaturePropertiesType* SignaturePropertiesType) {
    int grammar_id = 311;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_dc_SignaturePropertiesType(SignaturePropertiesType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 311:
            // Grammar: ID=311; read/write bits=2; START (Id), START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=313
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertiesType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertiesType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertiesType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertiesType->Id.charactersLen, SignaturePropertiesType->Id.characters, iso20_dc_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertiesType->Id_isUsed = 1u;
                    grammar_id = 313;
                    break;
                case 1:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=312
                    // decode: element
                    error = decode_iso20_dc_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
                    {
                        grammar_id = 312;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 312:
            // Grammar: ID=312; read/write bits=2; START (SignatureProperty), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=2
                    // decode: element
                    error = decode_iso20_dc_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
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
        case 313:
            // Grammar: ID=313; read/write bits=1; START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=314
                    // decode: element
                    error = decode_iso20_dc_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
                    {
                        grammar_id = 314;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 314:
            // Grammar: ID=314; read/write bits=2; START (SignatureProperty), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=2
                    // decode: element
                    error = decode_iso20_dc_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
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
                switch(eventCode)
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
int decode_iso20_dc_exiDocument(exi_bitstream_t* stream, struct iso20_dc_exiDocument* exiDoc) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == 0)
    {
        init_iso20_dc_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
        if (error == 0)
        {
            switch(eventCode)
            {
            case 0:
                error = decode_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(stream, &exiDoc->BPT_DC_CPDReqEnergyTransferMode);
                exiDoc->BPT_DC_CPDReqEnergyTransferMode_isUsed = 1u;
                break;
            case 1:
                error = decode_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(stream, &exiDoc->BPT_DC_CPDResEnergyTransferMode);
                exiDoc->BPT_DC_CPDResEnergyTransferMode_isUsed = 1u;
                break;
            case 2:
                error = decode_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(stream, &exiDoc->BPT_Dynamic_DC_CLReqControlMode);
                exiDoc->BPT_Dynamic_DC_CLReqControlMode_isUsed = 1u;
                break;
            case 3:
                error = decode_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(stream, &exiDoc->BPT_Dynamic_DC_CLResControlMode);
                exiDoc->BPT_Dynamic_DC_CLResControlMode_isUsed = 1u;
                break;
            case 4:
                error = decode_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(stream, &exiDoc->BPT_Scheduled_DC_CLReqControlMode);
                exiDoc->BPT_Scheduled_DC_CLReqControlMode_isUsed = 1u;
                break;
            case 5:
                error = decode_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(stream, &exiDoc->BPT_Scheduled_DC_CLResControlMode);
                exiDoc->BPT_Scheduled_DC_CLResControlMode_isUsed = 1u;
                break;
            case 6:
                error = decode_iso20_dc_CLReqControlModeType(stream, &exiDoc->CLReqControlMode);
                exiDoc->CLReqControlMode_isUsed = 1u;
                break;
            case 7:
                error = decode_iso20_dc_CLResControlModeType(stream, &exiDoc->CLResControlMode);
                exiDoc->CLResControlMode_isUsed = 1u;
                break;
            case 8:
                error = decode_iso20_dc_CanonicalizationMethodType(stream, &exiDoc->CanonicalizationMethod);
                exiDoc->CanonicalizationMethod_isUsed = 1u;
                break;
            case 9:
                error = decode_iso20_dc_DC_CPDReqEnergyTransferModeType(stream, &exiDoc->DC_CPDReqEnergyTransferMode);
                exiDoc->DC_CPDReqEnergyTransferMode_isUsed = 1u;
                break;
            case 10:
                error = decode_iso20_dc_DC_CPDResEnergyTransferModeType(stream, &exiDoc->DC_CPDResEnergyTransferMode);
                exiDoc->DC_CPDResEnergyTransferMode_isUsed = 1u;
                break;
            case 11:
                error = decode_iso20_dc_DC_CableCheckReqType(stream, &exiDoc->DC_CableCheckReq);
                exiDoc->DC_CableCheckReq_isUsed = 1u;
                break;
            case 12:
                error = decode_iso20_dc_DC_CableCheckResType(stream, &exiDoc->DC_CableCheckRes);
                exiDoc->DC_CableCheckRes_isUsed = 1u;
                break;
            case 13:
                error = decode_iso20_dc_DC_ChargeLoopReqType(stream, &exiDoc->DC_ChargeLoopReq);
                exiDoc->DC_ChargeLoopReq_isUsed = 1u;
                break;
            case 14:
                error = decode_iso20_dc_DC_ChargeLoopResType(stream, &exiDoc->DC_ChargeLoopRes);
                exiDoc->DC_ChargeLoopRes_isUsed = 1u;
                break;
            case 15:
                error = decode_iso20_dc_DC_ChargeParameterDiscoveryReqType(stream, &exiDoc->DC_ChargeParameterDiscoveryReq);
                exiDoc->DC_ChargeParameterDiscoveryReq_isUsed = 1u;
                break;
            case 16:
                error = decode_iso20_dc_DC_ChargeParameterDiscoveryResType(stream, &exiDoc->DC_ChargeParameterDiscoveryRes);
                exiDoc->DC_ChargeParameterDiscoveryRes_isUsed = 1u;
                break;
            case 17:
                error = decode_iso20_dc_DC_PreChargeReqType(stream, &exiDoc->DC_PreChargeReq);
                exiDoc->DC_PreChargeReq_isUsed = 1u;
                break;
            case 18:
                error = decode_iso20_dc_DC_PreChargeResType(stream, &exiDoc->DC_PreChargeRes);
                exiDoc->DC_PreChargeRes_isUsed = 1u;
                break;
            case 19:
                error = decode_iso20_dc_DC_WeldingDetectionReqType(stream, &exiDoc->DC_WeldingDetectionReq);
                exiDoc->DC_WeldingDetectionReq_isUsed = 1u;
                break;
            case 20:
                error = decode_iso20_dc_DC_WeldingDetectionResType(stream, &exiDoc->DC_WeldingDetectionRes);
                exiDoc->DC_WeldingDetectionRes_isUsed = 1u;
                break;
            case 21:
                error = decode_iso20_dc_DSAKeyValueType(stream, &exiDoc->DSAKeyValue);
                exiDoc->DSAKeyValue_isUsed = 1u;
                break;
            case 22:
                error = decode_iso20_dc_DigestMethodType(stream, &exiDoc->DigestMethod);
                exiDoc->DigestMethod_isUsed = 1u;
                break;
            case 23:
                // simple type! decode_iso20_dc_DigestValue;
                break;
            case 24:
                error = decode_iso20_dc_Dynamic_DC_CLReqControlModeType(stream, &exiDoc->Dynamic_DC_CLReqControlMode);
                exiDoc->Dynamic_DC_CLReqControlMode_isUsed = 1u;
                break;
            case 25:
                error = decode_iso20_dc_Dynamic_DC_CLResControlModeType(stream, &exiDoc->Dynamic_DC_CLResControlMode);
                exiDoc->Dynamic_DC_CLResControlMode_isUsed = 1u;
                break;
            case 26:
                error = decode_iso20_dc_KeyInfoType(stream, &exiDoc->KeyInfo);
                exiDoc->KeyInfo_isUsed = 1u;
                break;
            case 27:
                // simple type! decode_iso20_dc_KeyName;
                break;
            case 28:
                error = decode_iso20_dc_KeyValueType(stream, &exiDoc->KeyValue);
                exiDoc->KeyValue_isUsed = 1u;
                break;
            case 29:
                error = decode_iso20_dc_ManifestType(stream, &exiDoc->Manifest);
                exiDoc->Manifest_isUsed = 1u;
                break;
            case 30:
                // simple type! decode_iso20_dc_MgmtData;
                break;
            case 31:
                error = decode_iso20_dc_ObjectType(stream, &exiDoc->Object);
                exiDoc->Object_isUsed = 1u;
                break;
            case 32:
                error = decode_iso20_dc_PGPDataType(stream, &exiDoc->PGPData);
                exiDoc->PGPData_isUsed = 1u;
                break;
            case 33:
                error = decode_iso20_dc_RSAKeyValueType(stream, &exiDoc->RSAKeyValue);
                exiDoc->RSAKeyValue_isUsed = 1u;
                break;
            case 34:
                error = decode_iso20_dc_ReferenceType(stream, &exiDoc->Reference);
                exiDoc->Reference_isUsed = 1u;
                break;
            case 35:
                error = decode_iso20_dc_RetrievalMethodType(stream, &exiDoc->RetrievalMethod);
                exiDoc->RetrievalMethod_isUsed = 1u;
                break;
            case 36:
                error = decode_iso20_dc_SPKIDataType(stream, &exiDoc->SPKIData);
                exiDoc->SPKIData_isUsed = 1u;
                break;
            case 37:
                error = decode_iso20_dc_Scheduled_DC_CLReqControlModeType(stream, &exiDoc->Scheduled_DC_CLReqControlMode);
                exiDoc->Scheduled_DC_CLReqControlMode_isUsed = 1u;
                break;
            case 38:
                error = decode_iso20_dc_Scheduled_DC_CLResControlModeType(stream, &exiDoc->Scheduled_DC_CLResControlMode);
                exiDoc->Scheduled_DC_CLResControlMode_isUsed = 1u;
                break;
            case 39:
                error = decode_iso20_dc_SignatureMethodType(stream, &exiDoc->SignatureMethod);
                exiDoc->SignatureMethod_isUsed = 1u;
                break;
            case 40:
                error = decode_iso20_dc_SignaturePropertiesType(stream, &exiDoc->SignatureProperties);
                exiDoc->SignatureProperties_isUsed = 1u;
                break;
            case 41:
                error = decode_iso20_dc_SignaturePropertyType(stream, &exiDoc->SignatureProperty);
                exiDoc->SignatureProperty_isUsed = 1u;
                break;
            case 42:
                error = decode_iso20_dc_SignatureType(stream, &exiDoc->Signature);
                exiDoc->Signature_isUsed = 1u;
                break;
            case 43:
                error = decode_iso20_dc_SignatureValueType(stream, &exiDoc->SignatureValue);
                exiDoc->SignatureValue_isUsed = 1u;
                break;
            case 44:
                error = decode_iso20_dc_SignedInfoType(stream, &exiDoc->SignedInfo);
                exiDoc->SignedInfo_isUsed = 1u;
                break;
            case 45:
                error = decode_iso20_dc_TransformType(stream, &exiDoc->Transform);
                exiDoc->Transform_isUsed = 1u;
                break;
            case 46:
                error = decode_iso20_dc_TransformsType(stream, &exiDoc->Transforms);
                exiDoc->Transforms_isUsed = 1u;
                break;
            case 47:
                error = decode_iso20_dc_X509DataType(stream, &exiDoc->X509Data);
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

// main function for decoding fragment
int decode_iso20_dc_exiFragment(exi_bitstream_t* stream, struct iso20_dc_exiFragment* exiFrag) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_iso20_dc_exiFragment(exiFrag);

        error = exi_basetypes_decoder_nbit_uint(stream, 8, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = EXI_ERROR__NOT_IMPLEMENTED_YET;
            switch(eventCode)
            {
            case 0:
                // AckMaxDelay (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 1:
                // AdditionalServicesCosts (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 2:
                // Amount (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 3:
                // BPT_DC_CPDReqEnergyTransferMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 4:
                // BPT_DC_CPDResEnergyTransferMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 5:
                // BPT_DischargedEnergyReadingWh (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 6:
                // BPT_Dynamic_DC_CLReqControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 7:
                // BPT_Dynamic_DC_CLResControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 8:
                // BPT_InductiveEnergyReadingVARh (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 9:
                // BPT_Scheduled_DC_CLReqControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 10:
                // BPT_Scheduled_DC_CLResControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 11:
                // BatteryEnergyCapacity (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 12:
                // CLReqControlMode (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 13:
                // CLResControlMode (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 14:
                // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 15:
                // CapacitiveEnergyReadingVARh (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 16:
                // ChargedEnergyReadingWh (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 17:
                // ChargingComplete (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 18:
                // CostPerUnit (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 19:
                // DC_CPDReqEnergyTransferMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 20:
                // DC_CPDResEnergyTransferMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 21:
                // DC_CableCheckReq (urn:iso:std:iso:15118:-20:DC)
                break;
            case 22:
                // DC_CableCheckRes (urn:iso:std:iso:15118:-20:DC)
                break;
            case 23:
                // DC_ChargeLoopReq (urn:iso:std:iso:15118:-20:DC)
                break;
            case 24:
                // DC_ChargeLoopRes (urn:iso:std:iso:15118:-20:DC)
                break;
            case 25:
                // DC_ChargeParameterDiscoveryReq (urn:iso:std:iso:15118:-20:DC)
                break;
            case 26:
                // DC_ChargeParameterDiscoveryRes (urn:iso:std:iso:15118:-20:DC)
                error = decode_iso20_dc_DC_ChargeParameterDiscoveryResType(stream, &exiFrag->DC_ChargeParameterDiscoveryRes);
                exiFrag->DC_ChargeParameterDiscoveryRes_isUsed = 1u;
                break;
            case 27:
                // DC_PreChargeReq (urn:iso:std:iso:15118:-20:DC)
                break;
            case 28:
                // DC_PreChargeRes (urn:iso:std:iso:15118:-20:DC)
                break;
            case 29:
                // DC_WeldingDetectionReq (urn:iso:std:iso:15118:-20:DC)
                break;
            case 30:
                // DC_WeldingDetectionRes (urn:iso:std:iso:15118:-20:DC)
                break;
            case 31:
                // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 32:
                // DepartureTime (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 33:
                // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 34:
                // DigestValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 35:
                // DisplayParameters (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 36:
                // Dynamic_DC_CLReqControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 37:
                // Dynamic_DC_CLResControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 38:
                // EVMaximumChargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 39:
                // EVMaximumChargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 40:
                // EVMaximumDischargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 41:
                // EVMaximumDischargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 42:
                // EVMaximumEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 43:
                // EVMaximumV2XEnergyRequest (urn:iso:std:iso:15118:-20:DC)
                break;
            case 44:
                // EVMaximumVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 45:
                // EVMinimumChargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 46:
                // EVMinimumChargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 47:
                // EVMinimumDischargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 48:
                // EVMinimumDischargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 49:
                // EVMinimumEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 50:
                // EVMinimumV2XEnergyRequest (urn:iso:std:iso:15118:-20:DC)
                break;
            case 51:
                // EVMinimumVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 52:
                // EVPresentVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 53:
                // EVProcessing (urn:iso:std:iso:15118:-20:DC)
                break;
            case 54:
                // EVSECurrentLimitAchieved (urn:iso:std:iso:15118:-20:DC)
                break;
            case 55:
                // EVSEMaximumChargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 56:
                // EVSEMaximumChargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 57:
                // EVSEMaximumDischargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 58:
                // EVSEMaximumDischargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 59:
                // EVSEMaximumVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 60:
                // EVSEMinimumChargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 61:
                // EVSEMinimumChargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 62:
                // EVSEMinimumDischargeCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 63:
                // EVSEMinimumDischargePower (urn:iso:std:iso:15118:-20:DC)
                break;
            case 64:
                // EVSEMinimumVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 65:
                // EVSENotification (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 66:
                // EVSEPowerLimitAchieved (urn:iso:std:iso:15118:-20:DC)
                break;
            case 67:
                // EVSEPowerRampLimitation (urn:iso:std:iso:15118:-20:DC)
                break;
            case 68:
                // EVSEPresentCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 69:
                // EVSEPresentVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 70:
                // EVSEProcessing (urn:iso:std:iso:15118:-20:DC)
                break;
            case 71:
                // EVSEStatus (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 72:
                // EVSEVoltageLimitAchieved (urn:iso:std:iso:15118:-20:DC)
                break;
            case 73:
                // EVTargetCurrent (urn:iso:std:iso:15118:-20:DC)
                break;
            case 74:
                // EVTargetEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 75:
                // EVTargetVoltage (urn:iso:std:iso:15118:-20:DC)
                break;
            case 76:
                // EnergyCosts (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 77:
                // Exponent (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 78:
                // Exponent (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 79:
                // G (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 80:
                // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 81:
                // Header (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 82:
                // InletHot (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 83:
                // J (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 84:
                // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 85:
                // KeyName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 86:
                // KeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 87:
                // Manifest (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 88:
                // MaximumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 89:
                // MeterID (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 90:
                // MeterInfo (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 91:
                // MeterInfoRequested (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 92:
                // MeterSignature (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 93:
                // MeterStatus (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 94:
                // MeterTimestamp (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 95:
                // MgmtData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 96:
                // MinimumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 97:
                // Modulus (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 98:
                // NotificationMaxDelay (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 99:
                // Object (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 100:
                // OccupancyCosts (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 101:
                // OverstayCosts (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 102:
                // P (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 103:
                // PGPData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 104:
                // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 105:
                // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 106:
                // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 107:
                // PresentSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 108:
                // Q (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 109:
                // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 110:
                // Receipt (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 111:
                // Reference (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 112:
                // RemainingTimeToMaximumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 113:
                // RemainingTimeToMinimumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 114:
                // RemainingTimeToTargetSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 115:
                // ResponseCode (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 116:
                // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 117:
                // RootCertificateID (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 118:
                // SPKIData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 119:
                // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 120:
                // Scheduled_DC_CLReqControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 121:
                // Scheduled_DC_CLResControlMode (urn:iso:std:iso:15118:-20:DC)
                break;
            case 122:
                // Seed (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 123:
                // SessionID (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 124:
                // Signature (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 125:
                // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 126:
                // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 127:
                // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 128:
                // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 129:
                // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignedInfoType(stream, &exiFrag->SignedInfo);
                exiFrag->SignedInfo_isUsed = 1u;
                break;
            case 130:
                // TargetSOC (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 131:
                // TargetSOC (urn:iso:std:iso:15118:-20:DC)
                break;
            case 132:
                // TaxCosts (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 133:
                // TaxRuleID (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 134:
                // TimeAnchor (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 135:
                // TimeStamp (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 136:
                // Transform (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 137:
                // Transforms (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 138:
                // Value (urn:iso:std:iso:15118:-20:CommonTypes)
                break;
            case 139:
                // X509CRL (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 140:
                // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 141:
                // X509Data (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 142:
                // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 143:
                // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 144:
                // X509SKI (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 145:
                // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 146:
                // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 147:
                // XPath (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 148:
                // Y (http://www.w3.org/2000/09/xmldsig#)
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }

            if (error == EXI_ERROR__NO_ERROR)
            {
                // End Fragment
                error = exi_basetypes_decoder_nbit_uint(stream, 8, &eventCode);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    if (eventCode != 150)
                    {
                        error = EXI_ERROR__INCORRECT_END_FRAGMENT_VALUE;
                    }
                }
            }
        }
    }

    return error;
}

// main function for decoding xmldsig fragment
int decode_iso20_dc_xmldsigFragment(exi_bitstream_t* stream, struct iso20_dc_xmldsigFragment* xmldsigFrag) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_iso20_dc_xmldsigFragment(xmldsigFrag);

        error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = EXI_ERROR__NOT_IMPLEMENTED_YET;
            switch(eventCode)
            {
            case 0:
                // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_CanonicalizationMethodType(stream, &xmldsigFrag->CanonicalizationMethod);
                xmldsigFrag->CanonicalizationMethod_isUsed = 1u;
                break;
            case 1:
                // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_DSAKeyValueType(stream, &xmldsigFrag->DSAKeyValue);
                xmldsigFrag->DSAKeyValue_isUsed = 1u;
                break;
            case 2:
                // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_DigestMethodType(stream, &xmldsigFrag->DigestMethod);
                xmldsigFrag->DigestMethod_isUsed = 1u;
                break;
            case 3:
                // DigestValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 4:
                // Exponent (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 5:
                // G (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 6:
                // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 7:
                // J (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 8:
                // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_KeyInfoType(stream, &xmldsigFrag->KeyInfo);
                xmldsigFrag->KeyInfo_isUsed = 1u;
                break;
            case 9:
                // KeyName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 10:
                // KeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_KeyValueType(stream, &xmldsigFrag->KeyValue);
                xmldsigFrag->KeyValue_isUsed = 1u;
                break;
            case 11:
                // Manifest (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_ManifestType(stream, &xmldsigFrag->Manifest);
                xmldsigFrag->Manifest_isUsed = 1u;
                break;
            case 12:
                // MgmtData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 13:
                // Modulus (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 14:
                // Object (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_ObjectType(stream, &xmldsigFrag->Object);
                xmldsigFrag->Object_isUsed = 1u;
                break;
            case 15:
                // P (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 16:
                // PGPData (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_PGPDataType(stream, &xmldsigFrag->PGPData);
                xmldsigFrag->PGPData_isUsed = 1u;
                break;
            case 17:
                // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 18:
                // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 19:
                // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 20:
                // Q (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 21:
                // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_RSAKeyValueType(stream, &xmldsigFrag->RSAKeyValue);
                xmldsigFrag->RSAKeyValue_isUsed = 1u;
                break;
            case 22:
                // Reference (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_ReferenceType(stream, &xmldsigFrag->Reference);
                xmldsigFrag->Reference_isUsed = 1u;
                break;
            case 23:
                // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_RetrievalMethodType(stream, &xmldsigFrag->RetrievalMethod);
                xmldsigFrag->RetrievalMethod_isUsed = 1u;
                break;
            case 24:
                // SPKIData (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SPKIDataType(stream, &xmldsigFrag->SPKIData);
                xmldsigFrag->SPKIData_isUsed = 1u;
                break;
            case 25:
                // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 26:
                // Seed (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 27:
                // Signature (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignatureType(stream, &xmldsigFrag->Signature);
                xmldsigFrag->Signature_isUsed = 1u;
                break;
            case 28:
                // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignatureMethodType(stream, &xmldsigFrag->SignatureMethod);
                xmldsigFrag->SignatureMethod_isUsed = 1u;
                break;
            case 29:
                // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignaturePropertiesType(stream, &xmldsigFrag->SignatureProperties);
                xmldsigFrag->SignatureProperties_isUsed = 1u;
                break;
            case 30:
                // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignaturePropertyType(stream, &xmldsigFrag->SignatureProperty);
                xmldsigFrag->SignatureProperty_isUsed = 1u;
                break;
            case 31:
                // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignatureValueType(stream, &xmldsigFrag->SignatureValue);
                xmldsigFrag->SignatureValue_isUsed = 1u;
                break;
            case 32:
                // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_SignedInfoType(stream, &xmldsigFrag->SignedInfo);
                xmldsigFrag->SignedInfo_isUsed = 1u;
                break;
            case 33:
                // Transform (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_TransformType(stream, &xmldsigFrag->Transform);
                xmldsigFrag->Transform_isUsed = 1u;
                break;
            case 34:
                // Transforms (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_TransformsType(stream, &xmldsigFrag->Transforms);
                xmldsigFrag->Transforms_isUsed = 1u;
                break;
            case 35:
                // X509CRL (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 36:
                // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 37:
                // X509Data (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_X509DataType(stream, &xmldsigFrag->X509Data);
                xmldsigFrag->X509Data_isUsed = 1u;
                break;
            case 38:
                // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 39:
                // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso20_dc_X509IssuerSerialType(stream, &xmldsigFrag->X509IssuerSerial);
                xmldsigFrag->X509IssuerSerial_isUsed = 1u;
                break;
            case 40:
                // X509SKI (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 41:
                // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 42:
                // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 43:
                // XPath (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 44:
                // Y (http://www.w3.org/2000/09/xmldsig#)
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }

            if (error == EXI_ERROR__NO_ERROR)
            {
                // End Fragment
                error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    if (eventCode != 46)
                    {
                        error = EXI_ERROR__INCORRECT_END_FRAGMENT_VALUE;
                    }
                }
            }
        }
    }

    return error;
}


