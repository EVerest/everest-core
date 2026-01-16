// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_UTILS_HPP
#define POWERMETER_UTILS_HPP

#include <cstdint>
#include <cctype>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// #include "base64.hpp"
#include "transport.hpp"
#include <generated/interfaces/powermeter/Implementation.hpp>

/**
 * @brief Utility functions for converting Modbus data to various types
 *
 * These functions handle byte order conversion from Modbus (big-endian) format
 * to native integer types. Modbus transmits data in big-endian format where
 * the most significant byte comes first.
 */

namespace modbus_utils {

// Strong type wrappers to prevent parameter swapping
struct ByteOffset {
    explicit ByteOffset(transport::DataVector::size_type v) : value(v) {
    }
    operator transport::DataVector::size_type() const {
        return value;
    }

private:
    transport::DataVector::size_type value;
};

struct ByteLength {
    explicit ByteLength(transport::DataVector::size_type v) : value(v) {
    }
    operator transport::DataVector::size_type() const {
        return value;
    }

private:
    transport::DataVector::size_type value;
};

/**
 * @brief Convert 4 bytes from Modbus data to uint32_t
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 32-bit unsigned integer
 * @note According to EM580 Modbus spec: byte order within word is MSB->LSB,
 *       but for INT32/UINT32/UINT64, word order is LSW->MSW.
 *       So bytes are arranged as: [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
 *       which becomes: MSW_MSB MSW_LSB LSW_MSB LSW_LSB
 */
inline uint32_t to_uint32(const transport::DataVector& data, ByteOffset offset) {
    // Original byte order: [byte0, byte1, byte2, byte3] = [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
    // Convert to: MSW_MSB MSW_LSB LSW_MSB LSW_LSB = byte2<<24 | byte3<<16 | byte0<<8 | byte1
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    return static_cast<uint32_t>(data[off] << 24 | data[off + 1] << 16 | data[off + 2] << 8 | data[off + 3]);
}

/**
 * @brief Convert 4 bytes from Modbus data to int32_t
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 32-bit signed integer
 * @note According to EM580 Modbus spec: byte order within word is MSB->LSB,
 *       but for INT32/UINT32/UINT64, word order is LSW->MSW.
 *       So bytes are arranged as: [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
 *       which becomes: MSW_MSB MSW_LSB LSW_MSB LSW_LSB
 */
inline int32_t to_int32(const transport::DataVector& data, ByteOffset offset) {
    // Original byte order: [byte0, byte1, byte2, byte3] = [LSW_MSB, LSW_LSB, MSW_MSB, MSW_LSB]
    // Convert to: MSW_MSB MSW_LSB LSW_MSB LSW_LSB = byte2<<24 | byte3<<16 | byte0<<8 | byte1
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    return static_cast<int32_t>(data[off + 2] << 24 | data[off + 3] << 16 | data[off] << 8 | data[off + 1]);
}

/**
 * @brief Convert 2 bytes from Modbus data to uint16_t (big-endian)
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 16-bit unsigned integer
 */
inline uint16_t to_uint16(const transport::DataVector& data, ByteOffset offset) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    return static_cast<uint16_t>(data[off] << 8 | data[off + 1]);
}

/**
 * @brief Convert 2 bytes from Modbus data to int16_t (big-endian)
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @return The converted 16-bit signed integer
 */
inline int16_t to_int16(const transport::DataVector& data, ByteOffset offset) {
    uint16_t raw = to_uint16(data, offset);
    return static_cast<int16_t>(raw);
}

/**
 * @brief Convert a range of bytes to a hexadecimal string representation
 * @param data The Modbus data vector
 * @param offset Byte offset into the data vector
 * @param length Number of bytes to convert
 * @return Hexadecimal string (uppercase, no separators)
 */
inline std::string to_hex_string(const transport::DataVector& data, ByteOffset offset, ByteLength length) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    const auto len = static_cast<transport::DataVector::size_type>(length);
    std::stringstream ss;
    for (std::size_t index = 0; index < len; ++index) {
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[off + index]);
    }
    return ss.str();
}

} // namespace modbus_utils

