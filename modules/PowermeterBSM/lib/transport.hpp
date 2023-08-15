// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_TRANSPORT_HPP
#define POWERMETER_TRANSPORT_HPP

#include <modbus/modbus_client.hpp>

#include "known_model.hpp"
#include "protocol_related_types.hpp"

#include <memory>

/**
 * Baseclass for transport classes.
 *
 * Transports are:
 * - direct connection via modbus
 * - connection via SerialComHub
 */

#include <generated/interfaces/serial_communication_hub/Interface.hpp>

namespace transport {

class AbstractDataTransport {

public:
    using Spt = std::shared_ptr<AbstractDataTransport>;

    /**
     * Starting at SunspecDataModelAddress fetch Types::SunspecRegisterCount register contents.
     */
    virtual transport::DataVector fetch(protocol_related_types::SunspecDataModelAddress,
                                        protocol_related_types::SunspecRegisterCount) = 0;

    /**
     * fetch data from a KnownModel (example: Sunspec Common )
     */
    virtual transport::DataVector fetch(const known_model::AddressData& ad) = 0;

    /**
     * device specific: Trigger generation of a custom BSM signed snapshot.
     */
    virtual bool trigger_snapshot_generation_BSM() = 0;

    /**
     * device specific: Trigger generation of OCMF signed snapshot BSM power meter.
     */
    virtual bool trigger_snapshot_generation_BSM_OCMF() = 0;
};

/**
 * data transport via modbus.
 */
class ModbusTransport : public AbstractDataTransport {

protected:
    everest::connection::SerialDeviceConfiguration m_cfg;
    everest::connection::SerialDevice m_device;
    // everest::connection::SerialDeviceLogToStream      m_device;
    everest::connection::RTUConnection m_connection;
    everest::modbus::ModbusRTUClient m_modbus_client;
    protocol_related_types::ModbusUnitId m_unit_id;
    protocol_related_types::ModbusRegisterAddress m_base_address;

    bool trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress snapshot_trigger_register);

public:
    ModbusTransport(std::string serial_device_name, protocol_related_types::ModbusUnitId unit_id,
                    protocol_related_types::ModbusRegisterAddress base_address);

    everest::connection::RTUConnection& rtu_connection() {
        return m_connection;
    }

    everest::modbus::ModbusRTUClient& modbus_client() {
        return m_modbus_client;
    }

    everest::connection::SerialDevice serial_device() {
        return m_device;
    }

    virtual transport::DataVector fetch(protocol_related_types::SunspecDataModelAddress,
                                        protocol_related_types::SunspecRegisterCount) override;

    virtual transport::DataVector fetch(const known_model::AddressData& ad) override;

    virtual bool trigger_snapshot_generation_BSM() override;

    virtual bool trigger_snapshot_generation_BSM_OCMF() override;
};

/**
 * data transport via SerialComHub
 */

class SerialCommHubTransport : public AbstractDataTransport {

protected:
    serial_communication_hubIntf& m_serial_hub;
    protocol_related_types::ModbusUnitId m_unit_id;
    protocol_related_types::ModbusRegisterAddress m_base_address;

    bool trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress);

public:
    SerialCommHubTransport(serial_communication_hubIntf& serial_hub, protocol_related_types::ModbusUnitId unit_id,
                           protocol_related_types::ModbusRegisterAddress base_address) :
        m_serial_hub(serial_hub), m_unit_id(unit_id), m_base_address(base_address) {
    }

    virtual transport::DataVector fetch(protocol_related_types::SunspecDataModelAddress,
                                        protocol_related_types::SunspecRegisterCount) override;

    virtual transport::DataVector fetch(const known_model::AddressData& ad) override;

    virtual bool trigger_snapshot_generation_BSM() override;

    virtual bool trigger_snapshot_generation_BSM_OCMF() override;
};

} // namespace transport

#endif // POWERMETER_TRANSPORT_HPP
