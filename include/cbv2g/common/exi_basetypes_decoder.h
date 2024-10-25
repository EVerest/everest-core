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
  * @file exi_basetypes_decoder.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_BASETYPES_DECODER_H
#define EXI_BASETYPES_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>
#include "exi_basetypes.h"
#include "exi_bitstream.h"


/**
 * \brief       decoder for type bool
 *
 *              reads a boolean value from the bitstream and decodes it into value.
 *
 * \param       stream          EXI bitstream
 * \param       value           decoded value
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_bool(exi_bitstream_t* stream, int* value);

/**
 * \brief       decoder for type byte array
 *
 *              reads a bytes from the bitstream and writes it to the array.
 *
 * \param       stream          EXI bitstream
 * \param       bytes_len       length of the used bytes inside the array
 * \param       bytes           pointer to the first byte of the array
 * \param       bytes_size      size of the byte array
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_bytes(exi_bitstream_t* stream, size_t bytes_len, uint8_t* bytes, size_t bytes_size);

/**
 * \brief       decoder for n-bit unsigned integer
 *
 *              reads n-bits from the bitstream end decodes it into an unsigned integer value.
 *
 * \param       stream          EXI bitstream
 * \param       bit_count       number of bits to read from the stream
 * \param       value           Value into which the bit_count bits are decoded
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_nbit_uint(exi_bitstream_t* stream, size_t bit_count, uint32_t* value);

/**
 * \brief       decoder for type unsigned integer
 *
 *              this description applies to the specified functions, as only the value type is different.
 *              reads an unsigned integer value from the bitstream and decodes it into value.
 *              this decoder is for unsigned integer values with no restriction or having a value > 4096.
 *
 * \param       stream          EXI bitstream
 * \param       value           decoded value
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_uint_8(exi_bitstream_t* stream, uint8_t* value);
int exi_basetypes_decoder_uint_16(exi_bitstream_t* stream, uint16_t* value);
int exi_basetypes_decoder_uint_32(exi_bitstream_t* stream, uint32_t* value);
int exi_basetypes_decoder_uint_64(exi_bitstream_t* stream, uint64_t* value);
int exi_basetypes_decoder_unsigned(exi_bitstream_t* stream, exi_unsigned_t* value);

/**
 * \brief       decoder for type integer
 *
 *              this description applies to the specified functions, as only the value type is different.
 *              reads an integer value from the bitstream and decodes it into value.
 *              this decoder is for integer values with no restriction or having a value > 4096.
 *
 * \param       stream          EXI bitstream
 * \param       value           decoded value
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_integer_8(exi_bitstream_t* stream, int8_t* value);
int exi_basetypes_decoder_integer_16(exi_bitstream_t* stream, int16_t* value);
int exi_basetypes_decoder_integer_32(exi_bitstream_t* stream, int32_t* value);
int exi_basetypes_decoder_integer_64(exi_bitstream_t* stream, int64_t* value);
int exi_basetypes_decoder_signed(exi_bitstream_t* stream, exi_signed_t* value);

/**
 * \brief       decoder for type character array
 *
 *              reads a characters from the bitstream and writes it to the array.
 *
 * \param       stream          EXI bitstream
 * \param       bytes_len       length of the used characters inside the array
 * \param       bytes           pointer to the first character of the array
 * \param       bytes_size      size of the character array
 * \return                      NO_ERROR or error code
 *
 */
int exi_basetypes_decoder_characters(exi_bitstream_t* stream, size_t characters_len, exi_character_t* characters, size_t characters_size);


#ifdef __cplusplus
}
#endif

#endif /* EXI_BASETYPES_DECODER_H */

