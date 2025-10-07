// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace everest::lib::io::udp {

/**
 * @struct udp_payload
 * Dataset for UDP with maximum size of 64kBit
 */
struct udp_payload {
    /**
     * @brief udp_payload can be default constructed
     */
    udp_payload() = default;
    /**
     * @brief Create message from a std::string
     * @param[in] msg Payload as string
     */
    udp_payload(std::string const& msg);
    /**
     * @brief Create message from a const char*
     * @param[in] msg Payload as string. Needs to be null terminated
     */
    udp_payload(const char* msg);

    /**
     * @brief Compare to other object
     * @details Two objects are equal, if the buffers hold the same data
     * @param[in] other object to compare to
     */
    bool operator==(udp_payload const& other) const;

    /**
     * @brief Size of the message
     * @return Size
     */
    size_t size() const;

    /**
     * @brief Replace content
     * @details The replaces the internal buffer with a new message
     * @param[in] msg New message
     * @return True on succes, False if message is too long.
     */
    bool set_message(std::string const& msg);

    /**
     * @brief Replace content
     * @details The replaces the internal buffer with a new message
     * @param[in] buffer Pointer to new message
     * @param[in] size Bytes to copy
     * @return True on success, False is size is too big
     */
    bool set_message(void const* buffer, size_t size);
    /**
     * @brief Buffer holding all information
     */
    std::vector<uint8_t> buffer;

    /**
     * @var max_size
     * @brief The maximum size for UDP is limited to 64kBit
     */
    static constexpr size_t max_size = 64 * 1024;
};

} // namespace everest::lib::io::udp
