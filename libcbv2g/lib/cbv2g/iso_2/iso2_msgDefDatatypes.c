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
  * @file iso2_msgDefDatatypes.c
  * @brief Description goes here
  *
  **/
#include "cbv2g/iso_2/iso2_msgDefDatatypes.h"



// root elements of EXI doc
void init_iso2_exiDocument(struct iso2_exiDocument* exiDoc) {
    (void) exiDoc;
}
void init_iso2_CostType(struct iso2_CostType* CostType) {
    CostType->amountMultiplier_isUsed = 0u;
}

void init_iso2_TransformType(struct iso2_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso2_IntervalType(struct iso2_IntervalType* IntervalType) {
    (void) IntervalType;
}

void init_iso2_ConsumptionCostType(struct iso2_ConsumptionCostType* ConsumptionCostType) {
    ConsumptionCostType->Cost.arrayLen = 0u;
}

void init_iso2_TransformsType(struct iso2_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso2_DSAKeyValueType(struct iso2_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso2_X509IssuerSerialType(struct iso2_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso2_RelativeTimeIntervalType(struct iso2_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    RelativeTimeIntervalType->duration_isUsed = 0u;
}

void init_iso2_PMaxScheduleEntryType(struct iso2_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    PMaxScheduleEntryType->RelativeTimeInterval_isUsed = 0u;
    PMaxScheduleEntryType->TimeInterval_isUsed = 0u;
}

void init_iso2_DigestMethodType(struct iso2_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso2_RSAKeyValueType(struct iso2_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso2_SalesTariffEntryType(struct iso2_SalesTariffEntryType* SalesTariffEntryType) {
    SalesTariffEntryType->ConsumptionCost.arrayLen = 0u;
    SalesTariffEntryType->RelativeTimeInterval_isUsed = 0u;
    SalesTariffEntryType->TimeInterval_isUsed = 0u;
    SalesTariffEntryType->EPriceLevel_isUsed = 0u;
}

void init_iso2_CanonicalizationMethodType(struct iso2_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso2_SignatureMethodType(struct iso2_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso2_KeyValueType(struct iso2_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso2_PhysicalValueType(struct iso2_PhysicalValueType* PhysicalValueType) {
    (void) PhysicalValueType;
}

void init_iso2_ParameterType(struct iso2_ParameterType* ParameterType) {
    ParameterType->boolValue_isUsed = 0u;
    ParameterType->byteValue_isUsed = 0u;
    ParameterType->shortValue_isUsed = 0u;
    ParameterType->intValue_isUsed = 0u;
    ParameterType->physicalValue_isUsed = 0u;
    ParameterType->stringValue_isUsed = 0u;
}

void init_iso2_PMaxScheduleType(struct iso2_PMaxScheduleType* PMaxScheduleType) {
    PMaxScheduleType->PMaxScheduleEntry.arrayLen = 0u;
}

void init_iso2_ReferenceType(struct iso2_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso2_RetrievalMethodType(struct iso2_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso2_SalesTariffType(struct iso2_SalesTariffType* SalesTariffType) {
    SalesTariffType->SalesTariffEntry.arrayLen = 0u;
    SalesTariffType->Id_isUsed = 0u;
    SalesTariffType->SalesTariffDescription_isUsed = 0u;
    SalesTariffType->NumEPriceLevels_isUsed = 0u;
}

void init_iso2_X509DataType(struct iso2_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso2_PGPDataType(struct iso2_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso2_SPKIDataType(struct iso2_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso2_SignedInfoType(struct iso2_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso2_ProfileEntryType(struct iso2_ProfileEntryType* ProfileEntryType) {
    ProfileEntryType->ChargingProfileEntryMaxNumberOfPhasesInUse_isUsed = 0u;
}

void init_iso2_DC_EVStatusType(struct iso2_DC_EVStatusType* DC_EVStatusType) {
    (void) DC_EVStatusType;
}

void init_iso2_ParameterSetType(struct iso2_ParameterSetType* ParameterSetType) {
    ParameterSetType->Parameter.arrayLen = 0u;
}

void init_iso2_SAScheduleTupleType(struct iso2_SAScheduleTupleType* SAScheduleTupleType) {
    SAScheduleTupleType->SalesTariff_isUsed = 0u;
}

void init_iso2_SelectedServiceType(struct iso2_SelectedServiceType* SelectedServiceType) {
    SelectedServiceType->ParameterSetID_isUsed = 0u;
}

void init_iso2_ServiceType(struct iso2_ServiceType* ServiceType) {
    ServiceType->ServiceName_isUsed = 0u;
    ServiceType->ServiceScope_isUsed = 0u;
}

void init_iso2_SignatureValueType(struct iso2_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso2_SubCertificatesType(struct iso2_SubCertificatesType* SubCertificatesType) {
    SubCertificatesType->Certificate.arrayLen = 0u;
}

void init_iso2_KeyInfoType(struct iso2_KeyInfoType* KeyInfoType) {
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

void init_iso2_ObjectType(struct iso2_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso2_SupportedEnergyTransferModeType(struct iso2_SupportedEnergyTransferModeType* SupportedEnergyTransferModeType) {
    SupportedEnergyTransferModeType->EnergyTransferMode.arrayLen = 0u;
}

void init_iso2_CertificateChainType(struct iso2_CertificateChainType* CertificateChainType) {
    CertificateChainType->Id_isUsed = 0u;
    CertificateChainType->SubCertificates_isUsed = 0u;
}

void init_iso2_BodyBaseType(struct iso2_BodyBaseType* BodyBaseType) {
    (void) BodyBaseType;
}

void init_iso2_NotificationType(struct iso2_NotificationType* NotificationType) {
    NotificationType->FaultMsg_isUsed = 0u;
}

void init_iso2_DC_EVSEStatusType(struct iso2_DC_EVSEStatusType* DC_EVSEStatusType) {
    DC_EVSEStatusType->EVSEIsolationStatus_isUsed = 0u;
}

void init_iso2_EVSEStatusType(struct iso2_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_iso2_SelectedServiceListType(struct iso2_SelectedServiceListType* SelectedServiceListType) {
    SelectedServiceListType->SelectedService.arrayLen = 0u;
}

void init_iso2_PaymentOptionListType(struct iso2_PaymentOptionListType* PaymentOptionListType) {
    PaymentOptionListType->PaymentOption.arrayLen = 0u;
}

void init_iso2_SignatureType(struct iso2_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso2_ChargingProfileType(struct iso2_ChargingProfileType* ChargingProfileType) {
    ChargingProfileType->ProfileEntry.arrayLen = 0u;
}

void init_iso2_ServiceParameterListType(struct iso2_ServiceParameterListType* ServiceParameterListType) {
    ServiceParameterListType->ParameterSet.arrayLen = 0u;
}

void init_iso2_ListOfRootCertificateIDsType(struct iso2_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    ListOfRootCertificateIDsType->RootCertificateID.arrayLen = 0u;
}

void init_iso2_EVChargeParameterType(struct iso2_EVChargeParameterType* EVChargeParameterType) {
    EVChargeParameterType->DepartureTime_isUsed = 0u;
}

void init_iso2_AC_EVChargeParameterType(struct iso2_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    AC_EVChargeParameterType->DepartureTime_isUsed = 0u;
}

void init_iso2_DC_EVChargeParameterType(struct iso2_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    DC_EVChargeParameterType->DepartureTime_isUsed = 0u;
    DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed = 0u;
    DC_EVChargeParameterType->EVEnergyCapacity_isUsed = 0u;
    DC_EVChargeParameterType->EVEnergyRequest_isUsed = 0u;
    DC_EVChargeParameterType->FullSOC_isUsed = 0u;
    DC_EVChargeParameterType->BulkSOC_isUsed = 0u;
}

void init_iso2_SASchedulesType(struct iso2_SASchedulesType* SASchedulesType) {
    (void) SASchedulesType;
}

void init_iso2_SAScheduleListType(struct iso2_SAScheduleListType* SAScheduleListType) {
    SAScheduleListType->SAScheduleTuple.arrayLen = 0u;
}

void init_iso2_ChargeServiceType(struct iso2_ChargeServiceType* ChargeServiceType) {
    ChargeServiceType->ServiceName_isUsed = 0u;
    ChargeServiceType->ServiceScope_isUsed = 0u;
}

void init_iso2_EVPowerDeliveryParameterType(struct iso2_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
    (void) EVPowerDeliveryParameterType;
}

void init_iso2_DC_EVPowerDeliveryParameterType(struct iso2_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    DC_EVPowerDeliveryParameterType->BulkChargingComplete_isUsed = 0u;
}

void init_iso2_ContractSignatureEncryptedPrivateKeyType(struct iso2_ContractSignatureEncryptedPrivateKeyType* ContractSignatureEncryptedPrivateKeyType) {
    (void) ContractSignatureEncryptedPrivateKeyType;
}

void init_iso2_EVSEChargeParameterType(struct iso2_EVSEChargeParameterType* EVSEChargeParameterType) {
    (void) EVSEChargeParameterType;
}

void init_iso2_AC_EVSEChargeParameterType(struct iso2_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    (void) AC_EVSEChargeParameterType;
}

void init_iso2_DC_EVSEChargeParameterType(struct iso2_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed = 0u;
    DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered_isUsed = 0u;
}

void init_iso2_ServiceListType(struct iso2_ServiceListType* ServiceListType) {
    ServiceListType->Service.arrayLen = 0u;
}

void init_iso2_DiffieHellmanPublickeyType(struct iso2_DiffieHellmanPublickeyType* DiffieHellmanPublickeyType) {
    (void) DiffieHellmanPublickeyType;
}

void init_iso2_EMAIDType(struct iso2_EMAIDType* EMAIDType) {
    (void) EMAIDType;
}

void init_iso2_AC_EVSEStatusType(struct iso2_AC_EVSEStatusType* AC_EVSEStatusType) {
    (void) AC_EVSEStatusType;
}

void init_iso2_MeterInfoType(struct iso2_MeterInfoType* MeterInfoType) {
    MeterInfoType->MeterReading_isUsed = 0u;
    MeterInfoType->SigMeterReading_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->TMeter_isUsed = 0u;
}

void init_iso2_MessageHeaderType(struct iso2_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Notification_isUsed = 0u;
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso2_PowerDeliveryReqType(struct iso2_PowerDeliveryReqType* PowerDeliveryReqType) {
    PowerDeliveryReqType->ChargingProfile_isUsed = 0u;
    PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 0u;
    PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed = 0u;
}

void init_iso2_CurrentDemandResType(struct iso2_CurrentDemandResType* CurrentDemandResType) {
    CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed = 0u;
    CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 0u;
    CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 0u;
    CurrentDemandResType->MeterInfo_isUsed = 0u;
    CurrentDemandResType->ReceiptRequired_isUsed = 0u;
}

void init_iso2_ChargingStatusResType(struct iso2_ChargingStatusResType* ChargingStatusResType) {
    ChargingStatusResType->EVSEMaxCurrent_isUsed = 0u;
    ChargingStatusResType->MeterInfo_isUsed = 0u;
    ChargingStatusResType->ReceiptRequired_isUsed = 0u;
}

void init_iso2_AuthorizationReqType(struct iso2_AuthorizationReqType* AuthorizationReqType) {
    AuthorizationReqType->Id_isUsed = 0u;
    AuthorizationReqType->GenChallenge_isUsed = 0u;
}

void init_iso2_PreChargeReqType(struct iso2_PreChargeReqType* PreChargeReqType) {
    (void) PreChargeReqType;
}

void init_iso2_ServiceDetailResType(struct iso2_ServiceDetailResType* ServiceDetailResType) {
    ServiceDetailResType->ServiceParameterList_isUsed = 0u;
}

void init_iso2_PaymentServiceSelectionResType(struct iso2_PaymentServiceSelectionResType* PaymentServiceSelectionResType) {
    (void) PaymentServiceSelectionResType;
}

void init_iso2_CertificateUpdateReqType(struct iso2_CertificateUpdateReqType* CertificateUpdateReqType) {
    (void) CertificateUpdateReqType;
}

void init_iso2_SessionSetupResType(struct iso2_SessionSetupResType* SessionSetupResType) {
    SessionSetupResType->EVSETimeStamp_isUsed = 0u;
}

void init_iso2_CertificateInstallationReqType(struct iso2_CertificateInstallationReqType* CertificateInstallationReqType) {
    (void) CertificateInstallationReqType;
}

void init_iso2_CertificateInstallationResType(struct iso2_CertificateInstallationResType* CertificateInstallationResType) {
    (void) CertificateInstallationResType;
}

void init_iso2_WeldingDetectionResType(struct iso2_WeldingDetectionResType* WeldingDetectionResType) {
    (void) WeldingDetectionResType;
}

void init_iso2_CurrentDemandReqType(struct iso2_CurrentDemandReqType* CurrentDemandReqType) {
    CurrentDemandReqType->EVMaximumVoltageLimit_isUsed = 0u;
    CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 0u;
    CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 0u;
    CurrentDemandReqType->BulkChargingComplete_isUsed = 0u;
    CurrentDemandReqType->RemainingTimeToFullSoC_isUsed = 0u;
    CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 0u;
}

void init_iso2_PreChargeResType(struct iso2_PreChargeResType* PreChargeResType) {
    (void) PreChargeResType;
}

void init_iso2_CertificateUpdateResType(struct iso2_CertificateUpdateResType* CertificateUpdateResType) {
    CertificateUpdateResType->RetryCounter_isUsed = 0u;
}

void init_iso2_MeteringReceiptReqType(struct iso2_MeteringReceiptReqType* MeteringReceiptReqType) {
    MeteringReceiptReqType->Id_isUsed = 0u;
    MeteringReceiptReqType->SAScheduleTupleID_isUsed = 0u;
}

void init_iso2_ChargingStatusReqType(struct iso2_ChargingStatusReqType* ChargingStatusReqType) {
    (void) ChargingStatusReqType;
}

void init_iso2_SessionStopResType(struct iso2_SessionStopResType* SessionStopResType) {
    (void) SessionStopResType;
}

void init_iso2_ChargeParameterDiscoveryReqType(struct iso2_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    ChargeParameterDiscoveryReqType->MaxEntriesSAScheduleTuple_isUsed = 0u;
    ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryReqType->DC_EVChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryReqType->EVChargeParameter_isUsed = 0u;
}

void init_iso2_CableCheckReqType(struct iso2_CableCheckReqType* CableCheckReqType) {
    (void) CableCheckReqType;
}

void init_iso2_WeldingDetectionReqType(struct iso2_WeldingDetectionReqType* WeldingDetectionReqType) {
    (void) WeldingDetectionReqType;
}

void init_iso2_PowerDeliveryResType(struct iso2_PowerDeliveryResType* PowerDeliveryResType) {
    PowerDeliveryResType->AC_EVSEStatus_isUsed = 0u;
    PowerDeliveryResType->DC_EVSEStatus_isUsed = 0u;
    PowerDeliveryResType->EVSEStatus_isUsed = 0u;
}

void init_iso2_ChargeParameterDiscoveryResType(struct iso2_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    ChargeParameterDiscoveryResType->SAScheduleList_isUsed = 0u;
    ChargeParameterDiscoveryResType->SASchedules_isUsed = 0u;
    ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryResType->EVSEChargeParameter_isUsed = 0u;
}

void init_iso2_PaymentServiceSelectionReqType(struct iso2_PaymentServiceSelectionReqType* PaymentServiceSelectionReqType) {
    (void) PaymentServiceSelectionReqType;
}

void init_iso2_MeteringReceiptResType(struct iso2_MeteringReceiptResType* MeteringReceiptResType) {
    MeteringReceiptResType->AC_EVSEStatus_isUsed = 0u;
    MeteringReceiptResType->DC_EVSEStatus_isUsed = 0u;
    MeteringReceiptResType->EVSEStatus_isUsed = 0u;
}

void init_iso2_CableCheckResType(struct iso2_CableCheckResType* CableCheckResType) {
    (void) CableCheckResType;
}

void init_iso2_ServiceDiscoveryResType(struct iso2_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    ServiceDiscoveryResType->ServiceList_isUsed = 0u;
}

void init_iso2_ServiceDetailReqType(struct iso2_ServiceDetailReqType* ServiceDetailReqType) {
    (void) ServiceDetailReqType;
}

void init_iso2_SessionSetupReqType(struct iso2_SessionSetupReqType* SessionSetupReqType) {
    (void) SessionSetupReqType;
}

void init_iso2_SessionStopReqType(struct iso2_SessionStopReqType* SessionStopReqType) {
    (void) SessionStopReqType;
}

void init_iso2_ServiceDiscoveryReqType(struct iso2_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    ServiceDiscoveryReqType->ServiceScope_isUsed = 0u;
    ServiceDiscoveryReqType->ServiceCategory_isUsed = 0u;
}

void init_iso2_AuthorizationResType(struct iso2_AuthorizationResType* AuthorizationResType) {
    (void) AuthorizationResType;
}

void init_iso2_PaymentDetailsReqType(struct iso2_PaymentDetailsReqType* PaymentDetailsReqType) {
    (void) PaymentDetailsReqType;
}

void init_iso2_PaymentDetailsResType(struct iso2_PaymentDetailsResType* PaymentDetailsResType) {
    (void) PaymentDetailsResType;
}

void init_iso2_BodyType(struct iso2_BodyType* BodyType) {
    BodyType->AuthorizationReq_isUsed = 0u;
    BodyType->AuthorizationRes_isUsed = 0u;
    BodyType->BodyElement_isUsed = 0u;
    BodyType->CableCheckReq_isUsed = 0u;
    BodyType->CableCheckRes_isUsed = 0u;
    BodyType->CertificateInstallationReq_isUsed = 0u;
    BodyType->CertificateInstallationRes_isUsed = 0u;
    BodyType->CertificateUpdateReq_isUsed = 0u;
    BodyType->CertificateUpdateRes_isUsed = 0u;
    BodyType->ChargeParameterDiscoveryReq_isUsed = 0u;
    BodyType->ChargeParameterDiscoveryRes_isUsed = 0u;
    BodyType->ChargingStatusReq_isUsed = 0u;
    BodyType->ChargingStatusRes_isUsed = 0u;
    BodyType->CurrentDemandReq_isUsed = 0u;
    BodyType->CurrentDemandRes_isUsed = 0u;
    BodyType->MeteringReceiptReq_isUsed = 0u;
    BodyType->MeteringReceiptRes_isUsed = 0u;
    BodyType->PaymentDetailsReq_isUsed = 0u;
    BodyType->PaymentDetailsRes_isUsed = 0u;
    BodyType->PaymentServiceSelectionReq_isUsed = 0u;
    BodyType->PaymentServiceSelectionRes_isUsed = 0u;
    BodyType->PowerDeliveryReq_isUsed = 0u;
    BodyType->PowerDeliveryRes_isUsed = 0u;
    BodyType->PreChargeReq_isUsed = 0u;
    BodyType->PreChargeRes_isUsed = 0u;
    BodyType->ServiceDetailReq_isUsed = 0u;
    BodyType->ServiceDetailRes_isUsed = 0u;
    BodyType->ServiceDiscoveryReq_isUsed = 0u;
    BodyType->ServiceDiscoveryRes_isUsed = 0u;
    BodyType->SessionSetupReq_isUsed = 0u;
    BodyType->SessionSetupRes_isUsed = 0u;
    BodyType->SessionStopReq_isUsed = 0u;
    BodyType->SessionStopRes_isUsed = 0u;
    BodyType->WeldingDetectionReq_isUsed = 0u;
    BodyType->WeldingDetectionRes_isUsed = 0u;
}

void init_iso2_V2G_Message(struct iso2_V2G_Message* V2G_Message) {
    (void) V2G_Message;
}

// init for fragment
void init_iso2_exiFragment(struct iso2_exiFragment* exiFrag) {
    exiFrag->AuthorizationReq_isUsed = 0u;
    exiFrag->CertificateInstallationReq_isUsed = 0u;
    exiFrag->CertificateUpdateReq_isUsed = 0u;
    exiFrag->ContractSignatureCertChain_isUsed = 0u;
    exiFrag->ContractSignatureEncryptedPrivateKey_isUsed = 0u;
    exiFrag->DHpublickey_isUsed = 0u;
    exiFrag->MeteringReceiptReq_isUsed = 0u;
    exiFrag->SalesTariff_isUsed = 0u;
    exiFrag->SignedInfo_isUsed = 0u;
    exiFrag->eMAID_isUsed = 0u;
}

// init for xmldsig fragment
void init_iso2_xmldsigFragment(struct iso2_xmldsigFragment* xmldsigFrag) {
    xmldsigFrag->CanonicalizationMethod_isUsed = 0u;
    xmldsigFrag->DSAKeyValue_isUsed = 0u;
    xmldsigFrag->DigestMethod_isUsed = 0u;
    xmldsigFrag->KeyInfo_isUsed = 0u;
    xmldsigFrag->KeyValue_isUsed = 0u;
    xmldsigFrag->Object_isUsed = 0u;
    xmldsigFrag->PGPData_isUsed = 0u;
    xmldsigFrag->RSAKeyValue_isUsed = 0u;
    xmldsigFrag->Reference_isUsed = 0u;
    xmldsigFrag->RetrievalMethod_isUsed = 0u;
    xmldsigFrag->SPKIData_isUsed = 0u;
    xmldsigFrag->Signature_isUsed = 0u;
    xmldsigFrag->SignatureMethod_isUsed = 0u;
    xmldsigFrag->SignatureValue_isUsed = 0u;
    xmldsigFrag->SignedInfo_isUsed = 0u;
    xmldsigFrag->Transform_isUsed = 0u;
    xmldsigFrag->Transforms_isUsed = 0u;
    xmldsigFrag->X509Data_isUsed = 0u;
    xmldsigFrag->X509IssuerSerial_isUsed = 0u;
}


