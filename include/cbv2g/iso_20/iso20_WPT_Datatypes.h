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
  * @file iso20_WPT_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_WPT_DATATYPES_H
#define ISO20_WPT_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso20_wpt_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_anyType_BYTES_SIZE (4)
#define iso20_wpt_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_wpt_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_wpt_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_wpt_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_ReferenceType_4_ARRAY_SIZE (4)
#define iso20_wpt_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_wpt_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE (255)
#define iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE (255)
#define iso20_wpt_SSID_CHARACTER_SIZE (255 + ASCII_EXTRA_CHAR)
#define iso20_wpt_BSSID_CHARACTER_SIZE (12 + ASCII_EXTRA_CHAR)
#define iso20_wpt_IPAddress_CHARACTER_SIZE (39 + ASCII_EXTRA_CHAR)
#define iso20_wpt_sessionIDType_BYTES_SIZE (8)
#define iso20_wpt_Target_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_wpt_WPT_FinePositioningMethodType_8_ARRAY_SIZE (8)
#define iso20_wpt_WPT_PairingMethodType_8_ARRAY_SIZE (8)
#define iso20_wpt_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso20_wpt_meterSignatureType_BYTES_SIZE (64)
#define iso20_wpt_WPT_AlignmentCheckMethodType_8_ARRAY_SIZE (8)
#define iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE (8)
#define iso20_wpt_DetailedTaxType_10_ARRAY_SIZE (10)
#define iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE (16)
#define iso20_wpt_WPT_DataContainerType_BYTES_SIZE (256)


// enum for function numbers
typedef enum {
    iso20_wpt_CLReqControlMode = 0,
    iso20_wpt_CLResControlMode = 1,
    iso20_wpt_CanonicalizationMethod = 2,
    iso20_wpt_DSAKeyValue = 3,
    iso20_wpt_DigestMethod = 4,
    iso20_wpt_DigestValue = 5,
    iso20_wpt_KeyInfo = 6,
    iso20_wpt_KeyName = 7,
    iso20_wpt_KeyValue = 8,
    iso20_wpt_Manifest = 9,
    iso20_wpt_MgmtData = 10,
    iso20_wpt_Object = 11,
    iso20_wpt_PGPData = 12,
    iso20_wpt_RSAKeyValue = 13,
    iso20_wpt_Reference = 14,
    iso20_wpt_RetrievalMethod = 15,
    iso20_wpt_SPKIData = 16,
    iso20_wpt_Signature = 17,
    iso20_wpt_SignatureMethod = 18,
    iso20_wpt_SignatureProperties = 19,
    iso20_wpt_SignatureProperty = 20,
    iso20_wpt_SignatureValue = 21,
    iso20_wpt_SignedInfo = 22,
    iso20_wpt_Transform = 23,
    iso20_wpt_Transforms = 24,
    iso20_wpt_WPT_AlignmentCheckReq = 25,
    iso20_wpt_WPT_AlignmentCheckRes = 26,
    iso20_wpt_WPT_ChargeLoopReq = 27,
    iso20_wpt_WPT_ChargeLoopRes = 28,
    iso20_wpt_WPT_ChargeParameterDiscoveryReq = 29,
    iso20_wpt_WPT_ChargeParameterDiscoveryRes = 30,
    iso20_wpt_WPT_FinePositioningReq = 31,
    iso20_wpt_WPT_FinePositioningRes = 32,
    iso20_wpt_WPT_FinePositioningSetupReq = 33,
    iso20_wpt_WPT_FinePositioningSetupRes = 34,
    iso20_wpt_WPT_PairingReq = 35,
    iso20_wpt_WPT_PairingRes = 36,
    iso20_wpt_X509Data = 37
} iso20_wpt_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningMethod; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningMethodType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_FinePositioningMethodType_Manual = 0,
    iso20_wpt_WPT_FinePositioningMethodType_LF_TxEV = 1,
    iso20_wpt_WPT_FinePositioningMethodType_LF_TxPrimaryDevice = 2,
    iso20_wpt_WPT_FinePositioningMethodType_LPE = 3,
    iso20_wpt_WPT_FinePositioningMethodType_Proprietary = 4
} iso20_wpt_WPT_FinePositioningMethodType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}WPT_PairingMethod; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingMethodType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_PairingMethodType_External_confirmation = 0,
    iso20_wpt_WPT_PairingMethodType_LPE = 1,
    iso20_wpt_WPT_PairingMethodType_LF_TxEV = 2,
    iso20_wpt_WPT_PairingMethodType_LF_TxPrimaryDevice = 3,
    iso20_wpt_WPT_PairingMethodType_Optical = 4,
    iso20_wpt_WPT_PairingMethodType_Proprietary = 5
} iso20_wpt_WPT_PairingMethodType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckMethod; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckMethodType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_AlignmentCheckMethodType_PowerCheck = 0,
    iso20_wpt_WPT_AlignmentCheckMethodType_LPE = 1,
    iso20_wpt_WPT_AlignmentCheckMethodType_Proprietary = 2
} iso20_wpt_WPT_AlignmentCheckMethodType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSENotification; type={urn:iso:std:iso:15118:-20:CommonTypes}evseNotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_evseNotificationType_Pause = 0,
    iso20_wpt_evseNotificationType_ExitStandby = 1,
    iso20_wpt_evseNotificationType_Terminate = 2,
    iso20_wpt_evseNotificationType_ScheduleRenegotiation = 3,
    iso20_wpt_evseNotificationType_ServiceRenegotiation = 4,
    iso20_wpt_evseNotificationType_MeteringConfirmation = 5
} iso20_wpt_evseNotificationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}EVProcessing; type={urn:iso:std:iso:15118:-20:CommonTypes}processingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_processingType_Finished = 0,
    iso20_wpt_processingType_Ongoing = 1,
    iso20_wpt_processingType_Ongoing_WaitingForCustomerInteraction = 2
} iso20_wpt_processingType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_responseCodeType_OK = 0,
    iso20_wpt_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_wpt_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_wpt_responseCodeType_OK_OldSessionJoined = 3,
    iso20_wpt_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_wpt_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_wpt_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_wpt_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_wpt_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_wpt_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_wpt_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_wpt_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_wpt_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_wpt_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_wpt_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_wpt_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_wpt_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_wpt_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_wpt_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_wpt_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_wpt_responseCodeType_WARNING_WPT = 20,
    iso20_wpt_responseCodeType_FAILED = 21,
    iso20_wpt_responseCodeType_FAILED_AssociationError = 22,
    iso20_wpt_responseCodeType_FAILED_ContactorError = 23,
    iso20_wpt_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_wpt_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_wpt_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_wpt_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_wpt_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_wpt_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_wpt_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_wpt_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_wpt_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_wpt_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_wpt_responseCodeType_FAILED_SequenceError = 34,
    iso20_wpt_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_wpt_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_wpt_responseCodeType_FAILED_SignatureError = 37,
    iso20_wpt_responseCodeType_FAILED_UnknownSession = 38,
    iso20_wpt_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_wpt_responseCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}EVResultCode; type={urn:iso:std:iso:15118:-20:WPT}WPT_EVResultType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_EVResultType_EVResultUnknown = 0,
    iso20_wpt_WPT_EVResultType_EVResultSuccess = 1,
    iso20_wpt_WPT_EVResultType_EVResultFailed = 2
} iso20_wpt_WPT_EVResultType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}PDInputPowerClass; type={urn:iso:std:iso:15118:-20:WPT}WPT_PowerClassType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_PowerClassType_MF_WPT1 = 0,
    iso20_wpt_WPT_PowerClassType_MF_WPT2 = 1,
    iso20_wpt_WPT_PowerClassType_MF_WPT3 = 2,
    iso20_wpt_WPT_PowerClassType_MF_WPT4 = 3
} iso20_wpt_WPT_PowerClassType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}EVPCChargeDiagnostics; type={urn:iso:std:iso:15118:-20:WPT}WPT_EVPCChargeDiagnosticsType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_EVPCChargeDiagnosticsType_EVPCNoIssue = 0,
    iso20_wpt_WPT_EVPCChargeDiagnosticsType_EVPCTempOverheatDetected = 1,
    iso20_wpt_WPT_EVPCChargeDiagnosticsType_EVPCPowerTransferAnomalyDetected = 2,
    iso20_wpt_WPT_EVPCChargeDiagnosticsType_EVPCAnomalyDetected = 3
} iso20_wpt_WPT_EVPCChargeDiagnosticsType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:WPT}SPCChargeDiagnostics; type={urn:iso:std:iso:15118:-20:WPT}WPT_SPCChargeDiagnosticsType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCNoIssue = 0,
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCFODDetected = 1,
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCLOPDetected = 2,
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCTempOverheatDetected = 3,
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCPowerTransferAnomalyDetected = 4,
    iso20_wpt_WPT_SPCChargeDiagnosticsType_SPCAnomalyDetected = 5
} iso20_wpt_WPT_SPCChargeDiagnosticsType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_wpt_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_wpt_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_wpt_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso20_wpt_TransformsType {
    // Transform, TransformType
    struct iso20_wpt_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso20_wpt_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_wpt_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_wpt_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_wpt_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_wpt_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_wpt_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_wpt_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_wpt_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}PulseSequenceOrder; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxPulseOrderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: IndexNumber, unsignedShort (1, 1); TxRxIdentifier, numericIDType (1, 1);
