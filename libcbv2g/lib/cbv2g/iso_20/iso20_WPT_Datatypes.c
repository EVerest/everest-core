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
  * @file iso20_WPT_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "cbv2g/iso_20/iso20_WPT_Datatypes.h"



// root elements of EXI doc
void init_iso20_wpt_exiDocument(struct iso20_wpt_exiDocument* exiDoc) {
    exiDoc->WPT_FinePositioningSetupReq_isUsed = 0u;
    exiDoc->WPT_FinePositioningSetupRes_isUsed = 0u;
    exiDoc->WPT_FinePositioningReq_isUsed = 0u;
    exiDoc->WPT_FinePositioningRes_isUsed = 0u;
    exiDoc->WPT_PairingReq_isUsed = 0u;
    exiDoc->WPT_PairingRes_isUsed = 0u;
    exiDoc->WPT_ChargeParameterDiscoveryReq_isUsed = 0u;
    exiDoc->WPT_ChargeParameterDiscoveryRes_isUsed = 0u;
    exiDoc->WPT_AlignmentCheckReq_isUsed = 0u;
    exiDoc->WPT_AlignmentCheckRes_isUsed = 0u;
    exiDoc->WPT_ChargeLoopReq_isUsed = 0u;
    exiDoc->WPT_ChargeLoopRes_isUsed = 0u;
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
void init_iso20_wpt_TransformType(struct iso20_wpt_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso20_wpt_WPT_LF_RxRSSIType(struct iso20_wpt_WPT_LF_RxRSSIType* WPT_LF_RxRSSIType) {
    (void) WPT_LF_RxRSSIType;
}

void init_iso20_wpt_TransformsType(struct iso20_wpt_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso20_wpt_DSAKeyValueType(struct iso20_wpt_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso20_wpt_X509IssuerSerialType(struct iso20_wpt_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso20_wpt_DigestMethodType(struct iso20_wpt_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso20_wpt_RSAKeyValueType(struct iso20_wpt_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso20_wpt_WPT_LF_RxRSSIListType(struct iso20_wpt_WPT_LF_RxRSSIListType* WPT_LF_RxRSSIListType) {
    (void) WPT_LF_RxRSSIListType;
}

void init_iso20_wpt_CanonicalizationMethodType(struct iso20_wpt_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso20_wpt_WPT_TxRxPulseOrderType(struct iso20_wpt_WPT_TxRxPulseOrderType* WPT_TxRxPulseOrderType) {
    (void) WPT_TxRxPulseOrderType;
}

void init_iso20_wpt_WPT_LF_TxDataType(struct iso20_wpt_WPT_LF_TxDataType* WPT_LF_TxDataType) {
    (void) WPT_LF_TxDataType;
}

void init_iso20_wpt_WPT_LF_RxDataType(struct iso20_wpt_WPT_LF_RxDataType* WPT_LF_RxDataType) {
    (void) WPT_LF_RxDataType;
}

void init_iso20_wpt_SignatureMethodType(struct iso20_wpt_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso20_wpt_KeyValueType(struct iso20_wpt_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso20_wpt_WPT_CoordinateXYZType(struct iso20_wpt_WPT_CoordinateXYZType* WPT_CoordinateXYZType) {
    (void) WPT_CoordinateXYZType;
}

void init_iso20_wpt_ReferenceType(struct iso20_wpt_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso20_wpt_RetrievalMethodType(struct iso20_wpt_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso20_wpt_X509DataType(struct iso20_wpt_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso20_wpt_PGPDataType(struct iso20_wpt_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso20_wpt_SPKIDataType(struct iso20_wpt_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso20_wpt_SignedInfoType(struct iso20_wpt_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso20_wpt_SignatureValueType(struct iso20_wpt_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso20_wpt_RationalNumberType(struct iso20_wpt_RationalNumberType* RationalNumberType) {
    (void) RationalNumberType;
}

void init_iso20_wpt_WPT_LF_TxDataListType(struct iso20_wpt_WPT_LF_TxDataListType* WPT_LF_TxDataListType) {
    (void) WPT_LF_TxDataListType;
}

void init_iso20_wpt_KeyInfoType(struct iso20_wpt_KeyInfoType* KeyInfoType) {
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

void init_iso20_wpt_WPT_TxRxSpecDataType(struct iso20_wpt_WPT_TxRxSpecDataType* WPT_TxRxSpecDataType) {
    (void) WPT_TxRxSpecDataType;
}

void init_iso20_wpt_WPT_LF_RxDataListType(struct iso20_wpt_WPT_LF_RxDataListType* WPT_LF_RxDataListType) {
    (void) WPT_LF_RxDataListType;
}

void init_iso20_wpt_ObjectType(struct iso20_wpt_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso20_wpt_WPT_TxRxPackageSpecDataType(struct iso20_wpt_WPT_TxRxPackageSpecDataType* WPT_TxRxPackageSpecDataType) {
    WPT_TxRxPackageSpecDataType->PulseSequenceOrder.arrayLen = 0u;
}

void init_iso20_wpt_WPT_LF_TransmitterDataType(struct iso20_wpt_WPT_LF_TransmitterDataType* WPT_LF_TransmitterDataType) {
    WPT_LF_TransmitterDataType->TxSpecData.arrayLen = 0u;
    WPT_LF_TransmitterDataType->TxPackageSpecData_isUsed = 0u;
}

void init_iso20_wpt_AlternativeSECCType(struct iso20_wpt_AlternativeSECCType* AlternativeSECCType) {
    AlternativeSECCType->SSID_isUsed = 0u;
    AlternativeSECCType->BSSID_isUsed = 0u;
    AlternativeSECCType->IPAddress_isUsed = 0u;
    AlternativeSECCType->Port_isUsed = 0u;
}

void init_iso20_wpt_WPT_LF_ReceiverDataType(struct iso20_wpt_WPT_LF_ReceiverDataType* WPT_LF_ReceiverDataType) {
    WPT_LF_ReceiverDataType->RxSpecData.arrayLen = 0u;
}

void init_iso20_wpt_WPT_LF_DataPackageType(struct iso20_wpt_WPT_LF_DataPackageType* WPT_LF_DataPackageType) {
    WPT_LF_DataPackageType->LF_TxData_isUsed = 0u;
    WPT_LF_DataPackageType->LF_RxData_isUsed = 0u;
}

void init_iso20_wpt_DetailedCostType(struct iso20_wpt_DetailedCostType* DetailedCostType) {
    (void) DetailedCostType;
}

void init_iso20_wpt_SignatureType(struct iso20_wpt_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso20_wpt_DetailedTaxType(struct iso20_wpt_DetailedTaxType* DetailedTaxType) {
    (void) DetailedTaxType;
}

void init_iso20_wpt_MessageHeaderType(struct iso20_wpt_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso20_wpt_SignaturePropertyType(struct iso20_wpt_SignaturePropertyType* SignaturePropertyType) {
    SignaturePropertyType->Id_isUsed = 0u;
    SignaturePropertyType->ANY_isUsed = 0u;
}

void init_iso20_wpt_DisplayParametersType(struct iso20_wpt_DisplayParametersType* DisplayParametersType) {
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

void init_iso20_wpt_WPT_FinePositioningMethodListType(struct iso20_wpt_WPT_FinePositioningMethodListType* WPT_FinePositioningMethodListType) {
    WPT_FinePositioningMethodListType->WPT_FinePositioningMethod.arrayLen = 0u;
}

void init_iso20_wpt_EVSEStatusType(struct iso20_wpt_EVSEStatusType* EVSEStatusType) {
    (void) EVSEStatusType;
}

void init_iso20_wpt_WPT_PairingMethodListType(struct iso20_wpt_WPT_PairingMethodListType* WPT_PairingMethodListType) {
    WPT_PairingMethodListType->WPT_PairingMethod.arrayLen = 0u;
}

void init_iso20_wpt_MeterInfoType(struct iso20_wpt_MeterInfoType* MeterInfoType) {
    MeterInfoType->BPT_DischargedEnergyReadingWh_isUsed = 0u;
    MeterInfoType->CapacitiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->BPT_InductiveEnergyReadingVARh_isUsed = 0u;
    MeterInfoType->MeterSignature_isUsed = 0u;
    MeterInfoType->MeterStatus_isUsed = 0u;
    MeterInfoType->MeterTimestamp_isUsed = 0u;
}

void init_iso20_wpt_WPT_AlignmentCheckMethodListType(struct iso20_wpt_WPT_AlignmentCheckMethodListType* WPT_AlignmentCheckMethodListType) {
    WPT_AlignmentCheckMethodListType->WPT_AlignmentCheckMethod.arrayLen = 0u;
}

void init_iso20_wpt_WPT_LF_DataPackageListType(struct iso20_wpt_WPT_LF_DataPackageListType* WPT_LF_DataPackageListType) {
    (void) WPT_LF_DataPackageListType;
}

void init_iso20_wpt_AlternativeSECCListType(struct iso20_wpt_AlternativeSECCListType* AlternativeSECCListType) {
    AlternativeSECCListType->AlternativeSECC.arrayLen = 0u;
}

void init_iso20_wpt_ReceiptType(struct iso20_wpt_ReceiptType* ReceiptType) {
    ReceiptType->TaxCosts.arrayLen = 0u;
    ReceiptType->EnergyCosts_isUsed = 0u;
    ReceiptType->OccupancyCosts_isUsed = 0u;
    ReceiptType->AdditionalServicesCosts_isUsed = 0u;
    ReceiptType->OverstayCosts_isUsed = 0u;
}

void init_iso20_wpt_WPT_LF_SystemSetupDataType(struct iso20_wpt_WPT_LF_SystemSetupDataType* WPT_LF_SystemSetupDataType) {
    WPT_LF_SystemSetupDataType->LF_TransmitterSetupData_isUsed = 0u;
    WPT_LF_SystemSetupDataType->LF_ReceiverSetupData_isUsed = 0u;
}

void init_iso20_wpt_WPT_EVPCPowerControlParameterType(struct iso20_wpt_WPT_EVPCPowerControlParameterType* WPT_EVPCPowerControlParameterType) {
    (void) WPT_EVPCPowerControlParameterType;
}

void init_iso20_wpt_WPT_SPCPowerControlParameterType(struct iso20_wpt_WPT_SPCPowerControlParameterType* WPT_SPCPowerControlParameterType) {
    (void) WPT_SPCPowerControlParameterType;
}

void init_iso20_wpt_WPT_FinePositioningSetupReqType(struct iso20_wpt_WPT_FinePositioningSetupReqType* WPT_FinePositioningSetupReqType) {
    WPT_FinePositioningSetupReqType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_FinePositioningSetupReqType->LF_SystemSetupData_isUsed = 0u;
}

void init_iso20_wpt_WPT_FinePositioningSetupResType(struct iso20_wpt_WPT_FinePositioningSetupResType* WPT_FinePositioningSetupResType) {
    WPT_FinePositioningSetupResType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_FinePositioningSetupResType->LF_SystemSetupData_isUsed = 0u;
}

void init_iso20_wpt_WPT_FinePositioningReqType(struct iso20_wpt_WPT_FinePositioningReqType* WPT_FinePositioningReqType) {
    WPT_FinePositioningReqType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_FinePositioningReqType->WPT_LF_DataPackageList_isUsed = 0u;
}

void init_iso20_wpt_WPT_FinePositioningResType(struct iso20_wpt_WPT_FinePositioningResType* WPT_FinePositioningResType) {
    WPT_FinePositioningResType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_FinePositioningResType->WPT_LF_DataPackageList_isUsed = 0u;
}

void init_iso20_wpt_WPT_PairingReqType(struct iso20_wpt_WPT_PairingReqType* WPT_PairingReqType) {
    WPT_PairingReqType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_PairingReqType->ObservedIDCode_isUsed = 0u;
}

void init_iso20_wpt_WPT_PairingResType(struct iso20_wpt_WPT_PairingResType* WPT_PairingResType) {
    WPT_PairingResType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_PairingResType->ObservedIDCode_isUsed = 0u;
    WPT_PairingResType->AlternativeSECCList_isUsed = 0u;
}

void init_iso20_wpt_WPT_ChargeParameterDiscoveryReqType(struct iso20_wpt_WPT_ChargeParameterDiscoveryReqType* WPT_ChargeParameterDiscoveryReqType) {
    WPT_ChargeParameterDiscoveryReqType->VendorSpecificDataContainer.arrayLen = 0u;
}

void init_iso20_wpt_WPT_ChargeParameterDiscoveryResType(struct iso20_wpt_WPT_ChargeParameterDiscoveryResType* WPT_ChargeParameterDiscoveryResType) {
    WPT_ChargeParameterDiscoveryResType->SDManufacturerSpecificDataContainer.arrayLen = 0u;
}

void init_iso20_wpt_WPT_AlignmentCheckReqType(struct iso20_wpt_WPT_AlignmentCheckReqType* WPT_AlignmentCheckReqType) {
    WPT_AlignmentCheckReqType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_AlignmentCheckReqType->TargetCoilCurrent_isUsed = 0u;
}

void init_iso20_wpt_WPT_AlignmentCheckResType(struct iso20_wpt_WPT_AlignmentCheckResType* WPT_AlignmentCheckResType) {
    WPT_AlignmentCheckResType->VendorSpecificDataContainer.arrayLen = 0u;
    WPT_AlignmentCheckResType->PowerTransmitted_isUsed = 0u;
    WPT_AlignmentCheckResType->SupplyDeviceCurrent_isUsed = 0u;
}

void init_iso20_wpt_WPT_ChargeLoopReqType(struct iso20_wpt_WPT_ChargeLoopReqType* WPT_ChargeLoopReqType) {
    WPT_ChargeLoopReqType->ManufacturerSpecificDataContainer.arrayLen = 0u;
    WPT_ChargeLoopReqType->DisplayParameters_isUsed = 0u;
    WPT_ChargeLoopReqType->EVPCOperatingFrequency_isUsed = 0u;
    WPT_ChargeLoopReqType->EVPCPowerControlParameter_isUsed = 0u;
}

void init_iso20_wpt_WPT_ChargeLoopResType(struct iso20_wpt_WPT_ChargeLoopResType* WPT_ChargeLoopResType) {
    WPT_ChargeLoopResType->ManufacturerSpecificDataContainer.arrayLen = 0u;
    WPT_ChargeLoopResType->EVSEStatus_isUsed = 0u;
    WPT_ChargeLoopResType->MeterInfo_isUsed = 0u;
    WPT_ChargeLoopResType->Receipt_isUsed = 0u;
    WPT_ChargeLoopResType->SDPowerInput_isUsed = 0u;
    WPT_ChargeLoopResType->SPCOperatingFrequency_isUsed = 0u;
    WPT_ChargeLoopResType->SPCPowerControlParameter_isUsed = 0u;
}

void init_iso20_wpt_CLReqControlModeType(struct iso20_wpt_CLReqControlModeType* CLReqControlModeType) {
    (void) CLReqControlModeType;
}

void init_iso20_wpt_CLResControlModeType(struct iso20_wpt_CLResControlModeType* CLResControlModeType) {
    (void) CLResControlModeType;
}

void init_iso20_wpt_ManifestType(struct iso20_wpt_ManifestType* ManifestType) {
    ManifestType->Reference.arrayLen = 0u;
    ManifestType->Id_isUsed = 0u;
}

void init_iso20_wpt_SignaturePropertiesType(struct iso20_wpt_SignaturePropertiesType* SignaturePropertiesType) {
    SignaturePropertiesType->Id_isUsed = 0u;
}


