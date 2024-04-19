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
  * @file iso20_CommonMessages_Encoder.c
  * @brief Description goes here
  *
  **/
#include <stdint.h>

#include "common/exi_basetypes.h"
#include "common/exi_basetypes_encoder.h"
#include "common/exi_error_codes.h"
#include "common/exi_header.h"
#include "iso20_CommonMessages_Datatypes.h"
#include "iso20_CommonMessages_Encoder.h"



static int encode_iso20_TransformType(exi_bitstream_t* stream, const struct iso20_TransformType* TransformType);
static int encode_iso20_TransformsType(exi_bitstream_t* stream, const struct iso20_TransformsType* TransformsType);
static int encode_iso20_DSAKeyValueType(exi_bitstream_t* stream, const struct iso20_DSAKeyValueType* DSAKeyValueType);
static int encode_iso20_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso20_X509IssuerSerialType* X509IssuerSerialType);
static int encode_iso20_DigestMethodType(exi_bitstream_t* stream, const struct iso20_DigestMethodType* DigestMethodType);
static int encode_iso20_RSAKeyValueType(exi_bitstream_t* stream, const struct iso20_RSAKeyValueType* RSAKeyValueType);
static int encode_iso20_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso20_CanonicalizationMethodType* CanonicalizationMethodType);
static int encode_iso20_PriceLevelScheduleEntryType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleEntryType* PriceLevelScheduleEntryType);
static int encode_iso20_SignatureMethodType(exi_bitstream_t* stream, const struct iso20_SignatureMethodType* SignatureMethodType);
static int encode_iso20_KeyValueType(exi_bitstream_t* stream, const struct iso20_KeyValueType* KeyValueType);
static int encode_iso20_ReferenceType(exi_bitstream_t* stream, const struct iso20_ReferenceType* ReferenceType);
static int encode_iso20_RetrievalMethodType(exi_bitstream_t* stream, const struct iso20_RetrievalMethodType* RetrievalMethodType);
static int encode_iso20_X509DataType(exi_bitstream_t* stream, const struct iso20_X509DataType* X509DataType);
static int encode_iso20_PGPDataType(exi_bitstream_t* stream, const struct iso20_PGPDataType* PGPDataType);
static int encode_iso20_RationalNumberType(exi_bitstream_t* stream, const struct iso20_RationalNumberType* RationalNumberType);
static int encode_iso20_PowerScheduleEntryType(exi_bitstream_t* stream, const struct iso20_PowerScheduleEntryType* PowerScheduleEntryType);
static int encode_iso20_EVPriceRuleType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleType* EVPriceRuleType);
static int encode_iso20_EVPowerScheduleEntryType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleEntryType* EVPowerScheduleEntryType);
static int encode_iso20_EVPriceRuleStackType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleStackType* EVPriceRuleStackType);
static int encode_iso20_PriceRuleType(exi_bitstream_t* stream, const struct iso20_PriceRuleType* PriceRuleType);
static int encode_iso20_PowerScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_PowerScheduleEntryListType* PowerScheduleEntryListType);
static int encode_iso20_TaxRuleType(exi_bitstream_t* stream, const struct iso20_TaxRuleType* TaxRuleType);
static int encode_iso20_PriceRuleStackType(exi_bitstream_t* stream, const struct iso20_PriceRuleStackType* PriceRuleStackType);
static int encode_iso20_AdditionalServiceType(exi_bitstream_t* stream, const struct iso20_AdditionalServiceType* AdditionalServiceType);
static int encode_iso20_PowerScheduleType(exi_bitstream_t* stream, const struct iso20_PowerScheduleType* PowerScheduleType);
static int encode_iso20_EVPowerScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleEntryListType* EVPowerScheduleEntryListType);
static int encode_iso20_OverstayRuleType(exi_bitstream_t* stream, const struct iso20_OverstayRuleType* OverstayRuleType);
static int encode_iso20_EVPriceRuleStackListType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleStackListType* EVPriceRuleStackListType);
static int encode_iso20_SPKIDataType(exi_bitstream_t* stream, const struct iso20_SPKIDataType* SPKIDataType);
static int encode_iso20_SignedInfoType(exi_bitstream_t* stream, const struct iso20_SignedInfoType* SignedInfoType);
static int encode_iso20_EVPowerScheduleType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleType* EVPowerScheduleType);
static int encode_iso20_SignatureValueType(exi_bitstream_t* stream, const struct iso20_SignatureValueType* SignatureValueType);
static int encode_iso20_SubCertificatesType(exi_bitstream_t* stream, const struct iso20_SubCertificatesType* SubCertificatesType);
static int encode_iso20_ParameterType(exi_bitstream_t* stream, const struct iso20_ParameterType* ParameterType);
static int encode_iso20_EVAbsolutePriceScheduleType(exi_bitstream_t* stream, const struct iso20_EVAbsolutePriceScheduleType* EVAbsolutePriceScheduleType);
static int encode_iso20_DetailedCostType(exi_bitstream_t* stream, const struct iso20_DetailedCostType* DetailedCostType);
static int encode_iso20_KeyInfoType(exi_bitstream_t* stream, const struct iso20_KeyInfoType* KeyInfoType);
static int encode_iso20_ObjectType(exi_bitstream_t* stream, const struct iso20_ObjectType* ObjectType);
static int encode_iso20_PriceLevelScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleEntryListType* PriceLevelScheduleEntryListType);
static int encode_iso20_DetailedTaxType(exi_bitstream_t* stream, const struct iso20_DetailedTaxType* DetailedTaxType);
static int encode_iso20_TaxRuleListType(exi_bitstream_t* stream, const struct iso20_TaxRuleListType* TaxRuleListType);
static int encode_iso20_PriceRuleStackListType(exi_bitstream_t* stream, const struct iso20_PriceRuleStackListType* PriceRuleStackListType);
static int encode_iso20_OverstayRuleListType(exi_bitstream_t* stream, const struct iso20_OverstayRuleListType* OverstayRuleListType);
static int encode_iso20_AdditionalServiceListType(exi_bitstream_t* stream, const struct iso20_AdditionalServiceListType* AdditionalServiceListType);
static int encode_iso20_ServiceType(exi_bitstream_t* stream, const struct iso20_ServiceType* ServiceType);
static int encode_iso20_ParameterSetType(exi_bitstream_t* stream, const struct iso20_ParameterSetType* ParameterSetType);
static int encode_iso20_SupportedProvidersListType(exi_bitstream_t* stream, const struct iso20_SupportedProvidersListType* SupportedProvidersListType);
static int encode_iso20_ContractCertificateChainType(exi_bitstream_t* stream, const struct iso20_ContractCertificateChainType* ContractCertificateChainType);
static int encode_iso20_Dynamic_EVPPTControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_EVPPTControlModeType* Dynamic_EVPPTControlModeType);
static int encode_iso20_MeterInfoType(exi_bitstream_t* stream, const struct iso20_MeterInfoType* MeterInfoType);
static int encode_iso20_SignatureType(exi_bitstream_t* stream, const struct iso20_SignatureType* SignatureType);
static int encode_iso20_Scheduled_EVPPTControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_EVPPTControlModeType* Scheduled_EVPPTControlModeType);
static int encode_iso20_ReceiptType(exi_bitstream_t* stream, const struct iso20_ReceiptType* ReceiptType);
static int encode_iso20_AbsolutePriceScheduleType(exi_bitstream_t* stream, const struct iso20_AbsolutePriceScheduleType* AbsolutePriceScheduleType);
static int encode_iso20_EVPowerProfileEntryListType(exi_bitstream_t* stream, const struct iso20_EVPowerProfileEntryListType* EVPowerProfileEntryListType);
static int encode_iso20_Dynamic_SMDTControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SMDTControlModeType* Dynamic_SMDTControlModeType);
static int encode_iso20_EVEnergyOfferType(exi_bitstream_t* stream, const struct iso20_EVEnergyOfferType* EVEnergyOfferType);
static int encode_iso20_PriceLevelScheduleType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleType* PriceLevelScheduleType);
static int encode_iso20_ChargingScheduleType(exi_bitstream_t* stream, const struct iso20_ChargingScheduleType* ChargingScheduleType);
static int encode_iso20_ScheduleTupleType(exi_bitstream_t* stream, const struct iso20_ScheduleTupleType* ScheduleTupleType);
static int encode_iso20_Scheduled_SMDTControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SMDTControlModeType* Scheduled_SMDTControlModeType);
static int encode_iso20_MessageHeaderType(exi_bitstream_t* stream, const struct iso20_MessageHeaderType* MessageHeaderType);
static int encode_iso20_SignaturePropertyType(exi_bitstream_t* stream, const struct iso20_SignaturePropertyType* SignaturePropertyType);
static int encode_iso20_ServiceIDListType(exi_bitstream_t* stream, const struct iso20_ServiceIDListType* ServiceIDListType);
static int encode_iso20_SelectedServiceType(exi_bitstream_t* stream, const struct iso20_SelectedServiceType* SelectedServiceType);
static int encode_iso20_SignedMeteringDataType(exi_bitstream_t* stream, const struct iso20_SignedMeteringDataType* SignedMeteringDataType);
static int encode_iso20_SignedCertificateChainType(exi_bitstream_t* stream, const struct iso20_SignedCertificateChainType* SignedCertificateChainType);
static int encode_iso20_EIM_AReqAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_EIM_AReqAuthorizationModeType* EIM_AReqAuthorizationModeType);
static int encode_iso20_SelectedServiceListType(exi_bitstream_t* stream, const struct iso20_SelectedServiceListType* SelectedServiceListType);
static int encode_iso20_Dynamic_SEReqControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SEReqControlModeType* Dynamic_SEReqControlModeType);
static int encode_iso20_EVSEStatusType(exi_bitstream_t* stream, const struct iso20_EVSEStatusType* EVSEStatusType);
static int encode_iso20_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct iso20_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType);
static int encode_iso20_PnC_AReqAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_PnC_AReqAuthorizationModeType* PnC_AReqAuthorizationModeType);
static int encode_iso20_ServiceListType(exi_bitstream_t* stream, const struct iso20_ServiceListType* ServiceListType);
static int encode_iso20_ServiceParameterListType(exi_bitstream_t* stream, const struct iso20_ServiceParameterListType* ServiceParameterListType);
static int encode_iso20_Scheduled_SEReqControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SEReqControlModeType* Scheduled_SEReqControlModeType);
static int encode_iso20_EVPowerProfileType(exi_bitstream_t* stream, const struct iso20_EVPowerProfileType* EVPowerProfileType);
static int encode_iso20_CertificateChainType(exi_bitstream_t* stream, const struct iso20_CertificateChainType* CertificateChainType);
static int encode_iso20_EIM_ASResAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_EIM_ASResAuthorizationModeType* EIM_ASResAuthorizationModeType);
static int encode_iso20_Dynamic_SEResControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SEResControlModeType* Dynamic_SEResControlModeType);
static int encode_iso20_EMAIDListType(exi_bitstream_t* stream, const struct iso20_EMAIDListType* EMAIDListType);
static int encode_iso20_SignedInstallationDataType(exi_bitstream_t* stream, const struct iso20_SignedInstallationDataType* SignedInstallationDataType);
static int encode_iso20_PnC_ASResAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_PnC_ASResAuthorizationModeType* PnC_ASResAuthorizationModeType);
static int encode_iso20_Scheduled_SEResControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SEResControlModeType* Scheduled_SEResControlModeType);
static int encode_iso20_SessionSetupReqType(exi_bitstream_t* stream, const struct iso20_SessionSetupReqType* SessionSetupReqType);
static int encode_iso20_SessionSetupResType(exi_bitstream_t* stream, const struct iso20_SessionSetupResType* SessionSetupResType);
static int encode_iso20_AuthorizationSetupReqType(exi_bitstream_t* stream, const struct iso20_AuthorizationSetupReqType* AuthorizationSetupReqType);
static int encode_iso20_AuthorizationSetupResType(exi_bitstream_t* stream, const struct iso20_AuthorizationSetupResType* AuthorizationSetupResType);
static int encode_iso20_AuthorizationReqType(exi_bitstream_t* stream, const struct iso20_AuthorizationReqType* AuthorizationReqType);
static int encode_iso20_AuthorizationResType(exi_bitstream_t* stream, const struct iso20_AuthorizationResType* AuthorizationResType);
static int encode_iso20_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct iso20_ServiceDiscoveryReqType* ServiceDiscoveryReqType);
static int encode_iso20_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct iso20_ServiceDiscoveryResType* ServiceDiscoveryResType);
static int encode_iso20_ServiceDetailReqType(exi_bitstream_t* stream, const struct iso20_ServiceDetailReqType* ServiceDetailReqType);
static int encode_iso20_ServiceDetailResType(exi_bitstream_t* stream, const struct iso20_ServiceDetailResType* ServiceDetailResType);
static int encode_iso20_ServiceSelectionReqType(exi_bitstream_t* stream, const struct iso20_ServiceSelectionReqType* ServiceSelectionReqType);
static int encode_iso20_ServiceSelectionResType(exi_bitstream_t* stream, const struct iso20_ServiceSelectionResType* ServiceSelectionResType);
static int encode_iso20_ScheduleExchangeReqType(exi_bitstream_t* stream, const struct iso20_ScheduleExchangeReqType* ScheduleExchangeReqType);
static int encode_iso20_ScheduleExchangeResType(exi_bitstream_t* stream, const struct iso20_ScheduleExchangeResType* ScheduleExchangeResType);
static int encode_iso20_PowerDeliveryReqType(exi_bitstream_t* stream, const struct iso20_PowerDeliveryReqType* PowerDeliveryReqType);
static int encode_iso20_PowerDeliveryResType(exi_bitstream_t* stream, const struct iso20_PowerDeliveryResType* PowerDeliveryResType);
static int encode_iso20_MeteringConfirmationReqType(exi_bitstream_t* stream, const struct iso20_MeteringConfirmationReqType* MeteringConfirmationReqType);
static int encode_iso20_MeteringConfirmationResType(exi_bitstream_t* stream, const struct iso20_MeteringConfirmationResType* MeteringConfirmationResType);
static int encode_iso20_SessionStopReqType(exi_bitstream_t* stream, const struct iso20_SessionStopReqType* SessionStopReqType);
static int encode_iso20_SessionStopResType(exi_bitstream_t* stream, const struct iso20_SessionStopResType* SessionStopResType);
static int encode_iso20_CertificateInstallationReqType(exi_bitstream_t* stream, const struct iso20_CertificateInstallationReqType* CertificateInstallationReqType);
static int encode_iso20_CertificateInstallationResType(exi_bitstream_t* stream, const struct iso20_CertificateInstallationResType* CertificateInstallationResType);
static int encode_iso20_VehicleCheckInReqType(exi_bitstream_t* stream, const struct iso20_VehicleCheckInReqType* VehicleCheckInReqType);
static int encode_iso20_VehicleCheckInResType(exi_bitstream_t* stream, const struct iso20_VehicleCheckInResType* VehicleCheckInResType);
static int encode_iso20_VehicleCheckOutReqType(exi_bitstream_t* stream, const struct iso20_VehicleCheckOutReqType* VehicleCheckOutReqType);
static int encode_iso20_VehicleCheckOutResType(exi_bitstream_t* stream, const struct iso20_VehicleCheckOutResType* VehicleCheckOutResType);
static int encode_iso20_CLReqControlModeType(exi_bitstream_t* stream, const struct iso20_CLReqControlModeType* CLReqControlModeType);
static int encode_iso20_CLResControlModeType(exi_bitstream_t* stream, const struct iso20_CLResControlModeType* CLResControlModeType);
static int encode_iso20_ManifestType(exi_bitstream_t* stream, const struct iso20_ManifestType* ManifestType);
static int encode_iso20_SignaturePropertiesType(exi_bitstream_t* stream, const struct iso20_SignaturePropertiesType* SignaturePropertiesType);

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Transform; type={http://www.w3.org/2000/09/xmldsig#}TransformType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Algorithm, anyURI (1, 1); ANY, anyType (0, 1); XPath, string (0, 1);
static int encode_iso20_TransformType(exi_bitstream_t* stream, const struct iso20_TransformType* TransformType) {
    int grammar_id = 0;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 0:
            // Grammar: ID=0; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=1

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, TransformType->Algorithm.charactersLen, TransformType->Algorithm.characters, iso20_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 1;
                    }
                }
            }
            break;
        case 1:
            // Grammar: ID=1; read/write bits=3; START (XPath), START (ANY), END Element, START (ANY)
            if (TransformType->XPath_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (XPath, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TransformType->XPath.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, TransformType->XPath.charactersLen, TransformType->XPath.characters, iso20_XPath_CHARACTER_SIZE);
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)TransformType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, TransformType->ANY.bytesLen, TransformType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
// Particle: Transform, TransformType (1, 1);
static int encode_iso20_TransformsType(exi_bitstream_t* stream, const struct iso20_TransformsType* TransformsType) {
    int grammar_id = 4;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 4:
            // Grammar: ID=4; read/write bits=1; START (Transform)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (TransformType); next=5
                error = encode_iso20_TransformType(stream, &TransformsType->Transform);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 5;
                }
            }
            break;
        case 5:
            // Grammar: ID=5; read/write bits=2; START (Transform), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transform, TransformType); next=2
                    error = encode_iso20_TransformType(stream, &TransformsType->Transform);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
// Particle: P, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); Q, CryptoBinary (0, 1)(was 1, 1)(seq. ['P', 'Q']); G, CryptoBinary (0, 1); Y, CryptoBinary (1, 1); J, CryptoBinary (0, 1); Seed, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']); PgenCounter, CryptoBinary (0, 1)(was 1, 1)(seq. ['Seed', 'PgenCounter']);
static int encode_iso20_DSAKeyValueType(exi_bitstream_t* stream, const struct iso20_DSAKeyValueType* DSAKeyValueType) {
    int grammar_id = 6;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 6:
            // Grammar: ID=6; read/write bits=2; START (P), START (G), START (Y)
            if (DSAKeyValueType->P_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (P, base64Binary); next=7
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->P.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->P.bytesLen, DSAKeyValueType->P.bytes, iso20_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 7;
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
                    // Event: START (G, base64Binary); next=9
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso20_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 9;
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
                    // Event: START (Y, base64Binary); next=10
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            break;
        case 7:
            // Grammar: ID=7; read/write bits=1; START (Q)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=8
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Q.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Q.bytesLen, DSAKeyValueType->Q.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            }
            break;
        case 8:
            // Grammar: ID=8; read/write bits=2; START (G), START (Y)
            if (DSAKeyValueType->G_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (G, base64Binary); next=9
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->G.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->G.bytesLen, DSAKeyValueType->G.bytes, iso20_CryptoBinary_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 9;
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
                    // Event: START (Y, base64Binary); next=10
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            break;
        case 9:
            // Grammar: ID=9; read/write bits=1; START (Y)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=10
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Y.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Y.bytesLen, DSAKeyValueType->Y.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            break;
        case 10:
            // Grammar: ID=10; read/write bits=2; START (J), START (Seed), END Element
            if (DSAKeyValueType->J_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (J, base64Binary); next=11
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->J.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->J.bytesLen, DSAKeyValueType->J.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            }
            else if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 11:
            // Grammar: ID=11; read/write bits=2; START (Seed), END Element
            if (DSAKeyValueType->Seed_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Seed, base64Binary); next=12
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->Seed.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->Seed.bytesLen, DSAKeyValueType->Seed.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 12:
            // Grammar: ID=12; read/write bits=2; START (PgenCounter), END Element
            if (DSAKeyValueType->PgenCounter_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PgenCounter, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DSAKeyValueType->PgenCounter.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DSAKeyValueType->PgenCounter.bytesLen, DSAKeyValueType->PgenCounter.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_X509IssuerSerialType(exi_bitstream_t* stream, const struct iso20_X509IssuerSerialType* X509IssuerSerialType) {
    int grammar_id = 13;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 13:
            // Grammar: ID=13; read/write bits=1; START (X509IssuerName)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=14

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509IssuerSerialType->X509IssuerName.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, X509IssuerSerialType->X509IssuerName.charactersLen, X509IssuerSerialType->X509IssuerName.characters, iso20_X509IssuerName_CHARACTER_SIZE);
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
            break;
        case 14:
            // Grammar: ID=14; read/write bits=1; START (X509SerialNumber)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (decimal); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_32(stream, X509IssuerSerialType->X509SerialNumber);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_DigestMethodType(exi_bitstream_t* stream, const struct iso20_DigestMethodType* DigestMethodType) {
    int grammar_id = 15;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 15:
            // Grammar: ID=15; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=16

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(DigestMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, DigestMethodType->Algorithm.charactersLen, DigestMethodType->Algorithm.characters, iso20_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 16;
                    }
                }
            }
            break;
        case 16:
            // Grammar: ID=16; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)DigestMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, DigestMethodType->ANY.bytesLen, DigestMethodType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_RSAKeyValueType(exi_bitstream_t* stream, const struct iso20_RSAKeyValueType* RSAKeyValueType) {
    int grammar_id = 17;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 17:
            // Grammar: ID=17; read/write bits=1; START (Modulus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=18
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Modulus.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Modulus.bytesLen, RSAKeyValueType->Modulus.bytes, iso20_CryptoBinary_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 18;
                            }
                        }
                    }
                }
            }
            break;
        case 18:
            // Grammar: ID=18; read/write bits=1; START (Exponent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)RSAKeyValueType->Exponent.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, RSAKeyValueType->Exponent.bytesLen, RSAKeyValueType->Exponent.bytes, iso20_CryptoBinary_BYTES_SIZE);
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
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_CanonicalizationMethodType(exi_bitstream_t* stream, const struct iso20_CanonicalizationMethodType* CanonicalizationMethodType) {
    int grammar_id = 19;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 19:
            // Grammar: ID=19; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=20

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(CanonicalizationMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, CanonicalizationMethodType->Algorithm.charactersLen, CanonicalizationMethodType->Algorithm.characters, iso20_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 20;
                    }
                }
            }
            break;
        case 20:
            // Grammar: ID=20; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CanonicalizationMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, CanonicalizationMethodType->ANY.bytesLen, CanonicalizationMethodType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); PriceLevel, unsignedByte (1, 1);
