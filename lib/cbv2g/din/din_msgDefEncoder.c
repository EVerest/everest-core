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
  * @file din_msgDefEncoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_basetypes_encoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/din/din_msgDefDatatypes.h"
#include "cbv2g/din/din_msgDefEncoder.h"



static int encode_din_CostType(exi_bitstream_t* stream, const struct din_CostType* CostType);
static int encode_din_RelativeTimeIntervalType(exi_bitstream_t* stream, const struct din_RelativeTimeIntervalType* RelativeTimeIntervalType);
static int encode_din_IntervalType(exi_bitstream_t* stream, const struct din_IntervalType* IntervalType);
static int encode_din_ConsumptionCostType(exi_bitstream_t* stream, const struct din_ConsumptionCostType* ConsumptionCostType);
static int encode_din_TransformType(exi_bitstream_t* stream, const struct din_TransformType* TransformType);
static int encode_din_PMaxScheduleEntryType(exi_bitstream_t* stream, const struct din_PMaxScheduleEntryType* PMaxScheduleEntryType);
static int encode_din_SalesTariffEntryType(exi_bitstream_t* stream, const struct din_SalesTariffEntryType* SalesTariffEntryType);
static int encode_din_TransformsType(exi_bitstream_t* stream, const struct din_TransformsType* TransformsType);
static int encode_din_DSAKeyValueType(exi_bitstream_t* stream, const struct din_DSAKeyValueType* DSAKeyValueType);
static int encode_din_X509IssuerSerialType(exi_bitstream_t* stream, const struct din_X509IssuerSerialType* X509IssuerSerialType);
static int encode_din_DigestMethodType(exi_bitstream_t* stream, const struct din_DigestMethodType* DigestMethodType);
static int encode_din_RSAKeyValueType(exi_bitstream_t* stream, const struct din_RSAKeyValueType* RSAKeyValueType);
static int encode_din_PMaxScheduleType(exi_bitstream_t* stream, const struct din_PMaxScheduleType* PMaxScheduleType);
static int encode_din_SalesTariffType(exi_bitstream_t* stream, const struct din_SalesTariffType* SalesTariffType);
static int encode_din_CanonicalizationMethodType(exi_bitstream_t* stream, const struct din_CanonicalizationMethodType* CanonicalizationMethodType);
static int encode_din_ServiceTagType(exi_bitstream_t* stream, const struct din_ServiceTagType* ServiceTagType);
static int encode_din_ServiceType(exi_bitstream_t* stream, const struct din_ServiceType* ServiceType);
static int encode_din_SelectedServiceType(exi_bitstream_t* stream, const struct din_SelectedServiceType* SelectedServiceType);
static int encode_din_SAScheduleTupleType(exi_bitstream_t* stream, const struct din_SAScheduleTupleType* SAScheduleTupleType);
static int encode_din_AC_EVSEStatusType(exi_bitstream_t* stream, const struct din_AC_EVSEStatusType* AC_EVSEStatusType);
static int encode_din_SignatureMethodType(exi_bitstream_t* stream, const struct din_SignatureMethodType* SignatureMethodType);
static int encode_din_KeyValueType(exi_bitstream_t* stream, const struct din_KeyValueType* KeyValueType);
static int encode_din_SubCertificatesType(exi_bitstream_t* stream, const struct din_SubCertificatesType* SubCertificatesType);
static int encode_din_ProfileEntryType(exi_bitstream_t* stream, const struct din_ProfileEntryType* ProfileEntryType);
static int encode_din_ReferenceType(exi_bitstream_t* stream, const struct din_ReferenceType* ReferenceType);
static int encode_din_RetrievalMethodType(exi_bitstream_t* stream, const struct din_RetrievalMethodType* RetrievalMethodType);
static int encode_din_X509DataType(exi_bitstream_t* stream, const struct din_X509DataType* X509DataType);
static int encode_din_PGPDataType(exi_bitstream_t* stream, const struct din_PGPDataType* PGPDataType);
static int encode_din_SPKIDataType(exi_bitstream_t* stream, const struct din_SPKIDataType* SPKIDataType);
static int encode_din_SignedInfoType(exi_bitstream_t* stream, const struct din_SignedInfoType* SignedInfoType);
static int encode_din_DC_EVStatusType(exi_bitstream_t* stream, const struct din_DC_EVStatusType* DC_EVStatusType);
static int encode_din_SignatureValueType(exi_bitstream_t* stream, const struct din_SignatureValueType* SignatureValueType);
static int encode_din_CertificateChainType(exi_bitstream_t* stream, const struct din_CertificateChainType* CertificateChainType);
static int encode_din_DC_EVSEStatusType(exi_bitstream_t* stream, const struct din_DC_EVSEStatusType* DC_EVSEStatusType);
static int encode_din_PhysicalValueType(exi_bitstream_t* stream, const struct din_PhysicalValueType* PhysicalValueType);
static int encode_din_ParameterType(exi_bitstream_t* stream, const struct din_ParameterType* ParameterType);
static int encode_din_ParameterSetType(exi_bitstream_t* stream, const struct din_ParameterSetType* ParameterSetType);
static int encode_din_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
static int encode_din_PaymentOptionsType(exi_bitstream_t* stream, const struct din_PaymentOptionsType* PaymentOptionsType);
static int encode_din_SelectedServiceListType(exi_bitstream_t* stream, const struct din_SelectedServiceListType* SelectedServiceListType);
static int encode_din_AC_EVChargeParameterType(exi_bitstream_t* stream, const struct din_AC_EVChargeParameterType* AC_EVChargeParameterType);
static int encode_din_DC_EVChargeParameterType(exi_bitstream_t* stream, const struct din_DC_EVChargeParameterType* DC_EVChargeParameterType);
static int encode_din_EVChargeParameterType(exi_bitstream_t* stream, const struct din_EVChargeParameterType* EVChargeParameterType);
static int encode_din_ChargingProfileType(exi_bitstream_t* stream, const struct din_ChargingProfileType* ChargingProfileType);
static int encode_din_EVSEStatusType(exi_bitstream_t* stream, const struct din_EVSEStatusType* EVSEStatusType);
static int encode_din_KeyInfoType(exi_bitstream_t* stream, const struct din_KeyInfoType* KeyInfoType);
static int encode_din_ServiceChargeType(exi_bitstream_t* stream, const struct din_ServiceChargeType* ServiceChargeType);
static int encode_din_ServiceParameterListType(exi_bitstream_t* stream, const struct din_ServiceParameterListType* ServiceParameterListType);
static int encode_din_SAScheduleListType(exi_bitstream_t* stream, const struct din_SAScheduleListType* SAScheduleListType);
static int encode_din_SASchedulesType(exi_bitstream_t* stream, const struct din_SASchedulesType* SASchedulesType);
static int encode_din_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
static int encode_din_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
static int encode_din_ObjectType(exi_bitstream_t* stream, const struct din_ObjectType* ObjectType);
static int encode_din_ServiceTagListType(exi_bitstream_t* stream, const struct din_ServiceTagListType* ServiceTagListType);
static int encode_din_DC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
static int encode_din_AC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
static int encode_din_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_EVSEChargeParameterType* EVSEChargeParameterType);
static int encode_din_MeterInfoType(exi_bitstream_t* stream, const struct din_MeterInfoType* MeterInfoType);
static int encode_din_CertificateInstallationResType(exi_bitstream_t* stream, const struct din_CertificateInstallationResType* CertificateInstallationResType);
static int encode_din_CableCheckReqType(exi_bitstream_t* stream, const struct din_CableCheckReqType* CableCheckReqType);
static int encode_din_CableCheckResType(exi_bitstream_t* stream, const struct din_CableCheckResType* CableCheckResType);
static int encode_din_PreChargeReqType(exi_bitstream_t* stream, const struct din_PreChargeReqType* PreChargeReqType);
static int encode_din_PreChargeResType(exi_bitstream_t* stream, const struct din_PreChargeResType* PreChargeResType);
static int encode_din_CurrentDemandReqType(exi_bitstream_t* stream, const struct din_CurrentDemandReqType* CurrentDemandReqType);
static int encode_din_CurrentDemandResType(exi_bitstream_t* stream, const struct din_CurrentDemandResType* CurrentDemandResType);
static int encode_din_WeldingDetectionReqType(exi_bitstream_t* stream, const struct din_WeldingDetectionReqType* WeldingDetectionReqType);
static int encode_din_WeldingDetectionResType(exi_bitstream_t* stream, const struct din_WeldingDetectionResType* WeldingDetectionResType);
static int encode_din_SessionSetupReqType(exi_bitstream_t* stream, const struct din_SessionSetupReqType* SessionSetupReqType);
static int encode_din_CertificateInstallationReqType(exi_bitstream_t* stream, const struct din_CertificateInstallationReqType* CertificateInstallationReqType);
static int encode_din_SessionSetupResType(exi_bitstream_t* stream, const struct din_SessionSetupResType* SessionSetupResType);
static int encode_din_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
static int encode_din_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct din_ServiceDiscoveryResType* ServiceDiscoveryResType);
static int encode_din_ServiceDetailReqType(exi_bitstream_t* stream, const struct din_ServiceDetailReqType* ServiceDetailReqType);
static int encode_din_ServiceDetailResType(exi_bitstream_t* stream, const struct din_ServiceDetailResType* ServiceDetailResType);
static int encode_din_ServicePaymentSelectionReqType(exi_bitstream_t* stream, const struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType);
static int encode_din_ServicePaymentSelectionResType(exi_bitstream_t* stream, const struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType);
static int encode_din_PaymentDetailsReqType(exi_bitstream_t* stream, const struct din_PaymentDetailsReqType* PaymentDetailsReqType);
static int encode_din_PaymentDetailsResType(exi_bitstream_t* stream, const struct din_PaymentDetailsResType* PaymentDetailsResType);
static int encode_din_ContractAuthenticationReqType(exi_bitstream_t* stream, const struct din_ContractAuthenticationReqType* ContractAuthenticationReqType);
static int encode_din_ContractAuthenticationResType(exi_bitstream_t* stream, const struct din_ContractAuthenticationResType* ContractAuthenticationResType);
static int encode_din_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
static int encode_din_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
static int encode_din_PowerDeliveryReqType(exi_bitstream_t* stream, const struct din_PowerDeliveryReqType* PowerDeliveryReqType);
static int encode_din_PowerDeliveryResType(exi_bitstream_t* stream, const struct din_PowerDeliveryResType* PowerDeliveryResType);
static int encode_din_ChargingStatusReqType(exi_bitstream_t* stream, const struct din_ChargingStatusReqType* ChargingStatusReqType);
static int encode_din_ChargingStatusResType(exi_bitstream_t* stream, const struct din_ChargingStatusResType* ChargingStatusResType);
static int encode_din_MeteringReceiptReqType(exi_bitstream_t* stream, const struct din_MeteringReceiptReqType* MeteringReceiptReqType);
static int encode_din_MeteringReceiptResType(exi_bitstream_t* stream, const struct din_MeteringReceiptResType* MeteringReceiptResType);
static int encode_din_SessionStopType(exi_bitstream_t* stream, const struct din_SessionStopType* SessionStopType);
static int encode_din_SessionStopResType(exi_bitstream_t* stream, const struct din_SessionStopResType* SessionStopResType);
static int encode_din_CertificateUpdateReqType(exi_bitstream_t* stream, const struct din_CertificateUpdateReqType* CertificateUpdateReqType);
static int encode_din_CertificateUpdateResType(exi_bitstream_t* stream, const struct din_CertificateUpdateResType* CertificateUpdateResType);
static int encode_din_BodyBaseType(exi_bitstream_t* stream, const struct din_BodyBaseType* BodyBaseType);
static int encode_din_NotificationType(exi_bitstream_t* stream, const struct din_NotificationType* NotificationType);
static int encode_din_SignatureType(exi_bitstream_t* stream, const struct din_SignatureType* SignatureType);
static int encode_din_MessageHeaderType(exi_bitstream_t* stream, const struct din_MessageHeaderType* MessageHeaderType);
static int encode_din_BodyType(exi_bitstream_t* stream, const struct din_BodyType* BodyType);
static int encode_din_V2G_Message(exi_bitstream_t* stream, const struct din_V2G_Message* V2G_Message);

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Cost; type={urn:din:70121:2012:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
static int encode_din_CostType(exi_bitstream_t* stream, const struct din_CostType* CostType) {
    int grammar_id = 0;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (costKind)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=1
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, CostType->costKind);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 1;
                        }
                    }
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=1; START (amount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, CostType->amount);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=2; START (amountMultiplier), END Element
            if (CostType->amountMultiplier_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (amountMultiplier, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, (uint32_t)CostType->amountMultiplier - -3);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}RelativeTimeInterval; type={urn:din:70121:2012:MsgDataTypes}RelativeTimeIntervalType; base type=IntervalType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: start, unsignedInt (1, 1); duration, unsignedInt (0, 1);
