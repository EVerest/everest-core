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
  * @file exi_v2gtp.c
  * @brief Description goes here
  *
  **/

#include "exi_v2gtp.h"


/* V2G Transport Protocol version */
#define V2GTP_VERSION     0x01u
#define V2GTP_VERSION_NOT 0xFEu


void V2GTP_WriteHeader(uint8_t* stream_data, uint32_t stream_payload_length)
{
    V2GTP20_WriteHeader(stream_data, stream_payload_length, V2GTP20_SAP_PAYLOAD_ID);
}

void V2GTP20_WriteHeader(uint8_t* stream_data, uint32_t stream_payload_length, uint16_t v2gtp20_payload_id)
{
    /* write version and inverse version */
    stream_data[0] = V2GTP_VERSION;
    stream_data[1] = V2GTP_VERSION_NOT;

    /* write payload type */
    stream_data[2] = (uint8_t)(v2gtp20_payload_id >> 8 & 0xFFu);
    stream_data[3] = (uint8_t)(v2gtp20_payload_id & 0xFFu);

    /* write payload length */
    stream_data[4] = (uint8_t)(stream_payload_length >> 24 & 0xFFu);
    stream_data[5] = (uint8_t)(stream_payload_length >> 16 & 0xFFu);
    stream_data[6] = (uint8_t)(stream_payload_length >>  8 & 0xFFu);
    stream_data[7] = (uint8_t)(stream_payload_length & 0xFFu);
}

int V2GTP_ReadHeader(const uint8_t* stream_data, uint32_t* stream_payload_length)
{
    return V2GTP20_ReadHeader(stream_data, stream_payload_length, V2GTP20_SAP_PAYLOAD_ID);
}

int V2GTP20_ReadHeader(const uint8_t* stream_data, uint32_t* stream_payload_length, uint16_t v2gtp20_payload_id)
{
    uint16_t payload_id;

    /* check version and inversion */
    if ((stream_data[0] != V2GTP_VERSION) || (stream_data[1] != V2GTP_VERSION_NOT))
    {
        return V2GTP_ERROR__VERSION_DOES_NOT_MATCH;
    }

    /* check payload id */
    payload_id = (stream_data[2] << 8) | stream_data[3];
    if (payload_id != v2gtp20_payload_id)
    {
        return V2GTP_ERROR__PAYLOAD_ID_DOES_NOT_MATCH;
    }

    /* determine payload length */
    *stream_payload_length = (stream_data[4] << 24) | (stream_data[5] << 16) | (stream_data[6] << 8) | stream_data[7];

    return V2GTP_ERROR__NO_ERROR;
}

