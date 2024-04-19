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
  * @file appHand_Encoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "common/exi_basetypes_encoder.h"
#include "common/exi_error_codes.h"
#include "common/exi_header.h"
#include "appHand_Datatypes.h"



static int encode_appHand_AppProtocolType(exi_bitstream_t* stream, const struct appHand_AppProtocolType* AppProtocolType);
static int encode_appHand_supportedAppProtocolReq(exi_bitstream_t* stream, const struct appHand_supportedAppProtocolReq* supportedAppProtocolReq);
static int encode_appHand_supportedAppProtocolRes(exi_bitstream_t* stream, const struct appHand_supportedAppProtocolRes* supportedAppProtocolRes);

// Element: definition=complex; name=AppProtocol; type={urn:iso:15118:2:2010:AppProtocol}AppProtocolType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProtocolNamespace, protocolNamespaceType (1, 1); VersionNumberMajor, unsignedInt (1, 1); VersionNumberMinor, unsignedInt (1, 1); SchemaID, idType (1, 1); Priority, priorityType (1, 1);
static int encode_appHand_AppProtocolType(exi_bitstream_t* stream, const struct appHand_AppProtocolType* AppProtocolType) {
    int grammar_id = 0;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (ProtocolNamespace)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=1

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AppProtocolType->ProtocolNamespace.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AppProtocolType->ProtocolNamespace.charactersLen, AppProtocolType->ProtocolNamespace.characters, appHand_ProtocolNamespace_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 1;
                            }
                        }
                    }
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=1; START (VersionNumberMajor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, AppProtocolType->VersionNumberMajor);
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
            // Grammar: ID=2; read/write bits=1; START (VersionNumberMinor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, AppProtocolType->VersionNumberMinor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; START (SchemaID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=4
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)AppProtocolType->SchemaID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 4;
                        }
                    }
                }
            }
            break;
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Priority)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=5
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, (uint32_t)AppProtocolType->Priority - 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 5;
                        }
                    }
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=6
                done = 1;
                grammar_id = 6;
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

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolReq; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AppProtocol, AppProtocolType (1, 5);
static int encode_appHand_supportedAppProtocolReq(exi_bitstream_t* stream, const struct appHand_supportedAppProtocolReq* supportedAppProtocolReq) {
    int grammar_id = 7;
    int done = 0;
    int error = 0;
    uint16_t AppProtocol_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (AppProtocol)
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=8
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 8;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (AppProtocol), END Element
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=9
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 9;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=2; START (AppProtocol), END Element
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=10
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 10;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=2; START (AppProtocol), END Element
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=11
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 11;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (AppProtocol), END Element
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=12
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 12;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=2; START (AppProtocol), END Element
            if (AppProtocol_currentIndex < supportedAppProtocolReq->AppProtocol.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AppProtocolType); next=5
                    error = encode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[AppProtocol_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 5;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=6
                done = 1;
                grammar_id = 6;
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

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolRes; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ResponseCode, responseCodeType (1, 1); SchemaID, idType (0, 1);
static int encode_appHand_supportedAppProtocolRes(exi_bitstream_t* stream, const struct appHand_supportedAppProtocolRes* supportedAppProtocolRes) {
    int grammar_id = 13;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=14
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, supportedAppProtocolRes->ResponseCode);
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
            break;
        case 14:
            // Grammar: ID=14; read/write bits=2; START (SchemaID), END Element
            if (supportedAppProtocolRes->SchemaID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SchemaID, unsignedByte); next=5
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)supportedAppProtocolRes->SchemaID);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 5;
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
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=6
                done = 1;
                grammar_id = 6;
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
int encode_appHand_exiDocument(exi_bitstream_t* stream, struct appHand_exiDocument* exiDoc)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        if (exiDoc->supportedAppProtocolReq_isUsed == 1)
        {
            // encode event 0
            error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_appHand_supportedAppProtocolReq(stream, &exiDoc->supportedAppProtocolReq);
            }
        }
        else if (exiDoc->supportedAppProtocolRes_isUsed == 1)
        {
            // encode event 1
            error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_appHand_supportedAppProtocolRes(stream, &exiDoc->supportedAppProtocolRes);
            }
        }
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }
    }

    return error;
}


