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
  * @file iso2_msgDefEncoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_basetypes_encoder.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"
#include "cbv2g/iso_2/iso2_msgDefDatatypes.h"
#include "cbv2g/iso_2/iso2_msgDefEncoder.h"



static int encode_iso2_CostType(exi_bitstream_t* stream, const struct iso2_CostType* CostType);
static int encode_iso2_TransformType(exi_bitstream_t* stream, const struct iso2_TransformType* TransformType);
static int encode_iso2_IntervalType(exi_bitstream_t* stream, const struct iso2_IntervalType* IntervalType);
static int encode_iso2_TransformsType(exi_bitstream_t* stream, const struct iso2_TransformsType* TransformsType);
static int encode_iso2_DSAKeyValueType(exi_bitstream_t* stream, const struct iso2_DSAKeyValueType* DSAKeyValueType);
static int encode_iso2_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso2_X509IssuerSerialType* X509IssuerSerialType);
static int encode_iso2_RelativeTimeIntervalType(exi_bitstream_t* stream, const struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType);
static int encode_iso2_DigestMethodType(exi_bitstream_t* stream, const struct iso2_DigestMethodType* DigestMethodType);
static int encode_iso2_RSAKeyValueType(exi_bitstream_t* stream, const struct iso2_RSAKeyValueType* RSAKeyValueType);
static int encode_iso2_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso2_CanonicalizationMethodType* CanonicalizationMethodType);
static int encode_iso2_SignatureMethodType(exi_bitstream_t* stream, const struct iso2_SignatureMethodType* SignatureMethodType);
static int encode_iso2_KeyValueType(exi_bitstream_t* stream, const struct iso2_KeyValueType* KeyValueType);
static int encode_iso2_PhysicalValueType(exi_bitstream_t* stream, const struct iso2_PhysicalValueType* PhysicalValueType);
static int encode_iso2_ConsumptionCostType(exi_bitstream_t* stream, const struct iso2_ConsumptionCostType* ConsumptionCostType);
static int encode_iso2_PMaxScheduleEntryType(exi_bitstream_t* stream, const struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType);
static int encode_iso2_SalesTariffEntryType(exi_bitstream_t* stream, const struct iso2_SalesTariffEntryType* SalesTariffEntryType);
static int encode_iso2_ParameterType(exi_bitstream_t* stream, const struct iso2_ParameterType* ParameterType);
static int encode_iso2_PMaxScheduleType(exi_bitstream_t* stream, const struct iso2_PMaxScheduleType* PMaxScheduleType);
static int encode_iso2_ReferenceType(exi_bitstream_t* stream, const struct iso2_ReferenceType* ReferenceType);
static int encode_iso2_RetrievalMethodType(exi_bitstream_t* stream, const struct iso2_RetrievalMethodType* RetrievalMethodType);
static int encode_iso2_SalesTariffType(exi_bitstream_t* stream, const struct iso2_SalesTariffType* SalesTariffType);
static int encode_iso2_X509DataType(exi_bitstream_t* stream, const struct iso2_X509DataType* X509DataType);
static int encode_iso2_PGPDataType(exi_bitstream_t* stream, const struct iso2_PGPDataType* PGPDataType);
static int encode_iso2_SPKIDataType(exi_bitstream_t* stream, const struct iso2_SPKIDataType* SPKIDataType);
static int encode_iso2_SignedInfoType(exi_bitstream_t* stream, const struct iso2_SignedInfoType* SignedInfoType);
static int encode_iso2_ProfileEntryType(exi_bitstream_t* stream, const struct iso2_ProfileEntryType* ProfileEntryType);
static int encode_iso2_DC_EVStatusType(exi_bitstream_t* stream, const struct iso2_DC_EVStatusType* DC_EVStatusType);
static int encode_iso2_ParameterSetType(exi_bitstream_t* stream, const struct iso2_ParameterSetType* ParameterSetType);
static int encode_iso2_SAScheduleTupleType(exi_bitstream_t* stream, const struct iso2_SAScheduleTupleType* SAScheduleTupleType);
static int encode_iso2_SelectedServiceType(exi_bitstream_t* stream, const struct iso2_SelectedServiceType* SelectedServiceType);
static int encode_iso2_ServiceType(exi_bitstream_t* stream, const struct iso2_ServiceType* ServiceType);
static int encode_iso2_SignatureValueType(exi_bitstream_t* stream, const struct iso2_SignatureValueType* SignatureValueType);
static int encode_iso2_SubCertificatesType(exi_bitstream_t* stream, const struct iso2_SubCertificatesType* SubCertificatesType);
static int encode_iso2_KeyInfoType(exi_bitstream_t* stream, const struct iso2_KeyInfoType* KeyInfoType);
static int encode_iso2_ObjectType(exi_bitstream_t* stream, const struct iso2_ObjectType* ObjectType);
static int encode_iso2_SupportedEnergyTransferModeType(exi_bitstream_t* stream, const struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType);
static int encode_iso2_CertificateChainType(exi_bitstream_t* stream, const struct iso2_CertificateChainType* CertificateChainType);
static int encode_iso2_BodyBaseType(exi_bitstream_t* stream, const struct iso2_BodyBaseType* BodyBaseType);
static int encode_iso2_NotificationType(exi_bitstream_t* stream, const struct iso2_NotificationType* NotificationType);
static int encode_iso2_DC_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_DC_EVSEStatusType* DC_EVSEStatusType);
static int encode_iso2_SelectedServiceListType(exi_bitstream_t* stream, const struct iso2_SelectedServiceListType* SelectedServiceListType);
static int encode_iso2_PaymentOptionListType(exi_bitstream_t* stream, const struct iso2_PaymentOptionListType* PaymentOptionListType);
static int encode_iso2_SignatureType(exi_bitstream_t* stream, const struct iso2_SignatureType* SignatureType);
static int encode_iso2_ChargingProfileType(exi_bitstream_t* stream, const struct iso2_ChargingProfileType* ChargingProfileType);
static int encode_iso2_ServiceParameterListType(exi_bitstream_t* stream, const struct iso2_ServiceParameterListType* ServiceParameterListType);
static int encode_iso2_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
static int encode_iso2_AC_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType);
static int encode_iso2_DC_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType);
static int encode_iso2_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_EVChargeParameterType* EVChargeParameterType);
static int encode_iso2_SASchedulesType(exi_bitstream_t* stream, const struct iso2_SASchedulesType* SASchedulesType);
static int encode_iso2_SAScheduleListType(exi_bitstream_t* stream, const struct iso2_SAScheduleListType* SAScheduleListType);
static int encode_iso2_ChargeServiceType(exi_bitstream_t* stream, const struct iso2_ChargeServiceType* ChargeServiceType);
static int encode_iso2_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType);
static int encode_iso2_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType);
static int encode_iso2_ContractSignatureEncryptedPrivateKeyType(exi_bitstream_t* stream, const struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType);
static int encode_iso2_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_EVSEChargeParameterType* EVSEChargeParameterType);
static int encode_iso2_DC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType);
static int encode_iso2_ServiceListType(exi_bitstream_t* stream, const struct iso2_ServiceListType* ServiceListType);
static int encode_iso2_DiffieHellmanPublickeyType(exi_bitstream_t* stream, const struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType);
static int encode_iso2_EMAIDType(exi_bitstream_t* stream, const struct iso2_EMAIDType* EMAIDType);
static int encode_iso2_AC_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_AC_EVSEStatusType* AC_EVSEStatusType);
static int encode_iso2_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_EVSEStatusType* EVSEStatusType);
static int encode_iso2_AC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType);
static int encode_iso2_MeterInfoType(exi_bitstream_t* stream, const struct iso2_MeterInfoType* MeterInfoType);
static int encode_iso2_MessageHeaderType(exi_bitstream_t* stream, const struct iso2_MessageHeaderType* MessageHeaderType);
static int encode_iso2_PowerDeliveryReqType(exi_bitstream_t* stream, const struct iso2_PowerDeliveryReqType* PowerDeliveryReqType);
static int encode_iso2_CurrentDemandResType(exi_bitstream_t* stream, const struct iso2_CurrentDemandResType* CurrentDemandResType);
static int encode_iso2_ChargingStatusResType(exi_bitstream_t* stream, const struct iso2_ChargingStatusResType* ChargingStatusResType);
static int encode_iso2_AuthorizationReqType(exi_bitstream_t* stream, const struct iso2_AuthorizationReqType* AuthorizationReqType);
static int encode_iso2_PreChargeReqType(exi_bitstream_t* stream, const struct iso2_PreChargeReqType* PreChargeReqType);
static int encode_iso2_ServiceDetailResType(exi_bitstream_t* stream, const struct iso2_ServiceDetailResType* ServiceDetailResType);
static int encode_iso2_PaymentServiceSelectionResType(exi_bitstream_t* stream, const struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType);
static int encode_iso2_CertificateUpdateReqType(exi_bitstream_t* stream, const struct iso2_CertificateUpdateReqType* CertificateUpdateReqType);
static int encode_iso2_SessionSetupResType(exi_bitstream_t* stream, const struct iso2_SessionSetupResType* SessionSetupResType);
static int encode_iso2_CertificateInstallationReqType(exi_bitstream_t* stream, const struct iso2_CertificateInstallationReqType* CertificateInstallationReqType);
static int encode_iso2_CertificateInstallationResType(exi_bitstream_t* stream, const struct iso2_CertificateInstallationResType* CertificateInstallationResType);
static int encode_iso2_WeldingDetectionResType(exi_bitstream_t* stream, const struct iso2_WeldingDetectionResType* WeldingDetectionResType);
static int encode_iso2_CurrentDemandReqType(exi_bitstream_t* stream, const struct iso2_CurrentDemandReqType* CurrentDemandReqType);
static int encode_iso2_PreChargeResType(exi_bitstream_t* stream, const struct iso2_PreChargeResType* PreChargeResType);
static int encode_iso2_CertificateUpdateResType(exi_bitstream_t* stream, const struct iso2_CertificateUpdateResType* CertificateUpdateResType);
static int encode_iso2_MeteringReceiptReqType(exi_bitstream_t* stream, const struct iso2_MeteringReceiptReqType* MeteringReceiptReqType);
static int encode_iso2_ChargingStatusReqType(exi_bitstream_t* stream, const struct iso2_ChargingStatusReqType* ChargingStatusReqType);
static int encode_iso2_SessionStopResType(exi_bitstream_t* stream, const struct iso2_SessionStopResType* SessionStopResType);
static int encode_iso2_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType);
static int encode_iso2_CableCheckReqType(exi_bitstream_t* stream, const struct iso2_CableCheckReqType* CableCheckReqType);
static int encode_iso2_WeldingDetectionReqType(exi_bitstream_t* stream, const struct iso2_WeldingDetectionReqType* WeldingDetectionReqType);
static int encode_iso2_PowerDeliveryResType(exi_bitstream_t* stream, const struct iso2_PowerDeliveryResType* PowerDeliveryResType);
static int encode_iso2_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType);
static int encode_iso2_PaymentServiceSelectionReqType(exi_bitstream_t* stream, const struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType);
static int encode_iso2_MeteringReceiptResType(exi_bitstream_t* stream, const struct iso2_MeteringReceiptResType* MeteringReceiptResType);
static int encode_iso2_CableCheckResType(exi_bitstream_t* stream, const struct iso2_CableCheckResType* CableCheckResType);
static int encode_iso2_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType);
static int encode_iso2_ServiceDetailReqType(exi_bitstream_t* stream, const struct iso2_ServiceDetailReqType* ServiceDetailReqType);
static int encode_iso2_SessionSetupReqType(exi_bitstream_t* stream, const struct iso2_SessionSetupReqType* SessionSetupReqType);
static int encode_iso2_SessionStopReqType(exi_bitstream_t* stream, const struct iso2_SessionStopReqType* SessionStopReqType);
static int encode_iso2_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
static int encode_iso2_AuthorizationResType(exi_bitstream_t* stream, const struct iso2_AuthorizationResType* AuthorizationResType);
static int encode_iso2_PaymentDetailsReqType(exi_bitstream_t* stream, const struct iso2_PaymentDetailsReqType* PaymentDetailsReqType);
static int encode_iso2_PaymentDetailsResType(exi_bitstream_t* stream, const struct iso2_PaymentDetailsResType* PaymentDetailsResType);
static int encode_iso2_BodyType(exi_bitstream_t* stream, const struct iso2_BodyType* BodyType);
static int encode_iso2_V2G_Message(exi_bitstream_t* stream, const struct iso2_V2G_Message* V2G_Message);

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Cost; type={urn:iso:15118:2:2013:MsgDataTypes}CostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: costKind, costKindType (1, 1); amount, unsignedInt (1, 1); amountMultiplier, unitMultiplierType (0, 1);
static int encode_iso2_CostType(exi_bitstream_t* stream, const struct iso2_CostType* CostType) {
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int encode_iso2_TransformType(exi_bitstream_t* stream, const struct iso2_TransformType* TransformType) {
    int grammar_id = 5;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 5:
            // Grammar: ID=5; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=6

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 6;
                    }
                }
            }
            break;
        case 6:
            // Grammar: ID=6; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso2_XPath_CHARACTER_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, TransformType->ANY.bytesLen, TransformType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}TimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}IntervalType; base type=; content type=empty;
//          abstract=True; final=False;
static int encode_iso2_IntervalType(exi_bitstream_t* stream, const struct iso2_IntervalType* IntervalType) {
    // Element has no particles, so the function just encodes END Element
    (void)IntervalType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transforms; type={http://www.w3.org/2000/09/xmldsig#}TransformsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Transform, TransformType (1, 1) (original max unbounded);
static int encode_iso2_TransformsType(exi_bitstream_t* stream, const struct iso2_TransformsType* TransformsType) {
    int grammar_id = 7;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 7:
            // Grammar: ID=7; read/write bits=1; START (Transform)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (TransformType); next=8
                error = encode_iso2_TransformType(stream, &TransformsType->Transform);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 8;
                }
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (Transform), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transform, TransformType); next=3
                    error = encode_iso2_TransformType(stream, &TransformsType->Transform);
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
static int encode_iso2_DSAKeyValueType(exi_bitstream_t* stream, const struct iso2_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 9;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 9:
            // Grammar: ID=9; read/write bits=2; START (P), START (G), START (Y)
            if (DSAKeyValueType->P_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (P, base64Binary); next=10
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->P.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->P.bytesLen, DSAKeyValueType->P.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 10;
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
                    // Event: START (G, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 12;
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
                    // Event: START (Y, base64Binary); next=13
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 13;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 10:
            // Grammar: ID=10; read/write bits=1; START (Q)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=11
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Q.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Q.bytesLen, DSAKeyValueType->Q.bytes, iso2_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 11;
                            }
                        }
                    }
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (G), START (Y)
            if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 12;
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
                    // Event: START (Y, base64Binary); next=13
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 13;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=1; START (Y)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=13
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso2_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 13;
                            }
                        }
                    }
                }
            }
            break;
        case 13:
            // Grammar: ID=13; read/write bits=2; START (J), START (Seed), END Element
            if (DSAKeyValueType->J_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (J, base64Binary); next=14
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->J.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->J.bytesLen, DSAKeyValueType->J.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 14;
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
                    // Event: START (Seed, base64Binary); next=15
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 15;
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
        case 14:
            // Grammar: ID=14; read/write bits=2; START (Seed), END Element
            if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=15
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso2_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 15;
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
        case 15:
            // Grammar: ID=15; read/write bits=2; START (PgenCounter), END Element
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
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->PgenCounter.bytesLen, DSAKeyValueType->PgenCounter.bytes, iso2_CryptoBinary_BYTES_SIZE);
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
static int encode_iso2_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso2_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 16;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 16:
            // Grammar: ID=16; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=17

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509IssuerSerialType->X509IssuerName.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso2_X509IssuerName_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 17;
                            }
                        }
                    }
                }
            }
            break;
        case 17:
            // Grammar: ID=17; read/write bits=1; START (X509SerialNumber)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeInterval; type={urn:iso:15118:2:2013:MsgDataTypes}RelativeTimeIntervalType; base type=IntervalType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: start, AnonType (1, 1); duration, AnonType (0, 1);
static int encode_iso2_RelativeTimeIntervalType(exi_bitstream_t* stream, const struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    int grammar_id = 18;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 18:
            // Grammar: ID=18; read/write bits=1; START (start)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=19
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
                            grammar_id = 19;
                        }
                    }
                }
            }
            break;
        case 19:
            // Grammar: ID=19; read/write bits=2; START (duration), END Element
            if (RelativeTimeIntervalType->duration_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (duration, unsignedInt); next=3
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}DigestMethod; type={http://www.w3.org/2000/09/xmldsig#}DigestMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int encode_iso2_DigestMethodType(exi_bitstream_t* stream, const struct iso2_DigestMethodType* DigestMethodType) {
    int grammar_id = 20;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 20:
            // Grammar: ID=20; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=21

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(DigestMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 21;
                    }
                }
            }
            break;
        case 21:
            // Grammar: ID=21; read/write bits=2; START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_bytes(stream, DigestMethodType->ANY.bytesLen, DigestMethodType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
static int encode_iso2_RSAKeyValueType(exi_bitstream_t* stream, const struct iso2_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 22;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 22:
            // Grammar: ID=22; read/write bits=1; START (Modulus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=23
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Modulus.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Modulus.bytesLen, RSAKeyValueType->Modulus.bytes, iso2_CryptoBinary_BYTES_SIZE);
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
            break;
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Exponent)
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
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Exponent.bytesLen, RSAKeyValueType->Exponent.bytes, iso2_CryptoBinary_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethod; type={http://www.w3.org/2000/09/xmldsig#}CanonicalizationMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1);
static int encode_iso2_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso2_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 24;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 24:
            // Grammar: ID=24; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=25

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CanonicalizationMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 25;
                    }
                }
            }
            break;
        case 25:
            // Grammar: ID=25; read/write bits=2; START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_bytes(stream, CanonicalizationMethodType->ANY.bytesLen, CanonicalizationMethodType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureMethod; type={http://www.w3.org/2000/09/xmldsig#}SignatureMethodType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Algorithm, anyURI (1, 1); HMACOutputLength, HMACOutputLengthType (0, 1); ANY, anyType (0, 1);
