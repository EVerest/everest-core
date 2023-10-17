// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for the SLIP serial protocol
*/
#ifndef SLIP_PROTOCOL
#define SLIP_PROTOCOL

#include <everest/logging.hpp>
#include <optional>
#include <stdint.h>
#include "ld-ev.hpp"

#include "crc16.hpp"

namespace slip_protocol {

constexpr int SLIP_START_END_FRAME = 0xC0;
constexpr int SLIP_BROADCAST_ADDR = 0xFF;

constexpr uint16_t SLIP_SIZE_ON_ERROR = 1;

constexpr int8_t SLIP_ERROR_SIZE_ERROR = -1;
constexpr int8_t SLIP_ERROR_MALFORMED = -2;
constexpr int8_t SLIP_ERROR_CRC_MISMATCH = -3;

enum class SlipReturnStatus : std::int8_t {
    SLIP_ERROR_CRC_MISMATCH = -3,
    SLIP_ERROR_MALFORMED = -2,
    SLIP_ERROR_SIZE_ERROR = -1,
    SLIP_OK = 0,
    SLIP_ERROR_UNINITIALIZED = 1
};

class SlipProtocol {

public:
    SlipProtocol() = default;
    ~SlipProtocol() = default;

    std::vector<uint8_t> package_single(const uint8_t address, const std::vector<uint8_t>& payload);
    std::vector<uint8_t> package_multi(const uint8_t address, const std::vector<std::vector<uint8_t>>& multi_payload);

    SlipReturnStatus unpack(std::vector<uint8_t>& message, uint8_t listen_to_address);
    uint8_t get_message_counter();
    std::vector<uint8_t> retrieve_single_message();

private:
    std::vector<std::vector<uint8_t>> message_queue{};
    uint8_t message_counter{0};
    
};

} // namespace slip_protocol
#endif // SLIP_PROTOCOL
