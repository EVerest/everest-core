// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/pdu_correlation.hpp>
#include <modbus-server/transport_protocol.hpp>
#include <thread>

#include "dummy_modbus_transport.hpp"

using namespace modbus_server;

TEST(PDUCorrelationLayer, check_tcp_protocol_works_expectedly) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol(transport, 0x01, 0x0000);

    // check that first send context has transaction id 0
    auto context = protocol.new_send_context();
    protocol.send_blocking(pdu::GenericPDU(0x01, {0x02}), context);
    auto written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 9);
    ASSERT_EQ(written[0], 0x00);
    ASSERT_EQ(written[1], 0x00);

    // check that second send context has transaction id 1
    context = protocol.new_send_context();
    protocol.send_blocking(pdu::GenericPDU(0x03, {0x04}), context);
    written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 9);
    ASSERT_EQ(written[0], 0x00);
    ASSERT_EQ(written[1], 0x01);
}

TEST(PDUCorrelationLayer, request_response_single) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    pdu::GenericPDU request = pdu::GenericPDU(0xab, {0x01});

    // answer
    transport->add_incoming_data({
        0x00, 0x00,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xab,            // function code
        0x00, 0x01, 0x02 // data
    });

    auto thread = std::thread([&pal]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
    });

    auto response = pal.request_response(request, std::chrono::milliseconds(1000));
    ASSERT_EQ(response.function_code, 0xab);
    ASSERT_EQ(response.data.size(), 3);
    ASSERT_EQ(response.data[0], 0x00);
    ASSERT_EQ(response.data[1], 0x01);
    ASSERT_EQ(response.data[2], 0x02);

    thread.join();
}

TEST(PDUCorrelationLayer, request_response_parallel_out_of_order) {
    /*
     * What is tested here:
     * at time  5ms: request 1 is sent
     * at time 10ms: request 2 is sent
     * at time 20ms: poll is called, response 2 is received
     * at time 40ms: poll is called, response 1 is received
     *
     * Expected result:
     *   response 2 is received before response 1 because it was sent first
     *   response 1 contains response data to request 1
     *   response 2 contains response data to request 2
     */
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    pdu::GenericPDU request1 = pdu::GenericPDU(0xab, {0x01});
    pdu::GenericPDU request2 = pdu::GenericPDU(0xcd, {0x02});

    // answer for request2 (transaction id 1)
    transport->add_incoming_data({
        0x00, 0x01,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xcd,            // function code
        0x00, 0x01, 0x02 // data
    });

    // answer for request1 (transaction id 0)
    transport->add_incoming_data({
        0x00, 0x00,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xab,            // function code
        0x00, 0x01, 0x02 // data
    });

    auto thread = std::thread([&pal]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
    });

    pdu::GenericPDU response1, response2;
    std::chrono::steady_clock::time_point response1_time, response2_time;

    auto thread_req_1 = std::thread([&pal, &request1, &response1, &response1_time]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        response1 = pal.request_response(request1, std::chrono::milliseconds(1000));
        response1_time = std::chrono::steady_clock::now();
    });

    auto thread_req_2 = std::thread([&pal, &request2, &response2, &response2_time]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        response2 = pal.request_response(request2, std::chrono::milliseconds(1000));
        response2_time = std::chrono::steady_clock::now();
    });

    thread_req_1.join();
    thread_req_2.join();
    thread.join();

    ASSERT_EQ(response1.function_code, 0xab);
    ASSERT_EQ(response1.data.size(), 3);
    ASSERT_EQ(response1.data[0], 0x00);
    ASSERT_EQ(response1.data[1], 0x01);
    ASSERT_EQ(response1.data[2], 0x02);

    ASSERT_EQ(response2.function_code, 0xcd);
    ASSERT_EQ(response2.data.size(), 3);
    ASSERT_EQ(response2.data[0], 0x00);
    ASSERT_EQ(response2.data[1], 0x01);
    ASSERT_EQ(response2.data[2], 0x02);

    ASSERT_TRUE(response2_time < response1_time);
}

TEST(PDUCorrelationLayer, timeout) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport);
    PDUCorrelationLayer pal(protocol);

    pdu::GenericPDU request = pdu::GenericPDU(0xab, {0x01});

    ASSERT_THROW(pal.request_response(request, std::chrono::milliseconds(100)), std::runtime_error);
}

TEST(PDUCorrelationLayer, send_requestless) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport);
    PDUCorrelationLayer pal(protocol);

    pal.request_without_response(pdu::GenericPDU(0x01, {0x02}));

    auto written = transport->get_outgoing_data();

    ASSERT_EQ(written.size(), 9);
    // check pdu function code and data
    ASSERT_EQ(written[7], 0x01);
    ASSERT_EQ(written[8], 0x02);
}