static int encode_iso20_PriceLevelScheduleEntryType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleEntryType* PriceLevelScheduleEntryType) {
    int grammar_id = 21;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 21:
            // Grammar: ID=21; read/write bits=1; START (Duration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=22
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, PriceLevelScheduleEntryType->Duration);
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
            break;
        case 22:
            // Grammar: ID=22; read/write bits=1; START (PriceLevel)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceLevelScheduleEntryType->PriceLevel);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_SignatureMethodType(exi_bitstream_t* stream, const struct iso20_SignatureMethodType* SignatureMethodType) {
    int grammar_id = 23;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 23:
            // Grammar: ID=23; read/write bits=1; START (Algorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=24

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureMethodType->Algorithm.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignatureMethodType->Algorithm.charactersLen, SignatureMethodType->Algorithm.characters, iso20_Algorithm_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 24;
                    }
                }
            }
            break;
        case 24:
            // Grammar: ID=24; read/write bits=3; START (HMACOutputLength), START (ANY), END Element, START (ANY)
            if (SignatureMethodType->HMACOutputLength_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (HMACOutputLength, integer); next=25
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_32(stream, SignatureMethodType->HMACOutputLength);
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
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
            if (SignatureMethodType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureMethodType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignatureMethodType->ANY.bytesLen, SignatureMethodType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_KeyValueType(exi_bitstream_t* stream, const struct iso20_KeyValueType* KeyValueType) {
    int grammar_id = 26;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 26:
            // Grammar: ID=26; read/write bits=2; START (DSAKeyValue), START (RSAKeyValue), START (ANY)
            if (KeyValueType->DSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DSAKeyValue, DSAKeyValueType); next=2
                    error = encode_iso20_DSAKeyValueType(stream, &KeyValueType->DSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyValueType->RSAKeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RSAKeyValue, RSAKeyValueType); next=2
                    error = encode_iso20_RSAKeyValueType(stream, &KeyValueType->RSAKeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyValueType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyValueType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyValueType->ANY.bytesLen, KeyValueType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_ReferenceType(exi_bitstream_t* stream, const struct iso20_ReferenceType* ReferenceType) {
    int grammar_id = 27;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 27:
            // Grammar: ID=27; read/write bits=3; START (Id), START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=28

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Id.charactersLen, ReferenceType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 28;
                        }
                    }
                }
            }
            else if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=29

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 29;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=30

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 30;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=31
                    error = encode_iso20_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 31;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=32
                    error = encode_iso20_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 32;
                    }
                }
            }
            break;
        case 28:
            // Grammar: ID=28; read/write bits=3; START (Type), START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=29

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->Type.charactersLen, ReferenceType->Type.characters, iso20_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 29;
                        }
                    }
                }
            }
            else if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=30

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 30;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=31
                    error = encode_iso20_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 31;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=32
                    error = encode_iso20_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 32;
                    }
                }
            }
            break;
        case 29:
            // Grammar: ID=29; read/write bits=2; START (URI), START (Transforms), START (DigestMethod)
            if (ReferenceType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=30

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ReferenceType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ReferenceType->URI.charactersLen, ReferenceType->URI.characters, iso20_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 30;
                        }
                    }
                }
            }
            else if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=31
                    error = encode_iso20_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 31;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=32
                    error = encode_iso20_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 32;
                    }
                }
            }
            break;
        case 30:
            // Grammar: ID=30; read/write bits=2; START (Transforms), START (DigestMethod)
            if (ReferenceType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=31
                    error = encode_iso20_TransformsType(stream, &ReferenceType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 31;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DigestMethod, DigestMethodType); next=32
                    error = encode_iso20_DigestMethodType(stream, &ReferenceType->DigestMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 32;
                    }
                }
            }
            break;
        case 31:
            // Grammar: ID=31; read/write bits=1; START (DigestMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (DigestMethodType); next=32
                error = encode_iso20_DigestMethodType(stream, &ReferenceType->DigestMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 32;
                }
            }
            break;
        case 32:
            // Grammar: ID=32; read/write bits=1; START (DigestValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ReferenceType->DigestValue.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, ReferenceType->DigestValue.bytesLen, ReferenceType->DigestValue.bytes, iso20_DigestValueType_BYTES_SIZE);
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
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_RetrievalMethodType(exi_bitstream_t* stream, const struct iso20_RetrievalMethodType* RetrievalMethodType) {
    int grammar_id = 33;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 33:
            // Grammar: ID=33; read/write bits=3; START (Type), START (URI), START (Transforms), END Element
            if (RetrievalMethodType->Type_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Type, anyURI); next=34

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->Type.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->Type.charactersLen, RetrievalMethodType->Type.characters, iso20_Type_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 34;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=35

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 35;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 34:
            // Grammar: ID=34; read/write bits=2; START (URI), START (Transforms), END Element
            if (RetrievalMethodType->URI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (URI, anyURI); next=35

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(RetrievalMethodType->URI.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, RetrievalMethodType->URI.charactersLen, RetrievalMethodType->URI.characters, iso20_URI_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 35;
                        }
                    }
                }
            }
            else if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 35:
            // Grammar: ID=35; read/write bits=2; START (Transforms), END Element
            if (RetrievalMethodType->Transforms_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Transforms, TransformsType); next=2
                    error = encode_iso20_TransformsType(stream, &RetrievalMethodType->Transforms);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_X509DataType(exi_bitstream_t* stream, const struct iso20_X509DataType* X509DataType) {
    int grammar_id = 36;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 36:
            // Grammar: ID=36; read/write bits=3; START (X509IssuerSerial), START (X509SKI), START (X509SubjectName), START (X509Certificate), START (X509CRL), START (ANY)
            if (X509DataType->X509IssuerSerial_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerial, X509IssuerSerialType); next=2
                    error = encode_iso20_X509IssuerSerialType(stream, &X509DataType->X509IssuerSerial);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (X509DataType->X509SKI_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SKI, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509SKI.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509SKI.bytesLen, X509DataType->X509SKI.bytes, iso20_base64Binary_BYTES_SIZE);
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
                }
            }
            else if (X509DataType->X509SubjectName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509SubjectName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(X509DataType->X509SubjectName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, X509DataType->X509SubjectName.charactersLen, X509DataType->X509SubjectName.characters, iso20_X509SubjectName_CHARACTER_SIZE);
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
                }
            }
            else if (X509DataType->X509Certificate_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Certificate, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509Certificate.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509Certificate.bytesLen, X509DataType->X509Certificate.bytes, iso20_base64Binary_BYTES_SIZE);
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
                }
            }
            else if (X509DataType->X509CRL_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509CRL, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->X509CRL.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->X509CRL.bytesLen, X509DataType->X509CRL.bytes, iso20_base64Binary_BYTES_SIZE);
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
                }
            }
            else if (X509DataType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)X509DataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, X509DataType->ANY.bytesLen, X509DataType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_PGPDataType(exi_bitstream_t* stream, const struct iso20_PGPDataType* PGPDataType) {
    int grammar_id = 37;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 37:
            // Grammar: ID=37; read/write bits=2; START (PGPKeyID), START (PGPKeyPacket)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyID, base64Binary); next=38
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyID.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyID.bytesLen, PGPDataType->choice_1.PGPKeyID.bytes, iso20_base64Binary_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=39
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso20_base64Binary_BYTES_SIZE);
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
            break;
        case 38:
            // Grammar: ID=38; read/write bits=3; START (PGPKeyPacket), START (ANY), END Element, START (ANY)
            if (PGPDataType->choice_1_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPKeyPacket, base64Binary); next=39
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.PGPKeyPacket.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.PGPKeyPacket.bytesLen, PGPDataType->choice_1.PGPKeyPacket.bytes, iso20_base64Binary_BYTES_SIZE);
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
                    // Event: START (ANY, base64Binary); next=40
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso20_anyType_BYTES_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 39:
            // Grammar: ID=39; read/write bits=3; START (ANY), END Element, END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=40
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_1.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_1.ANY.bytesLen, PGPDataType->choice_1.ANY.bytes, iso20_anyType_BYTES_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 40:
            // Grammar: ID=40; read/write bits=1; START (PGPKeyPacket)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=41
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.PGPKeyPacket.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.PGPKeyPacket.bytesLen, PGPDataType->choice_2.PGPKeyPacket.bytes, iso20_base64Binary_BYTES_SIZE);
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
            break;
        case 41:
            // Grammar: ID=41; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=40
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PGPDataType->choice_2.ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, PGPDataType->choice_2.ANY.bytesLen, PGPDataType->choice_2.ANY.bytes, iso20_anyType_BYTES_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}rationalNumber; type={urn:iso:std:iso:15118:-20:CommonTypes}RationalNumberType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Exponent, byte (1, 1); Value, short (1, 1);
static int encode_iso20_RationalNumberType(exi_bitstream_t* stream, const struct iso20_RationalNumberType* RationalNumberType) {
    int grammar_id = 42;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 42:
            // Grammar: ID=42; read/write bits=1; START (Exponent)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (short); next=43
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // type has min_value = -128
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, RationalNumberType->Exponent + -128);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 43;
                        }
                    }
                }
            }
            break;
        case 43:
            // Grammar: ID=43; read/write bits=1; START (Value)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (int); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_integer_16(stream, RationalNumberType->Value);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); Power, RationalNumberType (1, 1); Power_L2, RationalNumberType (0, 1); Power_L3, RationalNumberType (0, 1);
static int encode_iso20_PowerScheduleEntryType(exi_bitstream_t* stream, const struct iso20_PowerScheduleEntryType* PowerScheduleEntryType) {
    int grammar_id = 44;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 44:
            // Grammar: ID=44; read/write bits=1; START (Duration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=45
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, PowerScheduleEntryType->Duration);
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
            // Grammar: ID=45; read/write bits=1; START (Power)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=46
                error = encode_iso20_RationalNumberType(stream, &PowerScheduleEntryType->Power);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 46;
                }
            }
            break;
        case 46:
            // Grammar: ID=46; read/write bits=2; START (Power_L2), START (Power_L3), END Element
            if (PowerScheduleEntryType->Power_L2_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Power_L2, RationalNumberType); next=47
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleEntryType->Power_L2);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 47;
                    }
                }
            }
            else if (PowerScheduleEntryType->Power_L3_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Power_L3, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleEntryType->Power_L3);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 47:
            // Grammar: ID=47; read/write bits=2; START (Power_L3), END Element
            if (PowerScheduleEntryType->Power_L3_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Power_L3, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleEntryType->Power_L3);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyFee, RationalNumberType (1, 1); PowerRangeStart, RationalNumberType (1, 1);
static int encode_iso20_EVPriceRuleType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleType* EVPriceRuleType) {
    int grammar_id = 48;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 48:
            // Grammar: ID=48; read/write bits=1; START (EnergyFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=49
                error = encode_iso20_RationalNumberType(stream, &EVPriceRuleType->EnergyFee);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 49;
                }
            }
            break;
        case 49:
            // Grammar: ID=49; read/write bits=1; START (PowerRangeStart)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &EVPriceRuleType->PowerRangeStart);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntry; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntryType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); Power, RationalNumberType (1, 1);
static int encode_iso20_EVPowerScheduleEntryType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleEntryType* EVPowerScheduleEntryType) {
    int grammar_id = 50;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 50:
            // Grammar: ID=50; read/write bits=1; START (Duration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=51
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, EVPowerScheduleEntryType->Duration);
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
            // Grammar: ID=51; read/write bits=1; START (Power)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &EVPowerScheduleEntryType->Power);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStack; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStackType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); EVPriceRule, EVPriceRuleType (1, 8);
static int encode_iso20_EVPriceRuleStackType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleStackType* EVPriceRuleStackType) {
    int grammar_id = 52;
    int done = 0;
    int error = 0;
    uint16_t EVPriceRule_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 52:
            // Grammar: ID=52; read/write bits=1; START (Duration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=53
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, EVPriceRuleStackType->Duration);
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
            // Grammar: ID=53; read/write bits=1; START (EVPriceRule)
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=54
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 54;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 54:
            // Grammar: ID=54; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=55
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 55;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 55:
            // Grammar: ID=55; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=56
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 56;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 56:
            // Grammar: ID=56; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=57
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 57;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 57:
            // Grammar: ID=57; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=58
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 58;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 58:
            // Grammar: ID=58; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=59
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 59:
            // Grammar: ID=59; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=60
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 60;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 60:
            // Grammar: ID=60; read/write bits=2; START (EVPriceRule), END Element
            if (EVPriceRule_currentIndex < EVPriceRuleStackType->EVPriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleType); next=2
                    error = encode_iso20_EVPriceRuleType(stream, &EVPriceRuleStackType->EVPriceRule.array[EVPriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRule; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EnergyFee, RationalNumberType (1, 1); ParkingFee, RationalNumberType (0, 1); ParkingFeePeriod, unsignedInt (0, 1); CarbonDioxideEmission, unsignedShort (0, 1); RenewableGenerationPercentage, unsignedByte (0, 1); PowerRangeStart, RationalNumberType (1, 1);
static int encode_iso20_PriceRuleType(exi_bitstream_t* stream, const struct iso20_PriceRuleType* PriceRuleType) {
    int grammar_id = 61;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 61:
            // Grammar: ID=61; read/write bits=1; START (EnergyFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=62
                error = encode_iso20_RationalNumberType(stream, &PriceRuleType->EnergyFee);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 62;
                }
            }
            break;
        case 62:
            // Grammar: ID=62; read/write bits=3; START (ParkingFee), START (ParkingFeePeriod), START (CarbonDioxideEmission), START (RenewableGenerationPercentage), START (PowerRangeStart)
            if (PriceRuleType->ParkingFee_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParkingFee, RationalNumberType); next=63
                    error = encode_iso20_RationalNumberType(stream, &PriceRuleType->ParkingFee);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 63;
                    }
                }
            }
            else if (PriceRuleType->ParkingFeePeriod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParkingFeePeriod, unsignedLong); next=64
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, PriceRuleType->ParkingFeePeriod);
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
            else if (PriceRuleType->CarbonDioxideEmission_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CarbonDioxideEmission, unsignedInt); next=65
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, PriceRuleType->CarbonDioxideEmission);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 65;
                            }
                        }
                    }
                }
            }
            else if (PriceRuleType->RenewableGenerationPercentage_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RenewableGenerationPercentage, unsignedShort); next=66
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceRuleType->RenewableGenerationPercentage);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerRangeStart, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PriceRuleType->PowerRangeStart);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 63:
            // Grammar: ID=63; read/write bits=3; START (ParkingFeePeriod), START (CarbonDioxideEmission), START (RenewableGenerationPercentage), START (PowerRangeStart)
            if (PriceRuleType->ParkingFeePeriod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParkingFeePeriod, unsignedLong); next=64
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, PriceRuleType->ParkingFeePeriod);
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
            else if (PriceRuleType->CarbonDioxideEmission_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CarbonDioxideEmission, unsignedInt); next=65
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, PriceRuleType->CarbonDioxideEmission);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 65;
                            }
                        }
                    }
                }
            }
            else if (PriceRuleType->RenewableGenerationPercentage_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RenewableGenerationPercentage, unsignedShort); next=66
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceRuleType->RenewableGenerationPercentage);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerRangeStart, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PriceRuleType->PowerRangeStart);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 64:
            // Grammar: ID=64; read/write bits=2; START (CarbonDioxideEmission), START (RenewableGenerationPercentage), START (PowerRangeStart)
            if (PriceRuleType->CarbonDioxideEmission_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CarbonDioxideEmission, unsignedInt); next=65
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, PriceRuleType->CarbonDioxideEmission);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 65;
                            }
                        }
                    }
                }
            }
            else if (PriceRuleType->RenewableGenerationPercentage_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RenewableGenerationPercentage, unsignedShort); next=66
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceRuleType->RenewableGenerationPercentage);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerRangeStart, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PriceRuleType->PowerRangeStart);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 65:
            // Grammar: ID=65; read/write bits=2; START (RenewableGenerationPercentage), START (PowerRangeStart)
            if (PriceRuleType->RenewableGenerationPercentage_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RenewableGenerationPercentage, unsignedShort); next=66
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceRuleType->RenewableGenerationPercentage);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerRangeStart, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &PriceRuleType->PowerRangeStart);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 66:
            // Grammar: ID=66; read/write bits=1; START (PowerRangeStart)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &PriceRuleType->PowerRangeStart);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PowerScheduleEntry, PowerScheduleEntryType (1, 1024);
static int encode_iso20_PowerScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_PowerScheduleEntryListType* PowerScheduleEntryListType) {
    int grammar_id = 67;
    int done = 0;
    int error = 0;
    uint16_t PowerScheduleEntry_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 67:
            // Grammar: ID=67; read/write bits=1; START (PowerScheduleEntry)
            if (PowerScheduleEntry_currentIndex < PowerScheduleEntryListType->PowerScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerScheduleEntryType); next=68
                    error = encode_iso20_PowerScheduleEntryType(stream, &PowerScheduleEntryListType->PowerScheduleEntry.array[PowerScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 68;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 68:
            // Grammar: ID=68; read/write bits=2; LOOP (PowerScheduleEntry), END Element
            if (PowerScheduleEntry_currentIndex < PowerScheduleEntryListType->PowerScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (PowerScheduleEntryType); next=3
                    error = encode_iso20_PowerScheduleEntryType(stream, &PowerScheduleEntryListType->PowerScheduleEntry.array[PowerScheduleEntry_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}TaxRule; type={urn:iso:std:iso:15118:-20:CommonMessages}TaxRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); TaxRuleName, nameType (0, 1); TaxRate, RationalNumberType (1, 1); TaxIncludedInPrice, boolean (0, 1); AppliesToEnergyFee, boolean (1, 1); AppliesToParkingFee, boolean (1, 1); AppliesToOverstayFee, boolean (1, 1); AppliesMinimumMaximumCost, boolean (1, 1);
static int encode_iso20_TaxRuleType(exi_bitstream_t* stream, const struct iso20_TaxRuleType* TaxRuleType) {
    int grammar_id = 69;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 69:
            // Grammar: ID=69; read/write bits=1; START (TaxRuleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=70
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, TaxRuleType->TaxRuleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 70;
                        }
                    }
                }
            }
            break;
        case 70:
            // Grammar: ID=70; read/write bits=2; START (TaxRuleName), START (TaxRate)
            if (TaxRuleType->TaxRuleName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleName, string); next=71

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(TaxRuleType->TaxRuleName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, TaxRuleType->TaxRuleName.charactersLen, TaxRuleType->TaxRuleName.characters, iso20_TaxRuleName_CHARACTER_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRate, RationalNumberType); next=72
                    error = encode_iso20_RationalNumberType(stream, &TaxRuleType->TaxRate);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 72;
                    }
                }
            }
            break;
        case 71:
            // Grammar: ID=71; read/write bits=1; START (TaxRate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=72
                error = encode_iso20_RationalNumberType(stream, &TaxRuleType->TaxRate);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 72;
                }
            }
            break;
        case 72:
            // Grammar: ID=72; read/write bits=2; START (TaxIncludedInPrice), START (AppliesToEnergyFee)
            if (TaxRuleType->TaxIncludedInPrice_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxIncludedInPrice, boolean); next=73
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, TaxRuleType->TaxIncludedInPrice);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 73;
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
                    // Event: START (AppliesToEnergyFee, boolean); next=74
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, TaxRuleType->AppliesToEnergyFee);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 74;
                            }
                        }
                    }
                }
            }
            break;
        case 73:
            // Grammar: ID=73; read/write bits=1; START (AppliesToEnergyFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=74
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, TaxRuleType->AppliesToEnergyFee);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 74;
                        }
                    }
                }
            }
            break;
        case 74:
            // Grammar: ID=74; read/write bits=1; START (AppliesToParkingFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=75
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, TaxRuleType->AppliesToParkingFee);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 75;
                        }
                    }
                }
            }
            break;
        case 75:
            // Grammar: ID=75; read/write bits=1; START (AppliesToOverstayFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=76
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, TaxRuleType->AppliesToOverstayFee);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 76;
                        }
                    }
                }
            }
            break;
        case 76:
            // Grammar: ID=76; read/write bits=1; START (AppliesMinimumMaximumCost)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, TaxRuleType->AppliesMinimumMaximumCost);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStack; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStackType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Duration, unsignedInt (1, 1); PriceRule, PriceRuleType (1, 8);
static int encode_iso20_PriceRuleStackType(exi_bitstream_t* stream, const struct iso20_PriceRuleStackType* PriceRuleStackType) {
    int grammar_id = 77;
    int done = 0;
    int error = 0;
    uint16_t PriceRule_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 77:
            // Grammar: ID=77; read/write bits=1; START (Duration)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=78
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, PriceRuleStackType->Duration);
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
            // Grammar: ID=78; read/write bits=1; START (PriceRule)
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=79
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 79;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 79:
            // Grammar: ID=79; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=80
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 80;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 80:
            // Grammar: ID=80; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=81
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 81;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 81:
            // Grammar: ID=81; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=82
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 82;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 82:
            // Grammar: ID=82; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=83
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 83;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 83:
            // Grammar: ID=83; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=84
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 84;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 84:
            // Grammar: ID=84; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=85
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 85:
            // Grammar: ID=85; read/write bits=2; START (PriceRule), END Element
            if (PriceRule_currentIndex < PriceRuleStackType->PriceRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleType); next=2
                    error = encode_iso20_PriceRuleType(stream, &PriceRuleStackType->PriceRule.array[PriceRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalService; type={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceName, nameType (1, 1); ServiceFee, RationalNumberType (1, 1);
static int encode_iso20_AdditionalServiceType(exi_bitstream_t* stream, const struct iso20_AdditionalServiceType* AdditionalServiceType) {
    int grammar_id = 86;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 86:
            // Grammar: ID=86; read/write bits=1; START (ServiceName)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=87

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AdditionalServiceType->ServiceName.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AdditionalServiceType->ServiceName.charactersLen, AdditionalServiceType->ServiceName.characters, iso20_ServiceName_CHARACTER_SIZE);
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
            }
            break;
        case 87:
            // Grammar: ID=87; read/write bits=1; START (ServiceFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &AdditionalServiceType->ServiceFee);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); AvailableEnergy, RationalNumberType (0, 1); PowerTolerance, RationalNumberType (0, 1); PowerScheduleEntries, PowerScheduleEntryListType (1, 1);
static int encode_iso20_PowerScheduleType(exi_bitstream_t* stream, const struct iso20_PowerScheduleType* PowerScheduleType) {
    int grammar_id = 88;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 88:
            // Grammar: ID=88; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=89
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, PowerScheduleType->TimeAnchor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 89;
                        }
                    }
                }
            }
            break;
        case 89:
            // Grammar: ID=89; read/write bits=2; START (AvailableEnergy), START (PowerTolerance), START (PowerScheduleEntries)
            if (PowerScheduleType->AvailableEnergy_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AvailableEnergy, RationalNumberType); next=90
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleType->AvailableEnergy);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 90;
                    }
                }
            }
            else if (PowerScheduleType->PowerTolerance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerTolerance, RationalNumberType); next=91
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleType->PowerTolerance);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 91;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerScheduleEntries, PowerScheduleEntryListType); next=2
                    error = encode_iso20_PowerScheduleEntryListType(stream, &PowerScheduleType->PowerScheduleEntries);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 90:
            // Grammar: ID=90; read/write bits=2; START (PowerTolerance), START (PowerScheduleEntries)
            if (PowerScheduleType->PowerTolerance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerTolerance, RationalNumberType); next=91
                    error = encode_iso20_RationalNumberType(stream, &PowerScheduleType->PowerTolerance);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 91;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerScheduleEntries, PowerScheduleEntryListType); next=2
                    error = encode_iso20_PowerScheduleEntryListType(stream, &PowerScheduleType->PowerScheduleEntries);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 91:
            // Grammar: ID=91; read/write bits=1; START (PowerScheduleEntries)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PowerScheduleEntryListType); next=2
                error = encode_iso20_PowerScheduleEntryListType(stream, &PowerScheduleType->PowerScheduleEntries);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerScheduleEntry, EVPowerScheduleEntryType (1, 1024);
static int encode_iso20_EVPowerScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleEntryListType* EVPowerScheduleEntryListType) {
    int grammar_id = 92;
    int done = 0;
    int error = 0;
    uint16_t EVPowerScheduleEntry_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 92:
            // Grammar: ID=92; read/write bits=1; START (EVPowerScheduleEntry)
            if (EVPowerScheduleEntry_currentIndex < EVPowerScheduleEntryListType->EVPowerScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPowerScheduleEntryType); next=93
                    error = encode_iso20_EVPowerScheduleEntryType(stream, &EVPowerScheduleEntryListType->EVPowerScheduleEntry.array[EVPowerScheduleEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 93;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 93:
            // Grammar: ID=93; read/write bits=2; LOOP (EVPowerScheduleEntry), END Element
            if (EVPowerScheduleEntry_currentIndex < EVPowerScheduleEntryListType->EVPowerScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EVPowerScheduleEntryType); next=3
                    error = encode_iso20_EVPowerScheduleEntryType(stream, &EVPowerScheduleEntryListType->EVPowerScheduleEntry.array[EVPowerScheduleEntry_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRule; type={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRuleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OverstayRuleDescription, descriptionType (0, 1); StartTime, unsignedInt (1, 1); OverstayFee, RationalNumberType (1, 1); OverstayFeePeriod, unsignedInt (1, 1);
static int encode_iso20_OverstayRuleType(exi_bitstream_t* stream, const struct iso20_OverstayRuleType* OverstayRuleType) {
    int grammar_id = 94;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 94:
            // Grammar: ID=94; read/write bits=2; START (OverstayRuleDescription), START (StartTime)
            if (OverstayRuleType->OverstayRuleDescription_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleDescription, string); next=95

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(OverstayRuleType->OverstayRuleDescription.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, OverstayRuleType->OverstayRuleDescription.charactersLen, OverstayRuleType->OverstayRuleDescription.characters, iso20_OverstayRuleDescription_CHARACTER_SIZE);
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
                    // Event: START (StartTime, unsignedLong); next=96
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, OverstayRuleType->StartTime);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 96;
                            }
                        }
                    }
                }
            }
            break;
        case 95:
            // Grammar: ID=95; read/write bits=1; START (StartTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=96
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, OverstayRuleType->StartTime);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 96;
                        }
                    }
                }
            }
            break;
        case 96:
            // Grammar: ID=96; read/write bits=1; START (OverstayFee)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=97
                error = encode_iso20_RationalNumberType(stream, &OverstayRuleType->OverstayFee);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 97;
                }
            }
            break;
        case 97:
            // Grammar: ID=97; read/write bits=1; START (OverstayFeePeriod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, OverstayRuleType->OverstayFeePeriod);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStacks; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPriceRuleStackListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPriceRuleStack, EVPriceRuleStackType (1, 1024);
