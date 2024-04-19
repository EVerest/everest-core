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
  * @file appHand_Encoder.h
  * @brief Description goes here
  *
  **/

#ifndef APP_HANDSHAKE_ENCODER_H
#define APP_HANDSHAKE_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "common/exi_basetypes.h"
#include "common/exi_bitstream.h"
#include "appHand_Datatypes.h"


// main function for encoding
int encode_appHand_exiDocument(exi_bitstream_t* stream, struct appHand_exiDocument* exiDoc);

#ifdef __cplusplus
}
#endif

#endif /* APP_HANDSHAKE_ENCODER_H */

