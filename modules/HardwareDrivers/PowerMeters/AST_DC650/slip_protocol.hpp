// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for the SLIP serial protocol
*/
#ifndef SLIP_PROTOCOL
#define SLIP_PROTOCOL

#include <cstdint>
#include <everest/crc/crc.hpp>
#include <everest/logging.hpp>

namespace slip_protocol {

constexpr int SLIP_START_END_FRAME = 0xC0;
constexpr int SLIP_BROADCAST_ADDR = 0xFF;

constexpr std::uint16_t SLIP_SIZE_ON_ERROR = 1;

constexpr std::int8_t SLIP_ERROR_SIZE_ERROR = -1;
constexpr std::int8_t SLIP_ERROR_MALFORMED = -2;
constexpr std::int8_t SLIP_ERROR_CRC_MISMATCH = -3;

enum class SlipReturnStatus : std::int8_t {
    SLIP_ERROR_CRC_MISMATCH = -3,
    SLIP_ERROR_MALFORMED = -2,
    SLIP_ERROR_SIZE_ERROR = -1,
    SLIP_OK = 0,
    SLIP_ERROR_UNINITIALIZED = 1
};

class SlipProtocol {

public:
    std::vector<std::uint8_t> package_single(std::uint8_t address, const std::vector<std::uint8_t>& payload);
    std::vector<std::uint8_t> package_multi(std::uint8_t address,
                                            const std::vector<std::vector<std::uint8_t>>& multi_payload);

    SlipReturnStatus unpack(std::vector<std::uint8_t>& message, std::uint8_t listen_to_address);
    auto get_message_counter() const {
        return message_counter;
    }
    std::vector<std::uint8_t> retrieve_single_message();

private:
    std::vector<std::vector<std::uint8_t>> message_queue;
    std::uint8_t message_counter{0};
};

} // namespace slip_protocol
#endif // SLIP_PROTOCOL