static int encode_iso20_EVPriceRuleStackListType(exi_bitstream_t* stream, const struct iso20_EVPriceRuleStackListType* EVPriceRuleStackListType) {
    int grammar_id = 98;
    int done = 0;
    int error = 0;
    uint16_t EVPriceRuleStack_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 98:
            // Grammar: ID=98; read/write bits=1; START (EVPriceRuleStack)
            if (EVPriceRuleStack_currentIndex < EVPriceRuleStackListType->EVPriceRuleStack.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPriceRuleStackType); next=99
                    error = encode_iso20_EVPriceRuleStackType(stream, &EVPriceRuleStackListType->EVPriceRuleStack.array[EVPriceRuleStack_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 99;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 99:
            // Grammar: ID=99; read/write bits=2; LOOP (EVPriceRuleStack), END Element
            if (EVPriceRuleStack_currentIndex < EVPriceRuleStackListType->EVPriceRuleStack.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (EVPriceRuleStackType); next=3
                    error = encode_iso20_EVPriceRuleStackType(stream, &EVPriceRuleStackListType->EVPriceRuleStack.array[EVPriceRuleStack_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_SPKIDataType(exi_bitstream_t* stream, const struct iso20_SPKIDataType* SPKIDataType) {
    int grammar_id = 100;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 100:
            // Grammar: ID=100; read/write bits=1; START (SPKISexp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=101
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->SPKISexp.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SPKIDataType->SPKISexp.bytesLen, SPKIDataType->SPKISexp.bytes, iso20_base64Binary_BYTES_SIZE);
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
            }
            break;
        case 101:
            // Grammar: ID=101; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SPKIDataType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SPKIDataType->ANY.bytesLen, SPKIDataType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
// Particle: Id, ID (0, 1); CanonicalizationMethod, CanonicalizationMethodType (1, 1); SignatureMethod, SignatureMethodType (1, 1); Reference, ReferenceType (1, 4);
static int encode_iso20_SignedInfoType(exi_bitstream_t* stream, const struct iso20_SignedInfoType* SignedInfoType) {
    int grammar_id = 102;
    int done = 0;
    int error = 0;
    uint16_t Reference_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 102:
            // Grammar: ID=102; read/write bits=2; START (Id), START (CanonicalizationMethod)
            if (SignedInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=103

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignedInfoType->Id.charactersLen, SignedInfoType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 103;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CanonicalizationMethod, CanonicalizationMethodType); next=104
                    error = encode_iso20_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 104;
                    }
                }
            }
            break;
        case 103:
            // Grammar: ID=103; read/write bits=1; START (CanonicalizationMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CanonicalizationMethodType); next=104
                error = encode_iso20_CanonicalizationMethodType(stream, &SignedInfoType->CanonicalizationMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 104;
                }
            }
            break;
        case 104:
            // Grammar: ID=104; read/write bits=1; START (SignatureMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignatureMethodType); next=105
                error = encode_iso20_SignatureMethodType(stream, &SignedInfoType->SignatureMethod);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 105;
                }
            }
            break;
        case 105:
            // Grammar: ID=105; read/write bits=1; START (Reference)
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=106
                    error = encode_iso20_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 106;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 106:
            // Grammar: ID=106; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=107
                    error = encode_iso20_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 107;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 107:
            // Grammar: ID=107; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=108
                    error = encode_iso20_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 108;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 108:
            // Grammar: ID=108; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=109
                    error = encode_iso20_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 109;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 109:
            // Grammar: ID=109; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < SignedInfoType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=2
                    error = encode_iso20_ReferenceType(stream, &SignedInfoType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EVPowerScheduleEntries, EVPowerScheduleEntryListType (1, 1);
static int encode_iso20_EVPowerScheduleType(exi_bitstream_t* stream, const struct iso20_EVPowerScheduleType* EVPowerScheduleType) {
    int grammar_id = 110;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 110:
            // Grammar: ID=110; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=111
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, EVPowerScheduleType->TimeAnchor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 111;
                        }
                    }
                }
            }
            break;
        case 111:
            // Grammar: ID=111; read/write bits=1; START (EVPowerScheduleEntries)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVPowerScheduleEntryListType); next=2
                error = encode_iso20_EVPowerScheduleEntryListType(stream, &EVPowerScheduleType->EVPowerScheduleEntries);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_SignatureValueType(exi_bitstream_t* stream, const struct iso20_SignatureValueType* SignatureValueType) {
    int grammar_id = 112;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 112:
            // Grammar: ID=112; read/write bits=2; START (Id), START (CONTENT)
            if (SignatureValueType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=113

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureValueType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureValueType->Id.charactersLen, SignatureValueType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 113;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CONTENT, base64Binary); next=2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso20_SignatureValueType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 113:
            // Grammar: ID=113; read/write bits=1; START (CONTENT)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignatureValueType->CONTENT.bytesLen);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bytes(stream, SignatureValueType->CONTENT.bytesLen, SignatureValueType->CONTENT.bytes, iso20_SignatureValueType_BYTES_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SubCertificates; type={urn:iso:std:iso:15118:-20:CommonMessages}SubCertificatesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 3);
static int encode_iso20_SubCertificatesType(exi_bitstream_t* stream, const struct iso20_SubCertificatesType* SubCertificatesType) {
    int grammar_id = 114;
    int done = 0;
    int error = 0;
    uint16_t Certificate_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 114:
            // Grammar: ID=114; read/write bits=1; START (Certificate)
            if (Certificate_currentIndex < SubCertificatesType->Certificate.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (base64Binary); next=115
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytes, iso20_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                Certificate_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 115;
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
        case 115:
            // Grammar: ID=115; read/write bits=2; START (Certificate), END Element
            if (Certificate_currentIndex < SubCertificatesType->Certificate.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (base64Binary); next=116
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytes, iso20_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                Certificate_currentIndex++;
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 116:
            // Grammar: ID=116; read/write bits=2; START (Certificate), END Element
            if (Certificate_currentIndex < SubCertificatesType->Certificate.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytesLen, SubCertificatesType->Certificate.array[Certificate_currentIndex].bytes, iso20_certificateType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                Certificate_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Parameter; type={urn:iso:std:iso:15118:-20:CommonMessages}ParameterType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False; choice=True;
// Particle: Name, nameType (1, 1); boolValue, boolean (0, 1); byteValue, byte (0, 1); shortValue, short (0, 1); intValue, int (0, 1); rationalNumber, RationalNumberType (0, 1); finiteString, nameType (0, 1);
static int encode_iso20_ParameterType(exi_bitstream_t* stream, const struct iso20_ParameterType* ParameterType) {
    int grammar_id = 117;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 117:
            // Grammar: ID=117; read/write bits=1; START (Name)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=118

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ParameterType->Name.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, ParameterType->Name.charactersLen, ParameterType->Name.characters, iso20_Name_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 118;
                    }
                }
            }
            break;
        case 118:
            // Grammar: ID=118; read/write bits=3; START (boolValue), START (byteValue), START (shortValue), START (intValue), START (rationalNumber), START (finiteString)
            if (ParameterType->boolValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (boolValue, boolean); next=2
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
                                grammar_id = 2;
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
                    // Event: START (byteValue, short); next=2
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
                                grammar_id = 2;
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
                    // Event: START (shortValue, int); next=2
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
                                grammar_id = 2;
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
                    // Event: START (intValue, long); next=2
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
                                grammar_id = 2;
                            }
                        }
                    }
                }
            }
            else if (ParameterType->rationalNumber_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (rationalNumber, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &ParameterType->rationalNumber);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (finiteString, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ParameterType->finiteString.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, ParameterType->finiteString.charactersLen, ParameterType->finiteString.characters, iso20_finiteString_CHARACTER_SIZE);
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
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVAbsolutePriceSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}EVAbsolutePriceScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); Currency, currencyType (1, 1); PriceAlgorithm, identifierType (1, 1); EVPriceRuleStacks, EVPriceRuleStackListType (1, 1);
static int encode_iso20_EVAbsolutePriceScheduleType(exi_bitstream_t* stream, const struct iso20_EVAbsolutePriceScheduleType* EVAbsolutePriceScheduleType) {
    int grammar_id = 119;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 119:
            // Grammar: ID=119; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=120
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, EVAbsolutePriceScheduleType->TimeAnchor);
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
            break;
        case 120:
            // Grammar: ID=120; read/write bits=1; START (Currency)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=121

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EVAbsolutePriceScheduleType->Currency.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, EVAbsolutePriceScheduleType->Currency.charactersLen, EVAbsolutePriceScheduleType->Currency.characters, iso20_Currency_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 121;
                            }
                        }
                    }
                }
            }
            break;
        case 121:
            // Grammar: ID=121; read/write bits=1; START (PriceAlgorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=122

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EVAbsolutePriceScheduleType->PriceAlgorithm.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, EVAbsolutePriceScheduleType->PriceAlgorithm.charactersLen, EVAbsolutePriceScheduleType->PriceAlgorithm.characters, iso20_PriceAlgorithm_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 122;
                            }
                        }
                    }
                }
            }
            break;
        case 122:
            // Grammar: ID=122; read/write bits=1; START (EVPriceRuleStacks)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVPriceRuleStackListType); next=2
                error = encode_iso20_EVPriceRuleStackListType(stream, &EVAbsolutePriceScheduleType->EVPriceRuleStacks);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}EnergyCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedCostType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Amount, RationalNumberType (1, 1); CostPerUnit, RationalNumberType (1, 1);
static int encode_iso20_DetailedCostType(exi_bitstream_t* stream, const struct iso20_DetailedCostType* DetailedCostType) {
    int grammar_id = 123;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 123:
            // Grammar: ID=123; read/write bits=1; START (Amount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=124
                error = encode_iso20_RationalNumberType(stream, &DetailedCostType->Amount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 124;
                }
            }
            break;
        case 124:
            // Grammar: ID=124; read/write bits=1; START (CostPerUnit)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &DetailedCostType->CostPerUnit);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
static int encode_iso20_KeyInfoType(exi_bitstream_t* stream, const struct iso20_KeyInfoType* KeyInfoType) {
    int grammar_id = 125;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 125:
            // Grammar: ID=125; read/write bits=4; START (Id), START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=126

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, KeyInfoType->Id.charactersLen, KeyInfoType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 126;
                        }
                    }
                }
            }
            else if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_KeyName_CHARACTER_SIZE);
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
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=2
                    error = encode_iso20_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=2
                    error = encode_iso20_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=2
                    error = encode_iso20_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=2
                    error = encode_iso20_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=2
                    error = encode_iso20_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_MgmtData_CHARACTER_SIZE);
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
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 8);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            break;
        case 126:
            // Grammar: ID=126; read/write bits=4; START (KeyName), START (KeyValue), START (RetrievalMethod), START (X509Data), START (PGPData), START (SPKIData), START (MgmtData), START (ANY)
            if (KeyInfoType->KeyName_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyName, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->KeyName.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->KeyName.charactersLen, KeyInfoType->KeyName.characters, iso20_KeyName_CHARACTER_SIZE);
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
                }
            }
            else if (KeyInfoType->KeyValue_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyValue, KeyValueType); next=2
                    error = encode_iso20_KeyValueType(stream, &KeyInfoType->KeyValue);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->RetrievalMethod_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (RetrievalMethod, RetrievalMethodType); next=2
                    error = encode_iso20_RetrievalMethodType(stream, &KeyInfoType->RetrievalMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->X509Data_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509Data, X509DataType); next=2
                    error = encode_iso20_X509DataType(stream, &KeyInfoType->X509Data);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->PGPData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PGPData, PGPDataType); next=2
                    error = encode_iso20_PGPDataType(stream, &KeyInfoType->PGPData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->SPKIData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SPKIData, SPKIDataType); next=2
                    error = encode_iso20_SPKIDataType(stream, &KeyInfoType->SPKIData);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (KeyInfoType->MgmtData_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MgmtData, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(KeyInfoType->MgmtData.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, KeyInfoType->MgmtData.charactersLen, KeyInfoType->MgmtData.characters, iso20_MgmtData_CHARACTER_SIZE);
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
                }
            }
            else if (KeyInfoType->ANY_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 4, 7);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)KeyInfoType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, KeyInfoType->ANY.bytesLen, KeyInfoType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
// Particle: Encoding, anyURI (0, 1); Id, ID (0, 1); MimeType, string (0, 1); ANY, anyType (0, 1)(old 1, 1);
static int encode_iso20_ObjectType(exi_bitstream_t* stream, const struct iso20_ObjectType* ObjectType) {
    int grammar_id = 127;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 127:
            // Grammar: ID=127; read/write bits=3; START (Encoding), START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Encoding_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Encoding, anyURI); next=128

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Encoding.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Encoding.charactersLen, ObjectType->Encoding.characters, iso20_Encoding_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 128;
                        }
                    }
                }
            }
            else if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=129

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 129;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=130

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 130;
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 128:
            // Grammar: ID=128; read/write bits=3; START (Id), START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=129

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->Id.charactersLen, ObjectType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 129;
                        }
                    }
                }
            }
            else if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=130

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 130;
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 129:
            // Grammar: ID=129; read/write bits=3; START (MimeType), START (ANY), END Element, START (ANY)
            if (ObjectType->MimeType_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MimeType, string); next=130

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ObjectType->MimeType.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ObjectType->MimeType.charactersLen, ObjectType->MimeType.characters, iso20_MimeType_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 130;
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 130:
            // Grammar: ID=130; read/write bits=2; START (ANY), END Element, START (ANY)
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
                    // Event: START (ANY, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ObjectType->ANY.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, ObjectType->ANY.bytesLen, ObjectType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PriceLevelScheduleEntry, PriceLevelScheduleEntryType (1, 1024);
static int encode_iso20_PriceLevelScheduleEntryListType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleEntryListType* PriceLevelScheduleEntryListType) {
    int grammar_id = 131;
    int done = 0;
    int error = 0;
    uint16_t PriceLevelScheduleEntry_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 131:
            // Grammar: ID=131; read/write bits=1; START (PriceLevelScheduleEntry)
            if (PriceLevelScheduleEntry_currentIndex < PriceLevelScheduleEntryListType->PriceLevelScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelScheduleEntryType); next=132
                    error = encode_iso20_PriceLevelScheduleEntryType(stream, &PriceLevelScheduleEntryListType->PriceLevelScheduleEntry.array[PriceLevelScheduleEntry_currentIndex++]);
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
            // Grammar: ID=132; read/write bits=2; LOOP (PriceLevelScheduleEntry), END Element
            if (PriceLevelScheduleEntry_currentIndex < PriceLevelScheduleEntryListType->PriceLevelScheduleEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (PriceLevelScheduleEntryType); next=3
                    error = encode_iso20_PriceLevelScheduleEntryType(stream, &PriceLevelScheduleEntryListType->PriceLevelScheduleEntry.array[PriceLevelScheduleEntry_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}TaxCosts; type={urn:iso:std:iso:15118:-20:CommonTypes}DetailedTaxType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRuleID, numericIDType (1, 1); Amount, RationalNumberType (1, 1);
static int encode_iso20_DetailedTaxType(exi_bitstream_t* stream, const struct iso20_DetailedTaxType* DetailedTaxType) {
    int grammar_id = 133;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 133:
            // Grammar: ID=133; read/write bits=1; START (TaxRuleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=134
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, DetailedTaxType->TaxRuleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 134;
                        }
                    }
                }
            }
            break;
        case 134:
            // Grammar: ID=134; read/write bits=1; START (Amount)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=2
                error = encode_iso20_RationalNumberType(stream, &DetailedTaxType->Amount);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}TaxRules; type={urn:iso:std:iso:15118:-20:CommonMessages}TaxRuleListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TaxRule, TaxRuleType (1, 10);
static int encode_iso20_TaxRuleListType(exi_bitstream_t* stream, const struct iso20_TaxRuleListType* TaxRuleListType) {
    int grammar_id = 135;
    int done = 0;
    int error = 0;
    uint16_t TaxRule_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 135:
            // Grammar: ID=135; read/write bits=1; START (TaxRule)
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=136
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 136;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 136:
            // Grammar: ID=136; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=137
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 137;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 137:
            // Grammar: ID=137; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=138
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 138;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 138:
            // Grammar: ID=138; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=139
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 139:
            // Grammar: ID=139; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=140
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 140;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 140:
            // Grammar: ID=140; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=141
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 141:
            // Grammar: ID=141; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=142
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 142;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 142:
            // Grammar: ID=142; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=143
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 143:
            // Grammar: ID=143; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=144
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 144;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 144:
            // Grammar: ID=144; read/write bits=2; START (TaxRule), END Element
            if (TaxRule_currentIndex < TaxRuleListType->TaxRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRuleType); next=2
                    error = encode_iso20_TaxRuleType(stream, &TaxRuleListType->TaxRule.array[TaxRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStacks; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceRuleStackListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PriceRuleStack, PriceRuleStackType (1, 64);
static int encode_iso20_PriceRuleStackListType(exi_bitstream_t* stream, const struct iso20_PriceRuleStackListType* PriceRuleStackListType) {
    int grammar_id = 145;
    int done = 0;
    int error = 0;
    uint16_t PriceRuleStack_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 145:
            // Grammar: ID=145; read/write bits=1; START (PriceRuleStack)
            if (PriceRuleStack_currentIndex < PriceRuleStackListType->PriceRuleStack.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleStackType); next=146
                    error = encode_iso20_PriceRuleStackType(stream, &PriceRuleStackListType->PriceRuleStack.array[PriceRuleStack_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 146;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 146:
            // Grammar: ID=146; read/write bits=2; LOOP (PriceRuleStack), END Element
            if (PriceRuleStack_currentIndex < PriceRuleStackListType->PriceRuleStack.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (PriceRuleStackType); next=3
                    error = encode_iso20_PriceRuleStackType(stream, &PriceRuleStackListType->PriceRuleStack.array[PriceRuleStack_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRules; type={urn:iso:std:iso:15118:-20:CommonMessages}OverstayRuleListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: OverstayTimeThreshold, unsignedInt (0, 1); OverstayPowerThreshold, RationalNumberType (0, 1); OverstayRule, OverstayRuleType (1, 5);
static int encode_iso20_OverstayRuleListType(exi_bitstream_t* stream, const struct iso20_OverstayRuleListType* OverstayRuleListType) {
    int grammar_id = 147;
    int done = 0;
    int error = 0;
    uint16_t OverstayRule_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 147:
            // Grammar: ID=147; read/write bits=2; START (OverstayTimeThreshold), START (OverstayPowerThreshold), START (OverstayRule)
            if (OverstayRuleListType->OverstayTimeThreshold_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayTimeThreshold, unsignedLong); next=152
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, OverstayRuleListType->OverstayTimeThreshold);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 152;
                            }
                        }
                    }
                }
            }
            else if (OverstayRuleListType->OverstayPowerThreshold_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayPowerThreshold, RationalNumberType); next=157
                    error = encode_iso20_RationalNumberType(stream, &OverstayRuleListType->OverstayPowerThreshold);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 157;
                    }
                }
            }
            else
            {
                if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (OverstayRuleType); next=148
                        error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 148;
                        }
                    }
                }
            }
            break;
        case 148:
            // Grammar: ID=148; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=149
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 149;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 149:
            // Grammar: ID=149; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=150
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 150;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 150:
            // Grammar: ID=150; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=151
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 151;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 151:
            // Grammar: ID=151; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=2
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 152:
            // Grammar: ID=152; read/write bits=2; START (OverstayPowerThreshold), START (OverstayRule)
            if (OverstayRuleListType->OverstayPowerThreshold_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayPowerThreshold, RationalNumberType); next=157
                    error = encode_iso20_RationalNumberType(stream, &OverstayRuleListType->OverstayPowerThreshold);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 157;
                    }
                }
            }
            else
            {
                if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (OverstayRuleType); next=153
                        error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 153;
                        }
                    }
                }
            }
            break;
        case 153:
            // Grammar: ID=153; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=154
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 154:
            // Grammar: ID=154; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=155
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 155;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 155:
            // Grammar: ID=155; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=156
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 156;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 156:
            // Grammar: ID=156; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=2
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 157:
            // Grammar: ID=157; read/write bits=1; START (OverstayRule)
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=158
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 158;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 158:
            // Grammar: ID=158; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=159
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 159;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 159:
            // Grammar: ID=159; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=160
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 160;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 160:
            // Grammar: ID=160; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=161
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 161;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 161:
            // Grammar: ID=161; read/write bits=2; START (OverstayRule), END Element
            if (OverstayRule_currentIndex < OverstayRuleListType->OverstayRule.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRuleType); next=2
                    error = encode_iso20_OverstayRuleType(stream, &OverstayRuleListType->OverstayRule.array[OverstayRule_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalSelectedServices; type={urn:iso:std:iso:15118:-20:CommonMessages}AdditionalServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: AdditionalService, AdditionalServiceType (1, 5);
static int encode_iso20_AdditionalServiceListType(exi_bitstream_t* stream, const struct iso20_AdditionalServiceListType* AdditionalServiceListType) {
    int grammar_id = 162;
    int done = 0;
    int error = 0;
    uint16_t AdditionalService_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 162:
            // Grammar: ID=162; read/write bits=1; START (AdditionalService)
            if (AdditionalService_currentIndex < AdditionalServiceListType->AdditionalService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServiceType); next=163
                    error = encode_iso20_AdditionalServiceType(stream, &AdditionalServiceListType->AdditionalService.array[AdditionalService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 163;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 163:
            // Grammar: ID=163; read/write bits=2; START (AdditionalService), END Element
            if (AdditionalService_currentIndex < AdditionalServiceListType->AdditionalService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServiceType); next=164
                    error = encode_iso20_AdditionalServiceType(stream, &AdditionalServiceListType->AdditionalService.array[AdditionalService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 164;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 164:
            // Grammar: ID=164; read/write bits=2; START (AdditionalService), END Element
            if (AdditionalService_currentIndex < AdditionalServiceListType->AdditionalService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServiceType); next=165
                    error = encode_iso20_AdditionalServiceType(stream, &AdditionalServiceListType->AdditionalService.array[AdditionalService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 165;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 165:
            // Grammar: ID=165; read/write bits=2; START (AdditionalService), END Element
            if (AdditionalService_currentIndex < AdditionalServiceListType->AdditionalService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServiceType); next=166
                    error = encode_iso20_AdditionalServiceType(stream, &AdditionalServiceListType->AdditionalService.array[AdditionalService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 166;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 166:
            // Grammar: ID=166; read/write bits=2; START (AdditionalService), END Element
            if (AdditionalService_currentIndex < AdditionalServiceListType->AdditionalService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServiceType); next=2
                    error = encode_iso20_AdditionalServiceType(stream, &AdditionalServiceListType->AdditionalService.array[AdditionalService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Service; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); FreeService, boolean (1, 1);
static int encode_iso20_ServiceType(exi_bitstream_t* stream, const struct iso20_ServiceType* ServiceType) {
    int grammar_id = 167;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 167:
            // Grammar: ID=167; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=168
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
                            grammar_id = 168;
                        }
                    }
                }
            }
            break;
        case 168:
            // Grammar: ID=168; read/write bits=1; START (FreeService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=2
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
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ParameterSet; type={urn:iso:std:iso:15118:-20:CommonMessages}ParameterSetType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSetID, serviceIDType (1, 1); Parameter, ParameterType (1, 5);
static int encode_iso20_ParameterSetType(exi_bitstream_t* stream, const struct iso20_ParameterSetType* ParameterSetType) {
    int grammar_id = 169;
    int done = 0;
    int error = 0;
    uint16_t Parameter_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 169:
            // Grammar: ID=169; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=170
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, ParameterSetType->ParameterSetID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 170;
                        }
                    }
                }
            }
            break;
        case 170:
            // Grammar: ID=170; read/write bits=1; START (Parameter)
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=171
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 171;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 171:
            // Grammar: ID=171; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=172
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 172;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 172:
            // Grammar: ID=172; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=173
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 173;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 173:
            // Grammar: ID=173; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=174
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 174:
            // Grammar: ID=174; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=175
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 175;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 175:
            // Grammar: ID=175; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=2
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 176;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 176:
            // Grammar: ID=176; read/write bits=2; START (Parameter), END Element
            if (Parameter_currentIndex < ParameterSetType->Parameter.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterType); next=2
                    error = encode_iso20_ParameterType(stream, &ParameterSetType->Parameter.array[Parameter_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SupportedProviders; type={urn:iso:std:iso:15118:-20:CommonMessages}SupportedProvidersListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ProviderID, nameType (1, 128);
static int encode_iso20_SupportedProvidersListType(exi_bitstream_t* stream, const struct iso20_SupportedProvidersListType* SupportedProvidersListType) {
    int grammar_id = 176;
    int done = 0;
    int error = 0;
    uint16_t ProviderID_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 176:
            // Grammar: ID=176; read/write bits=1; START (ProviderID)
            if (ProviderID_currentIndex < SupportedProvidersListType->ProviderID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=177

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].charactersLen, SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].characters, iso20_ProviderID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ProviderID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 177;
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
        case 177:
            // Grammar: ID=177; read/write bits=2; LOOP (ProviderID), END Element
            if (ProviderID_currentIndex < SupportedProvidersListType->ProviderID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (string); next=3

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].charactersLen, SupportedProvidersListType->ProviderID.array[ProviderID_currentIndex].characters, iso20_ProviderID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                ProviderID_currentIndex++;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ContractCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}ContractCertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (1, 1);
static int encode_iso20_ContractCertificateChainType(exi_bitstream_t* stream, const struct iso20_ContractCertificateChainType* ContractCertificateChainType) {
    int grammar_id = 178;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 178:
            // Grammar: ID=178; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=179
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)ContractCertificateChainType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, ContractCertificateChainType->Certificate.bytesLen, ContractCertificateChainType->Certificate.bytes, iso20_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 179;
                            }
                        }
                    }
                }
            }
            break;
        case 179:
            // Grammar: ID=179; read/write bits=1; START (SubCertificates)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SubCertificatesType); next=2
                error = encode_iso20_SubCertificatesType(stream, &ContractCertificateChainType->SubCertificates);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_EVPPTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_EVPPTControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_Dynamic_EVPPTControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_EVPPTControlModeType* Dynamic_EVPPTControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)Dynamic_EVPPTControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeterInfo; type={urn:iso:std:iso:15118:-20:CommonTypes}MeterInfoType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: MeterID, meterIDType (1, 1); ChargedEnergyReadingWh, unsignedLong (1, 1); BPT_DischargedEnergyReadingWh, unsignedLong (0, 1); CapacitiveEnergyReadingVARh, unsignedLong (0, 1); BPT_InductiveEnergyReadingVARh, unsignedLong (0, 1); MeterSignature, meterSignatureType (0, 1); MeterStatus, short (0, 1); MeterTimestamp, unsignedLong (0, 1);
static int encode_iso20_MeterInfoType(exi_bitstream_t* stream, const struct iso20_MeterInfoType* MeterInfoType) {
    int grammar_id = 180;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 180:
            // Grammar: ID=180; read/write bits=1; START (MeterID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=181

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(MeterInfoType->MeterID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, MeterInfoType->MeterID.charactersLen, MeterInfoType->MeterID.characters, iso20_MeterID_CHARACTER_SIZE);
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
            }
            break;
        case 181:
            // Grammar: ID=181; read/write bits=1; START (ChargedEnergyReadingWh)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=182
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->ChargedEnergyReadingWh);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 182;
                        }
                    }
                }
            }
            break;
        case 182:
            // Grammar: ID=182; read/write bits=3; START (BPT_DischargedEnergyReadingWh), START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_DischargedEnergyReadingWh, nonNegativeInteger); next=183
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_DischargedEnergyReadingWh);
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
            }
            else if (MeterInfoType->CapacitiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CapacitiveEnergyReadingVARh, nonNegativeInteger); next=184
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->CapacitiveEnergyReadingVARh);
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
            }
            else if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=185
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 185;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=186
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 186;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=187
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
                                grammar_id = 187;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 6);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 183:
            // Grammar: ID=183; read/write bits=3; START (CapacitiveEnergyReadingVARh), START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->CapacitiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (CapacitiveEnergyReadingVARh, nonNegativeInteger); next=184
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->CapacitiveEnergyReadingVARh);
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
            }
            else if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=185
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 185;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=186
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 186;
                                }
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=187
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
                                grammar_id = 187;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 184:
            // Grammar: ID=184; read/write bits=3; START (BPT_InductiveEnergyReadingVARh), START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_InductiveEnergyReadingVARh, nonNegativeInteger); next=185
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->BPT_InductiveEnergyReadingVARh);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 185;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=186
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 186;
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
                    // Event: START (MeterStatus, int); next=187
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
                                grammar_id = 187;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 185:
            // Grammar: ID=185; read/write bits=3; START (MeterSignature), START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->MeterSignature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterSignature, base64Binary); next=186
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MeterInfoType->MeterSignature.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, MeterInfoType->MeterSignature.bytesLen, MeterInfoType->MeterSignature.bytes, iso20_meterSignatureType_BYTES_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 186;
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
                    // Event: START (MeterStatus, int); next=187
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
                                grammar_id = 187;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 186:
            // Grammar: ID=186; read/write bits=2; START (MeterStatus), START (MeterTimestamp), END Element
            if (MeterInfoType->MeterStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterStatus, int); next=187
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
                                grammar_id = 187;
                            }
                        }
                    }
                }
            }
            else if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 187:
            // Grammar: ID=187; read/write bits=2; START (MeterTimestamp), END Element
            if (MeterInfoType->MeterTimestamp_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MeterTimestamp, nonNegativeInteger); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, MeterInfoType->MeterTimestamp);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