static int encode_din_RelativeTimeIntervalType(exi_bitstream_t* stream, const struct din_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    int grammar_id = 5;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 5:
            // Grammar: ID=5; read/write bits=1; START (start)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=6
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, RelativeTimeIntervalType->start);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 6;
                        }
                    }
                }
            }
            break;
        case 6:
            // Grammar: ID=6; read/write bits=2; START (duration), END Element
            if (RelativeTimeIntervalType->duration_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (duration, unsignedLong); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, RelativeTimeIntervalType->duration);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}TimeInterval; type={urn:din:70121:2012:MsgDataTypes}IntervalType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_IntervalType(exi_bitstream_t* stream, const struct din_IntervalType* IntervalType) {
    // Element has no particles, so the function just encodes END Element
    (void)IntervalType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ConsumptionCost; type={urn:din:70121:2012:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, unsignedInt (1, 1); Cost, CostType (0, 1) (original max unbounded);
static int encode_din_ConsumptionCostType(exi_bitstream_t* stream, const struct din_ConsumptionCostType* ConsumptionCostType) {
    int grammar_id = 7;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (startValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=8
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, ConsumptionCostType->startValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 8;
                        }
                    }
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (Cost), END Element
            if (ConsumptionCostType->Cost_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Cost, CostType); next=9
                    error = encode_din_CostType(stream, &ConsumptionCostType->Cost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 9;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=2; START (Cost), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Cost, CostType); next=3
                    error = encode_din_CostType(stream, &ConsumptionCostType->Cost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int encode_din_TransformType(exi_bitstream_t* stream, const struct din_TransformType* TransformType) {
    int grammar_id = 10;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 10:
            // Grammar: ID=10; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=11

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 11;
                    }
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
            if (TransformType->XPath_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (XPath, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->XPath.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, din_XPath_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (TransformType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)TransformType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, TransformType->ANY.bytesLen, TransformType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}PMaxScheduleEntry; type={urn:din:70121:2012:MsgDataTypes}PMaxScheduleEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); PMax, PMaxType (1, 1);
static int encode_din_PMaxScheduleEntryType(exi_bitstream_t* stream, const struct din_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    int grammar_id = 12;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 12:
            // Grammar: ID=12; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            if (PMaxScheduleEntryType->RelativeTimeInterval_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RelativeTimeInterval, IntervalType); next=13
                    error = encode_din_RelativeTimeIntervalType(stream, &PMaxScheduleEntryType->RelativeTimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 13;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (IntervalType); next=13
                    error = encode_din_IntervalType(stream, &PMaxScheduleEntryType->TimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 13;
                    }
                }
            }
            break;
        case 13:
            // Grammar: ID=13; read/write bits=1; START (PMax)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, PMaxScheduleEntryType->PMax);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SalesTariffEntry; type={urn:din:70121:2012:MsgDataTypes}SalesTariffEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); EPriceLevel, unsignedByte (1, 1); ConsumptionCost, ConsumptionCostType (0, 1) (original max unbounded);
static int encode_din_SalesTariffEntryType(exi_bitstream_t* stream, const struct din_SalesTariffEntryType* SalesTariffEntryType) {
    int grammar_id = 14;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 14:
            // Grammar: ID=14; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            if (SalesTariffEntryType->RelativeTimeInterval_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RelativeTimeInterval, IntervalType); next=15
                    error = encode_din_RelativeTimeIntervalType(stream, &SalesTariffEntryType->RelativeTimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 15;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (IntervalType); next=15
                    error = encode_din_IntervalType(stream, &SalesTariffEntryType->TimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 15;
                    }
                }
            }
            break;
        case 15:
            // Grammar: ID=15; read/write bits=1; START (EPriceLevel)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=16
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, SalesTariffEntryType->EPriceLevel);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 16;
                        }
                    }
                }
            }
            break;
        case 16:
            // Grammar: ID=16; read/write bits=2; START (ConsumptionCost), END Element
            if (SalesTariffEntryType->ConsumptionCost_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ConsumptionCost, ConsumptionCostType); next=17
                    error = encode_din_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 17;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 17:
            // Grammar: ID=17; read/write bits=2; START (ConsumptionCost), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ConsumptionCost, ConsumptionCostType); next=3
                    error = encode_din_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
static int encode_din_TransformsType(exi_bitstream_t* stream, const struct din_TransformsType* TransformsType) {
    int grammar_id = 18;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 18:
            // Grammar: ID=18; read/write bits=1; START (Transform)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (TransformType); next=19
                error = encode_din_TransformType(stream, &TransformsType->Transform);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 19;
                }
            }
            break;
        case 19:
            // Grammar: ID=19; read/write bits=2; START (Transform), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transform, TransformType); next=3
                    error = encode_din_TransformType(stream, &TransformsType->Transform);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}DSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: P, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); Q, CryptoBinary (0, 1) (was 1, 1) (seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1) (was 1, 1) (seq. ['Seed', 'PgenCounter']);
static int encode_din_DSAKeyValueType(exi_bitstream_t* stream, const struct din_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 20;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 20:
            // Grammar: ID=20; read/write bits=2; START (P), START (G), START (Y)
            if (DSAKeyValueType->P_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (P, base64Binary); next=21
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->P.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->P.bytesLen, DSAKeyValueType->P.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 21;
                                }
                            }
                        }
                    }
                }
            }
            else if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=23
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 23;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Y, base64Binary); next=24
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 24;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 21:
            // Grammar: ID=21; read/write bits=1; START (Q)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=22
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Q.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Q.bytesLen, DSAKeyValueType->Q.bytes, din_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 22;
                            }
                        }
                    }
                }
            }
            break;
        case 22:
            // Grammar: ID=22; read/write bits=2; START (G), START (Y)
            if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=23
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 23;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Y, base64Binary); next=24
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 24;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Y)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=24
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, din_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 24;
                            }
                        }
                    }
                }
            }
            break;
        case 24:
            // Grammar: ID=24; read/write bits=2; START (J), START (Seed), END Element
            if (DSAKeyValueType->J_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (J, base64Binary); next=25
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->J.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->J.bytesLen, DSAKeyValueType->J.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 25;
                                }
                            }
                        }
                    }
                }
            }
            else if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=26
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 26;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (Seed), END Element
            if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=26
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 26;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 26:
            // Grammar: ID=26; read/write bits=2; START (PgenCounter), END Element
            if (DSAKeyValueType->PgenCounter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PgenCounter, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->PgenCounter.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->PgenCounter.bytesLen, DSAKeyValueType->PgenCounter.bytes, din_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerial; type={http://www.w3.org/2000/09/xmldsig#}X509IssuerSerialType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerName, string (1, 1); X509SerialNumber, integer (1, 1);
static int encode_din_X509IssuerSerialType(exi_bitstream_t* stream, const struct din_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 27;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 27:
            // Grammar: ID=27; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=28

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509IssuerSerialType->X509IssuerName.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, din_X509IssuerName_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 28;
                            }
                        }
                    }
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=1; START (X509SerialNumber)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (decimal); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_signed(stream, &X509IssuerSerialType->X509SerialNumber);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int encode_din_DigestMethodType(exi_bitstream_t* stream, const struct din_DigestMethodType* DigestMethodType) {
    int grammar_id = 29;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 29:
            // Grammar: ID=29; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=30

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(DigestMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 30;
                    }
                }
            }
            break;
        case 30:
            // Grammar: ID=30; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (DigestMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DigestMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DigestMethodType->ANY.bytesLen, DigestMethodType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RSAKeyValue; type={http://www.w3.org/2000/09/xmldsig#}RSAKeyValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Modulus, CryptoBinary (1, 1); Exponent, CryptoBinary (1, 1);
static int encode_din_RSAKeyValueType(exi_bitstream_t* stream, const struct din_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 31;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 31:
            // Grammar: ID=31; read/write bits=1; START (Modulus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=32
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Modulus.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Modulus.bytesLen, RSAKeyValueType->Modulus.bytes, din_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 32;
                            }
                        }
                    }
                }
            }
            break;
        case 32:
            // Grammar: ID=32; read/write bits=1; START (Exponent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Exponent.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Exponent.bytesLen, RSAKeyValueType->Exponent.bytes, din_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}PMaxSchedule; type={urn:din:70121:2012:MsgDataTypes}PMaxScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PMaxScheduleID, SAIDType (1, 1); PMaxScheduleEntry, PMaxScheduleEntryType (1, 5) (original max unbounded);
static int encode_din_PMaxScheduleType(exi_bitstream_t* stream, const struct din_PMaxScheduleType* PMaxScheduleType) {
    int grammar_id = 33;
    int done = 0;
    int error = 0;
    uint16_t PMaxScheduleEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 33:
            // Grammar: ID=33; read/write bits=1; START (PMaxScheduleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=34
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, PMaxScheduleType->PMaxScheduleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 34;
                        }
                    }
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=1; START (PMaxScheduleEntry)
            if (PMaxScheduleEntry_currentIndex < PMaxScheduleType->PMaxScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EntryType); next=35
                    error = encode_din_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 35:
            // Grammar: ID=35; read/write bits=2; LOOP (PMaxScheduleEntry), END Element
            if (PMaxScheduleEntry_currentIndex < PMaxScheduleType->PMaxScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=35
                    error = encode_din_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 35;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SalesTariff; type={urn:din:70121:2012:MsgDataTypes}SalesTariffType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, IDREF (1, 1); SalesTariffID, SAIDType (1, 1); SalesTariffDescription, tariffDescriptionType (0, 1); NumEPriceLevels, unsignedByte (1, 1); SalesTariffEntry, SalesTariffEntryType (1, 5) (original max unbounded);
static int encode_din_SalesTariffType(exi_bitstream_t* stream, const struct din_SalesTariffType* SalesTariffType) {
    int grammar_id = 36;
    int done = 0;
    int error = 0;
    uint16_t SalesTariffEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=37

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SalesTariffType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SalesTariffType->Id.charactersLen, SalesTariffType->Id.characters, din_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 37;
                    }
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=1; START (SalesTariffID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=38
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, SalesTariffType->SalesTariffID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 38;
                        }
                    }
                }
            }
            break;
        case 38:
            // Grammar: ID=38; read/write bits=2; START (SalesTariffDescription), START (NumEPriceLevels)
            if (SalesTariffType->SalesTariffDescription_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SalesTariffDescription, string); next=39

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SalesTariffType->SalesTariffDescription.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SalesTariffType->SalesTariffDescription.charactersLen, SalesTariffType->SalesTariffDescription.characters, din_SalesTariffDescription_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 39;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (NumEPriceLevels, unsignedShort); next=40
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, SalesTariffType->NumEPriceLevels);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 40;
                            }
                        }
                    }
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=1; START (NumEPriceLevels)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=40
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, SalesTariffType->NumEPriceLevels);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 40;
                        }
                    }
                }
            }
            break;
        case 40:
            // Grammar: ID=40; read/write bits=1; START (SalesTariffEntry)
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EntryType); next=41
                    error = encode_din_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 41;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 41:
            // Grammar: ID=41; read/write bits=2; LOOP (SalesTariffEntry), END Element
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=41
                    error = encode_din_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 41;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int encode_din_CanonicalizationMethodType(exi_bitstream_t* stream, const struct din_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 42;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 42:
            // Grammar: ID=42; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=43

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CanonicalizationMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 43;
                    }
                }
            }
            break;
        case 43:
            // Grammar: ID=43; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (CanonicalizationMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CanonicalizationMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, CanonicalizationMethodType->ANY.bytesLen, CanonicalizationMethodType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ServiceTag; type={urn:din:70121:2012:MsgDataTypes}ServiceTagType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1);
static int encode_din_ServiceTagType(exi_bitstream_t* stream, const struct din_ServiceTagType* ServiceTagType) {
    int grammar_id = 44;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 44:
            // Grammar: ID=44; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=45
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ServiceTagType->ServiceID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 45;
                        }
                    }
                }
            }
            break;
        case 45:
            // Grammar: ID=45; read/write bits=2; START (ServiceName), START (ServiceCategory)
            if (ServiceTagType->ServiceName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceName, string); next=46

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceTagType->ServiceName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceTagType->ServiceName.charactersLen, ServiceTagType->ServiceName.characters, din_ServiceName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 46;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceCategory, string); next=47
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceTagType->ServiceCategory);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 47;
                            }
                        }
                    }
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=47
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceTagType->ServiceCategory);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 47;
                        }
                    }
                }
            }
            break;
        case 47:
            // Grammar: ID=47; read/write bits=2; START (ServiceScope), END Element
            if (ServiceTagType->ServiceScope_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceScope, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceTagType->ServiceScope.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceTagType->ServiceScope.charactersLen, ServiceTagType->ServiceScope.characters, din_ServiceScope_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Service; type={urn:din:70121:2012:MsgDataTypes}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceTag, ServiceTagType (1, 1); FreeService, boolean (1, 1);
static int encode_din_ServiceType(exi_bitstream_t* stream, const struct din_ServiceType* ServiceType) {
    int grammar_id = 48;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 48:
            // Grammar: ID=48; read/write bits=1; START (ServiceTag)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceTagType); next=49
                error = encode_din_ServiceTagType(stream, &ServiceType->ServiceTag);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 49;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=1; START (FreeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, ServiceType->FreeService);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SelectedService; type={urn:din:70121:2012:MsgDataTypes}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, short (0, 1);
static int encode_din_SelectedServiceType(exi_bitstream_t* stream, const struct din_SelectedServiceType* SelectedServiceType) {
    int grammar_id = 50;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 50:
            // Grammar: ID=50; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=51
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, SelectedServiceType->ServiceID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 51;
                        }
                    }
                }
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=2; START (ParameterSetID), END Element
            if (SelectedServiceType->ParameterSetID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetID, int); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, SelectedServiceType->ParameterSetID);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SAScheduleTuple; type={urn:din:70121:2012:MsgDataTypes}SAScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); PMaxSchedule, PMaxScheduleType (1, 1); SalesTariff, SalesTariffType (0, 1);
