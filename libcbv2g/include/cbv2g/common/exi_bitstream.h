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
  * @file exi_bitstream.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_BITSTREAM_H
#define EXI_BITSTREAM_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stddef.h>
#include <stdint.h>




#define EXI_BITSTREAM_MAX_BIT_COUNT 8


typedef void (*exi_status_callback)(int message_id, int status_code, int value_1, int value_2);

typedef struct exi_bitstream {
    /* byte array size and data */
    uint8_t* data;
    size_t data_size;

    /* byte array current byte and bit position in array */
    uint8_t bit_count;
    size_t byte_pos;

    /* flags for reset and length function */
    uint8_t _init_called;
    size_t _flag_byte_pos;

    /* Pointer to callback for reporting errors or logging if assigned */
    exi_status_callback status_callback;
} exi_bitstream_t;


/**
 * \brief       bitstream init
 *
 *              Initializes the exi bitstream with the given parameters.
 *
 * \param       stream              input or output stream
 * \param       data                pointer to EXI data
 * \param       data_size           size of EXI data
 * \param       data_offset         start of payload inside EXI data
 * \param       status_callback     pointer to callback function for error reporting or logging
 *
 */
void exi_bitstream_init(exi_bitstream_t* stream, uint8_t* data, size_t data_size, size_t data_offset, exi_status_callback status_callback);

/**
 * \brief       bitstream reset
 *
 *              Resets the exi bitstream to the parameters from the last init state.
 *
 * \param       stream      input or output stream
 *
 */
void exi_bitstream_reset(exi_bitstream_t* stream);

/**
 * \brief       bitstream get length
 *
 *              Returns the length of the stream.
 *
 * \param       stream      output Stream
 * \return                  length of stream
 *
 */
size_t exi_bitstream_get_length(const exi_bitstream_t* stream);

/**
 * \brief       bitstream write bits
 *
 *              Write the bit_count bits of value to the stream.
 *
 * \param       stream          output Stream
 * \param       bit_count       number of bits to write
 * \param       value           value to write
 * \return                      NO_ERROR or error code
 *
 */
int exi_bitstream_write_bits(exi_bitstream_t* stream, size_t bit_count, uint32_t value);

/**
 * \brief       bitstream write octet
 *
 *              write an octet to the stream.
 *
 * \param       stream          output Stream
 * \param       value           write octet value
 * \return                      NO_ERROR or error code
 *
 */
int exi_bitstream_write_octet(exi_bitstream_t* stream, uint8_t value);

/**
 * \brief       bitstream read bits
 *
 *              read the bit_count bits from the stream and return the result.
 *
 * \param       stream          input Stream
 * \param       bit_count       number of bits to read
 * \param       value           read value
 * \return                      NO_ERROR or error code
 *
 */
int exi_bitstream_read_bits(exi_bitstream_t* stream, size_t bit_count, uint32_t* value);

/**
 * \brief       bitstream read octet
 *
 *              read an octet from the stream and return the result.
 *
 * \param       stream          input Stream
 * \param       value           read octet value
 * \return                      NO_ERROR or error code
 *
 */
int exi_bitstream_read_octet(exi_bitstream_t* stream, uint8_t* value);


#ifdef __cplusplus
}
#endif

#endif /* EXI_BITSTREAM_H */