// Particle: Id, ID (0, 1); SignedInfo, SignedInfoType (1, 1); SignatureValue, SignatureValueType (1, 1); KeyInfo, KeyInfoType (0, 1); Object, ObjectType (0, 1);
static int encode_iso20_SignatureType(exi_bitstream_t* stream, const struct iso20_SignatureType* SignatureType) {
    int grammar_id = 188;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 188:
            // Grammar: ID=188; read/write bits=2; START (Id), START (SignedInfo)
            if (SignatureType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=189

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignatureType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignatureType->Id.charactersLen, SignatureType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 189;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignedInfo, SignedInfoType); next=190
                    error = encode_iso20_SignedInfoType(stream, &SignatureType->SignedInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 190;
                    }
                }
            }
            break;
        case 189:
            // Grammar: ID=189; read/write bits=1; START (SignedInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedInfoType); next=190
                error = encode_iso20_SignedInfoType(stream, &SignatureType->SignedInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 190;
                }
            }
            break;
        case 190:
            // Grammar: ID=190; read/write bits=1; START (SignatureValue)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=191
                error = encode_iso20_SignatureValueType(stream, &SignatureType->SignatureValue);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 191;
                }
            }
            break;
        case 191:
            // Grammar: ID=191; read/write bits=2; START (KeyInfo), START (Object), END Element
            if (SignatureType->KeyInfo_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (KeyInfo, KeyInfoType); next=193
                    error = encode_iso20_KeyInfoType(stream, &SignatureType->KeyInfo);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 193;
                    }
                }
            }
            else if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=192
                    error = encode_iso20_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 192;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 192:
            // Grammar: ID=192; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=2
                    error = encode_iso20_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 193:
            // Grammar: ID=193; read/write bits=2; START (Object), END Element
            if (SignatureType->Object_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=194
                    error = encode_iso20_ObjectType(stream, &SignatureType->Object);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 194:
            // Grammar: ID=194; read/write bits=2; START (Object), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Object, ObjectType); next=2
                    error = encode_iso20_ObjectType(stream, &SignatureType->Object);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_EVPPTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_EVPPTControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedScheduleTupleID, numericIDType (1, 1); PowerToleranceAcceptance, powerToleranceAcceptanceType (0, 1);
static int encode_iso20_Scheduled_EVPPTControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_EVPPTControlModeType* Scheduled_EVPPTControlModeType) {
    int grammar_id = 195;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 195:
            // Grammar: ID=195; read/write bits=1; START (SelectedScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=196
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, Scheduled_EVPPTControlModeType->SelectedScheduleTupleID);
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
            // Grammar: ID=196; read/write bits=2; START (PowerToleranceAcceptance), END Element
            if (Scheduled_EVPPTControlModeType->PowerToleranceAcceptance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerToleranceAcceptance, string); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, Scheduled_EVPPTControlModeType->PowerToleranceAcceptance);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Receipt; type={urn:iso:std:iso:15118:-20:CommonTypes}ReceiptType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); EnergyCosts, DetailedCostType (0, 1); OccupancyCosts, DetailedCostType (0, 1); AdditionalServicesCosts, DetailedCostType (0, 1); OverstayCosts, DetailedCostType (0, 1); TaxCosts, DetailedTaxType (0, 10);
static int encode_iso20_ReceiptType(exi_bitstream_t* stream, const struct iso20_ReceiptType* ReceiptType) {
    int grammar_id = 197;
    int done = 0;
    int error = 0;
    uint16_t TaxCosts_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 197:
            // Grammar: ID=197; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=198
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, ReceiptType->TimeAnchor);
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
            // Grammar: ID=198; read/write bits=3; START (EnergyCosts), START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->EnergyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EnergyCosts, DetailedCostType); next=208
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->EnergyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 208;
                    }
                }
            }
            else if (ReceiptType->OccupancyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OccupancyCosts, DetailedCostType); next=218
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 218;
                    }
                }
            }
            else if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=228
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 228;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=238
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 238;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=199 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 199;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 199:
            // Grammar: ID=199; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=200 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 200;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 200:
            // Grammar: ID=200; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=201 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 201;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 201:
            // Grammar: ID=201; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=202 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 202;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 202:
            // Grammar: ID=202; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=203 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 203;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 203:
            // Grammar: ID=203; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=204 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 204;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 204:
            // Grammar: ID=204; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=205 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 205;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 205:
            // Grammar: ID=205; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=206 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 206;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 206:
            // Grammar: ID=206; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=207 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 207;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 207:
            // Grammar: ID=207; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=2 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 208:
            // Grammar: ID=208; read/write bits=3; START (OccupancyCosts), START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->OccupancyCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OccupancyCosts, DetailedCostType); next=218
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OccupancyCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 218;
                    }
                }
            }
            else if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=228
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 228;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=238
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 238;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=209 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 209;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 209:
            // Grammar: ID=209; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=210 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 210;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 210:
            // Grammar: ID=210; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=211 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 211:
            // Grammar: ID=211; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=212 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 212;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 212:
            // Grammar: ID=212; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=213 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 213;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 213:
            // Grammar: ID=213; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=214 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 214;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 214:
            // Grammar: ID=214; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=215 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 215;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 215:
            // Grammar: ID=215; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=216 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 216;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 216:
            // Grammar: ID=216; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=217 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 217;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 217:
            // Grammar: ID=217; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=2 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 218:
            // Grammar: ID=218; read/write bits=3; START (AdditionalServicesCosts), START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->AdditionalServicesCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalServicesCosts, DetailedCostType); next=228
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->AdditionalServicesCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 228;
                    }
                }
            }
            else if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=238
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 238;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=219 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 219;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 219:
            // Grammar: ID=219; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=220 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 220;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 220:
            // Grammar: ID=220; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=221 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 221;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 221:
            // Grammar: ID=221; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=222 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 222;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 222:
            // Grammar: ID=222; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=223 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 223;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 223:
            // Grammar: ID=223; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=224 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 224;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 224:
            // Grammar: ID=224; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=225 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 225;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 225:
            // Grammar: ID=225; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=226 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 226;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 226:
            // Grammar: ID=226; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=227 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 227;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 227:
            // Grammar: ID=227; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=2 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 228:
            // Grammar: ID=228; read/write bits=2; START (OverstayCosts), START (TaxCosts), END Element
            if (ReceiptType->OverstayCosts_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayCosts, DetailedCostType); next=238
                    error = encode_iso20_DetailedCostType(stream, &ReceiptType->OverstayCosts);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 238;
                    }
                }
            }
            else if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=229 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 229;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 229:
            // Grammar: ID=229; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=230 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 230;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 230:
            // Grammar: ID=230; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=231 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 231;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 231:
            // Grammar: ID=231; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=232 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 232;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 232:
            // Grammar: ID=232; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=233 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 233;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 233:
            // Grammar: ID=233; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=234 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 234;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 234:
            // Grammar: ID=234; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=235 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 235;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 235:
            // Grammar: ID=235; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=236 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 236;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 236:
            // Grammar: ID=236; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=237 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 237;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 237:
            // Grammar: ID=237; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=2 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 238:
            // Grammar: ID=238; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=239 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 239;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 239:
            // Grammar: ID=239; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=240 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 240;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 240:
            // Grammar: ID=240; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=241 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 241;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 241:
            // Grammar: ID=241; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=242 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 242;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 242:
            // Grammar: ID=242; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=243 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 243;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 243:
            // Grammar: ID=243; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=244 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 244:
            // Grammar: ID=244; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=245 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 245;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 245:
            // Grammar: ID=245; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=246 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 246;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 246:
            // Grammar: ID=246; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=247 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 247;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 247:
            // Grammar: ID=247; read/write bits=2; START (TaxCosts), END Element
            if (TaxCosts_currentIndex < ReceiptType->TaxCosts.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxCosts, DetailedTaxType); next=2 (optional array)
                    error = encode_iso20_DetailedTaxType(stream, &ReceiptType->TaxCosts.array[TaxCosts_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AbsolutePriceSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}AbsolutePriceScheduleType; base type=PriceScheduleType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); TimeAnchor, unsignedLong (1, 1); PriceScheduleID, numericIDType (1, 1); PriceScheduleDescription, descriptionType (0, 1); Currency, currencyType (1, 1); Language, languageType (1, 1); PriceAlgorithm, identifierType (1, 1); MinimumCost, RationalNumberType (0, 1); MaximumCost, RationalNumberType (0, 1); TaxRules, TaxRuleListType (0, 1); PriceRuleStacks, PriceRuleStackListType (1, 1); OverstayRules, OverstayRuleListType (0, 1); AdditionalSelectedServices, AdditionalServiceListType (0, 1);
static int encode_iso20_AbsolutePriceScheduleType(exi_bitstream_t* stream, const struct iso20_AbsolutePriceScheduleType* AbsolutePriceScheduleType) {
    int grammar_id = 248;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 248:
            // Grammar: ID=248; read/write bits=2; START (Id), START (TimeAnchor)
            if (AbsolutePriceScheduleType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=249

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->Id.charactersLen, AbsolutePriceScheduleType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 249;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TimeAnchor, nonNegativeInteger); next=250
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, AbsolutePriceScheduleType->TimeAnchor);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 250;
                            }
                        }
                    }
                }
            }
            break;
        case 249:
            // Grammar: ID=249; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=250
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, AbsolutePriceScheduleType->TimeAnchor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 250;
                        }
                    }
                }
            }
            break;
        case 250:
            // Grammar: ID=250; read/write bits=1; START (PriceScheduleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=251
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, AbsolutePriceScheduleType->PriceScheduleID);
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
            // Grammar: ID=251; read/write bits=2; START (PriceScheduleDescription), START (Currency)
            if (AbsolutePriceScheduleType->PriceScheduleDescription_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceScheduleDescription, string); next=252

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->PriceScheduleDescription.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->PriceScheduleDescription.charactersLen, AbsolutePriceScheduleType->PriceScheduleDescription.characters, iso20_PriceScheduleDescription_CHARACTER_SIZE);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Currency, string); next=253

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->Currency.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->Currency.charactersLen, AbsolutePriceScheduleType->Currency.characters, iso20_Currency_CHARACTER_SIZE);
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
                }
            }
            break;
        case 252:
            // Grammar: ID=252; read/write bits=1; START (Currency)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=253

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->Currency.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->Currency.charactersLen, AbsolutePriceScheduleType->Currency.characters, iso20_Currency_CHARACTER_SIZE);
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
            }
            break;
        case 253:
            // Grammar: ID=253; read/write bits=1; START (Language)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=254

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->Language.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->Language.charactersLen, AbsolutePriceScheduleType->Language.characters, iso20_Language_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 254;
                            }
                        }
                    }
                }
            }
            break;
        case 254:
            // Grammar: ID=254; read/write bits=1; START (PriceAlgorithm)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=255

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(AbsolutePriceScheduleType->PriceAlgorithm.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, AbsolutePriceScheduleType->PriceAlgorithm.charactersLen, AbsolutePriceScheduleType->PriceAlgorithm.characters, iso20_PriceAlgorithm_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 255;
                            }
                        }
                    }
                }
            }
            break;
        case 255:
            // Grammar: ID=255; read/write bits=3; START (MinimumCost), START (MaximumCost), START (TaxRules), START (PriceRuleStacks)
            if (AbsolutePriceScheduleType->MinimumCost_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumCost, RationalNumberType); next=256
                    error = encode_iso20_RationalNumberType(stream, &AbsolutePriceScheduleType->MinimumCost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 256;
                    }
                }
            }
            else if (AbsolutePriceScheduleType->MaximumCost_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumCost, RationalNumberType); next=257
                    error = encode_iso20_RationalNumberType(stream, &AbsolutePriceScheduleType->MaximumCost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 257;
                    }
                }
            }
            else if (AbsolutePriceScheduleType->TaxRules_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRules, TaxRuleListType); next=258
                    error = encode_iso20_TaxRuleListType(stream, &AbsolutePriceScheduleType->TaxRules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 258;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleStacks, PriceRuleStackListType); next=259
                    error = encode_iso20_PriceRuleStackListType(stream, &AbsolutePriceScheduleType->PriceRuleStacks);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 259;
                    }
                }
            }
            break;
        case 256:
            // Grammar: ID=256; read/write bits=2; START (MaximumCost), START (TaxRules), START (PriceRuleStacks)
            if (AbsolutePriceScheduleType->MaximumCost_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MaximumCost, RationalNumberType); next=257
                    error = encode_iso20_RationalNumberType(stream, &AbsolutePriceScheduleType->MaximumCost);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 257;
                    }
                }
            }
            else if (AbsolutePriceScheduleType->TaxRules_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRules, TaxRuleListType); next=258
                    error = encode_iso20_TaxRuleListType(stream, &AbsolutePriceScheduleType->TaxRules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 258;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleStacks, PriceRuleStackListType); next=259
                    error = encode_iso20_PriceRuleStackListType(stream, &AbsolutePriceScheduleType->PriceRuleStacks);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 259;
                    }
                }
            }
            break;
        case 257:
            // Grammar: ID=257; read/write bits=2; START (TaxRules), START (PriceRuleStacks)
            if (AbsolutePriceScheduleType->TaxRules_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TaxRules, TaxRuleListType); next=258
                    error = encode_iso20_TaxRuleListType(stream, &AbsolutePriceScheduleType->TaxRules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 258;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceRuleStacks, PriceRuleStackListType); next=259
                    error = encode_iso20_PriceRuleStackListType(stream, &AbsolutePriceScheduleType->PriceRuleStacks);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 259;
                    }
                }
            }
            break;
        case 258:
            // Grammar: ID=258; read/write bits=1; START (PriceRuleStacks)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PriceRuleStackListType); next=259
                error = encode_iso20_PriceRuleStackListType(stream, &AbsolutePriceScheduleType->PriceRuleStacks);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 259;
                }
            }
            break;
        case 259:
            // Grammar: ID=259; read/write bits=2; START (OverstayRules), START (AdditionalSelectedServices), END Element
            if (AbsolutePriceScheduleType->OverstayRules_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (OverstayRules, OverstayRuleListType); next=260
                    error = encode_iso20_OverstayRuleListType(stream, &AbsolutePriceScheduleType->OverstayRules);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 260;
                    }
                }
            }
            else if (AbsolutePriceScheduleType->AdditionalSelectedServices_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalSelectedServices, AdditionalServiceListType); next=2
                    error = encode_iso20_AdditionalServiceListType(stream, &AbsolutePriceScheduleType->AdditionalSelectedServices);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 260:
            // Grammar: ID=260; read/write bits=2; START (AdditionalSelectedServices), END Element
            if (AbsolutePriceScheduleType->AdditionalSelectedServices_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AdditionalSelectedServices, AdditionalServiceListType); next=2
                    error = encode_iso20_AdditionalServiceListType(stream, &AbsolutePriceScheduleType->AdditionalSelectedServices);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileEntries; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileEntryListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerProfileEntry, PowerScheduleEntryType (1, 2048);
static int encode_iso20_EVPowerProfileEntryListType(exi_bitstream_t* stream, const struct iso20_EVPowerProfileEntryListType* EVPowerProfileEntryListType) {
    int grammar_id = 261;
    int done = 0;
    int error = 0;
    uint16_t EVPowerProfileEntry_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 261:
            // Grammar: ID=261; read/write bits=1; START (EVPowerProfileEntry)
            if (EVPowerProfileEntry_currentIndex < EVPowerProfileEntryListType->EVPowerProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PowerScheduleEntryType); next=262
                    error = encode_iso20_PowerScheduleEntryType(stream, &EVPowerProfileEntryListType->EVPowerProfileEntry.array[EVPowerProfileEntry_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 262;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 262:
            // Grammar: ID=262; read/write bits=2; LOOP (EVPowerProfileEntry), END Element
            if (EVPowerProfileEntry_currentIndex < EVPowerProfileEntryListType->EVPowerProfileEntry.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: LOOP (PowerScheduleEntryType); next=3
                    error = encode_iso20_PowerScheduleEntryType(stream, &EVPowerProfileEntryListType->EVPowerProfileEntry.array[EVPowerProfileEntry_currentIndex++]);
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SMDTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SMDTControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_Dynamic_SMDTControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SMDTControlModeType* Dynamic_SMDTControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)Dynamic_SMDTControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVEnergyOffer; type={urn:iso:std:iso:15118:-20:CommonMessages}EVEnergyOfferType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EVPowerSchedule, EVPowerScheduleType (1, 1); EVAbsolutePriceSchedule, EVAbsolutePriceScheduleType (1, 1);
static int encode_iso20_EVEnergyOfferType(exi_bitstream_t* stream, const struct iso20_EVEnergyOfferType* EVEnergyOfferType) {
    int grammar_id = 263;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 263:
            // Grammar: ID=263; read/write bits=1; START (EVPowerSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVPowerScheduleType); next=264
                error = encode_iso20_EVPowerScheduleType(stream, &EVEnergyOfferType->EVPowerSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 264;
                }
            }
            break;
        case 264:
            // Grammar: ID=264; read/write bits=1; START (EVAbsolutePriceSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVAbsolutePriceScheduleType); next=2
                error = encode_iso20_EVAbsolutePriceScheduleType(stream, &EVEnergyOfferType->EVAbsolutePriceSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}PriceLevelScheduleType; base type=PriceScheduleType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Id, ID (0, 1); TimeAnchor, unsignedLong (1, 1); PriceScheduleID, numericIDType (1, 1); PriceScheduleDescription, descriptionType (0, 1); NumberOfPriceLevels, unsignedByte (1, 1); PriceLevelScheduleEntries, PriceLevelScheduleEntryListType (1, 1);
static int encode_iso20_PriceLevelScheduleType(exi_bitstream_t* stream, const struct iso20_PriceLevelScheduleType* PriceLevelScheduleType) {
    int grammar_id = 265;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 265:
            // Grammar: ID=265; read/write bits=2; START (Id), START (TimeAnchor)
            if (PriceLevelScheduleType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=266

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PriceLevelScheduleType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, PriceLevelScheduleType->Id.charactersLen, PriceLevelScheduleType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 266;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TimeAnchor, nonNegativeInteger); next=267
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_64(stream, PriceLevelScheduleType->TimeAnchor);
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
        case 266:
            // Grammar: ID=266; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=267
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, PriceLevelScheduleType->TimeAnchor);
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
            break;
        case 267:
            // Grammar: ID=267; read/write bits=1; START (PriceScheduleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=268
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, PriceLevelScheduleType->PriceScheduleID);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 268;
                        }
                    }
                }
            }
            break;
        case 268:
            // Grammar: ID=268; read/write bits=2; START (PriceScheduleDescription), START (NumberOfPriceLevels)
            if (PriceLevelScheduleType->PriceScheduleDescription_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceScheduleDescription, string); next=269

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PriceLevelScheduleType->PriceScheduleDescription.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, PriceLevelScheduleType->PriceScheduleDescription.charactersLen, PriceLevelScheduleType->PriceScheduleDescription.characters, iso20_PriceScheduleDescription_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 269;
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
                    // Event: START (NumberOfPriceLevels, unsignedShort); next=270
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceLevelScheduleType->NumberOfPriceLevels);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 270;
                            }
                        }
                    }
                }
            }
            break;
        case 269:
            // Grammar: ID=269; read/write bits=1; START (NumberOfPriceLevels)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=270
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, PriceLevelScheduleType->NumberOfPriceLevels);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 270;
                        }
                    }
                }
            }
            break;
        case 270:
            // Grammar: ID=270; read/write bits=1; START (PriceLevelScheduleEntries)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PriceLevelScheduleEntryListType); next=2
                error = encode_iso20_PriceLevelScheduleEntryListType(stream, &PriceLevelScheduleType->PriceLevelScheduleEntries);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ChargingSchedule; type={urn:iso:std:iso:15118:-20:CommonMessages}ChargingScheduleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: PowerSchedule, PowerScheduleType (1, 1); AbsolutePriceSchedule, AbsolutePriceScheduleType (0, 1); PriceLevelSchedule, PriceLevelScheduleType (0, 1);
static int encode_iso20_ChargingScheduleType(exi_bitstream_t* stream, const struct iso20_ChargingScheduleType* ChargingScheduleType) {
    int grammar_id = 271;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 271:
            // Grammar: ID=271; read/write bits=1; START (PowerSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (PowerScheduleType); next=272
                error = encode_iso20_PowerScheduleType(stream, &ChargingScheduleType->PowerSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 272;
                }
            }
            break;
        case 272:
            // Grammar: ID=272; read/write bits=2; START (AbsolutePriceSchedule), START (PriceLevelSchedule), END Element
            if (ChargingScheduleType->AbsolutePriceSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AbsolutePriceSchedule, PriceScheduleType); next=2
                    error = encode_iso20_AbsolutePriceScheduleType(stream, &ChargingScheduleType->AbsolutePriceSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (ChargingScheduleType->PriceLevelSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelSchedule, PriceScheduleType); next=2
                    error = encode_iso20_PriceLevelScheduleType(stream, &ChargingScheduleType->PriceLevelSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleTuple; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleTupleType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ScheduleTupleID, numericIDType (1, 1); ChargingSchedule, ChargingScheduleType (1, 1); DischargingSchedule, ChargingScheduleType (0, 1);
static int encode_iso20_ScheduleTupleType(exi_bitstream_t* stream, const struct iso20_ScheduleTupleType* ScheduleTupleType) {
    int grammar_id = 273;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 273:
            // Grammar: ID=273; read/write bits=1; START (ScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=274
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, ScheduleTupleType->ScheduleTupleID);
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
            break;
        case 274:
            // Grammar: ID=274; read/write bits=1; START (ChargingSchedule)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ChargingScheduleType); next=275
                error = encode_iso20_ChargingScheduleType(stream, &ScheduleTupleType->ChargingSchedule);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 275;
                }
            }
            break;
        case 275:
            // Grammar: ID=275; read/write bits=2; START (DischargingSchedule), END Element
            if (ScheduleTupleType->DischargingSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DischargingSchedule, ChargingScheduleType); next=2
                    error = encode_iso20_ChargingScheduleType(stream, &ScheduleTupleType->DischargingSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SMDTControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SMDTControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedScheduleTupleID, numericIDType (1, 1);
static int encode_iso20_Scheduled_SMDTControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SMDTControlModeType* Scheduled_SMDTControlModeType) {
    int grammar_id = 276;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 276:
            // Grammar: ID=276; read/write bits=1; START (SelectedScheduleTupleID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, Scheduled_SMDTControlModeType->SelectedScheduleTupleID);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}Header; type={urn:iso:std:iso:15118:-20:CommonTypes}MessageHeaderType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SessionID, sessionIDType (1, 1); TimeStamp, unsignedLong (1, 1); Signature, SignatureType (0, 1);
static int encode_iso20_MessageHeaderType(exi_bitstream_t* stream, const struct iso20_MessageHeaderType* MessageHeaderType) {
    int grammar_id = 277;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 277:
            // Grammar: ID=277; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=278
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)MessageHeaderType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, MessageHeaderType->SessionID.bytesLen, MessageHeaderType->SessionID.bytes, iso20_sessionIDType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 278;
                            }
                        }
                    }
                }
            }
            break;
        case 278:
            // Grammar: ID=278; read/write bits=1; START (TimeStamp)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=279
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, MessageHeaderType->TimeStamp);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 279;
                        }
                    }
                }
            }
            break;
        case 279:
            // Grammar: ID=279; read/write bits=2; START (Signature), END Element
            if (MessageHeaderType->Signature_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Signature, SignatureType); next=2
                    error = encode_iso20_SignatureType(stream, &MessageHeaderType->Signature);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperty; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertyType; base type=; content type=mixed;
