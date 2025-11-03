// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__MODBUS_BASIC_SERVER_HPP
#define MODBUS_SERVER__MODBUS_BASIC_SERVER_HPP

#include <functional>
#include <logs/logs.hpp>
#include <modbus-server/frames.hpp>
#include <modbus-server/frames/read_holding_registers.hpp>
#include <modbus-server/frames/write_multiple_registers.hpp>
#include <modbus-server/frames/write_single_register.hpp>
#include <modbus-server/pdu_correlation.hpp>
#include <modbus-server/transport.hpp>
#include <modbus-server/transport_protocol.hpp>
#include <optional>
#include <stdexcept>

namespace modbus_server {

class ModbusBasicServer {
public:
    // An alias for a function that takes a request PDU and always returns a
    // corresponding response PDU
    template <class Req, class Res> using AlwaysRespondingPDUHandler = std::function<Res(Req)>;

protected:
    std::shared_ptr<PDUCorrelationLayerIntf> pas;

    logs::LogIntf log;

    std::optional<AlwaysRespondingPDUHandler<pdu::ReadHoldingRegistersRequest, pdu::ReadHoldingRegistersResponse>>
        read_registers_request;
    std::optional<AlwaysRespondingPDUHandler<pdu::WriteMultipleRegistersRequest, pdu::WriteMultipleRegistersResponse>>
        write_multiple_registers_request;
    std::optional<AlwaysRespondingPDUHandler<pdu::WriteSingleRegisterRequest, pdu::WriteSingleRegisterResponse>>
        write_single_register_request;

public:
    ModbusBasicServer(std::shared_ptr<PDUCorrelationLayerIntf> pas, logs::LogIntf log = logs::log_printf);

    void set_read_holding_registers_request_cb(
        AlwaysRespondingPDUHandler<pdu::ReadHoldingRegistersRequest, pdu::ReadHoldingRegistersResponse> fn);
    void set_write_multiple_registers_request_cb(
        AlwaysRespondingPDUHandler<pdu::WriteMultipleRegistersRequest, pdu::WriteMultipleRegistersResponse> fn);
    void set_write_single_register_request_cb(
        AlwaysRespondingPDUHandler<pdu::WriteSingleRegisterRequest, pdu::WriteSingleRegisterResponse> fn);

protected:
    /**
     * @brief Calls \c on_pdu and catches all exceptions. If \c on_pdu returns an
     * \c std::nullopt this function will do so too. If \c on_pdu throws an
     * exception, this function will catch it and return an error PDU that fits
     * the exception best.
     *
     * @details The mapping currently works as follows: pdu::DecodingError ->
     * ILLEGAL_DATA_VALUE; pdu::EncodingError -> SERVER_DEVICE_FAILURE;
     * ApplicationServerError -> calls to_pdu on exception object; any other
     * exception -> SERVER_DEVICE_FAILURE
     *
     * @param input the incoming PDU
     * @return std::optional<pdu::GenericPDU> the return value of \c on_pdu or an
     * pdu::ErrorPDU if an exception was thrown
     */
    std::optional<pdu::GenericPDU> on_pdu_error_handled(const pdu::GenericPDU& input);

    /**
     * @brief Incoming PDU handler, called by the \c PDUCorrelationLayer. Can be
     * extended by subclasses but \c ModbusBasicServer::on_pdu should always be
     * called at the end.
     *
     * This function can also throw exceptions which will all be caught by the
     * wrapper function \c on_pdu_error_handled
     *
     * @param input the incoming PDU
     * @return std::optional<pdu::GenericPDU> the corresponding response PDU, if a
     * answer is available; returns an ILLEGAL_FUNCTION error PDU if the functino
     * code is unknown or a corresponding handler is not set
     *
     * @throws modbus_server::pdu::DecodingError if the incoming PDU could not be
     * decoded (most likely client's fault)
     * @throws modbus_server::pdu::EncodingError if the response PDU could not be
     * encoded (most likely application server's fault)
     * @throws modbus_server::ApplicationServerError if the application wants to
     * send an error PDU as answer
     */
    virtual std::optional<pdu::GenericPDU> on_pdu(const pdu::GenericPDU& input);
};

} // namespace modbus_server

#endif