static int encode_iso2_SignatureMethodType(exi_bitstream_t* stream, const struct iso2_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 26;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 26:
            // Grammar: ID=26; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=27

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso2_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 27;
                    }
                }
            }
            break;
        case 27:
            // Grammar: ID=27; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            if (SignatureMethodType->HMACOutputLength_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (HMACOutputLength, integer); next=28
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
                                grammar_id = 28;
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
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
        case 28:
            // Grammar: ID=28; read/write bits=2; START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
static int encode_iso2_KeyValueType(exi_bitstream_t* stream, const struct iso2_KeyValueType* KeyValueType) {
    int grammar_id = 29;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 29:
            // Grammar: ID=29; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            if (KeyValueType->DSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DSAKeyValue, DSAKeyValueType); next=3
                    error = encode_iso2_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
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
                    error = encode_iso2_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
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
                            error = exi_basetypes_encoder_bytes(stream, KeyValueType->ANY.bytesLen, KeyValueType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileEntryMaxPower; type={urn:iso:15118:2:2013:MsgDataTypes}PhysicalValueType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Multiplier, unitMultiplierType (1, 1); Unit, unitSymbolType (1, 1); Value, short (1, 1);
static int encode_iso2_PhysicalValueType(exi_bitstream_t* stream, const struct iso2_PhysicalValueType* PhysicalValueType) {
    int grammar_id = 30;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 30:
            // Grammar: ID=30; read/write bits=1; START (Multiplier)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (byte); next=31
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
                            grammar_id = 31;
                        }
                    }
                }
            }
            break;
        case 31:
            // Grammar: ID=31; read/write bits=1; START (Unit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=32
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, PhysicalValueType->Unit);
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
            break;
        case 32:
            // Grammar: ID=32; read/write bits=1; START (Value)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCost; type={urn:iso:15118:2:2013:MsgDataTypes}ConsumptionCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: startValue, PhysicalValueType (1, 1); Cost, CostType (1, 3);
static int encode_iso2_ConsumptionCostType(exi_bitstream_t* stream, const struct iso2_ConsumptionCostType* ConsumptionCostType) {
    int grammar_id = 33;
    int done = 0;
    int error = 0;
    uint16_t Cost_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 33:
            // Grammar: ID=33; read/write bits=1; START (startValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=34
                error = encode_iso2_PhysicalValueType(stream, &ConsumptionCostType->startValue);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 34;
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=1; START (Cost)
            if (Cost_currentIndex < ConsumptionCostType->Cost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CostType); next=35
                    error = encode_iso2_CostType(stream, &ConsumptionCostType->Cost.array[Cost_currentIndex++]);
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
            // Grammar: ID=35; read/write bits=2; LOOP (Cost), END Element
            if (Cost_currentIndex < ConsumptionCostType->Cost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (CostType); next=35
                    error = encode_iso2_CostType(stream, &ConsumptionCostType->Cost.array[Cost_currentIndex++]);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntry; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); PMax, PhysicalValueType (1, 1);
static int encode_iso2_PMaxScheduleEntryType(exi_bitstream_t* stream, const struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    int grammar_id = 36;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            if (PMaxScheduleEntryType->RelativeTimeInterval_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RelativeTimeInterval, IntervalType); next=37
                    error = encode_iso2_RelativeTimeIntervalType(stream, &PMaxScheduleEntryType->RelativeTimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 37;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (IntervalType); next=37
                    error = encode_iso2_IntervalType(stream, &PMaxScheduleEntryType->TimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 37;
                    }
                }
            }
            break;
        case 37:
            // Grammar: ID=37; read/write bits=1; START (PMax)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &PMaxScheduleEntryType->PMax);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntry; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffEntryType; base type=EntryType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: RelativeTimeInterval, RelativeTimeIntervalType (0, 1); TimeInterval, IntervalType (0, 1); EPriceLevel, unsignedByte (0, 1); ConsumptionCost, ConsumptionCostType (0, 3);
static int encode_iso2_SalesTariffEntryType(exi_bitstream_t* stream, const struct iso2_SalesTariffEntryType* SalesTariffEntryType) {
    int grammar_id = 38;
    int done = 0;
    int error = 0;
    uint16_t ConsumptionCost_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 38:
            // Grammar: ID=38; read/write bits=2; START (RelativeTimeInterval), START (TimeInterval)
            if (SalesTariffEntryType->RelativeTimeInterval_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RelativeTimeInterval, IntervalType); next=39
                    error = encode_iso2_RelativeTimeIntervalType(stream, &SalesTariffEntryType->RelativeTimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 39;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (IntervalType); next=39
                    error = encode_iso2_IntervalType(stream, &SalesTariffEntryType->TimeInterval);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 39;
                    }
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=2; START (EPriceLevel), START (ConsumptionCost), END Element
            if (SalesTariffEntryType->EPriceLevel_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EPriceLevel, unsignedShort); next=41
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
                                grammar_id = 41;
                            }
                        }
                    }
                }
            }
            else if (ConsumptionCost_currentIndex < SalesTariffEntryType->ConsumptionCost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ConsumptionCost, ConsumptionCostType); next=40 (optional array)
                    error = encode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[ConsumptionCost_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 40;
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
        case 40:
            // Grammar: ID=40; read/write bits=2; LOOP (ConsumptionCost), END Element
            if (ConsumptionCost_currentIndex < SalesTariffEntryType->ConsumptionCost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ConsumptionCost, ConsumptionCostType); next=40 (optional array)
                    error = encode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[ConsumptionCost_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 40;
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
        case 41:
            // Grammar: ID=41; read/write bits=2; START (ConsumptionCost), END Element
            if (ConsumptionCost_currentIndex < SalesTariffEntryType->ConsumptionCost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ConsumptionCost, ConsumptionCostType); next=42 (optional array)
                    error = encode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[ConsumptionCost_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 42;
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
        case 42:
            // Grammar: ID=42; read/write bits=2; LOOP (ConsumptionCost), END Element
            if (ConsumptionCost_currentIndex < SalesTariffEntryType->ConsumptionCost.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ConsumptionCost, ConsumptionCostType); next=42 (optional array)
                    error = encode_iso2_ConsumptionCostType(stream, &SalesTariffEntryType->ConsumptionCost.array[ConsumptionCost_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 42;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Parameter; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, string (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); physicalValue, PhysicalValueType (0, 1); stringValue, string (0, 1);
static int encode_iso2_ParameterType(exi_bitstream_t* stream, const struct iso2_ParameterType* ParameterType) {
    int grammar_id = 43;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 43:
            // Grammar: ID=43; read/write bits=1; START (Name)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=44

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ParameterType->Name.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, ParameterType->Name.charactersLen, ParameterType->Name.characters, iso2_Name_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 44;
                    }
                }
            }
            break;
        case 44:
            // Grammar: ID=44; read/write bits=3; START (boolValue), START (byteValue), START (shortValue), START (intValue), START (physicalValue), START (stringValue)
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
                    error = encode_iso2_PhysicalValueType(stream, &ParameterType->physicalValue);
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
                            error = exi_basetypes_encoder_characters(stream, ParameterType->stringValue.charactersLen, ParameterType->stringValue.characters, iso2_stringValue_CHARACTER_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}PMaxSchedule; type={urn:iso:15118:2:2013:MsgDataTypes}PMaxScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PMaxScheduleEntry, PMaxScheduleEntryType (1, 12) (original max 1024);
static int encode_iso2_PMaxScheduleType(exi_bitstream_t* stream, const struct iso2_PMaxScheduleType* PMaxScheduleType) {
    int grammar_id = 45;
    int done = 0;
    int error = 0;
    uint16_t PMaxScheduleEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 45:
            // Grammar: ID=45; read/write bits=1; START (PMaxScheduleEntry)
            if (PMaxScheduleEntry_currentIndex < PMaxScheduleType->PMaxScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EntryType); next=46
                    error = encode_iso2_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 46;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; LOOP (PMaxScheduleEntry), END Element
            if (PMaxScheduleEntry_currentIndex < PMaxScheduleType->PMaxScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=46
                    error = encode_iso2_PMaxScheduleEntryType(stream, &PMaxScheduleType->PMaxScheduleEntry.array[PMaxScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 46;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Reference; type={http://www.w3.org/2000/09/xmldsig#}ReferenceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Type, anyURI (0, 1); URI, anyURI (0, 1); Transforms, TransformsType (0, 1); DigestMethod, DigestMethodType (1, 1); DigestValue, DigestValueType (1, 1);
static int encode_iso2_ReferenceType(exi_bitstream_t* stream, const struct iso2_ReferenceType* ReferenceType) {
    int grammar_id = 47;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 47:
            // Grammar: ID=47; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=48

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 48;
                        }
                    }
                }
            }
            else if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=49

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 49;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=50

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 50;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=51
                    error = encode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=52
                    error = encode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 52;
                    }
                }
            }
            break;
        case 48:
            // Grammar: ID=48; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=49

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 49;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=50

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 50;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=51
                    error = encode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=52
                    error = encode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 52;
                    }
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=50

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 50;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=51
                    error = encode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=52
                    error = encode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 52;
                    }
                }
            }
            break;
        case 50:
            // Grammar: ID=50; read/write bits=2; START (Transforms), START (DigestMethod)
            if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=51
                    error = encode_iso2_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 51;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=52
                    error = encode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 52;
                    }
                }
            }
            break;
        case 51:
            // Grammar: ID=51; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (DigestMethodType); next=52
                error = encode_iso2_DigestMethodType(stream, &ReferenceType->DigestMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 52;
                }
            }
            break;
        case 52:
            // Grammar: ID=52; read/write bits=1; START (DigestValue)
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
                        error = exi_basetypes_encoder_bytes(stream, ReferenceType->DigestValue.bytesLen, ReferenceType->DigestValue.bytes, iso2_DigestValueType_BYTES_SIZE);
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
static int encode_iso2_RetrievalMethodType(exi_bitstream_t* stream, const struct iso2_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 53;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 53:
            // Grammar: ID=53; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            if (RetrievalMethodType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=54

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso2_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 54;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=55

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 55;
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
                    error = encode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 54:
            // Grammar: ID=54; read/write bits=2; START (URI), START (Transforms), END Element
            if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=55

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso2_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 55;
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
                    error = encode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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
        case 55:
            // Grammar: ID=55; read/write bits=2; START (Transforms), END Element
            if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=3
                    error = encode_iso2_TransformsType(stream, &RetrievalMethodType->Transforms);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SalesTariff; type={urn:iso:15118:2:2013:MsgDataTypes}SalesTariffType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SalesTariffID, SAIDType (1, 1); SalesTariffDescription, tariffDescriptionType (0, 1); NumEPriceLevels, unsignedByte (0, 1); SalesTariffEntry, SalesTariffEntryType (1, 12) (original max 1024);
static int encode_iso2_SalesTariffType(exi_bitstream_t* stream, const struct iso2_SalesTariffType* SalesTariffType) {
    int grammar_id = 56;
    int done = 0;
    int error = 0;
    uint16_t SalesTariffEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 56:
            // Grammar: ID=56; read/write bits=2; START (Id), START (SalesTariffID)
            if (SalesTariffType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=57

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SalesTariffType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SalesTariffType->Id.charactersLen, SalesTariffType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 57;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SalesTariffID, unsignedByte); next=58
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)SalesTariffType->SalesTariffID - 1);
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
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=1; START (SalesTariffID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=58
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)SalesTariffType->SalesTariffID - 1);
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
            // Grammar: ID=58; read/write bits=2; START (SalesTariffDescription), START (NumEPriceLevels), START (SalesTariffEntry)
            if (SalesTariffType->SalesTariffDescription_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SalesTariffDescription, string); next=60

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SalesTariffType->SalesTariffDescription.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SalesTariffType->SalesTariffDescription.charactersLen, SalesTariffType->SalesTariffDescription.characters, iso2_SalesTariffDescription_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 60;
                                }
                            }
                        }
                    }
                }
            }
            else if (SalesTariffType->NumEPriceLevels_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (NumEPriceLevels, unsignedShort); next=62
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
                                grammar_id = 62;
                            }
                        }
                    }
                }
            }
            else
            {
                if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (EntryType); next=59
                        error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 59;
                        }
                    }
                }
            }
            break;
        case 59:
            // Grammar: ID=59; read/write bits=2; LOOP (SalesTariffEntry), END Element
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=59
                    error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 59;
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
        case 60:
            // Grammar: ID=60; read/write bits=2; START (NumEPriceLevels), START (SalesTariffEntry)
            if (SalesTariffType->NumEPriceLevels_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (NumEPriceLevels, unsignedShort); next=62
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
                                grammar_id = 62;
                            }
                        }
                    }
                }
            }
            else
            {
                if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (EntryType); next=61
                        error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 61;
                        }
                    }
                }
            }
            break;
        case 61:
            // Grammar: ID=61; read/write bits=2; LOOP (SalesTariffEntry), END Element
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=61
                    error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 61;
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
        case 62:
            // Grammar: ID=62; read/write bits=1; START (SalesTariffEntry)
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EntryType); next=63
                    error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 63;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 63:
            // Grammar: ID=63; read/write bits=2; LOOP (SalesTariffEntry), END Element
            if (SalesTariffEntry_currentIndex < SalesTariffType->SalesTariffEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EntryType); next=63
                    error = encode_iso2_SalesTariffEntryType(stream, &SalesTariffType->SalesTariffEntry.array[SalesTariffEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 63;
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
static int encode_iso2_X509DataType(exi_bitstream_t* stream, const struct iso2_X509DataType* X509DataType) {
    int grammar_id = 64;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 64:
            // Grammar: ID=64; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            if (X509DataType->X509IssuerSerial_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerial, X509IssuerSerialType); next=3
                    error = encode_iso2_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
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
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509SKI.bytesLen, X509DataType->X509SKI.bytes, iso2_base64Binary_BYTES_SIZE);
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
                            error = exi_basetypes_encoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso2_X509SubjectName_CHARACTER_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509Certificate.bytesLen, X509DataType->X509Certificate.bytes, iso2_base64Binary_BYTES_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509CRL.bytesLen, X509DataType->X509CRL.bytes, iso2_base64Binary_BYTES_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->ANY.bytesLen, X509DataType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
static int encode_iso2_PGPDataType(exi_bitstream_t* stream, const struct iso2_PGPDataType* PGPDataType) {
    int grammar_id = 65;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 65:
            // Grammar: ID=65; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyID, base64Binary); next=66
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyID.bytesLen, PGPDataType->choice_1.PGPKeyID.bytes, iso2_base64Binary_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=67
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso2_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 67;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=67
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso2_base64Binary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 67;
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
                    // Event: START (ANY, base64Binary); next=68
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso2_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 68;
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
        case 67:
            // Grammar: ID=67; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=68
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso2_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 68;
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
        case 68:
            // Grammar: ID=68; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=69
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.PGPKeyPacket.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.PGPKeyPacket.bytesLen, PGPDataType->choice_2.PGPKeyPacket.bytes, iso2_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 69;
                            }
                        }
                    }
                }
            }
            break;
        case 69:
            // Grammar: ID=69; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=68
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.ANY.bytesLen, PGPDataType->choice_2.ANY.bytes, iso2_anyType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 68;
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
static int encode_iso2_SPKIDataType(exi_bitstream_t* stream, const struct iso2_SPKIDataType* SPKIDataType) {
    int grammar_id = 70;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 70:
            // Grammar: ID=70; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=71
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->SPKISexp.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SPKIDataType->SPKISexp.bytesLen, SPKIDataType->SPKISexp.bytes, iso2_base64Binary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 71;
                            }
                        }
                    }
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=2; START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_bytes(stream, SPKIDataType->ANY.bytesLen, SPKIDataType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4) (original max unbounded);
static int encode_iso2_SignedInfoType(exi_bitstream_t* stream, const struct iso2_SignedInfoType* SignedInfoType) {
    int grammar_id = 72;
    int done = 0;
    int error = 0;
    uint16_t Reference_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 72:
            // Grammar: ID=72; read/write bits=2; START (Id), START (CanonicalizationMethod)
            if (SignedInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=73

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 73;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType); next=74
                    error = encode_iso2_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 74;
                    }
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CanonicalizationMethodType); next=74
                error = encode_iso2_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 74;
                }
            }
            break;
        case 74:
            // Grammar: ID=74; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignatureMethodType); next=75
                error = encode_iso2_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 75;
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; START (Reference)
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=76
                    error = encode_iso2_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 76;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=2; LOOP (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ReferenceType); next=76
                    error = encode_iso2_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 76;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntry; type={urn:iso:15118:2:2013:MsgDataTypes}ProfileEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ChargingProfileEntryStart, unsignedInt (1, 1); ChargingProfileEntryMaxPower, PhysicalValueType (1, 1); ChargingProfileEntryMaxNumberOfPhasesInUse, maxNumPhasesType (0, 1);
static int encode_iso2_ProfileEntryType(exi_bitstream_t* stream, const struct iso2_ProfileEntryType* ProfileEntryType) {
    int grammar_id = 77;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=1; START (ChargingProfileEntryStart)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=78
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
                            grammar_id = 78;
                        }
                    }
                }
            }
            break;
        case 78:
            // Grammar: ID=78; read/write bits=1; START (ChargingProfileEntryMaxPower)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=79
                error = encode_iso2_PhysicalValueType(stream, &ProfileEntryType->ChargingProfileEntryMaxPower);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 79;
                }
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=2; START (ChargingProfileEntryMaxNumberOfPhasesInUse), END Element
            if (ProfileEntryType->ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingProfileEntryMaxNumberOfPhasesInUse, byte); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, (uint32_t)ProfileEntryType->ChargingProfileEntryMaxNumberOfPhasesInUse - 1);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVStatusType; base type=EVStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVReady, boolean (1, 1); EVErrorCode, DC_EVErrorCodeType (1, 1); EVRESSSOC, percentValueType (1, 1);
static int encode_iso2_DC_EVStatusType(exi_bitstream_t* stream, const struct iso2_DC_EVStatusType* DC_EVStatusType) {
    int grammar_id = 80;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 80:
            // Grammar: ID=80; read/write bits=1; START (EVReady)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=81
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
                            grammar_id = 81;
                        }
                    }
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=1; START (EVErrorCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=82
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
                            grammar_id = 82;
                        }
                    }
                }
            }
            break;
        case 82:
            // Grammar: ID=82; read/write bits=1; START (EVRESSSOC)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}ParameterSet; type={urn:iso:15118:2:2013:MsgDataTypes}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, short (1, 1); Parameter, ParameterType (1, 16);