//          abstract=False; final=False; choice=True;
// Particle: Id, ID (0, 1); Target, anyURI (1, 1); ANY, anyType (0, 1);
static int encode_iso20_SignaturePropertyType(exi_bitstream_t* stream, const struct iso20_SignaturePropertyType* SignaturePropertyType) {
    int grammar_id = 280;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 280:
            // Grammar: ID=280; read/write bits=2; START (Id), START (Target)
            if (SignaturePropertyType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=281

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Id.charactersLen, SignaturePropertyType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 281;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Target, anyURI); next=282

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Target.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_Target_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 282;
                        }
                    }
                }
            }
            break;
        case 281:
            // Grammar: ID=281; read/write bits=1; START (Target)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (anyURI); next=282

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertyType->Target.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignaturePropertyType->Target.charactersLen, SignaturePropertyType->Target.characters, iso20_Target_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 282;
                    }
                }
            }
            break;
        case 282:
            // Grammar: ID=282; read/write bits=1; START (ANY)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignaturePropertyType->ANY.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignaturePropertyType->ANY.bytesLen, SignaturePropertyType->ANY.bytes, iso20_anyType_BYTES_SIZE);
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
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SupportedServiceIDs; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceIDListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 16);
static int encode_iso20_ServiceIDListType(exi_bitstream_t* stream, const struct iso20_ServiceIDListType* ServiceIDListType) {
    int grammar_id = 283;
    int done = 0;
    int error = 0;
    uint16_t ServiceID_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 283:
            // Grammar: ID=283; read/write bits=1; START (ServiceID)
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=284
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 284:
            // Grammar: ID=284; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=285
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 285;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 285:
            // Grammar: ID=285; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=286
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 286:
            // Grammar: ID=286; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=287
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 287:
            // Grammar: ID=287; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=288
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 288;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 288:
            // Grammar: ID=288; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=289
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 289:
            // Grammar: ID=289; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=290
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 290;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 290:
            // Grammar: ID=290; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=291
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 291:
            // Grammar: ID=291; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=292
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 292;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 292:
            // Grammar: ID=292; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=293
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 293;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 293:
            // Grammar: ID=293; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=294
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 294:
            // Grammar: ID=294; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=295
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 295;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 295:
            // Grammar: ID=295; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=296
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 296;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 296:
            // Grammar: ID=296; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=297
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 297;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 297:
            // Grammar: ID=297; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=298
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 298;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 298:
            // Grammar: ID=298; read/write bits=2; START (ServiceID), END Element
            if (ServiceID_currentIndex < ServiceIDListType->ServiceID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (unsignedShort); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, ServiceIDListType->ServiceID.array[ServiceID_currentIndex++]);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SelectedEnergyTransferService; type={urn:iso:std:iso:15118:-20:CommonMessages}SelectedServiceType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ServiceID, serviceIDType (1, 1); ParameterSetID, serviceIDType (1, 1);
static int encode_iso20_SelectedServiceType(exi_bitstream_t* stream, const struct iso20_SelectedServiceType* SelectedServiceType) {
    int grammar_id = 299;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 299:
            // Grammar: ID=299; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=300
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
                            grammar_id = 300;
                        }
                    }
                }
            }
            break;
        case 300:
            // Grammar: ID=300; read/write bits=1; START (ParameterSetID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, SelectedServiceType->ParameterSetID);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SignedMeteringData; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedMeteringDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); SessionID, sessionIDType (1, 1); MeterInfo, MeterInfoType (1, 1); Receipt, ReceiptType (0, 1); Dynamic_SMDTControlMode, Dynamic_SMDTControlModeType (0, 1); Scheduled_SMDTControlMode, Scheduled_SMDTControlModeType (0, 1);
static int encode_iso20_SignedMeteringDataType(exi_bitstream_t* stream, const struct iso20_SignedMeteringDataType* SignedMeteringDataType) {
    int grammar_id = 301;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 301:
            // Grammar: ID=301; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=302

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedMeteringDataType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignedMeteringDataType->Id.charactersLen, SignedMeteringDataType->Id.characters, iso20_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 302;
                    }
                }
            }
            break;
        case 302:
            // Grammar: ID=302; read/write bits=1; START (SessionID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (hexBinary); next=303
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedMeteringDataType->SessionID.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignedMeteringDataType->SessionID.bytesLen, SignedMeteringDataType->SessionID.bytes, iso20_sessionIDType_BYTES_SIZE);
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
            }
            break;
        case 303:
            // Grammar: ID=303; read/write bits=1; START (MeterInfo)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MeterInfoType); next=304
                error = encode_iso20_MeterInfoType(stream, &SignedMeteringDataType->MeterInfo);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 304;
                }
            }
            break;
        case 304:
            // Grammar: ID=304; read/write bits=2; START (Receipt), START (Dynamic_SMDTControlMode), START (Scheduled_SMDTControlMode)
            if (SignedMeteringDataType->Receipt_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Receipt, ReceiptType); next=305
                    error = encode_iso20_ReceiptType(stream, &SignedMeteringDataType->Receipt);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 305;
                    }
                }
            }
            else if (SignedMeteringDataType->Dynamic_SMDTControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_SMDTControlMode, Dynamic_SMDTControlModeType); next=2
                    error = encode_iso20_Dynamic_SMDTControlModeType(stream, &SignedMeteringDataType->Dynamic_SMDTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_SMDTControlMode, Scheduled_SMDTControlModeType); next=2
                    error = encode_iso20_Scheduled_SMDTControlModeType(stream, &SignedMeteringDataType->Scheduled_SMDTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 305:
            // Grammar: ID=305; read/write bits=2; START (Dynamic_SMDTControlMode), START (Scheduled_SMDTControlMode)
            if (SignedMeteringDataType->Dynamic_SMDTControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_SMDTControlMode, Dynamic_SMDTControlModeType); next=2
                    error = encode_iso20_Dynamic_SMDTControlModeType(stream, &SignedMeteringDataType->Dynamic_SMDTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_SMDTControlMode, Scheduled_SMDTControlModeType); next=2
                    error = encode_iso20_Scheduled_SMDTControlModeType(stream, &SignedMeteringDataType->Scheduled_SMDTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}OEMProvisioningCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedCertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
static int encode_iso20_SignedCertificateChainType(exi_bitstream_t* stream, const struct iso20_SignedCertificateChainType* SignedCertificateChainType) {
    int grammar_id = 306;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 306:
            // Grammar: ID=306; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=307

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedCertificateChainType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignedCertificateChainType->Id.charactersLen, SignedCertificateChainType->Id.characters, iso20_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 307;
                    }
                }
            }
            break;
        case 307:
            // Grammar: ID=307; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=308
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedCertificateChainType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignedCertificateChainType->Certificate.bytesLen, SignedCertificateChainType->Certificate.bytes, iso20_certificateType_BYTES_SIZE);
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
            // Grammar: ID=308; read/write bits=2; START (SubCertificates), END Element
            if (SignedCertificateChainType->SubCertificates_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SubCertificates, SubCertificatesType); next=2
                    error = encode_iso20_SubCertificatesType(stream, &SignedCertificateChainType->SubCertificates);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EIM_AReqAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}EIM_AReqAuthorizationModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_EIM_AReqAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_EIM_AReqAuthorizationModeType* EIM_AReqAuthorizationModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)EIM_AReqAuthorizationModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SelectedVASList; type={urn:iso:std:iso:15118:-20:CommonMessages}SelectedServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: SelectedService, SelectedServiceType (1, 16);
static int encode_iso20_SelectedServiceListType(exi_bitstream_t* stream, const struct iso20_SelectedServiceListType* SelectedServiceListType) {
    int grammar_id = 309;
    int done = 0;
    int error = 0;
    uint16_t SelectedService_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 309:
            // Grammar: ID=309; read/write bits=1; START (SelectedService)
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=310
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 310;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 310:
            // Grammar: ID=310; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=311
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 311;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 311:
            // Grammar: ID=311; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=312
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 312;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 312:
            // Grammar: ID=312; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=313
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 313;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 313:
            // Grammar: ID=313; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=314
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 314;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 314:
            // Grammar: ID=314; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=315
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 315;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 315:
            // Grammar: ID=315; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=316
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 316;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 316:
            // Grammar: ID=316; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=317
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 317;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 317:
            // Grammar: ID=317; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=318
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 318;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 318:
            // Grammar: ID=318; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=319
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 319;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 319:
            // Grammar: ID=319; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=320
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 320;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 320:
            // Grammar: ID=320; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=321
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 321;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 321:
            // Grammar: ID=321; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=322
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 322;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 322:
            // Grammar: ID=322; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=323
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 323;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 323:
            // Grammar: ID=323; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=324
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 324;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 324:
            // Grammar: ID=324; read/write bits=2; START (SelectedService), END Element
            if (SelectedService_currentIndex < SelectedServiceListType->SelectedService.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedServiceType); next=2
                    error = encode_iso20_SelectedServiceType(stream, &SelectedServiceListType->SelectedService.array[SelectedService_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEReqControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEReqControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (1, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); EVTargetEnergyRequest, RationalNumberType (1, 1); EVMaximumEnergyRequest, RationalNumberType (1, 1); EVMinimumEnergyRequest, RationalNumberType (1, 1); EVMaximumV2XEnergyRequest, RationalNumberType (0, 1); EVMinimumV2XEnergyRequest, RationalNumberType (0, 1);
static int encode_iso20_Dynamic_SEReqControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SEReqControlModeType* Dynamic_SEReqControlModeType) {
    int grammar_id = 325;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 325:
            // Grammar: ID=325; read/write bits=1; START (DepartureTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedLong); next=326
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_32(stream, Dynamic_SEReqControlModeType->DepartureTime);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 326;
                        }
                    }
                }
            }
            break;
        case 326:
            // Grammar: ID=326; read/write bits=2; START (MinimumSOC), START (TargetSOC), START (EVTargetEnergyRequest)
            if (Dynamic_SEReqControlModeType->MinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumSOC, byte); next=327
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEReqControlModeType->MinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 327;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEReqControlModeType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=328
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEReqControlModeType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 328;
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
                    // Event: START (EVTargetEnergyRequest, RationalNumberType); next=329
                    error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVTargetEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 329;
                    }
                }
            }
            break;
        case 327:
            // Grammar: ID=327; read/write bits=2; START (TargetSOC), START (EVTargetEnergyRequest)
            if (Dynamic_SEReqControlModeType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=328
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEReqControlModeType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 328;
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
                    // Event: START (EVTargetEnergyRequest, RationalNumberType); next=329
                    error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVTargetEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 329;
                    }
                }
            }
            break;
        case 328:
            // Grammar: ID=328; read/write bits=1; START (EVTargetEnergyRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=329
                error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVTargetEnergyRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 329;
                }
            }
            break;
        case 329:
            // Grammar: ID=329; read/write bits=1; START (EVMaximumEnergyRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=330
                error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVMaximumEnergyRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 330;
                }
            }
            break;
        case 330:
            // Grammar: ID=330; read/write bits=1; START (EVMinimumEnergyRequest)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (RationalNumberType); next=331
                error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVMinimumEnergyRequest);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 331;
                }
            }
            break;
        case 331:
            // Grammar: ID=331; read/write bits=2; START (EVMaximumV2XEnergyRequest), START (EVMinimumV2XEnergyRequest), END Element
            if (Dynamic_SEReqControlModeType->EVMaximumV2XEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumV2XEnergyRequest, RationalNumberType); next=332
                    error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVMaximumV2XEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 332;
                    }
                }
            }
            else if (Dynamic_SEReqControlModeType->EVMinimumV2XEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumV2XEnergyRequest, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVMinimumV2XEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 332:
            // Grammar: ID=332; read/write bits=2; START (EVMinimumV2XEnergyRequest), END Element
            if (Dynamic_SEReqControlModeType->EVMinimumV2XEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumV2XEnergyRequest, RationalNumberType); next=2
                    error = encode_iso20_RationalNumberType(stream, &Dynamic_SEReqControlModeType->EVMinimumV2XEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVSEStatus; type={urn:iso:std:iso:15118:-20:CommonTypes}EVSEStatusType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: NotificationMaxDelay, unsignedShort (1, 1); EVSENotification, evseNotificationType (1, 1);
static int encode_iso20_EVSEStatusType(exi_bitstream_t* stream, const struct iso20_EVSEStatusType* EVSEStatusType) {
    int grammar_id = 333;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 333:
            // Grammar: ID=333; read/write bits=1; START (NotificationMaxDelay)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedInt); next=334
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
                            grammar_id = 334;
                        }
                    }
                }
            }
            break;
        case 334:
            // Grammar: ID=334; read/write bits=1; START (EVSENotification)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 3, EVSEStatusType->EVSENotification);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ListOfRootCertificateIDs; type={urn:iso:std:iso:15118:-20:CommonTypes}ListOfRootCertificateIDsType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: RootCertificateID, X509IssuerSerialType (1, 20);
static int encode_iso20_ListOfRootCertificateIDsType(exi_bitstream_t* stream, const struct iso20_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    int grammar_id = 335;
    int done = 0;
    int error = 0;
    uint16_t RootCertificateID_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 335:
            // Grammar: ID=335; read/write bits=1; START (RootCertificateID)
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=336
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 336;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 336:
            // Grammar: ID=336; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=337
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 337;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 337:
            // Grammar: ID=337; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=338
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 338;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 338:
            // Grammar: ID=338; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=339
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 339;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 339:
            // Grammar: ID=339; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=340
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 340;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 340:
            // Grammar: ID=340; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=341
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 341;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 341:
            // Grammar: ID=341; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=342
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 342;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 342:
            // Grammar: ID=342; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=343
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 343;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 343:
            // Grammar: ID=343; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=344
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 344;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 344:
            // Grammar: ID=344; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=345
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 345;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 345:
            // Grammar: ID=345; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=346
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 346;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 346:
            // Grammar: ID=346; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=347
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 347;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 347:
            // Grammar: ID=347; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=348
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 348;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 348:
            // Grammar: ID=348; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=349
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 349;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 349:
            // Grammar: ID=349; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=350
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 350;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 350:
            // Grammar: ID=350; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=351
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 351;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 351:
            // Grammar: ID=351; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=352
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 352;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 352:
            // Grammar: ID=352; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=353
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 353;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 353:
            // Grammar: ID=353; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=354
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 354;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 354:
            // Grammar: ID=354; read/write bits=2; START (RootCertificateID), END Element
            if (RootCertificateID_currentIndex < ListOfRootCertificateIDsType->RootCertificateID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X509IssuerSerialType); next=2
                    error = encode_iso20_X509IssuerSerialType(stream, &ListOfRootCertificateIDsType->RootCertificateID.array[RootCertificateID_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PnC_AReqAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}PnC_AReqAuthorizationModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); GenChallenge, genChallengeType (1, 1); ContractCertificateChain, ContractCertificateChainType (1, 1);
static int encode_iso20_PnC_AReqAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_PnC_AReqAuthorizationModeType* PnC_AReqAuthorizationModeType) {
    int grammar_id = 355;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 355:
            // Grammar: ID=355; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=356

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(PnC_AReqAuthorizationModeType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, PnC_AReqAuthorizationModeType->Id.charactersLen, PnC_AReqAuthorizationModeType->Id.characters, iso20_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 356;
                    }
                }
            }
            break;
        case 356:
            // Grammar: ID=356; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=357
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PnC_AReqAuthorizationModeType->GenChallenge.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PnC_AReqAuthorizationModeType->GenChallenge.bytesLen, PnC_AReqAuthorizationModeType->GenChallenge.bytes, iso20_genChallengeType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 357;
                            }
                        }
                    }
                }
            }
            break;
        case 357:
            // Grammar: ID=357; read/write bits=1; START (ContractCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ContractCertificateChainType); next=2
                error = encode_iso20_ContractCertificateChainType(stream, &PnC_AReqAuthorizationModeType->ContractCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EnergyTransferServiceList; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Service, ServiceType (1, 8);
static int encode_iso20_ServiceListType(exi_bitstream_t* stream, const struct iso20_ServiceListType* ServiceListType) {
    int grammar_id = 358;
    int done = 0;
    int error = 0;
    uint16_t Service_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 358:
            // Grammar: ID=358; read/write bits=1; START (Service)
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=359
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 359;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 359:
            // Grammar: ID=359; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=360
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 360;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 360:
            // Grammar: ID=360; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=361
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 361;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 361:
            // Grammar: ID=361; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=362
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 362;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 362:
            // Grammar: ID=362; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=363
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 363;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 363:
            // Grammar: ID=363; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=364
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 364;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 364:
            // Grammar: ID=364; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=365
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 365;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 365:
            // Grammar: ID=365; read/write bits=2; START (Service), END Element
            if (Service_currentIndex < ServiceListType->Service.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ServiceType); next=2
                    error = encode_iso20_ServiceType(stream, &ServiceListType->Service.array[Service_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceParameterList; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceParameterListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ParameterSet, ParameterSetType (1, 5);
static int encode_iso20_ServiceParameterListType(exi_bitstream_t* stream, const struct iso20_ServiceParameterListType* ServiceParameterListType) {
    int grammar_id = 366;
    int done = 0;
    int error = 0;
    uint16_t ParameterSet_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 366:
            // Grammar: ID=366; read/write bits=1; START (ParameterSet)
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=367
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 367;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 367:
            // Grammar: ID=367; read/write bits=2; START (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=368
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 368;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 368:
            // Grammar: ID=368; read/write bits=2; START (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=369
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 369;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 369:
            // Grammar: ID=369; read/write bits=2; START (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=370
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 370;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 370:
            // Grammar: ID=370; read/write bits=2; START (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=371
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 371;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 371:
            // Grammar: ID=371; read/write bits=2; START (ParameterSet), END Element
            if (ParameterSet_currentIndex < ServiceParameterListType->ParameterSet.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParameterSetType); next=2
                    error = encode_iso20_ParameterSetType(stream, &ServiceParameterListType->ParameterSet.array[ParameterSet_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEReqControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEReqControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); EVTargetEnergyRequest, RationalNumberType (0, 1); EVMaximumEnergyRequest, RationalNumberType (0, 1); EVMinimumEnergyRequest, RationalNumberType (0, 1); EVEnergyOffer, EVEnergyOfferType (0, 1);
static int encode_iso20_Scheduled_SEReqControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SEReqControlModeType* Scheduled_SEReqControlModeType) {
    int grammar_id = 372;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 372:
            // Grammar: ID=372; read/write bits=3; START (DepartureTime), START (EVTargetEnergyRequest), START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVEnergyOffer), END Element
            if (Scheduled_SEReqControlModeType->DepartureTime_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DepartureTime, unsignedLong); next=373
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, Scheduled_SEReqControlModeType->DepartureTime);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 373;
                            }
                        }
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVTargetEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetEnergyRequest, RationalNumberType); next=374
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVTargetEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 374;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVMaximumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType); next=375
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMaximumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 375;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVMinimumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType); next=376
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMinimumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 376;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyOffer, EVEnergyOfferType); next=2
                    error = encode_iso20_EVEnergyOfferType(stream, &Scheduled_SEReqControlModeType->EVEnergyOffer);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 373:
            // Grammar: ID=373; read/write bits=3; START (EVTargetEnergyRequest), START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVEnergyOffer), END Element
            if (Scheduled_SEReqControlModeType->EVTargetEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTargetEnergyRequest, RationalNumberType); next=374
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVTargetEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 374;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVMaximumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType); next=375
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMaximumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 375;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVMinimumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType); next=376
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMinimumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 376;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyOffer, EVEnergyOfferType); next=2
                    error = encode_iso20_EVEnergyOfferType(stream, &Scheduled_SEReqControlModeType->EVEnergyOffer);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 374:
            // Grammar: ID=374; read/write bits=3; START (EVMaximumEnergyRequest), START (EVMinimumEnergyRequest), START (EVEnergyOffer), END Element
            if (Scheduled_SEReqControlModeType->EVMaximumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMaximumEnergyRequest, RationalNumberType); next=375
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMaximumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 375;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVMinimumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType); next=376
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMinimumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 376;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyOffer, EVEnergyOfferType); next=2
                    error = encode_iso20_EVEnergyOfferType(stream, &Scheduled_SEReqControlModeType->EVEnergyOffer);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 375:
            // Grammar: ID=375; read/write bits=2; START (EVMinimumEnergyRequest), START (EVEnergyOffer), END Element
            if (Scheduled_SEReqControlModeType->EVMinimumEnergyRequest_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVMinimumEnergyRequest, RationalNumberType); next=376
                    error = encode_iso20_RationalNumberType(stream, &Scheduled_SEReqControlModeType->EVMinimumEnergyRequest);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 376;
                    }
                }
            }
            else if (Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyOffer, EVEnergyOfferType); next=2
                    error = encode_iso20_EVEnergyOfferType(stream, &Scheduled_SEReqControlModeType->EVEnergyOffer);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 376:
            // Grammar: ID=376; read/write bits=2; START (EVEnergyOffer), END Element
            if (Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVEnergyOffer, EVEnergyOfferType); next=2
                    error = encode_iso20_EVEnergyOfferType(stream, &Scheduled_SEReqControlModeType->EVEnergyOffer);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfile; type={urn:iso:std:iso:15118:-20:CommonMessages}EVPowerProfileType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: TimeAnchor, unsignedLong (1, 1); Dynamic_EVPPTControlMode, Dynamic_EVPPTControlModeType (0, 1); Scheduled_EVPPTControlMode, Scheduled_EVPPTControlModeType (0, 1); EVPowerProfileEntries, EVPowerProfileEntryListType (1, 1);
static int encode_iso20_EVPowerProfileType(exi_bitstream_t* stream, const struct iso20_EVPowerProfileType* EVPowerProfileType) {
    int grammar_id = 377;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 377:
            // Grammar: ID=377; read/write bits=1; START (TimeAnchor)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=378
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, EVPowerProfileType->TimeAnchor);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 378;
                        }
                    }
                }
            }
            break;
        case 378:
            // Grammar: ID=378; read/write bits=2; START (Dynamic_EVPPTControlMode), START (Scheduled_EVPPTControlMode)
            if (EVPowerProfileType->Dynamic_EVPPTControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_EVPPTControlMode, Dynamic_EVPPTControlModeType); next=379
                    error = encode_iso20_Dynamic_EVPPTControlModeType(stream, &EVPowerProfileType->Dynamic_EVPPTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 379;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_EVPPTControlMode, Scheduled_EVPPTControlModeType); next=379
                    error = encode_iso20_Scheduled_EVPPTControlModeType(stream, &EVPowerProfileType->Scheduled_EVPPTControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 379;
                    }
                }
            }
            break;
        case 379:
            // Grammar: ID=379; read/write bits=1; START (EVPowerProfileEntries)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (EVPowerProfileEntryListType); next=2
                error = encode_iso20_EVPowerProfileEntryListType(stream, &EVPowerProfileType->EVPowerProfileEntries);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CPSCertificateChain; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateChainType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Certificate, certificateType (1, 1); SubCertificates, SubCertificatesType (0, 1);
static int encode_iso20_CertificateChainType(exi_bitstream_t* stream, const struct iso20_CertificateChainType* CertificateChainType) {
    int grammar_id = 380;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 380:
            // Grammar: ID=380; read/write bits=1; START (Certificate)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=381
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)CertificateChainType->Certificate.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, CertificateChainType->Certificate.bytesLen, CertificateChainType->Certificate.bytes, iso20_certificateType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 381;
                            }
                        }
                    }
                }
            }
            break;
        case 381:
            // Grammar: ID=381; read/write bits=2; START (SubCertificates), END Element
            if (CertificateChainType->SubCertificates_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SubCertificates, SubCertificatesType); next=2
                    error = encode_iso20_SubCertificatesType(stream, &CertificateChainType->SubCertificates);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}EIM_ASResAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}EIM_ASResAuthorizationModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_EIM_ASResAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_EIM_ASResAuthorizationModeType* EIM_ASResAuthorizationModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)EIM_ASResAuthorizationModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEResControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Dynamic_SEResControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: DepartureTime, unsignedInt (0, 1); MinimumSOC, percentValueType (0, 1); TargetSOC, percentValueType (0, 1); AbsolutePriceSchedule, AbsolutePriceScheduleType (0, 1); PriceLevelSchedule, PriceLevelScheduleType (0, 1);
