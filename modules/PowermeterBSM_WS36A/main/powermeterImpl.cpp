// SPDX-License-Identifier: Apache-2.0
// Copyright Qwello GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

#include <GenericPowermeter/serialization.hpp>
#include <array>
#include <chrono>
#include <ctime>
#include <fmt/core.h>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <math.h> /* ceil */
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

int16_t get_utc_offset() {
    std::time_t t = std::time(nullptr);
    const auto gmt_time = std::mktime(std::gmtime(&t));
    const auto local_time = std::mktime(std::localtime(&t));

    const auto diff_sec = std::difftime(local_time, gmt_time);
    return static_cast<int16_t>(diff_sec / 60);
}

constexpr module::utils::Register SIGNATURE_STATUS_START{41286, 1};
constexpr module::utils::Register SIGNATURE_STATUS_STOP{41540, 1};
constexpr module::utils::Register SIGNATURE_START{43295, 496};
constexpr module::utils::Register SIGNATURE_STOP{43795, 496};
constexpr module::utils::Register UNIX_TIME{40260, 2};
// When setting the time we have to set it with the offset - this is not clearly
// documented in the manual.
constexpr module::utils::Register UNIX_TIME_WITH_UTC_OFFSET{40260, 3};
constexpr module::utils::Register UTC_OFFSET{40262, 1};
constexpr module::utils::Register META_DATA_1{40279, 70};

// Public key related fields
constexpr module::utils::Register PUBLIC_KEY{40452, 32};
constexpr module::utils::Register PUBLIC_KEY_LEN{40451, 1};

/// @brief The status when reading the
constexpr uint16_t SIGNATURE_STATUS_DONE = 2;

/// @brief transforms string into the hex string
std::string to_hex(const std::string& input) {
    std::stringstream hex_stream;
    for (unsigned char c : input) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return hex_stream.str();
}

/// @brief returns a random id consisting of the current timestamp padded with
/// a random hex string until it reaches 20 chars.
std::string get_random_id() {
    // Get the current seconds as stirng.
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto secs = duration_cast<seconds>(now.time_since_epoch()).count();
    auto out = std::to_string(secs);

    // Pad them with a random hex number until we have reached the expected
    // length.
    constexpr size_t length = 20;
    out.reserve(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> uniform_dist(0, std::numeric_limits<size_t>::max());
    // If we have a machine where we can't generate `length` chars at once
    // we loop.
    while (out.size() < length) {
        std::stringstream sst;
        sst << std::hex << uniform_dist(gen);
        const auto str = sst.str();
        const auto size = std::min(length - out.size(), str.size());
        for (auto ii = 0; ii != size; ++ii)
            out.push_back(str.at(ii));
    }

    return out;
}

} // namespace

namespace module {
namespace main {

using namespace module::utils;
using namespace types::powermeter;
using namespace types::serial_comm_hub_requests;

void powermeterImpl::init() {
}

void powermeterImpl::ready() {
    // Read the length of the register in bytes
    EVLOG_info << "Reading public key";
    const auto public_key_length = read_register<uint16_t>(PUBLIC_KEY_LEN);
    EVLOG_info << "Length of public key in bytes: " << public_key_length;
    uint32_t reg_num = (public_key_length + 1) / 2;
    EVLOG_info << "Length of public key in registers: " << reg_num;
    const auto str = to_hex(read_register<std::string>(module::utils::Register{PUBLIC_KEY.start_register, reg_num})
                                .substr(0, public_key_length));
    EVLOG_info << "Publishing the public key: " << this->PUBLIC_KEY_HEADER + str;
    this->publish_public_key(this->PUBLIC_KEY_HEADER + str);
}

TransactionStartResponse powermeterImpl::handle_start_transaction_impl(const TransactionReq& value) {
    EVLOG_info << "Starting transaction";

    // Set the time.
    const auto time = read_register<uint32_t>(UNIX_TIME);
    EVLOG_info << "Current time " << time;

    write_register(UNIX_TIME_WITH_UTC_OFFSET, std::chrono::system_clock::now());

    const auto utc_offset = get_utc_offset();
    EVLOG_info << "The utc offset " << utc_offset;
    write_register(UTC_OFFSET, utc_offset);

    // Set the meta-data.
    const auto transaction_id = get_random_id();
    std::string data = fmt::format("{} {}", value.evse_id, transaction_id);
    write_register(META_DATA_1, data);

    // Wait for the signature to finish.
    while (read_register<uint16_t>(SIGNATURE_STATUS_START) == SIGNATURE_STATUS_DONE) {
    }
    write_register(SIGNATURE_STATUS_START, SIGNATURE_STATUS_DONE);

    // Read the start signature.
    const auto response = read_register<std::string>(SIGNATURE_START);
    EVLOG_info << "Started the transaction with " << response;

    TransactionStartResponse out{TransactionRequestStatus::OK};
    out.ocmf = response;
    return out;
}

TransactionStopResponse powermeterImpl::handle_stop_transaction_impl(const std::string& _) {
    EVLOG_info << "Stopping transaction";

    // Wait for the signature to finish.
    while (read_register<uint16_t>(SIGNATURE_STATUS_STOP) == SIGNATURE_STATUS_DONE) {
    }
    write_register(SIGNATURE_STATUS_STOP, SIGNATURE_STATUS_DONE);

    // Read the signature.
    const auto response = read_register<std::string>(SIGNATURE_STOP);
    EVLOG_info << "Stopped the transaction with " << response;

    return {TransactionRequestStatus::OK, response};
}

TransactionStartResponse powermeterImpl::handle_start_transaction(TransactionReq& value) {
    try {
        return handle_start_transaction_impl(value);
    } catch (const std::exception& _ex) {
        EVLOG_error << "Failed to start the transaction: " << _ex.what();
        // TODO(ddo) Ignore the errors for now to keep going.
        return {TransactionRequestStatus::OK, _ex.what()};
    }
}

TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    try {
        return handle_stop_transaction_impl(transaction_id);
    } catch (const std::exception& _ex) {
        EVLOG_error << "Failed to stop the transaction: " << _ex.what();
        return {TransactionRequestStatus::UNEXPECTED_ERROR, {}, _ex.what()};
    }
}

template <typename Output> Output powermeterImpl::read_register(const Register& _register) const {
    const auto read_register_impl = [this](const Register& _register) -> types::serial_comm_hub_requests::Result {
        switch (_register.type) {
        case RegisterType::HOLDING: {
            return mod->r_serial_comm_hub->call_modbus_read_holding_registers(
                config.powermeter_device_id, _register.start_register - config.modbus_base_address,
                _register.num_registers);
        }
        case RegisterType::INPUT: {
            return mod->r_serial_comm_hub->call_modbus_read_input_registers(
                config.powermeter_device_id, _register.start_register - config.modbus_base_address,
                _register.num_registers);
        }
        default:
            return {};
        }
    };

    const auto response = read_register_impl(_register);
    if (response.status_code != StatusCodeEnum::Success)
        throw std::runtime_error("Failed to send the message");
    return deserialize<Output>(response);
}

template <typename Input> void powermeterImpl::write_register(const Register& register_data, const Input& data) const {

    // Serialize the data into a vector.
    auto ser_data = serialize(data);

    // Pad the data with zeros if necessary.
    ser_data.data.resize(static_cast<size_t>(register_data.num_registers), int32_t{0});

    // Send the data to the modbus.
    const auto res = mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
        config.powermeter_device_id, register_data.start_register, ser_data);

    // Check the result.
    if (res != StatusCodeEnum::Success)
        throw std::runtime_error("Failed to write");
}

} // namespace main
} // namespace module