static int encode_iso2_ParameterSetType(exi_bitstream_t* stream, const struct iso2_ParameterSetType* ParameterSetType) {
    int grammar_id = 83;
    int done = 0;
    int error = 0;
    uint16_t Parameter_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 83:
            // Grammar: ID=83; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=84
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
                            grammar_id = 84;
                        }
                    }
                }
            }
            break;
        case 84:
            // Grammar: ID=84; read/write bits=1; START (Parameter)
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=85
                    error = encode_iso2_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 85;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=2; LOOP (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ParameterType); next=85
                    error = encode_iso2_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 85;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTuple; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SAScheduleTupleID, SAIDType (1, 1); PMaxSchedule, PMaxScheduleType (1, 1); SalesTariff, SalesTariffType (0, 1);
static int encode_iso2_SAScheduleTupleType(exi_bitstream_t* stream, const struct iso2_SAScheduleTupleType* SAScheduleTupleType) {
    int grammar_id = 86;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 86:
            // Grammar: ID=86; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=87
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)SAScheduleTupleType->SAScheduleTupleID - 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 87;
                        }
                    }
                }
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=1; START (PMaxSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PMaxScheduleType); next=88
                error = encode_iso2_PMaxScheduleType(stream, &SAScheduleTupleType->PMaxSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 88;
                }
            }
            break;
        case 88:
            // Grammar: ID=88; read/write bits=2; START (SalesTariff), END Element
            if (SAScheduleTupleType->SalesTariff_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SalesTariff, SalesTariffType); next=3
                    error = encode_iso2_SalesTariffType(stream, &SAScheduleTupleType->SalesTariff);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SelectedService; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, short (0, 1);
static int encode_iso2_SelectedServiceType(exi_bitstream_t* stream, const struct iso2_SelectedServiceType* SelectedServiceType) {
    int grammar_id = 89;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 89:
            // Grammar: ID=89; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=90
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
                            grammar_id = 90;
                        }
                    }
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=2; START (ParameterSetID), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}Service; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1);
static int encode_iso2_ServiceType(exi_bitstream_t* stream, const struct iso2_ServiceType* ServiceType) {
    int grammar_id = 91;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 91:
            // Grammar: ID=91; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=92
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ServiceType->ServiceID);
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
            break;
        case 92:
            // Grammar: ID=92; read/write bits=2; START (ServiceName), START (ServiceCategory)
            if (ServiceType->ServiceName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceName, string); next=93

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceType->ServiceName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceType->ServiceName.charactersLen, ServiceType->ServiceName.characters, iso2_ServiceName_CHARACTER_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceCategory, string); next=94
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceType->ServiceCategory);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 94;
                            }
                        }
                    }
                }
            }
            break;
        case 93:
            // Grammar: ID=93; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=94
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, ServiceType->ServiceCategory);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 94;
                        }
                    }
                }
            }
            break;
        case 94:
            // Grammar: ID=94; read/write bits=2; START (ServiceScope), START (FreeService)
            if (ServiceType->ServiceScope_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceScope, string); next=95

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceType->ServiceScope.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceType->ServiceScope.charactersLen, ServiceType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 95;
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
                    // Event: START (FreeService, boolean); next=3
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
            }
            break;
        case 95:
            // Grammar: ID=95; read/write bits=1; START (FreeService)
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureValue; type={http://www.w3.org/2000/09/xmldsig#}SignatureValueType; base type=base64Binary; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); CONTENT, SignatureValueType (1, 1);
static int encode_iso2_SignatureValueType(exi_bitstream_t* stream, const struct iso2_SignatureValueType* SignatureValueType) {
    int grammar_id = 96;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 96:
            // Grammar: ID=96; read/write bits=2; START (Id), START (CONTENT)
            if (SignatureValueType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=97

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureValueType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 97;
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
                        error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso2_SignatureValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 3;
                        }
                    }
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso2_SignatureValueType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SubCertificates; type={urn:iso:15118:2:2013:MsgDataTypes}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 4);
static int encode_iso2_SubCertificatesType(exi_bitstream_t* stream, const struct iso2_SubCertificatesType* SubCertificatesType) {
    int grammar_id = 98;
    int done = 0;
    int error = 0;
    uint16_t Certificate_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 98:
            // Grammar: ID=98; read/write bits=1; START (Certificate)
            if (Certificate_currentIndex < SubCertificatesType->Certificate.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (base64Binary); next=99
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytes, iso2_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                Certificate_currentIndex++;
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
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=2; LOOP (Certificate), END Element
            if (Certificate_currentIndex < SubCertificatesType->Certificate.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (base64Binary); next=99
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytes, iso2_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                Certificate_currentIndex++;
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}KeyInfo; type={http://www.w3.org/2000/09/xmldsig#}KeyInfoType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); KeyName, string (0, 1); KeyValue, KeyValueType (0, 1); RetrievalMethod, RetrievalMethodType (0, 1); X509Data, X509DataType (0, 1); PGPData, PGPDataType (0, 1); SPKIData, SPKIDataType (0, 1); MgmtData, string (0, 1); ANY, anyType (0, 1);
static int encode_iso2_KeyInfoType(exi_bitstream_t* stream, const struct iso2_KeyInfoType* KeyInfoType) {
    int grammar_id = 100;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 100:
            // Grammar: ID=100; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=101

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 101;
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
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso2_KeyName_CHARACTER_SIZE);
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
                    error = encode_iso2_KeyValueType(stream, &KeyInfoType->KeyValue);
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
                    error = encode_iso2_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
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
                    error = encode_iso2_X509DataType(stream, &KeyInfoType->X509Data);
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
                    error = encode_iso2_PGPDataType(stream, &KeyInfoType->PGPData);
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
                    error = encode_iso2_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso2_MgmtData_CHARACTER_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
        case 101:
            // Grammar: ID=101; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
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
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso2_KeyName_CHARACTER_SIZE);
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
                    error = encode_iso2_KeyValueType(stream, &KeyInfoType->KeyValue);
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
                    error = encode_iso2_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
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
                    error = encode_iso2_X509DataType(stream, &KeyInfoType->X509Data);
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
                    error = encode_iso2_PGPDataType(stream, &KeyInfoType->PGPData);
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
                    error = encode_iso2_SPKIDataType(stream, &KeyInfoType->SPKIData);
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
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso2_MgmtData_CHARACTER_SIZE);
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
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Object; type={http://www.w3.org/2000/09/xmldsig#}ObjectType; base type=; content type=mixed;
//          abstract=False; final=False;
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1) (old 1, 1);
static int encode_iso2_ObjectType(exi_bitstream_t* stream, const struct iso2_ObjectType* ObjectType) {
    int grammar_id = 102;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Encoding_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Encoding, anyURI); next=103

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Encoding.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso2_Encoding_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 103;
                        }
                    }
                }
            }
            else if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=104

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 104;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=105

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 105;
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
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
        case 103:
            // Grammar: ID=103; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=104

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 104;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=105

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 105;
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
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
        case 104:
            // Grammar: ID=104; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=105

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso2_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 105;
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
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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
        case 105:
            // Grammar: ID=105; read/write bits=2; START (ANY), END Element, START (ANY)
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
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso2_anyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferMode; type={urn:iso:15118:2:2013:MsgDataTypes}SupportedEnergyTransferModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyTransferMode, EnergyTransferModeType (1, 6);
static int encode_iso2_SupportedEnergyTransferModeType(exi_bitstream_t* stream, const struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType) {
    int grammar_id = 106;
    int done = 0;
    int error = 0;
    uint16_t EnergyTransferMode_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 106:
            // Grammar: ID=106; read/write bits=1; START (EnergyTransferMode)
            if (EnergyTransferMode_currentIndex < SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=107
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, SupportedEnergyTransferModeType->EnergyTransferMode.array[EnergyTransferMode_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 107;
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
        case 107:
            // Grammar: ID=107; read/write bits=2; LOOP (EnergyTransferMode), END Element
            if (EnergyTransferMode_currentIndex < SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=107
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, SupportedEnergyTransferModeType->EnergyTransferMode.array[EnergyTransferMode_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 107;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureCertChain; type={urn:iso:15118:2:2013:MsgDataTypes}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
static int encode_iso2_CertificateChainType(exi_bitstream_t* stream, const struct iso2_CertificateChainType* CertificateChainType) {
    int grammar_id = 108;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 108:
            // Grammar: ID=108; read/write bits=2; START (Id), START (Certificate)
            if (CertificateChainType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=109

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateChainType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateChainType->Id.charactersLen, CertificateChainType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 109;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Certificate, base64Binary); next=110
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateChainType->Certificate.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, CertificateChainType->Certificate.bytesLen, CertificateChainType->Certificate.bytes, iso2_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 110;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=110
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateChainType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateChainType->Certificate.bytesLen, CertificateChainType->Certificate.bytes, iso2_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 110;
                            }
                        }
                    }
                }
            }
            break;
        case 110:
            // Grammar: ID=110; read/write bits=2; START (SubCertificates), END Element
            if (CertificateChainType->SubCertificates_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SubCertificates, SubCertificatesType); next=3
                    error = encode_iso2_SubCertificatesType(stream, &CertificateChainType->SubCertificates);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}BodyElement; type={urn:iso:15118:2:2013:MsgBody}BodyBaseType; base type=; content type=empty;
//          abstract=True; final=False;
static int encode_iso2_BodyBaseType(exi_bitstream_t* stream, const struct iso2_BodyBaseType* BodyBaseType) {
    // Element has no particles, so the function just encodes END Element
    (void)BodyBaseType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgHeader}Notification; type={urn:iso:15118:2:2013:MsgDataTypes}NotificationType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: FaultCode, faultCodeType (1, 1); FaultMsg, faultMsgType (0, 1);
static int encode_iso2_NotificationType(exi_bitstream_t* stream, const struct iso2_NotificationType* NotificationType) {
    int grammar_id = 111;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 111:
            // Grammar: ID=111; read/write bits=1; START (FaultCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=112
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
                            grammar_id = 112;
                        }
                    }
                }
            }
            break;
        case 112:
            // Grammar: ID=112; read/write bits=2; START (FaultMsg), END Element
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
                            error = exi_basetypes_encoder_characters(stream, NotificationType->FaultMsg.charactersLen, NotificationType->FaultMsg.characters, iso2_FaultMsg_CHARACTER_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); EVSEIsolationStatus, isolationLevelType (0, 1); EVSEStatusCode, DC_EVSEStatusCodeType (1, 1);
static int encode_iso2_DC_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_DC_EVSEStatusType* DC_EVSEStatusType) {
    int grammar_id = 113;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 113:
            // Grammar: ID=113; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=114
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, DC_EVSEStatusType->NotificationMaxDelay);
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
            break;
        case 114:
            // Grammar: ID=114; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=115
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
                            grammar_id = 115;
                        }
                    }
                }
            }
            break;
        case 115:
            // Grammar: ID=115; read/write bits=2; START (EVSEIsolationStatus), START (EVSEStatusCode)
            if (DC_EVSEStatusType->EVSEIsolationStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEIsolationStatus, string); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, DC_EVSEStatusType->EVSEIsolationStatus);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 116;
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
                    // Event: START (EVSEStatusCode, string); next=3
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
                                grammar_id = 3;
                            }
                        }
                    }
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=1; START (EVSEStatusCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SelectedServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16);
static int encode_iso2_SelectedServiceListType(exi_bitstream_t* stream, const struct iso2_SelectedServiceListType* SelectedServiceListType) {
    int grammar_id = 117;
    int done = 0;
    int error = 0;
    uint16_t SelectedService_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 117:
            // Grammar: ID=117; read/write bits=1; START (SelectedService)
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=118
                    error = encode_iso2_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 118;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=2; LOOP (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (SelectedServiceType); next=118
                    error = encode_iso2_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 118;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentOptionList; type={urn:iso:15118:2:2013:MsgDataTypes}PaymentOptionListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PaymentOption, paymentOptionType (1, 2);
static int encode_iso2_PaymentOptionListType(exi_bitstream_t* stream, const struct iso2_PaymentOptionListType* PaymentOptionListType) {
    int grammar_id = 119;
    int done = 0;
    int error = 0;
    uint16_t PaymentOption_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 119:
            // Grammar: ID=119; read/write bits=1; START (PaymentOption)
            if (PaymentOption_currentIndex < PaymentOptionListType->PaymentOption.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=120
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PaymentOptionListType->PaymentOption.array[PaymentOption_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 120;
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
        case 120:
            // Grammar: ID=120; read/write bits=2; START (PaymentOption), END Element
            if (PaymentOption_currentIndex < PaymentOptionListType->PaymentOption.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PaymentOptionListType->PaymentOption.array[PaymentOption_currentIndex++]);
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

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Signature; type={http://www.w3.org/2000/09/xmldsig#}SignatureType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1) (original max unbounded);
static int encode_iso2_SignatureType(exi_bitstream_t* stream, const struct iso2_SignatureType* SignatureType) {
    int grammar_id = 121;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 121:
            // Grammar: ID=121; read/write bits=2; START (Id), START (SignedInfo)
            if (SignatureType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=122

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 122;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignedInfo, SignedInfoType); next=123
                    error = encode_iso2_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 123;
                    }
                }
            }
            break;
        case 122:
            // Grammar: ID=122; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedInfoType); next=123
                error = encode_iso2_SignedInfoType(stream, &SignatureType->SignedInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 123;
                }
            }
            break;
        case 123:
            // Grammar: ID=123; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=124
                error = encode_iso2_SignatureValueType(stream, &SignatureType->SignatureValue);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 124;
                }
            }
            break;
        case 124:
            // Grammar: ID=124; read/write bits=2; START (KeyInfo), START (Object), END Element
            if (SignatureType->KeyInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyInfo, KeyInfoType); next=126
                    error = encode_iso2_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 126;
                    }
                }
            }
            else if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=125
                    error = encode_iso2_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 125;
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
        case 125:
            // Grammar: ID=125; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=3
                    error = encode_iso2_ObjectType(stream, &SignatureType->Object);
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
        case 126:
            // Grammar: ID=126; read/write bits=2; START (Object), END Element
            if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=127
                    error = encode_iso2_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 127;
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
        case 127:
            // Grammar: ID=127; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=3
                    error = encode_iso2_ObjectType(stream, &SignatureType->Object);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingProfile; type={urn:iso:15118:2:2013:MsgDataTypes}ChargingProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProfileEntry, ProfileEntryType (1, 24);
static int encode_iso2_ChargingProfileType(exi_bitstream_t* stream, const struct iso2_ChargingProfileType* ChargingProfileType) {
    int grammar_id = 128;
    int done = 0;
    int error = 0;
    uint16_t ProfileEntry_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 128:
            // Grammar: ID=128; read/write bits=1; START (ProfileEntry)
            if (ProfileEntry_currentIndex < ChargingProfileType->ProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ProfileEntryType); next=129
                    error = encode_iso2_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ProfileEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 129;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=2; LOOP (ProfileEntry), END Element
            if (ProfileEntry_currentIndex < ChargingProfileType->ProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ProfileEntryType); next=129
                    error = encode_iso2_ProfileEntryType(stream, &ChargingProfileType->ProfileEntry.array[ProfileEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 129;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceParameterList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5) (original max 255);
static int encode_iso2_ServiceParameterListType(exi_bitstream_t* stream, const struct iso2_ServiceParameterListType* ServiceParameterListType) {
    int grammar_id = 130;
    int done = 0;
    int error = 0;
    uint16_t ParameterSet_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 130:
            // Grammar: ID=130; read/write bits=1; START (ParameterSet)
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=131
                    error = encode_iso2_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 131;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 131:
            // Grammar: ID=131; read/write bits=2; LOOP (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ParameterSetType); next=131
                    error = encode_iso2_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 131;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ListOfRootCertificateIDs; type={urn:iso:15118:2:2013:MsgDataTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, X509IssuerSerialType (1, 5) (original max 20);
static int encode_iso2_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    int grammar_id = 132;
    int done = 0;
    int error = 0;
    uint16_t RootCertificateID_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 132:
            // Grammar: ID=132; read/write bits=1; START (RootCertificateID)
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=133
                    error = encode_iso2_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 133;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 133:
            // Grammar: ID=133; read/write bits=2; LOOP (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (X509IssuerSerialType); next=133
                    error = encode_iso2_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 133;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); EAmount, PhysicalValueType (1, 1); EVMaxVoltage, PhysicalValueType (1, 1); EVMaxCurrent, PhysicalValueType (1, 1); EVMinCurrent, PhysicalValueType (1, 1);
static int encode_iso2_AC_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    int grammar_id = 134;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 134:
            // Grammar: ID=134; read/write bits=2; START (DepartureTime), START (EAmount)
            if (AC_EVChargeParameterType->DepartureTime_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DepartureTime, unsignedLong); next=135
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
                                grammar_id = 135;
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
                    // Event: START (EAmount, PhysicalValueType); next=136
                    error = encode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 136;
                    }
                }
            }
            break;
        case 135:
            // Grammar: ID=135; read/write bits=1; START (EAmount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=136
                error = encode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EAmount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 136;
                }
            }
            break;
        case 136:
            // Grammar: ID=136; read/write bits=1; START (EVMaxVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=137
                error = encode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 137;
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=1; START (EVMaxCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=138
                error = encode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMaxCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 138;
                }
            }
            break;
        case 138:
            // Grammar: ID=138; read/write bits=1; START (EVMinCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &AC_EVChargeParameterType->EVMinCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVChargeParameterType; base type=EVChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DepartureTime, unsignedInt (0, 1); DC_EVStatus, DC_EVStatusType (1, 1); EVMaximumCurrentLimit, PhysicalValueType (1, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); EVMaximumVoltageLimit, PhysicalValueType (1, 1); EVEnergyCapacity, PhysicalValueType (0, 1); EVEnergyRequest, PhysicalValueType (0, 1); FullSOC, percentValueType (0, 1); BulkSOC, percentValueType (0, 1);
