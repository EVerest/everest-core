// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include "transport.hpp"

#include <chrono>
#include <everest/logging.hpp>
#include <thread>
#include <type_traits>

namespace transport {

ModbusTransport::ModbusTransport(std::string serial_device_name, protocol_related_types::ModbusUnitId unit_id,
                                 protocol_related_types::ModbusRegisterAddress base_address) :
    m_cfg(serial_device_name),
    m_device(m_cfg),
    m_connection(m_device),
    m_modbus_client(m_connection),
    m_unit_id(unit_id),
    m_base_address(base_address) {
}

bool ModbusTransport::trigger_snapshot_generation(
    protocol_related_types::ModbusRegisterAddress snapshot_trigger_register) {

    using namespace std::chrono_literals;

    const everest::modbus::DataVectorUint16 bsm_powermeter_create_snapshot_command{0x0002};
    everest::modbus::ModbusDataContainerUint16 outgoing(everest::modbus::ByteOrder::LittleEndian,
                                                        bsm_powermeter_create_snapshot_command);

    m_modbus_client.write_multiple_registers(m_unit_id, snapshot_trigger_register.val, 1, outgoing, false);

    std::uint16_t response_status{};

    std::size_t counter = 10;

    while (counter-- > 0) {

        transport::DataVector response =
            m_modbus_client.read_holding_register(m_unit_id, snapshot_trigger_register.val, true);

        response_status = be16toh((*reinterpret_cast<std::uint16_t*>(&response.data()[0])));

        if (response_status == 0)
            return true;

        std::this_thread::sleep_for(750ms);
    }

    return false;
}

bool ModbusTransport::trigger_snapshot_generation_BSM() {

    return trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress(40525_sma));
}

bool ModbusTransport::trigger_snapshot_generation_BSM_OCMF() {

    return trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress(41795_sma));
}

transport::DataVector ModbusTransport::fetch(const known_model::AddressData& ad) {
    return fetch(ad.base_offset, ad.model_size);
}

transport::DataVector ModbusTransport::fetch(protocol_related_types::SunspecDataModelAddress model_address,
                                             protocol_related_types::SunspecRegisterCount model_length) {

    transport::DataVector response;
    response.reserve(model_length * 2); // this is a uint 8 vector
    std::size_t max_regiser_read = everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE;
    protocol_related_types::SunspecRegisterCount remaining_register_to_read{model_length};
    protocol_related_types::ModbusRegisterAddress read_address{m_base_address + model_address};

    while (remaining_register_to_read > 0) {
        std::size_t register_to_read =
            remaining_register_to_read > max_regiser_read ? max_regiser_read : remaining_register_to_read;

        transport::DataVector tmp =
            m_modbus_client.read_holding_register(m_unit_id, read_address.val, register_to_read);
        response.insert(response.end(), tmp.begin(), tmp.end());

        read_address.val += register_to_read;
        remaining_register_to_read -= register_to_read;
    }

    return response;
}

transport::DataVector SerialCommHubTransport::fetch(protocol_related_types::SunspecDataModelAddress model_address,
                                                    protocol_related_types::SunspecRegisterCount model_length) {

    transport::DataVector response;
    response.reserve(model_length * 2); // this is a uint 8 vector
    std::size_t max_regiser_read = everest::modbus::consts::rtu::MAX_REGISTER_PER_MESSAGE;
    protocol_related_types::SunspecRegisterCount remaining_register_to_read{model_length};
    protocol_related_types::ModbusRegisterAddress read_address{m_base_address + model_address};

    while (remaining_register_to_read > 0) {
        std::size_t register_to_read =
            remaining_register_to_read > max_regiser_read ? max_regiser_read : remaining_register_to_read;

        types::serial_comm_hub_requests::Result serial_com_hub_result =
            m_serial_hub.call_modbus_read_holding_registers(m_unit_id, read_address.val, register_to_read, 0);

        if (not serial_com_hub_result.value.has_value())
            throw std::runtime_error("no result from serial com hub!");

        // make sure that returned vector is a int32 vector
        static_assert(
            std::is_same<
                int32_t,
                decltype(types::serial_comm_hub_requests::Result::value)::this_type::value_type::value_type>::value);

        union {
            int32_t val_32;
            struct {
                uint8_t v3;
                uint8_t v2;
                uint8_t v1;
                uint8_t v0;
            } val_8;
        } swapit;

        static_assert(sizeof(swapit.val_32) == sizeof(swapit.val_8));

        transport::DataVector tmp{};

        for (auto item : serial_com_hub_result.value.get()) {
            swapit.val_32 = item;
            tmp.push_back(swapit.val_8.v2);
            tmp.push_back(swapit.val_8.v3);
        }

        response.insert(response.end(), tmp.begin(), tmp.end());

        read_address.val += register_to_read;
        remaining_register_to_read -= register_to_read;
    }

    return response;
}

transport::DataVector SerialCommHubTransport::fetch(const known_model::AddressData& ad) {

    return fetch(ad.base_offset, ad.model_size);
}

bool SerialCommHubTransport::trigger_snapshot_generation(
    protocol_related_types::ModbusRegisterAddress snapshot_trigger_register) {

    using namespace std::chrono_literals;
    using namespace std::string_literals;

    types::serial_comm_hub_requests::VectorUint16 trigger_create_snapshot_command{{0x0002}};

    types::serial_comm_hub_requests::StatusCodeEnum write_result =
        m_serial_hub.call_modbus_write_multiple_registers(m_unit_id, snapshot_trigger_register.val, trigger_create_snapshot_command, 0);

    if (not(types::serial_comm_hub_requests::StatusCodeEnum::Success == write_result))
        throw(""s + __PRETTY_FUNCTION__ + " SerialCommHub error : "s +
              types::serial_comm_hub_requests::status_code_enum_to_string( write_result ) );

    std::size_t counter = 10;

    while (counter-- > 0) {

        types::serial_comm_hub_requests::Result serial_com_hub_result =
            m_serial_hub.call_modbus_read_holding_registers(m_unit_id, snapshot_trigger_register.val, true, 0);

        if (not serial_com_hub_result.value.has_value())
            throw std::runtime_error("no result from serial com hub!");

        auto snapshot_generatrion_code = serial_com_hub_result.value.get();

        if ((not snapshot_generatrion_code.empty()) and (snapshot_generatrion_code[0] == 0))
            return true;

        std::this_thread::sleep_for(750ms);
    }

    return false;
}

bool SerialCommHubTransport::trigger_snapshot_generation_BSM() {

    return trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress(40525_sma));
}

bool SerialCommHubTransport::trigger_snapshot_generation_BSM_OCMF() {

    return trigger_snapshot_generation(protocol_related_types::ModbusRegisterAddress(41795_sma));
}

} // namespace transport