static int encode_din_SAScheduleTupleType(exi_bitstream_t* stream, const struct din_SAScheduleTupleType* SAScheduleTupleType) {
    int grammar_id = 52;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=53
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, SAScheduleTupleType->SAScheduleTupleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 53;
                        }
                    }
                }
            }
            break;
        case 53:
            // Grammar: ID=53; read/write bits=1; START (PMaxSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PMaxScheduleType); next=54
                error = encode_din_PMaxScheduleType(stream, &SAScheduleTupleType->PMaxSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 54;
                }
            }
            break;
        case 54:
            // Grammar: ID=54; read/write bits=2; START (SalesTariff), END Element
            if (SAScheduleTupleType->SalesTariff_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SalesTariff, SalesTariffType); next=3
                    error = encode_din_SalesTariffType(stream, &SAScheduleTupleType->SalesTariff);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}AC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: PowerSwitchClosed, boolean (1, 1); RCD, boolean (1, 1); NotificationMaxDelay, unsignedInt (1, 1); EVSENotification, EVSENotificationType (1, 1);
static int encode_din_AC_EVSEStatusType(exi_bitstream_t* stream, const struct din_AC_EVSEStatusType* AC_EVSEStatusType) {
    int grammar_id = 55;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 55:
            // Grammar: ID=55; read/write bits=1; START (PowerSwitchClosed)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=56
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, AC_EVSEStatusType->PowerSwitchClosed);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 56;
                        }
                    }
                }
            }
            break;
        case 56:
            // Grammar: ID=56; read/write bits=1; START (RCD)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=57
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, AC_EVSEStatusType->RCD);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 57;
                        }
                    }
                }
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=58
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, AC_EVSEStatusType->NotificationMaxDelay);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 58;
                        }
                    }
                }
            }
            break;
        case 58:
            // Grammar: ID=58; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, AC_EVSEStatusType->EVSENotification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
static int encode_din_SignatureMethodType(exi_bitstream_t* stream, const struct din_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 59;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 59:
            // Grammar: ID=59; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=60

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 60;
                    }
                }
            }
            break;
        case 60:
            // Grammar: ID=60; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            if (SignatureMethodType->HMACOutputLength_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (HMACOutputLength, integer); next=61
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 61;
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (SignatureMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 61:
            // Grammar: ID=61; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (SignatureMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyValue; type={http://www.w3.org/2000/09/xmldsig#}KeyValueType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: DSAKeyValue, DSAKeyValueType (0, 1); RSAKeyValue, RSAKeyValueType (0, 1); ANY, anyType (0, 1);
static int encode_din_KeyValueType(exi_bitstream_t* stream, const struct din_KeyValueType* KeyValueType) {
    int grammar_id = 62;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 62:
            // Grammar: ID=62; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            if (KeyValueType->DSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DSAKeyValue, DSAKeyValueType); next=3
                    error = encode_din_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyValueType->RSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RSAKeyValue, RSAKeyValueType); next=3
                    error = encode_din_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyValueType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyValueType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyValueType->ANY.bytesLen, KeyValueType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SubCertificates; type={urn:din:70121:2012:MsgDataTypes}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1) (original max unbounded);
static int encode_din_SubCertificatesType(exi_bitstream_t* stream, const struct din_SubCertificatesType* SubCertificatesType) {
    int grammar_id = 63;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 63:
            // Grammar: ID=63; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=64
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.bytesLen, SubCertificatesType->Certificate.bytes, din_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 64;
                            }
                        }
                    }
                }
            }
            break;
        case 64:
            // Grammar: ID=64; read/write bits=2; START (Certificate), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Certificate, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.bytesLen, SubCertificatesType->Certificate.bytes, din_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ProfileEntry; type={urn:din:70121:2012:MsgDataTypes}ProfileEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ChargingProfileEntryStart, unsignedInt (1, 1); ChargingProfileEntryMaxPower, PMaxType (1, 1);
static int encode_din_ProfileEntryType(exi_bitstream_t* stream, const struct din_ProfileEntryType* ProfileEntryType) {
    int grammar_id = 65;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 65:
            // Grammar: ID=65; read/write bits=1; START (ChargingProfileEntryStart)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=66
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, ProfileEntryType->ChargingProfileEntryStart);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 66;
                        }
                    }
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=1; START (ChargingProfileEntryMaxPower)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, ProfileEntryType->ChargingProfileEntryMaxPower);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
static int encode_din_ReferenceType(exi_bitstream_t* stream, const struct din_ReferenceType* ReferenceType) {
    int grammar_id = 67;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 67:
            // Grammar: ID=67; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=68

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 68;
                        }
                    }
                }
            }
            else if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=69

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, din_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 69;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=70

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 70;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=71
                    error = encode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 71;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=72
                    error = encode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 72;
                    }
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=69

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, din_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 69;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=70

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 70;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=71
                    error = encode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 71;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=72
                    error = encode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 72;
                    }
                }
            }
            break;
        case 69:
            // Grammar: ID=69; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=70

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 70;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=71
                    error = encode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 71;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=72
                    error = encode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 72;
                    }
                }
            }
            break;
        case 70:
            // Grammar: ID=70; read/write bits=2; START (Transforms), START (DigestMethod)
            if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=71
                    error = encode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 71;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=72
                    error = encode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 72;
                    }
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (DigestMethodType); next=72
                error = encode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 72;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=1; START (DigestValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ReferenceType->DigestValue.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, ReferenceType->DigestValue.bytesLen, ReferenceType->DigestValue.bytes, din_DigestValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}RetrievalMethod; type={http://www.w3.org/2000/09/xmldsig#}RetrievalMethodType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1);
static int encode_din_RetrievalMethodType(exi_bitstream_t* stream, const struct din_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 73;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 73:
            // Grammar: ID=73; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            if (RetrievalMethodType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=74

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, din_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 74;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=75

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, din_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 75;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=3
                    error = encode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 74:
            // Grammar: ID=74; read/write bits=2; START (URI), START (Transforms), END Element
            if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=75

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, din_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 75;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=3
                    error = encode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=2; START (Transforms), END Element
            if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=3
                    error = encode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
static int encode_din_X509DataType(exi_bitstream_t* stream, const struct din_X509DataType* X509DataType) {
    int grammar_id = 76;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 76:
            // Grammar: ID=76; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            if (X509DataType->X509IssuerSerial_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerial, X509IssuerSerialType); next=3
                    error = encode_din_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (X509DataType->X509SKI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SKI, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509SKI.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509SKI.bytesLen, X509DataType->X509SKI.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509SubjectName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SubjectName, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509DataType->X509SubjectName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, din_X509SubjectName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509Certificate_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Certificate, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509Certificate.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509Certificate.bytesLen, X509DataType->X509Certificate.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->X509CRL_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509CRL, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509CRL.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509CRL.bytesLen, X509DataType->X509CRL.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (X509DataType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->ANY.bytesLen, X509DataType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}PGPData; type={http://www.w3.org/2000/09/xmldsig#}PGPDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True; sequence=True (2;
// Particle: PGPKeyID, base64Binary (1, 1); PGPKeyPacket, base64Binary (0, 1); ANY, anyType (0, 1); PGPKeyPacket, base64Binary (1, 1); ANY, anyType (0, 1);
static int encode_din_PGPDataType(exi_bitstream_t* stream, const struct din_PGPDataType* PGPDataType) {
    int grammar_id = 77;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyID, base64Binary); next=78
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyID.bytesLen, PGPDataType->choice_1.PGPKeyID.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 78;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=79
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 79;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=79
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, din_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 79;
                                }
                            }
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=80
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 80;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=80
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 80;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=81
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.PGPKeyPacket.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.PGPKeyPacket.bytesLen, PGPDataType->choice_2.PGPKeyPacket.bytes, din_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 81;
                            }
                        }
                    }
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (PGPDataType->choice_2_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=80
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.ANY.bytesLen, PGPDataType->choice_2.ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 80;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SPKIData; type={http://www.w3.org/2000/09/xmldsig#}SPKIDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SPKISexp, base64Binary (1, 1); ANY, anyType (0, 1);
static int encode_din_SPKIDataType(exi_bitstream_t* stream, const struct din_SPKIDataType* SPKIDataType) {
    int grammar_id = 82;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 82:
            // Grammar: ID=82; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=83
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->SPKISexp.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SPKIDataType->SPKISexp.bytesLen, SPKIDataType->SPKISexp.bytes, din_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 83;
                            }
                        }
                    }
                }
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (SPKIDataType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SPKIDataType->ANY.bytesLen, SPKIDataType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignedInfo; type={http://www.w3.org/2000/09/xmldsig#}SignedInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 1) (original max 4);
static int encode_din_SignedInfoType(exi_bitstream_t* stream, const struct din_SignedInfoType* SignedInfoType) {
    int grammar_id = 84;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 84:
            // Grammar: ID=84; read/write bits=2; START (Id), START (CanonicalizationMethod)
            if (SignedInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=85

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 85;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType); next=86
                    error = encode_din_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 86;
                    }
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CanonicalizationMethodType); next=86
                error = encode_din_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 86;
                }
            }
            break;
        case 86:
            // Grammar: ID=86; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignatureMethodType); next=87
                error = encode_din_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 87;
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=1; START (Reference)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ReferenceType); next=88
                error = encode_din_ReferenceType(stream, &SignedInfoType->Reference);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 88;
                }
            }
            break;
        case 88:
            // Grammar: ID=88; read/write bits=2; START (Reference), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Reference, ReferenceType); next=3
                    error = encode_din_ReferenceType(stream, &SignedInfoType->Reference);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}DC_EVStatus; type={urn:din:70121:2012:MsgDataTypes}DC_EVStatusType; base type=EVStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVReady, boolean (1, 1); EVCabinConditioning, boolean (0, 1); EVRESSConditioning, boolean (0, 1); EVErrorCode, DC_EVErrorCodeType (1, 1); EVRESSSOC, percentValueType (1, 1);
static int encode_din_DC_EVStatusType(exi_bitstream_t* stream, const struct din_DC_EVStatusType* DC_EVStatusType) {
    int grammar_id = 89;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 89:
            // Grammar: ID=89; read/write bits=1; START (EVReady)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=90
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, DC_EVStatusType->EVReady);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 90;
                        }
                    }
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=2; START (EVCabinConditioning), START (EVRESSConditioning), START (EVErrorCode)
            if (DC_EVStatusType->EVCabinConditioning_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVCabinConditioning, boolean); next=91
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DC_EVStatusType->EVCabinConditioning);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 91;
                            }
                        }
                    }
                }
            }
            else if (DC_EVStatusType->EVRESSConditioning_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVRESSConditioning, boolean); next=92
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DC_EVStatusType->EVRESSConditioning);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 92;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVErrorCode, string); next=93
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 4, DC_EVStatusType->EVErrorCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 93;
                            }
                        }
                    }
                }
            }
            break;
        case 91:
            // Grammar: ID=91; read/write bits=2; START (EVRESSConditioning), START (EVErrorCode)
            if (DC_EVStatusType->EVRESSConditioning_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVRESSConditioning, boolean); next=92
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DC_EVStatusType->EVRESSConditioning);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 92;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVErrorCode, string); next=93
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 4, DC_EVStatusType->EVErrorCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 93;
                            }
                        }
                    }
                }
            }
            break;
        case 92:
            // Grammar: ID=92; read/write bits=1; START (EVErrorCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=93
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 4, DC_EVStatusType->EVErrorCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 93;
                        }
                    }
                }
            }
            break;
        case 93:
            // Grammar: ID=93; read/write bits=1; START (EVRESSSOC)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (byte); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVStatusType->EVRESSSOC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
