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
  * @file iso20_DC_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_DC_DATATYPES_H
#define ISO20_DC_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso20_dc_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_anyType_BYTES_SIZE (4)
#define iso20_dc_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_dc_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_dc_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_dc_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_ReferenceType_4_ARRAY_SIZE (4)
#define iso20_dc_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_dc_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_sessionIDType_BYTES_SIZE (8)
#define iso20_dc_Target_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_dc_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso20_dc_meterSignatureType_BYTES_SIZE (64)
#define iso20_dc_DetailedTaxType_10_ARRAY_SIZE (10)


// enum for function numbers
typedef enum {
    iso20_dc_BPT_DC_CPDReqEnergyTransferMode = 0,
    iso20_dc_BPT_DC_CPDResEnergyTransferMode = 1,
    iso20_dc_BPT_Dynamic_DC_CLReqControlMode = 2,
    iso20_dc_BPT_Dynamic_DC_CLResControlMode = 3,
    iso20_dc_BPT_Scheduled_DC_CLReqControlMode = 4,
    iso20_dc_BPT_Scheduled_DC_CLResControlMode = 5,
    iso20_dc_CLReqControlMode = 6,
    iso20_dc_CLResControlMode = 7,
    iso20_dc_CanonicalizationMethod = 8,
    iso20_dc_DC_CPDReqEnergyTransferMode = 9,
    iso20_dc_DC_CPDResEnergyTransferMode = 10,
    iso20_dc_DC_CableCheckReq = 11,
    iso20_dc_DC_CableCheckRes = 12,
    iso20_dc_DC_ChargeLoopReq = 13,
    iso20_dc_DC_ChargeLoopRes = 14,
    iso20_dc_DC_ChargeParameterDiscoveryReq = 15,
    iso20_dc_DC_ChargeParameterDiscoveryRes = 16,
    iso20_dc_DC_PreChargeReq = 17,
    iso20_dc_DC_PreChargeRes = 18,
    iso20_dc_DC_WeldingDetectionReq = 19,
    iso20_dc_DC_WeldingDetectionRes = 20,
    iso20_dc_DSAKeyValue = 21,
    iso20_dc_DigestMethod = 22,
    iso20_dc_DigestValue = 23,
    iso20_dc_Dynamic_DC_CLReqControlMode = 24,
    iso20_dc_Dynamic_DC_CLResControlMode = 25,
    iso20_dc_KeyInfo = 26,
    iso20_dc_KeyName = 27,
    iso20_dc_KeyValue = 28,
    iso20_dc_Manifest = 29,
    iso20_dc_MgmtData = 30,
    iso20_dc_Object = 31,
    iso20_dc_PGPData = 32,
    iso20_dc_RSAKeyValue = 33,
    iso20_dc_Reference = 34,
    iso20_dc_RetrievalMethod = 35,
    iso20_dc_SPKIData = 36,
    iso20_dc_Scheduled_DC_CLReqControlMode = 37,
    iso20_dc_Scheduled_DC_CLResControlMode = 38,
    iso20_dc_Signature = 39,
    iso20_dc_SignatureMethod = 40,
    iso20_dc_SignatureProperties = 41,
    iso20_dc_SignatureProperty = 42,
    iso20_dc_SignatureValue = 43,
    iso20_dc_SignedInfo = 44,
    iso20_dc_Transform = 45,
    iso20_dc_Transforms = 46,
    iso20_dc_X509Data = 47
} iso20_dc_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSENotification; type={urn:iso:std:iso:15118:-20:CommonTypes}evseNotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_dc_evseNotificationType_Pause = 0,
    iso20_dc_evseNotificationType_ExitStandby = 1,
    iso20_dc_evseNotificationType_Terminate = 2,
    iso20_dc_evseNotificationType_ScheduleRenegotiation = 3,
    iso20_dc_evseNotificationType_ServiceRenegotiation = 4,
    iso20_dc_evseNotificationType_MeteringConfirmation = 5
} iso20_dc_evseNotificationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_dc_responseCodeType_OK = 0,
    iso20_dc_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_dc_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_dc_responseCodeType_OK_OldSessionJoined = 3,
    iso20_dc_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_dc_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_dc_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_dc_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_dc_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_dc_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_dc_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_dc_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_dc_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_dc_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_dc_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_dc_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_dc_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_dc_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_dc_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_dc_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_dc_responseCodeType_WARNING_WPT = 20,
    iso20_dc_responseCodeType_FAILED = 21,
    iso20_dc_responseCodeType_FAILED_AssociationError = 22,
    iso20_dc_responseCodeType_FAILED_ContactorError = 23,
    iso20_dc_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_dc_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_dc_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_dc_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_dc_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_dc_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_dc_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_dc_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_dc_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_dc_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_dc_responseCodeType_FAILED_SequenceError = 34,
    iso20_dc_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_dc_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_dc_responseCodeType_FAILED_SignatureError = 37,
    iso20_dc_responseCodeType_FAILED_UnknownSession = 38,
    iso20_dc_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_dc_responseCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:DC}EVSEProcessing; type={urn:iso:std:iso:15118:-20:CommonTypes}processingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_dc_processingType_Finished = 0,
    iso20_dc_processingType_Ongoing = 1,
    iso20_dc_processingType_Ongoing_WaitingForCustomerInteraction = 2
} iso20_dc_processingType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_dc_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_dc_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_dc_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso20_dc_TransformsType {
    // Transform, TransformType
    struct iso20_dc_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso20_dc_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_dc_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_dc_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_dc_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_dc_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_dc_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_dc_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_dc_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_dc_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_dc_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_dc_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_dc_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_dc_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_dc_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_dc_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_dc_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_dc_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_dc_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_dc_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_dc_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_dc_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_dc_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_dc_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_dc_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_dc_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_dc_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
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
struct iso20_dc_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_dc_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_dc_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_dc_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_dc_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_dc_ReferenceType array[iso20_dc_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_dc_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_dc_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_dc_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_dc_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_dc_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_dc_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_dc_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_dc_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_dc_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso20_dc_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_dc_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_dc_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}EVMaximumChargePower; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_dc_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
struct iso20_dc_DetailedCostType {
    // Amount, RationalNumberType
    struct iso20_dc_RationalNumberType Amount;
    // CostPerUnit, RationalNumberType
    struct iso20_dc_RationalNumberType CostPerUnit;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso20_dc_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_dc_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_dc_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_dc_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_dc_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
struct iso20_dc_DetailedTaxType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // Amount, RationalNumberType
    struct iso20_dc_RationalNumberType Amount;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_dc_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_dc_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_dc_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_dc_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_dc_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dc_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}DC_CPDReqEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMinimumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); TargetSOC, percentValueType (0, 1);
