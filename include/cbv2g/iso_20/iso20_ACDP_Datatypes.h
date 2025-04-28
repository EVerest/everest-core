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
  * @file iso20_ACDP_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_ACDP_DATATYPES_H
#define ISO20_ACDP_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso20_acdp_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_anyType_BYTES_SIZE (4)
#define iso20_acdp_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_acdp_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_acdp_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_acdp_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_ReferenceType_4_ARRAY_SIZE (4)
#define iso20_acdp_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_acdp_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_acdp_sessionIDType_BYTES_SIZE (8)
#define iso20_acdp_Target_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)


// enum for function numbers
typedef enum {
    iso20_acdp_ACDP_ConnectReq = 0,
    iso20_acdp_ACDP_ConnectRes = 1,
    iso20_acdp_ACDP_DisconnectReq = 2,
    iso20_acdp_ACDP_DisconnectRes = 3,
    iso20_acdp_ACDP_SystemStatusReq = 4,
    iso20_acdp_ACDP_SystemStatusRes = 5,
    iso20_acdp_ACDP_VehiclePositioningReq = 6,
    iso20_acdp_ACDP_VehiclePositioningRes = 7,
    iso20_acdp_CLReqControlMode = 8,
    iso20_acdp_CLResControlMode = 9,
    iso20_acdp_CanonicalizationMethod = 10,
    iso20_acdp_DSAKeyValue = 11,
    iso20_acdp_DigestMethod = 12,
    iso20_acdp_DigestValue = 13,
    iso20_acdp_KeyInfo = 14,
    iso20_acdp_KeyName = 15,
    iso20_acdp_KeyValue = 16,
    iso20_acdp_Manifest = 17,
    iso20_acdp_MgmtData = 18,
    iso20_acdp_Object = 19,
    iso20_acdp_PGPData = 20,
    iso20_acdp_RSAKeyValue = 21,
    iso20_acdp_Reference = 22,
    iso20_acdp_RetrievalMethod = 23,
    iso20_acdp_SPKIData = 24,
    iso20_acdp_Signature = 25,
    iso20_acdp_SignatureMethod = 26,
    iso20_acdp_SignatureProperties = 27,
    iso20_acdp_SignatureProperty = 28,
    iso20_acdp_SignatureValue = 29,
    iso20_acdp_SignedInfo = 30,
    iso20_acdp_Transform = 31,
    iso20_acdp_Transforms = 32,
    iso20_acdp_X509Data = 33
} iso20_acdp_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVCPStatus; type={urn:iso:std:iso:15118:-20:ACDP}cpStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_cpStatusType_StateA = 0,
    iso20_acdp_cpStatusType_StateB = 1,
    iso20_acdp_cpStatusType_StateC = 2,
    iso20_acdp_cpStatusType_StateD = 3,
    iso20_acdp_cpStatusType_StateE = 4
} iso20_acdp_cpStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVErrorCode; type={urn:iso:std:iso:15118:-20:ACDP}errorCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_errorCodeType_OK_NoEVError = 0,
    iso20_acdp_errorCodeType_FAILED = 1,
    iso20_acdp_errorCodeType_FAILED_EmergencyEvent = 2,
    iso20_acdp_errorCodeType_FAILED_Breaker = 3,
    iso20_acdp_errorCodeType_FAILED_RESSTemperatureInhibit = 4,
    iso20_acdp_errorCodeType_FAILED_RESS = 5,
    iso20_acdp_errorCodeType_FAILED_ChargingCurrentDifferential = 6,
    iso20_acdp_errorCodeType_FAILED_ChargingVoltageOutOfRange = 7,
    iso20_acdp_errorCodeType_FAILED_Reserved1 = 8,
    iso20_acdp_errorCodeType_FAILED_Reserved2 = 9
} iso20_acdp_errorCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_responseCodeType_OK = 0,
    iso20_acdp_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_acdp_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_acdp_responseCodeType_OK_OldSessionJoined = 3,
    iso20_acdp_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_acdp_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_acdp_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_acdp_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_acdp_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_acdp_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_acdp_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_acdp_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_acdp_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_acdp_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_acdp_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_acdp_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_acdp_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_acdp_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_acdp_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_acdp_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_acdp_responseCodeType_WARNING_WPT = 20,
    iso20_acdp_responseCodeType_FAILED = 21,
    iso20_acdp_responseCodeType_FAILED_AssociationError = 22,
    iso20_acdp_responseCodeType_FAILED_ContactorError = 23,
    iso20_acdp_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_acdp_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_acdp_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_acdp_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_acdp_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_acdp_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_acdp_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_acdp_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_acdp_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_acdp_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_acdp_responseCodeType_FAILED_SequenceError = 34,
    iso20_acdp_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_acdp_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_acdp_responseCodeType_FAILED_SignatureError = 37,
    iso20_acdp_responseCodeType_FAILED_UnknownSession = 38,
    iso20_acdp_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_acdp_responseCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVElectricalChargingDeviceStatus; type={urn:iso:std:iso:15118:-20:ACDP}electricalChargingDeviceStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_electricalChargingDeviceStatusType_State_A = 0,
    iso20_acdp_electricalChargingDeviceStatusType_State_B = 1,
    iso20_acdp_electricalChargingDeviceStatusType_State_C = 2,
    iso20_acdp_electricalChargingDeviceStatusType_State_D = 3
} iso20_acdp_electricalChargingDeviceStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVSEProcessing; type={urn:iso:std:iso:15118:-20:CommonTypes}processingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_processingType_Finished = 0,
    iso20_acdp_processingType_Ongoing = 1,
    iso20_acdp_processingType_Ongoing_WaitingForCustomerInteraction = 2
} iso20_acdp_processingType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVSEMechanicalChargingDeviceStatus; type={urn:iso:std:iso:15118:-20:ACDP}mechanicalChargingDeviceStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_mechanicalChargingDeviceStatusType_Home = 0,
    iso20_acdp_mechanicalChargingDeviceStatusType_Moving = 1,
    iso20_acdp_mechanicalChargingDeviceStatusType_EndPosition = 2
} iso20_acdp_mechanicalChargingDeviceStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:ACDP}EVSEIsolationStatus; type={urn:iso:std:iso:15118:-20:ACDP}isolationStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_acdp_isolationStatusType_Invalid = 0,
    iso20_acdp_isolationStatusType_Safe = 1,
    iso20_acdp_isolationStatusType_Warning = 2,
    iso20_acdp_isolationStatusType_Fault = 3
} iso20_acdp_isolationStatusType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_acdp_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_acdp_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_acdp_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso20_acdp_TransformsType {
    // Transform, TransformType
    struct iso20_acdp_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso20_acdp_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_acdp_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_acdp_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_acdp_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_acdp_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_acdp_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_acdp_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_acdp_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_acdp_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_acdp_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_acdp_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_acdp_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_acdp_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_acdp_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_acdp_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_acdp_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_acdp_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_acdp_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_acdp_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_acdp_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_acdp_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_acdp_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_acdp_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_acdp_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_acdp_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_acdp_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
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
struct iso20_acdp_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_acdp_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_acdp_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_acdp_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_acdp_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_acdp_ReferenceType array[iso20_acdp_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_acdp_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_acdp_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_acdp_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_acdp_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_acdp_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_acdp_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_acdp_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_acdp_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_acdp_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso20_acdp_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_acdp_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_acdp_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso20_acdp_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_acdp_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_acdp_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_acdp_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_acdp_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}EVWLANStrength; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_acdp_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_acdp_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_acdp_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_acdp_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_acdp_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_acdp_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_acdp_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}EVTechnicalStatus; type={urn:iso:std:iso:15118:-20:ACDP}EVTechnicalStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVReadyToCharge, boolean (1, 1); EVImmobilizationRequest, boolean (1, 1); EVImmobilized, boolean (0, 1); EVWLANStrength, RationalNumberType (0, 1); EVCPStatus, cpStatusType (0, 1); EVSOC, percentValueType (0, 1); EVErrorCode, errorCodeType (0, 1); EVTimeout, boolean (0, 1);
struct iso20_acdp_EVTechnicalStatusType {
    // EVReadyToCharge, boolean
    int EVReadyToCharge;
    // EVImmobilizationRequest, boolean
    int EVImmobilizationRequest;
    // EVImmobilized, boolean
    int EVImmobilized;
    unsigned int EVImmobilized_isUsed:1;
    // EVWLANStrength, RationalNumberType
    struct iso20_acdp_RationalNumberType EVWLANStrength;
    unsigned int EVWLANStrength_isUsed:1;
    // EVCPStatus, cpStatusType (base: string)
    iso20_acdp_cpStatusType EVCPStatus;
    unsigned int EVCPStatus_isUsed:1;
    // EVSOC, percentValueType (base: byte)
    int8_t EVSOC;
    unsigned int EVSOC_isUsed:1;
    // EVErrorCode, errorCodeType (base: string)
    iso20_acdp_errorCodeType EVErrorCode;
    unsigned int EVErrorCode_isUsed:1;
    // EVTimeout, boolean
    int EVTimeout;
    unsigned int EVTimeout_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVMobilityStatus, boolean (1, 1); EVPositioningSupport, boolean (1, 1);
struct iso20_acdp_ACDP_VehiclePositioningReqType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // EVMobilityStatus, boolean
    int EVMobilityStatus;
    // EVPositioningSupport, boolean
    int EVPositioningSupport;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_VehiclePositioningResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); EVSEPositioningSupport, boolean (1, 1); EVRelativeXDeviation, short (1, 1); EVRelativeYDeviation, short (1, 1); ContactWindowXc, short (1, 1); ContactWindowYc, short (1, 1); EVInChargePosition, boolean (1, 1);
