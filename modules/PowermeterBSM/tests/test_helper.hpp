// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_DRIVER_BASE_HPP
#define POWERMETER_BSM_DRIVER_BASE_HPP

#include <modbus/modbus_client.hpp>

#include "lib/protocol_related_types.hpp"

/**
 * sunpec related helper tools.
 */
namespace test_helper {

// returns true if the tested register base is a sunspec base register
bool check_sunspec_device_base_regiser(everest::modbus::ModbusRTUClient& modbus_client,
                                       protocol_related_types::ModbusUnitId unit_id,
                                       protocol_related_types::ModbusRegisterAddress register_to_test);

protocol_related_types::ModbusRegisterAddress
get_sunspec_device_base_register(everest::connection::SerialDevice& serialdevice,
                                 protocol_related_types::ModbusUnitId unit_id);

// throws std::runtime_error if no baseregister could be detected.
protocol_related_types::ModbusRegisterAddress get_sunspec_device_base_register(everest::modbus::ModbusRTUClient&,
                                                                               protocol_related_types::ModbusUnitId);

} // namespace test_helper

#endif // POWERMETER_BSM_DRIVER_BASE_HPP