struct iso20_dc_DC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    // EVMinimumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargeCurrent;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParameters; type={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParametersType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PresentSOC, percentValueType (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); MaximumSOC, percentValueType (0, 1); RemainingTimeToMinimumSOC, unsignedInt (0, 1); RemainingTimeToTargetSOC, unsignedInt (0, 1); RemainingTimeToMaximumSOC, unsignedInt (0, 1); ChargingComplete, boolean (0, 1); BatteryEnergyCapacity, RationalNumberType (0, 1); InletHot, boolean (0, 1);
struct iso20_dc_DisplayParametersType {
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
    struct iso20_dc_RationalNumberType BatteryEnergyCapacity;
    unsigned int BatteryEnergyCapacity_isUsed:1;
    // InletHot, boolean
    int InletHot;
    unsigned int InletHot_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}DC_CPDResEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMinimumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1);
struct iso20_dc_DC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    // EVSEMinimumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargeCurrent;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;
    // EVSEMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumVoltage;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
struct iso20_dc_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, evseNotificationType (base: string)
    iso20_dc_evseNotificationType EVSENotification;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
struct iso20_dc_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso20_dc_MeterID_CHARACTER_SIZE];
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
        uint8_t bytes[iso20_dc_meterSignatureType_BYTES_SIZE];
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLReqControlModeType; base type=Dynamic_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1);
struct iso20_dc_Dynamic_DC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLReqControlModeType; base type=Scheduled_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVTargetCurrent, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMaximumChargeCurrent, RationalNumberType (0, 1); EVMaximumVoltage, RationalNumberType (0, 1); EVMinimumVoltage, RationalNumberType (0, 1);
struct iso20_dc_Scheduled_DC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVTargetCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetCurrent;
    // EVTargetVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetVoltage;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    unsigned int EVMaximumChargeCurrent_isUsed:1;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    unsigned int EVMaximumVoltage_isUsed:1;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;
    unsigned int EVMinimumVoltage_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_dc_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