static int encode_iso20_Dynamic_SEResControlModeType(exi_bitstream_t* stream, const struct iso20_Dynamic_SEResControlModeType* Dynamic_SEResControlModeType) {
    int grammar_id = 382;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 382:
            // Grammar: ID=382; read/write bits=3; START (DepartureTime), START (MinimumSOC), START (TargetSOC), START (AbsolutePriceSchedule), START (PriceLevelSchedule), END Element
            if (Dynamic_SEResControlModeType->DepartureTime_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DepartureTime, unsignedLong); next=383
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_32(stream, Dynamic_SEResControlModeType->DepartureTime);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 383;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->MinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumSOC, byte); next=384
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEResControlModeType->MinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 384;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=385
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEResControlModeType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 385;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->AbsolutePriceSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AbsolutePriceSchedule, PriceScheduleType); next=2
                    error = encode_iso20_AbsolutePriceScheduleType(stream, &Dynamic_SEResControlModeType->AbsolutePriceSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->PriceLevelSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelSchedule, PriceScheduleType); next=2
                    error = encode_iso20_PriceLevelScheduleType(stream, &Dynamic_SEResControlModeType->PriceLevelSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 5);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 383:
            // Grammar: ID=383; read/write bits=3; START (MinimumSOC), START (TargetSOC), START (AbsolutePriceSchedule), START (PriceLevelSchedule), END Element
            if (Dynamic_SEResControlModeType->MinimumSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (MinimumSOC, byte); next=384
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEResControlModeType->MinimumSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 384;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=385
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEResControlModeType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 385;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->AbsolutePriceSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AbsolutePriceSchedule, PriceScheduleType); next=2
                    error = encode_iso20_AbsolutePriceScheduleType(stream, &Dynamic_SEResControlModeType->AbsolutePriceSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->PriceLevelSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelSchedule, PriceScheduleType); next=2
                    error = encode_iso20_PriceLevelScheduleType(stream, &Dynamic_SEResControlModeType->PriceLevelSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 4);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 384:
            // Grammar: ID=384; read/write bits=3; START (TargetSOC), START (AbsolutePriceSchedule), START (PriceLevelSchedule), END Element
            if (Dynamic_SEResControlModeType->TargetSOC_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetSOC, byte); next=385
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 7, (uint32_t)Dynamic_SEResControlModeType->TargetSOC);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 385;
                            }
                        }
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->AbsolutePriceSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AbsolutePriceSchedule, PriceScheduleType); next=2
                    error = encode_iso20_AbsolutePriceScheduleType(stream, &Dynamic_SEResControlModeType->AbsolutePriceSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->PriceLevelSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelSchedule, PriceScheduleType); next=2
                    error = encode_iso20_PriceLevelScheduleType(stream, &Dynamic_SEResControlModeType->PriceLevelSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 385:
            // Grammar: ID=385; read/write bits=2; START (AbsolutePriceSchedule), START (PriceLevelSchedule), END Element
            if (Dynamic_SEResControlModeType->AbsolutePriceSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (AbsolutePriceSchedule, PriceScheduleType); next=2
                    error = encode_iso20_AbsolutePriceScheduleType(stream, &Dynamic_SEResControlModeType->AbsolutePriceSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else if (Dynamic_SEResControlModeType->PriceLevelSchedule_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PriceLevelSchedule, PriceScheduleType); next=2
                    error = encode_iso20_PriceLevelScheduleType(stream, &Dynamic_SEResControlModeType->PriceLevelSchedule);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PrioritizedEMAIDs; type={urn:iso:std:iso:15118:-20:CommonMessages}EMAIDListType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: EMAID, identifierType (1, 8);
static int encode_iso20_EMAIDListType(exi_bitstream_t* stream, const struct iso20_EMAIDListType* EMAIDListType) {
    int grammar_id = 386;
    int done = 0;
    int error = 0;
    uint16_t EMAID_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 386:
            // Grammar: ID=386; read/write bits=1; START (EMAID)
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=387

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 387;
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
        case 387:
            // Grammar: ID=387; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=388

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 388;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 388:
            // Grammar: ID=388; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=389

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 389;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 389:
            // Grammar: ID=389; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=390

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 390;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 390:
            // Grammar: ID=390; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=391

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 391;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 391:
            // Grammar: ID=391; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=392

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 392;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 392:
            // Grammar: ID=392; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=393

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 393;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 393:
            // Grammar: ID=393; read/write bits=2; START (EMAID), END Element
            if (EMAID_currentIndex < EMAIDListType->EMAID.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, EMAIDListType->EMAID.array[EMAID_currentIndex].charactersLen, EMAIDListType->EMAID.array[EMAID_currentIndex].characters, iso20_EMAID_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                EMAID_currentIndex++;
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 2;
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
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SignedInstallationData; type={urn:iso:std:iso:15118:-20:CommonMessages}SignedInstallationDataType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (1, 1); ContractCertificateChain, ContractCertificateChainType (1, 1); ECDHCurve, ecdhCurveType (1, 1); DHPublicKey, dhPublicKeyType (1, 1); SECP521_EncryptedPrivateKey, secp521_EncryptedPrivateKeyType (0, 1); X448_EncryptedPrivateKey, x448_EncryptedPrivateKeyType (0, 1); TPM_EncryptedPrivateKey, tpm_EncryptedPrivateKeyType (0, 1);
static int encode_iso20_SignedInstallationDataType(exi_bitstream_t* stream, const struct iso20_SignedInstallationDataType* SignedInstallationDataType) {
    int grammar_id = 394;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 394:
            // Grammar: ID=394; read/write bits=1; START (Id)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (NCName); next=395

                // string should not be found in table, so add 2
                error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignedInstallationDataType->Id.charactersLen + 2));
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_characters(stream, SignedInstallationDataType->Id.charactersLen, SignedInstallationDataType->Id.characters, iso20_Id_CHARACTER_SIZE);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 395;
                    }
                }
            }
            break;
        case 395:
            // Grammar: ID=395; read/write bits=1; START (ContractCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ContractCertificateChainType); next=396
                error = encode_iso20_ContractCertificateChainType(stream, &SignedInstallationDataType->ContractCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 396;
                }
            }
            break;
        case 396:
            // Grammar: ID=396; read/write bits=1; START (ECDHCurve)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=397
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, SignedInstallationDataType->ECDHCurve);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 397;
                        }
                    }
                }
            }
            break;
        case 397:
            // Grammar: ID=397; read/write bits=1; START (DHPublicKey)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=398
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedInstallationDataType->DHPublicKey.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, SignedInstallationDataType->DHPublicKey.bytesLen, SignedInstallationDataType->DHPublicKey.bytes, iso20_dhPublicKeyType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 398;
                            }
                        }
                    }
                }
            }
            break;
        case 398:
            // Grammar: ID=398; read/write bits=2; START (SECP521_EncryptedPrivateKey), START (X448_EncryptedPrivateKey), START (TPM_EncryptedPrivateKey)
            if (SignedInstallationDataType->SECP521_EncryptedPrivateKey_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SECP521_EncryptedPrivateKey, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedInstallationDataType->SECP521_EncryptedPrivateKey.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignedInstallationDataType->SECP521_EncryptedPrivateKey.bytesLen, SignedInstallationDataType->SECP521_EncryptedPrivateKey.bytes, iso20_secp521_EncryptedPrivateKeyType_BYTES_SIZE);
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
                }
            }
            else if (SignedInstallationDataType->X448_EncryptedPrivateKey_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (X448_EncryptedPrivateKey, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedInstallationDataType->X448_EncryptedPrivateKey.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignedInstallationDataType->X448_EncryptedPrivateKey.bytesLen, SignedInstallationDataType->X448_EncryptedPrivateKey.bytes, iso20_x448_EncryptedPrivateKeyType_BYTES_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TPM_EncryptedPrivateKey, base64Binary); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)SignedInstallationDataType->TPM_EncryptedPrivateKey.bytesLen);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_bytes(stream, SignedInstallationDataType->TPM_EncryptedPrivateKey.bytesLen, SignedInstallationDataType->TPM_EncryptedPrivateKey.bytes, iso20_tpm_EncryptedPrivateKeyType_BYTES_SIZE);
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
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PnC_ASResAuthorizationMode; type={urn:iso:std:iso:15118:-20:CommonMessages}PnC_ASResAuthorizationModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: GenChallenge, genChallengeType (1, 1); SupportedProviders, SupportedProvidersListType (0, 1);
static int encode_iso20_PnC_ASResAuthorizationModeType(exi_bitstream_t* stream, const struct iso20_PnC_ASResAuthorizationModeType* PnC_ASResAuthorizationModeType) {
    int grammar_id = 399;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 399:
            // Grammar: ID=399; read/write bits=1; START (GenChallenge)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (base64Binary); next=400
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)PnC_ASResAuthorizationModeType->GenChallenge.bytesLen);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bytes(stream, PnC_ASResAuthorizationModeType->GenChallenge.bytesLen, PnC_ASResAuthorizationModeType->GenChallenge.bytes, iso20_genChallengeType_BYTES_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 400;
                            }
                        }
                    }
                }
            }
            break;
        case 400:
            // Grammar: ID=400; read/write bits=2; START (SupportedProviders), END Element
            if (PnC_ASResAuthorizationModeType->SupportedProviders_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SupportedProviders, SupportedProvidersListType); next=2
                    error = encode_iso20_SupportedProvidersListType(stream, &PnC_ASResAuthorizationModeType->SupportedProviders);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEResControlMode; type={urn:iso:std:iso:15118:-20:CommonMessages}Scheduled_SEResControlModeType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: ScheduleTuple, ScheduleTupleType (1, 3);
static int encode_iso20_Scheduled_SEResControlModeType(exi_bitstream_t* stream, const struct iso20_Scheduled_SEResControlModeType* Scheduled_SEResControlModeType) {
    int grammar_id = 401;
    int done = 0;
    int error = 0;
    uint16_t ScheduleTuple_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 401:
            // Grammar: ID=401; read/write bits=1; START (ScheduleTuple)
            if (ScheduleTuple_currentIndex < Scheduled_SEResControlModeType->ScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ScheduleTupleType); next=402
                    error = encode_iso20_ScheduleTupleType(stream, &Scheduled_SEResControlModeType->ScheduleTuple.array[ScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 402;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 402:
            // Grammar: ID=402; read/write bits=2; START (ScheduleTuple), END Element
            if (ScheduleTuple_currentIndex < Scheduled_SEResControlModeType->ScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ScheduleTupleType); next=403
                    error = encode_iso20_ScheduleTupleType(stream, &Scheduled_SEResControlModeType->ScheduleTuple.array[ScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 403;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 403:
            // Grammar: ID=403; read/write bits=2; START (ScheduleTuple), END Element
            if (ScheduleTuple_currentIndex < Scheduled_SEResControlModeType->ScheduleTuple.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ScheduleTupleType); next=2
                    error = encode_iso20_ScheduleTupleType(stream, &Scheduled_SEResControlModeType->ScheduleTuple.array[ScheduleTuple_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupReq; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCCID, identifierType (1, 1);
static int encode_iso20_SessionSetupReqType(exi_bitstream_t* stream, const struct iso20_SessionSetupReqType* SessionSetupReqType) {
    int grammar_id = 404;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 404:
            // Grammar: ID=404; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=405
                error = encode_iso20_MessageHeaderType(stream, &SessionSetupReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 405;
                }
            }
            break;
        case 405:
            // Grammar: ID=405; read/write bits=1; START (EVCCID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionSetupReqType->EVCCID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SessionSetupReqType->EVCCID.charactersLen, SessionSetupReqType->EVCCID.characters, iso20_EVCCID_CHARACTER_SIZE);
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
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupRes; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEID, identifierType (1, 1);
static int encode_iso20_SessionSetupResType(exi_bitstream_t* stream, const struct iso20_SessionSetupResType* SessionSetupResType) {
    int grammar_id = 406;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 406:
            // Grammar: ID=406; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=407
                error = encode_iso20_MessageHeaderType(stream, &SessionSetupResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 407;
                }
            }
            break;
        case 407:
            // Grammar: ID=407; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=408
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, SessionSetupResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 408;
                        }
                    }
                }
            }
            break;
        case 408:
            // Grammar: ID=408; read/write bits=1; START (EVSEID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2

                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionSetupResType->EVSEID.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SessionSetupResType->EVSEID.charactersLen, SessionSetupResType->EVSEID.characters, iso20_EVSEID_CHARACTER_SIZE);
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
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupReq; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1);
static int encode_iso20_AuthorizationSetupReqType(exi_bitstream_t* stream, const struct iso20_AuthorizationSetupReqType* AuthorizationSetupReqType) {
    int grammar_id = 409;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 409:
            // Grammar: ID=409; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=2
                error = encode_iso20_MessageHeaderType(stream, &AuthorizationSetupReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupRes; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationSetupResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); AuthorizationServices, authorizationType (1, 2); CertificateInstallationService, boolean (1, 1); EIM_ASResAuthorizationMode, EIM_ASResAuthorizationModeType (0, 1); PnC_ASResAuthorizationMode, PnC_ASResAuthorizationModeType (0, 1);
static int encode_iso20_AuthorizationSetupResType(exi_bitstream_t* stream, const struct iso20_AuthorizationSetupResType* AuthorizationSetupResType) {
    int grammar_id = 410;
    int done = 0;
    int error = 0;
    uint16_t AuthorizationServices_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 410:
            // Grammar: ID=410; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=411
                error = encode_iso20_MessageHeaderType(stream, &AuthorizationSetupResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 411;
                }
            }
            break;
        case 411:
            // Grammar: ID=411; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=412
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, AuthorizationSetupResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 412;
                        }
                    }
                }
            }
            break;
        case 412:
            // Grammar: ID=412; read/write bits=1; START (AuthorizationServices)
            if (AuthorizationServices_currentIndex < AuthorizationSetupResType->AuthorizationServices.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=413
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, AuthorizationSetupResType->AuthorizationServices.array[AuthorizationServices_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 413;
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
        case 413:
            // Grammar: ID=413; read/write bits=2; START (AuthorizationServices), START (CertificateInstallationService)
            if (AuthorizationServices_currentIndex < AuthorizationSetupResType->AuthorizationServices.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (string); next=414
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, AuthorizationSetupResType->AuthorizationServices.array[AuthorizationServices_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 414;
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
                    // Event: START (CertificateInstallationService, boolean); next=415
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, AuthorizationSetupResType->CertificateInstallationService);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 415;
                            }
                        }
                    }
                }
            }
            break;
        case 414:
            // Grammar: ID=414; read/write bits=1; START (CertificateInstallationService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=415
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, AuthorizationSetupResType->CertificateInstallationService);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 415;
                        }
                    }
                }
            }
            break;
        case 415:
            // Grammar: ID=415; read/write bits=2; START (EIM_ASResAuthorizationMode), START (PnC_ASResAuthorizationMode)
            if (AuthorizationSetupResType->EIM_ASResAuthorizationMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EIM_ASResAuthorizationMode, EIM_ASResAuthorizationModeType); next=2
                    error = encode_iso20_EIM_ASResAuthorizationModeType(stream, &AuthorizationSetupResType->EIM_ASResAuthorizationMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PnC_ASResAuthorizationMode, PnC_ASResAuthorizationModeType); next=2
                    error = encode_iso20_PnC_ASResAuthorizationModeType(stream, &AuthorizationSetupResType->PnC_ASResAuthorizationMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SelectedAuthorizationService, authorizationType (1, 1); EIM_AReqAuthorizationMode, EIM_AReqAuthorizationModeType (0, 1); PnC_AReqAuthorizationMode, PnC_AReqAuthorizationModeType (0, 1);
static int encode_iso20_AuthorizationReqType(exi_bitstream_t* stream, const struct iso20_AuthorizationReqType* AuthorizationReqType) {
    int grammar_id = 416;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 416:
            // Grammar: ID=416; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=417
                error = encode_iso20_MessageHeaderType(stream, &AuthorizationReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 417;
                }
            }
            break;
        case 417:
            // Grammar: ID=417; read/write bits=1; START (SelectedAuthorizationService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=418
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, AuthorizationReqType->SelectedAuthorizationService);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 418;
                        }
                    }
                }
            }
            break;
        case 418:
            // Grammar: ID=418; read/write bits=2; START (EIM_AReqAuthorizationMode), START (PnC_AReqAuthorizationMode)
            if (AuthorizationReqType->EIM_AReqAuthorizationMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EIM_AReqAuthorizationMode, EIM_AReqAuthorizationModeType); next=2
                    error = encode_iso20_EIM_AReqAuthorizationModeType(stream, &AuthorizationReqType->EIM_AReqAuthorizationMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PnC_AReqAuthorizationMode, PnC_AReqAuthorizationModeType); next=2
                    error = encode_iso20_PnC_AReqAuthorizationModeType(stream, &AuthorizationReqType->PnC_AReqAuthorizationMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}AuthorizationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1);
static int encode_iso20_AuthorizationResType(exi_bitstream_t* stream, const struct iso20_AuthorizationResType* AuthorizationResType) {
    int grammar_id = 419;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 419:
            // Grammar: ID=419; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=420
                error = encode_iso20_MessageHeaderType(stream, &AuthorizationResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 420;
                }
            }
            break;
        case 420:
            // Grammar: ID=420; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=421
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, AuthorizationResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 421;
                        }
                    }
                }
            }
            break;
        case 421:
            // Grammar: ID=421; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
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
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SupportedServiceIDs, ServiceIDListType (0, 1);
static int encode_iso20_ServiceDiscoveryReqType(exi_bitstream_t* stream, const struct iso20_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    int grammar_id = 422;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 422:
            // Grammar: ID=422; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=423
                error = encode_iso20_MessageHeaderType(stream, &ServiceDiscoveryReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 423;
                }
            }
            break;
        case 423:
            // Grammar: ID=423; read/write bits=2; START (SupportedServiceIDs), END Element
            if (ServiceDiscoveryReqType->SupportedServiceIDs_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SupportedServiceIDs, ServiceIDListType); next=2
                    error = encode_iso20_ServiceIDListType(stream, &ServiceDiscoveryReqType->SupportedServiceIDs);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDiscoveryResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ServiceRenegotiationSupported, boolean (1, 1); EnergyTransferServiceList, ServiceListType (1, 1); VASList, ServiceListType (0, 1);
static int encode_iso20_ServiceDiscoveryResType(exi_bitstream_t* stream, const struct iso20_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    int grammar_id = 424;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 424:
            // Grammar: ID=424; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=425
                error = encode_iso20_MessageHeaderType(stream, &ServiceDiscoveryResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 425;
                }
            }
            break;
        case 425:
            // Grammar: ID=425; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=426
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, ServiceDiscoveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 426;
                        }
                    }
                }
            }
            break;
        case 426:
            // Grammar: ID=426; read/write bits=1; START (ServiceRenegotiationSupported)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (boolean); next=427
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_bool(stream, ServiceDiscoveryResType->ServiceRenegotiationSupported);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 427;
                        }
                    }
                }
            }
            break;
        case 427:
            // Grammar: ID=427; read/write bits=1; START (EnergyTransferServiceList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceListType); next=428
                error = encode_iso20_ServiceListType(stream, &ServiceDiscoveryResType->EnergyTransferServiceList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 428;
                }
            }
            break;
        case 428:
            // Grammar: ID=428; read/write bits=2; START (VASList), END Element
            if (ServiceDiscoveryResType->VASList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VASList, ServiceListType); next=2
                    error = encode_iso20_ServiceListType(stream, &ServiceDiscoveryResType->VASList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ServiceID, serviceIDType (1, 1);
static int encode_iso20_ServiceDetailReqType(exi_bitstream_t* stream, const struct iso20_ServiceDetailReqType* ServiceDetailReqType) {
    int grammar_id = 429;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 429:
            // Grammar: ID=429; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=430
                error = encode_iso20_MessageHeaderType(stream, &ServiceDetailReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 430;
                }
            }
            break;
        case 430:
            // Grammar: ID=430; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=2
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
                            grammar_id = 2;
                        }
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceDetailResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ServiceID, serviceIDType (1, 1); ServiceParameterList, ServiceParameterListType (1, 1);
static int encode_iso20_ServiceDetailResType(exi_bitstream_t* stream, const struct iso20_ServiceDetailResType* ServiceDetailResType) {
    int grammar_id = 431;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 431:
            // Grammar: ID=431; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=432
                error = encode_iso20_MessageHeaderType(stream, &ServiceDetailResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 432;
                }
            }
            break;
        case 432:
            // Grammar: ID=432; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=433
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, ServiceDetailResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 433;
                        }
                    }
                }
            }
            break;
        case 433:
            // Grammar: ID=433; read/write bits=1; START (ServiceID)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=434
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
                            grammar_id = 434;
                        }
                    }
                }
            }
            break;
        case 434:
            // Grammar: ID=434; read/write bits=1; START (ServiceParameterList)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ServiceParameterListType); next=2
                error = encode_iso20_ServiceParameterListType(stream, &ServiceDetailResType->ServiceParameterList);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SelectedEnergyTransferService, SelectedServiceType (1, 1); SelectedVASList, SelectedServiceListType (0, 1);
static int encode_iso20_ServiceSelectionReqType(exi_bitstream_t* stream, const struct iso20_ServiceSelectionReqType* ServiceSelectionReqType) {
    int grammar_id = 435;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 435:
            // Grammar: ID=435; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=436
                error = encode_iso20_MessageHeaderType(stream, &ServiceSelectionReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 436;
                }
            }
            break;
        case 436:
            // Grammar: ID=436; read/write bits=1; START (SelectedEnergyTransferService)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SelectedServiceType); next=437
                error = encode_iso20_SelectedServiceType(stream, &ServiceSelectionReqType->SelectedEnergyTransferService);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 437;
                }
            }
            break;
        case 437:
            // Grammar: ID=437; read/write bits=2; START (SelectedVASList), END Element
            if (ServiceSelectionReqType->SelectedVASList_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SelectedVASList, SelectedServiceListType); next=2
                    error = encode_iso20_SelectedServiceListType(stream, &ServiceSelectionReqType->SelectedVASList);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ServiceSelectionResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
static int encode_iso20_ServiceSelectionResType(exi_bitstream_t* stream, const struct iso20_ServiceSelectionResType* ServiceSelectionResType) {
    int grammar_id = 438;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 438:
            // Grammar: ID=438; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=439
                error = encode_iso20_MessageHeaderType(stream, &ServiceSelectionResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 439;
                }
            }
            break;
        case 439:
            // Grammar: ID=439; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, ServiceSelectionResType->ResponseCode);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeReq; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); MaximumSupportingPoints, maxSupportingPointsScheduleTupleType (1, 1); Dynamic_SEReqControlMode, Dynamic_SEReqControlModeType (0, 1); Scheduled_SEReqControlMode, Scheduled_SEReqControlModeType (0, 1);
static int encode_iso20_ScheduleExchangeReqType(exi_bitstream_t* stream, const struct iso20_ScheduleExchangeReqType* ScheduleExchangeReqType) {
    int grammar_id = 440;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 440:
            // Grammar: ID=440; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=441
                error = encode_iso20_MessageHeaderType(stream, &ScheduleExchangeReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 441;
                }
            }
            break;
        case 441:
            // Grammar: ID=441; read/write bits=1; START (MaximumSupportingPoints)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=442
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 10, (uint32_t)ScheduleExchangeReqType->MaximumSupportingPoints - 12);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 442;
                        }
                    }
                }
            }
            break;
        case 442:
            // Grammar: ID=442; read/write bits=2; START (Dynamic_SEReqControlMode), START (Scheduled_SEReqControlMode)
            if (ScheduleExchangeReqType->Dynamic_SEReqControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_SEReqControlMode, Dynamic_SEReqControlModeType); next=2
                    error = encode_iso20_Dynamic_SEReqControlModeType(stream, &ScheduleExchangeReqType->Dynamic_SEReqControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_SEReqControlMode, Scheduled_SEReqControlModeType); next=2
                    error = encode_iso20_Scheduled_SEReqControlModeType(stream, &ScheduleExchangeReqType->Scheduled_SEReqControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeRes; type={urn:iso:std:iso:15118:-20:CommonMessages}ScheduleExchangeResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); GoToPause, boolean (0, 1); Dynamic_SEResControlMode, Dynamic_SEResControlModeType (0, 1); Scheduled_SEResControlMode, Scheduled_SEResControlModeType (0, 1);
