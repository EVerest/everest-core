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
  * @file din_msgDefDecoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_basetypes_decoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/common/exi_types_decoder.h"
#include "cbv2g/din/din_msgDefDatatypes.h"
#include "cbv2g/din/din_msgDefDecoder.h"



static int decode_din_CostType(exi_bitstream_t* stream, struct din_CostType* CostType);
static int decode_din_RelativeTimeIntervalType(exi_bitstream_t* stream, struct din_RelativeTimeIntervalType* RelativeTimeIntervalType);
static int decode_din_IntervalType(exi_bitstream_t* stream, struct din_IntervalType* IntervalType);
static int decode_din_ConsumptionCostType(exi_bitstream_t* stream, struct din_ConsumptionCostType* ConsumptionCostType);
static int decode_din_TransformType(exi_bitstream_t* stream, struct din_TransformType* TransformType);
static int decode_din_PMaxScheduleEntryType(exi_bitstream_t* stream, struct din_PMaxScheduleEntryType* PMaxScheduleEntryType);
static int decode_din_SalesTariffEntryType(exi_bitstream_t* stream, struct din_SalesTariffEntryType* SalesTariffEntryType);
static int decode_din_TransformsType(exi_bitstream_t* stream, struct din_TransformsType* TransformsType);
static int decode_din_DSAKeyValueType(exi_bitstream_t* stream, struct din_DSAKeyValueType* DSAKeyValueType);
static int decode_din_X509IssuerSerialType(exi_bitstream_t* stream, struct din_X509IssuerSerialType* X509IssuerSerialType);
static int decode_din_DigestMethodType(exi_bitstream_t* stream, struct din_DigestMethodType* DigestMethodType);
static int decode_din_RSAKeyValueType(exi_bitstream_t* stream, struct din_RSAKeyValueType* RSAKeyValueType);
static int decode_din_PMaxScheduleType(exi_bitstream_t* stream, struct din_PMaxScheduleType* PMaxScheduleType);
static int decode_din_SalesTariffType(exi_bitstream_t* stream, struct din_SalesTariffType* SalesTariffType);
static int decode_din_CanonicalizationMethodType(exi_bitstream_t* stream, struct din_CanonicalizationMethodType* CanonicalizationMethodType);
static int decode_din_ServiceTagType(exi_bitstream_t* stream, struct din_ServiceTagType* ServiceTagType);
static int decode_din_ServiceType(exi_bitstream_t* stream, struct din_ServiceType* ServiceType);
static int decode_din_SelectedServiceType(exi_bitstream_t* stream, struct din_SelectedServiceType* SelectedServiceType);
static int decode_din_SAScheduleTupleType(exi_bitstream_t* stream, struct din_SAScheduleTupleType* SAScheduleTupleType);
static int decode_din_AC_EVSEStatusType(exi_bitstream_t* stream, struct din_AC_EVSEStatusType* AC_EVSEStatusType);
static int decode_din_SignatureMethodType(exi_bitstream_t* stream, struct din_SignatureMethodType* SignatureMethodType);
static int decode_din_KeyValueType(exi_bitstream_t* stream, struct din_KeyValueType* KeyValueType);
static int decode_din_SubCertificatesType(exi_bitstream_t* stream, struct din_SubCertificatesType* SubCertificatesType);
static int decode_din_ProfileEntryType(exi_bitstream_t* stream, struct din_ProfileEntryType* ProfileEntryType);
static int decode_din_ReferenceType(exi_bitstream_t* stream, struct din_ReferenceType* ReferenceType);
static int decode_din_RetrievalMethodType(exi_bitstream_t* stream, struct din_RetrievalMethodType* RetrievalMethodType);
static int decode_din_X509DataType(exi_bitstream_t* stream, struct din_X509DataType* X509DataType);
static int decode_din_PGPDataType(exi_bitstream_t* stream, struct din_PGPDataType* PGPDataType);
static int decode_din_SPKIDataType(exi_bitstream_t* stream, struct din_SPKIDataType* SPKIDataType);
static int decode_din_SignedInfoType(exi_bitstream_t* stream, struct din_SignedInfoType* SignedInfoType);
static int decode_din_DC_EVStatusType(exi_bitstream_t* stream, struct din_DC_EVStatusType* DC_EVStatusType);
static int decode_din_SignatureValueType(exi_bitstream_t* stream, struct din_SignatureValueType* SignatureValueType);
static int decode_din_CertificateChainType(exi_bitstream_t* stream, struct din_CertificateChainType* CertificateChainType);
static int decode_din_DC_EVSEStatusType(exi_bitstream_t* stream, struct din_DC_EVSEStatusType* DC_EVSEStatusType);
static int decode_din_PhysicalValueType(exi_bitstream_t* stream, struct din_PhysicalValueType* PhysicalValueType);
static int decode_din_ParameterType(exi_bitstream_t* stream, struct din_ParameterType* ParameterType);
static int decode_din_ParameterSetType(exi_bitstream_t* stream, struct din_ParameterSetType* ParameterSetType);
static int decode_din_ListOfRootCertificateIDsType(exi_bitstream_t* stream, struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
static int decode_din_PaymentOptionsType(exi_bitstream_t* stream, struct din_PaymentOptionsType* PaymentOptionsType);
static int decode_din_SelectedServiceListType(exi_bitstream_t* stream, struct din_SelectedServiceListType* SelectedServiceListType);
static int decode_din_AC_EVChargeParameterType(exi_bitstream_t* stream, struct din_AC_EVChargeParameterType* AC_EVChargeParameterType);
static int decode_din_DC_EVChargeParameterType(exi_bitstream_t* stream, struct din_DC_EVChargeParameterType* DC_EVChargeParameterType);
static int decode_din_EVChargeParameterType(exi_bitstream_t* stream, struct din_EVChargeParameterType* EVChargeParameterType);
static int decode_din_ChargingProfileType(exi_bitstream_t* stream, struct din_ChargingProfileType* ChargingProfileType);
static int decode_din_EVSEStatusType(exi_bitstream_t* stream, struct din_EVSEStatusType* EVSEStatusType);
static int decode_din_KeyInfoType(exi_bitstream_t* stream, struct din_KeyInfoType* KeyInfoType);
static int decode_din_ServiceChargeType(exi_bitstream_t* stream, struct din_ServiceChargeType* ServiceChargeType);
static int decode_din_ServiceParameterListType(exi_bitstream_t* stream, struct din_ServiceParameterListType* ServiceParameterListType);
static int decode_din_SAScheduleListType(exi_bitstream_t* stream, struct din_SAScheduleListType* SAScheduleListType);
static int decode_din_SASchedulesType(exi_bitstream_t* stream, struct din_SASchedulesType* SASchedulesType);
static int decode_din_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
static int decode_din_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
static int decode_din_ObjectType(exi_bitstream_t* stream, struct din_ObjectType* ObjectType);
static int decode_din_ServiceTagListType(exi_bitstream_t* stream, struct din_ServiceTagListType* ServiceTagListType);
static int decode_din_DC_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
static int decode_din_AC_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
static int decode_din_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_EVSEChargeParameterType* EVSEChargeParameterType);
static int decode_din_MeterInfoType(exi_bitstream_t* stream, struct din_MeterInfoType* MeterInfoType);
static int decode_din_CertificateInstallationResType(exi_bitstream_t* stream, struct din_CertificateInstallationResType* CertificateInstallationResType);
static int decode_din_CableCheckReqType(exi_bitstream_t* stream, struct din_CableCheckReqType* CableCheckReqType);
static int decode_din_CableCheckResType(exi_bitstream_t* stream, struct din_CableCheckResType* CableCheckResType);
static int decode_din_PreChargeReqType(exi_bitstream_t* stream, struct din_PreChargeReqType* PreChargeReqType);
static int decode_din_PreChargeResType(exi_bitstream_t* stream, struct din_PreChargeResType* PreChargeResType);
static int decode_din_CurrentDemandReqType(exi_bitstream_t* stream, struct din_CurrentDemandReqType* CurrentDemandReqType);
static int decode_din_CurrentDemandResType(exi_bitstream_t* stream, struct din_CurrentDemandResType* CurrentDemandResType);
static int decode_din_WeldingDetectionReqType(exi_bitstream_t* stream, struct din_WeldingDetectionReqType* WeldingDetectionReqType);
static int decode_din_WeldingDetectionResType(exi_bitstream_t* stream, struct din_WeldingDetectionResType* WeldingDetectionResType);
static int decode_din_SessionSetupReqType(exi_bitstream_t* stream, struct din_SessionSetupReqType* SessionSetupReqType);
static int decode_din_CertificateInstallationReqType(exi_bitstream_t* stream, struct din_CertificateInstallationReqType* CertificateInstallationReqType);
static int decode_din_SessionSetupResType(exi_bitstream_t* stream, struct din_SessionSetupResType* SessionSetupResType);
static int decode_din_ServiceDiscoveryReqType(exi_bitstream_t* stream, struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
static int decode_din_ServiceDiscoveryResType(exi_bitstream_t* stream, struct din_ServiceDiscoveryResType* ServiceDiscoveryResType);
static int decode_din_ServiceDetailReqType(exi_bitstream_t* stream, struct din_ServiceDetailReqType* ServiceDetailReqType);
static int decode_din_ServiceDetailResType(exi_bitstream_t* stream, struct din_ServiceDetailResType* ServiceDetailResType);
static int decode_din_ServicePaymentSelectionReqType(exi_bitstream_t* stream, struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType);
static int decode_din_ServicePaymentSelectionResType(exi_bitstream_t* stream, struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType);
static int decode_din_PaymentDetailsReqType(exi_bitstream_t* stream, struct din_PaymentDetailsReqType* PaymentDetailsReqType);
static int decode_din_PaymentDetailsResType(exi_bitstream_t* stream, struct din_PaymentDetailsResType* PaymentDetailsResType);
static int decode_din_ContractAuthenticationReqType(exi_bitstream_t* stream, struct din_ContractAuthenticationReqType* ContractAuthenticationReqType);
static int decode_din_ContractAuthenticationResType(exi_bitstream_t* stream, struct din_ContractAuthenticationResType* ContractAuthenticationResType);
static int decode_din_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
static int decode_din_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
static int decode_din_PowerDeliveryReqType(exi_bitstream_t* stream, struct din_PowerDeliveryReqType* PowerDeliveryReqType);
static int decode_din_PowerDeliveryResType(exi_bitstream_t* stream, struct din_PowerDeliveryResType* PowerDeliveryResType);
static int decode_din_ChargingStatusReqType(exi_bitstream_t* stream, struct din_ChargingStatusReqType* ChargingStatusReqType);
static int decode_din_ChargingStatusResType(exi_bitstream_t* stream, struct din_ChargingStatusResType* ChargingStatusResType);
static int decode_din_MeteringReceiptReqType(exi_bitstream_t* stream, struct din_MeteringReceiptReqType* MeteringReceiptReqType);
static int decode_din_MeteringReceiptResType(exi_bitstream_t* stream, struct din_MeteringReceiptResType* MeteringReceiptResType);
static int decode_din_SessionStopType(exi_bitstream_t* stream, struct din_SessionStopType* SessionStopType);
static int decode_din_SessionStopResType(exi_bitstream_t* stream, struct din_SessionStopResType* SessionStopResType);
static int decode_din_CertificateUpdateReqType(exi_bitstream_t* stream, struct din_CertificateUpdateReqType* CertificateUpdateReqType);
static int decode_din_CertificateUpdateResType(exi_bitstream_t* stream, struct din_CertificateUpdateResType* CertificateUpdateResType);
static int decode_din_BodyBaseType(exi_bitstream_t* stream, struct din_BodyBaseType* BodyBaseType);
static int decode_din_NotificationType(exi_bitstream_t* stream, struct din_NotificationType* NotificationType);
static int decode_din_SignatureType(exi_bitstream_t* stream, struct din_SignatureType* SignatureType);
static int decode_din_MessageHeaderType(exi_bitstream_t* stream, struct din_MessageHeaderType* MessageHeaderType);
static int decode_din_BodyType(exi_bitstream_t* stream, struct din_BodyType* BodyType);
static int decode_din_V2G_Message(exi_bitstream_t* stream, struct din_V2G_Message* V2G_Message);

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}Cost; type={urn:din:70121:2012:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
static int decode_din_CostType(exi_bitstream_t* stream, struct din_CostType* CostType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CostType(CostType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (costKind)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (costKind, costKindType (string)); next=1
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                CostType->costKind = (din_costKindType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 1;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=1; START (amount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (amount, unsignedInt (unsignedLong)); next=2
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &CostType->amount);
                    if (error == 0)
                    {
                        grammar_id = 2;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=2; START (amountMultiplier), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (amountMultiplier, unitMultiplierType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                            if (error == 0)
                            {
                                // type has min_value = -3
                                CostType->amountMultiplier = (int8_t)(value + -3);
                                CostType->amountMultiplier_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_RelativeTimeIntervalType(exi_bitstream_t* stream, struct din_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    int grammar_id = 5;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_RelativeTimeIntervalType(RelativeTimeIntervalType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 5:
            // Grammar: ID=5; read/write bits=1; START (start)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (start, unsignedInt (unsignedLong)); next=6
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &RelativeTimeIntervalType->start);
                    if (error == 0)
                    {
                        grammar_id = 6;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 6:
            // Grammar: ID=6; read/write bits=2; START (duration), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (duration, unsignedInt (unsignedLong)); next=3
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &RelativeTimeIntervalType->duration);
                    if (error == 0)
                    {
                        RelativeTimeIntervalType->duration_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_IntervalType(exi_bitstream_t* stream, struct din_IntervalType* IntervalType) {
    // Element has no particles, so the function just decodes END Element
    (void)IntervalType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}ConsumptionCost; type={urn:din:70121:2012:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, unsignedInt (1, 1); Cost, CostType (0, 1) (original max unbounded);
static int decode_din_ConsumptionCostType(exi_bitstream_t* stream, struct din_ConsumptionCostType* ConsumptionCostType) {
    int grammar_id = 7;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ConsumptionCostType(ConsumptionCostType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (startValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (startValue, unsignedInt (unsignedLong)); next=8
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &ConsumptionCostType->startValue);
                    if (error == 0)
                    {
                        grammar_id = 8;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (Cost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Cost, CostType (CostType)); next=9
                    // decode: element
                    error = decode_din_CostType(stream, &ConsumptionCostType->Cost);
                    if (error == 0)
                    {
                        ConsumptionCostType->Cost_isUsed = 1u;
                        grammar_id = 9;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 9:
            // Grammar: ID=9; read/write bits=2; START (Cost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Cost, CostType (CostType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_TransformType(exi_bitstream_t* stream, struct din_TransformType* TransformType) {
    int grammar_id = 10;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_TransformType(TransformType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 10:
            // Grammar: ID=10; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=11
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &TransformType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (TransformType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            TransformType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 11;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (XPath, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &TransformType->XPath.charactersLen);
                            if (error == 0)
                            {
                                if (TransformType->XPath.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    TransformType->XPath.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, din_XPath_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                TransformType->XPath_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &TransformType->ANY.bytesLen, &TransformType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        TransformType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PMaxScheduleEntryType(exi_bitstream_t* stream, struct din_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    int grammar_id = 12;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PMaxScheduleEntryType(PMaxScheduleEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 12:
            // Grammar: ID=12; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RelativeTimeInterval, RelativeTimeIntervalType (IntervalType)); next=13
                    // decode: element
                    error = decode_din_RelativeTimeIntervalType(stream, &PMaxScheduleEntryType->RelativeTimeInterval);
                    if (error == 0)
                    {
                        PMaxScheduleEntryType->RelativeTimeInterval_isUsed = 1u;
                        grammar_id = 13;
                    }
                    break;
                case 1:
                    // Abstract element or type: TimeInterval, IntervalType (IntervalType)
                    // decode: element
                    error = decode_din_IntervalType(stream, &PMaxScheduleEntryType->TimeInterval);
                    if (error == 0)
                    {
                        PMaxScheduleEntryType->TimeInterval_isUsed = 1u;
                        grammar_id = 13;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 13:
            // Grammar: ID=13; read/write bits=1; START (PMax)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMax, PMaxType (short)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &PMaxScheduleEntryType->PMax);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SalesTariffEntryType(exi_bitstream_t* stream, struct din_SalesTariffEntryType* SalesTariffEntryType) {
    int grammar_id = 14;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SalesTariffEntryType(SalesTariffEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 14:
            // Grammar: ID=14; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RelativeTimeInterval, RelativeTimeIntervalType (IntervalType)); next=15
                    // decode: element
                    error = decode_din_RelativeTimeIntervalType(stream, &SalesTariffEntryType->RelativeTimeInterval);
                    if (error == 0)
                    {
                        SalesTariffEntryType->RelativeTimeInterval_isUsed = 1u;
                        grammar_id = 15;
                    }
                    break;
                case 1:
                    // Abstract element or type: TimeInterval, IntervalType (IntervalType)
                    // decode: element
                    error = decode_din_IntervalType(stream, &SalesTariffEntryType->TimeInterval);
                    if (error == 0)
                    {
                        SalesTariffEntryType->TimeInterval_isUsed = 1u;
                        grammar_id = 15;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 15:
            // Grammar: ID=15; read/write bits=1; START (EPriceLevel)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EPriceLevel, unsignedByte (unsignedShort)); next=16
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                SalesTariffEntryType->EPriceLevel = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 16;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 16:
            // Grammar: ID=16; read/write bits=2; START (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=17
                    // decode: element
                    error = decode_din_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost);
                    if (error == 0)
                    {
                        SalesTariffEntryType->ConsumptionCost_isUsed = 1u;
                        grammar_id = 17;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 17:
            // Grammar: ID=17; read/write bits=2; START (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_TransformsType(exi_bitstream_t* stream, struct din_TransformsType* TransformsType) {
    int grammar_id = 18;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_TransformsType(TransformsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 18:
            // Grammar: ID=18; read/write bits=1; START (Transform)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=19
                    // decode: element
                    error = decode_din_TransformType(stream, &TransformsType->Transform);
                    if (error == 0)
                    {
                        grammar_id = 19;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 19:
            // Grammar: ID=19; read/write bits=2; START (Transform), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DSAKeyValueType(exi_bitstream_t* stream, struct din_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 20;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DSAKeyValueType(DSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 20:
            // Grammar: ID=20; read/write bits=2; START (P), START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (P, CryptoBinary (base64Binary)); next=21
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->P.bytesLen, &DSAKeyValueType->P.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->P_isUsed = 1u;
                        grammar_id = 21;
                    }
                    break;
                case 1:
                    // Event: START (G, CryptoBinary (base64Binary)); next=23
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 23;
                    }
                    break;
                case 2:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=24
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 24;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 21:
            // Grammar: ID=21; read/write bits=1; START (Q)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Q, CryptoBinary (base64Binary)); next=22
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Q.bytesLen, &DSAKeyValueType->Q.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Q_isUsed = 1u;
                        grammar_id = 22;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 22:
            // Grammar: ID=22; read/write bits=2; START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (G, CryptoBinary (base64Binary)); next=23
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 23;
                    }
                    break;
                case 1:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=24
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 24;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=24
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 24;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 24:
            // Grammar: ID=24; read/write bits=2; START (J), START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (J, CryptoBinary (base64Binary)); next=25
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->J.bytesLen, &DSAKeyValueType->J.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->J_isUsed = 1u;
                        grammar_id = 25;
                    }
                    break;
                case 1:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=26
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 26;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=26
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 26;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 26:
            // Grammar: ID=26; read/write bits=2; START (PgenCounter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PgenCounter, CryptoBinary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->PgenCounter.bytesLen, &DSAKeyValueType->PgenCounter.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->PgenCounter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_X509IssuerSerialType(exi_bitstream_t* stream, struct din_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 27;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_X509IssuerSerialType(X509IssuerSerialType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 27:
            // Grammar: ID=27; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerName, string (string)); next=28
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &X509IssuerSerialType->X509IssuerName.charactersLen);
                            if (error == 0)
                            {
                                if (X509IssuerSerialType->X509IssuerName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    X509IssuerSerialType->X509IssuerName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, din_X509IssuerName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 28;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=1; START (X509SerialNumber)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509SerialNumber, integer (decimal)); next=3
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &X509IssuerSerialType->X509SerialNumber);
                        if (error == 0)
                        {
                            grammar_id = 3;
                        }
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DigestMethodType(exi_bitstream_t* stream, struct din_DigestMethodType* DigestMethodType) {
    int grammar_id = 29;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DigestMethodType(DigestMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 29:
            // Grammar: ID=29; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=30
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &DigestMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (DigestMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            DigestMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 30;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 30:
            // Grammar: ID=30; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DigestMethodType->ANY.bytesLen, &DigestMethodType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        DigestMethodType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_RSAKeyValueType(exi_bitstream_t* stream, struct din_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 31;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_RSAKeyValueType(RSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 31:
            // Grammar: ID=31; read/write bits=1; START (Modulus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Modulus, CryptoBinary (base64Binary)); next=32
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Modulus.bytesLen, &RSAKeyValueType->Modulus.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 32;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 32:
            // Grammar: ID=32; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Exponent, CryptoBinary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Exponent.bytesLen, &RSAKeyValueType->Exponent.bytes[0], din_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PMaxScheduleType(exi_bitstream_t* stream, struct din_PMaxScheduleType* PMaxScheduleType) {
    int grammar_id = 33;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PMaxScheduleType(PMaxScheduleType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 33:
            // Grammar: ID=33; read/write bits=1; START (PMaxScheduleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMaxScheduleID, SAIDType (short)); next=34
                    // decode: short
                    error = decode_exi_type_integer16(stream, &PMaxScheduleType->PMaxScheduleID);
                    if (error == 0)
                    {
                        grammar_id = 34;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=1; START (PMaxScheduleEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMaxScheduleEntry, PMaxScheduleEntryType (EntryType)); next=35
                    // decode: element array
                    if (PMaxScheduleType->PMaxScheduleEntry.arrayLen < din_PMaxScheduleEntryType_5_ARRAY_SIZE)
                    {
                        error = decode_din_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleType->PMaxScheduleEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_PMaxScheduleEntryType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 35;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 35:
            // Grammar: ID=35; read/write bits=2; LOOP (PMaxScheduleEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (PMaxScheduleEntry, PMaxScheduleEntryType (EntryType)); next=35
                    // decode: element array
                    if (PMaxScheduleType->PMaxScheduleEntry.arrayLen < din_PMaxScheduleEntryType_5_ARRAY_SIZE)
                    {
                        error = decode_din_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleType->PMaxScheduleEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_PMaxScheduleEntryType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 35;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SalesTariffType(exi_bitstream_t* stream, struct din_SalesTariffType* SalesTariffType) {
    int grammar_id = 36;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SalesTariffType(SalesTariffType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=37
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SalesTariffType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SalesTariffType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SalesTariffType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SalesTariffType->Id.charactersLen, SalesTariffType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 37;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=1; START (SalesTariffID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffID, SAIDType (short)); next=38
                    // decode: short
                    error = decode_exi_type_integer16(stream, &SalesTariffType->SalesTariffID);
                    if (error == 0)
                    {
                        grammar_id = 38;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 38:
            // Grammar: ID=38; read/write bits=2; START (SalesTariffDescription), START (NumEPriceLevels)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffDescription, tariffDescriptionType (string)); next=39
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &SalesTariffType->SalesTariffDescription.charactersLen);
                            if (error == 0)
                            {
                                if (SalesTariffType->SalesTariffDescription.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    SalesTariffType->SalesTariffDescription.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, SalesTariffType->SalesTariffDescription.charactersLen, SalesTariffType->SalesTariffDescription.characters, din_SalesTariffDescription_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                SalesTariffType->SalesTariffDescription_isUsed = 1u;
                                grammar_id = 39;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (NumEPriceLevels, unsignedByte (unsignedShort)); next=40
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                SalesTariffType->NumEPriceLevels = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 40;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=1; START (NumEPriceLevels)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NumEPriceLevels, unsignedByte (unsignedShort)); next=40
                    // decode: unsigned byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                SalesTariffType->NumEPriceLevels = (uint8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 40;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 40:
            // Grammar: ID=40; read/write bits=1; START (SalesTariffEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=41
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < din_SalesTariffEntryType_5_ARRAY_SIZE)
                    {
                        error = decode_din_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SalesTariffEntryType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 41;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 41:
            // Grammar: ID=41; read/write bits=2; LOOP (SalesTariffEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=41
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < din_SalesTariffEntryType_5_ARRAY_SIZE)
                    {
                        error = decode_din_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SalesTariffEntryType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 41;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CanonicalizationMethodType(exi_bitstream_t* stream, struct din_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 42;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CanonicalizationMethodType(CanonicalizationMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 42:
            // Grammar: ID=42; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=43
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CanonicalizationMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (CanonicalizationMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CanonicalizationMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 43;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 43:
            // Grammar: ID=43; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CanonicalizationMethodType->ANY.bytesLen, &CanonicalizationMethodType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        CanonicalizationMethodType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceTagType(exi_bitstream_t* stream, struct din_ServiceTagType* ServiceTagType) {
    int grammar_id = 44;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceTagType(ServiceTagType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 44:
            // Grammar: ID=44; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=45
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ServiceTagType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 45;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 45:
            // Grammar: ID=45; read/write bits=2; START (ServiceName), START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceName, serviceNameType (string)); next=46
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ServiceTagType->ServiceName.charactersLen);
                            if (error == 0)
                            {
                                if (ServiceTagType->ServiceName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ServiceTagType->ServiceName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ServiceTagType->ServiceName.charactersLen, ServiceTagType->ServiceName.characters, din_ServiceName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ServiceTagType->ServiceName_isUsed = 1u;
                                grammar_id = 46;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=47
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                ServiceTagType->ServiceCategory = (din_serviceCategoryType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 47;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=47
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                ServiceTagType->ServiceCategory = (din_serviceCategoryType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 47;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 47:
            // Grammar: ID=47; read/write bits=2; START (ServiceScope), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceScope, serviceScopeType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ServiceTagType->ServiceScope.charactersLen);
                            if (error == 0)
                            {
                                if (ServiceTagType->ServiceScope.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ServiceTagType->ServiceScope.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ServiceTagType->ServiceScope.charactersLen, ServiceTagType->ServiceScope.characters, din_ServiceScope_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ServiceTagType->ServiceScope_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceType(exi_bitstream_t* stream, struct din_ServiceType* ServiceType) {
    int grammar_id = 48;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceType(ServiceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 48:
            // Grammar: ID=48; read/write bits=1; START (ServiceTag)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceTag, ServiceTagType (ServiceTagType)); next=49
                    // decode: element
                    error = decode_din_ServiceTagType(stream, &ServiceType->ServiceTag);
                    if (error == 0)
                    {
                        grammar_id = 49;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=1; START (FreeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FreeService, boolean (boolean)); next=3
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ServiceType->FreeService = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SelectedServiceType(exi_bitstream_t* stream, struct din_SelectedServiceType* SelectedServiceType) {
    int grammar_id = 50;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SelectedServiceType(SelectedServiceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 50:
            // Grammar: ID=50; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=51
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &SelectedServiceType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 51;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=2; START (ParameterSetID), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ParameterSetID, short (int)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &SelectedServiceType->ParameterSetID);
                    if (error == 0)
                    {
                        SelectedServiceType->ParameterSetID_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SAScheduleTupleType(exi_bitstream_t* stream, struct din_SAScheduleTupleType* SAScheduleTupleType) {
    int grammar_id = 52;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SAScheduleTupleType(SAScheduleTupleType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (short)); next=53
                    // decode: short
                    error = decode_exi_type_integer16(stream, &SAScheduleTupleType->SAScheduleTupleID);
                    if (error == 0)
                    {
                        grammar_id = 53;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 53:
            // Grammar: ID=53; read/write bits=1; START (PMaxSchedule)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMaxSchedule, PMaxScheduleType (PMaxScheduleType)); next=54
                    // decode: element
                    error = decode_din_PMaxScheduleType(stream, &SAScheduleTupleType->PMaxSchedule);
                    if (error == 0)
                    {
                        grammar_id = 54;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 54:
            // Grammar: ID=54; read/write bits=2; START (SalesTariff), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariff, SalesTariffType (SalesTariffType)); next=3
                    // decode: element
                    error = decode_din_SalesTariffType(stream, &SAScheduleTupleType->SalesTariff);
                    if (error == 0)
                    {
                        SAScheduleTupleType->SalesTariff_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_AC_EVSEStatusType(exi_bitstream_t* stream, struct din_AC_EVSEStatusType* AC_EVSEStatusType) {
    int grammar_id = 55;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_AC_EVSEStatusType(AC_EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 55:
            // Grammar: ID=55; read/write bits=1; START (PowerSwitchClosed)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PowerSwitchClosed, boolean (boolean)); next=56
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                AC_EVSEStatusType->PowerSwitchClosed = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 56;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 56:
            // Grammar: ID=56; read/write bits=1; START (RCD)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RCD, boolean (boolean)); next=57
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                AC_EVSEStatusType->RCD = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 57;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedInt (unsignedLong)); next=58
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &AC_EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 58;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 58:
            // Grammar: ID=58; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, EVSENotificationType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                AC_EVSEStatusType->EVSENotification = (din_EVSENotificationType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SignatureMethodType(exi_bitstream_t* stream, struct din_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 59;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SignatureMethodType(SignatureMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 59:
            // Grammar: ID=59; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=60
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, din_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 60;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 60:
            // Grammar: ID=60; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (HMACOutputLength, HMACOutputLengthType (integer)); next=61
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == 0)
                        {
                            SignatureMethodType->HMACOutputLength_isUsed = 1u;
                            grammar_id = 61;
                        }
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SignatureMethodType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 61:
            // Grammar: ID=61; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SignatureMethodType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_KeyValueType(exi_bitstream_t* stream, struct din_KeyValueType* KeyValueType) {
    int grammar_id = 62;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_KeyValueType(KeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 62:
            // Grammar: ID=62; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DSAKeyValue, DSAKeyValueType (DSAKeyValueType)); next=3
                    // decode: element
                    error = decode_din_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->DSAKeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (RSAKeyValue, RSAKeyValueType (RSAKeyValueType)); next=3
                    // decode: element
                    error = decode_din_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->RSAKeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyValueType->ANY.bytesLen, &KeyValueType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyValueType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SubCertificatesType(exi_bitstream_t* stream, struct din_SubCertificatesType* SubCertificatesType) {
    int grammar_id = 63;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SubCertificatesType(SubCertificatesType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 63:
            // Grammar: ID=63; read/write bits=1; START (Certificate)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Certificate, certificateType (base64Binary)); next=64
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SubCertificatesType->Certificate.bytesLen, &SubCertificatesType->Certificate.bytes[0], din_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 64;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 64:
            // Grammar: ID=64; read/write bits=2; START (Certificate), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Certificate, certificateType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SubCertificatesType->Certificate.bytesLen, &SubCertificatesType->Certificate.bytes[0], din_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ProfileEntryType(exi_bitstream_t* stream, struct din_ProfileEntryType* ProfileEntryType) {
    int grammar_id = 65;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ProfileEntryType(ProfileEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 65:
            // Grammar: ID=65; read/write bits=1; START (ChargingProfileEntryStart)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfileEntryStart, unsignedInt (unsignedLong)); next=66
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &ProfileEntryType->ChargingProfileEntryStart);
                    if (error == 0)
                    {
                        grammar_id = 66;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=1; START (ChargingProfileEntryMaxPower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfileEntryMaxPower, PMaxType (short)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ProfileEntryType->ChargingProfileEntryMaxPower);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ReferenceType(exi_bitstream_t* stream, struct din_ReferenceType* ReferenceType) {
    int grammar_id = 67;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ReferenceType(ReferenceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 67:
            // Grammar: ID=67; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=68
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Id_isUsed = 1u;
                    grammar_id = 68;
                    break;
                case 1:
                    // Event: START (Type, anyURI (anyURI)); next=69
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, din_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 69;
                    break;
                case 2:
                    // Event: START (URI, anyURI (anyURI)); next=70
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 70;
                    break;
                case 3:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=71
                    // decode: element
                    error = decode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 71;
                    }
                    break;
                case 4:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=72
                    // decode: element
                    error = decode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=69
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, din_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 69;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=70
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 70;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=71
                    // decode: element
                    error = decode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 71;
                    }
                    break;
                case 3:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=72
                    // decode: element
                    error = decode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 69:
            // Grammar: ID=69; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=70
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, din_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 70;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=71
                    // decode: element
                    error = decode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 71;
                    }
                    break;
                case 2:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=72
                    // decode: element
                    error = decode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 70:
            // Grammar: ID=70; read/write bits=2; START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=71
                    // decode: element
                    error = decode_din_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 71;
                    }
                    break;
                case 1:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=72
                    // decode: element
                    error = decode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=72
                    // decode: element
                    error = decode_din_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 72;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=1; START (DigestValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestValue, DigestValueType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ReferenceType->DigestValue.bytesLen, &ReferenceType->DigestValue.bytes[0], din_DigestValueType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_RetrievalMethodType(exi_bitstream_t* stream, struct din_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 73;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_RetrievalMethodType(RetrievalMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 73:
            // Grammar: ID=73; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=74
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, din_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->Type_isUsed = 1u;
                    grammar_id = 74;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=75
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, din_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 75;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 74:
            // Grammar: ID=74; read/write bits=2; START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=75
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, din_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 75;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=2; START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_din_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == 0)
                    {
                        RetrievalMethodType->Transforms_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_X509DataType(exi_bitstream_t* stream, struct din_X509DataType* X509DataType) {
    int grammar_id = 76;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_X509DataType(X509DataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 76:
            // Grammar: ID=76; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerSerial, X509IssuerSerialType (X509IssuerSerialType)); next=3
                    // decode: element
                    error = decode_din_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == 0)
                    {
                        X509DataType->X509IssuerSerial_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (X509SKI, base64Binary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509SKI.bytesLen, &X509DataType->X509SKI.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509SKI_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (X509SubjectName, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &X509DataType->X509SubjectName.charactersLen);
                            if (error == 0)
                            {
                                if (X509DataType->X509SubjectName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    X509DataType->X509SubjectName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, din_X509SubjectName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                X509DataType->X509SubjectName_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (X509Certificate, base64Binary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509Certificate.bytesLen, &X509DataType->X509Certificate.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509Certificate_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (X509CRL, base64Binary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509CRL.bytesLen, &X509DataType->X509CRL.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509CRL_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->ANY.bytesLen, &X509DataType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PGPDataType(exi_bitstream_t* stream, struct din_PGPDataType* PGPDataType) {
    int grammar_id = 77;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PGPDataType(PGPDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyID, base64Binary (base64Binary)); next=78
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyID.bytesLen, &PGPDataType->choice_1.PGPKeyID.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 78;
                    }
                    break;
                case 1:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=79
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 79;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=79
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 79;
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 80;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 80;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=81
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.PGPKeyPacket.bytesLen, &PGPDataType->choice_2.PGPKeyPacket.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 81;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=80
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.ANY.bytesLen, &PGPDataType->choice_2.ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_2.ANY_isUsed = 1u;
                        grammar_id = 80;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SPKIDataType(exi_bitstream_t* stream, struct din_SPKIDataType* SPKIDataType) {
    int grammar_id = 82;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SPKIDataType(SPKIDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 82:
            // Grammar: ID=82; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPKISexp, base64Binary (base64Binary)); next=83
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->SPKISexp.bytesLen, &SPKIDataType->SPKISexp.bytes[0], din_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 83;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->ANY.bytesLen, &SPKIDataType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        SPKIDataType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SignedInfoType(exi_bitstream_t* stream, struct din_SignedInfoType* SignedInfoType) {
    int grammar_id = 84;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SignedInfoType(SignedInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 84:
            // Grammar: ID=84; read/write bits=2; START (Id), START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=85
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignedInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignedInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignedInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignedInfoType->Id_isUsed = 1u;
                    grammar_id = 85;
                    break;
                case 1:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=86
                    // decode: element
                    error = decode_din_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 86;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=86
                    // decode: element
                    error = decode_din_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 86;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 86:
            // Grammar: ID=86; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureMethod, SignatureMethodType (SignatureMethodType)); next=87
                    // decode: element
                    error = decode_din_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                    if (error == 0)
                    {
                        grammar_id = 87;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=88
                    // decode: element
                    error = decode_din_ReferenceType(stream, &SignedInfoType->Reference);
                    if (error == 0)
                    {
                        grammar_id = 88;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 88:
            // Grammar: ID=88; read/write bits=2; START (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DC_EVStatusType(exi_bitstream_t* stream, struct din_DC_EVStatusType* DC_EVStatusType) {
    int grammar_id = 89;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DC_EVStatusType(DC_EVStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 89:
            // Grammar: ID=89; read/write bits=1; START (EVReady)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVReady, boolean (boolean)); next=90
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVReady = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 90;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=2; START (EVCabinConditioning), START (EVRESSConditioning), START (EVErrorCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVCabinConditioning, boolean (boolean)); next=91
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVCabinConditioning = value;
                                DC_EVStatusType->EVCabinConditioning_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 91;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVRESSConditioning, boolean (boolean)); next=92
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVRESSConditioning = value;
                                DC_EVStatusType->EVRESSConditioning_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (EVErrorCode, DC_EVErrorCodeType (string)); next=93
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVErrorCode = (din_DC_EVErrorCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 93;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 91:
            // Grammar: ID=91; read/write bits=2; START (EVRESSConditioning), START (EVErrorCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVRESSConditioning, boolean (boolean)); next=92
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVRESSConditioning = value;
                                DC_EVStatusType->EVRESSConditioning_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 92;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVErrorCode, DC_EVErrorCodeType (string)); next=93
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVErrorCode = (din_DC_EVErrorCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 93;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 92:
            // Grammar: ID=92; read/write bits=1; START (EVErrorCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVErrorCode, DC_EVErrorCodeType (string)); next=93
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVErrorCode = (din_DC_EVErrorCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 93;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 93:
            // Grammar: ID=93; read/write bits=1; START (EVRESSSOC)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVRESSSOC, percentValueType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVStatusType->EVRESSSOC = (int8_t)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SignatureValueType(exi_bitstream_t* stream, struct din_SignatureValueType* SignatureValueType) {
    int grammar_id = 94;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SignatureValueType(SignatureValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 94:
            // Grammar: ID=94; read/write bits=2; START (Id), START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=95
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureValueType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureValueType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureValueType->Id_isUsed = 1u;
                    grammar_id = 95;
                    break;
                case 1:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=3
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], din_SignatureValueType_BYTES_SIZE);
                        if (error == 0)
                        {
                            grammar_id = 3;
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 95:
            // Grammar: ID=95; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=3
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], din_SignatureValueType_BYTES_SIZE);
                        if (error == 0)
                        {
                            grammar_id = 3;
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CertificateChainType(exi_bitstream_t* stream, struct din_CertificateChainType* CertificateChainType) {
    int grammar_id = 96;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CertificateChainType(CertificateChainType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 96:
            // Grammar: ID=96; read/write bits=1; START (Certificate)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Certificate, certificateType (base64Binary)); next=97
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateChainType->Certificate.bytesLen, &CertificateChainType->Certificate.bytes[0], din_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 97;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=2; START (SubCertificates), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SubCertificates, SubCertificatesType (SubCertificatesType)); next=3
                    // decode: element
                    error = decode_din_SubCertificatesType(stream, &CertificateChainType->SubCertificates);
                    if (error == 0)
                    {
                        CertificateChainType->SubCertificates_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DC_EVSEStatusType(exi_bitstream_t* stream, struct din_DC_EVSEStatusType* DC_EVSEStatusType) {
    int grammar_id = 98;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DC_EVSEStatusType(DC_EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 98:
            // Grammar: ID=98; read/write bits=2; START (EVSEIsolationStatus), START (EVSEStatusCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEIsolationStatus, isolationLevelType (string)); next=99
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                DC_EVSEStatusType->EVSEIsolationStatus = (din_isolationLevelType)value;
                                DC_EVSEStatusType->EVSEIsolationStatus_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 99;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVSEStatusCode, DC_EVSEStatusCodeType (string)); next=100
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                DC_EVSEStatusType->EVSEStatusCode = (din_DC_EVSEStatusCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 100;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=1; START (EVSEStatusCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEStatusCode, DC_EVSEStatusCodeType (string)); next=100
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                DC_EVSEStatusType->EVSEStatusCode = (din_DC_EVSEStatusCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 100;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 100:
            // Grammar: ID=100; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedInt (unsignedLong)); next=101
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DC_EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 101;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 101:
            // Grammar: ID=101; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, EVSENotificationType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                DC_EVSEStatusType->EVSENotification = (din_EVSENotificationType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PhysicalValueType(exi_bitstream_t* stream, struct din_PhysicalValueType* PhysicalValueType) {
    int grammar_id = 102;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PhysicalValueType(PhysicalValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=1; START (Multiplier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Multiplier, unitMultiplierType (byte)); next=103
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                            if (error == 0)
                            {
                                // type has min_value = -3
                                PhysicalValueType->Multiplier = (int8_t)(value + -3);
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 103;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 103:
            // Grammar: ID=103; read/write bits=2; START (Unit), START (Value)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Unit, unitSymbolType (string)); next=104
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                PhysicalValueType->Unit = (din_unitSymbolType)value;
                                PhysicalValueType->Unit_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 104;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (Value, short (int)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &PhysicalValueType->Value);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 104:
            // Grammar: ID=104; read/write bits=1; START (Value)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Value, short (int)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &PhysicalValueType->Value);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ParameterType(exi_bitstream_t* stream, struct din_ParameterType* ParameterType) {
    int grammar_id = 105;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ParameterType(ParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 105:
            // Grammar: ID=105; read/write bits=1; START (Name)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Name, string (string)); next=106
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ParameterType->Name.charactersLen);
                    if (error == 0)
                    {
                        if (ParameterType->Name.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ParameterType->Name.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ParameterType->Name.charactersLen, ParameterType->Name.characters, din_Name_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 106;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 106:
            // Grammar: ID=106; read/write bits=1; START (ValueType)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ValueType, valueType (string)); next=107
                    // decode: enum (Attribute)
                    {
                        uint32_t value;
                        error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                        if (error == 0)
                        {
                            ParameterType->ValueType = (din_valueType)value;
                        }
                    }
                    grammar_id = 107;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 107:
            // Grammar: ID=107; read/write bits=3; START (boolValue), START (byteValue), START (shortValue), START (intValue), START (physicalValue), START (stringValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (boolValue, boolean (boolean)); next=3
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ParameterType->boolValue = value;
                                ParameterType->boolValue_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (byteValue, byte (short)); next=3
                    // decode: byte (restricted integer)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = -128
                                ParameterType->byteValue = (int8_t)(value + -128);
                                ParameterType->byteValue_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (shortValue, short (int)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ParameterType->shortValue);
                    if (error == 0)
                    {
                        ParameterType->shortValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (intValue, int (long)); next=3
                    // decode: int
                    error = decode_exi_type_integer32(stream, &ParameterType->intValue);
                    if (error == 0)
                    {
                        ParameterType->intValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (physicalValue, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &ParameterType->physicalValue);
                    if (error == 0)
                    {
                        ParameterType->physicalValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (stringValue, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ParameterType->stringValue.charactersLen);
                            if (error == 0)
                            {
                                if (ParameterType->stringValue.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ParameterType->stringValue.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ParameterType->stringValue.charactersLen, ParameterType->stringValue.characters, din_stringValue_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ParameterType->stringValue_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ParameterSetType(exi_bitstream_t* stream, struct din_ParameterSetType* ParameterSetType) {
    int grammar_id = 108;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ParameterSetType(ParameterSetType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 108:
            // Grammar: ID=108; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ParameterSetID, short (int)); next=109
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ParameterSetType->ParameterSetID);
                    if (error == 0)
                    {
                        grammar_id = 109;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=1; START (Parameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Parameter, ParameterType (ParameterType)); next=110
                    // decode: element
                    error = decode_din_ParameterType(stream, &ParameterSetType->Parameter);
                    if (error == 0)
                    {
                        grammar_id = 110;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 110:
            // Grammar: ID=110; read/write bits=2; START (Parameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Parameter, ParameterType (ParameterType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ListOfRootCertificateIDsType(exi_bitstream_t* stream, struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    int grammar_id = 111;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ListOfRootCertificateIDsType(ListOfRootCertificateIDsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 111:
            // Grammar: ID=111; read/write bits=1; START (RootCertificateID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RootCertificateID, rootCertificateIDType (string)); next=112
                    // decode: string (len, characters) (Array)
                    if (ListOfRootCertificateIDsType->RootCertificateID.arrayLen < din_rootCertificateIDType_5_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                error = exi_basetypes_decoder_uint_16(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen);
                                if (error == 0)
                                {
                                    if (ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen >= 2)
                                    {
                                        // string tables and table partitions are not supported, so the length has to be decremented by 2
                                        ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen -= 2;
                                        error = exi_basetypes_decoder_characters(stream, ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen, ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].characters, din_RootCertificateID_CHARACTER_SIZE);
                                        if (error == 0)
                                        {
                                            ListOfRootCertificateIDsType->RootCertificateID.arrayLen++;
                                        }
                                    }
                                    else
                                    {
                                        // the string seems to be in the table, but this is not supported
                                        error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                    }
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 112;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=2; LOOP (RootCertificateID), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (RootCertificateID, rootCertificateIDType (string)); next=112
                    // decode: string (len, characters) (Array)
                    if (ListOfRootCertificateIDsType->RootCertificateID.arrayLen < din_rootCertificateIDType_5_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                error = exi_basetypes_decoder_uint_16(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen);
                                if (error == 0)
                                {
                                    if (ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen >= 2)
                                    {
                                        // string tables and table partitions are not supported, so the length has to be decremented by 2
                                        ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen -= 2;
                                        error = exi_basetypes_decoder_characters(stream, ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].charactersLen, ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen].characters, din_RootCertificateID_CHARACTER_SIZE);
                                        if (error == 0)
                                        {
                                            ListOfRootCertificateIDsType->RootCertificateID.arrayLen++;
                                        }
                                    }
                                    else
                                    {
                                        // the string seems to be in the table, but this is not supported
                                        error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                    }
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 112;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PaymentOptionsType(exi_bitstream_t* stream, struct din_PaymentOptionsType* PaymentOptionsType) {
    int grammar_id = 113;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PaymentOptionsType(PaymentOptionsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 113:
            // Grammar: ID=113; read/write bits=1; START (PaymentOption)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOption, paymentOptionType (string)); next=114
                    // decode: enum array
                    if (PaymentOptionsType->PaymentOption.arrayLen < din_paymentOptionType_2_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                                if (error == 0)
                                {
                                    PaymentOptionsType->PaymentOption.array[PaymentOptionsType->PaymentOption.arrayLen] = (din_paymentOptionType)value;
                                    PaymentOptionsType->PaymentOption.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 114;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=2; START (PaymentOption), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOption, paymentOptionType (string)); next=3
                    // decode: enum array
                    if (PaymentOptionsType->PaymentOption.arrayLen < din_paymentOptionType_2_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                                if (error == 0)
                                {
                                    PaymentOptionsType->PaymentOption.array[PaymentOptionsType->PaymentOption.arrayLen] = (din_paymentOptionType)value;
                                    PaymentOptionsType->PaymentOption.arrayLen++;
                                }
                            }
                            else
                            {
                                // second level event is not supported
                                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                            }
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SelectedServiceListType(exi_bitstream_t* stream, struct din_SelectedServiceListType* SelectedServiceListType) {
    int grammar_id = 115;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SelectedServiceListType(SelectedServiceListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 115:
            // Grammar: ID=115; read/write bits=1; START (SelectedService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedService, SelectedServiceType (SelectedServiceType)); next=116
                    // decode: element array
                    if (SelectedServiceListType->SelectedService.arrayLen < din_SelectedServiceType_16_ARRAY_SIZE)
                    {
                        error = decode_din_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedServiceListType->SelectedService.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SelectedServiceType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 116;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=2; LOOP (SelectedService), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SelectedService, SelectedServiceType (SelectedServiceType)); next=116
                    // decode: element array
                    if (SelectedServiceListType->SelectedService.arrayLen < din_SelectedServiceType_16_ARRAY_SIZE)
                    {
                        error = decode_din_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedServiceListType->SelectedService.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SelectedServiceType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 116;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_AC_EVChargeParameterType(exi_bitstream_t* stream, struct din_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    int grammar_id = 117;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_AC_EVChargeParameterType(AC_EVChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 117:
            // Grammar: ID=117; read/write bits=1; START (DepartureTime)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=118
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &AC_EVChargeParameterType->DepartureTime);
                    if (error == 0)
                    {
                        grammar_id = 118;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=1; START (EAmount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EAmount, PhysicalValueType (PhysicalValueType)); next=119
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
                    if (error == 0)
                    {
                        grammar_id = 119;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 119:
            // Grammar: ID=119; read/write bits=1; START (EVMaxVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaxVoltage, PhysicalValueType (PhysicalValueType)); next=120
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxVoltage);
                    if (error == 0)
                    {
                        grammar_id = 120;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 120:
            // Grammar: ID=120; read/write bits=1; START (EVMaxCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaxCurrent, PhysicalValueType (PhysicalValueType)); next=121
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxCurrent);
                    if (error == 0)
                    {
                        grammar_id = 121;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 121:
            // Grammar: ID=121; read/write bits=1; START (EVMinCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMinCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMinCurrent);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DC_EVChargeParameterType(exi_bitstream_t* stream, struct din_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    int grammar_id = 122;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DC_EVChargeParameterType(DC_EVChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 122:
            // Grammar: ID=122; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=123
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 123;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 123:
            // Grammar: ID=123; read/write bits=1; START (EVMaximumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=124
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 124;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 124:
            // Grammar: ID=124; read/write bits=2; START (EVMaximumPowerLimit), START (EVMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=125
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 125;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=126
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 126;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 125:
            // Grammar: ID=125; read/write bits=1; START (EVMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=126
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 126;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 126:
            // Grammar: ID=126; read/write bits=3; START (EVEnergyCapacity), START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVEnergyCapacity, PhysicalValueType (PhysicalValueType)); next=127
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyCapacity);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyCapacity_isUsed = 1u;
                        grammar_id = 127;
                    }
                    break;
                case 1:
                    // Event: START (EVEnergyRequest, PhysicalValueType (PhysicalValueType)); next=128
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyRequest_isUsed = 1u;
                        grammar_id = 128;
                    }
                    break;
                case 2:
                    // Event: START (FullSOC, percentValueType (byte)); next=129
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->FullSOC = (int8_t)value;
                                DC_EVChargeParameterType->FullSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 129;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (BulkSOC, percentValueType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->BulkSOC = (int8_t)value;
                                DC_EVChargeParameterType->BulkSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 127:
            // Grammar: ID=127; read/write bits=3; START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVEnergyRequest, PhysicalValueType (PhysicalValueType)); next=128
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyRequest_isUsed = 1u;
                        grammar_id = 128;
                    }
                    break;
                case 1:
                    // Event: START (FullSOC, percentValueType (byte)); next=129
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->FullSOC = (int8_t)value;
                                DC_EVChargeParameterType->FullSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 129;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (BulkSOC, percentValueType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->BulkSOC = (int8_t)value;
                                DC_EVChargeParameterType->BulkSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 128:
            // Grammar: ID=128; read/write bits=2; START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FullSOC, percentValueType (byte)); next=129
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->FullSOC = (int8_t)value;
                                DC_EVChargeParameterType->FullSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 129;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (BulkSOC, percentValueType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->BulkSOC = (int8_t)value;
                                DC_EVChargeParameterType->BulkSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=2; START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BulkSOC, percentValueType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 7, &value);
                            if (error == 0)
                            {
                                DC_EVChargeParameterType->BulkSOC = (int8_t)value;
                                DC_EVChargeParameterType->BulkSOC_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_EVChargeParameterType(exi_bitstream_t* stream, struct din_EVChargeParameterType* EVChargeParameterType) {
    // Element has no particles, so the function just decodes END Element
    (void)EVChargeParameterType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingProfile; type={urn:din:70121:2012:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); ProfileEntry, ProfileEntryType (1, 24) (original max unbounded);
static int decode_din_ChargingProfileType(exi_bitstream_t* stream, struct din_ChargingProfileType* ChargingProfileType) {
    int grammar_id = 130;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ChargingProfileType(ChargingProfileType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 130:
            // Grammar: ID=130; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (short)); next=131
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ChargingProfileType->SAScheduleTupleID);
                    if (error == 0)
                    {
                        grammar_id = 131;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=1; START (ProfileEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ProfileEntry, ProfileEntryType (ProfileEntryType)); next=132
                    // decode: element array
                    if (ChargingProfileType->ProfileEntry.arrayLen < din_ProfileEntryType_24_ARRAY_SIZE)
                    {
                        error = decode_din_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ChargingProfileType->ProfileEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_ProfileEntryType_24_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 132;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 132:
            // Grammar: ID=132; read/write bits=2; LOOP (ProfileEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ProfileEntry, ProfileEntryType (ProfileEntryType)); next=132
                    // decode: element array
                    if (ChargingProfileType->ProfileEntry.arrayLen < din_ProfileEntryType_24_ARRAY_SIZE)
                    {
                        error = decode_din_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ChargingProfileType->ProfileEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_ProfileEntryType_24_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 132;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_EVSEStatusType(exi_bitstream_t* stream, struct din_EVSEStatusType* EVSEStatusType) {
    // Element has no particles, so the function just decodes END Element
    (void)EVSEStatusType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int decode_din_KeyInfoType(exi_bitstream_t* stream, struct din_KeyInfoType* KeyInfoType) {
    int grammar_id = 133;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_KeyInfoType(KeyInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 133:
            // Grammar: ID=133; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=134
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (KeyInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            KeyInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    KeyInfoType->Id_isUsed = 1u;
                    grammar_id = 134;
                    break;
                case 1:
                    // Event: START (KeyName, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->KeyName.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->KeyName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->KeyName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, din_KeyName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->KeyName_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (KeyValue, KeyValueType (KeyValueType)); next=3
                    // decode: element
                    error = decode_din_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=3
                    // decode: element
                    error = decode_din_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=3
                    // decode: element
                    error = decode_din_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=3
                    // decode: element
                    error = decode_din_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 6:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=3
                    // decode: element
                    error = decode_din_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == 0)
                    {
                        KeyInfoType->SPKIData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 7:
                    // Event: START (MgmtData, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->MgmtData.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->MgmtData.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->MgmtData.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, din_MgmtData_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->MgmtData_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 8:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyInfoType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 134:
            // Grammar: ID=134; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyName, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->KeyName.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->KeyName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->KeyName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, din_KeyName_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->KeyName_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (KeyValue, KeyValueType (KeyValueType)); next=3
                    // decode: element
                    error = decode_din_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=3
                    // decode: element
                    error = decode_din_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=3
                    // decode: element
                    error = decode_din_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=3
                    // decode: element
                    error = decode_din_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=3
                    // decode: element
                    error = decode_din_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == 0)
                    {
                        KeyInfoType->SPKIData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 6:
                    // Event: START (MgmtData, string (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->MgmtData.charactersLen);
                            if (error == 0)
                            {
                                if (KeyInfoType->MgmtData.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    KeyInfoType->MgmtData.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, din_MgmtData_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                KeyInfoType->MgmtData_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 7:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        KeyInfoType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceChargeType(exi_bitstream_t* stream, struct din_ServiceChargeType* ServiceChargeType) {
    int grammar_id = 135;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceChargeType(ServiceChargeType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 135:
            // Grammar: ID=135; read/write bits=1; START (ServiceTag)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceTag, ServiceTagType (ServiceTagType)); next=136
                    // decode: element
                    error = decode_din_ServiceTagType(stream, &ServiceChargeType->ServiceTag);
                    if (error == 0)
                    {
                        grammar_id = 136;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 136:
            // Grammar: ID=136; read/write bits=1; START (FreeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FreeService, boolean (boolean)); next=137
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ServiceChargeType->FreeService = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 137;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (EnergyTransferType)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EnergyTransferType, EVSESupportedEnergyTransferType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 4, &value);
                            if (error == 0)
                            {
                                ServiceChargeType->EnergyTransferType = (din_EVSESupportedEnergyTransferType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceParameterListType(exi_bitstream_t* stream, struct din_ServiceParameterListType* ServiceParameterListType) {
    int grammar_id = 138;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceParameterListType(ServiceParameterListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 138:
            // Grammar: ID=138; read/write bits=1; START (ParameterSet)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ParameterSet, ParameterSetType (ParameterSetType)); next=139
                    // decode: element array
                    if (ServiceParameterListType->ParameterSet.arrayLen < din_ParameterSetType_5_ARRAY_SIZE)
                    {
                        error = decode_din_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ServiceParameterListType->ParameterSet.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_ParameterSetType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 139;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 139:
            // Grammar: ID=139; read/write bits=2; LOOP (ParameterSet), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ParameterSet, ParameterSetType (ParameterSetType)); next=139
                    // decode: element array
                    if (ServiceParameterListType->ParameterSet.arrayLen < din_ParameterSetType_5_ARRAY_SIZE)
                    {
                        error = decode_din_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ServiceParameterListType->ParameterSet.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_ParameterSetType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 139;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SAScheduleListType(exi_bitstream_t* stream, struct din_SAScheduleListType* SAScheduleListType) {
    int grammar_id = 140;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SAScheduleListType(SAScheduleListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 140:
            // Grammar: ID=140; read/write bits=1; START (SAScheduleTuple)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTuple, SAScheduleTupleType (SAScheduleTupleType)); next=141
                    // decode: element array
                    if (SAScheduleListType->SAScheduleTuple.arrayLen < din_SAScheduleTupleType_5_ARRAY_SIZE)
                    {
                        error = decode_din_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleListType->SAScheduleTuple.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SAScheduleTupleType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 141;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=2; LOOP (SAScheduleTuple), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SAScheduleTuple, SAScheduleTupleType (SAScheduleTupleType)); next=141
                    // decode: element array
                    if (SAScheduleListType->SAScheduleTuple.arrayLen < din_SAScheduleTupleType_5_ARRAY_SIZE)
                    {
                        error = decode_din_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleListType->SAScheduleTuple.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only din_SAScheduleTupleType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 141;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SASchedulesType(exi_bitstream_t* stream, struct din_SASchedulesType* SASchedulesType) {
    // Element has no particles, so the function just decodes END Element
    (void)SASchedulesType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:din:70121:2012:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
static int decode_din_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    int grammar_id = 142;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DC_EVPowerDeliveryParameterType(DC_EVPowerDeliveryParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 142:
            // Grammar: ID=142; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=143
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &DC_EVPowerDeliveryParameterType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 143;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=144
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVPowerDeliveryParameterType->BulkChargingComplete = value;
                                DC_EVPowerDeliveryParameterType->BulkChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 144;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ChargingComplete, boolean (boolean)); next=3
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVPowerDeliveryParameterType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingComplete, boolean (boolean)); next=3
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                DC_EVPowerDeliveryParameterType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
    // Element has no particles, so the function just decodes END Element
    (void)EVPowerDeliveryParameterType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int decode_din_ObjectType(exi_bitstream_t* stream, struct din_ObjectType* ObjectType) {
    int grammar_id = 145;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ObjectType(ObjectType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 145:
            // Grammar: ID=145; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Encoding, anyURI (anyURI)); next=146
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Encoding.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Encoding.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Encoding.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, din_Encoding_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Encoding_isUsed = 1u;
                    grammar_id = 146;
                    break;
                case 1:
                    // Event: START (Id, ID (NCName)); next=147
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 147;
                    break;
                case 2:
                    // Event: START (MimeType, string (string)); next=148
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 148;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 4:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 146:
            // Grammar: ID=146; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=147
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 147;
                    break;
                case 1:
                    // Event: START (MimeType, string (string)); next=148
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 148;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 4:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 147:
            // Grammar: ID=147; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MimeType, string (string)); next=148
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, din_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 148;
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 148:
            // Grammar: ID=148; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], din_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        ObjectType->ANY_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceTagListType(exi_bitstream_t* stream, struct din_ServiceTagListType* ServiceTagListType) {
    int grammar_id = 149;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceTagListType(ServiceTagListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 149:
            // Grammar: ID=149; read/write bits=1; START (Service)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Service, ServiceType (ServiceType)); next=150
                    // decode: element
                    error = decode_din_ServiceType(stream, &ServiceTagListType->Service);
                    if (error == 0)
                    {
                        grammar_id = 150;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=2; START (Service), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Service, ServiceType (ServiceType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_DC_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    int grammar_id = 151;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_DC_EVSEChargeParameterType(DC_EVSEChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=152
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &DC_EVSEChargeParameterType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 152;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 152:
            // Grammar: ID=152; read/write bits=1; START (EVSEMaximumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=153
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 153;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 153:
            // Grammar: ID=153; read/write bits=2; START (EVSEMaximumPowerLimit), START (EVSEMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=154
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        DC_EVSEChargeParameterType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 154;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=155
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 155;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=1; START (EVSEMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=155
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 155;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=1; START (EVSEMinimumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=156
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 156;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 156:
            // Grammar: ID=156; read/write bits=1; START (EVSEMinimumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=157
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 157;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 157:
            // Grammar: ID=157; read/write bits=2; START (EVSECurrentRegulationTolerance), START (EVSEPeakCurrentRipple)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSECurrentRegulationTolerance, PhysicalValueType (PhysicalValueType)); next=158
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance);
                    if (error == 0)
                    {
                        DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed = 1u;
                        grammar_id = 158;
                    }
                    break;
                case 1:
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType (PhysicalValueType)); next=159
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                    if (error == 0)
                    {
                        grammar_id = 159;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=1; START (EVSEPeakCurrentRipple)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType (PhysicalValueType)); next=159
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                    if (error == 0)
                    {
                        grammar_id = 159;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 159:
            // Grammar: ID=159; read/write bits=2; START (EVSEEnergyToBeDelivered), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEEnergyToBeDelivered, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered);
                    if (error == 0)
                    {
                        DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_AC_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    int grammar_id = 160;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_AC_EVSEChargeParameterType(AC_EVSEChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 160:
            // Grammar: ID=160; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=161
                    // decode: element
                    error = decode_din_AC_EVSEStatusType(stream, &AC_EVSEChargeParameterType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 161;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 161:
            // Grammar: ID=161; read/write bits=1; START (EVSEMaxVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaxVoltage, PhysicalValueType (PhysicalValueType)); next=162
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxVoltage);
                    if (error == 0)
                    {
                        grammar_id = 162;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 162:
            // Grammar: ID=162; read/write bits=1; START (EVSEMaxCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaxCurrent, PhysicalValueType (PhysicalValueType)); next=163
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxCurrent);
                    if (error == 0)
                    {
                        grammar_id = 163;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=1; START (EVSEMinCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMinCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMinCurrent);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_EVSEChargeParameterType(exi_bitstream_t* stream, struct din_EVSEChargeParameterType* EVSEChargeParameterType) {
    // Element has no particles, so the function just decodes END Element
    (void)EVSEChargeParameterType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}MeterInfo; type={urn:din:70121:2012:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, PhysicalValueType (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
static int decode_din_MeterInfoType(exi_bitstream_t* stream, struct din_MeterInfoType* MeterInfoType) {
    int grammar_id = 164;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_MeterInfoType(MeterInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 164:
            // Grammar: ID=164; read/write bits=1; START (MeterID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterID, meterIDType (string)); next=165
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &MeterInfoType->MeterID.charactersLen);
                            if (error == 0)
                            {
                                if (MeterInfoType->MeterID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    MeterInfoType->MeterID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, din_MeterID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 165;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=3; START (MeterReading), START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterReading, PhysicalValueType (PhysicalValueType)); next=166
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &MeterInfoType->MeterReading);
                    if (error == 0)
                    {
                        MeterInfoType->MeterReading_isUsed = 1u;
                        grammar_id = 166;
                    }
                    break;
                case 1:
                    // Event: START (SigMeterReading, sigMeterReadingType (base64Binary)); next=167
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->SigMeterReading.bytesLen, &MeterInfoType->SigMeterReading.bytes[0], din_sigMeterReadingType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->SigMeterReading_isUsed = 1u;
                        grammar_id = 167;
                    }
                    break;
                case 2:
                    // Event: START (MeterStatus, meterStatusType (short)); next=168
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 168;
                    }
                    break;
                case 3:
                    // Event: START (TMeter, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &MeterInfoType->TMeter);
                    if (error == 0)
                    {
                        MeterInfoType->TMeter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 166:
            // Grammar: ID=166; read/write bits=3; START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SigMeterReading, sigMeterReadingType (base64Binary)); next=167
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->SigMeterReading.bytesLen, &MeterInfoType->SigMeterReading.bytes[0], din_sigMeterReadingType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->SigMeterReading_isUsed = 1u;
                        grammar_id = 167;
                    }
                    break;
                case 1:
                    // Event: START (MeterStatus, meterStatusType (short)); next=168
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 168;
                    }
                    break;
                case 2:
                    // Event: START (TMeter, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &MeterInfoType->TMeter);
                    if (error == 0)
                    {
                        MeterInfoType->TMeter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=2; START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterStatus, meterStatusType (short)); next=168
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 168;
                    }
                    break;
                case 1:
                    // Event: START (TMeter, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &MeterInfoType->TMeter);
                    if (error == 0)
                    {
                        MeterInfoType->TMeter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=2; START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (TMeter, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &MeterInfoType->TMeter);
                    if (error == 0)
                    {
                        MeterInfoType->TMeter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CertificateInstallationResType(exi_bitstream_t* stream, struct din_CertificateInstallationResType* CertificateInstallationResType) {
    int grammar_id = 169;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CertificateInstallationResType(CertificateInstallationResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 169:
            // Grammar: ID=169; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=170
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateInstallationResType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateInstallationResType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateInstallationResType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateInstallationResType->Id.charactersLen, CertificateInstallationResType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 170;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=171
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                CertificateInstallationResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 171;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=172
                    // decode: element
                    error = decode_din_CertificateChainType(stream, &CertificateInstallationResType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 172;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureEncryptedPrivateKey, privateKeyType (base64Binary)); next=173
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationResType->ContractSignatureEncryptedPrivateKey.bytesLen, &CertificateInstallationResType->ContractSignatureEncryptedPrivateKey.bytes[0], din_privateKeyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 173;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 173:
            // Grammar: ID=173; read/write bits=1; START (DHParams)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHParams, dHParamsType (base64Binary)); next=174
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationResType->DHParams.bytesLen, &CertificateInstallationResType->DHParams.bytes[0], din_dHParamsType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 174;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=1; START (ContractID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractID, contractIDType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CertificateInstallationResType->ContractID.charactersLen);
                            if (error == 0)
                            {
                                if (CertificateInstallationResType->ContractID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CertificateInstallationResType->ContractID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CertificateInstallationResType->ContractID.charactersLen, CertificateInstallationResType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CableCheckReqType(exi_bitstream_t* stream, struct din_CableCheckReqType* CableCheckReqType) {
    int grammar_id = 175;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CableCheckReqType(CableCheckReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=3
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &CableCheckReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CableCheckResType(exi_bitstream_t* stream, struct din_CableCheckResType* CableCheckResType) {
    int grammar_id = 176;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CableCheckResType(CableCheckResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 176:
            // Grammar: ID=176; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=177
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                CableCheckResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 177;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 177:
            // Grammar: ID=177; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=178
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &CableCheckResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 178;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, EVSEProcessingType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CableCheckResType->EVSEProcessing = (din_EVSEProcessingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PreChargeReqType(exi_bitstream_t* stream, struct din_PreChargeReqType* PreChargeReqType) {
    int grammar_id = 179;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PreChargeReqType(PreChargeReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 179:
            // Grammar: ID=179; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=180
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &PreChargeReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 180;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 180:
            // Grammar: ID=180; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=181
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &PreChargeReqType->EVTargetVoltage);
                    if (error == 0)
                    {
                        grammar_id = 181;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 181:
            // Grammar: ID=181; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &PreChargeReqType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PreChargeResType(exi_bitstream_t* stream, struct din_PreChargeResType* PreChargeResType) {
    int grammar_id = 182;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PreChargeResType(PreChargeResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 182:
            // Grammar: ID=182; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=183
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                PreChargeResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 183;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=184
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &PreChargeResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 184;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &PreChargeResType->EVSEPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CurrentDemandReqType(exi_bitstream_t* stream, struct din_CurrentDemandReqType* CurrentDemandReqType) {
    int grammar_id = 185;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CurrentDemandReqType(CurrentDemandReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 185:
            // Grammar: ID=185; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=186
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &CurrentDemandReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 186;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, PhysicalValueType (PhysicalValueType)); next=187
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetCurrent);
                    if (error == 0)
                    {
                        grammar_id = 187;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 187:
            // Grammar: ID=187; read/write bits=3; START (EVMaximumVoltageLimit), START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=188
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumVoltageLimit_isUsed = 1u;
                        grammar_id = 188;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=189
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=190
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 190;
                    }
                    break;
                case 3:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=191
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->BulkChargingComplete = value;
                                CurrentDemandReqType->BulkChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (ChargingComplete, boolean (boolean)); next=192
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 192;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=3; START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=189
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 189;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=190
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 190;
                    }
                    break;
                case 2:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=191
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->BulkChargingComplete = value;
                                CurrentDemandReqType->BulkChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (ChargingComplete, boolean (boolean)); next=192
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 192;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=2; START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=190
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 190;
                    }
                    break;
                case 1:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=191
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->BulkChargingComplete = value;
                                CurrentDemandReqType->BulkChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (ChargingComplete, boolean (boolean)); next=192
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 192;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=191
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->BulkChargingComplete = value;
                                CurrentDemandReqType->BulkChargingComplete_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 191;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ChargingComplete, boolean (boolean)); next=192
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 192;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingComplete, boolean (boolean)); next=192
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandReqType->ChargingComplete = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 192;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=2; START (RemainingTimeToFullSoC), START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToFullSoC, PhysicalValueType (PhysicalValueType)); next=193
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToFullSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToFullSoC_isUsed = 1u;
                        grammar_id = 193;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType (PhysicalValueType)); next=194
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 1u;
                        grammar_id = 194;
                    }
                    break;
                case 2:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 193:
            // Grammar: ID=193; read/write bits=2; START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType (PhysicalValueType)); next=194
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 1u;
                        grammar_id = 194;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 194:
            // Grammar: ID=194; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CurrentDemandResType(exi_bitstream_t* stream, struct din_CurrentDemandResType* CurrentDemandResType) {
    int grammar_id = 195;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CurrentDemandResType(CurrentDemandResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 195:
            // Grammar: ID=195; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=196
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                CurrentDemandResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 196;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 196:
            // Grammar: ID=196; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=197
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &CurrentDemandResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 197;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 197:
            // Grammar: ID=197; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=198
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 198;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=1; START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentCurrent, PhysicalValueType (PhysicalValueType)); next=199
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 199;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=1; START (EVSECurrentLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSECurrentLimitAchieved, boolean (boolean)); next=200
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandResType->EVSECurrentLimitAchieved = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 200;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=1; START (EVSEVoltageLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEVoltageLimitAchieved, boolean (boolean)); next=201
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandResType->EVSEVoltageLimitAchieved = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 201;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 201:
            // Grammar: ID=201; read/write bits=1; START (EVSEPowerLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPowerLimitAchieved, boolean (boolean)); next=202
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                CurrentDemandResType->EVSEPowerLimitAchieved = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 202;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 202:
            // Grammar: ID=202; read/write bits=3; START (EVSEMaximumVoltageLimit), START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=203
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumVoltageLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed = 1u;
                        grammar_id = 203;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=204
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 204;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=2; START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=204
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 204;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=2; START (EVSEMaximumPowerLimit), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_WeldingDetectionReqType(exi_bitstream_t* stream, struct din_WeldingDetectionReqType* WeldingDetectionReqType) {
    int grammar_id = 205;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_WeldingDetectionReqType(WeldingDetectionReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 205:
            // Grammar: ID=205; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=3
                    // decode: element
                    error = decode_din_DC_EVStatusType(stream, &WeldingDetectionReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_WeldingDetectionResType(exi_bitstream_t* stream, struct din_WeldingDetectionResType* WeldingDetectionResType) {
    int grammar_id = 206;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_WeldingDetectionResType(WeldingDetectionResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 206:
            // Grammar: ID=206; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=207
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                WeldingDetectionResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 207;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=208
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &WeldingDetectionResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 208;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &WeldingDetectionResType->EVSEPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SessionSetupReqType(exi_bitstream_t* stream, struct din_SessionSetupReqType* SessionSetupReqType) {
    int grammar_id = 209;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SessionSetupReqType(SessionSetupReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 209:
            // Grammar: ID=209; read/write bits=1; START (EVCCID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVCCID, evccIDType (hexBinary)); next=3
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &SessionSetupReqType->EVCCID.bytesLen, &SessionSetupReqType->EVCCID.bytes[0], din_evccIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CertificateInstallationReqType(exi_bitstream_t* stream, struct din_CertificateInstallationReqType* CertificateInstallationReqType) {
    int grammar_id = 210;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CertificateInstallationReqType(CertificateInstallationReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 210:
            // Grammar: ID=210; read/write bits=2; START (Id), START (OEMProvisioningCert)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=211
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateInstallationReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateInstallationReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateInstallationReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateInstallationReqType->Id.charactersLen, CertificateInstallationReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    CertificateInstallationReqType->Id_isUsed = 1u;
                    grammar_id = 211;
                    break;
                case 1:
                    // Event: START (OEMProvisioningCert, certificateType (base64Binary)); next=212
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationReqType->OEMProvisioningCert.bytesLen, &CertificateInstallationReqType->OEMProvisioningCert.bytes[0], din_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 212;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 211:
            // Grammar: ID=211; read/write bits=1; START (OEMProvisioningCert)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (OEMProvisioningCert, certificateType (base64Binary)); next=212
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationReqType->OEMProvisioningCert.bytesLen, &CertificateInstallationReqType->OEMProvisioningCert.bytes[0], din_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 212;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ListOfRootCertificateIDs, ListOfRootCertificateIDsType (ListOfRootCertificateIDsType)); next=213
                    // decode: element
                    error = decode_din_ListOfRootCertificateIDsType(stream, &CertificateInstallationReqType->ListOfRootCertificateIDs);
                    if (error == 0)
                    {
                        grammar_id = 213;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=1; START (DHParams)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHParams, dHParamsType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationReqType->DHParams.bytesLen, &CertificateInstallationReqType->DHParams.bytes[0], din_dHParamsType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SessionSetupResType(exi_bitstream_t* stream, struct din_SessionSetupResType* SessionSetupResType) {
    int grammar_id = 214;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SessionSetupResType(SessionSetupResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 214:
            // Grammar: ID=214; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=215
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                SessionSetupResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 215;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 215:
            // Grammar: ID=215; read/write bits=1; START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEID, evseIDType (hexBinary)); next=216
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &SessionSetupResType->EVSEID.bytesLen, &SessionSetupResType->EVSEID.bytes[0], din_evseIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 216;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=2; START (DateTimeNow), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DateTimeNow, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &SessionSetupResType->DateTimeNow);
                    if (error == 0)
                    {
                        SessionSetupResType->DateTimeNow_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceDiscoveryReqType(exi_bitstream_t* stream, struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    int grammar_id = 217;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceDiscoveryReqType(ServiceDiscoveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 217:
            // Grammar: ID=217; read/write bits=2; START (ServiceScope), START (ServiceCategory), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceScope, serviceScopeType (string)); next=218
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ServiceDiscoveryReqType->ServiceScope.charactersLen);
                            if (error == 0)
                            {
                                if (ServiceDiscoveryReqType->ServiceScope.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ServiceDiscoveryReqType->ServiceScope.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ServiceDiscoveryReqType->ServiceScope.charactersLen, ServiceDiscoveryReqType->ServiceScope.characters, din_ServiceScope_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ServiceDiscoveryReqType->ServiceScope_isUsed = 1u;
                                grammar_id = 218;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                ServiceDiscoveryReqType->ServiceCategory = (din_serviceCategoryType)value;
                                ServiceDiscoveryReqType->ServiceCategory_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 218:
            // Grammar: ID=218; read/write bits=2; START (ServiceCategory), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                ServiceDiscoveryReqType->ServiceCategory = (din_serviceCategoryType)value;
                                ServiceDiscoveryReqType->ServiceCategory_isUsed = 1u;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceDiscoveryResType(exi_bitstream_t* stream, struct din_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    int grammar_id = 219;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceDiscoveryResType(ServiceDiscoveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 219:
            // Grammar: ID=219; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=220
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ServiceDiscoveryResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 220;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=1; START (PaymentOptions)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOptions, PaymentOptionsType (PaymentOptionsType)); next=221
                    // decode: element
                    error = decode_din_PaymentOptionsType(stream, &ServiceDiscoveryResType->PaymentOptions);
                    if (error == 0)
                    {
                        grammar_id = 221;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 221:
            // Grammar: ID=221; read/write bits=1; START (ChargeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargeService, ServiceChargeType (ServiceType)); next=222
                    // decode: element
                    error = decode_din_ServiceChargeType(stream, &ServiceDiscoveryResType->ChargeService);
                    if (error == 0)
                    {
                        grammar_id = 222;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 222:
            // Grammar: ID=222; read/write bits=2; START (ServiceList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceList, ServiceTagListType (ServiceTagListType)); next=3
                    // decode: element
                    error = decode_din_ServiceTagListType(stream, &ServiceDiscoveryResType->ServiceList);
                    if (error == 0)
                    {
                        ServiceDiscoveryResType->ServiceList_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceDetailReqType(exi_bitstream_t* stream, struct din_ServiceDetailReqType* ServiceDetailReqType) {
    int grammar_id = 223;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceDetailReqType(ServiceDetailReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 223:
            // Grammar: ID=223; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=3
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ServiceDetailReqType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServiceDetailResType(exi_bitstream_t* stream, struct din_ServiceDetailResType* ServiceDetailResType) {
    int grammar_id = 224;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServiceDetailResType(ServiceDetailResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 224:
            // Grammar: ID=224; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=225
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ServiceDetailResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 225;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=226
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ServiceDetailResType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 226;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=2; START (ServiceParameterList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceParameterList, ServiceParameterListType (ServiceParameterListType)); next=3
                    // decode: element
                    error = decode_din_ServiceParameterListType(stream, &ServiceDetailResType->ServiceParameterList);
                    if (error == 0)
                    {
                        ServiceDetailResType->ServiceParameterList_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServicePaymentSelectionReqType(exi_bitstream_t* stream, struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType) {
    int grammar_id = 227;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServicePaymentSelectionReqType(ServicePaymentSelectionReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (SelectedPaymentOption)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedPaymentOption, paymentOptionType (string)); next=228
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ServicePaymentSelectionReqType->SelectedPaymentOption = (din_paymentOptionType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 228;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=1; START (SelectedServiceList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedServiceList, SelectedServiceListType (SelectedServiceListType)); next=3
                    // decode: element
                    error = decode_din_SelectedServiceListType(stream, &ServicePaymentSelectionReqType->SelectedServiceList);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ServicePaymentSelectionResType(exi_bitstream_t* stream, struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType) {
    int grammar_id = 229;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ServicePaymentSelectionResType(ServicePaymentSelectionResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 229:
            // Grammar: ID=229; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ServicePaymentSelectionResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PaymentDetailsReqType(exi_bitstream_t* stream, struct din_PaymentDetailsReqType* PaymentDetailsReqType) {
    int grammar_id = 230;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PaymentDetailsReqType(PaymentDetailsReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 230:
            // Grammar: ID=230; read/write bits=1; START (ContractID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractID, contractIDType (string)); next=231
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &PaymentDetailsReqType->ContractID.charactersLen);
                            if (error == 0)
                            {
                                if (PaymentDetailsReqType->ContractID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    PaymentDetailsReqType->ContractID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, PaymentDetailsReqType->ContractID.charactersLen, PaymentDetailsReqType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 231;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=3
                    // decode: element
                    error = decode_din_CertificateChainType(stream, &PaymentDetailsReqType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PaymentDetailsResType(exi_bitstream_t* stream, struct din_PaymentDetailsResType* PaymentDetailsResType) {
    int grammar_id = 232;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PaymentDetailsResType(PaymentDetailsResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 232:
            // Grammar: ID=232; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=233
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                PaymentDetailsResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 233;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (GenChallenge, genChallengeType (string)); next=234
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &PaymentDetailsResType->GenChallenge.charactersLen);
                            if (error == 0)
                            {
                                if (PaymentDetailsResType->GenChallenge.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    PaymentDetailsResType->GenChallenge.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, PaymentDetailsResType->GenChallenge.charactersLen, PaymentDetailsResType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 234;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=1; START (DateTimeNow)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DateTimeNow, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &PaymentDetailsResType->DateTimeNow);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ContractAuthenticationReqType(exi_bitstream_t* stream, struct din_ContractAuthenticationReqType* ContractAuthenticationReqType) {
    int grammar_id = 235;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ContractAuthenticationReqType(ContractAuthenticationReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 235:
            // Grammar: ID=235; read/write bits=2; START (Id), START (GenChallenge), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=236
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ContractAuthenticationReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ContractAuthenticationReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ContractAuthenticationReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ContractAuthenticationReqType->Id.charactersLen, ContractAuthenticationReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ContractAuthenticationReqType->Id_isUsed = 1u;
                    grammar_id = 236;
                    break;
                case 1:
                    // Event: START (GenChallenge, genChallengeType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ContractAuthenticationReqType->GenChallenge.charactersLen);
                            if (error == 0)
                            {
                                if (ContractAuthenticationReqType->GenChallenge.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ContractAuthenticationReqType->GenChallenge.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ContractAuthenticationReqType->GenChallenge.charactersLen, ContractAuthenticationReqType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ContractAuthenticationReqType->GenChallenge_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 236:
            // Grammar: ID=236; read/write bits=2; START (GenChallenge), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (GenChallenge, genChallengeType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ContractAuthenticationReqType->GenChallenge.charactersLen);
                            if (error == 0)
                            {
                                if (ContractAuthenticationReqType->GenChallenge.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ContractAuthenticationReqType->GenChallenge.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ContractAuthenticationReqType->GenChallenge.charactersLen, ContractAuthenticationReqType->GenChallenge.characters, din_GenChallenge_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                ContractAuthenticationReqType->GenChallenge_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ContractAuthenticationResType(exi_bitstream_t* stream, struct din_ContractAuthenticationResType* ContractAuthenticationResType) {
    int grammar_id = 237;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ContractAuthenticationResType(ContractAuthenticationResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 237:
            // Grammar: ID=237; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=238
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ContractAuthenticationResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 238;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 238:
            // Grammar: ID=238; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, EVSEProcessingType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ContractAuthenticationResType->EVSEProcessing = (din_EVSEProcessingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    int grammar_id = 239;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ChargeParameterDiscoveryReqType(ChargeParameterDiscoveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 239:
            // Grammar: ID=239; read/write bits=1; START (EVRequestedEnergyTransferType)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVRequestedEnergyTransferType, EVRequestedEnergyTransferType (string)); next=240
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                            if (error == 0)
                            {
                                ChargeParameterDiscoveryReqType->EVRequestedEnergyTransferType = (din_EVRequestedEnergyTransferType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 240;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=2; START (AC_EVChargeParameter), START (DC_EVChargeParameter), START (EVChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVChargeParameter, AC_EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_din_AC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->AC_EVChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVChargeParameter, DC_EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_din_DC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->DC_EVChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->DC_EVChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVChargeParameter, EVChargeParameterType (EVChargeParameterType)
                    // decode: element
                    error = decode_din_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->EVChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->EVChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    int grammar_id = 241;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ChargeParameterDiscoveryResType(ChargeParameterDiscoveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 241:
            // Grammar: ID=241; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=242
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ChargeParameterDiscoveryResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 242;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 242:
            // Grammar: ID=242; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, EVSEProcessingType (string)); next=243
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ChargeParameterDiscoveryResType->EVSEProcessing = (din_EVSEProcessingType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 243;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=2; START (SAScheduleList), START (SASchedules)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleList, SAScheduleListType (SASchedulesType)); next=244
                    // decode: element
                    error = decode_din_SAScheduleListType(stream, &ChargeParameterDiscoveryResType->SAScheduleList);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->SAScheduleList_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                case 1:
                    // Abstract element or type: SASchedules, SASchedulesType (SASchedulesType)
                    // decode: element
                    error = decode_din_SASchedulesType(stream, &ChargeParameterDiscoveryResType->SASchedules);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->SASchedules_isUsed = 1u;
                        grammar_id = 244;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=2; START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEChargeParameter, AC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_din_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVSEChargeParameter, DC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_din_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVSEChargeParameter, EVSEChargeParameterType (EVSEChargeParameterType)
                    // decode: element
                    error = decode_din_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PowerDeliveryReqType(exi_bitstream_t* stream, struct din_PowerDeliveryReqType* PowerDeliveryReqType) {
    int grammar_id = 245;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PowerDeliveryReqType(PowerDeliveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 245:
            // Grammar: ID=245; read/write bits=1; START (ReadyToChargeState)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ReadyToChargeState, boolean (boolean)); next=246
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                PowerDeliveryReqType->ReadyToChargeState = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 246;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=3; START (ChargingProfile), START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfile, ChargingProfileType (ChargingProfileType)); next=247
                    // decode: element
                    error = decode_din_ChargingProfileType(stream, &PowerDeliveryReqType->ChargingProfile);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->ChargingProfile_isUsed = 1u;
                        grammar_id = 247;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)); next=3
                    // decode: element
                    error = decode_din_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVPowerDeliveryParameter, EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)
                    // decode: element
                    error = decode_din_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 247:
            // Grammar: ID=247; read/write bits=2; START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)); next=3
                    // decode: element
                    error = decode_din_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Abstract element or type: EVPowerDeliveryParameter, EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)
                    // decode: element
                    error = decode_din_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_PowerDeliveryResType(exi_bitstream_t* stream, struct din_PowerDeliveryResType* PowerDeliveryResType) {
    int grammar_id = 248;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_PowerDeliveryResType(PowerDeliveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 248:
            // Grammar: ID=248; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=249
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                PowerDeliveryResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 249;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_din_AC_EVSEStatusType(stream, &PowerDeliveryResType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        PowerDeliveryResType->AC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_din_DC_EVSEStatusType(stream, &PowerDeliveryResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        PowerDeliveryResType->DC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVSEStatus, EVSEStatusType (EVSEStatusType)
                    // decode: element
                    error = decode_din_EVSEStatusType(stream, &PowerDeliveryResType->EVSEStatus);
                    if (error == 0)
                    {
                        PowerDeliveryResType->EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_ChargingStatusReqType(exi_bitstream_t* stream, struct din_ChargingStatusReqType* ChargingStatusReqType) {
    // Element has no particles, so the function just decodes END Element
    (void)ChargingStatusReqType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}ChargingStatusRes; type={urn:din:70121:2012:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
static int decode_din_ChargingStatusResType(exi_bitstream_t* stream, struct din_ChargingStatusResType* ChargingStatusResType) {
    int grammar_id = 250;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_ChargingStatusResType(ChargingStatusResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 250:
            // Grammar: ID=250; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=251
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                ChargingStatusResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 251;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=1; START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEID, evseIDType (hexBinary)); next=252
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &ChargingStatusResType->EVSEID.bytesLen, &ChargingStatusResType->EVSEID.bytes[0], din_evseIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 252;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (short)); next=253
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ChargingStatusResType->SAScheduleTupleID);
                    if (error == 0)
                    {
                        grammar_id = 253;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=2; START (EVSEMaxCurrent), START (MeterInfo), START (ReceiptRequired)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaxCurrent, PhysicalValueType (PhysicalValueType)); next=254
                    // decode: element
                    error = decode_din_PhysicalValueType(stream, &ChargingStatusResType->EVSEMaxCurrent);
                    if (error == 0)
                    {
                        ChargingStatusResType->EVSEMaxCurrent_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=255
                    // decode: element
                    error = decode_din_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == 0)
                    {
                        ChargingStatusResType->MeterInfo_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 2:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=256
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ChargingStatusResType->ReceiptRequired = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=2; START (MeterInfo), START (ReceiptRequired)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=255
                    // decode: element
                    error = decode_din_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == 0)
                    {
                        ChargingStatusResType->MeterInfo_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 1:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=256
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ChargingStatusResType->ReceiptRequired = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=1; START (ReceiptRequired)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=256
                    // decode: boolean
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 1, &value);
                            if (error == 0)
                            {
                                ChargingStatusResType->ReceiptRequired = value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_din_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_MeteringReceiptReqType(exi_bitstream_t* stream, struct din_MeteringReceiptReqType* MeteringReceiptReqType) {
    int grammar_id = 257;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_MeteringReceiptReqType(MeteringReceiptReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 257:
            // Grammar: ID=257; read/write bits=2; START (Id), START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=258
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &MeteringReceiptReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (MeteringReceiptReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            MeteringReceiptReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, MeteringReceiptReqType->Id.charactersLen, MeteringReceiptReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    MeteringReceiptReqType->Id_isUsed = 1u;
                    grammar_id = 258;
                    break;
                case 1:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=259
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MeteringReceiptReqType->SessionID.bytesLen, &MeteringReceiptReqType->SessionID.bytes[0], din_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 259;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=259
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MeteringReceiptReqType->SessionID.bytesLen, &MeteringReceiptReqType->SessionID.bytes[0], din_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 259;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=2; START (SAScheduleTupleID), START (MeterInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (short)); next=260
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeteringReceiptReqType->SAScheduleTupleID);
                    if (error == 0)
                    {
                        MeteringReceiptReqType->SAScheduleTupleID_isUsed = 1u;
                        grammar_id = 260;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=3
                    // decode: element
                    error = decode_din_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=1; START (MeterInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=3
                    // decode: element
                    error = decode_din_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_MeteringReceiptResType(exi_bitstream_t* stream, struct din_MeteringReceiptResType* MeteringReceiptResType) {
    int grammar_id = 261;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_MeteringReceiptResType(MeteringReceiptResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 261:
            // Grammar: ID=261; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=262
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                MeteringReceiptResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 262;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_din_AC_EVSEStatusType(stream, &MeteringReceiptResType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SessionStopType(exi_bitstream_t* stream, struct din_SessionStopType* SessionStopType) {
    // Element has no particles, so the function just decodes END Element
    (void)SessionStopType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgBody}SessionStopRes; type={urn:din:70121:2012:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int decode_din_SessionStopResType(exi_bitstream_t* stream, struct din_SessionStopResType* SessionStopResType) {
    int grammar_id = 263;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SessionStopResType(SessionStopResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 263:
            // Grammar: ID=263; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=3
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                SessionStopResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CertificateUpdateReqType(exi_bitstream_t* stream, struct din_CertificateUpdateReqType* CertificateUpdateReqType) {
    int grammar_id = 264;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CertificateUpdateReqType(CertificateUpdateReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 264:
            // Grammar: ID=264; read/write bits=2; START (Id), START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=265
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateUpdateReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateUpdateReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateUpdateReqType->Id.charactersLen, CertificateUpdateReqType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    CertificateUpdateReqType->Id_isUsed = 1u;
                    grammar_id = 265;
                    break;
                case 1:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=266
                    // decode: element
                    error = decode_din_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 266;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=266
                    // decode: element
                    error = decode_din_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 266;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (ContractID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractID, contractIDType (string)); next=267
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateReqType->ContractID.charactersLen);
                            if (error == 0)
                            {
                                if (CertificateUpdateReqType->ContractID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CertificateUpdateReqType->ContractID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CertificateUpdateReqType->ContractID.charactersLen, CertificateUpdateReqType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 267;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ListOfRootCertificateIDs, ListOfRootCertificateIDsType (ListOfRootCertificateIDsType)); next=268
                    // decode: element
                    error = decode_din_ListOfRootCertificateIDsType(stream, &CertificateUpdateReqType->ListOfRootCertificateIDs);
                    if (error == 0)
                    {
                        grammar_id = 268;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 268:
            // Grammar: ID=268; read/write bits=1; START (DHParams)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHParams, dHParamsType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateUpdateReqType->DHParams.bytesLen, &CertificateUpdateReqType->DHParams.bytes[0], din_dHParamsType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_CertificateUpdateResType(exi_bitstream_t* stream, struct din_CertificateUpdateResType* CertificateUpdateResType) {
    int grammar_id = 269;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_CertificateUpdateResType(CertificateUpdateResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 269:
            // Grammar: ID=269; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, IDREF (NCName)); next=270
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateResType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateUpdateResType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateUpdateResType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateUpdateResType->Id.charactersLen, CertificateUpdateResType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 270;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 270:
            // Grammar: ID=270; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=271
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 5, &value);
                            if (error == 0)
                            {
                                CertificateUpdateResType->ResponseCode = (din_responseCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 271;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 271:
            // Grammar: ID=271; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=272
                    // decode: element
                    error = decode_din_CertificateChainType(stream, &CertificateUpdateResType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 272;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 272:
            // Grammar: ID=272; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureEncryptedPrivateKey, privateKeyType (base64Binary)); next=273
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateUpdateResType->ContractSignatureEncryptedPrivateKey.bytesLen, &CertificateUpdateResType->ContractSignatureEncryptedPrivateKey.bytes[0], din_privateKeyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 273;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 273:
            // Grammar: ID=273; read/write bits=1; START (DHParams)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHParams, dHParamsType (base64Binary)); next=274
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateUpdateResType->DHParams.bytesLen, &CertificateUpdateResType->DHParams.bytes[0], din_dHParamsType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 274;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 274:
            // Grammar: ID=274; read/write bits=1; START (ContractID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractID, contractIDType (string)); next=275
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateResType->ContractID.charactersLen);
                            if (error == 0)
                            {
                                if (CertificateUpdateResType->ContractID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CertificateUpdateResType->ContractID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CertificateUpdateResType->ContractID.charactersLen, CertificateUpdateResType->ContractID.characters, din_ContractID_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 275;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 275:
            // Grammar: ID=275; read/write bits=1; START (RetryCounter)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RetryCounter, short (int)); next=3
                    // decode: short
                    error = decode_exi_type_integer16(stream, &CertificateUpdateResType->RetryCounter);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_BodyBaseType(exi_bitstream_t* stream, struct din_BodyBaseType* BodyBaseType) {
    // Element has no particles, so the function just decodes END Element
    (void)BodyBaseType;
    uint32_t eventCode;

    int error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode != 0)
        {
            error = EXI_ERROR__UNKNOWN_EVENT_CODE;
        }
    }

    return error;
}

// Element: definition=complex; name={urn:din:70121:2012:MsgHeader}Notification; type={urn:din:70121:2012:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
static int decode_din_NotificationType(exi_bitstream_t* stream, struct din_NotificationType* NotificationType) {
    int grammar_id = 276;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_NotificationType(NotificationType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 276:
            // Grammar: ID=276; read/write bits=1; START (FaultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FaultCode, faultCodeType (string)); next=277
                    // decode: enum
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                NotificationType->FaultCode = (din_faultCodeType)value;
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_nbit_uint is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                grammar_id = 277;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 277:
            // Grammar: ID=277; read/write bits=2; START (FaultMsg), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FaultMsg, faultMsgType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &NotificationType->FaultMsg.charactersLen);
                            if (error == 0)
                            {
                                if (NotificationType->FaultMsg.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    NotificationType->FaultMsg.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, NotificationType->FaultMsg.charactersLen, NotificationType->FaultMsg.characters, din_FaultMsg_CHARACTER_SIZE);
                                }
                                else
                                {
                                    // the string seems to be in the table, but this is not supported
                                    error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                                }
                            }
                        }
                        else
                        {
                            // second level event is not supported
                            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                        }
                    }

                    // if nothing went wrong, the error of exi_basetypes_decoder_characters is evaluated here
                    if (error == 0)
                    {
                        // END Element for simple type
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                NotificationType->FaultMsg_isUsed = 1u;
                                grammar_id = 3;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_SignatureType(exi_bitstream_t* stream, struct din_SignatureType* SignatureType) {
    int grammar_id = 278;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_SignatureType(SignatureType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 278:
            // Grammar: ID=278; read/write bits=2; START (Id), START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=279
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, din_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureType->Id_isUsed = 1u;
                    grammar_id = 279;
                    break;
                case 1:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=280
                    // decode: element
                    error = decode_din_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 280;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 279:
            // Grammar: ID=279; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=280
                    // decode: element
                    error = decode_din_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == 0)
                    {
                        grammar_id = 280;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 280:
            // Grammar: ID=280; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureValue, SignatureValueType (base64Binary)); next=281
                    // decode: element
                    error = decode_din_SignatureValueType(stream, &SignatureType->SignatureValue);
                    if (error == 0)
                    {
                        grammar_id = 281;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 281:
            // Grammar: ID=281; read/write bits=2; START (KeyInfo), START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyInfo, KeyInfoType (KeyInfoType)); next=283
                    // decode: element
                    error = decode_din_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == 0)
                    {
                        SignatureType->KeyInfo_isUsed = 1u;
                        grammar_id = 283;
                    }
                    break;
                case 1:
                    // Event: START (Object, ObjectType (ObjectType)); next=282
                    // decode: element
                    error = decode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 282;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 282:
            // Grammar: ID=282; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 283:
            // Grammar: ID=283; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=284
                    // decode: element
                    error = decode_din_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 284;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 284:
            // Grammar: ID=284; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=3
                    // decode: element
                    // This element should not occur a further time, its representation was reduced to a single element
                    error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_MessageHeaderType(exi_bitstream_t* stream, struct din_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 285;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_MessageHeaderType(MessageHeaderType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 285:
            // Grammar: ID=285; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=286
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MessageHeaderType->SessionID.bytesLen, &MessageHeaderType->SessionID.bytes[0], din_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 286;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 286:
            // Grammar: ID=286; read/write bits=2; START (Notification), START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Notification, NotificationType (NotificationType)); next=287
                    // decode: element
                    error = decode_din_NotificationType(stream, &MessageHeaderType->Notification);
                    if (error == 0)
                    {
                        MessageHeaderType->Notification_isUsed = 1u;
                        grammar_id = 287;
                    }
                    break;
                case 1:
                    // Event: START (Signature, SignatureType (SignatureType)); next=3
                    // decode: element
                    error = decode_din_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == 0)
                    {
                        MessageHeaderType->Signature_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 287:
            // Grammar: ID=287; read/write bits=2; START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Signature, SignatureType (SignatureType)); next=3
                    // decode: element
                    error = decode_din_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == 0)
                    {
                        MessageHeaderType->Signature_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_BodyType(exi_bitstream_t* stream, struct din_BodyType* BodyType) {
    int grammar_id = 288;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_BodyType(BodyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 288:
            // Grammar: ID=288; read bits=6; START (BodyMessage)
            error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: BodyElement
                    // decode: namespace element
                    error = decode_din_BodyBaseType(stream, &BodyType->BodyElement);
                    if (error == 0)
                    {
                        BodyType->BodyElement_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: CableCheckReq
                    // decode: namespace element
                    error = decode_din_CableCheckReqType(stream, &BodyType->CableCheckReq);
                    if (error == 0)
                    {
                        BodyType->CableCheckReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: CableCheckRes
                    // decode: namespace element
                    error = decode_din_CableCheckResType(stream, &BodyType->CableCheckRes);
                    if (error == 0)
                    {
                        BodyType->CableCheckRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: CertificateInstallationReq
                    // decode: namespace element
                    error = decode_din_CertificateInstallationReqType(stream, &BodyType->CertificateInstallationReq);
                    if (error == 0)
                    {
                        BodyType->CertificateInstallationReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: CertificateInstallationRes
                    // decode: namespace element
                    error = decode_din_CertificateInstallationResType(stream, &BodyType->CertificateInstallationRes);
                    if (error == 0)
                    {
                        BodyType->CertificateInstallationRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: CertificateUpdateReq
                    // decode: namespace element
                    error = decode_din_CertificateUpdateReqType(stream, &BodyType->CertificateUpdateReq);
                    if (error == 0)
                    {
                        BodyType->CertificateUpdateReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 6:
                    // Event: CertificateUpdateRes
                    // decode: namespace element
                    error = decode_din_CertificateUpdateResType(stream, &BodyType->CertificateUpdateRes);
                    if (error == 0)
                    {
                        BodyType->CertificateUpdateRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 7:
                    // Event: ChargeParameterDiscoveryReq
                    // decode: namespace element
                    error = decode_din_ChargeParameterDiscoveryReqType(stream, &BodyType->ChargeParameterDiscoveryReq);
                    if (error == 0)
                    {
                        BodyType->ChargeParameterDiscoveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 8:
                    // Event: ChargeParameterDiscoveryRes
                    // decode: namespace element
                    error = decode_din_ChargeParameterDiscoveryResType(stream, &BodyType->ChargeParameterDiscoveryRes);
                    if (error == 0)
                    {
                        BodyType->ChargeParameterDiscoveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 9:
                    // Event: ChargingStatusReq
                    // decode: namespace element
                    error = decode_din_ChargingStatusReqType(stream, &BodyType->ChargingStatusReq);
                    if (error == 0)
                    {
                        BodyType->ChargingStatusReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 10:
                    // Event: ChargingStatusRes
                    // decode: namespace element
                    error = decode_din_ChargingStatusResType(stream, &BodyType->ChargingStatusRes);
                    if (error == 0)
                    {
                        BodyType->ChargingStatusRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 11:
                    // Event: ContractAuthenticationReq
                    // decode: namespace element
                    error = decode_din_ContractAuthenticationReqType(stream, &BodyType->ContractAuthenticationReq);
                    if (error == 0)
                    {
                        BodyType->ContractAuthenticationReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 12:
                    // Event: ContractAuthenticationRes
                    // decode: namespace element
                    error = decode_din_ContractAuthenticationResType(stream, &BodyType->ContractAuthenticationRes);
                    if (error == 0)
                    {
                        BodyType->ContractAuthenticationRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 13:
                    // Event: CurrentDemandReq
                    // decode: namespace element
                    error = decode_din_CurrentDemandReqType(stream, &BodyType->CurrentDemandReq);
                    if (error == 0)
                    {
                        BodyType->CurrentDemandReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 14:
                    // Event: CurrentDemandRes
                    // decode: namespace element
                    error = decode_din_CurrentDemandResType(stream, &BodyType->CurrentDemandRes);
                    if (error == 0)
                    {
                        BodyType->CurrentDemandRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 15:
                    // Event: MeteringReceiptReq
                    // decode: namespace element
                    error = decode_din_MeteringReceiptReqType(stream, &BodyType->MeteringReceiptReq);
                    if (error == 0)
                    {
                        BodyType->MeteringReceiptReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 16:
                    // Event: MeteringReceiptRes
                    // decode: namespace element
                    error = decode_din_MeteringReceiptResType(stream, &BodyType->MeteringReceiptRes);
                    if (error == 0)
                    {
                        BodyType->MeteringReceiptRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 17:
                    // Event: PaymentDetailsReq
                    // decode: namespace element
                    error = decode_din_PaymentDetailsReqType(stream, &BodyType->PaymentDetailsReq);
                    if (error == 0)
                    {
                        BodyType->PaymentDetailsReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 18:
                    // Event: PaymentDetailsRes
                    // decode: namespace element
                    error = decode_din_PaymentDetailsResType(stream, &BodyType->PaymentDetailsRes);
                    if (error == 0)
                    {
                        BodyType->PaymentDetailsRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 19:
                    // Event: PowerDeliveryReq
                    // decode: namespace element
                    error = decode_din_PowerDeliveryReqType(stream, &BodyType->PowerDeliveryReq);
                    if (error == 0)
                    {
                        BodyType->PowerDeliveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 20:
                    // Event: PowerDeliveryRes
                    // decode: namespace element
                    error = decode_din_PowerDeliveryResType(stream, &BodyType->PowerDeliveryRes);
                    if (error == 0)
                    {
                        BodyType->PowerDeliveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 21:
                    // Event: PreChargeReq
                    // decode: namespace element
                    error = decode_din_PreChargeReqType(stream, &BodyType->PreChargeReq);
                    if (error == 0)
                    {
                        BodyType->PreChargeReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 22:
                    // Event: PreChargeRes
                    // decode: namespace element
                    error = decode_din_PreChargeResType(stream, &BodyType->PreChargeRes);
                    if (error == 0)
                    {
                        BodyType->PreChargeRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 23:
                    // Event: ServiceDetailReq
                    // decode: namespace element
                    error = decode_din_ServiceDetailReqType(stream, &BodyType->ServiceDetailReq);
                    if (error == 0)
                    {
                        BodyType->ServiceDetailReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 24:
                    // Event: ServiceDetailRes
                    // decode: namespace element
                    error = decode_din_ServiceDetailResType(stream, &BodyType->ServiceDetailRes);
                    if (error == 0)
                    {
                        BodyType->ServiceDetailRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 25:
                    // Event: ServiceDiscoveryReq
                    // decode: namespace element
                    error = decode_din_ServiceDiscoveryReqType(stream, &BodyType->ServiceDiscoveryReq);
                    if (error == 0)
                    {
                        BodyType->ServiceDiscoveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 26:
                    // Event: ServiceDiscoveryRes
                    // decode: namespace element
                    error = decode_din_ServiceDiscoveryResType(stream, &BodyType->ServiceDiscoveryRes);
                    if (error == 0)
                    {
                        BodyType->ServiceDiscoveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 27:
                    // Event: ServicePaymentSelectionReq
                    // decode: namespace element
                    error = decode_din_ServicePaymentSelectionReqType(stream, &BodyType->ServicePaymentSelectionReq);
                    if (error == 0)
                    {
                        BodyType->ServicePaymentSelectionReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 28:
                    // Event: ServicePaymentSelectionRes
                    // decode: namespace element
                    error = decode_din_ServicePaymentSelectionResType(stream, &BodyType->ServicePaymentSelectionRes);
                    if (error == 0)
                    {
                        BodyType->ServicePaymentSelectionRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 29:
                    // Event: SessionSetupReq
                    // decode: namespace element
                    error = decode_din_SessionSetupReqType(stream, &BodyType->SessionSetupReq);
                    if (error == 0)
                    {
                        BodyType->SessionSetupReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 30:
                    // Event: SessionSetupRes
                    // decode: namespace element
                    error = decode_din_SessionSetupResType(stream, &BodyType->SessionSetupRes);
                    if (error == 0)
                    {
                        BodyType->SessionSetupRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 31:
                    // Event: SessionStopReq
                    // decode: namespace element
                    error = decode_din_SessionStopType(stream, &BodyType->SessionStopReq);
                    if (error == 0)
                    {
                        BodyType->SessionStopReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 32:
                    // Event: SessionStopRes
                    // decode: namespace element
                    error = decode_din_SessionStopResType(stream, &BodyType->SessionStopRes);
                    if (error == 0)
                    {
                        BodyType->SessionStopRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 33:
                    // Event: WeldingDetectionReq
                    // decode: namespace element
                    error = decode_din_WeldingDetectionReqType(stream, &BodyType->WeldingDetectionReq);
                    if (error == 0)
                    {
                        BodyType->WeldingDetectionReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 34:
                    // Event: WeldingDetectionRes
                    // decode: namespace element
                    error = decode_din_WeldingDetectionResType(stream, &BodyType->WeldingDetectionRes);
                    if (error == 0)
                    {
                        BodyType->WeldingDetectionRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;

                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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
static int decode_din_V2G_Message(exi_bitstream_t* stream, struct din_V2G_Message* V2G_Message) {
    int grammar_id = 289;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_din_V2G_Message(V2G_Message);

    while (!done)
    {
        switch (grammar_id)
        {
        case 289:
            // Grammar: ID=289; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=290
                    // decode: element
                    error = decode_din_MessageHeaderType(stream, &V2G_Message->Header);
                    if (error == 0)
                    {
                        grammar_id = 290;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 290:
            // Grammar: ID=290; read/write bits=1; START (Body)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Body, BodyType (BodyType)); next=3
                    // decode: element
                    error = decode_din_BodyType(stream, &V2G_Message->Body);
                    if (error == 0)
                    {
                        grammar_id = 3;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 3:
            // Grammar: ID=3; read/write bits=1; END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
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


// main function for decoding
int decode_din_exiDocument(exi_bitstream_t* stream, struct din_exiDocument* exiDoc)
{
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_din_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 7, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            switch (eventCode)
            {
            case 0:
            case 77:
                error = decode_din_V2G_Message(stream, &exiDoc->V2G_Message);
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }
        }
    }

    return error;
}


