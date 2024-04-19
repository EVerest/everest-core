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
  * @file appHand_Decoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "common/exi_basetypes.h"
#include "common/exi_basetypes_decoder.h"
#include "common/exi_error_codes.h"
#include "common/exi_header.h"
#include "common/exi_types_decoder.h"
#include "appHand_Datatypes.h"



static int decode_appHand_AppProtocolType(exi_bitstream_t* stream, struct appHand_AppProtocolType* AppProtocolType);
static int decode_appHand_supportedAppProtocolReq(exi_bitstream_t* stream, struct appHand_supportedAppProtocolReq* supportedAppProtocolReq);
static int decode_appHand_supportedAppProtocolRes(exi_bitstream_t* stream, struct appHand_supportedAppProtocolRes* supportedAppProtocolRes);

// Element: definition=complex; name=AppProtocol; type={urn:iso:15118:2:2010:AppProtocol}AppProtocolType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProtocolNamespace, protocolNamespaceType (1, 1); VersionNumberMajor, unsignedInt (1, 1); VersionNumberMinor, unsignedInt (1, 1); SchemaID, idType (1, 1); Priority, priorityType (1, 1);
static int decode_appHand_AppProtocolType(exi_bitstream_t* stream, struct appHand_AppProtocolType* AppProtocolType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_appHand_AppProtocolType(AppProtocolType);

    while(!done)
    {
        switch(grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (ProtocolNamespace)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ProtocolNamespace, protocolNamespaceType (anyURI)); next=1
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &AppProtocolType->ProtocolNamespace.charactersLen);
                            if (error == 0)
                            {
                                if (AppProtocolType->ProtocolNamespace.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    AppProtocolType->ProtocolNamespace.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, AppProtocolType->ProtocolNamespace.charactersLen, AppProtocolType->ProtocolNamespace.characters, appHand_ProtocolNamespace_CHARACTER_SIZE);
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
                                grammar_id = 1;
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
        case 1:
            // Grammar: ID=1; read/write bits=1; START (VersionNumberMajor)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (VersionNumberMajor, unsignedInt (unsignedLong)); next=2
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &AppProtocolType->VersionNumberMajor);
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
            // Grammar: ID=2; read/write bits=1; START (VersionNumberMinor)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (VersionNumberMinor, unsignedInt (unsignedLong)); next=3
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &AppProtocolType->VersionNumberMinor);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; START (SchemaID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SchemaID, idType (unsignedByte)); next=4
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                AppProtocolType->SchemaID = (uint8_t)value;
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
                                grammar_id = 4;
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
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Priority)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (Priority, priorityType (unsignedByte)); next=5
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                AppProtocolType->Priority = (uint8_t)(value + 1);
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
                                grammar_id = 5;
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
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
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

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolReq; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AppProtocol, AppProtocolType (1, 5);
static int decode_appHand_supportedAppProtocolReq(exi_bitstream_t* stream, struct appHand_supportedAppProtocolReq* supportedAppProtocolReq) {
    int grammar_id = 7;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_appHand_supportedAppProtocolReq(supportedAppProtocolReq);

    while(!done)
    {
        switch(grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (AppProtocol)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=8
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 8;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (AppProtocol), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=9
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 9;
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=2; START (AppProtocol), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=10
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 10;
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=2; START (AppProtocol), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=11
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 11;
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (AppProtocol), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=12
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 12;
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=2; START (AppProtocol), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (AppProtocol, AppProtocolType (AppProtocolType)); next=5
                    // decode: element array
                    if (supportedAppProtocolReq->AppProtocol.arrayLen < appHand_AppProtocolType_5_ARRAY_SIZE)
                    {
                        error = decode_appHand_AppProtocolType(stream, &supportedAppProtocolReq->AppProtocol.array[supportedAppProtocolReq->AppProtocol.arrayLen++]);
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 5;
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
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

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolRes; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ResponseCode, responseCodeType (1, 1); SchemaID, idType (0, 1);
static int decode_appHand_supportedAppProtocolRes(exi_bitstream_t* stream, struct appHand_supportedAppProtocolRes* supportedAppProtocolRes) {
    int grammar_id = 13;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_appHand_supportedAppProtocolRes(supportedAppProtocolRes);

    while(!done)
    {
        switch(grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=14
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
                                supportedAppProtocolRes->ResponseCode = (appHand_responseCodeType)value;
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
            // Grammar: ID=14; read/write bits=2; START (SchemaID), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: START (SchemaID, idType (unsignedByte)); next=5
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                supportedAppProtocolRes->SchemaID = (uint8_t)value;
                                supportedAppProtocolRes->SchemaID_isUsed = 1u;
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
                                grammar_id = 5;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch(eventCode)
                {
                case 0:
                    // Event: END Element; next=6
                    done = 1;
                    grammar_id = 6;
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
int decode_appHand_exiDocument(exi_bitstream_t* stream, struct appHand_exiDocument* exiDoc) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == 0)
    {
        init_appHand_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
        if (error == 0)
        {
            switch(eventCode)
            {
            case 0:
                error = decode_appHand_supportedAppProtocolReq(stream, &exiDoc->supportedAppProtocolReq);
                exiDoc->supportedAppProtocolReq_isUsed = 1u;
                break;
            case 1:
                error = decode_appHand_supportedAppProtocolRes(stream, &exiDoc->supportedAppProtocolRes);
                exiDoc->supportedAppProtocolRes_isUsed = 1u;
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }
        }
    }

    return error;
}


