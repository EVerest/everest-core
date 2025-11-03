// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__CLIENT_HPP
#define MODBUS_SERVER__CLIENT_HPP

#include <modbus-server/pdu_correlation.hpp>
#include <vector>

namespace modbus_server {
namespace client {

class ModbusClient {
    std::shared_ptr<PDUCorrelationLayerIntf> pas;

protected:
    /**
     * @brief Utility function; if given response is an error, throw \c
     * PDUException
     *
     * @param response The response to check
     * @throws PDUException if the response is an error
     */
    void handle_error_response(const pdu::GenericPDU& response);

public:
    ModbusClient(std::shared_ptr<PDUCorrelationLayerIntf> pas);
    std::vector<uint16_t> read_holding_registers(uint16_t start_address, uint16_t quantity);
    void write_single_register(uint16_t address, uint16_t value);
    void write_multiple_registers(uint16_t start_address, const std::vector<uint16_t>& values);
};

} // namespace client
} // namespace modbus_server

#endif
