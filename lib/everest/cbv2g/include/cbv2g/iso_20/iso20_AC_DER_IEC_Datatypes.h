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
  * @file iso20_AC_DER_IEC_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_AC_DER_IEC_DATATYPES_H
#define ISO20_AC_DER_IEC_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso20_ac_der_iec_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_anyType_BYTES_SIZE (4)
#define iso20_ac_der_iec_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_ac_der_iec_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_ac_der_iec_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_ac_der_iec_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_ReferenceType_4_ARRAY_SIZE (4)
#define iso20_ac_der_iec_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_ac_der_iec_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_DataTupleType_10_ARRAY_SIZE (10)
#define iso20_ac_der_iec_sessionIDType_BYTES_SIZE (8)
#define iso20_ac_der_iec_Target_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso20_ac_der_iec_meterSignatureType_BYTES_SIZE (64)
#define iso20_ac_der_iec_DetailedTaxType_10_ARRAY_SIZE (10)


// enum for function numbers
typedef enum {
    iso20_ac_der_iec_AC_CPDReqEnergyTransferMode = 0,
    iso20_ac_der_iec_AC_CPDResEnergyTransferMode = 1,
    iso20_ac_der_iec_AC_ChargeLoopReq = 2,
    iso20_ac_der_iec_AC_ChargeLoopRes = 3,
    iso20_ac_der_iec_AC_ChargeParameterDiscoveryReq = 4,
    iso20_ac_der_iec_AC_ChargeParameterDiscoveryRes = 5,
    iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferMode = 6,
    iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferMode = 7,
    iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlMode = 8,
    iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlMode = 9,
    iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlMode = 10,
    iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlMode = 11,
    iso20_ac_der_iec_CLReqControlMode = 12,
    iso20_ac_der_iec_CLResControlMode = 13,
    iso20_ac_der_iec_CanonicalizationMethod = 14,
    iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferMode = 15,
    iso20_ac_der_iec_DER_AC_CPDResEnergyTransferMode = 16,
    iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlMode = 17,
    iso20_ac_der_iec_DER_Dynamic_AC_CLResControlMode = 18,
    iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlMode = 19,
    iso20_ac_der_iec_DER_Scheduled_AC_CLResControlMode = 20,
    iso20_ac_der_iec_DSAKeyValue = 21,
    iso20_ac_der_iec_DigestMethod = 22,
    iso20_ac_der_iec_DigestValue = 23,
    iso20_ac_der_iec_Dynamic_AC_CLReqControlMode = 24,
    iso20_ac_der_iec_Dynamic_AC_CLResControlMode = 25,
    iso20_ac_der_iec_KeyInfo = 26,
    iso20_ac_der_iec_KeyName = 27,
    iso20_ac_der_iec_KeyValue = 28,
    iso20_ac_der_iec_Manifest = 29,
    iso20_ac_der_iec_MgmtData = 30,
    iso20_ac_der_iec_Object = 31,
    iso20_ac_der_iec_PGPData = 32,
    iso20_ac_der_iec_RSAKeyValue = 33,
    iso20_ac_der_iec_Reference = 34,
    iso20_ac_der_iec_RetrievalMethod = 35,
    iso20_ac_der_iec_SPKIData = 36,
    iso20_ac_der_iec_Scheduled_AC_CLReqControlMode = 37,
    iso20_ac_der_iec_Scheduled_AC_CLResControlMode = 38,
    iso20_ac_der_iec_Signature = 39,
    iso20_ac_der_iec_SignatureMethod = 40,
    iso20_ac_der_iec_SignatureProperties = 41,
    iso20_ac_der_iec_SignatureProperty = 42,
    iso20_ac_der_iec_SignatureValue = 43,
    iso20_ac_der_iec_SignedInfo = 44,
    iso20_ac_der_iec_Transform = 45,
    iso20_ac_der_iec_Transforms = 46,
    iso20_ac_der_iec_X509Data = 47
} iso20_ac_der_iec_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}Excitation; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}powerFactorExcitationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_powerFactorExcitationType_OverExcited = 0,
    iso20_ac_der_iec_powerFactorExcitationType_UnderExcited = 1
} iso20_ac_der_iec_powerFactorExcitationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}xUnit; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}curveDataPointsUnitType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_curveDataPointsUnitType_V = 0,
    iso20_ac_der_iec_curveDataPointsUnitType_Hz = 1,
    iso20_ac_der_iec_curveDataPointsUnitType_W = 2,
    iso20_ac_der_iec_curveDataPointsUnitType_s = 3,
    iso20_ac_der_iec_curveDataPointsUnitType_var = 4
} iso20_ac_der_iec_curveDataPointsUnitType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}LockValueUnit; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}lockValueUnitType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_lockValueUnitType_V = 0,
    iso20_ac_der_iec_lockValueUnitType_Hz = 1,
    iso20_ac_der_iec_lockValueUnitType_W = 2,
    iso20_ac_der_iec_lockValueUnitType_s = 3,
    iso20_ac_der_iec_lockValueUnitType_var = 4
} iso20_ac_der_iec_lockValueUnitType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}PowerReference; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}powerReferenceType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_powerReferenceType_MaximumDischargePower = 0,
    iso20_ac_der_iec_powerReferenceType_MomentaryPower = 1
} iso20_ac_der_iec_powerReferenceType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSENotification; type={urn:iso:std:iso:15118:-20:CommonTypes}evseNotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_evseNotificationType_Pause = 0,
    iso20_ac_der_iec_evseNotificationType_ExitStandby = 1,
    iso20_ac_der_iec_evseNotificationType_Terminate = 2,
    iso20_ac_der_iec_evseNotificationType_ScheduleRenegotiation = 3,
    iso20_ac_der_iec_evseNotificationType_ServiceRenegotiation = 4,
    iso20_ac_der_iec_evseNotificationType_MeteringConfirmation = 5
} iso20_ac_der_iec_evseNotificationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_responseCodeType_OK = 0,
    iso20_ac_der_iec_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_ac_der_iec_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_ac_der_iec_responseCodeType_OK_OldSessionJoined = 3,
    iso20_ac_der_iec_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_ac_der_iec_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_ac_der_iec_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_ac_der_iec_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_ac_der_iec_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_ac_der_iec_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_ac_der_iec_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_ac_der_iec_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_ac_der_iec_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_ac_der_iec_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_ac_der_iec_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_ac_der_iec_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_ac_der_iec_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_ac_der_iec_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_ac_der_iec_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_ac_der_iec_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_ac_der_iec_responseCodeType_WARNING_WPT = 20,
    iso20_ac_der_iec_responseCodeType_FAILED = 21,
    iso20_ac_der_iec_responseCodeType_FAILED_AssociationError = 22,
    iso20_ac_der_iec_responseCodeType_FAILED_ContactorError = 23,
    iso20_ac_der_iec_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_ac_der_iec_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_ac_der_iec_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_ac_der_iec_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_ac_der_iec_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_ac_der_iec_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_ac_der_iec_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_ac_der_iec_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_ac_der_iec_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_ac_der_iec_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_ac_der_iec_responseCodeType_FAILED_SequenceError = 34,
    iso20_ac_der_iec_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_ac_der_iec_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_ac_der_iec_responseCodeType_FAILED_SignatureError = 37,
    iso20_ac_der_iec_responseCodeType_FAILED_UnknownSession = 38,
    iso20_ac_der_iec_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_ac_der_iec_responseCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}EVProcessing; type={urn:iso:std:iso:15118:-20:CommonTypes}processingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_processingType_Finished = 0,
    iso20_ac_der_iec_processingType_Ongoing = 1,
    iso20_ac_der_iec_processingType_Ongoing_WaitingForCustomerInteraction = 2
} iso20_ac_der_iec_processingType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}EVOperatingMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}evOperatingModeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_evOperatingModeType_GridFollowing = 0,
    iso20_ac_der_iec_evOperatingModeType_GridForming = 1
} iso20_ac_der_iec_evOperatingModeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}GridConnectionMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}gridConnectionModeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ac_der_iec_gridConnectionModeType_GridConnected = 0,
    iso20_ac_der_iec_gridConnectionModeType_GridIslanded = 1
} iso20_ac_der_iec_gridConnectionModeType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_ac_der_iec_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_der_iec_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_ac_der_iec_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso20_ac_der_iec_TransformsType {
    // Transform, TransformType
    struct iso20_ac_der_iec_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso20_ac_der_iec_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_ac_der_iec_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_ac_der_iec_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_der_iec_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_ac_der_iec_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_der_iec_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_ac_der_iec_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_ac_der_iec_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_ac_der_iec_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_ac_der_iec_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_ac_der_iec_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_ac_der_iec_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_ac_der_iec_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_ac_der_iec_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_ac_der_iec_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_ac_der_iec_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_ac_der_iec_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_ac_der_iec_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_ac_der_iec_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_ac_der_iec_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
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
struct iso20_ac_der_iec_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_ac_der_iec_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_ac_der_iec_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_ac_der_iec_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_ac_der_iec_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_ac_der_iec_ReferenceType array[iso20_ac_der_iec_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_ac_der_iec_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_ac_der_iec_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_ac_der_iec_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_ac_der_iec_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_ac_der_iec_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_ac_der_iec_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_ac_der_iec_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_ac_der_iec_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso20_ac_der_iec_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_ac_der_iec_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_ac_der_iec_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}EVMaximumChargePower; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_ac_der_iec_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}yValue; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}SetpointExcitationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SetpointValue, RationalNumberType (1, 1); Excitation, powerFactorExcitationType (0, 1);
struct iso20_ac_der_iec_SetpointExcitationType {
    // SetpointValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType SetpointValue;
    // Excitation, powerFactorExcitationType (base: string)
    iso20_ac_der_iec_powerFactorExcitationType Excitation;
    unsigned int Excitation_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}CurveDataPoint; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DataTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: xValue, RationalNumberType (1, 1); yValue, SetpointExcitationType (1, 1);
