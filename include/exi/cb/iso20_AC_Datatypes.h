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
  * @file iso20_AC_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_AC_DATATYPES_H
#define ISO20_AC_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "common/exi_basetypes.h"



#define iso20_ac_Algorithm_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_anyType_BYTES_SIZE 4
#define iso20_ac_XPath_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_CryptoBinary_BYTES_SIZE EXI_BYTE_ARRAY_MAX_LEN
#define iso20_ac_X509IssuerName_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_Id_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_Type_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_URI_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_DigestValueType_BYTES_SIZE EXI_BYTE_ARRAY_MAX_LEN
#define iso20_ac_base64Binary_BYTES_SIZE EXI_BYTE_ARRAY_MAX_LEN
#define iso20_ac_X509SubjectName_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_ReferenceType_4_ARRAY_SIZE 4
#define iso20_ac_SignatureValueType_BYTES_SIZE EXI_BYTE_ARRAY_MAX_LEN
#define iso20_ac_KeyName_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_MgmtData_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_Encoding_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_MimeType_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_sessionIDType_BYTES_SIZE 8
#define iso20_ac_Target_CHARACTER_SIZE EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR
#define iso20_ac_MeterID_CHARACTER_SIZE 32 + ASCII_EXTRA_CHAR
#define iso20_ac_meterSignatureType_BYTES_SIZE 64
#define iso20_ac_DetailedTaxType_10_ARRAY_SIZE 10


// enum for function numbers
typedef enum {
    iso20_ac_AC_CPDReqEnergyTransferMode = 0,
    iso20_ac_AC_CPDResEnergyTransferMode = 1,
    iso20_ac_AC_ChargeLoopReq = 2,
    iso20_ac_AC_ChargeLoopRes = 3,
    iso20_ac_AC_ChargeParameterDiscoveryReq = 4,
    iso20_ac_AC_ChargeParameterDiscoveryRes = 5,
    iso20_ac_BPT_AC_CPDReqEnergyTransferMode = 6,
    iso20_ac_BPT_AC_CPDResEnergyTransferMode = 7,
    iso20_ac_BPT_Dynamic_AC_CLReqControlMode = 8,
    iso20_ac_BPT_Dynamic_AC_CLResControlMode = 9,
    iso20_ac_BPT_Scheduled_AC_CLReqControlMode = 10,
    iso20_ac_BPT_Scheduled_AC_CLResControlMode = 11,
    iso20_ac_CLReqControlMode = 12,
    iso20_ac_CLResControlMode = 13,
    iso20_ac_CanonicalizationMethod = 14,
    iso20_ac_DSAKeyValue = 15,
    iso20_ac_DigestMethod = 16,
    iso20_ac_DigestValue = 17,
    iso20_ac_Dynamic_AC_CLReqControlMode = 18,
    iso20_ac_Dynamic_AC_CLResControlMode = 19,
    iso20_ac_KeyInfo = 20,
    iso20_ac_KeyName = 21,
    iso20_ac_KeyValue = 22,
    iso20_ac_Manifest = 23,
    iso20_ac_MgmtData = 24,
    iso20_ac_Object = 25,
    iso20_ac_PGPData = 26,
    iso20_ac_RSAKeyValue = 27,
    iso20_ac_Reference = 28,
    iso20_ac_RetrievalMethod = 29,
    iso20_ac_SPKIData = 30,
    iso20_ac_Scheduled_AC_CLReqControlMode = 31,
    iso20_ac_Scheduled_AC_CLResControlMode = 32,
    iso20_ac_Signature = 33,
    iso20_ac_SignatureMethod = 34,
    iso20_ac_SignatureProperties = 35,
    iso20_ac_SignatureProperty = 36,
    iso20_ac_SignatureValue = 37,
    iso20_ac_SignedInfo = 38,
    iso20_ac_Transform = 39,
    iso20_ac_Transforms = 40,
    iso20_ac_X509Data = 41
} iso20_ac_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSENotification; type={urn:iso:std:iso:15118:-20:CommonTypes}evseNotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_evseNotificationType_Pause = 0,
    iso20_ac_evseNotificationType_ExitStandby = 1,
    iso20_ac_evseNotificationType_Terminate = 2,
    iso20_ac_evseNotificationType_ScheduleRenegotiation = 3,
    iso20_ac_evseNotificationType_ServiceRenegotiation = 4,
    iso20_ac_evseNotificationType_MeteringConfirmation = 5
} iso20_ac_evseNotificationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_responseCodeType_OK = 0,
    iso20_ac_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_ac_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_ac_responseCodeType_OK_OldSessionJoined = 3,
    iso20_ac_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_ac_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_ac_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_ac_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_ac_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_ac_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_ac_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_ac_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_ac_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_ac_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_ac_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_ac_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_ac_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_ac_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_ac_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_ac_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_ac_responseCodeType_WARNING_WPT = 20,
    iso20_ac_responseCodeType_FAILED = 21,
    iso20_ac_responseCodeType_FAILED_AssociationError = 22,
    iso20_ac_responseCodeType_FAILED_ContactorError = 23,
    iso20_ac_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_ac_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_ac_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_ac_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_ac_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_ac_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_ac_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_ac_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_ac_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_ac_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_ac_responseCodeType_FAILED_SequenceError = 34,
    iso20_ac_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_ac_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_ac_responseCodeType_FAILED_SignatureError = 37,
    iso20_ac_responseCodeType_FAILED_UnknownSession = 38,
    iso20_ac_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_ac_responseCodeType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_ac_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_ac_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1);
