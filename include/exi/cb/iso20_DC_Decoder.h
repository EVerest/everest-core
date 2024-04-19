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
  * @file iso20_DC_Decoder.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_DC_DECODER_H
#define ISO20_DC_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "common/exi_bitstream.h"
#include "iso20_DC_Datatypes.h"


// main function for decoding
int decode_iso20_dc_exiDocument(exi_bitstream_t* stream, struct iso20_dc_exiDocument* exiDoc);
// decoding function for fragment
int decode_iso20_dc_exiFragment(exi_bitstream_t* stream, struct iso20_dc_exiFragment* exiFrag);
// decoding function for xmldsig fragment
int decode_iso20_dc_xmldsigFragment(exi_bitstream_t* stream, struct iso20_dc_xmldsigFragment* xmldsigFrag);

#ifdef __cplusplus
}
#endif

#endif /* ISO20_DC_DECODER_H */