static int encode_iso2_DC_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    int grammar_id = 139;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 139:
            // Grammar: ID=139; read/write bits=2; START (DepartureTime), START (DC_EVStatus)
            if (DC_EVChargeParameterType->DepartureTime_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DepartureTime, unsignedLong); next=140
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, DC_EVChargeParameterType->DepartureTime);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 140;
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
                    // Event: START (DC_EVStatus, EVStatusType); next=141
                    error = encode_iso2_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 141;
                    }
                }
            }
            break;
        case 140:
            // Grammar: ID=140; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=141
                error = encode_iso2_DC_EVStatusType(stream, &DC_EVChargeParameterType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 141;
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=1; START (EVMaximumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=142
                error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 142;
                }
            }
            break;
        case 142:
            // Grammar: ID=142; read/write bits=2; START (EVMaximumPowerLimit), START (EVMaximumVoltageLimit)
            if (DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=143
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 143;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType); next=144
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 144;
                    }
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=1; START (EVMaximumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=144
                error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVMaximumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 144;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=3; START (EVEnergyCapacity), START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->EVEnergyCapacity_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyCapacity, PhysicalValueType); next=145
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyCapacity);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 145;
                    }
                }
            }
            else if (DC_EVChargeParameterType->EVEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyRequest, PhysicalValueType); next=146
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 146;
                    }
                }
            }
            else if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=147
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
                                grammar_id = 147;
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
        case 145:
            // Grammar: ID=145; read/write bits=3; START (EVEnergyRequest), START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->EVEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyRequest, PhysicalValueType); next=146
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVChargeParameterType->EVEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 146;
                    }
                }
            }
            else if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=147
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
                                grammar_id = 147;
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
        case 146:
            // Grammar: ID=146; read/write bits=2; START (FullSOC), START (BulkSOC), END Element
            if (DC_EVChargeParameterType->FullSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (FullSOC, byte); next=147
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
                                grammar_id = 147;
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
        case 147:
            // Grammar: ID=147; read/write bits=2; START (BulkSOC), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVChargeParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); AC_EVChargeParameter, AC_EVChargeParameterType (1, 1); DC_EVChargeParameter, DC_EVChargeParameterType (1, 1);
static int encode_iso2_EVChargeParameterType(exi_bitstream_t* stream, const struct iso2_EVChargeParameterType* EVChargeParameterType) {
    int grammar_id = 148;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 148:
            // Grammar: ID=148; read/write bits=2; START (DepartureTime), START (AC_EVChargeParameter)
            if (EVChargeParameterType->DepartureTime_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DepartureTime, unsignedLong); next=149
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, EVChargeParameterType->DepartureTime);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 149;
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
                    // Event: START (AC_EVChargeParameter, EVChargeParameterType); next=150
                    error = encode_iso2_AC_EVChargeParameterType(stream, &EVChargeParameterType->AC_EVChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 150;
                    }
                }
            }
            break;
        case 149:
            // Grammar: ID=149; read/write bits=1; START (AC_EVChargeParameter)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVChargeParameterType); next=150
                error = encode_iso2_AC_EVChargeParameterType(stream, &EVChargeParameterType->AC_EVChargeParameter);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 150;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=1; START (DC_EVChargeParameter)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVChargeParameterType); next=3
                error = encode_iso2_DC_EVChargeParameterType(stream, &EVChargeParameterType->DC_EVChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SASchedules; type={urn:iso:15118:2:2013:MsgDataTypes}SASchedulesType; base type=; content type=empty;
//          abstract=True; final=False;
static int encode_iso2_SASchedulesType(exi_bitstream_t* stream, const struct iso2_SASchedulesType* SASchedulesType) {
    // Element has no particles, so the function just encodes END Element
    (void)SASchedulesType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleList; type={urn:iso:15118:2:2013:MsgDataTypes}SAScheduleListType; base type=SASchedulesType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SAScheduleTuple, SAScheduleTupleType (1, 3);
static int encode_iso2_SAScheduleListType(exi_bitstream_t* stream, const struct iso2_SAScheduleListType* SAScheduleListType) {
    int grammar_id = 151;
    int done = 0;
    int error = 0;
    uint16_t SAScheduleTuple_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 151:
            // Grammar: ID=151; read/write bits=1; START (SAScheduleTuple)
            if (SAScheduleTuple_currentIndex < SAScheduleListType->SAScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleTupleType); next=152
                    error = encode_iso2_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 152;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 152:
            // Grammar: ID=152; read/write bits=2; LOOP (SAScheduleTuple), END Element
            if (SAScheduleTuple_currentIndex < SAScheduleListType->SAScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (SAScheduleTupleType); next=152
                    error = encode_iso2_SAScheduleTupleType(stream, &SAScheduleListType->SAScheduleTuple.array[SAScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 152;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeService; type={urn:iso:15118:2:2013:MsgDataTypes}ChargeServiceType; base type=ServiceType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1); ServiceName, serviceNameType (0, 1); ServiceCategory, serviceCategoryType (1, 1); ServiceScope, serviceScopeType (0, 1); FreeService, boolean (1, 1); SupportedEnergyTransferMode, SupportedEnergyTransferModeType (1, 1);
static int encode_iso2_ChargeServiceType(exi_bitstream_t* stream, const struct iso2_ChargeServiceType* ChargeServiceType) {
    int grammar_id = 153;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 153:
            // Grammar: ID=153; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=154
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ChargeServiceType->ServiceID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 154;
                        }
                    }
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=2; START (ServiceName), START (ServiceCategory)
            if (ChargeServiceType->ServiceName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceName, string); next=155

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ChargeServiceType->ServiceName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ChargeServiceType->ServiceName.charactersLen, ChargeServiceType->ServiceName.characters, iso2_ServiceName_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 155;
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
                    // Event: START (ServiceCategory, string); next=156
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 2, ChargeServiceType->ServiceCategory);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 156;
                            }
                        }
                    }
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=1; START (ServiceCategory)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=156
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, ChargeServiceType->ServiceCategory);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 156;
                        }
                    }
                }
            }
            break;
        case 156:
            // Grammar: ID=156; read/write bits=2; START (ServiceScope), START (FreeService)
            if (ChargeServiceType->ServiceScope_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceScope, string); next=157

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ChargeServiceType->ServiceScope.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ChargeServiceType->ServiceScope.charactersLen, ChargeServiceType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 157;
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
                    // Event: START (FreeService, boolean); next=158
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, ChargeServiceType->FreeService);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 158;
                            }
                        }
                    }
                }
            }
            break;
        case 157:
            // Grammar: ID=157; read/write bits=1; START (FreeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=158
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, ChargeServiceType->FreeService);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 158;
                        }
                    }
                }
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=1; START (SupportedEnergyTransferMode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SupportedEnergyTransferModeType); next=3
                error = encode_iso2_SupportedEnergyTransferModeType(stream, &ChargeServiceType->SupportedEnergyTransferMode);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVPowerDeliveryParameterType; base type=; content type=empty;
//          abstract=True; final=False;
static int encode_iso2_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVPowerDeliveryParameterType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVPowerDeliveryParameterType; base type=EVPowerDeliveryParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1);
static int encode_iso2_DC_EVPowerDeliveryParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    int grammar_id = 159;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 159:
            // Grammar: ID=159; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=160
                error = encode_iso2_DC_EVStatusType(stream, &DC_EVPowerDeliveryParameterType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 160;
                }
            }
            break;
        case 160:
            // Grammar: ID=160; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            if (DC_EVPowerDeliveryParameterType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=161
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
                                grammar_id = 161;
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
        case 161:
            // Grammar: ID=161; read/write bits=1; START (ChargingComplete)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ContractSignatureEncryptedPrivateKey; type={urn:iso:15118:2:2013:MsgDataTypes}ContractSignatureEncryptedPrivateKeyType; base type=privateKeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, ContractSignatureEncryptedPrivateKeyType (1, 1);
static int encode_iso2_ContractSignatureEncryptedPrivateKeyType(exi_bitstream_t* stream, const struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType) {
    int grammar_id = 162;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 162:
            // Grammar: ID=162; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=163

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ContractSignatureEncryptedPrivateKeyType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, ContractSignatureEncryptedPrivateKeyType->Id.charactersLen, ContractSignatureEncryptedPrivateKeyType->Id.characters, iso2_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 163;
                    }
                }
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ContractSignatureEncryptedPrivateKeyType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, ContractSignatureEncryptedPrivateKeyType->CONTENT.bytesLen, ContractSignatureEncryptedPrivateKeyType->CONTENT.bytes, iso2_ContractSignatureEncryptedPrivateKeyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEChargeParameterType; base type=; content type=empty;
//          abstract=True; final=False;
static int encode_iso2_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_EVSEChargeParameterType* EVSEChargeParameterType) {
    // Element has no particles, so the function just encodes END Element
    (void)EVSEChargeParameterType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}DC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEMaximumCurrentLimit, PhysicalValueType (1, 1); EVSEMaximumPowerLimit, PhysicalValueType (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (1, 1); EVSEMinimumCurrentLimit, PhysicalValueType (1, 1); EVSEMinimumVoltageLimit, PhysicalValueType (1, 1); EVSECurrentRegulationTolerance, PhysicalValueType (0, 1); EVSEPeakCurrentRipple, PhysicalValueType (1, 1); EVSEEnergyToBeDelivered, PhysicalValueType (0, 1);
static int encode_iso2_DC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    int grammar_id = 164;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 164:
            // Grammar: ID=164; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=165
                error = encode_iso2_DC_EVSEStatusType(stream, &DC_EVSEChargeParameterType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 165;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=1; START (EVSEMaximumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=166
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 166;
                }
            }
            break;
        case 166:
            // Grammar: ID=166; read/write bits=1; START (EVSEMaximumPowerLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=167
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumPowerLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 167;
                }
            }
            break;
        case 167:
            // Grammar: ID=167; read/write bits=1; START (EVSEMaximumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=168
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMaximumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 168;
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=1; START (EVSEMinimumCurrentLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=169
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumCurrentLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 169;
                }
            }
            break;
        case 169:
            // Grammar: ID=169; read/write bits=1; START (EVSEMinimumVoltageLimit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=170
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEMinimumVoltageLimit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 170;
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=2; START (EVSECurrentRegulationTolerance), START (EVSEPeakCurrentRipple)
            if (DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSECurrentRegulationTolerance, PhysicalValueType); next=171
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 171;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEPeakCurrentRipple, PhysicalValueType); next=172
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 172;
                    }
                }
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=1; START (EVSEPeakCurrentRipple)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=172
                error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEPeakCurrentRipple);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 172;
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=2; START (EVSEEnergyToBeDelivered), END Element
            if (DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEEnergyToBeDelivered, PhysicalValueType); next=3
                    error = encode_iso2_PhysicalValueType(stream, &DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceList; type={urn:iso:15118:2:2013:MsgDataTypes}ServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 8);
static int encode_iso2_ServiceListType(exi_bitstream_t* stream, const struct iso2_ServiceListType* ServiceListType) {
    int grammar_id = 173;
    int done = 0;
    int error = 0;
    uint16_t Service_currentIndex = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 173:
            // Grammar: ID=173; read/write bits=1; START (Service)
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=174
                    error = encode_iso2_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 174;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=2; LOOP (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (ServiceType); next=174
                    error = encode_iso2_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 174;
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}DHpublickey; type={urn:iso:15118:2:2013:MsgDataTypes}DiffieHellmanPublickeyType; base type=dHpublickeyType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, DiffieHellmanPublickeyType (1, 1);
static int encode_iso2_DiffieHellmanPublickeyType(exi_bitstream_t* stream, const struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType) {
    int grammar_id = 175;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 175:
            // Grammar: ID=175; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=176

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(DiffieHellmanPublickeyType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, DiffieHellmanPublickeyType->Id.charactersLen, DiffieHellmanPublickeyType->Id.characters, iso2_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 176;
                    }
                }
            }
            break;
        case 176:
            // Grammar: ID=176; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=3
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DiffieHellmanPublickeyType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, DiffieHellmanPublickeyType->CONTENT.bytesLen, DiffieHellmanPublickeyType->CONTENT.bytes, iso2_DiffieHellmanPublickeyType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}eMAID; type={urn:iso:15118:2:2013:MsgDataTypes}EMAIDType; base type=eMAIDType; content type=simple;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); CONTENT, EMAIDType (1, 1);
static int encode_iso2_EMAIDType(exi_bitstream_t* stream, const struct iso2_EMAIDType* EMAIDType) {
    int grammar_id = 177;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 177:
            // Grammar: ID=177; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=178

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, EMAIDType->Id.charactersLen, EMAIDType->Id.characters, iso2_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 178;
                    }
                }
            }
            break;
        case 178:
            // Grammar: ID=178; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDType->CONTENT.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, EMAIDType->CONTENT.charactersLen, EMAIDType->CONTENT.characters, iso2_CONTENT_CHARACTER_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AC_EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEStatusType; base type=EVSEStatusType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); RCD, boolean (1, 1);
static int encode_iso2_AC_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_AC_EVSEStatusType* AC_EVSEStatusType) {
    int grammar_id = 179;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 179:
            // Grammar: ID=179; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=180
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, AC_EVSEStatusType->NotificationMaxDelay);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 180;
                        }
                    }
                }
            }
            break;
        case 180:
            // Grammar: ID=180; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=181
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
                            grammar_id = 181;
                        }
                    }
                }
            }
            break;
        case 181:
            // Grammar: ID=181; read/write bits=1; START (RCD)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=3
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatus; type={urn:iso:15118:2:2013:MsgDataTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=True; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, EVSENotificationType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1);
static int encode_iso2_EVSEStatusType(exi_bitstream_t* stream, const struct iso2_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 182;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 182:
            // Grammar: ID=182; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=183
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, EVSEStatusType->NotificationMaxDelay);
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
            // Grammar: ID=183; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=184
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, EVSEStatusType->EVSENotification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 184;
                        }
                    }
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=185
                error = encode_iso2_AC_EVSEStatusType(stream, &EVSEStatusType->AC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 185;
                }
            }
            break;
        case 185:
            // Grammar: ID=185; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=3
                error = encode_iso2_DC_EVSEStatusType(stream, &EVSEStatusType->DC_EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameter; type={urn:iso:15118:2:2013:MsgDataTypes}AC_EVSEChargeParameterType; base type=EVSEChargeParameterType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: AC_EVSEStatus, AC_EVSEStatusType (1, 1); EVSENominalVoltage, PhysicalValueType (1, 1); EVSEMaxCurrent, PhysicalValueType (1, 1);
static int encode_iso2_AC_EVSEChargeParameterType(exi_bitstream_t* stream, const struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    int grammar_id = 186;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 186:
            // Grammar: ID=186; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=187
                error = encode_iso2_AC_EVSEStatusType(stream, &AC_EVSEChargeParameterType->AC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 187;
                }
            }
            break;
        case 187:
            // Grammar: ID=187; read/write bits=1; START (EVSENominalVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=188
                error = encode_iso2_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSENominalVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 188;
                }
            }
            break;
        case 188:
            // Grammar: ID=188; read/write bits=1; START (EVSEMaxCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &AC_EVSEChargeParameterType->EVSEMaxCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeterInfo; type={urn:iso:15118:2:2013:MsgDataTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); MeterReading, unsignedLong (0, 1); SigMeterReading, sigMeterReadingType (0, 1); MeterStatus, meterStatusType (0, 1); TMeter, long (0, 1);
static int encode_iso2_MeterInfoType(exi_bitstream_t* stream, const struct iso2_MeterInfoType* MeterInfoType) {
    int grammar_id = 189;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 189:
            // Grammar: ID=189; read/write bits=1; START (MeterID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=190

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeterInfoType->MeterID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso2_MeterID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 190;
                            }
                        }
                    }
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=3; START (MeterReading), START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->MeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterReading, nonNegativeInteger); next=191
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterReading);
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
            else if (MeterInfoType->SigMeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SigMeterReading, base64Binary); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->SigMeterReading.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->SigMeterReading.bytesLen, MeterInfoType->SigMeterReading.bytes, iso2_sigMeterReadingType_BYTES_SIZE);
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
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=193
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
                                grammar_id = 193;
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
        case 191:
            // Grammar: ID=191; read/write bits=3; START (SigMeterReading), START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->SigMeterReading_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SigMeterReading, base64Binary); next=192
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->SigMeterReading.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->SigMeterReading.bytesLen, MeterInfoType->SigMeterReading.bytes, iso2_sigMeterReadingType_BYTES_SIZE);
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
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=193
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
                                grammar_id = 193;
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
        case 192:
            // Grammar: ID=192; read/write bits=2; START (MeterStatus), START (TMeter), END Element
            if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, short); next=193
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
                                grammar_id = 193;
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
        case 193:
            // Grammar: ID=193; read/write bits=2; START (TMeter), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Header; type={urn:iso:15118:2:2013:MsgHeader}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); Notification, NotificationType (0, 1); Signature, SignatureType (0, 1);
static int encode_iso2_MessageHeaderType(exi_bitstream_t* stream, const struct iso2_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 194;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 194:
            // Grammar: ID=194; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=195
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MessageHeaderType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MessageHeaderType->SessionID.bytesLen, MessageHeaderType->SessionID.bytes, iso2_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 195;
                            }
                        }
                    }
                }
            }
            break;
        case 195:
            // Grammar: ID=195; read/write bits=2; START (Notification), START (Signature), END Element
            if (MessageHeaderType->Notification_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Notification, NotificationType); next=196
                    error = encode_iso2_NotificationType(stream, &MessageHeaderType->Notification);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 196;
                    }
                }
            }
            else if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=3
                    error = encode_iso2_SignatureType(stream, &MessageHeaderType->Signature);
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
        case 196:
            // Grammar: ID=196; read/write bits=2; START (Signature), END Element
            if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=3
                    error = encode_iso2_SignatureType(stream, &MessageHeaderType->Signature);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReq; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargeProgress, chargeProgressType (1, 1); SAScheduleTupleID, SAIDType (1, 1); ChargingProfile, ChargingProfileType (0, 1); DC_EVPowerDeliveryParameter, DC_EVPowerDeliveryParameterType (0, 1); EVPowerDeliveryParameter, EVPowerDeliveryParameterType (0, 1);
