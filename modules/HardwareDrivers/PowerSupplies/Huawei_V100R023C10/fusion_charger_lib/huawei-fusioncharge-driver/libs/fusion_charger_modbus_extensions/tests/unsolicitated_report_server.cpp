// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/modbus/extensions/unsolicitated_report_server.hpp>

using namespace fusion_charger::modbus_extensions;

class DummyPDUCorrelationLayer : public modbus_server::PDUCorrelationLayerIntf {
    std::vector<modbus_server::pdu::GenericPDU> next_answer;
    std::vector<modbus_server::pdu::GenericPDU> last_request;

public:
    DummyPDUCorrelationLayer() = default;

    void blocking_poll() override {
    }
    bool poll() override {
        return false;
    }

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

TEST(UnsolicitatedReportBasicServer, send_unsolicitated_report_no_response) {
    auto corr_layer = std::make_shared<DummyPDUCorrelationLayer>();
    UnsolicitatedReportBasicServer server(corr_layer);
    UnsolicitatedReportRequest request;
    request.devices.push_back({0x1234, {{0x5678, 0x0001, {0x00, 0x01}}}});
    request.response_required = false;

    auto before_time = std::chrono::system_clock().now();
    auto response = server.send_unsolicitated_report(request, std::chrono::milliseconds(100));
    auto after_time = std::chrono::system_clock().now();

    ASSERT_FALSE(response.has_value());

    // Sending should be fast as no response is expected
    ASSERT_TRUE(after_time - before_time < std::chrono::milliseconds(10));

    auto last_request = corr_layer->get_last_request();
    ASSERT_EQ(last_request.function_code, 0x41);
    ASSERT_EQ(last_request.data[0], 0x91);
    // len
    ASSERT_EQ(last_request.data[1], 0x00);
    ASSERT_EQ(last_request.data[2], 13);
    //  no response
    ASSERT_EQ(last_request.data[3], 0x00);
    // devices
    ASSERT_EQ(last_request.data[4], 0x00);
    ASSERT_EQ(last_request.data[5], 0x01);
    // location
    ASSERT_EQ(last_request.data[6], 0x12);
    ASSERT_EQ(last_request.data[7], 0x34);
    // segments
    ASSERT_EQ(last_request.data[8], 0x00);
    ASSERT_EQ(last_request.data[9], 0x01);
    // start
    ASSERT_EQ(last_request.data[10], 0x56);
    ASSERT_EQ(last_request.data[11], 0x78);
    // more doesn't need to be checked...
}

TEST(UnsolicitatedReportBasicServer, send_unsolicitated_report_with_response) {
    auto corr_layer = std::make_shared<DummyPDUCorrelationLayer>();
    UnsolicitatedReportBasicServer server(corr_layer);

    UnsolicitatedReportRequest request;
    request.devices.push_back({0x1234, {{0x5678, 0x0001, {0x00, 0x01}}}});
    request.response_required = true;

    {
        UnsolicitatedReportResponse response;
        response.success = true;

        corr_layer->add_next_answer(response.to_generic());
    }

    auto before_time = std::chrono::system_clock().now();
    auto response = server.send_unsolicitated_report(request, std::chrono::milliseconds(100));
    auto after_time = std::chrono::system_clock().now();

    // Sending should be no longer than timeout
    ASSERT_TRUE(after_time - before_time < std::chrono::milliseconds(100));

    auto last_request = corr_layer->get_last_request();
    ASSERT_EQ(last_request.function_code, 0x41);
    ASSERT_EQ(last_request.data[0], 0x91);
    // len
    ASSERT_EQ(last_request.data[1], 0x00);
    ASSERT_EQ(last_request.data[2], 13);
    // with response
    ASSERT_EQ(last_request.data[3], 0x80);
    // more doesn't need to be checked...

    ASSERT_TRUE(response.has_value());
    ASSERT_TRUE(response.value().success);
}

TEST(UnsolicitatedReportBasicServer, sent_unsolicitated_report_is_defragmented) {
    auto corr_layer = std::make_shared<DummyPDUCorrelationLayer>();
    UnsolicitatedReportBasicServer server(corr_layer);
    UnsolicitatedReportRequest request;
    request.devices.push_back({0x1234,
                               {
                                   {0x5678, 0x0001, {0xde, 0xad}},
                                   {0x5679, 0x0001, {0xbe, 0xef}},
                               }});
    request.response_required = false;

    auto before_time = std::chrono::system_clock().now();
    auto response = server.send_unsolicitated_report(request, std::chrono::milliseconds(100));
    auto after_time = std::chrono::system_clock().now();

    ASSERT_FALSE(response.has_value());

    // Sending should be fast as no response is expected
    ASSERT_TRUE(after_time - before_time < std::chrono::milliseconds(10));

    auto last_request = corr_layer->get_last_request();
    ASSERT_EQ(last_request.function_code, 0x41);
    ASSERT_EQ(last_request.data.size(), 18);
    ASSERT_EQ(last_request.data[0], 0x91);
    // len
    ASSERT_EQ(last_request.data[1], 0x00);
    ASSERT_EQ(last_request.data[2], 15);
    //  no response
    ASSERT_EQ(last_request.data[3], 0x00);
    // devices
    ASSERT_EQ(last_request.data[4], 0x00);
    ASSERT_EQ(last_request.data[5], 0x01);
    // location
    ASSERT_EQ(last_request.data[6], 0x12);
    ASSERT_EQ(last_request.data[7], 0x34);
    // segments, only one segment should be sent
    ASSERT_EQ(last_request.data[8], 0x00);
    ASSERT_EQ(last_request.data[9], 0x01);
    // start
    ASSERT_EQ(last_request.data[10], 0x56);
    ASSERT_EQ(last_request.data[11], 0x78);
    // count
    ASSERT_EQ(last_request.data[12], 0x00);
    ASSERT_EQ(last_request.data[13], 0x02);
    // data
    ASSERT_EQ(last_request.data[14], 0xde);
    ASSERT_EQ(last_request.data[15], 0xad);
    ASSERT_EQ(last_request.data[16], 0xbe);
    ASSERT_EQ(last_request.data[17], 0xef);
}
