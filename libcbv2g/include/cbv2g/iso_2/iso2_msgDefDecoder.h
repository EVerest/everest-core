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
  * @file iso2_msgDefDecoder.h
  * @brief Description goes here
  *
  **/

#ifndef ISO2_MSG_DEF_DECODER_H
#define ISO2_MSG_DEF_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "cbv2g/common/exi_bitstream.h"
#include "iso2_msgDefDatatypes.h"


// main function for decoding
int decode_iso2_exiDocument(exi_bitstream_t* stream, struct iso2_exiDocument* exiDoc);
// decoding function for fragment
int decode_iso2_exiFragment(exi_bitstream_t* stream, struct iso2_exiFragment* exiFrag);
// decoding function for xmldsig fragment
int decode_iso2_xmldsigFragment(exi_bitstream_t* stream, struct iso2_xmldsigFragment* xmldsigFrag);

#ifdef __cplusplus
}
#endif

#endif /* ISO2_MSG_DEF_DECODER_H */

