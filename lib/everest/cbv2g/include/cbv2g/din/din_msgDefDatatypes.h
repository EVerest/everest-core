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
  * @file din_msgDefDatatypes.h
  * @brief Description goes here
  *
  **/

#ifndef DIN_MSG_DEF_DATATYPES_H
#define DIN_MSG_DEF_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"



#define din_Algorithm_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_anyType_BYTES_SIZE (4)
#define din_XPath_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_CryptoBinary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define din_X509IssuerName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_PMaxScheduleEntryType_5_ARRAY_SIZE (5)
#define din_Id_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_SalesTariffDescription_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define din_SalesTariffEntryType_5_ARRAY_SIZE (5)
#define din_ServiceName_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define din_ServiceScope_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define din_certificateType_BYTES_SIZE (1200)
#define din_Type_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_URI_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_DigestValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define din_base64Binary_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define din_X509SubjectName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_SignatureValueType_BYTES_SIZE (EXI_BYTE_ARRAY_MAX_LEN)
#define din_Name_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_stringValue_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_rootCertificateIDType_5_ARRAY_SIZE (5)
#define din_RootCertificateID_CHARACTER_SIZE (40 + ASCII_EXTRA_CHAR)
#define din_paymentOptionType_2_ARRAY_SIZE (2)
#define din_SelectedServiceType_16_ARRAY_SIZE (16)
#define din_ProfileEntryType_24_ARRAY_SIZE (24)
#define din_KeyName_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_MgmtData_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_ParameterSetType_5_ARRAY_SIZE (5)
#define din_SAScheduleTupleType_5_ARRAY_SIZE (5)
#define din_Encoding_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_MimeType_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_MeterID_CHARACTER_SIZE (32 + ASCII_EXTRA_CHAR)
#define din_sigMeterReadingType_BYTES_SIZE (32)
#define din_privateKeyType_BYTES_SIZE (128)
#define din_dHParamsType_BYTES_SIZE (256)
#define din_ContractID_CHARACTER_SIZE (24 + ASCII_EXTRA_CHAR)
#define din_evccIDType_BYTES_SIZE (8)
#define din_evseIDType_BYTES_SIZE (32)
#define din_GenChallenge_CHARACTER_SIZE (EXI_STRING_MAX_LEN + ASCII_EXTRA_CHAR)
#define din_sessionIDType_BYTES_SIZE (8)
#define din_FaultMsg_CHARACTER_SIZE (64 + ASCII_EXTRA_CHAR)


// enum for function numbers
typedef enum {
    din_AC_EVChargeParameter = 0,
    din_AC_EVSEChargeParameter = 1,
    din_AC_EVSEStatus = 2,
    din_BodyElement = 3,
    din_CableCheckReq = 4,
    din_CableCheckRes = 5,
    din_CanonicalizationMethod = 6,
    din_CertificateInstallationReq = 7,
    din_CertificateInstallationRes = 8,
    din_CertificateUpdateReq = 9,
    din_CertificateUpdateRes = 10,
    din_ChargeParameterDiscoveryReq = 11,
    din_ChargeParameterDiscoveryRes = 12,
    din_ChargingStatusReq = 13,
    din_ChargingStatusRes = 14,
    din_ContractAuthenticationReq = 15,
    din_ContractAuthenticationRes = 16,
    din_CurrentDemandReq = 17,
    din_CurrentDemandRes = 18,
    din_DC_EVChargeParameter = 19,
    din_DC_EVPowerDeliveryParameter = 20,
    din_DC_EVSEChargeParameter = 21,
    din_DC_EVSEStatus = 22,
    din_DC_EVStatus = 23,
    din_DSAKeyValue = 24,
    din_DigestMethod = 25,
    din_DigestValue = 26,
    din_EVChargeParameter = 27,
    din_EVPowerDeliveryParameter = 28,
    din_EVSEChargeParameter = 29,
    din_EVSEStatus = 30,
    din_EVStatus = 31,
    din_Entry = 32,
    din_KeyInfo = 33,
    din_KeyName = 34,
    din_KeyValue = 35,
    din_Manifest = 36,
    din_MeteringReceiptReq = 37,
    din_MeteringReceiptRes = 38,
    din_MgmtData = 39,
    din_Object = 40,
    din_PGPData = 41,
    din_PMaxScheduleEntry = 42,
    din_PaymentDetailsReq = 43,
    din_PaymentDetailsRes = 44,
    din_PowerDeliveryReq = 45,
    din_PowerDeliveryRes = 46,
    din_PreChargeReq = 47,
    din_PreChargeRes = 48,
    din_RSAKeyValue = 49,
    din_Reference = 50,
    din_RelativeTimeInterval = 51,
    din_RetrievalMethod = 52,
    din_SAScheduleList = 53,
    din_SASchedules = 54,
    din_SPKIData = 55,
    din_SalesTariffEntry = 56,
    din_ServiceCharge = 57,
    din_ServiceDetailReq = 58,
    din_ServiceDetailRes = 59,
    din_ServiceDiscoveryReq = 60,
    din_ServiceDiscoveryRes = 61,
    din_ServicePaymentSelectionReq = 62,
    din_ServicePaymentSelectionRes = 63,
    din_SessionSetupReq = 64,
    din_SessionSetupRes = 65,
    din_SessionStopReq = 66,
    din_SessionStopRes = 67,
    din_Signature = 68,
    din_SignatureMethod = 69,
    din_SignatureProperties = 70,
    din_SignatureProperty = 71,
    din_SignatureValue = 72,
    din_SignedInfo = 73,
    din_TimeInterval = 74,
    din_Transform = 75,
    din_Transforms = 76,
    din_V2G_Message = 77,
    din_WeldingDetectionReq = 78,
    din_WeldingDetectionRes = 79,
    din_X509Data = 80
} din_generatedFunctionNumbersType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}costKind; type={urn:din:70121:2012:MsgDataTypes}costKindType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_costKindType_relativePricePercentage = 0,
    din_costKindType_RenewableGenerationPercentage = 1,
    din_costKindType_CarbonDioxideEmission = 2
} din_costKindType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}EVSEIsolationStatus; type={urn:din:70121:2012:MsgDataTypes}isolationLevelType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_isolationLevelType_Invalid = 0,
    din_isolationLevelType_Valid = 1,
    din_isolationLevelType_Warning = 2,
    din_isolationLevelType_Fault = 3
} din_isolationLevelType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}PaymentOption; type={urn:din:70121:2012:MsgDataTypes}paymentOptionType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_paymentOptionType_Contract = 0,
    din_paymentOptionType_ExternalPayment = 1
} din_paymentOptionType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}EVSEStatusCode; type={urn:din:70121:2012:MsgDataTypes}DC_EVSEStatusCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_DC_EVSEStatusCodeType_EVSE_NotReady = 0,
    din_DC_EVSEStatusCodeType_EVSE_Ready = 1,
    din_DC_EVSEStatusCodeType_EVSE_Shutdown = 2,
    din_DC_EVSEStatusCodeType_EVSE_UtilityInterruptEvent = 3,
    din_DC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive = 4,
    din_DC_EVSEStatusCodeType_EVSE_EmergencyShutdown = 5,
    din_DC_EVSEStatusCodeType_EVSE_Malfunction = 6,
    din_DC_EVSEStatusCodeType_Reserved_8 = 7,
    din_DC_EVSEStatusCodeType_Reserved_9 = 8,
    din_DC_EVSEStatusCodeType_Reserved_A = 9,
    din_DC_EVSEStatusCodeType_Reserved_B = 10,
    din_DC_EVSEStatusCodeType_Reserved_C = 11
} din_DC_EVSEStatusCodeType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}Unit; type={urn:din:70121:2012:MsgDataTypes}unitSymbolType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_unitSymbolType_h = 0,
    din_unitSymbolType_m = 1,
    din_unitSymbolType_s = 2,
    din_unitSymbolType_A = 3,
    din_unitSymbolType_Ah = 4,
    din_unitSymbolType_V = 5,
    din_unitSymbolType_VA = 6,
    din_unitSymbolType_W = 7,
    din_unitSymbolType_W_s = 8,
    din_unitSymbolType_Wh = 9
} din_unitSymbolType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}EnergyTransferType; type={urn:din:70121:2012:MsgDataTypes}EVSESupportedEnergyTransferType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_EVSESupportedEnergyTransferType_AC_single_phase_core = 0,
    din_EVSESupportedEnergyTransferType_AC_three_phase_core = 1,
    din_EVSESupportedEnergyTransferType_DC_core = 2,
    din_EVSESupportedEnergyTransferType_DC_extended = 3,
    din_EVSESupportedEnergyTransferType_DC_combo_core = 4,
    din_EVSESupportedEnergyTransferType_DC_dual = 5,
    din_EVSESupportedEnergyTransferType_AC_core1p_DC_extended = 6,
    din_EVSESupportedEnergyTransferType_AC_single_DC_core = 7,
    din_EVSESupportedEnergyTransferType_AC_single_phase_three_phase_core_DC_extended = 8,
    din_EVSESupportedEnergyTransferType_AC_core3p_DC_extended = 9
} din_EVSESupportedEnergyTransferType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}EVErrorCode; type={urn:din:70121:2012:MsgDataTypes}DC_EVErrorCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_DC_EVErrorCodeType_NO_ERROR = 0,
    din_DC_EVErrorCodeType_FAILED_RESSTemperatureInhibit = 1,
    din_DC_EVErrorCodeType_FAILED_EVShiftPosition = 2,
    din_DC_EVErrorCodeType_FAILED_ChargerConnectorLockFault = 3,
    din_DC_EVErrorCodeType_FAILED_EVRESSMalfunction = 4,
    din_DC_EVErrorCodeType_FAILED_ChargingCurrentdifferential = 5,
    din_DC_EVErrorCodeType_FAILED_ChargingVoltageOutOfRange = 6,
    din_DC_EVErrorCodeType_Reserved_A = 7,
    din_DC_EVErrorCodeType_Reserved_B = 8,
    din_DC_EVErrorCodeType_Reserved_C = 9,
    din_DC_EVErrorCodeType_FAILED_ChargingSystemIncompatibility = 10,
    din_DC_EVErrorCodeType_NoData = 11
} din_DC_EVErrorCodeType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}EVSENotification; type={urn:din:70121:2012:MsgDataTypes}EVSENotificationType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_EVSENotificationType_None = 0,
    din_EVSENotificationType_StopCharging = 1,
    din_EVSENotificationType_ReNegotiation = 2
} din_EVSENotificationType;

