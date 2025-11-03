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
  * @file iso20_ACDP_Decoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_types_decoder.h"
#include "cbv2g/common/exi_basetypes_decoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/iso_20/iso20_ACDP_Datatypes.h"
#include "cbv2g/iso_20/iso20_ACDP_Decoder.h"



static int decode_iso20_acdp_TransformType(exi_bitstream_t* stream, struct iso20_acdp_TransformType* TransformType);
static int decode_iso20_acdp_TransformsType(exi_bitstream_t* stream, struct iso20_acdp_TransformsType* TransformsType);
static int decode_iso20_acdp_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_acdp_DSAKeyValueType* DSAKeyValueType);
static int decode_iso20_acdp_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_acdp_X509IssuerSerialType* X509IssuerSerialType);
static int decode_iso20_acdp_DigestMethodType(exi_bitstream_t* stream, struct iso20_acdp_DigestMethodType* DigestMethodType);
static int decode_iso20_acdp_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_acdp_RSAKeyValueType* RSAKeyValueType);
static int decode_iso20_acdp_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_acdp_CanonicalizationMethodType* CanonicalizationMethodType);
static int decode_iso20_acdp_SignatureMethodType(exi_bitstream_t* stream, struct iso20_acdp_SignatureMethodType* SignatureMethodType);
static int decode_iso20_acdp_KeyValueType(exi_bitstream_t* stream, struct iso20_acdp_KeyValueType* KeyValueType);
static int decode_iso20_acdp_ReferenceType(exi_bitstream_t* stream, struct iso20_acdp_ReferenceType* ReferenceType);
static int decode_iso20_acdp_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_acdp_RetrievalMethodType* RetrievalMethodType);
static int decode_iso20_acdp_X509DataType(exi_bitstream_t* stream, struct iso20_acdp_X509DataType* X509DataType);
static int decode_iso20_acdp_PGPDataType(exi_bitstream_t* stream, struct iso20_acdp_PGPDataType* PGPDataType);
static int decode_iso20_acdp_SPKIDataType(exi_bitstream_t* stream, struct iso20_acdp_SPKIDataType* SPKIDataType);
static int decode_iso20_acdp_SignedInfoType(exi_bitstream_t* stream, struct iso20_acdp_SignedInfoType* SignedInfoType);
static int decode_iso20_acdp_SignatureValueType(exi_bitstream_t* stream, struct iso20_acdp_SignatureValueType* SignatureValueType);
static int decode_iso20_acdp_KeyInfoType(exi_bitstream_t* stream, struct iso20_acdp_KeyInfoType* KeyInfoType);
static int decode_iso20_acdp_ObjectType(exi_bitstream_t* stream, struct iso20_acdp_ObjectType* ObjectType);
static int decode_iso20_acdp_SignatureType(exi_bitstream_t* stream, struct iso20_acdp_SignatureType* SignatureType);
static int decode_iso20_acdp_RationalNumberType(exi_bitstream_t* stream, struct iso20_acdp_RationalNumberType* RationalNumberType);
static int decode_iso20_acdp_MessageHeaderType(exi_bitstream_t* stream, struct iso20_acdp_MessageHeaderType* MessageHeaderType);
static int decode_iso20_acdp_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_acdp_SignaturePropertyType* SignaturePropertyType);
static int decode_iso20_acdp_EVTechnicalStatusType(exi_bitstream_t* stream, struct iso20_acdp_EVTechnicalStatusType* EVTechnicalStatusType);
static int decode_iso20_acdp_ACDP_VehiclePositioningReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_VehiclePositioningReqType* ACDP_VehiclePositioningReqType);
static int decode_iso20_acdp_ACDP_VehiclePositioningResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_VehiclePositioningResType* ACDP_VehiclePositioningResType);
static int decode_iso20_acdp_ACDP_ConnectReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_ConnectReqType* ACDP_ConnectReqType);
static int decode_iso20_acdp_ACDP_ConnectResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_ConnectResType* ACDP_ConnectResType);
static int decode_iso20_acdp_ACDP_SystemStatusReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_SystemStatusReqType* ACDP_SystemStatusReqType);
static int decode_iso20_acdp_ACDP_SystemStatusResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_SystemStatusResType* ACDP_SystemStatusResType);
static int decode_iso20_acdp_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_acdp_CLReqControlModeType* CLReqControlModeType);
static int decode_iso20_acdp_CLResControlModeType(exi_bitstream_t* stream, struct iso20_acdp_CLResControlModeType* CLResControlModeType);
static int decode_iso20_acdp_ManifestType(exi_bitstream_t* stream, struct iso20_acdp_ManifestType* ManifestType);
static int decode_iso20_acdp_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_acdp_SignaturePropertiesType* SignaturePropertiesType);

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int decode_iso20_acdp_TransformType(exi_bitstream_t* stream, struct iso20_acdp_TransformType* TransformType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_TransformType(TransformType);

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
                            error = exi_basetypes_decoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso20_acdp_Algorithm_CHARACTER_SIZE);
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
                                    error = exi_basetypes_decoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso20_acdp_XPath_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &TransformType->ANY.bytesLen, &TransformType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_TransformsType(exi_bitstream_t* stream, struct iso20_acdp_TransformsType* TransformsType) {
    int grammar_id = 4;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_TransformsType(TransformsType);

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
                    error = decode_iso20_acdp_TransformType(stream, &TransformsType->Transform);
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
static int decode_iso20_acdp_DSAKeyValueType(exi_bitstream_t* stream, struct iso20_acdp_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 6;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_DSAKeyValueType(DSAKeyValueType);

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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->P.bytesLen, &DSAKeyValueType->P.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->P_isUsed = 1u;
                        grammar_id = 7;
                    }
                    break;
                case 1:
                    // Event: START (G, CryptoBinary (base64Binary)); next=9
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 2:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Q.bytesLen, &DSAKeyValueType->Q.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 1:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->J.bytesLen, &DSAKeyValueType->J.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->J_isUsed = 1u;
                        grammar_id = 11;
                    }
                    break;
                case 1:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->PgenCounter.bytesLen, &DSAKeyValueType->PgenCounter.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
