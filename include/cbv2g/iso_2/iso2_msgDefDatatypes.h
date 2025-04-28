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
  * @file iso2_msgDefDatatypes.h
  * @brief Description goes here
  *
  **/

#ifndef ISO2_MSG_DEF_DATATYPES_H
#define ISO2_MSG_DEF_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define iso2_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_anyType_BYTES_SIZE (4)
#define iso2_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_CostType_3_ARRAY_SIZE (3)
#define iso2_ConsumptionCostType_3_ARRAY_SIZE (3)
#define iso2_Name_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_stringValue_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_PMaxScheduleEntryType_12_ARRAY_SIZE (12)
#define iso2_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_SalesTariffDescription_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso2_SalesTariffEntryType_12_ARRAY_SIZE (12)
#define iso2_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_ReferenceType_4_ARRAY_SIZE (4)
#define iso2_ParameterType_16_ARRAY_SIZE (16)
#define iso2_ServiceName_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso2_ServiceScope_CHARACTER_SIZE (64 + ASCII_EXTRA_CHAR)
#define iso2_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_certificateType_4_ARRAY_SIZE (4)
#define iso2_certificateType_BYTES_SIZE (800)
#define iso2_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_EnergyTransferModeType_6_ARRAY_SIZE (6)
#define iso2_FaultMsg_CHARACTER_SIZE (64 + ASCII_EXTRA_CHAR)
#define iso2_SelectedServiceType_16_ARRAY_SIZE (16)
#define iso2_paymentOptionType_2_ARRAY_SIZE (2)
#define iso2_ProfileEntryType_24_ARRAY_SIZE (24)
#define iso2_ParameterSetType_5_ARRAY_SIZE (5)
#define iso2_X509IssuerSerialType_5_ARRAY_SIZE (5)
#define iso2_SAScheduleTupleType_3_ARRAY_SIZE (3)
#define iso2_ContractSignatureEncryptedPrivateKeyType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_ServiceType_8_ARRAY_SIZE (8)
#define iso2_DiffieHellmanPublickeyType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define iso2_CONTENT_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define iso2_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define iso2_sigMeterReadingType_BYTES_SIZE (64)
#define iso2_sessionIDType_BYTES_SIZE (8)
#define iso2_EVSEID_CHARACTER_SIZE (37 + ASCII_EXTRA_CHAR)
#define iso2_genChallengeType_BYTES_SIZE (16)
#define iso2_eMAID_CHARACTER_SIZE (15 + ASCII_EXTRA_CHAR)
#define iso2_evccIDType_BYTES_SIZE (6)


// enum for function numbers
typedef enum {
    iso2_AC_EVChargeParameter = 0,
    iso2_AC_EVSEChargeParameter = 1,
    iso2_AC_EVSEStatus = 2,
    iso2_AuthorizationReq = 3,
    iso2_AuthorizationRes = 4,
    iso2_BodyElement = 5,
    iso2_CableCheckReq = 6,
    iso2_CableCheckRes = 7,
    iso2_CanonicalizationMethod = 8,
    iso2_CertificateInstallationReq = 9,
    iso2_CertificateInstallationRes = 10,
    iso2_CertificateUpdateReq = 11,
    iso2_CertificateUpdateRes = 12,
    iso2_ChargeParameterDiscoveryReq = 13,
    iso2_ChargeParameterDiscoveryRes = 14,
    iso2_ChargingStatusReq = 15,
    iso2_ChargingStatusRes = 16,
    iso2_CurrentDemandReq = 17,
    iso2_CurrentDemandRes = 18,
    iso2_DC_EVChargeParameter = 19,
    iso2_DC_EVPowerDeliveryParameter = 20,
    iso2_DC_EVSEChargeParameter = 21,
    iso2_DC_EVSEStatus = 22,
    iso2_DC_EVStatus = 23,
    iso2_DSAKeyValue = 24,
    iso2_DigestMethod = 25,
    iso2_DigestValue = 26,
    iso2_EVChargeParameter = 27,
    iso2_EVPowerDeliveryParameter = 28,
    iso2_EVSEChargeParameter = 29,
    iso2_EVSEStatus = 30,
    iso2_EVStatus = 31,
    iso2_Entry = 32,
    iso2_KeyInfo = 33,
    iso2_KeyName = 34,
    iso2_KeyValue = 35,
    iso2_Manifest = 36,
    iso2_MeteringReceiptReq = 37,
    iso2_MeteringReceiptRes = 38,
    iso2_MgmtData = 39,
    iso2_Object = 40,
    iso2_PGPData = 41,
    iso2_PMaxScheduleEntry = 42,
    iso2_PaymentDetailsReq = 43,
    iso2_PaymentDetailsRes = 44,
    iso2_PaymentServiceSelectionReq = 45,
    iso2_PaymentServiceSelectionRes = 46,
    iso2_PowerDeliveryReq = 47,
    iso2_PowerDeliveryRes = 48,
    iso2_PreChargeReq = 49,
    iso2_PreChargeRes = 50,
    iso2_RSAKeyValue = 51,
    iso2_Reference = 52,
    iso2_RelativeTimeInterval = 53,
    iso2_RetrievalMethod = 54,
    iso2_SAScheduleList = 55,
    iso2_SASchedules = 56,
    iso2_SPKIData = 57,
    iso2_SalesTariffEntry = 58,
    iso2_ServiceDetailReq = 59,
    iso2_ServiceDetailRes = 60,
    iso2_ServiceDiscoveryReq = 61,
    iso2_ServiceDiscoveryRes = 62,
    iso2_SessionSetupReq = 63,
    iso2_SessionSetupRes = 64,
    iso2_SessionStopReq = 65,
    iso2_SessionStopRes = 66,
    iso2_Signature = 67,
    iso2_SignatureMethod = 68,
    iso2_SignatureProperties = 69,
    iso2_SignatureProperty = 70,
    iso2_SignatureValue = 71,
    iso2_SignedInfo = 72,
    iso2_TimeInterval = 73,
    iso2_Transform = 74,
    iso2_Transforms = 75,
    iso2_V2G_Message = 76,
    iso2_WeldingDetectionReq = 77,
    iso2_WeldingDetectionRes = 78,
    iso2_X509Data = 79
} iso2_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}costKind; type={urn:iso:15118:2:2013:MsgDataTypes}costKindType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_costKindType_relativePricePercentage = 0,
    iso2_costKindType_RenewableGenerationPercentage = 1,
    iso2_costKindType_CarbonDioxideEmission = 2
} iso2_costKindType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}Unit; type={urn:iso:15118:2:2013:MsgDataTypes}unitSymbolType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_unitSymbolType_h = 0,
    iso2_unitSymbolType_m = 1,
    iso2_unitSymbolType_s = 2,
    iso2_unitSymbolType_A = 3,
    iso2_unitSymbolType_V = 4,
    iso2_unitSymbolType_W = 5,
    iso2_unitSymbolType_Wh = 6
} iso2_unitSymbolType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}EVErrorCode; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVErrorCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_DC_EVErrorCodeType_NO_ERROR = 0,
    iso2_DC_EVErrorCodeType_FAILED_RESSTemperatureInhibit = 1,
    iso2_DC_EVErrorCodeType_FAILED_EVShiftPosition = 2,
    iso2_DC_EVErrorCodeType_FAILED_ChargerConnectorLockFault = 3,
    iso2_DC_EVErrorCodeType_FAILED_EVRESSMalfunction = 4,
    iso2_DC_EVErrorCodeType_FAILED_ChargingCurrentdifferential = 5,
    iso2_DC_EVErrorCodeType_FAILED_ChargingVoltageOutOfRange = 6,
    iso2_DC_EVErrorCodeType_Reserved_A = 7,
    iso2_DC_EVErrorCodeType_Reserved_B = 8,
    iso2_DC_EVErrorCodeType_Reserved_C = 9,
    iso2_DC_EVErrorCodeType_FAILED_ChargingSystemIncompatibility = 10,
    iso2_DC_EVErrorCodeType_NoData = 11
} iso2_DC_EVErrorCodeType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}FaultCode; type={urn:iso:15118:2:2013:MsgDataTypes}faultCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_faultCodeType_ParsingError = 0,
    iso2_faultCodeType_NoTLSRootCertificatAvailable = 1,
    iso2_faultCodeType_UnknownError = 2
} iso2_faultCodeType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}EVSENotification; type={urn:iso:15118:2:2013:MsgDataTypes}EVSENotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_EVSENotificationType_None = 0,
    iso2_EVSENotificationType_StopCharging = 1,
    iso2_EVSENotificationType_ReNegotiation = 2
} iso2_EVSENotificationType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEIsolationStatus; type={urn:iso:15118:2:2013:MsgDataTypes}isolationLevelType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_isolationLevelType_Invalid = 0,
    iso2_isolationLevelType_Valid = 1,
    iso2_isolationLevelType_Warning = 2,
    iso2_isolationLevelType_Fault = 3,
    iso2_isolationLevelType_No_IMD = 4
} iso2_isolationLevelType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}ServiceCategory; type={urn:iso:15118:2:2013:MsgDataTypes}serviceCategoryType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_serviceCategoryType_EVCharging = 0,
    iso2_serviceCategoryType_Internet = 1,
    iso2_serviceCategoryType_ContractCertificate = 2,
    iso2_serviceCategoryType_OtherCustom = 3
} iso2_serviceCategoryType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatusCode; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEStatusCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_DC_EVSEStatusCodeType_EVSE_NotReady = 0,
    iso2_DC_EVSEStatusCodeType_EVSE_Ready = 1,
    iso2_DC_EVSEStatusCodeType_EVSE_Shutdown = 2,
    iso2_DC_EVSEStatusCodeType_EVSE_UtilityInterruptEvent = 3,
    iso2_DC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive = 4,
    iso2_DC_EVSEStatusCodeType_EVSE_EmergencyShutdown = 5,
    iso2_DC_EVSEStatusCodeType_EVSE_Malfunction = 6,
    iso2_DC_EVSEStatusCodeType_Reserved_8 = 7,
    iso2_DC_EVSEStatusCodeType_Reserved_9 = 8,
    iso2_DC_EVSEStatusCodeType_Reserved_A = 9,
    iso2_DC_EVSEStatusCodeType_Reserved_B = 10,
    iso2_DC_EVSEStatusCodeType_Reserved_C = 11
} iso2_DC_EVSEStatusCodeType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}ChargeProgress; type={urn:iso:15118:2:2013:MsgDataTypes}chargeProgressType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_chargeProgressType_Start = 0,
    iso2_chargeProgressType_Stop = 1,
    iso2_chargeProgressType_Renegotiate = 2
} iso2_chargeProgressType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}ResponseCode; type={urn:iso:15118:2:2013:MsgDataTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_responseCodeType_OK = 0,
    iso2_responseCodeType_OK_NewSessionEstablished = 1,
    iso2_responseCodeType_OK_OldSessionJoined = 2,
    iso2_responseCodeType_OK_CertificateExpiresSoon = 3,
    iso2_responseCodeType_FAILED = 4,
    iso2_responseCodeType_FAILED_SequenceError = 5,
    iso2_responseCodeType_FAILED_ServiceIDInvalid = 6,
    iso2_responseCodeType_FAILED_UnknownSession = 7,
    iso2_responseCodeType_FAILED_ServiceSelectionInvalid = 8,
    iso2_responseCodeType_FAILED_PaymentSelectionInvalid = 9,
    iso2_responseCodeType_FAILED_CertificateExpired = 10,
    iso2_responseCodeType_FAILED_SignatureError = 11,
    iso2_responseCodeType_FAILED_NoCertificateAvailable = 12,
    iso2_responseCodeType_FAILED_CertChainError = 13,
    iso2_responseCodeType_FAILED_ChallengeInvalid = 14,
    iso2_responseCodeType_FAILED_ContractCanceled = 15,
    iso2_responseCodeType_FAILED_WrongChargeParameter = 16,
    iso2_responseCodeType_FAILED_PowerDeliveryNotApplied = 17,
    iso2_responseCodeType_FAILED_TariffSelectionInvalid = 18,
    iso2_responseCodeType_FAILED_ChargingProfileInvalid = 19,
    iso2_responseCodeType_FAILED_MeteringSignatureNotValid = 20,
    iso2_responseCodeType_FAILED_NoChargeServiceSelected = 21,
    iso2_responseCodeType_FAILED_WrongEnergyTransferMode = 22,
    iso2_responseCodeType_FAILED_ContactorError = 23,
    iso2_responseCodeType_FAILED_CertificateNotAllowedAtThisEVSE = 24,
    iso2_responseCodeType_FAILED_CertificateRevoked = 25
} iso2_responseCodeType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}SelectedPaymentOption; type={urn:iso:15118:2:2013:MsgDataTypes}paymentOptionType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_paymentOptionType_Contract = 0,
    iso2_paymentOptionType_ExternalPayment = 1
} iso2_paymentOptionType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}ChargingSession; type={urn:iso:15118:2:2013:MsgDataTypes}chargingSessionType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_chargingSessionType_Terminate = 0,
    iso2_chargingSessionType_Pause = 1
} iso2_chargingSessionType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}RequestedEnergyTransferMode; type={urn:iso:15118:2:2013:MsgDataTypes}EnergyTransferModeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_EnergyTransferModeType_AC_single_phase_core = 0,
    iso2_EnergyTransferModeType_AC_three_phase_core = 1,
    iso2_EnergyTransferModeType_DC_core = 2,
    iso2_EnergyTransferModeType_DC_extended = 3,
    iso2_EnergyTransferModeType_DC_combo_core = 4,
    iso2_EnergyTransferModeType_DC_unique = 5
} iso2_EnergyTransferModeType;

