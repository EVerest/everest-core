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
  * @file exi_types_decoder.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_TYPES_DECODER_H
#define EXI_TYPES_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif



#include "exi_basetypes.h"
#include "exi_bitstream.h"


/**
 * \brief       Decode hexBinary
 *
 * \param       stream              EXI bitstream
 * \param       value_len           uint16_t (out) used length of decoded value
 * \param       value_buffer        byte buffer (out) decoded value
 * \param       value_buffer_size   size of the buffer
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_hex_binary(exi_bitstream_t* stream, uint16_t* value_len, uint8_t* value_buffer, size_t value_buffer_size);

/**
 * \brief       Decode 8-bit integer
 *
 * \param       stream              EXI bitstream
 * \param       value               int8_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_integer8(exi_bitstream_t* stream, int8_t* value);

/**
 * \brief       Decode 16-bit integer
 *
 * \param       stream              EXI bitstream
 * \param       value               int16_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_integer16(exi_bitstream_t* stream, int16_t* value);

/**
 * \brief       Decode 32-bit integer
 *
 * \param       stream              EXI bitstream
 * \param       value               int32_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_integer32(exi_bitstream_t* stream, int32_t* value);

/**
 * \brief       Decode 64-bit integer
 *
 * \param       stream              EXI bitstream
 * \param       value               int64_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_integer64(exi_bitstream_t* stream, int64_t* value);


/**
 * \brief       Decode 8-bit unsigned integer
 *
 * \param       stream              EXI bitstream
 * \param       value               uint8_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_uint8(exi_bitstream_t* stream, uint8_t* value);

/**
 * \brief       Decode 16-bit unsigned integer
 *
 * \param       stream              EXI bitstream
 * \param       value               uint16_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_uint16(exi_bitstream_t* stream, uint16_t* value);

/**
 * \brief       Decode 32-bit unsigned integer
 *
 * \param       stream              EXI bitstream
 * \param       value               uint32_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_uint32(exi_bitstream_t* stream, uint32_t* value);

/**
 * \brief       Decode 64-bit unsigned integer
 *
 * \param       stream              EXI bitstream
 * \param       value               uint64_t (out) decoded value
 * \return                          Error-Code <> 0, if no error 0
 *
 */
int decode_exi_type_uint64(exi_bitstream_t* stream, uint64_t* value);


#ifdef __cplusplus
}
#endif

#endif /* EXI_TYPES_DECODER_H */