static int encode_iso2_PowerDeliveryReqType(exi_bitstream_t* stream, const struct iso2_PowerDeliveryReqType* PowerDeliveryReqType) {
    int grammar_id = 197;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 197:
            // Grammar: ID=197; read/write bits=1; START (ChargeProgress)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=198
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, PowerDeliveryReqType->ChargeProgress);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 198;
                        }
                    }
                }
            }
            break;
        case 198:
            // Grammar: ID=198; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=199
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)PowerDeliveryReqType->SAScheduleTupleID - 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 199;
                        }
                    }
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=3; START (ChargingProfile), START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            if (PowerDeliveryReqType->ChargingProfile_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ChargingProfile, ChargingProfileType); next=200
                    error = encode_iso2_ChargingProfileType(stream, &PowerDeliveryReqType->ChargingProfile);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 200;
                    }
                }
            }
            else if (PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVPowerDeliveryParameter, EVPowerDeliveryParameterType); next=3
                    error = encode_iso2_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
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
                    error = encode_iso2_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
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
        case 200:
            // Grammar: ID=200; read/write bits=2; START (DC_EVPowerDeliveryParameter), START (EVPowerDeliveryParameter), END Element
            if (PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVPowerDeliveryParameter, EVPowerDeliveryParameterType); next=3
                    error = encode_iso2_DC_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->DC_EVPowerDeliveryParameter);
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
                    error = encode_iso2_EVPowerDeliveryParameterType(stream, &PowerDeliveryReqType->EVPowerDeliveryParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandRes; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1); EVSEPresentCurrent, PhysicalValueType (1, 1); EVSECurrentLimitAchieved, boolean (1, 1); EVSEVoltageLimitAchieved, boolean (1, 1); EVSEPowerLimitAchieved, boolean (1, 1); EVSEMaximumVoltageLimit, PhysicalValueType (0, 1); EVSEMaximumCurrentLimit, PhysicalValueType (0, 1); EVSEMaximumPowerLimit, PhysicalValueType (0, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1);
static int encode_iso2_CurrentDemandResType(exi_bitstream_t* stream, const struct iso2_CurrentDemandResType* CurrentDemandResType) {
    int grammar_id = 201;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 201:
            // Grammar: ID=201; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=202
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
                            grammar_id = 202;
                        }
                    }
                }
            }
            break;
        case 202:
            // Grammar: ID=202; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=203
                error = encode_iso2_DC_EVSEStatusType(stream, &CurrentDemandResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 203;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=204
                error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 204;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=1; START (EVSEPresentCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=205
                error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEPresentCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 205;
                }
            }
            break;
        case 205:
            // Grammar: ID=205; read/write bits=1; START (EVSECurrentLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=206
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
                            grammar_id = 206;
                        }
                    }
                }
            }
            break;
        case 206:
            // Grammar: ID=206; read/write bits=1; START (EVSEVoltageLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=207
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
                            grammar_id = 207;
                        }
                    }
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=1; START (EVSEPowerLimitAchieved)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=208
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
                            grammar_id = 208;
                        }
                    }
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=3; START (EVSEMaximumVoltageLimit), START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), START (EVSEID)
            if (CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumVoltageLimit, PhysicalValueType); next=209
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 209;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType); next=210
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 210;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=211
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 211;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEID, string); next=212

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CurrentDemandResType->EVSEID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
        case 209:
            // Grammar: ID=209; read/write bits=2; START (EVSEMaximumCurrentLimit), START (EVSEMaximumPowerLimit), START (EVSEID)
            if (CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumCurrentLimit, PhysicalValueType); next=210
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 210;
                    }
                }
            }
            else if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=211
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 211;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEID, string); next=212

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CurrentDemandResType->EVSEID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
        case 210:
            // Grammar: ID=210; read/write bits=2; START (EVSEMaximumPowerLimit), START (EVSEID)
            if (CurrentDemandResType->EVSEMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaximumPowerLimit, PhysicalValueType); next=211
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandResType->EVSEMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 211;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEID, string); next=212

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CurrentDemandResType->EVSEID.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
            // Grammar: ID=211; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=212

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CurrentDemandResType->EVSEID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CurrentDemandResType->EVSEID.charactersLen, CurrentDemandResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
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
            // Grammar: ID=212; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=213
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)CurrentDemandResType->SAScheduleTupleID - 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 213;
                        }
                    }
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=2; START (MeterInfo), START (ReceiptRequired), END Element
            if (CurrentDemandResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=214
                    error = encode_iso2_MeterInfoType(stream, &CurrentDemandResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 214;
                    }
                }
            }
            else if (CurrentDemandResType->ReceiptRequired_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandResType->ReceiptRequired);
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
        case 214:
            // Grammar: ID=214; read/write bits=2; START (ReceiptRequired), END Element
            if (CurrentDemandResType->ReceiptRequired_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, CurrentDemandResType->ReceiptRequired);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusRes; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); SAScheduleTupleID, SAIDType (1, 1); EVSEMaxCurrent, PhysicalValueType (0, 1); MeterInfo, MeterInfoType (0, 1); ReceiptRequired, boolean (0, 1); AC_EVSEStatus, AC_EVSEStatusType (1, 1);
static int encode_iso2_ChargingStatusResType(exi_bitstream_t* stream, const struct iso2_ChargingStatusResType* ChargingStatusResType) {
    int grammar_id = 215;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 215:
            // Grammar: ID=215; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=216
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
                            grammar_id = 216;
                        }
                    }
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=217

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ChargingStatusResType->EVSEID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ChargingStatusResType->EVSEID.charactersLen, ChargingStatusResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 217;
                            }
                        }
                    }
                }
            }
            break;
        case 217:
            // Grammar: ID=217; read/write bits=1; START (SAScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedByte); next=218
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)ChargingStatusResType->SAScheduleTupleID - 1);
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
            break;
        case 218:
            // Grammar: ID=218; read/write bits=3; START (EVSEMaxCurrent), START (MeterInfo), START (ReceiptRequired), START (AC_EVSEStatus)
            if (ChargingStatusResType->EVSEMaxCurrent_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEMaxCurrent, PhysicalValueType); next=219
                    error = encode_iso2_PhysicalValueType(stream, &ChargingStatusResType->EVSEMaxCurrent);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 219;
                    }
                }
            }
            else if (ChargingStatusResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=220
                    error = encode_iso2_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 220;
                    }
                }
            }
            else if (ChargingStatusResType->ReceiptRequired_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=221
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
                                grammar_id = 221;
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
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 219:
            // Grammar: ID=219; read/write bits=2; START (MeterInfo), START (ReceiptRequired), START (AC_EVSEStatus)
            if (ChargingStatusResType->MeterInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=220
                    error = encode_iso2_MeterInfoType(stream, &ChargingStatusResType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 220;
                    }
                }
            }
            else if (ChargingStatusResType->ReceiptRequired_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=221
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
                                grammar_id = 221;
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
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=2; START (ReceiptRequired), START (AC_EVSEStatus)
            if (ChargingStatusResType->ReceiptRequired_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReceiptRequired, boolean); next=221
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
                                grammar_id = 221;
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
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 221:
            // Grammar: ID=221; read/write bits=1; START (AC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=3
                error = encode_iso2_AC_EVSEStatusType(stream, &ChargingStatusResType->AC_EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationReq; type={urn:iso:15118:2:2013:MsgBody}AuthorizationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); GenChallenge, genChallengeType (0, 1);
static int encode_iso2_AuthorizationReqType(exi_bitstream_t* stream, const struct iso2_AuthorizationReqType* AuthorizationReqType) {
    int grammar_id = 222;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 222:
            // Grammar: ID=222; read/write bits=2; START (Id), START (GenChallenge), END Element
            if (AuthorizationReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=223

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AuthorizationReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AuthorizationReqType->Id.charactersLen, AuthorizationReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 223;
                        }
                    }
                }
            }
            else if (AuthorizationReqType->GenChallenge_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (GenChallenge, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)AuthorizationReqType->GenChallenge.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, AuthorizationReqType->GenChallenge.bytesLen, AuthorizationReqType->GenChallenge.bytes, iso2_genChallengeType_BYTES_SIZE);
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
        case 223:
            // Grammar: ID=223; read/write bits=2; START (GenChallenge), END Element
            if (AuthorizationReqType->GenChallenge_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (GenChallenge, base64Binary); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)AuthorizationReqType->GenChallenge.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, AuthorizationReqType->GenChallenge.bytesLen, AuthorizationReqType->GenChallenge.bytes, iso2_genChallengeType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeReq; type={urn:iso:15118:2:2013:MsgBody}PreChargeReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetVoltage, PhysicalValueType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1);
static int encode_iso2_PreChargeReqType(exi_bitstream_t* stream, const struct iso2_PreChargeReqType* PreChargeReqType) {
    int grammar_id = 224;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 224:
            // Grammar: ID=224; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=225
                error = encode_iso2_DC_EVStatusType(stream, &PreChargeReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 225;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=226
                error = encode_iso2_PhysicalValueType(stream, &PreChargeReqType->EVTargetVoltage);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 226;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &PreChargeReqType->EVTargetCurrent);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (0, 1);
static int encode_iso2_ServiceDetailResType(exi_bitstream_t* stream, const struct iso2_ServiceDetailResType* ServiceDetailResType) {
    int grammar_id = 227;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 227:
            // Grammar: ID=227; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=228
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
                            grammar_id = 228;
                        }
                    }
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=229
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
                            grammar_id = 229;
                        }
                    }
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=2; START (ServiceParameterList), END Element
            if (ServiceDetailResType->ServiceParameterList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceParameterList, ServiceParameterListType); next=3
                    error = encode_iso2_ServiceParameterListType(stream, &ServiceDetailResType->ServiceParameterList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionRes; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int encode_iso2_PaymentServiceSelectionResType(exi_bitstream_t* stream, const struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType) {
    int grammar_id = 230;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 230:
            // Grammar: ID=230; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, PaymentServiceSelectionResType->ResponseCode);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReq; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); eMAID, eMAIDType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
static int encode_iso2_CertificateUpdateReqType(exi_bitstream_t* stream, const struct iso2_CertificateUpdateReqType* CertificateUpdateReqType) {
    int grammar_id = 231;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 231:
            // Grammar: ID=231; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=232

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateReqType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CertificateUpdateReqType->Id.charactersLen, CertificateUpdateReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 232;
                    }
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=233
                error = encode_iso2_CertificateChainType(stream, &CertificateUpdateReqType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 233;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=1; START (eMAID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=234

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateUpdateReqType->eMAID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, CertificateUpdateReqType->eMAID.charactersLen, CertificateUpdateReqType->eMAID.characters, iso2_eMAID_CHARACTER_SIZE);
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
            // Grammar: ID=234; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ListOfRootCertificateIDsType); next=3
                error = encode_iso2_ListOfRootCertificateIDsType(stream, &CertificateUpdateReqType->ListOfRootCertificateIDs);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupRes; type={urn:iso:15118:2:2013:MsgBody}SessionSetupResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEID, evseIDType (1, 1); EVSETimeStamp, long (0, 1);
static int encode_iso2_SessionSetupResType(exi_bitstream_t* stream, const struct iso2_SessionSetupResType* SessionSetupResType) {
    int grammar_id = 235;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 235:
            // Grammar: ID=235; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=236
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
                            grammar_id = 236;
                        }
                    }
                }
            }
            break;
        case 236:
            // Grammar: ID=236; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=237

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionSetupResType->EVSEID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SessionSetupResType->EVSEID.charactersLen, SessionSetupResType->EVSEID.characters, iso2_EVSEID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 237;
                            }
                        }
                    }
                }
            }
            break;
        case 237:
            // Grammar: ID=237; read/write bits=2; START (EVSETimeStamp), END Element
            if (SessionSetupResType->EVSETimeStamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSETimeStamp, integer); next=3
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_64(stream, SessionSetupResType->EVSETimeStamp);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReq; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (1, 1); OEMProvisioningCert, certificateType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1);
static int encode_iso2_CertificateInstallationReqType(exi_bitstream_t* stream, const struct iso2_CertificateInstallationReqType* CertificateInstallationReqType) {
    int grammar_id = 238;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 238:
            // Grammar: ID=238; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=239

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CertificateInstallationReqType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CertificateInstallationReqType->Id.charactersLen, CertificateInstallationReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 239;
                    }
                }
            }
            break;
        case 239:
            // Grammar: ID=239; read/write bits=1; START (OEMProvisioningCert)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=240
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateInstallationReqType->OEMProvisioningCert.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateInstallationReqType->OEMProvisioningCert.bytesLen, CertificateInstallationReqType->OEMProvisioningCert.bytes, iso2_certificateType_BYTES_SIZE);
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
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ListOfRootCertificateIDsType); next=3
                error = encode_iso2_ListOfRootCertificateIDsType(stream, &CertificateInstallationReqType->ListOfRootCertificateIDs);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateInstallationRes; type={urn:iso:15118:2:2013:MsgBody}CertificateInstallationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1);
static int encode_iso2_CertificateInstallationResType(exi_bitstream_t* stream, const struct iso2_CertificateInstallationResType* CertificateInstallationResType) {
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
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, CertificateInstallationResType->ResponseCode);
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
            // Grammar: ID=242; read/write bits=1; START (SAProvisioningCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=243
                error = encode_iso2_CertificateChainType(stream, &CertificateInstallationResType->SAProvisioningCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 243;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=244
                error = encode_iso2_CertificateChainType(stream, &CertificateInstallationResType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 244;
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (privateKeyType); next=245
                error = encode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &CertificateInstallationResType->ContractSignatureEncryptedPrivateKey);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 245;
                }
            }
            break;
        case 245:
            // Grammar: ID=245; read/write bits=1; START (DHpublickey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (dHpublickeyType); next=246
                error = encode_iso2_DiffieHellmanPublickeyType(stream, &CertificateInstallationResType->DHpublickey);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 246;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=1; START (eMAID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (eMAIDType); next=3
                error = encode_iso2_EMAIDType(stream, &CertificateInstallationResType->eMAID);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionRes; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int encode_iso2_WeldingDetectionResType(exi_bitstream_t* stream, const struct iso2_WeldingDetectionResType* WeldingDetectionResType) {
    int grammar_id = 247;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 247:
            // Grammar: ID=247; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=248
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
                            grammar_id = 248;
                        }
                    }
                }
            }
            break;
        case 248:
            // Grammar: ID=248; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=249
                error = encode_iso2_DC_EVSEStatusType(stream, &WeldingDetectionResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 249;
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &WeldingDetectionResType->EVSEPresentVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CurrentDemandReq; type={urn:iso:15118:2:2013:MsgBody}CurrentDemandReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1); EVTargetCurrent, PhysicalValueType (1, 1); EVMaximumVoltageLimit, PhysicalValueType (0, 1); EVMaximumCurrentLimit, PhysicalValueType (0, 1); EVMaximumPowerLimit, PhysicalValueType (0, 1); BulkChargingComplete, boolean (0, 1); ChargingComplete, boolean (1, 1); RemainingTimeToFullSoC, PhysicalValueType (0, 1); RemainingTimeToBulkSoC, PhysicalValueType (0, 1); EVTargetVoltage, PhysicalValueType (1, 1);
static int encode_iso2_CurrentDemandReqType(exi_bitstream_t* stream, const struct iso2_CurrentDemandReqType* CurrentDemandReqType) {
    int grammar_id = 250;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 250:
            // Grammar: ID=250; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=251
                error = encode_iso2_DC_EVStatusType(stream, &CurrentDemandReqType->DC_EVStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 251;
                }
            }
            break;
        case 251:
            // Grammar: ID=251; read/write bits=1; START (EVTargetCurrent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=252
                error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetCurrent);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 252;
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=3; START (EVMaximumVoltageLimit), START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumVoltageLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumVoltageLimit, PhysicalValueType); next=253
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumVoltageLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 253;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType); next=254
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=255
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=256
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
                                grammar_id = 256;
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
                    // Event: START (ChargingComplete, boolean); next=257
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
                                grammar_id = 257;
                            }
                        }
                    }
                }
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=3; START (EVMaximumCurrentLimit), START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumCurrentLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumCurrentLimit, PhysicalValueType); next=254
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumCurrentLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 254;
                    }
                }
            }
            else if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=255
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=256
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
                                grammar_id = 256;
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
                    // Event: START (ChargingComplete, boolean); next=257
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
                                grammar_id = 257;
                            }
                        }
                    }
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=2; START (EVMaximumPowerLimit), START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->EVMaximumPowerLimit_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumPowerLimit, PhysicalValueType); next=255
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVMaximumPowerLimit);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 255;
                    }
                }
            }
            else if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=256
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
                                grammar_id = 256;
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
                    // Event: START (ChargingComplete, boolean); next=257
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
                                grammar_id = 257;
                            }
                        }
                    }
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=2; START (BulkChargingComplete), START (ChargingComplete)
            if (CurrentDemandReqType->BulkChargingComplete_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BulkChargingComplete, boolean); next=256
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
                                grammar_id = 256;
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
                    // Event: START (ChargingComplete, boolean); next=257
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
                                grammar_id = 257;
                            }
                        }
                    }
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=1; START (ChargingComplete)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=257
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
                            grammar_id = 257;
                        }
                    }
                }
            }
            break;
        case 257:
            // Grammar: ID=257; read/write bits=2; START (RemainingTimeToFullSoC), START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            if (CurrentDemandReqType->RemainingTimeToFullSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToFullSoC, PhysicalValueType); next=258
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToFullSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 258;
                    }
                }
            }
            else if (CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType); next=259
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 259;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetVoltage, PhysicalValueType); next=3
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=2; START (RemainingTimeToBulkSoC), START (EVTargetVoltage)
            if (CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RemainingTimeToBulkSoC, PhysicalValueType); next=259
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->RemainingTimeToBulkSoC);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 259;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetVoltage, PhysicalValueType); next=3
                    error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=1; START (EVTargetVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &CurrentDemandReqType->EVTargetVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PreChargeRes; type={urn:iso:15118:2:2013:MsgBody}PreChargeResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEPresentVoltage, PhysicalValueType (1, 1);
static int encode_iso2_PreChargeResType(exi_bitstream_t* stream, const struct iso2_PreChargeResType* PreChargeResType) {
    int grammar_id = 260;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 260:
            // Grammar: ID=260; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=261
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
                            grammar_id = 261;
                        }
                    }
                }
            }
            break;
        case 261:
            // Grammar: ID=261; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=262
                error = encode_iso2_DC_EVSEStatusType(stream, &PreChargeResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 262;
                }
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=1; START (EVSEPresentVoltage)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PhysicalValueType); next=3
                error = encode_iso2_PhysicalValueType(stream, &PreChargeResType->EVSEPresentVoltage);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CertificateUpdateRes; type={urn:iso:15118:2:2013:MsgBody}CertificateUpdateResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); SAProvisioningCertificateChain, CertificateChainType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1); ContractSignatureEncryptedPrivateKey, ContractSignatureEncryptedPrivateKeyType (1, 1); DHpublickey, DiffieHellmanPublickeyType (1, 1); eMAID, EMAIDType (1, 1); RetryCounter, short (0, 1);
