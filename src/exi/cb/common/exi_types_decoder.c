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
  * @file exi_types_decoder.c
  * @brief Description goes here
  *
  **/

#include "common/exi_basetypes.h"
#include "common/exi_basetypes_decoder.h"
#include "common/exi_bitstream.h"
#include "common/exi_error_codes.h"
#include "common/exi_types_decoder.h"


// *********
// HexBinary
// *********
int decode_exi_type_hex_binary(exi_bitstream_t* stream, uint16_t* value_len, uint8_t* value_buffer, size_t value_buffer_size)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_uint_16(stream, value_len);
            if (error == 0)
            {
                error = exi_basetypes_decoder_bytes(stream, *value_len, value_buffer, value_buffer_size);
            }
        }
        else
        {
            // Second level event is not supported
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    // if nothing went wrong, the error of last decoding is evaluated here
    if (error == 0)
    {
        // test EE for simple element
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if (eventCode != 0)
            {
                // deviants are not supported or also typecast and nillable
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

// *********
// integers
// *********
int decode_exi_type_integer8(exi_bitstream_t* stream, int8_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_integer_8(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if(eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_integer16(exi_bitstream_t* stream, int16_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_integer_16(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if(eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_integer32(exi_bitstream_t* stream, int32_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_integer_32(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if(eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_integer64(exi_bitstream_t* stream, int64_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_integer_64(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if(eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}


int decode_exi_type_uint8(exi_bitstream_t* stream, uint8_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_uint_8(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if (eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_uint16(exi_bitstream_t* stream, uint16_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_uint_16(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if (eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_uint32(exi_bitstream_t* stream, uint32_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_uint_32(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if (eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

int decode_exi_type_uint64(exi_bitstream_t* stream, uint64_t* value)
{
    uint32_t eventCode;
    int error;

    error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
    if (error == 0)
    {
        if (eventCode == 0)
        {
            error = exi_basetypes_decoder_uint_64(stream, value);
        }
        else
        {
            /* Second level event is not supported */
            error = EXI_ERROR__UNSUPPORTED_SUB_EVENT;
        }
    }

    /* if nothing went wrong, the error of last decoding is evaluated here */
    if (error == 0)
    {
        /* test EE for simple element */
        error = exi_basetypes_decoder_nbit_uint(stream, 1, &eventCode);
        if (error == 0)
        {
            if (eventCode != 0)
            {
                /* deviants are not supported or also typecast and nillable */
                error = EXI_ERROR__DEVIANTS_NOT_SUPPORTED;
            }
        }
    }

    return error;
}

