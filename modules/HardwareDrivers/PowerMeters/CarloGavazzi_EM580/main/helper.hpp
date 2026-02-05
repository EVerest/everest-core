// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef CARLO_GAVAZZI_EM580_HELPER_HPP
#define CARLO_GAVAZZI_EM580_HELPER_HPP

#include <array>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <everest/logging.hpp>

#include "transport.hpp"
#include <generated/interfaces/powermeter/Implementation.hpp>

namespace em580 {
namespace registers {

constexpr std::int32_t MODBUS_BASE_ADDRESS = 300001;

constexpr std::int32_t MODBUS_SIGNATURE_TYPE_ADDRESS = 309472; // 24FFh: Signature type (UINT16)
constexpr std::int32_t MODBUS_PUBLIC_KEY_ADDRESS = 309473;     // 2500h: Public key (UINT16[130])
// DER formatted public key (Table 4.20/4.21), mandatory to read whole block
// from 2600h.
constexpr std::int32_t MODBUS_PUBLIC_KEY_DER_ADDRESS = 309729;     // 2600h: Public key DER (read-only)
constexpr std::uint16_t MODBUS_PUBLIC_KEY_DER_WORD_COUNT_256 = 46; // 2600h..262Dh (92 bytes, DER length 0x5A + 2)
constexpr std::uint16_t MODBUS_PUBLIC_KEY_DER_WORD_COUNT_384 = 62; // 2600h..263Dh (124 bytes, DER length 0x7A + 2)

constexpr std::int32_t MODBUS_SIGNED_MAP_ADDRESS = 302049;
constexpr std::int32_t MODBUS_SIGNED_MAP_SIGNATURE_ADDRESS = 302126;

constexpr std::int32_t MODBUS_REAL_TIME_VALUES_ADDRESS = 300001;
constexpr std::uint16_t MODBUS_REAL_TIME_VALUES_COUNT = 80; // Registers 300001-300080 (0x50 = 80 words)

constexpr std::int32_t MODBUS_TEMPERATURE_ADDRESS = 300776; // Internal Temperature

constexpr std::int32_t MODBUS_FIRMWARE_MEASURE_MODULE_ADDRESS = 300771; // Measure module firmware version/revision
constexpr std::int32_t MODBUS_FIRMWARE_COMMUNICATION_MODULE_ADDRESS =
    300772;                                                         // Communication module firmware version/revision
constexpr std::int32_t MODBUS_SERIAL_NUMBER_START_ADDRESS = 320481; // Serial number (7 registers: 320481-320487)
constexpr std::uint16_t MODBUS_SERIAL_NUMBER_REGISTER_COUNT = 7;    // 7 UINT16 registers = 14 bytes
constexpr std::int32_t MODBUS_PRODUCTION_YEAR_ADDRESS = 320488;     // Production year (1 UINT16 register)

// Device state register (Table 4.30, Section 4.3.6)
constexpr std::int32_t MODBUS_DEVICE_STATE_ADDRESS = 320499; // 5012h: Device state (UINT16 bitfield)

// Time synchronization registers
constexpr std::int32_t MODBUS_UTC_TIMESTAMP_ADDRESS = 328723;   // UTC Timestamp for synchronization (INT64, 4 words)
constexpr std::int32_t MODBUS_TIMEZONE_OFFSET_ADDRESS = 328722; // Local time delta in minutes (INT16, 1 word)

// OCMF Transaction registers (Table 4.34)
constexpr std::int32_t MODBUS_OCMF_IDENTIFICATION_STATUS_ADDRESS = 328673; // 7000h: OCMF Ident. Status (UINT16)
constexpr std::int32_t MODBUS_OCMF_IDENTIFICATION_LEVEL_ADDRESS = 328674;  // 7001h: OCMF Ident. Level (UINT16)
constexpr std::int32_t MODBUS_OCMF_IDENTIFICATION_FLAGS_START_ADDRESS =
    328675;                                                              // 7002h: OCMF Ident. Flags 1-4 (4 UINT16)
constexpr std::uint16_t MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT = 4;      // 4 flags
constexpr std::int32_t MODBUS_OCMF_IDENTIFICATION_TYPE_ADDRESS = 328679; // 7006h: OCMF Ident. Type (UINT16)
constexpr std::int32_t MODBUS_OCMF_IDENTIFICATION_DATA_START_ADDRESS =
    328680; // 7007h: OCMF Ident. Data (CHAR[40] = 20 words)
constexpr std::uint16_t MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT = 20; // 40 bytes = 20 words
constexpr std::int32_t MODBUS_OCMF_CHARGING_POINT_ID_TYPE_ADDRESS =
    328700; // 701Bh: OCMF Charging point identifier type (UINT16)
constexpr std::int32_t MODBUS_OCMF_CHARGING_POINT_ID_START_ADDRESS = 328701; // 701Ch: OCMF CPI (CHAR[40] = 20 words)
constexpr std::uint16_t MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT = 20;       // 40 bytes = 20 words
constexpr std::int32_t MODBUS_OCMF_SESSION_MODALITY_ADDRESS = 328727;        // 7036h: OCMF Session Modality (UINT16)
constexpr std::uint16_t MODBUS_OCMF_SESSION_MODALITY_CHARGING_VEHICLE = 0;   // Charging vehicle
constexpr std::uint16_t MODBUS_OCMF_SESSION_MODALITY_VEHICLE_TO_GRID = 1;    // Vehicle to grid
constexpr std::uint16_t MODBUS_OCMF_SESSION_MODALITY_BIDIRECTIONAL = 2;      // Bidirectional

// Tariff text register (Table 4.32)
// 326881 (6900h): Tariff text (CHAR[252] = 126 words)
constexpr std::int32_t MODBUS_OCMF_TARIFF_TEXT_ADDRESS = 326881;  // 6900h: Tariff text (CHAR[252] = 126 words)
constexpr std::uint16_t MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT = 126; // 252 bytes = 126 words (CHAR[252])
constexpr std::int32_t MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS =
    328417; // 6F00h: OCMF Transaction ID Generation (UINT16)

// Tariff update register (Table 4.33)
constexpr std::int32_t MODBUS_OCMF_TARIFF_UPDATE_ADDRESS = 327085; // 69CCh: Tariff update (UINT16)

// OCMF Command register (Table 4.35)
// The register is UINT16 containing the ASCII code (e.g. 'B', 'E', 'A').
constexpr std::int32_t MODBUS_OCMF_COMMAND_ADDRESS = 328737; // 7040h: OCMF Command Data (UINT16)
constexpr std::uint16_t MODBUS_OCMF_COMMAND_START = 0x42;    // Start transaction ('B')
constexpr std::uint16_t MODBUS_OCMF_COMMAND_END = 0x45;      // End transaction ('E')
constexpr std::uint16_t MODBUS_OCMF_COMMAND_ABORT = 0x41;    // Abort transaction ('A')

// OCMF State / status registers (Table 4.39 and related)
constexpr std::int32_t MODBUS_OCMF_STATE_ADDRESS = 328929;           // 7100h: OCMF State (UINT16)
constexpr std::int32_t MODBUS_OCMF_TRANSACTION_ID_ADDRESS = 328931;  // 7102h: OCMF Transaction ID (UINT32)
constexpr std::uint16_t MODBUS_OCMF_STATE_NOT_READY = 0;             // Not ready
constexpr std::uint16_t MODBUS_OCMF_STATE_RUNNING = 1;               // Running
constexpr std::uint16_t MODBUS_OCMF_STATE_READY = 2;                 // Ready
constexpr std::uint16_t MODBUS_OCMF_STATE_CORRUPTED = 3;             // Corrupted
constexpr std::int32_t MODBUS_OCMF_STATE_SIZE_ADDRESS = 328930;      // 7101h: OCMF Size (UINT16)
constexpr std::int32_t MODBUS_OCMF_STATE_FILE_ADDRESS = 328945;      // 7110h: OCMF File (max theoretically 2031 words)
constexpr std::uint16_t MODBUS_OCMF_STATE_FILE_WORD_COUNT = 2031;    // 2031 words = 4062 bytes
constexpr std::int32_t MODBUS_OCMF_CHARGING_STATUS_ADDRESS = 328742; // 7045h: Charging status (UINT16)
constexpr std::int32_t MODBUS_OCMF_LAST_TRANSACTION_ID_ADDRESS = 328762; // 7059h: Last transaction id (CHAR[])
constexpr std::uint16_t MODBUS_OCMF_LAST_TRANSACTION_ID_WORD_COUNT = 14; // 14 bytes = 7 words
constexpr std::int32_t MODBUS_OCMF_TIME_SYNC_STATUS_ADDRESS = 328769;    // 7060h: Time synchronization status (UINT16)

} // namespace registers
} // namespace em580