static int encode_iso2_CertificateUpdateResType(exi_bitstream_t* stream, const struct iso2_CertificateUpdateResType* CertificateUpdateResType) {
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
                // Event: START (string); next=264
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
                            grammar_id = 264;
                        }
                    }
                }
            }
            break;
        case 264:
            // Grammar: ID=264; read/write bits=1; START (SAProvisioningCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=265
                error = encode_iso2_CertificateChainType(stream, &CertificateUpdateResType->SAProvisioningCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 265;
                }
            }
            break;
        case 265:
            // Grammar: ID=265; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=266
                error = encode_iso2_CertificateChainType(stream, &CertificateUpdateResType->ContractSignatureCertChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 266;
                }
            }
            break;
        case 266:
            // Grammar: ID=266; read/write bits=1; START (ContractSignatureEncryptedPrivateKey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (privateKeyType); next=267
                error = encode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &CertificateUpdateResType->ContractSignatureEncryptedPrivateKey);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 267;
                }
            }
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (DHpublickey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (dHpublickeyType); next=268
                error = encode_iso2_DiffieHellmanPublickeyType(stream, &CertificateUpdateResType->DHpublickey);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 268;
                }
            }
            break;
        case 268:
            // Grammar: ID=268; read/write bits=1; START (eMAID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (eMAIDType); next=269
                error = encode_iso2_EMAIDType(stream, &CertificateUpdateResType->eMAID);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 269;
                }
            }
            break;
        case 269:
            // Grammar: ID=269; read/write bits=2; START (RetryCounter), END Element
            if (CertificateUpdateResType->RetryCounter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetryCounter, int); next=3
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReq; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); SessionID, sessionIDType (1, 1); SAScheduleTupleID, SAIDType (0, 1); MeterInfo, MeterInfoType (1, 1);
static int encode_iso2_MeteringReceiptReqType(exi_bitstream_t* stream, const struct iso2_MeteringReceiptReqType* MeteringReceiptReqType) {
    int grammar_id = 270;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 270:
            // Grammar: ID=270; read/write bits=2; START (Id), START (SessionID)
            if (MeteringReceiptReqType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=271

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeteringReceiptReqType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeteringReceiptReqType->Id.charactersLen, MeteringReceiptReqType->Id.characters, iso2_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 271;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SessionID, hexBinary); next=272
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeteringReceiptReqType->SessionID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeteringReceiptReqType->SessionID.bytesLen, MeteringReceiptReqType->SessionID.bytes, iso2_sessionIDType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 272;
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 271:
            // Grammar: ID=271; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=272
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeteringReceiptReqType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MeteringReceiptReqType->SessionID.bytesLen, MeteringReceiptReqType->SessionID.bytes, iso2_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 272;
                            }
                        }
                    }
                }
            }
            break;
        case 272:
            // Grammar: ID=272; read/write bits=2; START (SAScheduleTupleID), START (MeterInfo)
            if (MeteringReceiptReqType->SAScheduleTupleID_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleTupleID, unsignedByte); next=273
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, (uint32_t)MeteringReceiptReqType->SAScheduleTupleID - 1);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterInfo, MeterInfoType); next=3
                    error = encode_iso2_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 273:
            // Grammar: ID=273; read/write bits=1; START (MeterInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MeterInfoType); next=3
                error = encode_iso2_MeterInfoType(stream, &MeteringReceiptReqType->MeterInfo);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargingStatusReq; type={urn:iso:15118:2:2013:MsgBody}ChargingStatusReqType; base type=BodyBaseType; content type=empty;
//          abstract=False; final=False; derivation=extension;
static int encode_iso2_ChargingStatusReqType(exi_bitstream_t* stream, const struct iso2_ChargingStatusReqType* ChargingStatusReqType) {
    // Element has no particles, so the function just encodes END Element
    (void)ChargingStatusReqType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopRes; type={urn:iso:15118:2:2013:MsgBody}SessionStopResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1);
static int encode_iso2_SessionStopResType(exi_bitstream_t* stream, const struct iso2_SessionStopResType* SessionStopResType) {
    int grammar_id = 274;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 274:
            // Grammar: ID=274; read/write bits=1; START (ResponseCode)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: MaxEntriesSAScheduleTuple, unsignedShort (0, 1); RequestedEnergyTransferMode, EnergyTransferModeType (1, 1); AC_EVChargeParameter, AC_EVChargeParameterType (0, 1); DC_EVChargeParameter, DC_EVChargeParameterType (0, 1); EVChargeParameter, EVChargeParameterType (0, 1);
static int encode_iso2_ChargeParameterDiscoveryReqType(exi_bitstream_t* stream, const struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    int grammar_id = 275;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 275:
            // Grammar: ID=275; read/write bits=2; START (MaxEntriesSAScheduleTuple), START (RequestedEnergyTransferMode)
            if (ChargeParameterDiscoveryReqType->MaxEntriesSAScheduleTuple_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaxEntriesSAScheduleTuple, unsignedInt); next=276
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ChargeParameterDiscoveryReqType->MaxEntriesSAScheduleTuple);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 276;
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
                    // Event: START (RequestedEnergyTransferMode, string); next=277
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 3, ChargeParameterDiscoveryReqType->RequestedEnergyTransferMode);
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
            }
            break;
        case 276:
            // Grammar: ID=276; read/write bits=1; START (RequestedEnergyTransferMode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=277
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, ChargeParameterDiscoveryReqType->RequestedEnergyTransferMode);
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
            // Grammar: ID=277; read/write bits=2; START (AC_EVChargeParameter), START (DC_EVChargeParameter), START (EVChargeParameter)
            if (ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVChargeParameter, EVChargeParameterType); next=3
                    error = encode_iso2_AC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->AC_EVChargeParameter);
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
                    error = encode_iso2_DC_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->DC_EVChargeParameter);
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
                    // Event: START (EVChargeParameter, EVChargeParameterType); next=3
                    error = encode_iso2_EVChargeParameterType(stream, &ChargeParameterDiscoveryReqType->EVChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckReq; type={urn:iso:15118:2:2013:MsgBody}CableCheckReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int encode_iso2_CableCheckReqType(exi_bitstream_t* stream, const struct iso2_CableCheckReqType* CableCheckReqType) {
    int grammar_id = 278;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 278:
            // Grammar: ID=278; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=3
                error = encode_iso2_DC_EVStatusType(stream, &CableCheckReqType->DC_EVStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReq; type={urn:iso:15118:2:2013:MsgBody}WeldingDetectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: DC_EVStatus, DC_EVStatusType (1, 1);
static int encode_iso2_WeldingDetectionReqType(exi_bitstream_t* stream, const struct iso2_WeldingDetectionReqType* WeldingDetectionReqType) {
    int grammar_id = 279;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 279:
            // Grammar: ID=279; read/write bits=1; START (DC_EVStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVStatusType); next=3
                error = encode_iso2_DC_EVStatusType(stream, &WeldingDetectionReqType->DC_EVStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PowerDeliveryRes; type={urn:iso:15118:2:2013:MsgBody}PowerDeliveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
static int encode_iso2_PowerDeliveryResType(exi_bitstream_t* stream, const struct iso2_PowerDeliveryResType* PowerDeliveryResType) {
    int grammar_id = 280;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 280:
            // Grammar: ID=280; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=281
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
                            grammar_id = 281;
                        }
                    }
                }
            }
            break;
        case 281:
            // Grammar: ID=281; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            if (PowerDeliveryResType->AC_EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_AC_EVSEStatusType(stream, &PowerDeliveryResType->AC_EVSEStatus);
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
                    error = encode_iso2_DC_EVSEStatusType(stream, &PowerDeliveryResType->DC_EVSEStatus);
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
                    // Event: START (EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_EVSEStatusType(stream, &PowerDeliveryResType->EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ChargeParameterDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1); SAScheduleList, SAScheduleListType (0, 1); SASchedules, SASchedulesType (0, 1); AC_EVSEChargeParameter, AC_EVSEChargeParameterType (0, 1); DC_EVSEChargeParameter, DC_EVSEChargeParameterType (0, 1); EVSEChargeParameter, EVSEChargeParameterType (0, 1);
static int encode_iso2_ChargeParameterDiscoveryResType(exi_bitstream_t* stream, const struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    int grammar_id = 282;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 282:
            // Grammar: ID=282; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=283
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
                            grammar_id = 283;
                        }
                    }
                }
            }
            break;
        case 283:
            // Grammar: ID=283; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=284
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, ChargeParameterDiscoveryResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 284;
                        }
                    }
                }
            }
            break;
        case 284:
            // Grammar: ID=284; read/write bits=3; START (SAScheduleList), START (SASchedules), START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            if (ChargeParameterDiscoveryResType->SAScheduleList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SAScheduleList, SASchedulesType); next=285
                    error = encode_iso2_SAScheduleListType(stream, &ChargeParameterDiscoveryResType->SAScheduleList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 285;
                    }
                }
            }
            else if (ChargeParameterDiscoveryResType->SASchedules_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (SASchedulesType); next=285
                    error = encode_iso2_SASchedulesType(stream, &ChargeParameterDiscoveryResType->SASchedules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 285;
                    }
                }
            }
            else if (ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEChargeParameter, EVSEChargeParameterType); next=3
                    error = encode_iso2_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVSEChargeParameter, EVSEChargeParameterType); next=3
                    error = encode_iso2_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Abstract element or type: START (EVSEChargeParameterType); next=3
                    error = encode_iso2_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            break;
        case 285:
            // Grammar: ID=285; read/write bits=2; START (AC_EVSEChargeParameter), START (DC_EVSEChargeParameter), START (EVSEChargeParameter)
            if (ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEChargeParameter, EVSEChargeParameterType); next=3
                    error = encode_iso2_AC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->AC_EVSEChargeParameter);
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
                    error = encode_iso2_DC_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->DC_EVSEChargeParameter);
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
                    error = encode_iso2_EVSEChargeParameterType(stream, &ChargeParameterDiscoveryResType->EVSEChargeParameter);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReq; type={urn:iso:15118:2:2013:MsgBody}PaymentServiceSelectionReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: SelectedPaymentOption, paymentOptionType (1, 1); SelectedServiceList, SelectedServiceListType (1, 1);
static int encode_iso2_PaymentServiceSelectionReqType(exi_bitstream_t* stream, const struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType) {
    int grammar_id = 286;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 286:
            // Grammar: ID=286; read/write bits=1; START (SelectedPaymentOption)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=287
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, PaymentServiceSelectionReqType->SelectedPaymentOption);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 287;
                        }
                    }
                }
            }
            break;
        case 287:
            // Grammar: ID=287; read/write bits=1; START (SelectedServiceList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SelectedServiceListType); next=3
                error = encode_iso2_SelectedServiceListType(stream, &PaymentServiceSelectionReqType->SelectedServiceList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}MeteringReceiptRes; type={urn:iso:15118:2:2013:MsgBody}MeteringReceiptResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); AC_EVSEStatus, AC_EVSEStatusType (0, 1); DC_EVSEStatus, DC_EVSEStatusType (0, 1); EVSEStatus, EVSEStatusType (0, 1);
static int encode_iso2_MeteringReceiptResType(exi_bitstream_t* stream, const struct iso2_MeteringReceiptResType* MeteringReceiptResType) {
    int grammar_id = 288;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 288:
            // Grammar: ID=288; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=289
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
                            grammar_id = 289;
                        }
                    }
                }
            }
            break;
        case 289:
            // Grammar: ID=289; read/write bits=2; START (AC_EVSEStatus), START (DC_EVSEStatus), START (EVSEStatus)
            if (MeteringReceiptResType->AC_EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_AC_EVSEStatusType(stream, &MeteringReceiptResType->AC_EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (MeteringReceiptResType->DC_EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DC_EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_DC_EVSEStatusType(stream, &MeteringReceiptResType->DC_EVSEStatus);
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
                    // Event: START (EVSEStatus, EVSEStatusType); next=3
                    error = encode_iso2_EVSEStatusType(stream, &MeteringReceiptResType->EVSEStatus);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}CableCheckRes; type={urn:iso:15118:2:2013:MsgBody}CableCheckResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); DC_EVSEStatus, DC_EVSEStatusType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int encode_iso2_CableCheckResType(exi_bitstream_t* stream, const struct iso2_CableCheckResType* CableCheckResType) {
    int grammar_id = 290;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 290:
            // Grammar: ID=290; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=291
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
                            grammar_id = 291;
                        }
                    }
                }
            }
            break;
        case 291:
            // Grammar: ID=291; read/write bits=1; START (DC_EVSEStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVSEStatusType); next=292
                error = encode_iso2_DC_EVSEStatusType(stream, &CableCheckResType->DC_EVSEStatus);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 292;
                }
            }
            break;
        case 292:
            // Grammar: ID=292; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, CableCheckResType->EVSEProcessing);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryRes; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); PaymentOptionList, PaymentOptionListType (1, 1); ChargeService, ChargeServiceType (1, 1); ServiceList, ServiceListType (0, 1);
static int encode_iso2_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    int grammar_id = 293;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 293:
            // Grammar: ID=293; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=294
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
                            grammar_id = 294;
                        }
                    }
                }
            }
            break;
        case 294:
            // Grammar: ID=294; read/write bits=1; START (PaymentOptionList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PaymentOptionListType); next=295
                error = encode_iso2_PaymentOptionListType(stream, &ServiceDiscoveryResType->PaymentOptionList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 295;
                }
            }
            break;
        case 295:
            // Grammar: ID=295; read/write bits=1; START (ChargeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceType); next=296
                error = encode_iso2_ChargeServiceType(stream, &ServiceDiscoveryResType->ChargeService);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 296;
                }
            }
            break;
        case 296:
            // Grammar: ID=296; read/write bits=2; START (ServiceList), END Element
            if (ServiceDiscoveryResType->ServiceList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceList, ServiceListType); next=3
                    error = encode_iso2_ServiceListType(stream, &ServiceDiscoveryResType->ServiceList);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDetailReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDetailReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceID, serviceIDType (1, 1);
static int encode_iso2_ServiceDetailReqType(exi_bitstream_t* stream, const struct iso2_ServiceDetailReqType* ServiceDetailReqType) {
    int grammar_id = 297;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 297:
            // Grammar: ID=297; read/write bits=1; START (ServiceID)
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionSetupReq; type={urn:iso:15118:2:2013:MsgBody}SessionSetupReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: EVCCID, evccIDType (1, 1);
static int encode_iso2_SessionSetupReqType(exi_bitstream_t* stream, const struct iso2_SessionSetupReqType* SessionSetupReqType) {
    int grammar_id = 298;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 298:
            // Grammar: ID=298; read/write bits=1; START (EVCCID)
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
                        error = exi_basetypes_encoder_bytes(stream, SessionSetupReqType->EVCCID.bytesLen, SessionSetupReqType->EVCCID.bytes, iso2_evccIDType_BYTES_SIZE);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}SessionStopReq; type={urn:iso:15118:2:2013:MsgBody}SessionStopReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ChargingSession, chargingSessionType (1, 1);
static int encode_iso2_SessionStopReqType(exi_bitstream_t* stream, const struct iso2_SessionStopReqType* SessionStopReqType) {
    int grammar_id = 299;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 299:
            // Grammar: ID=299; read/write bits=1; START (ChargingSession)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, SessionStopReqType->ChargingSession);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReq; type={urn:iso:15118:2:2013:MsgBody}ServiceDiscoveryReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ServiceScope, serviceScopeType (0, 1); ServiceCategory, serviceCategoryType (0, 1);
static int encode_iso2_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    int grammar_id = 300;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 300:
            // Grammar: ID=300; read/write bits=2; START (ServiceScope), START (ServiceCategory), END Element
            if (ServiceDiscoveryReqType->ServiceScope_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceScope, string); next=301

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ServiceDiscoveryReqType->ServiceScope.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ServiceDiscoveryReqType->ServiceScope.charactersLen, ServiceDiscoveryReqType->ServiceScope.characters, iso2_ServiceScope_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 301;
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
        case 301:
            // Grammar: ID=301; read/write bits=2; START (ServiceCategory), END Element
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}AuthorizationRes; type={urn:iso:15118:2:2013:MsgBody}AuthorizationResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); EVSEProcessing, EVSEProcessingType (1, 1);
static int encode_iso2_AuthorizationResType(exi_bitstream_t* stream, const struct iso2_AuthorizationResType* AuthorizationResType) {
    int grammar_id = 302;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 302:
            // Grammar: ID=302; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=303
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 5, AuthorizationResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 303;
                        }
                    }
                }
            }
            break;
        case 303:
            // Grammar: ID=303; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, AuthorizationResType->EVSEProcessing);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReq; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsReqType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: eMAID, eMAIDType (1, 1); ContractSignatureCertChain, CertificateChainType (1, 1);
static int encode_iso2_PaymentDetailsReqType(exi_bitstream_t* stream, const struct iso2_PaymentDetailsReqType* PaymentDetailsReqType) {
    int grammar_id = 304;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 304:
            // Grammar: ID=304; read/write bits=1; START (eMAID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=305

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PaymentDetailsReqType->eMAID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, PaymentDetailsReqType->eMAID.charactersLen, PaymentDetailsReqType->eMAID.characters, iso2_eMAID_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 305;
                            }
                        }
                    }
                }
            }
            break;
        case 305:
            // Grammar: ID=305; read/write bits=1; START (ContractSignatureCertChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=3
                error = encode_iso2_CertificateChainType(stream, &PaymentDetailsReqType->ContractSignatureCertChain);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgBody}PaymentDetailsRes; type={urn:iso:15118:2:2013:MsgBody}PaymentDetailsResType; base type=BodyBaseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: ResponseCode, responseCodeType (1, 1); GenChallenge, genChallengeType (1, 1); EVSETimeStamp, long (1, 1);
