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
  * @file exi_basetypes_decoder.c
  * @brief Description goes here
  *
  **/

#include "common/exi_basetypes.h"
#include "common/exi_basetypes_decoder.h"
#include "common/exi_bitstream.h"
#include "common/exi_error_codes.h"


/*****************************************************************************
 * local functions
 *****************************************************************************/
static int exi_basetypes_decoder_read_unsigned(exi_bitstream_t* stream, exi_unsigned_t* exi_unsigned)
{
    const uint8_t MSB = (1u << 7);

    int found_sequence_end = 0;
    uint8_t* current_octet = exi_unsigned->octets;
    exi_unsigned->octets_count = 0;

    while (exi_unsigned->octets_count < EXI_BASETYPES_MAX_OCTETS_SUPPORTED)
    {
        int error;
        error = exi_bitstream_read_octet(stream, current_octet);
        if (error != EXI_ERROR__NO_ERROR)
        {
            return error;
        }

        exi_unsigned->octets_count++;

        if ((*current_octet & MSB) == 0)
        {
            found_sequence_end = 1;
            break;
        }

        current_octet++;
    }

    return (found_sequence_end) ? EXI_ERROR__NO_ERROR : EXI_ERROR__SUPPORTED_MAX_OCTETS_OVERRUN;
}


/*****************************************************************************
 * interface functions - bool
 *****************************************************************************/
int exi_basetypes_decoder_bool(exi_bitstream_t* stream, int* value)
{
    int error;
    uint32_t bit;

    error = exi_bitstream_read_bits(stream, 1, &bit);
    if (error == EXI_ERROR__NO_ERROR)
    {
        *value = (bit) ? 1 : 0;
    }

    return error;
}

/*****************************************************************************
 * interface functions - bytes, binary
 *****************************************************************************/
int exi_basetypes_decoder_bytes(exi_bitstream_t* stream, size_t bytes_len, uint8_t* bytes, size_t bytes_size)
{

    if (bytes_len > bytes_size)
    {
        return EXI_ERROR__BYTE_BUFFER_TOO_SMALL;
    }

    uint8_t* current_byte = bytes;

    for (size_t n = 0; n < bytes_len; n++)
    {
        int error;
        error = exi_bitstream_read_octet(stream, current_byte);
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
int exi_basetypes_decoder_nbit_uint(exi_bitstream_t* stream, size_t bit_count, uint32_t* value)
{
    return exi_bitstream_read_bits(stream, bit_count, value);
}

int exi_basetypes_decoder_uint_8(exi_bitstream_t* stream, uint8_t* value)
{
    int error;
    exi_unsigned_t exi_unsigned;
    uint32_t result;

    error = exi_basetypes_decoder_read_unsigned(stream, &exi_unsigned);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_convert_from_unsigned(&exi_unsigned, &result, EXI_BASETYPES_UINT8_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    *value = (uint8_t)result;

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_decoder_uint_16(exi_bitstream_t* stream, uint16_t* value)
{
    int error;
    exi_unsigned_t exi_unsigned;
    uint32_t result;

    error = exi_basetypes_decoder_read_unsigned(stream, &exi_unsigned);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_convert_from_unsigned(&exi_unsigned, &result, EXI_BASETYPES_UINT16_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    *value = (uint16_t)result;

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_decoder_uint_32(exi_bitstream_t* stream, uint32_t* value)
{
    int error;
    exi_unsigned_t exi_unsigned;

    error = exi_basetypes_decoder_read_unsigned(stream, &exi_unsigned);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_convert_from_unsigned(&exi_unsigned, value, EXI_BASETYPES_UINT32_MAX_OCTETS);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_decoder_uint_64(exi_bitstream_t* stream, uint64_t* value)
{
    int error;
    exi_unsigned_t exi_unsigned;

    error = exi_basetypes_decoder_read_unsigned(stream, &exi_unsigned);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_convert_64_from_unsigned(&exi_unsigned, value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    return EXI_ERROR__NO_ERROR;
}

/*****************************************************************************
 * interface functions - integer
 *****************************************************************************/
int exi_basetypes_decoder_integer_8(exi_bitstream_t* stream, int8_t* value)
{
    int sign;
    int error;

    error = exi_basetypes_decoder_bool(stream, &sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_decoder_uint_8(stream, (uint8_t*)value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    if (sign)
    {
        *value = -(*value + 1);
    }

    return error;
}

int exi_basetypes_decoder_integer_16(exi_bitstream_t* stream, int16_t* value)
{
    int sign;
    int error;

    error = exi_basetypes_decoder_bool(stream, &sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_decoder_uint_16(stream, (uint16_t*)value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    if (sign)
    {
        *value = -(*value + 1);
    }

    return error;
}

int exi_basetypes_decoder_integer_32(exi_bitstream_t* stream, int32_t* value)
{
    int sign;
    int error;

    error = exi_basetypes_decoder_bool(stream, &sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_decoder_uint_32(stream, (uint32_t*)value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    if (sign)
    {
        *value = -(*value + 1);
    }

    return error;
}

int exi_basetypes_decoder_integer_64(exi_bitstream_t* stream, int64_t* value)
{
    int sign;
    int error;

    error = exi_basetypes_decoder_bool(stream, &sign);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    error = exi_basetypes_decoder_uint_64(stream, (uint64_t*)value);
    if (error != EXI_ERROR__NO_ERROR)
    {
        return error;
    }

    if (sign)
    {
        *value = -(*value + 1);
    }

    return error;
}

/*****************************************************************************
 * interface functions - characters, string
 *****************************************************************************/
int exi_basetypes_decoder_characters(exi_bitstream_t* stream, size_t characters_len, exi_character_t* characters, size_t characters_size)
{
    const uint8_t ASCII_MAX_VALUE = 127;

    if (characters_len + EXTRA_CHAR > characters_size)
    {
        return EXI_ERROR__CHARACTER_BUFFER_TOO_SMALL;
    }

    uint8_t* current_char = (uint8_t*)characters;

    for (size_t n = 0; n < characters_len; n++)
    {
        int error;
        error = exi_bitstream_read_octet(stream, current_char);
        if (error != EXI_ERROR__NO_ERROR)
        {
            return error;
        }

        if (*current_char > ASCII_MAX_VALUE)
        {
            return EXI_ERROR__UNSUPPORTED_CHARACTER_VALUE;
        }

        current_char++;
    }

    *current_char = ASCII_CHAR_TERMINATOR;

    return EXI_ERROR__NO_ERROR;
}