struct iso20_ac_TransformsType {
    // Transform, TransformType
    struct iso20_ac_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); Q, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']);
struct iso20_ac_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_ac_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_ac_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    int32_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_ac_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_ac_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    int32_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_ac_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_ac_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_ac_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_ac_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_ac_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_ac_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_ac_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_ac_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_ac_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_ac_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_ac_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_ac_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_ac_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_ac_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_ac_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_ac_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_2;
        unsigned int choice_2_isUsed:1;


    };
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SPKIData; type={http://www.w3.org/2000/09/xmldsig#}SPKIDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPKISexp, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_ac_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_ac_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4);
struct iso20_ac_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_ac_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_ac_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_ac_ReferenceType array[iso20_ac_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_ac_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_ac_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_ac_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_ac_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_ac_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_ac_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_ac_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_ac_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_ac_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1)(old 1, 1);
struct iso20_ac_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_ac_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_ac_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}EVMaximumChargePower; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_ac_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
struct iso20_ac_DetailedCostType {
    // Amount, RationalNumberType
    struct iso20_ac_RationalNumberType Amount;
    // CostPerUnit, RationalNumberType
    struct iso20_ac_RationalNumberType CostPerUnit;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1);
struct iso20_ac_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_ac_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_ac_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_ac_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_ac_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
struct iso20_ac_DetailedTaxType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // Amount, RationalNumberType
    struct iso20_ac_RationalNumberType Amount;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_ac_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_ac_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_ac_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_ac_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC}AC_CPDReqEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_AC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParameters; type={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParametersType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PresentSOC, percentValueType (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); MaximumSOC, percentValueType (0, 1); RemainingTimeToMinimumSOC, unsignedInt (0, 1); RemainingTimeToTargetSOC, unsignedInt (0, 1); RemainingTimeToMaximumSOC, unsignedInt (0, 1); ChargingComplete, boolean (0, 1); BatteryEnergyCapacity, RationalNumberType (0, 1); InletHot, boolean (0, 1);
struct iso20_ac_DisplayParametersType {
    // PresentSOC, percentValueType (base: byte)
    int8_t PresentSOC;
    unsigned int PresentSOC_isUsed:1;
    // MinimumSOC, percentValueType (base: byte)
    int8_t MinimumSOC;
    unsigned int MinimumSOC_isUsed:1;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // MaximumSOC, percentValueType (base: byte)
    int8_t MaximumSOC;
    unsigned int MaximumSOC_isUsed:1;
    // RemainingTimeToMinimumSOC, unsignedInt (base: unsignedLong)
    uint32_t RemainingTimeToMinimumSOC;
    unsigned int RemainingTimeToMinimumSOC_isUsed:1;
    // RemainingTimeToTargetSOC, unsignedInt (base: unsignedLong)
    uint32_t RemainingTimeToTargetSOC;
    unsigned int RemainingTimeToTargetSOC_isUsed:1;
    // RemainingTimeToMaximumSOC, unsignedInt (base: unsignedLong)
    uint32_t RemainingTimeToMaximumSOC;
    unsigned int RemainingTimeToMaximumSOC_isUsed:1;
    // ChargingComplete, boolean
    int ChargingComplete;
    unsigned int ChargingComplete_isUsed:1;
    // BatteryEnergyCapacity, RationalNumberType
    struct iso20_ac_RationalNumberType BatteryEnergyCapacity;
    unsigned int BatteryEnergyCapacity_isUsed:1;
    // InletHot, boolean
    int InletHot;
    unsigned int InletHot_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC}AC_CPDResEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower_L2, RationalNumberType (0, 1); EVSEMinimumChargePower_L3, RationalNumberType (0, 1); EVSENominalFrequency, RationalNumberType (1, 1); MaximumPowerAsymmetry, RationalNumberType (0, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_AC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower;
    // EVSEMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower_L2;
    unsigned int EVSEMinimumChargePower_L2_isUsed:1;
    // EVSEMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower_L3;
    unsigned int EVSEMinimumChargePower_L3_isUsed:1;
    // EVSENominalFrequency, RationalNumberType
    struct iso20_ac_RationalNumberType EVSENominalFrequency;
    // MaximumPowerAsymmetry, RationalNumberType
    struct iso20_ac_RationalNumberType MaximumPowerAsymmetry;
    unsigned int MaximumPowerAsymmetry_isUsed:1;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
struct iso20_ac_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, evseNotificationType (base: string)
    iso20_ac_evseNotificationType EVSENotification;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}Scheduled_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC}Scheduled_AC_CLReqControlModeType; base type=Scheduled_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (0, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_Scheduled_AC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower;
    unsigned int EVPresentReactivePower_isUsed:1;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}Dynamic_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC}Dynamic_AC_CLReqControlModeType; base type=Dynamic_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (1, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_Dynamic_AC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_ac_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
struct iso20_ac_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso20_ac_MeterID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MeterID;
    // ChargedEnergyReadingWh, unsignedLong (base: nonNegativeInteger)
    uint64_t ChargedEnergyReadingWh;
    // BPT_DischargedEnergyReadingWh, unsignedLong (base: nonNegativeInteger)
    uint64_t BPT_DischargedEnergyReadingWh;
    unsigned int BPT_DischargedEnergyReadingWh_isUsed:1;
    // CapacitiveEnergyReadingVARh, unsignedLong (base: nonNegativeInteger)
    uint64_t CapacitiveEnergyReadingVARh;
    unsigned int CapacitiveEnergyReadingVARh_isUsed:1;
    // BPT_InductiveEnergyReadingVARh, unsignedLong (base: nonNegativeInteger)
    uint64_t BPT_InductiveEnergyReadingVARh;
    unsigned int BPT_InductiveEnergyReadingVARh_isUsed:1;
    // MeterSignature, meterSignatureType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_meterSignatureType_BYTES_SIZE];
        uint16_t bytesLen;
    } MeterSignature;
    unsigned int MeterSignature_isUsed:1;

    // MeterStatus, short (base: int)
    int16_t MeterStatus;
    unsigned int MeterStatus_isUsed:1;
    // MeterTimestamp, unsignedLong (base: nonNegativeInteger)
    uint64_t MeterTimestamp;
    unsigned int MeterTimestamp_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
