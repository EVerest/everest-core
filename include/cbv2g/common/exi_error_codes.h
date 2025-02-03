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
  * @file exi_error_codes.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_ERROR_CODES_H
#define EXI_ERROR_CODES_H

#ifdef __cplusplus
extern "C" {
#endif



#define EXI_ERROR__NO_ERROR 0

//      stream processing -1 to -19
#define EXI_ERROR__BITSTREAM_OVERFLOW -1

//      stream header -20 to -29
#define EXI_ERROR__HEADER_COOKIE_NOT_SUPPORTED -20
#define EXI_ERROR__HEADER_OPTIONS_NOT_SUPPORTED -21
#define EXI_ERROR__HEADER_INCORRECT -22

//      stream read -30 to -39
#define EXI_ERROR__SUPPORTED_MAX_OCTETS_OVERRUN -30
#define EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS -31

//      stream write -40 to -49

//      decoder -50 to -69
#define EXI_ERROR__UNKNOWN_EVENT_FOR_DECODING -50
#define EXI_ERROR__DECODER_NOT_IMPLEMENTED -69

//      encoder -70 to -89
#define EXI_ERROR__UNKNOWN_EVENT_FOR_ENCODING -70
#define EXI_ERROR__ENCODER_NOT_IMPLEMENTED -89

//      common errors -100 to -129
#define EXI_ERROR__BIT_COUNT_LARGER_THAN_TYPE_SIZE -100
#define EXI_ERROR__BYTE_COUNT_LARGER_THAN_TYPE_SIZE -101
#define EXI_ERROR__ARRAY_OUT_OF_BOUNDS -110
#define EXI_ERROR__CHARACTER_BUFFER_TOO_SMALL -111
#define EXI_ERROR__BYTE_BUFFER_TOO_SMALL -112
#define EXI_ERROR__ENCODED_INTEGER_SIZE_LARGER_THAN_DESTINATION -113

//      grammar errors -130 to -149
#define EXI_ERROR__UNKNOWN_GRAMMAR_ID -130

//      event errors -150 to -169
#define EXI_ERROR__UNKNOWN_EVENT_CODE -150
#define EXI_ERROR__UNSUPPORTED_SUB_EVENT -151

//      document errors -170 to -199
#define EXI_ERROR__DEVIANTS_NOT_SUPPORTED -170

//      datatype errors -200 to -229
#define EXI_ERROR__STRINGVALUES_NOT_SUPPORTED -200

#define EXI_ERROR__UNSUPPORTED_INTEGER_VALUE_TYPE -210
#define EXI_ERROR__UNSUPPORTED_DATETIME_TYPE -211
#define EXI_ERROR__UNSUPPORTED_CHARACTER_VALUE -212

//      fragment errors -230 to -259
#define EXI_ERROR__INCORRECT_END_FRAGMENT_VALUE -230

//      internal errors
#define EXI_ERROR__NOT_IMPLEMENTED_YET -299


#ifdef __cplusplus
}
#endif

#endif /* EXI_ERROR_CODES_H */

