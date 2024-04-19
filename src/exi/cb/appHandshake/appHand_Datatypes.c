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
  * @file appHand_Datatypes.c
  * @brief Description goes here
  *
  **/
#include "appHand_Datatypes.h"



// root elements of EXI doc
void init_appHand_exiDocument(struct appHand_exiDocument* exiDoc) {
    exiDoc->supportedAppProtocolReq_isUsed = 0u;
    exiDoc->supportedAppProtocolRes_isUsed = 0u;
}
void init_appHand_AppProtocolType(struct appHand_AppProtocolType* AppProtocolType) {
    (void) AppProtocolType;
}

void init_appHand_supportedAppProtocolReq(struct appHand_supportedAppProtocolReq* supportedAppProtocolReq) {
    supportedAppProtocolReq->AppProtocol.arrayLen = 0u;
}

void init_appHand_supportedAppProtocolRes(struct appHand_supportedAppProtocolRes* supportedAppProtocolRes) {
    supportedAppProtocolRes->SchemaID_isUsed = 0u;
}


