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
  * @file exi_v2gtp.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_V2GTP_H
#define EXI_V2GTP_H

#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>


/*====================================================================*
 * error defines;
 *--------------------------------------------------------------------*/
#define V2GTP_ERROR__NO_ERROR 0
#define V2GTP_ERROR__VERSION_DOES_NOT_MATCH -1
#define V2GTP_ERROR__PAYLOAD_ID_DOES_NOT_MATCH -2

/*====================================================================*
 * global defines;
 *--------------------------------------------------------------------*/
#define V2GTP_HEADER_LENGTH 8u

#define V2GTP20_SAP_PAYLOAD_ID             0x8001u
#define V2GTP20_MAINSTREAM_PAYLOAD_ID      0x8002u
#define V2GTP20_AC_MAINSTREAM_PAYLOAD_ID   0x8003u
#define V2GTP20_DC_MAINSTREAM_PAYLOAD_ID   0x8004u
#define V2GTP20_ACDP_MAINSTREAM_PAYLOAD_ID 0x8005u
#define V2GTP20_WPT_MAINSTREAM_PAYLOAD_ID  0x8006u

#define V2GTP20_SCHEDULE_RENEGOTIATION_PAYLOAD_ID 0x8101u
#define V2GTP20_METERING_CONFIRMATION_PAYLOAD_ID  0x8102u
#define V2GTP20_ACDP_SYSTEM_STATUS_PAYLOAD_ID     0x8103u
#define V2GTP20_PARKING_STATUS_PAYLOAD_ID         0x8104u

#define V2GTP20_SDP_REQUEST_PAYLOAD_ID           0x9000u
#define V2GTP20_SDP_RESPONSE_PAYLOAD_ID          0x9001u
#define V2GTP20_SDP_REQUEST_WIRELESS_PAYLOAD_ID  0x9002u
#define V2GTP20_SDP_RESPONSE_WIRELESS_PAYLOAD_ID 0x9003u

/*====================================================================*
 * interface
 *--------------------------------------------------------------------*/
/**
 * @brief Writes the V2GTP Header and the given payload length into the data stream
 *        2nd function is the -20 version with different payload ids.
 *
 * @retval void
 *
 */
void V2GTP_WriteHeader(uint8_t* stream_data, uint32_t stream_payload_length);
void V2GTP20_WriteHeader(uint8_t* stream_data, uint32_t stream_payload_length, uint16_t v2gtp20_payload_id);

/**
 * @brief Verifies the V2GTP Header and returns the payload length from the data stream
 *        2nd function is the -20 version with different payload ids.
 *
 * @retval  on success V2GTP_ERROR__NO_ERROR otherwise an error code
 *
 */
int V2GTP_ReadHeader(const uint8_t* stream_data, uint32_t* stream_payload_length);
int V2GTP20_ReadHeader(const uint8_t* stream_data, uint32_t* stream_payload_length, uint16_t v2gtp20_payload_id);


#ifdef __cplusplus
}
#endif

#endif /* EXI_V2GTP_H */

