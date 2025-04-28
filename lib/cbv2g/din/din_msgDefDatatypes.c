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
  * @file din_msgDefDatatypes.c
  * @brief Description goes here
  *
  **/
#include "cbv2g/din/din_msgDefDatatypes.h"



// root elements of EXI doc
void init_din_exiDocument(struct din_exiDocument* exiDoc) {
    (void) exiDoc;
}
void init_din_CostType(struct din_CostType* CostType) {
    CostType->amountMultiplier_isUsed = 0u;
}

void init_din_RelativeTimeIntervalType(struct din_RelativeTimeIntervalType* RelativeTimeIntervalType) {
    RelativeTimeIntervalType->duration_isUsed = 0u;
}

void init_din_IntervalType(struct din_IntervalType* IntervalType) {
    (void) IntervalType;
}

void init_din_ConsumptionCostType(struct din_ConsumptionCostType* ConsumptionCostType) {
    ConsumptionCostType->Cost_isUsed = 0u;
}

void init_din_TransformType(struct din_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_din_PMaxScheduleEntryType(struct din_PMaxScheduleEntryType* PMaxScheduleEntryType) {
    PMaxScheduleEntryType->RelativeTimeInterval_isUsed = 0u;
    PMaxScheduleEntryType->TimeInterval_isUsed = 0u;
}

void init_din_SalesTariffEntryType(struct din_SalesTariffEntryType* SalesTariffEntryType) {
    SalesTariffEntryType->RelativeTimeInterval_isUsed = 0u;
    SalesTariffEntryType->TimeInterval_isUsed = 0u;
    SalesTariffEntryType->ConsumptionCost_isUsed = 0u;
}

void init_din_TransformsType(struct din_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_din_DSAKeyValueType(struct din_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_din_X509IssuerSerialType(struct din_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_din_DigestMethodType(struct din_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_din_RSAKeyValueType(struct din_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_din_ParameterType(struct din_ParameterType* ParameterType) {
    ParameterType->boolValue_isUsed = 0u;
    ParameterType->byteValue_isUsed = 0u;
    ParameterType->shortValue_isUsed = 0u;
    ParameterType->intValue_isUsed = 0u;
    ParameterType->physicalValue_isUsed = 0u;
    ParameterType->stringValue_isUsed = 0u;
}

void init_din_PMaxScheduleType(struct din_PMaxScheduleType* PMaxScheduleType) {
    PMaxScheduleType->PMaxScheduleEntry.arrayLen = 0u;
}

void init_din_SalesTariffType(struct din_SalesTariffType* SalesTariffType) {
    SalesTariffType->SalesTariffEntry.arrayLen = 0u;
    SalesTariffType->SalesTariffDescription_isUsed = 0u;
}

void init_din_CanonicalizationMethodType(struct din_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_din_ServiceTagType(struct din_ServiceTagType* ServiceTagType) {
    ServiceTagType->ServiceName_isUsed = 0u;
    ServiceTagType->ServiceScope_isUsed = 0u;
}

void init_din_ServiceType(struct din_ServiceType* ServiceType) {
    (void) ServiceType;
}

void init_din_ParameterSetType(struct din_ParameterSetType* ParameterSetType) {
    (void) ParameterSetType;
}

void init_din_SelectedServiceType(struct din_SelectedServiceType* SelectedServiceType) {
    SelectedServiceType->ParameterSetID_isUsed = 0u;
}

void init_din_SAScheduleTupleType(struct din_SAScheduleTupleType* SAScheduleTupleType) {
    SAScheduleTupleType->SalesTariff_isUsed = 0u;
}

void init_din_AC_EVSEStatusType(struct din_AC_EVSEStatusType* AC_EVSEStatusType) {
    (void) AC_EVSEStatusType;
}

void init_din_SignatureMethodType(struct din_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_din_KeyValueType(struct din_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_din_SubCertificatesType(struct din_SubCertificatesType* SubCertificatesType) {
    (void) SubCertificatesType;
}

void init_din_ProfileEntryType(struct din_ProfileEntryType* ProfileEntryType) {
    (void) ProfileEntryType;
}

void init_din_ReferenceType(struct din_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_din_RetrievalMethodType(struct din_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_din_X509DataType(struct din_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_din_PGPDataType(struct din_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_din_SPKIDataType(struct din_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_din_SignedInfoType(struct din_SignedInfoType* SignedInfoType) {
    SignedInfoType->Id_isUsed = 0u;
}

void init_din_DC_EVStatusType(struct din_DC_EVStatusType* DC_EVStatusType) {
    DC_EVStatusType->EVCabinConditioning_isUsed = 0u;
    DC_EVStatusType->EVRESSConditioning_isUsed = 0u;
}

void init_din_SignatureValueType(struct din_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_din_CertificateChainType(struct din_CertificateChainType* CertificateChainType) {
    CertificateChainType->SubCertificates_isUsed = 0u;
}

void init_din_DC_EVSEStatusType(struct din_DC_EVSEStatusType* DC_EVSEStatusType) {
    DC_EVSEStatusType->EVSEIsolationStatus_isUsed = 0u;
}

void init_din_PhysicalValueType(struct din_PhysicalValueType* PhysicalValueType) {
    PhysicalValueType->Unit_isUsed = 0u;
}

void init_din_ListOfRootCertificateIDsType(struct din_ListOfRootCertificateIDsType* ListOfRootCertificateIDsType) {
    ListOfRootCertificateIDsType->RootCertificateID.arrayLen = 0u;
}

void init_din_PaymentOptionsType(struct din_PaymentOptionsType* PaymentOptionsType) {
    PaymentOptionsType->PaymentOption.arrayLen = 0u;
}

void init_din_SelectedServiceListType(struct din_SelectedServiceListType* SelectedServiceListType) {
    SelectedServiceListType->SelectedService.arrayLen = 0u;
}

void init_din_AC_EVChargeParameterType(struct din_AC_EVChargeParameterType* AC_EVChargeParameterType) {
    (void) AC_EVChargeParameterType;
}

void init_din_DC_EVChargeParameterType(struct din_DC_EVChargeParameterType* DC_EVChargeParameterType) {
    DC_EVChargeParameterType->EVMaximumPowerLimit_isUsed = 0u;
    DC_EVChargeParameterType->EVEnergyCapacity_isUsed = 0u;
    DC_EVChargeParameterType->EVEnergyRequest_isUsed = 0u;
    DC_EVChargeParameterType->FullSOC_isUsed = 0u;
    DC_EVChargeParameterType->BulkSOC_isUsed = 0u;
}

void init_din_EVChargeParameterType(struct din_EVChargeParameterType* EVChargeParameterType) {
    (void) EVChargeParameterType;
}

void init_din_ChargingProfileType(struct din_ChargingProfileType* ChargingProfileType) {
    ChargingProfileType->ProfileEntry.arrayLen = 0u;
}

void init_din_EVSEStatusType(struct din_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_din_KeyInfoType(struct din_KeyInfoType* KeyInfoType) {
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

void init_din_ServiceChargeType(struct din_ServiceChargeType* ServiceChargeType) {
    (void) ServiceChargeType;
}

void init_din_ServiceParameterListType(struct din_ServiceParameterListType* ServiceParameterListType) {
    ServiceParameterListType->ParameterSet.arrayLen = 0u;
}

void init_din_SAScheduleListType(struct din_SAScheduleListType* SAScheduleListType) {
    SAScheduleListType->SAScheduleTuple.arrayLen = 0u;
}

void init_din_SASchedulesType(struct din_SASchedulesType* SASchedulesType) {
    (void) SASchedulesType;
}

void init_din_DC_EVPowerDeliveryParameterType(struct din_DC_EVPowerDeliveryParameterType* DC_EVPowerDeliveryParameterType) {
    DC_EVPowerDeliveryParameterType->BulkChargingComplete_isUsed = 0u;
}

void init_din_EVPowerDeliveryParameterType(struct din_EVPowerDeliveryParameterType* EVPowerDeliveryParameterType) {
    (void) EVPowerDeliveryParameterType;
}

void init_din_ObjectType(struct din_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_din_ServiceTagListType(struct din_ServiceTagListType* ServiceTagListType) {
    (void) ServiceTagListType;
}

void init_din_DC_EVSEChargeParameterType(struct din_DC_EVSEChargeParameterType* DC_EVSEChargeParameterType) {
    DC_EVSEChargeParameterType->EVSEMaximumPowerLimit_isUsed = 0u;
    DC_EVSEChargeParameterType->EVSECurrentRegulationTolerance_isUsed = 0u;
    DC_EVSEChargeParameterType->EVSEEnergyToBeDelivered_isUsed = 0u;
}

void init_din_AC_EVSEChargeParameterType(struct din_AC_EVSEChargeParameterType* AC_EVSEChargeParameterType) {
    (void) AC_EVSEChargeParameterType;
}

void init_din_EVSEChargeParameterType(struct din_EVSEChargeParameterType* EVSEChargeParameterType) {
    (void) EVSEChargeParameterType;
}

void init_din_MeterInfoType(struct din_MeterInfoType* MeterInfoType) {
    MeterInfoType->MeterReading_isUsed = 0u;
    MeterInfoType->SigMeterReading_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->TMeter_isUsed = 0u;
}

void init_din_CertificateInstallationResType(struct din_CertificateInstallationResType* CertificateInstallationResType) {
    (void) CertificateInstallationResType;
}

void init_din_CableCheckReqType(struct din_CableCheckReqType* CableCheckReqType) {
    (void) CableCheckReqType;
}

void init_din_CableCheckResType(struct din_CableCheckResType* CableCheckResType) {
    (void) CableCheckResType;
}

void init_din_PreChargeReqType(struct din_PreChargeReqType* PreChargeReqType) {
    (void) PreChargeReqType;
}

void init_din_PreChargeResType(struct din_PreChargeResType* PreChargeResType) {
    (void) PreChargeResType;
}

void init_din_CurrentDemandReqType(struct din_CurrentDemandReqType* CurrentDemandReqType) {
    CurrentDemandReqType->EVMaximumVoltageLimit_isUsed = 0u;
    CurrentDemandReqType->EVMaximumCurrentLimit_isUsed = 0u;
    CurrentDemandReqType->EVMaximumPowerLimit_isUsed = 0u;
    CurrentDemandReqType->BulkChargingComplete_isUsed = 0u;
    CurrentDemandReqType->RemainingTimeToFullSoC_isUsed = 0u;
    CurrentDemandReqType->RemainingTimeToBulkSoC_isUsed = 0u;
}

void init_din_CurrentDemandResType(struct din_CurrentDemandResType* CurrentDemandResType) {
    CurrentDemandResType->EVSEMaximumVoltageLimit_isUsed = 0u;
    CurrentDemandResType->EVSEMaximumCurrentLimit_isUsed = 0u;
    CurrentDemandResType->EVSEMaximumPowerLimit_isUsed = 0u;
}

void init_din_WeldingDetectionReqType(struct din_WeldingDetectionReqType* WeldingDetectionReqType) {
    (void) WeldingDetectionReqType;
}

void init_din_WeldingDetectionResType(struct din_WeldingDetectionResType* WeldingDetectionResType) {
    (void) WeldingDetectionResType;
}

void init_din_SessionSetupReqType(struct din_SessionSetupReqType* SessionSetupReqType) {
    (void) SessionSetupReqType;
}

void init_din_CertificateInstallationReqType(struct din_CertificateInstallationReqType* CertificateInstallationReqType) {
    CertificateInstallationReqType->Id_isUsed = 0u;
}

void init_din_SessionSetupResType(struct din_SessionSetupResType* SessionSetupResType) {
    SessionSetupResType->DateTimeNow_isUsed = 0u;
}

void init_din_ServiceDiscoveryReqType(struct din_ServiceDiscoveryReqType* ServiceDiscoveryReqType) {
    ServiceDiscoveryReqType->ServiceScope_isUsed = 0u;
    ServiceDiscoveryReqType->ServiceCategory_isUsed = 0u;
}

void init_din_ServiceDiscoveryResType(struct din_ServiceDiscoveryResType* ServiceDiscoveryResType) {
    ServiceDiscoveryResType->ServiceList_isUsed = 0u;
}

void init_din_ServiceDetailReqType(struct din_ServiceDetailReqType* ServiceDetailReqType) {
    (void) ServiceDetailReqType;
}

void init_din_ServiceDetailResType(struct din_ServiceDetailResType* ServiceDetailResType) {
    ServiceDetailResType->ServiceParameterList_isUsed = 0u;
}

void init_din_ServicePaymentSelectionReqType(struct din_ServicePaymentSelectionReqType* ServicePaymentSelectionReqType) {
    (void) ServicePaymentSelectionReqType;
}

void init_din_ServicePaymentSelectionResType(struct din_ServicePaymentSelectionResType* ServicePaymentSelectionResType) {
    (void) ServicePaymentSelectionResType;
}

void init_din_PaymentDetailsReqType(struct din_PaymentDetailsReqType* PaymentDetailsReqType) {
    (void) PaymentDetailsReqType;
}

void init_din_PaymentDetailsResType(struct din_PaymentDetailsResType* PaymentDetailsResType) {
    (void) PaymentDetailsResType;
}

void init_din_ContractAuthenticationReqType(struct din_ContractAuthenticationReqType* ContractAuthenticationReqType) {
    ContractAuthenticationReqType->Id_isUsed = 0u;
    ContractAuthenticationReqType->GenChallenge_isUsed = 0u;
}

void init_din_ContractAuthenticationResType(struct din_ContractAuthenticationResType* ContractAuthenticationResType) {
    (void) ContractAuthenticationResType;
}

void init_din_ChargeParameterDiscoveryReqType(struct din_ChargeParameterDiscoveryReqType* ChargeParameterDiscoveryReqType) {
    ChargeParameterDiscoveryReqType->AC_EVChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryReqType->DC_EVChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryReqType->EVChargeParameter_isUsed = 0u;
}

void init_din_ChargeParameterDiscoveryResType(struct din_ChargeParameterDiscoveryResType* ChargeParameterDiscoveryResType) {
    ChargeParameterDiscoveryResType->SAScheduleList_isUsed = 0u;
    ChargeParameterDiscoveryResType->SASchedules_isUsed = 0u;
    ChargeParameterDiscoveryResType->AC_EVSEChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryResType->DC_EVSEChargeParameter_isUsed = 0u;
    ChargeParameterDiscoveryResType->EVSEChargeParameter_isUsed = 0u;
}

void init_din_PowerDeliveryReqType(struct din_PowerDeliveryReqType* PowerDeliveryReqType) {
    PowerDeliveryReqType->ChargingProfile_isUsed = 0u;
    PowerDeliveryReqType->DC_EVPowerDeliveryParameter_isUsed = 0u;
    PowerDeliveryReqType->EVPowerDeliveryParameter_isUsed = 0u;
}

void init_din_PowerDeliveryResType(struct din_PowerDeliveryResType* PowerDeliveryResType) {
    PowerDeliveryResType->AC_EVSEStatus_isUsed = 0u;
    PowerDeliveryResType->DC_EVSEStatus_isUsed = 0u;
    PowerDeliveryResType->EVSEStatus_isUsed = 0u;
}

void init_din_ChargingStatusReqType(struct din_ChargingStatusReqType* ChargingStatusReqType) {
    (void) ChargingStatusReqType;
}

void init_din_ChargingStatusResType(struct din_ChargingStatusResType* ChargingStatusResType) {
    ChargingStatusResType->EVSEMaxCurrent_isUsed = 0u;
    ChargingStatusResType->MeterInfo_isUsed = 0u;
}

void init_din_MeteringReceiptReqType(struct din_MeteringReceiptReqType* MeteringReceiptReqType) {
    MeteringReceiptReqType->Id_isUsed = 0u;
    MeteringReceiptReqType->SAScheduleTupleID_isUsed = 0u;
}

void init_din_MeteringReceiptResType(struct din_MeteringReceiptResType* MeteringReceiptResType) {
    (void) MeteringReceiptResType;
}

void init_din_SessionStopType(struct din_SessionStopType* SessionStopType) {
    (void) SessionStopType;
}

void init_din_SessionStopResType(struct din_SessionStopResType* SessionStopResType) {
    (void) SessionStopResType;
}

void init_din_CertificateUpdateReqType(struct din_CertificateUpdateReqType* CertificateUpdateReqType) {
    CertificateUpdateReqType->Id_isUsed = 0u;
}

void init_din_CertificateUpdateResType(struct din_CertificateUpdateResType* CertificateUpdateResType) {
    (void) CertificateUpdateResType;
}

void init_din_BodyBaseType(struct din_BodyBaseType* BodyBaseType) {
    (void) BodyBaseType;
}

void init_din_NotificationType(struct din_NotificationType* NotificationType) {
    NotificationType->FaultMsg_isUsed = 0u;
}

void init_din_SignatureType(struct din_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_din_MessageHeaderType(struct din_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Notification_isUsed = 0u;
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_din_BodyType(struct din_BodyType* BodyType) {
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
    BodyType->ContractAuthenticationReq_isUsed = 0u;
    BodyType->ContractAuthenticationRes_isUsed = 0u;
    BodyType->CurrentDemandReq_isUsed = 0u;
    BodyType->CurrentDemandRes_isUsed = 0u;
    BodyType->MeteringReceiptReq_isUsed = 0u;
    BodyType->MeteringReceiptRes_isUsed = 0u;
    BodyType->PaymentDetailsReq_isUsed = 0u;
    BodyType->PaymentDetailsRes_isUsed = 0u;
    BodyType->PowerDeliveryReq_isUsed = 0u;
    BodyType->PowerDeliveryRes_isUsed = 0u;
    BodyType->PreChargeReq_isUsed = 0u;
    BodyType->PreChargeRes_isUsed = 0u;
    BodyType->ServiceDetailReq_isUsed = 0u;
    BodyType->ServiceDetailRes_isUsed = 0u;
    BodyType->ServiceDiscoveryReq_isUsed = 0u;
    BodyType->ServiceDiscoveryRes_isUsed = 0u;
    BodyType->ServicePaymentSelectionReq_isUsed = 0u;
    BodyType->ServicePaymentSelectionRes_isUsed = 0u;
    BodyType->SessionSetupReq_isUsed = 0u;
    BodyType->SessionSetupRes_isUsed = 0u;
    BodyType->SessionStopReq_isUsed = 0u;
    BodyType->SessionStopRes_isUsed = 0u;
    BodyType->WeldingDetectionReq_isUsed = 0u;
    BodyType->WeldingDetectionRes_isUsed = 0u;
}

void init_din_V2G_Message(struct din_V2G_Message* V2G_Message) {
    (void) V2G_Message;
}