struct iso20_wpt_WPT_TxRxPulseOrderType {
    // IndexNumber, unsignedShort (base: unsignedInt)
    uint16_t IndexNumber;
    // TxRxIdentifier, numericIDType (base: unsignedInt)
    uint32_t TxRxIdentifier;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_wpt_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_wpt_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_wpt_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_wpt_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_wpt_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxRxPosition; type={urn:iso:std:iso:15118:-20:WPT}WPT_CoordinateXYZType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Coord_X, short (1, 1); Coord_Y, short (1, 1); Coord_Z, short (1, 1);
struct iso20_wpt_WPT_CoordinateXYZType {
    // Coord_X, short (base: int)
    int16_t Coord_X;
    // Coord_Y, short (base: int)
    int16_t Coord_Y;
    // Coord_Z, short (base: int)
    int16_t Coord_Z;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_wpt_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_wpt_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_wpt_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_wpt_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_wpt_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_wpt_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_wpt_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_wpt_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_wpt_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_wpt_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_wpt_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_wpt_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_wpt_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
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
struct iso20_wpt_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_wpt_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_wpt_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_wpt_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_wpt_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_wpt_ReferenceType array[iso20_wpt_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_wpt_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}SignalFrequency; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_wpt_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}RSSIDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxRSSIType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxIdentifier, numericIDType (1, 1); RSSI, RationalNumberType (1, 1);
struct iso20_wpt_WPT_LF_RxRSSIType {
    // TxIdentifier, numericIDType (base: unsignedInt)
    uint32_t TxIdentifier;
    // RSSI, RationalNumberType
    struct iso20_wpt_RationalNumberType RSSI;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}RSSIData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxRSSIListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RSSIDataList, WPT_LF_RxRSSIType (1, 1);
struct iso20_wpt_WPT_LF_RxRSSIListType {
    // RSSIDataList, WPT_LF_RxRSSIType
    struct iso20_wpt_WPT_LF_RxRSSIType RSSIDataList;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxIdentifier, numericIDType (1, 1); EIRP, RationalNumberType (1, 1);
struct iso20_wpt_WPT_LF_TxDataType {
    // TxIdentifier, numericIDType (base: unsignedInt)
    uint32_t TxIdentifier;
    // EIRP, RationalNumberType
    struct iso20_wpt_RationalNumberType EIRP;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RxIdentifier, numericIDType (1, 1); RSSIData, WPT_LF_RxRSSIListType (1, 1);
struct iso20_wpt_WPT_LF_RxDataType {
    // RxIdentifier, numericIDType (base: unsignedInt)
    uint32_t RxIdentifier;
    // RSSIData, WPT_LF_RxRSSIListType
    struct iso20_wpt_WPT_LF_RxRSSIListType RSSIData;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_TxData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TxDataListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_LF_TxDataList, WPT_LF_TxDataType (1, 1);
struct iso20_wpt_WPT_LF_TxDataListType {
    // WPT_LF_TxDataList, WPT_LF_TxDataType
    struct iso20_wpt_WPT_LF_TxDataType WPT_LF_TxDataList;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_wpt_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_wpt_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_wpt_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_wpt_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_wpt_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_wpt_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_wpt_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_wpt_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxSpecData; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxSpecDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TxRxIdentifier, numericIDType (1, 1); TxRxPosition, WPT_CoordinateXYZType (1, 1); TxRxOrientation, WPT_CoordinateXYZType (1, 1);
struct iso20_wpt_WPT_TxRxSpecDataType {
    // TxRxIdentifier, numericIDType (base: unsignedInt)
    uint32_t TxRxIdentifier;
    // TxRxPosition, WPT_CoordinateXYZType
    struct iso20_wpt_WPT_CoordinateXYZType TxRxPosition;
    // TxRxOrientation, WPT_CoordinateXYZType
    struct iso20_wpt_WPT_CoordinateXYZType TxRxOrientation;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_RxData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_RxDataListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_LF_RxDataList, WPT_LF_RxDataType (1, 1);
struct iso20_wpt_WPT_LF_RxDataListType {
    // WPT_LF_RxDataList, WPT_LF_RxDataType
    struct iso20_wpt_WPT_LF_RxDataType WPT_LF_RxDataList;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso20_wpt_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_wpt_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_wpt_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}TxPackageSpecData; type={urn:iso:std:iso:15118:-20:WPT}WPT_TxRxPackageSpecDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PulseSequenceOrder, WPT_TxRxPulseOrderType (2, 255); PulseSeparationTime, unsignedShort (1, 1); PulseDuration, unsignedShort (1, 1); PackageSeparationTime, unsignedShort (1, 1);
struct iso20_wpt_WPT_TxRxPackageSpecDataType {
    // PulseSequenceOrder, WPT_TxRxPulseOrderType
    struct {
        struct iso20_wpt_WPT_TxRxPulseOrderType array[iso20_wpt_WPT_TxRxPulseOrderType_255_ARRAY_SIZE];
        uint16_t arrayLen;
    } PulseSequenceOrder;    // PulseSeparationTime, unsignedShort (base: unsignedInt)
    uint16_t PulseSeparationTime;
    // PulseDuration, unsignedShort (base: unsignedInt)
    uint16_t PulseDuration;
    // PackageSeparationTime, unsignedShort (base: unsignedInt)
    uint16_t PackageSeparationTime;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_TransmitterSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_TransmitterDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumberOfTransmitters, unsignedByte (1, 1); SignalFrequency, RationalNumberType (1, 1); TxSpecData, WPT_TxRxSpecDataType (2, 255); TxPackageSpecData, WPT_TxRxPackageSpecDataType (0, 1);
struct iso20_wpt_WPT_LF_TransmitterDataType {
    // NumberOfTransmitters, unsignedByte (base: unsignedShort)
    uint8_t NumberOfTransmitters;
    // SignalFrequency, RationalNumberType
    struct iso20_wpt_RationalNumberType SignalFrequency;
    // TxSpecData, WPT_TxRxSpecDataType
    struct {
        struct iso20_wpt_WPT_TxRxSpecDataType array[iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE];
        uint16_t arrayLen;
    } TxSpecData;    // TxPackageSpecData, WPT_TxRxPackageSpecDataType
    struct iso20_wpt_WPT_TxRxPackageSpecDataType TxPackageSpecData;
    unsigned int TxPackageSpecData_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}AlternativeSECC; type={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SSID, identifierType (0, 1); BSSID, bssidType (0, 1); IPAddress, ipaddressType (0, 1); Port, unsignedShort (0, 1);
struct iso20_wpt_AlternativeSECCType {
    // SSID, identifierType (base: string)
    struct {
        char characters[iso20_wpt_SSID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } SSID;
    unsigned int SSID_isUsed:1;
    // BSSID, bssidType (base: string)
    struct {
        char characters[iso20_wpt_BSSID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } BSSID;
    unsigned int BSSID_isUsed:1;
    // IPAddress, ipaddressType (base: string)
    struct {
        char characters[iso20_wpt_IPAddress_CHARACTER_SIZE];
        uint16_t charactersLen;
    } IPAddress;
    unsigned int IPAddress_isUsed:1;
    // Port, unsignedShort (base: unsignedInt)
    uint16_t Port;
    unsigned int Port_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_ReceiverSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_ReceiverDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumberOfReceivers, unsignedByte (1, 1); RxSpecData, WPT_TxRxSpecDataType (2, 255);
struct iso20_wpt_WPT_LF_ReceiverDataType {
    // NumberOfReceivers, unsignedByte (base: unsignedShort)
    uint8_t NumberOfReceivers;
    // RxSpecData, WPT_TxRxSpecDataType
    struct {
        struct iso20_wpt_WPT_TxRxSpecDataType array[iso20_wpt_WPT_TxRxSpecDataType_255_ARRAY_SIZE];
        uint16_t arrayLen;
    } RxSpecData;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackage; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PackageIndex, unsignedByte (1, 1); LF_TxData, WPT_LF_TxDataListType (0, 1); LF_RxData, WPT_LF_RxDataListType (0, 1);
struct iso20_wpt_WPT_LF_DataPackageType {
    // PackageIndex, unsignedByte (base: unsignedShort)
    uint8_t PackageIndex;
    // LF_TxData, WPT_LF_TxDataListType
    struct iso20_wpt_WPT_LF_TxDataListType LF_TxData;
    unsigned int LF_TxData_isUsed:1;
    // LF_RxData, WPT_LF_RxDataListType
    struct iso20_wpt_WPT_LF_RxDataListType LF_RxData;
    unsigned int LF_RxData_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
struct iso20_wpt_DetailedCostType {
    // Amount, RationalNumberType
    struct iso20_wpt_RationalNumberType Amount;
    // CostPerUnit, RationalNumberType
    struct iso20_wpt_RationalNumberType CostPerUnit;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso20_wpt_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_wpt_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_wpt_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_wpt_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_wpt_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
struct iso20_wpt_DetailedTaxType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // Amount, RationalNumberType
    struct iso20_wpt_RationalNumberType Amount;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_wpt_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_wpt_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_wpt_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_wpt_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_wpt_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_wpt_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParameters; type={urn:iso:std:iso:15118:-20:CommonTypes}DisplayParametersType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PresentSOC, percentValueType (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); MaximumSOC, percentValueType (0, 1); RemainingTimeToMinimumSOC, unsignedInt (0, 1); RemainingTimeToTargetSOC, unsignedInt (0, 1); RemainingTimeToMaximumSOC, unsignedInt (0, 1); ChargingComplete, boolean (0, 1); BatteryEnergyCapacity, RationalNumberType (0, 1); InletHot, boolean (0, 1);
struct iso20_wpt_DisplayParametersType {
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
    struct iso20_wpt_RationalNumberType BatteryEnergyCapacity;
    unsigned int BatteryEnergyCapacity_isUsed:1;
    // InletHot, boolean
    int InletHot;
    unsigned int InletHot_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDeviceFinePositioningMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_FinePositioningMethod, WPT_FinePositioningMethodType (1, 8);
struct iso20_wpt_WPT_FinePositioningMethodListType {
    // WPT_FinePositioningMethod, WPT_FinePositioningMethodType (base: string)
    struct {
        iso20_wpt_WPT_FinePositioningMethodType array[iso20_wpt_WPT_FinePositioningMethodType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } WPT_FinePositioningMethod;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
struct iso20_wpt_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, evseNotificationType (base: string)
    iso20_wpt_evseNotificationType EVSENotification;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDevicePairingMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_PairingMethod, WPT_PairingMethodType (1, 8);
struct iso20_wpt_WPT_PairingMethodListType {
    // WPT_PairingMethod, WPT_PairingMethodType (base: string)
    struct {
        iso20_wpt_WPT_PairingMethodType array[iso20_wpt_WPT_PairingMethodType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } WPT_PairingMethod;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
struct iso20_wpt_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso20_wpt_MeterID_CHARACTER_SIZE];
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
        uint8_t bytes[iso20_wpt_meterSignatureType_BYTES_SIZE];
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

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVDeviceAlignmentCheckMethodList; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckMethodListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: WPT_AlignmentCheckMethod, WPT_AlignmentCheckMethodType (1, 8);
struct iso20_wpt_WPT_AlignmentCheckMethodListType {
    // WPT_AlignmentCheckMethod, WPT_AlignmentCheckMethodType (base: string)
    struct {
        iso20_wpt_WPT_AlignmentCheckMethodType array[iso20_wpt_WPT_AlignmentCheckMethodType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } WPT_AlignmentCheckMethod;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageList; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_DataPackageListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NumPackages, unsignedByte (1, 1); WPT_LF_DataPackage, WPT_LF_DataPackageType (1, 1);
struct iso20_wpt_WPT_LF_DataPackageListType {
    // NumPackages, unsignedByte (base: unsignedShort)
    uint8_t NumPackages;
    // WPT_LF_DataPackage, WPT_LF_DataPackageType
    struct iso20_wpt_WPT_LF_DataPackageType WPT_LF_DataPackage;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCList; type={urn:iso:std:iso:15118:-20:WPT}AlternativeSECCListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AlternativeSECC, AlternativeSECCType (1, 8);
struct iso20_wpt_AlternativeSECCListType {
    // AlternativeSECC, AlternativeSECCType
    struct {
        struct iso20_wpt_AlternativeSECCType array[iso20_wpt_AlternativeSECCType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } AlternativeSECC;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
struct iso20_wpt_ReceiptType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EnergyCosts, DetailedCostType
    struct iso20_wpt_DetailedCostType EnergyCosts;
    unsigned int EnergyCosts_isUsed:1;
    // OccupancyCosts, DetailedCostType
    struct iso20_wpt_DetailedCostType OccupancyCosts;
    unsigned int OccupancyCosts_isUsed:1;
    // AdditionalServicesCosts, DetailedCostType
    struct iso20_wpt_DetailedCostType AdditionalServicesCosts;
    unsigned int AdditionalServicesCosts_isUsed:1;
    // OverstayCosts, DetailedCostType
    struct iso20_wpt_DetailedCostType OverstayCosts;
    unsigned int OverstayCosts_isUsed:1;
    // TaxCosts, DetailedTaxType
    struct {
        struct iso20_wpt_DetailedTaxType array[iso20_wpt_DetailedTaxType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxCosts;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}LF_SystemSetupData; type={urn:iso:std:iso:15118:-20:WPT}WPT_LF_SystemSetupDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: LF_TransmitterSetupData, WPT_LF_TransmitterDataType (0, 1); LF_ReceiverSetupData, WPT_LF_ReceiverDataType (0, 1);
struct iso20_wpt_WPT_LF_SystemSetupDataType {
    // LF_TransmitterSetupData, WPT_LF_TransmitterDataType
    struct iso20_wpt_WPT_LF_TransmitterDataType LF_TransmitterSetupData;
    unsigned int LF_TransmitterSetupData_isUsed:1;
    // LF_ReceiverSetupData, WPT_LF_ReceiverDataType
    struct iso20_wpt_WPT_LF_ReceiverDataType LF_ReceiverSetupData;
    unsigned int LF_ReceiverSetupData_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}EVPCPowerControlParameter; type={urn:iso:std:iso:15118:-20:WPT}WPT_EVPCPowerControlParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPCCoilCurrentRequest, RationalNumberType (1, 1); EVPCCoilCurrentInformation, RationalNumberType (1, 1); EVPCCurrentOutputInformation, RationalNumberType (1, 1); EVPCVoltageOutputInformation, RationalNumberType (1, 1);
struct iso20_wpt_WPT_EVPCPowerControlParameterType {
    // EVPCCoilCurrentRequest, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCCoilCurrentRequest;
    // EVPCCoilCurrentInformation, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCCoilCurrentInformation;
    // EVPCCurrentOutputInformation, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCCurrentOutputInformation;
    // EVPCVoltageOutputInformation, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCVoltageOutputInformation;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}SPCPowerControlParameter; type={urn:iso:std:iso:15118:-20:WPT}WPT_SPCPowerControlParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPCPrimaryDeviceCoilCurrentInformation, RationalNumberType (1, 1);
struct iso20_wpt_WPT_SPCPowerControlParameterType {
    // SPCPrimaryDeviceCoilCurrentInformation, RationalNumberType
    struct iso20_wpt_RationalNumberType SPCPrimaryDeviceCoilCurrentInformation;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (1, 1); EVDevicePairingMethodList, WPT_PairingMethodListType (1, 1); EVDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (1, 1); NaturalOffset, unsignedShort (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); LF_SystemSetupData, WPT_LF_SystemSetupDataType (0, 1);
struct iso20_wpt_WPT_FinePositioningSetupReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_wpt_processingType EVProcessing;
    // EVDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType
    struct iso20_wpt_WPT_FinePositioningMethodListType EVDeviceFinePositioningMethodList;
    // EVDevicePairingMethodList, WPT_PairingMethodListType
    struct iso20_wpt_WPT_PairingMethodListType EVDevicePairingMethodList;
    // EVDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType
    struct iso20_wpt_WPT_AlignmentCheckMethodListType EVDeviceAlignmentCheckMethodList;
    // NaturalOffset, unsignedShort (base: unsignedInt)
    uint16_t NaturalOffset;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;

    // LF_SystemSetupData, WPT_LF_SystemSetupDataType
    struct iso20_wpt_WPT_LF_SystemSetupDataType LF_SystemSetupData;
    unsigned int LF_SystemSetupData_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); PrimaryDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType (1, 1); PrimaryDevicePairingMethodList, WPT_PairingMethodListType (1, 1); PrimaryDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType (1, 1); NaturalOffset, unsignedShort (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); LF_SystemSetupData, WPT_LF_SystemSetupDataType (0, 1);
struct iso20_wpt_WPT_FinePositioningSetupResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // PrimaryDeviceFinePositioningMethodList, WPT_FinePositioningMethodListType
    struct iso20_wpt_WPT_FinePositioningMethodListType PrimaryDeviceFinePositioningMethodList;
    // PrimaryDevicePairingMethodList, WPT_PairingMethodListType
    struct iso20_wpt_WPT_PairingMethodListType PrimaryDevicePairingMethodList;
    // PrimaryDeviceAlignmentCheckMethodList, WPT_AlignmentCheckMethodListType
    struct iso20_wpt_WPT_AlignmentCheckMethodListType PrimaryDeviceAlignmentCheckMethodList;
    // NaturalOffset, unsignedShort (base: unsignedInt)
    uint16_t NaturalOffset;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;

    // LF_SystemSetupData, WPT_LF_SystemSetupDataType
    struct iso20_wpt_WPT_LF_SystemSetupDataType LF_SystemSetupData;
    unsigned int LF_SystemSetupData_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); WPT_LF_DataPackageList, WPT_LF_DataPackageListType (0, 1);
struct iso20_wpt_WPT_FinePositioningReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_wpt_processingType EVProcessing;
    // EVResultCode, WPT_EVResultType (base: string)
    iso20_wpt_WPT_EVResultType EVResultCode;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;

    // WPT_LF_DataPackageList, WPT_LF_DataPackageListType
    struct iso20_wpt_WPT_LF_DataPackageListType WPT_LF_DataPackageList;
    unsigned int WPT_LF_DataPackageList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_FinePositioningResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16); WPT_LF_DataPackageList, WPT_LF_DataPackageListType (0, 1);
struct iso20_wpt_WPT_FinePositioningResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_wpt_processingType EVSEProcessing;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;

    // WPT_LF_DataPackageList, WPT_LF_DataPackageListType
    struct iso20_wpt_WPT_LF_DataPackageListType WPT_LF_DataPackageList;
    unsigned int WPT_LF_DataPackageList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_PairingReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); ObservedIDCode, numericIDType (0, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_PairingReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_wpt_processingType EVProcessing;
    // ObservedIDCode, numericIDType (base: unsignedInt)
    uint32_t ObservedIDCode;
    unsigned int ObservedIDCode_isUsed:1;
    // EVResultCode, WPT_EVResultType (base: string)
    iso20_wpt_WPT_EVResultType EVResultCode;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_PairingRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_PairingResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); ObservedIDCode, numericIDType (0, 1); AlternativeSECCList, AlternativeSECCListType (0, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_PairingResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_wpt_processingType EVSEProcessing;
    // ObservedIDCode, numericIDType (base: unsignedInt)
    uint32_t ObservedIDCode;
    unsigned int ObservedIDCode_isUsed:1;
    // AlternativeSECCList, AlternativeSECCListType
    struct iso20_wpt_AlternativeSECCListType AlternativeSECCList;
    unsigned int AlternativeSECCList_isUsed:1;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryReqType; base type=ChargeParameterDiscoveryReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVPCMaxReceivablePower, RationalNumberType (1, 1); SDMaxGroundClearence, unsignedShort (1, 1); SDMinGroundClearence, unsignedShort (1, 1); EVPCNaturalFrequency, RationalNumberType (1, 1); EVPCDeviceLocalControl, boolean (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // EVPCMaxReceivablePower, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCMaxReceivablePower;
    // SDMaxGroundClearence, unsignedShort (base: unsignedInt)
    uint16_t SDMaxGroundClearence;
    // SDMinGroundClearence, unsignedShort (base: unsignedInt)
    uint16_t SDMinGroundClearence;
    // EVPCNaturalFrequency, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCNaturalFrequency;
    // EVPCDeviceLocalControl, boolean
    int EVPCDeviceLocalControl;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeParameterDiscoveryResType; base type=ChargeParameterDiscoveryResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); PDInputPowerClass, WPT_PowerClassType (1, 1); SDMinOutputPower, RationalNumberType (1, 1); SDMaxOutputPower, RationalNumberType (1, 1); SDMaxGroundClearanceSupport, unsignedShort (1, 1); SDMinGroundClearanceSupport, unsignedShort (1, 1); PDMinCoilCurrent, RationalNumberType (1, 1); PDMaxCoilCurrent, RationalNumberType (1, 1); SDManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_ChargeParameterDiscoveryResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // PDInputPowerClass, WPT_PowerClassType (base: string)
    iso20_wpt_WPT_PowerClassType PDInputPowerClass;
    // SDMinOutputPower, RationalNumberType
    struct iso20_wpt_RationalNumberType SDMinOutputPower;
    // SDMaxOutputPower, RationalNumberType
    struct iso20_wpt_RationalNumberType SDMaxOutputPower;
    // SDMaxGroundClearanceSupport, unsignedShort (base: unsignedInt)
    uint16_t SDMaxGroundClearanceSupport;
    // SDMinGroundClearanceSupport, unsignedShort (base: unsignedInt)
    uint16_t SDMinGroundClearanceSupport;
    // PDMinCoilCurrent, RationalNumberType
    struct iso20_wpt_RationalNumberType PDMinCoilCurrent;
    // PDMaxCoilCurrent, RationalNumberType
    struct iso20_wpt_RationalNumberType PDMaxCoilCurrent;
    // SDManufacturerSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } SDManufacturerSpecificDataContainer;
    unsigned int SDManufacturerSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); TargetCoilCurrent, RationalNumberType (0, 1); EVResultCode, WPT_EVResultType (1, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_AlignmentCheckReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_wpt_processingType EVProcessing;
    // TargetCoilCurrent, RationalNumberType
    struct iso20_wpt_RationalNumberType TargetCoilCurrent;
    unsigned int TargetCoilCurrent_isUsed:1;
    // EVResultCode, WPT_EVResultType (base: string)
    iso20_wpt_WPT_EVResultType EVResultCode;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_AlignmentCheckResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); PowerTransmitted, RationalNumberType (0, 1); SupplyDeviceCurrent, RationalNumberType (0, 1); VendorSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_AlignmentCheckResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_wpt_processingType EVSEProcessing;
    // PowerTransmitted, RationalNumberType
    struct iso20_wpt_RationalNumberType PowerTransmitted;
    unsigned int PowerTransmitted_isUsed:1;
    // SupplyDeviceCurrent, RationalNumberType
    struct iso20_wpt_RationalNumberType SupplyDeviceCurrent;
    unsigned int SupplyDeviceCurrent_isUsed:1;
    // VendorSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } VendorSpecificDataContainer;
    unsigned int VendorSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopReq; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopReqType; base type=ChargeLoopReqType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); DisplayParameters, DisplayParametersType (0, 1); MeterInfoRequested, boolean (1, 1); EVPCPowerRequest, RationalNumberType (1, 1); EVPCPowerOutput, RationalNumberType (1, 1); EVPCChargeDiagnostics, WPT_EVPCChargeDiagnosticsType (1, 1); EVPCOperatingFrequency, RationalNumberType (0, 1); EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType (0, 1); ManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_ChargeLoopReqType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // DisplayParameters, DisplayParametersType
    struct iso20_wpt_DisplayParametersType DisplayParameters;
    unsigned int DisplayParameters_isUsed:1;
    // MeterInfoRequested, boolean
    int MeterInfoRequested;
    // EVPCPowerRequest, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCPowerRequest;
    // EVPCPowerOutput, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCPowerOutput;
    // EVPCChargeDiagnostics, WPT_EVPCChargeDiagnosticsType (base: string)
    iso20_wpt_WPT_EVPCChargeDiagnosticsType EVPCChargeDiagnostics;
    // EVPCOperatingFrequency, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCOperatingFrequency;
    unsigned int EVPCOperatingFrequency_isUsed:1;
    // EVPCPowerControlParameter, WPT_EVPCPowerControlParameterType
    struct iso20_wpt_WPT_EVPCPowerControlParameterType EVPCPowerControlParameter;
    unsigned int EVPCPowerControlParameter_isUsed:1;
    // ManufacturerSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } ManufacturerSpecificDataContainer;
    unsigned int ManufacturerSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopRes; type={urn:iso:std:iso:15118:-20:WPT}WPT_ChargeLoopResType; base type=ChargeLoopResType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1); MeterInfo, MeterInfoType (0, 1); Receipt, ReceiptType (0, 1); EVPCPowerRequest, RationalNumberType (1, 1); SDPowerInput, RationalNumberType (0, 1); SPCMaxOutputPowerLimit, RationalNumberType (1, 1); SPCMinOutputPowerLimit, RationalNumberType (1, 1); SPCChargeDiagnostics, WPT_SPCChargeDiagnosticsType (1, 1); SPCOperatingFrequency, RationalNumberType (0, 1); SPCPowerControlParameter, WPT_SPCPowerControlParameterType (0, 1); ManufacturerSpecificDataContainer, WPT_DataContainerType (0, 16);
struct iso20_wpt_WPT_ChargeLoopResType {
    // Header, MessageHeaderType
    struct iso20_wpt_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_wpt_responseCodeType ResponseCode;
    // EVSEStatus, EVSEStatusType
    struct iso20_wpt_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso20_wpt_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // Receipt, ReceiptType
    struct iso20_wpt_ReceiptType Receipt;
    unsigned int Receipt_isUsed:1;
    // EVPCPowerRequest, RationalNumberType
    struct iso20_wpt_RationalNumberType EVPCPowerRequest;
    // SDPowerInput, RationalNumberType
    struct iso20_wpt_RationalNumberType SDPowerInput;
    unsigned int SDPowerInput_isUsed:1;
    // SPCMaxOutputPowerLimit, RationalNumberType
    struct iso20_wpt_RationalNumberType SPCMaxOutputPowerLimit;
    // SPCMinOutputPowerLimit, RationalNumberType
    struct iso20_wpt_RationalNumberType SPCMinOutputPowerLimit;
    // SPCChargeDiagnostics, WPT_SPCChargeDiagnosticsType (base: string)
    iso20_wpt_WPT_SPCChargeDiagnosticsType SPCChargeDiagnostics;
    // SPCOperatingFrequency, RationalNumberType
    struct iso20_wpt_RationalNumberType SPCOperatingFrequency;
    unsigned int SPCOperatingFrequency_isUsed:1;
    // SPCPowerControlParameter, WPT_SPCPowerControlParameterType
    struct iso20_wpt_WPT_SPCPowerControlParameterType SPCPowerControlParameter;
    unsigned int SPCPowerControlParameter_isUsed:1;
    // ManufacturerSpecificDataContainer, WPT_DataContainerType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_wpt_WPT_DataContainerType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_wpt_WPT_DataContainerType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } ManufacturerSpecificDataContainer;
    unsigned int ManufacturerSpecificDataContainer_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_wpt_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_wpt_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_wpt_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_wpt_ReferenceType array[iso20_wpt_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
struct iso20_wpt_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_wpt_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_wpt_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_wpt_exiDocument {
    union {
        struct iso20_wpt_WPT_FinePositioningSetupReqType WPT_FinePositioningSetupReq;
        struct iso20_wpt_WPT_FinePositioningSetupResType WPT_FinePositioningSetupRes;
        struct iso20_wpt_WPT_FinePositioningReqType WPT_FinePositioningReq;
        struct iso20_wpt_WPT_FinePositioningResType WPT_FinePositioningRes;
        struct iso20_wpt_WPT_PairingReqType WPT_PairingReq;
        struct iso20_wpt_WPT_PairingResType WPT_PairingRes;
        struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType WPT_ChargeParameterDiscoveryReq;
        struct iso20_wpt_WPT_ChargeParameterDiscoveryResType WPT_ChargeParameterDiscoveryRes;
        struct iso20_wpt_WPT_AlignmentCheckReqType WPT_AlignmentCheckReq;
        struct iso20_wpt_WPT_AlignmentCheckResType WPT_AlignmentCheckRes;
        struct iso20_wpt_WPT_ChargeLoopReqType WPT_ChargeLoopReq;
        struct iso20_wpt_WPT_ChargeLoopResType WPT_ChargeLoopRes;
        struct iso20_wpt_CLReqControlModeType CLReqControlMode;
        struct iso20_wpt_CLResControlModeType CLResControlMode;
        struct iso20_wpt_SignatureType Signature;
        struct iso20_wpt_SignatureValueType SignatureValue;
        struct iso20_wpt_SignedInfoType SignedInfo;
        struct iso20_wpt_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_wpt_SignatureMethodType SignatureMethod;
        struct iso20_wpt_ReferenceType Reference;
        struct iso20_wpt_TransformsType Transforms;
        struct iso20_wpt_TransformType Transform;
        struct iso20_wpt_DigestMethodType DigestMethod;
        struct iso20_wpt_KeyInfoType KeyInfo;
        struct iso20_wpt_KeyValueType KeyValue;
        struct iso20_wpt_RetrievalMethodType RetrievalMethod;
        struct iso20_wpt_X509DataType X509Data;
        struct iso20_wpt_PGPDataType PGPData;
        struct iso20_wpt_SPKIDataType SPKIData;
        struct iso20_wpt_ObjectType Object;
        struct iso20_wpt_ManifestType Manifest;
        struct iso20_wpt_SignaturePropertiesType SignatureProperties;
        struct iso20_wpt_SignaturePropertyType SignatureProperty;
        struct iso20_wpt_DSAKeyValueType DSAKeyValue;
        struct iso20_wpt_RSAKeyValueType RSAKeyValue;
    };
    unsigned int WPT_FinePositioningSetupReq_isUsed:1;
    unsigned int WPT_FinePositioningSetupRes_isUsed:1;
    unsigned int WPT_FinePositioningReq_isUsed:1;
    unsigned int WPT_FinePositioningRes_isUsed:1;
    unsigned int WPT_PairingReq_isUsed:1;
    unsigned int WPT_PairingRes_isUsed:1;
    unsigned int WPT_ChargeParameterDiscoveryReq_isUsed:1;
    unsigned int WPT_ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int WPT_AlignmentCheckReq_isUsed:1;
    unsigned int WPT_AlignmentCheckRes_isUsed:1;
    unsigned int WPT_ChargeLoopReq_isUsed:1;
    unsigned int WPT_ChargeLoopRes_isUsed:1;
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
void init_iso20_wpt_exiDocument(struct iso20_wpt_exiDocument* exiDoc);
void init_iso20_wpt_WPT_FinePositioningSetupReqType(struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReq);
void init_iso20_wpt_WPT_FinePositioningSetupResType(struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupRes);
void init_iso20_wpt_WPT_FinePositioningReqType(struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReq);
void init_iso20_wpt_WPT_FinePositioningResType(struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningRes);
void init_iso20_wpt_WPT_PairingReqType(struct iso20_wpt_WPT_PairingReqType* WPT_PairingReq);
void init_iso20_wpt_WPT_PairingResType(struct iso20_wpt_WPT_PairingResType* WPT_PairingRes);
void init_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReq);
void init_iso20_wpt_WPT_ChargeParameterDiscoveryResType(struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryRes);
void init_iso20_wpt_WPT_AlignmentCheckReqType(struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReq);
void init_iso20_wpt_WPT_AlignmentCheckResType(struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckRes);
void init_iso20_wpt_WPT_ChargeLoopReqType(struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReq);
void init_iso20_wpt_WPT_ChargeLoopResType(struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopRes);
void init_iso20_wpt_CLReqControlModeType(struct iso20_wpt_CLReqControlModeType* CLReqControlMode);
void init_iso20_wpt_CLResControlModeType(struct iso20_wpt_CLResControlModeType* CLResControlMode);
void init_iso20_wpt_SignatureType(struct iso20_wpt_SignatureType* Signature);
void init_iso20_wpt_SignatureValueType(struct iso20_wpt_SignatureValueType* SignatureValue);
void init_iso20_wpt_SignedInfoType(struct iso20_wpt_SignedInfoType* SignedInfo);
void init_iso20_wpt_CanonicalizationMethodType(struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_wpt_SignatureMethodType(struct iso20_wpt_SignatureMethodType* SignatureMethod);
void init_iso20_wpt_ReferenceType(struct iso20_wpt_ReferenceType* Reference);
void init_iso20_wpt_TransformsType(struct iso20_wpt_TransformsType* Transforms);
void init_iso20_wpt_TransformType(struct iso20_wpt_TransformType* Transform);
void init_iso20_wpt_DigestMethodType(struct iso20_wpt_DigestMethodType* DigestMethod);
void init_iso20_wpt_KeyInfoType(struct iso20_wpt_KeyInfoType* KeyInfo);
void init_iso20_wpt_KeyValueType(struct iso20_wpt_KeyValueType* KeyValue);
void init_iso20_wpt_RetrievalMethodType(struct iso20_wpt_RetrievalMethodType* RetrievalMethod);
void init_iso20_wpt_X509DataType(struct iso20_wpt_X509DataType* X509Data);
void init_iso20_wpt_PGPDataType(struct iso20_wpt_PGPDataType* PGPData);
void init_iso20_wpt_SPKIDataType(struct iso20_wpt_SPKIDataType* SPKIData);
void init_iso20_wpt_ObjectType(struct iso20_wpt_ObjectType* Object);
void init_iso20_wpt_ManifestType(struct iso20_wpt_ManifestType* Manifest);
void init_iso20_wpt_SignaturePropertiesType(struct iso20_wpt_SignaturePropertiesType* SignatureProperties);
void init_iso20_wpt_SignaturePropertyType(struct iso20_wpt_SignaturePropertyType* SignatureProperty);
void init_iso20_wpt_DSAKeyValueType(struct iso20_wpt_DSAKeyValueType* DSAKeyValue);
void init_iso20_wpt_RSAKeyValueType(struct iso20_wpt_RSAKeyValueType* RSAKeyValue);
void init_iso20_wpt_WPT_LF_RxRSSIType(struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType);
void init_iso20_wpt_X509IssuerSerialType(struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_wpt_WPT_LF_RxRSSIListType(struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType);
void init_iso20_wpt_WPT_TxRxPulseOrderType(struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType);
void init_iso20_wpt_WPT_LF_TxDataType(struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType);
void init_iso20_wpt_WPT_LF_RxDataType(struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType);
void init_iso20_wpt_WPT_CoordinateXYZType(struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType);
void init_iso20_wpt_RationalNumberType(struct iso20_wpt_RationalNumberType* RationalNumberType);
void init_iso20_wpt_WPT_LF_TxDataListType(struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType);
void init_iso20_wpt_WPT_TxRxSpecDataType(struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType);
void init_iso20_wpt_WPT_LF_RxDataListType(struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType);
void init_iso20_wpt_WPT_TxRxPackageSpecDataType(struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType);
void init_iso20_wpt_WPT_LF_TransmitterDataType(struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType);
void init_iso20_wpt_AlternativeSECCType(struct iso20_wpt_AlternativeSECCType* AlternativeSECCType);
void init_iso20_wpt_WPT_LF_ReceiverDataType(struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType);
void init_iso20_wpt_WPT_LF_DataPackageType(struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType);
void init_iso20_wpt_DetailedCostType(struct iso20_wpt_DetailedCostType* DetailedCostType);
void init_iso20_wpt_DetailedTaxType(struct iso20_wpt_DetailedTaxType* DetailedTaxType);
void init_iso20_wpt_MessageHeaderType(struct iso20_wpt_MessageHeaderType* MessageHeaderType);
void init_iso20_wpt_DisplayParametersType(struct iso20_wpt_DisplayParametersType* DisplayParametersType);
void init_iso20_wpt_WPT_FinePositioningMethodListType(struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType);
void init_iso20_wpt_EVSEStatusType(struct iso20_wpt_EVSEStatusType* EVSEStatusType);
void init_iso20_wpt_WPT_PairingMethodListType(struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType);
void init_iso20_wpt_MeterInfoType(struct iso20_wpt_MeterInfoType* MeterInfoType);
void init_iso20_wpt_WPT_AlignmentCheckMethodListType(struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType);
void init_iso20_wpt_WPT_LF_DataPackageListType(struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType);
void init_iso20_wpt_AlternativeSECCListType(struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType);
void init_iso20_wpt_ReceiptType(struct iso20_wpt_ReceiptType* ReceiptType);
void init_iso20_wpt_WPT_LF_SystemSetupDataType(struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType);
void init_iso20_wpt_WPT_EVPCPowerControlParameterType(struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType);
void init_iso20_wpt_WPT_SPCPowerControlParameterType(struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_WPT_DATATYPES_H */