static int encode_din_SignatureValueType(exi_bitstream_t* stream, const struct din_SignatureValueType* SignatureValueType) {
    int grammar_id = 94;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 94:
            // Grammar: ID=94; read/write bits=2; START (Id), START (CONTENT)
            if (SignatureValueType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=95

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureValueType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 95;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CONTENT, base64Binary); next=3
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, din_SignatureValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 95:
            // Grammar: ID=95; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, din_SignatureValueType_BYTES_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractSignatureCertChain; type={urn:din:70121:2012:MsgDataTypes}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
static int encode_din_CertificateChainType(exi_bitstream_t* stream, const struct din_CertificateChainType* CertificateChainType) {
    int grammar_id = 96;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 96:
            // Grammar: ID=96; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=97
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateChainType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateChainType->Certificate.bytesLen, CertificateChainType->Certificate.bytes, din_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 97;
                            }
                        }
                    }
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=2; START (SubCertificates), END Element
            if (CertificateChainType->SubCertificates_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SubCertificates, SubCertificatesType); next=3
                    error = encode_din_SubCertificatesType(stream, &CertificateChainType->SubCertificates);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}DC_EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}DC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVSEIsolationStatus, isolationLevelType (0, 1); EVSEStatusCode, DC_EVSEStatusCodeType (1, 1); NotificationMaxDelay, unsignedInt (1, 1); EVSENotification, EVSENotificationType (1, 1);
static int encode_din_DC_EVSEStatusType(exi_bitstream_t* stream, const struct din_DC_EVSEStatusType* DC_EVSEStatusType) {
    int grammar_id = 98;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 98:
            // Grammar: ID=98; read/write bits=2; START (EVSEIsolationStatus), START (EVSEStatusCode)
            if (DC_EVSEStatusType->EVSEIsolationStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEIsolationStatus, string); next=99
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, DC_EVSEStatusType->EVSEIsolationStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 99;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEStatusCode, string); next=100
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 4, DC_EVSEStatusType->EVSEStatusCode);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 100;
                            }
                        }
                    }
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=1; START (EVSEStatusCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=100
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 4, DC_EVSEStatusType->EVSEStatusCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 100;
                        }
                    }
                }
            }
            break;
        case 100:
            // Grammar: ID=100; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=101
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, DC_EVSEStatusType->NotificationMaxDelay);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 101;
                        }
                    }
                }
            }
            break;
        case 101:
            // Grammar: ID=101; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, DC_EVSEStatusType->EVSENotification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}EVTargetVoltage; type={urn:din:70121:2012:MsgDataTypes}PhysicalValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Multiplier, unitMultiplierType (1, 1); Unit, unitSymbolType (0, 1); Value, short (1, 1);
static int encode_din_PhysicalValueType(exi_bitstream_t* stream, const struct din_PhysicalValueType* PhysicalValueType) {
    int grammar_id = 102;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=1; START (Multiplier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (byte); next=103
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, (uint32_t)PhysicalValueType->Multiplier - -3);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 103;
                        }
                    }
                }
            }
            break;
        case 103:
            // Grammar: ID=103; read/write bits=2; START (Unit), START (Value)
            if (PhysicalValueType->Unit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Unit, string); next=104
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 4, PhysicalValueType->Unit);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 104;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Value, int); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, PhysicalValueType->Value);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 104:
            // Grammar: ID=104; read/write bits=1; START (Value)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, PhysicalValueType->Value);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Parameter; type={urn:din:70121:2012:MsgDataTypes}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, string (1, 1); ValueType, valueType (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); physicalValue, PhysicalValueType (0, 1); stringValue, string (0, 1);
static int encode_din_ParameterType(exi_bitstream_t* stream, const struct din_ParameterType* ParameterType) {
    int grammar_id = 105;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 105:
            // Grammar: ID=105; read/write bits=1; START (Name)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=106

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ParameterType->Name.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, ParameterType->Name.charactersLen, ParameterType->Name.characters, din_Name_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 106;
                    }
                }
            }
            break;
        case 106:
            // Grammar: ID=106; read/write bits=1; START (ValueType)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=107

                error = exi_basetypes_encoder_nbit_uint(stream, 3, ParameterType->ValueType);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 107;
                }
            }
            break;
        case 107:
            // Grammar: ID=107; read/write bits=3; START (boolValue), START (byteValue), START (shortValue), START (intValue), START (physicalValue), START (stringValue)
            if (ParameterType->boolValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (boolValue, boolean); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, ParameterType->boolValue);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else if (ParameterType->byteValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (byteValue, short); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // type has min_value = -128
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, ParameterType->byteValue + -128);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else if (ParameterType->shortValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (shortValue, int); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, ParameterType->shortValue);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else if (ParameterType->intValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (intValue, long); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_32(stream, ParameterType->intValue);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else if (ParameterType->physicalValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (physicalValue, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &ParameterType->physicalValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (stringValue, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ParameterType->stringValue.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ParameterType->stringValue.charactersLen, ParameterType->stringValue.characters, din_stringValue_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ParameterSet; type={urn:din:70121:2012:MsgDataTypes}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, short (1, 1); Parameter, ParameterType (1, 1) (original max unbounded);
static int encode_din_ParameterSetType(exi_bitstream_t* stream, const struct din_ParameterSetType* ParameterSetType) {
    int grammar_id = 108;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 108:
            // Grammar: ID=108; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=109
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, ParameterSetType->ParameterSetID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 109;
                        }
                    }
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=1; START (Parameter)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ParameterType); next=110
                error = encode_din_ParameterType(stream, &ParameterSetType->Parameter);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 110;
                }
            }
            break;
        case 110:
            // Grammar: ID=110; read/write bits=2; START (Parameter), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Parameter, ParameterType); next=3
                    error = encode_din_ParameterType(stream, &ParameterSetType->Parameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ListOfRootCertificateIDs; type={urn:din:70121:2012:MsgDataTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, rootCertificateIDType (1, 5) (original max unbounded);
static int encode_din_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    int grammar_id = 111;
    int done = 0;
    int error = 0;
    uint16_t RootCertificateID_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 111:
            // Grammar: ID=111; read/write bits=1; START (RootCertificateID)
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=112

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].charactersLen, ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].characters, din_RootCertificateID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                RootCertificateID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 112;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=2; LOOP (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=112

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].charactersLen, ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex].characters, din_RootCertificateID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                RootCertificateID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 112;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentOptions; type={urn:din:70121:2012:MsgDataTypes}PaymentOptionsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PaymentOption, paymentOptionType (1, 2) (original max unbounded);
static int encode_din_PaymentOptionsType(exi_bitstream_t* stream, const struct din_PaymentOptionsType* PaymentOptionsType) {
    int grammar_id = 113;
    int done = 0;
    int error = 0;
    uint16_t PaymentOption_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 113:
            // Grammar: ID=113; read/write bits=1; START (PaymentOption)
            if (PaymentOption_currentIndex < PaymentOptionsType->PaymentOption.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=114
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PaymentOptionsType->PaymentOption.array[PaymentOption_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 114;
                            }
                        }
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=2; START (PaymentOption), END Element
            if (PaymentOption_currentIndex < PaymentOptionsType->PaymentOption.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PaymentOptionsType->PaymentOption.array[PaymentOption_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SelectedServiceList; type={urn:din:70121:2012:MsgDataTypes}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16) (original max unbounded);
static int encode_din_SelectedServiceListType(exi_bitstream_t* stream, const struct din_SelectedServiceListType* SelectedServiceListType) {
    int grammar_id = 115;
    int done = 0;
    int error = 0;
    uint16_t SelectedService_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 115:
            // Grammar: ID=115; read/write bits=1; START (SelectedService)
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=116
                    error = encode_din_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 116;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=2; LOOP (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (SelectedServiceType); next=116
                    error = encode_din_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 116;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}AC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (1, 1); EAmount, PhysicalValueType (1, 1); EVMaxVoltage, PhysicalValueType (1, 1); EVMaxCurrent, PhysicalValueType (1, 1); EVMinCurrent, PhysicalValueType (1, 1);
static int encode_din_AC_EVChargeParameterType(exi_bitstream_t* stream, const struct din_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    int grammar_id = 117;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 117:
            // Grammar: ID=117; read/write bits=1; START (DepartureTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=118
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, AC_EVChargeParameterType->DepartureTime);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 118;
                        }
                    }
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=1; START (EAmount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=119
                error = encode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 119;
                }
            }
            break;
        case 119:
            // Grammar: ID=119; read/write bits=1; START (EVMaxVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=120
                error = encode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 120;
                }
            }
            break;
        case 120:
            // Grammar: ID=120; read/write bits=1; START (EVMaxCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=121
                error = encode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 121;
                }
            }
            break;
        case 121:
            // Grammar: ID=121; read/write bits=1; START (EVMinCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMinCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVMaximumCurrentLimit, PhysicalValueType (1, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); EVMaximumVoltageLimit, PhysicalValueType (1, 1); EVEnergyCapacity, PhysicalValueType (0, 1); EVEnergyRequest, PhysicalValueType (0, 1); FullSOC, percentValueType (0, 1); BulkSOC, percentValueType (0, 1);
static int encode_din_DC_EVChargeParameterType(exi_bitstream_t* stream, const struct din_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    int grammar_id = 122;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 122:
            // Grammar: ID=122; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=123
                error = encode_din_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 123;
                }
            }
            break;
        case 123:
            // Grammar: ID=123; read/write bits=1; START (EVMaximumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=124
                error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 124;
                }
            }
            break;
        case 124:
            // Grammar: ID=124; read/write bits=2; START (EVMaximumPowerLimit), START (EVMaximumVoltageLimit)
            if (DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=125
                    error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 125;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType); next=126
                    error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 126;
                    }
                }
            }
            break;
        case 125:
            // Grammar: ID=125; read/write bits=1; START (EVMaximumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=126
                error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 126;
                }
            }
            break;
        case 126:
            // Grammar: ID=126; read/write bits=3; START (EVEnergyCapacity), START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->EVEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyCapacity, PhysicalValueType); next=127
                    error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 127;
                    }
                }
            }
            else if (DC_EVChargeParameterType->EVEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyRequest, PhysicalValueType); next=128
                    error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 128;
                    }
                }
            }
            else if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=129
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->FullSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 129;
                            }
                        }
                    }
                }
            }
            else if (DC_EVChargeParameterType->BulkSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkSOC, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->BulkSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 127:
            // Grammar: ID=127; read/write bits=3; START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->EVEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyRequest, PhysicalValueType); next=128
                    error = encode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 128;
                    }
                }
            }
            else if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=129
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->FullSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 129;
                            }
                        }
                    }
                }
            }
            else if (DC_EVChargeParameterType->BulkSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkSOC, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->BulkSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 128:
            // Grammar: ID=128; read/write bits=2; START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=129
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->FullSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 129;
                            }
                        }
                    }
                }
            }
            else if (DC_EVChargeParameterType->BulkSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkSOC, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->BulkSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=2; START (BulkSOC), END Element
            if (DC_EVChargeParameterType->BulkSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkSOC, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)DC_EVChargeParameterType->BulkSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVChargeParameter; type={urn:din:70121:2012:MsgDataTypes}EVChargeParameterType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_EVChargeParameterType(exi_bitstream_t* stream, const struct din_EVChargeParameterType* EVChargeParameterType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVChargeParameterType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingProfile; type={urn:din:70121:2012:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); ProfileEntry, ProfileEntryType (1, 24) (original max unbounded);
static int encode_din_ChargingProfileType(exi_bitstream_t* stream, const struct din_ChargingProfileType* ChargingProfileType) {
    int grammar_id = 130;
    int done = 0;
    int error = 0;
    uint16_t ProfileEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 130:
            // Grammar: ID=130; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=131
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, ChargingProfileType->SAScheduleTupleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 131;
                        }
                    }
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=1; START (ProfileEntry)
            if (ProfileEntry_currentIndex < ChargingProfileType->ProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ProfileEntryType); next=132
                    error = encode_din_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ProfileEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 132;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 132:
            // Grammar: ID=132; read/write bits=2; LOOP (ProfileEntry), END Element
            if (ProfileEntry_currentIndex < ChargingProfileType->ProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ProfileEntryType); next=132
                    error = encode_din_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ProfileEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 132;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVSEStatus; type={urn:din:70121:2012:MsgDataTypes}EVSEStatusType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_EVSEStatusType(exi_bitstream_t* stream, const struct din_EVSEStatusType* EVSEStatusType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVSEStatusType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int encode_din_KeyInfoType(exi_bitstream_t* stream, const struct din_KeyInfoType* KeyInfoType) {
    int grammar_id = 133;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 133:
            // Grammar: ID=133; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=134

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 134;
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, din_KeyName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=3
                    error = encode_din_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=3
                    error = encode_din_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=3
                    error = encode_din_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=3
                    error = encode_din_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=3
                    error = encode_din_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, din_MgmtData_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 134:
            // Grammar: ID=134; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, din_KeyName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=3
                    error = encode_din_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=3
                    error = encode_din_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=3
                    error = encode_din_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=3
                    error = encode_din_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=3
                    error = encode_din_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, din_MgmtData_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeService; type={urn:din:70121:2012:MsgDataTypes}ServiceChargeType; base type=ServiceType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceTag, ServiceTagType (1, 1); FreeService, boolean (1, 1); EnergyTransferType, EVSESupportedEnergyTransferType (1, 1);
static int encode_din_ServiceChargeType(exi_bitstream_t* stream, const struct din_ServiceChargeType* ServiceChargeType) {
    int grammar_id = 135;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 135:
            // Grammar: ID=135; read/write bits=1; START (ServiceTag)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceTagType); next=136
                error = encode_din_ServiceTagType(stream, &ServiceChargeType->ServiceTag);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 136;
                }
            }
            break;
        case 136:
            // Grammar: ID=136; read/write bits=1; START (FreeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=137
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, ServiceChargeType->FreeService);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 137;
                        }
                    }
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (EnergyTransferType)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 4, ServiceChargeType->EnergyTransferType);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceParameterList; type={urn:din:70121:2012:MsgDataTypes}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5) (original max unbounded);
static int encode_din_ServiceParameterListType(exi_bitstream_t* stream, const struct din_ServiceParameterListType* ServiceParameterListType) {
    int grammar_id = 138;
    int done = 0;
    int error = 0;
    uint16_t ParameterSet_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 138:
            // Grammar: ID=138; read/write bits=1; START (ParameterSet)
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=139
                    error = encode_din_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 139;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 139:
            // Grammar: ID=139; read/write bits=2; LOOP (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ParameterSetType); next=139
                    error = encode_din_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 139;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SAScheduleList; type={urn:din:70121:2012:MsgDataTypes}SAScheduleListType; base type=SASchedulesType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SAScheduleTuple, SAScheduleTupleType (1, 5) (original max unbounded);
static int encode_din_SAScheduleListType(exi_bitstream_t* stream, const struct din_SAScheduleListType* SAScheduleListType) {
    int grammar_id = 140;
    int done = 0;
    int error = 0;
    uint16_t SAScheduleTuple_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 140:
            // Grammar: ID=140; read/write bits=1; START (SAScheduleTuple)
            if (SAScheduleTuple_currentIndex < SAScheduleListType->SAScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleTupleType); next=141
                    error = encode_din_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 141;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=2; LOOP (SAScheduleTuple), END Element
            if (SAScheduleTuple_currentIndex < SAScheduleListType->SAScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (SAScheduleTupleType); next=141
                    error = encode_din_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 141;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}SASchedules; type={urn:din:70121:2012:MsgDataTypes}SASchedulesType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_SASchedulesType(exi_bitstream_t* stream, const struct din_SASchedulesType* SASchedulesType) {
    // Element has no particles, so the function just encodes END Element
    (void)SASchedulesType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
static int encode_din_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    int grammar_id = 142;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 142:
            // Grammar: ID=142; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=143
                error = encode_din_DC_EVStatusType(stream, &DC_EVPowerDeliveryParameterType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 143;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            if (DC_EVPowerDeliveryParameterType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=144
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DC_EVPowerDeliveryParameterType->BulkChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 144;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, DC_EVPowerDeliveryParameterType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, DC_EVPowerDeliveryParameterType->ChargingComplete);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVPowerDeliveryParameter; type={urn:din:70121:2012:MsgDataTypes}EVPowerDeliveryParameterType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVPowerDeliveryParameterType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int encode_din_ObjectType(exi_bitstream_t* stream, const struct din_ObjectType* ObjectType) {
    int grammar_id = 145;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 145:
            // Grammar: ID=145; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Encoding_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Encoding, anyURI); next=146

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Encoding.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, din_Encoding_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 146;
                        }
                    }
                }
            }
            else if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=147

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 147;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=148

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 148;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=3)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 146:
            // Grammar: ID=146; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=147

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 147;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=148

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 148;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=2)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 147:
            // Grammar: ID=147; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=148

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 148;
                        }
                    }
                }
            }
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=1)
            //{
            // ***** //
            else if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 148:
            // Grammar: ID=148; read/write bits=2; START (ANY), END Element, START (ANY)
            // ***** //
            //{
                // No code for unsupported generic event: ANY (index=0)
            //{
            // ***** //
            if (ObjectType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, din_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceList; type={urn:din:70121:2012:MsgDataTypes}ServiceTagListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 1) (original max unbounded);
