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

#include "cbv2g/common/exi_error_codes.h"
#include "cbv2g/common/exi_basetypes.h"
#include "cbv2g/common/exi_bitstream.h"


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

        dummy >>= 7u;
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

        dummy >>= 7u;
        if (dummy == 0)
        {
            break;
        }

        *current_octet |= EXI_BASETYPES_OCTET_SEQ_FLAG_MASK;
        current_octet++;
    }

    return (exi_unsigned->octets_count <= EXI_BASETYPES_UINT64_MAX_OCTETS) ? EXI_ERROR__NO_ERROR : EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
}

int exi_basetypes_convert_to_signed(exi_signed_t* exi_signed, int32_t value, size_t max_octets)
{
    if (value < 0) {
        exi_signed->is_negative = 1;
        return exi_basetypes_convert_to_unsigned(&exi_signed->data, -value, max_octets);
    }
    exi_signed->is_negative = 0;
    return exi_basetypes_convert_to_unsigned(&exi_signed->data, value, max_octets);
}

int exi_basetypes_convert_64_to_signed(exi_signed_t* exi_signed, int64_t value)
{
    if (value < 0) {
        exi_signed->is_negative = 1;
        return exi_basetypes_convert_64_to_unsigned(&exi_signed->data, -value);
    }
    exi_signed->is_negative = 0;
    return exi_basetypes_convert_64_to_unsigned(&exi_signed->data, value);
}

int exi_basetypes_convert_from_unsigned(const exi_unsigned_t* exi_unsigned, uint32_t* value, size_t max_octets)
{
    if (exi_unsigned->octets_count > max_octets)
    {
        return EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
    }

    const uint8_t* current_octet = exi_unsigned->octets;
    *value = 0;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++)
    {
        *value = *value + ((uint32_t)(*current_octet & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK) << (n * 7));

        current_octet++;
    }

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_convert_64_from_unsigned(const exi_unsigned_t* exi_unsigned, uint64_t* value)
{
    if (exi_unsigned->octets_count > EXI_BASETYPES_UINT64_MAX_OCTETS)
    {
        return EXI_ERROR__OCTET_COUNT_LARGER_THAN_TYPE_SUPPORTS;
    }

    const uint8_t* current_octet = exi_unsigned->octets;
    *value = 0;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++)
    {
        *value = *value + ((uint64_t)(*current_octet & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK) << (n * 7));

        current_octet++;
    }

    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_convert_from_signed(const exi_signed_t* exi_signed, int32_t* value, size_t max_octets)
{
    uint32_t u_value = 0;
    int res = exi_basetypes_convert_from_unsigned(&exi_signed->data, &u_value, max_octets);
    *value = (exi_signed->is_negative == 0) ? u_value : -u_value;
    return res;
}

int exi_basetypes_convert_64_from_signed(const exi_signed_t* exi_signed, int64_t* value)
{
    uint64_t u_value = 0;
    int res = exi_basetypes_convert_64_from_unsigned(&exi_signed->data, &u_value);
    *value = (exi_signed->is_negative == 0) ? u_value : -u_value;
    return res;
}

static void _reverse_array(uint8_t* data, size_t data_size)
{
    if (!data_size)
    {
        return;
    }

    size_t i = 0;
    size_t j = data_size - 1;
    while (i < j) {
        const uint8_t temp = data[i];
        data[i] = data[j];
        data[j] = temp;
        i++;
        j--;
    }
}

int exi_basetypes_convert_bytes_from_unsigned(const exi_unsigned_t* exi_unsigned, uint8_t* data, size_t* data_len, size_t data_size)
{
    // raw EXI 7/8 integer byte stream with flags (exi_unsigned) to API exi_unsigned_t representation (data, data_len)
    const uint8_t* current_octet = exi_unsigned->octets;
    uint16_t temp = 0;
    *data_len = 0;
    size_t total_offset = 0;

    for (size_t n = 0; n < exi_unsigned->octets_count; n++) {
        temp += ((uint16_t)(*current_octet & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK) << total_offset);
        total_offset += 7;
        if (total_offset >= 8) {
            if (*data_len >= data_size) {
                return EXI_ERROR__ENCODED_INTEGER_SIZE_LARGER_THAN_DESTINATION;
            }
            total_offset -= 8;
            data[*data_len] = temp & 0xFF;
            (*data_len)++;
            temp >>= 8;
        }
        current_octet++;
    }
    if (total_offset != 0 && (temp & 0xFF) != 0) {
        if (*data_len >= data_size) {
            return EXI_ERROR__ENCODED_INTEGER_SIZE_LARGER_THAN_DESTINATION;
        }
        data[*data_len] = temp & 0xFF;
        (*data_len)++;
    }
    _reverse_array(data, *data_len);
    return EXI_ERROR__NO_ERROR;
}

int exi_basetypes_convert_bytes_to_unsigned(exi_unsigned_t* exi_unsigned, const uint8_t* data, size_t data_len)
{
    // API exi_unsigned_t representation (data, data_len) to raw EXI 7/8 integer byte stream with flags (exi_unsigned)
    uint8_t *current_octet = &exi_unsigned->octets[0];
    uint16_t dummy = 0;
    uint8_t dummy_count = 0;
    exi_unsigned->octets_count = 0;

    // first, determine the number of relevant bits (or octets), to have a termination criterion
    size_t bytenum;
    for (bytenum = 0; bytenum < data_len; bytenum++) {
        if (data[bytenum] != 0)
            break;
    }
    if (bytenum == data_len) {
        // special case: all zeros
        *current_octet = 0;
        exi_unsigned->octets_count = 1;
        return EXI_ERROR__NO_ERROR;
    }
    // bytenum is now index of big-endian first relevant byte
    // number of total input relevant bits t is (data_len - bytenum - 1) * 8 plus number of relevant bits in data[bytenum]
    uint8_t byte = data[bytenum];
    int bits_in_byte = 0;
    while (byte) {
        bits_in_byte++;
        byte >>= 1;
    }

    const int total_relevant_input_bits = (data_len - bytenum - 1) * 8 + bits_in_byte;
    const size_t exi_expected_octets_count = (total_relevant_input_bits + 6) / 7; // integer division ceil

    size_t incount = 0;
    for (size_t outcount = 0; outcount < exi_expected_octets_count; outcount++) {
        if (dummy_count < 7) {
            // fill dummy when more flushable bits are needed
            dummy |= (data[data_len - incount - 1] << dummy_count);
            dummy_count += 8;
            incount++;
        }
        *current_octet = (uint8_t)(dummy & EXI_BASETYPES_OCTET_SEQ_VALUE_MASK);
        exi_unsigned->octets_count++;
        if (exi_unsigned->octets_count < exi_expected_octets_count) {
            *current_octet |= EXI_BASETYPES_OCTET_SEQ_FLAG_MASK;
        } else {
            break;
        }
        current_octet++;
        dummy >>= 7u;
        dummy_count -= 7;
    }

    return EXI_ERROR__NO_ERROR;
}