namespace modbus_utils {

inline void check_bounds_or_throw(const transport::DataVector& data, transport::DataVector::size_type offset,
                                  transport::DataVector::size_type needed_bytes, const char* what) {
    if (offset > data.size() || needed_bytes > (data.size() - offset)) {
        throw std::out_of_range(std::string(what) + ": offset/length out of range (offset=" + std::to_string(offset) +
                                ", needed=" + std::to_string(needed_bytes) + ", size=" + std::to_string(data.size()) +
                                ")");
    }
}

// Strong type wrappers to prevent parameter swapping
struct ByteOffset {
    explicit ByteOffset(transport::DataVector::size_type val) : value(val) {
    }
    explicit operator transport::DataVector::size_type() const {
        return value;
    }

private:
    transport::DataVector::size_type value;
};

struct ByteLength {
    explicit ByteLength(transport::DataVector::size_type val) : value(val) {
    }
    explicit operator transport::DataVector::size_type() const {
        return value;
    }

private:
    transport::DataVector::size_type value;
};

inline std::uint32_t to_uint32(const transport::DataVector& data, ByteOffset offset) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    check_bounds_or_throw(data, off, 4, "to_uint32");
    return static_cast<std::uint32_t>(data[off] << 24 | data[off + 1] << 16 | data[off + 2] << 8 | data[off + 3]);
}

