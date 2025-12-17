// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>
#include <sys/socket.h>

#include <modbus-server/transport.hpp>
#include <thread>

TEST(SocketTransport, read_works) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    std::vector<std::uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    err = write(fds[1], data.data(), data.size());
    ASSERT_EQ(err, data.size());

    std::vector<std::uint8_t> read_data = transport.read_bytes(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        ASSERT_EQ(data[i], read_data[i]);
    }

    close(fds[1]);
    close(fds[0]);
}

TEST(SocketTransport, fragmented_read_works) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    std::vector<std::uint8_t> data = {0x01, 0x02, 0x03, 0x04};

    auto thread = std::thread([&fds, &data]() {
        int err;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        err = write(fds[1], data.data(), 2);
        ASSERT_EQ(err, 2);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        err = write(fds[1], data.data() + 2, 2);
        ASSERT_EQ(err, 2);
    });

    std::vector<std::uint8_t> read_data = transport.read_bytes(data.size());
    ASSERT_EQ(read_data.size(), data.size());
    for (size_t i = 0; i < data.size(); i++) {
        ASSERT_EQ(data[i], read_data[i]);
    }

    close(fds[1]);
    close(fds[0]);

    thread.join();
}

TEST(SocketTransport, write_works) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    std::vector<std::uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    transport.write_bytes(data);

    std::vector<std::uint8_t> read_data(data.size());
    err = read(fds[1], read_data.data(), read_data.size());
    ASSERT_EQ(err, data.size());

    for (size_t i = 0; i < data.size(); i++) {
        ASSERT_EQ(data[i], read_data[i]);
    }
}

TEST(SocketTransport, closed_socket_read_throws_err) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    close(fds[1]);

    ASSERT_THROW(transport.read_bytes(1), modbus_server::transport_exceptions::ConnectionClosedException);

    close(fds[0]);
}

TEST(SocketTransport, not_existing_socket_read_throws_err) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    close(fds[1]);
    close(fds[0]);

    // socket closed on both ends should throw runtime_error
    ASSERT_THROW(transport.read_bytes(1), std::runtime_error);
}

TEST(SocketTransport, closed_socket_write_throws_err) {
    int fds[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    ASSERT_EQ(err, 0);

    modbus_server::ModbusSocketTransport transport(fds[0]);

    close(fds[0]);
    close(fds[1]);

    ASSERT_THROW(transport.write_bytes({0x01}), std::runtime_error);
}