struct iso20_ac_ReceiptType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EnergyCosts, DetailedCostType
    struct iso20_ac_DetailedCostType EnergyCosts;
    unsigned int EnergyCosts_isUsed:1;
    // OccupancyCosts, DetailedCostType
    struct iso20_ac_DetailedCostType OccupancyCosts;
    unsigned int OccupancyCosts_isUsed:1;
    // AdditionalServicesCosts, DetailedCostType
    struct iso20_ac_DetailedCostType AdditionalServicesCosts;
    unsigned int AdditionalServicesCosts_isUsed:1;
    // OverstayCosts, DetailedCostType
    struct iso20_ac_DetailedCostType OverstayCosts;
    unsigned int OverstayCosts_isUsed:1;
    // TaxCosts, DetailedTaxType
    struct {
        struct iso20_ac_DetailedTaxType array[iso20_ac_DetailedTaxType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxCosts;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}Scheduled_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC}Scheduled_AC_CLResControlModeType; base type=Scheduled_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSETargetActivePower, RationalNumberType (0, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_Scheduled_AC_CLResControlModeType {
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower;
    unsigned int EVSETargetActivePower_isUsed:1;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}Dynamic_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC}Dynamic_AC_CLResControlModeType; base type=Dynamic_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSETargetActivePower, RationalNumberType (1, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_Dynamic_AC_CLResControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // MinimumSOC, percentValueType (base: byte)
    int8_t MinimumSOC;
    unsigned int MinimumSOC_isUsed:1;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // AckMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t AckMaxDelay;
    unsigned int AckMaxDelay_isUsed:1;
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_ac_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_AC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC}BPT_AC_CPDReqEnergyTransferModeType; base type=AC_CPDReqEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:AC}AC_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); AC_CPDReqEnergyTransferMode, AC_CPDReqEnergyTransferModeType (0, 1); BPT_AC_CPDReqEnergyTransferMode, BPT_AC_CPDReqEnergyTransferModeType (0, 1);
struct iso20_ac_AC_ChargeParameterDiscoveryReqType {
    // Header, MessageHeaderType
    struct iso20_ac_MessageHeaderType Header;
    // AC_CPDReqEnergyTransferMode, AC_CPDReqEnergyTransferModeType
    struct iso20_ac_AC_CPDReqEnergyTransferModeType AC_CPDReqEnergyTransferMode;
    unsigned int AC_CPDReqEnergyTransferMode_isUsed:1;
    // BPT_AC_CPDReqEnergyTransferMode, BPT_AC_CPDReqEnergyTransferModeType (base: AC_CPDReqEnergyTransferModeType)
    struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType BPT_AC_CPDReqEnergyTransferMode;
    unsigned int BPT_AC_CPDReqEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_AC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC}BPT_AC_CPDResEnergyTransferModeType; base type=AC_CPDResEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower_L2, RationalNumberType (0, 1); EVSEMinimumChargePower_L3, RationalNumberType (0, 1); EVSENominalFrequency, RationalNumberType (1, 1); MaximumPowerAsymmetry, RationalNumberType (0, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargePower_L2, RationalNumberType (0, 1); EVSEMaximumDischargePower_L3, RationalNumberType (0, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower_L2, RationalNumberType (0, 1); EVSEMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_BPT_AC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower;
    // EVSEMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower_L2;
    unsigned int EVSEMinimumChargePower_L2_isUsed:1;
    // EVSEMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumChargePower_L3;
    unsigned int EVSEMinimumChargePower_L3_isUsed:1;
    // EVSENominalFrequency, RationalNumberType
    struct iso20_ac_RationalNumberType EVSENominalFrequency;
    // MaximumPowerAsymmetry, RationalNumberType
    struct iso20_ac_RationalNumberType MaximumPowerAsymmetry;
    unsigned int MaximumPowerAsymmetry_isUsed:1;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumDischargePower_L2;
    unsigned int EVSEMaximumDischargePower_L2_isUsed:1;
    // EVSEMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMaximumDischargePower_L3;
    unsigned int EVSEMaximumDischargePower_L3_isUsed:1;
    // EVSEMinimumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumDischargePower;
    // EVSEMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumDischargePower_L2;
    unsigned int EVSEMinimumDischargePower_L2_isUsed:1;
    // EVSEMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEMinimumDischargePower_L3;
    unsigned int EVSEMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:AC}AC_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); AC_CPDResEnergyTransferMode, AC_CPDResEnergyTransferModeType (0, 1); BPT_AC_CPDResEnergyTransferMode, BPT_AC_CPDResEnergyTransferModeType (0, 1);