static int decode_iso20_acdp_X509IssuerSerialType(exi_bitstream_t* stream, struct iso20_acdp_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 13;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_X509IssuerSerialType(X509IssuerSerialType);

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
                                    error = exi_basetypes_decoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso20_acdp_X509IssuerName_CHARACTER_SIZE);
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
static int decode_iso20_acdp_DigestMethodType(exi_bitstream_t* stream, struct iso20_acdp_DigestMethodType* DigestMethodType) {
    int grammar_id = 15;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_DigestMethodType(DigestMethodType);

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
                            error = exi_basetypes_decoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso20_acdp_Algorithm_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &DigestMethodType->ANY.bytesLen, &DigestMethodType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_RSAKeyValueType(exi_bitstream_t* stream, struct iso20_acdp_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 17;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_RSAKeyValueType(RSAKeyValueType);

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
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Modulus.bytesLen, &RSAKeyValueType->Modulus.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Exponent.bytesLen, &RSAKeyValueType->Exponent.bytes[0], iso20_acdp_CryptoBinary_BYTES_SIZE);
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
static int decode_iso20_acdp_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso20_acdp_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 19;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_CanonicalizationMethodType(CanonicalizationMethodType);

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
                            error = exi_basetypes_decoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso20_acdp_Algorithm_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &CanonicalizationMethodType->ANY.bytesLen, &CanonicalizationMethodType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
static int decode_iso20_acdp_SignatureMethodType(exi_bitstream_t* stream, struct iso20_acdp_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 21;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignatureMethodType(SignatureMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 21:
            // Grammar: ID=21; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso20_acdp_Algorithm_CHARACTER_SIZE);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (HMACOutputLength, HMACOutputLengthType (integer)); next=23
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == 0)
                        {
                            SignatureMethodType->HMACOutputLength_isUsed = 1u;
                            grammar_id = 23;
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_KeyValueType(exi_bitstream_t* stream, struct iso20_acdp_KeyValueType* KeyValueType) {
    int grammar_id = 24;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_KeyValueType(KeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 24:
            // Grammar: ID=24; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DSAKeyValue, DSAKeyValueType (DSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->DSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (RSAKeyValue, RSAKeyValueType (RSAKeyValueType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->RSAKeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyValueType->ANY.bytesLen, &KeyValueType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
static int decode_iso20_acdp_ReferenceType(exi_bitstream_t* stream, struct iso20_acdp_ReferenceType* ReferenceType) {
    int grammar_id = 25;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ReferenceType(ReferenceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 25:
            // Grammar: ID=25; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_acdp_Type_CHARACTER_SIZE);
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_acdp_URI_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 4:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_acdp_DigestMethodType(stream, &ReferenceType->DigestMethod);
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
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_acdp_Type_CHARACTER_SIZE);
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_acdp_URI_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 3:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_acdp_DigestMethodType(stream, &ReferenceType->DigestMethod);
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
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_acdp_URI_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 2:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_acdp_DigestMethodType(stream, &ReferenceType->DigestMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=29
                    // decode: element
                    error = decode_iso20_acdp_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 29;
                    }
                    break;
                case 1:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_acdp_DigestMethodType(stream, &ReferenceType->DigestMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=30
                    // decode: element
                    error = decode_iso20_acdp_DigestMethodType(stream, &ReferenceType->DigestMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestValue, DigestValueType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ReferenceType->DigestValue.bytesLen, &ReferenceType->DigestValue.bytes[0], iso20_acdp_DigestValueType_BYTES_SIZE);
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
static int decode_iso20_acdp_RetrievalMethodType(exi_bitstream_t* stream, struct iso20_acdp_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 31;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_RetrievalMethodType(RetrievalMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 31:
            // Grammar: ID=31; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso20_acdp_Type_CHARACTER_SIZE);
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
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_acdp_URI_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_TransformsType(stream, &RetrievalMethodType->Transforms);
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
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_acdp_URI_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_TransformsType(stream, &RetrievalMethodType->Transforms);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_TransformsType(stream, &RetrievalMethodType->Transforms);
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
static int decode_iso20_acdp_X509DataType(exi_bitstream_t* stream, struct iso20_acdp_X509DataType* X509DataType) {
    int grammar_id = 34;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_X509DataType(X509DataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 34:
            // Grammar: ID=34; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerSerial, X509IssuerSerialType (X509IssuerSerialType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == 0)
                    {
                        X509DataType->X509IssuerSerial_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 1:
                    // Event: START (X509SKI, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509SKI.bytesLen, &X509DataType->X509SKI.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
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
                                    error = exi_basetypes_decoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso20_acdp_X509SubjectName_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509Certificate.bytesLen, &X509DataType->X509Certificate.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509Certificate_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509CRL, base64Binary (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509CRL.bytesLen, &X509DataType->X509CRL.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509CRL_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->ANY.bytesLen, &X509DataType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_PGPDataType(exi_bitstream_t* stream, struct iso20_acdp_PGPDataType* PGPDataType) {
    int grammar_id = 35;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_PGPDataType(PGPDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 35:
            // Grammar: ID=35; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyID, base64Binary (base64Binary)); next=36
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyID.bytesLen, &PGPDataType->choice_1.PGPKeyID.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 36;
                    }
                    break;
                case 1:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=37
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=37
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
                switch (eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=39
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.PGPKeyPacket.bytesLen, &PGPDataType->choice_2.PGPKeyPacket.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
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
                switch (eventCode)
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
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.ANY.bytesLen, &PGPDataType->choice_2.ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_SPKIDataType(exi_bitstream_t* stream, struct iso20_acdp_SPKIDataType* SPKIDataType) {
    int grammar_id = 40;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SPKIDataType(SPKIDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 40:
            // Grammar: ID=40; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPKISexp, base64Binary (base64Binary)); next=41
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->SPKISexp.bytesLen, &SPKIDataType->SPKISexp.bytes[0], iso20_acdp_base64Binary_BYTES_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->ANY.bytesLen, &SPKIDataType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
static int decode_iso20_acdp_SignedInfoType(exi_bitstream_t* stream, struct iso20_acdp_SignedInfoType* SignedInfoType) {
    int grammar_id = 42;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignedInfoType(SignedInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 42:
            // Grammar: ID=42; read/write bits=2; START (Id), START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
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
                            error = exi_basetypes_decoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=44
                    // decode: element
                    error = decode_iso20_acdp_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureMethod, SignatureMethodType (SignatureMethodType)); next=45
                    // decode: element
                    error = decode_iso20_acdp_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
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
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=46
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
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
            // Grammar: ID=46; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=46
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 46;
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
static int decode_iso20_acdp_SignatureValueType(exi_bitstream_t* stream, struct iso20_acdp_SignatureValueType* SignatureValueType) {
    int grammar_id = 47;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignatureValueType(SignatureValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 47:
            // Grammar: ID=47; read/write bits=2; START (Id), START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=48
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureValueType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureValueType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureValueType->Id_isUsed = 1u;
                    grammar_id = 48;
                    break;
                case 1:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=2
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_acdp_SignatureValueType_BYTES_SIZE);
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
        case 48:
            // Grammar: ID=48; read/write bits=1; START (CONTENT)
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
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso20_acdp_SignatureValueType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int decode_iso20_acdp_KeyInfoType(exi_bitstream_t* stream, struct iso20_acdp_KeyInfoType* KeyInfoType) {
    int grammar_id = 49;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_KeyInfoType(KeyInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 49:
            // Grammar: ID=49; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=50
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (KeyInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            KeyInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    KeyInfoType->Id_isUsed = 1u;
                    grammar_id = 50;
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_acdp_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 6:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_acdp_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
        case 50:
            // Grammar: ID=50; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_acdp_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso20_acdp_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 2:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 3:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 4:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 2;
                    }
                    break;
                case 5:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_acdp_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int decode_iso20_acdp_ObjectType(exi_bitstream_t* stream, struct iso20_acdp_ObjectType* ObjectType) {
    int grammar_id = 51;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ObjectType(ObjectType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 51:
            // Grammar: ID=51; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Encoding, anyURI (anyURI)); next=52
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Encoding.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Encoding.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Encoding.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso20_acdp_Encoding_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Encoding_isUsed = 1u;
                    grammar_id = 52;
                    break;
                case 1:
                    // Event: START (Id, ID (NCName)); next=53
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 53;
                    break;
                case 2:
                    // Event: START (MimeType, string (string)); next=54
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_acdp_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 54;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
        case 52:
            // Grammar: ID=52; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=53
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 53;
                    break;
                case 1:
                    // Event: START (MimeType, string (string)); next=54
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_acdp_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 54;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
        case 53:
            // Grammar: ID=53; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MimeType, string (string)); next=54
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_acdp_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 54;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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
        case 54:
            // Grammar: ID=54; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
static int decode_iso20_acdp_SignatureType(exi_bitstream_t* stream, struct iso20_acdp_SignatureType* SignatureType) {
    int grammar_id = 55;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignatureType(SignatureType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 55:
            // Grammar: ID=55; read/write bits=2; START (Id), START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=56
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureType->Id_isUsed = 1u;
                    grammar_id = 56;
                    break;
                case 1:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=57
                    // decode: element
                    error = decode_iso20_acdp_SignedInfoType(stream, &SignatureType->SignedInfo);
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
        case 56:
            // Grammar: ID=56; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=57
                    // decode: element
                    error = decode_iso20_acdp_SignedInfoType(stream, &SignatureType->SignedInfo);
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
            // Grammar: ID=57; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureValue, SignatureValueType (base64Binary)); next=58
                    // decode: element
                    error = decode_iso20_acdp_SignatureValueType(stream, &SignatureType->SignatureValue);
                    if (error == 0)
                    {
                        grammar_id = 58;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 58:
            // Grammar: ID=58; read/write bits=2; START (KeyInfo), START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyInfo, KeyInfoType (KeyInfoType)); next=60
                    // decode: element
                    error = decode_iso20_acdp_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == 0)
                    {
                        SignatureType->KeyInfo_isUsed = 1u;
                        grammar_id = 60;
                    }
                    break;
                case 1:
                    // Event: START (Object, ObjectType (ObjectType)); next=59
                    // decode: element
                    error = decode_iso20_acdp_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 59;
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
        case 59:
            // Grammar: ID=59; read/write bits=2; START (Object), END Element
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
        case 60:
            // Grammar: ID=60; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=61
                    // decode: element
                    error = decode_iso20_acdp_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 61;
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
        case 61:
            // Grammar: ID=61; read/write bits=2; START (Object), END Element
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}EVWLANStrength; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
static int decode_iso20_acdp_RationalNumberType(exi_bitstream_t* stream, struct iso20_acdp_RationalNumberType* RationalNumberType) {
    int grammar_id = 62;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_RationalNumberType(RationalNumberType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 62:
            // Grammar: ID=62; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Exponent, byte (short)); next=63
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
                                grammar_id = 63;
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
        case 63:
            // Grammar: ID=63; read/write bits=1; START (Value)
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
static int decode_iso20_acdp_MessageHeaderType(exi_bitstream_t* stream, struct iso20_acdp_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 64;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_MessageHeaderType(MessageHeaderType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 64:
            // Grammar: ID=64; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=65
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MessageHeaderType->SessionID.bytesLen, &MessageHeaderType->SessionID.bytes[0], iso20_acdp_sessionIDType_BYTES_SIZE);
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
            // Grammar: ID=65; read/write bits=1; START (TimeStamp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TimeStamp, unsignedLong (nonNegativeInteger)); next=66
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MessageHeaderType->TimeStamp);
                    if (error == 0)
                    {
                        grammar_id = 66;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=2; START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Signature, SignatureType (SignatureType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_SignatureType(stream, &MessageHeaderType->Signature);
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
static int decode_iso20_acdp_SignaturePropertyType(exi_bitstream_t* stream, struct iso20_acdp_SignaturePropertyType* SignaturePropertyType) {
    int grammar_id = 67;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignaturePropertyType(SignaturePropertyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 67:
            // Grammar: ID=67; read/write bits=2; START (Id), START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=68
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Id.charactersLen, SignaturePropertyType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertyType->Id_isUsed = 1u;
                    grammar_id = 68;
                    break;
                case 1:
                    // Event: START (Target, anyURI (anyURI)); next=69
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_acdp_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 69;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=1; START (Target)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Target, anyURI (anyURI)); next=69
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertyType->Target.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertyType->Target.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertyType->Target.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_acdp_Target_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 69;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 69:
            // Grammar: ID=69; read/write bits=1; START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=2
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignaturePropertyType->ANY.bytesLen, &SignaturePropertyType->ANY.bytes[0], iso20_acdp_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}EVTechnicalStatus; type={urn:iso:std:iso:15118:-20:ACDP}EVTechnicalStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVReadyToCharge, boolean (1, 1); EVImmobilizationRequest, boolean (1, 1); EVImmobilized, boolean (0, 1); EVWLANStrength, RationalNumberType (0, 1); EVCPStatus, cpStatusType (0, 1); EVSOC, percentValueType (0, 1); EVErrorCode, errorCodeType (0, 1); EVTimeout, boolean (0, 1);
static int decode_iso20_acdp_EVTechnicalStatusType(exi_bitstream_t* stream, struct iso20_acdp_EVTechnicalStatusType* EVTechnicalStatusType) {
    int grammar_id = 70;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_EVTechnicalStatusType(EVTechnicalStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 70:
            // Grammar: ID=70; read/write bits=1; START (EVReadyToCharge)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVReadyToCharge, boolean (boolean)); next=71
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
                                EVTechnicalStatusType->EVReadyToCharge = value;
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
                                grammar_id = 71;
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
        case 71:
            // Grammar: ID=71; read/write bits=1; START (EVImmobilizationRequest)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVImmobilizationRequest, boolean (boolean)); next=72
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
                                EVTechnicalStatusType->EVImmobilizationRequest = value;
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
                                grammar_id = 72;
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
        case 72:
            // Grammar: ID=72; read/write bits=3; START (EVImmobilized), START (EVWLANStrength), START (EVCPStatus), START (EVSOC), START (EVErrorCode), START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVImmobilized, boolean (boolean)); next=73
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
                                EVTechnicalStatusType->EVImmobilized = value;
                                EVTechnicalStatusType->EVImmobilized_isUsed = 1u;
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
                                grammar_id = 73;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVWLANStrength, RationalNumberType (RationalNumberType)); next=74
                    // decode: element
                    error = decode_iso20_acdp_RationalNumberType(stream, &EVTechnicalStatusType->EVWLANStrength);
                    if (error == 0)
                    {
                        EVTechnicalStatusType->EVWLANStrength_isUsed = 1u;
                        grammar_id = 74;
                    }
                    break;
                case 2:
                    // Event: START (EVCPStatus, cpStatusType (string)); next=75
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
                                EVTechnicalStatusType->EVCPStatus = (iso20_acdp_cpStatusType)value;
                                EVTechnicalStatusType->EVCPStatus_isUsed = 1u;
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
                                grammar_id = 75;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (EVSOC, percentValueType (byte)); next=76
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
                                EVTechnicalStatusType->EVSOC = (int8_t)value;
                                EVTechnicalStatusType->EVSOC_isUsed = 1u;
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
                                grammar_id = 76;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (EVErrorCode, errorCodeType (string)); next=77
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                EVTechnicalStatusType->EVErrorCode = (iso20_acdp_errorCodeType)value;
                                EVTechnicalStatusType->EVErrorCode_isUsed = 1u;
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
                                grammar_id = 77;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 5:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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
        case 73:
            // Grammar: ID=73; read/write bits=3; START (EVWLANStrength), START (EVCPStatus), START (EVSOC), START (EVErrorCode), START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVWLANStrength, RationalNumberType (RationalNumberType)); next=74
                    // decode: element
                    error = decode_iso20_acdp_RationalNumberType(stream, &EVTechnicalStatusType->EVWLANStrength);
                    if (error == 0)
                    {
                        EVTechnicalStatusType->EVWLANStrength_isUsed = 1u;
                        grammar_id = 74;
                    }
                    break;
                case 1:
                    // Event: START (EVCPStatus, cpStatusType (string)); next=75
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
                                EVTechnicalStatusType->EVCPStatus = (iso20_acdp_cpStatusType)value;
                                EVTechnicalStatusType->EVCPStatus_isUsed = 1u;
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
                                grammar_id = 75;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (EVSOC, percentValueType (byte)); next=76
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
                                EVTechnicalStatusType->EVSOC = (int8_t)value;
                                EVTechnicalStatusType->EVSOC_isUsed = 1u;
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
                                grammar_id = 76;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (EVErrorCode, errorCodeType (string)); next=77
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                EVTechnicalStatusType->EVErrorCode = (iso20_acdp_errorCodeType)value;
                                EVTechnicalStatusType->EVErrorCode_isUsed = 1u;
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
                                grammar_id = 77;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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
        case 74:
            // Grammar: ID=74; read/write bits=3; START (EVCPStatus), START (EVSOC), START (EVErrorCode), START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVCPStatus, cpStatusType (string)); next=75
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
                                EVTechnicalStatusType->EVCPStatus = (iso20_acdp_cpStatusType)value;
                                EVTechnicalStatusType->EVCPStatus_isUsed = 1u;
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
                                grammar_id = 75;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVSOC, percentValueType (byte)); next=76
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
                                EVTechnicalStatusType->EVSOC = (int8_t)value;
                                EVTechnicalStatusType->EVSOC_isUsed = 1u;
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
                                grammar_id = 76;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (EVErrorCode, errorCodeType (string)); next=77
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                EVTechnicalStatusType->EVErrorCode = (iso20_acdp_errorCodeType)value;
                                EVTechnicalStatusType->EVErrorCode_isUsed = 1u;
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
                                grammar_id = 77;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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
        case 75:
            // Grammar: ID=75; read/write bits=3; START (EVSOC), START (EVErrorCode), START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSOC, percentValueType (byte)); next=76
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
                                EVTechnicalStatusType->EVSOC = (int8_t)value;
                                EVTechnicalStatusType->EVSOC_isUsed = 1u;
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
                                grammar_id = 76;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVErrorCode, errorCodeType (string)); next=77
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                EVTechnicalStatusType->EVErrorCode = (iso20_acdp_errorCodeType)value;
                                EVTechnicalStatusType->EVErrorCode_isUsed = 1u;
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
                                grammar_id = 77;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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
        case 76:
            // Grammar: ID=76; read/write bits=2; START (EVErrorCode), START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVErrorCode, errorCodeType (string)); next=77
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                EVTechnicalStatusType->EVErrorCode = (iso20_acdp_errorCodeType)value;
                                EVTechnicalStatusType->EVErrorCode_isUsed = 1u;
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
                                grammar_id = 77;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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
        case 77:
            // Grammar: ID=77; read/write bits=2; START (EVTimeout), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTimeout, boolean (boolean)); next=2
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
                                EVTechnicalStatusType->EVTimeout = value;
                                EVTechnicalStatusType->EVTimeout_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVMobilityStatus, boolean (1, 1); EVPositioningSupport, boolean (1, 1);
static int decode_iso20_acdp_ACDP_VehiclePositioningReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_VehiclePositioningReqType* ACDP_VehiclePositioningReqType) {
    int grammar_id = 78;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_VehiclePositioningReqType(ACDP_VehiclePositioningReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 78:
            // Grammar: ID=78; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=79
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_VehiclePositioningReqType->Header);
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
            // Grammar: ID=79; read/write bits=1; START (EVMobilityStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMobilityStatus, boolean (boolean)); next=80
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
                                ACDP_VehiclePositioningReqType->EVMobilityStatus = value;
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
            // Grammar: ID=80; read/write bits=1; START (EVPositioningSupport)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVPositioningSupport, boolean (boolean)); next=2
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
                                ACDP_VehiclePositioningReqType->EVPositioningSupport = value;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); EVSEPositioningSupport, boolean (1, 1); EVRelativeXDeviation, short (1, 1); EVRelativeYDeviation, short (1, 1); ContactWindowXc, short (1, 1); ContactWindowYc, short (1, 1); EVInChargePosition, boolean (1, 1);
static int decode_iso20_acdp_ACDP_VehiclePositioningResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_VehiclePositioningResType* ACDP_VehiclePositioningResType) {
    int grammar_id = 81;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_VehiclePositioningResType(ACDP_VehiclePositioningResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 81:
            // Grammar: ID=81; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=82
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_VehiclePositioningResType->Header);
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
            // Grammar: ID=82; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=83
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
                                ACDP_VehiclePositioningResType->ResponseCode = (iso20_acdp_responseCodeType)value;
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
                                grammar_id = 83;
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
        case 83:
            // Grammar: ID=83; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=84
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
                                ACDP_VehiclePositioningResType->EVSEProcessing = (iso20_acdp_processingType)value;
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
                                grammar_id = 84;
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
        case 84:
            // Grammar: ID=84; read/write bits=1; START (EVSEPositioningSupport)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPositioningSupport, boolean (boolean)); next=85
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
                                ACDP_VehiclePositioningResType->EVSEPositioningSupport = value;
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
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=1; START (EVRelativeXDeviation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVRelativeXDeviation, short (int)); next=86
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ACDP_VehiclePositioningResType->EVRelativeXDeviation);
                    if (error == 0)
                    {
                        grammar_id = 86;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 86:
            // Grammar: ID=86; read/write bits=1; START (EVRelativeYDeviation)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVRelativeYDeviation, short (int)); next=87
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ACDP_VehiclePositioningResType->EVRelativeYDeviation);
                    if (error == 0)
                    {
                        grammar_id = 87;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=1; START (ContactWindowXc)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContactWindowXc, short (int)); next=88
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ACDP_VehiclePositioningResType->ContactWindowXc);
                    if (error == 0)
                    {
                        grammar_id = 88;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 88:
            // Grammar: ID=88; read/write bits=1; START (ContactWindowYc)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContactWindowYc, short (int)); next=89
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ACDP_VehiclePositioningResType->ContactWindowYc);
                    if (error == 0)
                    {
                        grammar_id = 89;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 89:
            // Grammar: ID=89; read/write bits=1; START (EVInChargePosition)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVInChargePosition, boolean (boolean)); next=2
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
                                ACDP_VehiclePositioningResType->EVInChargePosition = value;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (1, 1);
static int decode_iso20_acdp_ACDP_ConnectReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_ConnectReqType* ACDP_ConnectReqType) {
    int grammar_id = 90;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_ConnectReqType(ACDP_ConnectReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 90:
            // Grammar: ID=90; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=91
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_ConnectReqType->Header);
                    if (error == 0)
                    {
                        grammar_id = 91;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 91:
            // Grammar: ID=91; read/write bits=1; START (EVElectricalChargingDeviceStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (string)); next=2
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
                                ACDP_ConnectReqType->EVElectricalChargingDeviceStatus = (iso20_acdp_electricalChargingDeviceStatusType)value;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); EVSEElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (1, 1); EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (1, 1);
static int decode_iso20_acdp_ACDP_ConnectResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_ConnectResType* ACDP_ConnectResType) {
    int grammar_id = 92;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_ConnectResType(ACDP_ConnectResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 92:
            // Grammar: ID=92; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=93
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_ConnectResType->Header);
                    if (error == 0)
                    {
                        grammar_id = 93;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 93:
            // Grammar: ID=93; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=94
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
                                ACDP_ConnectResType->ResponseCode = (iso20_acdp_responseCodeType)value;
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
                                grammar_id = 94;
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
        case 94:
            // Grammar: ID=94; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, processingType (string)); next=95
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
                                ACDP_ConnectResType->EVSEProcessing = (iso20_acdp_processingType)value;
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
                                grammar_id = 95;
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
        case 95:
            // Grammar: ID=95; read/write bits=1; START (EVSEElectricalChargingDeviceStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (string)); next=96
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
                                ACDP_ConnectResType->EVSEElectricalChargingDeviceStatus = (iso20_acdp_electricalChargingDeviceStatusType)value;
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
                                grammar_id = 96;
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
        case 96:
            // Grammar: ID=96; read/write bits=1; START (EVSEMechanicalChargingDeviceStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (string)); next=2
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
                                ACDP_ConnectResType->EVSEMechanicalChargingDeviceStatus = (iso20_acdp_mechanicalChargingDeviceStatusType)value;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVTechnicalStatus, EVTechnicalStatusType (1, 1);
static int decode_iso20_acdp_ACDP_SystemStatusReqType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_SystemStatusReqType* ACDP_SystemStatusReqType) {
    int grammar_id = 97;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_SystemStatusReqType(ACDP_SystemStatusReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 97:
            // Grammar: ID=97; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=98
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_SystemStatusReqType->Header);
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
            // Grammar: ID=98; read/write bits=1; START (EVTechnicalStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTechnicalStatus, EVTechnicalStatusType (EVTechnicalStatusType)); next=2
                    // decode: element
                    error = decode_iso20_acdp_EVTechnicalStatusType(stream, &ACDP_SystemStatusReqType->EVTechnicalStatus);
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (1, 1); EVSEReadyToCharge, boolean (1, 1); EVSEIsolationStatus, isolationStatusType (1, 1); EVSEDisabled, boolean (1, 1); EVSEUtilityInterruptEvent, boolean (1, 1); EVSEEmergencyShutdown, boolean (1, 1); EVSEMalfunction, boolean (1, 1); EVInChargePosition, boolean (1, 1); EVAssociationStatus, boolean (1, 1);
static int decode_iso20_acdp_ACDP_SystemStatusResType(exi_bitstream_t* stream, struct iso20_acdp_ACDP_SystemStatusResType* ACDP_SystemStatusResType) {
    int grammar_id = 99;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ACDP_SystemStatusResType(ACDP_SystemStatusResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 99:
            // Grammar: ID=99; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=100
                    // decode: element
                    error = decode_iso20_acdp_MessageHeaderType(stream, &ACDP_SystemStatusResType->Header);
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
            // Grammar: ID=100; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=101
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
                                ACDP_SystemStatusResType->ResponseCode = (iso20_acdp_responseCodeType)value;
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
                                grammar_id = 101;
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
        case 101:
            // Grammar: ID=101; read/write bits=1; START (EVSEMechanicalChargingDeviceStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (string)); next=102
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
                                ACDP_SystemStatusResType->EVSEMechanicalChargingDeviceStatus = (iso20_acdp_mechanicalChargingDeviceStatusType)value;
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
                                grammar_id = 102;
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
        case 102:
            // Grammar: ID=102; read/write bits=1; START (EVSEReadyToCharge)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEReadyToCharge, boolean (boolean)); next=103
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
                                ACDP_SystemStatusResType->EVSEReadyToCharge = value;
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
                                grammar_id = 103;
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
        case 103:
            // Grammar: ID=103; read/write bits=1; START (EVSEIsolationStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEIsolationStatus, isolationStatusType (string)); next=104
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
                                ACDP_SystemStatusResType->EVSEIsolationStatus = (iso20_acdp_isolationStatusType)value;
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
            // Grammar: ID=104; read/write bits=1; START (EVSEDisabled)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEDisabled, boolean (boolean)); next=105
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
                                ACDP_SystemStatusResType->EVSEDisabled = value;
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
                                grammar_id = 105;
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
        case 105:
            // Grammar: ID=105; read/write bits=1; START (EVSEUtilityInterruptEvent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEUtilityInterruptEvent, boolean (boolean)); next=106
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
                                ACDP_SystemStatusResType->EVSEUtilityInterruptEvent = value;
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
                                grammar_id = 106;
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
        case 106:
            // Grammar: ID=106; read/write bits=1; START (EVSEEmergencyShutdown)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEEmergencyShutdown, boolean (boolean)); next=107
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
                                ACDP_SystemStatusResType->EVSEEmergencyShutdown = value;
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
                                grammar_id = 107;
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
        case 107:
            // Grammar: ID=107; read/write bits=1; START (EVSEMalfunction)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMalfunction, boolean (boolean)); next=108
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
                                ACDP_SystemStatusResType->EVSEMalfunction = value;
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
                                grammar_id = 108;
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
        case 108:
            // Grammar: ID=108; read/write bits=1; START (EVInChargePosition)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVInChargePosition, boolean (boolean)); next=109
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
                                ACDP_SystemStatusResType->EVInChargePosition = value;
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
                                grammar_id = 109;
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
        case 109:
            // Grammar: ID=109; read/write bits=1; START (EVAssociationStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVAssociationStatus, boolean (boolean)); next=2
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
                                ACDP_SystemStatusResType->EVAssociationStatus = value;
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int decode_iso20_acdp_CLReqControlModeType(exi_bitstream_t* stream, struct iso20_acdp_CLReqControlModeType* CLReqControlModeType) {
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
static int decode_iso20_acdp_CLResControlModeType(exi_bitstream_t* stream, struct iso20_acdp_CLResControlModeType* CLResControlModeType) {
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
static int decode_iso20_acdp_ManifestType(exi_bitstream_t* stream, struct iso20_acdp_ManifestType* ManifestType) {
    int grammar_id = 110;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_ManifestType(ManifestType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 110:
            // Grammar: ID=110; read/write bits=2; START (Id), START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=112
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ManifestType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ManifestType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ManifestType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ManifestType->Id.charactersLen, ManifestType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ManifestType->Id_isUsed = 1u;
                    grammar_id = 112;
                    break;
                case 1:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=111
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 111;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 111:
            // Grammar: ID=111; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=111
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 111;
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
        case 112:
            // Grammar: ID=112; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=113
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 113;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 113:
            // Grammar: ID=113; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=113
                    // decode: element array
                    if (ManifestType->Reference.arrayLen < iso20_acdp_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso20_acdp_ReferenceType(stream, &ManifestType->Reference.array[ManifestType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso20_acdp_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 113;
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
static int decode_iso20_acdp_SignaturePropertiesType(exi_bitstream_t* stream, struct iso20_acdp_SignaturePropertiesType* SignaturePropertiesType) {
    int grammar_id = 114;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso20_acdp_SignaturePropertiesType(SignaturePropertiesType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 114:
            // Grammar: ID=114; read/write bits=2; START (Id), START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=116
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignaturePropertiesType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignaturePropertiesType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignaturePropertiesType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignaturePropertiesType->Id.charactersLen, SignaturePropertiesType->Id.characters, iso20_acdp_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignaturePropertiesType->Id_isUsed = 1u;
                    grammar_id = 116;
                    break;
                case 1:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=115
                    // decode: element
                    error = decode_iso20_acdp_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
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
            // Grammar: ID=115; read/write bits=2; START (SignatureProperty), END Element
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
        case 116:
            // Grammar: ID=116; read/write bits=1; START (SignatureProperty)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureProperty, SignaturePropertyType (SignaturePropertyType)); next=117
                    // decode: element
                    error = decode_iso20_acdp_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == 0)
                    {
                        grammar_id = 117;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 117:
            // Grammar: ID=117; read/write bits=2; START (SignatureProperty), END Element
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
int decode_iso20_acdp_exiDocument(exi_bitstream_t* stream, struct iso20_acdp_exiDocument* exiDoc) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == 0)
    {
        init_iso20_acdp_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
        if (error == 0)
        {
            switch (eventCode)
            {
            case 0:
                error = decode_iso20_acdp_ACDP_ConnectReqType(stream, &exiDoc->ACDP_ConnectReq);
                exiDoc->ACDP_ConnectReq_isUsed = 1u;
                break;
            case 1:
                error = decode_iso20_acdp_ACDP_ConnectReqType(stream, &exiDoc->ACDP_DisconnectReq);
                exiDoc->ACDP_DisconnectReq_isUsed = 1u;
                break;
            case 2:
                error = decode_iso20_acdp_ACDP_ConnectResType(stream, &exiDoc->ACDP_ConnectRes);
                exiDoc->ACDP_ConnectRes_isUsed = 1u;
                break;
            case 3:
                error = decode_iso20_acdp_ACDP_ConnectResType(stream, &exiDoc->ACDP_DisconnectRes);
                exiDoc->ACDP_DisconnectRes_isUsed = 1u;
                break;
            case 4:
                error = decode_iso20_acdp_ACDP_SystemStatusReqType(stream, &exiDoc->ACDP_SystemStatusReq);
                exiDoc->ACDP_SystemStatusReq_isUsed = 1u;
                break;
            case 5:
                error = decode_iso20_acdp_ACDP_SystemStatusResType(stream, &exiDoc->ACDP_SystemStatusRes);
                exiDoc->ACDP_SystemStatusRes_isUsed = 1u;
                break;
            case 6:
                error = decode_iso20_acdp_ACDP_VehiclePositioningReqType(stream, &exiDoc->ACDP_VehiclePositioningReq);
                exiDoc->ACDP_VehiclePositioningReq_isUsed = 1u;
                break;
            case 7:
                error = decode_iso20_acdp_ACDP_VehiclePositioningResType(stream, &exiDoc->ACDP_VehiclePositioningRes);
                exiDoc->ACDP_VehiclePositioningRes_isUsed = 1u;
                break;
            case 8:
                error = decode_iso20_acdp_CLReqControlModeType(stream, &exiDoc->CLReqControlMode);
                exiDoc->CLReqControlMode_isUsed = 1u;
                break;
            case 9:
                error = decode_iso20_acdp_CLResControlModeType(stream, &exiDoc->CLResControlMode);
                exiDoc->CLResControlMode_isUsed = 1u;
                break;
            case 10:
                error = decode_iso20_acdp_CanonicalizationMethodType(stream, &exiDoc->CanonicalizationMethod);
                exiDoc->CanonicalizationMethod_isUsed = 1u;
                break;
            case 11:
                error = decode_iso20_acdp_DSAKeyValueType(stream, &exiDoc->DSAKeyValue);
                exiDoc->DSAKeyValue_isUsed = 1u;
                break;
            case 12:
                error = decode_iso20_acdp_DigestMethodType(stream, &exiDoc->DigestMethod);
                exiDoc->DigestMethod_isUsed = 1u;
                break;
            case 13:
                // simple type! decode_iso20_acdp_DigestValue;
                break;
            case 14:
                error = decode_iso20_acdp_KeyInfoType(stream, &exiDoc->KeyInfo);
                exiDoc->KeyInfo_isUsed = 1u;
                break;
            case 15:
                // simple type! decode_iso20_acdp_KeyName;
                break;
            case 16:
                error = decode_iso20_acdp_KeyValueType(stream, &exiDoc->KeyValue);
                exiDoc->KeyValue_isUsed = 1u;
                break;
            case 17:
                error = decode_iso20_acdp_ManifestType(stream, &exiDoc->Manifest);
                exiDoc->Manifest_isUsed = 1u;
                break;
            case 18:
                // simple type! decode_iso20_acdp_MgmtData;
                break;
            case 19:
                error = decode_iso20_acdp_ObjectType(stream, &exiDoc->Object);
                exiDoc->Object_isUsed = 1u;
                break;
            case 20:
                error = decode_iso20_acdp_PGPDataType(stream, &exiDoc->PGPData);
                exiDoc->PGPData_isUsed = 1u;
                break;
            case 21:
                error = decode_iso20_acdp_RSAKeyValueType(stream, &exiDoc->RSAKeyValue);
                exiDoc->RSAKeyValue_isUsed = 1u;
                break;
            case 22:
                error = decode_iso20_acdp_ReferenceType(stream, &exiDoc->Reference);
                exiDoc->Reference_isUsed = 1u;
                break;
            case 23:
                error = decode_iso20_acdp_RetrievalMethodType(stream, &exiDoc->RetrievalMethod);
                exiDoc->RetrievalMethod_isUsed = 1u;
                break;
            case 24:
                error = decode_iso20_acdp_SPKIDataType(stream, &exiDoc->SPKIData);
                exiDoc->SPKIData_isUsed = 1u;
                break;
            case 25:
                error = decode_iso20_acdp_SignatureMethodType(stream, &exiDoc->SignatureMethod);
                exiDoc->SignatureMethod_isUsed = 1u;
                break;
            case 26:
                error = decode_iso20_acdp_SignaturePropertiesType(stream, &exiDoc->SignatureProperties);
                exiDoc->SignatureProperties_isUsed = 1u;
                break;
            case 27:
                error = decode_iso20_acdp_SignaturePropertyType(stream, &exiDoc->SignatureProperty);
                exiDoc->SignatureProperty_isUsed = 1u;
                break;
            case 28:
                error = decode_iso20_acdp_SignatureType(stream, &exiDoc->Signature);
                exiDoc->Signature_isUsed = 1u;
                break;
            case 29:
                error = decode_iso20_acdp_SignatureValueType(stream, &exiDoc->SignatureValue);
                exiDoc->SignatureValue_isUsed = 1u;
                break;
            case 30:
                error = decode_iso20_acdp_SignedInfoType(stream, &exiDoc->SignedInfo);
                exiDoc->SignedInfo_isUsed = 1u;
                break;
            case 31:
                error = decode_iso20_acdp_TransformType(stream, &exiDoc->Transform);
                exiDoc->Transform_isUsed = 1u;
                break;
            case 32:
                error = decode_iso20_acdp_TransformsType(stream, &exiDoc->Transforms);
                exiDoc->Transforms_isUsed = 1u;
                break;
            case 33:
                error = decode_iso20_acdp_X509DataType(stream, &exiDoc->X509Data);
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