// Element: definition=enum; name={urn:din:70121:2012:MsgDataTypes}FaultCode; type={urn:din:70121:2012:MsgDataTypes}faultCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_faultCodeType_ParsingError = 0,
    din_faultCodeType_NoTLSRootCertificatAvailable = 1,
    din_faultCodeType_UnknownError = 2
} din_faultCodeType;

// Element: definition=enum; name={urn:din:70121:2012:MsgBody}ResponseCode; type={urn:din:70121:2012:MsgDataTypes}responseCodeType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_responseCodeType_OK = 0,
    din_responseCodeType_OK_NewSessionEstablished = 1,
    din_responseCodeType_OK_OldSessionJoined = 2,
    din_responseCodeType_OK_CertificateExpiresSoon = 3,
    din_responseCodeType_FAILED = 4,
    din_responseCodeType_FAILED_SequenceError = 5,
    din_responseCodeType_FAILED_ServiceIDInvalid = 6,
    din_responseCodeType_FAILED_UnknownSession = 7,
    din_responseCodeType_FAILED_ServiceSelectionInvalid = 8,
    din_responseCodeType_FAILED_PaymentSelectionInvalid = 9,
    din_responseCodeType_FAILED_CertificateExpired = 10,
    din_responseCodeType_FAILED_SignatureError = 11,
    din_responseCodeType_FAILED_NoCertificateAvailable = 12,
    din_responseCodeType_FAILED_CertChainError = 13,
    din_responseCodeType_FAILED_ChallengeInvalid = 14,
    din_responseCodeType_FAILED_ContractCanceled = 15,
    din_responseCodeType_FAILED_WrongChargeParameter = 16,
    din_responseCodeType_FAILED_PowerDeliveryNotApplied = 17,
    din_responseCodeType_FAILED_TariffSelectionInvalid = 18,
    din_responseCodeType_FAILED_ChargingProfileInvalid = 19,
    din_responseCodeType_FAILED_EVSEPresentVoltageToLow = 20,
    din_responseCodeType_FAILED_MeteringSignatureNotValid = 21,
    din_responseCodeType_FAILED_WrongEnergyTransferType = 22
} din_responseCodeType;

// Element: definition=enum; name={urn:din:70121:2012:MsgBody}EVRequestedEnergyTransferType; type={urn:din:70121:2012:MsgDataTypes}EVRequestedEnergyTransferType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_EVRequestedEnergyTransferType_AC_single_phase_core = 0,
    din_EVRequestedEnergyTransferType_AC_three_phase_core = 1,
    din_EVRequestedEnergyTransferType_DC_core = 2,
    din_EVRequestedEnergyTransferType_DC_extended = 3,
    din_EVRequestedEnergyTransferType_DC_combo_core = 4,
    din_EVRequestedEnergyTransferType_DC_unique = 5
} din_EVRequestedEnergyTransferType;

// Element: definition=enum; name={urn:din:70121:2012:MsgBody}ServiceCategory; type={urn:din:70121:2012:MsgDataTypes}serviceCategoryType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_serviceCategoryType_EVCharging = 0,
    din_serviceCategoryType_Internet = 1,
    din_serviceCategoryType_ContractCertificate = 2,
    din_serviceCategoryType_OtherCustom = 3
} din_serviceCategoryType;

// Element: definition=enum; name={urn:din:70121:2012:MsgBody}EVSEProcessing; type={urn:din:70121:2012:MsgDataTypes}EVSEProcessingType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_EVSEProcessingType_Finished = 0,
    din_EVSEProcessingType_Ongoing = 1
} din_EVSEProcessingType;