struct iso20_dc_ReceiptType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EnergyCosts, DetailedCostType
    struct iso20_dc_DetailedCostType EnergyCosts;
    unsigned int EnergyCosts_isUsed:1;
    // OccupancyCosts, DetailedCostType
    struct iso20_dc_DetailedCostType OccupancyCosts;
    unsigned int OccupancyCosts_isUsed:1;
    // AdditionalServicesCosts, DetailedCostType
    struct iso20_dc_DetailedCostType AdditionalServicesCosts;
    unsigned int AdditionalServicesCosts_isUsed:1;
    // OverstayCosts, DetailedCostType
    struct iso20_dc_DetailedCostType OverstayCosts;
    unsigned int OverstayCosts_isUsed:1;
    // TaxCosts, DetailedTaxType
    struct {
        struct iso20_dc_DetailedTaxType array[iso20_dc_DetailedTaxType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxCosts;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}Dynamic_DC_CLResControlModeType; base type=Dynamic_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1);
struct iso20_dc_Dynamic_DC_CLResControlModeType {
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
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}Scheduled_DC_CLResControlModeType; base type=Scheduled_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (0, 1); EVSEMaximumChargeCurrent, RationalNumberType (0, 1); EVSEMaximumVoltage, RationalNumberType (0, 1);
struct iso20_dc_Scheduled_DC_CLResControlModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    unsigned int EVSEMaximumChargePower_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    unsigned int EVSEMinimumChargePower_isUsed:1;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    unsigned int EVSEMaximumChargeCurrent_isUsed:1;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;
    unsigned int EVSEMaximumVoltage_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_dc_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CableCheckReq; type={urn:iso:std:iso:15118:-20:DC}DC_CableCheckReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1);
struct iso20_dc_DC_CableCheckReqType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_CableCheckRes; type={urn:iso:std:iso:15118:-20:DC}DC_CableCheckResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1);
struct iso20_dc_DC_CableCheckResType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_dc_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_dc_processingType EVSEProcessing;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_PreChargeReq; type={urn:iso:std:iso:15118:-20:DC}DC_PreChargeReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVPresentVoltage, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1);
struct iso20_dc_DC_PreChargeReqType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_dc_processingType EVProcessing;
    // EVPresentVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVPresentVoltage;
    // EVTargetVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_PreChargeRes; type={urn:iso:std:iso:15118:-20:DC}DC_PreChargeResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1);
struct iso20_dc_DC_PreChargeResType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_dc_responseCodeType ResponseCode;
    // EVSEPresentVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionReq; type={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1);
struct iso20_dc_DC_WeldingDetectionReqType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_dc_processingType EVProcessing;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionRes; type={urn:iso:std:iso:15118:-20:DC}DC_WeldingDetectionResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1);
struct iso20_dc_DC_WeldingDetectionResType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_dc_responseCodeType ResponseCode;
    // EVSEPresentVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDReqEnergyTransferModeType; base type=DC_CPDReqEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMinimumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); TargetSOC, percentValueType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMaximumDischargeCurrent, RationalNumberType (1, 1); EVMinimumDischargeCurrent, RationalNumberType (1, 1);
struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    // EVMinimumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargeCurrent;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargePower;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumDischargePower;
    // EVMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargeCurrent;
    // EVMinimumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumDischargeCurrent;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); BPT_DC_CPDReqEnergyTransferMode, BPT_DC_CPDReqEnergyTransferModeType (0, 1); DC_CPDReqEnergyTransferMode, DC_CPDReqEnergyTransferModeType (0, 1);
struct iso20_dc_DC_ChargeParameterDiscoveryReqType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // BPT_DC_CPDReqEnergyTransferMode, BPT_DC_CPDReqEnergyTransferModeType (base: DC_CPDReqEnergyTransferModeType)
    struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType BPT_DC_CPDReqEnergyTransferMode;
    unsigned int BPT_DC_CPDReqEnergyTransferMode_isUsed:1;
    // DC_CPDReqEnergyTransferMode, DC_CPDReqEnergyTransferModeType
    struct iso20_dc_DC_CPDReqEnergyTransferModeType DC_CPDReqEnergyTransferMode;
    unsigned int DC_CPDReqEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:DC}BPT_DC_CPDResEnergyTransferModeType; base type=DC_CPDResEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMinimumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargeCurrent, RationalNumberType (1, 1); EVSEMinimumDischargeCurrent, RationalNumberType (1, 1);
struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    // EVSEMinimumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargeCurrent;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;
    // EVSEMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumVoltage;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumDischargePower;
    // EVSEMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargeCurrent;
    // EVSEMinimumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumDischargeCurrent;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); BPT_DC_CPDResEnergyTransferMode, BPT_DC_CPDResEnergyTransferModeType (0, 1); DC_CPDResEnergyTransferMode, DC_CPDResEnergyTransferModeType (0, 1);
