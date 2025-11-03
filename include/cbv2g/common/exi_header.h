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
  * @file exi_header.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_HEADER_H
#define EXI_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "exi_bitstream.h"


#define EXI_SIMPLE_HEADER_BIT_SIZE 8
#define EXI_SIMPLE_HEADER_VALUE 0x80


/**
 * \brief       Writes a simple EXI header (0x80)
 *
 * \param       stream   EXI bitstream
 * \return               NO_ERROR or an error code
 *
 */
int exi_header_write(exi_bitstream_t* stream);

/**
 * \brief       Reads the simple EXI header
 *
 * \param       stream   EXI bitstream
 * \param       header   EXI simple header value from the stream
 * \return               NO_ERROR or an error code
 *
 */
int exi_header_read(exi_bitstream_t* stream, uint32_t* header);

/**
 * \brief       Reads and checks the simple EXI header
 *
 * \param       stream   EXI bitstream
 * \return               NO_ERROR or an error code
 *
 */
int exi_header_read_and_check(exi_bitstream_t* stream);


#ifdef __cplusplus
}
#endif

#endif /* EXI_HEADER_H */