static int encode_din_ServiceTagListType(exi_bitstream_t* stream, const struct din_ServiceTagListType* ServiceTagListType) {
    int grammar_id = 149;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 149:
            // Grammar: ID=149; read/write bits=1; START (Service)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceType); next=150
                error = encode_din_ServiceType(stream, &ServiceTagListType->Service);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 150;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=2; START (Service), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Service, ServiceType); next=3
                    error = encode_din_ServiceType(stream, &ServiceTagListType->Service);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEMaximumCurrentLimit, PhysicalValueType (1, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1); EVSEMaximumVoltageLimit, PhysicalValueType (1, 1); EVSEMinimumCurrentLimit, PhysicalValueType (1, 1); EVSEMinimumVoltageLimit, PhysicalValueType (1, 1); EVSECurrentRegulationTolerance, PhysicalValueType (0, 1); EVSEPeakCurrentRipple, PhysicalValueType (1, 1); EVSEEnergyToBeDelivered, PhysicalValueType (0, 1);
static int encode_din_DC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    int grammar_id = 151;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=152
                error = encode_din_DC_EVSEStatusType(stream, &DC_EVSEChargeParameterType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 152;
                }
            }
            break;
        case 152:
            // Grammar: ID=152; read/write bits=1; START (EVSEMaximumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=153
                error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 153;
                }
            }
            break;
        case 153:
            // Grammar: ID=153; read/write bits=2; START (EVSEMaximumPowerLimit), START (EVSEMaximumVoltageLimit)
            if (DC_EVSEChargeParameterType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=154
                    error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 154;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType); next=155
                    error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 155;
                    }
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=1; START (EVSEMaximumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=155
                error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 155;
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=1; START (EVSEMinimumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=156
                error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 156;
                }
            }
            break;
        case 156:
            // Grammar: ID=156; read/write bits=1; START (EVSEMinimumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=157
                error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 157;
                }
            }
            break;
        case 157:
            // Grammar: ID=157; read/write bits=2; START (EVSECurrentRegulationTolerance), START (EVSEPeakCurrentRipple)
            if (DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSECurrentRegulationTolerance, PhysicalValueType); next=158
                    error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 158;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType); next=159
                    error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 159;
                    }
                }
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=1; START (EVSEPeakCurrentRipple)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=159
                error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 159;
                }
            }
            break;
        case 159:
            // Grammar: ID=159; read/write bits=2; START (EVSEEnergyToBeDelivered), END Element
            if (DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEEnergyToBeDelivered, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}AC_EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}AC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: AC_EVSEStatus, AC_EVSEStatusType (1, 1); EVSEMaxVoltage, PhysicalValueType (1, 1); EVSEMaxCurrent, PhysicalValueType (1, 1); EVSEMinCurrent, PhysicalValueType (1, 1);
static int encode_din_AC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    int grammar_id = 160;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 160:
            // Grammar: ID=160; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=161
                error = encode_din_AC_EVSEStatusType(stream, &AC_EVSEChargeParameterType->AC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 161;
                }
            }
            break;
        case 161:
            // Grammar: ID=161; read/write bits=1; START (EVSEMaxVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=162
                error = encode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 162;
                }
            }
            break;
        case 162:
            // Grammar: ID=162; read/write bits=1; START (EVSEMaxCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=163
                error = encode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 163;
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=1; START (EVSEMinCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMinCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}EVSEChargeParameter; type={urn:din:70121:2012:MsgDataTypes}EVSEChargeParameterType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_EVSEChargeParameterType(exi_bitstream_t* stream, const struct din_EVSEChargeParameterType* EVSEChargeParameterType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVSEChargeParameterType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeterInfo; type={urn:din:70121:2012:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, PhysicalValueType (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
static int encode_din_MeterInfoType(exi_bitstream_t* stream, const struct din_MeterInfoType* MeterInfoType) {
    int grammar_id = 164;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 164:
            // Grammar: ID=164; read/write bits=1; START (MeterID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=165

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeterInfoType->MeterID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, din_MeterID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 165;
                            }
                        }
                    }
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=3; START (MeterReading), START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->MeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterReading, PhysicalValueType); next=166
                    error = encode_din_PhysicalValueType(stream, &MeterInfoType->MeterReading);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 166;
                    }
                }
            }
            else if (MeterInfoType->SigMeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SigMeterReading, base64Binary); next=167
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->SigMeterReading.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->SigMeterReading.bytesLen, MeterInfoType->SigMeterReading.bytes, din_sigMeterReadingType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 167;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=168
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 168;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->TMeter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TMeter, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, MeterInfoType->TMeter);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 166:
            // Grammar: ID=166; read/write bits=3; START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->SigMeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SigMeterReading, base64Binary); next=167
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->SigMeterReading.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->SigMeterReading.bytesLen, MeterInfoType->SigMeterReading.bytes, din_sigMeterReadingType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 167;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=168
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 168;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->TMeter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TMeter, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, MeterInfoType->TMeter);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=2; START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=168
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeterInfoType->MeterStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 168;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->TMeter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TMeter, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, MeterInfoType->TMeter);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=2; START (TMeter), END Element
            if (MeterInfoType->TMeter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TMeter, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, MeterInfoType->TMeter);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateInstallationRes; type={urn:din:70121:2012:MsgBody}CertificateInstallationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (1, 1); ResponseCode, responseCodeType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, privateKeyType (1, 1); DHParams, dHParamsType (1, 1); ContractID, contractIDType (1, 1);
static int encode_din_CertificateInstallationResType(exi_bitstream_t* stream, const struct din_CertificateInstallationResType* CertificateInstallationResType) {
    int grammar_id = 169;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 169:
            // Grammar: ID=169; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=170

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateInstallationResType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CertificateInstallationResType->Id.charactersLen, CertificateInstallationResType->Id.characters, din_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 170;
                    }
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=171
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, CertificateInstallationResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 171;
                        }
                    }
                }
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=172
                error = encode_din_CertificateChainType(stream, &CertificateInstallationResType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 172;
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=173
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationResType->ContractSignatureEncryptedPrivateKey.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateInstallationResType->ContractSignatureEncryptedPrivateKey.bytesLen, CertificateInstallationResType->ContractSignatureEncryptedPrivateKey.bytes, din_privateKeyType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 173;
                            }
                        }
                    }
                }
            }
            break;
        case 173:
            // Grammar: ID=173; read/write bits=1; START (DHParams)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=174
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationResType->DHParams.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateInstallationResType->DHParams.bytesLen, CertificateInstallationResType->DHParams.bytes, din_dHParamsType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 174;
                            }
                        }
                    }
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=1; START (ContractID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateInstallationResType->ContractID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateInstallationResType->ContractID.charactersLen, CertificateInstallationResType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CableCheckReq; type={urn:din:70121:2012:MsgBody}CableCheckReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int encode_din_CableCheckReqType(exi_bitstream_t* stream, const struct din_CableCheckReqType* CableCheckReqType) {
    int grammar_id = 175;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=3
                error = encode_din_DC_EVStatusType(stream, &CableCheckReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CableCheckRes; type={urn:din:70121:2012:MsgBody}CableCheckResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int encode_din_CableCheckResType(exi_bitstream_t* stream, const struct din_CableCheckResType* CableCheckResType) {
    int grammar_id = 176;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 176:
            // Grammar: ID=176; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=177
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, CableCheckResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 177;
                        }
                    }
                }
            }
            break;
        case 177:
            // Grammar: ID=177; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=178
                error = encode_din_DC_EVSEStatusType(stream, &CableCheckResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 178;
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, CableCheckResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PreChargeReq; type={urn:din:70121:2012:MsgBody}PreChargeReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetVoltage, PhysicalValueType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1);
static int encode_din_PreChargeReqType(exi_bitstream_t* stream, const struct din_PreChargeReqType* PreChargeReqType) {
    int grammar_id = 179;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 179:
            // Grammar: ID=179; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=180
                error = encode_din_DC_EVStatusType(stream, &PreChargeReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 180;
                }
            }
            break;
        case 180:
            // Grammar: ID=180; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=181
                error = encode_din_PhysicalValueType(stream, &PreChargeReqType->EVTargetVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 181;
                }
            }
            break;
        case 181:
            // Grammar: ID=181; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &PreChargeReqType->EVTargetCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PreChargeRes; type={urn:din:70121:2012:MsgBody}PreChargeResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int encode_din_PreChargeResType(exi_bitstream_t* stream, const struct din_PreChargeResType* PreChargeResType) {
    int grammar_id = 182;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 182:
            // Grammar: ID=182; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=183
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, PreChargeResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 183;
                        }
                    }
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=184
                error = encode_din_DC_EVSEStatusType(stream, &PreChargeResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 184;
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &PreChargeResType->EVSEPresentVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CurrentDemandReq; type={urn:din:70121:2012:MsgBody}CurrentDemandReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1); EVMaximumVoltageLimit, PhysicalValueType (0, 1); EVMaximumCurrentLimit, PhysicalValueType (0, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1); RemainingTimeToFullSoC, PhysicalValueType (0, 1); RemainingTimeToBulkSoC, PhysicalValueType (0, 1); EVTargetVoltage, PhysicalValueType (1, 1);
static int encode_din_CurrentDemandReqType(exi_bitstream_t* stream, const struct din_CurrentDemandReqType* CurrentDemandReqType) {
    int grammar_id = 185;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 185:
            // Grammar: ID=185; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=186
                error = encode_din_DC_EVStatusType(stream, &CurrentDemandReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 186;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=187
                error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 187;
                }
            }
            break;
        case 187:
            // Grammar: ID=187; read/write bits=3; START (EVMaximumVoltageLimit), START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumVoltageLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType); next=188
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 188;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType); next=189
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 189;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=190
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 190;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->BulkChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 191;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 192;
                            }
                        }
                    }
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=3; START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType); next=189
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 189;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=190
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 190;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->BulkChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 191;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 192;
                            }
                        }
                    }
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=2; START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=190
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 190;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->BulkChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 191;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 192;
                            }
                        }
                    }
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->BulkChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 191;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingComplete, boolean); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->ChargingComplete);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 192;
                            }
                        }
                    }
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=192
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, CurrentDemandReqType->ChargingComplete);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 192;
                        }
                    }
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=2; START (RemainingTimeToFullSoC), START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            if (CurrentDemandReqType->RemainingTimeToFullSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToFullSoC, PhysicalValueType); next=193
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToFullSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 193;
                    }
                }
            }
            else if (CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType); next=194
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 194;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetVoltage, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 193:
            // Grammar: ID=193; read/write bits=2; START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            if (CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType); next=194
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 194;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetVoltage, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 194:
            // Grammar: ID=194; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CurrentDemandRes; type={urn:din:70121:2012:MsgBody}CurrentDemandResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1); EVSEPresentCurrent, PhysicalValueType (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (0, 1); EVSEMaximumCurrentLimit, PhysicalValueType (0, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1);
