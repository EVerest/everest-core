// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include "test_helper.hpp"

namespace test_helper {

bool check_sunspec_device_base_regiser(everest::modbus::ModbusRTUClient& modbus_client,
                                       protocol_related_types::ModbusUnitId unit_id,
                                       protocol_related_types::ModbusRegisterAddress register_to_test) {
    const everest::modbus::DataVectorUint8 SunSMarker{0x53, 0x75, 0x6e, 0x53};
    try {
        return SunSMarker == modbus_client.read_holding_register(unit_id, register_to_test.val, 2);
    } catch (const std::runtime_error& e) {
        // std::cout << e.what() << std::flush;
    }
    return false;
}

protocol_related_types::ModbusRegisterAddress
get_sunspec_device_base_register(everest::connection::SerialDevice& serialdevice,
                                 protocol_related_types::ModbusUnitId unit_id) {

    everest::connection::RTUConnection connection(serialdevice);
    everest::modbus::ModbusRTUClient modbus_client(connection);

    return get_sunspec_device_base_register(modbus_client, unit_id);
}

protocol_related_types::ModbusRegisterAddress
get_sunspec_device_base_register(everest::modbus::ModbusRTUClient& modbus_client,
                                 protocol_related_types::ModbusUnitId unit_id) {
    // The beginning of the device Modbus map MUST be located at one of three Modbus addresses
    // in the Modbus holding register address space: 0, 40000 (0x9C40) or 50000 (0xC350). These
    // Modbus addresses are the full 16-bit, 0-based addresses in the Modbus protocol messages.

    protocol_related_types::ModbusRegisterAddress register_to_test[]{0_mra, 40000_mra, 50000_mra};

    for (protocol_related_types::ModbusRegisterAddress mra : register_to_test)
        if (check_sunspec_device_base_regiser(modbus_client, unit_id, mra))
            return mra;

    throw std::runtime_error("not a sunspec device!");
}

} // namespace test_helper