struct iso20_ac_AC_ChargeParameterDiscoveryResType {
    // Header, MessageHeaderType
    struct iso20_ac_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_ac_responseCodeType ResponseCode;
    // AC_CPDResEnergyTransferMode, AC_CPDResEnergyTransferModeType
    struct iso20_ac_AC_CPDResEnergyTransferModeType AC_CPDResEnergyTransferMode;
    unsigned int AC_CPDResEnergyTransferMode_isUsed:1;
    // BPT_AC_CPDResEnergyTransferMode, BPT_AC_CPDResEnergyTransferModeType (base: AC_CPDResEnergyTransferModeType)
    struct iso20_ac_BPT_AC_CPDResEnergyTransferModeType BPT_AC_CPDResEnergyTransferMode;
    unsigned int BPT_AC_CPDResEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_Scheduled_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC}BPT_Scheduled_AC_CLReqControlModeType; base type=Scheduled_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (0, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (0, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (0, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower;
    unsigned int EVPresentReactivePower_isUsed:1;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower;
    unsigned int EVMaximumDischargePower_isUsed:1;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower;
    unsigned int EVMinimumDischargePower_isUsed:1;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_Scheduled_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC}BPT_Scheduled_AC_CLResControlModeType; base type=Scheduled_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSETargetActivePower, RationalNumberType (0, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType {
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower;
    unsigned int EVSETargetActivePower_isUsed:1;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_Dynamic_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC}BPT_Dynamic_AC_CLReqControlModeType; base type=Dynamic_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (1, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;
    // EVMaximumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMaximumV2XEnergyRequest;
    unsigned int EVMaximumV2XEnergyRequest_isUsed:1;
    // EVMinimumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_RationalNumberType EVMinimumV2XEnergyRequest;
    unsigned int EVMinimumV2XEnergyRequest_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:AC}AC_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); BPT_Dynamic_AC_CLReqControlMode, BPT_Dynamic_AC_CLReqControlModeType (0, 1); BPT_Scheduled_AC_CLReqControlMode, BPT_Scheduled_AC_CLReqControlModeType (0, 1); CLReqControlMode, CLReqControlModeType (0, 1); Dynamic_AC_CLReqControlMode, Dynamic_AC_CLReqControlModeType (0, 1); Scheduled_AC_CLReqControlMode, Scheduled_AC_CLReqControlModeType (0, 1);
struct iso20_ac_AC_ChargeLoopReqType {
    // Header, MessageHeaderType
    struct iso20_ac_MessageHeaderType Header;
    // DisplayParameters, DisplayParametersType
    struct iso20_ac_DisplayParametersType DisplayParameters;
    unsigned int DisplayParameters_isUsed:1;
    // MeterInfoRequested, boolean
    int MeterInfoRequested;
    // BPT_Dynamic_AC_CLReqControlMode, BPT_Dynamic_AC_CLReqControlModeType (base: Dynamic_AC_CLReqControlModeType)
    struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType BPT_Dynamic_AC_CLReqControlMode;
    unsigned int BPT_Dynamic_AC_CLReqControlMode_isUsed:1;
    // BPT_Scheduled_AC_CLReqControlMode, BPT_Scheduled_AC_CLReqControlModeType (base: Scheduled_AC_CLReqControlModeType)
    struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType BPT_Scheduled_AC_CLReqControlMode;
    unsigned int BPT_Scheduled_AC_CLReqControlMode_isUsed:1;
    // CLReqControlMode, CLReqControlModeType
    struct iso20_ac_CLReqControlModeType CLReqControlMode;
    unsigned int CLReqControlMode_isUsed:1;
    // Dynamic_AC_CLReqControlMode, Dynamic_AC_CLReqControlModeType (base: Dynamic_CLReqControlModeType)
    struct iso20_ac_Dynamic_AC_CLReqControlModeType Dynamic_AC_CLReqControlMode;
    unsigned int Dynamic_AC_CLReqControlMode_isUsed:1;
    // Scheduled_AC_CLReqControlMode, Scheduled_AC_CLReqControlModeType (base: Scheduled_CLReqControlModeType)
    struct iso20_ac_Scheduled_AC_CLReqControlModeType Scheduled_AC_CLReqControlMode;
    unsigned int Scheduled_AC_CLReqControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}BPT_Dynamic_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC}BPT_Dynamic_AC_CLResControlModeType; base type=Dynamic_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSETargetActivePower, RationalNumberType (1, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // MinimumSOC, percentValueType (base: byte)
    int8_t MinimumSOC;
    unsigned int MinimumSOC_isUsed:1;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // AckMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t AckMaxDelay;
    unsigned int AckMaxDelay_isUsed:1;
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC}AC_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:AC}AC_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVSETargetFrequency, RationalNumberType (0, 1); BPT_Dynamic_AC_CLResControlMode, BPT_Dynamic_AC_CLResControlModeType (0, 1); BPT_Scheduled_AC_CLResControlMode, BPT_Scheduled_AC_CLResControlModeType (0, 1); CLResControlMode, CLResControlModeType (0, 1); Dynamic_AC_CLResControlMode, Dynamic_AC_CLResControlModeType (0, 1); Scheduled_AC_CLResControlMode, Scheduled_AC_CLResControlModeType (0, 1);
struct iso20_ac_AC_ChargeLoopResType {
    // Header, MessageHeaderType
    struct iso20_ac_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_ac_responseCodeType ResponseCode;
    // EVSEStatus, EVSEStatusType
    struct iso20_ac_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso20_ac_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // Receipt, ReceiptType
    struct iso20_ac_ReceiptType Receipt;
    unsigned int Receipt_isUsed:1;
    // EVSETargetFrequency, RationalNumberType
    struct iso20_ac_RationalNumberType EVSETargetFrequency;
    unsigned int EVSETargetFrequency_isUsed:1;
    // BPT_Dynamic_AC_CLResControlMode, BPT_Dynamic_AC_CLResControlModeType (base: Dynamic_AC_CLResControlModeType)
    struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType BPT_Dynamic_AC_CLResControlMode;
    unsigned int BPT_Dynamic_AC_CLResControlMode_isUsed:1;
    // BPT_Scheduled_AC_CLResControlMode, BPT_Scheduled_AC_CLResControlModeType (base: Scheduled_AC_CLResControlModeType)
    struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType BPT_Scheduled_AC_CLResControlMode;
    unsigned int BPT_Scheduled_AC_CLResControlMode_isUsed:1;
    // CLResControlMode, CLResControlModeType
    struct iso20_ac_CLResControlModeType CLResControlMode;
    unsigned int CLResControlMode_isUsed:1;
    // Dynamic_AC_CLResControlMode, Dynamic_AC_CLResControlModeType (base: Dynamic_CLResControlModeType)
    struct iso20_ac_Dynamic_AC_CLResControlModeType Dynamic_AC_CLResControlMode;
    unsigned int Dynamic_AC_CLResControlMode_isUsed:1;
    // Scheduled_AC_CLResControlMode, Scheduled_AC_CLResControlModeType (base: Scheduled_CLResControlModeType)
    struct iso20_ac_Scheduled_AC_CLResControlModeType Scheduled_AC_CLResControlMode;
    unsigned int Scheduled_AC_CLResControlMode_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4);
