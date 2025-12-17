// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/client.hpp>
#include <modbus-server/frames/read_holding_registers.hpp>
#include <modbus-server/frames/write_multiple_registers.hpp>
#include <modbus-server/frames/write_single_register.hpp>
#include <stdexcept>

using namespace modbus_server::client;
using namespace modbus_server;

ModbusClient::ModbusClient(std::shared_ptr<PDUCorrelationLayerIntf> pas) : pas(pas) {
}

void ModbusClient::handle_error_response(const pdu::GenericPDU& response) {
    if ((response.function_code & 0x80) != 0) {
        throw pdu::PDUException(response);
    }
}

std::vector<std::uint16_t> ModbusClient::read_holding_registers(std::uint16_t start_address, std::uint16_t quantity) {
    pdu::ReadHoldingRegistersRequest request;
    request.register_start = start_address;
    request.register_count = quantity;

    pdu::GenericPDU generic = request.to_generic();

    auto response = pas->request_response(generic, std::chrono::seconds(5));
    handle_error_response(response);

    pdu::ReadHoldingRegistersResponse response_data;
    response_data.from_generic(response);

    return response_data.get_register_data();
}

void ModbusClient::write_single_register(std::uint16_t address, std::uint16_t value) {
    pdu::WriteSingleRegisterRequest request;
    request.register_address = address;
    request.register_value = value;

    pdu::GenericPDU generic = request.to_generic();

    auto resp = pas->request_response(generic, std::chrono::seconds(5));
    handle_error_response(resp);
}

void ModbusClient::write_multiple_registers(std::uint16_t start_address, const std::vector<std::uint16_t>& values) {
    pdu::WriteMultipleRegistersRequest request;
    request.register_start = start_address;
    request.register_count = values.size();
    request.register_data.reserve(values.size() * 2);

    for (auto value : values) {
        request.register_data.push_back(value >> 8);
        request.register_data.push_back(value & 0xFF);
    }

    pdu::GenericPDU generic = request.to_generic();

    auto resp = pas->request_response(generic, std::chrono::seconds(5));
    handle_error_response(resp);
}