// Element: definition=enum; name={urn:iso:15118:2:2013:MsgBody}EVSEProcessing; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEProcessingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    iso2_EVSEProcessingType_Finished = 0,
    iso2_EVSEProcessingType_Ongoing = 1,
    iso2_EVSEProcessingType_Ongoing_WaitingForCustomerInteraction = 2
} iso2_EVSEProcessingType;

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Cost; type={urn:iso:15118:2:2013:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
struct iso2_CostType {
    // costKind, costKindType (base: string)
    iso2_costKindType costKind;
    // amount, unsignedInt (base: unsignedLong)
    uint32_t amount;
    // amountMultiplier, unitMultiplierType (base: byte)
    int8_t amountMultiplier;
    unsigned int amountMultiplier_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct iso2_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso2_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[iso2_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}TimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}IntervalType; base type=; content type=empty;
//          abstract=True; final=False;
// Particle: 
struct iso2_IntervalType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct iso2_TransformsType {
    // Transform, TransformType
    struct iso2_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct iso2_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct iso2_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[iso2_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeIntervalType; base type=IntervalType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: start, AnonType (1, 1); duration, AnonType (0, 1);
struct iso2_RelativeTimeIntervalType {
    // start, AnonType (base: unsignedInt)
    uint32_t start;
    // duration, AnonType (base: unsignedInt)
    uint32_t duration;
    unsigned int duration_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso2_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso2_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct iso2_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[iso2_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct iso2_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso2_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct iso2_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[iso2_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct iso2_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct iso2_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct iso2_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileEntryMaxPower; type={urn:iso:15118:2:2013:MsgDataTypes}PhysicalValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Multiplier, unitMultiplierType (1, 1); Unit, unitSymbolType (1, 1); Value, short (1, 1);
struct iso2_PhysicalValueType {
    // Multiplier, unitMultiplierType (base: byte)
    int8_t Multiplier;
    // Unit, unitSymbolType (base: string)
    iso2_unitSymbolType Unit;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCost; type={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, PhysicalValueType (1, 1); Cost, CostType (1, 3);
struct iso2_ConsumptionCostType {
    // startValue, PhysicalValueType
    struct iso2_PhysicalValueType startValue;
    // Cost, CostType
    struct {
        struct iso2_CostType array[iso2_CostType_3_ARRAY_SIZE];
        uint16_t arrayLen;
    } Cost;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntry; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); PMax, PhysicalValueType (1, 1);
struct iso2_PMaxScheduleEntryType {
    // RelativeTimeInterval, RelativeTimeIntervalType (base: IntervalType)
    struct iso2_RelativeTimeIntervalType RelativeTimeInterval;
    unsigned int RelativeTimeInterval_isUsed:1;
    // TimeInterval, IntervalType
    struct iso2_IntervalType TimeInterval;
    unsigned int TimeInterval_isUsed:1;
    // PMax, PhysicalValueType
    struct iso2_PhysicalValueType PMax;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntry; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); EPriceLevel, unsignedByte (0, 1); ConsumptionCost, ConsumptionCostType (0, 3);
struct iso2_SalesTariffEntryType {
    // RelativeTimeInterval, RelativeTimeIntervalType (base: IntervalType)
    struct iso2_RelativeTimeIntervalType RelativeTimeInterval;
    unsigned int RelativeTimeInterval_isUsed:1;
    // TimeInterval, IntervalType
    struct iso2_IntervalType TimeInterval;
    unsigned int TimeInterval_isUsed:1;
    // EPriceLevel, unsignedByte (base: unsignedShort)
    uint8_t EPriceLevel;
    unsigned int EPriceLevel_isUsed:1;
    // ConsumptionCost, ConsumptionCostType
    struct {
        struct iso2_ConsumptionCostType array[iso2_ConsumptionCostType_3_ARRAY_SIZE];
        uint16_t arrayLen;
    } ConsumptionCost;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Parameter; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, string (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); physicalValue, PhysicalValueType (0, 1); stringValue, string (0, 1);
struct iso2_ParameterType {
    // Attribute: Name, string
    struct {
        char characters[iso2_Name_CHARACTER_SIZE];
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
    // physicalValue, PhysicalValueType
    struct iso2_PhysicalValueType physicalValue;
    unsigned int physicalValue_isUsed:1;
    // stringValue, string
    struct {
        char characters[iso2_stringValue_CHARACTER_SIZE];
        uint16_t charactersLen;
    } stringValue;
    unsigned int stringValue_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxSchedule; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PMaxScheduleEntry, PMaxScheduleEntryType (1, 12) (original max 1024);
struct iso2_PMaxScheduleType {
    // PMaxScheduleEntry, PMaxScheduleEntryType (base: EntryType)
    struct {
        struct iso2_PMaxScheduleEntryType array[iso2_PMaxScheduleEntryType_12_ARRAY_SIZE];
        uint16_t arrayLen;
    } PMaxScheduleEntry;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct iso2_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[iso2_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso2_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso2_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct iso2_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct iso2_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[iso2_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[iso2_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct iso2_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariff; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SalesTariffID, SAIDType (1, 1); SalesTariffDescription, tariffDescriptionType (0, 1); NumEPriceLevels, unsignedByte (0, 1); SalesTariffEntry, SalesTariffEntryType (1, 12) (original max 1024);
struct iso2_SalesTariffType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SalesTariffID, SAIDType (base: unsignedByte)
    uint8_t SalesTariffID;
    // SalesTariffDescription, tariffDescriptionType (base: string)
    struct {
        char characters[iso2_SalesTariffDescription_CHARACTER_SIZE];
        uint16_t charactersLen;
    } SalesTariffDescription;
    unsigned int SalesTariffDescription_isUsed:1;
    // NumEPriceLevels, unsignedByte (base: unsignedShort)
    uint8_t NumEPriceLevels;
    unsigned int NumEPriceLevels_isUsed:1;
    // SalesTariffEntry, SalesTariffEntryType (base: EntryType)
    struct {
        struct iso2_SalesTariffEntryType array[iso2_SalesTariffEntryType_12_ARRAY_SIZE];
        uint16_t arrayLen;
    } SalesTariffEntry;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct iso2_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct iso2_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[iso2_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct iso2_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso2_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[iso2_anyType_BYTES_SIZE];
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
struct iso2_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[iso2_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
struct iso2_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct iso2_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct iso2_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct {
        struct iso2_ReferenceType array[iso2_ReferenceType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Reference;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntry; type={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ChargingProfileEntryStart, unsignedInt (1, 1); ChargingProfileEntryMaxPower, PhysicalValueType (1, 1); ChargingProfileEntryMaxNumberOfPhasesInUse, maxNumPhasesType (0, 1);
struct iso2_ProfileEntryType {
    // ChargingProfileEntryStart, unsignedInt (base: unsignedLong)
    uint32_t ChargingProfileEntryStart;
    // ChargingProfileEntryMaxPower, PhysicalValueType
    struct iso2_PhysicalValueType ChargingProfileEntryMaxPower;
    // ChargingProfileEntryMaxNumberOfPhasesInUse, maxNumPhasesType (base: byte)
    int8_t ChargingProfileEntryMaxNumberOfPhasesInUse;
    unsigned int ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatusType; base type=EVStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVReady, boolean (1, 1); EVErrorCode, DC_EVErrorCodeType (1, 1); EVRESSSOC, percentValueType (1, 1);
struct iso2_DC_EVStatusType {
    // EVReady, boolean
    int EVReady;
    // EVErrorCode, DC_EVErrorCodeType (base: string)
    iso2_DC_EVErrorCodeType EVErrorCode;
    // EVRESSSOC, percentValueType (base: byte)
    int8_t EVRESSSOC;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ParameterSet; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, short (1, 1); Parameter, ParameterType (1, 16);
struct iso2_ParameterSetType {
    // ParameterSetID, short (base: int)
    int16_t ParameterSetID;
    // Parameter, ParameterType
    struct {
        struct iso2_ParameterType array[iso2_ParameterType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } Parameter;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTuple; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); PMaxSchedule, PMaxScheduleType (1, 1); SalesTariff, SalesTariffType (0, 1);
struct iso2_SAScheduleTupleType {
    // SAScheduleTupleID, SAIDType (base: unsignedByte)
    uint8_t SAScheduleTupleID;
    // PMaxSchedule, PMaxScheduleType
    struct iso2_PMaxScheduleType PMaxSchedule;
    // SalesTariff, SalesTariffType
    struct iso2_SalesTariffType SalesTariff;
    unsigned int SalesTariff_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SelectedService; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, short (0, 1);
struct iso2_SelectedServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ParameterSetID, short (base: int)
    int16_t ParameterSetID;
    unsigned int ParameterSetID_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Service; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1);
struct iso2_ServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceName, serviceNameType (base: string)
    struct {
        char characters[iso2_ServiceName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceName;
    unsigned int ServiceName_isUsed:1;
    // ServiceCategory, serviceCategoryType (base: string)
    iso2_serviceCategoryType ServiceCategory;
    // ServiceScope, serviceScopeType (base: string)
    struct {
        char characters[iso2_ServiceScope_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceScope;
    unsigned int ServiceScope_isUsed:1;
    // FreeService, boolean
    int FreeService;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct iso2_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SubCertificates; type={urn:iso:15118:2:2013:MsgDataTypes}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 4);
struct iso2_SubCertificatesType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        struct {
            uint8_t bytes[iso2_certificateType_BYTES_SIZE];
            uint16_t bytesLen;
        } array[iso2_certificateType_4_ARRAY_SIZE];
        uint16_t arrayLen;
    } Certificate;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct iso2_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[iso2_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct iso2_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct iso2_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct iso2_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct iso2_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct iso2_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[iso2_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct iso2_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[iso2_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[iso2_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferMode; type={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyTransferMode, EnergyTransferModeType (1, 6);
struct iso2_SupportedEnergyTransferModeType {
    // EnergyTransferMode, EnergyTransferModeType (base: string)
    struct {
        iso2_EnergyTransferModeType array[iso2_EnergyTransferModeType_6_ARRAY_SIZE];
        uint16_t arrayLen;
    } EnergyTransferMode;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureCertChain; type={urn:iso:15118:2:2013:MsgDataTypes}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
struct iso2_CertificateChainType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;

    // SubCertificates, SubCertificatesType
    struct iso2_SubCertificatesType SubCertificates;
    unsigned int SubCertificates_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}BodyElement; type={urn:iso:15118:2:2013:MsgBody}BodyBaseType; base type=; content type=empty;
//          abstract=True; final=False;
// Particle: 
struct iso2_BodyBaseType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgHeader}Notification; type={urn:iso:15118:2:2013:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
struct iso2_NotificationType {
    // FaultCode, faultCodeType (base: string)
    iso2_faultCodeType FaultCode;
    // FaultMsg, faultMsgType (base: string)
    struct {
        char characters[iso2_FaultMsg_CHARACTER_SIZE];
        uint16_t charactersLen;
    } FaultMsg;
    unsigned int FaultMsg_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); EVSEIsolationStatus, isolationLevelType (0, 1); EVSEStatusCode, DC_EVSEStatusCodeType (1, 1);
struct iso2_DC_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, EVSENotificationType (base: string)
    iso2_EVSENotificationType EVSENotification;
    // EVSEIsolationStatus, isolationLevelType (base: string)
    iso2_isolationLevelType EVSEIsolationStatus;
    unsigned int EVSEIsolationStatus_isUsed:1;
    // EVSEStatusCode, DC_EVSEStatusCodeType (base: string)
    iso2_DC_EVSEStatusCodeType EVSEStatusCode;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SelectedServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16);
struct iso2_SelectedServiceListType {
    // SelectedService, SelectedServiceType
    struct {
        struct iso2_SelectedServiceType array[iso2_SelectedServiceType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } SelectedService;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentOptionList; type={urn:iso:15118:2:2013:MsgDataTypes}PaymentOptionListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PaymentOption, paymentOptionType (1, 2);
struct iso2_PaymentOptionListType {
    // PaymentOption, paymentOptionType (base: string)
    struct {
        iso2_paymentOptionType array[iso2_paymentOptionType_2_ARRAY_SIZE];
        uint16_t arrayLen;
    } PaymentOption;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct iso2_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct iso2_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct iso2_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct iso2_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct iso2_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingProfile; type={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProfileEntry, ProfileEntryType (1, 24);
struct iso2_ChargingProfileType {
    // ProfileEntry, ProfileEntryType
    struct {
        struct iso2_ProfileEntryType array[iso2_ProfileEntryType_24_ARRAY_SIZE];
        uint16_t arrayLen;
    } ProfileEntry;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceParameterList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5) (original max 255);
struct iso2_ServiceParameterListType {
    // ParameterSet, ParameterSetType
    struct {
        struct iso2_ParameterSetType array[iso2_ParameterSetType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } ParameterSet;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ListOfRootCertificateIDs; type={urn:iso:15118:2:2013:MsgDataTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, X509IssuerSerialType (1, 5) (original max 20);
struct iso2_ListOfRootCertificateIDsType {
    // RootCertificateID, X509IssuerSerialType
    struct {
        struct iso2_X509IssuerSerialType array[iso2_X509IssuerSerialType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } RootCertificateID;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EAmount, PhysicalValueType (1, 1); EVMaxVoltage, PhysicalValueType (1, 1); EVMaxCurrent, PhysicalValueType (1, 1); EVMinCurrent, PhysicalValueType (1, 1);
struct iso2_AC_EVChargeParameterType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // EAmount, PhysicalValueType
    struct iso2_PhysicalValueType EAmount;
    // EVMaxVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVMaxVoltage;
    // EVMaxCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVMaxCurrent;
    // EVMinCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVMinCurrent;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); DC_EVStatus, DC_EVStatusType (1, 1); EVMaximumCurrentLimit, PhysicalValueType (1, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); EVMaximumVoltageLimit, PhysicalValueType (1, 1); EVEnergyCapacity, PhysicalValueType (0, 1); EVEnergyRequest, PhysicalValueType (0, 1); FullSOC, percentValueType (0, 1); BulkSOC, percentValueType (0, 1);
struct iso2_DC_EVChargeParameterType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;
    // EVMaximumCurrentLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumCurrentLimit;
    // EVMaximumPowerLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumPowerLimit;
    unsigned int EVMaximumPowerLimit_isUsed:1;
    // EVMaximumVoltageLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumVoltageLimit;
    // EVEnergyCapacity, PhysicalValueType
    struct iso2_PhysicalValueType EVEnergyCapacity;
    unsigned int EVEnergyCapacity_isUsed:1;
    // EVEnergyRequest, PhysicalValueType
    struct iso2_PhysicalValueType EVEnergyRequest;
    unsigned int EVEnergyRequest_isUsed:1;
    // FullSOC, percentValueType (base: byte)
    int8_t FullSOC;
    unsigned int FullSOC_isUsed:1;
    // BulkSOC, percentValueType (base: byte)
    int8_t BulkSOC;
    unsigned int BulkSOC_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); AC_EVChargeParameter, AC_EVChargeParameterType (1, 1); DC_EVChargeParameter, DC_EVChargeParameterType (1, 1);
struct iso2_EVChargeParameterType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    unsigned int DepartureTime_isUsed:1;
    // AC_EVChargeParameter, AC_EVChargeParameterType (base: EVChargeParameterType)
    struct iso2_AC_EVChargeParameterType AC_EVChargeParameter;
    // DC_EVChargeParameter, DC_EVChargeParameterType (base: EVChargeParameterType)
    struct iso2_DC_EVChargeParameterType DC_EVChargeParameter;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SASchedules; type={urn:iso:15118:2:2013:MsgDataTypes}SASchedulesType; base type=; content type=empty;
//          abstract=True; final=False;
// Particle: 
struct iso2_SASchedulesType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleList; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleListType; base type=SASchedulesType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SAScheduleTuple, SAScheduleTupleType (1, 3);
struct iso2_SAScheduleListType {
    // SAScheduleTuple, SAScheduleTupleType
    struct {
        struct iso2_SAScheduleTupleType array[iso2_SAScheduleTupleType_3_ARRAY_SIZE];
        uint16_t arrayLen;
    } SAScheduleTuple;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeService; type={urn:iso:15118:2:2013:MsgDataTypes}ChargeServiceType; base type=ServiceType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1); SupportedEnergyTransferMode, SupportedEnergyTransferModeType (1, 1);
struct iso2_ChargeServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceName, serviceNameType (base: string)
    struct {
        char characters[iso2_ServiceName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceName;
    unsigned int ServiceName_isUsed:1;
    // ServiceCategory, serviceCategoryType (base: string)
    iso2_serviceCategoryType ServiceCategory;
    // ServiceScope, serviceScopeType (base: string)
    struct {
        char characters[iso2_ServiceScope_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceScope;
    unsigned int ServiceScope_isUsed:1;
    // FreeService, boolean
    int FreeService;
    // SupportedEnergyTransferMode, SupportedEnergyTransferModeType
    struct iso2_SupportedEnergyTransferModeType SupportedEnergyTransferMode;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameterType; base type=; content type=empty;
//          abstract=True; final=False;
// Particle: 
struct iso2_EVPowerDeliveryParameterType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
struct iso2_DC_EVPowerDeliveryParameterType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;
    // BulkChargingComplete, boolean
    int BulkChargingComplete;
    unsigned int BulkChargingComplete_isUsed:1;
    // ChargingComplete, boolean
    int ChargingComplete;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureEncryptedPrivateKey; type={urn:iso:15118:2:2013:MsgDataTypes}ContractSignatureEncryptedPrivateKeyType; base type=privateKeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, ContractSignatureEncryptedPrivateKeyType (1, 1);
struct iso2_ContractSignatureEncryptedPrivateKeyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // CONTENT, ContractSignatureEncryptedPrivateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_ContractSignatureEncryptedPrivateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameterType; base type=; content type=empty;
//          abstract=True; final=False;
// Particle: 
struct iso2_EVSEChargeParameterType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEMaximumCurrentLimit, PhysicalValueType (1, 1); EVSEMaximumPowerLimit, PhysicalValueType (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (1, 1); EVSEMinimumCurrentLimit, PhysicalValueType (1, 1); EVSEMinimumVoltageLimit, PhysicalValueType (1, 1); EVSECurrentRegulationTolerance, PhysicalValueType (0, 1); EVSEPeakCurrentRipple, PhysicalValueType (1, 1); EVSEEnergyToBeDelivered, PhysicalValueType (0, 1);
struct iso2_DC_EVSEChargeParameterType {
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEMaximumCurrentLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumCurrentLimit;
    // EVSEMaximumPowerLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumPowerLimit;
    // EVSEMaximumVoltageLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumVoltageLimit;
    // EVSEMinimumCurrentLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMinimumCurrentLimit;
    // EVSEMinimumVoltageLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMinimumVoltageLimit;
    // EVSECurrentRegulationTolerance, PhysicalValueType
    struct iso2_PhysicalValueType EVSECurrentRegulationTolerance;
    unsigned int EVSECurrentRegulationTolerance_isUsed:1;
    // EVSEPeakCurrentRipple, PhysicalValueType
    struct iso2_PhysicalValueType EVSEPeakCurrentRipple;
    // EVSEEnergyToBeDelivered, PhysicalValueType
    struct iso2_PhysicalValueType EVSEEnergyToBeDelivered;
    unsigned int EVSEEnergyToBeDelivered_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 8);
struct iso2_ServiceListType {
    // Service, ServiceType
    struct {
        struct iso2_ServiceType array[iso2_ServiceType_8_ARRAY_SIZE];
        uint16_t arrayLen;
    } Service;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DHpublickey; type={urn:iso:15118:2:2013:MsgDataTypes}DiffieHellmanPublickeyType; base type=dHpublickeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, DiffieHellmanPublickeyType (1, 1);
struct iso2_DiffieHellmanPublickeyType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // CONTENT, DiffieHellmanPublickeyType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_DiffieHellmanPublickeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}eMAID; type={urn:iso:15118:2:2013:MsgDataTypes}EMAIDType; base type=eMAIDType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, EMAIDType (1, 1);
struct iso2_EMAIDType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // CONTENT, EMAIDType (base: string)
    struct {
        char characters[iso2_CONTENT_CHARACTER_SIZE];
        uint16_t charactersLen;
    } CONTENT;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); RCD, boolean (1, 1);
struct iso2_AC_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, EVSENotificationType (base: string)
    iso2_EVSENotificationType EVSENotification;
    // RCD, boolean
    int RCD;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1);
struct iso2_EVSEStatusType {
    // NotificationMaxDelay, unsignedShort (base: unsignedInt)
    uint16_t NotificationMaxDelay;
    // EVSENotification, EVSENotificationType (base: string)
    iso2_EVSENotificationType EVSENotification;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_AC_EVSEStatusType AC_EVSEStatus;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: AC_EVSEStatus, AC_EVSEStatusType (1, 1); EVSENominalVoltage, PhysicalValueType (1, 1); EVSEMaxCurrent, PhysicalValueType (1, 1);
struct iso2_AC_EVSEChargeParameterType {
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_AC_EVSEStatusType AC_EVSEStatus;
    // EVSENominalVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVSENominalVoltage;
    // EVSEMaxCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaxCurrent;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeterInfo; type={urn:iso:15118:2:2013:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, unsignedLong (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
struct iso2_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[iso2_MeterID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MeterID;
    // MeterReading, unsignedLong (base: nonNegativeInteger)
    uint64_t MeterReading;
    unsigned int MeterReading_isUsed:1;
    // SigMeterReading, sigMeterReadingType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_sigMeterReadingType_BYTES_SIZE];
        uint16_t bytesLen;
    } SigMeterReading;
    unsigned int SigMeterReading_isUsed:1;

    // MeterStatus, meterStatusType (base: short)
    int16_t MeterStatus;
    unsigned int MeterStatus_isUsed:1;
    // TMeter, long (base: integer)
    int64_t TMeter;
    unsigned int TMeter_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Header; type={urn:iso:15118:2:2013:MsgHeader}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); Notification, NotificationType (0, 1); Signature, SignatureType (0, 1);
struct iso2_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso2_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // Notification, NotificationType
    struct iso2_NotificationType Notification;
    unsigned int Notification_isUsed:1;
    // Signature, SignatureType
    struct iso2_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReq; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargeProgress, chargeProgressType (1, 1); SAScheduleTupleID, SAIDType (1, 1); ChargingProfile, ChargingProfileType (0, 1); DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (0, 1); EVPowerDeliveryParameter, EVPowerDeliveryParameterType (0, 1);
struct iso2_PowerDeliveryReqType {
    // ChargeProgress, chargeProgressType (base: string)
    iso2_chargeProgressType ChargeProgress;
    // SAScheduleTupleID, SAIDType (base: unsignedByte)
    uint8_t SAScheduleTupleID;
    // ChargingProfile, ChargingProfileType
    struct iso2_ChargingProfileType ChargingProfile;
    unsigned int ChargingProfile_isUsed:1;
    // DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (base: EVPowerDeliveryParameterType)
    struct iso2_DC_EVPowerDeliveryParameterType DC_EVPowerDeliveryParameter;
    unsigned int DC_EVPowerDeliveryParameter_isUsed:1;
    // EVPowerDeliveryParameter, EVPowerDeliveryParameterType
    struct iso2_EVPowerDeliveryParameterType EVPowerDeliveryParameter;
    unsigned int EVPowerDeliveryParameter_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandRes; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1); EVSEPresentCurrent, PhysicalValueType (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (0, 1); EVSEMaximumCurrentLimit, PhysicalValueType (0, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1);
struct iso2_CurrentDemandResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVSEPresentVoltage;
    // EVSEPresentCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVSEPresentCurrent;
    // EVSECurrentLimitAchieved, boolean
    int EVSECurrentLimitAchieved;
    // EVSEVoltageLimitAchieved, boolean
    int EVSEVoltageLimitAchieved;
    // EVSEPowerLimitAchieved, boolean
    int EVSEPowerLimitAchieved;
    // EVSEMaximumVoltageLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumVoltageLimit;
    unsigned int EVSEMaximumVoltageLimit_isUsed:1;
    // EVSEMaximumCurrentLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumCurrentLimit;
    unsigned int EVSEMaximumCurrentLimit_isUsed:1;
    // EVSEMaximumPowerLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaximumPowerLimit;
    unsigned int EVSEMaximumPowerLimit_isUsed:1;
    // EVSEID, evseIDType (base: string)
    struct {
        char characters[iso2_EVSEID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVSEID;
    // SAScheduleTupleID, SAIDType (base: unsignedByte)
    uint8_t SAScheduleTupleID;
    // MeterInfo, MeterInfoType
    struct iso2_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // ReceiptRequired, boolean
    int ReceiptRequired;
    unsigned int ReceiptRequired_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusRes; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
struct iso2_ChargingStatusResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // EVSEID, evseIDType (base: string)
    struct {
        char characters[iso2_EVSEID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVSEID;
    // SAScheduleTupleID, SAIDType (base: unsignedByte)
    uint8_t SAScheduleTupleID;
    // EVSEMaxCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVSEMaxCurrent;
    unsigned int EVSEMaxCurrent_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso2_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // ReceiptRequired, boolean
    int ReceiptRequired;
    unsigned int ReceiptRequired_isUsed:1;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_AC_EVSEStatusType AC_EVSEStatus;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationReq; type={urn:iso:15118:2:2013:MsgBody}AuthorizationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); GenChallenge, genChallengeType (0, 1);
struct iso2_AuthorizationReqType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // GenChallenge, genChallengeType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_genChallengeType_BYTES_SIZE];
        uint16_t bytesLen;
    } GenChallenge;
    unsigned int GenChallenge_isUsed:1;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeReq; type={urn:iso:15118:2:2013:MsgBody}PreChargeReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetVoltage, PhysicalValueType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1);
struct iso2_PreChargeReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;
    // EVTargetVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVTargetVoltage;
    // EVTargetCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVTargetCurrent;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (0, 1);
struct iso2_ServiceDetailResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceParameterList, ServiceParameterListType
    struct iso2_ServiceParameterListType ServiceParameterList;
    unsigned int ServiceParameterList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionRes; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
struct iso2_PaymentServiceSelectionResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReq; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); eMAID, eMAIDType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
struct iso2_CertificateUpdateReqType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // ContractSignatureCertChain, CertificateChainType
    struct iso2_CertificateChainType ContractSignatureCertChain;
    // eMAID, eMAIDType (base: string)
    struct {
        char characters[iso2_eMAID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } eMAID;
    // ListOfRootCertificateIDs, ListOfRootCertificateIDsType
    struct iso2_ListOfRootCertificateIDsType ListOfRootCertificateIDs;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupRes; type={urn:iso:15118:2:2013:MsgBody}SessionSetupResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); EVSETimeStamp, long (0, 1);
struct iso2_SessionSetupResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // EVSEID, evseIDType (base: string)
    struct {
        char characters[iso2_EVSEID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } EVSEID;
    // EVSETimeStamp, long (base: integer)
    int64_t EVSETimeStamp;
    unsigned int EVSETimeStamp_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReq; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); OEMProvisioningCert, certificateType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
struct iso2_CertificateInstallationReqType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // OEMProvisioningCert, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } OEMProvisioningCert;

    // ListOfRootCertificateIDs, ListOfRootCertificateIDsType
    struct iso2_ListOfRootCertificateIDsType ListOfRootCertificateIDs;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationRes; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1);
struct iso2_CertificateInstallationResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // SAProvisioningCertificateChain, CertificateChainType
    struct iso2_CertificateChainType SAProvisioningCertificateChain;
    // ContractSignatureCertChain, CertificateChainType
    struct iso2_CertificateChainType ContractSignatureCertChain;
    // ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (base: privateKeyType)
    struct iso2_ContractSignatureEncryptedPrivateKeyType ContractSignatureEncryptedPrivateKey;
    // DHpublickey, DiffieHellmanPublickeyType (base: dHpublickeyType)
    struct iso2_DiffieHellmanPublickeyType DHpublickey;
    // eMAID, EMAIDType (base: eMAIDType)
    struct iso2_EMAIDType eMAID;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionRes; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
struct iso2_WeldingDetectionResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandReq; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1); EVMaximumVoltageLimit, PhysicalValueType (0, 1); EVMaximumCurrentLimit, PhysicalValueType (0, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1); RemainingTimeToFullSoC, PhysicalValueType (0, 1); RemainingTimeToBulkSoC, PhysicalValueType (0, 1); EVTargetVoltage, PhysicalValueType (1, 1);
struct iso2_CurrentDemandReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;
    // EVTargetCurrent, PhysicalValueType
    struct iso2_PhysicalValueType EVTargetCurrent;
    // EVMaximumVoltageLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumVoltageLimit;
    unsigned int EVMaximumVoltageLimit_isUsed:1;
    // EVMaximumCurrentLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumCurrentLimit;
    unsigned int EVMaximumCurrentLimit_isUsed:1;
    // EVMaximumPowerLimit, PhysicalValueType
    struct iso2_PhysicalValueType EVMaximumPowerLimit;
    unsigned int EVMaximumPowerLimit_isUsed:1;
    // BulkChargingComplete, boolean
    int BulkChargingComplete;
    unsigned int BulkChargingComplete_isUsed:1;
    // ChargingComplete, boolean
    int ChargingComplete;
    // RemainingTimeToFullSoC, PhysicalValueType
    struct iso2_PhysicalValueType RemainingTimeToFullSoC;
    unsigned int RemainingTimeToFullSoC_isUsed:1;
    // RemainingTimeToBulkSoC, PhysicalValueType
    struct iso2_PhysicalValueType RemainingTimeToBulkSoC;
    unsigned int RemainingTimeToBulkSoC_isUsed:1;
    // EVTargetVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVTargetVoltage;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeRes; type={urn:iso:15118:2:2013:MsgBody}PreChargeResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
struct iso2_PreChargeResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct iso2_PhysicalValueType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateRes; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1); RetryCounter, short (0, 1);
struct iso2_CertificateUpdateResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // SAProvisioningCertificateChain, CertificateChainType
    struct iso2_CertificateChainType SAProvisioningCertificateChain;
    // ContractSignatureCertChain, CertificateChainType
    struct iso2_CertificateChainType ContractSignatureCertChain;
    // ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (base: privateKeyType)
    struct iso2_ContractSignatureEncryptedPrivateKeyType ContractSignatureEncryptedPrivateKey;
    // DHpublickey, DiffieHellmanPublickeyType (base: dHpublickeyType)
    struct iso2_DiffieHellmanPublickeyType DHpublickey;
    // eMAID, EMAIDType (base: eMAIDType)
    struct iso2_EMAIDType eMAID;
    // RetryCounter, short (base: int)
    int16_t RetryCounter;
    unsigned int RetryCounter_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReq; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); SessionID, sessionIDType (1, 1); SAScheduleTupleID, SAIDType (0, 1); MeterInfo, MeterInfoType (1, 1);
struct iso2_MeteringReceiptReqType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[iso2_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso2_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // SAScheduleTupleID, SAIDType (base: unsignedByte)
    uint8_t SAScheduleTupleID;
    unsigned int SAScheduleTupleID_isUsed:1;
    // MeterInfo, MeterInfoType
    struct iso2_MeterInfoType MeterInfo;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusReq; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusReqType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
// Particle: 
struct iso2_ChargingStatusReqType {
    int _unused;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopRes; type={urn:iso:15118:2:2013:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
struct iso2_SessionStopResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: MaxEntriesSAScheduleTuple, unsignedShort (0, 1); RequestedEnergyTransferMode, EnergyTransferModeType (1, 1); AC_EVChargeParameter, AC_EVChargeParameterType (0, 1); DC_EVChargeParameter, DC_EVChargeParameterType (0, 1); EVChargeParameter, EVChargeParameterType (0, 1);
struct iso2_ChargeParameterDiscoveryReqType {
    // MaxEntriesSAScheduleTuple, unsignedShort (base: unsignedInt)
    uint16_t MaxEntriesSAScheduleTuple;
    unsigned int MaxEntriesSAScheduleTuple_isUsed:1;
    // RequestedEnergyTransferMode, EnergyTransferModeType (base: string)
    iso2_EnergyTransferModeType RequestedEnergyTransferMode;
    // AC_EVChargeParameter, AC_EVChargeParameterType (base: EVChargeParameterType)
    struct iso2_AC_EVChargeParameterType AC_EVChargeParameter;
    unsigned int AC_EVChargeParameter_isUsed:1;
    // DC_EVChargeParameter, DC_EVChargeParameterType (base: EVChargeParameterType)
    struct iso2_DC_EVChargeParameterType DC_EVChargeParameter;
    unsigned int DC_EVChargeParameter_isUsed:1;
    // EVChargeParameter, EVChargeParameterType
    struct iso2_EVChargeParameterType EVChargeParameter;
    unsigned int EVChargeParameter_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckReq; type={urn:iso:15118:2:2013:MsgBody}CableCheckReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
struct iso2_CableCheckReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReq; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
struct iso2_WeldingDetectionReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct iso2_DC_EVStatusType DC_EVStatus;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryRes; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
struct iso2_PowerDeliveryResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_AC_EVSEStatusType AC_EVSEStatus;
    unsigned int AC_EVSEStatus_isUsed:1;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    unsigned int DC_EVSEStatus_isUsed:1;
    // EVSEStatus, EVSEStatusType
    struct iso2_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1); SAScheduleList, SAScheduleListType (0, 1); SASchedules, SASchedulesType (0, 1); AC_EVSEChargeParameter, AC_EVSEChargeParameterType (0, 1); DC_EVSEChargeParameter, DC_EVSEChargeParameterType (0, 1); EVSEChargeParameter, EVSEChargeParameterType (0, 1);
struct iso2_ChargeParameterDiscoveryResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // EVSEProcessing, EVSEProcessingType (base: string)
    iso2_EVSEProcessingType EVSEProcessing;
    // SAScheduleList, SAScheduleListType (base: SASchedulesType)
    struct iso2_SAScheduleListType SAScheduleList;
    unsigned int SAScheduleList_isUsed:1;
    // SASchedules, SASchedulesType
    struct iso2_SASchedulesType SASchedules;
    unsigned int SASchedules_isUsed:1;
    // AC_EVSEChargeParameter, AC_EVSEChargeParameterType (base: EVSEChargeParameterType)
    struct iso2_AC_EVSEChargeParameterType AC_EVSEChargeParameter;
    unsigned int AC_EVSEChargeParameter_isUsed:1;
    // DC_EVSEChargeParameter, DC_EVSEChargeParameterType (base: EVSEChargeParameterType)
    struct iso2_DC_EVSEChargeParameterType DC_EVSEChargeParameter;
    unsigned int DC_EVSEChargeParameter_isUsed:1;
    // EVSEChargeParameter, EVSEChargeParameterType
    struct iso2_EVSEChargeParameterType EVSEChargeParameter;
    unsigned int EVSEChargeParameter_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReq; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SelectedPaymentOption, paymentOptionType (1, 1); SelectedServiceList, SelectedServiceListType (1, 1);
struct iso2_PaymentServiceSelectionReqType {
    // SelectedPaymentOption, paymentOptionType (base: string)
    iso2_paymentOptionType SelectedPaymentOption;
    // SelectedServiceList, SelectedServiceListType
    struct iso2_SelectedServiceListType SelectedServiceList;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptRes; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
struct iso2_MeteringReceiptResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_AC_EVSEStatusType AC_EVSEStatus;
    unsigned int AC_EVSEStatus_isUsed:1;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    unsigned int DC_EVSEStatus_isUsed:1;
    // EVSEStatus, EVSEStatusType
    struct iso2_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckRes; type={urn:iso:15118:2:2013:MsgBody}CableCheckResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
struct iso2_CableCheckResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct iso2_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEProcessing, EVSEProcessingType (base: string)
    iso2_EVSEProcessingType EVSEProcessing;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); PaymentOptionList, PaymentOptionListType (1, 1); ChargeService, ChargeServiceType (1, 1); ServiceList, ServiceListType (0, 1);
struct iso2_ServiceDiscoveryResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // PaymentOptionList, PaymentOptionListType
    struct iso2_PaymentOptionListType PaymentOptionList;
    // ChargeService, ChargeServiceType (base: ServiceType)
    struct iso2_ChargeServiceType ChargeService;
    // ServiceList, ServiceListType
    struct iso2_ServiceListType ServiceList;
    unsigned int ServiceList_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1);
struct iso2_ServiceDetailReqType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupReq; type={urn:iso:15118:2:2013:MsgBody}SessionSetupReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVCCID, evccIDType (1, 1);
struct iso2_SessionSetupReqType {
    // EVCCID, evccIDType (base: hexBinary)
    struct {
        uint8_t bytes[iso2_evccIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } EVCCID;


};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopReq; type={urn:iso:15118:2:2013:MsgBody}SessionStopReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargingSession, chargingSessionType (1, 1);
struct iso2_SessionStopReqType {
    // ChargingSession, chargingSessionType (base: string)
    iso2_chargingSessionType ChargingSession;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceScope, serviceScopeType (0, 1); ServiceCategory, serviceCategoryType (0, 1);
struct iso2_ServiceDiscoveryReqType {
    // ServiceScope, serviceScopeType (base: string)
    struct {
        char characters[iso2_ServiceScope_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceScope;
    unsigned int ServiceScope_isUsed:1;
    // ServiceCategory, serviceCategoryType (base: string)
    iso2_serviceCategoryType ServiceCategory;
    unsigned int ServiceCategory_isUsed:1;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationRes; type={urn:iso:15118:2:2013:MsgBody}AuthorizationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
struct iso2_AuthorizationResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // EVSEProcessing, EVSEProcessingType (base: string)
    iso2_EVSEProcessingType EVSEProcessing;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReq; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: eMAID, eMAIDType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1);
struct iso2_PaymentDetailsReqType {
    // eMAID, eMAIDType (base: string)
    struct {
        char characters[iso2_eMAID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } eMAID;
    // ContractSignatureCertChain, CertificateChainType
    struct iso2_CertificateChainType ContractSignatureCertChain;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsRes; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); GenChallenge, genChallengeType (1, 1); EVSETimeStamp, long (1, 1);
struct iso2_PaymentDetailsResType {
    // ResponseCode, responseCodeType (base: string)
    iso2_responseCodeType ResponseCode;
    // GenChallenge, genChallengeType (base: base64Binary)
    struct {
        uint8_t bytes[iso2_genChallengeType_BYTES_SIZE];
        uint16_t bytesLen;
    } GenChallenge;

    // EVSETimeStamp, long (base: integer)
    int64_t EVSETimeStamp;

};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Body; type={urn:iso:15118:2:2013:MsgBody}BodyType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AuthorizationReq, AuthorizationReqType (0, 1); AuthorizationRes, AuthorizationResType (0, 1); BodyElement, BodyBaseType (0, 1); CableCheckReq, CableCheckReqType (0, 1); CableCheckRes, CableCheckResType (0, 1); CertificateInstallationReq, CertificateInstallationReqType (0, 1); CertificateInstallationRes, CertificateInstallationResType (0, 1); CertificateUpdateReq, CertificateUpdateReqType (0, 1); CertificateUpdateRes, CertificateUpdateResType (0, 1); ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReqType (0, 1); ChargeParameterDiscoveryRes, ChargeParameterDiscoveryResType (0, 1); ChargingStatusReq, ChargingStatusReqType (0, 1); ChargingStatusRes, ChargingStatusResType (0, 1); CurrentDemandReq, CurrentDemandReqType (0, 1); CurrentDemandRes, CurrentDemandResType (0, 1); MeteringReceiptReq, MeteringReceiptReqType (0, 1); MeteringReceiptRes, MeteringReceiptResType (0, 1); PaymentDetailsReq, PaymentDetailsReqType (0, 1); PaymentDetailsRes, PaymentDetailsResType (0, 1); PaymentServiceSelectionReq, PaymentServiceSelectionReqType (0, 1); PaymentServiceSelectionRes, PaymentServiceSelectionResType (0, 1); PowerDeliveryReq, PowerDeliveryReqType (0, 1); PowerDeliveryRes, PowerDeliveryResType (0, 1); PreChargeReq, PreChargeReqType (0, 1); PreChargeRes, PreChargeResType (0, 1); ServiceDetailReq, ServiceDetailReqType (0, 1); ServiceDetailRes, ServiceDetailResType (0, 1); ServiceDiscoveryReq, ServiceDiscoveryReqType (0, 1); ServiceDiscoveryRes, ServiceDiscoveryResType (0, 1); SessionSetupReq, SessionSetupReqType (0, 1); SessionSetupRes, SessionSetupResType (0, 1); SessionStopReq, SessionStopReqType (0, 1); SessionStopRes, SessionStopResType (0, 1); WeldingDetectionReq, WeldingDetectionReqType (0, 1); WeldingDetectionRes, WeldingDetectionResType (0, 1);
struct iso2_BodyType {
    union {
        struct iso2_AuthorizationReqType AuthorizationReq;
        struct iso2_AuthorizationResType AuthorizationRes;
        struct iso2_BodyBaseType BodyElement;
        struct iso2_CableCheckReqType CableCheckReq;
        struct iso2_CableCheckResType CableCheckRes;
        struct iso2_CertificateInstallationReqType CertificateInstallationReq;
        struct iso2_CertificateInstallationResType CertificateInstallationRes;
        struct iso2_CertificateUpdateReqType CertificateUpdateReq;
        struct iso2_CertificateUpdateResType CertificateUpdateRes;
        struct iso2_ChargeParameterDiscoveryReqType ChargeParameterDiscoveryReq;
        struct iso2_ChargeParameterDiscoveryResType ChargeParameterDiscoveryRes;
        struct iso2_ChargingStatusReqType ChargingStatusReq;
        struct iso2_ChargingStatusResType ChargingStatusRes;
        struct iso2_CurrentDemandReqType CurrentDemandReq;
        struct iso2_CurrentDemandResType CurrentDemandRes;
        struct iso2_MeteringReceiptReqType MeteringReceiptReq;
        struct iso2_MeteringReceiptResType MeteringReceiptRes;
        struct iso2_PaymentDetailsReqType PaymentDetailsReq;
        struct iso2_PaymentDetailsResType PaymentDetailsRes;
        struct iso2_PaymentServiceSelectionReqType PaymentServiceSelectionReq;
        struct iso2_PaymentServiceSelectionResType PaymentServiceSelectionRes;
        struct iso2_PowerDeliveryReqType PowerDeliveryReq;
        struct iso2_PowerDeliveryResType PowerDeliveryRes;
        struct iso2_PreChargeReqType PreChargeReq;
        struct iso2_PreChargeResType PreChargeRes;
        struct iso2_ServiceDetailReqType ServiceDetailReq;
        struct iso2_ServiceDetailResType ServiceDetailRes;
        struct iso2_ServiceDiscoveryReqType ServiceDiscoveryReq;
        struct iso2_ServiceDiscoveryResType ServiceDiscoveryRes;
        struct iso2_SessionSetupReqType SessionSetupReq;
        struct iso2_SessionSetupResType SessionSetupRes;
        struct iso2_SessionStopReqType SessionStopReq;
        struct iso2_SessionStopResType SessionStopRes;
        struct iso2_WeldingDetectionReqType WeldingDetectionReq;
        struct iso2_WeldingDetectionResType WeldingDetectionRes;
    };
    unsigned int AuthorizationReq_isUsed:1;
    unsigned int AuthorizationRes_isUsed:1;
    unsigned int BodyElement_isUsed:1;
    unsigned int CableCheckReq_isUsed:1;
    unsigned int CableCheckRes_isUsed:1;
    unsigned int CertificateInstallationReq_isUsed:1;
    unsigned int CertificateInstallationRes_isUsed:1;
    unsigned int CertificateUpdateReq_isUsed:1;
    unsigned int CertificateUpdateRes_isUsed:1;
    unsigned int ChargeParameterDiscoveryReq_isUsed:1;
    unsigned int ChargeParameterDiscoveryRes_isUsed:1;
    unsigned int ChargingStatusReq_isUsed:1;
    unsigned int ChargingStatusRes_isUsed:1;
    unsigned int CurrentDemandReq_isUsed:1;
    unsigned int CurrentDemandRes_isUsed:1;
    unsigned int MeteringReceiptReq_isUsed:1;
    unsigned int MeteringReceiptRes_isUsed:1;
    unsigned int PaymentDetailsReq_isUsed:1;
    unsigned int PaymentDetailsRes_isUsed:1;
    unsigned int PaymentServiceSelectionReq_isUsed:1;
    unsigned int PaymentServiceSelectionRes_isUsed:1;
    unsigned int PowerDeliveryReq_isUsed:1;
    unsigned int PowerDeliveryRes_isUsed:1;
    unsigned int PreChargeReq_isUsed:1;
    unsigned int PreChargeRes_isUsed:1;
    unsigned int ServiceDetailReq_isUsed:1;
    unsigned int ServiceDetailRes_isUsed:1;
    unsigned int ServiceDiscoveryReq_isUsed:1;
    unsigned int ServiceDiscoveryRes_isUsed:1;
    unsigned int SessionSetupReq_isUsed:1;
    unsigned int SessionSetupRes_isUsed:1;
    unsigned int SessionStopReq_isUsed:1;
    unsigned int SessionStopRes_isUsed:1;
    unsigned int WeldingDetectionReq_isUsed:1;
    unsigned int WeldingDetectionRes_isUsed:1;
};

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}V2G_Message; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Header, MessageHeaderType (1, 1); Body, BodyType (1, 1);
struct iso2_V2G_Message {
    // Header, MessageHeaderType
    struct iso2_MessageHeaderType Header;
    // Body, BodyType
    struct iso2_BodyType Body;

};



// root elements of EXI doc
struct iso2_exiDocument {
    struct iso2_V2G_Message V2G_Message;
};

// elements of EXI fragment
struct iso2_exiFragment {
    union {
        struct iso2_AuthorizationReqType AuthorizationReq;
        struct iso2_CertificateInstallationReqType CertificateInstallationReq;
        struct iso2_CertificateUpdateReqType CertificateUpdateReq;
        struct iso2_CertificateChainType ContractSignatureCertChain;
        struct iso2_ContractSignatureEncryptedPrivateKeyType ContractSignatureEncryptedPrivateKey;
        struct iso2_DiffieHellmanPublickeyType DHpublickey;
        struct iso2_MeteringReceiptReqType MeteringReceiptReq;
        struct iso2_SalesTariffType SalesTariff;
        struct iso2_SignedInfoType SignedInfo;
        struct iso2_EMAIDType eMAID;
    };
    unsigned int AuthorizationReq_isUsed:1;
    unsigned int CertificateInstallationReq_isUsed:1;
    unsigned int CertificateUpdateReq_isUsed:1;
    unsigned int ContractSignatureCertChain_isUsed:1;
    unsigned int ContractSignatureEncryptedPrivateKey_isUsed:1;
    unsigned int DHpublickey_isUsed:1;
    unsigned int MeteringReceiptReq_isUsed:1;
    unsigned int SalesTariff_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
    unsigned int eMAID_isUsed:1;
};

// elements of xmldsig fragment
struct iso2_xmldsigFragment {
    union {
        struct iso2_CanonicalizationMethodType CanonicalizationMethod;
        struct iso2_DSAKeyValueType DSAKeyValue;
        struct iso2_DigestMethodType DigestMethod;
        struct iso2_KeyInfoType KeyInfo;
        struct iso2_KeyValueType KeyValue;
        struct iso2_ObjectType Object;
        struct iso2_PGPDataType PGPData;
        struct iso2_RSAKeyValueType RSAKeyValue;
        struct iso2_ReferenceType Reference;
        struct iso2_RetrievalMethodType RetrievalMethod;
        struct iso2_SPKIDataType SPKIData;
        struct iso2_SignatureType Signature;
        struct iso2_SignatureMethodType SignatureMethod;
        struct iso2_SignatureValueType SignatureValue;
        struct iso2_SignedInfoType SignedInfo;
        struct iso2_TransformType Transform;
        struct iso2_TransformsType Transforms;
        struct iso2_X509DataType X509Data;
        struct iso2_X509IssuerSerialType X509IssuerSerial;
    };
    unsigned int CanonicalizationMethod_isUsed:1;
    unsigned int DSAKeyValue_isUsed:1;
    unsigned int DigestMethod_isUsed:1;
    unsigned int KeyInfo_isUsed:1;
    unsigned int KeyValue_isUsed:1;
    unsigned int Object_isUsed:1;
    unsigned int PGPData_isUsed:1;
    unsigned int RSAKeyValue_isUsed:1;
    unsigned int Reference_isUsed:1;
    unsigned int RetrievalMethod_isUsed:1;
    unsigned int SPKIData_isUsed:1;
    unsigned int Signature_isUsed:1;
    unsigned int SignatureMethod_isUsed:1;
    unsigned int SignatureValue_isUsed:1;
    unsigned int SignedInfo_isUsed:1;
    unsigned int Transform_isUsed:1;
    unsigned int Transforms_isUsed:1;
    unsigned int X509Data_isUsed:1;
    unsigned int X509IssuerSerial_isUsed:1;
};

// init for structs
void init_iso2_exiDocument(struct iso2_exiDocument* exiDoc);
void init_iso2_V2G_Message(struct iso2_V2G_Message* V2G_Message);
void init_iso2_CostType(struct iso2_CostType* CostType);
void init_iso2_TransformType(struct iso2_TransformType* TransformType);
void init_iso2_IntervalType(struct iso2_IntervalType* IntervalType);
void init_iso2_ConsumptionCostType(struct iso2_ConsumptionCostType* ConsumptionCostType);
void init_iso2_TransformsType(struct iso2_TransformsType* TransformsType);
void init_iso2_DSAKeyValueType(struct iso2_DSAKeyValueType* DSAKeyValueType);
void init_iso2_X509IssuerSerialType(struct iso2_X509IssuerSerialType* X509IssuerSerialType);
void init_iso2_RelativeTimeIntervalType(struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType);
void init_iso2_PMaxScheduleEntryType(struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType);
void init_iso2_DigestMethodType(struct iso2_DigestMethodType* DigestMethodType);
void init_iso2_RSAKeyValueType(struct iso2_RSAKeyValueType* RSAKeyValueType);
void init_iso2_SalesTariffEntryType(struct iso2_SalesTariffEntryType* SalesTariffEntryType);
void init_iso2_CanonicalizationMethodType(struct iso2_CanonicalizationMethodType* CanonicalizationMethodType);
void init_iso2_SignatureMethodType(struct iso2_SignatureMethodType* SignatureMethodType);
void init_iso2_KeyValueType(struct iso2_KeyValueType* KeyValueType);
void init_iso2_PhysicalValueType(struct iso2_PhysicalValueType* PhysicalValueType);
void init_iso2_ParameterType(struct iso2_ParameterType* ParameterType);
void init_iso2_PMaxScheduleType(struct iso2_PMaxScheduleType* PMaxScheduleType);
void init_iso2_ReferenceType(struct iso2_ReferenceType* ReferenceType);
void init_iso2_RetrievalMethodType(struct iso2_RetrievalMethodType* RetrievalMethodType);
void init_iso2_SalesTariffType(struct iso2_SalesTariffType* SalesTariffType);
void init_iso2_X509DataType(struct iso2_X509DataType* X509DataType);
void init_iso2_PGPDataType(struct iso2_PGPDataType* PGPDataType);
void init_iso2_SPKIDataType(struct iso2_SPKIDataType* SPKIDataType);
void init_iso2_SignedInfoType(struct iso2_SignedInfoType* SignedInfoType);
void init_iso2_ProfileEntryType(struct iso2_ProfileEntryType* ProfileEntryType);
void init_iso2_DC_EVStatusType(struct iso2_DC_EVStatusType* DC_EVStatusType);
void init_iso2_ParameterSetType(struct iso2_ParameterSetType* ParameterSetType);
void init_iso2_SAScheduleTupleType(struct iso2_SAScheduleTupleType* SAScheduleTupleType);
void init_iso2_SelectedServiceType(struct iso2_SelectedServiceType* SelectedServiceType);
void init_iso2_ServiceType(struct iso2_ServiceType* ServiceType);
void init_iso2_SignatureValueType(struct iso2_SignatureValueType* SignatureValueType);
void init_iso2_SubCertificatesType(struct iso2_SubCertificatesType* SubCertificatesType);
void init_iso2_KeyInfoType(struct iso2_KeyInfoType* KeyInfoType);
void init_iso2_ObjectType(struct iso2_ObjectType* ObjectType);
void init_iso2_SupportedEnergyTransferModeType(struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType);
void init_iso2_CertificateChainType(struct iso2_CertificateChainType* CertificateChainType);
void init_iso2_BodyBaseType(struct iso2_BodyBaseType* BodyBaseType);
void init_iso2_NotificationType(struct iso2_NotificationType* NotificationType);
void init_iso2_DC_EVSEStatusType(struct iso2_DC_EVSEStatusType* DC_EVSEStatusType);
void init_iso2_EVSEStatusType(struct iso2_EVSEStatusType* EVSEStatusType);
void init_iso2_SelectedServiceListType(struct iso2_SelectedServiceListType* SelectedServiceListType);
void init_iso2_PaymentOptionListType(struct iso2_PaymentOptionListType* PaymentOptionListType);
void init_iso2_SignatureType(struct iso2_SignatureType* SignatureType);
void init_iso2_ChargingProfileType(struct iso2_ChargingProfileType* ChargingProfileType);
void init_iso2_ServiceParameterListType(struct iso2_ServiceParameterListType* ServiceParameterListType);
void init_iso2_ListOfRootCertificateIDsType(struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
void init_iso2_EVChargeParameterType(struct iso2_EVChargeParameterType* EVChargeParameterType);
void init_iso2_AC_EVChargeParameterType(struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType);
void init_iso2_DC_EVChargeParameterType(struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType);
void init_iso2_SASchedulesType(struct iso2_SASchedulesType* SASchedulesType);
void init_iso2_SAScheduleListType(struct iso2_SAScheduleListType* SAScheduleListType);
void init_iso2_ChargeServiceType(struct iso2_ChargeServiceType* ChargeServiceType);
void init_iso2_EVPowerDeliveryParameterType(struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
void init_iso2_DC_EVPowerDeliveryParameterType(struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
void init_iso2_ContractSignatureEncryptedPrivateKeyType(struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType);
void init_iso2_EVSEChargeParameterType(struct iso2_EVSEChargeParameterType* EVSEChargeParameterType);
void init_iso2_AC_EVSEChargeParameterType(struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
void init_iso2_DC_EVSEChargeParameterType(struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
void init_iso2_ServiceListType(struct iso2_ServiceListType* ServiceListType);
void init_iso2_DiffieHellmanPublickeyType(struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType);
void init_iso2_EMAIDType(struct iso2_EMAIDType* EMAIDType);
void init_iso2_AC_EVSEStatusType(struct iso2_AC_EVSEStatusType* AC_EVSEStatusType);
void init_iso2_MeterInfoType(struct iso2_MeterInfoType* MeterInfoType);
void init_iso2_MessageHeaderType(struct iso2_MessageHeaderType* MessageHeaderType);
void init_iso2_PowerDeliveryReqType(struct iso2_PowerDeliveryReqType* PowerDeliveryReqType);
void init_iso2_CurrentDemandResType(struct iso2_CurrentDemandResType* CurrentDemandResType);
void init_iso2_ChargingStatusResType(struct iso2_ChargingStatusResType* ChargingStatusResType);
void init_iso2_AuthorizationReqType(struct iso2_AuthorizationReqType* AuthorizationReqType);
void init_iso2_PreChargeReqType(struct iso2_PreChargeReqType* PreChargeReqType);
void init_iso2_ServiceDetailResType(struct iso2_ServiceDetailResType* ServiceDetailResType);
void init_iso2_PaymentServiceSelectionResType(struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType);
void init_iso2_CertificateUpdateReqType(struct iso2_CertificateUpdateReqType* CertificateUpdateReqType);
void init_iso2_SessionSetupResType(struct iso2_SessionSetupResType* SessionSetupResType);
void init_iso2_CertificateInstallationReqType(struct iso2_CertificateInstallationReqType* CertificateInstallationReqType);
void init_iso2_CertificateInstallationResType(struct iso2_CertificateInstallationResType* CertificateInstallationResType);
void init_iso2_WeldingDetectionResType(struct iso2_WeldingDetectionResType* WeldingDetectionResType);
void init_iso2_CurrentDemandReqType(struct iso2_CurrentDemandReqType* CurrentDemandReqType);
void init_iso2_PreChargeResType(struct iso2_PreChargeResType* PreChargeResType);
void init_iso2_CertificateUpdateResType(struct iso2_CertificateUpdateResType* CertificateUpdateResType);
void init_iso2_MeteringReceiptReqType(struct iso2_MeteringReceiptReqType* MeteringReceiptReqType);
void init_iso2_ChargingStatusReqType(struct iso2_ChargingStatusReqType* ChargingStatusReqType);
void init_iso2_SessionStopResType(struct iso2_SessionStopResType* SessionStopResType);
void init_iso2_ChargeParameterDiscoveryReqType(struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
void init_iso2_CableCheckReqType(struct iso2_CableCheckReqType* CableCheckReqType);
void init_iso2_WeldingDetectionReqType(struct iso2_WeldingDetectionReqType* WeldingDetectionReqType);
void init_iso2_PowerDeliveryResType(struct iso2_PowerDeliveryResType* PowerDeliveryResType);
void init_iso2_ChargeParameterDiscoveryResType(struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
void init_iso2_PaymentServiceSelectionReqType(struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType);
void init_iso2_MeteringReceiptResType(struct iso2_MeteringReceiptResType* MeteringReceiptResType);
void init_iso2_CableCheckResType(struct iso2_CableCheckResType* CableCheckResType);
void init_iso2_ServiceDiscoveryResType(struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType);
void init_iso2_ServiceDetailReqType(struct iso2_ServiceDetailReqType* ServiceDetailReqType);
void init_iso2_SessionSetupReqType(struct iso2_SessionSetupReqType* SessionSetupReqType);
void init_iso2_SessionStopReqType(struct iso2_SessionStopReqType* SessionStopReqType);
void init_iso2_ServiceDiscoveryReqType(struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
void init_iso2_AuthorizationResType(struct iso2_AuthorizationResType* AuthorizationResType);
void init_iso2_PaymentDetailsReqType(struct iso2_PaymentDetailsReqType* PaymentDetailsReqType);
void init_iso2_PaymentDetailsResType(struct iso2_PaymentDetailsResType* PaymentDetailsResType);
void init_iso2_BodyType(struct iso2_BodyType* BodyType);
void init_iso2_exiFragment(struct iso2_exiFragment* exiFrag);
void init_iso2_xmldsigFragment(struct iso2_xmldsigFragment* xmldsigFrag);


#ifdef __cplusplus
}
#endif

#endif /* ISO2_MSG_DEF_DATATYPES_H */