struct iso20_ac_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_ac_ReferenceType array[iso20_ac_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1);
struct iso20_ac_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_ac_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_ac_exiDocument {
    union {
        struct iso20_ac_AC_ChargeParameterDiscoveryReqType AC_ChargeParameterDiscoveryReq;
        struct iso20_ac_AC_ChargeParameterDiscoveryResType AC_ChargeParameterDiscoveryRes;
        struct iso20_ac_AC_ChargeLoopReqType AC_ChargeLoopReq;
        struct iso20_ac_AC_ChargeLoopResType AC_ChargeLoopRes;
        struct iso20_ac_AC_CPDReqEnergyTransferModeType AC_CPDReqEnergyTransferMode;
        struct iso20_ac_AC_CPDResEnergyTransferModeType AC_CPDResEnergyTransferMode;
        struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType BPT_AC_CPDReqEnergyTransferMode;
        struct iso20_ac_BPT_AC_CPDResEnergyTransferModeType BPT_AC_CPDResEnergyTransferMode;
        struct iso20_ac_Scheduled_AC_CLReqControlModeType Scheduled_AC_CLReqControlMode;
        struct iso20_ac_Scheduled_AC_CLResControlModeType Scheduled_AC_CLResControlMode;
        struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType BPT_Scheduled_AC_CLReqControlMode;
        struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType BPT_Scheduled_AC_CLResControlMode;
        struct iso20_ac_Dynamic_AC_CLReqControlModeType Dynamic_AC_CLReqControlMode;
        struct iso20_ac_Dynamic_AC_CLResControlModeType Dynamic_AC_CLResControlMode;
        struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType BPT_Dynamic_AC_CLReqControlMode;
        struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType BPT_Dynamic_AC_CLResControlMode;
        struct iso20_ac_CLReqControlModeType CLReqControlMode;
        struct iso20_ac_CLResControlModeType CLResControlMode;
        struct iso20_ac_SignatureType Signature;
        struct iso20_ac_SignatureValueType SignatureValue;
        struct iso20_ac_SignedInfoType SignedInfo;
        struct iso20_ac_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_ac_SignatureMethodType SignatureMethod;
        struct iso20_ac_ReferenceType Reference;
        struct iso20_ac_TransformsType Transforms;
        struct iso20_ac_TransformType Transform;
        struct iso20_ac_DigestMethodType DigestMethod;
        struct iso20_ac_KeyInfoType KeyInfo;
        struct iso20_ac_KeyValueType KeyValue;
        struct iso20_ac_RetrievalMethodType RetrievalMethod;
        struct iso20_ac_X509DataType X509Data;
        struct iso20_ac_PGPDataType PGPData;
        struct iso20_ac_SPKIDataType SPKIData;
        struct iso20_ac_ObjectType Object;
        struct iso20_ac_ManifestType Manifest;
        struct iso20_ac_SignaturePropertiesType SignatureProperties;
        struct iso20_ac_SignaturePropertyType SignatureProperty;
        struct iso20_ac_DSAKeyValueType DSAKeyValue;
        struct iso20_ac_RSAKeyValueType RSAKeyValue;
    };
    unsigned int AC_ChargeParameterDiscoveryReq_isUsed:1;
    unsigned int AC_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int AC_ChargeLoopReq_isUsed:1;
    unsigned int AC_ChargeLoopRes_isUsed:1;
    unsigned int AC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int AC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int BPT_AC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int BPT_AC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int Scheduled_AC_CLReqControlMode_isUsed:1;
    unsigned int Scheduled_AC_CLResControlMode_isUsed:1;
    unsigned int BPT_Scheduled_AC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Scheduled_AC_CLResControlMode_isUsed:1;
    unsigned int Dynamic_AC_CLReqControlMode_isUsed:1;
    unsigned int Dynamic_AC_CLResControlMode_isUsed:1;
    unsigned int BPT_Dynamic_AC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Dynamic_AC_CLResControlMode_isUsed:1;
    unsigned int CLReqControlMode_isUsed:1;
    unsigned int CLResControlMode_isUsed:1;
    unsigned int Signature_isUsed:1;
    unsigned int SignatureValue_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
    unsigned int CanonicalizationMethod_isUsed:1;
    unsigned int SignatureMethod_isUsed:1;
    unsigned int Reference_isUsed:1;
    unsigned int Transforms_isUsed:1;
    unsigned int Transform_isUsed:1;
    unsigned int DigestMethod_isUsed:1;
    unsigned int KeyInfo_isUsed:1;
    unsigned int KeyValue_isUsed:1;
    unsigned int RetrievalMethod_isUsed:1;
    unsigned int X509Data_isUsed:1;
    unsigned int PGPData_isUsed:1;
    unsigned int SPKIData_isUsed:1;
    unsigned int Object_isUsed:1;
    unsigned int Manifest_isUsed:1;
    unsigned int SignatureProperties_isUsed:1;
    unsigned int SignatureProperty_isUsed:1;
    unsigned int DSAKeyValue_isUsed:1;
    unsigned int RSAKeyValue_isUsed:1;
};