struct iso20_dc_DC_ChargeParameterDiscoveryResType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_dc_responseCodeType ResponseCode;
    // BPT_DC_CPDResEnergyTransferMode, BPT_DC_CPDResEnergyTransferModeType (base: DC_CPDResEnergyTransferModeType)
    struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType BPT_DC_CPDResEnergyTransferMode;
    unsigned int BPT_DC_CPDResEnergyTransferMode_isUsed:1;
    // DC_CPDResEnergyTransferMode, DC_CPDResEnergyTransferModeType
    struct iso20_dc_DC_CPDResEnergyTransferModeType DC_CPDResEnergyTransferMode;
    unsigned int DC_CPDResEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLReqControlModeType; base type=Scheduled_DC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVTargetCurrent, RationalNumberType (1, 1); EVTargetVoltage, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMaximumChargeCurrent, RationalNumberType (0, 1); EVMaximumVoltage, RationalNumberType (0, 1); EVMinimumVoltage, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (0, 1); EVMaximumDischargeCurrent, RationalNumberType (0, 1);
struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVTargetCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetCurrent;
    // EVTargetVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetVoltage;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    unsigned int EVMaximumChargeCurrent_isUsed:1;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    unsigned int EVMaximumVoltage_isUsed:1;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;
    unsigned int EVMinimumVoltage_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargePower;
    unsigned int EVMaximumDischargePower_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumDischargePower;
    unsigned int EVMinimumDischargePower_isUsed:1;
    // EVMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargeCurrent;
    unsigned int EVMaximumDischargeCurrent_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Scheduled_DC_CLResControlModeType; base type=Scheduled_DC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (0, 1); EVSEMaximumChargeCurrent, RationalNumberType (0, 1); EVSEMaximumVoltage, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (0, 1); EVSEMinimumDischargePower, RationalNumberType (0, 1); EVSEMaximumDischargeCurrent, RationalNumberType (0, 1); EVSEMinimumVoltage, RationalNumberType (0, 1);
struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    unsigned int EVSEMaximumChargePower_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    unsigned int EVSEMinimumChargePower_isUsed:1;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    unsigned int EVSEMaximumChargeCurrent_isUsed:1;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;
    unsigned int EVSEMaximumVoltage_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargePower;
    unsigned int EVSEMaximumDischargePower_isUsed:1;
    // EVSEMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumDischargePower;
    unsigned int EVSEMinimumDischargePower_isUsed:1;
    // EVSEMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargeCurrent;
    unsigned int EVSEMaximumDischargeCurrent_isUsed:1;
    // EVSEMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumVoltage;
    unsigned int EVSEMinimumVoltage_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLReqControlModeType; base type=Dynamic_DC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMaximumChargeCurrent, RationalNumberType (1, 1); EVMaximumVoltage, RationalNumberType (1, 1); EVMinimumVoltage, RationalNumberType (1, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMaximumDischargeCurrent, RationalNumberType (1, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargePower;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumChargePower;
    // EVMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumChargeCurrent;
    // EVMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumVoltage;
    // EVMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumVoltage;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargePower;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumDischargePower;
    // EVMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumDischargeCurrent;
    // EVMaximumV2XEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMaximumV2XEnergyRequest;
    unsigned int EVMaximumV2XEnergyRequest_isUsed:1;
    // EVMinimumV2XEnergyRequest, RationalNumberType
    struct iso20_dc_RationalNumberType EVMinimumV2XEnergyRequest;
    unsigned int EVMinimumV2XEnergyRequest_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); EVPresentVoltage, RationalNumberType (1, 1); BPT_Dynamic_DC_CLReqControlMode, BPT_Dynamic_DC_CLReqControlModeType (0, 1); BPT_Scheduled_DC_CLReqControlMode, BPT_Scheduled_DC_CLReqControlModeType (0, 1); CLReqControlMode, CLReqControlModeType (0, 1); Dynamic_DC_CLReqControlMode, Dynamic_DC_CLReqControlModeType (0, 1); Scheduled_DC_CLReqControlMode, Scheduled_DC_CLReqControlModeType (0, 1);
struct iso20_dc_DC_ChargeLoopReqType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // DisplayParameters, DisplayParametersType
    struct iso20_dc_DisplayParametersType DisplayParameters;
    unsigned int DisplayParameters_isUsed:1;
    // MeterInfoRequested, boolean
    int MeterInfoRequested;
    // EVPresentVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVPresentVoltage;
    // BPT_Dynamic_DC_CLReqControlMode, BPT_Dynamic_DC_CLReqControlModeType (base: Dynamic_DC_CLReqControlModeType)
    struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType BPT_Dynamic_DC_CLReqControlMode;
    unsigned int BPT_Dynamic_DC_CLReqControlMode_isUsed:1;
    // BPT_Scheduled_DC_CLReqControlMode, BPT_Scheduled_DC_CLReqControlModeType (base: Scheduled_DC_CLReqControlModeType)
    struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType BPT_Scheduled_DC_CLReqControlMode;
    unsigned int BPT_Scheduled_DC_CLReqControlMode_isUsed:1;
    // CLReqControlMode, CLReqControlModeType
    struct iso20_dc_CLReqControlModeType CLReqControlMode;
    unsigned int CLReqControlMode_isUsed:1;
    // Dynamic_DC_CLReqControlMode, Dynamic_DC_CLReqControlModeType (base: Dynamic_CLReqControlModeType)
    struct iso20_dc_Dynamic_DC_CLReqControlModeType Dynamic_DC_CLReqControlMode;
    unsigned int Dynamic_DC_CLReqControlMode_isUsed:1;
    // Scheduled_DC_CLReqControlMode, Scheduled_DC_CLReqControlModeType (base: Scheduled_CLReqControlModeType)
    struct iso20_dc_Scheduled_DC_CLReqControlModeType Scheduled_DC_CLReqControlMode;
    unsigned int Scheduled_DC_CLReqControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLResControlMode; type={urn:iso:std:iso:15118:-20:DC}BPT_Dynamic_DC_CLResControlModeType; base type=Dynamic_DC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMaximumChargeCurrent, RationalNumberType (1, 1); EVSEMaximumVoltage, RationalNumberType (1, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargeCurrent, RationalNumberType (1, 1); EVSEMinimumVoltage, RationalNumberType (1, 1);
struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType {
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
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargePower;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumChargePower;
    // EVSEMaximumChargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumChargeCurrent;
    // EVSEMaximumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumVoltage;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMinimumDischargePower, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumDischargePower;
    // EVSEMaximumDischargeCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMaximumDischargeCurrent;
    // EVSEMinimumVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEMinimumVoltage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:DC}DC_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVSEPresentCurrent, RationalNumberType (1, 1); EVSEPresentVoltage, RationalNumberType (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); BPT_Dynamic_DC_CLResControlMode, BPT_Dynamic_DC_CLResControlModeType (0, 1); BPT_Scheduled_DC_CLResControlMode, BPT_Scheduled_DC_CLResControlModeType (0, 1); CLResControlMode, CLResControlModeType (0, 1); Dynamic_DC_CLResControlMode, Dynamic_DC_CLResControlModeType (0, 1); Scheduled_DC_CLResControlMode, Scheduled_DC_CLResControlModeType (0, 1);
struct iso20_dc_DC_ChargeLoopResType {
    // Header, MessageHeaderType
    struct iso20_dc_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_dc_responseCodeType ResponseCode;
    // EVSEStatus, EVSEStatusType
    struct iso20_dc_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso20_dc_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // Receipt, ReceiptType
    struct iso20_dc_ReceiptType Receipt;
    unsigned int Receipt_isUsed:1;
    // EVSEPresentCurrent, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPresentCurrent;
    // EVSEPresentVoltage, RationalNumberType
    struct iso20_dc_RationalNumberType EVSEPresentVoltage;
    // EVSEPowerLimitAchieved, boolean
    int EVSEPowerLimitAchieved;
    // EVSECurrentLimitAchieved, boolean
    int EVSECurrentLimitAchieved;
    // EVSEVoltageLimitAchieved, boolean
    int EVSEVoltageLimitAchieved;
    // BPT_Dynamic_DC_CLResControlMode, BPT_Dynamic_DC_CLResControlModeType (base: Dynamic_DC_CLResControlModeType)
    struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType BPT_Dynamic_DC_CLResControlMode;
    unsigned int BPT_Dynamic_DC_CLResControlMode_isUsed:1;
    // BPT_Scheduled_DC_CLResControlMode, BPT_Scheduled_DC_CLResControlModeType (base: Scheduled_DC_CLResControlModeType)
    struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType BPT_Scheduled_DC_CLResControlMode;
    unsigned int BPT_Scheduled_DC_CLResControlMode_isUsed:1;
    // CLResControlMode, CLResControlModeType
    struct iso20_dc_CLResControlModeType CLResControlMode;
    unsigned int CLResControlMode_isUsed:1;
    // Dynamic_DC_CLResControlMode, Dynamic_DC_CLResControlModeType (base: Dynamic_CLResControlModeType)
    struct iso20_dc_Dynamic_DC_CLResControlModeType Dynamic_DC_CLResControlMode;
    unsigned int Dynamic_DC_CLResControlMode_isUsed:1;
    // Scheduled_DC_CLResControlMode, Scheduled_DC_CLResControlModeType (base: Scheduled_CLResControlModeType)
    struct iso20_dc_Scheduled_DC_CLResControlModeType Scheduled_DC_CLResControlMode;
    unsigned int Scheduled_DC_CLResControlMode_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_dc_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_dc_ReferenceType array[iso20_dc_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
struct iso20_dc_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_dc_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_dc_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_dc_exiDocument {
    union {
        struct iso20_dc_DC_ChargeParameterDiscoveryReqType DC_ChargeParameterDiscoveryReq;
        struct iso20_dc_DC_ChargeParameterDiscoveryResType DC_ChargeParameterDiscoveryRes;
        struct iso20_dc_DC_CableCheckReqType DC_CableCheckReq;
        struct iso20_dc_DC_CableCheckResType DC_CableCheckRes;
        struct iso20_dc_DC_PreChargeReqType DC_PreChargeReq;
        struct iso20_dc_DC_PreChargeResType DC_PreChargeRes;
        struct iso20_dc_DC_ChargeLoopReqType DC_ChargeLoopReq;
        struct iso20_dc_DC_ChargeLoopResType DC_ChargeLoopRes;
        struct iso20_dc_DC_WeldingDetectionReqType DC_WeldingDetectionReq;
        struct iso20_dc_DC_WeldingDetectionResType DC_WeldingDetectionRes;
        struct iso20_dc_DC_CPDReqEnergyTransferModeType DC_CPDReqEnergyTransferMode;
        struct iso20_dc_DC_CPDResEnergyTransferModeType DC_CPDResEnergyTransferMode;
        struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType BPT_DC_CPDReqEnergyTransferMode;
        struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType BPT_DC_CPDResEnergyTransferMode;
        struct iso20_dc_Scheduled_DC_CLReqControlModeType Scheduled_DC_CLReqControlMode;
        struct iso20_dc_Scheduled_DC_CLResControlModeType Scheduled_DC_CLResControlMode;
        struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType BPT_Scheduled_DC_CLReqControlMode;
        struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType BPT_Scheduled_DC_CLResControlMode;
        struct iso20_dc_Dynamic_DC_CLReqControlModeType Dynamic_DC_CLReqControlMode;
        struct iso20_dc_Dynamic_DC_CLResControlModeType Dynamic_DC_CLResControlMode;
        struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType BPT_Dynamic_DC_CLReqControlMode;
        struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType BPT_Dynamic_DC_CLResControlMode;
        struct iso20_dc_CLReqControlModeType CLReqControlMode;
        struct iso20_dc_CLResControlModeType CLResControlMode;
        struct iso20_dc_SignatureType Signature;
        struct iso20_dc_SignatureValueType SignatureValue;
        struct iso20_dc_SignedInfoType SignedInfo;
        struct iso20_dc_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_dc_SignatureMethodType SignatureMethod;
        struct iso20_dc_ReferenceType Reference;
        struct iso20_dc_TransformsType Transforms;
        struct iso20_dc_TransformType Transform;
        struct iso20_dc_DigestMethodType DigestMethod;
        struct iso20_dc_KeyInfoType KeyInfo;
        struct iso20_dc_KeyValueType KeyValue;
        struct iso20_dc_RetrievalMethodType RetrievalMethod;
        struct iso20_dc_X509DataType X509Data;
        struct iso20_dc_PGPDataType PGPData;
        struct iso20_dc_SPKIDataType SPKIData;
        struct iso20_dc_ObjectType Object;
        struct iso20_dc_ManifestType Manifest;
        struct iso20_dc_SignaturePropertiesType SignatureProperties;
        struct iso20_dc_SignaturePropertyType SignatureProperty;
        struct iso20_dc_DSAKeyValueType DSAKeyValue;
        struct iso20_dc_RSAKeyValueType RSAKeyValue;
    };
    unsigned int DC_ChargeParameterDiscoveryReq_isUsed:1;
    unsigned int DC_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int DC_CableCheckReq_isUsed:1;
    unsigned int DC_CableCheckRes_isUsed:1;
    unsigned int DC_PreChargeReq_isUsed:1;
    unsigned int DC_PreChargeRes_isUsed:1;
    unsigned int DC_ChargeLoopReq_isUsed:1;
    unsigned int DC_ChargeLoopRes_isUsed:1;
    unsigned int DC_WeldingDetectionReq_isUsed:1;
    unsigned int DC_WeldingDetectionRes_isUsed:1;
    unsigned int DC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int DC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int BPT_DC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int BPT_DC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int Scheduled_DC_CLReqControlMode_isUsed:1;
    unsigned int Scheduled_DC_CLResControlMode_isUsed:1;
    unsigned int BPT_Scheduled_DC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Scheduled_DC_CLResControlMode_isUsed:1;
    unsigned int Dynamic_DC_CLReqControlMode_isUsed:1;
    unsigned int Dynamic_DC_CLResControlMode_isUsed:1;
    unsigned int BPT_Dynamic_DC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Dynamic_DC_CLResControlMode_isUsed:1;
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
struct iso20_dc_exiFragment {
    union {
        struct iso20_dc_DC_ChargeParameterDiscoveryResType DC_ChargeParameterDiscoveryRes;
        struct iso20_dc_SignedInfoType SignedInfo;
    };
    unsigned int DC_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
};

// elements of xmldsig fragment
struct iso20_dc_xmldsigFragment {
    union {
        struct iso20_dc_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_dc_DSAKeyValueType DSAKeyValue;
        struct iso20_dc_DigestMethodType DigestMethod;
        struct iso20_dc_KeyInfoType KeyInfo;
        struct iso20_dc_KeyValueType KeyValue;
        struct iso20_dc_ManifestType Manifest;
        struct iso20_dc_ObjectType Object;
        struct iso20_dc_PGPDataType PGPData;
        struct iso20_dc_RSAKeyValueType RSAKeyValue;
        struct iso20_dc_ReferenceType Reference;
        struct iso20_dc_RetrievalMethodType RetrievalMethod;
        struct iso20_dc_SPKIDataType SPKIData;
        struct iso20_dc_SignatureType Signature;
        struct iso20_dc_SignatureMethodType SignatureMethod;
        struct iso20_dc_SignaturePropertiesType SignatureProperties;
        struct iso20_dc_SignaturePropertyType SignatureProperty;
        struct iso20_dc_SignatureValueType SignatureValue;
        struct iso20_dc_SignedInfoType SignedInfo;
        struct iso20_dc_TransformType Transform;
        struct iso20_dc_TransformsType Transforms;
        struct iso20_dc_X509DataType X509Data;
        struct iso20_dc_X509IssuerSerialType X509IssuerSerial;
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
void init_iso20_dc_exiDocument(struct iso20_dc_exiDocument* exiDoc);
void init_iso20_dc_DC_ChargeParameterDiscoveryReqType(struct iso20_dc_DC_ChargeParameterDiscoveryReqType* DC_ChargeParameterDiscoveryReq);
void init_iso20_dc_DC_ChargeParameterDiscoveryResType(struct iso20_dc_DC_ChargeParameterDiscoveryResType* DC_ChargeParameterDiscoveryRes);
void init_iso20_dc_DC_CableCheckReqType(struct iso20_dc_DC_CableCheckReqType* DC_CableCheckReq);
void init_iso20_dc_DC_CableCheckResType(struct iso20_dc_DC_CableCheckResType* DC_CableCheckRes);
void init_iso20_dc_DC_PreChargeReqType(struct iso20_dc_DC_PreChargeReqType* DC_PreChargeReq);
void init_iso20_dc_DC_PreChargeResType(struct iso20_dc_DC_PreChargeResType* DC_PreChargeRes);
void init_iso20_dc_DC_ChargeLoopReqType(struct iso20_dc_DC_ChargeLoopReqType* DC_ChargeLoopReq);
void init_iso20_dc_DC_ChargeLoopResType(struct iso20_dc_DC_ChargeLoopResType* DC_ChargeLoopRes);
void init_iso20_dc_DC_WeldingDetectionReqType(struct iso20_dc_DC_WeldingDetectionReqType* DC_WeldingDetectionReq);
void init_iso20_dc_DC_WeldingDetectionResType(struct iso20_dc_DC_WeldingDetectionResType* DC_WeldingDetectionRes);
void init_iso20_dc_DC_CPDReqEnergyTransferModeType(struct iso20_dc_DC_CPDReqEnergyTransferModeType* DC_CPDReqEnergyTransferMode);
void init_iso20_dc_DC_CPDResEnergyTransferModeType(struct iso20_dc_DC_CPDResEnergyTransferModeType* DC_CPDResEnergyTransferMode);
void init_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType* BPT_DC_CPDReqEnergyTransferMode);
void init_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType* BPT_DC_CPDResEnergyTransferMode);
void init_iso20_dc_Scheduled_DC_CLReqControlModeType(struct iso20_dc_Scheduled_DC_CLReqControlModeType* Scheduled_DC_CLReqControlMode);
void init_iso20_dc_Scheduled_DC_CLResControlModeType(struct iso20_dc_Scheduled_DC_CLResControlModeType* Scheduled_DC_CLResControlMode);
void init_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType* BPT_Scheduled_DC_CLReqControlMode);
void init_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType* BPT_Scheduled_DC_CLResControlMode);
void init_iso20_dc_Dynamic_DC_CLReqControlModeType(struct iso20_dc_Dynamic_DC_CLReqControlModeType* Dynamic_DC_CLReqControlMode);
void init_iso20_dc_Dynamic_DC_CLResControlModeType(struct iso20_dc_Dynamic_DC_CLResControlModeType* Dynamic_DC_CLResControlMode);
void init_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType* BPT_Dynamic_DC_CLReqControlMode);
void init_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType* BPT_Dynamic_DC_CLResControlMode);
void init_iso20_dc_CLReqControlModeType(struct iso20_dc_CLReqControlModeType* CLReqControlMode);
void init_iso20_dc_CLResControlModeType(struct iso20_dc_CLResControlModeType* CLResControlMode);
void init_iso20_dc_SignatureType(struct iso20_dc_SignatureType* Signature);
void init_iso20_dc_SignatureValueType(struct iso20_dc_SignatureValueType* SignatureValue);
void init_iso20_dc_SignedInfoType(struct iso20_dc_SignedInfoType* SignedInfo);
void init_iso20_dc_CanonicalizationMethodType(struct iso20_dc_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_dc_SignatureMethodType(struct iso20_dc_SignatureMethodType* SignatureMethod);
void init_iso20_dc_ReferenceType(struct iso20_dc_ReferenceType* Reference);
void init_iso20_dc_TransformsType(struct iso20_dc_TransformsType* Transforms);
void init_iso20_dc_TransformType(struct iso20_dc_TransformType* Transform);
void init_iso20_dc_DigestMethodType(struct iso20_dc_DigestMethodType* DigestMethod);
void init_iso20_dc_KeyInfoType(struct iso20_dc_KeyInfoType* KeyInfo);
void init_iso20_dc_KeyValueType(struct iso20_dc_KeyValueType* KeyValue);
void init_iso20_dc_RetrievalMethodType(struct iso20_dc_RetrievalMethodType* RetrievalMethod);
void init_iso20_dc_X509DataType(struct iso20_dc_X509DataType* X509Data);
void init_iso20_dc_PGPDataType(struct iso20_dc_PGPDataType* PGPData);
void init_iso20_dc_SPKIDataType(struct iso20_dc_SPKIDataType* SPKIData);
void init_iso20_dc_ObjectType(struct iso20_dc_ObjectType* Object);
void init_iso20_dc_ManifestType(struct iso20_dc_ManifestType* Manifest);
void init_iso20_dc_SignaturePropertiesType(struct iso20_dc_SignaturePropertiesType* SignatureProperties);
void init_iso20_dc_SignaturePropertyType(struct iso20_dc_SignaturePropertyType* SignatureProperty);
void init_iso20_dc_DSAKeyValueType(struct iso20_dc_DSAKeyValueType* DSAKeyValue);
void init_iso20_dc_RSAKeyValueType(struct iso20_dc_RSAKeyValueType* RSAKeyValue);
void init_iso20_dc_X509IssuerSerialType(struct iso20_dc_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_dc_RationalNumberType(struct iso20_dc_RationalNumberType* RationalNumberType);
void init_iso20_dc_DetailedCostType(struct iso20_dc_DetailedCostType* DetailedCostType);
void init_iso20_dc_DetailedTaxType(struct iso20_dc_DetailedTaxType* DetailedTaxType);
void init_iso20_dc_MessageHeaderType(struct iso20_dc_MessageHeaderType* MessageHeaderType);
void init_iso20_dc_DisplayParametersType(struct iso20_dc_DisplayParametersType* DisplayParametersType);
void init_iso20_dc_EVSEStatusType(struct iso20_dc_EVSEStatusType* EVSEStatusType);
void init_iso20_dc_MeterInfoType(struct iso20_dc_MeterInfoType* MeterInfoType);
void init_iso20_dc_ReceiptType(struct iso20_dc_ReceiptType* ReceiptType);
void init_iso20_dc_exiFragment(struct iso20_dc_exiFragment* exiFrag);
void init_iso20_dc_xmldsigFragment(struct iso20_dc_xmldsigFragment* xmldsigFrag);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_DC_DATATYPES_H */

