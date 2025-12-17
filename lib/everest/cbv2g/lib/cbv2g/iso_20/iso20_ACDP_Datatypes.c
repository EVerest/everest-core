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
  * @file iso20_ACDP_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "cbv2g/iso_20/iso20_ACDP_Datatypes.h"



// root elements of EXI doc
void init_iso20_acdp_exiDocument(struct iso20_acdp_exiDocument* exiDoc) {
    exiDoc->ACDP_VehiclePositioningReq_isUsed = 0u;
    exiDoc->ACDP_VehiclePositioningRes_isUsed = 0u;
    exiDoc->ACDP_ConnectReq_isUsed = 0u;
    exiDoc->ACDP_ConnectRes_isUsed = 0u;
    exiDoc->ACDP_DisconnectReq_isUsed = 0u;
    exiDoc->ACDP_DisconnectRes_isUsed = 0u;
    exiDoc->ACDP_SystemStatusReq_isUsed = 0u;
    exiDoc->ACDP_SystemStatusRes_isUsed = 0u;
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
void init_iso20_acdp_TransformType(struct iso20_acdp_TransformType* TransformType) {
    TransformType->ANY_isUsed = 0u;
    TransformType->XPath_isUsed = 0u;
}

void init_iso20_acdp_TransformsType(struct iso20_acdp_TransformsType* TransformsType) {
    (void) TransformsType;
}

void init_iso20_acdp_DSAKeyValueType(struct iso20_acdp_DSAKeyValueType* DSAKeyValueType) {
    DSAKeyValueType->P_isUsed = 0u;
    DSAKeyValueType->Q_isUsed = 0u;
    DSAKeyValueType->G_isUsed = 0u;
    DSAKeyValueType->J_isUsed = 0u;
    DSAKeyValueType->Seed_isUsed = 0u;
    DSAKeyValueType->PgenCounter_isUsed = 0u;
}

void init_iso20_acdp_X509IssuerSerialType(struct iso20_acdp_X509IssuerSerialType* X509IssuerSerialType) {
    (void) X509IssuerSerialType;
}

void init_iso20_acdp_DigestMethodType(struct iso20_acdp_DigestMethodType* DigestMethodType) {
    DigestMethodType->ANY_isUsed = 0u;
}

void init_iso20_acdp_RSAKeyValueType(struct iso20_acdp_RSAKeyValueType* RSAKeyValueType) {
    (void) RSAKeyValueType;
}

void init_iso20_acdp_CanonicalizationMethodType(struct iso20_acdp_CanonicalizationMethodType* CanonicalizationMethodType) {
    CanonicalizationMethodType->ANY_isUsed = 0u;
}

void init_iso20_acdp_SignatureMethodType(struct iso20_acdp_SignatureMethodType* SignatureMethodType) {
    SignatureMethodType->HMACOutputLength_isUsed = 0u;
    SignatureMethodType->ANY_isUsed = 0u;
}

void init_iso20_acdp_KeyValueType(struct iso20_acdp_KeyValueType* KeyValueType) {
    KeyValueType->DSAKeyValue_isUsed = 0u;
    KeyValueType->RSAKeyValue_isUsed = 0u;
    KeyValueType->ANY_isUsed = 0u;
}

void init_iso20_acdp_ReferenceType(struct iso20_acdp_ReferenceType* ReferenceType) {
    ReferenceType->Id_isUsed = 0u;
    ReferenceType->Type_isUsed = 0u;
    ReferenceType->URI_isUsed = 0u;
    ReferenceType->Transforms_isUsed = 0u;
}

void init_iso20_acdp_RetrievalMethodType(struct iso20_acdp_RetrievalMethodType* RetrievalMethodType) {
    RetrievalMethodType->Type_isUsed = 0u;
    RetrievalMethodType->URI_isUsed = 0u;
    RetrievalMethodType->Transforms_isUsed = 0u;
}

void init_iso20_acdp_X509DataType(struct iso20_acdp_X509DataType* X509DataType) {
    X509DataType->X509IssuerSerial_isUsed = 0u;
    X509DataType->X509SKI_isUsed = 0u;
    X509DataType->X509SubjectName_isUsed = 0u;
    X509DataType->X509Certificate_isUsed = 0u;
    X509DataType->X509CRL_isUsed = 0u;
    X509DataType->ANY_isUsed = 0u;
}

void init_iso20_acdp_PGPDataType(struct iso20_acdp_PGPDataType* PGPDataType) {
    PGPDataType->choice_1_isUsed = 0u;
    PGPDataType->choice_2_isUsed = 0u;
}

void init_iso20_acdp_SPKIDataType(struct iso20_acdp_SPKIDataType* SPKIDataType) {
    SPKIDataType->ANY_isUsed = 0u;
}

void init_iso20_acdp_SignedInfoType(struct iso20_acdp_SignedInfoType* SignedInfoType) {
    SignedInfoType->Reference.arrayLen = 0u;
    SignedInfoType->Id_isUsed = 0u;
}

void init_iso20_acdp_SignatureValueType(struct iso20_acdp_SignatureValueType* SignatureValueType) {
    SignatureValueType->Id_isUsed = 0u;
}

void init_iso20_acdp_KeyInfoType(struct iso20_acdp_KeyInfoType* KeyInfoType) {
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

void init_iso20_acdp_ObjectType(struct iso20_acdp_ObjectType* ObjectType) {
    ObjectType->Encoding_isUsed = 0u;
    ObjectType->Id_isUsed = 0u;
    ObjectType->MimeType_isUsed = 0u;
    ObjectType->ANY_isUsed = 0u;
}

void init_iso20_acdp_SignatureType(struct iso20_acdp_SignatureType* SignatureType) {
    SignatureType->Id_isUsed = 0u;
    SignatureType->KeyInfo_isUsed = 0u;
    SignatureType->Object_isUsed = 0u;
}

void init_iso20_acdp_RationalNumberType(struct iso20_acdp_RationalNumberType* RationalNumberType) {
    (void) RationalNumberType;
}

void init_iso20_acdp_MessageHeaderType(struct iso20_acdp_MessageHeaderType* MessageHeaderType) {
    MessageHeaderType->Signature_isUsed = 0u;
}

void init_iso20_acdp_SignaturePropertyType(struct iso20_acdp_SignaturePropertyType* SignaturePropertyType) {
    SignaturePropertyType->Id_isUsed = 0u;
    SignaturePropertyType->ANY_isUsed = 0u;
}

void init_iso20_acdp_EVTechnicalStatusType(struct iso20_acdp_EVTechnicalStatusType* EVTechnicalStatusType) {
    EVTechnicalStatusType->EVImmobilized_isUsed = 0u;
    EVTechnicalStatusType->EVWLANStrength_isUsed = 0u;
    EVTechnicalStatusType->EVCPStatus_isUsed = 0u;
    EVTechnicalStatusType->EVSOC_isUsed = 0u;
    EVTechnicalStatusType->EVErrorCode_isUsed = 0u;
    EVTechnicalStatusType->EVTimeout_isUsed = 0u;
}

void init_iso20_acdp_ACDP_VehiclePositioningReqType(struct iso20_acdp_ACDP_VehiclePositioningReqType* ACDP_VehiclePositioningReqType) {
    (void) ACDP_VehiclePositioningReqType;
}

void init_iso20_acdp_ACDP_VehiclePositioningResType(struct iso20_acdp_ACDP_VehiclePositioningResType* ACDP_VehiclePositioningResType) {
    (void) ACDP_VehiclePositioningResType;
}

void init_iso20_acdp_ACDP_ConnectReqType(struct iso20_acdp_ACDP_ConnectReqType* ACDP_ConnectReqType) {
    (void) ACDP_ConnectReqType;
}

void init_iso20_acdp_ACDP_ConnectResType(struct iso20_acdp_ACDP_ConnectResType* ACDP_ConnectResType) {
    (void) ACDP_ConnectResType;
}

void init_iso20_acdp_ACDP_SystemStatusReqType(struct iso20_acdp_ACDP_SystemStatusReqType* ACDP_SystemStatusReqType) {
    (void) ACDP_SystemStatusReqType;
}

void init_iso20_acdp_ACDP_SystemStatusResType(struct iso20_acdp_ACDP_SystemStatusResType* ACDP_SystemStatusResType) {
    (void) ACDP_SystemStatusResType;
}

void init_iso20_acdp_CLReqControlModeType(struct iso20_acdp_CLReqControlModeType* CLReqControlModeType) {
    (void) CLReqControlModeType;
}

void init_iso20_acdp_CLResControlModeType(struct iso20_acdp_CLResControlModeType* CLResControlModeType) {
    (void) CLResControlModeType;
}

void init_iso20_acdp_ManifestType(struct iso20_acdp_ManifestType* ManifestType) {
    ManifestType->Reference.arrayLen = 0u;
    ManifestType->Id_isUsed = 0u;
}

void init_iso20_acdp_SignaturePropertiesType(struct iso20_acdp_SignaturePropertiesType* SignaturePropertiesType) {
    SignaturePropertiesType->Id_isUsed = 0u;
}


