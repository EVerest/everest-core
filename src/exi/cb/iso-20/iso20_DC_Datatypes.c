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
  * @file iso20_DC_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "iso20_DC_Datatypes.h"



// root elements of EXI doc
void init_iso20_dc_exiDocument(struct iso20_dc_exiDocument* exiDoc) {
    exiDoc->DC_ChargeParameterDiscoveryReq_isUsed = 0u;
    exiDoc->DC_ChargeParameterDiscoveryRes_isUsed = 0u;
    exiDoc->DC_CableCheckReq_isUsed = 0u;
    exiDoc->DC_CableCheckRes_isUsed = 0u;
    exiDoc->DC_PreChargeReq_isUsed = 0u;
    exiDoc->DC_PreChargeRes_isUsed = 0u;
    exiDoc->DC_ChargeLoopReq_isUsed = 0u;
    exiDoc->DC_ChargeLoopRes_isUsed = 0u;
    exiDoc->DC_WeldingDetectionReq_isUsed = 0u;
    exiDoc->DC_WeldingDetectionRes_isUsed = 0u;
    exiDoc->DC_CPDReqEnergyTransferMode_isUsed = 0u;
    exiDoc->DC_CPDResEnergyTransferMode_isUsed = 0u;
    exiDoc->BPT_DC_CPDReqEnergyTransferMode_isUsed = 0u;
    exiDoc->BPT_DC_CPDResEnergyTransferMode_isUsed = 0u;
    exiDoc->Scheduled_DC_CLReqControlMode_isUsed = 0u;
    exiDoc->Scheduled_DC_CLResControlMode_isUsed = 0u;
    exiDoc->BPT_Scheduled_DC_CLReqControlMode_isUsed = 0u;
    exiDoc->BPT_Scheduled_DC_CLResControlMode_isUsed = 0u;
    exiDoc->Dynamic_DC_CLReqControlMode_isUsed = 0u;
    exiDoc->Dynamic_DC_CLResControlMode_isUsed = 0u;
    exiDoc->BPT_Dynamic_DC_CLReqControlMode_isUsed = 0u;
    exiDoc->BPT_Dynamic_DC_CLResControlMode_isUsed = 0u;
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
void init_iso20_dc_TransformType(struct iso20_dc_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso20_dc_TransformsType(struct iso20_dc_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso20_dc_DSAKeyValueType(struct iso20_dc_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso20_dc_X509IssuerSerialType(struct iso20_dc_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso20_dc_DigestMethodType(struct iso20_dc_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso20_dc_RSAKeyValueType(struct iso20_dc_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso20_dc_CanonicalizationMethodType(struct iso20_dc_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso20_dc_SignatureMethodType(struct iso20_dc_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso20_dc_KeyValueType(struct iso20_dc_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso20_dc_ReferenceType(struct iso20_dc_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso20_dc_RetrievalMethodType(struct iso20_dc_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso20_dc_X509DataType(struct iso20_dc_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso20_dc_PGPDataType(struct iso20_dc_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso20_dc_SPKIDataType(struct iso20_dc_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso20_dc_SignedInfoType(struct iso20_dc_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso20_dc_SignatureValueType(struct iso20_dc_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso20_dc_KeyInfoType(struct iso20_dc_KeyInfoType* KeyInfoType) {
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

void init_iso20_dc_ObjectType(struct iso20_dc_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso20_dc_RationalNumberType(struct iso20_dc_RationalNumberType* RationalNumberType) {
    (void) RationalNumberType;
}

void init_iso20_dc_DetailedCostType(struct iso20_dc_DetailedCostType* DetailedCostType) {
    (void) DetailedCostType;
}

void init_iso20_dc_SignatureType(struct iso20_dc_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso20_dc_DetailedTaxType(struct iso20_dc_DetailedTaxType* DetailedTaxType) {
    (void) DetailedTaxType;
}

void init_iso20_dc_MessageHeaderType(struct iso20_dc_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso20_dc_SignaturePropertyType(struct iso20_dc_SignaturePropertyType* SignaturePropertyType) {
    SignaturePropertyType->Id_isUsed = 0u;
    SignaturePropertyType->ANY_isUsed = 0u;
}

void init_iso20_dc_DC_CPDReqEnergyTransferModeType(struct iso20_dc_DC_CPDReqEnergyTransferModeType* DC_CPDReqEnergyTransferModeType) {
    DC_CPDReqEnergyTransferModeType->TargetSOC_isUsed = 0u;
}

void init_iso20_dc_DisplayParametersType(struct iso20_dc_DisplayParametersType* DisplayParametersType) {
    DisplayParametersType->PresentSOC_isUsed = 0u;
    DisplayParametersType->MinimumSOC_isUsed = 0u;
    DisplayParametersType->TargetSOC_isUsed = 0u;
    DisplayParametersType->MaximumSOC_isUsed = 0u;
    DisplayParametersType->RemainingTimeToMinimumSOC_isUsed = 0u;
    DisplayParametersType->RemainingTimeToTargetSOC_isUsed = 0u;
    DisplayParametersType->RemainingTimeToMaximumSOC_isUsed = 0u;
    DisplayParametersType->ChargingComplete_isUsed = 0u;
    DisplayParametersType->BatteryEnergyCapacity_isUsed = 0u;
    DisplayParametersType->InletHot_isUsed = 0u;
}

void init_iso20_dc_DC_CPDResEnergyTransferModeType(struct iso20_dc_DC_CPDResEnergyTransferModeType* DC_CPDResEnergyTransferModeType) {
    DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 0u;
}

void init_iso20_dc_EVSEStatusType(struct iso20_dc_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_iso20_dc_MeterInfoType(struct iso20_dc_MeterInfoType* MeterInfoType) {
    MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 0u;
    MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->MeterSignature_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->MeterTimestamp_isUsed = 0u;
}

void init_iso20_dc_Scheduled_DC_CLReqControlModeType(struct iso20_dc_Scheduled_DC_CLReqControlModeType* Scheduled_DC_CLReqControlModeType) {
    Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMaximumChargePower_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 0u;
    Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 0u;
}

void init_iso20_dc_Dynamic_DC_CLReqControlModeType(struct iso20_dc_Dynamic_DC_CLReqControlModeType* Dynamic_DC_CLReqControlModeType) {
    Dynamic_DC_CLReqControlModeType->DepartureTime_isUsed = 0u;
}

void init_iso20_dc_CLReqControlModeType(struct iso20_dc_CLReqControlModeType* CLReqControlModeType) {
    (void) CLReqControlModeType;
}

void init_iso20_dc_ReceiptType(struct iso20_dc_ReceiptType* ReceiptType) {
    ReceiptType->TaxCosts.arrayLen = 0u;
    ReceiptType->EnergyCosts_isUsed = 0u;
    ReceiptType->OccupancyCosts_isUsed = 0u;
    ReceiptType->AdditionalServicesCosts_isUsed = 0u;
    ReceiptType->OverstayCosts_isUsed = 0u;
}

void init_iso20_dc_Scheduled_DC_CLResControlModeType(struct iso20_dc_Scheduled_DC_CLResControlModeType* Scheduled_DC_CLResControlModeType) {
    Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower_isUsed = 0u;
    Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 0u;
    Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 0u;
    Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 0u;
}

void init_iso20_dc_Dynamic_DC_CLResControlModeType(struct iso20_dc_Dynamic_DC_CLResControlModeType* Dynamic_DC_CLResControlModeType) {
    Dynamic_DC_CLResControlModeType->DepartureTime_isUsed = 0u;
    Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 0u;
    Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 0u;
    Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 0u;
}

void init_iso20_dc_CLResControlModeType(struct iso20_dc_CLResControlModeType* CLResControlModeType) {
    (void) CLResControlModeType;
}

void init_iso20_dc_DC_ChargeParameterDiscoveryReqType(struct iso20_dc_DC_ChargeParameterDiscoveryReqType* DC_ChargeParameterDiscoveryReqType) {
    DC_ChargeParameterDiscoveryReqType->BPT_DC_CPDReqEnergyTransferMode_isUsed = 0u;
    DC_ChargeParameterDiscoveryReqType->DC_CPDReqEnergyTransferMode_isUsed = 0u;
}

void init_iso20_dc_DC_ChargeParameterDiscoveryResType(struct iso20_dc_DC_ChargeParameterDiscoveryResType* DC_ChargeParameterDiscoveryResType) {
    DC_ChargeParameterDiscoveryResType->BPT_DC_CPDResEnergyTransferMode_isUsed = 0u;
    DC_ChargeParameterDiscoveryResType->DC_CPDResEnergyTransferMode_isUsed = 0u;
}

void init_iso20_dc_DC_CableCheckReqType(struct iso20_dc_DC_CableCheckReqType* DC_CableCheckReqType) {
    (void) DC_CableCheckReqType;
}

void init_iso20_dc_DC_CableCheckResType(struct iso20_dc_DC_CableCheckResType* DC_CableCheckResType) {
    (void) DC_CableCheckResType;
}

void init_iso20_dc_DC_PreChargeReqType(struct iso20_dc_DC_PreChargeReqType* DC_PreChargeReqType) {
    (void) DC_PreChargeReqType;
}

void init_iso20_dc_DC_PreChargeResType(struct iso20_dc_DC_PreChargeResType* DC_PreChargeResType) {
    (void) DC_PreChargeResType;
}

void init_iso20_dc_DC_ChargeLoopReqType(struct iso20_dc_DC_ChargeLoopReqType* DC_ChargeLoopReqType) {
    DC_ChargeLoopReqType->DisplayParameters_isUsed = 0u;
    DC_ChargeLoopReqType->BPT_Dynamic_DC_CLReqControlMode_isUsed = 0u;
    DC_ChargeLoopReqType->BPT_Scheduled_DC_CLReqControlMode_isUsed = 0u;
    DC_ChargeLoopReqType->CLReqControlMode_isUsed = 0u;
    DC_ChargeLoopReqType->Dynamic_DC_CLReqControlMode_isUsed = 0u;
    DC_ChargeLoopReqType->Scheduled_DC_CLReqControlMode_isUsed = 0u;
}

void init_iso20_dc_DC_ChargeLoopResType(struct iso20_dc_DC_ChargeLoopResType* DC_ChargeLoopResType) {
    DC_ChargeLoopResType->EVSEStatus_isUsed = 0u;
    DC_ChargeLoopResType->MeterInfo_isUsed = 0u;
    DC_ChargeLoopResType->Receipt_isUsed = 0u;
    DC_ChargeLoopResType->BPT_Dynamic_DC_CLResControlMode_isUsed = 0u;
    DC_ChargeLoopResType->BPT_Scheduled_DC_CLResControlMode_isUsed = 0u;
    DC_ChargeLoopResType->CLResControlMode_isUsed = 0u;
    DC_ChargeLoopResType->Dynamic_DC_CLResControlMode_isUsed = 0u;
    DC_ChargeLoopResType->Scheduled_DC_CLResControlMode_isUsed = 0u;
}

void init_iso20_dc_DC_WeldingDetectionReqType(struct iso20_dc_DC_WeldingDetectionReqType* DC_WeldingDetectionReqType) {
    (void) DC_WeldingDetectionReqType;
}

void init_iso20_dc_DC_WeldingDetectionResType(struct iso20_dc_DC_WeldingDetectionResType* DC_WeldingDetectionResType) {
    (void) DC_WeldingDetectionResType;
}

void init_iso20_dc_BPT_DC_CPDReqEnergyTransferModeType(struct iso20_dc_BPT_DC_CPDReqEnergyTransferModeType* BPT_DC_CPDReqEnergyTransferModeType) {
    BPT_DC_CPDReqEnergyTransferModeType->TargetSOC_isUsed = 0u;
}

void init_iso20_dc_BPT_DC_CPDResEnergyTransferModeType(struct iso20_dc_BPT_DC_CPDResEnergyTransferModeType* BPT_DC_CPDResEnergyTransferModeType) {
    BPT_DC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 0u;
}

void init_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType* BPT_Scheduled_DC_CLReqControlModeType) {
    BPT_Scheduled_DC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMinimumChargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumChargeCurrent_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumVoltage_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMinimumVoltage_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLReqControlModeType->EVMaximumDischargeCurrent_isUsed = 0u;
}

void init_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType* BPT_Scheduled_DC_CLResControlModeType) {
    BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumChargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumChargeCurrent_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumVoltage_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumDischargePower_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMaximumDischargeCurrent_isUsed = 0u;
    BPT_Scheduled_DC_CLResControlModeType->EVSEMinimumVoltage_isUsed = 0u;
}

void init_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType* BPT_Dynamic_DC_CLReqControlModeType) {
    BPT_Dynamic_DC_CLReqControlModeType->DepartureTime_isUsed = 0u;
    BPT_Dynamic_DC_CLReqControlModeType->EVMaximumV2XEnergyRequest_isUsed = 0u;
    BPT_Dynamic_DC_CLReqControlModeType->EVMinimumV2XEnergyRequest_isUsed = 0u;
}

void init_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType* BPT_Dynamic_DC_CLResControlModeType) {
    BPT_Dynamic_DC_CLResControlModeType->DepartureTime_isUsed = 0u;
    BPT_Dynamic_DC_CLResControlModeType->MinimumSOC_isUsed = 0u;
    BPT_Dynamic_DC_CLResControlModeType->TargetSOC_isUsed = 0u;
    BPT_Dynamic_DC_CLResControlModeType->AckMaxDelay_isUsed = 0u;
}

void init_iso20_dc_ManifestType(struct iso20_dc_ManifestType* ManifestType) {
    ManifestType->Reference.arrayLen = 0u;
    ManifestType->Id_isUsed = 0u;
}

void init_iso20_dc_SignaturePropertiesType(struct iso20_dc_SignaturePropertiesType* SignaturePropertiesType) {
    SignaturePropertiesType->Id_isUsed = 0u;
}

// init for fragment
void init_iso20_dc_exiFragment(struct iso20_dc_exiFragment* exiFrag) {
    exiFrag->DC_ChargeParameterDiscoveryRes_isUsed = 0u;
    exiFrag->SignedInfo_isUsed = 0u;
}

// init for xmldsig fragment
void init_iso20_dc_xmldsigFragment(struct iso20_dc_xmldsigFragment* xmldsigFrag) {
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