// elements of EXI fragment
struct iso20_ac_exiFragment {
    union {
        struct iso20_ac_AC_ChargeParameterDiscoveryResType AC_ChargeParameterDiscoveryRes;
        struct iso20_ac_SignedInfoType SignedInfo;
    };
    unsigned int AC_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
};

// elements of xmldsig fragment
struct iso20_ac_xmldsigFragment {
    union {
        struct iso20_ac_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_ac_DSAKeyValueType DSAKeyValue;
        struct iso20_ac_DigestMethodType DigestMethod;
        struct iso20_ac_KeyInfoType KeyInfo;
        struct iso20_ac_KeyValueType KeyValue;
        struct iso20_ac_ManifestType Manifest;
        struct iso20_ac_ObjectType Object;
        struct iso20_ac_PGPDataType PGPData;
        struct iso20_ac_RSAKeyValueType RSAKeyValue;
        struct iso20_ac_ReferenceType Reference;
        struct iso20_ac_RetrievalMethodType RetrievalMethod;
        struct iso20_ac_SPKIDataType SPKIData;
        struct iso20_ac_SignatureType Signature;
        struct iso20_ac_SignatureMethodType SignatureMethod;
        struct iso20_ac_SignaturePropertiesType SignatureProperties;
        struct iso20_ac_SignaturePropertyType SignatureProperty;
        struct iso20_ac_SignatureValueType SignatureValue;
        struct iso20_ac_SignedInfoType SignedInfo;
        struct iso20_ac_TransformType Transform;
        struct iso20_ac_TransformsType Transforms;
        struct iso20_ac_X509DataType X509Data;
        struct iso20_ac_X509IssuerSerialType X509IssuerSerial;
    };
    unsigned int CanonicalizationMethod_isUsed:1;
    unsigned int DSAKeyValue_isUsed:1;
    unsigned int DigestMethod_isUsed:1;
    unsigned int KeyInfo_isUsed:1;
    unsigned int KeyValue_isUsed:1;
    unsigned int Manifest_isUsed:1;
    unsigned int Object_isUsed:1;
    unsigned int PGPData_isUsed:1;
    unsigned int RSAKeyValue_isUsed:1;
    unsigned int Reference_isUsed:1;
    unsigned int RetrievalMethod_isUsed:1;
    unsigned int SPKIData_isUsed:1;
    unsigned int Signature_isUsed:1;
    unsigned int SignatureMethod_isUsed:1;
    unsigned int SignatureProperties_isUsed:1;
    unsigned int SignatureProperty_isUsed:1;
    unsigned int SignatureValue_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
    unsigned int Transform_isUsed:1;
    unsigned int Transforms_isUsed:1;
    unsigned int X509Data_isUsed:1;
    unsigned int X509IssuerSerial_isUsed:1;
};

