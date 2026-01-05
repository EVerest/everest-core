// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__TRANSPORT_PROTOCOL_HPP
#define MODBUS_SERVER__TRANSPORT_PROTOCOL_HPP

#include <memory>
#include <tuple>

#include "transport.hpp"

namespace modbus_server {

/**
 * @brief The modbus protocol layer, uses a transport layer to send and receive
 * data. Splits the data into Modbus PDUs and a corresponding context for this
 * message.
 *
 * The context is used to store information about the message, such as the
 * transaction id for Modbus TCP.
 *
 * The context is then used to correlate the response with the request by
 * another higher layer.
 *
 */
class ModbusProtocol {
public:
    struct Context {
        std::vector<std::uint8_t> data;

        bool operator==(const Context& other) const;
        bool operator!=(const Context& other) const;
    };

protected:
    std::shared_ptr<ModbusTransport> transport;

public:
    ModbusProtocol(std::shared_ptr<ModbusTransport> transport);

    /**
     * @brief If not responding to a request, e.g. when sending a request, use
     * this method to create a new context for sending a message.
     *
     * @return Context The new context to be used for sending a message
     */
    virtual Context new_send_context() = 0;

    /**
     * @brief Receive a PDU via the transport layer in a blocking manner. The
     * context of the message is also returned, which can then be used to either
     * correlate received response to its request or received request to the
     * response.
     *
     * @return std::tuple<Context, pdu::GenericPDU> The context and the received
     * PDU as a tuple
     */

    virtual std::tuple<Context, pdu::GenericPDU> receive_blocking() = 0;

    virtual std::optional<std::pair<Context, pdu::GenericPDU>> try_receive() = 0;
    /**
     * @brief Send a PDU via the transport layer and block until the message is
     * sent. If you need to send a message without already having a context, use
     * \c new_send_context to create a new context (e.g. when sending a request).
     * If answering to a request use the context of the request to answer.
     *
     * @param pdu The PDU to send
     * @param context The context of the message
     */
    virtual void send_blocking(const pdu::GenericPDU& pdu, const Context& context) = 0;
};

/**
 * @brief A modbus protocol implementation for Modbus TCP.
 *
 */
class ModbusTCPProtocol : public ModbusProtocol {
protected:
    std::uint16_t sending_unit_id = 0xFF; // default 0xFF
    std::uint16_t current_transaction_id;
    struct ModbusTCPContext {
        std::uint16_t transaction_id;
        std::uint16_t protocol_id;
        std::uint8_t unit_id;

        ModbusTCPContext();
        ModbusTCPContext(const Context& context);
        Context to_context();
    };

public:
    /**
     * @brief Create a new ModbusTCPProtocol using a transport layer
     *
     * @param transport the transport layer to use
     */
    ModbusTCPProtocol(std::shared_ptr<ModbusTransport> transport);
    /**
     * @brief Create a new ModbusTCPProtocol using a transport layer, a unit id
     * for sending and an initial transaction id for sending
     *
     * @param transport the transport layer to use
     * @param unit_id the unit id to use for sending
     * @param transaction_id the initial transaction id to use for sending
     */
    ModbusTCPProtocol(std::shared_ptr<ModbusTransport> transport, std::uint16_t unit_id, std::uint16_t transaction_id);

    Context new_send_context() override;
    std::tuple<Context, pdu::GenericPDU> receive_blocking() override;
    std::optional<std::pair<Context, pdu::GenericPDU>> try_receive() override;
    void send_blocking(const pdu::GenericPDU& pdu, const Context& context) override;
};

} // namespace modbus_server

#endif