/**
 * @brief Utility functions for converting OCMF enum types to Modbus register values
 */
namespace ocmf_utils {

inline bool is_uuid36(const std::string& s) {
    if (s.size() != 36) {
        return false;
    }
    for (std::size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (c != '-') {
                return false;
            }
            continue;
        }
        if (!std::isxdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

/// Extract the transaction id (UUID) from an OCMF record by parsing the JSON `"TT"` field
/// and taking the UUID after the last "<=>" marker.
///
/// Expected input looks like: `OCMF|{...,"TT":"...<=>UUID",...}|{...}`
inline std::optional<std::string> extract_transaction_id_from_ocmf_record(const std::string& ocmf_record) {
    // Find JSON key "TT"
    const std::string key = "\"TT\"";
    std::size_t key_pos = ocmf_record.find(key);
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    // Find ':' after the key and the opening quote of the JSON string value.
    std::size_t colon_pos = ocmf_record.find(':', key_pos + key.size());
    if (colon_pos == std::string::npos) {
        return std::nullopt;
    }

    std::size_t value_start = ocmf_record.find('"', colon_pos + 1);
    if (value_start == std::string::npos) {
        return std::nullopt;
    }
    ++value_start; // move past opening quote

    // Parse a JSON string (escape-aware) until the closing unescaped quote.
    std::string tt_value;
    tt_value.reserve(128);
    bool escaped = false;
    for (std::size_t i = value_start; i < ocmf_record.size(); ++i) {
        const char c = ocmf_record[i];
        if (escaped) {
            // Keep the escaped character as-is; for our purposes we only need to find "<=>".
            tt_value.push_back(c);
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"') {
            // end of JSON string
            break;
        }
        tt_value.push_back(c);
    }

    const std::string marker = "<=>";
    const std::size_t marker_pos = tt_value.rfind(marker);
    if (marker_pos == std::string::npos) {
        return std::nullopt;
    }

    std::string tail = tt_value.substr(marker_pos + marker.size());
    // Trim whitespace
    while (!tail.empty() && std::isspace(static_cast<unsigned char>(tail.front()))) {
        tail.erase(tail.begin());
    }
    while (!tail.empty() && std::isspace(static_cast<unsigned char>(tail.back()))) {
        tail.pop_back();
    }

    // Try to find a UUID inside the tail (prefer the last match, in case of extra suffix/prefix).
    std::optional<std::string> last_uuid;
    if (tail.size() >= 36) {
        for (std::size_t i = 0; i + 36 <= tail.size(); ++i) {
            const std::string candidate = tail.substr(i, 36);
            if (is_uuid36(candidate)) {
                last_uuid = candidate;
            }
        }
    }
    return last_uuid;
}

/**
 * @brief Convert OCMFIdentificationFlags enum to numeric value for Modbus register
 * @param flag The OCMF identification flag enum value
 * @return The numeric value to write to the Modbus register
 */
inline uint16_t flag_to_value(types::powermeter::OCMFIdentificationFlags flag) {
    switch (flag) {
    case types::powermeter::OCMFIdentificationFlags::RFID_NONE:
        return 0;
    case types::powermeter::OCMFIdentificationFlags::RFID_PLAIN:
        return 1;
    case types::powermeter::OCMFIdentificationFlags::RFID_RELATED:
        return 2;
    case types::powermeter::OCMFIdentificationFlags::RFID_PSK:
        return 3;
    case types::powermeter::OCMFIdentificationFlags::OCPP_NONE:
        return 0;
    case types::powermeter::OCMFIdentificationFlags::OCPP_RS:
        return 1;
    case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH:
        return 2;
    case types::powermeter::OCMFIdentificationFlags::OCPP_RS_TLS:
        return 3;
    case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH_TLS:
        return 4;
    case types::powermeter::OCMFIdentificationFlags::OCPP_CACHE:
        return 5;
    case types::powermeter::OCMFIdentificationFlags::OCPP_WHITELIST:
        return 6;
    case types::powermeter::OCMFIdentificationFlags::OCPP_CERTIFIED:
        return 7;
    case types::powermeter::OCMFIdentificationFlags::ISO15118_NONE:
        return 0;
    case types::powermeter::OCMFIdentificationFlags::ISO15118_PNC:
        return 1;
    case types::powermeter::OCMFIdentificationFlags::PLMN_NONE:
        return 0;
    case types::powermeter::OCMFIdentificationFlags::PLMN_RING:
        return 1;
    case types::powermeter::OCMFIdentificationFlags::PLMN_SMS:
        return 2;
    }
    return 0;
}

/**
 * @brief Convert OCMFIdentificationLevel enum to numeric value for Modbus register
 * @param level The OCMF identification level enum value
 * @return The numeric value to write to the Modbus register
 */
inline uint16_t level_to_value(types::powermeter::OCMFIdentificationLevel level) {
    switch (level) {
    case types::powermeter::OCMFIdentificationLevel::NONE:
        return 0;
    case types::powermeter::OCMFIdentificationLevel::HEARSAY:
        return 1;
    case types::powermeter::OCMFIdentificationLevel::TRUSTED:
        return 2;
    case types::powermeter::OCMFIdentificationLevel::VERIFIED:
        return 3;
    case types::powermeter::OCMFIdentificationLevel::CERTIFIED:
        return 4;
    case types::powermeter::OCMFIdentificationLevel::SECURE:
        return 5;
    case types::powermeter::OCMFIdentificationLevel::MISMATCH:
        return 6;
    case types::powermeter::OCMFIdentificationLevel::INVALID:
        return 7;
    case types::powermeter::OCMFIdentificationLevel::OUTDATED:
        return 8;
    case types::powermeter::OCMFIdentificationLevel::UNKNOWN:
        return 9;
    }
    return 0;
}

/**
 * @brief Convert OCMFIdentificationType enum to numeric value for Modbus register
 * @param type The OCMF identification type enum value
 * @return The numeric value to write to the Modbus register
 */
inline uint16_t type_to_value(types::powermeter::OCMFIdentificationType type) {
    switch (type) {
    case types::powermeter::OCMFIdentificationType::NONE:
        return 0;
    case types::powermeter::OCMFIdentificationType::DENIED:
        return 1;
    case types::powermeter::OCMFIdentificationType::UNDEFINED:
        return 2;
    case types::powermeter::OCMFIdentificationType::ISO14443:
        return 10;
    case types::powermeter::OCMFIdentificationType::ISO15693:
        return 11;
    case types::powermeter::OCMFIdentificationType::EMAID:
        return 20;
    case types::powermeter::OCMFIdentificationType::EVCCID:
        return 21;
    case types::powermeter::OCMFIdentificationType::EVCOID:
        return 30;
    case types::powermeter::OCMFIdentificationType::ISO7812:
        return 40;
    case types::powermeter::OCMFIdentificationType::CARD_TXN_NR:
        return 50;
    case types::powermeter::OCMFIdentificationType::CENTRAL:
        return 60;
    case types::powermeter::OCMFIdentificationType::CENTRAL_1:
        return 61;
    case types::powermeter::OCMFIdentificationType::CENTRAL_2:
        return 62;
    case types::powermeter::OCMFIdentificationType::LOCAL:
        return 70;
    case types::powermeter::OCMFIdentificationType::LOCAL_1:
        return 71;
    case types::powermeter::OCMFIdentificationType::LOCAL_2:
        return 72;
    case types::powermeter::OCMFIdentificationType::PHONE_NUMBER:
        return 80;
    case types::powermeter::OCMFIdentificationType::KEY_CODE:
        return 90;
    }
    return 0;
}

} // namespace ocmf_utils

#endif // POWERMETER_UTILS_HPP