struct iso20_acdp_ACDP_VehiclePositioningResType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_acdp_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_acdp_processingType EVSEProcessing;
    // EVSEPositioningSupport, boolean
    int EVSEPositioningSupport;
    // EVRelativeXDeviation, short (base: int)
    int16_t EVRelativeXDeviation;
    // EVRelativeYDeviation, short (base: int)
    int16_t EVRelativeYDeviation;
    // ContactWindowXc, short (base: int)
    int16_t ContactWindowXc;
    // ContactWindowYc, short (base: int)
    int16_t ContactWindowYc;
    // EVInChargePosition, boolean
    int EVInChargePosition;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (1, 1);
struct iso20_acdp_ACDP_ConnectReqType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // EVElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (base: string)
    iso20_acdp_electricalChargingDeviceStatusType EVElectricalChargingDeviceStatus;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_ConnectResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); EVSEElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (1, 1); EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (1, 1);
struct iso20_acdp_ACDP_ConnectResType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_acdp_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_acdp_processingType EVSEProcessing;
    // EVSEElectricalChargingDeviceStatus, electricalChargingDeviceStatusType (base: string)
    iso20_acdp_electricalChargingDeviceStatusType EVSEElectricalChargingDeviceStatus;
    // EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (base: string)
    iso20_acdp_mechanicalChargingDeviceStatusType EVSEMechanicalChargingDeviceStatus;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusReq; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVTechnicalStatus, EVTechnicalStatusType (1, 1);
