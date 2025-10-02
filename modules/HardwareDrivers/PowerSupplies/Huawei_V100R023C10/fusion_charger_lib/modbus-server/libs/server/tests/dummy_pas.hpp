// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_TESTS__DUMMY_PAS_HPP
#define MODBUS_TESTS__DUMMY_PAS_HPP

#include <modbus-server/pdu_correlation.hpp>

class DummyPDUCorrelationLayer : public modbus_server::PDUCorrelationLayerIntf {
    std::vector<modbus_server::pdu::GenericPDU> next_answer;
    std::vector<modbus_server::pdu::GenericPDU> last_request;

public:
    DummyPDUCorrelationLayer() = default;

    void blocking_poll() override {
    }
    bool poll() override {
        return false;
    };

    modbus_server::pdu::GenericPDU request_response(const modbus_server::pdu::GenericPDU& request,
                                                    std::chrono::milliseconds timeout) override {
        last_request.push_back(request);

        if (next_answer.empty()) {
            throw std::runtime_error("No answer available");
        }
        auto answer = next_answer.front();
        next_answer.erase(next_answer.begin());
        return answer;
    }

    void request_without_response(const modbus_server::pdu::GenericPDU& request) override {
        last_request.push_back(request);
    }

    void add_next_answer(const modbus_server::pdu::GenericPDU& answer) {
        next_answer.push_back(answer);
    }

    std::optional<modbus_server::pdu::GenericPDU> call_on_pdu(const modbus_server::pdu::GenericPDU& pdu) {
        return this->on_pdu.value()(pdu);
    }

    modbus_server::pdu::GenericPDU get_last_request() {
        if (last_request.empty()) {
            throw std::runtime_error("No request available");
        }
        auto request = last_request.front();
        last_request.erase(last_request.begin());
        return request;
    }
};

#endif