// init for structs
void init_iso20_ac_exiDocument(struct iso20_ac_exiDocument* exiDoc);
void init_iso20_ac_AC_ChargeParameterDiscoveryReqType(struct iso20_ac_AC_ChargeParameterDiscoveryReqType* AC_ChargeParameterDiscoveryReq);
void init_iso20_ac_AC_ChargeParameterDiscoveryResType(struct iso20_ac_AC_ChargeParameterDiscoveryResType* AC_ChargeParameterDiscoveryRes);
void init_iso20_ac_AC_ChargeLoopReqType(struct iso20_ac_AC_ChargeLoopReqType* AC_ChargeLoopReq);
void init_iso20_ac_AC_ChargeLoopResType(struct iso20_ac_AC_ChargeLoopResType* AC_ChargeLoopRes);
void init_iso20_ac_AC_CPDReqEnergyTransferModeType(struct iso20_ac_AC_CPDReqEnergyTransferModeType* AC_CPDReqEnergyTransferMode);
void init_iso20_ac_AC_CPDResEnergyTransferModeType(struct iso20_ac_AC_CPDResEnergyTransferModeType* AC_CPDResEnergyTransferMode);
void init_iso20_ac_BPT_AC_CPDReqEnergyTransferModeType(struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType* BPT_AC_CPDReqEnergyTransferMode);
void init_iso20_ac_BPT_AC_CPDResEnergyTransferModeType(struct iso20_ac_BPT_AC_CPDResEnergyTransferModeType* BPT_AC_CPDResEnergyTransferMode);
void init_iso20_ac_Scheduled_AC_CLReqControlModeType(struct iso20_ac_Scheduled_AC_CLReqControlModeType* Scheduled_AC_CLReqControlMode);
void init_iso20_ac_Scheduled_AC_CLResControlModeType(struct iso20_ac_Scheduled_AC_CLResControlModeType* Scheduled_AC_CLResControlMode);
void init_iso20_ac_BPT_Scheduled_AC_CLReqControlModeType(struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType* BPT_Scheduled_AC_CLReqControlMode);
void init_iso20_ac_BPT_Scheduled_AC_CLResControlModeType(struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType* BPT_Scheduled_AC_CLResControlMode);
void init_iso20_ac_Dynamic_AC_CLReqControlModeType(struct iso20_ac_Dynamic_AC_CLReqControlModeType* Dynamic_AC_CLReqControlMode);
void init_iso20_ac_Dynamic_AC_CLResControlModeType(struct iso20_ac_Dynamic_AC_CLResControlModeType* Dynamic_AC_CLResControlMode);
void init_iso20_ac_BPT_Dynamic_AC_CLReqControlModeType(struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType* BPT_Dynamic_AC_CLReqControlMode);
void init_iso20_ac_BPT_Dynamic_AC_CLResControlModeType(struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType* BPT_Dynamic_AC_CLResControlMode);
void init_iso20_ac_CLReqControlModeType(struct iso20_ac_CLReqControlModeType* CLReqControlMode);
void init_iso20_ac_CLResControlModeType(struct iso20_ac_CLResControlModeType* CLResControlMode);
void init_iso20_ac_SignatureType(struct iso20_ac_SignatureType* Signature);
void init_iso20_ac_SignatureValueType(struct iso20_ac_SignatureValueType* SignatureValue);
void init_iso20_ac_SignedInfoType(struct iso20_ac_SignedInfoType* SignedInfo);
void init_iso20_ac_CanonicalizationMethodType(struct iso20_ac_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_ac_SignatureMethodType(struct iso20_ac_SignatureMethodType* SignatureMethod);
void init_iso20_ac_ReferenceType(struct iso20_ac_ReferenceType* Reference);
void init_iso20_ac_TransformsType(struct iso20_ac_TransformsType* Transforms);
void init_iso20_ac_TransformType(struct iso20_ac_TransformType* Transform);
void init_iso20_ac_DigestMethodType(struct iso20_ac_DigestMethodType* DigestMethod);
void init_iso20_ac_KeyInfoType(struct iso20_ac_KeyInfoType* KeyInfo);
void init_iso20_ac_KeyValueType(struct iso20_ac_KeyValueType* KeyValue);
void init_iso20_ac_RetrievalMethodType(struct iso20_ac_RetrievalMethodType* RetrievalMethod);
void init_iso20_ac_X509DataType(struct iso20_ac_X509DataType* X509Data);
void init_iso20_ac_PGPDataType(struct iso20_ac_PGPDataType* PGPData);
void init_iso20_ac_SPKIDataType(struct iso20_ac_SPKIDataType* SPKIData);
void init_iso20_ac_ObjectType(struct iso20_ac_ObjectType* Object);
void init_iso20_ac_ManifestType(struct iso20_ac_ManifestType* Manifest);
void init_iso20_ac_SignaturePropertiesType(struct iso20_ac_SignaturePropertiesType* SignatureProperties);
void init_iso20_ac_SignaturePropertyType(struct iso20_ac_SignaturePropertyType* SignatureProperty);
void init_iso20_ac_DSAKeyValueType(struct iso20_ac_DSAKeyValueType* DSAKeyValue);
void init_iso20_ac_RSAKeyValueType(struct iso20_ac_RSAKeyValueType* RSAKeyValue);
void init_iso20_ac_X509IssuerSerialType(struct iso20_ac_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_ac_RationalNumberType(struct iso20_ac_RationalNumberType* RationalNumberType);
void init_iso20_ac_DetailedCostType(struct iso20_ac_DetailedCostType* DetailedCostType);
void init_iso20_ac_DetailedTaxType(struct iso20_ac_DetailedTaxType* DetailedTaxType);
void init_iso20_ac_MessageHeaderType(struct iso20_ac_MessageHeaderType* MessageHeaderType);
void init_iso20_ac_DisplayParametersType(struct iso20_ac_DisplayParametersType* DisplayParametersType);
void init_iso20_ac_EVSEStatusType(struct iso20_ac_EVSEStatusType* EVSEStatusType);
void init_iso20_ac_MeterInfoType(struct iso20_ac_MeterInfoType* MeterInfoType);
void init_iso20_ac_ReceiptType(struct iso20_ac_ReceiptType* ReceiptType);
void init_iso20_ac_exiFragment(struct iso20_ac_exiFragment* exiFrag);
void init_iso20_ac_xmldsigFragment(struct iso20_ac_xmldsigFragment* xmldsigFrag);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_AC_DATATYPES_H */

