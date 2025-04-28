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
  * @file iso2_msgDefDecoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_basetypes_decoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/common/exi_types_decoder.h"
#include "cbv2g/iso_2/iso2_msgDefDatatypes.h"
#include "cbv2g/iso_2/iso2_msgDefDecoder.h"



static int decode_iso2_CostType(exi_bitstream_t* stream, struct iso2_CostType* CostType);
static int decode_iso2_TransformType(exi_bitstream_t* stream, struct iso2_TransformType* TransformType);
static int decode_iso2_IntervalType(exi_bitstream_t* stream, struct iso2_IntervalType* IntervalType);
static int decode_iso2_TransformsType(exi_bitstream_t* stream, struct iso2_TransformsType* TransformsType);
static int decode_iso2_DSAKeyValueType(exi_bitstream_t* stream, struct iso2_DSAKeyValueType* DSAKeyValueType);
static int decode_iso2_X509IssuerSerialType(exi_bitstream_t* stream, struct iso2_X509IssuerSerialType* X509IssuerSerialType);
static int decode_iso2_RelativeTimeIntervalType(exi_bitstream_t* stream, struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType);
static int decode_iso2_DigestMethodType(exi_bitstream_t* stream, struct iso2_DigestMethodType* DigestMethodType);
static int decode_iso2_RSAKeyValueType(exi_bitstream_t* stream, struct iso2_RSAKeyValueType* RSAKeyValueType);
static int decode_iso2_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso2_CanonicalizationMethodType* CanonicalizationMethodType);
static int decode_iso2_SignatureMethodType(exi_bitstream_t* stream, struct iso2_SignatureMethodType* SignatureMethodType);
static int decode_iso2_KeyValueType(exi_bitstream_t* stream, struct iso2_KeyValueType* KeyValueType);
static int decode_iso2_PhysicalValueType(exi_bitstream_t* stream, struct iso2_PhysicalValueType* PhysicalValueType);
static int decode_iso2_ConsumptionCostType(exi_bitstream_t* stream, struct iso2_ConsumptionCostType* ConsumptionCostType);
static int decode_iso2_PMaxScheduleEntryType(exi_bitstream_t* stream, struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType);
static int decode_iso2_SalesTariffEntryType(exi_bitstream_t* stream, struct iso2_SalesTariffEntryType* SalesTariffEntryType);
static int decode_iso2_ParameterType(exi_bitstream_t* stream, struct iso2_ParameterType* ParameterType);
static int decode_iso2_PMaxScheduleType(exi_bitstream_t* stream, struct iso2_PMaxScheduleType* PMaxScheduleType);
static int decode_iso2_ReferenceType(exi_bitstream_t* stream, struct iso2_ReferenceType* ReferenceType);
static int decode_iso2_RetrievalMethodType(exi_bitstream_t* stream, struct iso2_RetrievalMethodType* RetrievalMethodType);
static int decode_iso2_SalesTariffType(exi_bitstream_t* stream, struct iso2_SalesTariffType* SalesTariffType);
static int decode_iso2_X509DataType(exi_bitstream_t* stream, struct iso2_X509DataType* X509DataType);
static int decode_iso2_PGPDataType(exi_bitstream_t* stream, struct iso2_PGPDataType* PGPDataType);
static int decode_iso2_SPKIDataType(exi_bitstream_t* stream, struct iso2_SPKIDataType* SPKIDataType);
static int decode_iso2_SignedInfoType(exi_bitstream_t* stream, struct iso2_SignedInfoType* SignedInfoType);
static int decode_iso2_ProfileEntryType(exi_bitstream_t* stream, struct iso2_ProfileEntryType* ProfileEntryType);
static int decode_iso2_DC_EVStatusType(exi_bitstream_t* stream, struct iso2_DC_EVStatusType* DC_EVStatusType);
static int decode_iso2_ParameterSetType(exi_bitstream_t* stream, struct iso2_ParameterSetType* ParameterSetType);
static int decode_iso2_SAScheduleTupleType(exi_bitstream_t* stream, struct iso2_SAScheduleTupleType* SAScheduleTupleType);
static int decode_iso2_SelectedServiceType(exi_bitstream_t* stream, struct iso2_SelectedServiceType* SelectedServiceType);
static int decode_iso2_ServiceType(exi_bitstream_t* stream, struct iso2_ServiceType* ServiceType);
static int decode_iso2_SignatureValueType(exi_bitstream_t* stream, struct iso2_SignatureValueType* SignatureValueType);
static int decode_iso2_SubCertificatesType(exi_bitstream_t* stream, struct iso2_SubCertificatesType* SubCertificatesType);
static int decode_iso2_KeyInfoType(exi_bitstream_t* stream, struct iso2_KeyInfoType* KeyInfoType);
static int decode_iso2_ObjectType(exi_bitstream_t* stream, struct iso2_ObjectType* ObjectType);
static int decode_iso2_SupportedEnergyTransferModeType(exi_bitstream_t* stream, struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType);
static int decode_iso2_CertificateChainType(exi_bitstream_t* stream, struct iso2_CertificateChainType* CertificateChainType);
static int decode_iso2_BodyBaseType(exi_bitstream_t* stream, struct iso2_BodyBaseType* BodyBaseType);
static int decode_iso2_NotificationType(exi_bitstream_t* stream, struct iso2_NotificationType* NotificationType);
static int decode_iso2_DC_EVSEStatusType(exi_bitstream_t* stream, struct iso2_DC_EVSEStatusType* DC_EVSEStatusType);
static int decode_iso2_SelectedServiceListType(exi_bitstream_t* stream, struct iso2_SelectedServiceListType* SelectedServiceListType);
static int decode_iso2_PaymentOptionListType(exi_bitstream_t* stream, struct iso2_PaymentOptionListType* PaymentOptionListType);
static int decode_iso2_SignatureType(exi_bitstream_t* stream, struct iso2_SignatureType* SignatureType);
static int decode_iso2_ChargingProfileType(exi_bitstream_t* stream, struct iso2_ChargingProfileType* ChargingProfileType);
static int decode_iso2_ServiceParameterListType(exi_bitstream_t* stream, struct iso2_ServiceParameterListType* ServiceParameterListType);
static int decode_iso2_ListOfRootCertificateIDsType(exi_bitstream_t* stream, struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
static int decode_iso2_AC_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType);
static int decode_iso2_DC_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType);
static int decode_iso2_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_EVChargeParameterType* EVChargeParameterType);
static int decode_iso2_SASchedulesType(exi_bitstream_t* stream, struct iso2_SASchedulesType* SASchedulesType);
static int decode_iso2_SAScheduleListType(exi_bitstream_t* stream, struct iso2_SAScheduleListType* SAScheduleListType);
static int decode_iso2_ChargeServiceType(exi_bitstream_t* stream, struct iso2_ChargeServiceType* ChargeServiceType);
static int decode_iso2_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
static int decode_iso2_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
static int decode_iso2_ContractSignatureEncryptedPrivateKeyType(exi_bitstream_t* stream, struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType);
static int decode_iso2_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_EVSEChargeParameterType* EVSEChargeParameterType);
static int decode_iso2_DC_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
static int decode_iso2_ServiceListType(exi_bitstream_t* stream, struct iso2_ServiceListType* ServiceListType);
static int decode_iso2_DiffieHellmanPublickeyType(exi_bitstream_t* stream, struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType);
static int decode_iso2_EMAIDType(exi_bitstream_t* stream, struct iso2_EMAIDType* EMAIDType);
static int decode_iso2_AC_EVSEStatusType(exi_bitstream_t* stream, struct iso2_AC_EVSEStatusType* AC_EVSEStatusType);
static int decode_iso2_EVSEStatusType(exi_bitstream_t* stream, struct iso2_EVSEStatusType* EVSEStatusType);
static int decode_iso2_AC_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
static int decode_iso2_MeterInfoType(exi_bitstream_t* stream, struct iso2_MeterInfoType* MeterInfoType);
static int decode_iso2_MessageHeaderType(exi_bitstream_t* stream, struct iso2_MessageHeaderType* MessageHeaderType);
static int decode_iso2_PowerDeliveryReqType(exi_bitstream_t* stream, struct iso2_PowerDeliveryReqType* PowerDeliveryReqType);
static int decode_iso2_CurrentDemandResType(exi_bitstream_t* stream, struct iso2_CurrentDemandResType* CurrentDemandResType);
static int decode_iso2_ChargingStatusResType(exi_bitstream_t* stream, struct iso2_ChargingStatusResType* ChargingStatusResType);
static int decode_iso2_AuthorizationReqType(exi_bitstream_t* stream, struct iso2_AuthorizationReqType* AuthorizationReqType);
static int decode_iso2_PreChargeReqType(exi_bitstream_t* stream, struct iso2_PreChargeReqType* PreChargeReqType);
static int decode_iso2_ServiceDetailResType(exi_bitstream_t* stream, struct iso2_ServiceDetailResType* ServiceDetailResType);
static int decode_iso2_PaymentServiceSelectionResType(exi_bitstream_t* stream, struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType);
static int decode_iso2_CertificateUpdateReqType(exi_bitstream_t* stream, struct iso2_CertificateUpdateReqType* CertificateUpdateReqType);
static int decode_iso2_SessionSetupResType(exi_bitstream_t* stream, struct iso2_SessionSetupResType* SessionSetupResType);
static int decode_iso2_CertificateInstallationReqType(exi_bitstream_t* stream, struct iso2_CertificateInstallationReqType* CertificateInstallationReqType);
static int decode_iso2_CertificateInstallationResType(exi_bitstream_t* stream, struct iso2_CertificateInstallationResType* CertificateInstallationResType);
static int decode_iso2_WeldingDetectionResType(exi_bitstream_t* stream, struct iso2_WeldingDetectionResType* WeldingDetectionResType);
static int decode_iso2_CurrentDemandReqType(exi_bitstream_t* stream, struct iso2_CurrentDemandReqType* CurrentDemandReqType);
static int decode_iso2_PreChargeResType(exi_bitstream_t* stream, struct iso2_PreChargeResType* PreChargeResType);
static int decode_iso2_CertificateUpdateResType(exi_bitstream_t* stream, struct iso2_CertificateUpdateResType* CertificateUpdateResType);
static int decode_iso2_MeteringReceiptReqType(exi_bitstream_t* stream, struct iso2_MeteringReceiptReqType* MeteringReceiptReqType);
static int decode_iso2_ChargingStatusReqType(exi_bitstream_t* stream, struct iso2_ChargingStatusReqType* ChargingStatusReqType);
static int decode_iso2_SessionStopResType(exi_bitstream_t* stream, struct iso2_SessionStopResType* SessionStopResType);
static int decode_iso2_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
static int decode_iso2_CableCheckReqType(exi_bitstream_t* stream, struct iso2_CableCheckReqType* CableCheckReqType);
static int decode_iso2_WeldingDetectionReqType(exi_bitstream_t* stream, struct iso2_WeldingDetectionReqType* WeldingDetectionReqType);
static int decode_iso2_PowerDeliveryResType(exi_bitstream_t* stream, struct iso2_PowerDeliveryResType* PowerDeliveryResType);
static int decode_iso2_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
static int decode_iso2_PaymentServiceSelectionReqType(exi_bitstream_t* stream, struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType);
static int decode_iso2_MeteringReceiptResType(exi_bitstream_t* stream, struct iso2_MeteringReceiptResType* MeteringReceiptResType);
static int decode_iso2_CableCheckResType(exi_bitstream_t* stream, struct iso2_CableCheckResType* CableCheckResType);
static int decode_iso2_ServiceDiscoveryResType(exi_bitstream_t* stream, struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType);
static int decode_iso2_ServiceDetailReqType(exi_bitstream_t* stream, struct iso2_ServiceDetailReqType* ServiceDetailReqType);
static int decode_iso2_SessionSetupReqType(exi_bitstream_t* stream, struct iso2_SessionSetupReqType* SessionSetupReqType);
static int decode_iso2_SessionStopReqType(exi_bitstream_t* stream, struct iso2_SessionStopReqType* SessionStopReqType);
static int decode_iso2_ServiceDiscoveryReqType(exi_bitstream_t* stream, struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
static int decode_iso2_AuthorizationResType(exi_bitstream_t* stream, struct iso2_AuthorizationResType* AuthorizationResType);
static int decode_iso2_PaymentDetailsReqType(exi_bitstream_t* stream, struct iso2_PaymentDetailsReqType* PaymentDetailsReqType);
static int decode_iso2_PaymentDetailsResType(exi_bitstream_t* stream, struct iso2_PaymentDetailsResType* PaymentDetailsResType);
static int decode_iso2_BodyType(exi_bitstream_t* stream, struct iso2_BodyType* BodyType);
static int decode_iso2_V2G_Message(exi_bitstream_t* stream, struct iso2_V2G_Message* V2G_Message);

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Cost; type={urn:iso:15118:2:2013:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
static int decode_iso2_CostType(exi_bitstream_t* stream, struct iso2_CostType* CostType) {
    int grammar_id = 0;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CostType(CostType);

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
                                CostType->costKind = (iso2_costKindType)value;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int decode_iso2_TransformType(exi_bitstream_t* stream, struct iso2_TransformType* TransformType) {
    int grammar_id = 5;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_TransformType(TransformType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 5:
            // Grammar: ID=5; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=6
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &TransformType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (TransformType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            TransformType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 6;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 6:
            // Grammar: ID=6; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
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
                                    error = exi_basetypes_decoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso2_XPath_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &TransformType->ANY.bytesLen, &TransformType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}TimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}IntervalType; base type=; content type=empty;
//          abstract=True; final=False;
static int decode_iso2_IntervalType(exi_bitstream_t* stream, struct iso2_IntervalType* IntervalType) {
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
static int decode_iso2_TransformsType(exi_bitstream_t* stream, struct iso2_TransformsType* TransformsType) {
    int grammar_id = 7;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_TransformsType(TransformsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (Transform)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transform, TransformType (TransformType)); next=8
                    // decode: element
                    error = decode_iso2_TransformType(stream, &TransformsType->Transform);
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
            // Grammar: ID=8; read/write bits=2; START (Transform), END Element
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
static int decode_iso2_DSAKeyValueType(exi_bitstream_t* stream, struct iso2_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 9;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DSAKeyValueType(DSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 9:
            // Grammar: ID=9; read/write bits=2; START (P), START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (P, CryptoBinary (base64Binary)); next=10
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->P.bytesLen, &DSAKeyValueType->P.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->P_isUsed = 1u;
                        grammar_id = 10;
                    }
                    break;
                case 1:
                    // Event: START (G, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 12;
                    }
                    break;
                case 2:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=13
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 13;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=1; START (Q)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Q, CryptoBinary (base64Binary)); next=11
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Q.bytesLen, &DSAKeyValueType->Q.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Q_isUsed = 1u;
                        grammar_id = 11;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (G), START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (G, CryptoBinary (base64Binary)); next=12
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->G.bytesLen, &DSAKeyValueType->G.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->G_isUsed = 1u;
                        grammar_id = 12;
                    }
                    break;
                case 1:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=13
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 13;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=1; START (Y)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Y, CryptoBinary (base64Binary)); next=13
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Y.bytesLen, &DSAKeyValueType->Y.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
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
            // Grammar: ID=13; read/write bits=2; START (J), START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (J, CryptoBinary (base64Binary)); next=14
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->J.bytesLen, &DSAKeyValueType->J.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->J_isUsed = 1u;
                        grammar_id = 14;
                    }
                    break;
                case 1:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=15
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 15;
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
        case 14:
            // Grammar: ID=14; read/write bits=2; START (Seed), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Seed, CryptoBinary (base64Binary)); next=15
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->Seed.bytesLen, &DSAKeyValueType->Seed.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        DSAKeyValueType->Seed_isUsed = 1u;
                        grammar_id = 15;
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
        case 15:
            // Grammar: ID=15; read/write bits=2; START (PgenCounter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PgenCounter, CryptoBinary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &DSAKeyValueType->PgenCounter.bytesLen, &DSAKeyValueType->PgenCounter.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
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
static int decode_iso2_X509IssuerSerialType(exi_bitstream_t* stream, struct iso2_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 16;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_X509IssuerSerialType(X509IssuerSerialType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 16:
            // Grammar: ID=16; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerName, string (string)); next=17
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
                                    error = exi_basetypes_decoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso2_X509IssuerName_CHARACTER_SIZE);
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
                                grammar_id = 17;
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
        case 17:
            // Grammar: ID=17; read/write bits=1; START (X509SerialNumber)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeIntervalType; base type=IntervalType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: start, AnonType (1, 1); duration, AnonType (0, 1);
static int decode_iso2_RelativeTimeIntervalType(exi_bitstream_t* stream, struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    int grammar_id = 18;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_RelativeTimeIntervalType(RelativeTimeIntervalType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 18:
            // Grammar: ID=18; read/write bits=1; START (start)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (start, AnonType (unsignedInt)); next=19
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &RelativeTimeIntervalType->start);
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
            // Grammar: ID=19; read/write bits=2; START (duration), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (duration, AnonType (unsignedInt)); next=3
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int decode_iso2_DigestMethodType(exi_bitstream_t* stream, struct iso2_DigestMethodType* DigestMethodType) {
    int grammar_id = 20;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DigestMethodType(DigestMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 20:
            // Grammar: ID=20; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=21
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &DigestMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (DigestMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            DigestMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 21;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 21:
            // Grammar: ID=21; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &DigestMethodType->ANY.bytesLen, &DigestMethodType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
static int decode_iso2_RSAKeyValueType(exi_bitstream_t* stream, struct iso2_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 22;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_RSAKeyValueType(RSAKeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 22:
            // Grammar: ID=22; read/write bits=1; START (Modulus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Modulus, CryptoBinary (base64Binary)); next=23
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Modulus.bytesLen, &RSAKeyValueType->Modulus.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 23;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Exponent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Exponent, CryptoBinary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &RSAKeyValueType->Exponent.bytesLen, &RSAKeyValueType->Exponent.bytes[0], iso2_CryptoBinary_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int decode_iso2_CanonicalizationMethodType(exi_bitstream_t* stream, struct iso2_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 24;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CanonicalizationMethodType(CanonicalizationMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 24:
            // Grammar: ID=24; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=25
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CanonicalizationMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (CanonicalizationMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CanonicalizationMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 25;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &CanonicalizationMethodType->ANY.bytesLen, &CanonicalizationMethodType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
static int decode_iso2_SignatureMethodType(exi_bitstream_t* stream, struct iso2_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 26;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SignatureMethodType(SignatureMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 26:
            // Grammar: ID=26; read/write bits=1; START (Algorithm)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Algorithm, anyURI (anyURI)); next=27
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureMethodType->Algorithm.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureMethodType->Algorithm.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureMethodType->Algorithm.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 27;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 27:
            // Grammar: ID=27; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (HMACOutputLength, HMACOutputLengthType (integer)); next=28
                    // decode: signed
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_signed(stream, &SignatureMethodType->HMACOutputLength);
                        if (error == 0)
                        {
                            SignatureMethodType->HMACOutputLength_isUsed = 1u;
                            grammar_id = 28;
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
        case 28:
            // Grammar: ID=28; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &SignatureMethodType->ANY.bytesLen, &SignatureMethodType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
static int decode_iso2_KeyValueType(exi_bitstream_t* stream, struct iso2_KeyValueType* KeyValueType) {
    int grammar_id = 29;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_KeyValueType(KeyValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 29:
            // Grammar: ID=29; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DSAKeyValue, DSAKeyValueType (DSAKeyValueType)); next=3
                    // decode: element
                    error = decode_iso2_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->DSAKeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (RSAKeyValue, RSAKeyValueType (RSAKeyValueType)); next=3
                    // decode: element
                    error = decode_iso2_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == 0)
                    {
                        KeyValueType->RSAKeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &KeyValueType->ANY.bytesLen, &KeyValueType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileEntryMaxPower; type={urn:iso:15118:2:2013:MsgDataTypes}PhysicalValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Multiplier, unitMultiplierType (1, 1); Unit, unitSymbolType (1, 1); Value, short (1, 1);
static int decode_iso2_PhysicalValueType(exi_bitstream_t* stream, struct iso2_PhysicalValueType* PhysicalValueType) {
    int grammar_id = 30;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PhysicalValueType(PhysicalValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 30:
            // Grammar: ID=30; read/write bits=1; START (Multiplier)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Multiplier, unitMultiplierType (byte)); next=31
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
                                grammar_id = 31;
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
        case 31:
            // Grammar: ID=31; read/write bits=1; START (Unit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Unit, unitSymbolType (string)); next=32
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
                                PhysicalValueType->Unit = (iso2_unitSymbolType)value;
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
                                grammar_id = 32;
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
        case 32:
            // Grammar: ID=32; read/write bits=1; START (Value)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCost; type={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, PhysicalValueType (1, 1); Cost, CostType (1, 3);
static int decode_iso2_ConsumptionCostType(exi_bitstream_t* stream, struct iso2_ConsumptionCostType* ConsumptionCostType) {
    int grammar_id = 33;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ConsumptionCostType(ConsumptionCostType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 33:
            // Grammar: ID=33; read/write bits=1; START (startValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (startValue, PhysicalValueType (PhysicalValueType)); next=34
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &ConsumptionCostType->startValue);
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
            // Grammar: ID=34; read/write bits=1; START (Cost)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Cost, CostType (CostType)); next=35
                    // decode: element array
                    if (ConsumptionCostType->Cost.arrayLen < iso2_CostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_CostType(stream, &ConsumptionCostType->Cost.array[ConsumptionCostType->Cost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_CostType_3_ARRAY_SIZE elements
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
            // Grammar: ID=35; read/write bits=2; LOOP (Cost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Cost, CostType (CostType)); next=35
                    // decode: element array
                    if (ConsumptionCostType->Cost.arrayLen < iso2_CostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_CostType(stream, &ConsumptionCostType->Cost.array[ConsumptionCostType->Cost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_CostType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ConsumptionCostType->Cost.arrayLen < 3)
                    {
                        grammar_id = 35;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntry; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); PMax, PhysicalValueType (1, 1);
static int decode_iso2_PMaxScheduleEntryType(exi_bitstream_t* stream, struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    int grammar_id = 36;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PMaxScheduleEntryType(PMaxScheduleEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RelativeTimeInterval, RelativeTimeIntervalType (IntervalType)); next=37
                    // decode: element
                    error = decode_iso2_RelativeTimeIntervalType(stream, &PMaxScheduleEntryType->RelativeTimeInterval);
                    if (error == 0)
                    {
                        PMaxScheduleEntryType->RelativeTimeInterval_isUsed = 1u;
                        grammar_id = 37;
                    }
                    break;
                case 1:
                    // Abstract element or type: TimeInterval, IntervalType (IntervalType)
                    // decode: element
                    error = decode_iso2_IntervalType(stream, &PMaxScheduleEntryType->TimeInterval);
                    if (error == 0)
                    {
                        PMaxScheduleEntryType->TimeInterval_isUsed = 1u;
                        grammar_id = 37;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=1; START (PMax)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMax, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &PMaxScheduleEntryType->PMax);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntry; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); EPriceLevel, unsignedByte (0, 1); ConsumptionCost, ConsumptionCostType (0, 3);
static int decode_iso2_SalesTariffEntryType(exi_bitstream_t* stream, struct iso2_SalesTariffEntryType* SalesTariffEntryType) {
    int grammar_id = 38;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SalesTariffEntryType(SalesTariffEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 38:
            // Grammar: ID=38; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RelativeTimeInterval, RelativeTimeIntervalType (IntervalType)); next=39
                    // decode: element
                    error = decode_iso2_RelativeTimeIntervalType(stream, &SalesTariffEntryType->RelativeTimeInterval);
                    if (error == 0)
                    {
                        SalesTariffEntryType->RelativeTimeInterval_isUsed = 1u;
                        grammar_id = 39;
                    }
                    break;
                case 1:
                    // Abstract element or type: TimeInterval, IntervalType (IntervalType)
                    // decode: element
                    error = decode_iso2_IntervalType(stream, &SalesTariffEntryType->TimeInterval);
                    if (error == 0)
                    {
                        SalesTariffEntryType->TimeInterval_isUsed = 1u;
                        grammar_id = 39;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=2; START (EPriceLevel), START (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EPriceLevel, unsignedByte (unsignedShort)); next=41
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
                                SalesTariffEntryType->EPriceLevel_isUsed = 1u;
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
                                grammar_id = 41;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=40
                    // decode: element array
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < iso2_ConsumptionCostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[SalesTariffEntryType->ConsumptionCost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ConsumptionCostType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 40;
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
        case 40:
            // Grammar: ID=40; read/write bits=2; LOOP (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=40
                    // decode: element array
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < iso2_ConsumptionCostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[SalesTariffEntryType->ConsumptionCost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ConsumptionCostType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < 3)
                    {
                        grammar_id = 40;
                    }
                    else
                    {
                        grammar_id = 41;
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
        case 41:
            // Grammar: ID=41; read/write bits=2; START (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=42
                    // decode: element array
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < iso2_ConsumptionCostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[SalesTariffEntryType->ConsumptionCost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ConsumptionCostType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 42;
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
        case 42:
            // Grammar: ID=42; read/write bits=2; LOOP (ConsumptionCost), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ConsumptionCost, ConsumptionCostType (ConsumptionCostType)); next=42
                    // decode: element array
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < iso2_ConsumptionCostType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[SalesTariffEntryType->ConsumptionCost.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ConsumptionCostType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SalesTariffEntryType->ConsumptionCost.arrayLen < 3)
                    {
                        grammar_id = 42;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Parameter; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, string (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); physicalValue, PhysicalValueType (0, 1); stringValue, string (0, 1);
static int decode_iso2_ParameterType(exi_bitstream_t* stream, struct iso2_ParameterType* ParameterType) {
    int grammar_id = 43;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ParameterType(ParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 43:
            // Grammar: ID=43; read/write bits=1; START (Name)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Name, string (string)); next=44
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ParameterType->Name.charactersLen);
                    if (error == 0)
                    {
                        if (ParameterType->Name.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ParameterType->Name.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ParameterType->Name.charactersLen, ParameterType->Name.characters, iso2_Name_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 44;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 44:
            // Grammar: ID=44; read/write bits=3; START (boolValue), START (byteValue), START (shortValue), START (intValue), START (physicalValue), START (stringValue)
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
                    error = decode_iso2_PhysicalValueType(stream, &ParameterType->physicalValue);
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
                                    error = exi_basetypes_decoder_characters(stream, ParameterType->stringValue.charactersLen, ParameterType->stringValue.characters, iso2_stringValue_CHARACTER_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxSchedule; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PMaxScheduleEntry, PMaxScheduleEntryType (1, 12) (original max 1024);
static int decode_iso2_PMaxScheduleType(exi_bitstream_t* stream, struct iso2_PMaxScheduleType* PMaxScheduleType) {
    int grammar_id = 45;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PMaxScheduleType(PMaxScheduleType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 45:
            // Grammar: ID=45; read/write bits=1; START (PMaxScheduleEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMaxScheduleEntry, PMaxScheduleEntryType (EntryType)); next=46
                    // decode: element array
                    if (PMaxScheduleType->PMaxScheduleEntry.arrayLen < iso2_PMaxScheduleEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleType->PMaxScheduleEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_PMaxScheduleEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 46;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; LOOP (PMaxScheduleEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (PMaxScheduleEntry, PMaxScheduleEntryType (EntryType)); next=46
                    // decode: element array
                    if (PMaxScheduleType->PMaxScheduleEntry.arrayLen < iso2_PMaxScheduleEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleType->PMaxScheduleEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_PMaxScheduleEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (PMaxScheduleType->PMaxScheduleEntry.arrayLen < 1024)
                    {
                        grammar_id = 46;
                    }
                    else
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
static int decode_iso2_ReferenceType(exi_bitstream_t* stream, struct iso2_ReferenceType* ReferenceType) {
    int grammar_id = 47;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ReferenceType(ReferenceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 47:
            // Grammar: ID=47; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=48
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Id_isUsed = 1u;
                    grammar_id = 48;
                    break;
                case 1:
                    // Event: START (Type, anyURI (anyURI)); next=49
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 49;
                    break;
                case 2:
                    // Event: START (URI, anyURI (anyURI)); next=50
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 50;
                    break;
                case 3:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=51
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 51;
                    }
                    break;
                case 4:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=52
                    // decode: element
                    error = decode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 52;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 48:
            // Grammar: ID=48; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=49
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->Type_isUsed = 1u;
                    grammar_id = 49;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=50
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 50;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=51
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 51;
                    }
                    break;
                case 3:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=52
                    // decode: element
                    error = decode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 52;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=50
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ReferenceType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (ReferenceType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ReferenceType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ReferenceType->URI_isUsed = 1u;
                    grammar_id = 50;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=51
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 51;
                    }
                    break;
                case 2:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=52
                    // decode: element
                    error = decode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 52;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 50:
            // Grammar: ID=50; read/write bits=2; START (Transforms), START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=51
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == 0)
                    {
                        ReferenceType->Transforms_isUsed = 1u;
                        grammar_id = 51;
                    }
                    break;
                case 1:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=52
                    // decode: element
                    error = decode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 52;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestMethod, DigestMethodType (DigestMethodType)); next=52
                    // decode: element
                    error = decode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == 0)
                    {
                        grammar_id = 52;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 52:
            // Grammar: ID=52; read/write bits=1; START (DigestValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DigestValue, DigestValueType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &ReferenceType->DigestValue.bytesLen, &ReferenceType->DigestValue.bytes[0], iso2_DigestValueType_BYTES_SIZE);
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
static int decode_iso2_RetrievalMethodType(exi_bitstream_t* stream, struct iso2_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 53;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_RetrievalMethodType(RetrievalMethodType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 53:
            // Grammar: ID=53; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Type, anyURI (anyURI)); next=54
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->Type.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->Type.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->Type.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->Type_isUsed = 1u;
                    grammar_id = 54;
                    break;
                case 1:
                    // Event: START (URI, anyURI (anyURI)); next=55
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 55;
                    break;
                case 2:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 54:
            // Grammar: ID=54; read/write bits=2; START (URI), START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (URI, anyURI (anyURI)); next=55
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &RetrievalMethodType->URI.charactersLen);
                    if (error == 0)
                    {
                        if (RetrievalMethodType->URI.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            RetrievalMethodType->URI.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    RetrievalMethodType->URI_isUsed = 1u;
                    grammar_id = 55;
                    break;
                case 1:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 55:
            // Grammar: ID=55; read/write bits=2; START (Transforms), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Transforms, TransformsType (TransformsType)); next=3
                    // decode: element
                    error = decode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariff; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SalesTariffID, SAIDType (1, 1); SalesTariffDescription, tariffDescriptionType (0, 1); NumEPriceLevels, unsignedByte (0, 1); SalesTariffEntry, SalesTariffEntryType (1, 12) (original max 1024);
static int decode_iso2_SalesTariffType(exi_bitstream_t* stream, struct iso2_SalesTariffType* SalesTariffType) {
    int grammar_id = 56;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SalesTariffType(SalesTariffType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 56:
            // Grammar: ID=56; read/write bits=2; START (Id), START (SalesTariffID)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=57
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SalesTariffType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SalesTariffType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SalesTariffType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SalesTariffType->Id.charactersLen, SalesTariffType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SalesTariffType->Id_isUsed = 1u;
                    grammar_id = 57;
                    break;
                case 1:
                    // Event: START (SalesTariffID, SAIDType (unsignedByte)); next=58
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                SalesTariffType->SalesTariffID = (uint8_t)(value + 1);
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
                                grammar_id = 58;
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
            // Grammar: ID=57; read/write bits=1; START (SalesTariffID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffID, SAIDType (unsignedByte)); next=58
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                SalesTariffType->SalesTariffID = (uint8_t)(value + 1);
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
                                grammar_id = 58;
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
        case 58:
            // Grammar: ID=58; read/write bits=2; START (SalesTariffDescription), START (NumEPriceLevels), START (SalesTariffEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffDescription, tariffDescriptionType (string)); next=60
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
                                    error = exi_basetypes_decoder_characters(stream, SalesTariffType->SalesTariffDescription.charactersLen, SalesTariffType->SalesTariffDescription.characters, iso2_SalesTariffDescription_CHARACTER_SIZE);
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
                                grammar_id = 60;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (NumEPriceLevels, unsignedByte (unsignedShort)); next=62
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
                                SalesTariffType->NumEPriceLevels_isUsed = 1u;
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
                                grammar_id = 62;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=59
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 59;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 59:
            // Grammar: ID=59; read/write bits=2; LOOP (SalesTariffEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=59
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SalesTariffType->SalesTariffEntry.arrayLen < 1024)
                    {
                        grammar_id = 59;
                    }
                    else
                    {
                        grammar_id = 60;
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
        case 60:
            // Grammar: ID=60; read/write bits=2; START (NumEPriceLevels), START (SalesTariffEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NumEPriceLevels, unsignedByte (unsignedShort)); next=62
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
                                SalesTariffType->NumEPriceLevels_isUsed = 1u;
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
                                grammar_id = 62;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=61
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 61;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 61:
            // Grammar: ID=61; read/write bits=2; LOOP (SalesTariffEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=61
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SalesTariffType->SalesTariffEntry.arrayLen < 1024)
                    {
                        grammar_id = 61;
                    }
                    else
                    {
                        grammar_id = 62;
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
        case 62:
            // Grammar: ID=62; read/write bits=1; START (SalesTariffEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=63
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 63;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 63:
            // Grammar: ID=63; read/write bits=2; LOOP (SalesTariffEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SalesTariffEntry, SalesTariffEntryType (EntryType)); next=63
                    // decode: element array
                    if (SalesTariffType->SalesTariffEntry.arrayLen < iso2_SalesTariffEntryType_12_ARRAY_SIZE)
                    {
                        error = decode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffType->SalesTariffEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SalesTariffEntryType_12_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SalesTariffType->SalesTariffEntry.arrayLen < 1024)
                    {
                        grammar_id = 63;
                    }
                    else
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}X509Data; type={http://www.w3.org/2000/09/xmldsig#}X509DataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: X509IssuerSerial, X509IssuerSerialType (0, 1); X509SKI, base64Binary (0, 1); X509SubjectName, string (0, 1); X509Certificate, base64Binary (0, 1); X509CRL, base64Binary (0, 1); ANY, anyType (0, 1);
static int decode_iso2_X509DataType(exi_bitstream_t* stream, struct iso2_X509DataType* X509DataType) {
    int grammar_id = 64;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_X509DataType(X509DataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 64:
            // Grammar: ID=64; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (X509IssuerSerial, X509IssuerSerialType (X509IssuerSerialType)); next=3
                    // decode: element
                    error = decode_iso2_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == 0)
                    {
                        X509DataType->X509IssuerSerial_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (X509SKI, base64Binary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509SKI.bytesLen, &X509DataType->X509SKI.bytes[0], iso2_base64Binary_BYTES_SIZE);
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
                                    error = exi_basetypes_decoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso2_X509SubjectName_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509Certificate.bytesLen, &X509DataType->X509Certificate.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509Certificate_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (X509CRL, base64Binary (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->X509CRL.bytesLen, &X509DataType->X509CRL.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        X509DataType->X509CRL_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (ANY, anyType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &X509DataType->ANY.bytesLen, &X509DataType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
static int decode_iso2_PGPDataType(exi_bitstream_t* stream, struct iso2_PGPDataType* PGPDataType) {
    int grammar_id = 65;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PGPDataType(PGPDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 65:
            // Grammar: ID=65; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyID, base64Binary (base64Binary)); next=66
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyID.bytesLen, &PGPDataType->choice_1.PGPKeyID.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 66;
                    }
                    break;
                case 1:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=67
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 67;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=67
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.PGPKeyPacket.bytesLen, &PGPDataType->choice_1.PGPKeyPacket.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.PGPKeyPacket_isUsed = 1u;
                        grammar_id = 67;
                    }
                    break;
                case 1:
                    // Event: START (ANY, anyType (base64Binary)); next=68
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 2:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 3:
                    // Event: START (ANY, anyType (base64Binary)); next=68
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso2_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 68;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 67:
            // Grammar: ID=67; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=68
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
                    // Event: START (ANY, anyType (base64Binary)); next=68
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_1.ANY.bytesLen, &PGPDataType->choice_1.ANY.bytes[0], iso2_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_1.ANY_isUsed = 1u;
                        grammar_id = 68;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PGPKeyPacket, base64Binary (base64Binary)); next=69
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.PGPKeyPacket.bytesLen, &PGPDataType->choice_2.PGPKeyPacket.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 69;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 69:
            // Grammar: ID=69; read/write bits=2; START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ANY, anyType (base64Binary)); next=68
                    // decode: event not accepted
                    error = EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING;
                    break;
                case 1:
                    // Event: END Element; next=4
                    done = 1;
                    grammar_id = 4;
                    break;
                case 2:
                    // Event: START (ANY, anyType (base64Binary)); next=68
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PGPDataType->choice_2.ANY.bytesLen, &PGPDataType->choice_2.ANY.bytes[0], iso2_anyType_BYTES_SIZE);
                    if (error == 0)
                    {
                        PGPDataType->choice_2.ANY_isUsed = 1u;
                        grammar_id = 68;
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
static int decode_iso2_SPKIDataType(exi_bitstream_t* stream, struct iso2_SPKIDataType* SPKIDataType) {
    int grammar_id = 70;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SPKIDataType(SPKIDataType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 70:
            // Grammar: ID=70; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SPKISexp, base64Binary (base64Binary)); next=71
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->SPKISexp.bytesLen, &SPKIDataType->SPKISexp.bytes[0], iso2_base64Binary_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 71;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &SPKIDataType->ANY.bytesLen, &SPKIDataType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
static int decode_iso2_SignedInfoType(exi_bitstream_t* stream, struct iso2_SignedInfoType* SignedInfoType) {
    int grammar_id = 72;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SignedInfoType(SignedInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 72:
            // Grammar: ID=72; read/write bits=2; START (Id), START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=73
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignedInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignedInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignedInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignedInfoType->Id_isUsed = 1u;
                    grammar_id = 73;
                    break;
                case 1:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=74
                    // decode: element
                    error = decode_iso2_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 74;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType (CanonicalizationMethodType)); next=74
                    // decode: element
                    error = decode_iso2_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == 0)
                    {
                        grammar_id = 74;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 74:
            // Grammar: ID=74; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureMethod, SignatureMethodType (SignatureMethodType)); next=75
                    // decode: element
                    error = decode_iso2_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                    if (error == 0)
                    {
                        grammar_id = 75;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; START (Reference)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Reference, ReferenceType (ReferenceType)); next=76
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso2_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso2_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 76;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=2; LOOP (Reference), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Reference, ReferenceType (ReferenceType)); next=76
                    // decode: element array
                    if (SignedInfoType->Reference.arrayLen < iso2_ReferenceType_4_ARRAY_SIZE)
                    {
                        error = decode_iso2_ReferenceType(stream, &SignedInfoType->Reference.array[SignedInfoType->Reference.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ReferenceType_4_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 76;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntry; type={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ChargingProfileEntryStart, unsignedInt (1, 1); ChargingProfileEntryMaxPower, PhysicalValueType (1, 1); ChargingProfileEntryMaxNumberOfPhasesInUse, maxNumPhasesType (0, 1);
static int decode_iso2_ProfileEntryType(exi_bitstream_t* stream, struct iso2_ProfileEntryType* ProfileEntryType) {
    int grammar_id = 77;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ProfileEntryType(ProfileEntryType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=1; START (ChargingProfileEntryStart)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfileEntryStart, unsignedInt (unsignedLong)); next=78
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &ProfileEntryType->ChargingProfileEntryStart);
                    if (error == 0)
                    {
                        grammar_id = 78;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=1; START (ChargingProfileEntryMaxPower)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfileEntryMaxPower, PhysicalValueType (PhysicalValueType)); next=79
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &ProfileEntryType->ChargingProfileEntryMaxPower);
                    if (error == 0)
                    {
                        grammar_id = 79;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=2; START (ChargingProfileEntryMaxNumberOfPhasesInUse), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfileEntryMaxNumberOfPhasesInUse, maxNumPhasesType (byte)); next=3
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                ProfileEntryType->ChargingProfileEntryMaxNumberOfPhasesInUse = (int8_t)(value + 1);
                                ProfileEntryType->ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatusType; base type=EVStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVReady, boolean (1, 1); EVErrorCode, DC_EVErrorCodeType (1, 1); EVRESSSOC, percentValueType (1, 1);
static int decode_iso2_DC_EVStatusType(exi_bitstream_t* stream, struct iso2_DC_EVStatusType* DC_EVStatusType) {
    int grammar_id = 80;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DC_EVStatusType(DC_EVStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 80:
            // Grammar: ID=80; read/write bits=1; START (EVReady)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVReady, boolean (boolean)); next=81
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
                                grammar_id = 81;
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
        case 81:
            // Grammar: ID=81; read/write bits=1; START (EVErrorCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVErrorCode, DC_EVErrorCodeType (string)); next=82
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
                                DC_EVStatusType->EVErrorCode = (iso2_DC_EVErrorCodeType)value;
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
                                grammar_id = 82;
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
        case 82:
            // Grammar: ID=82; read/write bits=1; START (EVRESSSOC)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ParameterSet; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, short (1, 1); Parameter, ParameterType (1, 16);
static int decode_iso2_ParameterSetType(exi_bitstream_t* stream, struct iso2_ParameterSetType* ParameterSetType) {
    int grammar_id = 83;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ParameterSetType(ParameterSetType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 83:
            // Grammar: ID=83; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ParameterSetID, short (int)); next=84
                    // decode: short
                    error = decode_exi_type_integer16(stream, &ParameterSetType->ParameterSetID);
                    if (error == 0)
                    {
                        grammar_id = 84;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 84:
            // Grammar: ID=84; read/write bits=1; START (Parameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Parameter, ParameterType (ParameterType)); next=85
                    // decode: element array
                    if (ParameterSetType->Parameter.arrayLen < iso2_ParameterType_16_ARRAY_SIZE)
                    {
                        error = decode_iso2_ParameterType(stream, &ParameterSetType->Parameter.array[ParameterSetType->Parameter.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ParameterType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 85;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=2; LOOP (Parameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Parameter, ParameterType (ParameterType)); next=85
                    // decode: element array
                    if (ParameterSetType->Parameter.arrayLen < iso2_ParameterType_16_ARRAY_SIZE)
                    {
                        error = decode_iso2_ParameterType(stream, &ParameterSetType->Parameter.array[ParameterSetType->Parameter.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ParameterType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ParameterSetType->Parameter.arrayLen < 16)
                    {
                        grammar_id = 85;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTuple; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); PMaxSchedule, PMaxScheduleType (1, 1); SalesTariff, SalesTariffType (0, 1);
static int decode_iso2_SAScheduleTupleType(exi_bitstream_t* stream, struct iso2_SAScheduleTupleType* SAScheduleTupleType) {
    int grammar_id = 86;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SAScheduleTupleType(SAScheduleTupleType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 86:
            // Grammar: ID=86; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (unsignedByte)); next=87
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                SAScheduleTupleType->SAScheduleTupleID = (uint8_t)(value + 1);
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
                                grammar_id = 87;
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
        case 87:
            // Grammar: ID=87; read/write bits=1; START (PMaxSchedule)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PMaxSchedule, PMaxScheduleType (PMaxScheduleType)); next=88
                    // decode: element
                    error = decode_iso2_PMaxScheduleType(stream, &SAScheduleTupleType->PMaxSchedule);
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
            // Grammar: ID=88; read/write bits=2; START (SalesTariff), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SalesTariff, SalesTariffType (SalesTariffType)); next=3
                    // decode: element
                    error = decode_iso2_SalesTariffType(stream, &SAScheduleTupleType->SalesTariff);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SelectedService; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, short (0, 1);
static int decode_iso2_SelectedServiceType(exi_bitstream_t* stream, struct iso2_SelectedServiceType* SelectedServiceType) {
    int grammar_id = 89;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SelectedServiceType(SelectedServiceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 89:
            // Grammar: ID=89; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=90
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &SelectedServiceType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 90;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=2; START (ParameterSetID), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Service; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1);
static int decode_iso2_ServiceType(exi_bitstream_t* stream, struct iso2_ServiceType* ServiceType) {
    int grammar_id = 91;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceType(ServiceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 91:
            // Grammar: ID=91; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=92
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ServiceType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 92;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 92:
            // Grammar: ID=92; read/write bits=2; START (ServiceName), START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceName, serviceNameType (string)); next=93
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ServiceType->ServiceName.charactersLen);
                            if (error == 0)
                            {
                                if (ServiceType->ServiceName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ServiceType->ServiceName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ServiceType->ServiceName.charactersLen, ServiceType->ServiceName.characters, iso2_ServiceName_CHARACTER_SIZE);
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
                                ServiceType->ServiceName_isUsed = 1u;
                                grammar_id = 93;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=94
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
                                ServiceType->ServiceCategory = (iso2_serviceCategoryType)value;
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
                                grammar_id = 94;
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
            // Grammar: ID=93; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=94
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
                                ServiceType->ServiceCategory = (iso2_serviceCategoryType)value;
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
                                grammar_id = 94;
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
        case 94:
            // Grammar: ID=94; read/write bits=2; START (ServiceScope), START (FreeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceScope, serviceScopeType (string)); next=95
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ServiceType->ServiceScope.charactersLen);
                            if (error == 0)
                            {
                                if (ServiceType->ServiceScope.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ServiceType->ServiceScope.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ServiceType->ServiceScope.charactersLen, ServiceType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
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
                                ServiceType->ServiceScope_isUsed = 1u;
                                grammar_id = 95;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
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
        case 95:
            // Grammar: ID=95; read/write bits=1; START (FreeService)
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
static int decode_iso2_SignatureValueType(exi_bitstream_t* stream, struct iso2_SignatureValueType* SignatureValueType) {
    int grammar_id = 96;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SignatureValueType(SignatureValueType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 96:
            // Grammar: ID=96; read/write bits=2; START (Id), START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=97
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureValueType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureValueType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureValueType->Id_isUsed = 1u;
                    grammar_id = 97;
                    break;
                case 1:
                    // Event: START (CONTENT, SignatureValueType (base64Binary)); next=3
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureValueType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso2_SignatureValueType_BYTES_SIZE);
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
        case 97:
            // Grammar: ID=97; read/write bits=1; START (CONTENT)
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
                        error = exi_basetypes_decoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, &SignatureValueType->CONTENT.bytes[0], iso2_SignatureValueType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SubCertificates; type={urn:iso:15118:2:2013:MsgDataTypes}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 4);
static int decode_iso2_SubCertificatesType(exi_bitstream_t* stream, struct iso2_SubCertificatesType* SubCertificatesType) {
    int grammar_id = 98;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SubCertificatesType(SubCertificatesType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 98:
            // Grammar: ID=98; read/write bits=1; START (Certificate)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Certificate, certificateType (base64Binary)); next=99
                    // decode exi type: base64Binary (Array)
                    if (SubCertificatesType->Certificate.arrayLen < iso2_certificateType_4_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &SubCertificatesType->Certificate.array[SubCertificatesType->Certificate.arrayLen].bytesLen, &SubCertificatesType->Certificate.array[SubCertificatesType->Certificate.arrayLen].bytes[0], iso2_certificateType_BYTES_SIZE);
                        if (error == 0)
                        {
                            SubCertificatesType->Certificate.arrayLen++;
                            grammar_id = 99;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=2; LOOP (Certificate), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Certificate, certificateType (base64Binary)); next=99
                    // decode exi type: base64Binary (Array)
                    if (SubCertificatesType->Certificate.arrayLen < iso2_certificateType_4_ARRAY_SIZE)
                    {
                        error = decode_exi_type_hex_binary(stream, &SubCertificatesType->Certificate.array[SubCertificatesType->Certificate.arrayLen].bytesLen, &SubCertificatesType->Certificate.array[SubCertificatesType->Certificate.arrayLen].bytes[0], iso2_certificateType_BYTES_SIZE);
                        if (error == 0)
                        {
                            SubCertificatesType->Certificate.arrayLen++;
                            grammar_id = 99;
                        }
                    }
                    else
                    {
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int decode_iso2_KeyInfoType(exi_bitstream_t* stream, struct iso2_KeyInfoType* KeyInfoType) {
    int grammar_id = 100;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_KeyInfoType(KeyInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 100:
            // Grammar: ID=100; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 4, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=101
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &KeyInfoType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (KeyInfoType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            KeyInfoType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    KeyInfoType->Id_isUsed = 1u;
                    grammar_id = 101;
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso2_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso2_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=3
                    // decode: element
                    error = decode_iso2_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=3
                    // decode: element
                    error = decode_iso2_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=3
                    // decode: element
                    error = decode_iso2_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 6:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=3
                    // decode: element
                    error = decode_iso2_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso2_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
        case 101:
            // Grammar: ID=101; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso2_KeyName_CHARACTER_SIZE);
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
                    error = decode_iso2_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == 0)
                    {
                        KeyInfoType->KeyValue_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (RetrievalMethod, RetrievalMethodType (RetrievalMethodType)); next=3
                    // decode: element
                    error = decode_iso2_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == 0)
                    {
                        KeyInfoType->RetrievalMethod_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (X509Data, X509DataType (X509DataType)); next=3
                    // decode: element
                    error = decode_iso2_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == 0)
                    {
                        KeyInfoType->X509Data_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: START (PGPData, PGPDataType (PGPDataType)); next=3
                    // decode: element
                    error = decode_iso2_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == 0)
                    {
                        KeyInfoType->PGPData_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: START (SPKIData, SPKIDataType (SPKIDataType)); next=3
                    // decode: element
                    error = decode_iso2_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                                    error = exi_basetypes_decoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso2_MgmtData_CHARACTER_SIZE);
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
                    error = decode_exi_type_hex_binary(stream, &KeyInfoType->ANY.bytesLen, &KeyInfoType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int decode_iso2_ObjectType(exi_bitstream_t* stream, struct iso2_ObjectType* ObjectType) {
    int grammar_id = 102;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ObjectType(ObjectType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Encoding, anyURI (anyURI)); next=103
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Encoding.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Encoding.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Encoding.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso2_Encoding_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Encoding_isUsed = 1u;
                    grammar_id = 103;
                    break;
                case 1:
                    // Event: START (Id, ID (NCName)); next=104
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 104;
                    break;
                case 2:
                    // Event: START (MimeType, string (string)); next=105
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 105;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
        case 103:
            // Grammar: ID=103; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=104
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->Id_isUsed = 1u;
                    grammar_id = 104;
                    break;
                case 1:
                    // Event: START (MimeType, string (string)); next=105
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 105;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
        case 104:
            // Grammar: ID=104; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MimeType, string (string)); next=105
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ObjectType->MimeType.charactersLen);
                    if (error == 0)
                    {
                        if (ObjectType->MimeType.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ObjectType->MimeType.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    ObjectType->MimeType_isUsed = 1u;
                    grammar_id = 105;
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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
        case 105:
            // Grammar: ID=105; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    error = decode_exi_type_hex_binary(stream, &ObjectType->ANY.bytesLen, &ObjectType->ANY.bytes[0], iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferMode; type={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyTransferMode, EnergyTransferModeType (1, 6);
static int decode_iso2_SupportedEnergyTransferModeType(exi_bitstream_t* stream, struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType) {
    int grammar_id = 106;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SupportedEnergyTransferModeType(SupportedEnergyTransferModeType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 106:
            // Grammar: ID=106; read/write bits=1; START (EnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EnergyTransferMode, EnergyTransferModeType (string)); next=107
                    // decode: enum array
                    if (SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen < iso2_EnergyTransferModeType_6_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    SupportedEnergyTransferModeType->EnergyTransferMode.array[SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen] = (iso2_EnergyTransferModeType)value;
                                    SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen++;
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
                                grammar_id = 107;
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
        case 107:
            // Grammar: ID=107; read/write bits=2; LOOP (EnergyTransferMode), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (EnergyTransferMode, EnergyTransferModeType (string)); next=107
                    // decode: enum array
                    if (SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen < iso2_EnergyTransferModeType_6_ARRAY_SIZE)
                    {
                        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                        if (error == 0)
                        {
                            if (eventCode == 0)
                            {
                                uint32_t value;
                                error = exi_basetypes_decoder_nbit_uint(stream, 3, &value);
                                if (error == 0)
                                {
                                    SupportedEnergyTransferModeType->EnergyTransferMode.array[SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen] = (iso2_EnergyTransferModeType)value;
                                    SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen++;
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
                                grammar_id = 107;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureCertChain; type={urn:iso:15118:2:2013:MsgDataTypes}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
static int decode_iso2_CertificateChainType(exi_bitstream_t* stream, struct iso2_CertificateChainType* CertificateChainType) {
    int grammar_id = 108;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CertificateChainType(CertificateChainType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 108:
            // Grammar: ID=108; read/write bits=2; START (Id), START (Certificate)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=109
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateChainType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateChainType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateChainType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateChainType->Id.charactersLen, CertificateChainType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    CertificateChainType->Id_isUsed = 1u;
                    grammar_id = 109;
                    break;
                case 1:
                    // Event: START (Certificate, certificateType (base64Binary)); next=110
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateChainType->Certificate.bytesLen, &CertificateChainType->Certificate.bytes[0], iso2_certificateType_BYTES_SIZE);
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
        case 109:
            // Grammar: ID=109; read/write bits=1; START (Certificate)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Certificate, certificateType (base64Binary)); next=110
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateChainType->Certificate.bytesLen, &CertificateChainType->Certificate.bytes[0], iso2_certificateType_BYTES_SIZE);
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
            // Grammar: ID=110; read/write bits=2; START (SubCertificates), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SubCertificates, SubCertificatesType (SubCertificatesType)); next=3
                    // decode: element
                    error = decode_iso2_SubCertificatesType(stream, &CertificateChainType->SubCertificates);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}BodyElement; type={urn:iso:15118:2:2013:MsgBody}BodyBaseType; base type=; content type=empty;
//          abstract=True; final=False;
static int decode_iso2_BodyBaseType(exi_bitstream_t* stream, struct iso2_BodyBaseType* BodyBaseType) {
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgHeader}Notification; type={urn:iso:15118:2:2013:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
static int decode_iso2_NotificationType(exi_bitstream_t* stream, struct iso2_NotificationType* NotificationType) {
    int grammar_id = 111;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_NotificationType(NotificationType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 111:
            // Grammar: ID=111; read/write bits=1; START (FaultCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FaultCode, faultCodeType (string)); next=112
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
                                NotificationType->FaultCode = (iso2_faultCodeType)value;
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
            // Grammar: ID=112; read/write bits=2; START (FaultMsg), END Element
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
                                    error = exi_basetypes_decoder_characters(stream, NotificationType->FaultMsg.charactersLen, NotificationType->FaultMsg.characters, iso2_FaultMsg_CHARACTER_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); EVSEIsolationStatus, isolationLevelType (0, 1); EVSEStatusCode, DC_EVSEStatusCodeType (1, 1);
static int decode_iso2_DC_EVSEStatusType(exi_bitstream_t* stream, struct iso2_DC_EVSEStatusType* DC_EVSEStatusType) {
    int grammar_id = 113;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DC_EVSEStatusType(DC_EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 113:
            // Grammar: ID=113; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedShort (unsignedInt)); next=114
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &DC_EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 114;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 114:
            // Grammar: ID=114; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, EVSENotificationType (string)); next=115
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
                                DC_EVSEStatusType->EVSENotification = (iso2_EVSENotificationType)value;
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
                                grammar_id = 115;
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
        case 115:
            // Grammar: ID=115; read/write bits=2; START (EVSEIsolationStatus), START (EVSEStatusCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEIsolationStatus, isolationLevelType (string)); next=116
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
                                DC_EVSEStatusType->EVSEIsolationStatus = (iso2_isolationLevelType)value;
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
                                grammar_id = 116;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (EVSEStatusCode, DC_EVSEStatusCodeType (string)); next=3
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
                                DC_EVSEStatusType->EVSEStatusCode = (iso2_DC_EVSEStatusCodeType)value;
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
        case 116:
            // Grammar: ID=116; read/write bits=1; START (EVSEStatusCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEStatusCode, DC_EVSEStatusCodeType (string)); next=3
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
                                DC_EVSEStatusType->EVSEStatusCode = (iso2_DC_EVSEStatusCodeType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SelectedServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16);
static int decode_iso2_SelectedServiceListType(exi_bitstream_t* stream, struct iso2_SelectedServiceListType* SelectedServiceListType) {
    int grammar_id = 117;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SelectedServiceListType(SelectedServiceListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 117:
            // Grammar: ID=117; read/write bits=1; START (SelectedService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedService, SelectedServiceType (SelectedServiceType)); next=118
                    // decode: element array
                    if (SelectedServiceListType->SelectedService.arrayLen < iso2_SelectedServiceType_16_ARRAY_SIZE)
                    {
                        error = decode_iso2_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedServiceListType->SelectedService.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SelectedServiceType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 118;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=2; LOOP (SelectedService), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SelectedService, SelectedServiceType (SelectedServiceType)); next=118
                    // decode: element array
                    if (SelectedServiceListType->SelectedService.arrayLen < iso2_SelectedServiceType_16_ARRAY_SIZE)
                    {
                        error = decode_iso2_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedServiceListType->SelectedService.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SelectedServiceType_16_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SelectedServiceListType->SelectedService.arrayLen < 16)
                    {
                        grammar_id = 118;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentOptionList; type={urn:iso:15118:2:2013:MsgDataTypes}PaymentOptionListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PaymentOption, paymentOptionType (1, 2);
static int decode_iso2_PaymentOptionListType(exi_bitstream_t* stream, struct iso2_PaymentOptionListType* PaymentOptionListType) {
    int grammar_id = 119;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PaymentOptionListType(PaymentOptionListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 119:
            // Grammar: ID=119; read/write bits=1; START (PaymentOption)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOption, paymentOptionType (string)); next=120
                    // decode: enum array
                    if (PaymentOptionListType->PaymentOption.arrayLen < iso2_paymentOptionType_2_ARRAY_SIZE)
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
                                    PaymentOptionListType->PaymentOption.array[PaymentOptionListType->PaymentOption.arrayLen] = (iso2_paymentOptionType)value;
                                    PaymentOptionListType->PaymentOption.arrayLen++;
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
                                grammar_id = 120;
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
        case 120:
            // Grammar: ID=120; read/write bits=2; START (PaymentOption), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOption, paymentOptionType (string)); next=3
                    // decode: enum array
                    if (PaymentOptionListType->PaymentOption.arrayLen < iso2_paymentOptionType_2_ARRAY_SIZE)
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
                                    PaymentOptionListType->PaymentOption.array[PaymentOptionListType->PaymentOption.arrayLen] = (iso2_paymentOptionType)value;
                                    PaymentOptionListType->PaymentOption.arrayLen++;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
static int decode_iso2_SignatureType(exi_bitstream_t* stream, struct iso2_SignatureType* SignatureType) {
    int grammar_id = 121;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SignatureType(SignatureType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 121:
            // Grammar: ID=121; read/write bits=2; START (Id), START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=122
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &SignatureType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (SignatureType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            SignatureType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    SignatureType->Id_isUsed = 1u;
                    grammar_id = 122;
                    break;
                case 1:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=123
                    // decode: element
                    error = decode_iso2_SignedInfoType(stream, &SignatureType->SignedInfo);
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
        case 122:
            // Grammar: ID=122; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignedInfo, SignedInfoType (SignedInfoType)); next=123
                    // decode: element
                    error = decode_iso2_SignedInfoType(stream, &SignatureType->SignedInfo);
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
            // Grammar: ID=123; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SignatureValue, SignatureValueType (base64Binary)); next=124
                    // decode: element
                    error = decode_iso2_SignatureValueType(stream, &SignatureType->SignatureValue);
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
            // Grammar: ID=124; read/write bits=2; START (KeyInfo), START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (KeyInfo, KeyInfoType (KeyInfoType)); next=126
                    // decode: element
                    error = decode_iso2_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == 0)
                    {
                        SignatureType->KeyInfo_isUsed = 1u;
                        grammar_id = 126;
                    }
                    break;
                case 1:
                    // Event: START (Object, ObjectType (ObjectType)); next=125
                    // decode: element
                    error = decode_iso2_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 125;
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
        case 125:
            // Grammar: ID=125; read/write bits=2; START (Object), END Element
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
        case 126:
            // Grammar: ID=126; read/write bits=2; START (Object), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Object, ObjectType (ObjectType)); next=127
                    // decode: element
                    error = decode_iso2_ObjectType(stream, &SignatureType->Object);
                    if (error == 0)
                    {
                        SignatureType->Object_isUsed = 1u;
                        grammar_id = 127;
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
        case 127:
            // Grammar: ID=127; read/write bits=2; START (Object), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingProfile; type={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProfileEntry, ProfileEntryType (1, 24);
static int decode_iso2_ChargingProfileType(exi_bitstream_t* stream, struct iso2_ChargingProfileType* ChargingProfileType) {
    int grammar_id = 128;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ChargingProfileType(ChargingProfileType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 128:
            // Grammar: ID=128; read/write bits=1; START (ProfileEntry)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ProfileEntry, ProfileEntryType (ProfileEntryType)); next=129
                    // decode: element array
                    if (ChargingProfileType->ProfileEntry.arrayLen < iso2_ProfileEntryType_24_ARRAY_SIZE)
                    {
                        error = decode_iso2_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ChargingProfileType->ProfileEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ProfileEntryType_24_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 129;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=2; LOOP (ProfileEntry), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ProfileEntry, ProfileEntryType (ProfileEntryType)); next=129
                    // decode: element array
                    if (ChargingProfileType->ProfileEntry.arrayLen < iso2_ProfileEntryType_24_ARRAY_SIZE)
                    {
                        error = decode_iso2_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ChargingProfileType->ProfileEntry.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ProfileEntryType_24_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ChargingProfileType->ProfileEntry.arrayLen < 24)
                    {
                        grammar_id = 129;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceParameterList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5) (original max 255);
static int decode_iso2_ServiceParameterListType(exi_bitstream_t* stream, struct iso2_ServiceParameterListType* ServiceParameterListType) {
    int grammar_id = 130;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceParameterListType(ServiceParameterListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 130:
            // Grammar: ID=130; read/write bits=1; START (ParameterSet)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ParameterSet, ParameterSetType (ParameterSetType)); next=131
                    // decode: element array
                    if (ServiceParameterListType->ParameterSet.arrayLen < iso2_ParameterSetType_5_ARRAY_SIZE)
                    {
                        error = decode_iso2_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ServiceParameterListType->ParameterSet.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ParameterSetType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 131;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=2; LOOP (ParameterSet), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (ParameterSet, ParameterSetType (ParameterSetType)); next=131
                    // decode: element array
                    if (ServiceParameterListType->ParameterSet.arrayLen < iso2_ParameterSetType_5_ARRAY_SIZE)
                    {
                        error = decode_iso2_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ServiceParameterListType->ParameterSet.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ParameterSetType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ServiceParameterListType->ParameterSet.arrayLen < 255)
                    {
                        grammar_id = 131;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ListOfRootCertificateIDs; type={urn:iso:15118:2:2013:MsgDataTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, X509IssuerSerialType (1, 5) (original max 20);
static int decode_iso2_ListOfRootCertificateIDsType(exi_bitstream_t* stream, struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    int grammar_id = 132;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ListOfRootCertificateIDsType(ListOfRootCertificateIDsType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 132:
            // Grammar: ID=132; read/write bits=1; START (RootCertificateID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RootCertificateID, X509IssuerSerialType (X509IssuerSerialType)); next=133
                    // decode: element array
                    if (ListOfRootCertificateIDsType->RootCertificateID.arrayLen < iso2_X509IssuerSerialType_5_ARRAY_SIZE)
                    {
                        error = decode_iso2_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_X509IssuerSerialType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 133;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 133:
            // Grammar: ID=133; read/write bits=2; LOOP (RootCertificateID), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (RootCertificateID, X509IssuerSerialType (X509IssuerSerialType)); next=133
                    // decode: element array
                    if (ListOfRootCertificateIDsType->RootCertificateID.arrayLen < iso2_X509IssuerSerialType_5_ARRAY_SIZE)
                    {
                        error = decode_iso2_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[ListOfRootCertificateIDsType->RootCertificateID.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_X509IssuerSerialType_5_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ListOfRootCertificateIDsType->RootCertificateID.arrayLen < 20)
                    {
                        grammar_id = 133;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EAmount, PhysicalValueType (1, 1); EVMaxVoltage, PhysicalValueType (1, 1); EVMaxCurrent, PhysicalValueType (1, 1); EVMinCurrent, PhysicalValueType (1, 1);
static int decode_iso2_AC_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    int grammar_id = 134;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_AC_EVChargeParameterType(AC_EVChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 134:
            // Grammar: ID=134; read/write bits=2; START (DepartureTime), START (EAmount)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=135
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &AC_EVChargeParameterType->DepartureTime);
                    if (error == 0)
                    {
                        AC_EVChargeParameterType->DepartureTime_isUsed = 1u;
                        grammar_id = 135;
                    }
                    break;
                case 1:
                    // Event: START (EAmount, PhysicalValueType (PhysicalValueType)); next=136
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
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
        case 135:
            // Grammar: ID=135; read/write bits=1; START (EAmount)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EAmount, PhysicalValueType (PhysicalValueType)); next=136
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
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
            // Grammar: ID=136; read/write bits=1; START (EVMaxVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaxVoltage, PhysicalValueType (PhysicalValueType)); next=137
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxVoltage);
                    if (error == 0)
                    {
                        grammar_id = 137;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (EVMaxCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaxCurrent, PhysicalValueType (PhysicalValueType)); next=138
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxCurrent);
                    if (error == 0)
                    {
                        grammar_id = 138;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 138:
            // Grammar: ID=138; read/write bits=1; START (EVMinCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMinCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMinCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); DC_EVStatus, DC_EVStatusType (1, 1); EVMaximumCurrentLimit, PhysicalValueType (1, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); EVMaximumVoltageLimit, PhysicalValueType (1, 1); EVEnergyCapacity, PhysicalValueType (0, 1); EVEnergyRequest, PhysicalValueType (0, 1); FullSOC, percentValueType (0, 1); BulkSOC, percentValueType (0, 1);
static int decode_iso2_DC_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    int grammar_id = 139;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DC_EVChargeParameterType(DC_EVChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 139:
            // Grammar: ID=139; read/write bits=2; START (DepartureTime), START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=140
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &DC_EVChargeParameterType->DepartureTime);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->DepartureTime_isUsed = 1u;
                        grammar_id = 140;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=141
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 141;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 140:
            // Grammar: ID=140; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=141
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 141;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=1; START (EVMaximumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=142
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 142;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 142:
            // Grammar: ID=142; read/write bits=2; START (EVMaximumPowerLimit), START (EVMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=143
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 143;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=144
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 144;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=1; START (EVMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=144
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 144;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=3; START (EVEnergyCapacity), START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVEnergyCapacity, PhysicalValueType (PhysicalValueType)); next=145
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyCapacity);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyCapacity_isUsed = 1u;
                        grammar_id = 145;
                    }
                    break;
                case 1:
                    // Event: START (EVEnergyRequest, PhysicalValueType (PhysicalValueType)); next=146
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyRequest_isUsed = 1u;
                        grammar_id = 146;
                    }
                    break;
                case 2:
                    // Event: START (FullSOC, percentValueType (byte)); next=147
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
                                grammar_id = 147;
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
        case 145:
            // Grammar: ID=145; read/write bits=3; START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVEnergyRequest, PhysicalValueType (PhysicalValueType)); next=146
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == 0)
                    {
                        DC_EVChargeParameterType->EVEnergyRequest_isUsed = 1u;
                        grammar_id = 146;
                    }
                    break;
                case 1:
                    // Event: START (FullSOC, percentValueType (byte)); next=147
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
                                grammar_id = 147;
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
        case 146:
            // Grammar: ID=146; read/write bits=2; START (FullSOC), START (BulkSOC), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FullSOC, percentValueType (byte)); next=147
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
                                grammar_id = 147;
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
        case 147:
            // Grammar: ID=147; read/write bits=2; START (BulkSOC), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); AC_EVChargeParameter, AC_EVChargeParameterType (1, 1); DC_EVChargeParameter, DC_EVChargeParameterType (1, 1);
static int decode_iso2_EVChargeParameterType(exi_bitstream_t* stream, struct iso2_EVChargeParameterType* EVChargeParameterType) {
    int grammar_id = 148;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_EVChargeParameterType(EVChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 148:
            // Grammar: ID=148; read/write bits=2; START (DepartureTime), START (AC_EVChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DepartureTime, unsignedInt (unsignedLong)); next=149
                    // decode: unsigned int
                    error = decode_exi_type_uint32(stream, &EVChargeParameterType->DepartureTime);
                    if (error == 0)
                    {
                        EVChargeParameterType->DepartureTime_isUsed = 1u;
                        grammar_id = 149;
                    }
                    break;
                case 1:
                    // Event: START (AC_EVChargeParameter, AC_EVChargeParameterType (EVChargeParameterType)); next=150
                    // decode: element
                    error = decode_iso2_AC_EVChargeParameterType(stream, &EVChargeParameterType->AC_EVChargeParameter);
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
        case 149:
            // Grammar: ID=149; read/write bits=1; START (AC_EVChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVChargeParameter, AC_EVChargeParameterType (EVChargeParameterType)); next=150
                    // decode: element
                    error = decode_iso2_AC_EVChargeParameterType(stream, &EVChargeParameterType->AC_EVChargeParameter);
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
            // Grammar: ID=150; read/write bits=1; START (DC_EVChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVChargeParameter, DC_EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVChargeParameterType(stream, &EVChargeParameterType->DC_EVChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SASchedules; type={urn:iso:15118:2:2013:MsgDataTypes}SASchedulesType; base type=; content type=empty;
//          abstract=True; final=False;
static int decode_iso2_SASchedulesType(exi_bitstream_t* stream, struct iso2_SASchedulesType* SASchedulesType) {
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleList; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleListType; base type=SASchedulesType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SAScheduleTuple, SAScheduleTupleType (1, 3);
static int decode_iso2_SAScheduleListType(exi_bitstream_t* stream, struct iso2_SAScheduleListType* SAScheduleListType) {
    int grammar_id = 151;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SAScheduleListType(SAScheduleListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=1; START (SAScheduleTuple)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTuple, SAScheduleTupleType (SAScheduleTupleType)); next=152
                    // decode: element array
                    if (SAScheduleListType->SAScheduleTuple.arrayLen < iso2_SAScheduleTupleType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleListType->SAScheduleTuple.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SAScheduleTupleType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 152;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 152:
            // Grammar: ID=152; read/write bits=2; LOOP (SAScheduleTuple), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (SAScheduleTuple, SAScheduleTupleType (SAScheduleTupleType)); next=152
                    // decode: element array
                    if (SAScheduleListType->SAScheduleTuple.arrayLen < iso2_SAScheduleTupleType_3_ARRAY_SIZE)
                    {
                        error = decode_iso2_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleListType->SAScheduleTuple.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_SAScheduleTupleType_3_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (SAScheduleListType->SAScheduleTuple.arrayLen < 3)
                    {
                        grammar_id = 152;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeService; type={urn:iso:15118:2:2013:MsgDataTypes}ChargeServiceType; base type=ServiceType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1); SupportedEnergyTransferMode, SupportedEnergyTransferModeType (1, 1);
static int decode_iso2_ChargeServiceType(exi_bitstream_t* stream, struct iso2_ChargeServiceType* ChargeServiceType) {
    int grammar_id = 153;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ChargeServiceType(ChargeServiceType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 153:
            // Grammar: ID=153; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=154
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ChargeServiceType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 154;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=2; START (ServiceName), START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceName, serviceNameType (string)); next=155
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ChargeServiceType->ServiceName.charactersLen);
                            if (error == 0)
                            {
                                if (ChargeServiceType->ServiceName.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ChargeServiceType->ServiceName.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ChargeServiceType->ServiceName.charactersLen, ChargeServiceType->ServiceName.characters, iso2_ServiceName_CHARACTER_SIZE);
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
                                ChargeServiceType->ServiceName_isUsed = 1u;
                                grammar_id = 155;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=156
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
                                ChargeServiceType->ServiceCategory = (iso2_serviceCategoryType)value;
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
                                grammar_id = 156;
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
        case 155:
            // Grammar: ID=155; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceCategory, serviceCategoryType (string)); next=156
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
                                ChargeServiceType->ServiceCategory = (iso2_serviceCategoryType)value;
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
                                grammar_id = 156;
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
        case 156:
            // Grammar: ID=156; read/write bits=2; START (ServiceScope), START (FreeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceScope, serviceScopeType (string)); next=157
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ChargeServiceType->ServiceScope.charactersLen);
                            if (error == 0)
                            {
                                if (ChargeServiceType->ServiceScope.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ChargeServiceType->ServiceScope.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ChargeServiceType->ServiceScope.charactersLen, ChargeServiceType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
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
                                ChargeServiceType->ServiceScope_isUsed = 1u;
                                grammar_id = 157;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (FreeService, boolean (boolean)); next=158
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
                                ChargeServiceType->FreeService = value;
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
                                grammar_id = 158;
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
        case 157:
            // Grammar: ID=157; read/write bits=1; START (FreeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (FreeService, boolean (boolean)); next=158
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
                                ChargeServiceType->FreeService = value;
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
                                grammar_id = 158;
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
        case 158:
            // Grammar: ID=158; read/write bits=1; START (SupportedEnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SupportedEnergyTransferMode, SupportedEnergyTransferModeType (SupportedEnergyTransferModeType)); next=3
                    // decode: element
                    error = decode_iso2_SupportedEnergyTransferModeType(stream, &ChargeServiceType->SupportedEnergyTransferMode);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameterType; base type=; content type=empty;
//          abstract=True; final=False;
static int decode_iso2_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
static int decode_iso2_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    int grammar_id = 159;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DC_EVPowerDeliveryParameterType(DC_EVPowerDeliveryParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 159:
            // Grammar: ID=159; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=160
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &DC_EVPowerDeliveryParameterType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 160;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 160:
            // Grammar: ID=160; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=161
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
                                grammar_id = 161;
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
        case 161:
            // Grammar: ID=161; read/write bits=1; START (ChargingComplete)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureEncryptedPrivateKey; type={urn:iso:15118:2:2013:MsgDataTypes}ContractSignatureEncryptedPrivateKeyType; base type=privateKeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, ContractSignatureEncryptedPrivateKeyType (1, 1);
static int decode_iso2_ContractSignatureEncryptedPrivateKeyType(exi_bitstream_t* stream, struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType) {
    int grammar_id = 162;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ContractSignatureEncryptedPrivateKeyType(ContractSignatureEncryptedPrivateKeyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 162:
            // Grammar: ID=162; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=163
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &ContractSignatureEncryptedPrivateKeyType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (ContractSignatureEncryptedPrivateKeyType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            ContractSignatureEncryptedPrivateKeyType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, ContractSignatureEncryptedPrivateKeyType->Id.charactersLen, ContractSignatureEncryptedPrivateKeyType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 163;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CONTENT, ContractSignatureEncryptedPrivateKeyType (base64Binary)); next=3
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &ContractSignatureEncryptedPrivateKeyType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, ContractSignatureEncryptedPrivateKeyType->CONTENT.bytesLen, &ContractSignatureEncryptedPrivateKeyType->CONTENT.bytes[0], iso2_ContractSignatureEncryptedPrivateKeyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameterType; base type=; content type=empty;
//          abstract=True; final=False;
static int decode_iso2_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_EVSEChargeParameterType* EVSEChargeParameterType) {
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEMaximumCurrentLimit, PhysicalValueType (1, 1); EVSEMaximumPowerLimit, PhysicalValueType (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (1, 1); EVSEMinimumCurrentLimit, PhysicalValueType (1, 1); EVSEMinimumVoltageLimit, PhysicalValueType (1, 1); EVSECurrentRegulationTolerance, PhysicalValueType (0, 1); EVSEPeakCurrentRipple, PhysicalValueType (1, 1); EVSEEnergyToBeDelivered, PhysicalValueType (0, 1);
static int decode_iso2_DC_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    int grammar_id = 164;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DC_EVSEChargeParameterType(DC_EVSEChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 164:
            // Grammar: ID=164; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=165
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &DC_EVSEChargeParameterType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 165;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=1; START (EVSEMaximumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=166
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 166;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 166:
            // Grammar: ID=166; read/write bits=1; START (EVSEMaximumPowerLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=167
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        grammar_id = 167;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=1; START (EVSEMaximumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=168
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 168;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=1; START (EVSEMinimumCurrentLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=169
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumCurrentLimit);
                    if (error == 0)
                    {
                        grammar_id = 169;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 169:
            // Grammar: ID=169; read/write bits=1; START (EVSEMinimumVoltageLimit)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMinimumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=170
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumVoltageLimit);
                    if (error == 0)
                    {
                        grammar_id = 170;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=2; START (EVSECurrentRegulationTolerance), START (EVSEPeakCurrentRipple)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSECurrentRegulationTolerance, PhysicalValueType (PhysicalValueType)); next=171
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance);
                    if (error == 0)
                    {
                        DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed = 1u;
                        grammar_id = 171;
                    }
                    break;
                case 1:
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType (PhysicalValueType)); next=172
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
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
        case 171:
            // Grammar: ID=171; read/write bits=1; START (EVSEPeakCurrentRipple)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType (PhysicalValueType)); next=172
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
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
            // Grammar: ID=172; read/write bits=2; START (EVSEEnergyToBeDelivered), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEEnergyToBeDelivered, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 8);
static int decode_iso2_ServiceListType(exi_bitstream_t* stream, struct iso2_ServiceListType* ServiceListType) {
    int grammar_id = 173;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceListType(ServiceListType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 173:
            // Grammar: ID=173; read/write bits=1; START (Service)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Service, ServiceType (ServiceType)); next=174
                    // decode: element array
                    if (ServiceListType->Service.arrayLen < iso2_ServiceType_8_ARRAY_SIZE)
                    {
                        error = decode_iso2_ServiceType(stream, &ServiceListType->Service.array[ServiceListType->Service.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ServiceType_8_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    grammar_id = 174;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=2; LOOP (Service), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: LOOP (Service, ServiceType (ServiceType)); next=174
                    // decode: element array
                    if (ServiceListType->Service.arrayLen < iso2_ServiceType_8_ARRAY_SIZE)
                    {
                        error = decode_iso2_ServiceType(stream, &ServiceListType->Service.array[ServiceListType->Service.arrayLen++]);
                    }
                    else
                    {
                        // static array not large enough, only iso2_ServiceType_8_ARRAY_SIZE elements
                        error = EXI_ERROR__ARRAY_OUT_OF_BOUNDS;
                    }
                    // LOOP breakout code for schema given maximum, regardless of ARRAY_SIZE definition
                    if (ServiceListType->Service.arrayLen < 8)
                    {
                        grammar_id = 174;
                    }
                    else
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DHpublickey; type={urn:iso:15118:2:2013:MsgDataTypes}DiffieHellmanPublickeyType; base type=dHpublickeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, DiffieHellmanPublickeyType (1, 1);
static int decode_iso2_DiffieHellmanPublickeyType(exi_bitstream_t* stream, struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType) {
    int grammar_id = 175;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_DiffieHellmanPublickeyType(DiffieHellmanPublickeyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=176
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &DiffieHellmanPublickeyType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (DiffieHellmanPublickeyType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            DiffieHellmanPublickeyType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, DiffieHellmanPublickeyType->Id.charactersLen, DiffieHellmanPublickeyType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 176;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 176:
            // Grammar: ID=176; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CONTENT, DiffieHellmanPublickeyType (base64Binary)); next=3
                    // decode exi type: base64Binary (simple)
                    error = exi_basetypes_decoder_uint_16(stream, &DiffieHellmanPublickeyType->CONTENT.bytesLen);
                    if (error == 0)
                    {
                        error = exi_basetypes_decoder_bytes(stream, DiffieHellmanPublickeyType->CONTENT.bytesLen, &DiffieHellmanPublickeyType->CONTENT.bytes[0], iso2_DiffieHellmanPublickeyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}eMAID; type={urn:iso:15118:2:2013:MsgDataTypes}EMAIDType; base type=eMAIDType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, EMAIDType (1, 1);
static int decode_iso2_EMAIDType(exi_bitstream_t* stream, struct iso2_EMAIDType* EMAIDType) {
    int grammar_id = 177;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_EMAIDType(EMAIDType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 177:
            // Grammar: ID=177; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=178
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &EMAIDType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (EMAIDType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            EMAIDType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, EMAIDType->Id.charactersLen, EMAIDType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 178;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=1; START (CONTENT)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (CONTENT, EMAIDType (string)); next=3
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_uint_16(stream, &EMAIDType->CONTENT.charactersLen);
                    if (error == 0)
                    {
                        if (EMAIDType->CONTENT.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            EMAIDType->CONTENT.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, EMAIDType->CONTENT.charactersLen, EMAIDType->CONTENT.characters, iso2_CONTENT_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 3;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); RCD, boolean (1, 1);
static int decode_iso2_AC_EVSEStatusType(exi_bitstream_t* stream, struct iso2_AC_EVSEStatusType* AC_EVSEStatusType) {
    int grammar_id = 179;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_AC_EVSEStatusType(AC_EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 179:
            // Grammar: ID=179; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedShort (unsignedInt)); next=180
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &AC_EVSEStatusType->NotificationMaxDelay);
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
            // Grammar: ID=180; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, EVSENotificationType (string)); next=181
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
                                AC_EVSEStatusType->EVSENotification = (iso2_EVSENotificationType)value;
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
                                grammar_id = 181;
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
        case 181:
            // Grammar: ID=181; read/write bits=1; START (RCD)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RCD, boolean (boolean)); next=3
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1);
static int decode_iso2_EVSEStatusType(exi_bitstream_t* stream, struct iso2_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 182;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_EVSEStatusType(EVSEStatusType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 182:
            // Grammar: ID=182; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (NotificationMaxDelay, unsignedShort (unsignedInt)); next=183
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &EVSEStatusType->NotificationMaxDelay);
                    if (error == 0)
                    {
                        grammar_id = 183;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENotification, EVSENotificationType (string)); next=184
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
                                EVSEStatusType->EVSENotification = (iso2_EVSENotificationType)value;
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
                                grammar_id = 184;
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
        case 184:
            // Grammar: ID=184; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=185
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &EVSEStatusType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 185;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 185:
            // Grammar: ID=185; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &EVSEStatusType->DC_EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: AC_EVSEStatus, AC_EVSEStatusType (1, 1); EVSENominalVoltage, PhysicalValueType (1, 1); EVSEMaxCurrent, PhysicalValueType (1, 1);
static int decode_iso2_AC_EVSEChargeParameterType(exi_bitstream_t* stream, struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    int grammar_id = 186;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_AC_EVSEChargeParameterType(AC_EVSEChargeParameterType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 186:
            // Grammar: ID=186; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=187
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &AC_EVSEChargeParameterType->AC_EVSEStatus);
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
            // Grammar: ID=187; read/write bits=1; START (EVSENominalVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSENominalVoltage, PhysicalValueType (PhysicalValueType)); next=188
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSENominalVoltage);
                    if (error == 0)
                    {
                        grammar_id = 188;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=1; START (EVSEMaxCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaxCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeterInfo; type={urn:iso:15118:2:2013:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, unsignedLong (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
static int decode_iso2_MeterInfoType(exi_bitstream_t* stream, struct iso2_MeterInfoType* MeterInfoType) {
    int grammar_id = 189;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_MeterInfoType(MeterInfoType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 189:
            // Grammar: ID=189; read/write bits=1; START (MeterID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterID, meterIDType (string)); next=190
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
                                    error = exi_basetypes_decoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso2_MeterID_CHARACTER_SIZE);
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
                                grammar_id = 190;
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
            // Grammar: ID=190; read/write bits=3; START (MeterReading), START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterReading, unsignedLong (nonNegativeInteger)); next=191
                    // decode: unsigned long int
                    error = decode_exi_type_uint64(stream, &MeterInfoType->MeterReading);
                    if (error == 0)
                    {
                        MeterInfoType->MeterReading_isUsed = 1u;
                        grammar_id = 191;
                    }
                    break;
                case 1:
                    // Event: START (SigMeterReading, sigMeterReadingType (base64Binary)); next=192
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->SigMeterReading.bytesLen, &MeterInfoType->SigMeterReading.bytes[0], iso2_sigMeterReadingType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->SigMeterReading_isUsed = 1u;
                        grammar_id = 192;
                    }
                    break;
                case 2:
                    // Event: START (MeterStatus, meterStatusType (short)); next=193
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 193;
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
        case 191:
            // Grammar: ID=191; read/write bits=3; START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SigMeterReading, sigMeterReadingType (base64Binary)); next=192
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &MeterInfoType->SigMeterReading.bytesLen, &MeterInfoType->SigMeterReading.bytes[0], iso2_sigMeterReadingType_BYTES_SIZE);
                    if (error == 0)
                    {
                        MeterInfoType->SigMeterReading_isUsed = 1u;
                        grammar_id = 192;
                    }
                    break;
                case 1:
                    // Event: START (MeterStatus, meterStatusType (short)); next=193
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 193;
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
        case 192:
            // Grammar: ID=192; read/write bits=2; START (MeterStatus), START (TMeter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterStatus, meterStatusType (short)); next=193
                    // decode: short
                    error = decode_exi_type_integer16(stream, &MeterInfoType->MeterStatus);
                    if (error == 0)
                    {
                        MeterInfoType->MeterStatus_isUsed = 1u;
                        grammar_id = 193;
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
        case 193:
            // Grammar: ID=193; read/write bits=2; START (TMeter), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Header; type={urn:iso:15118:2:2013:MsgHeader}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); Notification, NotificationType (0, 1); Signature, SignatureType (0, 1);
static int decode_iso2_MessageHeaderType(exi_bitstream_t* stream, struct iso2_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 194;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_MessageHeaderType(MessageHeaderType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 194:
            // Grammar: ID=194; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=195
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MessageHeaderType->SessionID.bytesLen, &MessageHeaderType->SessionID.bytes[0], iso2_sessionIDType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 195;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 195:
            // Grammar: ID=195; read/write bits=2; START (Notification), START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Notification, NotificationType (NotificationType)); next=196
                    // decode: element
                    error = decode_iso2_NotificationType(stream, &MessageHeaderType->Notification);
                    if (error == 0)
                    {
                        MessageHeaderType->Notification_isUsed = 1u;
                        grammar_id = 196;
                    }
                    break;
                case 1:
                    // Event: START (Signature, SignatureType (SignatureType)); next=3
                    // decode: element
                    error = decode_iso2_SignatureType(stream, &MessageHeaderType->Signature);
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
        case 196:
            // Grammar: ID=196; read/write bits=2; START (Signature), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Signature, SignatureType (SignatureType)); next=3
                    // decode: element
                    error = decode_iso2_SignatureType(stream, &MessageHeaderType->Signature);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReq; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargeProgress, chargeProgressType (1, 1); SAScheduleTupleID, SAIDType (1, 1); ChargingProfile, ChargingProfileType (0, 1); DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (0, 1); EVPowerDeliveryParameter, EVPowerDeliveryParameterType (0, 1);
static int decode_iso2_PowerDeliveryReqType(exi_bitstream_t* stream, struct iso2_PowerDeliveryReqType* PowerDeliveryReqType) {
    int grammar_id = 197;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PowerDeliveryReqType(PowerDeliveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 197:
            // Grammar: ID=197; read/write bits=1; START (ChargeProgress)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargeProgress, chargeProgressType (string)); next=198
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
                                PowerDeliveryReqType->ChargeProgress = (iso2_chargeProgressType)value;
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
                                grammar_id = 198;
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
        case 198:
            // Grammar: ID=198; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (unsignedByte)); next=199
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                PowerDeliveryReqType->SAScheduleTupleID = (uint8_t)(value + 1);
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
                                grammar_id = 199;
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
        case 199:
            // Grammar: ID=199; read/write bits=3; START (ChargingProfile), START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingProfile, ChargingProfileType (ChargingProfileType)); next=200
                    // decode: element
                    error = decode_iso2_ChargingProfileType(stream, &PowerDeliveryReqType->ChargingProfile);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->ChargingProfile_isUsed = 1u;
                        grammar_id = 200;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVPowerDeliveryParameter, EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)
                    // decode: element
                    error = decode_iso2_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
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
        case 200:
            // Grammar: ID=200; read/write bits=2; START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
                    if (error == 0)
                    {
                        PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Abstract element or type: EVPowerDeliveryParameter, EVPowerDeliveryParameterType (EVPowerDeliveryParameterType)
                    // decode: element
                    error = decode_iso2_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandRes; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1); EVSEPresentCurrent, PhysicalValueType (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (0, 1); EVSEMaximumCurrentLimit, PhysicalValueType (0, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1);
static int decode_iso2_CurrentDemandResType(exi_bitstream_t* stream, struct iso2_CurrentDemandResType* CurrentDemandResType) {
    int grammar_id = 201;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CurrentDemandResType(CurrentDemandResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 201:
            // Grammar: ID=201; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=202
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
                                CurrentDemandResType->ResponseCode = (iso2_responseCodeType)value;
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
            // Grammar: ID=202; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=203
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &CurrentDemandResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 203;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=204
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentVoltage);
                    if (error == 0)
                    {
                        grammar_id = 204;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=1; START (EVSEPresentCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentCurrent, PhysicalValueType (PhysicalValueType)); next=205
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentCurrent);
                    if (error == 0)
                    {
                        grammar_id = 205;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 205:
            // Grammar: ID=205; read/write bits=1; START (EVSECurrentLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSECurrentLimitAchieved, boolean (boolean)); next=206
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
                                grammar_id = 206;
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
        case 206:
            // Grammar: ID=206; read/write bits=1; START (EVSEVoltageLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEVoltageLimitAchieved, boolean (boolean)); next=207
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
            // Grammar: ID=207; read/write bits=1; START (EVSEPowerLimitAchieved)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPowerLimitAchieved, boolean (boolean)); next=208
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
                                grammar_id = 208;
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
        case 208:
            // Grammar: ID=208; read/write bits=3; START (EVSEMaximumVoltageLimit), START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=209
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumVoltageLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed = 1u;
                        grammar_id = 209;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=210
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 210;
                    }
                    break;
                case 2:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=211
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 211;
                    }
                    break;
                case 3:
                    // Event: START (EVSEID, evseIDType (string)); next=212
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CurrentDemandResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (CurrentDemandResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CurrentDemandResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 212;
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
        case 209:
            // Grammar: ID=209; read/write bits=2; START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=210
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 210;
                    }
                    break;
                case 1:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=211
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 211;
                    }
                    break;
                case 2:
                    // Event: START (EVSEID, evseIDType (string)); next=212
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CurrentDemandResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (CurrentDemandResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CurrentDemandResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 212;
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
        case 210:
            // Grammar: ID=210; read/write bits=2; START (EVSEMaximumPowerLimit), START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=211
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 211;
                    }
                    break;
                case 1:
                    // Event: START (EVSEID, evseIDType (string)); next=212
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CurrentDemandResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (CurrentDemandResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CurrentDemandResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 212;
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
        case 211:
            // Grammar: ID=211; read/write bits=1; START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEID, evseIDType (string)); next=212
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CurrentDemandResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (CurrentDemandResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CurrentDemandResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 212;
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
        case 212:
            // Grammar: ID=212; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (unsignedByte)); next=213
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                CurrentDemandResType->SAScheduleTupleID = (uint8_t)(value + 1);
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
                                grammar_id = 213;
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
        case 213:
            // Grammar: ID=213; read/write bits=2; START (MeterInfo), START (ReceiptRequired), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=214
                    // decode: element
                    error = decode_iso2_MeterInfoType(stream, &CurrentDemandResType->MeterInfo);
                    if (error == 0)
                    {
                        CurrentDemandResType->MeterInfo_isUsed = 1u;
                        grammar_id = 214;
                    }
                    break;
                case 1:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=3
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
                                CurrentDemandResType->ReceiptRequired = value;
                                CurrentDemandResType->ReceiptRequired_isUsed = 1u;
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
        case 214:
            // Grammar: ID=214; read/write bits=2; START (ReceiptRequired), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=3
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
                                CurrentDemandResType->ReceiptRequired = value;
                                CurrentDemandResType->ReceiptRequired_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusRes; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
static int decode_iso2_ChargingStatusResType(exi_bitstream_t* stream, struct iso2_ChargingStatusResType* ChargingStatusResType) {
    int grammar_id = 215;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ChargingStatusResType(ChargingStatusResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 215:
            // Grammar: ID=215; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=216
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
                                ChargingStatusResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 216;
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
        case 216:
            // Grammar: ID=216; read/write bits=1; START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEID, evseIDType (string)); next=217
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &ChargingStatusResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (ChargingStatusResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    ChargingStatusResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, ChargingStatusResType->EVSEID.charactersLen, ChargingStatusResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 217;
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
        case 217:
            // Grammar: ID=217; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (unsignedByte)); next=218
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                ChargingStatusResType->SAScheduleTupleID = (uint8_t)(value + 1);
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
                                grammar_id = 218;
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
        case 218:
            // Grammar: ID=218; read/write bits=3; START (EVSEMaxCurrent), START (MeterInfo), START (ReceiptRequired), START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEMaxCurrent, PhysicalValueType (PhysicalValueType)); next=219
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &ChargingStatusResType->EVSEMaxCurrent);
                    if (error == 0)
                    {
                        ChargingStatusResType->EVSEMaxCurrent_isUsed = 1u;
                        grammar_id = 219;
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=220
                    // decode: element
                    error = decode_iso2_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == 0)
                    {
                        ChargingStatusResType->MeterInfo_isUsed = 1u;
                        grammar_id = 220;
                    }
                    break;
                case 2:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=221
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
                                ChargingStatusResType->ReceiptRequired_isUsed = 1u;
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
                                grammar_id = 221;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
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
        case 219:
            // Grammar: ID=219; read/write bits=2; START (MeterInfo), START (ReceiptRequired), START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=220
                    // decode: element
                    error = decode_iso2_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == 0)
                    {
                        ChargingStatusResType->MeterInfo_isUsed = 1u;
                        grammar_id = 220;
                    }
                    break;
                case 1:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=221
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
                                ChargingStatusResType->ReceiptRequired_isUsed = 1u;
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
                                grammar_id = 221;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
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
        case 220:
            // Grammar: ID=220; read/write bits=2; START (ReceiptRequired), START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ReceiptRequired, boolean (boolean)); next=221
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
                                ChargingStatusResType->ReceiptRequired_isUsed = 1u;
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
                                grammar_id = 221;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
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
        case 221:
            // Grammar: ID=221; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationReq; type={urn:iso:15118:2:2013:MsgBody}AuthorizationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); GenChallenge, genChallengeType (0, 1);
static int decode_iso2_AuthorizationReqType(exi_bitstream_t* stream, struct iso2_AuthorizationReqType* AuthorizationReqType) {
    int grammar_id = 222;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_AuthorizationReqType(AuthorizationReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 222:
            // Grammar: ID=222; read/write bits=2; START (Id), START (GenChallenge), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=223
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &AuthorizationReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (AuthorizationReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            AuthorizationReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, AuthorizationReqType->Id.charactersLen, AuthorizationReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    AuthorizationReqType->Id_isUsed = 1u;
                    grammar_id = 223;
                    break;
                case 1:
                    // Event: START (GenChallenge, genChallengeType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &AuthorizationReqType->GenChallenge.bytesLen, &AuthorizationReqType->GenChallenge.bytes[0], iso2_genChallengeType_BYTES_SIZE);
                    if (error == 0)
                    {
                        AuthorizationReqType->GenChallenge_isUsed = 1u;
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
        case 223:
            // Grammar: ID=223; read/write bits=2; START (GenChallenge), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (GenChallenge, genChallengeType (base64Binary)); next=3
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &AuthorizationReqType->GenChallenge.bytesLen, &AuthorizationReqType->GenChallenge.bytes[0], iso2_genChallengeType_BYTES_SIZE);
                    if (error == 0)
                    {
                        AuthorizationReqType->GenChallenge_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeReq; type={urn:iso:15118:2:2013:MsgBody}PreChargeReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetVoltage, PhysicalValueType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1);
static int decode_iso2_PreChargeReqType(exi_bitstream_t* stream, struct iso2_PreChargeReqType* PreChargeReqType) {
    int grammar_id = 224;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PreChargeReqType(PreChargeReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 224:
            // Grammar: ID=224; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=225
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &PreChargeReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 225;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=226
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &PreChargeReqType->EVTargetVoltage);
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
            // Grammar: ID=226; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &PreChargeReqType->EVTargetCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (0, 1);
static int decode_iso2_ServiceDetailResType(exi_bitstream_t* stream, struct iso2_ServiceDetailResType* ServiceDetailResType) {
    int grammar_id = 227;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceDetailResType(ServiceDetailResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=228
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
                                ServiceDetailResType->ResponseCode = (iso2_responseCodeType)value;
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
            // Grammar: ID=228; read/write bits=1; START (ServiceID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceID, serviceIDType (unsignedShort)); next=229
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ServiceDetailResType->ServiceID);
                    if (error == 0)
                    {
                        grammar_id = 229;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=2; START (ServiceParameterList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceParameterList, ServiceParameterListType (ServiceParameterListType)); next=3
                    // decode: element
                    error = decode_iso2_ServiceParameterListType(stream, &ServiceDetailResType->ServiceParameterList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionRes; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int decode_iso2_PaymentServiceSelectionResType(exi_bitstream_t* stream, struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType) {
    int grammar_id = 230;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PaymentServiceSelectionResType(PaymentServiceSelectionResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 230:
            // Grammar: ID=230; read/write bits=1; START (ResponseCode)
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
                                PaymentServiceSelectionResType->ResponseCode = (iso2_responseCodeType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReq; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); eMAID, eMAIDType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
static int decode_iso2_CertificateUpdateReqType(exi_bitstream_t* stream, struct iso2_CertificateUpdateReqType* CertificateUpdateReqType) {
    int grammar_id = 231;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CertificateUpdateReqType(CertificateUpdateReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 231:
            // Grammar: ID=231; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=232
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateUpdateReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateUpdateReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateUpdateReqType->Id.charactersLen, CertificateUpdateReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 232;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=233
                    // decode: element
                    error = decode_iso2_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                    if (error == 0)
                    {
                        grammar_id = 233;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=1; START (eMAID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (eMAID, eMAIDType (string)); next=234
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &CertificateUpdateReqType->eMAID.charactersLen);
                            if (error == 0)
                            {
                                if (CertificateUpdateReqType->eMAID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    CertificateUpdateReqType->eMAID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, CertificateUpdateReqType->eMAID.charactersLen, CertificateUpdateReqType->eMAID.characters, iso2_eMAID_CHARACTER_SIZE);
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
            // Grammar: ID=234; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ListOfRootCertificateIDs, ListOfRootCertificateIDsType (ListOfRootCertificateIDsType)); next=3
                    // decode: element
                    error = decode_iso2_ListOfRootCertificateIDsType(stream, &CertificateUpdateReqType->ListOfRootCertificateIDs);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupRes; type={urn:iso:15118:2:2013:MsgBody}SessionSetupResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); EVSETimeStamp, long (0, 1);
static int decode_iso2_SessionSetupResType(exi_bitstream_t* stream, struct iso2_SessionSetupResType* SessionSetupResType) {
    int grammar_id = 235;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SessionSetupResType(SessionSetupResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 235:
            // Grammar: ID=235; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=236
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
                                SessionSetupResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 236;
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
        case 236:
            // Grammar: ID=236; read/write bits=1; START (EVSEID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEID, evseIDType (string)); next=237
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &SessionSetupResType->EVSEID.charactersLen);
                            if (error == 0)
                            {
                                if (SessionSetupResType->EVSEID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    SessionSetupResType->EVSEID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, SessionSetupResType->EVSEID.charactersLen, SessionSetupResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
                                grammar_id = 237;
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
        case 237:
            // Grammar: ID=237; read/write bits=2; START (EVSETimeStamp), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSETimeStamp, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &SessionSetupResType->EVSETimeStamp);
                    if (error == 0)
                    {
                        SessionSetupResType->EVSETimeStamp_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReq; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); OEMProvisioningCert, certificateType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
static int decode_iso2_CertificateInstallationReqType(exi_bitstream_t* stream, struct iso2_CertificateInstallationReqType* CertificateInstallationReqType) {
    int grammar_id = 238;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CertificateInstallationReqType(CertificateInstallationReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 238:
            // Grammar: ID=238; read/write bits=1; START (Id)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=239
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &CertificateInstallationReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (CertificateInstallationReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            CertificateInstallationReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, CertificateInstallationReqType->Id.charactersLen, CertificateInstallationReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    grammar_id = 239;
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 239:
            // Grammar: ID=239; read/write bits=1; START (OEMProvisioningCert)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (OEMProvisioningCert, certificateType (base64Binary)); next=240
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &CertificateInstallationReqType->OEMProvisioningCert.bytesLen, &CertificateInstallationReqType->OEMProvisioningCert.bytes[0], iso2_certificateType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 240;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ListOfRootCertificateIDs, ListOfRootCertificateIDsType (ListOfRootCertificateIDsType)); next=3
                    // decode: element
                    error = decode_iso2_ListOfRootCertificateIDsType(stream, &CertificateInstallationReqType->ListOfRootCertificateIDs);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationRes; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1);
static int decode_iso2_CertificateInstallationResType(exi_bitstream_t* stream, struct iso2_CertificateInstallationResType* CertificateInstallationResType) {
    int grammar_id = 241;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CertificateInstallationResType(CertificateInstallationResType);

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
                                CertificateInstallationResType->ResponseCode = (iso2_responseCodeType)value;
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
            // Grammar: ID=242; read/write bits=1; START (SAProvisioningCertificateChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAProvisioningCertificateChain, CertificateChainType (CertificateChainType)); next=243
                    // decode: element
                    error = decode_iso2_CertificateChainType(stream, &CertificateInstallationResType->SAProvisioningCertificateChain);
                    if (error == 0)
                    {
                        grammar_id = 243;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=244
                    // decode: element
                    error = decode_iso2_CertificateChainType(stream, &CertificateInstallationResType->ContractSignatureCertChain);
                    if (error == 0)
                    {
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
            // Grammar: ID=244; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (privateKeyType)); next=245
                    // decode: element
                    error = decode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &CertificateInstallationResType->ContractSignatureEncryptedPrivateKey);
                    if (error == 0)
                    {
                        grammar_id = 245;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 245:
            // Grammar: ID=245; read/write bits=1; START (DHpublickey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHpublickey, DiffieHellmanPublickeyType (dHpublickeyType)); next=246
                    // decode: element
                    error = decode_iso2_DiffieHellmanPublickeyType(stream, &CertificateInstallationResType->DHpublickey);
                    if (error == 0)
                    {
                        grammar_id = 246;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=1; START (eMAID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (eMAID, EMAIDType (eMAIDType)); next=3
                    // decode: element
                    error = decode_iso2_EMAIDType(stream, &CertificateInstallationResType->eMAID);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionRes; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int decode_iso2_WeldingDetectionResType(exi_bitstream_t* stream, struct iso2_WeldingDetectionResType* WeldingDetectionResType) {
    int grammar_id = 247;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_WeldingDetectionResType(WeldingDetectionResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 247:
            // Grammar: ID=247; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=248
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
                                WeldingDetectionResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 248;
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
        case 248:
            // Grammar: ID=248; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=249
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &WeldingDetectionResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 249;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &WeldingDetectionResType->EVSEPresentVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandReq; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1); EVMaximumVoltageLimit, PhysicalValueType (0, 1); EVMaximumCurrentLimit, PhysicalValueType (0, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1); RemainingTimeToFullSoC, PhysicalValueType (0, 1); RemainingTimeToBulkSoC, PhysicalValueType (0, 1); EVTargetVoltage, PhysicalValueType (1, 1);
static int decode_iso2_CurrentDemandReqType(exi_bitstream_t* stream, struct iso2_CurrentDemandReqType* CurrentDemandReqType) {
    int grammar_id = 250;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CurrentDemandReqType(CurrentDemandReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 250:
            // Grammar: ID=250; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=251
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &CurrentDemandReqType->DC_EVStatus);
                    if (error == 0)
                    {
                        grammar_id = 251;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetCurrent, PhysicalValueType (PhysicalValueType)); next=252
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetCurrent);
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
            // Grammar: ID=252; read/write bits=3; START (EVMaximumVoltageLimit), START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType (PhysicalValueType)); next=253
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumVoltageLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumVoltageLimit_isUsed = 1u;
                        grammar_id = 253;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=254
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 2:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=255
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 3:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=256
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
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 4:
                    // Event: START (ChargingComplete, boolean (boolean)); next=257
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
                                grammar_id = 257;
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
        case 253:
            // Grammar: ID=253; read/write bits=3; START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType (PhysicalValueType)); next=254
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 1u;
                        grammar_id = 254;
                    }
                    break;
                case 1:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=255
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 2:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=256
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
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 3:
                    // Event: START (ChargingComplete, boolean (boolean)); next=257
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
                                grammar_id = 257;
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
            // Grammar: ID=254; read/write bits=2; START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType (PhysicalValueType)); next=255
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == 0)
                    {
                        CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 1u;
                        grammar_id = 255;
                    }
                    break;
                case 1:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=256
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
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 2:
                    // Event: START (ChargingComplete, boolean (boolean)); next=257
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
                                grammar_id = 257;
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
            // Grammar: ID=255; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (BulkChargingComplete, boolean (boolean)); next=256
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
                                grammar_id = 256;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (ChargingComplete, boolean (boolean)); next=257
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
                                grammar_id = 257;
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
            // Grammar: ID=256; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingComplete, boolean (boolean)); next=257
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
                                grammar_id = 257;
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
        case 257:
            // Grammar: ID=257; read/write bits=2; START (RemainingTimeToFullSoC), START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToFullSoC, PhysicalValueType (PhysicalValueType)); next=258
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToFullSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToFullSoC_isUsed = 1u;
                        grammar_id = 258;
                    }
                    break;
                case 1:
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType (PhysicalValueType)); next=259
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 1u;
                        grammar_id = 259;
                    }
                    break;
                case 2:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
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
        case 258:
            // Grammar: ID=258; read/write bits=2; START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType (PhysicalValueType)); next=259
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == 0)
                    {
                        CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 1u;
                        grammar_id = 259;
                    }
                    break;
                case 1:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
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
        case 259:
            // Grammar: ID=259; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVTargetVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeRes; type={urn:iso:15118:2:2013:MsgBody}PreChargeResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int decode_iso2_PreChargeResType(exi_bitstream_t* stream, struct iso2_PreChargeResType* PreChargeResType) {
    int grammar_id = 260;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PreChargeResType(PreChargeResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 260:
            // Grammar: ID=260; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=261
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
                                PreChargeResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 261;
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
        case 261:
            // Grammar: ID=261; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=262
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &PreChargeResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 262;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEPresentVoltage, PhysicalValueType (PhysicalValueType)); next=3
                    // decode: element
                    error = decode_iso2_PhysicalValueType(stream, &PreChargeResType->EVSEPresentVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateRes; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1); RetryCounter, short (0, 1);
static int decode_iso2_CertificateUpdateResType(exi_bitstream_t* stream, struct iso2_CertificateUpdateResType* CertificateUpdateResType) {
    int grammar_id = 263;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CertificateUpdateResType(CertificateUpdateResType);

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
                    // Event: START (ResponseCode, responseCodeType (string)); next=264
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
                                CertificateUpdateResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 264;
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
        case 264:
            // Grammar: ID=264; read/write bits=1; START (SAProvisioningCertificateChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAProvisioningCertificateChain, CertificateChainType (CertificateChainType)); next=265
                    // decode: element
                    error = decode_iso2_CertificateChainType(stream, &CertificateUpdateResType->SAProvisioningCertificateChain);
                    if (error == 0)
                    {
                        grammar_id = 265;
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
                    error = decode_iso2_CertificateChainType(stream, &CertificateUpdateResType->ContractSignatureCertChain);
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
            // Grammar: ID=266; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (privateKeyType)); next=267
                    // decode: element
                    error = decode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &CertificateUpdateResType->ContractSignatureEncryptedPrivateKey);
                    if (error == 0)
                    {
                        grammar_id = 267;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (DHpublickey)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DHpublickey, DiffieHellmanPublickeyType (dHpublickeyType)); next=268
                    // decode: element
                    error = decode_iso2_DiffieHellmanPublickeyType(stream, &CertificateUpdateResType->DHpublickey);
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
            // Grammar: ID=268; read/write bits=1; START (eMAID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (eMAID, EMAIDType (eMAIDType)); next=269
                    // decode: element
                    error = decode_iso2_EMAIDType(stream, &CertificateUpdateResType->eMAID);
                    if (error == 0)
                    {
                        grammar_id = 269;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 269:
            // Grammar: ID=269; read/write bits=2; START (RetryCounter), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
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
                        CertificateUpdateResType->RetryCounter_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReq; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); SessionID, sessionIDType (1, 1); SAScheduleTupleID, SAIDType (0, 1); MeterInfo, MeterInfoType (1, 1);
static int decode_iso2_MeteringReceiptReqType(exi_bitstream_t* stream, struct iso2_MeteringReceiptReqType* MeteringReceiptReqType) {
    int grammar_id = 270;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_MeteringReceiptReqType(MeteringReceiptReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 270:
            // Grammar: ID=270; read/write bits=2; START (Id), START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Id, ID (NCName)); next=271
                    // decode: string (len, characters) (Attribute)
                    error = exi_basetypes_decoder_uint_16(stream, &MeteringReceiptReqType->Id.charactersLen);
                    if (error == 0)
                    {
                        if (MeteringReceiptReqType->Id.charactersLen >= 2)
                        {
                            // string tables and table partitions are not supported, so the length has to be decremented by 2
                            MeteringReceiptReqType->Id.charactersLen -= 2;
                            error = exi_basetypes_decoder_characters(stream, MeteringReceiptReqType->Id.charactersLen, MeteringReceiptReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        }
                        else
                        {
                            // the string seems to be in the table, but this is not supported
                            error = EXI_ERROR__STRINGVALUES_NOT_SUPPORTED;
                        }
                    }
                    MeteringReceiptReqType->Id_isUsed = 1u;
                    grammar_id = 271;
                    break;
                case 1:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=272
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MeteringReceiptReqType->SessionID.bytesLen, &MeteringReceiptReqType->SessionID.bytes[0], iso2_sessionIDType_BYTES_SIZE);
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
        case 271:
            // Grammar: ID=271; read/write bits=1; START (SessionID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SessionID, sessionIDType (hexBinary)); next=272
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &MeteringReceiptReqType->SessionID.bytesLen, &MeteringReceiptReqType->SessionID.bytes[0], iso2_sessionIDType_BYTES_SIZE);
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
            // Grammar: ID=272; read/write bits=2; START (SAScheduleTupleID), START (MeterInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleTupleID, SAIDType (unsignedByte)); next=273
                    // decode: restricted integer (4096 or fewer values)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            uint32_t value;
                            error = exi_basetypes_decoder_nbit_uint(stream, 8, &value);
                            if (error == 0)
                            {
                                // type has min_value = 1
                                MeteringReceiptReqType->SAScheduleTupleID = (uint8_t)(value + 1);
                                MeteringReceiptReqType->SAScheduleTupleID_isUsed = 1u;
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
                                grammar_id = 273;
                            }
                            else
                            {
                                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
                            }
                        }
                    }
                    break;
                case 1:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=3
                    // decode: element
                    error = decode_iso2_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
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
        case 273:
            // Grammar: ID=273; read/write bits=1; START (MeterInfo)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MeterInfo, MeterInfoType (MeterInfoType)); next=3
                    // decode: element
                    error = decode_iso2_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusReq; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusReqType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
static int decode_iso2_ChargingStatusReqType(exi_bitstream_t* stream, struct iso2_ChargingStatusReqType* ChargingStatusReqType) {
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopRes; type={urn:iso:15118:2:2013:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int decode_iso2_SessionStopResType(exi_bitstream_t* stream, struct iso2_SessionStopResType* SessionStopResType) {
    int grammar_id = 274;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SessionStopResType(SessionStopResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 274:
            // Grammar: ID=274; read/write bits=1; START (ResponseCode)
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
                                SessionStopResType->ResponseCode = (iso2_responseCodeType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: MaxEntriesSAScheduleTuple, unsignedShort (0, 1); RequestedEnergyTransferMode, EnergyTransferModeType (1, 1); AC_EVChargeParameter, AC_EVChargeParameterType (0, 1); DC_EVChargeParameter, DC_EVChargeParameterType (0, 1); EVChargeParameter, EVChargeParameterType (0, 1);
static int decode_iso2_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    int grammar_id = 275;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ChargeParameterDiscoveryReqType(ChargeParameterDiscoveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 275:
            // Grammar: ID=275; read/write bits=2; START (MaxEntriesSAScheduleTuple), START (RequestedEnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (MaxEntriesSAScheduleTuple, unsignedShort (unsignedInt)); next=276
                    // decode: unsigned short
                    error = decode_exi_type_uint16(stream, &ChargeParameterDiscoveryReqType->MaxEntriesSAScheduleTuple);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->MaxEntriesSAScheduleTuple_isUsed = 1u;
                        grammar_id = 276;
                    }
                    break;
                case 1:
                    // Event: START (RequestedEnergyTransferMode, EnergyTransferModeType (string)); next=277
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
                                ChargeParameterDiscoveryReqType->RequestedEnergyTransferMode = (iso2_EnergyTransferModeType)value;
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
        case 276:
            // Grammar: ID=276; read/write bits=1; START (RequestedEnergyTransferMode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (RequestedEnergyTransferMode, EnergyTransferModeType (string)); next=277
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
                                ChargeParameterDiscoveryReqType->RequestedEnergyTransferMode = (iso2_EnergyTransferModeType)value;
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
            // Grammar: ID=277; read/write bits=2; START (AC_EVChargeParameter), START (DC_EVChargeParameter), START (EVChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVChargeParameter, AC_EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->AC_EVChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVChargeParameter, DC_EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->DC_EVChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryReqType->DC_EVChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (EVChargeParameter, EVChargeParameterType (EVChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->EVChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckReq; type={urn:iso:15118:2:2013:MsgBody}CableCheckReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int decode_iso2_CableCheckReqType(exi_bitstream_t* stream, struct iso2_CableCheckReqType* CableCheckReqType) {
    int grammar_id = 278;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CableCheckReqType(CableCheckReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 278:
            // Grammar: ID=278; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &CableCheckReqType->DC_EVStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReq; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int decode_iso2_WeldingDetectionReqType(exi_bitstream_t* stream, struct iso2_WeldingDetectionReqType* WeldingDetectionReqType) {
    int grammar_id = 279;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_WeldingDetectionReqType(WeldingDetectionReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 279:
            // Grammar: ID=279; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVStatus, DC_EVStatusType (EVStatusType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVStatusType(stream, &WeldingDetectionReqType->DC_EVStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryRes; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
static int decode_iso2_PowerDeliveryResType(exi_bitstream_t* stream, struct iso2_PowerDeliveryResType* PowerDeliveryResType) {
    int grammar_id = 280;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PowerDeliveryResType(PowerDeliveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 280:
            // Grammar: ID=280; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=281
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
                                PowerDeliveryResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 281;
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
        case 281:
            // Grammar: ID=281; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &PowerDeliveryResType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        PowerDeliveryResType->AC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &PowerDeliveryResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        PowerDeliveryResType->DC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (EVSEStatus, EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_EVSEStatusType(stream, &PowerDeliveryResType->EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1); SAScheduleList, SAScheduleListType (0, 1); SASchedules, SASchedulesType (0, 1); AC_EVSEChargeParameter, AC_EVSEChargeParameterType (0, 1); DC_EVSEChargeParameter, DC_EVSEChargeParameterType (0, 1); EVSEChargeParameter, EVSEChargeParameterType (0, 1);
static int decode_iso2_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    int grammar_id = 282;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ChargeParameterDiscoveryResType(ChargeParameterDiscoveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 282:
            // Grammar: ID=282; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=283
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
                                ChargeParameterDiscoveryResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 283;
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
        case 283:
            // Grammar: ID=283; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSEProcessing, EVSEProcessingType (string)); next=284
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
                                ChargeParameterDiscoveryResType->EVSEProcessing = (iso2_EVSEProcessingType)value;
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
                                grammar_id = 284;
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
        case 284:
            // Grammar: ID=284; read/write bits=3; START (SAScheduleList), START (SASchedules), START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 3, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SAScheduleList, SAScheduleListType (SASchedulesType)); next=285
                    // decode: element
                    error = decode_iso2_SAScheduleListType(stream, &ChargeParameterDiscoveryResType->SAScheduleList);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->SAScheduleList_isUsed = 1u;
                        grammar_id = 285;
                    }
                    break;
                case 1:
                    // Abstract element or type: SASchedules, SASchedulesType (SASchedulesType)
                    // decode: element
                    error = decode_iso2_SASchedulesType(stream, &ChargeParameterDiscoveryResType->SASchedules);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->SASchedules_isUsed = 1u;
                        grammar_id = 285;
                    }
                    break;
                case 2:
                    // Event: START (AC_EVSEChargeParameter, AC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: START (DC_EVSEChargeParameter, DC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Abstract element or type: EVSEChargeParameter, EVSEChargeParameterType (EVSEChargeParameterType)
                    // decode: element
                    error = decode_iso2_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
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
        case 285:
            // Grammar: ID=285; read/write bits=2; START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEChargeParameter, AC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVSEChargeParameter, DC_EVSEChargeParameterType (EVSEChargeParameterType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
                    if (error == 0)
                    {
                        ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Abstract element or type: EVSEChargeParameter, EVSEChargeParameterType (EVSEChargeParameterType)
                    // decode: element
                    error = decode_iso2_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReq; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SelectedPaymentOption, paymentOptionType (1, 1); SelectedServiceList, SelectedServiceListType (1, 1);
static int decode_iso2_PaymentServiceSelectionReqType(exi_bitstream_t* stream, struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType) {
    int grammar_id = 286;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PaymentServiceSelectionReqType(PaymentServiceSelectionReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 286:
            // Grammar: ID=286; read/write bits=1; START (SelectedPaymentOption)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedPaymentOption, paymentOptionType (string)); next=287
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
                                PaymentServiceSelectionReqType->SelectedPaymentOption = (iso2_paymentOptionType)value;
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
                                grammar_id = 287;
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
        case 287:
            // Grammar: ID=287; read/write bits=1; START (SelectedServiceList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (SelectedServiceList, SelectedServiceListType (SelectedServiceListType)); next=3
                    // decode: element
                    error = decode_iso2_SelectedServiceListType(stream, &PaymentServiceSelectionReqType->SelectedServiceList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptRes; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
static int decode_iso2_MeteringReceiptResType(exi_bitstream_t* stream, struct iso2_MeteringReceiptResType* MeteringReceiptResType) {
    int grammar_id = 288;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_MeteringReceiptResType(MeteringReceiptResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 288:
            // Grammar: ID=288; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=289
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
                                MeteringReceiptResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 289;
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
        case 289:
            // Grammar: ID=289; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (AC_EVSEStatus, AC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_AC_EVSEStatusType(stream, &MeteringReceiptResType->AC_EVSEStatus);
                    if (error == 0)
                    {
                        MeteringReceiptResType->AC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &MeteringReceiptResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        MeteringReceiptResType->DC_EVSEStatus_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: START (EVSEStatus, EVSEStatusType (EVSEStatusType)); next=3
                    // decode: element
                    error = decode_iso2_EVSEStatusType(stream, &MeteringReceiptResType->EVSEStatus);
                    if (error == 0)
                    {
                        MeteringReceiptResType->EVSEStatus_isUsed = 1u;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckRes; type={urn:iso:15118:2:2013:MsgBody}CableCheckResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int decode_iso2_CableCheckResType(exi_bitstream_t* stream, struct iso2_CableCheckResType* CableCheckResType) {
    int grammar_id = 290;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_CableCheckResType(CableCheckResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 290:
            // Grammar: ID=290; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=291
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
                                CableCheckResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 291;
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
        case 291:
            // Grammar: ID=291; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (DC_EVSEStatus, DC_EVSEStatusType (EVSEStatusType)); next=292
                    // decode: element
                    error = decode_iso2_DC_EVSEStatusType(stream, &CableCheckResType->DC_EVSEStatus);
                    if (error == 0)
                    {
                        grammar_id = 292;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 292:
            // Grammar: ID=292; read/write bits=1; START (EVSEProcessing)
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
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                CableCheckResType->EVSEProcessing = (iso2_EVSEProcessingType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); PaymentOptionList, PaymentOptionListType (1, 1); ChargeService, ChargeServiceType (1, 1); ServiceList, ServiceListType (0, 1);
static int decode_iso2_ServiceDiscoveryResType(exi_bitstream_t* stream, struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    int grammar_id = 293;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceDiscoveryResType(ServiceDiscoveryResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 293:
            // Grammar: ID=293; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=294
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
                                ServiceDiscoveryResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 294;
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
        case 294:
            // Grammar: ID=294; read/write bits=1; START (PaymentOptionList)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (PaymentOptionList, PaymentOptionListType (PaymentOptionListType)); next=295
                    // decode: element
                    error = decode_iso2_PaymentOptionListType(stream, &ServiceDiscoveryResType->PaymentOptionList);
                    if (error == 0)
                    {
                        grammar_id = 295;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 295:
            // Grammar: ID=295; read/write bits=1; START (ChargeService)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargeService, ChargeServiceType (ServiceType)); next=296
                    // decode: element
                    error = decode_iso2_ChargeServiceType(stream, &ServiceDiscoveryResType->ChargeService);
                    if (error == 0)
                    {
                        grammar_id = 296;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 296:
            // Grammar: ID=296; read/write bits=2; START (ServiceList), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceList, ServiceListType (ServiceListType)); next=3
                    // decode: element
                    error = decode_iso2_ServiceListType(stream, &ServiceDiscoveryResType->ServiceList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1);
static int decode_iso2_ServiceDetailReqType(exi_bitstream_t* stream, struct iso2_ServiceDetailReqType* ServiceDetailReqType) {
    int grammar_id = 297;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceDetailReqType(ServiceDetailReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 297:
            // Grammar: ID=297; read/write bits=1; START (ServiceID)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupReq; type={urn:iso:15118:2:2013:MsgBody}SessionSetupReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVCCID, evccIDType (1, 1);
static int decode_iso2_SessionSetupReqType(exi_bitstream_t* stream, struct iso2_SessionSetupReqType* SessionSetupReqType) {
    int grammar_id = 298;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SessionSetupReqType(SessionSetupReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 298:
            // Grammar: ID=298; read/write bits=1; START (EVCCID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVCCID, evccIDType (hexBinary)); next=3
                    // decode exi type: hexBinary
                    error = decode_exi_type_hex_binary(stream, &SessionSetupReqType->EVCCID.bytesLen, &SessionSetupReqType->EVCCID.bytes[0], iso2_evccIDType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopReq; type={urn:iso:15118:2:2013:MsgBody}SessionStopReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargingSession, chargingSessionType (1, 1);
static int decode_iso2_SessionStopReqType(exi_bitstream_t* stream, struct iso2_SessionStopReqType* SessionStopReqType) {
    int grammar_id = 299;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_SessionStopReqType(SessionStopReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 299:
            // Grammar: ID=299; read/write bits=1; START (ChargingSession)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ChargingSession, chargingSessionType (string)); next=3
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
                                SessionStopReqType->ChargingSession = (iso2_chargingSessionType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceScope, serviceScopeType (0, 1); ServiceCategory, serviceCategoryType (0, 1);
static int decode_iso2_ServiceDiscoveryReqType(exi_bitstream_t* stream, struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    int grammar_id = 300;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_ServiceDiscoveryReqType(ServiceDiscoveryReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 300:
            // Grammar: ID=300; read/write bits=2; START (ServiceScope), START (ServiceCategory), END Element
            error = exi_basetypes_decoder_nbit_uint(stream, 2, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ServiceScope, serviceScopeType (string)); next=301
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
                                    error = exi_basetypes_decoder_characters(stream, ServiceDiscoveryReqType->ServiceScope.charactersLen, ServiceDiscoveryReqType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
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
                                grammar_id = 301;
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
                                ServiceDiscoveryReqType->ServiceCategory = (iso2_serviceCategoryType)value;
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
        case 301:
            // Grammar: ID=301; read/write bits=2; START (ServiceCategory), END Element
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
                                ServiceDiscoveryReqType->ServiceCategory = (iso2_serviceCategoryType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationRes; type={urn:iso:15118:2:2013:MsgBody}AuthorizationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int decode_iso2_AuthorizationResType(exi_bitstream_t* stream, struct iso2_AuthorizationResType* AuthorizationResType) {
    int grammar_id = 302;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_AuthorizationResType(AuthorizationResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 302:
            // Grammar: ID=302; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=303
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
                                AuthorizationResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 303;
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
        case 303:
            // Grammar: ID=303; read/write bits=1; START (EVSEProcessing)
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
                            error = exi_basetypes_decoder_nbit_uint(stream, 2, &value);
                            if (error == 0)
                            {
                                AuthorizationResType->EVSEProcessing = (iso2_EVSEProcessingType)value;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReq; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: eMAID, eMAIDType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1);
static int decode_iso2_PaymentDetailsReqType(exi_bitstream_t* stream, struct iso2_PaymentDetailsReqType* PaymentDetailsReqType) {
    int grammar_id = 304;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PaymentDetailsReqType(PaymentDetailsReqType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 304:
            // Grammar: ID=304; read/write bits=1; START (eMAID)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (eMAID, eMAIDType (string)); next=305
                    // decode: string (len, characters)
                    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
                    if (error == 0)
                    {
                        if (eventCode == 0)
                        {
                            error = exi_basetypes_decoder_uint_16(stream, &PaymentDetailsReqType->eMAID.charactersLen);
                            if (error == 0)
                            {
                                if (PaymentDetailsReqType->eMAID.charactersLen >= 2)
                                {
                                    // string tables and table partitions are not supported, so the length has to be decremented by 2
                                    PaymentDetailsReqType->eMAID.charactersLen -= 2;
                                    error = exi_basetypes_decoder_characters(stream, PaymentDetailsReqType->eMAID.charactersLen, PaymentDetailsReqType->eMAID.characters, iso2_eMAID_CHARACTER_SIZE);
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
                                grammar_id = 305;
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
        case 305:
            // Grammar: ID=305; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ContractSignatureCertChain, CertificateChainType (CertificateChainType)); next=3
                    // decode: element
                    error = decode_iso2_CertificateChainType(stream, &PaymentDetailsReqType->ContractSignatureCertChain);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsRes; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); GenChallenge, genChallengeType (1, 1); EVSETimeStamp, long (1, 1);
static int decode_iso2_PaymentDetailsResType(exi_bitstream_t* stream, struct iso2_PaymentDetailsResType* PaymentDetailsResType) {
    int grammar_id = 306;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_PaymentDetailsResType(PaymentDetailsResType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 306:
            // Grammar: ID=306; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (ResponseCode, responseCodeType (string)); next=307
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
                                PaymentDetailsResType->ResponseCode = (iso2_responseCodeType)value;
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
                                grammar_id = 307;
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
        case 307:
            // Grammar: ID=307; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (GenChallenge, genChallengeType (base64Binary)); next=308
                    // decode exi type: base64Binary
                    error = decode_exi_type_hex_binary(stream, &PaymentDetailsResType->GenChallenge.bytesLen, &PaymentDetailsResType->GenChallenge.bytes[0], iso2_genChallengeType_BYTES_SIZE);
                    if (error == 0)
                    {
                        grammar_id = 308;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 308:
            // Grammar: ID=308; read/write bits=1; START (EVSETimeStamp)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (EVSETimeStamp, long (integer)); next=3
                    // decode: long int
                    error = decode_exi_type_integer64(stream, &PaymentDetailsResType->EVSETimeStamp);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Body; type={urn:iso:15118:2:2013:MsgBody}BodyType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AuthorizationReq, AuthorizationReqType (0, 1); AuthorizationRes, AuthorizationResType (0, 1); BodyElement, BodyBaseType (0, 1); CableCheckReq, CableCheckReqType (0, 1); CableCheckRes, CableCheckResType (0, 1); CertificateInstallationReq, CertificateInstallationReqType (0, 1); CertificateInstallationRes, CertificateInstallationResType (0, 1); CertificateUpdateReq, CertificateUpdateReqType (0, 1); CertificateUpdateRes, CertificateUpdateResType (0, 1); ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReqType (0, 1); ChargeParameterDiscoveryRes, ChargeParameterDiscoveryResType (0, 1); ChargingStatusReq, ChargingStatusReqType (0, 1); ChargingStatusRes, ChargingStatusResType (0, 1); CurrentDemandReq, CurrentDemandReqType (0, 1); CurrentDemandRes, CurrentDemandResType (0, 1); MeteringReceiptReq, MeteringReceiptReqType (0, 1); MeteringReceiptRes, MeteringReceiptResType (0, 1); PaymentDetailsReq, PaymentDetailsReqType (0, 1); PaymentDetailsRes, PaymentDetailsResType (0, 1); PaymentServiceSelectionReq, PaymentServiceSelectionReqType (0, 1); PaymentServiceSelectionRes, PaymentServiceSelectionResType (0, 1); PowerDeliveryReq, PowerDeliveryReqType (0, 1); PowerDeliveryRes, PowerDeliveryResType (0, 1); PreChargeReq, PreChargeReqType (0, 1); PreChargeRes, PreChargeResType (0, 1); ServiceDetailReq, ServiceDetailReqType (0, 1); ServiceDetailRes, ServiceDetailResType (0, 1); ServiceDiscoveryReq, ServiceDiscoveryReqType (0, 1); ServiceDiscoveryRes, ServiceDiscoveryResType (0, 1); SessionSetupReq, SessionSetupReqType (0, 1); SessionSetupRes, SessionSetupResType (0, 1); SessionStopReq, SessionStopReqType (0, 1); SessionStopRes, SessionStopResType (0, 1); WeldingDetectionReq, WeldingDetectionReqType (0, 1); WeldingDetectionRes, WeldingDetectionResType (0, 1);
static int decode_iso2_BodyType(exi_bitstream_t* stream, struct iso2_BodyType* BodyType) {
    int grammar_id = 309;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_BodyType(BodyType);

    while (!done)
    {
        switch (grammar_id)
        {
        case 309:
            // Grammar: ID=309; read bits=6; START (BodyMessage)
            error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: AuthorizationReq
                    // decode: namespace element
                    error = decode_iso2_AuthorizationReqType(stream, &BodyType->AuthorizationReq);
                    if (error == 0)
                    {
                        BodyType->AuthorizationReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 1:
                    // Event: AuthorizationRes
                    // decode: namespace element
                    error = decode_iso2_AuthorizationResType(stream, &BodyType->AuthorizationRes);
                    if (error == 0)
                    {
                        BodyType->AuthorizationRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 2:
                    // Event: BodyElement
                    // decode: namespace element
                    error = decode_iso2_BodyBaseType(stream, &BodyType->BodyElement);
                    if (error == 0)
                    {
                        BodyType->BodyElement_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 3:
                    // Event: CableCheckReq
                    // decode: namespace element
                    error = decode_iso2_CableCheckReqType(stream, &BodyType->CableCheckReq);
                    if (error == 0)
                    {
                        BodyType->CableCheckReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 4:
                    // Event: CableCheckRes
                    // decode: namespace element
                    error = decode_iso2_CableCheckResType(stream, &BodyType->CableCheckRes);
                    if (error == 0)
                    {
                        BodyType->CableCheckRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 5:
                    // Event: CertificateInstallationReq
                    // decode: namespace element
                    error = decode_iso2_CertificateInstallationReqType(stream, &BodyType->CertificateInstallationReq);
                    if (error == 0)
                    {
                        BodyType->CertificateInstallationReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 6:
                    // Event: CertificateInstallationRes
                    // decode: namespace element
                    error = decode_iso2_CertificateInstallationResType(stream, &BodyType->CertificateInstallationRes);
                    if (error == 0)
                    {
                        BodyType->CertificateInstallationRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 7:
                    // Event: CertificateUpdateReq
                    // decode: namespace element
                    error = decode_iso2_CertificateUpdateReqType(stream, &BodyType->CertificateUpdateReq);
                    if (error == 0)
                    {
                        BodyType->CertificateUpdateReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 8:
                    // Event: CertificateUpdateRes
                    // decode: namespace element
                    error = decode_iso2_CertificateUpdateResType(stream, &BodyType->CertificateUpdateRes);
                    if (error == 0)
                    {
                        BodyType->CertificateUpdateRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 9:
                    // Event: ChargeParameterDiscoveryReq
                    // decode: namespace element
                    error = decode_iso2_ChargeParameterDiscoveryReqType(stream, &BodyType->ChargeParameterDiscoveryReq);
                    if (error == 0)
                    {
                        BodyType->ChargeParameterDiscoveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 10:
                    // Event: ChargeParameterDiscoveryRes
                    // decode: namespace element
                    error = decode_iso2_ChargeParameterDiscoveryResType(stream, &BodyType->ChargeParameterDiscoveryRes);
                    if (error == 0)
                    {
                        BodyType->ChargeParameterDiscoveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 11:
                    // Event: ChargingStatusReq
                    // decode: namespace element
                    error = decode_iso2_ChargingStatusReqType(stream, &BodyType->ChargingStatusReq);
                    if (error == 0)
                    {
                        BodyType->ChargingStatusReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 12:
                    // Event: ChargingStatusRes
                    // decode: namespace element
                    error = decode_iso2_ChargingStatusResType(stream, &BodyType->ChargingStatusRes);
                    if (error == 0)
                    {
                        BodyType->ChargingStatusRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 13:
                    // Event: CurrentDemandReq
                    // decode: namespace element
                    error = decode_iso2_CurrentDemandReqType(stream, &BodyType->CurrentDemandReq);
                    if (error == 0)
                    {
                        BodyType->CurrentDemandReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 14:
                    // Event: CurrentDemandRes
                    // decode: namespace element
                    error = decode_iso2_CurrentDemandResType(stream, &BodyType->CurrentDemandRes);
                    if (error == 0)
                    {
                        BodyType->CurrentDemandRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 15:
                    // Event: MeteringReceiptReq
                    // decode: namespace element
                    error = decode_iso2_MeteringReceiptReqType(stream, &BodyType->MeteringReceiptReq);
                    if (error == 0)
                    {
                        BodyType->MeteringReceiptReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 16:
                    // Event: MeteringReceiptRes
                    // decode: namespace element
                    error = decode_iso2_MeteringReceiptResType(stream, &BodyType->MeteringReceiptRes);
                    if (error == 0)
                    {
                        BodyType->MeteringReceiptRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 17:
                    // Event: PaymentDetailsReq
                    // decode: namespace element
                    error = decode_iso2_PaymentDetailsReqType(stream, &BodyType->PaymentDetailsReq);
                    if (error == 0)
                    {
                        BodyType->PaymentDetailsReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 18:
                    // Event: PaymentDetailsRes
                    // decode: namespace element
                    error = decode_iso2_PaymentDetailsResType(stream, &BodyType->PaymentDetailsRes);
                    if (error == 0)
                    {
                        BodyType->PaymentDetailsRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 19:
                    // Event: PaymentServiceSelectionReq
                    // decode: namespace element
                    error = decode_iso2_PaymentServiceSelectionReqType(stream, &BodyType->PaymentServiceSelectionReq);
                    if (error == 0)
                    {
                        BodyType->PaymentServiceSelectionReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 20:
                    // Event: PaymentServiceSelectionRes
                    // decode: namespace element
                    error = decode_iso2_PaymentServiceSelectionResType(stream, &BodyType->PaymentServiceSelectionRes);
                    if (error == 0)
                    {
                        BodyType->PaymentServiceSelectionRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 21:
                    // Event: PowerDeliveryReq
                    // decode: namespace element
                    error = decode_iso2_PowerDeliveryReqType(stream, &BodyType->PowerDeliveryReq);
                    if (error == 0)
                    {
                        BodyType->PowerDeliveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 22:
                    // Event: PowerDeliveryRes
                    // decode: namespace element
                    error = decode_iso2_PowerDeliveryResType(stream, &BodyType->PowerDeliveryRes);
                    if (error == 0)
                    {
                        BodyType->PowerDeliveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 23:
                    // Event: PreChargeReq
                    // decode: namespace element
                    error = decode_iso2_PreChargeReqType(stream, &BodyType->PreChargeReq);
                    if (error == 0)
                    {
                        BodyType->PreChargeReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 24:
                    // Event: PreChargeRes
                    // decode: namespace element
                    error = decode_iso2_PreChargeResType(stream, &BodyType->PreChargeRes);
                    if (error == 0)
                    {
                        BodyType->PreChargeRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 25:
                    // Event: ServiceDetailReq
                    // decode: namespace element
                    error = decode_iso2_ServiceDetailReqType(stream, &BodyType->ServiceDetailReq);
                    if (error == 0)
                    {
                        BodyType->ServiceDetailReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 26:
                    // Event: ServiceDetailRes
                    // decode: namespace element
                    error = decode_iso2_ServiceDetailResType(stream, &BodyType->ServiceDetailRes);
                    if (error == 0)
                    {
                        BodyType->ServiceDetailRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 27:
                    // Event: ServiceDiscoveryReq
                    // decode: namespace element
                    error = decode_iso2_ServiceDiscoveryReqType(stream, &BodyType->ServiceDiscoveryReq);
                    if (error == 0)
                    {
                        BodyType->ServiceDiscoveryReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 28:
                    // Event: ServiceDiscoveryRes
                    // decode: namespace element
                    error = decode_iso2_ServiceDiscoveryResType(stream, &BodyType->ServiceDiscoveryRes);
                    if (error == 0)
                    {
                        BodyType->ServiceDiscoveryRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 29:
                    // Event: SessionSetupReq
                    // decode: namespace element
                    error = decode_iso2_SessionSetupReqType(stream, &BodyType->SessionSetupReq);
                    if (error == 0)
                    {
                        BodyType->SessionSetupReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 30:
                    // Event: SessionSetupRes
                    // decode: namespace element
                    error = decode_iso2_SessionSetupResType(stream, &BodyType->SessionSetupRes);
                    if (error == 0)
                    {
                        BodyType->SessionSetupRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 31:
                    // Event: SessionStopReq
                    // decode: namespace element
                    error = decode_iso2_SessionStopReqType(stream, &BodyType->SessionStopReq);
                    if (error == 0)
                    {
                        BodyType->SessionStopReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 32:
                    // Event: SessionStopRes
                    // decode: namespace element
                    error = decode_iso2_SessionStopResType(stream, &BodyType->SessionStopRes);
                    if (error == 0)
                    {
                        BodyType->SessionStopRes_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 33:
                    // Event: WeldingDetectionReq
                    // decode: namespace element
                    error = decode_iso2_WeldingDetectionReqType(stream, &BodyType->WeldingDetectionReq);
                    if (error == 0)
                    {
                        BodyType->WeldingDetectionReq_isUsed = 1u;
                        grammar_id = 3;
                    }
                    break;
                case 34:
                    // Event: WeldingDetectionRes
                    // decode: namespace element
                    error = decode_iso2_WeldingDetectionResType(stream, &BodyType->WeldingDetectionRes);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}V2G_Message; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Header, MessageHeaderType (1, 1); Body, BodyType (1, 1);
static int decode_iso2_V2G_Message(exi_bitstream_t* stream, struct iso2_V2G_Message* V2G_Message) {
    int grammar_id = 310;
    int done = 0;
    uint32_t eventCode;
    int error;

    init_iso2_V2G_Message(V2G_Message);

    while (!done)
    {
        switch (grammar_id)
        {
        case 310:
            // Grammar: ID=310; read/write bits=1; START (Header)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Header, MessageHeaderType (MessageHeaderType)); next=311
                    // decode: element
                    error = decode_iso2_MessageHeaderType(stream, &V2G_Message->Header);
                    if (error == 0)
                    {
                        grammar_id = 311;
                    }
                    break;
                default:
                    error = EXI_ERROR__UNKNOWN_EVENT_CODE;
                    break;
                }
            }
            break;
        case 311:
            // Grammar: ID=311; read/write bits=1; START (Body)
            error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
            if (error == 0)
            {
                switch (eventCode)
                {
                case 0:
                    // Event: START (Body, BodyType (BodyType)); next=3
                    // decode: element
                    error = decode_iso2_BodyType(stream, &V2G_Message->Body);
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
int decode_iso2_exiDocument(exi_bitstream_t* stream, struct iso2_exiDocument* exiDoc)
{
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_iso2_exiDocument(exiDoc);

        error = exi_basetypes_decoder_nbit_uint(stream, 7, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            switch (eventCode)
            {
            case 0:
            case 76:
                error = decode_iso2_V2G_Message(stream, &exiDoc->V2G_Message);
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }
        }
    }

    return error;
}

// main function for decoding fragment
/* NOTE! There may be problems when comparing the signature of the eMAID.
   In the ISO 15118-2 schema there are two different types with problematic names,
   EMAIDType and eMAIDType. The fragment de- and encoder of e.g. openV2G considers
   this type as generic type EXISchemaInformedElementFragmentGrammar. We treat it as a complex type.
   We have not yet been able to determine why this particular type has to be coded as a generic type,
   and only for the fragment decoder and encoder.
   This is why we have not yet adapted our fragment coders, and it can lead to the problem mentioned. */
int decode_iso2_exiFragment(exi_bitstream_t* stream, struct iso2_exiFragment* exiFrag) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_iso2_exiFragment(exiFrag);

        error = exi_basetypes_decoder_nbit_uint(stream, 8, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = EXI_ERROR__NOT_IMPLEMENTED_YET;
            switch (eventCode)
            {
            case 0:
                // AC_EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 1:
                // AC_EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 2:
                // AC_EVSEStatus (urn:iso:15118:2:2013:MsgBody)
                break;
            case 3:
                // AC_EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 4:
                // AuthorizationReq (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_AuthorizationReqType(stream, &exiFrag->AuthorizationReq);
                exiFrag->AuthorizationReq_isUsed = 1u;
                break;
            case 5:
                // AuthorizationRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 6:
                // Body (urn:iso:15118:2:2013:MsgDef)
                break;
            case 7:
                // BodyElement (urn:iso:15118:2:2013:MsgBody)
                break;
            case 8:
                // BulkChargingComplete (urn:iso:15118:2:2013:MsgBody)
                break;
            case 9:
                // BulkChargingComplete (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 10:
                // BulkSOC (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 11:
                // CableCheckReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 12:
                // CableCheckRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 13:
                // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 14:
                // Certificate (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 15:
                // CertificateInstallationReq (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_CertificateInstallationReqType(stream, &exiFrag->CertificateInstallationReq);
                exiFrag->CertificateInstallationReq_isUsed = 1u;
                break;
            case 16:
                // CertificateInstallationRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 17:
                // CertificateUpdateReq (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_CertificateUpdateReqType(stream, &exiFrag->CertificateUpdateReq);
                exiFrag->CertificateUpdateReq_isUsed = 1u;
                break;
            case 18:
                // CertificateUpdateRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 19:
                // ChargeParameterDiscoveryReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 20:
                // ChargeParameterDiscoveryRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 21:
                // ChargeProgress (urn:iso:15118:2:2013:MsgBody)
                break;
            case 22:
                // ChargeService (urn:iso:15118:2:2013:MsgBody)
                break;
            case 23:
                // ChargingComplete (urn:iso:15118:2:2013:MsgBody)
                break;
            case 24:
                // ChargingComplete (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 25:
                // ChargingProfile (urn:iso:15118:2:2013:MsgBody)
                break;
            case 26:
                // ChargingProfileEntryMaxNumberOfPhasesInUse (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 27:
                // ChargingProfileEntryMaxPower (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 28:
                // ChargingProfileEntryStart (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 29:
                // ChargingSession (urn:iso:15118:2:2013:MsgBody)
                break;
            case 30:
                // ChargingStatusReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 31:
                // ChargingStatusRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 32:
                // ConsumptionCost (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 33:
                // ContractSignatureCertChain (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_CertificateChainType(stream, &exiFrag->ContractSignatureCertChain);
                exiFrag->ContractSignatureCertChain_isUsed = 1u;
                break;
            case 34:
                // ContractSignatureEncryptedPrivateKey (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &exiFrag->ContractSignatureEncryptedPrivateKey);
                exiFrag->ContractSignatureEncryptedPrivateKey_isUsed = 1u;
                break;
            case 35:
                // Cost (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 36:
                // CurrentDemandReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 37:
                // CurrentDemandRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 38:
                // DC_EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 39:
                // DC_EVPowerDeliveryParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 40:
                // DC_EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 41:
                // DC_EVSEStatus (urn:iso:15118:2:2013:MsgBody)
                break;
            case 42:
                // DC_EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 43:
                // DC_EVStatus (urn:iso:15118:2:2013:MsgBody)
                break;
            case 44:
                // DC_EVStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 45:
                // DHpublickey (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_DiffieHellmanPublickeyType(stream, &exiFrag->DHpublickey);
                exiFrag->DHpublickey_isUsed = 1u;
                break;
            case 46:
                // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 47:
                // DepartureTime (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 48:
                // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 49:
                // DigestValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 50:
                // EAmount (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 51:
                // EPriceLevel (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 52:
                // EVCCID (urn:iso:15118:2:2013:MsgBody)
                break;
            case 53:
                // EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 54:
                // EVEnergyCapacity (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 55:
                // EVEnergyRequest (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 56:
                // EVErrorCode (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 57:
                // EVMaxCurrent (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 58:
                // EVMaxVoltage (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 59:
                // EVMaximumCurrentLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 60:
                // EVMaximumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 61:
                // EVMaximumPowerLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 62:
                // EVMaximumPowerLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 63:
                // EVMaximumVoltageLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 64:
                // EVMaximumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 65:
                // EVMinCurrent (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 66:
                // EVPowerDeliveryParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 67:
                // EVRESSSOC (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 68:
                // EVReady (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 69:
                // EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 70:
                // EVSECurrentLimitAchieved (urn:iso:15118:2:2013:MsgBody)
                break;
            case 71:
                // EVSECurrentRegulationTolerance (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 72:
                // EVSEEnergyToBeDelivered (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 73:
                // EVSEID (urn:iso:15118:2:2013:MsgBody)
                break;
            case 74:
                // EVSEIsolationStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 75:
                // EVSEMaxCurrent (urn:iso:15118:2:2013:MsgBody)
                break;
            case 76:
                // EVSEMaxCurrent (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 77:
                // EVSEMaximumCurrentLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 78:
                // EVSEMaximumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 79:
                // EVSEMaximumPowerLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 80:
                // EVSEMaximumPowerLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 81:
                // EVSEMaximumVoltageLimit (urn:iso:15118:2:2013:MsgBody)
                break;
            case 82:
                // EVSEMaximumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 83:
                // EVSEMinimumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 84:
                // EVSEMinimumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 85:
                // EVSENominalVoltage (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 86:
                // EVSENotification (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 87:
                // EVSEPeakCurrentRipple (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 88:
                // EVSEPowerLimitAchieved (urn:iso:15118:2:2013:MsgBody)
                break;
            case 89:
                // EVSEPresentCurrent (urn:iso:15118:2:2013:MsgBody)
                break;
            case 90:
                // EVSEPresentVoltage (urn:iso:15118:2:2013:MsgBody)
                break;
            case 91:
                // EVSEProcessing (urn:iso:15118:2:2013:MsgBody)
                break;
            case 92:
                // EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 93:
                // EVSEStatusCode (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 94:
                // EVSETimeStamp (urn:iso:15118:2:2013:MsgBody)
                break;
            case 95:
                // EVSEVoltageLimitAchieved (urn:iso:15118:2:2013:MsgBody)
                break;
            case 96:
                // EVStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 97:
                // EVTargetCurrent (urn:iso:15118:2:2013:MsgBody)
                break;
            case 98:
                // EVTargetVoltage (urn:iso:15118:2:2013:MsgBody)
                break;
            case 99:
                // EnergyTransferMode (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 100:
                // Entry (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 101:
                // Exponent (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 102:
                // FaultCode (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 103:
                // FaultMsg (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 104:
                // FreeService (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 105:
                // FullSOC (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 106:
                // G (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 107:
                // GenChallenge (urn:iso:15118:2:2013:MsgBody)
                break;
            case 108:
                // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 109:
                // Header (urn:iso:15118:2:2013:MsgDef)
                break;
            case 110:
                // J (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 111:
                // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 112:
                // KeyName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 113:
                // KeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 114:
                // ListOfRootCertificateIDs (urn:iso:15118:2:2013:MsgBody)
                break;
            case 115:
                // Manifest (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 116:
                // MaxEntriesSAScheduleTuple (urn:iso:15118:2:2013:MsgBody)
                break;
            case 117:
                // MeterID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 118:
                // MeterInfo (urn:iso:15118:2:2013:MsgBody)
                break;
            case 119:
                // MeterReading (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 120:
                // MeterStatus (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 121:
                // MeteringReceiptReq (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_MeteringReceiptReqType(stream, &exiFrag->MeteringReceiptReq);
                exiFrag->MeteringReceiptReq_isUsed = 1u;
                break;
            case 122:
                // MeteringReceiptRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 123:
                // MgmtData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 124:
                // Modulus (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 125:
                // Multiplier (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 126:
                // Notification (urn:iso:15118:2:2013:MsgHeader)
                break;
            case 127:
                // NotificationMaxDelay (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 128:
                // NumEPriceLevels (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 129:
                // OEMProvisioningCert (urn:iso:15118:2:2013:MsgBody)
                break;
            case 130:
                // Object (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 131:
                // P (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 132:
                // PGPData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 133:
                // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 134:
                // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 135:
                // PMax (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 136:
                // PMaxSchedule (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 137:
                // PMaxScheduleEntry (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 138:
                // Parameter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 139:
                // ParameterSet (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 140:
                // ParameterSetID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 141:
                // PaymentDetailsReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 142:
                // PaymentDetailsRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 143:
                // PaymentOption (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 144:
                // PaymentOptionList (urn:iso:15118:2:2013:MsgBody)
                break;
            case 145:
                // PaymentServiceSelectionReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 146:
                // PaymentServiceSelectionRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 147:
                // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 148:
                // PowerDeliveryReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 149:
                // PowerDeliveryRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 150:
                // PreChargeReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 151:
                // PreChargeRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 152:
                // ProfileEntry (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 153:
                // Q (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 154:
                // RCD (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 155:
                // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 156:
                // ReceiptRequired (urn:iso:15118:2:2013:MsgBody)
                break;
            case 157:
                // Reference (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 158:
                // RelativeTimeInterval (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 159:
                // RemainingTimeToBulkSoC (urn:iso:15118:2:2013:MsgBody)
                break;
            case 160:
                // RemainingTimeToFullSoC (urn:iso:15118:2:2013:MsgBody)
                break;
            case 161:
                // RequestedEnergyTransferMode (urn:iso:15118:2:2013:MsgBody)
                break;
            case 162:
                // ResponseCode (urn:iso:15118:2:2013:MsgBody)
                break;
            case 163:
                // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 164:
                // RetryCounter (urn:iso:15118:2:2013:MsgBody)
                break;
            case 165:
                // RootCertificateID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 166:
                // SAProvisioningCertificateChain (urn:iso:15118:2:2013:MsgBody)
                break;
            case 167:
                // SAScheduleList (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 168:
                // SAScheduleTuple (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 169:
                // SAScheduleTupleID (urn:iso:15118:2:2013:MsgBody)
                break;
            case 170:
                // SAScheduleTupleID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 171:
                // SASchedules (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 172:
                // SPKIData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 173:
                // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 174:
                // SalesTariff (urn:iso:15118:2:2013:MsgDataTypes)
                error = decode_iso2_SalesTariffType(stream, &exiFrag->SalesTariff);
                exiFrag->SalesTariff_isUsed = 1u;
                break;
            case 175:
                // SalesTariffDescription (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 176:
                // SalesTariffEntry (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 177:
                // SalesTariffID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 178:
                // Seed (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 179:
                // SelectedPaymentOption (urn:iso:15118:2:2013:MsgBody)
                break;
            case 180:
                // SelectedService (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 181:
                // SelectedServiceList (urn:iso:15118:2:2013:MsgBody)
                break;
            case 182:
                // Service (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 183:
                // ServiceCategory (urn:iso:15118:2:2013:MsgBody)
                break;
            case 184:
                // ServiceCategory (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 185:
                // ServiceDetailReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 186:
                // ServiceDetailRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 187:
                // ServiceDiscoveryReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 188:
                // ServiceDiscoveryRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 189:
                // ServiceID (urn:iso:15118:2:2013:MsgBody)
                break;
            case 190:
                // ServiceID (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 191:
                // ServiceList (urn:iso:15118:2:2013:MsgBody)
                break;
            case 192:
                // ServiceName (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 193:
                // ServiceParameterList (urn:iso:15118:2:2013:MsgBody)
                break;
            case 194:
                // ServiceScope (urn:iso:15118:2:2013:MsgBody)
                break;
            case 195:
                // ServiceScope (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 196:
                // SessionID (urn:iso:15118:2:2013:MsgBody)
                break;
            case 197:
                // SessionID (urn:iso:15118:2:2013:MsgHeader)
                break;
            case 198:
                // SessionSetupReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 199:
                // SessionSetupRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 200:
                // SessionStopReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 201:
                // SessionStopRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 202:
                // SigMeterReading (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 203:
                // Signature (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 204:
                // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 205:
                // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 206:
                // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 207:
                // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 208:
                // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SignedInfoType(stream, &exiFrag->SignedInfo);
                exiFrag->SignedInfo_isUsed = 1u;
                break;
            case 209:
                // SubCertificates (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 210:
                // SupportedEnergyTransferMode (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 211:
                // TMeter (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 212:
                // TimeInterval (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 213:
                // Transform (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 214:
                // Transforms (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 215:
                // Unit (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 216:
                // V2G_Message (urn:iso:15118:2:2013:MsgDef)
                break;
            case 217:
                // Value (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 218:
                // WeldingDetectionReq (urn:iso:15118:2:2013:MsgBody)
                break;
            case 219:
                // WeldingDetectionRes (urn:iso:15118:2:2013:MsgBody)
                break;
            case 220:
                // X509CRL (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 221:
                // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 222:
                // X509Data (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 223:
                // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 224:
                // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 225:
                // X509SKI (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 226:
                // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 227:
                // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 228:
                // XPath (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 229:
                // Y (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 230:
                // amount (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 231:
                // amountMultiplier (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 232:
                // boolValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 233:
                // byteValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 234:
                // costKind (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 235:
                // duration (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 236:
                // eMAID (urn:iso:15118:2:2013:MsgBody)
                error = decode_iso2_EMAIDType(stream, &exiFrag->eMAID);
                exiFrag->eMAID_isUsed = 1u;
                break;
            case 237:
                // intValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 238:
                // physicalValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 239:
                // shortValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 240:
                // start (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 241:
                // startValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            case 242:
                // stringValue (urn:iso:15118:2:2013:MsgDataTypes)
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }

            if (error == EXI_ERROR__NO_ERROR)
            {
                // End Fragment
                error = exi_basetypes_decoder_nbit_uint(stream, 8, &eventCode);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    if (eventCode != 244)
                    {
                        error = EXI_ERROR__INCORRECT_END_FRAGMENT_VALUE;
                    }
                }
            }
        }
    }

    return error;
}

// main function for decoding xmldsig fragment
int decode_iso2_xmldsigFragment(exi_bitstream_t* stream, struct iso2_xmldsigFragment* xmldsigFrag) {
    uint32_t eventCode;
    int error = exi_header_read_and_check(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        init_iso2_xmldsigFragment(xmldsigFrag);

        error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = EXI_ERROR__NOT_IMPLEMENTED_YET;
            switch (eventCode)
            {
            case 0:
                // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_CanonicalizationMethodType(stream, &xmldsigFrag->CanonicalizationMethod);
                xmldsigFrag->CanonicalizationMethod_isUsed = 1u;
                break;
            case 1:
                // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_DSAKeyValueType(stream, &xmldsigFrag->DSAKeyValue);
                xmldsigFrag->DSAKeyValue_isUsed = 1u;
                break;
            case 2:
                // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_DigestMethodType(stream, &xmldsigFrag->DigestMethod);
                xmldsigFrag->DigestMethod_isUsed = 1u;
                break;
            case 3:
                // DigestValue (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 4:
                // Exponent (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 5:
                // G (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 6:
                // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 7:
                // J (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 8:
                // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_KeyInfoType(stream, &xmldsigFrag->KeyInfo);
                xmldsigFrag->KeyInfo_isUsed = 1u;
                break;
            case 9:
                // KeyName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 10:
                // KeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_KeyValueType(stream, &xmldsigFrag->KeyValue);
                xmldsigFrag->KeyValue_isUsed = 1u;
                break;
            case 11:
                // Manifest (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 12:
                // MgmtData (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 13:
                // Modulus (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 14:
                // Object (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_ObjectType(stream, &xmldsigFrag->Object);
                xmldsigFrag->Object_isUsed = 1u;
                break;
            case 15:
                // P (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 16:
                // PGPData (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_PGPDataType(stream, &xmldsigFrag->PGPData);
                xmldsigFrag->PGPData_isUsed = 1u;
                break;
            case 17:
                // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 18:
                // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 19:
                // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 20:
                // Q (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 21:
                // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_RSAKeyValueType(stream, &xmldsigFrag->RSAKeyValue);
                xmldsigFrag->RSAKeyValue_isUsed = 1u;
                break;
            case 22:
                // Reference (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_ReferenceType(stream, &xmldsigFrag->Reference);
                xmldsigFrag->Reference_isUsed = 1u;
                break;
            case 23:
                // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_RetrievalMethodType(stream, &xmldsigFrag->RetrievalMethod);
                xmldsigFrag->RetrievalMethod_isUsed = 1u;
                break;
            case 24:
                // SPKIData (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SPKIDataType(stream, &xmldsigFrag->SPKIData);
                xmldsigFrag->SPKIData_isUsed = 1u;
                break;
            case 25:
                // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 26:
                // Seed (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 27:
                // Signature (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SignatureType(stream, &xmldsigFrag->Signature);
                xmldsigFrag->Signature_isUsed = 1u;
                break;
            case 28:
                // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SignatureMethodType(stream, &xmldsigFrag->SignatureMethod);
                xmldsigFrag->SignatureMethod_isUsed = 1u;
                break;
            case 29:
                // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 30:
                // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 31:
                // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SignatureValueType(stream, &xmldsigFrag->SignatureValue);
                xmldsigFrag->SignatureValue_isUsed = 1u;
                break;
            case 32:
                // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_SignedInfoType(stream, &xmldsigFrag->SignedInfo);
                xmldsigFrag->SignedInfo_isUsed = 1u;
                break;
            case 33:
                // Transform (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_TransformType(stream, &xmldsigFrag->Transform);
                xmldsigFrag->Transform_isUsed = 1u;
                break;
            case 34:
                // Transforms (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_TransformsType(stream, &xmldsigFrag->Transforms);
                xmldsigFrag->Transforms_isUsed = 1u;
                break;
            case 35:
                // X509CRL (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 36:
                // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 37:
                // X509Data (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_X509DataType(stream, &xmldsigFrag->X509Data);
                xmldsigFrag->X509Data_isUsed = 1u;
                break;
            case 38:
                // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 39:
                // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
                error = decode_iso2_X509IssuerSerialType(stream, &xmldsigFrag->X509IssuerSerial);
                xmldsigFrag->X509IssuerSerial_isUsed = 1u;
                break;
            case 40:
                // X509SKI (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 41:
                // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 42:
                // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 43:
                // XPath (http://www.w3.org/2000/09/xmldsig#)
                break;
            case 44:
                // Y (http://www.w3.org/2000/09/xmldsig#)
                break;
            default:
                error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
                break;
            }

            if (error == EXI_ERROR__NO_ERROR)
            {
                // End Fragment
                error = exi_basetypes_decoder_nbit_uint(stream, 6, &eventCode);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    if (eventCode != 46)
                    {
                        error = EXI_ERROR__INCORRECT_END_FRAGMENT_VALUE;
                    }
                }
            }
        }
    }

    return error;
}