static int encode_iso2_PaymentDetailsResType(exi_bitstream_t* stream, const struct iso2_PaymentDetailsResType* PaymentDetailsResType) {
    int grammar_id = 306;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 306:
            // Grammar: ID=306; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=307
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
                            grammar_id = 307;
                        }
                    }
                }
            }
            break;
        case 307:
            // Grammar: ID=307; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=308
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PaymentDetailsResType->GenChallenge.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PaymentDetailsResType->GenChallenge.bytesLen, PaymentDetailsResType->GenChallenge.bytes, iso2_genChallengeType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 308;
                            }
                        }
                    }
                }
            }
            break;
        case 308:
            // Grammar: ID=308; read/write bits=1; START (EVSETimeStamp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (integer); next=3
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_64(stream, PaymentDetailsResType->EVSETimeStamp);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}Body; type={urn:iso:15118:2:2013:MsgBody}BodyType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AuthorizationReq, AuthorizationReqType (0, 1); AuthorizationRes, AuthorizationResType (0, 1); BodyElement, BodyBaseType (0, 1); CableCheckReq, CableCheckReqType (0, 1); CableCheckRes, CableCheckResType (0, 1); CertificateInstallationReq, CertificateInstallationReqType (0, 1); CertificateInstallationRes, CertificateInstallationResType (0, 1); CertificateUpdateReq, CertificateUpdateReqType (0, 1); CertificateUpdateRes, CertificateUpdateResType (0, 1); ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReqType (0, 1); ChargeParameterDiscoveryRes, ChargeParameterDiscoveryResType (0, 1); ChargingStatusReq, ChargingStatusReqType (0, 1); ChargingStatusRes, ChargingStatusResType (0, 1); CurrentDemandReq, CurrentDemandReqType (0, 1); CurrentDemandRes, CurrentDemandResType (0, 1); MeteringReceiptReq, MeteringReceiptReqType (0, 1); MeteringReceiptRes, MeteringReceiptResType (0, 1); PaymentDetailsReq, PaymentDetailsReqType (0, 1); PaymentDetailsRes, PaymentDetailsResType (0, 1); PaymentServiceSelectionReq, PaymentServiceSelectionReqType (0, 1); PaymentServiceSelectionRes, PaymentServiceSelectionResType (0, 1); PowerDeliveryReq, PowerDeliveryReqType (0, 1); PowerDeliveryRes, PowerDeliveryResType (0, 1); PreChargeReq, PreChargeReqType (0, 1); PreChargeRes, PreChargeResType (0, 1); ServiceDetailReq, ServiceDetailReqType (0, 1); ServiceDetailRes, ServiceDetailResType (0, 1); ServiceDiscoveryReq, ServiceDiscoveryReqType (0, 1); ServiceDiscoveryRes, ServiceDiscoveryResType (0, 1); SessionSetupReq, SessionSetupReqType (0, 1); SessionSetupRes, SessionSetupResType (0, 1); SessionStopReq, SessionStopReqType (0, 1); SessionStopRes, SessionStopResType (0, 1); WeldingDetectionReq, WeldingDetectionReqType (0, 1); WeldingDetectionRes, WeldingDetectionResType (0, 1);
static int encode_iso2_BodyType(exi_bitstream_t* stream, const struct iso2_BodyType* BodyType) {
    int grammar_id = 309;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 309:
            // Grammar: ID=309; read/write bits=6; START (AuthorizationReq)
            if (BodyType->AuthorizationReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: AuthorizationReq
                    error = encode_iso2_AuthorizationReqType(stream, &BodyType->AuthorizationReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->AuthorizationRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: AuthorizationRes
                    error = encode_iso2_AuthorizationResType(stream, &BodyType->AuthorizationRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->BodyElement_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: BodyElement
                    error = encode_iso2_BodyBaseType(stream, &BodyType->BodyElement);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CableCheckReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CableCheckReq
                    error = encode_iso2_CableCheckReqType(stream, &BodyType->CableCheckReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CableCheckRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CableCheckRes
                    error = encode_iso2_CableCheckResType(stream, &BodyType->CableCheckRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateInstallationReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateInstallationReq
                    error = encode_iso2_CertificateInstallationReqType(stream, &BodyType->CertificateInstallationReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateInstallationRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateInstallationRes
                    error = encode_iso2_CertificateInstallationResType(stream, &BodyType->CertificateInstallationRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateUpdateReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateUpdateReq
                    error = encode_iso2_CertificateUpdateReqType(stream, &BodyType->CertificateUpdateReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->CertificateUpdateRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: CertificateUpdateRes
                    error = encode_iso2_CertificateUpdateResType(stream, &BodyType->CertificateUpdateRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargeParameterDiscoveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 9);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargeParameterDiscoveryReq
                    error = encode_iso2_ChargeParameterDiscoveryReqType(stream, &BodyType->ChargeParameterDiscoveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargeParameterDiscoveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 10);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargeParameterDiscoveryRes
                    error = encode_iso2_ChargeParameterDiscoveryResType(stream, &BodyType->ChargeParameterDiscoveryRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargingStatusReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 11);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargingStatusReq
                    error = encode_iso2_ChargingStatusReqType(stream, &BodyType->ChargingStatusReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ChargingStatusRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 12);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ChargingStatusRes
                    error = encode_iso2_ChargingStatusResType(stream, &BodyType->ChargingStatusRes);
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
                    error = encode_iso2_CurrentDemandReqType(stream, &BodyType->CurrentDemandReq);
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
                    error = encode_iso2_CurrentDemandResType(stream, &BodyType->CurrentDemandRes);
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
                    error = encode_iso2_MeteringReceiptReqType(stream, &BodyType->MeteringReceiptReq);
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
                    error = encode_iso2_MeteringReceiptResType(stream, &BodyType->MeteringReceiptRes);
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
                    error = encode_iso2_PaymentDetailsReqType(stream, &BodyType->PaymentDetailsReq);
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
                    error = encode_iso2_PaymentDetailsResType(stream, &BodyType->PaymentDetailsRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PaymentServiceSelectionReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 19);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PaymentServiceSelectionReq
                    error = encode_iso2_PaymentServiceSelectionReqType(stream, &BodyType->PaymentServiceSelectionReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PaymentServiceSelectionRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 20);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PaymentServiceSelectionRes
                    error = encode_iso2_PaymentServiceSelectionResType(stream, &BodyType->PaymentServiceSelectionRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PowerDeliveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 21);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PowerDeliveryReq
                    error = encode_iso2_PowerDeliveryReqType(stream, &BodyType->PowerDeliveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PowerDeliveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PowerDeliveryRes
                    error = encode_iso2_PowerDeliveryResType(stream, &BodyType->PowerDeliveryRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PreChargeReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PreChargeReq
                    error = encode_iso2_PreChargeReqType(stream, &BodyType->PreChargeReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->PreChargeRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: PreChargeRes
                    error = encode_iso2_PreChargeResType(stream, &BodyType->PreChargeRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDetailReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 25);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDetailReq
                    error = encode_iso2_ServiceDetailReqType(stream, &BodyType->ServiceDetailReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDetailRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 26);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDetailRes
                    error = encode_iso2_ServiceDetailResType(stream, &BodyType->ServiceDetailRes);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDiscoveryReq_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 27);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDiscoveryReq
                    error = encode_iso2_ServiceDiscoveryReqType(stream, &BodyType->ServiceDiscoveryReq);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 3;
                    }
                }
            }
            else if (BodyType->ServiceDiscoveryRes_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: ServiceDiscoveryRes
                    error = encode_iso2_ServiceDiscoveryResType(stream, &BodyType->ServiceDiscoveryRes);
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
                    error = encode_iso2_SessionSetupReqType(stream, &BodyType->SessionSetupReq);
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
                    error = encode_iso2_SessionSetupResType(stream, &BodyType->SessionSetupRes);
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
                    error = encode_iso2_SessionStopReqType(stream, &BodyType->SessionStopReq);
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
                    error = encode_iso2_SessionStopResType(stream, &BodyType->SessionStopRes);
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
                    error = encode_iso2_WeldingDetectionReqType(stream, &BodyType->WeldingDetectionReq);
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
                    error = encode_iso2_WeldingDetectionResType(stream, &BodyType->WeldingDetectionRes);
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

// Element: definition=complex; name={urn:iso:15118:2:2013:MsgDef}V2G_Message; type=AnonymousType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Header, MessageHeaderType (1, 1); Body, BodyType (1, 1);
static int encode_iso2_V2G_Message(exi_bitstream_t* stream, const struct iso2_V2G_Message* V2G_Message) {
    int grammar_id = 310;
    int done = 0;
    int error = 0;

    while (!done)
    {
        switch (grammar_id)
        {
        case 310:
            // Grammar: ID=310; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=311
                error = encode_iso2_MessageHeaderType(stream, &V2G_Message->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 311;
                }
            }
            break;
        case 311:
            // Grammar: ID=311; read/write bits=1; START (Body)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (BodyType); next=3
                error = encode_iso2_BodyType(stream, &V2G_Message->Body);
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
int encode_iso2_exiDocument(exi_bitstream_t* stream, struct iso2_exiDocument* exiDoc)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        // encode event exiDoc->V2G_Message with index 76
        error = exi_basetypes_encoder_nbit_uint(stream, 7, 76);
        if (error == EXI_ERROR__NO_ERROR)
        {
            error = encode_iso2_V2G_Message(stream, &exiDoc->V2G_Message);
        }
    }

    return error;
}

// main function for encoding fragment
/* NOTE! There may be problems when comparing the signature of the eMAID.
   In the ISO 15118-2 schema there are two different types with problematic names,
   EMAIDType and eMAIDType. The fragment de- and encoder of e.g. openV2G considers
   this type as generic type EXISchemaInformedElementFragmentGrammar. We treat it as a complex type.
   We have not yet been able to determine why this particular type has to be coded as a generic type,
   and only for the fragment decoder and encoder.
   This is why we have not yet adapted our fragment coders, and it can lead to the problem mentioned. */
int encode_iso2_exiFragment(exi_bitstream_t* stream, struct iso2_exiFragment* exiFrag)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        // AC_EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        if (0 == 1)
        {
            error = EXI_ERROR__NOT_IMPLEMENTED_YET;
        }
        // AC_EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 1
        // AC_EVSEStatus (urn:iso:15118:2:2013:MsgBody)
        // event 2
        // AC_EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 3
        // AuthorizationReq (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->AuthorizationReq_isUsed == 1)
        {
            // encode event 4
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 4);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_AuthorizationReqType(stream, &exiFrag->AuthorizationReq);
            }
        }
        // AuthorizationRes (urn:iso:15118:2:2013:MsgBody)
        // event 5
        // Body (urn:iso:15118:2:2013:MsgDef)
        // event 6
        // BodyElement (urn:iso:15118:2:2013:MsgBody)
        // event 7
        // BulkChargingComplete (urn:iso:15118:2:2013:MsgBody)
        // event 8
        // BulkChargingComplete (urn:iso:15118:2:2013:MsgDataTypes)
        // event 9
        // BulkSOC (urn:iso:15118:2:2013:MsgDataTypes)
        // event 10
        // CableCheckReq (urn:iso:15118:2:2013:MsgBody)
        // event 11
        // CableCheckRes (urn:iso:15118:2:2013:MsgBody)
        // event 12
        // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 13
        // Certificate (urn:iso:15118:2:2013:MsgDataTypes)
        // event 14
        // CertificateInstallationReq (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->CertificateInstallationReq_isUsed == 1)
        {
            // encode event 15
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 15);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_CertificateInstallationReqType(stream, &exiFrag->CertificateInstallationReq);
            }
        }
        // CertificateInstallationRes (urn:iso:15118:2:2013:MsgBody)
        // event 16
        // CertificateUpdateReq (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->CertificateUpdateReq_isUsed == 1)
        {
            // encode event 17
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 17);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_CertificateUpdateReqType(stream, &exiFrag->CertificateUpdateReq);
            }
        }
        // CertificateUpdateRes (urn:iso:15118:2:2013:MsgBody)
        // event 18
        // ChargeParameterDiscoveryReq (urn:iso:15118:2:2013:MsgBody)
        // event 19
        // ChargeParameterDiscoveryRes (urn:iso:15118:2:2013:MsgBody)
        // event 20
        // ChargeProgress (urn:iso:15118:2:2013:MsgBody)
        // event 21
        // ChargeService (urn:iso:15118:2:2013:MsgBody)
        // event 22
        // ChargingComplete (urn:iso:15118:2:2013:MsgBody)
        // event 23
        // ChargingComplete (urn:iso:15118:2:2013:MsgDataTypes)
        // event 24
        // ChargingProfile (urn:iso:15118:2:2013:MsgBody)
        // event 25
        // ChargingProfileEntryMaxNumberOfPhasesInUse (urn:iso:15118:2:2013:MsgDataTypes)
        // event 26
        // ChargingProfileEntryMaxPower (urn:iso:15118:2:2013:MsgDataTypes)
        // event 27
        // ChargingProfileEntryStart (urn:iso:15118:2:2013:MsgDataTypes)
        // event 28
        // ChargingSession (urn:iso:15118:2:2013:MsgBody)
        // event 29
        // ChargingStatusReq (urn:iso:15118:2:2013:MsgBody)
        // event 30
        // ChargingStatusRes (urn:iso:15118:2:2013:MsgBody)
        // event 31
        // ConsumptionCost (urn:iso:15118:2:2013:MsgDataTypes)
        // event 32
        // ContractSignatureCertChain (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->ContractSignatureCertChain_isUsed == 1)
        {
            // encode event 33
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 33);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_CertificateChainType(stream, &exiFrag->ContractSignatureCertChain);
            }
        }
        // ContractSignatureEncryptedPrivateKey (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->ContractSignatureEncryptedPrivateKey_isUsed == 1)
        {
            // encode event 34
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 34);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_ContractSignatureEncryptedPrivateKeyType(stream, &exiFrag->ContractSignatureEncryptedPrivateKey);
            }
        }
        // Cost (urn:iso:15118:2:2013:MsgDataTypes)
        // event 35
        // CurrentDemandReq (urn:iso:15118:2:2013:MsgBody)
        // event 36
        // CurrentDemandRes (urn:iso:15118:2:2013:MsgBody)
        // event 37
        // DC_EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 38
        // DC_EVPowerDeliveryParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 39
        // DC_EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 40
        // DC_EVSEStatus (urn:iso:15118:2:2013:MsgBody)
        // event 41
        // DC_EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 42
        // DC_EVStatus (urn:iso:15118:2:2013:MsgBody)
        // event 43
        // DC_EVStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 44
        // DHpublickey (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->DHpublickey_isUsed == 1)
        {
            // encode event 45
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 45);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_DiffieHellmanPublickeyType(stream, &exiFrag->DHpublickey);
            }
        }
        // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 46
        // DepartureTime (urn:iso:15118:2:2013:MsgDataTypes)
        // event 47
        // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 48
        // DigestValue (http://www.w3.org/2000/09/xmldsig#)
        // event 49
        // EAmount (urn:iso:15118:2:2013:MsgDataTypes)
        // event 50
        // EPriceLevel (urn:iso:15118:2:2013:MsgDataTypes)
        // event 51
        // EVCCID (urn:iso:15118:2:2013:MsgBody)
        // event 52
        // EVChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 53
        // EVEnergyCapacity (urn:iso:15118:2:2013:MsgDataTypes)
        // event 54
        // EVEnergyRequest (urn:iso:15118:2:2013:MsgDataTypes)
        // event 55
        // EVErrorCode (urn:iso:15118:2:2013:MsgDataTypes)
        // event 56
        // EVMaxCurrent (urn:iso:15118:2:2013:MsgDataTypes)
        // event 57
        // EVMaxVoltage (urn:iso:15118:2:2013:MsgDataTypes)
        // event 58
        // EVMaximumCurrentLimit (urn:iso:15118:2:2013:MsgBody)
        // event 59
        // EVMaximumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 60
        // EVMaximumPowerLimit (urn:iso:15118:2:2013:MsgBody)
        // event 61
        // EVMaximumPowerLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 62
        // EVMaximumVoltageLimit (urn:iso:15118:2:2013:MsgBody)
        // event 63
        // EVMaximumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 64
        // EVMinCurrent (urn:iso:15118:2:2013:MsgDataTypes)
        // event 65
        // EVPowerDeliveryParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 66
        // EVRESSSOC (urn:iso:15118:2:2013:MsgDataTypes)
        // event 67
        // EVReady (urn:iso:15118:2:2013:MsgDataTypes)
        // event 68
        // EVSEChargeParameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 69
        // EVSECurrentLimitAchieved (urn:iso:15118:2:2013:MsgBody)
        // event 70
        // EVSECurrentRegulationTolerance (urn:iso:15118:2:2013:MsgDataTypes)
        // event 71
        // EVSEEnergyToBeDelivered (urn:iso:15118:2:2013:MsgDataTypes)
        // event 72
        // EVSEID (urn:iso:15118:2:2013:MsgBody)
        // event 73
        // EVSEIsolationStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 74
        // EVSEMaxCurrent (urn:iso:15118:2:2013:MsgBody)
        // event 75
        // EVSEMaxCurrent (urn:iso:15118:2:2013:MsgDataTypes)
        // event 76
        // EVSEMaximumCurrentLimit (urn:iso:15118:2:2013:MsgBody)
        // event 77
        // EVSEMaximumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 78
        // EVSEMaximumPowerLimit (urn:iso:15118:2:2013:MsgBody)
        // event 79
        // EVSEMaximumPowerLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 80
        // EVSEMaximumVoltageLimit (urn:iso:15118:2:2013:MsgBody)
        // event 81
        // EVSEMaximumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 82
        // EVSEMinimumCurrentLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 83
        // EVSEMinimumVoltageLimit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 84
        // EVSENominalVoltage (urn:iso:15118:2:2013:MsgDataTypes)
        // event 85
        // EVSENotification (urn:iso:15118:2:2013:MsgDataTypes)
        // event 86
        // EVSEPeakCurrentRipple (urn:iso:15118:2:2013:MsgDataTypes)
        // event 87
        // EVSEPowerLimitAchieved (urn:iso:15118:2:2013:MsgBody)
        // event 88
        // EVSEPresentCurrent (urn:iso:15118:2:2013:MsgBody)
        // event 89
        // EVSEPresentVoltage (urn:iso:15118:2:2013:MsgBody)
        // event 90
        // EVSEProcessing (urn:iso:15118:2:2013:MsgBody)
        // event 91
        // EVSEStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 92
        // EVSEStatusCode (urn:iso:15118:2:2013:MsgDataTypes)
        // event 93
        // EVSETimeStamp (urn:iso:15118:2:2013:MsgBody)
        // event 94
        // EVSEVoltageLimitAchieved (urn:iso:15118:2:2013:MsgBody)
        // event 95
        // EVStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 96
        // EVTargetCurrent (urn:iso:15118:2:2013:MsgBody)
        // event 97
        // EVTargetVoltage (urn:iso:15118:2:2013:MsgBody)
        // event 98
        // EnergyTransferMode (urn:iso:15118:2:2013:MsgDataTypes)
        // event 99
        // Entry (urn:iso:15118:2:2013:MsgDataTypes)
        // event 100
        // Exponent (http://www.w3.org/2000/09/xmldsig#)
        // event 101
        // FaultCode (urn:iso:15118:2:2013:MsgDataTypes)
        // event 102
        // FaultMsg (urn:iso:15118:2:2013:MsgDataTypes)
        // event 103
        // FreeService (urn:iso:15118:2:2013:MsgDataTypes)
        // event 104
        // FullSOC (urn:iso:15118:2:2013:MsgDataTypes)
        // event 105
        // G (http://www.w3.org/2000/09/xmldsig#)
        // event 106
        // GenChallenge (urn:iso:15118:2:2013:MsgBody)
        // event 107
        // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
        // event 108
        // Header (urn:iso:15118:2:2013:MsgDef)
        // event 109
        // J (http://www.w3.org/2000/09/xmldsig#)
        // event 110
        // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
        // event 111
        // KeyName (http://www.w3.org/2000/09/xmldsig#)
        // event 112
        // KeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 113
        // ListOfRootCertificateIDs (urn:iso:15118:2:2013:MsgBody)
        // event 114
        // Manifest (http://www.w3.org/2000/09/xmldsig#)
        // event 115
        // MaxEntriesSAScheduleTuple (urn:iso:15118:2:2013:MsgBody)
        // event 116
        // MeterID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 117
        // MeterInfo (urn:iso:15118:2:2013:MsgBody)
        // event 118
        // MeterReading (urn:iso:15118:2:2013:MsgDataTypes)
        // event 119
        // MeterStatus (urn:iso:15118:2:2013:MsgDataTypes)
        // event 120
        // MeteringReceiptReq (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->MeteringReceiptReq_isUsed == 1)
        {
            // encode event 121
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 121);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_MeteringReceiptReqType(stream, &exiFrag->MeteringReceiptReq);
            }
        }
        // MeteringReceiptRes (urn:iso:15118:2:2013:MsgBody)
        // event 122
        // MgmtData (http://www.w3.org/2000/09/xmldsig#)
        // event 123
        // Modulus (http://www.w3.org/2000/09/xmldsig#)
        // event 124
        // Multiplier (urn:iso:15118:2:2013:MsgDataTypes)
        // event 125
        // Notification (urn:iso:15118:2:2013:MsgHeader)
        // event 126
        // NotificationMaxDelay (urn:iso:15118:2:2013:MsgDataTypes)
        // event 127
        // NumEPriceLevels (urn:iso:15118:2:2013:MsgDataTypes)
        // event 128
        // OEMProvisioningCert (urn:iso:15118:2:2013:MsgBody)
        // event 129
        // Object (http://www.w3.org/2000/09/xmldsig#)
        // event 130
        // P (http://www.w3.org/2000/09/xmldsig#)
        // event 131
        // PGPData (http://www.w3.org/2000/09/xmldsig#)
        // event 132
        // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
        // event 133
        // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
        // event 134
        // PMax (urn:iso:15118:2:2013:MsgDataTypes)
        // event 135
        // PMaxSchedule (urn:iso:15118:2:2013:MsgDataTypes)
        // event 136
        // PMaxScheduleEntry (urn:iso:15118:2:2013:MsgDataTypes)
        // event 137
        // Parameter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 138
        // ParameterSet (urn:iso:15118:2:2013:MsgDataTypes)
        // event 139
        // ParameterSetID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 140
        // PaymentDetailsReq (urn:iso:15118:2:2013:MsgBody)
        // event 141
        // PaymentDetailsRes (urn:iso:15118:2:2013:MsgBody)
        // event 142
        // PaymentOption (urn:iso:15118:2:2013:MsgDataTypes)
        // event 143
        // PaymentOptionList (urn:iso:15118:2:2013:MsgBody)
        // event 144
        // PaymentServiceSelectionReq (urn:iso:15118:2:2013:MsgBody)
        // event 145
        // PaymentServiceSelectionRes (urn:iso:15118:2:2013:MsgBody)
        // event 146
        // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
        // event 147
        // PowerDeliveryReq (urn:iso:15118:2:2013:MsgBody)
        // event 148
        // PowerDeliveryRes (urn:iso:15118:2:2013:MsgBody)
        // event 149
        // PreChargeReq (urn:iso:15118:2:2013:MsgBody)
        // event 150
        // PreChargeRes (urn:iso:15118:2:2013:MsgBody)
        // event 151
        // ProfileEntry (urn:iso:15118:2:2013:MsgDataTypes)
        // event 152
        // Q (http://www.w3.org/2000/09/xmldsig#)
        // event 153
        // RCD (urn:iso:15118:2:2013:MsgDataTypes)
        // event 154
        // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 155
        // ReceiptRequired (urn:iso:15118:2:2013:MsgBody)
        // event 156
        // Reference (http://www.w3.org/2000/09/xmldsig#)
        // event 157
        // RelativeTimeInterval (urn:iso:15118:2:2013:MsgDataTypes)
        // event 158
        // RemainingTimeToBulkSoC (urn:iso:15118:2:2013:MsgBody)
        // event 159
        // RemainingTimeToFullSoC (urn:iso:15118:2:2013:MsgBody)
        // event 160
        // RequestedEnergyTransferMode (urn:iso:15118:2:2013:MsgBody)
        // event 161
        // ResponseCode (urn:iso:15118:2:2013:MsgBody)
        // event 162
        // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 163
        // RetryCounter (urn:iso:15118:2:2013:MsgBody)
        // event 164
        // RootCertificateID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 165
        // SAProvisioningCertificateChain (urn:iso:15118:2:2013:MsgBody)
        // event 166
        // SAScheduleList (urn:iso:15118:2:2013:MsgDataTypes)
        // event 167
        // SAScheduleTuple (urn:iso:15118:2:2013:MsgDataTypes)
        // event 168
        // SAScheduleTupleID (urn:iso:15118:2:2013:MsgBody)
        // event 169
        // SAScheduleTupleID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 170
        // SASchedules (urn:iso:15118:2:2013:MsgDataTypes)
        // event 171
        // SPKIData (http://www.w3.org/2000/09/xmldsig#)
        // event 172
        // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
        // event 173
        // SalesTariff (urn:iso:15118:2:2013:MsgDataTypes)
        else if (exiFrag->SalesTariff_isUsed == 1)
        {
            // encode event 174
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 174);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SalesTariffType(stream, &exiFrag->SalesTariff);
            }
        }
        // SalesTariffDescription (urn:iso:15118:2:2013:MsgDataTypes)
        // event 175
        // SalesTariffEntry (urn:iso:15118:2:2013:MsgDataTypes)
        // event 176
        // SalesTariffID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 177
        // Seed (http://www.w3.org/2000/09/xmldsig#)
        // event 178
        // SelectedPaymentOption (urn:iso:15118:2:2013:MsgBody)
        // event 179
        // SelectedService (urn:iso:15118:2:2013:MsgDataTypes)
        // event 180
        // SelectedServiceList (urn:iso:15118:2:2013:MsgBody)
        // event 181
        // Service (urn:iso:15118:2:2013:MsgDataTypes)
        // event 182
        // ServiceCategory (urn:iso:15118:2:2013:MsgBody)
        // event 183
        // ServiceCategory (urn:iso:15118:2:2013:MsgDataTypes)
        // event 184
        // ServiceDetailReq (urn:iso:15118:2:2013:MsgBody)
        // event 185
        // ServiceDetailRes (urn:iso:15118:2:2013:MsgBody)
        // event 186
        // ServiceDiscoveryReq (urn:iso:15118:2:2013:MsgBody)
        // event 187
        // ServiceDiscoveryRes (urn:iso:15118:2:2013:MsgBody)
        // event 188
        // ServiceID (urn:iso:15118:2:2013:MsgBody)
        // event 189
        // ServiceID (urn:iso:15118:2:2013:MsgDataTypes)
        // event 190
        // ServiceList (urn:iso:15118:2:2013:MsgBody)
        // event 191
        // ServiceName (urn:iso:15118:2:2013:MsgDataTypes)
        // event 192
        // ServiceParameterList (urn:iso:15118:2:2013:MsgBody)
        // event 193
        // ServiceScope (urn:iso:15118:2:2013:MsgBody)
        // event 194
        // ServiceScope (urn:iso:15118:2:2013:MsgDataTypes)
        // event 195
        // SessionID (urn:iso:15118:2:2013:MsgBody)
        // event 196
        // SessionID (urn:iso:15118:2:2013:MsgHeader)
        // event 197
        // SessionSetupReq (urn:iso:15118:2:2013:MsgBody)
        // event 198
        // SessionSetupRes (urn:iso:15118:2:2013:MsgBody)
        // event 199
        // SessionStopReq (urn:iso:15118:2:2013:MsgBody)
        // event 200
        // SessionStopRes (urn:iso:15118:2:2013:MsgBody)
        // event 201
        // SigMeterReading (urn:iso:15118:2:2013:MsgDataTypes)
        // event 202
        // Signature (http://www.w3.org/2000/09/xmldsig#)
        // event 203
        // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 204
        // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
        // event 205
        // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
        // event 206
        // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
        // event 207
        // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
        else if (exiFrag->SignedInfo_isUsed == 1)
        {
            // encode event 208
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 208);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SignedInfoType(stream, &exiFrag->SignedInfo);
            }
        }
        // SubCertificates (urn:iso:15118:2:2013:MsgDataTypes)
        // event 209
        // SupportedEnergyTransferMode (urn:iso:15118:2:2013:MsgDataTypes)
        // event 210
        // TMeter (urn:iso:15118:2:2013:MsgDataTypes)
        // event 211
        // TimeInterval (urn:iso:15118:2:2013:MsgDataTypes)
        // event 212
        // Transform (http://www.w3.org/2000/09/xmldsig#)
        // event 213
        // Transforms (http://www.w3.org/2000/09/xmldsig#)
        // event 214
        // Unit (urn:iso:15118:2:2013:MsgDataTypes)
        // event 215
        // V2G_Message (urn:iso:15118:2:2013:MsgDef)
        // event 216
        // Value (urn:iso:15118:2:2013:MsgDataTypes)
        // event 217
        // WeldingDetectionReq (urn:iso:15118:2:2013:MsgBody)
        // event 218
        // WeldingDetectionRes (urn:iso:15118:2:2013:MsgBody)
        // event 219
        // X509CRL (http://www.w3.org/2000/09/xmldsig#)
        // event 220
        // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
        // event 221
        // X509Data (http://www.w3.org/2000/09/xmldsig#)
        // event 222
        // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
        // event 223
        // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
        // event 224
        // X509SKI (http://www.w3.org/2000/09/xmldsig#)
        // event 225
        // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
        // event 226
        // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
        // event 227
        // XPath (http://www.w3.org/2000/09/xmldsig#)
        // event 228
        // Y (http://www.w3.org/2000/09/xmldsig#)
        // event 229
        // amount (urn:iso:15118:2:2013:MsgDataTypes)
        // event 230
        // amountMultiplier (urn:iso:15118:2:2013:MsgDataTypes)
        // event 231
        // boolValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 232
        // byteValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 233
        // costKind (urn:iso:15118:2:2013:MsgDataTypes)
        // event 234
        // duration (urn:iso:15118:2:2013:MsgDataTypes)
        // event 235
        // eMAID (urn:iso:15118:2:2013:MsgBody)
        else if (exiFrag->eMAID_isUsed == 1)
        {
            // encode event 236
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 236);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_EMAIDType(stream, &exiFrag->eMAID);
            }
        }
        // intValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 237
        // physicalValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 238
        // shortValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 239
        // start (urn:iso:15118:2:2013:MsgDataTypes)
        // event 240
        // startValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 241
        // stringValue (urn:iso:15118:2:2013:MsgDataTypes)
        // event 242
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }

        if (error == EXI_ERROR__NO_ERROR)
        {
            // End Fragment
            error = exi_basetypes_encoder_nbit_uint(stream, 8, 244);
        }
    }

    return error;
}