struct iso20_ac_der_iec_DataTupleType {
    // xValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType xValue;
    // yValue, SetpointExcitationType
    struct iso20_ac_der_iec_SetpointExcitationType yValue;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}CurveDataPoints; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}CurveDataPointsListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: CurveDataPoint, DataTupleType (2, 10);
struct iso20_ac_der_iec_CurveDataPointsListType {
    // CurveDataPoint, DataTupleType
    struct {
        struct iso20_ac_der_iec_DataTupleType array[iso20_ac_der_iec_DataTupleType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } CurveDataPoint;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}VoltVar; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DERCurveType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: xUnit, curveDataPointsUnitType (1, 1); yUnit, curveDataPointsUnitType (1, 1); CurveDataPoints, CurveDataPointsListType (1, 1); MinCosPhi, RationalNumberType (0, 1); LockValueUnit, lockValueUnitType (0, 1); LockInValue, RationalNumberType (0, 1); LockOutValue, RationalNumberType (0, 1); PT1ResponseReactivePower, boolean (1, 1); StepResponseTimeConstantReactivePower, RationalNumberType (1, 1); IntentionalDelay, RationalNumberType (0, 1);
struct iso20_ac_der_iec_DERCurveType {
    // xUnit, curveDataPointsUnitType (base: string)
    iso20_ac_der_iec_curveDataPointsUnitType xUnit;
    // yUnit, curveDataPointsUnitType (base: string)
    iso20_ac_der_iec_curveDataPointsUnitType yUnit;
    // CurveDataPoints, CurveDataPointsListType
    struct iso20_ac_der_iec_CurveDataPointsListType CurveDataPoints;
    // MinCosPhi, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType MinCosPhi;
    unsigned int MinCosPhi_isUsed:1;
    // LockValueUnit, lockValueUnitType (base: string)
    iso20_ac_der_iec_lockValueUnitType LockValueUnit;
    unsigned int LockValueUnit_isUsed:1;
    // LockInValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType LockInValue;
    unsigned int LockInValue_isUsed:1;
    // LockOutValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType LockOutValue;
    unsigned int LockOutValue_isUsed:1;
    // PT1ResponseReactivePower, boolean
    int PT1ResponseReactivePower;
    // StepResponseTimeConstantReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantReactivePower;
    // IntentionalDelay, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType IntentionalDelay;
    unsigned int IntentionalDelay_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}UnderFrequencyWatt; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}FrequencyWattType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Fstart, RationalNumberType (1, 1); Fstop, RationalNumberType (1, 1); IntentionalDelayFstop, unsignedShort (0, 1); Slope, RationalNumberType (1, 1); DeactivationTime, unsignedShort (0, 1); IntentionalDelayPowerControl, unsignedShort (0, 1); PowerReference, powerReferenceType (1, 1); HysteresisControl, boolean (1, 1); PowerUpRamp, unsignedShort (0, 1); PT1ResponseActivePower, boolean (1, 1); StepResponseTimeConstantActivePower, RationalNumberType (1, 1);
struct iso20_ac_der_iec_FrequencyWattType {
    // Fstart, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType Fstart;
    // Fstop, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType Fstop;
    // IntentionalDelayFstop, unsignedShort (base: unsignedInt)
    uint16_t IntentionalDelayFstop;
    unsigned int IntentionalDelayFstop_isUsed:1;
    // Slope, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType Slope;
    // DeactivationTime, unsignedShort (base: unsignedInt)
    uint16_t DeactivationTime;
    unsigned int DeactivationTime_isUsed:1;
    // IntentionalDelayPowerControl, unsignedShort (base: unsignedInt)
    uint16_t IntentionalDelayPowerControl;
    unsigned int IntentionalDelayPowerControl_isUsed:1;
    // PowerReference, powerReferenceType (base: string)
    iso20_ac_der_iec_powerReferenceType PowerReference;
    // HysteresisControl, boolean
    int HysteresisControl;
    // PowerUpRamp, unsignedShort (base: unsignedInt)
    uint16_t PowerUpRamp;
    unsigned int PowerUpRamp_isUsed:1;
    // PT1ResponseActivePower, boolean
    int PT1ResponseActivePower;
    // StepResponseTimeConstantActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantActivePower;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}VoltWatt; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}VoltWattType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PowerReference, powerReferenceType (1, 1); UStart, RationalNumberType (1, 1); UStop, RationalNumberType (1, 1); PT1ResponseActivePower, boolean (1, 1); StepResponseTimeConstantActivePower, RationalNumberType (1, 1); IntentionalDelayPowerControl, unsignedInt (0, 1);
struct iso20_ac_der_iec_VoltWattType {
    // PowerReference, powerReferenceType (base: string)
    iso20_ac_der_iec_powerReferenceType PowerReference;
    // UStart, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType UStart;
    // UStop, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType UStop;
    // PT1ResponseActivePower, boolean
    int PT1ResponseActivePower;
    // StepResponseTimeConstantActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantActivePower;
    // IntentionalDelayPowerControl, unsignedInt (base: unsignedLong)
    uint32_t IntentionalDelayPowerControl;
    unsigned int IntentionalDelayPowerControl_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}OvervoltageFaultRideThrough; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}FaultRideThroughType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: VoltageLimitStartFRT, RationalNumberType (1, 1); VoltageLimitStopFRT, RationalNumberType (0, 1); VoltageRecoveryLimit, RationalNumberType (0, 1); VoltageRideThroughPositiveCurveKFactor, RationalNumberType (0, 1); VoltageRideThroughNegativeCurveKFactor, RationalNumberType (0, 1); PT1ResponseActivePower, boolean (1, 1); StepResponseTimeConstantActivePower, RationalNumberType (1, 1); PT1ResponseReactivePower, boolean (1, 1); StepResponseTimeConstantReactivePower, RationalNumberType (1, 1);
