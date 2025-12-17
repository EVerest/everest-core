// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <modbus-server/pdu_correlation.hpp>

using namespace modbus_server;

void modbus_server::PDUCorrelationLayer::on_poll_data(modbus_server::ModbusProtocol::Context context,
                                                      pdu::GenericPDU pdu) {
    {
        std::lock_guard<std::mutex> lock(this->listening_for_response_mutex);
        for (auto& entry : this->listening_for_response) {
            if (entry.context != context) {
                continue;
            }

            if ((entry.function_code & 0x7f) != (pdu.function_code & 0x7f)) {
                continue;
            }

            entry.pdu = pdu;
            this->listening_for_response_cv.notify_all();
            return;
        }
    }

    if (!this->on_pdu.has_value()) {
        return;
    }

    auto response = this->on_pdu.value()(pdu);
    if (response.has_value()) {
        this->protocol->send_blocking(response.value(), context);
    }
}

void PDUCorrelationLayer::blocking_poll() {
    auto [context, pdu] = this->protocol->receive_blocking();
    on_poll_data(context, pdu);
}

bool modbus_server::PDUCorrelationLayer::poll() {
    std::optional<std::pair<modbus_server::ModbusProtocol::Context, pdu::GenericPDU>> data =
        this->protocol->try_receive();

    if (!data.has_value()) {
        return false;
    }

    on_poll_data(data.value().first, data.value().second);
    return true;
}

pdu::GenericPDU PDUCorrelationLayer::request_response(const pdu::GenericPDU& request,
                                                      std::chrono::milliseconds timeout) {
    auto context = this->protocol->new_send_context();

    {
        std::lock_guard<std::mutex> lock(this->listening_for_response_mutex);
        this->listening_for_response.push_back({context, request.function_code, std::nullopt});
    }

    this->protocol->send_blocking(request, context);

    auto end_time = std::chrono::steady_clock::now() + timeout;

    while (std::chrono::steady_clock::now() < end_time) {
        auto rest_timeout = end_time - std::chrono::steady_clock::now();

        std::unique_lock<std::mutex> lock(this->listening_for_response_mutex);
        this->listening_for_response_cv.wait_for(lock, rest_timeout);

        // check if we have a response
        ssize_t index = -1;
        for (ssize_t i = 0; i < this->listening_for_response.size(); i++) {
            if (this->listening_for_response[i].context == context) {
                index = i;
            }
        }

        if (index == -1) {
            throw std::runtime_error("context is gone");
        }

        // if we have a response, erase the entry and return the response
        if (this->listening_for_response[index].pdu.has_value()) {
            auto ret = this->listening_for_response[index].pdu.value();
            this->listening_for_response.erase(this->listening_for_response.begin() + index);

            return ret;
        }

        // if we don't have a response, check if we have timed out and wait again
    }

    throw std::runtime_error("timeout");
}

void PDUCorrelationLayer::request_without_response(const pdu::GenericPDU& request) {
    auto context = this->protocol->new_send_context();
    this->protocol->send_blocking(request, context);
}
