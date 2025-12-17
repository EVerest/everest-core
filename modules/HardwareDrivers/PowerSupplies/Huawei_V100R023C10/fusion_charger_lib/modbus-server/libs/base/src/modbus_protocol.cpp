// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/transport_protocol.hpp>

using namespace modbus_server;

ModbusProtocol::ModbusProtocol(std::shared_ptr<ModbusTransport> transport) : transport(transport) {
}

bool ModbusProtocol::Context::operator==(const ModbusProtocol::Context& other) const {
    return this->data == other.data;
}

bool ModbusProtocol::Context::operator!=(const ModbusProtocol::Context& other) const {
    return this->data != other.data;
}

ModbusTCPProtocol::ModbusTCPContext::ModbusTCPContext(const ModbusProtocol::Context& context) {
    this->transaction_id = context.data[0] << 8 | context.data[1];
    this->protocol_id = context.data[2] << 8 | context.data[3];
    this->unit_id = context.data[4];
}

ModbusTCPProtocol::ModbusTCPContext::ModbusTCPContext() : transaction_id(0), protocol_id(0), unit_id(0) {
}

ModbusProtocol::Context ModbusTCPProtocol::ModbusTCPContext::to_context() {
    ModbusProtocol::Context context;
    context.data.resize(5);
    context.data[0] = this->transaction_id >> 8;
    context.data[1] = this->transaction_id & 0xFF;
    context.data[2] = this->protocol_id >> 8;
    context.data[3] = this->protocol_id & 0xFF;
    context.data[4] = this->unit_id;
    return context;
}

ModbusTCPProtocol::ModbusTCPProtocol(std::shared_ptr<ModbusTransport> transport) :
    ModbusProtocol(transport), current_transaction_id(0) {
}

ModbusTCPProtocol::ModbusTCPProtocol(std::shared_ptr<ModbusTransport> transport, std::uint16_t unit_id,
                                     std::uint16_t transaction_id) :
    ModbusProtocol(transport), sending_unit_id(unit_id), current_transaction_id(transaction_id) {
}

ModbusTCPProtocol::Context ModbusTCPProtocol::new_send_context() {
    ModbusTCPContext context;
    context.transaction_id = this->current_transaction_id++;
    context.protocol_id = 0;
    context.unit_id = sending_unit_id;
    return context.to_context();
}

std::tuple<ModbusProtocol::Context, pdu::GenericPDU> ModbusTCPProtocol::receive_blocking() {
    auto header = this->transport->read_bytes(7);

    std::uint16_t transaction_id = header[0] << 8 | header[1];
    std::uint16_t protocol_id = header[2] << 8 | header[3];
    std::uint16_t length = header[4] << 8 | header[5];
    std::uint8_t unit_id = header[6];

    auto data = this->transport->read_bytes(length - 1);

    ModbusTCPContext context;
    context.transaction_id = transaction_id;
    context.protocol_id = protocol_id;
    context.unit_id = unit_id;

    return {context.to_context(), pdu::GenericPDU(data)};
}

std::optional<std::pair<modbus_server::ModbusProtocol::Context, pdu::GenericPDU>>
modbus_server::ModbusTCPProtocol::try_receive() {
    auto header_opt = this->transport->try_read_bytes(7);
    if (!header_opt.has_value()) {
        return std::nullopt;
    }
    auto header = header_opt.value();

    std::uint16_t transaction_id = header[0] << 8 | header[1];
    std::uint16_t protocol_id = header[2] << 8 | header[3];
    std::uint16_t length = header[4] << 8 | header[5];
    std::uint8_t unit_id = header[6];

    if (length <= 1) {
        return std::nullopt; // first of all: nothing to be read, secondly: the
                             // length is invalid
    }

    auto data = this->transport->read_bytes(length - 1);

    if (data.size() != length - 1) {
        throw std::runtime_error("Transport returned less data than requested");
    }

    ModbusTCPContext context;
    context.transaction_id = transaction_id;
    context.protocol_id = protocol_id;
    context.unit_id = unit_id;

    std::pair<modbus_server::ModbusProtocol::Context, pdu::GenericPDU> result = {context.to_context(),
                                                                                 pdu::GenericPDU(data)};

    return std::optional(result);
}
void ModbusTCPProtocol::send_blocking(const pdu::GenericPDU& pdu, const ModbusProtocol::Context& context) {
    auto pdu_data = pdu.to_vector();
    std::uint16_t size_in_header = pdu_data.size() + 1; // +1 for the unit id in the header

    auto context_parsed = ModbusTCPContext(context);

    std::vector<std::uint8_t> frame(7);
    frame[0] = context_parsed.transaction_id >> 8;
    frame[1] = context_parsed.transaction_id & 0xFF;
    frame[2] = context_parsed.protocol_id >> 8;
    frame[3] = context_parsed.protocol_id & 0xFF;
    frame[4] = size_in_header >> 8;
    frame[5] = size_in_header & 0xFF;
    frame[6] = context_parsed.unit_id;

    frame.insert(frame.end(), pdu_data.begin(), pdu_data.end());

    this->transport->write_bytes(frame);
}
