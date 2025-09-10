// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__PDU_CORRELATION_HPP
#define MODBUS_SERVER__PDU_CORRELATION_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>

#include "transport_protocol.hpp"

namespace modbus_server {

class PDUCorrelationLayerIntf {
public:
    using On_PDU_Callback_t = std::function<std::optional<pdu::GenericPDU>(const pdu::GenericPDU&)>;

protected:
    std::optional<On_PDU_Callback_t> on_pdu;

public:
    PDUCorrelationLayerIntf() = default;
    virtual ~PDUCorrelationLayerIntf() = default;

    /**
     * @brief Read all incoming data and correlate it with the requests that are
     * in transit. This method should be called regularly to ensure that all PDUs
     * are correlated.
     *
     * @note Note that without running this function regularly, nothing will be
     * received, thus \c request_response will not work correctly.
     *
     */
    virtual void blocking_poll() = 0;

    virtual bool poll() = 0;

    /**
     * @brief Send a request and wait for a corresponding response with the same
     * context and function code (masked with 0x7f to be able to receive error
     * responses).
     *
     * @param request The request to send
     * @param timeout The timeout to wait for the response.
     * @return pdu::GenericPDU The response to the request
     * @throws std::runtime_error If the timeout is reached
     */
    virtual pdu::GenericPDU request_response(const pdu::GenericPDU& request, std::chrono::milliseconds timeout) = 0;

    /**
     * @brief Send a request without waiting for a response.
     *
     * @param request The request to send
     */
    virtual void request_without_response(const pdu::GenericPDU& request) = 0;

    /**
     * @brief Set the callback for when a PDU is received that is not part of a
     * request_response call.
     */
    void set_on_pdu(On_PDU_Callback_t on_pdu) {
        this->on_pdu = on_pdu;
    }
};

class PDUCorrelationLayer : public PDUCorrelationLayerIntf {
protected:
    std::shared_ptr<ModbusProtocol> protocol;

    struct ListeningForResponseEntry {
        ModbusProtocol::Context context;
        uint8_t function_code;
        std::optional<pdu::GenericPDU> pdu;
    };

    std::vector<ListeningForResponseEntry> listening_for_response;

    std::mutex listening_for_response_mutex;
    std::condition_variable listening_for_response_cv;

    void on_poll_data(modbus_server::ModbusProtocol::Context context, pdu::GenericPDU pdu);

public:
    PDUCorrelationLayer(std::shared_ptr<ModbusProtocol> protocol) : protocol(protocol) {
    }

    void blocking_poll() override;
    bool poll() override;
    pdu::GenericPDU request_response(const pdu::GenericPDU& request, std::chrono::milliseconds timeout) override;
    void request_without_response(const pdu::GenericPDU& request) override;
};

}; // namespace modbus_server

#endif
