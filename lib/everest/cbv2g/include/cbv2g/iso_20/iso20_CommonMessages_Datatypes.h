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
  * @file iso20_CommonMessages_Datatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_COMMON_MESSAGES_DATATYPES_H
#define ISO20_COMMON_MESSAGES_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso20_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_anyType_BYTES_SIZE (4)
#define iso20_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_EVPriceRuleType_8_ARRAY_SIZE (8)
#define iso20_PowerScheduleEntryType_1024_ARRAY_SIZE (1024)
#define iso20_TaxRuleName_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_PriceRuleType_8_ARRAY_SIZE (8)
#define iso20_ServiceName_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_EVPowerScheduleEntryType_1024_ARRAY_SIZE (1024)
#define iso20_OverstayRuleDescription_CHARACTER_SIZE (160 + ASCII_EXTRA_CHAR)
#define iso20_EVPriceRuleStackType_1024_ARRAY_SIZE (1024)
#define iso20_ReferenceType_4_ARRAY_SIZE (4)
#define iso20_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso20_certificateType_3_ARRAY_SIZE (3)
#define iso20_certificateType_BYTES_SIZE (1600)
#define iso20_Name_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_finiteString_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_Currency_CHARACTER_SIZE (3 + ASCII_EXTRA_CHAR)
#define iso20_PriceAlgorithm_CHARACTER_SIZE (255 + ASCII_EXTRA_CHAR)
#define iso20_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_PriceLevelScheduleEntryType_1024_ARRAY_SIZE (1024)
#define iso20_TaxRuleType_10_ARRAY_SIZE (10)
#define iso20_PriceRuleStackType_64_ARRAY_SIZE (64)
#define iso20_OverstayRuleType_5_ARRAY_SIZE (5)
#define iso20_AdditionalServiceType_5_ARRAY_SIZE (5)
#define iso20_ParameterType_8_ARRAY_SIZE (8)
#define iso20_nameType_128_ARRAY_SIZE (128)
#define iso20_ProviderID_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso20_meterSignatureType_BYTES_SIZE (64)
#define iso20_DetailedTaxType_10_ARRAY_SIZE (10)
#define iso20_PriceScheduleDescription_CHARACTER_SIZE (160 + ASCII_EXTRA_CHAR)
#define iso20_Language_CHARACTER_SIZE (3 + ASCII_EXTRA_CHAR)
#define iso20_PowerScheduleEntryType_2048_ARRAY_SIZE (2048)
#define iso20_sessionIDType_BYTES_SIZE (8)
#define iso20_Target_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso20_serviceIDType_16_ARRAY_SIZE (16)
#define iso20_SelectedServiceType_16_ARRAY_SIZE (16)
#define iso20_X509IssuerSerialType_20_ARRAY_SIZE (20)
#define iso20_genChallengeType_BYTES_SIZE (16)
#define iso20_ServiceType_8_ARRAY_SIZE (8)
#define iso20_ParameterSetType_4_ARRAY_SIZE (4)
#define iso20_identifierType_8_ARRAY_SIZE (8)
#define iso20_EMAID_CHARACTER_SIZE (255 + ASCII_EXTRA_CHAR)
#define iso20_dhPublicKeyType_BYTES_SIZE (133)
#define iso20_secp521_EncryptedPrivateKeyType_BYTES_SIZE (94)
#define iso20_x448_EncryptedPrivateKeyType_BYTES_SIZE (84)
#define iso20_tpm_EncryptedPrivateKeyType_BYTES_SIZE (206)
#define iso20_ScheduleTupleType_3_ARRAY_SIZE (3)
#define iso20_EVCCID_CHARACTER_SIZE (255 + ASCII_EXTRA_CHAR)
#define iso20_EVSEID_CHARACTER_SIZE (255 + ASCII_EXTRA_CHAR)
#define iso20_authorizationType_2_ARRAY_SIZE (2)
#define iso20_EVTerminationCode_CHARACTER_SIZE (80 + ASCII_EXTRA_CHAR)
#define iso20_EVTerminationExplanation_CHARACTER_SIZE (160 + ASCII_EXTRA_CHAR)


// enum for function numbers
typedef enum {
    iso20_AuthorizationReq = 0,
    iso20_AuthorizationRes = 1,
    iso20_AuthorizationSetupReq = 2,
    iso20_AuthorizationSetupRes = 3,
    iso20_CLReqControlMode = 4,
    iso20_CLResControlMode = 5,
    iso20_CanonicalizationMethod = 6,
    iso20_CertificateInstallationReq = 7,
    iso20_CertificateInstallationRes = 8,
    iso20_DSAKeyValue = 9,
    iso20_DigestMethod = 10,
    iso20_DigestValue = 11,
    iso20_KeyInfo = 12,
    iso20_KeyName = 13,
    iso20_KeyValue = 14,
    iso20_Manifest = 15,
    iso20_MeteringConfirmationReq = 16,
    iso20_MeteringConfirmationRes = 17,
    iso20_MgmtData = 18,
    iso20_Object = 19,
    iso20_PGPData = 20,
    iso20_PowerDeliveryReq = 21,
    iso20_PowerDeliveryRes = 22,
    iso20_RSAKeyValue = 23,
    iso20_Reference = 24,
    iso20_RetrievalMethod = 25,
    iso20_SPKIData = 26,
    iso20_ScheduleExchangeReq = 27,
    iso20_ScheduleExchangeRes = 28,
    iso20_ServiceDetailReq = 29,
    iso20_ServiceDetailRes = 30,
    iso20_ServiceDiscoveryReq = 31,
    iso20_ServiceDiscoveryRes = 32,
    iso20_ServiceSelectionReq = 33,
    iso20_ServiceSelectionRes = 34,
    iso20_SessionSetupReq = 35,
    iso20_SessionSetupRes = 36,
    iso20_SessionStopReq = 37,
    iso20_SessionStopRes = 38,
    iso20_Signature = 39,
    iso20_SignatureMethod = 40,
    iso20_SignatureProperties = 41,
    iso20_SignatureProperty = 42,
    iso20_SignatureValue = 43,
    iso20_SignedInfo = 44,
    iso20_SignedInstallationData = 45,
    iso20_SignedMeteringData = 46,
    iso20_Transform = 47,
    iso20_Transforms = 48,
    iso20_VehicleCheckInReq = 49,
    iso20_VehicleCheckInRes = 50,
    iso20_VehicleCheckOutReq = 51,
    iso20_VehicleCheckOutRes = 52,
    iso20_X509Data = 53
} iso20_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerToleranceAcceptance; type={urn:iso:std:iso:15118:-20:CommonMessages}powerToleranceAcceptanceType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_powerToleranceAcceptanceType_PowerToleranceNotConfirmed = 0,
    iso20_powerToleranceAcceptanceType_PowerToleranceConfirmed = 1
} iso20_powerToleranceAcceptanceType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}EVSENotification; type={urn:iso:std:iso:15118:-20:CommonTypes}evseNotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_evseNotificationType_Pause = 0,
    iso20_evseNotificationType_ExitStandby = 1,
    iso20_evseNotificationType_Terminate = 2,
    iso20_evseNotificationType_ScheduleRenegotiation = 3,
    iso20_evseNotificationType_ServiceRenegotiation = 4,
    iso20_evseNotificationType_MeteringConfirmation = 5
} iso20_evseNotificationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}ECDHCurve; type={urn:iso:std:iso:15118:-20:CommonMessages}ecdhCurveType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_ecdhCurveType_SECP521 = 0,
    iso20_ecdhCurveType_X448 = 1
} iso20_ecdhCurveType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonTypes}ResponseCode; type={urn:iso:std:iso:15118:-20:CommonTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_responseCodeType_OK = 0,
    iso20_responseCodeType_OK_CertificateExpiresSoon = 1,
    iso20_responseCodeType_OK_NewSessionEstablished = 2,
    iso20_responseCodeType_OK_OldSessionJoined = 3,
    iso20_responseCodeType_OK_PowerToleranceConfirmed = 4,
    iso20_responseCodeType_WARNING_AuthorizationSelectionInvalid = 5,
    iso20_responseCodeType_WARNING_CertificateExpired = 6,
    iso20_responseCodeType_WARNING_CertificateNotYetValid = 7,
    iso20_responseCodeType_WARNING_CertificateRevoked = 8,
    iso20_responseCodeType_WARNING_CertificateValidationError = 9,
    iso20_responseCodeType_WARNING_ChallengeInvalid = 10,
    iso20_responseCodeType_WARNING_EIMAuthorizationFailure = 11,
    iso20_responseCodeType_WARNING_eMSPUnknown = 12,
    iso20_responseCodeType_WARNING_EVPowerProfileViolation = 13,
    iso20_responseCodeType_WARNING_GeneralPnCAuthorizationError = 14,
    iso20_responseCodeType_WARNING_NoCertificateAvailable = 15,
    iso20_responseCodeType_WARNING_NoContractMatchingPCIDFound = 16,
    iso20_responseCodeType_WARNING_PowerToleranceNotConfirmed = 17,
    iso20_responseCodeType_WARNING_ScheduleRenegotiationFailed = 18,
    iso20_responseCodeType_WARNING_StandbyNotAllowed = 19,
    iso20_responseCodeType_WARNING_WPT = 20,
    iso20_responseCodeType_FAILED = 21,
    iso20_responseCodeType_FAILED_AssociationError = 22,
    iso20_responseCodeType_FAILED_ContactorError = 23,
    iso20_responseCodeType_FAILED_EVPowerProfileInvalid = 24,
    iso20_responseCodeType_FAILED_EVPowerProfileViolation = 25,
    iso20_responseCodeType_FAILED_MeteringSignatureNotValid = 26,
    iso20_responseCodeType_FAILED_NoEnergyTransferServiceSelected = 27,
    iso20_responseCodeType_FAILED_NoServiceRenegotiationSupported = 28,
    iso20_responseCodeType_FAILED_PauseNotAllowed = 29,
    iso20_responseCodeType_FAILED_PowerDeliveryNotApplied = 30,
    iso20_responseCodeType_FAILED_PowerToleranceNotConfirmed = 31,
    iso20_responseCodeType_FAILED_ScheduleRenegotiation = 32,
    iso20_responseCodeType_FAILED_ScheduleSelectionInvalid = 33,
    iso20_responseCodeType_FAILED_SequenceError = 34,
    iso20_responseCodeType_FAILED_ServiceIDInvalid = 35,
    iso20_responseCodeType_FAILED_ServiceSelectionInvalid = 36,
    iso20_responseCodeType_FAILED_SignatureError = 37,
    iso20_responseCodeType_FAILED_UnknownSession = 38,
    iso20_responseCodeType_FAILED_WrongChargeParameter = 39
} iso20_responseCodeType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}ChargingSession; type={urn:iso:std:iso:15118:-20:CommonMessages}chargingSessionType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_chargingSessionType_Pause = 0,
    iso20_chargingSessionType_Terminate = 1,
    iso20_chargingSessionType_ServiceRenegotiation = 2
} iso20_chargingSessionType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}EVCheckInStatus; type={urn:iso:std:iso:15118:-20:CommonMessages}evCheckInStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_evCheckInStatusType_CheckIn = 0,
    iso20_evCheckInStatusType_Processing = 1,
    iso20_evCheckInStatusType_Completed = 2
} iso20_evCheckInStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}EVCheckOutStatus; type={urn:iso:std:iso:15118:-20:CommonMessages}evCheckOutStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_evCheckOutStatusType_CheckOut = 0,
    iso20_evCheckOutStatusType_Processing = 1,
    iso20_evCheckOutStatusType_Completed = 2
} iso20_evCheckOutStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationServices; type={urn:iso:std:iso:15118:-20:CommonMessages}authorizationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_authorizationType_EIM = 0,
    iso20_authorizationType_PnC = 1
} iso20_authorizationType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}EVSEProcessing; type={urn:iso:std:iso:15118:-20:CommonTypes}processingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_processingType_Finished = 0,
    iso20_processingType_Ongoing = 1,
    iso20_processingType_Ongoing_WaitingForCustomerInteraction = 2
} iso20_processingType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}ChargeProgress; type={urn:iso:std:iso:15118:-20:CommonMessages}chargeProgressType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_chargeProgressType_Start = 0,
    iso20_chargeProgressType_Stop = 1,
    iso20_chargeProgressType_Standby = 2,
    iso20_chargeProgressType_ScheduleRenegotiation = 3
} iso20_chargeProgressType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}ParkingMethod; type={urn:iso:std:iso:15118:-20:CommonMessages}parkingMethodType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_parkingMethodType_AutoParking = 0,
    iso20_parkingMethodType_MVGuideManual = 1,
    iso20_parkingMethodType_Manual = 2
} iso20_parkingMethodType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}EVSECheckOutStatus; type={urn:iso:std:iso:15118:-20:CommonMessages}evseCheckOutStatusType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_evseCheckOutStatusType_Scheduled = 0,
    iso20_evseCheckOutStatusType_Completed = 1
} iso20_evseCheckOutStatusType;

