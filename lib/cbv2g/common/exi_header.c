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
  * @file exi_header.c
  * @brief Description goes here
  *
  **/

#include "cbv2g/common/exi_bitstream.h"
#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_header.h"


int exi_header_write(exi_bitstream_t* stream)
{
    return exi_bitstream_write_bits(stream, EXI_SIMPLE_HEADER_BIT_SIZE, EXI_SIMPLE_HEADER_VALUE);
}

int exi_header_read(exi_bitstream_t* stream, uint32_t* header)
{
    return exi_bitstream_read_bits(stream, EXI_SIMPLE_HEADER_BIT_SIZE, header);
}

int exi_header_read_and_check(exi_bitstream_t* stream)
{
    int error;
    uint32_t header;

    int result = EXI_ERROR__NO_ERROR;

    error = exi_header_read(stream, &header);
    if (error)
    {
        return error;
    }

    // EXI header:
    // - two Distinguishing Bits 0b10
    // - one Presence Bit for EXI Options "absence of options" 0b0
    // - EXI format version "Final version 1" 0b00000
    // results in eight header bits 0b10000000 = 0x80
    if (header != EXI_SIMPLE_HEADER_VALUE) {
        result = EXI_ERROR__HEADER_INCORRECT;
    }

    return result;
}

