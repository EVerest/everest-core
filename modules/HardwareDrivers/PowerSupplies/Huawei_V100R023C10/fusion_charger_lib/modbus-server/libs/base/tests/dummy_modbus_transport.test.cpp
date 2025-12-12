// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "dummy_modbus_transport.hpp"

#include <gtest/gtest.h>

#include <modbus-server/transport.hpp>
#include <modbus-server/transport_protocol.hpp>

TEST(DummyModbusTransport, dummy_works) {
    DummyModbusTransport transport;
    transport.add_incoming_data({0x01, 0x02, 0x03});
    auto read = transport.read_bytes(3);
    ASSERT_EQ(read.size(), 3);
    ASSERT_EQ(read[0], 0x01);
    ASSERT_EQ(read[1], 0x02);
    ASSERT_EQ(read[2], 0x03);

    transport.write_bytes({0x05, 0x06});

    auto write = transport.get_outgoing_data();
    ASSERT_EQ(write.size(), 2);
    ASSERT_EQ(write[0], 0x05);
    ASSERT_EQ(write[1], 0x06);
}

TEST(DummyModbusTransport, read_bytes_empty) {
    DummyModbusTransport transport;
    ASSERT_THROW(transport.read_bytes(1), std::runtime_error);
}
