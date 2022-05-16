// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "powermeterImpl.hpp"


using namespace everest;

namespace module {
namespace main {

void powermeterImpl::init() {
    this->init_modbus_client();
}

void powermeterImpl::ready() {
    this->meter_loop_thread = std::thread( [this] { run_meter_loop(); } );
}

void powermeterImpl::run_meter_loop() {
    EVLOG(debug) << "Starting ModbusMeter loop";
    int32_t power_in, power_out, energy_in, energy_out, power_surplus, energy_surplus;
    while (true) {

        // Reading meter values from MODBUS device
        power_in = (uint32_t) this->read_power_in();
        power_out = (uint32_t) this->read_power_out();
        energy_in = (uint32_t) this->read_energy_in();
        energy_out = (uint32_t) this->read_energy_out();

        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(date::utc_clock::now().time_since_epoch()).count();

        // Publishing relevant vars
        json j;
        j["time_stamp"] = timestamp;
        j["meter_id"] = "MODBUS_POWERMETER";
        j["energy_Wh_import"]["total"] = energy_in;
        j["energy_Wh_export"]["total"] = energy_out;

        j["power_W"]["total"] = power_in-power_out;
        std::this_thread::sleep_for(std::chrono::milliseconds(config.update_interval));

    }
}

void powermeterImpl::init_modbus_client() {
    this->tcp_conn = std::make_unique<connection::TCPConnection>(config.modbus_ip_address, config.modbus_port);
    this->modbus_client = std::make_unique<modbus::ModbusTCPClient>(*this->tcp_conn);
}

uint32_t powermeterImpl::read_power_in() {
    std::vector<uint8_t> bytevector = this->modbus_client->read_holding_register(config.power_unit_id, config.power_in_register, config.power_in_length);
    modbus::utils::print_message_hex(bytevector);
    return sunspec::conversion::bytevector_to_uint32(bytevector);
}

uint32_t powermeterImpl::read_power_out() {
    std::vector<uint8_t> bytevector = this->modbus_client->read_holding_register(config.power_unit_id, config.power_out_register, config.power_out_length);
    return sunspec::conversion::bytevector_to_uint32(bytevector);
}

uint32_t powermeterImpl::read_energy_in() {
    std::vector<uint8_t> bytevector = this->modbus_client->read_holding_register(config.energy_unit_id, config.energy_in_register, config.energy_in_length);
    return sunspec::conversion::bytevector_to_uint32(bytevector);
}

uint32_t powermeterImpl::read_energy_out() {
    std::vector<uint8_t> bytevector = this->modbus_client->read_holding_register(config.energy_unit_id, config.energy_out_register, config.energy_out_length);
    return sunspec::conversion::bytevector_to_uint32(bytevector);
}

std::string powermeterImpl::handle_get_signed_meter_value(std::string& auth_token) {
    return "NOT_AVAILABLE";
}

} // namespace main
} // namespace module