static int encode_din_CurrentDemandResType(exi_bitstream_t* stream, const struct din_CurrentDemandResType* CurrentDemandResType) {
    int grammar_id = 195;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 195:
            // Grammar: ID=195; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=196
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, CurrentDemandResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 196;
                        }
                    }
                }
            }
            break;
        case 196:
            // Grammar: ID=196; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=197
                error = encode_din_DC_EVSEStatusType(stream, &CurrentDemandResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 197;
                }
            }
            break;
        case 197:
            // Grammar: ID=197; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=198
                error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 198;
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=1; START (EVSEPresentCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=199
                error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 199;
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=1; START (EVSECurrentLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=200
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, CurrentDemandResType->EVSECurrentLimitAchieved);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 200;
                        }
                    }
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=1; START (EVSEVoltageLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=201
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, CurrentDemandResType->EVSEVoltageLimitAchieved);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 201;
                        }
                    }
                }
            }
            break;
        case 201:
            // Grammar: ID=201; read/write bits=1; START (EVSEPowerLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=202
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, CurrentDemandResType->EVSEPowerLimitAchieved);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 202;
                        }
                    }
                }
            }
            break;
        case 202:
            // Grammar: ID=202; read/write bits=3; START (EVSEMaximumVoltageLimit), START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), END Element
            if (CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType); next=203
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 203;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType); next=204
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 204;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=2; START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), END Element
            if (CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType); next=204
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 204;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=2; START (EVSEMaximumPowerLimit), END Element
            if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=3
                    error = encode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}WeldingDetectionReq; type={urn:din:70121:2012:MsgBody}WeldingDetectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int encode_din_WeldingDetectionReqType(exi_bitstream_t* stream, const struct din_WeldingDetectionReqType* WeldingDetectionReqType) {
    int grammar_id = 205;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 205:
            // Grammar: ID=205; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=3
                error = encode_din_DC_EVStatusType(stream, &WeldingDetectionReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}WeldingDetectionRes; type={urn:din:70121:2012:MsgBody}WeldingDetectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int encode_din_WeldingDetectionResType(exi_bitstream_t* stream, const struct din_WeldingDetectionResType* WeldingDetectionResType) {
    int grammar_id = 206;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 206:
            // Grammar: ID=206; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=207
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, WeldingDetectionResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 207;
                        }
                    }
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=208
                error = encode_din_DC_EVSEStatusType(stream, &WeldingDetectionResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 208;
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_din_PhysicalValueType(stream, &WeldingDetectionResType->EVSEPresentVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionSetupReq; type={urn:din:70121:2012:MsgBody}SessionSetupReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVCCID, evccIDType (1, 1);
static int encode_din_SessionSetupReqType(exi_bitstream_t* stream, const struct din_SessionSetupReqType* SessionSetupReqType) {
    int grammar_id = 209;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 209:
            // Grammar: ID=209; read/write bits=1; START (EVCCID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SessionSetupReqType->EVCCID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SessionSetupReqType->EVCCID.bytesLen, SessionSetupReqType->EVCCID.bytes, din_evccIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateInstallationReq; type={urn:din:70121:2012:MsgBody}CertificateInstallationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); OEMProvisioningCert, certificateType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); DHParams, dHParamsType (1, 1);
static int encode_din_CertificateInstallationReqType(exi_bitstream_t* stream, const struct din_CertificateInstallationReqType* CertificateInstallationReqType) {
    int grammar_id = 210;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 210:
            // Grammar: ID=210; read/write bits=2; START (Id), START (OEMProvisioningCert)
            if (CertificateInstallationReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=211

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateInstallationReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateInstallationReqType->Id.charactersLen, CertificateInstallationReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 211;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OEMProvisioningCert, base64Binary); next=212
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationReqType->OEMProvisioningCert.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, CertificateInstallationReqType->OEMProvisioningCert.bytesLen, CertificateInstallationReqType->OEMProvisioningCert.bytes, din_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 212;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 211:
            // Grammar: ID=211; read/write bits=1; START (OEMProvisioningCert)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=212
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationReqType->OEMProvisioningCert.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateInstallationReqType->OEMProvisioningCert.bytesLen, CertificateInstallationReqType->OEMProvisioningCert.bytes, din_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 212;
                            }
                        }
                    }
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ListOfRootCertificateIDsType); next=213
                error = encode_din_ListOfRootCertificateIDsType(stream, &CertificateInstallationReqType->ListOfRootCertificateIDs);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 213;
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=1; START (DHParams)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationReqType->DHParams.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateInstallationReqType->DHParams.bytesLen, CertificateInstallationReqType->DHParams.bytes, din_dHParamsType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionSetupRes; type={urn:din:70121:2012:MsgBody}SessionSetupResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); DateTimeNow, long (0, 1);
static int encode_din_SessionSetupResType(exi_bitstream_t* stream, const struct din_SessionSetupResType* SessionSetupResType) {
    int grammar_id = 214;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 214:
            // Grammar: ID=214; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=215
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, SessionSetupResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 215;
                        }
                    }
                }
            }
            break;
        case 215:
            // Grammar: ID=215; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=216
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SessionSetupResType->EVSEID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SessionSetupResType->EVSEID.bytesLen, SessionSetupResType->EVSEID.bytes, din_evseIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 216;
                            }
                        }
                    }
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=2; START (DateTimeNow), END Element
            if (SessionSetupResType->DateTimeNow_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DateTimeNow, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, SessionSetupResType->DateTimeNow);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDiscoveryReq; type={urn:din:70121:2012:MsgBody}ServiceDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceScope, serviceScopeType (0, 1); ServiceCategory, serviceCategoryType (0, 1);
static int encode_din_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    int grammar_id = 217;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 217:
            // Grammar: ID=217; read/write bits=2; START (ServiceScope), START (ServiceCategory), END Element
            if (ServiceDiscoveryReqType->ServiceScope_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceScope, string); next=218

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceDiscoveryReqType->ServiceScope.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceDiscoveryReqType->ServiceScope.charactersLen, ServiceDiscoveryReqType->ServiceScope.characters, din_ServiceScope_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 218;
                                }
                            }
                        }
                    }
                }
            }
            else if (ServiceDiscoveryReqType->ServiceCategory_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceCategory, string); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceDiscoveryReqType->ServiceCategory);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 218:
            // Grammar: ID=218; read/write bits=2; START (ServiceCategory), END Element
            if (ServiceDiscoveryReqType->ServiceCategory_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceCategory, string); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceDiscoveryReqType->ServiceCategory);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDiscoveryRes; type={urn:din:70121:2012:MsgBody}ServiceDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); PaymentOptions, PaymentOptionsType (1, 1); ChargeService, ServiceChargeType (1, 1); ServiceList, ServiceTagListType (0, 1);
static int encode_din_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct din_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    int grammar_id = 219;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 219:
            // Grammar: ID=219; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=220
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ServiceDiscoveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 220;
                        }
                    }
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=1; START (PaymentOptions)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PaymentOptionsType); next=221
                error = encode_din_PaymentOptionsType(stream, &ServiceDiscoveryResType->PaymentOptions);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 221;
                }
            }
            break;
        case 221:
            // Grammar: ID=221; read/write bits=1; START (ChargeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceType); next=222
                error = encode_din_ServiceChargeType(stream, &ServiceDiscoveryResType->ChargeService);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 222;
                }
            }
            break;
        case 222:
            // Grammar: ID=222; read/write bits=2; START (ServiceList), END Element
            if (ServiceDiscoveryResType->ServiceList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceList, ServiceTagListType); next=3
                    error = encode_din_ServiceTagListType(stream, &ServiceDiscoveryResType->ServiceList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDetailReq; type={urn:din:70121:2012:MsgBody}ServiceDetailReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1);
static int encode_din_ServiceDetailReqType(exi_bitstream_t* stream, const struct din_ServiceDetailReqType* ServiceDetailReqType) {
    int grammar_id = 223;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 223:
            // Grammar: ID=223; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ServiceDetailReqType->ServiceID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServiceDetailRes; type={urn:din:70121:2012:MsgBody}ServiceDetailResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (0, 1);
static int encode_din_ServiceDetailResType(exi_bitstream_t* stream, const struct din_ServiceDetailResType* ServiceDetailResType) {
    int grammar_id = 224;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 224:
            // Grammar: ID=224; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=225
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ServiceDetailResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 225;
                        }
                    }
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=226
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ServiceDetailResType->ServiceID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 226;
                        }
                    }
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=2; START (ServiceParameterList), END Element
            if (ServiceDetailResType->ServiceParameterList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceParameterList, ServiceParameterListType); next=3
                    error = encode_din_ServiceParameterListType(stream, &ServiceDetailResType->ServiceParameterList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServicePaymentSelectionReq; type={urn:din:70121:2012:MsgBody}ServicePaymentSelectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SelectedPaymentOption, paymentOptionType (1, 1); SelectedServiceList, SelectedServiceListType (1, 1);
static int encode_din_ServicePaymentSelectionReqType(exi_bitstream_t* stream, const struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType) {
    int grammar_id = 227;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (SelectedPaymentOption)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=228
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, ServicePaymentSelectionReqType->SelectedPaymentOption);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 228;
                        }
                    }
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=1; START (SelectedServiceList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SelectedServiceListType); next=3
                error = encode_din_SelectedServiceListType(stream, &ServicePaymentSelectionReqType->SelectedServiceList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ServicePaymentSelectionRes; type={urn:din:70121:2012:MsgBody}ServicePaymentSelectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int encode_din_ServicePaymentSelectionResType(exi_bitstream_t* stream, const struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType) {
    int grammar_id = 229;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 229:
            // Grammar: ID=229; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ServicePaymentSelectionResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentDetailsReq; type={urn:din:70121:2012:MsgBody}PaymentDetailsReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ContractID, contractIDType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1);
static int encode_din_PaymentDetailsReqType(exi_bitstream_t* stream, const struct din_PaymentDetailsReqType* PaymentDetailsReqType) {
    int grammar_id = 230;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 230:
            // Grammar: ID=230; read/write bits=1; START (ContractID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=231

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PaymentDetailsReqType->ContractID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, PaymentDetailsReqType->ContractID.charactersLen, PaymentDetailsReqType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 231;
                            }
                        }
                    }
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=3
                error = encode_din_CertificateChainType(stream, &PaymentDetailsReqType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PaymentDetailsRes; type={urn:din:70121:2012:MsgBody}PaymentDetailsResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); GenChallenge, genChallengeType (1, 1); DateTimeNow, long (1, 1);
