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
  * @file exi_basetypes_encoder.c
  * @brief Description goes here
  *
  **/

#include "common/exi_basetypes.h"
#include "common/exi_basetypes_encoder.h"
#include "common/exi_bitstream.h"
#include "common/exi_error_codes.h"


/*****************************************************************************
 * local functions
 *****************************************************************************/
static int exi_basetypes_encoder_write_unsigned(exi_bitstream_t* stream, exi_unsigned_t* exi_unsigned)
{
    uint8_t* current_octet = exi_unsigned->octets;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++)
    {
        int error;
        error = exi_bitstream_write_octet(stream, *current_octet);
        if (error != EXI_ERROR__NO_ERROR)
        {
            return error;
        }

        current_octet++;
    }

    return EXI_ERROR__NO_ERROR;
}


/*****************************************************************************
 * interface functions - bool
 *****************************************************************************/
int exi_basetypes_encoder_bool(exi_bitstream_t* stream, int value)
{
    int error;
    uint32_t bit = (value) ? 1 : 0;

    error = exi_bitstream_write_bits(stream, 1, bit);

    return error;
}

/*****************************************************************************
 * interface functions - bytes, binary
 *****************************************************************************/
int exi_basetypes_encoder_bytes(exi_bitstream_t* stream, size_t bytes_len, const uint8_t* bytes, size_t bytes_size)
{
    if (bytes_len > bytes_size)
    {
        return EXI_ERROR__BYTE_BUFFER_TOO_SMALL;
    }

    uint8_t* current_byte = (uint8_t*)bytes;

    for (size_t n = 0; n < bytes_len; n++)
    {
        int error;
        error = exi_bitstream_write_octet(stream, *current_byte);
        if (error != EXI_ERROR__NO_ERROR)
        {
            return error;
        }

        current_byte++;
    }

    return EXI_ERROR__NO_ERROR;
}

/*****************************************************************************
 * interface functions - unsigned integer
 *****************************************************************************/
int exi_basetypes_encoder_nbit_uint(exi_bitstream_t* stream, size_t bit_count, uint32_t value)
{
    return exi_bitstream_write_bits(stream, bit_count, value);
}

int exi_basetypes_encoder_uint_8(exi_bitstream_t* stream, uint8_t value)
{
    int error;
    exi_unsigned_t exi_unsigned;
    uint32_t result = (uint32_t)value;

    error = exi_basetypes_convert_to_unsigned(&exi_unsigned, result, EXI_BASETYPES_UINT8_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return exi_basetypes_encoder_write_unsigned(stream, &exi_unsigned);
}

int exi_basetypes_encoder_uint_16(exi_bitstream_t* stream, uint16_t value)
{
    int error;
    exi_unsigned_t exi_unsigned;
    uint32_t result = (uint32_t)value;

    error = exi_basetypes_convert_to_unsigned(&exi_unsigned, result, EXI_BASETYPES_UINT16_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return exi_basetypes_encoder_write_unsigned(stream, &exi_unsigned);
}

int exi_basetypes_encoder_uint_32(exi_bitstream_t* stream, uint32_t value)
{
    int error;
    exi_unsigned_t exi_unsigned;

    error = exi_basetypes_convert_to_unsigned(&exi_unsigned, value, EXI_BASETYPES_UINT32_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return exi_basetypes_encoder_write_unsigned(stream, &exi_unsigned);
}

int exi_basetypes_encoder_uint_64(exi_bitstream_t* stream, uint64_t value)
{
    int error;
    exi_unsigned_t exi_unsigned;

    error = exi_basetypes_convert_64_to_unsigned(&exi_unsigned, value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return exi_basetypes_encoder_write_unsigned(stream, &exi_unsigned);
}

/*****************************************************************************
 * interface functions - integer
 *****************************************************************************/
int exi_basetypes_encoder_integer_8(exi_bitstream_t* stream, int8_t value)
{
    int error;
    int sign = (value < 0) ? 1 : 0;

    error = exi_basetypes_encoder_bool(stream, sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    uint8_t result = (uint8_t)value;

    if (sign)
    {
        result = -value - 1;
    }

    return exi_basetypes_encoder_uint_8(stream, result);
}

int exi_basetypes_encoder_integer_16(exi_bitstream_t* stream, int16_t value)
{
    int error;
    int sign = (value < 0) ? 1 : 0;

    error = exi_basetypes_encoder_bool(stream, sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    uint16_t result = (uint16_t)value;

    if (sign)
    {
        result = -value - 1;
    }

    return exi_basetypes_encoder_uint_16(stream, result);
}

int exi_basetypes_encoder_integer_32(exi_bitstream_t* stream, int32_t value)
{
    int error;
    int sign = (value < 0) ? 1 : 0;

    error = exi_basetypes_encoder_bool(stream, sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    uint32_t result = (uint32_t)value;

    if (sign)
    {
        result = -value - 1;
    }

    return exi_basetypes_encoder_uint_32(stream, result);
}

int exi_basetypes_encoder_integer_64(exi_bitstream_t* stream, int64_t value)
{
    int error;
    int sign = (value < 0) ? 1 : 0;

    error = exi_basetypes_encoder_bool(stream, sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    uint64_t result = (uint64_t)value;

    if (sign)
    {
        result = -value - 1;
    }

    return exi_basetypes_encoder_uint_64(stream, result);
}

/*****************************************************************************
 * interface functions - characters, string
 *****************************************************************************/
int exi_basetypes_encoder_characters(exi_bitstream_t* stream, size_t characters_len, const exi_character_t* characters, size_t characters_size)
{
    const uint8_t ASCII_MAX_VALUE = 127;

    if (characters_len > characters_size)
    {
        return EXI_ERROR__CHARACTER_BUFFER_TOO_SMALL;
    }

    uint8_t* current_char = (uint8_t*)characters;

    for (size_t n = 0; n < characters_len; n++)
    {
        int error;
        if (*current_char > ASCII_MAX_VALUE)
        {
            return EXI_ERROR__UNSUPPORTED_CHARACTER_VALUE;
        }

        error = exi_bitstream_write_octet(stream, *current_char);
        if (error != EXI_ERROR__NO_ERROR)
        {
            return error;
        }

        current_char++;
    }

    return EXI_ERROR__NO_ERROR;
}

