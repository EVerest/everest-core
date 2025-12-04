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
  * @file exi_basetypes_encoder.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_BASETYPES_ENCODER_H
#define EXI_BASETYPES_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>
#include "exi_basetypes.h"
#include "exi_bitstream.h"


/**
 * \brief       encoder for type bool
 *
 *              encodes a boolean value and writes it to the bitstream.
 *
 * \param       stream          EXI bitstream
 * \param       value           value to encode
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_bool(exi_bitstream_t* stream, int value);

/**
 * \brief       encoder for type byte array
 *
 *              encodes a byte array and writes it to the bitstream.
 *
 * \param       stream          EXI bitstream
 * \param       bytes_len       length of the used bytes inside the array
 * \param       bytes           pointer to the first byte of the array
 * \param       bytes_size      size of the byte array
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_bytes(exi_bitstream_t* stream, size_t bytes_len, const uint8_t* bytes, size_t bytes_size);

/**
 * \brief       encoder for n-bit unsigned integer
 *
 *              encodes a n-bit unsigned value and writes it to the bitstream.
 *
 * \param       stream          EXI bitstream
 * \param       bit_count       number of bit representing the value
 * \param       value           Value from which the bit_count least significant bits are to be encoded
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_nbit_uint(exi_bitstream_t* stream, size_t bit_count, uint32_t value);

/**
 * \brief       encoder for type unsigned integer
 *
 *              this description applies to the specified functions, as only the value type is different.
 *              encodes an unsigned integer value and writes it to the bitstream.
 *              this encoder is for uint values with no restriction or having a value < 4096.
 *
 * \param       stream          EXI bitstream
 * \param       value           value to encode
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_uint_8(exi_bitstream_t* stream, uint8_t value);
int exi_basetypes_encoder_uint_16(exi_bitstream_t* stream, uint16_t value);
int exi_basetypes_encoder_uint_32(exi_bitstream_t* stream, uint32_t value);
int exi_basetypes_encoder_uint_64(exi_bitstream_t* stream, uint64_t value);
int exi_basetypes_encoder_unsigned(exi_bitstream_t* stream, const exi_unsigned_t* value);

/**
 * \brief       encoder for type integer
 *
 *              this description applies to the specified functions, as only the value type is different.
 *              encodes an integer value and writes it to the bitstream.
 *              this encoder is for int values with no restriction or having a value < 4096.
 *
 * \param       stream          EXI bitstream
 * \param       value           value to encode
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_integer_8(exi_bitstream_t* stream, int8_t value);
int exi_basetypes_encoder_integer_16(exi_bitstream_t* stream, int16_t value);
int exi_basetypes_encoder_integer_32(exi_bitstream_t* stream, int32_t value);
int exi_basetypes_encoder_integer_64(exi_bitstream_t* stream, int64_t value);
int exi_basetypes_encoder_signed(exi_bitstream_t* stream, const exi_signed_t* value);

/**
 * \brief       encoder for type exi_character array
 *
 *              encodes an exi_character (char) array and writes it to the bitstream.
 *
 * \param       stream          EXI bitstream
 * \param       bytes_len       length of the used characters inside the array
 * \param       bytes           pointer to the first character of the array
 * \param       bytes_size      size of the character array
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_encoder_characters(exi_bitstream_t* stream, size_t characters_len, const exi_character_t* characters, size_t characters_size);


#ifdef __cplusplus
}
#endif

#endif /* EXI_BASETYPES_ENCODER_H */

