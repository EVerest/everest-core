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
  * @file iso20_WPT_Encoder.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_WPT_ENCODER_H
#define ISO20_WPT_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "cbv2g/common/exi_bitstream.h"
#include "iso20_WPT_Datatypes.h"


// main function for encoding
int encode_iso20_wpt_exiDocument(exi_bitstream_t* stream, struct iso20_wpt_exiDocument* exiDoc);

#ifdef __cplusplus
}
#endif

#endif /* ISO20_WPT_ENCODER_H */

