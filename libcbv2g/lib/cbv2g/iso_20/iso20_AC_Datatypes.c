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
  * @file iso20_AC_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "cbv2g/iso_20/iso20_AC_Datatypes.h"



// root elements of EXI doc
void init_iso20_ac_exiDocument(struct iso20_ac_exiDocument* exiDoc) {
    exiDoc->AC_ChargeParameterDiscoveryReq_isUsed = 0u;
    exiDoc->AC_ChargeParameterDiscoveryRes_isUsed = 0u;
    exiDoc->AC_ChargeLoopReq_isUsed = 0u;
    exiDoc->AC_ChargeLoopRes_isUsed = 0u;
    exiDoc->AC_CPDReqEnergyTransferMode_isUsed = 0u;
    exiDoc->AC_CPDResEnergyTransferMode_isUsed = 0u;
    exiDoc->BPT_AC_CPDReqEnergyTransferMode_isUsed = 0u;
    exiDoc->BPT_AC_CPDResEnergyTransferMode_isUsed = 0u;
    exiDoc->Scheduled_AC_CLReqControlMode_isUsed = 0u;
    exiDoc->Scheduled_AC_CLResControlMode_isUsed = 0u;
    exiDoc->BPT_Scheduled_AC_CLReqControlMode_isUsed = 0u;
    exiDoc->BPT_Scheduled_AC_CLResControlMode_isUsed = 0u;
    exiDoc->Dynamic_AC_CLReqControlMode_isUsed = 0u;
    exiDoc->Dynamic_AC_CLResControlMode_isUsed = 0u;
    exiDoc->BPT_Dynamic_AC_CLReqControlMode_isUsed = 0u;
    exiDoc->BPT_Dynamic_AC_CLResControlMode_isUsed = 0u;
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
void init_iso20_ac_TransformType(struct iso20_ac_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso20_ac_TransformsType(struct iso20_ac_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso20_ac_DSAKeyValueType(struct iso20_ac_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso20_ac_X509IssuerSerialType(struct iso20_ac_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso20_ac_DigestMethodType(struct iso20_ac_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso20_ac_RSAKeyValueType(struct iso20_ac_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso20_ac_CanonicalizationMethodType(struct iso20_ac_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso20_ac_SignatureMethodType(struct iso20_ac_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso20_ac_KeyValueType(struct iso20_ac_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso20_ac_ReferenceType(struct iso20_ac_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso20_ac_RetrievalMethodType(struct iso20_ac_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso20_ac_X509DataType(struct iso20_ac_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso20_ac_PGPDataType(struct iso20_ac_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso20_ac_SPKIDataType(struct iso20_ac_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso20_ac_SignedInfoType(struct iso20_ac_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso20_ac_SignatureValueType(struct iso20_ac_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso20_ac_KeyInfoType(struct iso20_ac_KeyInfoType* KeyInfoType) {
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

void init_iso20_ac_ObjectType(struct iso20_ac_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso20_ac_RationalNumberType(struct iso20_ac_RationalNumberType* RationalNumberType) {
    (void) RationalNumberType;
}

void init_iso20_ac_DetailedCostType(struct iso20_ac_DetailedCostType* DetailedCostType) {
    (void) DetailedCostType;
}

void init_iso20_ac_SignatureType(struct iso20_ac_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso20_ac_DetailedTaxType(struct iso20_ac_DetailedTaxType* DetailedTaxType) {
    (void) DetailedTaxType;
}

void init_iso20_ac_MessageHeaderType(struct iso20_ac_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso20_ac_SignaturePropertyType(struct iso20_ac_SignaturePropertyType* SignaturePropertyType) {
    SignaturePropertyType->Id_isUsed = 0u;
    SignaturePropertyType->ANY_isUsed = 0u;
}

void init_iso20_ac_AC_CPDReqEnergyTransferModeType(struct iso20_ac_AC_CPDReqEnergyTransferModeType* AC_CPDReqEnergyTransferModeType) {
    AC_CPDReqEnergyTransferModeType->EVMaximumChargePower_L2_isUsed = 0u;
    AC_CPDReqEnergyTransferModeType->EVMaximumChargePower_L3_isUsed = 0u;
    AC_CPDReqEnergyTransferModeType->EVMinimumChargePower_L2_isUsed = 0u;
    AC_CPDReqEnergyTransferModeType->EVMinimumChargePower_L3_isUsed = 0u;
}

void init_iso20_ac_DisplayParametersType(struct iso20_ac_DisplayParametersType* DisplayParametersType) {
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

void init_iso20_ac_AC_CPDResEnergyTransferModeType(struct iso20_ac_AC_CPDResEnergyTransferModeType* AC_CPDResEnergyTransferModeType) {
    AC_CPDResEnergyTransferModeType->EVSEMaximumChargePower_L2_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEMaximumChargePower_L3_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEMinimumChargePower_L2_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEMinimumChargePower_L3_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->MaximumPowerAsymmetry_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_L3_isUsed = 0u;
}

void init_iso20_ac_EVSEStatusType(struct iso20_ac_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_iso20_ac_Dynamic_AC_CLReqControlModeType(struct iso20_ac_Dynamic_AC_CLReqControlModeType* Dynamic_AC_CLReqControlModeType) {
    Dynamic_AC_CLReqControlModeType->DepartureTime_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVMaximumChargePower_L2_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVMaximumChargePower_L3_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVMinimumChargePower_L2_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVMinimumChargePower_L3_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVPresentActivePower_L2_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVPresentActivePower_L3_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVPresentReactivePower_L2_isUsed = 0u;
    Dynamic_AC_CLReqControlModeType->EVPresentReactivePower_L3_isUsed = 0u;
}

void init_iso20_ac_Scheduled_AC_CLReqControlModeType(struct iso20_ac_Scheduled_AC_CLReqControlModeType* Scheduled_AC_CLReqControlModeType) {
    Scheduled_AC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_L2_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_L3_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_L2_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_L3_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVPresentActivePower_L2_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVPresentActivePower_L3_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_L2_isUsed = 0u;
    Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_L3_isUsed = 0u;
}

void init_iso20_ac_CLReqControlModeType(struct iso20_ac_CLReqControlModeType* CLReqControlModeType) {
    (void) CLReqControlModeType;
}

void init_iso20_ac_MeterInfoType(struct iso20_ac_MeterInfoType* MeterInfoType) {
    MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 0u;
    MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->MeterSignature_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->MeterTimestamp_isUsed = 0u;
}

void init_iso20_ac_ReceiptType(struct iso20_ac_ReceiptType* ReceiptType) {
    ReceiptType->TaxCosts.arrayLen = 0u;
    ReceiptType->EnergyCosts_isUsed = 0u;
    ReceiptType->OccupancyCosts_isUsed = 0u;
    ReceiptType->AdditionalServicesCosts_isUsed = 0u;
    ReceiptType->OverstayCosts_isUsed = 0u;
}

void init_iso20_ac_Scheduled_AC_CLResControlModeType(struct iso20_ac_Scheduled_AC_CLResControlModeType* Scheduled_AC_CLResControlModeType) {
    Scheduled_AC_CLResControlModeType->EVSETargetActivePower_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSETargetActivePower_L2_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSETargetActivePower_L3_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_L2_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_L3_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_L3_isUsed = 0u;
}

void init_iso20_ac_Dynamic_AC_CLResControlModeType(struct iso20_ac_Dynamic_AC_CLResControlModeType* Dynamic_AC_CLResControlModeType) {
    Dynamic_AC_CLResControlModeType->DepartureTime_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->MinimumSOC_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->TargetSOC_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->AckMaxDelay_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSETargetActivePower_L2_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSETargetActivePower_L3_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_L2_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_L3_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_L3_isUsed = 0u;
}

void init_iso20_ac_CLResControlModeType(struct iso20_ac_CLResControlModeType* CLResControlModeType) {
    (void) CLResControlModeType;
}

void init_iso20_ac_AC_ChargeParameterDiscoveryReqType(struct iso20_ac_AC_ChargeParameterDiscoveryReqType* AC_ChargeParameterDiscoveryReqType) {
    AC_ChargeParameterDiscoveryReqType->AC_CPDReqEnergyTransferMode_isUsed = 0u;
    AC_ChargeParameterDiscoveryReqType->BPT_AC_CPDReqEnergyTransferMode_isUsed = 0u;
}

void init_iso20_ac_AC_ChargeParameterDiscoveryResType(struct iso20_ac_AC_ChargeParameterDiscoveryResType* AC_ChargeParameterDiscoveryResType) {
    AC_ChargeParameterDiscoveryResType->AC_CPDResEnergyTransferMode_isUsed = 0u;
    AC_ChargeParameterDiscoveryResType->BPT_AC_CPDResEnergyTransferMode_isUsed = 0u;
}

void init_iso20_ac_AC_ChargeLoopReqType(struct iso20_ac_AC_ChargeLoopReqType* AC_ChargeLoopReqType) {
    AC_ChargeLoopReqType->DisplayParameters_isUsed = 0u;
    AC_ChargeLoopReqType->BPT_Dynamic_AC_CLReqControlMode_isUsed = 0u;
    AC_ChargeLoopReqType->BPT_Scheduled_AC_CLReqControlMode_isUsed = 0u;
    AC_ChargeLoopReqType->CLReqControlMode_isUsed = 0u;
    AC_ChargeLoopReqType->Dynamic_AC_CLReqControlMode_isUsed = 0u;
    AC_ChargeLoopReqType->Scheduled_AC_CLReqControlMode_isUsed = 0u;
}

void init_iso20_ac_AC_ChargeLoopResType(struct iso20_ac_AC_ChargeLoopResType* AC_ChargeLoopResType) {
    AC_ChargeLoopResType->EVSEStatus_isUsed = 0u;
    AC_ChargeLoopResType->MeterInfo_isUsed = 0u;
    AC_ChargeLoopResType->Receipt_isUsed = 0u;
    AC_ChargeLoopResType->EVSETargetFrequency_isUsed = 0u;
    AC_ChargeLoopResType->BPT_Dynamic_AC_CLResControlMode_isUsed = 0u;
    AC_ChargeLoopResType->BPT_Scheduled_AC_CLResControlMode_isUsed = 0u;
    AC_ChargeLoopResType->CLResControlMode_isUsed = 0u;
    AC_ChargeLoopResType->Dynamic_AC_CLResControlMode_isUsed = 0u;
    AC_ChargeLoopResType->Scheduled_AC_CLResControlMode_isUsed = 0u;
}

void init_iso20_ac_BPT_AC_CPDReqEnergyTransferModeType(struct iso20_ac_BPT_AC_CPDReqEnergyTransferModeType* BPT_AC_CPDReqEnergyTransferModeType) {
    BPT_AC_CPDReqEnergyTransferModeType->EVMaximumChargePower_L2_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMaximumChargePower_L3_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMinimumChargePower_L2_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMinimumChargePower_L3_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMaximumDischargePower_L2_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMaximumDischargePower_L3_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMinimumDischargePower_L2_isUsed = 0u;
    BPT_AC_CPDReqEnergyTransferModeType->EVMinimumDischargePower_L3_isUsed = 0u;
}

void init_iso20_ac_BPT_AC_CPDResEnergyTransferModeType(struct iso20_ac_BPT_AC_CPDResEnergyTransferModeType* BPT_AC_CPDResEnergyTransferModeType) {
    BPT_AC_CPDResEnergyTransferModeType->EVSEMaximumChargePower_L2_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMaximumChargePower_L3_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMinimumChargePower_L2_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMinimumChargePower_L3_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->MaximumPowerAsymmetry_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEPowerRampLimitation_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEPresentActivePower_L3_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMaximumDischargePower_L2_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMaximumDischargePower_L3_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMinimumDischargePower_L2_isUsed = 0u;
    BPT_AC_CPDResEnergyTransferModeType->EVSEMinimumDischargePower_L3_isUsed = 0u;
}

void init_iso20_ac_BPT_Scheduled_AC_CLReqControlModeType(struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType* BPT_Scheduled_AC_CLReqControlModeType) {
    BPT_Scheduled_AC_CLReqControlModeType->EVTargetEnergyRequest_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumEnergyRequest_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumEnergyRequest_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumChargePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumChargePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVPresentActivePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVPresentActivePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVPresentReactivePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumDischargePower_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumDischargePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMaximumDischargePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumDischargePower_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumDischargePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLReqControlModeType->EVMinimumDischargePower_L3_isUsed = 0u;
}

void init_iso20_ac_BPT_Scheduled_AC_CLResControlModeType(struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType* BPT_Scheduled_AC_CLResControlModeType) {
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetActivePower_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetActivePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetActivePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSETargetReactivePower_L3_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    BPT_Scheduled_AC_CLResControlModeType->EVSEPresentActivePower_L3_isUsed = 0u;
}

void init_iso20_ac_BPT_Dynamic_AC_CLReqControlModeType(struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType* BPT_Dynamic_AC_CLReqControlModeType) {
    BPT_Dynamic_AC_CLReqControlModeType->DepartureTime_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMaximumChargePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMaximumChargePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMinimumChargePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMinimumChargePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVPresentActivePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVPresentActivePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVPresentReactivePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVPresentReactivePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMaximumDischargePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMaximumDischargePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMinimumDischargePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMinimumDischargePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMaximumV2XEnergyRequest_isUsed = 0u;
    BPT_Dynamic_AC_CLReqControlModeType->EVMinimumV2XEnergyRequest_isUsed = 0u;
}

void init_iso20_ac_BPT_Dynamic_AC_CLResControlModeType(struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType* BPT_Dynamic_AC_CLResControlModeType) {
    BPT_Dynamic_AC_CLResControlModeType->DepartureTime_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->MinimumSOC_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->TargetSOC_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->AckMaxDelay_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSETargetActivePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSETargetActivePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSETargetReactivePower_L3_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_L2_isUsed = 0u;
    BPT_Dynamic_AC_CLResControlModeType->EVSEPresentActivePower_L3_isUsed = 0u;
}

void init_iso20_ac_ManifestType(struct iso20_ac_ManifestType* ManifestType) {
    ManifestType->Reference.arrayLen = 0u;
    ManifestType->Id_isUsed = 0u;
}

void init_iso20_ac_SignaturePropertiesType(struct iso20_ac_SignaturePropertiesType* SignaturePropertiesType) {
    SignaturePropertiesType->Id_isUsed = 0u;
}

// init for fragment
void init_iso20_ac_exiFragment(struct iso20_ac_exiFragment* exiFrag) {
    exiFrag->AC_ChargeParameterDiscoveryRes_isUsed = 0u;
    exiFrag->SignedInfo_isUsed = 0u;
}

// init for xmldsig fragment
void init_iso20_ac_xmldsigFragment(struct iso20_ac_xmldsigFragment* xmldsigFrag) {
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


