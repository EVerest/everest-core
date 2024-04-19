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
  * @file iso20_CommonMessages_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "iso20_CommonMessages_Datatypes.h"



// root elements of EXI doc
void init_iso20_exiDocument(struct iso20_exiDocument* exiDoc) {
    exiDoc->SessionSetupReq_isUsed = 0u;
    exiDoc->SessionSetupRes_isUsed = 0u;
    exiDoc->AuthorizationSetupReq_isUsed = 0u;
    exiDoc->AuthorizationSetupRes_isUsed = 0u;
    exiDoc->AuthorizationReq_isUsed = 0u;
    exiDoc->AuthorizationRes_isUsed = 0u;
    exiDoc->ServiceDiscoveryReq_isUsed = 0u;
    exiDoc->ServiceDiscoveryRes_isUsed = 0u;
    exiDoc->ServiceDetailReq_isUsed = 0u;
    exiDoc->ServiceDetailRes_isUsed = 0u;
    exiDoc->ServiceSelectionReq_isUsed = 0u;
    exiDoc->ServiceSelectionRes_isUsed = 0u;
    exiDoc->ScheduleExchangeReq_isUsed = 0u;
    exiDoc->ScheduleExchangeRes_isUsed = 0u;
    exiDoc->PowerDeliveryReq_isUsed = 0u;
    exiDoc->PowerDeliveryRes_isUsed = 0u;
    exiDoc->MeteringConfirmationReq_isUsed = 0u;
    exiDoc->MeteringConfirmationRes_isUsed = 0u;
    exiDoc->SessionStopReq_isUsed = 0u;
    exiDoc->SessionStopRes_isUsed = 0u;
    exiDoc->CertificateInstallationReq_isUsed = 0u;
    exiDoc->CertificateInstallationRes_isUsed = 0u;
    exiDoc->VehicleCheckInReq_isUsed = 0u;
    exiDoc->VehicleCheckInRes_isUsed = 0u;
    exiDoc->VehicleCheckOutReq_isUsed = 0u;
    exiDoc->VehicleCheckOutRes_isUsed = 0u;
    exiDoc->SignedInstallationData_isUsed = 0u;
    exiDoc->SignedMeteringData_isUsed = 0u;
    exiDoc->CLReqControlMode_isUsed = 0u;
    exiDoc->CLResControlMode_isUsed = 0u;
    exiDoc->Signature_isUsed = 0u;
    exiDoc->SignatureValue_isUsed = 0u;
    exiDoc->SignedInfo_isUsed = 0u;
    exiDoc->CanonicalizationMethod_isUsed = 0u;
    exiDoc->SignatureMethod_isUsed = 0u;
    exiDoc->Reference_isUsed = 0u;
    exiDoc->Transforms_isUsed = 0u;
    exiDoc->Transform_isUsed = 0u;
    exiDoc->DigestMethod_isUsed = 0u;
    exiDoc->KeyInfo_isUsed = 0u;
    exiDoc->KeyValue_isUsed = 0u;
    exiDoc->RetrievalMethod_isUsed = 0u;
    exiDoc->X509Data_isUsed = 0u;
    exiDoc->PGPData_isUsed = 0u;
    exiDoc->SPKIData_isUsed = 0u;
    exiDoc->Object_isUsed = 0u;
    exiDoc->Manifest_isUsed = 0u;
    exiDoc->SignatureProperties_isUsed = 0u;
    exiDoc->SignatureProperty_isUsed = 0u;
    exiDoc->DSAKeyValue_isUsed = 0u;
    exiDoc->RSAKeyValue_isUsed = 0u;
}
void init_iso20_TransformType(struct iso20_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso20_PowerScheduleEntryType(struct iso20_PowerScheduleEntryType* PowerScheduleEntryType) {
    PowerScheduleEntryType->Power_L2_isUsed = 0u;
    PowerScheduleEntryType->Power_L3_isUsed = 0u;
}

void init_iso20_EVPriceRuleType(struct iso20_EVPriceRuleType* EVPriceRuleType) {
    (void) EVPriceRuleType;
}

void init_iso20_TransformsType(struct iso20_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso20_DSAKeyValueType(struct iso20_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso20_X509IssuerSerialType(struct iso20_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso20_EVPowerScheduleEntryType(struct iso20_EVPowerScheduleEntryType* EVPowerScheduleEntryType) {
    (void) EVPowerScheduleEntryType;
}

void init_iso20_EVPriceRuleStackType(struct iso20_EVPriceRuleStackType* EVPriceRuleStackType) {
    EVPriceRuleStackType->EVPriceRule.arrayLen = 0u;
}

void init_iso20_DigestMethodType(struct iso20_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso20_RSAKeyValueType(struct iso20_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso20_PriceRuleType(struct iso20_PriceRuleType* PriceRuleType) {
    PriceRuleType->ParkingFee_isUsed = 0u;
    PriceRuleType->ParkingFeePeriod_isUsed = 0u;
    PriceRuleType->CarbonDioxideEmission_isUsed = 0u;
    PriceRuleType->RenewableGenerationPercentage_isUsed = 0u;
}

void init_iso20_PowerScheduleEntryListType(struct iso20_PowerScheduleEntryListType* PowerScheduleEntryListType) {
    PowerScheduleEntryListType->PowerScheduleEntry.arrayLen = 0u;
}

void init_iso20_CanonicalizationMethodType(struct iso20_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso20_TaxRuleType(struct iso20_TaxRuleType* TaxRuleType) {
    TaxRuleType->TaxRuleName_isUsed = 0u;
    TaxRuleType->TaxIncludedInPrice_isUsed = 0u;
}

void init_iso20_PriceRuleStackType(struct iso20_PriceRuleStackType* PriceRuleStackType) {
    PriceRuleStackType->PriceRule.arrayLen = 0u;
}

void init_iso20_AdditionalServiceType(struct iso20_AdditionalServiceType* AdditionalServiceType) {
    (void) AdditionalServiceType;
}

void init_iso20_PriceLevelScheduleEntryType(struct iso20_PriceLevelScheduleEntryType* PriceLevelScheduleEntryType) {
    (void) PriceLevelScheduleEntryType;
}

void init_iso20_PowerScheduleType(struct iso20_PowerScheduleType* PowerScheduleType) {
    PowerScheduleType->AvailableEnergy_isUsed = 0u;
    PowerScheduleType->PowerTolerance_isUsed = 0u;
}

void init_iso20_SignatureMethodType(struct iso20_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso20_KeyValueType(struct iso20_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso20_EVPowerScheduleEntryListType(struct iso20_EVPowerScheduleEntryListType* EVPowerScheduleEntryListType) {
    EVPowerScheduleEntryListType->EVPowerScheduleEntry.arrayLen = 0u;
}

void init_iso20_ReferenceType(struct iso20_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso20_RetrievalMethodType(struct iso20_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso20_OverstayRuleType(struct iso20_OverstayRuleType* OverstayRuleType) {
    OverstayRuleType->OverstayRuleDescription_isUsed = 0u;
}

void init_iso20_X509DataType(struct iso20_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso20_EVPriceRuleStackListType(struct iso20_EVPriceRuleStackListType* EVPriceRuleStackListType) {
    EVPriceRuleStackListType->EVPriceRuleStack.arrayLen = 0u;
}

void init_iso20_PGPDataType(struct iso20_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso20_RationalNumberType(struct iso20_RationalNumberType* RationalNumberType) {
    (void) RationalNumberType;
}

void init_iso20_SPKIDataType(struct iso20_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso20_SignedInfoType(struct iso20_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso20_EVPowerScheduleType(struct iso20_EVPowerScheduleType* EVPowerScheduleType) {
    (void) EVPowerScheduleType;
}

void init_iso20_SignatureValueType(struct iso20_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso20_SubCertificatesType(struct iso20_SubCertificatesType* SubCertificatesType) {
    SubCertificatesType->Certificate.arrayLen = 0u;
}

void init_iso20_ParameterType(struct iso20_ParameterType* ParameterType) {
    ParameterType->boolValue_isUsed = 0u;
    ParameterType->byteValue_isUsed = 0u;
    ParameterType->shortValue_isUsed = 0u;
    ParameterType->intValue_isUsed = 0u;
    ParameterType->rationalNumber_isUsed = 0u;
    ParameterType->finiteString_isUsed = 0u;
}

void init_iso20_EVAbsolutePriceScheduleType(struct iso20_EVAbsolutePriceScheduleType* EVAbsolutePriceScheduleType) {
    (void) EVAbsolutePriceScheduleType;
}

void init_iso20_ChargingScheduleType(struct iso20_ChargingScheduleType* ChargingScheduleType) {
    ChargingScheduleType->AbsolutePriceSchedule_isUsed = 0u;
    ChargingScheduleType->PriceLevelSchedule_isUsed = 0u;
}

void init_iso20_DetailedCostType(struct iso20_DetailedCostType* DetailedCostType) {
    (void) DetailedCostType;
}

void init_iso20_KeyInfoType(struct iso20_KeyInfoType* KeyInfoType) {
    KeyInfoType->Id_isUsed = 0u;
    KeyInfoType->KeyName_isUsed = 0u;
    KeyInfoType->KeyValue_isUsed = 0u;
    KeyInfoType->RetrievalMethod_isUsed = 0u;
    KeyInfoType->X509Data_isUsed = 0u;
    KeyInfoType->PGPData_isUsed = 0u;
    KeyInfoType->SPKIData_isUsed = 0u;
    KeyInfoType->MgmtData_isUsed = 0u;
    KeyInfoType->ANY_isUsed = 0u;
}

void init_iso20_ObjectType(struct iso20_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso20_PriceLevelScheduleEntryListType(struct iso20_PriceLevelScheduleEntryListType* PriceLevelScheduleEntryListType) {
    PriceLevelScheduleEntryListType->PriceLevelScheduleEntry.arrayLen = 0u;
}

void init_iso20_DetailedTaxType(struct iso20_DetailedTaxType* DetailedTaxType) {
    (void) DetailedTaxType;
}

void init_iso20_TaxRuleListType(struct iso20_TaxRuleListType* TaxRuleListType) {
    TaxRuleListType->TaxRule.arrayLen = 0u;
}

void init_iso20_PriceRuleStackListType(struct iso20_PriceRuleStackListType* PriceRuleStackListType) {
    PriceRuleStackListType->PriceRuleStack.arrayLen = 0u;
}

void init_iso20_OverstayRuleListType(struct iso20_OverstayRuleListType* OverstayRuleListType) {
    OverstayRuleListType->OverstayRule.arrayLen = 0u;
    OverstayRuleListType->OverstayTimeThreshold_isUsed = 0u;
    OverstayRuleListType->OverstayPowerThreshold_isUsed = 0u;
}

void init_iso20_AdditionalServiceListType(struct iso20_AdditionalServiceListType* AdditionalServiceListType) {
    AdditionalServiceListType->AdditionalService.arrayLen = 0u;
}

void init_iso20_ServiceType(struct iso20_ServiceType* ServiceType) {
    (void) ServiceType;
}

void init_iso20_ParameterSetType(struct iso20_ParameterSetType* ParameterSetType) {
    ParameterSetType->Parameter.arrayLen = 0u;
}

void init_iso20_ScheduleTupleType(struct iso20_ScheduleTupleType* ScheduleTupleType) {
    ScheduleTupleType->DischargingSchedule_isUsed = 0u;
}

void init_iso20_SupportedProvidersListType(struct iso20_SupportedProvidersListType* SupportedProvidersListType) {
    SupportedProvidersListType->ProviderID.arrayLen = 0u;
}

void init_iso20_ContractCertificateChainType(struct iso20_ContractCertificateChainType* ContractCertificateChainType) {
    (void) ContractCertificateChainType;
}

void init_iso20_Dynamic_EVPPTControlModeType(struct iso20_Dynamic_EVPPTControlModeType* Dynamic_EVPPTControlModeType) {
    (void) Dynamic_EVPPTControlModeType;
}

void init_iso20_MeterInfoType(struct iso20_MeterInfoType* MeterInfoType) {
    MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 0u;
    MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->MeterSignature_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->MeterTimestamp_isUsed = 0u;
}

void init_iso20_SignatureType(struct iso20_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso20_Scheduled_EVPPTControlModeType(struct iso20_Scheduled_EVPPTControlModeType* Scheduled_EVPPTControlModeType) {
    Scheduled_EVPPTControlModeType->PowerToleranceAcceptance_isUsed = 0u;
}

void init_iso20_ReceiptType(struct iso20_ReceiptType* ReceiptType) {
    ReceiptType->TaxCosts.arrayLen = 0u;
    ReceiptType->EnergyCosts_isUsed = 0u;
    ReceiptType->OccupancyCosts_isUsed = 0u;
    ReceiptType->AdditionalServicesCosts_isUsed = 0u;
    ReceiptType->OverstayCosts_isUsed = 0u;
}

void init_iso20_AbsolutePriceScheduleType(struct iso20_AbsolutePriceScheduleType* AbsolutePriceScheduleType) {
    AbsolutePriceScheduleType->Id_isUsed = 0u;
    AbsolutePriceScheduleType->PriceScheduleDescription_isUsed = 0u;
    AbsolutePriceScheduleType->MinimumCost_isUsed = 0u;
    AbsolutePriceScheduleType->MaximumCost_isUsed = 0u;
    AbsolutePriceScheduleType->TaxRules_isUsed = 0u;
    AbsolutePriceScheduleType->OverstayRules_isUsed = 0u;
    AbsolutePriceScheduleType->AdditionalSelectedServices_isUsed = 0u;
}

void init_iso20_EVPowerProfileEntryListType(struct iso20_EVPowerProfileEntryListType* EVPowerProfileEntryListType) {
    EVPowerProfileEntryListType->EVPowerProfileEntry.arrayLen = 0u;
}

void init_iso20_Dynamic_SMDTControlModeType(struct iso20_Dynamic_SMDTControlModeType* Dynamic_SMDTControlModeType) {
    (void) Dynamic_SMDTControlModeType;
}

void init_iso20_EVEnergyOfferType(struct iso20_EVEnergyOfferType* EVEnergyOfferType) {
    (void) EVEnergyOfferType;
}

void init_iso20_PriceLevelScheduleType(struct iso20_PriceLevelScheduleType* PriceLevelScheduleType) {
    PriceLevelScheduleType->Id_isUsed = 0u;
    PriceLevelScheduleType->PriceScheduleDescription_isUsed = 0u;
}

void init_iso20_Scheduled_SMDTControlModeType(struct iso20_Scheduled_SMDTControlModeType* Scheduled_SMDTControlModeType) {
    (void) Scheduled_SMDTControlModeType;
}

void init_iso20_MessageHeaderType(struct iso20_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso20_SignaturePropertyType(struct iso20_SignaturePropertyType* SignaturePropertyType) {
    SignaturePropertyType->Id_isUsed = 0u;
    SignaturePropertyType->ANY_isUsed = 0u;
}

void init_iso20_ServiceIDListType(struct iso20_ServiceIDListType* ServiceIDListType) {
    ServiceIDListType->ServiceID.arrayLen = 0u;
}

void init_iso20_SelectedServiceType(struct iso20_SelectedServiceType* SelectedServiceType) {
    (void) SelectedServiceType;
}

void init_iso20_SignedMeteringDataType(struct iso20_SignedMeteringDataType* SignedMeteringDataType) {
    SignedMeteringDataType->Receipt_isUsed = 0u;
    SignedMeteringDataType->Dynamic_SMDTControlMode_isUsed = 0u;
    SignedMeteringDataType->Scheduled_SMDTControlMode_isUsed = 0u;
}

void init_iso20_SignedCertificateChainType(struct iso20_SignedCertificateChainType* SignedCertificateChainType) {
    SignedCertificateChainType->SubCertificates_isUsed = 0u;
}

void init_iso20_EIM_AReqAuthorizationModeType(struct iso20_EIM_AReqAuthorizationModeType* EIM_AReqAuthorizationModeType) {
    (void) EIM_AReqAuthorizationModeType;
}

void init_iso20_SelectedServiceListType(struct iso20_SelectedServiceListType* SelectedServiceListType) {
    SelectedServiceListType->SelectedService.arrayLen = 0u;
}

void init_iso20_Dynamic_SEReqControlModeType(struct iso20_Dynamic_SEReqControlModeType* Dynamic_SEReqControlModeType) {
    Dynamic_SEReqControlModeType->MinimumSOC_isUsed = 0u;
    Dynamic_SEReqControlModeType->TargetSOC_isUsed = 0u;
    Dynamic_SEReqControlModeType->EVMaximumV2XEnergyRequest_isUsed = 0u;
    Dynamic_SEReqControlModeType->EVMinimumV2XEnergyRequest_isUsed = 0u;
}

void init_iso20_EVSEStatusType(struct iso20_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_iso20_ListOfRootCertificateIDsType(struct iso20_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    ListOfRootCertificateIDsType->RootCertificateID.arrayLen = 0u;
}

void init_iso20_PnC_AReqAuthorizationModeType(struct iso20_PnC_AReqAuthorizationModeType* PnC_AReqAuthorizationModeType) {
    (void) PnC_AReqAuthorizationModeType;
}

void init_iso20_ServiceListType(struct iso20_ServiceListType* ServiceListType) {
    ServiceListType->Service.arrayLen = 0u;
}

void init_iso20_ServiceParameterListType(struct iso20_ServiceParameterListType* ServiceParameterListType) {
    ServiceParameterListType->ParameterSet.arrayLen = 0u;
}

void init_iso20_Scheduled_SEReqControlModeType(struct iso20_Scheduled_SEReqControlModeType* Scheduled_SEReqControlModeType) {
    Scheduled_SEReqControlModeType->DepartureTime_isUsed = 0u;
    Scheduled_SEReqControlModeType->EVTargetEnergyRequest_isUsed = 0u;
    Scheduled_SEReqControlModeType->EVMaximumEnergyRequest_isUsed = 0u;
    Scheduled_SEReqControlModeType->EVMinimumEnergyRequest_isUsed = 0u;
    Scheduled_SEReqControlModeType->EVEnergyOffer_isUsed = 0u;
}

void init_iso20_EVPowerProfileType(struct iso20_EVPowerProfileType* EVPowerProfileType) {
    EVPowerProfileType->Dynamic_EVPPTControlMode_isUsed = 0u;
    EVPowerProfileType->Scheduled_EVPPTControlMode_isUsed = 0u;
}

void init_iso20_CertificateChainType(struct iso20_CertificateChainType* CertificateChainType) {
    CertificateChainType->SubCertificates_isUsed = 0u;
}

void init_iso20_EIM_ASResAuthorizationModeType(struct iso20_EIM_ASResAuthorizationModeType* EIM_ASResAuthorizationModeType) {
    (void) EIM_ASResAuthorizationModeType;
}

void init_iso20_Dynamic_SEResControlModeType(struct iso20_Dynamic_SEResControlModeType* Dynamic_SEResControlModeType) {
    Dynamic_SEResControlModeType->DepartureTime_isUsed = 0u;
    Dynamic_SEResControlModeType->MinimumSOC_isUsed = 0u;
    Dynamic_SEResControlModeType->TargetSOC_isUsed = 0u;
    Dynamic_SEResControlModeType->AbsolutePriceSchedule_isUsed = 0u;
    Dynamic_SEResControlModeType->PriceLevelSchedule_isUsed = 0u;
}

void init_iso20_EMAIDListType(struct iso20_EMAIDListType* EMAIDListType) {
    EMAIDListType->EMAID.arrayLen = 0u;
}

void init_iso20_SignedInstallationDataType(struct iso20_SignedInstallationDataType* SignedInstallationDataType) {
    SignedInstallationDataType->SECP521_EncryptedPrivateKey_isUsed = 0u;
    SignedInstallationDataType->X448_EncryptedPrivateKey_isUsed = 0u;
    SignedInstallationDataType->TPM_EncryptedPrivateKey_isUsed = 0u;
}

void init_iso20_PnC_ASResAuthorizationModeType(struct iso20_PnC_ASResAuthorizationModeType* PnC_ASResAuthorizationModeType) {
    PnC_ASResAuthorizationModeType->SupportedProviders_isUsed = 0u;
}

void init_iso20_Scheduled_SEResControlModeType(struct iso20_Scheduled_SEResControlModeType* Scheduled_SEResControlModeType) {
    Scheduled_SEResControlModeType->ScheduleTuple.arrayLen = 0u;
}

void init_iso20_SessionSetupReqType(struct iso20_SessionSetupReqType* SessionSetupReqType) {
    (void) SessionSetupReqType;
}

void init_iso20_SessionSetupResType(struct iso20_SessionSetupResType* SessionSetupResType) {
    (void) SessionSetupResType;
}

void init_iso20_AuthorizationSetupReqType(struct iso20_AuthorizationSetupReqType* AuthorizationSetupReqType) {
    (void) AuthorizationSetupReqType;
}

void init_iso20_AuthorizationSetupResType(struct iso20_AuthorizationSetupResType* AuthorizationSetupResType) {
    AuthorizationSetupResType->AuthorizationServices.arrayLen = 0u;
    AuthorizationSetupResType->EIM_ASResAuthorizationMode_isUsed = 0u;
    AuthorizationSetupResType->PnC_ASResAuthorizationMode_isUsed = 0u;
}

void init_iso20_AuthorizationReqType(struct iso20_AuthorizationReqType* AuthorizationReqType) {
    AuthorizationReqType->EIM_AReqAuthorizationMode_isUsed = 0u;
    AuthorizationReqType->PnC_AReqAuthorizationMode_isUsed = 0u;
}

void init_iso20_AuthorizationResType(struct iso20_AuthorizationResType* AuthorizationResType) {
    (void) AuthorizationResType;
}

void init_iso20_ServiceDiscoveryReqType(struct iso20_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    ServiceDiscoveryReqType->SupportedServiceIDs_isUsed = 0u;
}

void init_iso20_ServiceDiscoveryResType(struct iso20_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    ServiceDiscoveryResType->VASList_isUsed = 0u;
}

void init_iso20_ServiceDetailReqType(struct iso20_ServiceDetailReqType* ServiceDetailReqType) {
    (void) ServiceDetailReqType;
}

void init_iso20_ServiceDetailResType(struct iso20_ServiceDetailResType* ServiceDetailResType) {
    (void) ServiceDetailResType;
}

void init_iso20_ServiceSelectionReqType(struct iso20_ServiceSelectionReqType* ServiceSelectionReqType) {
    ServiceSelectionReqType->SelectedVASList_isUsed = 0u;
}

void init_iso20_ServiceSelectionResType(struct iso20_ServiceSelectionResType* ServiceSelectionResType) {
    (void) ServiceSelectionResType;
}

void init_iso20_ScheduleExchangeReqType(struct iso20_ScheduleExchangeReqType* ScheduleExchangeReqType) {
    ScheduleExchangeReqType->Dynamic_SEReqControlMode_isUsed = 0u;
    ScheduleExchangeReqType->Scheduled_SEReqControlMode_isUsed = 0u;
}

void init_iso20_ScheduleExchangeResType(struct iso20_ScheduleExchangeResType* ScheduleExchangeResType) {
    ScheduleExchangeResType->GoToPause_isUsed = 0u;
    ScheduleExchangeResType->Dynamic_SEResControlMode_isUsed = 0u;
    ScheduleExchangeResType->Scheduled_SEResControlMode_isUsed = 0u;
}

void init_iso20_PowerDeliveryReqType(struct iso20_PowerDeliveryReqType* PowerDeliveryReqType) {
    PowerDeliveryReqType->EVPowerProfile_isUsed = 0u;
    PowerDeliveryReqType->BPT_ChannelSelection_isUsed = 0u;
}

void init_iso20_PowerDeliveryResType(struct iso20_PowerDeliveryResType* PowerDeliveryResType) {
    PowerDeliveryResType->EVSEStatus_isUsed = 0u;
}

void init_iso20_MeteringConfirmationReqType(struct iso20_MeteringConfirmationReqType* MeteringConfirmationReqType) {
    (void) MeteringConfirmationReqType;
}

void init_iso20_MeteringConfirmationResType(struct iso20_MeteringConfirmationResType* MeteringConfirmationResType) {
    (void) MeteringConfirmationResType;
}

void init_iso20_SessionStopReqType(struct iso20_SessionStopReqType* SessionStopReqType) {
    SessionStopReqType->EVTerminationCode_isUsed = 0u;
    SessionStopReqType->EVTerminationExplanation_isUsed = 0u;
}

void init_iso20_SessionStopResType(struct iso20_SessionStopResType* SessionStopResType) {
    (void) SessionStopResType;
}

void init_iso20_CertificateInstallationReqType(struct iso20_CertificateInstallationReqType* CertificateInstallationReqType) {
    CertificateInstallationReqType->PrioritizedEMAIDs_isUsed = 0u;
}

void init_iso20_CertificateInstallationResType(struct iso20_CertificateInstallationResType* CertificateInstallationResType) {
    (void) CertificateInstallationResType;
}

void init_iso20_VehicleCheckInReqType(struct iso20_VehicleCheckInReqType* VehicleCheckInReqType) {
    VehicleCheckInReqType->VehicleFrame_isUsed = 0u;
    VehicleCheckInReqType->DeviceOffset_isUsed = 0u;
    VehicleCheckInReqType->VehicleTravel_isUsed = 0u;
}

void init_iso20_VehicleCheckInResType(struct iso20_VehicleCheckInResType* VehicleCheckInResType) {
    VehicleCheckInResType->ParkingSpace_isUsed = 0u;
    VehicleCheckInResType->DeviceLocation_isUsed = 0u;
    VehicleCheckInResType->TargetDistance_isUsed = 0u;
}

void init_iso20_VehicleCheckOutReqType(struct iso20_VehicleCheckOutReqType* VehicleCheckOutReqType) {
    (void) VehicleCheckOutReqType;
}

void init_iso20_VehicleCheckOutResType(struct iso20_VehicleCheckOutResType* VehicleCheckOutResType) {
    (void) VehicleCheckOutResType;
}

void init_iso20_CLReqControlModeType(struct iso20_CLReqControlModeType* CLReqControlModeType) {
    (void) CLReqControlModeType;
}

void init_iso20_CLResControlModeType(struct iso20_CLResControlModeType* CLResControlModeType) {
    (void) CLResControlModeType;
}

void init_iso20_ManifestType(struct iso20_ManifestType* ManifestType) {
    ManifestType->Reference.arrayLen = 0u;
    ManifestType->Id_isUsed = 0u;
}

void init_iso20_SignaturePropertiesType(struct iso20_SignaturePropertiesType* SignaturePropertiesType) {
    SignaturePropertiesType->Id_isUsed = 0u;
}

// init for fragment
void init_iso20_exiFragment(struct iso20_exiFragment* exiFrag) {
    exiFrag->AbsolutePriceSchedule_isUsed = 0u;
    exiFrag->CertificateInstallationReq_isUsed = 0u;
    exiFrag->MeteringConfirmationReq_isUsed = 0u;
    exiFrag->PnC_AReqAuthorizationMode_isUsed = 0u;
    exiFrag->SignedInfo_isUsed = 0u;
    exiFrag->SignedInstallationData_isUsed = 0u;
}

// init for xmldsig fragment
void init_iso20_xmldsigFragment(struct iso20_xmldsigFragment* xmldsigFrag) {
    xmldsigFrag->CanonicalizationMethod_isUsed = 0u;
    xmldsigFrag->DSAKeyValue_isUsed = 0u;
    xmldsigFrag->DigestMethod_isUsed = 0u;
    xmldsigFrag->KeyInfo_isUsed = 0u;
    xmldsigFrag->KeyValue_isUsed = 0u;
    xmldsigFrag->Manifest_isUsed = 0u;
    xmldsigFrag->Object_isUsed = 0u;
    xmldsigFrag->PGPData_isUsed = 0u;
    xmldsigFrag->RSAKeyValue_isUsed = 0u;
    xmldsigFrag->Reference_isUsed = 0u;
    xmldsigFrag->RetrievalMethod_isUsed = 0u;
    xmldsigFrag->SPKIData_isUsed = 0u;
    xmldsigFrag->Signature_isUsed = 0u;
    xmldsigFrag->SignatureMethod_isUsed = 0u;
    xmldsigFrag->SignatureProperties_isUsed = 0u;
    xmldsigFrag->SignatureProperty_isUsed = 0u;
    xmldsigFrag->SignatureValue_isUsed = 0u;
    xmldsigFrag->SignedInfo_isUsed = 0u;
    xmldsigFrag->Transform_isUsed = 0u;
    xmldsigFrag->Transforms_isUsed = 0u;
    xmldsigFrag->X509Data_isUsed = 0u;
    xmldsigFrag->X509IssuerSerial_isUsed = 0u;
}