struct iso20_acdp_ACDP_SystemStatusReqType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // EVTechnicalStatus, EVTechnicalStatusType
    struct iso20_acdp_EVTechnicalStatusType EVTechnicalStatus;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusRes; type={urn:iso:std:iso:15118:-20:ACDP}ACDP_SystemStatusResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (1, 1); EVSEReadyToCharge, boolean (1, 1); EVSEIsolationStatus, isolationStatusType (1, 1); EVSEDisabled, boolean (1, 1); EVSEUtilityInterruptEvent, boolean (1, 1); EVSEEmergencyShutdown, boolean (1, 1); EVSEMalfunction, boolean (1, 1); EVInChargePosition, boolean (1, 1); EVAssociationStatus, boolean (1, 1);
struct iso20_acdp_ACDP_SystemStatusResType {
    // Header, MessageHeaderType
    struct iso20_acdp_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_acdp_responseCodeType ResponseCode;
    // EVSEMechanicalChargingDeviceStatus, mechanicalChargingDeviceStatusType (base: string)
    iso20_acdp_mechanicalChargingDeviceStatusType EVSEMechanicalChargingDeviceStatus;
    // EVSEReadyToCharge, boolean
    int EVSEReadyToCharge;
    // EVSEIsolationStatus, isolationStatusType (base: string)
    iso20_acdp_isolationStatusType EVSEIsolationStatus;
    // EVSEDisabled, boolean
    int EVSEDisabled;
    // EVSEUtilityInterruptEvent, boolean
    int EVSEUtilityInterruptEvent;
    // EVSEEmergencyShutdown, boolean
    int EVSEEmergencyShutdown;
    // EVSEMalfunction, boolean
    int EVSEMalfunction;
    // EVInChargePosition, boolean
    int EVInChargePosition;
    // EVAssociationStatus, boolean
    int EVAssociationStatus;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_acdp_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_acdp_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_acdp_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_acdp_ReferenceType array[iso20_acdp_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
struct iso20_acdp_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_acdp_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_acdp_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_acdp_exiDocument {
    union {
        struct iso20_acdp_ACDP_VehiclePositioningReqType ACDP_VehiclePositioningReq;
        struct iso20_acdp_ACDP_VehiclePositioningResType ACDP_VehiclePositioningRes;
        struct iso20_acdp_ACDP_ConnectReqType ACDP_ConnectReq;
        struct iso20_acdp_ACDP_ConnectResType ACDP_ConnectRes;
        struct iso20_acdp_ACDP_ConnectReqType ACDP_DisconnectReq;
        struct iso20_acdp_ACDP_ConnectResType ACDP_DisconnectRes;
        struct iso20_acdp_ACDP_SystemStatusReqType ACDP_SystemStatusReq;
        struct iso20_acdp_ACDP_SystemStatusResType ACDP_SystemStatusRes;
        struct iso20_acdp_CLReqControlModeType CLReqControlMode;
        struct iso20_acdp_CLResControlModeType CLResControlMode;
        struct iso20_acdp_SignatureType Signature;
        struct iso20_acdp_SignatureValueType SignatureValue;
        struct iso20_acdp_SignedInfoType SignedInfo;
        struct iso20_acdp_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_acdp_SignatureMethodType SignatureMethod;
        struct iso20_acdp_ReferenceType Reference;
        struct iso20_acdp_TransformsType Transforms;
        struct iso20_acdp_TransformType Transform;
        struct iso20_acdp_DigestMethodType DigestMethod;
        struct iso20_acdp_KeyInfoType KeyInfo;
        struct iso20_acdp_KeyValueType KeyValue;
        struct iso20_acdp_RetrievalMethodType RetrievalMethod;
        struct iso20_acdp_X509DataType X509Data;
        struct iso20_acdp_PGPDataType PGPData;
        struct iso20_acdp_SPKIDataType SPKIData;
        struct iso20_acdp_ObjectType Object;
        struct iso20_acdp_ManifestType Manifest;
        struct iso20_acdp_SignaturePropertiesType SignatureProperties;
        struct iso20_acdp_SignaturePropertyType SignatureProperty;
        struct iso20_acdp_DSAKeyValueType DSAKeyValue;
        struct iso20_acdp_RSAKeyValueType RSAKeyValue;
    };
    unsigned int ACDP_VehiclePositioningReq_isUsed:1;
    unsigned int ACDP_VehiclePositioningRes_isUsed:1;
    unsigned int ACDP_ConnectReq_isUsed:1;
    unsigned int ACDP_ConnectRes_isUsed:1;
    unsigned int ACDP_DisconnectReq_isUsed:1;
    unsigned int ACDP_DisconnectRes_isUsed:1;
    unsigned int ACDP_SystemStatusReq_isUsed:1;
    unsigned int ACDP_SystemStatusRes_isUsed:1;
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

// init for structs
void init_iso20_acdp_exiDocument(struct iso20_acdp_exiDocument* exiDoc);
void init_iso20_acdp_ACDP_VehiclePositioningReqType(struct iso20_acdp_ACDP_VehiclePositioningReqType* ACDP_VehiclePositioningReq);
void init_iso20_acdp_ACDP_VehiclePositioningResType(struct iso20_acdp_ACDP_VehiclePositioningResType* ACDP_VehiclePositioningRes);
void init_iso20_acdp_ACDP_ConnectReqType(struct iso20_acdp_ACDP_ConnectReqType* ACDP_DisconnectReq);
void init_iso20_acdp_ACDP_ConnectResType(struct iso20_acdp_ACDP_ConnectResType* ACDP_DisconnectRes);
void init_iso20_acdp_ACDP_SystemStatusReqType(struct iso20_acdp_ACDP_SystemStatusReqType* ACDP_SystemStatusReq);
void init_iso20_acdp_ACDP_SystemStatusResType(struct iso20_acdp_ACDP_SystemStatusResType* ACDP_SystemStatusRes);
void init_iso20_acdp_CLReqControlModeType(struct iso20_acdp_CLReqControlModeType* CLReqControlMode);
void init_iso20_acdp_CLResControlModeType(struct iso20_acdp_CLResControlModeType* CLResControlMode);
void init_iso20_acdp_SignatureType(struct iso20_acdp_SignatureType* Signature);
void init_iso20_acdp_SignatureValueType(struct iso20_acdp_SignatureValueType* SignatureValue);
void init_iso20_acdp_SignedInfoType(struct iso20_acdp_SignedInfoType* SignedInfo);
void init_iso20_acdp_CanonicalizationMethodType(struct iso20_acdp_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_acdp_SignatureMethodType(struct iso20_acdp_SignatureMethodType* SignatureMethod);
void init_iso20_acdp_ReferenceType(struct iso20_acdp_ReferenceType* Reference);
void init_iso20_acdp_TransformsType(struct iso20_acdp_TransformsType* Transforms);
void init_iso20_acdp_TransformType(struct iso20_acdp_TransformType* Transform);
void init_iso20_acdp_DigestMethodType(struct iso20_acdp_DigestMethodType* DigestMethod);
void init_iso20_acdp_KeyInfoType(struct iso20_acdp_KeyInfoType* KeyInfo);
void init_iso20_acdp_KeyValueType(struct iso20_acdp_KeyValueType* KeyValue);
void init_iso20_acdp_RetrievalMethodType(struct iso20_acdp_RetrievalMethodType* RetrievalMethod);
void init_iso20_acdp_X509DataType(struct iso20_acdp_X509DataType* X509Data);
void init_iso20_acdp_PGPDataType(struct iso20_acdp_PGPDataType* PGPData);
void init_iso20_acdp_SPKIDataType(struct iso20_acdp_SPKIDataType* SPKIData);
void init_iso20_acdp_ObjectType(struct iso20_acdp_ObjectType* Object);
void init_iso20_acdp_ManifestType(struct iso20_acdp_ManifestType* Manifest);
void init_iso20_acdp_SignaturePropertiesType(struct iso20_acdp_SignaturePropertiesType* SignatureProperties);
void init_iso20_acdp_SignaturePropertyType(struct iso20_acdp_SignaturePropertyType* SignatureProperty);
void init_iso20_acdp_DSAKeyValueType(struct iso20_acdp_DSAKeyValueType* DSAKeyValue);
void init_iso20_acdp_RSAKeyValueType(struct iso20_acdp_RSAKeyValueType* RSAKeyValue);
void init_iso20_acdp_X509IssuerSerialType(struct iso20_acdp_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_acdp_RationalNumberType(struct iso20_acdp_RationalNumberType* RationalNumberType);
void init_iso20_acdp_MessageHeaderType(struct iso20_acdp_MessageHeaderType* MessageHeaderType);
void init_iso20_acdp_EVTechnicalStatusType(struct iso20_acdp_EVTechnicalStatusType* EVTechnicalStatusType);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_ACDP_DATATYPES_H */