// main function for encoding xmldsig fragment
int encode_iso2_xmldsigFragment(exi_bitstream_t* stream, struct iso2_xmldsigFragment* xmldsigFrag)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
        if (xmldsigFrag->CanonicalizationMethod_isUsed == 1)
        {
            // encode event 0
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_CanonicalizationMethodType(stream, &xmldsigFrag->CanonicalizationMethod);
            }
        }
        // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->DSAKeyValue_isUsed == 1)
        {
            // encode event 1
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_DSAKeyValueType(stream, &xmldsigFrag->DSAKeyValue);
            }
        }
        // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->DigestMethod_isUsed == 1)
        {
            // encode event 2
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_DigestMethodType(stream, &xmldsigFrag->DigestMethod);
            }
        }
        // DigestValue (http://www.w3.org/2000/09/xmldsig#)
        // event 3
        // Exponent (http://www.w3.org/2000/09/xmldsig#)
        // event 4
        // G (http://www.w3.org/2000/09/xmldsig#)
        // event 5
        // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
        // event 6
        // J (http://www.w3.org/2000/09/xmldsig#)
        // event 7
        // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->KeyInfo_isUsed == 1)
        {
            // encode event 8
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 8);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_KeyInfoType(stream, &xmldsigFrag->KeyInfo);
            }
        }
        // KeyName (http://www.w3.org/2000/09/xmldsig#)
        // event 9
        // KeyValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->KeyValue_isUsed == 1)
        {
            // encode event 10
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 10);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_KeyValueType(stream, &xmldsigFrag->KeyValue);
            }
        }
        // Manifest (http://www.w3.org/2000/09/xmldsig#)
        // event 11
        // MgmtData (http://www.w3.org/2000/09/xmldsig#)
        // event 12
        // Modulus (http://www.w3.org/2000/09/xmldsig#)
        // event 13
        // Object (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Object_isUsed == 1)
        {
            // encode event 14
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 14);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_ObjectType(stream, &xmldsigFrag->Object);
            }
        }
        // P (http://www.w3.org/2000/09/xmldsig#)
        // event 15
        // PGPData (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->PGPData_isUsed == 1)
        {
            // encode event 16
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 16);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_PGPDataType(stream, &xmldsigFrag->PGPData);
            }
        }
        // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
        // event 17
        // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
        // event 18
        // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
        // event 19
        // Q (http://www.w3.org/2000/09/xmldsig#)
        // event 20
        // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->RSAKeyValue_isUsed == 1)
        {
            // encode event 21
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 21);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_RSAKeyValueType(stream, &xmldsigFrag->RSAKeyValue);
            }
        }
        // Reference (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Reference_isUsed == 1)
        {
            // encode event 22
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_ReferenceType(stream, &xmldsigFrag->Reference);
            }
        }
        // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->RetrievalMethod_isUsed == 1)
        {
            // encode event 23
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_RetrievalMethodType(stream, &xmldsigFrag->RetrievalMethod);
            }
        }
        // SPKIData (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SPKIData_isUsed == 1)
        {
            // encode event 24
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SPKIDataType(stream, &xmldsigFrag->SPKIData);
            }
        }
        // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
        // event 25
        // Seed (http://www.w3.org/2000/09/xmldsig#)
        // event 26
        // Signature (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Signature_isUsed == 1)
        {
            // encode event 27
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 27);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SignatureType(stream, &xmldsigFrag->Signature);
            }
        }
        // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureMethod_isUsed == 1)
        {
            // encode event 28
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SignatureMethodType(stream, &xmldsigFrag->SignatureMethod);
            }
        }
        // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
        // event 29
        // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
        // event 30
        // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureValue_isUsed == 1)
        {
            // encode event 31
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 31);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SignatureValueType(stream, &xmldsigFrag->SignatureValue);
            }
        }
        // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignedInfo_isUsed == 1)
        {
            // encode event 32
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 32);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_SignedInfoType(stream, &xmldsigFrag->SignedInfo);
            }
        }
        // Transform (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Transform_isUsed == 1)
        {
            // encode event 33
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 33);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_TransformType(stream, &xmldsigFrag->Transform);
            }
        }
        // Transforms (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Transforms_isUsed == 1)
        {
            // encode event 34
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 34);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_TransformsType(stream, &xmldsigFrag->Transforms);
            }
        }
        // X509CRL (http://www.w3.org/2000/09/xmldsig#)
        // event 35
        // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
        // event 36
        // X509Data (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->X509Data_isUsed == 1)
        {
            // encode event 37
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 37);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_X509DataType(stream, &xmldsigFrag->X509Data);
            }
        }
        // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
        // event 38
        // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->X509IssuerSerial_isUsed == 1)
        {
            // encode event 39
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 39);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso2_X509IssuerSerialType(stream, &xmldsigFrag->X509IssuerSerial);
            }
        }
        // X509SKI (http://www.w3.org/2000/09/xmldsig#)
        // event 40
        // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
        // event 41
        // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
        // event 42
        // XPath (http://www.w3.org/2000/09/xmldsig#)
        // event 43
        // Y (http://www.w3.org/2000/09/xmldsig#)
        // event 44
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }

        if (error == EXI_ERROR__NO_ERROR)
        {
            // End Fragment
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 46);
        }
    }

    return error;
}