// Element: definition=enum; name=ValueType; type={urn:din:70121:2012:MsgDataTypes}valueType; base type=string; content type=simple;
//          abstract=False; final=False; derivation=restriction;
typedef enum {
    din_valueType_bool = 0,
    din_valueType_byte = 1,
    din_valueType_short = 2,
    din_valueType_int = 3,
    din_valueType_physicalValue = 4,
    din_valueType_string = 5
} din_valueType;

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Cost; type={urn:din:70121:2012:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
struct din_CostType {
    // costKind, costKindType (base: string)
    din_costKindType costKind;
    // amount, unsignedInt (base: unsignedLong)
    uint32_t amount;
    // amountMultiplier, unitMultiplierType (base: byte)
    int8_t amountMultiplier;
    unsigned int amountMultiplier_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}RelativeTimeInterval; type={urn:din:70121:2012:MsgDataTypes}RelativeTimeIntervalType; base type=IntervalType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: start, unsignedInt (1, 1); duration, unsignedInt (0, 1);
struct din_RelativeTimeIntervalType {
    // start, unsignedInt (base: unsignedLong)
    uint32_t start;
    // duration, unsignedInt (base: unsignedLong)
    uint32_t duration;
    unsigned int duration_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}TimeInterval; type={urn:din:70121:2012:MsgDataTypes}IntervalType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_IntervalType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ConsumptionCost; type={urn:din:70121:2012:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, unsignedInt (1, 1); Cost, CostType (0, 1) (original max unbounded);
struct din_ConsumptionCostType {
    // startValue, unsignedInt (base: unsignedLong)
    uint32_t startValue;
    // Cost, CostType
    struct din_CostType Cost;
    unsigned int Cost_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
struct din_TransformType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[din_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;

    // XPath, string
    struct {
        char characters[din_XPath_CHARACTER_SIZE];
        uint16_t charactersLen;
    } XPath;
    unsigned int XPath_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}PMaxScheduleEntry; type={urn:din:70121:2012:MsgDataTypes}PMaxScheduleEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); PMax, PMaxType (1, 1);
struct din_PMaxScheduleEntryType {
    // RelativeTimeInterval, RelativeTimeIntervalType (base: IntervalType)
    struct din_RelativeTimeIntervalType RelativeTimeInterval;
    unsigned int RelativeTimeInterval_isUsed:1;
    // TimeInterval, IntervalType
    struct din_IntervalType TimeInterval;
    unsigned int TimeInterval_isUsed:1;
    // PMax, PMaxType (base: short)
    int16_t PMax;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SalesTariffEntry; type={urn:din:70121:2012:MsgDataTypes}SalesTariffEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); EPriceLevel, unsignedByte (1, 1); ConsumptionCost, ConsumptionCostType (0, 1) (original max unbounded);
struct din_SalesTariffEntryType {
    // RelativeTimeInterval, RelativeTimeIntervalType (base: IntervalType)
    struct din_RelativeTimeIntervalType RelativeTimeInterval;
    unsigned int RelativeTimeInterval_isUsed:1;
    // TimeInterval, IntervalType
    struct din_IntervalType TimeInterval;
    unsigned int TimeInterval_isUsed:1;
    // EPriceLevel, unsignedByte (base: unsignedShort)
    uint8_t EPriceLevel;
    // ConsumptionCost, ConsumptionCostType
    struct din_ConsumptionCostType ConsumptionCost;
    unsigned int ConsumptionCost_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
struct din_TransformsType {
    // Transform, TransformType
    struct din_TransformType Transform;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
struct din_DSAKeyValueType {
    // P, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } P;
    unsigned int P_isUsed:1;

    // Q, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Q;
    unsigned int Q_isUsed:1;

    // G, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } G;
    unsigned int G_isUsed:1;

    // Y, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Y;

    // J, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } J;
    unsigned int J_isUsed:1;

    // Seed, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Seed;
    unsigned int Seed_isUsed:1;

    // PgenCounter, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } PgenCounter;
    unsigned int PgenCounter_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
struct din_X509IssuerSerialType {
    // X509IssuerName, string
    struct {
        char characters[din_X509IssuerName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509IssuerName;
    // X509SerialNumber, integer (base: decimal)
    exi_signed_t X509SerialNumber;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct din_DigestMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[din_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
struct din_RSAKeyValueType {
    // Modulus, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Modulus;

    // Exponent, CryptoBinary (base: base64Binary)
    struct {
        uint8_t bytes[din_CryptoBinary_BYTES_SIZE];
        uint16_t bytesLen;
    } Exponent;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}PMaxSchedule; type={urn:din:70121:2012:MsgDataTypes}PMaxScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PMaxScheduleID, SAIDType (1, 1); PMaxScheduleEntry, PMaxScheduleEntryType (1, 5) (original max unbounded);
struct din_PMaxScheduleType {
    // PMaxScheduleID, SAIDType (base: short)
    int16_t PMaxScheduleID;
    // PMaxScheduleEntry, PMaxScheduleEntryType (base: EntryType)
    struct {
        struct din_PMaxScheduleEntryType array[din_PMaxScheduleEntryType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } PMaxScheduleEntry;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SalesTariff; type={urn:din:70121:2012:MsgDataTypes}SalesTariffType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, IDREF (1, 1); SalesTariffID, SAIDType (1, 1); SalesTariffDescription, tariffDescriptionType (0, 1); NumEPriceLevels, unsignedByte (1, 1); SalesTariffEntry, SalesTariffEntryType (1, 5) (original max unbounded);
struct din_SalesTariffType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // SalesTariffID, SAIDType (base: short)
    int16_t SalesTariffID;
    // SalesTariffDescription, tariffDescriptionType (base: string)
    struct {
        char characters[din_SalesTariffDescription_CHARACTER_SIZE];
        uint16_t charactersLen;
    } SalesTariffDescription;
    unsigned int SalesTariffDescription_isUsed:1;
    // NumEPriceLevels, unsignedByte (base: unsignedShort)
    uint8_t NumEPriceLevels;
    // SalesTariffEntry, SalesTariffEntryType (base: EntryType)
    struct {
        struct din_SalesTariffEntryType array[din_SalesTariffEntryType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } SalesTariffEntry;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
struct din_CanonicalizationMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[din_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ServiceTag; type={urn:din:70121:2012:MsgDataTypes}ServiceTagType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1);
struct din_ServiceTagType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceName, serviceNameType (base: string)
    struct {
        char characters[din_ServiceName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceName;
    unsigned int ServiceName_isUsed:1;
    // ServiceCategory, serviceCategoryType (base: string)
    din_serviceCategoryType ServiceCategory;
    // ServiceScope, serviceScopeType (base: string)
    struct {
        char characters[din_ServiceScope_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceScope;
    unsigned int ServiceScope_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Service; type={urn:din:70121:2012:MsgDataTypes}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceTag, ServiceTagType (1, 1); FreeService, boolean (1, 1);
struct din_ServiceType {
    // ServiceTag, ServiceTagType
    struct din_ServiceTagType ServiceTag;
    // FreeService, boolean
    int FreeService;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SelectedService; type={urn:din:70121:2012:MsgDataTypes}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, short (0, 1);
struct din_SelectedServiceType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ParameterSetID, short (base: int)
    int16_t ParameterSetID;
    unsigned int ParameterSetID_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SAScheduleTuple; type={urn:din:70121:2012:MsgDataTypes}SAScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); PMaxSchedule, PMaxScheduleType (1, 1); SalesTariff, SalesTariffType (0, 1);
struct din_SAScheduleTupleType {
    // SAScheduleTupleID, SAIDType (base: short)
    int16_t SAScheduleTupleID;
    // PMaxSchedule, PMaxScheduleType
    struct din_PMaxScheduleType PMaxSchedule;
    // SalesTariff, SalesTariffType
    struct din_SalesTariffType SalesTariff;
    unsigned int SalesTariff_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}AC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: PowerSwitchClosed, boolean (1, 1); RCD, boolean (1, 1); NotificationMaxDelay, unsignedInt (1, 1); EVSENotification, EVSENotificationType (1, 1);
struct din_AC_EVSEStatusType {
    // PowerSwitchClosed, boolean
    int PowerSwitchClosed;
    // RCD, boolean
    int RCD;
    // NotificationMaxDelay, unsignedInt (base: unsignedLong)
    uint32_t NotificationMaxDelay;
    // EVSENotification, EVSENotificationType (base: string)
    din_EVSENotificationType EVSENotification;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
struct din_SignatureMethodType {
    // Attribute: Algorithm, anyURI
    struct {
        char characters[din_Algorithm_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Algorithm;
    // HMACOutputLength, HMACOutputLengthType (base: integer)
    exi_signed_t HMACOutputLength;
    unsigned int HMACOutputLength_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
struct din_KeyValueType {
    // DSAKeyValue, DSAKeyValueType
    struct din_DSAKeyValueType DSAKeyValue;
    unsigned int DSAKeyValue_isUsed:1;
    // RSAKeyValue, RSAKeyValueType
    struct din_RSAKeyValueType RSAKeyValue;
    unsigned int RSAKeyValue_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SubCertificates; type={urn:din:70121:2012:MsgDataTypes}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1) (original max unbounded);
struct din_SubCertificatesType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[din_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ProfileEntry; type={urn:din:70121:2012:MsgDataTypes}ProfileEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ChargingProfileEntryStart, unsignedInt (1, 1); ChargingProfileEntryMaxPower, PMaxType (1, 1);
struct din_ProfileEntryType {
    // ChargingProfileEntryStart, unsignedInt (base: unsignedLong)
    uint32_t ChargingProfileEntryStart;
    // ChargingProfileEntryMaxPower, PMaxType (base: short)
    int16_t ChargingProfileEntryMaxPower;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
struct din_ReferenceType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: Type, anyURI
    struct {
        char characters[din_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[din_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct din_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;
    // DigestMethod, DigestMethodType
    struct din_DigestMethodType DigestMethod;
    // DigestValue, DigestValueType (base: base64Binary)
    struct {
        uint8_t bytes[din_DigestValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } DigestValue;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
struct din_RetrievalMethodType {
    // Attribute: Type, anyURI
    struct {
        char characters[din_Type_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Type;
    unsigned int Type_isUsed:1;
    // Attribute: URI, anyURI
    struct {
        char characters[din_URI_CHARACTER_SIZE];
        uint16_t charactersLen;
    } URI;
    unsigned int URI_isUsed:1;
    // Transforms, TransformsType
    struct din_TransformsType Transforms;
    unsigned int Transforms_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
struct din_X509DataType {
    // X509IssuerSerial, X509IssuerSerialType
    struct din_X509IssuerSerialType X509IssuerSerial;
    unsigned int X509IssuerSerial_isUsed:1;
    // X509SKI, base64Binary
    struct {
        uint8_t bytes[din_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509SKI;
    unsigned int X509SKI_isUsed:1;

    // X509SubjectName, string
    struct {
        char characters[din_X509SubjectName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } X509SubjectName;
    unsigned int X509SubjectName_isUsed:1;
    // X509Certificate, base64Binary
    struct {
        uint8_t bytes[din_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509Certificate;
    unsigned int X509Certificate_isUsed:1;

    // X509CRL, base64Binary
    struct {
        uint8_t bytes[din_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } X509CRL;
    unsigned int X509CRL_isUsed:1;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
struct din_PGPDataType {
    union {
        // sequence of choice 1
        struct {
            // PGPKeyID, base64Binary
            struct {
                uint8_t bytes[din_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyID;

            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[din_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;
            unsigned int PGPKeyPacket_isUsed:1;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[din_anyType_BYTES_SIZE];
                uint16_t bytesLen;
            } ANY;
            unsigned int ANY_isUsed:1;


        } choice_1;
        unsigned int choice_1_isUsed:1;

        // sequence of choice 2
        struct {
            // PGPKeyPacket, base64Binary
            struct {
                uint8_t bytes[din_base64Binary_BYTES_SIZE];
                uint16_t bytesLen;
            } PGPKeyPacket;

            // ANY, anyType (base: base64Binary)
            struct {
                uint8_t bytes[din_anyType_BYTES_SIZE];
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
struct din_SPKIDataType {
    // SPKISexp, base64Binary
    struct {
        uint8_t bytes[din_base64Binary_BYTES_SIZE];
        uint16_t bytesLen;
    } SPKISexp;

    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 1) (original max 4);
struct din_SignedInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CanonicalizationMethod, CanonicalizationMethodType
    struct din_CanonicalizationMethodType CanonicalizationMethod;
    // SignatureMethod, SignatureMethodType
    struct din_SignatureMethodType SignatureMethod;
    // Reference, ReferenceType
    struct din_ReferenceType Reference;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}DC_EVStatus; type={urn:din:70121:2012:MsgDataTypes}DC_EVStatusType; base type=EVStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVReady, boolean (1, 1); EVCabinConditioning, boolean (0, 1); EVRESSConditioning, boolean (0, 1); EVErrorCode, DC_EVErrorCodeType (1, 1); EVRESSSOC, percentValueType (1, 1);
struct din_DC_EVStatusType {
    // EVReady, boolean
    int EVReady;
    // EVCabinConditioning, boolean
    int EVCabinConditioning;
    unsigned int EVCabinConditioning_isUsed:1;
    // EVRESSConditioning, boolean
    int EVRESSConditioning;
    unsigned int EVRESSConditioning_isUsed:1;
    // EVErrorCode, DC_EVErrorCodeType (base: string)
    din_DC_EVErrorCodeType EVErrorCode;
    // EVRESSSOC, percentValueType (base: byte)
    int8_t EVRESSSOC;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
struct din_SignatureValueType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // CONTENT, SignatureValueType (base: base64Binary)
    struct {
        uint8_t bytes[din_SignatureValueType_BYTES_SIZE];
        uint16_t bytesLen;
    } CONTENT;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractSignatureCertChain; type={urn:din:70121:2012:MsgDataTypes}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
struct din_CertificateChainType {
    // Certificate, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[din_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } Certificate;

    // SubCertificates, SubCertificatesType
    struct din_SubCertificatesType SubCertificates;
    unsigned int SubCertificates_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}DC_EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}DC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEIsolationStatus, isolationLevelType (0, 1); EVSEStatusCode, DC_EVSEStatusCodeType (1, 1); NotificationMaxDelay, unsignedInt (1, 1); EVSENotification, EVSENotificationType (1, 1);
struct din_DC_EVSEStatusType {
    // EVSEIsolationStatus, isolationLevelType (base: string)
    din_isolationLevelType EVSEIsolationStatus;
    unsigned int EVSEIsolationStatus_isUsed:1;
    // EVSEStatusCode, DC_EVSEStatusCodeType (base: string)
    din_DC_EVSEStatusCodeType EVSEStatusCode;
    // NotificationMaxDelay, unsignedInt (base: unsignedLong)
    uint32_t NotificationMaxDelay;
    // EVSENotification, EVSENotificationType (base: string)
    din_EVSENotificationType EVSENotification;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}EVTargetVoltage; type={urn:din:70121:2012:MsgDataTypes}PhysicalValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Multiplier, unitMultiplierType (1, 1); Unit, unitSymbolType (0, 1); Value, short (1, 1);
struct din_PhysicalValueType {
    // Multiplier, unitMultiplierType (base: byte)
    int8_t Multiplier;
    // Unit, unitSymbolType (base: string)
    din_unitSymbolType Unit;
    unsigned int Unit_isUsed:1;
    // Value, short (base: int)
    int16_t Value;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Parameter; type={urn:din:70121:2012:MsgDataTypes}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, string (1, 1); ValueType, valueType (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); physicalValue, PhysicalValueType (0, 1); stringValue, string (0, 1);
struct din_ParameterType {
    // Attribute: Name, string
    struct {
        char characters[din_Name_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Name;
    // Attribute: ValueType, valueType (base: string)
    din_valueType ValueType;
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
    struct din_PhysicalValueType physicalValue;
    unsigned int physicalValue_isUsed:1;
    // stringValue, string
    struct {
        char characters[din_stringValue_CHARACTER_SIZE];
        uint16_t charactersLen;
    } stringValue;
    unsigned int stringValue_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ParameterSet; type={urn:din:70121:2012:MsgDataTypes}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, short (1, 1); Parameter, ParameterType (1, 1) (original max unbounded);
struct din_ParameterSetType {
    // ParameterSetID, short (base: int)
    int16_t ParameterSetID;
    // Parameter, ParameterType
    struct din_ParameterType Parameter;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ListOfRootCertificateIDs; type={urn:din:70121:2012:MsgDataTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, rootCertificateIDType (1, 5) (original max unbounded);
struct din_ListOfRootCertificateIDsType {
    // RootCertificateID, rootCertificateIDType (base: string)
    struct {
        struct {
            char characters[din_RootCertificateID_CHARACTER_SIZE];
            uint16_t charactersLen;
        } array[din_rootCertificateIDType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } RootCertificateID;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentOptions; type={urn:din:70121:2012:MsgDataTypes}PaymentOptionsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PaymentOption, paymentOptionType (1, 2) (original max unbounded);
struct din_PaymentOptionsType {
    // PaymentOption, paymentOptionType (base: string)
    struct {
        din_paymentOptionType array[din_paymentOptionType_2_ARRAY_SIZE];
        uint16_t arrayLen;
    } PaymentOption;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SelectedServiceList; type={urn:din:70121:2012:MsgDataTypes}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16) (original max unbounded);
struct din_SelectedServiceListType {
    // SelectedService, SelectedServiceType
    struct {
        struct din_SelectedServiceType array[din_SelectedServiceType_16_ARRAY_SIZE];
        uint16_t arrayLen;
    } SelectedService;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}AC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (1, 1); EAmount, PhysicalValueType (1, 1); EVMaxVoltage, PhysicalValueType (1, 1); EVMaxCurrent, PhysicalValueType (1, 1); EVMinCurrent, PhysicalValueType (1, 1);
struct din_AC_EVChargeParameterType {
    // DepartureTime, unsignedInt (base: unsignedLong)
    uint32_t DepartureTime;
    // EAmount, PhysicalValueType
    struct din_PhysicalValueType EAmount;
    // EVMaxVoltage, PhysicalValueType
    struct din_PhysicalValueType EVMaxVoltage;
    // EVMaxCurrent, PhysicalValueType
    struct din_PhysicalValueType EVMaxCurrent;
    // EVMinCurrent, PhysicalValueType
    struct din_PhysicalValueType EVMinCurrent;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVMaximumCurrentLimit, PhysicalValueType (1, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); EVMaximumVoltageLimit, PhysicalValueType (1, 1); EVEnergyCapacity, PhysicalValueType (0, 1); EVEnergyRequest, PhysicalValueType (0, 1); FullSOC, percentValueType (0, 1); BulkSOC, percentValueType (0, 1);
struct din_DC_EVChargeParameterType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;
    // EVMaximumCurrentLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumCurrentLimit;
    // EVMaximumPowerLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumPowerLimit;
    unsigned int EVMaximumPowerLimit_isUsed:1;
    // EVMaximumVoltageLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumVoltageLimit;
    // EVEnergyCapacity, PhysicalValueType
    struct din_PhysicalValueType EVEnergyCapacity;
    unsigned int EVEnergyCapacity_isUsed:1;
    // EVEnergyRequest, PhysicalValueType
    struct din_PhysicalValueType EVEnergyRequest;
    unsigned int EVEnergyRequest_isUsed:1;
    // FullSOC, percentValueType (base: byte)
    int8_t FullSOC;
    unsigned int FullSOC_isUsed:1;
    // BulkSOC, percentValueType (base: byte)
    int8_t BulkSOC;
    unsigned int BulkSOC_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}EVChargeParameterType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_EVChargeParameterType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingProfile; type={urn:din:70121:2012:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); ProfileEntry, ProfileEntryType (1, 24) (original max unbounded);
struct din_ChargingProfileType {
    // SAScheduleTupleID, SAIDType (base: short)
    int16_t SAScheduleTupleID;
    // ProfileEntry, ProfileEntryType
    struct {
        struct din_ProfileEntryType array[din_ProfileEntryType_24_ARRAY_SIZE];
        uint16_t arrayLen;
    } ProfileEntry;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}EVSEStatusType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_EVSEStatusType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
struct din_KeyInfoType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // KeyName, string
    struct {
        char characters[din_KeyName_CHARACTER_SIZE];
        uint16_t charactersLen;
    } KeyName;
    unsigned int KeyName_isUsed:1;
    // KeyValue, KeyValueType
    struct din_KeyValueType KeyValue;
    unsigned int KeyValue_isUsed:1;
    // RetrievalMethod, RetrievalMethodType
    struct din_RetrievalMethodType RetrievalMethod;
    unsigned int RetrievalMethod_isUsed:1;
    // X509Data, X509DataType
    struct din_X509DataType X509Data;
    unsigned int X509Data_isUsed:1;
    // PGPData, PGPDataType
    struct din_PGPDataType PGPData;
    unsigned int PGPData_isUsed:1;
    // SPKIData, SPKIDataType
    struct din_SPKIDataType SPKIData;
    unsigned int SPKIData_isUsed:1;
    // MgmtData, string
    struct {
        char characters[din_MgmtData_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MgmtData;
    unsigned int MgmtData_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeService; type={urn:din:70121:2012:MsgDataTypes}ServiceChargeType; base type=ServiceType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceTag, ServiceTagType (1, 1); FreeService, boolean (1, 1); EnergyTransferType, EVSESupportedEnergyTransferType (1, 1);
struct din_ServiceChargeType {
    // ServiceTag, ServiceTagType
    struct din_ServiceTagType ServiceTag;
    // FreeService, boolean
    int FreeService;
    // EnergyTransferType, EVSESupportedEnergyTransferType (base: string)
    din_EVSESupportedEnergyTransferType EnergyTransferType;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceParameterList; type={urn:din:70121:2012:MsgDataTypes}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5) (original max unbounded);
struct din_ServiceParameterListType {
    // ParameterSet, ParameterSetType
    struct {
        struct din_ParameterSetType array[din_ParameterSetType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } ParameterSet;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SAScheduleList; type={urn:din:70121:2012:MsgDataTypes}SAScheduleListType; base type=SASchedulesType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SAScheduleTuple, SAScheduleTupleType (1, 5) (original max unbounded);
struct din_SAScheduleListType {
    // SAScheduleTuple, SAScheduleTupleType
    struct {
        struct din_SAScheduleTupleType array[din_SAScheduleTupleType_5_ARRAY_SIZE];
        uint16_t arrayLen;
    } SAScheduleTuple;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SASchedules; type={urn:din:70121:2012:MsgDataTypes}SASchedulesType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_SASchedulesType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
struct din_DC_EVPowerDeliveryParameterType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;
    // BulkChargingComplete, boolean
    int BulkChargingComplete;
    unsigned int BulkChargingComplete_isUsed:1;
    // ChargingComplete, boolean
    int ChargingComplete;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVPowerDeliveryParameter; type={urn:din:70121:2012:MsgDataTypes}EVPowerDeliveryParameterType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_EVPowerDeliveryParameterType {
    int _unused;
};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
struct din_ObjectType {
    // Attribute: Encoding, anyURI
    struct {
        char characters[din_Encoding_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Encoding;
    unsigned int Encoding_isUsed:1;
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // Attribute: MimeType, string
    struct {
        char characters[din_MimeType_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MimeType;
    unsigned int MimeType_isUsed:1;
    // ANY, anyType (base: base64Binary)
    struct {
        uint8_t bytes[din_anyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ANY;
    unsigned int ANY_isUsed:1;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceList; type={urn:din:70121:2012:MsgDataTypes}ServiceTagListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 1) (original max unbounded);
struct din_ServiceTagListType {
    // Service, ServiceType
    struct din_ServiceType Service;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEMaximumCurrentLimit, PhysicalValueType (1, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1); EVSEMaximumVoltageLimit, PhysicalValueType (1, 1); EVSEMinimumCurrentLimit, PhysicalValueType (1, 1); EVSEMinimumVoltageLimit, PhysicalValueType (1, 1); EVSECurrentRegulationTolerance, PhysicalValueType (0, 1); EVSEPeakCurrentRipple, PhysicalValueType (1, 1); EVSEEnergyToBeDelivered, PhysicalValueType (0, 1);
struct din_DC_EVSEChargeParameterType {
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEMaximumCurrentLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumCurrentLimit;
    // EVSEMaximumPowerLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumPowerLimit;
    unsigned int EVSEMaximumPowerLimit_isUsed:1;
    // EVSEMaximumVoltageLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumVoltageLimit;
    // EVSEMinimumCurrentLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMinimumCurrentLimit;
    // EVSEMinimumVoltageLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMinimumVoltageLimit;
    // EVSECurrentRegulationTolerance, PhysicalValueType
    struct din_PhysicalValueType EVSECurrentRegulationTolerance;
    unsigned int EVSECurrentRegulationTolerance_isUsed:1;
    // EVSEPeakCurrentRipple, PhysicalValueType
    struct din_PhysicalValueType EVSEPeakCurrentRipple;
    // EVSEEnergyToBeDelivered, PhysicalValueType
    struct din_PhysicalValueType EVSEEnergyToBeDelivered;
    unsigned int EVSEEnergyToBeDelivered_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}AC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: AC_EVSEStatus, AC_EVSEStatusType (1, 1); EVSEMaxVoltage, PhysicalValueType (1, 1); EVSEMaxCurrent, PhysicalValueType (1, 1); EVSEMinCurrent, PhysicalValueType (1, 1);
struct din_AC_EVSEChargeParameterType {
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct din_AC_EVSEStatusType AC_EVSEStatus;
    // EVSEMaxVoltage, PhysicalValueType
    struct din_PhysicalValueType EVSEMaxVoltage;
    // EVSEMaxCurrent, PhysicalValueType
    struct din_PhysicalValueType EVSEMaxCurrent;
    // EVSEMinCurrent, PhysicalValueType
    struct din_PhysicalValueType EVSEMinCurrent;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}EVSEChargeParameterType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_EVSEChargeParameterType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeterInfo; type={urn:din:70121:2012:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, PhysicalValueType (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
struct din_MeterInfoType {
    // MeterID, meterIDType (base: string)
    struct {
        char characters[din_MeterID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } MeterID;
    // MeterReading, PhysicalValueType
    struct din_PhysicalValueType MeterReading;
    unsigned int MeterReading_isUsed:1;
    // SigMeterReading, sigMeterReadingType (base: base64Binary)
    struct {
        uint8_t bytes[din_sigMeterReadingType_BYTES_SIZE];
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateInstallationRes; type={urn:din:70121:2012:MsgBody}CertificateInstallationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (1, 1); ResponseCode, responseCodeType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, privateKeyType (1, 1); DHParams, dHParamsType (1, 1); ContractID, contractIDType (1, 1);
struct din_CertificateInstallationResType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // ContractSignatureCertChain, CertificateChainType
    struct din_CertificateChainType ContractSignatureCertChain;
    // ContractSignatureEncryptedPrivateKey, privateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[din_privateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ContractSignatureEncryptedPrivateKey;

    // DHParams, dHParamsType (base: base64Binary)
    struct {
        uint8_t bytes[din_dHParamsType_BYTES_SIZE];
        uint16_t bytesLen;
    } DHParams;

    // ContractID, contractIDType (base: string)
    struct {
        char characters[din_ContractID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ContractID;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CableCheckReq; type={urn:din:70121:2012:MsgBody}CableCheckReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
struct din_CableCheckReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CableCheckRes; type={urn:din:70121:2012:MsgBody}CableCheckResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
struct din_CableCheckResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEProcessing, EVSEProcessingType (base: string)
    din_EVSEProcessingType EVSEProcessing;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PreChargeReq; type={urn:din:70121:2012:MsgBody}PreChargeReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetVoltage, PhysicalValueType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1);
struct din_PreChargeReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;
    // EVTargetVoltage, PhysicalValueType
    struct din_PhysicalValueType EVTargetVoltage;
    // EVTargetCurrent, PhysicalValueType
    struct din_PhysicalValueType EVTargetCurrent;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PreChargeRes; type={urn:din:70121:2012:MsgBody}PreChargeResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
struct din_PreChargeResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct din_PhysicalValueType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CurrentDemandReq; type={urn:din:70121:2012:MsgBody}CurrentDemandReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1); EVMaximumVoltageLimit, PhysicalValueType (0, 1); EVMaximumCurrentLimit, PhysicalValueType (0, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1); RemainingTimeToFullSoC, PhysicalValueType (0, 1); RemainingTimeToBulkSoC, PhysicalValueType (0, 1); EVTargetVoltage, PhysicalValueType (1, 1);
struct din_CurrentDemandReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;
    // EVTargetCurrent, PhysicalValueType
    struct din_PhysicalValueType EVTargetCurrent;
    // EVMaximumVoltageLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumVoltageLimit;
    unsigned int EVMaximumVoltageLimit_isUsed:1;
    // EVMaximumCurrentLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumCurrentLimit;
    unsigned int EVMaximumCurrentLimit_isUsed:1;
    // EVMaximumPowerLimit, PhysicalValueType
    struct din_PhysicalValueType EVMaximumPowerLimit;
    unsigned int EVMaximumPowerLimit_isUsed:1;
    // BulkChargingComplete, boolean
    int BulkChargingComplete;
    unsigned int BulkChargingComplete_isUsed:1;
    // ChargingComplete, boolean
    int ChargingComplete;
    // RemainingTimeToFullSoC, PhysicalValueType
    struct din_PhysicalValueType RemainingTimeToFullSoC;
    unsigned int RemainingTimeToFullSoC_isUsed:1;
    // RemainingTimeToBulkSoC, PhysicalValueType
    struct din_PhysicalValueType RemainingTimeToBulkSoC;
    unsigned int RemainingTimeToBulkSoC_isUsed:1;
    // EVTargetVoltage, PhysicalValueType
    struct din_PhysicalValueType EVTargetVoltage;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CurrentDemandRes; type={urn:din:70121:2012:MsgBody}CurrentDemandResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1); EVSEPresentCurrent, PhysicalValueType (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (0, 1); EVSEMaximumCurrentLimit, PhysicalValueType (0, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1);
struct din_CurrentDemandResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct din_PhysicalValueType EVSEPresentVoltage;
    // EVSEPresentCurrent, PhysicalValueType
    struct din_PhysicalValueType EVSEPresentCurrent;
    // EVSECurrentLimitAchieved, boolean
    int EVSECurrentLimitAchieved;
    // EVSEVoltageLimitAchieved, boolean
    int EVSEVoltageLimitAchieved;
    // EVSEPowerLimitAchieved, boolean
    int EVSEPowerLimitAchieved;
    // EVSEMaximumVoltageLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumVoltageLimit;
    unsigned int EVSEMaximumVoltageLimit_isUsed:1;
    // EVSEMaximumCurrentLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumCurrentLimit;
    unsigned int EVSEMaximumCurrentLimit_isUsed:1;
    // EVSEMaximumPowerLimit, PhysicalValueType
    struct din_PhysicalValueType EVSEMaximumPowerLimit;
    unsigned int EVSEMaximumPowerLimit_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}WeldingDetectionReq; type={urn:din:70121:2012:MsgBody}WeldingDetectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
struct din_WeldingDetectionReqType {
    // DC_EVStatus, DC_EVStatusType (base: EVStatusType)
    struct din_DC_EVStatusType DC_EVStatus;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}WeldingDetectionRes; type={urn:din:70121:2012:MsgBody}WeldingDetectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
struct din_WeldingDetectionResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    // EVSEPresentVoltage, PhysicalValueType
    struct din_PhysicalValueType EVSEPresentVoltage;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionSetupReq; type={urn:din:70121:2012:MsgBody}SessionSetupReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVCCID, evccIDType (1, 1);
struct din_SessionSetupReqType {
    // EVCCID, evccIDType (base: hexBinary)
    struct {
        uint8_t bytes[din_evccIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } EVCCID;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateInstallationReq; type={urn:din:70121:2012:MsgBody}CertificateInstallationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); OEMProvisioningCert, certificateType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); DHParams, dHParamsType (1, 1);
struct din_CertificateInstallationReqType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // OEMProvisioningCert, certificateType (base: base64Binary)
    struct {
        uint8_t bytes[din_certificateType_BYTES_SIZE];
        uint16_t bytesLen;
    } OEMProvisioningCert;

    // ListOfRootCertificateIDs, ListOfRootCertificateIDsType
    struct din_ListOfRootCertificateIDsType ListOfRootCertificateIDs;
    // DHParams, dHParamsType (base: base64Binary)
    struct {
        uint8_t bytes[din_dHParamsType_BYTES_SIZE];
        uint16_t bytesLen;
    } DHParams;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionSetupRes; type={urn:din:70121:2012:MsgBody}SessionSetupResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); DateTimeNow, long (0, 1);
struct din_SessionSetupResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // EVSEID, evseIDType (base: hexBinary)
    struct {
        uint8_t bytes[din_evseIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } EVSEID;

    // DateTimeNow, long (base: integer)
    int64_t DateTimeNow;
    unsigned int DateTimeNow_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDiscoveryReq; type={urn:din:70121:2012:MsgBody}ServiceDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceScope, serviceScopeType (0, 1); ServiceCategory, serviceCategoryType (0, 1);
struct din_ServiceDiscoveryReqType {
    // ServiceScope, serviceScopeType (base: string)
    struct {
        char characters[din_ServiceScope_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ServiceScope;
    unsigned int ServiceScope_isUsed:1;
    // ServiceCategory, serviceCategoryType (base: string)
    din_serviceCategoryType ServiceCategory;
    unsigned int ServiceCategory_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDiscoveryRes; type={urn:din:70121:2012:MsgBody}ServiceDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); PaymentOptions, PaymentOptionsType (1, 1); ChargeService, ServiceChargeType (1, 1); ServiceList, ServiceTagListType (0, 1);
struct din_ServiceDiscoveryResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // PaymentOptions, PaymentOptionsType
    struct din_PaymentOptionsType PaymentOptions;
    // ChargeService, ServiceChargeType (base: ServiceType)
    struct din_ServiceChargeType ChargeService;
    // ServiceList, ServiceTagListType
    struct din_ServiceTagListType ServiceList;
    unsigned int ServiceList_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDetailReq; type={urn:din:70121:2012:MsgBody}ServiceDetailReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1);
struct din_ServiceDetailReqType {
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDetailRes; type={urn:din:70121:2012:MsgBody}ServiceDetailResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (0, 1);
struct din_ServiceDetailResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // ServiceID, serviceIDType (base: unsignedShort)
    uint16_t ServiceID;
    // ServiceParameterList, ServiceParameterListType
    struct din_ServiceParameterListType ServiceParameterList;
    unsigned int ServiceParameterList_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServicePaymentSelectionReq; type={urn:din:70121:2012:MsgBody}ServicePaymentSelectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SelectedPaymentOption, paymentOptionType (1, 1); SelectedServiceList, SelectedServiceListType (1, 1);
struct din_ServicePaymentSelectionReqType {
    // SelectedPaymentOption, paymentOptionType (base: string)
    din_paymentOptionType SelectedPaymentOption;
    // SelectedServiceList, SelectedServiceListType
    struct din_SelectedServiceListType SelectedServiceList;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServicePaymentSelectionRes; type={urn:din:70121:2012:MsgBody}ServicePaymentSelectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
struct din_ServicePaymentSelectionResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentDetailsReq; type={urn:din:70121:2012:MsgBody}PaymentDetailsReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ContractID, contractIDType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1);
struct din_PaymentDetailsReqType {
    // ContractID, contractIDType (base: string)
    struct {
        char characters[din_ContractID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ContractID;
    // ContractSignatureCertChain, CertificateChainType
    struct din_CertificateChainType ContractSignatureCertChain;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentDetailsRes; type={urn:din:70121:2012:MsgBody}PaymentDetailsResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); GenChallenge, genChallengeType (1, 1); DateTimeNow, long (1, 1);
struct din_PaymentDetailsResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // GenChallenge, genChallengeType (base: string)
    struct {
        char characters[din_GenChallenge_CHARACTER_SIZE];
        uint16_t charactersLen;
    } GenChallenge;
    // DateTimeNow, long (base: integer)
    int64_t DateTimeNow;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractAuthenticationReq; type={urn:din:70121:2012:MsgBody}ContractAuthenticationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); GenChallenge, genChallengeType (0, 1);
struct din_ContractAuthenticationReqType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // GenChallenge, genChallengeType (base: string)
    struct {
        char characters[din_GenChallenge_CHARACTER_SIZE];
        uint16_t charactersLen;
    } GenChallenge;
    unsigned int GenChallenge_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractAuthenticationRes; type={urn:din:70121:2012:MsgBody}ContractAuthenticationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
struct din_ContractAuthenticationResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // EVSEProcessing, EVSEProcessingType (base: string)
    din_EVSEProcessingType EVSEProcessing;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryReq; type={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVRequestedEnergyTransferType, EVRequestedEnergyTransferType (1, 1); AC_EVChargeParameter, AC_EVChargeParameterType (0, 1); DC_EVChargeParameter, DC_EVChargeParameterType (0, 1); EVChargeParameter, EVChargeParameterType (0, 1);
struct din_ChargeParameterDiscoveryReqType {
    // EVRequestedEnergyTransferType, EVRequestedEnergyTransferType (base: string)
    din_EVRequestedEnergyTransferType EVRequestedEnergyTransferType;
    // AC_EVChargeParameter, AC_EVChargeParameterType (base: EVChargeParameterType)
    struct din_AC_EVChargeParameterType AC_EVChargeParameter;
    unsigned int AC_EVChargeParameter_isUsed:1;
    // DC_EVChargeParameter, DC_EVChargeParameterType (base: EVChargeParameterType)
    struct din_DC_EVChargeParameterType DC_EVChargeParameter;
    unsigned int DC_EVChargeParameter_isUsed:1;
    // EVChargeParameter, EVChargeParameterType
    struct din_EVChargeParameterType EVChargeParameter;
    unsigned int EVChargeParameter_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryRes; type={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1); SAScheduleList, SAScheduleListType (0, 1); SASchedules, SASchedulesType (0, 1); AC_EVSEChargeParameter, AC_EVSEChargeParameterType (0, 1); DC_EVSEChargeParameter, DC_EVSEChargeParameterType (0, 1); EVSEChargeParameter, EVSEChargeParameterType (0, 1);
struct din_ChargeParameterDiscoveryResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // EVSEProcessing, EVSEProcessingType (base: string)
    din_EVSEProcessingType EVSEProcessing;
    // SAScheduleList, SAScheduleListType (base: SASchedulesType)
    struct din_SAScheduleListType SAScheduleList;
    unsigned int SAScheduleList_isUsed:1;
    // SASchedules, SASchedulesType
    struct din_SASchedulesType SASchedules;
    unsigned int SASchedules_isUsed:1;
    // AC_EVSEChargeParameter, AC_EVSEChargeParameterType (base: EVSEChargeParameterType)
    struct din_AC_EVSEChargeParameterType AC_EVSEChargeParameter;
    unsigned int AC_EVSEChargeParameter_isUsed:1;
    // DC_EVSEChargeParameter, DC_EVSEChargeParameterType (base: EVSEChargeParameterType)
    struct din_DC_EVSEChargeParameterType DC_EVSEChargeParameter;
    unsigned int DC_EVSEChargeParameter_isUsed:1;
    // EVSEChargeParameter, EVSEChargeParameterType
    struct din_EVSEChargeParameterType EVSEChargeParameter;
    unsigned int EVSEChargeParameter_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PowerDeliveryReq; type={urn:din:70121:2012:MsgBody}PowerDeliveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ReadyToChargeState, boolean (1, 1); ChargingProfile, ChargingProfileType (0, 1); DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (0, 1); EVPowerDeliveryParameter, EVPowerDeliveryParameterType (0, 1);
struct din_PowerDeliveryReqType {
    // ReadyToChargeState, boolean
    int ReadyToChargeState;
    // ChargingProfile, ChargingProfileType
    struct din_ChargingProfileType ChargingProfile;
    unsigned int ChargingProfile_isUsed:1;
    // DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (base: EVPowerDeliveryParameterType)
    struct din_DC_EVPowerDeliveryParameterType DC_EVPowerDeliveryParameter;
    unsigned int DC_EVPowerDeliveryParameter_isUsed:1;
    // EVPowerDeliveryParameter, EVPowerDeliveryParameterType
    struct din_EVPowerDeliveryParameterType EVPowerDeliveryParameter;
    unsigned int EVPowerDeliveryParameter_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PowerDeliveryRes; type={urn:din:70121:2012:MsgBody}PowerDeliveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
struct din_PowerDeliveryResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct din_AC_EVSEStatusType AC_EVSEStatus;
    unsigned int AC_EVSEStatus_isUsed:1;
    // DC_EVSEStatus, DC_EVSEStatusType (base: EVSEStatusType)
    struct din_DC_EVSEStatusType DC_EVSEStatus;
    unsigned int DC_EVSEStatus_isUsed:1;
    // EVSEStatus, EVSEStatusType
    struct din_EVSEStatusType EVSEStatus;
    unsigned int EVSEStatus_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingStatusReq; type={urn:din:70121:2012:MsgBody}ChargingStatusReqType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
// Particle: 
struct din_ChargingStatusReqType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingStatusRes; type={urn:din:70121:2012:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
struct din_ChargingStatusResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // EVSEID, evseIDType (base: hexBinary)
    struct {
        uint8_t bytes[din_evseIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } EVSEID;

    // SAScheduleTupleID, SAIDType (base: short)
    int16_t SAScheduleTupleID;
    // EVSEMaxCurrent, PhysicalValueType
    struct din_PhysicalValueType EVSEMaxCurrent;
    unsigned int EVSEMaxCurrent_isUsed:1;
    // MeterInfo, MeterInfoType
    struct din_MeterInfoType MeterInfo;
    unsigned int MeterInfo_isUsed:1;
    // ReceiptRequired, boolean
    int ReceiptRequired;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct din_AC_EVSEStatusType AC_EVSEStatus;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeteringReceiptReq; type={urn:din:70121:2012:MsgBody}MeteringReceiptReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); SessionID, sessionIDType (1, 1); SAScheduleTupleID, SAIDType (0, 1); MeterInfo, MeterInfoType (1, 1);
struct din_MeteringReceiptReqType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[din_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // SAScheduleTupleID, SAIDType (base: short)
    int16_t SAScheduleTupleID;
    unsigned int SAScheduleTupleID_isUsed:1;
    // MeterInfo, MeterInfoType
    struct din_MeterInfoType MeterInfo;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeteringReceiptRes; type={urn:din:70121:2012:MsgBody}MeteringReceiptResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
struct din_MeteringReceiptResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // AC_EVSEStatus, AC_EVSEStatusType (base: EVSEStatusType)
    struct din_AC_EVSEStatusType AC_EVSEStatus;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionStopReq; type={urn:din:70121:2012:MsgBody}SessionStopType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
// Particle: 
struct din_SessionStopType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionStopRes; type={urn:din:70121:2012:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
struct din_SessionStopResType {
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateUpdateReq; type={urn:din:70121:2012:MsgBody}CertificateUpdateReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractID, contractIDType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); DHParams, dHParamsType (1, 1);
struct din_CertificateUpdateReqType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // ContractSignatureCertChain, CertificateChainType
    struct din_CertificateChainType ContractSignatureCertChain;
    // ContractID, contractIDType (base: string)
    struct {
        char characters[din_ContractID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ContractID;
    // ListOfRootCertificateIDs, ListOfRootCertificateIDsType
    struct din_ListOfRootCertificateIDsType ListOfRootCertificateIDs;
    // DHParams, dHParamsType (base: base64Binary)
    struct {
        uint8_t bytes[din_dHParamsType_BYTES_SIZE];
        uint16_t bytesLen;
    } DHParams;


};

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateUpdateRes; type={urn:din:70121:2012:MsgBody}CertificateUpdateResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (1, 1); ResponseCode, responseCodeType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, privateKeyType (1, 1); DHParams, dHParamsType (1, 1); ContractID, contractIDType (1, 1); RetryCounter, short (1, 1);
struct din_CertificateUpdateResType {
    // Attribute: Id, IDREF (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    // ResponseCode, responseCodeType (base: string)
    din_responseCodeType ResponseCode;
    // ContractSignatureCertChain, CertificateChainType
    struct din_CertificateChainType ContractSignatureCertChain;
    // ContractSignatureEncryptedPrivateKey, privateKeyType (base: base64Binary)
    struct {
        uint8_t bytes[din_privateKeyType_BYTES_SIZE];
        uint16_t bytesLen;
    } ContractSignatureEncryptedPrivateKey;

    // DHParams, dHParamsType (base: base64Binary)
    struct {
        uint8_t bytes[din_dHParamsType_BYTES_SIZE];
        uint16_t bytesLen;
    } DHParams;

    // ContractID, contractIDType (base: string)
    struct {
        char characters[din_ContractID_CHARACTER_SIZE];
        uint16_t charactersLen;
    } ContractID;
    // RetryCounter, short (base: int)
    int16_t RetryCounter;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}BodyElement; type={urn:din:70121:2012:MsgDef}BodyBaseType; base type=; content type=empty;
//          abstract=False; final=False;
// Particle: 
struct din_BodyBaseType {
    int _unused;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgHeader}Notification; type={urn:din:70121:2012:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
struct din_NotificationType {
    // FaultCode, faultCodeType (base: string)
    din_faultCodeType FaultCode;
    // FaultMsg, faultMsgType (base: string)
    struct {
        char characters[din_FaultMsg_CHARACTER_SIZE];
        uint16_t charactersLen;
    } FaultMsg;
    unsigned int FaultMsg_isUsed:1;

};

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
struct din_SignatureType {
    // Attribute: Id, ID (base: NCName)
    struct {
        char characters[din_Id_CHARACTER_SIZE];
        uint16_t charactersLen;
    } Id;
    unsigned int Id_isUsed:1;
    // SignedInfo, SignedInfoType
    struct din_SignedInfoType SignedInfo;
    // SignatureValue, SignatureValueType (base: base64Binary)
    struct din_SignatureValueType SignatureValue;
    // KeyInfo, KeyInfoType
    struct din_KeyInfoType KeyInfo;
    unsigned int KeyInfo_isUsed:1;
    // Object, ObjectType
    struct din_ObjectType Object;
    unsigned int Object_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}Header; type={urn:din:70121:2012:MsgHeader}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); Notification, NotificationType (0, 1); Signature, SignatureType (0, 1);
struct din_MessageHeaderType {
    // SessionID, sessionIDType (base: hexBinary)
    struct {
        uint8_t bytes[din_sessionIDType_BYTES_SIZE];
        uint16_t bytesLen;
    } SessionID;

    // Notification, NotificationType
    struct din_NotificationType Notification;
    unsigned int Notification_isUsed:1;
    // Signature, SignatureType
    struct din_SignatureType Signature;
    unsigned int Signature_isUsed:1;

};

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}Body; type={urn:din:70121:2012:MsgDef}BodyType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: BodyElement, BodyBaseType (0, 1); CableCheckReq, CableCheckReqType (0, 1); CableCheckRes, CableCheckResType (0, 1); CertificateInstallationReq, CertificateInstallationReqType (0, 1); CertificateInstallationRes, CertificateInstallationResType (0, 1); CertificateUpdateReq, CertificateUpdateReqType (0, 1); CertificateUpdateRes, CertificateUpdateResType (0, 1); ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReqType (0, 1); ChargeParameterDiscoveryRes, ChargeParameterDiscoveryResType (0, 1); ChargingStatusReq, ChargingStatusReqType (0, 1); ChargingStatusRes, ChargingStatusResType (0, 1); ContractAuthenticationReq, ContractAuthenticationReqType (0, 1); ContractAuthenticationRes, ContractAuthenticationResType (0, 1); CurrentDemandReq, CurrentDemandReqType (0, 1); CurrentDemandRes, CurrentDemandResType (0, 1); MeteringReceiptReq, MeteringReceiptReqType (0, 1); MeteringReceiptRes, MeteringReceiptResType (0, 1); PaymentDetailsReq, PaymentDetailsReqType (0, 1); PaymentDetailsRes, PaymentDetailsResType (0, 1); PowerDeliveryReq, PowerDeliveryReqType (0, 1); PowerDeliveryRes, PowerDeliveryResType (0, 1); PreChargeReq, PreChargeReqType (0, 1); PreChargeRes, PreChargeResType (0, 1); ServiceDetailReq, ServiceDetailReqType (0, 1); ServiceDetailRes, ServiceDetailResType (0, 1); ServiceDiscoveryReq, ServiceDiscoveryReqType (0, 1); ServiceDiscoveryRes, ServiceDiscoveryResType (0, 1); ServicePaymentSelectionReq, ServicePaymentSelectionReqType (0, 1); ServicePaymentSelectionRes, ServicePaymentSelectionResType (0, 1); SessionSetupReq, SessionSetupReqType (0, 1); SessionSetupRes, SessionSetupResType (0, 1); SessionStopReq, SessionStopType (0, 1); SessionStopRes, SessionStopResType (0, 1); WeldingDetectionReq, WeldingDetectionReqType (0, 1); WeldingDetectionRes, WeldingDetectionResType (0, 1);
struct din_BodyType {
    union {
        struct din_BodyBaseType BodyElement;
        struct din_CableCheckReqType CableCheckReq;
        struct din_CableCheckResType CableCheckRes;
        struct din_CertificateInstallationReqType CertificateInstallationReq;
        struct din_CertificateInstallationResType CertificateInstallationRes;
        struct din_CertificateUpdateReqType CertificateUpdateReq;
        struct din_CertificateUpdateResType CertificateUpdateRes;
        struct din_ChargeParameterDiscoveryReqType ChargeParameterDiscoveryReq;
        struct din_ChargeParameterDiscoveryResType ChargeParameterDiscoveryRes;
        struct din_ChargingStatusReqType ChargingStatusReq;
        struct din_ChargingStatusResType ChargingStatusRes;
        struct din_ContractAuthenticationReqType ContractAuthenticationReq;
        struct din_ContractAuthenticationResType ContractAuthenticationRes;
        struct din_CurrentDemandReqType CurrentDemandReq;
        struct din_CurrentDemandResType CurrentDemandRes;
        struct din_MeteringReceiptReqType MeteringReceiptReq;
        struct din_MeteringReceiptResType MeteringReceiptRes;
        struct din_PaymentDetailsReqType PaymentDetailsReq;
        struct din_PaymentDetailsResType PaymentDetailsRes;
        struct din_PowerDeliveryReqType PowerDeliveryReq;
        struct din_PowerDeliveryResType PowerDeliveryRes;
        struct din_PreChargeReqType PreChargeReq;
        struct din_PreChargeResType PreChargeRes;
        struct din_ServiceDetailReqType ServiceDetailReq;
        struct din_ServiceDetailResType ServiceDetailRes;
        struct din_ServiceDiscoveryReqType ServiceDiscoveryReq;
        struct din_ServiceDiscoveryResType ServiceDiscoveryRes;
        struct din_ServicePaymentSelectionReqType ServicePaymentSelectionReq;
        struct din_ServicePaymentSelectionResType ServicePaymentSelectionRes;
        struct din_SessionSetupReqType SessionSetupReq;
        struct din_SessionSetupResType SessionSetupRes;
        struct din_SessionStopType SessionStopReq;
        struct din_SessionStopResType SessionStopRes;
        struct din_WeldingDetectionReqType WeldingDetectionReq;
        struct din_WeldingDetectionResType WeldingDetectionRes;
    };
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
    unsigned int ContractAuthenticationReq_isUsed:1;
    unsigned int ContractAuthenticationRes_isUsed:1;
    unsigned int CurrentDemandReq_isUsed:1;
    unsigned int CurrentDemandRes_isUsed:1;
    unsigned int MeteringReceiptReq_isUsed:1;
    unsigned int MeteringReceiptRes_isUsed:1;
    unsigned int PaymentDetailsReq_isUsed:1;
    unsigned int PaymentDetailsRes_isUsed:1;
    unsigned int PowerDeliveryReq_isUsed:1;
    unsigned int PowerDeliveryRes_isUsed:1;
    unsigned int PreChargeReq_isUsed:1;
    unsigned int PreChargeRes_isUsed:1;
    unsigned int ServiceDetailReq_isUsed:1;
    unsigned int ServiceDetailRes_isUsed:1;
    unsigned int ServiceDiscoveryReq_isUsed:1;
    unsigned int ServiceDiscoveryRes_isUsed:1;
    unsigned int ServicePaymentSelectionReq_isUsed:1;
    unsigned int ServicePaymentSelectionRes_isUsed:1;
    unsigned int SessionSetupReq_isUsed:1;
    unsigned int SessionSetupRes_isUsed:1;
    unsigned int SessionStopReq_isUsed:1;
    unsigned int SessionStopRes_isUsed:1;
    unsigned int WeldingDetectionReq_isUsed:1;
    unsigned int WeldingDetectionRes_isUsed:1;
};

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}V2G_Message; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Header, MessageHeaderType (1, 1); Body, BodyType (1, 1);
struct din_V2G_Message {
    // Header, MessageHeaderType
    struct din_MessageHeaderType Header;
    // Body, BodyType
    struct din_BodyType Body;

};



// root elements of EXI doc
struct din_exiDocument {
    struct din_V2G_Message V2G_Message;
};

// init for structs
void init_din_exiDocument(struct din_exiDocument* exiDoc);
void init_din_V2G_Message(struct din_V2G_Message* V2G_Message);
void init_din_CostType(struct din_CostType* CostType);
void init_din_RelativeTimeIntervalType(struct din_RelativeTimeIntervalType* RelativeTimeIntervalType);
void init_din_IntervalType(struct din_IntervalType* IntervalType);
void init_din_ConsumptionCostType(struct din_ConsumptionCostType* ConsumptionCostType);
void init_din_TransformType(struct din_TransformType* TransformType);
void init_din_PMaxScheduleEntryType(struct din_PMaxScheduleEntryType* PMaxScheduleEntryType);
void init_din_SalesTariffEntryType(struct din_SalesTariffEntryType* SalesTariffEntryType);
void init_din_TransformsType(struct din_TransformsType* TransformsType);
void init_din_DSAKeyValueType(struct din_DSAKeyValueType* DSAKeyValueType);
void init_din_X509IssuerSerialType(struct din_X509IssuerSerialType* X509IssuerSerialType);
void init_din_DigestMethodType(struct din_DigestMethodType* DigestMethodType);
void init_din_RSAKeyValueType(struct din_RSAKeyValueType* RSAKeyValueType);
void init_din_ParameterType(struct din_ParameterType* ParameterType);
void init_din_PMaxScheduleType(struct din_PMaxScheduleType* PMaxScheduleType);
void init_din_SalesTariffType(struct din_SalesTariffType* SalesTariffType);
void init_din_CanonicalizationMethodType(struct din_CanonicalizationMethodType* CanonicalizationMethodType);
void init_din_ServiceTagType(struct din_ServiceTagType* ServiceTagType);
void init_din_ServiceType(struct din_ServiceType* ServiceType);
void init_din_ParameterSetType(struct din_ParameterSetType* ParameterSetType);
void init_din_SelectedServiceType(struct din_SelectedServiceType* SelectedServiceType);
void init_din_SAScheduleTupleType(struct din_SAScheduleTupleType* SAScheduleTupleType);
void init_din_AC_EVSEStatusType(struct din_AC_EVSEStatusType* AC_EVSEStatusType);
void init_din_SignatureMethodType(struct din_SignatureMethodType* SignatureMethodType);
void init_din_KeyValueType(struct din_KeyValueType* KeyValueType);
void init_din_SubCertificatesType(struct din_SubCertificatesType* SubCertificatesType);
void init_din_ProfileEntryType(struct din_ProfileEntryType* ProfileEntryType);
void init_din_ReferenceType(struct din_ReferenceType* ReferenceType);
void init_din_RetrievalMethodType(struct din_RetrievalMethodType* RetrievalMethodType);
void init_din_X509DataType(struct din_X509DataType* X509DataType);
void init_din_PGPDataType(struct din_PGPDataType* PGPDataType);
void init_din_SPKIDataType(struct din_SPKIDataType* SPKIDataType);
void init_din_SignedInfoType(struct din_SignedInfoType* SignedInfoType);
void init_din_DC_EVStatusType(struct din_DC_EVStatusType* DC_EVStatusType);
void init_din_SignatureValueType(struct din_SignatureValueType* SignatureValueType);
void init_din_CertificateChainType(struct din_CertificateChainType* CertificateChainType);
void init_din_DC_EVSEStatusType(struct din_DC_EVSEStatusType* DC_EVSEStatusType);
void init_din_PhysicalValueType(struct din_PhysicalValueType* PhysicalValueType);
void init_din_ListOfRootCertificateIDsType(struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
void init_din_PaymentOptionsType(struct din_PaymentOptionsType* PaymentOptionsType);
void init_din_SelectedServiceListType(struct din_SelectedServiceListType* SelectedServiceListType);
void init_din_AC_EVChargeParameterType(struct din_AC_EVChargeParameterType* AC_EVChargeParameterType);
void init_din_DC_EVChargeParameterType(struct din_DC_EVChargeParameterType* DC_EVChargeParameterType);
void init_din_EVChargeParameterType(struct din_EVChargeParameterType* EVChargeParameterType);
void init_din_ChargingProfileType(struct din_ChargingProfileType* ChargingProfileType);
void init_din_EVSEStatusType(struct din_EVSEStatusType* EVSEStatusType);
void init_din_KeyInfoType(struct din_KeyInfoType* KeyInfoType);
void init_din_ServiceChargeType(struct din_ServiceChargeType* ServiceChargeType);
void init_din_ServiceParameterListType(struct din_ServiceParameterListType* ServiceParameterListType);
void init_din_SAScheduleListType(struct din_SAScheduleListType* SAScheduleListType);
void init_din_SASchedulesType(struct din_SASchedulesType* SASchedulesType);
void init_din_DC_EVPowerDeliveryParameterType(struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
void init_din_EVPowerDeliveryParameterType(struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
void init_din_ObjectType(struct din_ObjectType* ObjectType);
void init_din_ServiceTagListType(struct din_ServiceTagListType* ServiceTagListType);
void init_din_DC_EVSEChargeParameterType(struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
void init_din_AC_EVSEChargeParameterType(struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
void init_din_EVSEChargeParameterType(struct din_EVSEChargeParameterType* EVSEChargeParameterType);
void init_din_MeterInfoType(struct din_MeterInfoType* MeterInfoType);
void init_din_CertificateInstallationResType(struct din_CertificateInstallationResType* CertificateInstallationResType);
void init_din_CableCheckReqType(struct din_CableCheckReqType* CableCheckReqType);
void init_din_CableCheckResType(struct din_CableCheckResType* CableCheckResType);
void init_din_PreChargeReqType(struct din_PreChargeReqType* PreChargeReqType);
void init_din_PreChargeResType(struct din_PreChargeResType* PreChargeResType);
void init_din_CurrentDemandReqType(struct din_CurrentDemandReqType* CurrentDemandReqType);
void init_din_CurrentDemandResType(struct din_CurrentDemandResType* CurrentDemandResType);
void init_din_WeldingDetectionReqType(struct din_WeldingDetectionReqType* WeldingDetectionReqType);
void init_din_WeldingDetectionResType(struct din_WeldingDetectionResType* WeldingDetectionResType);
void init_din_SessionSetupReqType(struct din_SessionSetupReqType* SessionSetupReqType);
void init_din_CertificateInstallationReqType(struct din_CertificateInstallationReqType* CertificateInstallationReqType);
void init_din_SessionSetupResType(struct din_SessionSetupResType* SessionSetupResType);
void init_din_ServiceDiscoveryReqType(struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
void init_din_ServiceDiscoveryResType(struct din_ServiceDiscoveryResType* ServiceDiscoveryResType);
void init_din_ServiceDetailReqType(struct din_ServiceDetailReqType* ServiceDetailReqType);
void init_din_ServiceDetailResType(struct din_ServiceDetailResType* ServiceDetailResType);
void init_din_ServicePaymentSelectionReqType(struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType);
void init_din_ServicePaymentSelectionResType(struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType);
void init_din_PaymentDetailsReqType(struct din_PaymentDetailsReqType* PaymentDetailsReqType);
void init_din_PaymentDetailsResType(struct din_PaymentDetailsResType* PaymentDetailsResType);
void init_din_ContractAuthenticationReqType(struct din_ContractAuthenticationReqType* ContractAuthenticationReqType);
void init_din_ContractAuthenticationResType(struct din_ContractAuthenticationResType* ContractAuthenticationResType);
void init_din_ChargeParameterDiscoveryReqType(struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
void init_din_ChargeParameterDiscoveryResType(struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
void init_din_PowerDeliveryReqType(struct din_PowerDeliveryReqType* PowerDeliveryReqType);
void init_din_PowerDeliveryResType(struct din_PowerDeliveryResType* PowerDeliveryResType);
void init_din_ChargingStatusReqType(struct din_ChargingStatusReqType* ChargingStatusReqType);
void init_din_ChargingStatusResType(struct din_ChargingStatusResType* ChargingStatusResType);
void init_din_MeteringReceiptReqType(struct din_MeteringReceiptReqType* MeteringReceiptReqType);
void init_din_MeteringReceiptResType(struct din_MeteringReceiptResType* MeteringReceiptResType);
void init_din_SessionStopType(struct din_SessionStopType* SessionStopType);
void init_din_SessionStopResType(struct din_SessionStopResType* SessionStopResType);
void init_din_CertificateUpdateReqType(struct din_CertificateUpdateReqType* CertificateUpdateReqType);
void init_din_CertificateUpdateResType(struct din_CertificateUpdateResType* CertificateUpdateResType);
void init_din_BodyBaseType(struct din_BodyBaseType* BodyBaseType);
void init_din_NotificationType(struct din_NotificationType* NotificationType);
void init_din_SignatureType(struct din_SignatureType* SignatureType);
void init_din_MessageHeaderType(struct din_MessageHeaderType* MessageHeaderType);
void init_din_BodyType(struct din_BodyType* BodyType);


#ifdef __cplusplus
}
#endif

#endif /* DIN_MSG_DEF_DATATYPES_H */