static int encode_iso20_ScheduleExchangeResType(exi_bitstream_t* stream, const struct iso20_ScheduleExchangeResType* ScheduleExchangeResType) {
    int grammar_id = 443;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 443:
            // Grammar: ID=443; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=444
                error = encode_iso20_MessageHeaderType(stream, &ScheduleExchangeResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 444;
                }
            }
            break;
        case 444:
            // Grammar: ID=444; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=445
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, ScheduleExchangeResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 445;
                        }
                    }
                }
            }
            break;
        case 445:
            // Grammar: ID=445; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=446
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, ScheduleExchangeResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 446;
                        }
                    }
                }
            }
            break;
        case 446:
            // Grammar: ID=446; read/write bits=2; START (GoToPause), START (Dynamic_SEResControlMode), START (Scheduled_SEResControlMode)
            if (ScheduleExchangeResType->GoToPause_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (GoToPause, boolean); next=447
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_bool(stream, ScheduleExchangeResType->GoToPause);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 447;
                            }
                        }
                    }
                }
            }
            else if (ScheduleExchangeResType->Dynamic_SEResControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_SEResControlMode, Dynamic_SEResControlModeType); next=2
                    error = encode_iso20_Dynamic_SEResControlModeType(stream, &ScheduleExchangeResType->Dynamic_SEResControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_SEResControlMode, Scheduled_SEResControlModeType); next=2
                    error = encode_iso20_Scheduled_SEResControlModeType(stream, &ScheduleExchangeResType->Scheduled_SEResControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 447:
            // Grammar: ID=447; read/write bits=2; START (Dynamic_SEResControlMode), START (Scheduled_SEResControlMode)
            if (ScheduleExchangeResType->Dynamic_SEResControlMode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Dynamic_SEResControlMode, Dynamic_SEResControlModeType); next=2
                    error = encode_iso20_Dynamic_SEResControlModeType(stream, &ScheduleExchangeResType->Dynamic_SEResControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Scheduled_SEResControlMode, Scheduled_SEResControlModeType); next=2
                    error = encode_iso20_Scheduled_SEResControlModeType(stream, &ScheduleExchangeResType->Scheduled_SEResControlMode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryReq; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVProcessing, processingType (1, 1); ChargeProgress, chargeProgressType (1, 1); EVPowerProfile, EVPowerProfileType (0, 1); BPT_ChannelSelection, channelSelectionType (0, 1);
static int encode_iso20_PowerDeliveryReqType(exi_bitstream_t* stream, const struct iso20_PowerDeliveryReqType* PowerDeliveryReqType) {
    int grammar_id = 448;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 448:
            // Grammar: ID=448; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=449
                error = encode_iso20_MessageHeaderType(stream, &PowerDeliveryReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 449;
                }
            }
            break;
        case 449:
            // Grammar: ID=449; read/write bits=1; START (EVProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=450
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, PowerDeliveryReqType->EVProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 450;
                        }
                    }
                }
            }
            break;
        case 450:
            // Grammar: ID=450; read/write bits=1; START (ChargeProgress)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=451
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
                            grammar_id = 451;
                        }
                    }
                }
            }
            break;
        case 451:
            // Grammar: ID=451; read/write bits=2; START (EVPowerProfile), START (BPT_ChannelSelection), END Element
            if (PowerDeliveryReqType->EVPowerProfile_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVPowerProfile, EVPowerProfileType); next=452
                    error = encode_iso20_EVPowerProfileType(stream, &PowerDeliveryReqType->EVPowerProfile);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 452;
                    }
                }
            }
            else if (PowerDeliveryReqType->BPT_ChannelSelection_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_ChannelSelection, string); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PowerDeliveryReqType->BPT_ChannelSelection);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 452:
            // Grammar: ID=452; read/write bits=2; START (BPT_ChannelSelection), END Element
            if (PowerDeliveryReqType->BPT_ChannelSelection_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (BPT_ChannelSelection, string); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, PowerDeliveryReqType->BPT_ChannelSelection);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryRes; type={urn:iso:std:iso:15118:-20:CommonMessages}PowerDeliveryResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEStatus, EVSEStatusType (0, 1);
static int encode_iso20_PowerDeliveryResType(exi_bitstream_t* stream, const struct iso20_PowerDeliveryResType* PowerDeliveryResType) {
    int grammar_id = 453;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 453:
            // Grammar: ID=453; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=454
                error = encode_iso20_MessageHeaderType(stream, &PowerDeliveryResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 454;
                }
            }
            break;
        case 454:
            // Grammar: ID=454; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=455
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, PowerDeliveryResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 455;
                        }
                    }
                }
            }
            break;
        case 455:
            // Grammar: ID=455; read/write bits=2; START (EVSEStatus), END Element
            if (PowerDeliveryResType->EVSEStatus_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVSEStatus, EVSEStatusType); next=2
                    error = encode_iso20_EVSEStatusType(stream, &PowerDeliveryResType->EVSEStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); SignedMeteringData, SignedMeteringDataType (1, 1);
static int encode_iso20_MeteringConfirmationReqType(exi_bitstream_t* stream, const struct iso20_MeteringConfirmationReqType* MeteringConfirmationReqType) {
    int grammar_id = 456;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 456:
            // Grammar: ID=456; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=457
                error = encode_iso20_MessageHeaderType(stream, &MeteringConfirmationReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 457;
                }
            }
            break;
        case 457:
            // Grammar: ID=457; read/write bits=1; START (SignedMeteringData)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedMeteringDataType); next=2
                error = encode_iso20_SignedMeteringDataType(stream, &MeteringConfirmationReqType->SignedMeteringData);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 2;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}MeteringConfirmationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
static int encode_iso20_MeteringConfirmationResType(exi_bitstream_t* stream, const struct iso20_MeteringConfirmationResType* MeteringConfirmationResType) {
    int grammar_id = 458;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 458:
            // Grammar: ID=458; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=459
                error = encode_iso20_MessageHeaderType(stream, &MeteringConfirmationResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 459;
                }
            }
            break;
        case 459:
            // Grammar: ID=459; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, MeteringConfirmationResType->ResponseCode);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopReq; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ChargingSession, chargingSessionType (1, 1); EVTerminationCode, nameType (0, 1); EVTerminationExplanation, descriptionType (0, 1);
static int encode_iso20_SessionStopReqType(exi_bitstream_t* stream, const struct iso20_SessionStopReqType* SessionStopReqType) {
    int grammar_id = 460;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 460:
            // Grammar: ID=460; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=461
                error = encode_iso20_MessageHeaderType(stream, &SessionStopReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 461;
                }
            }
            break;
        case 461:
            // Grammar: ID=461; read/write bits=1; START (ChargingSession)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=462
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, SessionStopReqType->ChargingSession);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 462;
                        }
                    }
                }
            }
            break;
        case 462:
            // Grammar: ID=462; read/write bits=2; START (EVTerminationCode), START (EVTerminationExplanation), END Element
            if (SessionStopReqType->EVTerminationCode_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTerminationCode, string); next=463

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionStopReqType->EVTerminationCode.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SessionStopReqType->EVTerminationCode.charactersLen, SessionStopReqType->EVTerminationCode.characters, iso20_EVTerminationCode_CHARACTER_SIZE);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                // encode END Element
                                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                                if (error == EXI_ERROR__NO_ERROR)
                                {
                                    grammar_id = 463;
                                }
                            }
                        }
                    }
                }
            }
            else if (SessionStopReqType->EVTerminationExplanation_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTerminationExplanation, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionStopReqType->EVTerminationExplanation.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SessionStopReqType->EVTerminationExplanation.charactersLen, SessionStopReqType->EVTerminationExplanation.characters, iso20_EVTerminationExplanation_CHARACTER_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 463:
            // Grammar: ID=463; read/write bits=2; START (EVTerminationExplanation), END Element
            if (SessionStopReqType->EVTerminationExplanation_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (EVTerminationExplanation, string); next=2

                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // string should not be found in table, so add 2
                        error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SessionStopReqType->EVTerminationExplanation.charactersLen + 2));
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            error = exi_basetypes_encoder_characters(stream, SessionStopReqType->EVTerminationExplanation.charactersLen, SessionStopReqType->EVTerminationExplanation.characters, iso20_EVTerminationExplanation_CHARACTER_SIZE);
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
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopRes; type={urn:iso:std:iso:15118:-20:CommonMessages}SessionStopResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1);
static int encode_iso20_SessionStopResType(exi_bitstream_t* stream, const struct iso20_SessionStopResType* SessionStopResType) {
    int grammar_id = 464;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 464:
            // Grammar: ID=464; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=465
                error = encode_iso20_MessageHeaderType(stream, &SessionStopResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 465;
                }
            }
            break;
        case 465:
            // Grammar: ID=465; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, SessionStopResType->ResponseCode);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationReq; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); OEMProvisioningCertificateChain, SignedCertificateChainType (1, 1); ListOfRootCertificateIDs, ListOfRootCertificateIDsType (1, 1); MaximumContractCertificateChains, unsignedByte (1, 1); PrioritizedEMAIDs, EMAIDListType (0, 1);
static int encode_iso20_CertificateInstallationReqType(exi_bitstream_t* stream, const struct iso20_CertificateInstallationReqType* CertificateInstallationReqType) {
    int grammar_id = 466;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 466:
            // Grammar: ID=466; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=467
                error = encode_iso20_MessageHeaderType(stream, &CertificateInstallationReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 467;
                }
            }
            break;
        case 467:
            // Grammar: ID=467; read/write bits=1; START (OEMProvisioningCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedCertificateChainType); next=468
                error = encode_iso20_SignedCertificateChainType(stream, &CertificateInstallationReqType->OEMProvisioningCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 468;
                }
            }
            break;
        case 468:
            // Grammar: ID=468; read/write bits=1; START (ListOfRootCertificateIDs)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (ListOfRootCertificateIDsType); next=469
                error = encode_iso20_ListOfRootCertificateIDsType(stream, &CertificateInstallationReqType->ListOfRootCertificateIDs);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 469;
                }
            }
            break;
        case 469:
            // Grammar: ID=469; read/write bits=1; START (MaximumContractCertificateChains)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=470
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, CertificateInstallationReqType->MaximumContractCertificateChains);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 470;
                        }
                    }
                }
            }
            break;
        case 470:
            // Grammar: ID=470; read/write bits=2; START (PrioritizedEMAIDs), END Element
            if (CertificateInstallationReqType->PrioritizedEMAIDs_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (PrioritizedEMAIDs, EMAIDListType); next=2
                    error = encode_iso20_EMAIDListType(stream, &CertificateInstallationReqType->PrioritizedEMAIDs);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationRes; type={urn:iso:std:iso:15118:-20:CommonMessages}CertificateInstallationResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSEProcessing, processingType (1, 1); CPSCertificateChain, CertificateChainType (1, 1); SignedInstallationData, SignedInstallationDataType (1, 1); RemainingContractCertificateChains, unsignedByte (1, 1);
static int encode_iso20_CertificateInstallationResType(exi_bitstream_t* stream, const struct iso20_CertificateInstallationResType* CertificateInstallationResType) {
    int grammar_id = 471;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 471:
            // Grammar: ID=471; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=472
                error = encode_iso20_MessageHeaderType(stream, &CertificateInstallationResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 472;
                }
            }
            break;
        case 472:
            // Grammar: ID=472; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=473
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, CertificateInstallationResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 473;
                        }
                    }
                }
            }
            break;
        case 473:
            // Grammar: ID=473; read/write bits=1; START (EVSEProcessing)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=474
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, CertificateInstallationResType->EVSEProcessing);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 474;
                        }
                    }
                }
            }
            break;
        case 474:
            // Grammar: ID=474; read/write bits=1; START (CPSCertificateChain)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (CertificateChainType); next=475
                error = encode_iso20_CertificateChainType(stream, &CertificateInstallationResType->CPSCertificateChain);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 475;
                }
            }
            break;
        case 475:
            // Grammar: ID=475; read/write bits=1; START (SignedInstallationData)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignedInstallationDataType); next=476
                error = encode_iso20_SignedInstallationDataType(stream, &CertificateInstallationResType->SignedInstallationData);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 476;
                }
            }
            break;
        case 476:
            // Grammar: ID=476; read/write bits=1; START (RemainingContractCertificateChains)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (unsignedShort); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 8, CertificateInstallationResType->RemainingContractCertificateChains);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInReq; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCheckInStatus, evCheckInStatusType (1, 1); ParkingMethod, parkingMethodType (1, 1); VehicleFrame, short (0, 1); DeviceOffset, short (0, 1); VehicleTravel, short (0, 1);
static int encode_iso20_VehicleCheckInReqType(exi_bitstream_t* stream, const struct iso20_VehicleCheckInReqType* VehicleCheckInReqType) {
    int grammar_id = 477;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 477:
            // Grammar: ID=477; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=478
                error = encode_iso20_MessageHeaderType(stream, &VehicleCheckInReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 478;
                }
            }
            break;
        case 478:
            // Grammar: ID=478; read/write bits=1; START (EVCheckInStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=479
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, VehicleCheckInReqType->EVCheckInStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 479;
                        }
                    }
                }
            }
            break;
        case 479:
            // Grammar: ID=479; read/write bits=1; START (ParkingMethod)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=480
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, VehicleCheckInReqType->ParkingMethod);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 480;
                        }
                    }
                }
            }
            break;
        case 480:
            // Grammar: ID=480; read/write bits=3; START (VehicleFrame), START (DeviceOffset), START (VehicleTravel), END Element
            if (VehicleCheckInReqType->VehicleFrame_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VehicleFrame, int); next=481
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->VehicleFrame);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 481;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInReqType->DeviceOffset_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DeviceOffset, int); next=482
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->DeviceOffset);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 482;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInReqType->VehicleTravel_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VehicleTravel, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->VehicleTravel);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 481:
            // Grammar: ID=481; read/write bits=2; START (DeviceOffset), START (VehicleTravel), END Element
            if (VehicleCheckInReqType->DeviceOffset_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DeviceOffset, int); next=482
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->DeviceOffset);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 482;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInReqType->VehicleTravel_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VehicleTravel, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->VehicleTravel);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 482:
            // Grammar: ID=482; read/write bits=2; START (VehicleTravel), END Element
            if (VehicleCheckInReqType->VehicleTravel_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (VehicleTravel, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInReqType->VehicleTravel);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInRes; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckInResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); ParkingSpace, short (0, 1); DeviceLocation, short (0, 1); TargetDistance, short (0, 1);
static int encode_iso20_VehicleCheckInResType(exi_bitstream_t* stream, const struct iso20_VehicleCheckInResType* VehicleCheckInResType) {
    int grammar_id = 483;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 483:
            // Grammar: ID=483; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=484
                error = encode_iso20_MessageHeaderType(stream, &VehicleCheckInResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 484;
                }
            }
            break;
        case 484:
            // Grammar: ID=484; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=485
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, VehicleCheckInResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 485;
                        }
                    }
                }
            }
            break;
        case 485:
            // Grammar: ID=485; read/write bits=3; START (ParkingSpace), START (DeviceLocation), START (TargetDistance), END Element
            if (VehicleCheckInResType->ParkingSpace_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ParkingSpace, int); next=486
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->ParkingSpace);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 486;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInResType->DeviceLocation_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DeviceLocation, int); next=487
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->DeviceLocation);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 487;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInResType->TargetDistance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetDistance, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->TargetDistance);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 3, 3);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 486:
            // Grammar: ID=486; read/write bits=2; START (DeviceLocation), START (TargetDistance), END Element
            if (VehicleCheckInResType->DeviceLocation_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (DeviceLocation, int); next=487
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->DeviceLocation);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            // encode END Element
                            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                            if (error == EXI_ERROR__NO_ERROR)
                            {
                                grammar_id = 487;
                            }
                        }
                    }
                }
            }
            else if (VehicleCheckInResType->TargetDistance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetDistance, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->TargetDistance);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 2);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 487:
            // Grammar: ID=487; read/write bits=2; START (TargetDistance), END Element
            if (VehicleCheckInResType->TargetDistance_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (TargetDistance, int); next=2
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_integer_16(stream, VehicleCheckInResType->TargetDistance);
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
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutReq; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutReqType; base type=V2GRequestType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); EVCheckOutStatus, evCheckOutStatusType (1, 1); CheckOutTime, unsignedLong (1, 1);
static int encode_iso20_VehicleCheckOutReqType(exi_bitstream_t* stream, const struct iso20_VehicleCheckOutReqType* VehicleCheckOutReqType) {
    int grammar_id = 488;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 488:
            // Grammar: ID=488; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=489
                error = encode_iso20_MessageHeaderType(stream, &VehicleCheckOutReqType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 489;
                }
            }
            break;
        case 489:
            // Grammar: ID=489; read/write bits=1; START (EVCheckOutStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=490
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, VehicleCheckOutReqType->EVCheckOutStatus);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 490;
                        }
                    }
                }
            }
            break;
        case 490:
            // Grammar: ID=490; read/write bits=1; START (CheckOutTime)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (nonNegativeInteger); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_uint_64(stream, VehicleCheckOutReqType->CheckOutTime);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutRes; type={urn:iso:std:iso:15118:-20:CommonMessages}VehicleCheckOutResType; base type=V2GResponseType; content type=ELEMENT-ONLY;
//          abstract=False; final=False; derivation=extension;
// Particle: Header, MessageHeaderType (1, 1); ResponseCode, responseCodeType (1, 1); EVSECheckOutStatus, evseCheckOutStatusType (1, 1);
static int encode_iso20_VehicleCheckOutResType(exi_bitstream_t* stream, const struct iso20_VehicleCheckOutResType* VehicleCheckOutResType) {
    int grammar_id = 491;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 491:
            // Grammar: ID=491; read/write bits=1; START (Header)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (MessageHeaderType); next=492
                error = encode_iso20_MessageHeaderType(stream, &VehicleCheckOutResType->Header);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 492;
                }
            }
            break;
        case 492:
            // Grammar: ID=492; read/write bits=1; START (ResponseCode)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=493
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 6, VehicleCheckOutResType->ResponseCode);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // encode END Element
                        error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 493;
                        }
                    }
                }
            }
            break;
        case 493:
            // Grammar: ID=493; read/write bits=1; START (EVSECheckOutStatus)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (string); next=2
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 1, VehicleCheckOutResType->EVSECheckOutStatus);
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
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLReqControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_CLReqControlModeType(exi_bitstream_t* stream, const struct iso20_CLReqControlModeType* CLReqControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)CLReqControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlMode; type={urn:iso:std:iso:15118:-20:CommonTypes}CLResControlModeType; base type=; content type=empty;
//          abstract=False; final=False;
static int encode_iso20_CLResControlModeType(exi_bitstream_t* stream, const struct iso20_CLResControlModeType* CLResControlModeType) {
    // Element has no particles, so the function just encodes END Element
    (void)CLResControlModeType;

    int error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);

    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}Manifest; type={http://www.w3.org/2000/09/xmldsig#}ManifestType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); Reference, ReferenceType (1, 4);
static int encode_iso20_ManifestType(exi_bitstream_t* stream, const struct iso20_ManifestType* ManifestType) {
    int grammar_id = 494;
    int done = 0;
    int error = 0;
    uint16_t Reference_currentIndex = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 494:
            // Grammar: ID=494; read/write bits=2; START (Id), START (Reference)
            if (ManifestType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=499

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(ManifestType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, ManifestType->Id.charactersLen, ManifestType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 499;
                        }
                    }
                }
            }
            else
            {
                if (Reference_currentIndex < ManifestType->Reference.arrayLen)
                {
                    error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        // Event: START (ReferenceType); next=495
                        error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 495;
                        }
                    }
                }
            }
            break;
        case 495:
            // Grammar: ID=495; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=496
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 496;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 496:
            // Grammar: ID=496; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=497
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 497;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 497:
            // Grammar: ID=497; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=498
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 498;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 498:
            // Grammar: ID=498; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=2
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 499:
            // Grammar: ID=499; read/write bits=1; START (Reference)
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=500
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 500;
                    }
                }
            }
            else
            {
                error = EXI_ERROR__UNKNOWN_EVENT_CODE;
            }
            break;
        case 500:
            // Grammar: ID=500; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=501
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 501;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 501:
            // Grammar: ID=501; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=502
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 502;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 502:
            // Grammar: ID=502; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=503
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 503;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 503:
            // Grammar: ID=503; read/write bits=2; START (Reference), END Element
            if (Reference_currentIndex < ManifestType->Reference.arrayLen)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (ReferenceType); next=2
                    error = encode_iso20_ReferenceType(stream, &ManifestType->Reference.array[Reference_currentIndex++]);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
            break;
        }

        if (error)
        {
            done = 1;
        }
    }
    return error;
}