TEST(PDUCorrelationLayer, on_pdu_callback) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    pal.set_on_pdu([](const pdu::GenericPDU& pdu) { return pdu::GenericPDU(pdu.function_code, {0x03, 0x04, 0x05}); });

    transport->add_incoming_data({
        0xde, 0xad,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xef,            // function code
        0x00, 0x01, 0x02 // data
    });

    pal.blocking_poll();

    auto written = transport->get_outgoing_data();

    ASSERT_EQ(written.size(), 11);
    ASSERT_EQ(written[0], 0xde);
    ASSERT_EQ(written[1], 0xad);

    ASSERT_EQ(written[7], 0xef);
    ASSERT_EQ(written[8], 0x03);
    ASSERT_EQ(written[9], 0x04);
    ASSERT_EQ(written[10], 0x05);
}

TEST(PDUCorrelationLayer, on_pdu_callback_ignores_request_response) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    std::uint8_t callback_calls = 0;

    pal.set_on_pdu([&callback_calls](const pdu::GenericPDU& pdu) {
        callback_calls++;
        return std::nullopt;
    });

    pdu::GenericPDU request = pdu::GenericPDU(0xef, {0x01});

    // answer
    transport->add_incoming_data({
        0x00, 0x00,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xef,            // function code
        0x00, 0x01, 0x02 // data
    });

    auto thread = std::thread([&pal]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
    });

    auto response = pal.request_response(request, std::chrono::milliseconds(1000));
    ASSERT_EQ(response.function_code, 0xef);
    ASSERT_EQ(response.data.size(), 3);
    ASSERT_EQ(response.data[0], 0x00);
    ASSERT_EQ(response.data[1], 0x01);
    ASSERT_EQ(response.data[2], 0x02);

    thread.join();

    ASSERT_EQ(callback_calls, 0);
}

TEST(PDUCorrelationLayer, given_no_pdu_callback_ignores_all_incoming) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    // some random packet
    transport->add_incoming_data({
        0x00, 0x00,      // transaction id
        0x00, 0x00,      // protocol id
        0x00, 0x05,      // length
        0x01,            // unit id
        0xef,            // function code
        0x00, 0x01, 0x02 // data
    });

    pal.blocking_poll();

    // we are still alive, nice
    ASSERT_TRUE(true);
}

TEST(PDUCorrelationLayer, error_response) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    transport->add_incoming_data({
        0x00, 0x00, // transaction id
        0x00, 0x00, // protocol id
        0x00, 0x03, // length
        0x01,       // unit id
        0x83,       // function code
        0x02        // exception code
    });

    auto thread = std::thread([&pal]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
    });

    auto request = pdu::GenericPDU(0x03, {0x02});
    auto response = pal.request_response(request, std::chrono::milliseconds(1000));
    ASSERT_EQ(response.function_code, 0x83);
    ASSERT_EQ(response.data.size(), 1);
    ASSERT_EQ(response.data[0], 0x02);

    thread.join();
}

TEST(PDUCorrelationLayer, error_response_different_function_code) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    PDUCorrelationLayer pal(protocol);

    int callback_calls = 0;
    pal.set_on_pdu([&callback_calls](const pdu::GenericPDU& pdu) {
        callback_calls++;
        return std::nullopt;
    });

    transport->add_incoming_data({
        0x00, 0x00, // transaction id
        0x00, 0x00, // protocol id
        0x00, 0x03, // length
        0x01,       // unit id
        0x83,       // function code
        0x02        // exception code
    });

    auto thread = std::thread([&pal]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        pal.blocking_poll();
    });

    auto request = pdu::GenericPDU(0x02, {0x02});
    ASSERT_THROW(pal.request_response(request, std::chrono::milliseconds(100)), std::runtime_error);

    thread.join();

    ASSERT_EQ(callback_calls, 1);
}

TEST(PDUCorrelationLayer, timeout_works_accurate) {
    auto transport = std::make_shared<DummyModbusTransport>();
    auto protocol = std::make_shared<ModbusTCPProtocol>(transport, 0x01, 0x0000);
    auto pal = std::make_shared<PDUCorrelationLayer>(protocol);

    auto thread = std::thread([&pal, &transport]() {
        // 4 random packets, after 10ms, 20ms, 30ms, 40ms
        for (std::uint8_t i = 0; i < 4; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            transport->add_incoming_data({
                0xff, i,    // transaction id
                0x00, 0x00, // protocol id
                0x00, 0x03, // length
                0x01,       // unit id
                0x05,       // function code
                0x02        // exception code
            });
            pal->blocking_poll();
        }
    });

    auto request = pdu::GenericPDU(0x01, {0x02});
    auto time_before = std::chrono::steady_clock::now();
    ASSERT_THROW(pal->request_response(request, std::chrono::milliseconds(50)), std::runtime_error);
    auto time_after = std::chrono::steady_clock::now();

    ASSERT_TRUE(time_after - time_before < std::chrono::milliseconds(60)); // 10ms tolerance

    thread.join();
}