inline std::int32_t to_int32(const transport::DataVector& data, ByteOffset offset) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    check_bounds_or_throw(data, off, 4, "to_int32");
    return static_cast<std::int32_t>(data[off + 2] << 24 | data[off + 3] << 16 | data[off] << 8 | data[off + 1]);
}

inline std::uint16_t to_uint16(const transport::DataVector& data, ByteOffset offset) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    check_bounds_or_throw(data, off, 2, "to_uint16");
    return static_cast<std::uint16_t>(data[off] << 8 | data[off + 1]);
}

inline std::int16_t to_int16(const transport::DataVector& data, ByteOffset offset) {
    std::uint16_t raw = to_uint16(data, offset);
    return static_cast<std::int16_t>(raw);
}

inline std::string to_hex_string(const transport::DataVector& data, ByteOffset offset, ByteLength length) {
    const auto off = static_cast<transport::DataVector::size_type>(offset);
    const auto len = static_cast<transport::DataVector::size_type>(length);
    check_bounds_or_throw(data, off, len, "to_hex_string");
    std::stringstream ss;
    for (std::size_t index = 0; index < len; ++index) {
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[off + index]);
    }
    return ss.str();
}

inline std::size_t max_payload_bytes_for_words(std::size_t max_words) {
    const std::size_t capacity_bytes = max_words * 2;
    return capacity_bytes > 0 ? capacity_bytes - 1 : 0; // reserve NUL
}

inline void log_truncation_warning_if_needed(const char* field_name, const std::string& value, std::size_t max_words) {
    const std::size_t capacity_bytes = max_words * 2;
    const std::size_t max_payload_bytes = max_payload_bytes_for_words(max_words);
    if (value.size() > max_payload_bytes) {
        EVLOG_warning << field_name << " too long (" << value.size() << " bytes). Max is " << max_payload_bytes
                      << " bytes (" << max_words << " words / " << capacity_bytes << " bytes incl. NUL). "
                      << "It will be truncated.";
    }
}

/// Converts a string to a big-endian Modbus CHAR array (vector of UINT16 words)
/// that is **0-terminated** and contains only the **used** part (i.e. no full
/// fixed-length padding).
///
/// - Max capacity is `max_words * 2` bytes.
/// - Ensures a terminating `\\0` byte is present within the returned data.
/// - If `str` is too long, it is truncated to fit `max_words * 2 - 1` bytes (+
/// 1 byte terminator).
inline std::vector<std::uint16_t> string_to_modbus_char_array(const std::string& str, std::size_t max_words) {
    const std::size_t max_bytes = max_words * 2;
    if (max_bytes == 0) {
        return {};
    }

    const std::size_t used_len = std::min(str.size(), max_bytes - 1); // leave space for terminator
    const std::size_t bytes_to_write = used_len + 1;                  // include terminator byte
    const std::size_t words_to_write = (bytes_to_write + 1) / 2;      // ceil(bytes/2)

    std::vector<std::uint16_t> data(words_to_write, 0);
    for (std::size_t i = 0; i < used_len; ++i) {
        const std::size_t word_idx = i / 2;
        if ((i % 2) == 0) {
            data[word_idx] = static_cast<std::uint8_t>(str[i]) << 8;
        } else {
            data[word_idx] |= static_cast<std::uint8_t>(str[i]);
        }
    }
    return data;
}

} // namespace modbus_utils

