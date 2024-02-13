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
  * @file appHand_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef APP_HANDSHAKE_DATATYPES_H
#define APP_HANDSHAKE_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>



#define appHand_ProtocolNamespace_CHARACTER_SIZE (100)
#define appHand_AppProtocolType_5_ARRAY_SIZE (5)


// enum for function numbers
typedef enum {
    appHand_supportedAppProtocolReq = 0,
    appHand_supportedAppProtocolRes = 1
} appHand_generatedFunctionNumbersType;

// Element: definition=enum; name=ResponseCode; type={urn:iso:15118:2:2010:AppProtocol}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    appHand_responseCodeType_OK_SuccessfulNegotiation = 0,
    appHand_responseCodeType_OK_SuccessfulNegotiationWithMinorDeviation = 1,
    appHand_responseCodeType_Failed_NoNegotiation = 2
} appHand_responseCodeType;

// Element: definition=complex; name=AppProtocol; type={urn:iso:15118:2:2010:AppProtocol}AppProtocolType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProtocolNamespace, protocolNamespaceType (1, 1); VersionNumberMajor, unsignedInt (1, 1); VersionNumberMinor, unsignedInt (1, 1); SchemaID, idType (1, 1); Priority, priorityType (1, 1);
struct appHand_AppProtocolType {
    // ProtocolNamespace, protocolNamespaceType (base: anyURI)
    struct {
        char characters[appHand_ProtocolNamespace_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ProtocolNamespace;
    // VersionNumberMajor, unsignedInt (base: unsignedLong)
    uint32_t VersionNumberMajor;
    // VersionNumberMinor, unsignedInt (base: unsignedLong)
    uint32_t VersionNumberMinor;
    // SchemaID, idType (base: unsignedByte)
    uint8_t SchemaID;
    // Priority, priorityType (base: unsignedByte)
    uint8_t Priority;

};

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolReq; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AppProtocol, AppProtocolType (1, 5);
struct appHand_supportedAppProtocolReq {
    // AppProtocol, AppProtocolType
    struct {
        struct appHand_AppProtocolType array[appHand_AppProtocolType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } AppProtocol;
};

// Element: definition=complex; name={urn:iso:15118:2:2010:AppProtocol}supportedAppProtocolRes; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ResponseCode, responseCodeType (1, 1); SchemaID, idType (0, 1);
struct appHand_supportedAppProtocolRes {
    // ResponseCode, responseCodeType (base: string)
    appHand_responseCodeType ResponseCode;
    // SchemaID, idType (base: unsignedByte)
    uint8_t SchemaID;
    unsigned int SchemaID_isUsed:1;

};



// root elements of EXI doc
struct appHand_exiDocument {
    union {
        struct appHand_supportedAppProtocolReq supportedAppProtocolReq;
        struct appHand_supportedAppProtocolRes supportedAppProtocolRes;
    };
    unsigned int supportedAppProtocolReq_isUsed:1;
    unsigned int supportedAppProtocolRes_isUsed:1;
};

// init for structs
void init_appHand_exiDocument(struct appHand_exiDocument* exiDoc);
void init_appHand_supportedAppProtocolReq(struct appHand_supportedAppProtocolReq* supportedAppProtocolReq);
void init_appHand_supportedAppProtocolRes(struct appHand_supportedAppProtocolRes* supportedAppProtocolRes);
void init_appHand_AppProtocolType(struct appHand_AppProtocolType* AppProtocolType);


#ifdef __cplusplus
}
#endif

#endif /* APP_HANDSHAKE_DATATYPES_H */