// Element: definition=complex; name={http://www.w3.org/2000/09/xmldsig#}SignatureProperties; type={http://www.w3.org/2000/09/xmldsig#}SignaturePropertiesType; base type=; content type=ELEMENT-ONLY;
//          abstract=False; final=False;
// Particle: Id, ID (0, 1); SignatureProperty, SignaturePropertyType (1, 1);
static int encode_iso20_SignaturePropertiesType(exi_bitstream_t* stream, const struct iso20_SignaturePropertiesType* SignaturePropertiesType) {
    int grammar_id = 504;
    int done = 0;
    int error = 0;

    while(!done)
    {
        switch(grammar_id)
        {
        case 504:
            // Grammar: ID=504; read/write bits=2; START (Id), START (SignatureProperty)
            if (SignaturePropertiesType->Id_isUsed == 1u)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (Id, NCName); next=506

                    // string should not be found in table, so add 2
                    error = exi_basetypes_encoder_uint_16(stream, (uint16_t)(SignaturePropertiesType->Id.charactersLen + 2));
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        error = exi_basetypes_encoder_characters(stream, SignaturePropertiesType->Id.charactersLen, SignaturePropertiesType->Id.characters, iso20_Id_CHARACTER_SIZE);
                        if (error == EXI_ERROR__NO_ERROR)
                        {
                            grammar_id = 506;
                        }
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=505
                    error = encode_iso20_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 505;
                    }
                }
            }
            break;
        case 505:
            // Grammar: ID=505; read/write bits=2; START (SignatureProperty), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=2
                    error = encode_iso20_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 506:
            // Grammar: ID=506; read/write bits=1; START (SignatureProperty)
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: START (SignaturePropertyType); next=507
                error = encode_iso20_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    grammar_id = 507;
                }
            }
            break;
        case 507:
            // Grammar: ID=507; read/write bits=2; START (SignatureProperty), END Element
            if (1 == 0)
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 0);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: START (SignatureProperty, SignaturePropertyType); next=2
                    error = encode_iso20_SignaturePropertyType(stream, &SignaturePropertiesType->SignatureProperty);
                    if (error == EXI_ERROR__NO_ERROR)
                    {
                        grammar_id = 2;
                    }
                }
            }
            else
            {
                error = exi_basetypes_encoder_nbit_uint(stream, 2, 1);
                if (error == EXI_ERROR__NO_ERROR)
                {
                    // Event: END Element; next=3
                    done = 1;
                    grammar_id = 3;
                }
            }
            break;
        case 2:
            // Grammar: ID=2; read/write bits=1; END Element
            error = exi_basetypes_encoder_nbit_uint(stream, 1, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                // Event: END Element; next=3
                done = 1;
                grammar_id = 3;
            }
            break;
        default:
            error = EXI_ERROR__UNKNOWN_GRAMMAR_ID;
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
int encode_iso20_exiDocument(exi_bitstream_t* stream, struct iso20_exiDocument* exiDoc)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        if (exiDoc->AuthorizationReq_isUsed == 1)
        {
            // encode event 0
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_AuthorizationReqType(stream, &exiDoc->AuthorizationReq);
            }
        }
        else if (exiDoc->AuthorizationRes_isUsed == 1)
        {
            // encode event 1
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_AuthorizationResType(stream, &exiDoc->AuthorizationRes);
            }
        }
        else if (exiDoc->AuthorizationSetupReq_isUsed == 1)
        {
            // encode event 2
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_AuthorizationSetupReqType(stream, &exiDoc->AuthorizationSetupReq);
            }
        }
        else if (exiDoc->AuthorizationSetupRes_isUsed == 1)
        {
            // encode event 3
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 3);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_AuthorizationSetupResType(stream, &exiDoc->AuthorizationSetupRes);
            }
        }
        else if (exiDoc->CLReqControlMode_isUsed == 1)
        {
            // encode event 4
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 4);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CLReqControlModeType(stream, &exiDoc->CLReqControlMode);
            }
        }
        else if (exiDoc->CLResControlMode_isUsed == 1)
        {
            // encode event 5
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 5);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CLResControlModeType(stream, &exiDoc->CLResControlMode);
            }
        }
        else if (exiDoc->CanonicalizationMethod_isUsed == 1)
        {
            // encode event 6
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 6);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CanonicalizationMethodType(stream, &exiDoc->CanonicalizationMethod);
            }
        }
        else if (exiDoc->CertificateInstallationReq_isUsed == 1)
        {
            // encode event 7
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 7);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CertificateInstallationReqType(stream, &exiDoc->CertificateInstallationReq);
            }
        }
        else if (exiDoc->CertificateInstallationRes_isUsed == 1)
        {
            // encode event 8
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 8);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CertificateInstallationResType(stream, &exiDoc->CertificateInstallationRes);
            }
        }
        else if (exiDoc->DSAKeyValue_isUsed == 1)
        {
            // encode event 9
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 9);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_DSAKeyValueType(stream, &exiDoc->DSAKeyValue);
            }
        }
        else if (exiDoc->DigestMethod_isUsed == 1)
        {
            // encode event 10
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 10);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_DigestMethodType(stream, &exiDoc->DigestMethod);
            }
        }
        // simple type! encode_iso20_DigestValue;
        else if (exiDoc->KeyInfo_isUsed == 1)
        {
            // encode event 12
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 12);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_KeyInfoType(stream, &exiDoc->KeyInfo);
            }
        }
        // simple type! encode_iso20_KeyName;
        else if (exiDoc->KeyValue_isUsed == 1)
        {
            // encode event 14
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 14);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_KeyValueType(stream, &exiDoc->KeyValue);
            }
        }
        else if (exiDoc->Manifest_isUsed == 1)
        {
            // encode event 15
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 15);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ManifestType(stream, &exiDoc->Manifest);
            }
        }
        else if (exiDoc->MeteringConfirmationReq_isUsed == 1)
        {
            // encode event 16
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 16);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_MeteringConfirmationReqType(stream, &exiDoc->MeteringConfirmationReq);
            }
        }
        else if (exiDoc->MeteringConfirmationRes_isUsed == 1)
        {
            // encode event 17
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 17);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_MeteringConfirmationResType(stream, &exiDoc->MeteringConfirmationRes);
            }
        }
        // simple type! encode_iso20_MgmtData;
        else if (exiDoc->Object_isUsed == 1)
        {
            // encode event 19
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 19);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ObjectType(stream, &exiDoc->Object);
            }
        }
        else if (exiDoc->PGPData_isUsed == 1)
        {
            // encode event 20
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 20);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_PGPDataType(stream, &exiDoc->PGPData);
            }
        }
        else if (exiDoc->PowerDeliveryReq_isUsed == 1)
        {
            // encode event 21
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 21);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_PowerDeliveryReqType(stream, &exiDoc->PowerDeliveryReq);
            }
        }
        else if (exiDoc->PowerDeliveryRes_isUsed == 1)
        {
            // encode event 22
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_PowerDeliveryResType(stream, &exiDoc->PowerDeliveryRes);
            }
        }
        else if (exiDoc->RSAKeyValue_isUsed == 1)
        {
            // encode event 23
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_RSAKeyValueType(stream, &exiDoc->RSAKeyValue);
            }
        }
        else if (exiDoc->Reference_isUsed == 1)
        {
            // encode event 24
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ReferenceType(stream, &exiDoc->Reference);
            }
        }
        else if (exiDoc->RetrievalMethod_isUsed == 1)
        {
            // encode event 25
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 25);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_RetrievalMethodType(stream, &exiDoc->RetrievalMethod);
            }
        }
        else if (exiDoc->SPKIData_isUsed == 1)
        {
            // encode event 26
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 26);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SPKIDataType(stream, &exiDoc->SPKIData);
            }
        }
        else if (exiDoc->ScheduleExchangeReq_isUsed == 1)
        {
            // encode event 27
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 27);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ScheduleExchangeReqType(stream, &exiDoc->ScheduleExchangeReq);
            }
        }
        else if (exiDoc->ScheduleExchangeRes_isUsed == 1)
        {
            // encode event 28
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ScheduleExchangeResType(stream, &exiDoc->ScheduleExchangeRes);
            }
        }
        else if (exiDoc->ServiceDetailReq_isUsed == 1)
        {
            // encode event 29
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 29);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceDetailReqType(stream, &exiDoc->ServiceDetailReq);
            }
        }
        else if (exiDoc->ServiceDetailRes_isUsed == 1)
        {
            // encode event 30
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 30);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceDetailResType(stream, &exiDoc->ServiceDetailRes);
            }
        }
        else if (exiDoc->ServiceDiscoveryReq_isUsed == 1)
        {
            // encode event 31
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 31);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceDiscoveryReqType(stream, &exiDoc->ServiceDiscoveryReq);
            }
        }
        else if (exiDoc->ServiceDiscoveryRes_isUsed == 1)
        {
            // encode event 32
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 32);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceDiscoveryResType(stream, &exiDoc->ServiceDiscoveryRes);
            }
        }
        else if (exiDoc->ServiceSelectionReq_isUsed == 1)
        {
            // encode event 33
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 33);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceSelectionReqType(stream, &exiDoc->ServiceSelectionReq);
            }
        }
        else if (exiDoc->ServiceSelectionRes_isUsed == 1)
        {
            // encode event 34
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 34);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ServiceSelectionResType(stream, &exiDoc->ServiceSelectionRes);
            }
        }
        else if (exiDoc->SessionSetupReq_isUsed == 1)
        {
            // encode event 35
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 35);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SessionSetupReqType(stream, &exiDoc->SessionSetupReq);
            }
        }
        else if (exiDoc->SessionSetupRes_isUsed == 1)
        {
            // encode event 36
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 36);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SessionSetupResType(stream, &exiDoc->SessionSetupRes);
            }
        }
        else if (exiDoc->SessionStopReq_isUsed == 1)
        {
            // encode event 37
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 37);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SessionStopReqType(stream, &exiDoc->SessionStopReq);
            }
        }
        else if (exiDoc->SessionStopRes_isUsed == 1)
        {
            // encode event 38
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 38);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SessionStopResType(stream, &exiDoc->SessionStopRes);
            }
        }
        else if (exiDoc->SignatureMethod_isUsed == 1)
        {
            // encode event 39
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 39);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignatureMethodType(stream, &exiDoc->SignatureMethod);
            }
        }
        else if (exiDoc->SignatureProperties_isUsed == 1)
        {
            // encode event 40
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 40);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignaturePropertiesType(stream, &exiDoc->SignatureProperties);
            }
        }
        else if (exiDoc->SignatureProperty_isUsed == 1)
        {
            // encode event 41
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 41);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignaturePropertyType(stream, &exiDoc->SignatureProperty);
            }
        }
        else if (exiDoc->Signature_isUsed == 1)
        {
            // encode event 42
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 42);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignatureType(stream, &exiDoc->Signature);
            }
        }
        else if (exiDoc->SignatureValue_isUsed == 1)
        {
            // encode event 43
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 43);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignatureValueType(stream, &exiDoc->SignatureValue);
            }
        }
        else if (exiDoc->SignedInfo_isUsed == 1)
        {
            // encode event 44
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 44);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedInfoType(stream, &exiDoc->SignedInfo);
            }
        }
        else if (exiDoc->SignedInstallationData_isUsed == 1)
        {
            // encode event 45
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 45);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedInstallationDataType(stream, &exiDoc->SignedInstallationData);
            }
        }
        else if (exiDoc->SignedMeteringData_isUsed == 1)
        {
            // encode event 46
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 46);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedMeteringDataType(stream, &exiDoc->SignedMeteringData);
            }
        }
        else if (exiDoc->Transform_isUsed == 1)
        {
            // encode event 47
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 47);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_TransformType(stream, &exiDoc->Transform);
            }
        }
        else if (exiDoc->Transforms_isUsed == 1)
        {
            // encode event 48
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 48);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_TransformsType(stream, &exiDoc->Transforms);
            }
        }
        else if (exiDoc->VehicleCheckInReq_isUsed == 1)
        {
            // encode event 49
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 49);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_VehicleCheckInReqType(stream, &exiDoc->VehicleCheckInReq);
            }
        }
        else if (exiDoc->VehicleCheckInRes_isUsed == 1)
        {
            // encode event 50
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 50);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_VehicleCheckInResType(stream, &exiDoc->VehicleCheckInRes);
            }
        }
        else if (exiDoc->VehicleCheckOutReq_isUsed == 1)
        {
            // encode event 51
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 51);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_VehicleCheckOutReqType(stream, &exiDoc->VehicleCheckOutReq);
            }
        }
        else if (exiDoc->VehicleCheckOutRes_isUsed == 1)
        {
            // encode event 52
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 52);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_VehicleCheckOutResType(stream, &exiDoc->VehicleCheckOutRes);
            }
        }
        else if (exiDoc->X509Data_isUsed == 1)
        {
            // encode event 53
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 53);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_X509DataType(stream, &exiDoc->X509Data);
            }
        }
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }
    }

    return error;
}

// main function for encoding fragment
int encode_iso20_exiFragment(exi_bitstream_t* stream, struct iso20_exiFragment* exiFrag)
{
    int error = exi_header_write(stream);

    if (error == EXI_ERROR__NO_ERROR)
    {
        // AbsolutePriceSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        if (exiFrag->AbsolutePriceSchedule_isUsed == 1)
        {
            // encode event 0
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 0);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_AbsolutePriceScheduleType(stream, &exiFrag->AbsolutePriceSchedule);
            }
        }
        // AckMaxDelay (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 1
        // AdditionalSelectedServices (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 2
        // AdditionalService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 3
        // AdditionalServicesCosts (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 4
        // Amount (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 5
        // AppliesMinimumMaximumCost (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 6
        // AppliesToEnergyFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 7
        // AppliesToOverstayFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 8
        // AppliesToParkingFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 9
        // AuthorizationReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 10
        // AuthorizationRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 11
        // AuthorizationServices (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 12
        // AuthorizationSetupReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 13
        // AuthorizationSetupRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 14
        // AvailableEnergy (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 15
        // BPT_ChannelSelection (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 16
        // BPT_DischargedEnergyReadingWh (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 17
        // BPT_InductiveEnergyReadingVARh (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 18
        // BatteryEnergyCapacity (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 19
        // CLReqControlMode (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 20
        // CLResControlMode (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 21
        // CPSCertificateChain (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 22
        // CanonicalizationMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 23
        // CapacitiveEnergyReadingVARh (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 24
        // CarbonDioxideEmission (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 25
        // Certificate (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 26
        // CertificateInstallationReq (urn:iso:std:iso:15118:-20:CommonMessages)
        else if (exiFrag->CertificateInstallationReq_isUsed == 1)
        {
            // encode event 27
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 27);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_CertificateInstallationReqType(stream, &exiFrag->CertificateInstallationReq);
            }
        }
        // CertificateInstallationRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 28
        // CertificateInstallationService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 29
        // ChargeProgress (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 30
        // ChargedEnergyReadingWh (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 31
        // ChargingComplete (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 32
        // ChargingSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 33
        // ChargingSession (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 34
        // CheckOutTime (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 35
        // ContractCertificateChain (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 36
        // CostPerUnit (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 37
        // Currency (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 38
        // DHPublicKey (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 39
        // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 40
        // DepartureTime (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 41
        // DepartureTime (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 42
        // DeviceLocation (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 43
        // DeviceOffset (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 44
        // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 45
        // DigestValue (http://www.w3.org/2000/09/xmldsig#)
        // event 46
        // DischargingSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 47
        // DisplayParameters (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 48
        // Duration (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 49
        // Dynamic_EVPPTControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 50
        // Dynamic_SEReqControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 51
        // Dynamic_SEResControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 52
        // Dynamic_SMDTControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 53
        // ECDHCurve (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 54
        // EIM_AReqAuthorizationMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 55
        // EIM_ASResAuthorizationMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 56
        // EMAID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 57
        // EVAbsolutePriceSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 58
        // EVCCID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 59
        // EVCheckInStatus (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 60
        // EVCheckOutStatus (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 61
        // EVEnergyOffer (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 62
        // EVMaximumEnergyRequest (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 63
        // EVMaximumEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 64
        // EVMaximumV2XEnergyRequest (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 65
        // EVMinimumEnergyRequest (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 66
        // EVMinimumEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 67
        // EVMinimumV2XEnergyRequest (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 68
        // EVPowerProfile (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 69
        // EVPowerProfileEntries (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 70
        // EVPowerProfileEntry (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 71
        // EVPowerSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 72
        // EVPowerScheduleEntries (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 73
        // EVPowerScheduleEntry (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 74
        // EVPriceRule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 75
        // EVPriceRuleStack (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 76
        // EVPriceRuleStacks (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 77
        // EVProcessing (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 78
        // EVSECheckOutStatus (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 79
        // EVSEID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 80
        // EVSENotification (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 81
        // EVSEProcessing (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 82
        // EVSEStatus (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 83
        // EVSEStatus (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 84
        // EVTargetEnergyRequest (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 85
        // EVTargetEnergyRequest (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 86
        // EVTerminationCode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 87
        // EVTerminationExplanation (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 88
        // EnergyCosts (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 89
        // EnergyFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 90
        // EnergyTransferServiceList (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 91
        // Exponent (http://www.w3.org/2000/09/xmldsig#)
        // event 92
        // Exponent (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 93
        // FreeService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 94
        // G (http://www.w3.org/2000/09/xmldsig#)
        // event 95
        // GenChallenge (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 96
        // GoToPause (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 97
        // HMACOutputLength (http://www.w3.org/2000/09/xmldsig#)
        // event 98
        // Header (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 99
        // InletHot (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 100
        // J (http://www.w3.org/2000/09/xmldsig#)
        // event 101
        // KeyInfo (http://www.w3.org/2000/09/xmldsig#)
        // event 102
        // KeyName (http://www.w3.org/2000/09/xmldsig#)
        // event 103
        // KeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 104
        // Language (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 105
        // ListOfRootCertificateIDs (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 106
        // Manifest (http://www.w3.org/2000/09/xmldsig#)
        // event 107
        // MaximumContractCertificateChains (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 108
        // MaximumCost (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 109
        // MaximumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 110
        // MaximumSupportingPoints (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 111
        // MeterID (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 112
        // MeterInfo (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 113
        // MeterInfo (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 114
        // MeterInfoRequested (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 115
        // MeterSignature (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 116
        // MeterStatus (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 117
        // MeterTimestamp (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 118
        // MeteringConfirmationReq (urn:iso:std:iso:15118:-20:CommonMessages)
        else if (exiFrag->MeteringConfirmationReq_isUsed == 1)
        {
            // encode event 119
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 119);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_MeteringConfirmationReqType(stream, &exiFrag->MeteringConfirmationReq);
            }
        }
        // MeteringConfirmationRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 120
        // MgmtData (http://www.w3.org/2000/09/xmldsig#)
        // event 121
        // MinimumCost (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 122
        // MinimumSOC (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 123
        // MinimumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 124
        // Modulus (http://www.w3.org/2000/09/xmldsig#)
        // event 125
        // NotificationMaxDelay (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 126
        // NumberOfPriceLevels (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 127
        // OEMProvisioningCertificateChain (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 128
        // Object (http://www.w3.org/2000/09/xmldsig#)
        // event 129
        // OccupancyCosts (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 130
        // OverstayCosts (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 131
        // OverstayFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 132
        // OverstayFeePeriod (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 133
        // OverstayPowerThreshold (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 134
        // OverstayRule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 135
        // OverstayRuleDescription (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 136
        // OverstayRules (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 137
        // OverstayTimeThreshold (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 138
        // P (http://www.w3.org/2000/09/xmldsig#)
        // event 139
        // PGPData (http://www.w3.org/2000/09/xmldsig#)
        // event 140
        // PGPKeyID (http://www.w3.org/2000/09/xmldsig#)
        // event 141
        // PGPKeyPacket (http://www.w3.org/2000/09/xmldsig#)
        // event 142
        // Parameter (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 143
        // ParameterSet (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 144
        // ParameterSetID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 145
        // ParkingFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 146
        // ParkingFeePeriod (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 147
        // ParkingMethod (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 148
        // ParkingSpace (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 149
        // PgenCounter (http://www.w3.org/2000/09/xmldsig#)
        // event 150
        // PnC_AReqAuthorizationMode (urn:iso:std:iso:15118:-20:CommonMessages)
        else if (exiFrag->PnC_AReqAuthorizationMode_isUsed == 1)
        {
            // encode event 151
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 151);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_PnC_AReqAuthorizationModeType(stream, &exiFrag->PnC_AReqAuthorizationMode);
            }
        }
        // PnC_ASResAuthorizationMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 152
        // Power (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 153
        // PowerDeliveryReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 154
        // PowerDeliveryRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 155
        // PowerRangeStart (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 156
        // PowerSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 157
        // PowerScheduleEntries (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 158
        // PowerScheduleEntry (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 159
        // PowerTolerance (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 160
        // PowerToleranceAcceptance (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 161
        // Power_L2 (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 162
        // Power_L3 (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 163
        // PresentSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 164
        // PriceAlgorithm (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 165
        // PriceLevel (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 166
        // PriceLevelSchedule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 167
        // PriceLevelScheduleEntries (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 168
        // PriceLevelScheduleEntry (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 169
        // PriceRule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 170
        // PriceRuleStack (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 171
        // PriceRuleStacks (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 172
        // PriceScheduleDescription (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 173
        // PriceScheduleID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 174
        // PrioritizedEMAIDs (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 175
        // ProviderID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 176
        // Q (http://www.w3.org/2000/09/xmldsig#)
        // event 177
        // RSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        // event 178
        // Receipt (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 179
        // Receipt (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 180
        // Reference (http://www.w3.org/2000/09/xmldsig#)
        // event 181
        // RemainingContractCertificateChains (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 182
        // RemainingTimeToMaximumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 183
        // RemainingTimeToMinimumSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 184
        // RemainingTimeToTargetSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 185
        // RenewableGenerationPercentage (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 186
        // ResponseCode (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 187
        // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 188
        // RootCertificateID (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 189
        // SECP521_EncryptedPrivateKey (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 190
        // SPKIData (http://www.w3.org/2000/09/xmldsig#)
        // event 191
        // SPKISexp (http://www.w3.org/2000/09/xmldsig#)
        // event 192
        // ScheduleExchangeReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 193
        // ScheduleExchangeRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 194
        // ScheduleTuple (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 195
        // ScheduleTupleID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 196
        // Scheduled_EVPPTControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 197
        // Scheduled_SEReqControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 198
        // Scheduled_SEResControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 199
        // Scheduled_SMDTControlMode (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 200
        // Seed (http://www.w3.org/2000/09/xmldsig#)
        // event 201
        // SelectedAuthorizationService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 202
        // SelectedEnergyTransferService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 203
        // SelectedScheduleTupleID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 204
        // SelectedService (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 205
        // SelectedVASList (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 206
        // Service (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 207
        // ServiceDetailReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 208
        // ServiceDetailRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 209
        // ServiceDiscoveryReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 210
        // ServiceDiscoveryRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 211
        // ServiceFee (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 212
        // ServiceID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 213
        // ServiceName (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 214
        // ServiceParameterList (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 215
        // ServiceRenegotiationSupported (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 216
        // ServiceSelectionReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 217
        // ServiceSelectionRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 218
        // SessionID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 219
        // SessionID (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 220
        // SessionSetupReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 221
        // SessionSetupRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 222
        // SessionStopReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 223
        // SessionStopRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 224
        // Signature (http://www.w3.org/2000/09/xmldsig#)
        // event 225
        // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
        // event 226
        // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
        // event 227
        // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
        // event 228
        // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
        // event 229
        // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
        else if (exiFrag->SignedInfo_isUsed == 1)
        {
            // encode event 230
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 230);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedInfoType(stream, &exiFrag->SignedInfo);
            }
        }
        // SignedInstallationData (urn:iso:std:iso:15118:-20:CommonMessages)
        else if (exiFrag->SignedInstallationData_isUsed == 1)
        {
            // encode event 231
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 231);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedInstallationDataType(stream, &exiFrag->SignedInstallationData);
            }
        }
        // SignedMeteringData (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 232
        // StartTime (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 233
        // SubCertificates (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 234
        // SupportedProviders (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 235
        // SupportedServiceIDs (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 236
        // TPM_EncryptedPrivateKey (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 237
        // TargetDistance (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 238
        // TargetOffsetX (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 239
        // TargetOffsetY (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 240
        // TargetSOC (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 241
        // TargetSOC (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 242
        // TaxCosts (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 243
        // TaxIncludedInPrice (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 244
        // TaxRate (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 245
        // TaxRule (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 246
        // TaxRuleID (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 247
        // TaxRuleID (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 248
        // TaxRuleName (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 249
        // TaxRules (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 250
        // TimeAnchor (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 251
        // TimeAnchor (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 252
        // TimeStamp (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 253
        // Transform (http://www.w3.org/2000/09/xmldsig#)
        // event 254
        // Transforms (http://www.w3.org/2000/09/xmldsig#)
        // event 255
        // VASList (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 256
        // Value (urn:iso:std:iso:15118:-20:CommonTypes)
        // event 257
        // VehicleCheckInReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 258
        // VehicleCheckInRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 259
        // VehicleCheckOutReq (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 260
        // VehicleCheckOutRes (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 261
        // VehicleFrame (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 262
        // VehicleTravel (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 263
        // X448_EncryptedPrivateKey (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 264
        // X509CRL (http://www.w3.org/2000/09/xmldsig#)
        // event 265
        // X509Certificate (http://www.w3.org/2000/09/xmldsig#)
        // event 266
        // X509Data (http://www.w3.org/2000/09/xmldsig#)
        // event 267
        // X509IssuerName (http://www.w3.org/2000/09/xmldsig#)
        // event 268
        // X509IssuerSerial (http://www.w3.org/2000/09/xmldsig#)
        // event 269
        // X509SKI (http://www.w3.org/2000/09/xmldsig#)
        // event 270
        // X509SerialNumber (http://www.w3.org/2000/09/xmldsig#)
        // event 271
        // X509SubjectName (http://www.w3.org/2000/09/xmldsig#)
        // event 272
        // XPath (http://www.w3.org/2000/09/xmldsig#)
        // event 273
        // Y (http://www.w3.org/2000/09/xmldsig#)
        // event 274
        // boolValue (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 275
        // byteValue (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 276
        // finiteString (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 277
        // intValue (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 278
        // rationalNumber (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 279
        // shortValue (urn:iso:std:iso:15118:-20:CommonMessages)
        // event 280
        else
        {
            error = EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING;
        }

        if (error == EXI_ERROR__NO_ERROR)
        {
            // End Fragment
            error = exi_basetypes_encoder_nbit_uint(stream, 9, 282);
        }
    }

    return error;
}

// main function for encoding xmldsig fragment
int encode_iso20_xmldsigFragment(exi_bitstream_t* stream, struct iso20_xmldsigFragment* xmldsigFrag)
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
                error = encode_iso20_CanonicalizationMethodType(stream, &xmldsigFrag->CanonicalizationMethod);
            }
        }
        // DSAKeyValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->DSAKeyValue_isUsed == 1)
        {
            // encode event 1
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 1);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_DSAKeyValueType(stream, &xmldsigFrag->DSAKeyValue);
            }
        }
        // DigestMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->DigestMethod_isUsed == 1)
        {
            // encode event 2
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 2);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_DigestMethodType(stream, &xmldsigFrag->DigestMethod);
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
                error = encode_iso20_KeyInfoType(stream, &xmldsigFrag->KeyInfo);
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
                error = encode_iso20_KeyValueType(stream, &xmldsigFrag->KeyValue);
            }
        }
        // Manifest (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Manifest_isUsed == 1)
        {
            // encode event 11
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 11);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ManifestType(stream, &xmldsigFrag->Manifest);
            }
        }
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
                error = encode_iso20_ObjectType(stream, &xmldsigFrag->Object);
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
                error = encode_iso20_PGPDataType(stream, &xmldsigFrag->PGPData);
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
                error = encode_iso20_RSAKeyValueType(stream, &xmldsigFrag->RSAKeyValue);
            }
        }
        // Reference (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Reference_isUsed == 1)
        {
            // encode event 22
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 22);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_ReferenceType(stream, &xmldsigFrag->Reference);
            }
        }
        // RetrievalMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->RetrievalMethod_isUsed == 1)
        {
            // encode event 23
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 23);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_RetrievalMethodType(stream, &xmldsigFrag->RetrievalMethod);
            }
        }
        // SPKIData (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SPKIData_isUsed == 1)
        {
            // encode event 24
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 24);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SPKIDataType(stream, &xmldsigFrag->SPKIData);
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
                error = encode_iso20_SignatureType(stream, &xmldsigFrag->Signature);
            }
        }
        // SignatureMethod (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureMethod_isUsed == 1)
        {
            // encode event 28
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 28);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignatureMethodType(stream, &xmldsigFrag->SignatureMethod);
            }
        }
        // SignatureProperties (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureProperties_isUsed == 1)
        {
            // encode event 29
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 29);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignaturePropertiesType(stream, &xmldsigFrag->SignatureProperties);
            }
        }
        // SignatureProperty (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureProperty_isUsed == 1)
        {
            // encode event 30
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 30);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignaturePropertyType(stream, &xmldsigFrag->SignatureProperty);
            }
        }
        // SignatureValue (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignatureValue_isUsed == 1)
        {
            // encode event 31
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 31);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignatureValueType(stream, &xmldsigFrag->SignatureValue);
            }
        }
        // SignedInfo (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->SignedInfo_isUsed == 1)
        {
            // encode event 32
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 32);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_SignedInfoType(stream, &xmldsigFrag->SignedInfo);
            }
        }
        // Transform (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Transform_isUsed == 1)
        {
            // encode event 33
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 33);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_TransformType(stream, &xmldsigFrag->Transform);
            }
        }
        // Transforms (http://www.w3.org/2000/09/xmldsig#)
        else if (xmldsigFrag->Transforms_isUsed == 1)
        {
            // encode event 34
            error = exi_basetypes_encoder_nbit_uint(stream, 6, 34);
            if (error == EXI_ERROR__NO_ERROR)
            {
                error = encode_iso20_TransformsType(stream, &xmldsigFrag->Transforms);
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
                error = encode_iso20_X509DataType(stream, &xmldsigFrag->X509Data);
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
                error = encode_iso20_X509IssuerSerialType(stream, &xmldsigFrag->X509IssuerSerial);
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