// Element: definition=enum; name={urn:iso:std:iso:15118:-20:CommonMessages}BPT_ChannelSelection; type={urn:iso:std:iso:15118:-20:CommonMessages}channelSelectionType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso20_channelSelectionType_Charge = 0,
    iso20_channelSelectionType_Discharge = 1
} iso20_channelSelectionType;

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso20_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso20_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso20_TransformsType {
    // Transform, TransformType
    struct iso20_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso20_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso20_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso20_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso20_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso20_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); PriceLevel, unsignedByte (1, 1);
struct iso20_PriceLevelScheduleEntryType {
    // Duration, unsignedInt (base: unsignedLong)
    uint32_t Duration;
    // PriceLevel, unsignedByte (base: unsignedShort)
    uint8_t PriceLevel;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso20_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso20_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso20_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso20_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso20_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso20_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso20_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso20_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso20_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso20_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso20_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso20_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso20_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso20_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso20_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_2;
        unsigned int choice_2_isUsed:1;


    };
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}rationalNumber; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
struct iso20_RationalNumberType {
    // Exponent, byte (base: short)
    int8_t Exponent;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); Power, RationalNumberType (1, 1); Power_L2, RationalNumberType (0, 1); Power_L3, RationalNumberType (0, 1);
struct iso20_PowerScheduleEntryType {
    // Duration, unsignedInt (base: unsignedLong)
    uint32_t Duration;
    // Power, RationalNumberType
    struct iso20_RationalNumberType Power;
    // Power_L2, RationalNumberType
    struct iso20_RationalNumberType Power_L2;
    unsigned int Power_L2_isUsed:1;
    // Power_L3, RationalNumberType
    struct iso20_RationalNumberType Power_L3;
    unsigned int Power_L3_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyFee, RationalNumberType (1, 1); PowerRangeStart, RationalNumberType (1, 1);
struct iso20_EVPriceRuleType {
    // EnergyFee, RationalNumberType
    struct iso20_RationalNumberType EnergyFee;
    // PowerRangeStart, RationalNumberType
    struct iso20_RationalNumberType PowerRangeStart;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); Power, RationalNumberType (1, 1);
struct iso20_EVPowerScheduleEntryType {
    // Duration, unsignedInt (base: unsignedLong)
    uint32_t Duration;
    // Power, RationalNumberType
    struct iso20_RationalNumberType Power;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStack; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStackType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); EVPriceRule, EVPriceRuleType (1, 8);
struct iso20_EVPriceRuleStackType {
    // Duration, unsignedInt (base: unsignedLong)
    uint32_t Duration;
    // EVPriceRule, EVPriceRuleType
    struct {
        struct iso20_EVPriceRuleType array[iso20_EVPriceRuleType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } EVPriceRule;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRule; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyFee, RationalNumberType (1, 1); ParkingFee, RationalNumberType (0, 1); ParkingFeePeriod, unsignedInt (0, 1); CarbonDioxideEmission, unsignedShort (0, 1); RenewableGenerationPercentage, unsignedByte (0, 1); PowerRangeStart, RationalNumberType (1, 1);
struct iso20_PriceRuleType {
    // EnergyFee, RationalNumberType
    struct iso20_RationalNumberType EnergyFee;
    // ParkingFee, RationalNumberType
    struct iso20_RationalNumberType ParkingFee;
    unsigned int ParkingFee_isUsed:1;
    // ParkingFeePeriod, unsignedInt (base: unsignedLong)
    uint32_t ParkingFeePeriod;
    unsigned int ParkingFeePeriod_isUsed:1;
    // CarbonDioxideEmission, unsignedShort (base: unsignedInt)
    uint16_t CarbonDioxideEmission;
    unsigned int CarbonDioxideEmission_isUsed:1;
    // RenewableGenerationPercentage, unsignedByte (base: unsignedShort)
    uint8_t RenewableGenerationPercentage;
    unsigned int RenewableGenerationPercentage_isUsed:1;
    // PowerRangeStart, RationalNumberType
    struct iso20_RationalNumberType PowerRangeStart;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PowerScheduleEntry, PowerScheduleEntryType (1, 1024);
struct iso20_PowerScheduleEntryListType {
    // PowerScheduleEntry, PowerScheduleEntryType
    struct {
        struct iso20_PowerScheduleEntryType array[iso20_PowerScheduleEntryType_1024_ARRAY_SIZE];
        uint16_t arrayLen;
    } PowerScheduleEntry;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}TaxRule; type={urn:iso:std:iso:15118:-20:CommonMessages}TaxRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); TaxRuleName, nameType (0, 1); TaxRate, RationalNumberType (1, 1); TaxIncludedInPrice, boolean (0, 1); AppliesToEnergyFee, boolean (1, 1); AppliesToParkingFee, boolean (1, 1); AppliesToOverstayFee, boolean (1, 1); AppliesMinimumMaximumCost, boolean (1, 1);
struct iso20_TaxRuleType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // TaxRuleName, nameType (base: string)
    struct {
        char characters[iso20_TaxRuleName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } TaxRuleName;
    unsigned int TaxRuleName_isUsed:1;
    // TaxRate, RationalNumberType
    struct iso20_RationalNumberType TaxRate;
    // TaxIncludedInPrice, boolean
    int TaxIncludedInPrice;
    unsigned int TaxIncludedInPrice_isUsed:1;
    // AppliesToEnergyFee, boolean
    int AppliesToEnergyFee;
    // AppliesToParkingFee, boolean
    int AppliesToParkingFee;
    // AppliesToOverstayFee, boolean
    int AppliesToOverstayFee;
    // AppliesMinimumMaximumCost, boolean
    int AppliesMinimumMaximumCost;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStack; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStackType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); PriceRule, PriceRuleType (1, 8);
struct iso20_PriceRuleStackType {
    // Duration, unsignedInt (base: unsignedLong)
    uint32_t Duration;
    // PriceRule, PriceRuleType
    struct {
        struct iso20_PriceRuleType array[iso20_PriceRuleType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } PriceRule;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalService; type={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceName, nameType (1, 1); ServiceFee, RationalNumberType (1, 1);
struct iso20_AdditionalServiceType {
    // ServiceName, nameType (base: string)
    struct {
        char characters[iso20_ServiceName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceName;
    // ServiceFee, RationalNumberType
    struct iso20_RationalNumberType ServiceFee;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); AvailableEnergy, RationalNumberType (0, 1); PowerTolerance, RationalNumberType (0, 1); PowerScheduleEntries, PowerScheduleEntryListType (1, 1);
struct iso20_PowerScheduleType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // AvailableEnergy, RationalNumberType
    struct iso20_RationalNumberType AvailableEnergy;
    unsigned int AvailableEnergy_isUsed:1;
    // PowerTolerance, RationalNumberType
    struct iso20_RationalNumberType PowerTolerance;
    unsigned int PowerTolerance_isUsed:1;
    // PowerScheduleEntries, PowerScheduleEntryListType
    struct iso20_PowerScheduleEntryListType PowerScheduleEntries;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerScheduleEntry, EVPowerScheduleEntryType (1, 1024);
struct iso20_EVPowerScheduleEntryListType {
    // EVPowerScheduleEntry, EVPowerScheduleEntryType
    struct {
        struct iso20_EVPowerScheduleEntryType array[iso20_EVPowerScheduleEntryType_1024_ARRAY_SIZE];
        uint16_t arrayLen;
    } EVPowerScheduleEntry;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRule; type={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OverstayRuleDescription, descriptionType (0, 1); StartTime, unsignedInt (1, 1); OverstayFee, RationalNumberType (1, 1); OverstayFeePeriod, unsignedInt (1, 1);
struct iso20_OverstayRuleType {
    // OverstayRuleDescription, descriptionType (base: string)
    struct {
        char characters[iso20_OverstayRuleDescription_CHARACTER_SIZE];
        uint16_t charactersLen;
    } OverstayRuleDescription;
    unsigned int OverstayRuleDescription_isUsed:1;
    // StartTime, unsignedInt (base: unsignedLong)
    uint32_t StartTime;
    // OverstayFee, RationalNumberType
    struct iso20_RationalNumberType OverstayFee;
    // OverstayFeePeriod, unsignedInt (base: unsignedLong)
    uint32_t OverstayFeePeriod;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStacks; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStackListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPriceRuleStack, EVPriceRuleStackType (1, 1024);
struct iso20_EVPriceRuleStackListType {
    // EVPriceRuleStack, EVPriceRuleStackType
    struct {
        struct iso20_EVPriceRuleStackType array[iso20_EVPriceRuleStackType_1024_ARRAY_SIZE];
        uint16_t arrayLen;
    } EVPriceRuleStack;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SPKIData; type={http://www.w3.org/2000/09/xmldsig#}SPKIDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPKISexp, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso20_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso20_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso20_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso20_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso20_ReferenceType array[iso20_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EVPowerScheduleEntries, EVPowerScheduleEntryListType (1, 1);
struct iso20_EVPowerScheduleType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EVPowerScheduleEntries, EVPowerScheduleEntryListType
    struct iso20_EVPowerScheduleEntryListType EVPowerScheduleEntries;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso20_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SubCertificates; type={urn:iso:std:iso:15118:-20:CommonMessages}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 3);
struct iso20_SubCertificatesType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso20_certificateType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso20_certificateType_3_ARRAY_SIZE];
        uint16_t arrayLen;
    } Certificate;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Parameter; type={urn:iso:std:iso:15118:-20:CommonMessages}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, nameType (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); rationalNumber, RationalNumberType (0, 1); finiteString, nameType (0, 1);
struct iso20_ParameterType {
    // Attribute: Name, nameType (base: string)
    struct {
        char characters[iso20_Name_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Name;
    // boolValue, boolean
    int boolValue;
    unsigned int boolValue_isUsed:1;
    // byteValue, byte (base: short)
    int8_t byteValue;
    unsigned int byteValue_isUsed:1;
    // shortValue, short (base: int)
    int16_t shortValue;
    unsigned int shortValue_isUsed:1;
    // intValue, int (base: long)
    int32_t intValue;
    unsigned int intValue_isUsed:1;
    // rationalNumber, RationalNumberType
    struct iso20_RationalNumberType rationalNumber;
    unsigned int rationalNumber_isUsed:1;
    // finiteString, nameType (base: string)
    struct {
        char characters[iso20_finiteString_CHARACTER_SIZE];
        uint16_t charactersLen;
    } finiteString;
    unsigned int finiteString_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVAbsolutePriceSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVAbsolutePriceScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); Currency, currencyType (1, 1); PriceAlgorithm, identifierType (1, 1); EVPriceRuleStacks, EVPriceRuleStackListType (1, 1);
struct iso20_EVAbsolutePriceScheduleType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // Currency, currencyType (base: string)
    struct {
        char characters[iso20_Currency_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Currency;
    // PriceAlgorithm, identifierType (base: string)
    struct {
        char characters[iso20_PriceAlgorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } PriceAlgorithm;
    // EVPriceRuleStacks, EVPriceRuleStackListType
    struct iso20_EVPriceRuleStackListType EVPriceRuleStacks;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
struct iso20_DetailedCostType {
    // Amount, RationalNumberType
    struct iso20_RationalNumberType Amount;
    // CostPerUnit, RationalNumberType
    struct iso20_RationalNumberType CostPerUnit;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso20_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso20_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso20_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso20_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso20_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso20_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso20_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso20_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso20_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso20_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso20_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PriceLevelScheduleEntry, PriceLevelScheduleEntryType (1, 1024);
struct iso20_PriceLevelScheduleEntryListType {
    // PriceLevelScheduleEntry, PriceLevelScheduleEntryType
    struct {
        struct iso20_PriceLevelScheduleEntryType array[iso20_PriceLevelScheduleEntryType_1024_ARRAY_SIZE];
        uint16_t arrayLen;
    } PriceLevelScheduleEntry;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
struct iso20_DetailedTaxType {
    // TaxRuleID, numericIDType (base: unsignedInt)
    uint32_t TaxRuleID;
    // Amount, RationalNumberType
    struct iso20_RationalNumberType Amount;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}TaxRules; type={urn:iso:std:iso:15118:-20:CommonMessages}TaxRuleListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRule, TaxRuleType (1, 10);
struct iso20_TaxRuleListType {
    // TaxRule, TaxRuleType
    struct {
        struct iso20_TaxRuleType array[iso20_TaxRuleType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxRule;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStacks; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStackListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PriceRuleStack, PriceRuleStackType (1, 64) (original max 1024);
struct iso20_PriceRuleStackListType {
    // PriceRuleStack, PriceRuleStackType
    struct {
        struct iso20_PriceRuleStackType array[iso20_PriceRuleStackType_64_ARRAY_SIZE];
        uint16_t arrayLen;
    } PriceRuleStack;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRules; type={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRuleListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OverstayTimeThreshold, unsignedInt (0, 1); OverstayPowerThreshold, RationalNumberType (0, 1); OverstayRule, OverstayRuleType (1, 5);
struct iso20_OverstayRuleListType {
    // OverstayTimeThreshold, unsignedInt (base: unsignedLong)
    uint32_t OverstayTimeThreshold;
    unsigned int OverstayTimeThreshold_isUsed:1;
    // OverstayPowerThreshold, RationalNumberType
    struct iso20_RationalNumberType OverstayPowerThreshold;
    unsigned int OverstayPowerThreshold_isUsed:1;
    // OverstayRule, OverstayRuleType
    struct {
        struct iso20_OverstayRuleType array[iso20_OverstayRuleType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } OverstayRule;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalSelectedServices; type={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AdditionalService, AdditionalServiceType (1, 5);
struct iso20_AdditionalServiceListType {
    // AdditionalService, AdditionalServiceType
    struct {
        struct iso20_AdditionalServiceType array[iso20_AdditionalServiceType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } AdditionalService;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Service; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); FreeService, boolean (1, 1);
struct iso20_ServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // FreeService, boolean
    int FreeService;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ParameterSet; type={urn:iso:std:iso:15118:-20:CommonMessages}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, serviceIDType (1, 1); Parameter, ParameterType (1, 8) (original max 32);
struct iso20_ParameterSetType {
    // ParameterSetID, serviceIDType (base: unsignedShort)
    uint16_t ParameterSetID;
    // Parameter, ParameterType
    struct {
        struct iso20_ParameterType array[iso20_ParameterType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } Parameter;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SupportedProviders; type={urn:iso:std:iso:15118:-20:CommonMessages}SupportedProvidersListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProviderID, nameType (1, 128);
struct iso20_SupportedProvidersListType {
    // ProviderID, nameType (base: string)
    struct {
        struct {
            char characters[iso20_ProviderID_CHARACTER_SIZE];
            uint16_t charactersLen;
        } array[iso20_nameType_128_ARRAY_SIZE];
        uint16_t arrayLen;
    } ProviderID;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ContractCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}ContractCertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (1, 1);
struct iso20_ContractCertificateChainType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;

    // SubCertificates, SubCertificatesType
    struct iso20_SubCertificatesType SubCertificates;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_EVPPTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_EVPPTControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_Dynamic_EVPPTControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
struct iso20_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso20_MeterID_CHARACTER_SIZE];
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
        uint8_t bytes[iso20_meterSignatureType_BYTES_SIZE];
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso20_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso20_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso20_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso20_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso20_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_EVPPTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_EVPPTControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedScheduleTupleID, numericIDType (1, 1); PowerToleranceAcceptance, powerToleranceAcceptanceType (0, 1);
struct iso20_Scheduled_EVPPTControlModeType {
    // SelectedScheduleTupleID, numericIDType (base: unsignedInt)
    uint32_t SelectedScheduleTupleID;
    // PowerToleranceAcceptance, powerToleranceAcceptanceType (base: string)
    iso20_powerToleranceAcceptanceType PowerToleranceAcceptance;
    unsigned int PowerToleranceAcceptance_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
struct iso20_ReceiptType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // EnergyCosts, DetailedCostType
    struct iso20_DetailedCostType EnergyCosts;
    unsigned int EnergyCosts_isUsed:1;
    // OccupancyCosts, DetailedCostType
    struct iso20_DetailedCostType OccupancyCosts;
    unsigned int OccupancyCosts_isUsed:1;
    // AdditionalServicesCosts, DetailedCostType
    struct iso20_DetailedCostType AdditionalServicesCosts;
    unsigned int AdditionalServicesCosts_isUsed:1;
    // OverstayCosts, DetailedCostType
    struct iso20_DetailedCostType OverstayCosts;
    unsigned int OverstayCosts_isUsed:1;
    // TaxCosts, DetailedTaxType
    struct {
        struct iso20_DetailedTaxType array[iso20_DetailedTaxType_10_ARRAY_SIZE];
        uint16_t arrayLen;
    } TaxCosts;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AbsolutePriceSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}AbsolutePriceScheduleType; base type=PriceScheduleType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); TimeAnchor, unsignedLong (1, 1); PriceScheduleID, numericIDType (1, 1); PriceScheduleDescription, descriptionType (0, 1); Currency, currencyType (1, 1); Language, languageType (1, 1); PriceAlgorithm, identifierType (1, 1); MinimumCost, RationalNumberType (0, 1); MaximumCost, RationalNumberType (0, 1); TaxRules, TaxRuleListType (0, 1); PriceRuleStacks, PriceRuleStackListType (1, 1); OverstayRules, OverstayRuleListType (0, 1); AdditionalSelectedServices, AdditionalServiceListType (0, 1);
struct iso20_AbsolutePriceScheduleType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // PriceScheduleID, numericIDType (base: unsignedInt)
    uint32_t PriceScheduleID;
    // PriceScheduleDescription, descriptionType (base: string)
    struct {
        char characters[iso20_PriceScheduleDescription_CHARACTER_SIZE];
        uint16_t charactersLen;
    } PriceScheduleDescription;
    unsigned int PriceScheduleDescription_isUsed:1;
    // Currency, currencyType (base: string)
    struct {
        char characters[iso20_Currency_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Currency;
    // Language, languageType (base: string)
    struct {
        char characters[iso20_Language_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Language;
    // PriceAlgorithm, identifierType (base: string)
    struct {
        char characters[iso20_PriceAlgorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } PriceAlgorithm;
    // MinimumCost, RationalNumberType
    struct iso20_RationalNumberType MinimumCost;
    unsigned int MinimumCost_isUsed:1;
    // MaximumCost, RationalNumberType
    struct iso20_RationalNumberType MaximumCost;
    unsigned int MaximumCost_isUsed:1;
    // TaxRules, TaxRuleListType
    struct iso20_TaxRuleListType TaxRules;
    unsigned int TaxRules_isUsed:1;
    // PriceRuleStacks, PriceRuleStackListType
    struct iso20_PriceRuleStackListType PriceRuleStacks;
    // OverstayRules, OverstayRuleListType
    struct iso20_OverstayRuleListType OverstayRules;
    unsigned int OverstayRules_isUsed:1;
    // AdditionalSelectedServices, AdditionalServiceListType
    struct iso20_AdditionalServiceListType AdditionalSelectedServices;
    unsigned int AdditionalSelectedServices_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerProfileEntry, PowerScheduleEntryType (1, 2048);
struct iso20_EVPowerProfileEntryListType {
    // EVPowerProfileEntry, PowerScheduleEntryType
    struct {
        struct iso20_PowerScheduleEntryType array[iso20_PowerScheduleEntryType_2048_ARRAY_SIZE];
        uint16_t arrayLen;
    } EVPowerProfileEntry;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SMDTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SMDTControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_Dynamic_SMDTControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVEnergyOffer; type={urn:iso:std:iso:15118:-20:CommonMessages}EVEnergyOfferType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerSchedule, EVPowerScheduleType (1, 1); EVAbsolutePriceSchedule, EVAbsolutePriceScheduleType (1, 1);
struct iso20_EVEnergyOfferType {
    // EVPowerSchedule, EVPowerScheduleType
    struct iso20_EVPowerScheduleType EVPowerSchedule;
    // EVAbsolutePriceSchedule, EVAbsolutePriceScheduleType
    struct iso20_EVAbsolutePriceScheduleType EVAbsolutePriceSchedule;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleType; base type=PriceScheduleType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); TimeAnchor, unsignedLong (1, 1); PriceScheduleID, numericIDType (1, 1); PriceScheduleDescription, descriptionType (0, 1); NumberOfPriceLevels, unsignedByte (1, 1); PriceLevelScheduleEntries, PriceLevelScheduleEntryListType (1, 1);
struct iso20_PriceLevelScheduleType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // PriceScheduleID, numericIDType (base: unsignedInt)
    uint32_t PriceScheduleID;
    // PriceScheduleDescription, descriptionType (base: string)
    struct {
        char characters[iso20_PriceScheduleDescription_CHARACTER_SIZE];
        uint16_t charactersLen;
    } PriceScheduleDescription;
    unsigned int PriceScheduleDescription_isUsed:1;
    // NumberOfPriceLevels, unsignedByte (base: unsignedShort)
    uint8_t NumberOfPriceLevels;
    // PriceLevelScheduleEntries, PriceLevelScheduleEntryListType
    struct iso20_PriceLevelScheduleEntryListType PriceLevelScheduleEntries;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ChargingSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}ChargingScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PowerSchedule, PowerScheduleType (1, 1); AbsolutePriceSchedule, AbsolutePriceScheduleType (0, 1); PriceLevelSchedule, PriceLevelScheduleType (0, 1);
struct iso20_ChargingScheduleType {
    // PowerSchedule, PowerScheduleType
    struct iso20_PowerScheduleType PowerSchedule;
    // AbsolutePriceSchedule, AbsolutePriceScheduleType (base: PriceScheduleType)
    struct iso20_AbsolutePriceScheduleType AbsolutePriceSchedule;
    unsigned int AbsolutePriceSchedule_isUsed:1;
    // PriceLevelSchedule, PriceLevelScheduleType (base: PriceScheduleType)
    struct iso20_PriceLevelScheduleType PriceLevelSchedule;
    unsigned int PriceLevelSchedule_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleTuple; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ScheduleTupleID, numericIDType (1, 1); ChargingSchedule, ChargingScheduleType (1, 1); DischargingSchedule, ChargingScheduleType (0, 1);
struct iso20_ScheduleTupleType {
    // ScheduleTupleID, numericIDType (base: unsignedInt)
    uint32_t ScheduleTupleID;
    // ChargingSchedule, ChargingScheduleType
    struct iso20_ChargingScheduleType ChargingSchedule;
    // DischargingSchedule, ChargingScheduleType
    struct iso20_ChargingScheduleType DischargingSchedule;
    unsigned int DischargingSchedule_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SMDTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SMDTControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedScheduleTupleID, numericIDType (1, 1);
struct iso20_Scheduled_SMDTControlModeType {
    // SelectedScheduleTupleID, numericIDType (base: unsignedInt)
    uint32_t SelectedScheduleTupleID;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
struct iso20_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // TimeStamp, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeStamp;
    // Signature, SignatureType
    struct iso20_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
struct iso20_SignaturePropertyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Target, anyURI
    struct {
        char characters[iso20_Target_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Target;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SupportedServiceIDs; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceIDListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 16);
struct iso20_ServiceIDListType {
    // ServiceID, serviceIDType (base: unsignedShort)
    struct {
        uint16_t array[iso20_serviceIDType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } ServiceID;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SelectedEnergyTransferService; type={urn:iso:std:iso:15118:-20:CommonMessages}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, serviceIDType (1, 1);
struct iso20_SelectedServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ParameterSetID, serviceIDType (base: unsignedShort)
    uint16_t ParameterSetID;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SignedMeteringData; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedMeteringDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); SessionID, sessionIDType (1, 1); MeterInfo, MeterInfoType (1, 1); Receipt, ReceiptType (0, 1); Dynamic_SMDTControlMode, Dynamic_SMDTControlModeType (0, 1); Scheduled_SMDTControlMode, Scheduled_SMDTControlModeType (0, 1);
struct iso20_SignedMeteringDataType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso20_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // MeterInfo, MeterInfoType
    struct iso20_MeterInfoType MeterInfo;
    // Receipt, ReceiptType
    struct iso20_ReceiptType Receipt;
    unsigned int Receipt_isUsed:1;
    // Dynamic_SMDTControlMode, Dynamic_SMDTControlModeType
    struct iso20_Dynamic_SMDTControlModeType Dynamic_SMDTControlMode;
    unsigned int Dynamic_SMDTControlMode_isUsed:1;
    // Scheduled_SMDTControlMode, Scheduled_SMDTControlModeType
    struct iso20_Scheduled_SMDTControlModeType Scheduled_SMDTControlMode;
    unsigned int Scheduled_SMDTControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OEMProvisioningCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedCertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
struct iso20_SignedCertificateChainType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;

    // SubCertificates, SubCertificatesType
    struct iso20_SubCertificatesType SubCertificates;
    unsigned int SubCertificates_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EIM_AReqAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}EIM_AReqAuthorizationModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_EIM_AReqAuthorizationModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SelectedVASList; type={urn:iso:std:iso:15118:-20:CommonMessages}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16);
struct iso20_SelectedServiceListType {
    // SelectedService, SelectedServiceType
    struct {
        struct iso20_SelectedServiceType array[iso20_SelectedServiceType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } SelectedService;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEReqControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEReqControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (1, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
struct iso20_Dynamic_SEReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    // MinimumSOC, percentValueType (base: byte)
    int8_t MinimumSOC;
    unsigned int MinimumSOC_isUsed:1;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVTargetEnergyRequest;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMaximumEnergyRequest;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMinimumEnergyRequest;
    // EVMaximumV2XEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMaximumV2XEnergyRequest;
    unsigned int EVMaximumV2XEnergyRequest_isUsed:1;
    // EVMinimumV2XEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMinimumV2XEnergyRequest;
    unsigned int EVMinimumV2XEnergyRequest_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
struct iso20_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, evseNotificationType (base: string)
    iso20_evseNotificationType EVSENotification;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ListOfRootCertificateIDs; type={urn:iso:std:iso:15118:-20:CommonTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, X509IssuerSerialType (1, 20);
struct iso20_ListOfRootCertificateIDsType {
    // RootCertificateID, X509IssuerSerialType
    struct {
        struct iso20_X509IssuerSerialType array[iso20_X509IssuerSerialType_20_ARRAY_SIZE];
        uint16_t arrayLen;
    } RootCertificateID;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PnC_AReqAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}PnC_AReqAuthorizationModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); GenChallenge, genChallengeType (1, 1); ContractCertificateChain, ContractCertificateChainType (1, 1);
struct iso20_PnC_AReqAuthorizationModeType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // GenChallenge, genChallengeType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_genChallengeType_BYTES_SIZE];
        uint16_t bytesLen;
    } GenChallenge;

    // ContractCertificateChain, ContractCertificateChainType
    struct iso20_ContractCertificateChainType ContractCertificateChain;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EnergyTransferServiceList; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 8);
struct iso20_ServiceListType {
    // Service, ServiceType
    struct {
        struct iso20_ServiceType array[iso20_ServiceType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } Service;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceParameterList; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 4) (original max 32);
struct iso20_ServiceParameterListType {
    // ParameterSet, ParameterSetType
    struct {
        struct iso20_ParameterSetType array[iso20_ParameterSetType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } ParameterSet;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEReqControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEReqControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVEnergyOffer, EVEnergyOfferType (0, 1);
struct iso20_Scheduled_SEReqControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EVTargetEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVTargetEnergyRequest;
    unsigned int EVTargetEnergyRequest_isUsed:1;
    // EVMaximumEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMaximumEnergyRequest;
    unsigned int EVMaximumEnergyRequest_isUsed:1;
    // EVMinimumEnergyRequest, RationalNumberType
    struct iso20_RationalNumberType EVMinimumEnergyRequest;
    unsigned int EVMinimumEnergyRequest_isUsed:1;
    // EVEnergyOffer, EVEnergyOfferType
    struct iso20_EVEnergyOfferType EVEnergyOffer;
    unsigned int EVEnergyOffer_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfile; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); Dynamic_EVPPTControlMode, Dynamic_EVPPTControlModeType (0, 1); Scheduled_EVPPTControlMode, Scheduled_EVPPTControlModeType (0, 1); EVPowerProfileEntries, EVPowerProfileEntryListType (1, 1);
struct iso20_EVPowerProfileType {
    // TimeAnchor, unsignedLong (base: nonNegativeInteger)
    uint64_t TimeAnchor;
    // Dynamic_EVPPTControlMode, Dynamic_EVPPTControlModeType
    struct iso20_Dynamic_EVPPTControlModeType Dynamic_EVPPTControlMode;
    unsigned int Dynamic_EVPPTControlMode_isUsed:1;
    // Scheduled_EVPPTControlMode, Scheduled_EVPPTControlModeType
    struct iso20_Scheduled_EVPPTControlModeType Scheduled_EVPPTControlMode;
    unsigned int Scheduled_EVPPTControlMode_isUsed:1;
    // EVPowerProfileEntries, EVPowerProfileEntryListType
    struct iso20_EVPowerProfileEntryListType EVPowerProfileEntries;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CPSCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
struct iso20_CertificateChainType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;

    // SubCertificates, SubCertificatesType
    struct iso20_SubCertificatesType SubCertificates;
    unsigned int SubCertificates_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EIM_ASResAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}EIM_ASResAuthorizationModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_EIM_ASResAuthorizationModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEResControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEResControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AbsolutePriceSchedule, AbsolutePriceScheduleType (0, 1); PriceLevelSchedule, PriceLevelScheduleType (0, 1);
struct iso20_Dynamic_SEResControlModeType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // MinimumSOC, percentValueType (base: byte)
    int8_t MinimumSOC;
    unsigned int MinimumSOC_isUsed:1;
    // TargetSOC, percentValueType (base: byte)
    int8_t TargetSOC;
    unsigned int TargetSOC_isUsed:1;
    // AbsolutePriceSchedule, AbsolutePriceScheduleType (base: PriceScheduleType)
    struct iso20_AbsolutePriceScheduleType AbsolutePriceSchedule;
    unsigned int AbsolutePriceSchedule_isUsed:1;
    // PriceLevelSchedule, PriceLevelScheduleType (base: PriceScheduleType)
    struct iso20_PriceLevelScheduleType PriceLevelSchedule;
    unsigned int PriceLevelSchedule_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PrioritizedEMAIDs; type={urn:iso:std:iso:15118:-20:CommonMessages}EMAIDListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EMAID, identifierType (1, 8);
struct iso20_EMAIDListType {
    // EMAID, identifierType (base: string)
    struct {
        struct {
            char characters[iso20_EMAID_CHARACTER_SIZE];
            uint16_t charactersLen;
        } array[iso20_identifierType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } EMAID;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SignedInstallationData; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedInstallationDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); ContractCertificateChain, ContractCertificateChainType (1, 1); ECDHCurve, ecdhCurveType (1, 1); DHPublicKey, dhPublicKeyType (1, 1); SECP521_EncryptedPrivateKey, secp521_EncryptedPrivateKeyType (0, 1); X448_EncryptedPrivateKey, x448_EncryptedPrivateKeyType (0, 1); TPM_EncryptedPrivateKey, tpm_EncryptedPrivateKeyType (0, 1);
struct iso20_SignedInstallationDataType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // ContractCertificateChain, ContractCertificateChainType
    struct iso20_ContractCertificateChainType ContractCertificateChain;
    // ECDHCurve, ecdhCurveType (base: string)
    iso20_ecdhCurveType ECDHCurve;
    // DHPublicKey, dhPublicKeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_dhPublicKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } DHPublicKey;

    // SECP521_EncryptedPrivateKey, secp521_EncryptedPrivateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_secp521_EncryptedPrivateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } SECP521_EncryptedPrivateKey;
    unsigned int SECP521_EncryptedPrivateKey_isUsed:1;

    // X448_EncryptedPrivateKey, x448_EncryptedPrivateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_x448_EncryptedPrivateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } X448_EncryptedPrivateKey;
    unsigned int X448_EncryptedPrivateKey_isUsed:1;

    // TPM_EncryptedPrivateKey, tpm_EncryptedPrivateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_tpm_EncryptedPrivateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } TPM_EncryptedPrivateKey;
    unsigned int TPM_EncryptedPrivateKey_isUsed:1;


};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PnC_ASResAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}PnC_ASResAuthorizationModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: GenChallenge, genChallengeType (1, 1); SupportedProviders, SupportedProvidersListType (0, 1);
struct iso20_PnC_ASResAuthorizationModeType {
    // GenChallenge, genChallengeType (base: base64Binary)
    struct {
        uint8_t bytes[iso20_genChallengeType_BYTES_SIZE];
        uint16_t bytesLen;
    } GenChallenge;

    // SupportedProviders, SupportedProvidersListType
    struct iso20_SupportedProvidersListType SupportedProviders;
    unsigned int SupportedProviders_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEResControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEResControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ScheduleTuple, ScheduleTupleType (1, 3);
struct iso20_Scheduled_SEResControlModeType {
    // ScheduleTuple, ScheduleTupleType
    struct {
        struct iso20_ScheduleTupleType array[iso20_ScheduleTupleType_3_ARRAY_SIZE];
        uint16_t arrayLen;
    } ScheduleTuple;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupReq; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCCID, identifierType (1, 1);
struct iso20_SessionSetupReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // EVCCID, identifierType (base: string)
    struct {
        char characters[iso20_EVCCID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVCCID;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupRes; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEID, identifierType (1, 1);
struct iso20_SessionSetupResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSEID, identifierType (base: string)
    struct {
        char characters[iso20_EVSEID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVSEID;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupReq; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1);
struct iso20_AuthorizationSetupReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupRes; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); AuthorizationServices, authorizationType (1, 2); CertificateInstallationService, boolean (1, 1); EIM_ASResAuthorizationMode, EIM_ASResAuthorizationModeType (0, 1); PnC_ASResAuthorizationMode, PnC_ASResAuthorizationModeType (0, 1);
struct iso20_AuthorizationSetupResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // AuthorizationServices, authorizationType (base: string)
    struct {
        iso20_authorizationType array[iso20_authorizationType_2_ARRAY_SIZE];
        uint16_t arrayLen;
    } AuthorizationServices;    // CertificateInstallationService, boolean
    int CertificateInstallationService;
    // EIM_ASResAuthorizationMode, EIM_ASResAuthorizationModeType
    struct iso20_EIM_ASResAuthorizationModeType EIM_ASResAuthorizationMode;
    unsigned int EIM_ASResAuthorizationMode_isUsed:1;
    // PnC_ASResAuthorizationMode, PnC_ASResAuthorizationModeType
    struct iso20_PnC_ASResAuthorizationModeType PnC_ASResAuthorizationMode;
    unsigned int PnC_ASResAuthorizationMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SelectedAuthorizationService, authorizationType (1, 1); EIM_AReqAuthorizationMode, EIM_AReqAuthorizationModeType (0, 1); PnC_AReqAuthorizationMode, PnC_AReqAuthorizationModeType (0, 1);
struct iso20_AuthorizationReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // SelectedAuthorizationService, authorizationType (base: string)
    iso20_authorizationType SelectedAuthorizationService;
    // EIM_AReqAuthorizationMode, EIM_AReqAuthorizationModeType
    struct iso20_EIM_AReqAuthorizationModeType EIM_AReqAuthorizationMode;
    unsigned int EIM_AReqAuthorizationMode_isUsed:1;
    // PnC_AReqAuthorizationMode, PnC_AReqAuthorizationModeType
    struct iso20_PnC_AReqAuthorizationModeType PnC_AReqAuthorizationMode;
    unsigned int PnC_AReqAuthorizationMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1);
struct iso20_AuthorizationResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_processingType EVSEProcessing;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SupportedServiceIDs, ServiceIDListType (0, 1);
struct iso20_ServiceDiscoveryReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // SupportedServiceIDs, ServiceIDListType
    struct iso20_ServiceIDListType SupportedServiceIDs;
    unsigned int SupportedServiceIDs_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ServiceRenegotiationSupported, boolean (1, 1); EnergyTransferServiceList, ServiceListType (1, 1); VASList, ServiceListType (0, 1);
struct iso20_ServiceDiscoveryResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // ServiceRenegotiationSupported, boolean
    int ServiceRenegotiationSupported;
    // EnergyTransferServiceList, ServiceListType
    struct iso20_ServiceListType EnergyTransferServiceList;
    // VASList, ServiceListType
    struct iso20_ServiceListType VASList;
    unsigned int VASList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ServiceID, serviceIDType (1, 1);
struct iso20_ServiceDetailReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (1, 1);
struct iso20_ServiceDetailResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceParameterList, ServiceParameterListType
    struct iso20_ServiceParameterListType ServiceParameterList;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SelectedEnergyTransferService, SelectedServiceType (1, 1); SelectedVASList, SelectedServiceListType (0, 1);
struct iso20_ServiceSelectionReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // SelectedEnergyTransferService, SelectedServiceType
    struct iso20_SelectedServiceType SelectedEnergyTransferService;
    // SelectedVASList, SelectedServiceListType
    struct iso20_SelectedServiceListType SelectedVASList;
    unsigned int SelectedVASList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
struct iso20_ServiceSelectionResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); MaximumSupportingPoints, maxSupportingPointsScheduleTupleType (1, 1); Dynamic_SEReqControlMode, Dynamic_SEReqControlModeType (0, 1); Scheduled_SEReqControlMode, Scheduled_SEReqControlModeType (0, 1);
struct iso20_ScheduleExchangeReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // MaximumSupportingPoints, maxSupportingPointsScheduleTupleType (base: unsignedShort)
    uint16_t MaximumSupportingPoints;
    // Dynamic_SEReqControlMode, Dynamic_SEReqControlModeType
    struct iso20_Dynamic_SEReqControlModeType Dynamic_SEReqControlMode;
    unsigned int Dynamic_SEReqControlMode_isUsed:1;
    // Scheduled_SEReqControlMode, Scheduled_SEReqControlModeType
    struct iso20_Scheduled_SEReqControlModeType Scheduled_SEReqControlMode;
    unsigned int Scheduled_SEReqControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); GoToPause, boolean (0, 1); Dynamic_SEResControlMode, Dynamic_SEResControlModeType (0, 1); Scheduled_SEResControlMode, Scheduled_SEResControlModeType (0, 1);
struct iso20_ScheduleExchangeResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_processingType EVSEProcessing;
    // GoToPause, boolean
    int GoToPause;
    unsigned int GoToPause_isUsed:1;
    // Dynamic_SEResControlMode, Dynamic_SEResControlModeType
    struct iso20_Dynamic_SEResControlModeType Dynamic_SEResControlMode;
    unsigned int Dynamic_SEResControlMode_isUsed:1;
    // Scheduled_SEResControlMode, Scheduled_SEResControlModeType
    struct iso20_Scheduled_SEResControlModeType Scheduled_SEResControlMode;
    unsigned int Scheduled_SEResControlMode_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryReq; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); ChargeProgress, chargeProgressType (1, 1); EVPowerProfile, EVPowerProfileType (0, 1); BPT_ChannelSelection, channelSelectionType (0, 1);
struct iso20_PowerDeliveryReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // EVProcessing, processingType (base: string)
    iso20_processingType EVProcessing;
    // ChargeProgress, chargeProgressType (base: string)
    iso20_chargeProgressType ChargeProgress;
    // EVPowerProfile, EVPowerProfileType
    struct iso20_EVPowerProfileType EVPowerProfile;
    unsigned int EVPowerProfile_isUsed:1;
    // BPT_ChannelSelection, channelSelectionType (base: string)
    iso20_channelSelectionType BPT_ChannelSelection;
    unsigned int BPT_ChannelSelection_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryRes; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1);
struct iso20_PowerDeliveryResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSEStatus, EVSEStatusType
    struct iso20_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SignedMeteringData, SignedMeteringDataType (1, 1);
struct iso20_MeteringConfirmationReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // SignedMeteringData, SignedMeteringDataType
    struct iso20_SignedMeteringDataType SignedMeteringData;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
struct iso20_MeteringConfirmationResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopReq; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ChargingSession, chargingSessionType (1, 1); EVTerminationCode, nameType (0, 1); EVTerminationExplanation, descriptionType (0, 1);
struct iso20_SessionStopReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ChargingSession, chargingSessionType (base: string)
    iso20_chargingSessionType ChargingSession;
    // EVTerminationCode, nameType (base: string)
    struct {
        char characters[iso20_EVTerminationCode_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVTerminationCode;
    unsigned int EVTerminationCode_isUsed:1;
    // EVTerminationExplanation, descriptionType (base: string)
    struct {
        char characters[iso20_EVTerminationExplanation_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVTerminationExplanation;
    unsigned int EVTerminationExplanation_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopRes; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
struct iso20_SessionStopResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); OEMProvisioningCertificateChain, SignedCertificateChainType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); MaximumContractCertificateChains, unsignedByte (1, 1); PrioritizedEMAIDs, EMAIDListType (0, 1);
struct iso20_CertificateInstallationReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // OEMProvisioningCertificateChain, SignedCertificateChainType
    struct iso20_SignedCertificateChainType OEMProvisioningCertificateChain;
    // ListOfRootCertificateIDs, ListOfRootCertificateIDsType
    struct iso20_ListOfRootCertificateIDsType ListOfRootCertificateIDs;
    // MaximumContractCertificateChains, unsignedByte (base: unsignedShort)
    uint8_t MaximumContractCertificateChains;
    // PrioritizedEMAIDs, EMAIDListType
    struct iso20_EMAIDListType PrioritizedEMAIDs;
    unsigned int PrioritizedEMAIDs_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); CPSCertificateChain, CertificateChainType (1, 1); SignedInstallationData, SignedInstallationDataType (1, 1); RemainingContractCertificateChains, unsignedByte (1, 1);
struct iso20_CertificateInstallationResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSEProcessing, processingType (base: string)
    iso20_processingType EVSEProcessing;
    // CPSCertificateChain, CertificateChainType
    struct iso20_CertificateChainType CPSCertificateChain;
    // SignedInstallationData, SignedInstallationDataType
    struct iso20_SignedInstallationDataType SignedInstallationData;
    // RemainingContractCertificateChains, unsignedByte (base: unsignedShort)
    uint8_t RemainingContractCertificateChains;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInReq; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCheckInStatus, evCheckInStatusType (1, 1); ParkingMethod, parkingMethodType (1, 1); VehicleFrame, short (0, 1); DeviceOffset, short (0, 1); VehicleTravel, short (0, 1);
struct iso20_VehicleCheckInReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // EVCheckInStatus, evCheckInStatusType (base: string)
    iso20_evCheckInStatusType EVCheckInStatus;
    // ParkingMethod, parkingMethodType (base: string)
    iso20_parkingMethodType ParkingMethod;
    // VehicleFrame, short (base: int)
    int16_t VehicleFrame;
    unsigned int VehicleFrame_isUsed:1;
    // DeviceOffset, short (base: int)
    int16_t DeviceOffset;
    unsigned int DeviceOffset_isUsed:1;
    // VehicleTravel, short (base: int)
    int16_t VehicleTravel;
    unsigned int VehicleTravel_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInRes; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ParkingSpace, short (0, 1); DeviceLocation, short (0, 1); TargetDistance, short (0, 1);
struct iso20_VehicleCheckInResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // ParkingSpace, short (base: int)
    int16_t ParkingSpace;
    unsigned int ParkingSpace_isUsed:1;
    // DeviceLocation, short (base: int)
    int16_t DeviceLocation;
    unsigned int DeviceLocation_isUsed:1;
    // TargetDistance, short (base: int)
    int16_t TargetDistance;
    unsigned int TargetDistance_isUsed:1;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutReq; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCheckOutStatus, evCheckOutStatusType (1, 1); CheckOutTime, unsignedLong (1, 1);
struct iso20_VehicleCheckOutReqType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // EVCheckOutStatus, evCheckOutStatusType (base: string)
    iso20_evCheckOutStatusType EVCheckOutStatus;
    // CheckOutTime, unsignedLong (base: nonNegativeInteger)
    uint64_t CheckOutTime;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutRes; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSECheckOutStatus, evseCheckOutStatusType (1, 1);
struct iso20_VehicleCheckOutResType {
    // Header, MessageHeaderType
    struct iso20_MessageHeaderType Header;
    // ResponseCode, responseCodeType (base: string)
    iso20_responseCodeType ResponseCode;
    // EVSECheckOutStatus, evseCheckOutStatusType (base: string)
    iso20_evseCheckOutStatusType EVSECheckOutStatus;

};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_CLReqControlModeType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct iso20_CLResControlModeType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso20_ManifestType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Reference, ReferenceType
    struct {
        struct iso20_ReferenceType array[iso20_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1) (original max unbounded);
struct iso20_SignaturePropertiesType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso20_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignatureProperty, SignaturePropertyType
    struct iso20_SignaturePropertyType SignatureProperty;

};



// root elements of EXI doc
struct iso20_exiDocument {
    union {
        struct iso20_SessionSetupReqType SessionSetupReq;
        struct iso20_SessionSetupResType SessionSetupRes;
        struct iso20_AuthorizationSetupReqType AuthorizationSetupReq;
        struct iso20_AuthorizationSetupResType AuthorizationSetupRes;
        struct iso20_AuthorizationReqType AuthorizationReq;
        struct iso20_AuthorizationResType AuthorizationRes;
        struct iso20_ServiceDiscoveryReqType ServiceDiscoveryReq;
        struct iso20_ServiceDiscoveryResType ServiceDiscoveryRes;
        struct iso20_ServiceDetailReqType ServiceDetailReq;
        struct iso20_ServiceDetailResType ServiceDetailRes;
        struct iso20_ServiceSelectionReqType ServiceSelectionReq;
        struct iso20_ServiceSelectionResType ServiceSelectionRes;
        struct iso20_ScheduleExchangeReqType ScheduleExchangeReq;
        struct iso20_ScheduleExchangeResType ScheduleExchangeRes;
        struct iso20_PowerDeliveryReqType PowerDeliveryReq;
        struct iso20_PowerDeliveryResType PowerDeliveryRes;
        struct iso20_MeteringConfirmationReqType MeteringConfirmationReq;
        struct iso20_MeteringConfirmationResType MeteringConfirmationRes;
        struct iso20_SessionStopReqType SessionStopReq;
        struct iso20_SessionStopResType SessionStopRes;
        struct iso20_CertificateInstallationReqType CertificateInstallationReq;
        struct iso20_CertificateInstallationResType CertificateInstallationRes;
        struct iso20_VehicleCheckInReqType VehicleCheckInReq;
        struct iso20_VehicleCheckInResType VehicleCheckInRes;
        struct iso20_VehicleCheckOutReqType VehicleCheckOutReq;
        struct iso20_VehicleCheckOutResType VehicleCheckOutRes;
        struct iso20_SignedInstallationDataType SignedInstallationData;
        struct iso20_SignedMeteringDataType SignedMeteringData;
        struct iso20_CLReqControlModeType CLReqControlMode;
        struct iso20_CLResControlModeType CLResControlMode;
        struct iso20_SignatureType Signature;
        struct iso20_SignatureValueType SignatureValue;
        struct iso20_SignedInfoType SignedInfo;
        struct iso20_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_SignatureMethodType SignatureMethod;
        struct iso20_ReferenceType Reference;
        struct iso20_TransformsType Transforms;
        struct iso20_TransformType Transform;
        struct iso20_DigestMethodType DigestMethod;
        struct iso20_KeyInfoType KeyInfo;
        struct iso20_KeyValueType KeyValue;
        struct iso20_RetrievalMethodType RetrievalMethod;
        struct iso20_X509DataType X509Data;
        struct iso20_PGPDataType PGPData;
        struct iso20_SPKIDataType SPKIData;
        struct iso20_ObjectType Object;
        struct iso20_ManifestType Manifest;
        struct iso20_SignaturePropertiesType SignatureProperties;
        struct iso20_SignaturePropertyType SignatureProperty;
        struct iso20_DSAKeyValueType DSAKeyValue;
        struct iso20_RSAKeyValueType RSAKeyValue;
    };
    unsigned int SessionSetupReq_isUsed:1;
    unsigned int SessionSetupRes_isUsed:1;
    unsigned int AuthorizationSetupReq_isUsed:1;
    unsigned int AuthorizationSetupRes_isUsed:1;
    unsigned int AuthorizationReq_isUsed:1;
    unsigned int AuthorizationRes_isUsed:1;
    unsigned int ServiceDiscoveryReq_isUsed:1;
    unsigned int ServiceDiscoveryRes_isUsed:1;
    unsigned int ServiceDetailReq_isUsed:1;
    unsigned int ServiceDetailRes_isUsed:1;
    unsigned int ServiceSelectionReq_isUsed:1;
    unsigned int ServiceSelectionRes_isUsed:1;
    unsigned int ScheduleExchangeReq_isUsed:1;
    unsigned int ScheduleExchangeRes_isUsed:1;
    unsigned int PowerDeliveryReq_isUsed:1;
    unsigned int PowerDeliveryRes_isUsed:1;
    unsigned int MeteringConfirmationReq_isUsed:1;
    unsigned int MeteringConfirmationRes_isUsed:1;
    unsigned int SessionStopReq_isUsed:1;
    unsigned int SessionStopRes_isUsed:1;
    unsigned int CertificateInstallationReq_isUsed:1;
    unsigned int CertificateInstallationRes_isUsed:1;
    unsigned int VehicleCheckInReq_isUsed:1;
    unsigned int VehicleCheckInRes_isUsed:1;
    unsigned int VehicleCheckOutReq_isUsed:1;
    unsigned int VehicleCheckOutRes_isUsed:1;
    unsigned int SignedInstallationData_isUsed:1;
    unsigned int SignedMeteringData_isUsed:1;
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
struct iso20_exiFragment {
    union {
        struct iso20_AbsolutePriceScheduleType AbsolutePriceSchedule;
        struct iso20_CertificateInstallationReqType CertificateInstallationReq;
        struct iso20_MeteringConfirmationReqType MeteringConfirmationReq;
        struct iso20_PnC_AReqAuthorizationModeType PnC_AReqAuthorizationMode;
        struct iso20_SignedInfoType SignedInfo;
        struct iso20_SignedInstallationDataType SignedInstallationData;
    };
    unsigned int AbsolutePriceSchedule_isUsed:1;
    unsigned int CertificateInstallationReq_isUsed:1;
    unsigned int MeteringConfirmationReq_isUsed:1;
    unsigned int PnC_AReqAuthorizationMode_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
    unsigned int SignedInstallationData_isUsed:1;
};

// elements of xmldsig fragment
struct iso20_xmldsigFragment {
    union {
        struct iso20_CanonicalizationMethodType CanonicalizationMethod;
        struct iso20_DSAKeyValueType DSAKeyValue;
        struct iso20_DigestMethodType DigestMethod;
        struct iso20_KeyInfoType KeyInfo;
        struct iso20_KeyValueType KeyValue;
        struct iso20_ManifestType Manifest;
        struct iso20_ObjectType Object;
        struct iso20_PGPDataType PGPData;
        struct iso20_RSAKeyValueType RSAKeyValue;
        struct iso20_ReferenceType Reference;
        struct iso20_RetrievalMethodType RetrievalMethod;
        struct iso20_SPKIDataType SPKIData;
        struct iso20_SignatureType Signature;
        struct iso20_SignatureMethodType SignatureMethod;
        struct iso20_SignaturePropertiesType SignatureProperties;
        struct iso20_SignaturePropertyType SignatureProperty;
        struct iso20_SignatureValueType SignatureValue;
        struct iso20_SignedInfoType SignedInfo;
        struct iso20_TransformType Transform;
        struct iso20_TransformsType Transforms;
        struct iso20_X509DataType X509Data;
        struct iso20_X509IssuerSerialType X509IssuerSerial;
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
void init_iso20_exiDocument(struct iso20_exiDocument* exiDoc);
void init_iso20_SessionSetupReqType(struct iso20_SessionSetupReqType* SessionSetupReq);
void init_iso20_SessionSetupResType(struct iso20_SessionSetupResType* SessionSetupRes);
void init_iso20_AuthorizationSetupReqType(struct iso20_AuthorizationSetupReqType* AuthorizationSetupReq);
void init_iso20_AuthorizationSetupResType(struct iso20_AuthorizationSetupResType* AuthorizationSetupRes);
void init_iso20_AuthorizationReqType(struct iso20_AuthorizationReqType* AuthorizationReq);
void init_iso20_AuthorizationResType(struct iso20_AuthorizationResType* AuthorizationRes);
void init_iso20_ServiceDiscoveryReqType(struct iso20_ServiceDiscoveryReqType* ServiceDiscoveryReq);
void init_iso20_ServiceDiscoveryResType(struct iso20_ServiceDiscoveryResType* ServiceDiscoveryRes);
void init_iso20_ServiceDetailReqType(struct iso20_ServiceDetailReqType* ServiceDetailReq);
void init_iso20_ServiceDetailResType(struct iso20_ServiceDetailResType* ServiceDetailRes);
void init_iso20_ServiceSelectionReqType(struct iso20_ServiceSelectionReqType* ServiceSelectionReq);
void init_iso20_ServiceSelectionResType(struct iso20_ServiceSelectionResType* ServiceSelectionRes);
void init_iso20_ScheduleExchangeReqType(struct iso20_ScheduleExchangeReqType* ScheduleExchangeReq);
void init_iso20_ScheduleExchangeResType(struct iso20_ScheduleExchangeResType* ScheduleExchangeRes);
void init_iso20_PowerDeliveryReqType(struct iso20_PowerDeliveryReqType* PowerDeliveryReq);
void init_iso20_PowerDeliveryResType(struct iso20_PowerDeliveryResType* PowerDeliveryRes);
void init_iso20_MeteringConfirmationReqType(struct iso20_MeteringConfirmationReqType* MeteringConfirmationReq);
void init_iso20_MeteringConfirmationResType(struct iso20_MeteringConfirmationResType* MeteringConfirmationRes);
void init_iso20_SessionStopReqType(struct iso20_SessionStopReqType* SessionStopReq);
void init_iso20_SessionStopResType(struct iso20_SessionStopResType* SessionStopRes);
void init_iso20_CertificateInstallationReqType(struct iso20_CertificateInstallationReqType* CertificateInstallationReq);
void init_iso20_CertificateInstallationResType(struct iso20_CertificateInstallationResType* CertificateInstallationRes);
void init_iso20_VehicleCheckInReqType(struct iso20_VehicleCheckInReqType* VehicleCheckInReq);
void init_iso20_VehicleCheckInResType(struct iso20_VehicleCheckInResType* VehicleCheckInRes);
void init_iso20_VehicleCheckOutReqType(struct iso20_VehicleCheckOutReqType* VehicleCheckOutReq);
void init_iso20_VehicleCheckOutResType(struct iso20_VehicleCheckOutResType* VehicleCheckOutRes);
void init_iso20_SignedInstallationDataType(struct iso20_SignedInstallationDataType* SignedInstallationData);
void init_iso20_SignedMeteringDataType(struct iso20_SignedMeteringDataType* SignedMeteringData);
void init_iso20_CLReqControlModeType(struct iso20_CLReqControlModeType* CLReqControlMode);
void init_iso20_CLResControlModeType(struct iso20_CLResControlModeType* CLResControlMode);
void init_iso20_SignatureType(struct iso20_SignatureType* Signature);
void init_iso20_SignatureValueType(struct iso20_SignatureValueType* SignatureValue);
void init_iso20_SignedInfoType(struct iso20_SignedInfoType* SignedInfo);
void init_iso20_CanonicalizationMethodType(struct iso20_CanonicalizationMethodType* CanonicalizationMethod);
void init_iso20_SignatureMethodType(struct iso20_SignatureMethodType* SignatureMethod);
void init_iso20_ReferenceType(struct iso20_ReferenceType* Reference);
void init_iso20_TransformsType(struct iso20_TransformsType* Transforms);
void init_iso20_TransformType(struct iso20_TransformType* Transform);
void init_iso20_DigestMethodType(struct iso20_DigestMethodType* DigestMethod);
void init_iso20_KeyInfoType(struct iso20_KeyInfoType* KeyInfo);
void init_iso20_KeyValueType(struct iso20_KeyValueType* KeyValue);
void init_iso20_RetrievalMethodType(struct iso20_RetrievalMethodType* RetrievalMethod);
void init_iso20_X509DataType(struct iso20_X509DataType* X509Data);
void init_iso20_PGPDataType(struct iso20_PGPDataType* PGPData);
void init_iso20_SPKIDataType(struct iso20_SPKIDataType* SPKIData);
void init_iso20_ObjectType(struct iso20_ObjectType* Object);
void init_iso20_ManifestType(struct iso20_ManifestType* Manifest);
void init_iso20_SignaturePropertiesType(struct iso20_SignaturePropertiesType* SignatureProperties);
void init_iso20_SignaturePropertyType(struct iso20_SignaturePropertyType* SignatureProperty);
void init_iso20_DSAKeyValueType(struct iso20_DSAKeyValueType* DSAKeyValue);
void init_iso20_RSAKeyValueType(struct iso20_RSAKeyValueType* RSAKeyValue);
void init_iso20_PowerScheduleEntryType(struct iso20_PowerScheduleEntryType* PowerScheduleEntryType);
void init_iso20_EVPriceRuleType(struct iso20_EVPriceRuleType* EVPriceRuleType);
void init_iso20_X509IssuerSerialType(struct iso20_X509IssuerSerialType* X509IssuerSerialType);
void init_iso20_EVPowerScheduleEntryType(struct iso20_EVPowerScheduleEntryType* EVPowerScheduleEntryType);
void init_iso20_EVPriceRuleStackType(struct iso20_EVPriceRuleStackType* EVPriceRuleStackType);
void init_iso20_PriceRuleType(struct iso20_PriceRuleType* PriceRuleType);
void init_iso20_PowerScheduleEntryListType(struct iso20_PowerScheduleEntryListType* PowerScheduleEntryListType);
void init_iso20_TaxRuleType(struct iso20_TaxRuleType* TaxRuleType);
void init_iso20_PriceRuleStackType(struct iso20_PriceRuleStackType* PriceRuleStackType);
void init_iso20_AdditionalServiceType(struct iso20_AdditionalServiceType* AdditionalServiceType);
void init_iso20_PriceLevelScheduleEntryType(struct iso20_PriceLevelScheduleEntryType* PriceLevelScheduleEntryType);
void init_iso20_PowerScheduleType(struct iso20_PowerScheduleType* PowerScheduleType);
void init_iso20_EVPowerScheduleEntryListType(struct iso20_EVPowerScheduleEntryListType* EVPowerScheduleEntryListType);
void init_iso20_OverstayRuleType(struct iso20_OverstayRuleType* OverstayRuleType);
void init_iso20_EVPriceRuleStackListType(struct iso20_EVPriceRuleStackListType* EVPriceRuleStackListType);
void init_iso20_RationalNumberType(struct iso20_RationalNumberType* RationalNumberType);
void init_iso20_EVPowerScheduleType(struct iso20_EVPowerScheduleType* EVPowerScheduleType);
void init_iso20_SubCertificatesType(struct iso20_SubCertificatesType* SubCertificatesType);
void init_iso20_ParameterType(struct iso20_ParameterType* ParameterType);
void init_iso20_EVAbsolutePriceScheduleType(struct iso20_EVAbsolutePriceScheduleType* EVAbsolutePriceScheduleType);
void init_iso20_ChargingScheduleType(struct iso20_ChargingScheduleType* ChargingScheduleType);
void init_iso20_DetailedCostType(struct iso20_DetailedCostType* DetailedCostType);
void init_iso20_PriceLevelScheduleEntryListType(struct iso20_PriceLevelScheduleEntryListType* PriceLevelScheduleEntryListType);
void init_iso20_DetailedTaxType(struct iso20_DetailedTaxType* DetailedTaxType);
void init_iso20_TaxRuleListType(struct iso20_TaxRuleListType* TaxRuleListType);
void init_iso20_PriceRuleStackListType(struct iso20_PriceRuleStackListType* PriceRuleStackListType);
void init_iso20_OverstayRuleListType(struct iso20_OverstayRuleListType* OverstayRuleListType);
void init_iso20_AdditionalServiceListType(struct iso20_AdditionalServiceListType* AdditionalServiceListType);
void init_iso20_ServiceType(struct iso20_ServiceType* ServiceType);
void init_iso20_ParameterSetType(struct iso20_ParameterSetType* ParameterSetType);
void init_iso20_ScheduleTupleType(struct iso20_ScheduleTupleType* ScheduleTupleType);
void init_iso20_SupportedProvidersListType(struct iso20_SupportedProvidersListType* SupportedProvidersListType);
void init_iso20_ContractCertificateChainType(struct iso20_ContractCertificateChainType* ContractCertificateChainType);
void init_iso20_Dynamic_EVPPTControlModeType(struct iso20_Dynamic_EVPPTControlModeType* Dynamic_EVPPTControlModeType);
void init_iso20_MeterInfoType(struct iso20_MeterInfoType* MeterInfoType);
void init_iso20_Scheduled_EVPPTControlModeType(struct iso20_Scheduled_EVPPTControlModeType* Scheduled_EVPPTControlModeType);
void init_iso20_ReceiptType(struct iso20_ReceiptType* ReceiptType);
void init_iso20_AbsolutePriceScheduleType(struct iso20_AbsolutePriceScheduleType* AbsolutePriceScheduleType);
void init_iso20_EVPowerProfileEntryListType(struct iso20_EVPowerProfileEntryListType* EVPowerProfileEntryListType);
void init_iso20_Dynamic_SMDTControlModeType(struct iso20_Dynamic_SMDTControlModeType* Dynamic_SMDTControlModeType);
void init_iso20_EVEnergyOfferType(struct iso20_EVEnergyOfferType* EVEnergyOfferType);
void init_iso20_PriceLevelScheduleType(struct iso20_PriceLevelScheduleType* PriceLevelScheduleType);
void init_iso20_Scheduled_SMDTControlModeType(struct iso20_Scheduled_SMDTControlModeType* Scheduled_SMDTControlModeType);
void init_iso20_MessageHeaderType(struct iso20_MessageHeaderType* MessageHeaderType);
void init_iso20_ServiceIDListType(struct iso20_ServiceIDListType* ServiceIDListType);
void init_iso20_SelectedServiceType(struct iso20_SelectedServiceType* SelectedServiceType);
void init_iso20_SignedCertificateChainType(struct iso20_SignedCertificateChainType* SignedCertificateChainType);
void init_iso20_EIM_AReqAuthorizationModeType(struct iso20_EIM_AReqAuthorizationModeType* EIM_AReqAuthorizationModeType);
void init_iso20_SelectedServiceListType(struct iso20_SelectedServiceListType* SelectedServiceListType);
void init_iso20_Dynamic_SEReqControlModeType(struct iso20_Dynamic_SEReqControlModeType* Dynamic_SEReqControlModeType);
void init_iso20_EVSEStatusType(struct iso20_EVSEStatusType* EVSEStatusType);
void init_iso20_ListOfRootCertificateIDsType(struct iso20_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
void init_iso20_PnC_AReqAuthorizationModeType(struct iso20_PnC_AReqAuthorizationModeType* PnC_AReqAuthorizationModeType);
void init_iso20_ServiceListType(struct iso20_ServiceListType* ServiceListType);
void init_iso20_ServiceParameterListType(struct iso20_ServiceParameterListType* ServiceParameterListType);
void init_iso20_Scheduled_SEReqControlModeType(struct iso20_Scheduled_SEReqControlModeType* Scheduled_SEReqControlModeType);
void init_iso20_EVPowerProfileType(struct iso20_EVPowerProfileType* EVPowerProfileType);
void init_iso20_CertificateChainType(struct iso20_CertificateChainType* CertificateChainType);
void init_iso20_EIM_ASResAuthorizationModeType(struct iso20_EIM_ASResAuthorizationModeType* EIM_ASResAuthorizationModeType);
void init_iso20_Dynamic_SEResControlModeType(struct iso20_Dynamic_SEResControlModeType* Dynamic_SEResControlModeType);
void init_iso20_EMAIDListType(struct iso20_EMAIDListType* EMAIDListType);
void init_iso20_PnC_ASResAuthorizationModeType(struct iso20_PnC_ASResAuthorizationModeType* PnC_ASResAuthorizationModeType);
void init_iso20_Scheduled_SEResControlModeType(struct iso20_Scheduled_SEResControlModeType* Scheduled_SEResControlModeType);
void init_iso20_exiFragment(struct iso20_exiFragment* exiFrag);
void init_iso20_xmldsigFragment(struct iso20_xmldsigFragment* xmldsigFrag);


#ifdef __cplusplus
}
#endif

#endif /* ISO20_COMMON_MESSAGES_DATATYPES_H */