struct iso20_ac_der_iec_FaultRideThroughType {
    // VoltageLimitStartFRT, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType VoltageLimitStartFRT;
    // VoltageLimitStopFRT, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType VoltageLimitStopFRT;
    unsigned int VoltageLimitStopFRT_isUsed:1;
    // VoltageRecoveryLimit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType VoltageRecoveryLimit;
    unsigned int VoltageRecoveryLimit_isUsed:1;
    // VoltageRideThroughPositiveCurveKFactor, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType VoltageRideThroughPositiveCurveKFactor;
    unsigned int VoltageRideThroughPositiveCurveKFactor_isUsed:1;
    // VoltageRideThroughNegativeCurveKFactor, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType VoltageRideThroughNegativeCurveKFactor;
    unsigned int VoltageRideThroughNegativeCurveKFactor_isUsed:1;
    // PT1ResponseActivePower, boolean
    int PT1ResponseActivePower;
    // StepResponseTimeConstantActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantActivePower;
    // PT1ResponseReactivePower, boolean
    int PT1ResponseReactivePower;
    // StepResponseTimeConstantReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantReactivePower;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
struct iso20_ac_der_iec_DetailedCostType {
    // Amount, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType Amount;
    // CostPerUnit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType CostPerUnit;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso20_ac_der_iec_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_ac_der_iec_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_ac_der_iec_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_ac_der_iec_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_ac_der_iec_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}ZeroCurrent; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}ZeroCurrentType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OverVoltageLimit, RationalNumberType (0, 1); UnderVoltageLimit, RationalNumberType (0, 1); OverVoltageRecoveryLimit, RationalNumberType (0, 1); UnderVoltageRecoveryLimit, RationalNumberType (0, 1); PT1ResponseActivePower, boolean (1, 1); StepResponseTimeConstantActivePower, RationalNumberType (1, 1); PT1ResponseReactivePower, boolean (1, 1); StepResponseTimeConstantReactivePower, RationalNumberType (1, 1);
struct iso20_ac_der_iec_ZeroCurrentType {
    // OverVoltageLimit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType OverVoltageLimit;
    unsigned int OverVoltageLimit_isUsed:1;
    // UnderVoltageLimit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType UnderVoltageLimit;
    unsigned int UnderVoltageLimit_isUsed:1;
    // OverVoltageRecoveryLimit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType OverVoltageRecoveryLimit;
    unsigned int OverVoltageRecoveryLimit_isUsed:1;
    // UnderVoltageRecoveryLimit, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType UnderVoltageRecoveryLimit;
    unsigned int UnderVoltageRecoveryLimit_isUsed:1;
    // PT1ResponseActivePower, boolean
    int PT1ResponseActivePower;
    // StepResponseTimeConstantActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantActivePower;
    // PT1ResponseReactivePower, boolean
    int PT1ResponseReactivePower;
    // StepResponseTimeConstantReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantReactivePower;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}ReactivePowerSupport; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}ReactivePowerSupportType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: VoltVar, DERCurveType (0, 1); WattVar, DERCurveType (0, 1); WattCosPhi, DERCurveType (0, 1);
struct iso20_ac_der_iec_ReactivePowerSupportType {
    // VoltVar, DERCurveType
    struct iso20_ac_der_iec_DERCurveType VoltVar;
    unsigned int VoltVar_isUsed:1;
    // WattVar, DERCurveType
    struct iso20_ac_der_iec_DERCurveType WattVar;
    unsigned int WattVar_isUsed:1;
    // WattCosPhi, DERCurveType
    struct iso20_ac_der_iec_DERCurveType WattCosPhi;
    unsigned int WattCosPhi_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}ActivePowerSupport; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}ActivePowerSupportType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: UnderFrequencyWatt, FrequencyWattType (0, 1); OverFrequencyWatt, FrequencyWattType (0, 1); VoltWatt, VoltWattType (0, 1);
struct iso20_ac_der_iec_ActivePowerSupportType {
    // UnderFrequencyWatt, FrequencyWattType
    struct iso20_ac_der_iec_FrequencyWattType UnderFrequencyWatt;
    unsigned int UnderFrequencyWatt_isUsed:1;
    // OverFrequencyWatt, FrequencyWattType
    struct iso20_ac_der_iec_FrequencyWattType OverFrequencyWatt;
    unsigned int OverFrequencyWatt_isUsed:1;
    // VoltWatt, VoltWattType
    struct iso20_ac_der_iec_VoltWattType VoltWatt;
    unsigned int VoltWatt_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
struct iso20_ac_der_iec_DetailedTaxType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // Amount, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType Amount;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_ac_der_iec_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_ac_der_iec_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_ac_der_iec_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_ac_der_iec_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_ac_der_iec_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_CPDReqEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParameters; type={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParametersType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PresentSOC, percentValueType (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); MaximumSOC, percentValueType (0, 1); RemainingTimeToMinimumSOC, unsignedInt (0, 1); RemainingTimeToTargetSOC, unsignedInt (0, 1); RemainingTimeToMaximumSOC, unsignedInt (0, 1); ChargingComplete, boolean (0, 1); BatteryEnergyCapacity, RationalNumberType (0, 1); InletHot, boolean (0, 1);
struct iso20_ac_der_iec_DisplayParametersType {
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
    struct iso20_ac_der_iec_RationalNumberType BatteryEnergyCapacity;
    unsigned int BatteryEnergyCapacity_isUsed:1;
    // InletHot, boolean
    int InletHot;
    unsigned int InletHot_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_CPDResEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower_L2, RationalNumberType (0, 1); EVSEMinimumChargePower_L3, RationalNumberType (0, 1); EVSENominalFrequency, RationalNumberType (1, 1); MaximumPowerAsymmetry, RationalNumberType (0, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_AC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower;
    // EVSEMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L2;
    unsigned int EVSEMinimumChargePower_L2_isUsed:1;
    // EVSEMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L3;
    unsigned int EVSEMinimumChargePower_L3_isUsed:1;
    // EVSENominalFrequency, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalFrequency;
    // MaximumPowerAsymmetry, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType MaximumPowerAsymmetry;
    unsigned int MaximumPowerAsymmetry_isUsed:1;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
struct iso20_ac_der_iec_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, evseNotificationType (base: string)
    iso20_ac_der_iec_evseNotificationType EVSENotification;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}Dynamic_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}Dynamic_AC_CLReqControlModeType; base type=Dynamic_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (1, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_Dynamic_AC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}Scheduled_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}Scheduled_AC_CLReqControlModeType; base type=Scheduled_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (0, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_Scheduled_AC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    unsigned int EVPresentReactivePower_isUsed:1;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_ac_der_iec_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
struct iso20_ac_der_iec_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso20_ac_der_iec_MeterID_CHARACTER_SIZE];
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
        uint8_t bytes[iso20_ac_der_iec_meterSignatureType_BYTES_SIZE];
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
struct iso20_ac_der_iec_ReceiptType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EnergyCosts, DetailedCostType
    struct iso20_ac_der_iec_DetailedCostType EnergyCosts;
    unsigned int EnergyCosts_isUsed:1;
    // OccupancyCosts, DetailedCostType
    struct iso20_ac_der_iec_DetailedCostType OccupancyCosts;
    unsigned int OccupancyCosts_isUsed:1;
    // AdditionalServicesCosts, DetailedCostType
    struct iso20_ac_der_iec_DetailedCostType AdditionalServicesCosts;
    unsigned int AdditionalServicesCosts_isUsed:1;
    // OverstayCosts, DetailedCostType
    struct iso20_ac_der_iec_DetailedCostType OverstayCosts;
    unsigned int OverstayCosts_isUsed:1;
    // TaxCosts, DetailedTaxType
    struct {
        struct iso20_ac_der_iec_DetailedTaxType array[iso20_ac_der_iec_DetailedTaxType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxCosts;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}Dynamic_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}Dynamic_AC_CLResControlModeType; base type=Dynamic_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSETargetActivePower, RationalNumberType (1, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_Dynamic_AC_CLResControlModeType {
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
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}Scheduled_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}Scheduled_AC_CLResControlModeType; base type=Scheduled_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSETargetActivePower, RationalNumberType (0, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_Scheduled_AC_CLResControlModeType {
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    unsigned int EVSETargetActivePower_isUsed:1;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_ac_der_iec_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}EVReactivePowerLimits; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}EVReactivePowerLimitsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVMaximumChargeReactivePower, RationalNumberType (1, 1); EVMaximumChargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumChargeReactivePower_L3, RationalNumberType (0, 1); EVMinimumChargeReactivePower, RationalNumberType (0, 1); EVMinimumChargeReactivePower_L2, RationalNumberType (0, 1); EVMinimumChargeReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargeReactivePower, RationalNumberType (1, 1); EVMaximumDischargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumDischargeReactivePower_L3, RationalNumberType (0, 1); EVMinimumDischargeReactivePower, RationalNumberType (0, 1); EVMinimumDischargeReactivePower_L2, RationalNumberType (0, 1); EVMinimumDischargeReactivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_EVReactivePowerLimitsType {
    // EVMaximumChargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower;
    // EVMaximumChargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L2;
    unsigned int EVMaximumChargeReactivePower_L2_isUsed:1;
    // EVMaximumChargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L3;
    unsigned int EVMaximumChargeReactivePower_L3_isUsed:1;
    // EVMinimumChargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargeReactivePower;
    unsigned int EVMinimumChargeReactivePower_isUsed:1;
    // EVMinimumChargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargeReactivePower_L2;
    unsigned int EVMinimumChargeReactivePower_L2_isUsed:1;
    // EVMinimumChargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargeReactivePower_L3;
    unsigned int EVMinimumChargeReactivePower_L3_isUsed:1;
    // EVMaximumDischargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower;
    // EVMaximumDischargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L2;
    unsigned int EVMaximumDischargeReactivePower_L2_isUsed:1;
    // EVMaximumDischargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L3;
    unsigned int EVMaximumDischargeReactivePower_L3_isUsed:1;
    // EVMinimumDischargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargeReactivePower;
    unsigned int EVMinimumDischargeReactivePower_isUsed:1;
    // EVMinimumDischargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargeReactivePower_L2;
    unsigned int EVMinimumDischargeReactivePower_L2_isUsed:1;
    // EVMinimumDischargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargeReactivePower_L3;
    unsigned int EVMinimumDischargeReactivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DSOQSetpoint; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DSOQSetpointType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DSOQSetpointValue, RationalNumberType (1, 1); DSOQSetpointValue_L2, RationalNumberType (0, 1); DSOQSetpointValue_L3, RationalNumberType (0, 1); PT1ResponseReactivePower, boolean (1, 1); StepResponseTimeConstantReactivePower, RationalNumberType (1, 1);
struct iso20_ac_der_iec_DSOQSetpointType {
    // DSOQSetpointValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOQSetpointValue;
    // DSOQSetpointValue_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOQSetpointValue_L2;
    unsigned int DSOQSetpointValue_L2_isUsed:1;
    // DSOQSetpointValue_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOQSetpointValue_L3;
    unsigned int DSOQSetpointValue_L3_isUsed:1;
    // PT1ResponseReactivePower, boolean
    int PT1ResponseReactivePower;
    // StepResponseTimeConstantReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantReactivePower;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DERControl; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DERControlType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OvervoltageFaultRideThrough, FaultRideThroughType (0, 1); UndervoltageFaultRideThrough, FaultRideThroughType (0, 1); ZeroCurrent, ZeroCurrentType (0, 1); ReactivePowerSupport, ReactivePowerSupportType (0, 1); ActivePowerSupport, ActivePowerSupportType (0, 1); MaximumLevelDCInjection, RationalNumberType (0, 1);
struct iso20_ac_der_iec_DERControlType {
    // OvervoltageFaultRideThrough, FaultRideThroughType
    struct iso20_ac_der_iec_FaultRideThroughType OvervoltageFaultRideThrough;
    unsigned int OvervoltageFaultRideThrough_isUsed:1;
    // UndervoltageFaultRideThrough, FaultRideThroughType
    struct iso20_ac_der_iec_FaultRideThroughType UndervoltageFaultRideThrough;
    unsigned int UndervoltageFaultRideThrough_isUsed:1;
    // ZeroCurrent, ZeroCurrentType
    struct iso20_ac_der_iec_ZeroCurrentType ZeroCurrent;
    unsigned int ZeroCurrent_isUsed:1;
    // ReactivePowerSupport, ReactivePowerSupportType
    struct iso20_ac_der_iec_ReactivePowerSupportType ReactivePowerSupport;
    unsigned int ReactivePowerSupport_isUsed:1;
    // ActivePowerSupport, ActivePowerSupportType
    struct iso20_ac_der_iec_ActivePowerSupportType ActivePowerSupport;
    unsigned int ActivePowerSupport_isUsed:1;
    // MaximumLevelDCInjection, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType MaximumLevelDCInjection;
    unsigned int MaximumLevelDCInjection_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DSOCosPhiSetpoint; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DSOCosPhiSetpointType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DSOCosPhiSetpointValue, RationalNumberType (1, 1); DSOCosPhiSetpointValue_L2, RationalNumberType (0, 1); DSOCosPhiSetpointValue_L3, RationalNumberType (0, 1); Excitation, powerFactorExcitationType (1, 1); PT1ResponseReactivePower, boolean (1, 1); StepResponseTimeConstantReactivePower, RationalNumberType (1, 1);
struct iso20_ac_der_iec_DSOCosPhiSetpointType {
    // DSOCosPhiSetpointValue, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOCosPhiSetpointValue;
    // DSOCosPhiSetpointValue_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOCosPhiSetpointValue_L2;
    unsigned int DSOCosPhiSetpointValue_L2_isUsed:1;
    // DSOCosPhiSetpointValue_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOCosPhiSetpointValue_L3;
    unsigned int DSOCosPhiSetpointValue_L3_isUsed:1;
    // Excitation, powerFactorExcitationType (base: string)
    iso20_ac_der_iec_powerFactorExcitationType Excitation;
    // PT1ResponseReactivePower, boolean
    int PT1ResponseReactivePower;
    // StepResponseTimeConstantReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType StepResponseTimeConstantReactivePower;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_AC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_AC_CPDReqEnergyTransferModeType; base type=AC_CPDReqEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_AC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_AC_CPDResEnergyTransferModeType; base type=AC_CPDResEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower_L2, RationalNumberType (0, 1); EVSEMinimumChargePower_L3, RationalNumberType (0, 1); EVSENominalFrequency, RationalNumberType (1, 1); MaximumPowerAsymmetry, RationalNumberType (0, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargePower_L2, RationalNumberType (0, 1); EVSEMaximumDischargePower_L3, RationalNumberType (0, 1); EVSEMinimumDischargePower, RationalNumberType (1, 1); EVSEMinimumDischargePower_L2, RationalNumberType (0, 1); EVSEMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower;
    // EVSEMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L2;
    unsigned int EVSEMinimumChargePower_L2_isUsed:1;
    // EVSEMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L3;
    unsigned int EVSEMinimumChargePower_L3_isUsed:1;
    // EVSENominalFrequency, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalFrequency;
    // MaximumPowerAsymmetry, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType MaximumPowerAsymmetry;
    unsigned int MaximumPowerAsymmetry_isUsed:1;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L2;
    unsigned int EVSEMaximumDischargePower_L2_isUsed:1;
    // EVSEMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L3;
    unsigned int EVSEMaximumDischargePower_L3_isUsed:1;
    // EVSEMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumDischargePower;
    // EVSEMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumDischargePower_L2;
    unsigned int EVSEMinimumDischargePower_L2_isUsed:1;
    // EVSEMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumDischargePower_L3;
    unsigned int EVSEMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_AC_CPDReqEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_AC_CPDReqEnergyTransferModeType; base type=AC_CPDReqEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVProcessing, processingType (1, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1); EVSessionTotalDischargeEnergyAvailable, RationalNumberType (0, 1); EVReactivePowerLimits, EVReactivePowerLimitsType (0, 1);
struct iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType {
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVProcessing, processingType (base: string)
    iso20_ac_der_iec_processingType EVProcessing;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;
    // EVSessionTotalDischargeEnergyAvailable, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSessionTotalDischargeEnergyAvailable;
    unsigned int EVSessionTotalDischargeEnergyAvailable_isUsed:1;
    // EVReactivePowerLimits, EVReactivePowerLimitsType
    struct iso20_ac_der_iec_EVReactivePowerLimitsType EVReactivePowerLimits;
    unsigned int EVReactivePowerLimits_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); AC_CPDReqEnergyTransferMode, AC_CPDReqEnergyTransferModeType (0, 1); BPT_AC_CPDReqEnergyTransferMode, BPT_AC_CPDReqEnergyTransferModeType (0, 1); DER_AC_CPDReqEnergyTransferMode, DER_AC_CPDReqEnergyTransferModeType (0, 1);
struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType {
    // Header, MessageHeaderType
    struct iso20_ac_der_iec_MessageHeaderType Header;
    // AC_CPDReqEnergyTransferMode, AC_CPDReqEnergyTransferModeType
    struct iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType AC_CPDReqEnergyTransferMode;
    unsigned int AC_CPDReqEnergyTransferMode_isUsed:1;
    // BPT_AC_CPDReqEnergyTransferMode, BPT_AC_CPDReqEnergyTransferModeType (base: AC_CPDReqEnergyTransferModeType)
    struct iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType BPT_AC_CPDReqEnergyTransferMode;
    unsigned int BPT_AC_CPDReqEnergyTransferMode_isUsed:1;
    // DER_AC_CPDReqEnergyTransferMode, DER_AC_CPDReqEnergyTransferModeType (base: AC_CPDReqEnergyTransferModeType)
    struct iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType DER_AC_CPDReqEnergyTransferMode;
    unsigned int DER_AC_CPDReqEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_AC_CPDResEnergyTransferMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_AC_CPDResEnergyTransferModeType; base type=AC_CPDResEnergyTransferModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMinimumChargePower, RationalNumberType (1, 1); EVSEMinimumChargePower_L2, RationalNumberType (0, 1); EVSEMinimumChargePower_L3, RationalNumberType (0, 1); EVSENominalFrequency, RationalNumberType (1, 1); MaximumPowerAsymmetry, RationalNumberType (0, 1); EVSEPowerRampLimitation, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1); EVSENominalChargePower, RationalNumberType (1, 1); EVSENominalChargePower_L2, RationalNumberType (0, 1); EVSENominalChargePower_L3, RationalNumberType (0, 1); EVSENominalDischargePower, RationalNumberType (1, 1); EVSENominalDischargePower_L2, RationalNumberType (0, 1); EVSENominalDischargePower_L3, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargePower_L2, RationalNumberType (0, 1); EVSEMaximumDischargePower_L3, RationalNumberType (0, 1); EVOperatingMode, evOperatingModeType (1, 1); GridConnectionMode, gridConnectionModeType (1, 1); DERControl, DERControlType (1, 1);
struct iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType {
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower;
    // EVSEMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L2;
    unsigned int EVSEMinimumChargePower_L2_isUsed:1;
    // EVSEMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMinimumChargePower_L3;
    unsigned int EVSEMinimumChargePower_L3_isUsed:1;
    // EVSENominalFrequency, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalFrequency;
    // MaximumPowerAsymmetry, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType MaximumPowerAsymmetry;
    unsigned int MaximumPowerAsymmetry_isUsed:1;
    // EVSEPowerRampLimitation, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPowerRampLimitation;
    unsigned int EVSEPowerRampLimitation_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;
    // EVSENominalChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalChargePower;
    // EVSENominalChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalChargePower_L2;
    unsigned int EVSENominalChargePower_L2_isUsed:1;
    // EVSENominalChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalChargePower_L3;
    unsigned int EVSENominalChargePower_L3_isUsed:1;
    // EVSENominalDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalDischargePower;
    // EVSENominalDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalDischargePower_L2;
    unsigned int EVSENominalDischargePower_L2_isUsed:1;
    // EVSENominalDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSENominalDischargePower_L3;
    unsigned int EVSENominalDischargePower_L3_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L2;
    unsigned int EVSEMaximumDischargePower_L2_isUsed:1;
    // EVSEMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L3;
    unsigned int EVSEMaximumDischargePower_L3_isUsed:1;
    // EVOperatingMode, evOperatingModeType (base: string)
    iso20_ac_der_iec_evOperatingModeType EVOperatingMode;
    // GridConnectionMode, gridConnectionModeType (base: string)
    iso20_ac_der_iec_gridConnectionModeType GridConnectionMode;
    // DERControl, DERControlType
    struct iso20_ac_der_iec_DERControlType DERControl;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); AC_CPDResEnergyTransferMode, AC_CPDResEnergyTransferModeType (0, 1); BPT_AC_CPDResEnergyTransferMode, BPT_AC_CPDResEnergyTransferModeType (0, 1); DER_AC_CPDResEnergyTransferMode, DER_AC_CPDResEnergyTransferModeType (0, 1);
struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType {
    // Header, MessageHeaderType
    struct iso20_ac_der_iec_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_ac_der_iec_responseCodeType ResponseCode;
    // AC_CPDResEnergyTransferMode, AC_CPDResEnergyTransferModeType
    struct iso20_ac_der_iec_AC_CPDResEnergyTransferModeType AC_CPDResEnergyTransferMode;
    unsigned int AC_CPDResEnergyTransferMode_isUsed:1;
    // BPT_AC_CPDResEnergyTransferMode, BPT_AC_CPDResEnergyTransferModeType (base: AC_CPDResEnergyTransferModeType)
    struct iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType BPT_AC_CPDResEnergyTransferMode;
    unsigned int BPT_AC_CPDResEnergyTransferMode_isUsed:1;
    // DER_AC_CPDResEnergyTransferMode, DER_AC_CPDResEnergyTransferModeType (base: AC_CPDResEnergyTransferModeType)
    struct iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType DER_AC_CPDResEnergyTransferMode;
    unsigned int DER_AC_CPDResEnergyTransferMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Scheduled_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Scheduled_AC_CLReqControlModeType; base type=Scheduled_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (0, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (0, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (0, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    unsigned int EVPresentReactivePower_isUsed:1;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    unsigned int EVMaximumDischargePower_isUsed:1;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    unsigned int EVMinimumDischargePower_isUsed:1;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Scheduled_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Scheduled_AC_CLResControlModeType; base type=Scheduled_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSETargetActivePower, RationalNumberType (0, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlModeType {
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    unsigned int EVSETargetActivePower_isUsed:1;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Dynamic_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Dynamic_AC_CLReqControlModeType; base type=Dynamic_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (1, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;
    // EVMaximumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumV2XEnergyRequest;
    unsigned int EVMaximumV2XEnergyRequest_isUsed:1;
    // EVMinimumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumV2XEnergyRequest;
    unsigned int EVMinimumV2XEnergyRequest_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Dynamic_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}BPT_Dynamic_AC_CLResControlModeType; base type=Dynamic_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSETargetActivePower, RationalNumberType (1, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1);
struct iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlModeType {
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
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Dynamic_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Dynamic_AC_CLReqControlModeType; base type=Dynamic_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumChargePower, RationalNumberType (1, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (1, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (1, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1); EVMaximumChargeReactivePower, RationalNumberType (0, 1); EVMaximumChargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumChargeReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargeReactivePower, RationalNumberType (0, 1); EVMaximumDischargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumDischargeReactivePower_L3, RationalNumberType (0, 1); GridEventCondition, unsignedByte (1, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1); EVSessionTotalDischargeEnergyAvailable, RationalNumberType (0, 1);
struct iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;
    // EVMaximumChargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower;
    unsigned int EVMaximumChargeReactivePower_isUsed:1;
    // EVMaximumChargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L2;
    unsigned int EVMaximumChargeReactivePower_L2_isUsed:1;
    // EVMaximumChargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L3;
    unsigned int EVMaximumChargeReactivePower_L3_isUsed:1;
    // EVMaximumDischargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower;
    unsigned int EVMaximumDischargeReactivePower_isUsed:1;
    // EVMaximumDischargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L2;
    unsigned int EVMaximumDischargeReactivePower_L2_isUsed:1;
    // EVMaximumDischargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L3;
    unsigned int EVMaximumDischargeReactivePower_L3_isUsed:1;
    // GridEventCondition, unsignedByte (base: unsignedShort)
    uint8_t GridEventCondition;
    // EVMaximumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumV2XEnergyRequest;
    unsigned int EVMaximumV2XEnergyRequest_isUsed:1;
    // EVMinimumV2XEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumV2XEnergyRequest;
    unsigned int EVMinimumV2XEnergyRequest_isUsed:1;
    // EVSessionTotalDischargeEnergyAvailable, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSessionTotalDischargeEnergyAvailable;
    unsigned int EVSessionTotalDischargeEnergyAvailable_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Dynamic_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Dynamic_AC_CLResControlModeType; base type=Dynamic_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AckMaxDelay, unsignedShort (0, 1); EVSETargetActivePower, RationalNumberType (1, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargePower_L2, RationalNumberType (0, 1); EVSEMaximumDischargePower_L3, RationalNumberType (0, 1); DSOMaximumDischargePower, RationalNumberType (0, 1); DSOMaximumDischargePower_L2, RationalNumberType (0, 1); DSOMaximumDischargePower_L3, RationalNumberType (0, 1); DSOQSetpoint, DSOQSetpointType (0, 1); DSOCosPhiSetpoint, DSOCosPhiSetpointType (0, 1);
struct iso20_ac_der_iec_DER_Dynamic_AC_CLResControlModeType {
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
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L2;
    unsigned int EVSEMaximumDischargePower_L2_isUsed:1;
    // EVSEMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L3;
    unsigned int EVSEMaximumDischargePower_L3_isUsed:1;
    // DSOMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower;
    unsigned int DSOMaximumDischargePower_isUsed:1;
    // DSOMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower_L2;
    unsigned int DSOMaximumDischargePower_L2_isUsed:1;
    // DSOMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower_L3;
    unsigned int DSOMaximumDischargePower_L3_isUsed:1;
    // DSOQSetpoint, DSOQSetpointType
    struct iso20_ac_der_iec_DSOQSetpointType DSOQSetpoint;
    unsigned int DSOQSetpoint_isUsed:1;
    // DSOCosPhiSetpoint, DSOCosPhiSetpointType
    struct iso20_ac_der_iec_DSOCosPhiSetpointType DSOCosPhiSetpoint;
    unsigned int DSOCosPhiSetpoint_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Scheduled_AC_CLReqControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Scheduled_AC_CLReqControlModeType; base type=Scheduled_AC_CLReqControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVMaximumChargePower, RationalNumberType (0, 1); EVMaximumChargePower_L2, RationalNumberType (0, 1); EVMaximumChargePower_L3, RationalNumberType (0, 1); EVMinimumChargePower, RationalNumberType (0, 1); EVMinimumChargePower_L2, RationalNumberType (0, 1); EVMinimumChargePower_L3, RationalNumberType (0, 1); EVPresentActivePower, RationalNumberType (1, 1); EVPresentActivePower_L2, RationalNumberType (0, 1); EVPresentActivePower_L3, RationalNumberType (0, 1); EVPresentReactivePower, RationalNumberType (0, 1); EVPresentReactivePower_L2, RationalNumberType (0, 1); EVPresentReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargePower, RationalNumberType (1, 1); EVMaximumDischargePower_L2, RationalNumberType (0, 1); EVMaximumDischargePower_L3, RationalNumberType (0, 1); EVMinimumDischargePower, RationalNumberType (1, 1); EVMinimumDischargePower_L2, RationalNumberType (0, 1); EVMinimumDischargePower_L3, RationalNumberType (0, 1); EVMaximumChargeReactivePower, RationalNumberType (0, 1); EVMaximumChargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumChargeReactivePower_L3, RationalNumberType (0, 1); EVMaximumDischargeReactivePower, RationalNumberType (0, 1); EVMaximumDischargeReactivePower_L2, RationalNumberType (0, 1); EVMaximumDischargeReactivePower_L3, RationalNumberType (0, 1); GridEventCondition, unsignedByte (1, 1);
struct iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlModeType {
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower;
    unsigned int EVMaximumChargePower_isUsed:1;
    // EVMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L2;
    unsigned int EVMaximumChargePower_L2_isUsed:1;
    // EVMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargePower_L3;
    unsigned int EVMaximumChargePower_L3_isUsed:1;
    // EVMinimumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower;
    unsigned int EVMinimumChargePower_isUsed:1;
    // EVMinimumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L2;
    unsigned int EVMinimumChargePower_L2_isUsed:1;
    // EVMinimumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumChargePower_L3;
    unsigned int EVMinimumChargePower_L3_isUsed:1;
    // EVPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower;
    // EVPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L2;
    unsigned int EVPresentActivePower_L2_isUsed:1;
    // EVPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentActivePower_L3;
    unsigned int EVPresentActivePower_L3_isUsed:1;
    // EVPresentReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower;
    unsigned int EVPresentReactivePower_isUsed:1;
    // EVPresentReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L2;
    unsigned int EVPresentReactivePower_L2_isUsed:1;
    // EVPresentReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVPresentReactivePower_L3;
    unsigned int EVPresentReactivePower_L3_isUsed:1;
    // EVMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower;
    // EVMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L2;
    unsigned int EVMaximumDischargePower_L2_isUsed:1;
    // EVMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargePower_L3;
    unsigned int EVMaximumDischargePower_L3_isUsed:1;
    // EVMinimumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower;
    // EVMinimumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L2;
    unsigned int EVMinimumDischargePower_L2_isUsed:1;
    // EVMinimumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMinimumDischargePower_L3;
    unsigned int EVMinimumDischargePower_L3_isUsed:1;
    // EVMaximumChargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower;
    unsigned int EVMaximumChargeReactivePower_isUsed:1;
    // EVMaximumChargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L2;
    unsigned int EVMaximumChargeReactivePower_L2_isUsed:1;
    // EVMaximumChargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumChargeReactivePower_L3;
    unsigned int EVMaximumChargeReactivePower_L3_isUsed:1;
    // EVMaximumDischargeReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower;
    unsigned int EVMaximumDischargeReactivePower_isUsed:1;
    // EVMaximumDischargeReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L2;
    unsigned int EVMaximumDischargeReactivePower_L2_isUsed:1;
    // EVMaximumDischargeReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVMaximumDischargeReactivePower_L3;
    unsigned int EVMaximumDischargeReactivePower_L3_isUsed:1;
    // GridEventCondition, unsignedByte (base: unsignedShort)
    uint8_t GridEventCondition;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); BPT_Dynamic_AC_CLReqControlMode, BPT_Dynamic_AC_CLReqControlModeType (0, 1); BPT_Scheduled_AC_CLReqControlMode, BPT_Scheduled_AC_CLReqControlModeType (0, 1); CLReqControlMode, CLReqControlModeType (0, 1); DER_Dynamic_AC_CLReqControlMode, DER_Dynamic_AC_CLReqControlModeType (0, 1); DER_Scheduled_AC_CLReqControlMode, DER_Scheduled_AC_CLReqControlModeType (0, 1); Dynamic_AC_CLReqControlMode, Dynamic_AC_CLReqControlModeType (0, 1); Scheduled_AC_CLReqControlMode, Scheduled_AC_CLReqControlModeType (0, 1);
struct iso20_ac_der_iec_AC_ChargeLoopReqType {
    // Header, MessageHeaderType
    struct iso20_ac_der_iec_MessageHeaderType Header;
    // DisplayParameters, DisplayParametersType
    struct iso20_ac_der_iec_DisplayParametersType DisplayParameters;
    unsigned int DisplayParameters_isUsed:1;
    // MeterInfoRequested, boolean
    int MeterInfoRequested;
    // BPT_Dynamic_AC_CLReqControlMode, BPT_Dynamic_AC_CLReqControlModeType (base: Dynamic_AC_CLReqControlModeType)
    struct iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlModeType BPT_Dynamic_AC_CLReqControlMode;
    unsigned int BPT_Dynamic_AC_CLReqControlMode_isUsed:1;
    // BPT_Scheduled_AC_CLReqControlMode, BPT_Scheduled_AC_CLReqControlModeType (base: Scheduled_AC_CLReqControlModeType)
    struct iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlModeType BPT_Scheduled_AC_CLReqControlMode;
    unsigned int BPT_Scheduled_AC_CLReqControlMode_isUsed:1;
    // CLReqControlMode, CLReqControlModeType
    struct iso20_ac_der_iec_CLReqControlModeType CLReqControlMode;
    unsigned int CLReqControlMode_isUsed:1;
    // DER_Dynamic_AC_CLReqControlMode, DER_Dynamic_AC_CLReqControlModeType (base: Dynamic_AC_CLReqControlModeType)
    struct iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlModeType DER_Dynamic_AC_CLReqControlMode;
    unsigned int DER_Dynamic_AC_CLReqControlMode_isUsed:1;
    // DER_Scheduled_AC_CLReqControlMode, DER_Scheduled_AC_CLReqControlModeType (base: Scheduled_AC_CLReqControlModeType)
    struct iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlModeType DER_Scheduled_AC_CLReqControlMode;
    unsigned int DER_Scheduled_AC_CLReqControlMode_isUsed:1;
    // Dynamic_AC_CLReqControlMode, Dynamic_AC_CLReqControlModeType (base: Dynamic_CLReqControlModeType)
    struct iso20_ac_der_iec_Dynamic_AC_CLReqControlModeType Dynamic_AC_CLReqControlMode;
    unsigned int Dynamic_AC_CLReqControlMode_isUsed:1;
    // Scheduled_AC_CLReqControlMode, Scheduled_AC_CLReqControlModeType (base: Scheduled_CLReqControlModeType)
    struct iso20_ac_der_iec_Scheduled_AC_CLReqControlModeType Scheduled_AC_CLReqControlMode;
    unsigned int Scheduled_AC_CLReqControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Scheduled_AC_CLResControlMode; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}DER_Scheduled_AC_CLResControlModeType; base type=Scheduled_AC_CLResControlModeType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSETargetActivePower, RationalNumberType (0, 1); EVSETargetActivePower_L2, RationalNumberType (0, 1); EVSETargetActivePower_L3, RationalNumberType (0, 1); EVSETargetReactivePower, RationalNumberType (0, 1); EVSETargetReactivePower_L2, RationalNumberType (0, 1); EVSETargetReactivePower_L3, RationalNumberType (0, 1); EVSEPresentActivePower, RationalNumberType (0, 1); EVSEPresentActivePower_L2, RationalNumberType (0, 1); EVSEPresentActivePower_L3, RationalNumberType (0, 1); EVSEMaximumChargePower, RationalNumberType (1, 1); EVSEMaximumChargePower_L2, RationalNumberType (0, 1); EVSEMaximumChargePower_L3, RationalNumberType (0, 1); EVSEMaximumDischargePower, RationalNumberType (1, 1); EVSEMaximumDischargePower_L2, RationalNumberType (0, 1); EVSEMaximumDischargePower_L3, RationalNumberType (0, 1); DSOMaximumDischargePower, RationalNumberType (0, 1); DSOMaximumDischargePower_L2, RationalNumberType (0, 1); DSOMaximumDischargePower_L3, RationalNumberType (0, 1); DSOQSetpoint, DSOQSetpointType (0, 1); DSOCosPhiSetpoint, DSOCosPhiSetpointType (0, 1);
struct iso20_ac_der_iec_DER_Scheduled_AC_CLResControlModeType {
    // EVSETargetActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower;
    unsigned int EVSETargetActivePower_isUsed:1;
    // EVSETargetActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L2;
    unsigned int EVSETargetActivePower_L2_isUsed:1;
    // EVSETargetActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetActivePower_L3;
    unsigned int EVSETargetActivePower_L3_isUsed:1;
    // EVSETargetReactivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower;
    unsigned int EVSETargetReactivePower_isUsed:1;
    // EVSETargetReactivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L2;
    unsigned int EVSETargetReactivePower_L2_isUsed:1;
    // EVSETargetReactivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetReactivePower_L3;
    unsigned int EVSETargetReactivePower_L3_isUsed:1;
    // EVSEPresentActivePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower;
    unsigned int EVSEPresentActivePower_isUsed:1;
    // EVSEPresentActivePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L2;
    unsigned int EVSEPresentActivePower_L2_isUsed:1;
    // EVSEPresentActivePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEPresentActivePower_L3;
    unsigned int EVSEPresentActivePower_L3_isUsed:1;
    // EVSEMaximumChargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower;
    // EVSEMaximumChargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L2;
    unsigned int EVSEMaximumChargePower_L2_isUsed:1;
    // EVSEMaximumChargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumChargePower_L3;
    unsigned int EVSEMaximumChargePower_L3_isUsed:1;
    // EVSEMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower;
    // EVSEMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L2;
    unsigned int EVSEMaximumDischargePower_L2_isUsed:1;
    // EVSEMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSEMaximumDischargePower_L3;
    unsigned int EVSEMaximumDischargePower_L3_isUsed:1;
    // DSOMaximumDischargePower, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower;
    unsigned int DSOMaximumDischargePower_isUsed:1;
    // DSOMaximumDischargePower_L2, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower_L2;
    unsigned int DSOMaximumDischargePower_L2_isUsed:1;
    // DSOMaximumDischargePower_L3, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType DSOMaximumDischargePower_L3;
    unsigned int DSOMaximumDischargePower_L3_isUsed:1;
    // DSOQSetpoint, DSOQSetpointType
    struct iso20_ac_der_iec_DSOQSetpointType DSOQSetpoint;
    unsigned int DSOQSetpoint_isUsed:1;
    // DSOCosPhiSetpoint, DSOCosPhiSetpointType
    struct iso20_ac_der_iec_DSOCosPhiSetpointType DSOCosPhiSetpoint;
    unsigned int DSOCosPhiSetpoint_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:AC-DER-IEC}AC_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVSETargetFrequency, RationalNumberType (0, 1); BPT_Dynamic_AC_CLResControlMode, BPT_Dynamic_AC_CLResControlModeType (0, 1); BPT_Scheduled_AC_CLResControlMode, BPT_Scheduled_AC_CLResControlModeType (0, 1); CLResControlMode, CLResControlModeType (0, 1); DER_Dynamic_AC_CLResControlMode, DER_Dynamic_AC_CLResControlModeType (0, 1); DER_Scheduled_AC_CLResControlMode, DER_Scheduled_AC_CLResControlModeType (0, 1); Dynamic_AC_CLResControlMode, Dynamic_AC_CLResControlModeType (0, 1); Scheduled_AC_CLResControlMode, Scheduled_AC_CLResControlModeType (0, 1);
struct iso20_ac_der_iec_AC_ChargeLoopResType {
    // Header, MessageHeaderType
    struct iso20_ac_der_iec_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_ac_der_iec_responseCodeType ResponseCode;
    // EVSEStatus, EVSEStatusType
    struct iso20_ac_der_iec_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso20_ac_der_iec_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // Receipt, ReceiptType
    struct iso20_ac_der_iec_ReceiptType Receipt;
    unsigned int Receipt_isUsed:1;
    // EVSETargetFrequency, RationalNumberType
    struct iso20_ac_der_iec_RationalNumberType EVSETargetFrequency;
    unsigned int EVSETargetFrequency_isUsed:1;
    // BPT_Dynamic_AC_CLResControlMode, BPT_Dynamic_AC_CLResControlModeType (base: Dynamic_AC_CLResControlModeType)
    struct iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlModeType BPT_Dynamic_AC_CLResControlMode;
    unsigned int BPT_Dynamic_AC_CLResControlMode_isUsed:1;
    // BPT_Scheduled_AC_CLResControlMode, BPT_Scheduled_AC_CLResControlModeType (base: Scheduled_AC_CLResControlModeType)
    struct iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlModeType BPT_Scheduled_AC_CLResControlMode;
    unsigned int BPT_Scheduled_AC_CLResControlMode_isUsed:1;
    // CLResControlMode, CLResControlModeType
    struct iso20_ac_der_iec_CLResControlModeType CLResControlMode;
    unsigned int CLResControlMode_isUsed:1;
    // DER_Dynamic_AC_CLResControlMode, DER_Dynamic_AC_CLResControlModeType (base: Dynamic_AC_CLResControlModeType)
    struct iso20_ac_der_iec_DER_Dynamic_AC_CLResControlModeType DER_Dynamic_AC_CLResControlMode;
    unsigned int DER_Dynamic_AC_CLResControlMode_isUsed:1;
    // DER_Scheduled_AC_CLResControlMode, DER_Scheduled_AC_CLResControlModeType (base: Scheduled_AC_CLResControlModeType)
    struct iso20_ac_der_iec_DER_Scheduled_AC_CLResControlModeType DER_Scheduled_AC_CLResControlMode;
    unsigned int DER_Scheduled_AC_CLResControlMode_isUsed:1;
    // Dynamic_AC_CLResControlMode, Dynamic_AC_CLResControlModeType (base: Dynamic_CLResControlModeType)
    struct iso20_ac_der_iec_Dynamic_AC_CLResControlModeType Dynamic_AC_CLResControlMode;
    unsigned int Dynamic_AC_CLResControlMode_isUsed:1;
    // Scheduled_AC_CLResControlMode, Scheduled_AC_CLResControlModeType (base: Scheduled_CLResControlModeType)
    struct iso20_ac_der_iec_Scheduled_AC_CLResControlModeType Scheduled_AC_CLResControlMode;
    unsigned int Scheduled_AC_CLResControlMode_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_ac_der_iec_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_ac_der_iec_ReferenceType array[iso20_ac_der_iec_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
struct iso20_ac_der_iec_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_ac_der_iec_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_ac_der_iec_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_ac_der_iec_exiDocument {
    union {
        struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType AC_ChargeParameterDiscoveryReq;
        struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType AC_ChargeParameterDiscoveryRes;
        struct iso20_ac_der_iec_AC_ChargeLoopReqType AC_ChargeLoopReq;
        struct iso20_ac_der_iec_AC_ChargeLoopResType AC_ChargeLoopRes;
        struct iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType AC_CPDReqEnergyTransferMode;
        struct iso20_ac_der_iec_AC_CPDResEnergyTransferModeType AC_CPDResEnergyTransferMode;
        struct iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType BPT_AC_CPDReqEnergyTransferMode;
        struct iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType BPT_AC_CPDResEnergyTransferMode;
        struct iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType DER_AC_CPDReqEnergyTransferMode;
        struct iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType DER_AC_CPDResEnergyTransferMode;
        struct iso20_ac_der_iec_CLReqControlModeType CLReqControlMode;
        struct iso20_ac_der_iec_Scheduled_AC_CLReqControlModeType Scheduled_AC_CLReqControlMode;
        struct iso20_ac_der_iec_CLResControlModeType CLResControlMode;
        struct iso20_ac_der_iec_Scheduled_AC_CLResControlModeType Scheduled_AC_CLResControlMode;
        struct iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlModeType BPT_Scheduled_AC_CLReqControlMode;
        struct iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlModeType BPT_Scheduled_AC_CLResControlMode;
        struct iso20_ac_der_iec_Dynamic_AC_CLReqControlModeType Dynamic_AC_CLReqControlMode;
        struct iso20_ac_der_iec_Dynamic_AC_CLResControlModeType Dynamic_AC_CLResControlMode;
        struct iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlModeType BPT_Dynamic_AC_CLReqControlMode;
        struct iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlModeType BPT_Dynamic_AC_CLResControlMode;
        struct iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlModeType DER_Dynamic_AC_CLReqControlMode;
        struct iso20_ac_der_iec_DER_Dynamic_AC_CLResControlModeType DER_Dynamic_AC_CLResControlMode;
        struct iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlModeType DER_Scheduled_AC_CLReqControlMode;
        struct iso20_ac_der_iec_DER_Scheduled_AC_CLResControlModeType DER_Scheduled_AC_CLResControlMode;
        struct iso20_ac_der_iec_SignatureType Signature;
        struct iso20_ac_der_iec_SignatureValueType SignatureValue;
        struct iso20_ac_der_iec_SignedInfoType SignedInfo;
        struct iso20_ac_der_iec_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_ac_der_iec_SignatureMethodType SignatureMethod;
        struct iso20_ac_der_iec_ReferenceType Reference;
        struct iso20_ac_der_iec_TransformsType Transforms;
        struct iso20_ac_der_iec_TransformType Transform;
        struct iso20_ac_der_iec_DigestMethodType DigestMethod;
        struct iso20_ac_der_iec_KeyInfoType KeyInfo;
        struct iso20_ac_der_iec_KeyValueType KeyValue;
        struct iso20_ac_der_iec_RetrievalMethodType RetrievalMethod;
        struct iso20_ac_der_iec_X509DataType X509Data;
        struct iso20_ac_der_iec_PGPDataType PGPData;
        struct iso20_ac_der_iec_SPKIDataType SPKIData;
        struct iso20_ac_der_iec_ObjectType Object;
        struct iso20_ac_der_iec_ManifestType Manifest;
        struct iso20_ac_der_iec_SignaturePropertiesType SignatureProperties;
        struct iso20_ac_der_iec_SignaturePropertyType SignatureProperty;
        struct iso20_ac_der_iec_DSAKeyValueType DSAKeyValue;
        struct iso20_ac_der_iec_RSAKeyValueType RSAKeyValue;
    };
    unsigned int AC_ChargeParameterDiscoveryReq_isUsed:1;
    unsigned int AC_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int AC_ChargeLoopReq_isUsed:1;
    unsigned int AC_ChargeLoopRes_isUsed:1;
    unsigned int AC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int AC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int BPT_AC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int BPT_AC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int DER_AC_CPDReqEnergyTransferMode_isUsed:1;
    unsigned int DER_AC_CPDResEnergyTransferMode_isUsed:1;
    unsigned int CLReqControlMode_isUsed:1;
    unsigned int Scheduled_AC_CLReqControlMode_isUsed:1;
    unsigned int CLResControlMode_isUsed:1;
    unsigned int Scheduled_AC_CLResControlMode_isUsed:1;
    unsigned int BPT_Scheduled_AC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Scheduled_AC_CLResControlMode_isUsed:1;
    unsigned int Dynamic_AC_CLReqControlMode_isUsed:1;
    unsigned int Dynamic_AC_CLResControlMode_isUsed:1;
    unsigned int BPT_Dynamic_AC_CLReqControlMode_isUsed:1;
    unsigned int BPT_Dynamic_AC_CLResControlMode_isUsed:1;
    unsigned int DER_Dynamic_AC_CLReqControlMode_isUsed:1;
    unsigned int DER_Dynamic_AC_CLResControlMode_isUsed:1;
    unsigned int DER_Scheduled_AC_CLReqControlMode_isUsed:1;
    unsigned int DER_Scheduled_AC_CLResControlMode_isUsed:1;
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
struct iso20_ac_der_iec_exiFragment {
    union {
        struct iso20_ac_der_iec_SignedInfoType SignedInfo;
    };
    unsigned int SignedInfo_isUsed:1;
};

// elements of xmldsig fragment
struct iso20_ac_der_iec_xmldsigFragment {
    union {
        struct iso20_ac_der_iec_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_ac_der_iec_DSAKeyValueType DSAKeyValue;
        struct iso20_ac_der_iec_DigestMethodType DigestMethod;
        struct iso20_ac_der_iec_KeyInfoType KeyInfo;
        struct iso20_ac_der_iec_KeyValueType KeyValue;
        struct iso20_ac_der_iec_ManifestType Manifest;
        struct iso20_ac_der_iec_ObjectType Object;
        struct iso20_ac_der_iec_PGPDataType PGPData;
        struct iso20_ac_der_iec_RSAKeyValueType RSAKeyValue;
        struct iso20_ac_der_iec_ReferenceType Reference;
        struct iso20_ac_der_iec_RetrievalMethodType RetrievalMethod;
        struct iso20_ac_der_iec_SPKIDataType SPKIData;
        struct iso20_ac_der_iec_SignatureType Signature;
        struct iso20_ac_der_iec_SignatureMethodType SignatureMethod;
        struct iso20_ac_der_iec_SignaturePropertiesType SignatureProperties;
        struct iso20_ac_der_iec_SignaturePropertyType SignatureProperty;
        struct iso20_ac_der_iec_SignatureValueType SignatureValue;
        struct iso20_ac_der_iec_SignedInfoType SignedInfo;
        struct iso20_ac_der_iec_TransformType Transform;
        struct iso20_ac_der_iec_TransformsType Transforms;
        struct iso20_ac_der_iec_X509DataType X509Data;
        struct iso20_ac_der_iec_X509IssuerSerialType X509IssuerSerial;
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
void init_iso20_ac_der_iec_exiDocument(struct iso20_ac_der_iec_exiDocument* exiDoc);
void init_iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType(struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType* AC_ChargeParameterDiscoveryReq);
void init_iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType(struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType* AC_ChargeParameterDiscoveryRes);
void init_iso20_ac_der_iec_AC_ChargeLoopReqType(struct iso20_ac_der_iec_AC_ChargeLoopReqType* AC_ChargeLoopReq);
void init_iso20_ac_der_iec_AC_ChargeLoopResType(struct iso20_ac_der_iec_AC_ChargeLoopResType* AC_ChargeLoopRes);
void init_iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType(struct iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType* AC_CPDReqEnergyTransferMode);
void init_iso20_ac_der_iec_AC_CPDResEnergyTransferModeType(struct iso20_ac_der_iec_AC_CPDResEnergyTransferModeType* AC_CPDResEnergyTransferMode);
void init_iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType(struct iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType* BPT_AC_CPDReqEnergyTransferMode);
void init_iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType(struct iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType* BPT_AC_CPDResEnergyTransferMode);
void init_iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType(struct iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType* DER_AC_CPDReqEnergyTransferMode);
void init_iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType(struct iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType* DER_AC_CPDResEnergyTransferMode);
void init_iso20_ac_der_iec_CLReqControlModeType(struct iso20_ac_der_iec_CLReqControlModeType* CLReqControlMode);
void init_iso20_ac_der_iec_Scheduled_AC_CLReqControlModeType(struct iso20_ac_der_iec_Scheduled_AC_CLReqControlModeType* Scheduled_AC_CLReqControlMode);
void init_iso20_ac_der_iec_CLResControlModeType(struct iso20_ac_der_iec_CLResControlModeType* CLResControlMode);
void init_iso20_ac_der_iec_Scheduled_AC_CLResControlModeType(struct iso20_ac_der_iec_Scheduled_AC_CLResControlModeType* Scheduled_AC_CLResControlMode);
void init_iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlModeType(struct iso20_ac_der_iec_BPT_Scheduled_AC_CLReqControlModeType* BPT_Scheduled_AC_CLReqControlMode);
void init_iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlModeType(struct iso20_ac_der_iec_BPT_Scheduled_AC_CLResControlModeType* BPT_Scheduled_AC_CLResControlMode);
void init_iso20_ac_der_iec_Dynamic_AC_CLReqControlModeType(struct iso20_ac_der_iec_Dynamic_AC_CLReqControlModeType* Dynamic_AC_CLReqControlMode);
void init_iso20_ac_der_iec_Dynamic_AC_CLResControlModeType(struct iso20_ac_der_iec_Dynamic_AC_CLResControlModeType* Dynamic_AC_CLResControlMode);
void init_iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlModeType(struct iso20_ac_der_iec_BPT_Dynamic_AC_CLReqControlModeType* BPT_Dynamic_AC_CLReqControlMode);
void init_iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlModeType(struct iso20_ac_der_iec_BPT_Dynamic_AC_CLResControlModeType* BPT_Dynamic_AC_CLResControlMode);
void init_iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlModeType(struct iso20_ac_der_iec_DER_Dynamic_AC_CLReqControlModeType* DER_Dynamic_AC_CLReqControlMode);
void init_iso20_ac_der_iec_DER_Dynamic_AC_CLResControlModeType(struct iso20_ac_der_iec_DER_Dynamic_AC_CLResControlModeType* DER_Dynamic_AC_CLResControlMode);
void init_iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlModeType(struct iso20_ac_der_iec_DER_Scheduled_AC_CLReqControlModeType* DER_Scheduled_AC_CLReqControlMode);
void init_iso20_ac_der_iec_DER_Scheduled_AC_CLResControlModeType(struct iso20_ac_der_iec_DER_Scheduled_AC_CLResControlModeType* DER_Scheduled_AC_CLResControlMode);
void init_iso20_ac_der_iec_SignatureType(struct iso20_ac_der_iec_SignatureType* Signature);
void init_iso20_ac_der_iec_SignatureValueType(struct iso20_ac_der_iec_SignatureValueType* SignatureValue);
void init_iso20_ac_der_iec_SignedInfoType(struct iso20_ac_der_iec_SignedInfoType* SignedInfo);
void init_iso20_ac_der_iec_CanonicalizationMethodType(struct iso20_ac_der_iec_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_ac_der_iec_SignatureMethodType(struct iso20_ac_der_iec_SignatureMethodType* SignatureMethod);
void init_iso20_ac_der_iec_ReferenceType(struct iso20_ac_der_iec_ReferenceType* Reference);
void init_iso20_ac_der_iec_TransformsType(struct iso20_ac_der_iec_TransformsType* Transforms);
void init_iso20_ac_der_iec_TransformType(struct iso20_ac_der_iec_TransformType* Transform);
void init_iso20_ac_der_iec_DigestMethodType(struct iso20_ac_der_iec_DigestMethodType* DigestMethod);
void init_iso20_ac_der_iec_KeyInfoType(struct iso20_ac_der_iec_KeyInfoType* KeyInfo);
void init_iso20_ac_der_iec_KeyValueType(struct iso20_ac_der_iec_KeyValueType* KeyValue);
void init_iso20_ac_der_iec_RetrievalMethodType(struct iso20_ac_der_iec_RetrievalMethodType* RetrievalMethod);
void init_iso20_ac_der_iec_X509DataType(struct iso20_ac_der_iec_X509DataType* X509Data);
void init_iso20_ac_der_iec_PGPDataType(struct iso20_ac_der_iec_PGPDataType* PGPData);
void init_iso20_ac_der_iec_SPKIDataType(struct iso20_ac_der_iec_SPKIDataType* SPKIData);
void init_iso20_ac_der_iec_ObjectType(struct iso20_ac_der_iec_ObjectType* Object);
void init_iso20_ac_der_iec_ManifestType(struct iso20_ac_der_iec_ManifestType* Manifest);
void init_iso20_ac_der_iec_SignaturePropertiesType(struct iso20_ac_der_iec_SignaturePropertiesType* SignatureProperties);
void init_iso20_ac_der_iec_SignaturePropertyType(struct iso20_ac_der_iec_SignaturePropertyType* SignatureProperty);
void init_iso20_ac_der_iec_DSAKeyValueType(struct iso20_ac_der_iec_DSAKeyValueType* DSAKeyValue);
void init_iso20_ac_der_iec_RSAKeyValueType(struct iso20_ac_der_iec_RSAKeyValueType* RSAKeyValue);
void init_iso20_ac_der_iec_SetpointExcitationType(struct iso20_ac_der_iec_SetpointExcitationType* SetpointExcitationType);
void init_iso20_ac_der_iec_X509IssuerSerialType(struct iso20_ac_der_iec_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_ac_der_iec_DataTupleType(struct iso20_ac_der_iec_DataTupleType* DataTupleType);
void init_iso20_ac_der_iec_CurveDataPointsListType(struct iso20_ac_der_iec_CurveDataPointsListType* CurveDataPointsListType);
void init_iso20_ac_der_iec_DERCurveType(struct iso20_ac_der_iec_DERCurveType* DERCurveType);
void init_iso20_ac_der_iec_FrequencyWattType(struct iso20_ac_der_iec_FrequencyWattType* FrequencyWattType);
void init_iso20_ac_der_iec_VoltWattType(struct iso20_ac_der_iec_VoltWattType* VoltWattType);
void init_iso20_ac_der_iec_RationalNumberType(struct iso20_ac_der_iec_RationalNumberType* RationalNumberType);
void init_iso20_ac_der_iec_FaultRideThroughType(struct iso20_ac_der_iec_FaultRideThroughType* FaultRideThroughType);
void init_iso20_ac_der_iec_DetailedCostType(struct iso20_ac_der_iec_DetailedCostType* DetailedCostType);
void init_iso20_ac_der_iec_ZeroCurrentType(struct iso20_ac_der_iec_ZeroCurrentType* ZeroCurrentType);
void init_iso20_ac_der_iec_ReactivePowerSupportType(struct iso20_ac_der_iec_ReactivePowerSupportType* ReactivePowerSupportType);
void init_iso20_ac_der_iec_ActivePowerSupportType(struct iso20_ac_der_iec_ActivePowerSupportType* ActivePowerSupportType);
void init_iso20_ac_der_iec_DetailedTaxType(struct iso20_ac_der_iec_DetailedTaxType* DetailedTaxType);
void init_iso20_ac_der_iec_MessageHeaderType(struct iso20_ac_der_iec_MessageHeaderType* MessageHeaderType);
void init_iso20_ac_der_iec_DisplayParametersType(struct iso20_ac_der_iec_DisplayParametersType* DisplayParametersType);
void init_iso20_ac_der_iec_EVSEStatusType(struct iso20_ac_der_iec_EVSEStatusType* EVSEStatusType);
void init_iso20_ac_der_iec_MeterInfoType(struct iso20_ac_der_iec_MeterInfoType* MeterInfoType);
void init_iso20_ac_der_iec_ReceiptType(struct iso20_ac_der_iec_ReceiptType* ReceiptType);
void init_iso20_ac_der_iec_EVReactivePowerLimitsType(struct iso20_ac_der_iec_EVReactivePowerLimitsType* EVReactivePowerLimitsType);
void init_iso20_ac_der_iec_DSOQSetpointType(struct iso20_ac_der_iec_DSOQSetpointType* DSOQSetpointType);
void init_iso20_ac_der_iec_DERControlType(struct iso20_ac_der_iec_DERControlType* DERControlType);
void init_iso20_ac_der_iec_DSOCosPhiSetpointType(struct iso20_ac_der_iec_DSOCosPhiSetpointType* DSOCosPhiSetpointType);
void init_iso20_ac_der_iec_exiFragment(struct iso20_ac_der_iec_exiFragment* exiFrag);
void init_iso20_ac_der_iec_xmldsigFragment(struct iso20_ac_der_iec_xmldsigFragment* xmldsigFrag);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_AC_DER_IEC_DATATYPES_H */

