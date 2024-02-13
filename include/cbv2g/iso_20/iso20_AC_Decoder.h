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
  * @file iso20_AC_Decoder.h
  * @brief Description goes here
  *
  **/

#ifndef ISO20_AC_DECODER_H
#define ISO20_AC_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "cbv2g/common/exi_bitstream.h"
#include "iso20_AC_Datatypes.h"


// main function for decoding
int decode_iso20_ac_exiDocument(exi_bitstream_t* stream, struct iso20_ac_exiDocument* exiDoc);
// decoding function for fragment
int decode_iso20_ac_exiFragment(exi_bitstream_t* stream, struct iso20_ac_exiFragment* exiFrag);
// decoding function for xmldsig fragment
int decode_iso20_ac_xmldsigFragment(exi_bitstream_t* stream, struct iso20_ac_xmldsigFragment* xmldsigFrag);

#ifdef __cplusplus
}
#endif

#endif /* ISO20_AC_DECODER_H */

