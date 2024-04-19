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
  * @file exi_bitstream.c
  * @brief Description goes here
  *
  **/

#include "common/exi_bitstream.h"
#include "common/exi_error_codes.h"


/*****************************************************************************
 * local functions
 *****************************************************************************/
static int exi_bitstream_has_overflow(exi_bitstream_t* stream)
{
    if (stream->bit_count == EXI_BITSTREAM_MAX_BIT_COUNT)
    {
        if (stream->byte_pos < stream->data_size)
        {
            stream->byte_pos++;
            stream->bit_count = 0;
        }
        else
        {
            return EXI_ERROR__BITSTREAM_OVERFLOW;
        }
    }

    return EXI_ERROR__NO_ERROR;
}

static int exi_bitstream_write_bit(exi_bitstream_t* stream, uint8_t bit)
{
    // check whether the bit to be written is within the stream capacity
    if (exi_bitstream_has_overflow(stream))
    {
        return EXI_ERROR__BITSTREAM_OVERFLOW;
    }

    // point to current byte
    uint8_t* current_byte = stream->data + stream->byte_pos;

    if (stream->bit_count == 0)
    {
        // clear everything if at the beginning of a new byte
        *current_byte = 0;
    }

    if (bit)
    {
        *current_byte = *current_byte | (1u << (EXI_BITSTREAM_MAX_BIT_COUNT - (stream->bit_count + 1u)));
    }

    stream->bit_count++;

    return EXI_ERROR__NO_ERROR;
}

static int exi_bitstream_read_bit(exi_bitstream_t* stream, uint8_t* bit)
{
    // check whether the bit to be read is within the stream capacity
    if (exi_bitstream_has_overflow(stream))
    {
        return EXI_ERROR__BITSTREAM_OVERFLOW;
    }

    uint8_t current_bit = *(stream->data + stream->byte_pos) >> (EXI_BITSTREAM_MAX_BIT_COUNT - (stream->bit_count + 1u));
    *bit = (current_bit & 1u) ? 1 : 0;

    stream->bit_count++;

    return EXI_ERROR__NO_ERROR;
}

/*****************************************************************************
 * interface functions
 *****************************************************************************/
void exi_bitstream_init(exi_bitstream_t* stream, uint8_t* data, size_t data_size, size_t data_offset, exi_status_callback status_callback)
{
    stream->byte_pos = data_offset;
    stream->bit_count = 0;

    stream->data = data;
    stream->data_size = data_size;

    stream->_init_called = 1;
    stream->_flag_byte_pos = data_offset;

    stream->status_callback = status_callback;
}

void exi_bitstream_reset(exi_bitstream_t* stream)
{
    if (stream->_init_called)
    {
        stream->byte_pos = stream->_flag_byte_pos;
    }
    else
    {
        stream->byte_pos = 0;
    }

    stream->bit_count = 0;
}

size_t exi_bitstream_get_length(const exi_bitstream_t* stream)
{
    size_t length = stream->byte_pos;

    if (stream->_init_called && (stream->_flag_byte_pos > 0))
    {
        length -= stream->_flag_byte_pos;
    }

    length += stream->bit_count > 0u ? 1u : 0u;

    return length;
}

int exi_bitstream_write_bits(exi_bitstream_t* stream, size_t bit_count, uint32_t value)
{
    if (bit_count > 32)
    {
        return EXI_ERROR__BIT_COUNT_LARGER_THAN_TYPE_SIZE;
    }

    int error = EXI_ERROR__NO_ERROR;

    for (size_t n = 0; n < bit_count; n++)
    {
        uint8_t bit;
        bit = (value & (1u << (bit_count - n - 1))) > 0;

        error = exi_bitstream_write_bit(stream, bit);
        if (error != EXI_ERROR__NO_ERROR)
        {
            break;
        }
    }

    return error;
}

int exi_bitstream_write_octet(exi_bitstream_t* stream, uint8_t value)
{
    return exi_bitstream_write_bits(stream, 8, (uint32_t)value);
}

int exi_bitstream_read_bits(exi_bitstream_t* stream, size_t bit_count, uint32_t* value)
{
    *value = 0;

    if (bit_count > 32)
    {
        return EXI_ERROR__BIT_COUNT_LARGER_THAN_TYPE_SIZE;
    }

    int error = EXI_ERROR__NO_ERROR;

    for (size_t n = 0; n < bit_count; n++)
    {
        uint8_t bit;
        error = exi_bitstream_read_bit(stream, &bit);
        if (error != EXI_ERROR__NO_ERROR)
        {
            break;
        }

        *value = (*value << 1u) | bit;
    }
    return error;
}

int exi_bitstream_read_octet(exi_bitstream_t* stream, uint8_t* value)
{
    *value = 0;

    int error = EXI_ERROR__NO_ERROR;

    for (int n = 0; n < 8; n++)
    {
        uint8_t bit;
        error = exi_bitstream_read_bit(stream, &bit);
        if (error != EXI_ERROR__NO_ERROR)
        {
            break;
        }

        *value = (*value << 1u) | bit;
    }
    return error;
}

