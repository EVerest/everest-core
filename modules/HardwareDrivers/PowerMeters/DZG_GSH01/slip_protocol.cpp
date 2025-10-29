// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "slip_protocol.hpp"

#include <cstring>
#include <endian.h>
#include <everest/crc/crc.hpp>
#include <everest/logging.hpp>
#include <fcntl.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/time.h>

#include <fmt/core.h>

namespace {
inline void remove_start_and_stop_frame(std::vector<std::uint8_t>& vec) {
    vec.erase(vec.begin());
    vec.pop_back();
}

inline bool is_message_crc_correct(std::vector<std::uint8_t>& vec) {
    auto crc_check = static_cast<std::uint16_t>((vec[vec.size() - 1] << 8) | vec[vec.size() - 2]);
    // remove CRC tail from vector
    for (std::uint8_t i = 0; i < 2; i++) {
        vec.pop_back();
    }
    auto crc_calc = calculate_xModem_crc16(vec);
    return crc_check == crc_calc;
}

inline void restore_special_characters(std::vector<std::uint8_t>& vec) {
    for (std::uint16_t j = 0; j < (vec.size() - 1);
         j++) { // can only go to vec.size() - 1 because two bytes will be checked
        if ((vec.at(j) == 0xDB) && (vec.at(j + 1) == 0xDC)) {
            vec.at(j) = 0xC0;
            vec.erase(vec.begin() + j + 1);
        } else if ((vec.at(j) == 0xDB) && (vec.at(j + 1) == 0xDD)) {
            vec.at(j) = 0xDB;
            vec.erase(vec.begin() + j + 1);
        }
    }
}
} // namespace

namespace slip_protocol {

std::vector<std::uint8_t> SlipProtocol::package_single(const std::uint8_t address, std::vector<std::uint8_t>& payload) {
    std::vector<std::uint8_t> vec{};

    // address to the front
    payload.insert(payload.begin(), address);

    // CRC16 to the end
    std::uint16_t crc = calculate_xModem_crc16(payload);
    payload.push_back(static_cast<std::uint8_t>(crc & 0x00FF));        // LSB CRC16
    payload.push_back(static_cast<std::uint8_t>((crc >> 8) & 0x00FF)); // MSB CRC16

    // replacement of special characters (including address and CRC)
    for (auto payload_byte : payload) {
        if (payload_byte == 0xC0) { // check for and replace special char 0xC0
            vec.push_back(0xDB);
            vec.push_back(0xDC);
        } else if (payload_byte == 0xDB) { // check for and replace special char 0xDB
            vec.push_back(0xDB);
            vec.push_back(0xDD);
        } else { // otherwise just use normal input
            vec.push_back(payload_byte);
        }
    }

    // add start frame to front (can be done only after special characters have been replaced)
    vec.insert(vec.begin(), SLIP_START_END_FRAME);

    // end frame
    vec.push_back(SLIP_START_END_FRAME);

    return vec;
}

std::vector<std::uint8_t> SlipProtocol::package_multi(const std::uint8_t address,
                                                      const std::vector<std::vector<std::uint8_t>>& multi_payload) {
    std::vector<std::uint8_t> payload{};

    // concatenate requests
    for (auto multi_payload_part : multi_payload) {
        payload.insert(payload.end(), multi_payload_part.begin(), multi_payload_part.end());
    }

    // ...and return as one long request
    return package_single(address, payload);
}

SlipReturnStatus SlipProtocol::unpack(std::vector<std::uint8_t>& message, std::uint8_t listen_to_address) {
    SlipReturnStatus retval = SlipReturnStatus::SLIP_ERROR_UNINITIALIZED;
    std::uint8_t message_start_end_frame_counter = 0;

    if (message.size() < 1) {
        return SlipReturnStatus::SLIP_ERROR_SIZE_ERROR;
    }

    // check if first element is SLIP_START_END_FRAME and if not, drop first item(s) until start frame is first
    std::uint8_t i = 0;
    while (i < message.size()) {
        if (message.at(i) == SLIP_START_END_FRAME)
            break;
        i++;
    }
    if (i > 0) {
        std::uint8_t j = 0;
        while (message.size() > 0) {
            if (j == i)
                break;
            message.erase(message.begin());
            j++;
        }
    }

    // count number of SLIP_START_END_FRAMEs to check for multiple (or broken) messages
    for (std::uint16_t n = 0; n < message.size(); n++) {
        if (message.at(n) == SLIP_START_END_FRAME) {
            message_start_end_frame_counter++;
        }
    }

    if (message_start_end_frame_counter != 2) { // unexpected/broken message -OR- multiple messages received

        // split message vector into sub-vectors by delimiter (SLIP_START_END_FRAME)
        std::vector<std::vector<std::uint8_t>> sub_messages{};
        std::vector<std::uint8_t> current_sub_message{};
        for (std::uint16_t n = 0; n < message.size(); n++) {
            if (message.at(n) == SLIP_START_END_FRAME) {
                if (current_sub_message.size() > 0) {
                    if (listen_to_address == 0xFF) {
                        // listen_to_address is broadcast address: listen to all messages
                        sub_messages.push_back(current_sub_message);
                    } else {
                        // dedicated client address selected, only listen to this address
                        if (current_sub_message.at(0) == listen_to_address) {
                            sub_messages.push_back(current_sub_message);
                        }
                    }
                    current_sub_message.clear();
                } else {
                    // intentionally do nothing on empty sub_message
                }
            } else {
                current_sub_message.push_back(message.at(n));
            }
        }

        // from here on, we have all message parts as elements in sub_messages
        for (auto sub_message : sub_messages) {
            restore_special_characters(sub_message);
            // check all sub-messages' CRC and only process on match
            if (is_message_crc_correct(sub_message)) {
                // on correct CRC
                message_queue.push_back(sub_message);
                message_counter++;
                if (retval == SlipReturnStatus::SLIP_ERROR_UNINITIALIZED) { // only set SLIP_OK if no other error
                    retval = SlipReturnStatus::SLIP_OK;
                }
            } else {
                retval = SlipReturnStatus::SLIP_ERROR_MALFORMED;
                EVLOG_error << "Malformed message received!";
            }
        }
    } else { // single message received

        if (message.size() > 3) {
            // only process messages for correct client or if listen_to_address is broadcast address
            if ((message.at(1) == listen_to_address) || (0xFF == listen_to_address)) {
                // check if last element is SLIP_START_END_FRAME and if not, reduce message size
                while (message.at(message.size() - 1) != SLIP_START_END_FRAME) {
                    if (message.size() <= 3)
                        break;
                    message.pop_back();
                }

                remove_start_and_stop_frame(message);
                // check for special characters and restore to original contents
                restore_special_characters(message);
                if (is_message_crc_correct(message)) {
                    message_queue.push_back(message);
                    message_counter++;
                    retval = SlipReturnStatus::SLIP_OK;
                } else {
                    retval = SlipReturnStatus::SLIP_ERROR_CRC_MISMATCH;
                    EVLOG_error << "CRC mismatch!";
                }
            } else {
                retval = SlipReturnStatus::SLIP_OK;
            }
        } else {
            retval = SlipReturnStatus::SLIP_ERROR_SIZE_ERROR;
            EVLOG_error << "Message broken: too short!";
        }
    }
    return retval;
}

std::vector<std::uint8_t> SlipProtocol::retrieve_single_message() {
    if (message_queue.size() > 0) {
        message_counter--;
        auto ret_vec = message_queue.back();
        message_queue.pop_back();
        return ret_vec;
    }
    return {};
}

} // namespace slip_protocol
