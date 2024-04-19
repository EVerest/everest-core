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
  * @file exi_basetypes.c
  * @brief Description goes here
  *
  **/

#include "common/exi_error_codes.h"
#include "common/exi_basetypes.h"
#include "common/exi_bitstream.h"


/*****************************************************************************
 * interface functions
 *****************************************************************************/
int exi_basetypes_convert_to_unsigned(exi_unsigned_t* exi_unsigned, uint32_t value, size_t max_octets)
{
    exi_unsigned->octets_count = 0;
    uint8_t* current_octet = exi_unsigned->octets;
    uint32_t dummy = value;

    for (size_t n = 0; n < EXI_BASETYPES_UINT32_MAX_OCTETS; n++)
    {
        exi_unsigned->octets_count++;
        *current_octet = (uint8_t)(dummy & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK);

        dummy = dummy >> 7u;
        if (dummy == 0)
        {
            break;
        }

        *current_octet |= EXI_BASETYPES_OCTET_SEQ_FLAG_MASK;
        current_octet++;
    }

    return (exi_unsigned->octets_count <= max_octets) ? EXI_ERROR__NO_ERROR : EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
}

int exi_basetypes_convert_64_to_unsigned(exi_unsigned_t* exi_unsigned, uint64_t value)
{
    exi_unsigned->octets_count = 0;
    uint8_t* current_octet = exi_unsigned->octets;
    uint64_t dummy = value;

    for (size_t n = 0; n < EXI_BASETYPES_UINT64_MAX_OCTETS; n++)
    {
        exi_unsigned->octets_count++;
        *current_octet = (uint8_t)(dummy & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK);

        dummy = dummy >> 7u;
        if (dummy == 0)
        {
            break;
        }

        *current_octet |= EXI_BASETYPES_OCTET_SEQ_FLAG_MASK;
        current_octet++;
    }

    return (exi_unsigned->octets_count <= EXI_BASETYPES_UINT64_MAX_OCTETS) ? EXI_ERROR__NO_ERROR : EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
}

int exi_basetypes_convert_from_unsigned(exi_unsigned_t* exi_unsigned, uint32_t* value, size_t max_octets)
{
    if (exi_unsigned->octets_count > max_octets)
    {
        return EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
    }

    uint8_t* current_octet = exi_unsigned->octets;
    *value = 0;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++)
    {
        *value = *value + ((uint32_t)(*current_octet & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK) << (n * 7));

        current_octet++;
    }

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_convert_64_from_unsigned(exi_unsigned_t* exi_unsigned, uint64_t* value)
{
    if (exi_unsigned->octets_count > EXI_BASETYPES_UINT64_MAX_OCTETS)
    {
        return EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
    }

    uint8_t* current_octet = exi_unsigned->octets;
    *value = 0;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++)
    {
        *value = *value + ((uint64_t)(*current_octet & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK) << (n * 7));

        current_octet++;
    }

    return EXI_ERROR__NO_ERROR;
}