static int encode_din_PaymentDetailsResType(exi_bitstream_t* stream, const struct din_PaymentDetailsResType* PaymentDetailsResType) {
    int grammar_id = 232;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 232:
            // Grammar: ID=232; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=233
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, PaymentDetailsResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 233;
                        }
                    }
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=234

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PaymentDetailsResType->GenChallenge.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, PaymentDetailsResType->GenChallenge.charactersLen, PaymentDetailsResType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 234;
                            }
                        }
                    }
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=1; START (DateTimeNow)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (integer); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_64(stream, PaymentDetailsResType->DateTimeNow);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractAuthenticationReq; type={urn:din:70121:2012:MsgBody}ContractAuthenticationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); GenChallenge, genChallengeType (0, 1);
static int encode_din_ContractAuthenticationReqType(exi_bitstream_t* stream, const struct din_ContractAuthenticationReqType* ContractAuthenticationReqType) {
    int grammar_id = 235;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 235:
            // Grammar: ID=235; read/write bits=2; START (Id), START (GenChallenge), END Element
            if (ContractAuthenticationReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=236

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ContractAuthenticationReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ContractAuthenticationReqType->Id.charactersLen, ContractAuthenticationReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 236;
                        }
                    }
                }
            }
            else if (ContractAuthenticationReqType->GenChallenge_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (GenChallenge, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ContractAuthenticationReqType->GenChallenge.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ContractAuthenticationReqType->GenChallenge.charactersLen, ContractAuthenticationReqType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 236:
            // Grammar: ID=236; read/write bits=2; START (GenChallenge), END Element
            if (ContractAuthenticationReqType->GenChallenge_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (GenChallenge, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ContractAuthenticationReqType->GenChallenge.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ContractAuthenticationReqType->GenChallenge.charactersLen, ContractAuthenticationReqType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ContractAuthenticationRes; type={urn:din:70121:2012:MsgBody}ContractAuthenticationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int encode_din_ContractAuthenticationResType(exi_bitstream_t* stream, const struct din_ContractAuthenticationResType* ContractAuthenticationResType) {
    int grammar_id = 237;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 237:
            // Grammar: ID=237; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=238
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ContractAuthenticationResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 238;
                        }
                    }
                }
            }
            break;
        case 238:
            // Grammar: ID=238; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, ContractAuthenticationResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryReq; type={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVRequestedEnergyTransferType, EVRequestedEnergyTransferType (1, 1); AC_EVChargeParameter, AC_EVChargeParameterType (0, 1); DC_EVChargeParameter, DC_EVChargeParameterType (0, 1); EVChargeParameter, EVChargeParameterType (0, 1);
static int encode_din_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    int grammar_id = 239;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 239:
            // Grammar: ID=239; read/write bits=1; START (EVRequestedEnergyTransferType)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=240
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, ChargeParameterDiscoveryReqType->EVRequestedEnergyTransferType);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 240;
                        }
                    }
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=2; START (AC_EVChargeParameter), START (DC_EVChargeParameter), START (EVChargeParameter)
            if (ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVChargeParameter, EVChargeParameterType); next=3
                    error = encode_din_AC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->AC_EVChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (ChargeParameterDiscoveryReqType->DC_EVChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVChargeParameter, EVChargeParameterType); next=3
                    error = encode_din_DC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->DC_EVChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVChargeParameterType); next=3
                    error = encode_din_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->EVChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryRes; type={urn:din:70121:2012:MsgBody}ChargeParameterDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1); SAScheduleList, SAScheduleListType (0, 1); SASchedules, SASchedulesType (0, 1); AC_EVSEChargeParameter, AC_EVSEChargeParameterType (0, 1); DC_EVSEChargeParameter, DC_EVSEChargeParameterType (0, 1); EVSEChargeParameter, EVSEChargeParameterType (0, 1);
static int encode_din_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    int grammar_id = 241;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 241:
            // Grammar: ID=241; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=242
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ChargeParameterDiscoveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 242;
                        }
                    }
                }
            }
            break;
        case 242:
            // Grammar: ID=242; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=243
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, ChargeParameterDiscoveryResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 243;
                        }
                    }
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=2; START (SAScheduleList), START (SASchedules)
            if (ChargeParameterDiscoveryResType->SAScheduleList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleList, SASchedulesType); next=244
                    error = encode_din_SAScheduleListType(stream, &ChargeParameterDiscoveryResType->SAScheduleList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 244;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (SASchedulesType); next=244
                    error = encode_din_SASchedulesType(stream, &ChargeParameterDiscoveryResType->SASchedules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 244;
                    }
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=2; START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            if (ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEChargeParameter, EVSEChargeParameterType); next=3
                    error = encode_din_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVSEChargeParameter, EVSEChargeParameterType); next=3
                    error = encode_din_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVSEChargeParameterType); next=3
                    error = encode_din_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PowerDeliveryReq; type={urn:din:70121:2012:MsgBody}PowerDeliveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ReadyToChargeState, boolean (1, 1); ChargingProfile, ChargingProfileType (0, 1); DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (0, 1); EVPowerDeliveryParameter, EVPowerDeliveryParameterType (0, 1);
static int encode_din_PowerDeliveryReqType(exi_bitstream_t* stream, const struct din_PowerDeliveryReqType* PowerDeliveryReqType) {
    int grammar_id = 245;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 245:
            // Grammar: ID=245; read/write bits=1; START (ReadyToChargeState)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=246
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, PowerDeliveryReqType->ReadyToChargeState);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 246;
                        }
                    }
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=3; START (ChargingProfile), START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            if (PowerDeliveryReqType->ChargingProfile_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingProfile, ChargingProfileType); next=247
                    error = encode_din_ChargingProfileType(stream, &PowerDeliveryReqType->ChargingProfile);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 247;
                    }
                }
            }
            else if (PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVPowerDeliveryParameter, EVPowerDeliveryParameterType); next=3
                    error = encode_din_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVPowerDeliveryParameterType); next=3
                    error = encode_din_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 247:
            // Grammar: ID=247; read/write bits=2; START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            if (PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVPowerDeliveryParameter, EVPowerDeliveryParameterType); next=3
                    error = encode_din_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVPowerDeliveryParameterType); next=3
                    error = encode_din_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}PowerDeliveryRes; type={urn:din:70121:2012:MsgBody}PowerDeliveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
static int encode_din_PowerDeliveryResType(exi_bitstream_t* stream, const struct din_PowerDeliveryResType* PowerDeliveryResType) {
    int grammar_id = 248;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 248:
            // Grammar: ID=248; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=249
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, PowerDeliveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 249;
                        }
                    }
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            if (PowerDeliveryResType->AC_EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_din_AC_EVSEStatusType(stream, &PowerDeliveryResType->AC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (PowerDeliveryResType->DC_EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_din_DC_EVSEStatusType(stream, &PowerDeliveryResType->DC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVSEStatusType); next=3
                    error = encode_din_EVSEStatusType(stream, &PowerDeliveryResType->EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingStatusReq; type={urn:din:70121:2012:MsgBody}ChargingStatusReqType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
static int encode_din_ChargingStatusReqType(exi_bitstream_t* stream, const struct din_ChargingStatusReqType* ChargingStatusReqType) {
    // Element has no particles, so the function just encodes END Element
    (void)ChargingStatusReqType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingStatusRes; type={urn:din:70121:2012:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
static int encode_din_ChargingStatusResType(exi_bitstream_t* stream, const struct din_ChargingStatusResType* ChargingStatusResType) {
    int grammar_id = 250;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 250:
            // Grammar: ID=250; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=251
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, ChargingStatusResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 251;
                        }
                    }
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=252
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ChargingStatusResType->EVSEID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, ChargingStatusResType->EVSEID.bytesLen, ChargingStatusResType->EVSEID.bytes, din_evseIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 252;
                            }
                        }
                    }
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=253
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, ChargingStatusResType->SAScheduleTupleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 253;
                        }
                    }
                }
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=2; START (EVSEMaxCurrent), START (MeterInfo), START (ReceiptRequired)
            if (ChargingStatusResType->EVSEMaxCurrent_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaxCurrent, PhysicalValueType); next=254
                    error = encode_din_PhysicalValueType(stream, &ChargingStatusResType->EVSEMaxCurrent);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            else if (ChargingStatusResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=255
                    error = encode_din_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=256
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, ChargingStatusResType->ReceiptRequired);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 256;
                            }
                        }
                    }
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=2; START (MeterInfo), START (ReceiptRequired)
            if (ChargingStatusResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=255
                    error = encode_din_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=256
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, ChargingStatusResType->ReceiptRequired);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 256;
                            }
                        }
                    }
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=1; START (ReceiptRequired)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=256
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, ChargingStatusResType->ReceiptRequired);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 256;
                        }
                    }
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=3
                error = encode_din_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeteringReceiptReq; type={urn:din:70121:2012:MsgBody}MeteringReceiptReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); SessionID, sessionIDType (1, 1); SAScheduleTupleID, SAIDType (0, 1); MeterInfo, MeterInfoType (1, 1);
static int encode_din_MeteringReceiptReqType(exi_bitstream_t* stream, const struct din_MeteringReceiptReqType* MeteringReceiptReqType) {
    int grammar_id = 257;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 257:
            // Grammar: ID=257; read/write bits=2; START (Id), START (SessionID)
            if (MeteringReceiptReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=258

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeteringReceiptReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeteringReceiptReqType->Id.charactersLen, MeteringReceiptReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 258;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SessionID, hexBinary); next=259
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeteringReceiptReqType->SessionID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeteringReceiptReqType->SessionID.bytesLen, MeteringReceiptReqType->SessionID.bytes, din_sessionIDType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 259;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=259
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeteringReceiptReqType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MeteringReceiptReqType->SessionID.bytesLen, MeteringReceiptReqType->SessionID.bytes, din_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 259;
                            }
                        }
                    }
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=2; START (SAScheduleTupleID), START (MeterInfo)
            if (MeteringReceiptReqType->SAScheduleTupleID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleTupleID, short); next=260
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, MeteringReceiptReqType->SAScheduleTupleID);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 260;
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=3
                    error = encode_din_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=1; START (MeterInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MeterInfoType); next=3
                error = encode_din_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeteringReceiptRes; type={urn:din:70121:2012:MsgBody}MeteringReceiptResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
static int encode_din_MeteringReceiptResType(exi_bitstream_t* stream, const struct din_MeteringReceiptResType* MeteringReceiptResType) {
    int grammar_id = 261;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 261:
            // Grammar: ID=261; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=262
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, MeteringReceiptResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 262;
                        }
                    }
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=3
                error = encode_din_AC_EVSEStatusType(stream, &MeteringReceiptResType->AC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionStopReq; type={urn:din:70121:2012:MsgBody}SessionStopType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
static int encode_din_SessionStopType(exi_bitstream_t* stream, const struct din_SessionStopType* SessionStopType) {
    // Element has no particles, so the function just encodes END Element
    (void)SessionStopType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionStopRes; type={urn:din:70121:2012:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int encode_din_SessionStopResType(exi_bitstream_t* stream, const struct din_SessionStopResType* SessionStopResType) {
    int grammar_id = 263;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 263:
            // Grammar: ID=263; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, SessionStopResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateUpdateReq; type={urn:din:70121:2012:MsgBody}CertificateUpdateReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (0, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractID, contractIDType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); DHParams, dHParamsType (1, 1);
static int encode_din_CertificateUpdateReqType(exi_bitstream_t* stream, const struct din_CertificateUpdateReqType* CertificateUpdateReqType) {
    int grammar_id = 264;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 264:
            // Grammar: ID=264; read/write bits=2; START (Id), START (ContractSignatureCertChain)
            if (CertificateUpdateReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=265

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateUpdateReqType->Id.charactersLen, CertificateUpdateReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 265;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ContractSignatureCertChain, CertificateChainType); next=266
                    error = encode_din_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 266;
                    }
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=266
                error = encode_din_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 266;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (ContractID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=267

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateReqType->ContractID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateUpdateReqType->ContractID.charactersLen, CertificateUpdateReqType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 267;
                            }
                        }
                    }
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ListOfRootCertificateIDsType); next=268
                error = encode_din_ListOfRootCertificateIDsType(stream, &CertificateUpdateReqType->ListOfRootCertificateIDs);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 268;
                }
            }
            break;
        case 268:
            // Grammar: ID=268; read/write bits=1; START (DHParams)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateUpdateReqType->DHParams.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateUpdateReqType->DHParams.bytesLen, CertificateUpdateReqType->DHParams.bytes, din_dHParamsType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}CertificateUpdateRes; type={urn:din:70121:2012:MsgBody}CertificateUpdateResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, IDREF (1, 1); ResponseCode, responseCodeType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, privateKeyType (1, 1); DHParams, dHParamsType (1, 1); ContractID, contractIDType (1, 1); RetryCounter, short (1, 1);
