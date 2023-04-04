// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CRC16_HPP
#define CRC16_HPP
#include <stdint.h>

uint16_t MODBUS_CRC16(const uint8_t* buf, int len);
#endif