// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_TRANSPORT_HPP
#define POWERMETER_TRANSPORT_HPP

/**
 * Baseclass for transport classes.
 *
 * Transports are:
 * - direct connection via modbus
 * - connection via SerialComHub
 */

#include <generated/interfaces/serial_communication_hub/Interface.hpp>

namespace transport {

using DataVector = std::vector<std::uint8_t>;

class AbstractModbusTransport {

public:
    virtual transport::DataVector fetch(int address, int register_count) = 0;
};

/**
 * data transport via SerialComHub
 */

class SerialCommHubTransport : public AbstractModbusTransport {

protected:
    serial_communication_hubIntf& m_serial_hub;
    int m_device_id;
    int m_base_address;

public:
    SerialCommHubTransport(serial_communication_hubIntf& serial_hub, int device_id, int base_address) :
        m_serial_hub(serial_hub), m_device_id(device_id), m_base_address(base_address) {
    }

    virtual transport::DataVector fetch(int address, int register_count) override;
};

} // namespace transport

#endif // POWERMETER_TRANSPORT_HPP