static int encode_din_CertificateUpdateResType(exi_bitstream_t* stream, const struct din_CertificateUpdateResType* CertificateUpdateResType) {
    int grammar_id = 269;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 269:
            // Grammar: ID=269; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=270

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateResType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CertificateUpdateResType->Id.charactersLen, CertificateUpdateResType->Id.characters, din_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 270;
                    }
                }
            }
            break;
        case 270:
            // Grammar: ID=270; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=271
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, CertificateUpdateResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 271;
                        }
                    }
                }
            }
            break;
        case 271:
            // Grammar: ID=271; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=272
                error = encode_din_CertificateChainType(stream, &CertificateUpdateResType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 272;
                }
            }
            break;
        case 272:
            // Grammar: ID=272; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=273
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateUpdateResType->ContractSignatureEncryptedPrivateKey.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateUpdateResType->ContractSignatureEncryptedPrivateKey.bytesLen, CertificateUpdateResType->ContractSignatureEncryptedPrivateKey.bytes, din_privateKeyType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 273;
                            }
                        }
                    }
                }
            }
            break;
        case 273:
            // Grammar: ID=273; read/write bits=1; START (DHParams)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=274
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateUpdateResType->DHParams.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateUpdateResType->DHParams.bytesLen, CertificateUpdateResType->DHParams.bytes, din_dHParamsType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 274;
                            }
                        }
                    }
                }
            }
            break;
        case 274:
            // Grammar: ID=274; read/write bits=1; START (ContractID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=275

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateResType->ContractID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateUpdateResType->ContractID.charactersLen, CertificateUpdateResType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 275;
                            }
                        }
                    }
                }
            }
            break;
        case 275:
            // Grammar: ID=275; read/write bits=1; START (RetryCounter)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, CertificateUpdateResType->RetryCounter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}BodyElement; type={urn:din:70121:2012:MsgDef}BodyBaseType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_din_BodyBaseType(exi_bitstream_t* stream, const struct din_BodyBaseType* BodyBaseType) {
    // Element has no particles, so the function just encodes END Element
    (void)BodyBaseType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgHeader}Notification; type={urn:din:70121:2012:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
static int encode_din_NotificationType(exi_bitstream_t* stream, const struct din_NotificationType* NotificationType) {
    int grammar_id = 276;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 276:
            // Grammar: ID=276; read/write bits=1; START (FaultCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=277
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, NotificationType->FaultCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 277;
                        }
                    }
                }
            }
            break;
        case 277:
            // Grammar: ID=277; read/write bits=2; START (FaultMsg), END Element
            if (NotificationType->FaultMsg_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FaultMsg, string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(NotificationType->FaultMsg.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, NotificationType->FaultMsg.charactersLen, NotificationType->FaultMsg.characters, din_FaultMsg_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 3;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
static int encode_din_SignatureType(exi_bitstream_t* stream, const struct din_SignatureType* SignatureType) {
    int grammar_id = 278;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 278:
            // Grammar: ID=278; read/write bits=2; START (Id), START (SignedInfo)
            if (SignatureType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=279

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, din_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 279;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignedInfo, SignedInfoType); next=280
                    error = encode_din_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 280;
                    }
                }
            }
            break;
        case 279:
            // Grammar: ID=279; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedInfoType); next=280
                error = encode_din_SignedInfoType(stream, &SignatureType->SignedInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 280;
                }
            }
            break;
        case 280:
            // Grammar: ID=280; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=281
                error = encode_din_SignatureValueType(stream, &SignatureType->SignatureValue);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 281;
                }
            }
            break;
        case 281:
            // Grammar: ID=281; read/write bits=2; START (KeyInfo), START (Object), END Element
            if (SignatureType->KeyInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyInfo, KeyInfoType); next=283
                    error = encode_din_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 283;
                    }
                }
            }
            else if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=282
                    error = encode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 282;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 282:
            // Grammar: ID=282; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=3
                    error = encode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 283:
            // Grammar: ID=283; read/write bits=2; START (Object), END Element
            if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=284
                    error = encode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 284;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 284:
            // Grammar: ID=284; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=3
                    error = encode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}Header; type={urn:din:70121:2012:MsgHeader}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); Notification, NotificationType (0, 1); Signature, SignatureType (0, 1);
static int encode_din_MessageHeaderType(exi_bitstream_t* stream, const struct din_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 285;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 285:
            // Grammar: ID=285; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=286
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MessageHeaderType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MessageHeaderType->SessionID.bytesLen, MessageHeaderType->SessionID.bytes, din_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 286;
                            }
                        }
                    }
                }
            }
            break;
        case 286:
            // Grammar: ID=286; read/write bits=2; START (Notification), START (Signature), END Element
            if (MessageHeaderType->Notification_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Notification, NotificationType); next=287
                    error = encode_din_NotificationType(stream, &MessageHeaderType->Notification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 287;
                    }
                }
            }
            else if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=3
                    error = encode_din_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 287:
            // Grammar: ID=287; read/write bits=2; START (Signature), END Element
            if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=3
                    error = encode_din_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}Body; type={urn:din:70121:2012:MsgDef}BodyType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: BodyElement, BodyBaseType (0, 1); CableCheckReq, CableCheckReqType (0, 1); CableCheckRes, CableCheckResType (0, 1); CertificateInstallationReq, CertificateInstallationReqType (0, 1); CertificateInstallationRes, CertificateInstallationResType (0, 1); CertificateUpdateReq, CertificateUpdateReqType (0, 1); CertificateUpdateRes, CertificateUpdateResType (0, 1); ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReqType (0, 1); ChargeParameterDiscoveryRes, ChargeParameterDiscoveryResType (0, 1); ChargingStatusReq, ChargingStatusReqType (0, 1); ChargingStatusRes, ChargingStatusResType (0, 1); ContractAuthenticationReq, ContractAuthenticationReqType (0, 1); ContractAuthenticationRes, ContractAuthenticationResType (0, 1); CurrentDemandReq, CurrentDemandReqType (0, 1); CurrentDemandRes, CurrentDemandResType (0, 1); MeteringReceiptReq, MeteringReceiptReqType (0, 1); MeteringReceiptRes, MeteringReceiptResType (0, 1); PaymentDetailsReq, PaymentDetailsReqType (0, 1); PaymentDetailsRes, PaymentDetailsResType (0, 1); PowerDeliveryReq, PowerDeliveryReqType (0, 1); PowerDeliveryRes, PowerDeliveryResType (0, 1); PreChargeReq, PreChargeReqType (0, 1); PreChargeRes, PreChargeResType (0, 1); ServiceDetailReq, ServiceDetailReqType (0, 1); ServiceDetailRes, ServiceDetailResType (0, 1); ServiceDiscoveryReq, ServiceDiscoveryReqType (0, 1); ServiceDiscoveryRes, ServiceDiscoveryResType (0, 1); ServicePaymentSelectionReq, ServicePaymentSelectionReqType (0, 1); ServicePaymentSelectionRes, ServicePaymentSelectionResType (0, 1); SessionSetupReq, SessionSetupReqType (0, 1); SessionSetupRes, SessionSetupResType (0, 1); SessionStopReq, SessionStopType (0, 1); SessionStopRes, SessionStopResType (0, 1); WeldingDetectionReq, WeldingDetectionReqType (0, 1); WeldingDetectionRes, WeldingDetectionResType (0, 1);
static int encode_din_BodyType(exi_bitstream_t* stream, const struct din_BodyType* BodyType) {
    int grammar_id = 288;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 288:
            // Grammar: ID=288; read/write bits=6; START (BodyElement)
            if (BodyType->BodyElement_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: BodyElement
                    error = encode_din_BodyBaseType(stream, &BodyType->BodyElement);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CableCheckReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CableCheckReq
                    error = encode_din_CableCheckReqType(stream, &BodyType->CableCheckReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CableCheckRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CableCheckRes
                    error = encode_din_CableCheckResType(stream, &BodyType->CableCheckRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateInstallationReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateInstallationReq
                    error = encode_din_CertificateInstallationReqType(stream, &BodyType->CertificateInstallationReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateInstallationRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateInstallationRes
                    error = encode_din_CertificateInstallationResType(stream, &BodyType->CertificateInstallationRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateUpdateReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateUpdateReq
                    error = encode_din_CertificateUpdateReqType(stream, &BodyType->CertificateUpdateReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateUpdateRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateUpdateRes
                    error = encode_din_CertificateUpdateResType(stream, &BodyType->CertificateUpdateRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargeParameterDiscoveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargeParameterDiscoveryReq
                    error = encode_din_ChargeParameterDiscoveryReqType(stream, &BodyType->ChargeParameterDiscoveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargeParameterDiscoveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargeParameterDiscoveryRes
                    error = encode_din_ChargeParameterDiscoveryResType(stream, &BodyType->ChargeParameterDiscoveryRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargingStatusReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 9);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargingStatusReq
                    error = encode_din_ChargingStatusReqType(stream, &BodyType->ChargingStatusReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargingStatusRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 10);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargingStatusRes
                    error = encode_din_ChargingStatusResType(stream, &BodyType->ChargingStatusRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ContractAuthenticationReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 11);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ContractAuthenticationReq
                    error = encode_din_ContractAuthenticationReqType(stream, &BodyType->ContractAuthenticationReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ContractAuthenticationRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 12);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ContractAuthenticationRes
                    error = encode_din_ContractAuthenticationResType(stream, &BodyType->ContractAuthenticationRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CurrentDemandReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 13);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CurrentDemandReq
                    error = encode_din_CurrentDemandReqType(stream, &BodyType->CurrentDemandReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CurrentDemandRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 14);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CurrentDemandRes
                    error = encode_din_CurrentDemandResType(stream, &BodyType->CurrentDemandRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->MeteringReceiptReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 15);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: MeteringReceiptReq
                    error = encode_din_MeteringReceiptReqType(stream, &BodyType->MeteringReceiptReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->MeteringReceiptRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 16);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: MeteringReceiptRes
                    error = encode_din_MeteringReceiptResType(stream, &BodyType->MeteringReceiptRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PaymentDetailsReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 17);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PaymentDetailsReq
                    error = encode_din_PaymentDetailsReqType(stream, &BodyType->PaymentDetailsReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PaymentDetailsRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 18);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PaymentDetailsRes
                    error = encode_din_PaymentDetailsResType(stream, &BodyType->PaymentDetailsRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PowerDeliveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 19);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PowerDeliveryReq
                    error = encode_din_PowerDeliveryReqType(stream, &BodyType->PowerDeliveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PowerDeliveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 20);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PowerDeliveryRes
                    error = encode_din_PowerDeliveryResType(stream, &BodyType->PowerDeliveryRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PreChargeReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 21);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PreChargeReq
                    error = encode_din_PreChargeReqType(stream, &BodyType->PreChargeReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PreChargeRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PreChargeRes
                    error = encode_din_PreChargeResType(stream, &BodyType->PreChargeRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDetailReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDetailReq
                    error = encode_din_ServiceDetailReqType(stream, &BodyType->ServiceDetailReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDetailRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDetailRes
                    error = encode_din_ServiceDetailResType(stream, &BodyType->ServiceDetailRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDiscoveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 25);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDiscoveryReq
                    error = encode_din_ServiceDiscoveryReqType(stream, &BodyType->ServiceDiscoveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDiscoveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 26);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDiscoveryRes
                    error = encode_din_ServiceDiscoveryResType(stream, &BodyType->ServiceDiscoveryRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServicePaymentSelectionReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 27);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServicePaymentSelectionReq
                    error = encode_din_ServicePaymentSelectionReqType(stream, &BodyType->ServicePaymentSelectionReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServicePaymentSelectionRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServicePaymentSelectionRes
                    error = encode_din_ServicePaymentSelectionResType(stream, &BodyType->ServicePaymentSelectionRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->SessionSetupReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 29);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: SessionSetupReq
                    error = encode_din_SessionSetupReqType(stream, &BodyType->SessionSetupReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->SessionSetupRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 30);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: SessionSetupRes
                    error = encode_din_SessionSetupResType(stream, &BodyType->SessionSetupRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->SessionStopReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 31);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: SessionStopReq
                    error = encode_din_SessionStopType(stream, &BodyType->SessionStopReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->SessionStopRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 32);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: SessionStopRes
                    error = encode_din_SessionStopResType(stream, &BodyType->SessionStopRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->WeldingDetectionReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 33);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: WeldingDetectionReq
                    error = encode_din_WeldingDetectionReqType(stream, &BodyType->WeldingDetectionReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->WeldingDetectionRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 34);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: WeldingDetectionRes
                    error = encode_din_WeldingDetectionResType(stream, &BodyType->WeldingDetectionRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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

// Element: definition=complex; name={urn:din:70121:2012:MsgDef}V2G_Message; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Header, MessageHeaderType (1, 1); Body, BodyType (1, 1);
static int encode_din_V2G_Message(exi_bitstream_t* stream, const struct din_V2G_Message* V2G_Message) {
    int grammar_id = 289;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 289:
            // Grammar: ID=289; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=290
                error = encode_din_MessageHeaderType(stream, &V2G_Message->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 290;
                }
            }
            break;
        case 290:
            // Grammar: ID=290; read/write bits=1; START (Body)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (BodyType); next=3
                error = encode_din_BodyType(stream, &V2G_Message->Body);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 3;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=4
                done = 1;
                grammar_id = 4;
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


// main function for encoding
int encode_din_exiDocument(exi_bitstream_t* stream, struct din_exiDocument* exiDoc)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        // encode event exiDoc->V2G_Message with index 77
        error = exi_basetypes_encoder_nbit_uint(stream, 7, 77);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = encode_din_V2G_Message(stream, &exiDoc->V2G_Message);
        }
    }

    return error;
}