namespace ocmf {

/// Confirm OCMF file read by writing NOT_READY (0) into the OCMF state
/// register.
inline void confirm_file_read(transport::AbstractModbusTransport& modbus_transport) {
    std::vector<std::uint16_t> ocmf_confirmation_data = {em580::registers::MODBUS_OCMF_STATE_NOT_READY};
    modbus_transport.write_multiple_registers(em580::registers::MODBUS_OCMF_STATE_ADDRESS, ocmf_confirmation_data);
}

/// Wait until OCMF state becomes READY (2).
/// @return true if READY, false on CORRUPTED or timeout.
inline bool wait_for_ready(transport::AbstractModbusTransport& modbus_transport,
                           std::chrono::milliseconds poll_interval = std::chrono::milliseconds{100},
                           int max_retries = 10) {
    std::uint16_t state = em580::registers::MODBUS_OCMF_STATE_NOT_READY;
    transport::DataVector state_data;
    int retries = 0;

    while (state != em580::registers::MODBUS_OCMF_STATE_READY) {
        state_data = modbus_transport.fetch(em580::registers::MODBUS_OCMF_STATE_ADDRESS, 1);
        state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

        if (state == em580::registers::MODBUS_OCMF_STATE_CORRUPTED) {
            return false;
        }
        if (state != em580::registers::MODBUS_OCMF_STATE_READY) {
            EVLOG_info << "OCMF state: " << state;
            std::this_thread::sleep_for(poll_interval);
            retries++;
            if (retries > max_retries) {
                return false;
            }
        }
    }
    return true;
}

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

inline std::optional<std::string> extract_transaction_id_from_ocmf_record(const std::string& ocmf_record) {
    const std::string key = "\"TT\"";
    std::size_t key_pos = ocmf_record.find(key);
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    std::size_t colon_pos = ocmf_record.find(':', key_pos + key.size());
    if (colon_pos == std::string::npos) {
        return std::nullopt;
    }

    std::size_t value_start = ocmf_record.find('"', colon_pos + 1);
    if (value_start == std::string::npos) {
        return std::nullopt;
    }
    ++value_start;

    std::string tt_value;
    tt_value.reserve(128);
    bool escaped = false;
    for (std::size_t i = value_start; i < ocmf_record.size(); ++i) {
        const char c = ocmf_record[i];
        if (escaped) {
            tt_value.push_back(c);
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"') {
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
    while (!tail.empty() && std::isspace(static_cast<unsigned char>(tail.front()))) {
        tail.erase(tail.begin());
    }
    while (!tail.empty() && std::isspace(static_cast<unsigned char>(tail.back()))) {
        tail.pop_back();
    }

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

inline std::uint16_t flag_to_value(types::powermeter::OCMFIdentificationFlags flag) {
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

inline std::uint16_t level_to_value(types::powermeter::OCMFIdentificationLevel level) {
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

inline std::uint16_t type_to_value(types::powermeter::OCMFIdentificationType type) {
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

} // namespace ocmf

namespace device_state_utils {

inline std::vector<std::string> decode_device_state_errors(std::uint16_t device_state) {
    struct BitError {
        const char* message;
        std::uint16_t bit;
    };

    static constexpr std::array<BitError, 12> errors = {{
        {"V1N over maximum range", 0U},
        {"V2N over maximum range", 1U},
        {"V3N over maximum range", 2U},
        {"V12 over maximum range", 3U},
        {"V23 over maximum range", 4U},
        {"V31 over maximum range", 5U},
        {"I1 over maximum range", 6U},
        {"I2 over maximum range", 7U},
        {"I3 over maximum range", 8U},
        {"Frequency outside validity range", 9U},
        {"EVCS module internal fault", 12U},
        {"Measure module internal fault", 13U},
    }};

    std::vector<std::string> out;
    for (const auto& err : errors) {
        if ((device_state & static_cast<std::uint16_t>(1U << err.bit)) != 0U) {
            out.emplace_back(err.message);
        }
    }
    return out;
}

} // namespace device_state_utils

#endif // CARLO_GAVAZZI_EM580_HELPER_HPP
