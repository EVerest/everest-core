// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <atomic>
#include <chrono>
#include <set>
#include <thread>
#include <utility>

#include <date/date.h>
#include <fmt/core.h>
#include <utils/date.hpp>

#include "everest/logging.hpp"
#include "powermeterImpl.hpp"
#include "utils.hpp"

const int MODBUS_BASE_ADDRESS = 300001;

const int MODBUS_SIGNATURE_TYPE_ADDRESS = 309472;
const int MODBUS_PUBLIC_KEY_ADDRESS = 309473;

const int MODBUS_SIGNED_MAP_ADDRESS = 302049;
const int MODBUS_SIGNED_MAP_SIGNATURE_ADDRESS = 302126;

const int MODBUS_REAL_TIME_VALUES_ADDRESS = 300001;
const int MODBUS_REAL_TIME_VALUES_COUNT = 80; // Registers 300001-300080 (0x50 = 80 words)
// This range includes:
// - 300001-300052: Real-time values (52 words)
// - 300053-300054: kWh (+) TOT (energy import) - INT32, 2 words, byte offset 104 (52*2)
// - Gap: 300055-300078 (ignored)
// - 300079-300080: kWh (-) TOT (energy export) - INT32, 2 words, byte offset 156 (78*2)

const int MODBUS_TEMPERATURE_ADDRESS = 300776; // Internal Temperature

const int MODBUS_FIRMWARE_MEASURE_MODULE_ADDRESS = 300771;       // Measure module firmware version/revision
const int MODBUS_FIRMWARE_COMMUNICATION_MODULE_ADDRESS = 300772; // Communication module firmware version/revision
const int MODBUS_SERIAL_NUMBER_START_ADDRESS = 320481;           // Serial number (7 registers: 320481-320487)
const int MODBUS_SERIAL_NUMBER_REGISTER_COUNT = 7;               // 7 UINT16 registers = 14 bytes
const int MODBUS_PRODUCTION_YEAR_ADDRESS = 320488;               // Production year (1 UINT16 register)

// Device state register (Table 4.30, Section 4.3.6)
const int MODBUS_DEVICE_STATE_ADDRESS = 320499; // 5012h: Device state (UINT16 bitfield)

// Time synchronization registers
const int MODBUS_UTC_TIMESTAMP_ADDRESS = 328723;   // UTC Timestamp for synchronization (INT64, 4 words)
const int MODBUS_TIMEZONE_OFFSET_ADDRESS = 328722; // Local time delta in minutes (INT16, 1 word)

// OCMF Transaction registers (Table 4.34)
const int MODBUS_OCMF_IDENTIFICATION_STATUS_ADDRESS = 328673;      // 7000h: OCMF Ident. Status (UINT16)
const int MODBUS_OCMF_IDENTIFICATION_LEVEL_ADDRESS = 328674;       // 7001h: OCMF Ident. Level (UINT16)
const int MODBUS_OCMF_IDENTIFICATION_FLAGS_START_ADDRESS = 328675; // 7002h: OCMF Ident. Flags 1-4 (4 UINT16)
const int MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT = 4;              // 4 flags
const int MODBUS_OCMF_IDENTIFICATION_TYPE_ADDRESS = 328679;        // 7006h: OCMF Ident. Type (UINT16)
const int MODBUS_OCMF_IDENTIFICATION_DATA_START_ADDRESS = 328680;  // 7007h: OCMF Ident. Data (CHAR[40] = 20 words)
const int MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT = 20;         // 40 bytes = 20 words
const int MODBUS_OCMF_CHARGING_POINT_ID_TYPE_ADDRESS = 328700;    // 701Bh: OCMF Charging point identifier type (UINT16)
const int MODBUS_OCMF_CHARGING_POINT_ID_START_ADDRESS = 328701;   // 701Ch: OCMF CPI (CHAR[40] = 20 words)
const int MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT = 20;          // 40 bytes = 20 words
const int MODBUS_OCMF_SESSION_MODALITY_ADDRESS = 328727;          // 7036h: OCMF Session Modality (UINT16)
const uint16_t MODBUS_OCMF_SESSION_MODALITY_CHARGING_VEHICLE = 0; // Charging vehicle
const uint16_t MODBUS_OCMF_SESSION_MODALITY_VEHICLE_TO_GRID = 1;  // Vehicle to grid
const uint16_t MODBUS_OCMF_SESSION_MODALITY_BIDIRECTIONAL = 2;    // Bidirectional

// Tariff text register (Table 4.32)
// 326881 (6900h): Tariff text (CHAR[252] = 126 words)
const int MODBUS_OCMF_TARIFF_TEXT_ADDRESS = 326881;               // 6900h: Tariff text (CHAR[252] = 126 words)
const int MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT = 123;               // 246 bytes = 123 words
const int MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS = 328417; // 6F00h: OCMF Transaction ID Generation (UINT16)

// Tariff update register (Table 4.33)
const int MODBUS_OCMF_TARIFF_UPDATE_ADDRESS = 327085; // 69CCh: Tariff update (UINT16)

// OCMF Command register (Table 4.35)
// The device uses CHAR semantics for the command register: the ASCII character is stored in the MSB of the UINT16.
// Example: 'B' -> 0x4200.
const int MODBUS_OCMF_COMMAND_ADDRESS = 328737;  // 7040h: OCMF Command Data (UINT16)
const uint16_t MODBUS_OCMF_COMMAND_START = 0x42; // Start transaction ('B' in MSB)
const uint16_t MODBUS_OCMF_COMMAND_END = 0x45;   // End transaction ('E' in MSB)
const uint16_t MODBUS_OCMF_COMMAND_ABORT = 0x41; // Abort transaction ('A' in MSB)

// OCMF State / status registers (Table 4.39 and related)
const int MODBUS_OCMF_STATE_ADDRESS = 328929;               // 7100h: OCMF State (UINT16)
const uint16_t MODBUS_OCMF_STATE_NOT_READY = 0;             // Not ready
const uint16_t MODBUS_OCMF_STATE_RUNNING = 1;               // Running
const uint16_t MODBUS_OCMF_STATE_READY = 2;                 // Ready
const uint16_t MODBUS_OCMF_STATE_CORRUPTED = 3;             // Corrupted
const int MODBUS_OCMF_STATE_SIZE_ADDRESS = 328930;          // 7101h: OCMF Size (UINT16)
const int MODBUS_OCMF_STATE_FILE_ADDRESS = 328945;          // 7110h: OCMF File (max theoretically 2031 words)
const int MODBUS_OCMF_STATE_FILE_WORD_COUNT = 2031;         // 2031 words = 4062 bytes
const int MODBUS_OCMF_CHARGING_STATUS_ADDRESS = 328742;     // 7045h: Charging status (UINT16)
const int MODBUS_OCMF_LAST_TRANSACTION_ID_ADDRESS = 328762; // 7059h: Last transaction id (CHAR[])
const int MODBUS_OCMF_LAST_TRANSACTION_ID_WORD_COUNT = 14;  // 14 bytes = 7 words
const int MODBUS_OCMF_TIME_SYNC_STATUS_ADDRESS = 328769;    // 7060h: Time synchronization status (UINT16)

// Byte offsets for Modbus register 300001-300055 (physical addresses 0000h-0036h)
// Each INT32 register is 4 bytes, each INT16 register is 2 bytes
namespace Offsets {
// Voltage registers (INT32, 4 bytes each)
constexpr size_t V_L1_N = 0; // 300001 (0000h)
constexpr size_t V_L2_N = 4; // 300003 (0002h)
constexpr size_t V_L3_N = 8; // 300005 (0004h)

// Current registers (INT32, 4 bytes each)
constexpr size_t A_L1 = 24; // 300013 (000Ch)
constexpr size_t A_L2 = 28; // 300015 (000Eh)
constexpr size_t A_L3 = 32; // 300017 (0010h)

// Power registers (INT32, 4 bytes each)
constexpr size_t W_L1 = 36;  // 300019 (0012h)
constexpr size_t W_L2 = 40;  // 300021 (0014h)
constexpr size_t W_L3 = 44;  // 300023 (0016h)
constexpr size_t W_SYS = 80; // 300041 (0028h)

// Reactive power registers (INT32, 4 bytes each)
constexpr size_t VAR_L1 = 60;  // 300031 (001Eh)
constexpr size_t VAR_L2 = 64;  // 300033 (0020h)
constexpr size_t VAR_L3 = 68;  // 300035 (0022h)
constexpr size_t VAR_SYS = 88; // 300045 (002Ch)

// Phase sequence register (INT16, 2 bytes)
constexpr size_t PHASE_SEQUENCE = 100; // 300051 (0032h)

// Frequency register (INT16, 2 bytes)
constexpr size_t FREQUENCY = 102; // 300052 (0033h)

// Energy registers (INT32, 4 bytes each) - within extended read range (300001-300080)
constexpr size_t ENERGY_IMPORT = 104; // 300053 (0034h) - kWh (+) TOT, byte offset 104 (52*2)
constexpr size_t ENERGY_EXPORT = 156; // 300079 (004Eh) - kWh (-) TOT, byte offset 156 (78*2)
} // namespace Offsets

// Scaling factors from Modbus document
namespace Factors {
constexpr float VOLTAGE = 0.1F;            // Value weight: Volt*10
constexpr float CURRENT = 0.001F;          // Value weight: Ampere*1000
constexpr float POWER = 0.1F;              // Value weight: Watt*10
constexpr float REACTIVE_POWER = 0.1F;     // Value weight: var*10
constexpr float FREQUENCY = 0.1F;          // Value weight: Hz*10
constexpr float ENERGY_KWH_TO_WH = 100.0F; // Value weight: kWh*10, convert to Wh (kWh*10 * 100 = Wh)
constexpr float TEMPERATURE = 0.1F;        // Value weight: Temperature*10
} // namespace Factors

namespace module::main {

void powermeterImpl::init() {
    // Set up error handler for CommunicationFault
    transport::ErrorHandler error_handler = [this](const std::string& error_message) {
        // Check if error is already active to avoid duplicate errors
        if (!this->error_state_monitor->is_error_active("powermeter/CommunicationFault", "CommunicationError")) {
            EVLOG_error << "Raising CommunicationFault: " << error_message;
            auto error = this->error_factory->create_error("powermeter/CommunicationFault", "CommunicationError",
                                                            error_message, Everest::error::Severity::High);
            raise_error(error);
        }
    };

    // Set up clear error handler for CommunicationFault
    transport::ClearErrorHandler clear_error_handler = [this]() {
        // Clear CommunicationFault error if it's active
        if (this->error_state_monitor->is_error_active("powermeter/CommunicationFault", "CommunicationError")) {
            EVLOG_info << "Clearing CommunicationFault: Communication restored";
            clear_error("powermeter/CommunicationFault", "CommunicationError");
        }
    };

    p_modbus_transport = move(std::make_unique<transport::SerialCommHubTransport>(
        *(mod->r_modbus.get()), config.powermeter_device_id, MODBUS_BASE_ADDRESS, config.initial_connection_retry_count,
        config.initial_connection_retry_delay_ms, config.communication_retry_count,
        config.communication_retry_delay_ms, error_handler, clear_error_handler));
}

void powermeterImpl::read_signature_config() {
    EVLOG_info << "Read the signature public key...";

    enum SignatureType {
        SIGNATURE_256_BIT,
        SIGNATURE_384_BIT,
        SIGNATURE_NONE
    };

    auto read_signature_type = [this]() {
        transport::DataVector data = p_modbus_transport->fetch(MODBUS_SIGNATURE_TYPE_ADDRESS, 1);
        return static_cast<SignatureType>(modbus_utils::to_uint16(data, modbus_utils::ByteOffset{0}));
    };

    auto read_public_key = [this](int lengthInBits) {
        const transport::DataVector data =
            p_modbus_transport->fetch(MODBUS_PUBLIC_KEY_ADDRESS, (lengthInBits >> 3) + 1);
        return modbus_utils::to_hex_string(data, modbus_utils::ByteOffset{0},
                                           modbus_utils::ByteLength{data.size() - 1});
    };

    const SignatureType signature_type = read_signature_type();
    std::string signature_type_string;

    switch (signature_type) {
    case SIGNATURE_256_BIT:
        this->m_public_key_length_in_bits = 256;
        signature_type_string = "256-bit";
        break;
    case SIGNATURE_384_BIT:
        this->m_public_key_length_in_bits = 384;
        signature_type_string = "384-bit";
        break;
    default:
        signature_type_string = "none";
        throw std::runtime_error("no signature keys are configured, device is not eichrecht compliant");
    }
    EVLOG_info << "Signature type detected: " << signature_type_string;

    this->m_public_key_hex = read_public_key(this->m_public_key_length_in_bits);
    EVLOG_info << "Public key: " << this->m_public_key_hex;
    this->publish_public_key_ocmf(this->m_public_key_hex);
}

void powermeterImpl::read_firmware_versions() {
    EVLOG_info << "Read the firmware versions...";

    // Read measure module firmware version/revision (register 300771)
    transport::DataVector measure_fw_data = p_modbus_transport->fetch(MODBUS_FIRMWARE_MEASURE_MODULE_ADDRESS, 1);
    uint16_t measure_fw_value = modbus_utils::to_uint16(measure_fw_data, modbus_utils::ByteOffset{0});

    // Parse firmware version: MSB bits 0-3 = Minor, bits 4-7 = Major, LSB = Revision
    uint8_t major = (measure_fw_value >> 8) & 0xF0;
    major = major >> 4; // Shift right to get actual major version (0-15)
    uint8_t minor = (measure_fw_value >> 8) & 0x0F;
    uint8_t revision = measure_fw_value & 0xFF;

    m_measure_module_firmware_version = fmt::format("{}.{}.{}", major, minor, revision);
    EVLOG_info << "Measure module firmware version: " << m_measure_module_firmware_version;

    // Read communication module firmware version/revision (register 300772)
    transport::DataVector comm_fw_data = p_modbus_transport->fetch(MODBUS_FIRMWARE_COMMUNICATION_MODULE_ADDRESS, 1);
    uint16_t comm_fw_value = modbus_utils::to_uint16(comm_fw_data, modbus_utils::ByteOffset{0});

    // Parse firmware version: MSB bits 0-3 = Minor, bits 4-7 = Major, LSB = Revision
    major = (comm_fw_value >> 8) & 0xF0;
    major = major >> 4; // Shift right to get actual major version (0-15)
    minor = (comm_fw_value >> 8) & 0x0F;
    revision = comm_fw_value & 0xFF;

    m_communication_module_firmware_version = fmt::format("{}.{}.{}", major, minor, revision);
    EVLOG_info << "Communication module firmware version: " << m_communication_module_firmware_version;
}

void powermeterImpl::read_serial_number() {
    EVLOG_info << "Read the serial number...";
    // Read serial number (registers 320481-320487, 7 UINT16 registers = 14 bytes)
    transport::DataVector serial_data =
        p_modbus_transport->fetch(MODBUS_SERIAL_NUMBER_START_ADDRESS, MODBUS_SERIAL_NUMBER_REGISTER_COUNT);

    // Convert bytes to string (serial number is stored as ASCII)
    // Modbus returns data in big-endian format: each UINT16 register is [MSB, LSB]
    // So for 7 registers, we get: [reg0_MSB, reg0_LSB, reg1_MSB, reg1_LSB, ...]
    // We assume the string contains only printable characters and null terminator is correctly set or at the end
    std::string serial_str;
    serial_str.reserve(14);
    for (const auto& byte : serial_data) {
        char byte_char = static_cast<char>(byte);
        // Stop at null terminator if present
        if (byte_char == '\0') {
            break;
        }
        serial_str += byte_char;
    }

    // Read production year (register 320488, 1 UINT16 register)
    transport::DataVector year_data = p_modbus_transport->fetch(MODBUS_PRODUCTION_YEAR_ADDRESS, 1);
    uint16_t production_year = modbus_utils::to_uint16(year_data, modbus_utils::ByteOffset{0});

    // Combine serial number and production year with a dot separator
    m_serial_number = serial_str + "." + std::to_string(production_year);
    EVLOG_info << "Serial number: " << m_serial_number;
}

void powermeterImpl::configure_device() {
    EVLOG_info << "Configure the device...";
    read_firmware_versions();
    read_serial_number();
    read_signature_config();
    // need a delay here because if the device comes from a power outage, the time sync will fail
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Initial time synchronization
    synchronize_time();
    // Set timezone offset
    set_timezone(config.timezone_offset_minutes);

    // configure the device to use automtic transaction id generation
    // write 1 to register 328672 (7000h)
    EVLOG_info << "Configuring the device to use automtic transaction id generation";
    std::vector<uint16_t> data = {0};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS, data);

    EVLOG_info << "Device configured";
    // TODO(fmihut): check how to recover from a power outage
}

void powermeterImpl::ready() {
    // Retry logic is now handled by SerialCommHubTransport
    std::thread live_measure_publisher_thread([this] {
        std::atomic_bool device_not_configured = true;
        while (true) {
            try {
                if (device_not_configured.load()) {
                    configure_device();
                    device_not_configured = false;
                }
                read_powermeter_values();
                read_device_state();
            } catch (const std::exception& e) {
                EVLOG_error << "Failed to communicate with the device, try again in 10 seconds: " << e.what();
                device_not_configured = true;
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    live_measure_publisher_thread.detach();

    // Start time synchronization thread
    std::thread time_sync_thread_obj([this]() { time_sync_thread(); });
    time_sync_thread_obj.detach();
}

std::vector<uint16_t> powermeterImpl::string_to_modbus_char_array(const std::string& str, size_t word_count) {
    // Convert string to Modbus CHAR array (null-terminated, padded with zeros)
    // Modbus stores strings as big-endian words: [MSB, LSB] per word
    std::vector<uint16_t> data(word_count, 0);
    size_t byte_count = word_count * 2;
    size_t str_len = std::min(str.length(), byte_count - 1); // Leave space for null terminator

    // Convert string bytes to Modbus words (big-endian: MSB, LSB)
    for (size_t i = 0; i < str_len; ++i) {
        size_t word_idx = i / 2;
        if (i % 2 == 0) {
            // MSB of word
            data[word_idx] = static_cast<uint8_t>(str[i]) << 8;
        } else {
            // LSB of word
            data[word_idx] |= static_cast<uint8_t>(str[i]);
        }
    }
    // Null terminator is already handled by initialization (all zeros)
    // The first byte after the string is 0, which identifies EOL
    return data;
}

void powermeterImpl::write_transaction_registers(const types::powermeter::TransactionReq& transaction_req) {

    // Helper function to convert OCMFIdentificationFlags enum to numeric value
    auto flag_to_value = [](types::powermeter::OCMFIdentificationFlags flag) -> uint16_t {
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
    };

    // 1. Write OCMF Identification Status (register 328673, 7000h)
    // 0 = NOT_ASSIGNED (False), 1 = ASSIGNED (True)
    uint16_t identification_status_value =
        (transaction_req.identification_status == types::powermeter::OCMFUserIdentificationStatus::ASSIGNED) ? 1 : 0;
    std::vector<uint16_t> status_data = {identification_status_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_STATUS_ADDRESS, status_data);

    // 2. Write OCMF Identification Level (register 328674, 7001h) - optional
    uint16_t identification_level_value = 0; // Default: NONE
    if (transaction_req.identification_level.has_value()) {
        switch (transaction_req.identification_level.value()) {
        case types::powermeter::OCMFIdentificationLevel::NONE:
            identification_level_value = 0;
            break;
        case types::powermeter::OCMFIdentificationLevel::HEARSAY:
            identification_level_value = 1;
            break;
        case types::powermeter::OCMFIdentificationLevel::TRUSTED:
            identification_level_value = 2;
            break;
        case types::powermeter::OCMFIdentificationLevel::VERIFIED:
            identification_level_value = 3;
            break;
        case types::powermeter::OCMFIdentificationLevel::CERTIFIED:
            identification_level_value = 4;
            break;
        case types::powermeter::OCMFIdentificationLevel::SECURE:
            identification_level_value = 5;
            break;
        case types::powermeter::OCMFIdentificationLevel::MISMATCH:
            identification_level_value = 6;
            break;
        case types::powermeter::OCMFIdentificationLevel::INVALID:
            identification_level_value = 7;
            break;
        case types::powermeter::OCMFIdentificationLevel::OUTDATED:
            identification_level_value = 8;
            break;
        case types::powermeter::OCMFIdentificationLevel::UNKNOWN:
            identification_level_value = 9;
            break;
        }
    }
    std::vector<uint16_t> level_data = {identification_level_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_LEVEL_ADDRESS, level_data);

    // 3. Write OCMF Identification Flags (registers 328675-328678, 7002h-7005h) - up to 4 flags
    std::vector<uint16_t> flags_data(MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT, 0);
    for (size_t i = 0; i < transaction_req.identification_flags.size() && i < MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT;
         ++i) {
        flags_data[i] = flag_to_value(transaction_req.identification_flags[i]);
    }
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_FLAGS_START_ADDRESS, flags_data);

    // 4. Write OCMF Identification Type (register 328679, 7006h)
    uint16_t identification_type_value = 0; // Default: NONE
    switch (transaction_req.identification_type) {
    case types::powermeter::OCMFIdentificationType::NONE:
        identification_type_value = 0;
        break;
    case types::powermeter::OCMFIdentificationType::DENIED:
        identification_type_value = 1;
        break;
    case types::powermeter::OCMFIdentificationType::UNDEFINED:
        identification_type_value = 2;
        break;
    case types::powermeter::OCMFIdentificationType::ISO14443:
        identification_type_value = 10;
        break;
    case types::powermeter::OCMFIdentificationType::ISO15693:
        identification_type_value = 11;
        break;
    case types::powermeter::OCMFIdentificationType::EMAID:
        identification_type_value = 20;
        break;
    case types::powermeter::OCMFIdentificationType::EVCCID:
        identification_type_value = 21;
        break;
    case types::powermeter::OCMFIdentificationType::EVCOID:
        identification_type_value = 30;
        break;
    case types::powermeter::OCMFIdentificationType::ISO7812:
        identification_type_value = 40;
        break;
    case types::powermeter::OCMFIdentificationType::CARD_TXN_NR:
        identification_type_value = 50;
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL:
        identification_type_value = 60;
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL_1:
        identification_type_value = 61;
        break;
    case types::powermeter::OCMFIdentificationType::CENTRAL_2:
        identification_type_value = 62;
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL:
        identification_type_value = 70;
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL_1:
        identification_type_value = 71;
        break;
    case types::powermeter::OCMFIdentificationType::LOCAL_2:
        identification_type_value = 72;
        break;
    case types::powermeter::OCMFIdentificationType::PHONE_NUMBER:
        identification_type_value = 80;
        break;
    case types::powermeter::OCMFIdentificationType::KEY_CODE:
        identification_type_value = 90;
        break;
    }
    std::vector<uint16_t> type_data = {identification_type_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_TYPE_ADDRESS, type_data);

    // 5. Write OCMF Identification Data (registers 328680-328699, 7007h-701Ah) - CHAR[40] = 20 words
    // Format: identification_data + ',' + transaction_id
    // Max length: 40 characters - the transaction_id is 36 characters max
    std::string client_id_str = transaction_req.identification_data.value_or("");
    client_id_str;
    std::vector<uint16_t> id_data =
        string_to_modbus_char_array(client_id_str, MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT);
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_DATA_START_ADDRESS, id_data);

    // 6. Write OCMF Charging point identifier type (register 328700, 701Bh)
    // 0 = EVSEID, 1 = CBIDC (default to EVSEID)
    uint16_t charging_point_id_type = 0; // EVSEID
    std::vector<uint16_t> id_type_data = {charging_point_id_type};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_CHARGING_POINT_ID_TYPE_ADDRESS, id_type_data);

    // 7. Write OCMF Charging point identifier (registers 328701-328720, 701Ch-702Fh) - CHAR[40] = 20 words (evse_id)
    transaction_req.evse_id;
    std::vector<uint16_t> evse_id_data =
        string_to_modbus_char_array(transaction_req.evse_id, MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT);
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_CHARGING_POINT_ID_START_ADDRESS, evse_id_data);
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& treq) {
    // Helper function to validate strings for EM580 device
    // According to EM580 Modbus document (line 2376-2377 and APPENDIX), only specific ASCII characters are allowed
    // Exact allowed characters from APPENDIX table:
    //   33 (!), 36 ($), 37 (%), 39 ('), 40-47 (()*+,-./), 48-57 (0-9), 58 (:), 60-63 (<=>?), 65-90 (A-Z), 97-122 (a-z),
    //   128 (€), 156 (£), 157 (¥)
    // NOT allowed: space (32), pipe (124), quote (34), hash (35), ampersand (38), semicolon (59), and other characters
    // Returns true if valid, false if invalid characters are found
    auto validate_string_for_em580 = [](const std::string& str) -> bool {
        // Build set of allowed character codes (exact list from APPENDIX)
        static const std::set<uint8_t> allowed = []() {
            std::set<uint8_t> a;
            a.insert(33); // !
            a.insert(36); // $
            a.insert(37); // %
            for (uint8_t c = 39; c <= 58; ++c)
                a.insert(c); // ()*+,-./0-9:
            for (uint8_t c = 60; c <= 63; ++c)
                a.insert(c); // <=>?
            for (uint8_t c = 65; c <= 90; ++c)
                a.insert(c); // A-Z
            for (uint8_t c = 97; c <= 122; ++c)
                a.insert(c); // a-z
            a.insert(128);   // €
            a.insert(156);   // £
            a.insert(157);   // ¥
            return a;
        }();

        for (size_t i = 0; i < str.length(); ++i) {
            uint8_t code = static_cast<uint8_t>(str[i]);
            if (allowed.find(code) == allowed.end()) {
                return false;
            }
        }

        return true;
    };

    try {
        EVLOG_info << "Starting transaction with transaction id: " << treq.transaction_id
                   << " and evse id: " << treq.evse_id << " and identification status: " << treq.identification_status
                   << " and identification type: "
                   << types::powermeter::ocmfidentification_type_to_string(treq.identification_type)
                   << " and identification level: "
                   << types::powermeter::ocmfidentification_level_to_string(
                          treq.identification_level.value_or(types::powermeter::OCMFIdentificationLevel::NONE))
                   << " and identification data: " << treq.identification_data.value_or("")
                   << " and tariff text: " << treq.tariff_text.value_or("none");
        // Write transaction registers first
        EVLOG_info << "Write transaction registers...";
        write_transaction_registers(treq);

        // 8. Write tariff text (register 326881, 6900h) - CHAR[252] = 126 words
        // The meter expects a null-terminated string; the helper initializes all words to 0,
        //
        // IMPORTANT: EM580 only allows specific ASCII characters (see APPENDIX). Space and special characters are NOT
        // allowed.
        //
        EVLOG_info << "Write tariff text...";
        std::string tariff_text = treq.tariff_text.value_or("") + "<=>" + treq.transaction_id;
        if (!validate_string_for_em580(tariff_text)) {
            EVLOG_error << "String contains invalid characters for EM580 device: '" << tariff_text << "'";
            return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "Invalid tariff text (device supports only an subset of ASCII characters)"};
        } else {
            std::vector<uint16_t> tariff_text_data =
            string_to_modbus_char_array(tariff_text, MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT);
            p_modbus_transport->write_multiple_registers(MODBUS_OCMF_TARIFF_TEXT_ADDRESS, tariff_text_data);
        }

        EVLOG_info << "Write session modality ... to charging vehicle";
        std::vector<uint16_t> session_modality_data = {MODBUS_OCMF_SESSION_MODALITY_CHARGING_VEHICLE};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_SESSION_MODALITY_ADDRESS, session_modality_data);

        // Check OCMF state and ensure it's NOT_READY before starting a transaction
        // According to the Modbus document, the OCMF state must be NOT_READY (0) to start a new transaction
        transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
        uint16_t ocmf_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});
        EVLOG_info << "OCMF state before starting transaction: " << ocmf_state;

        if (ocmf_state != MODBUS_OCMF_STATE_NOT_READY) {
            if (ocmf_state == MODBUS_OCMF_STATE_READY) {
                // If state is READY, we need to reset it to NOT_READY to allow a new transaction
                // This confirms the reading of the previous OCMF file (if any) and allows a new session
                EVLOG_info << "OCMF state is READY, resetting to NOT_READY to allow new transaction";
                std::vector<uint16_t> reset_state_data = {MODBUS_OCMF_STATE_NOT_READY};
                p_modbus_transport->write_multiple_registers(MODBUS_OCMF_STATE_ADDRESS, reset_state_data);
                // Wait a bit for the state to update
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else if (ocmf_state == MODBUS_OCMF_STATE_RUNNING) {
                // If a transaction is already running, we cannot start a new one
                throw std::runtime_error(
                    "Cannot start transaction: OCMF state is RUNNING (transaction already active)");
            } else if (ocmf_state == MODBUS_OCMF_STATE_CORRUPTED) {
                // If state is CORRUPTED, we should reset it
                EVLOG_warning << "OCMF state is CORRUPTED, resetting to NOT_READY";
                std::vector<uint16_t> reset_state_data = {MODBUS_OCMF_STATE_NOT_READY};
                p_modbus_transport->write_multiple_registers(MODBUS_OCMF_STATE_ADDRESS, reset_state_data);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // Write 'B' command to start transaction (Table 4.35, register 328737)
        std::vector<uint16_t> command_data1 = {MODBUS_OCMF_COMMAND_START};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_COMMAND_ADDRESS, command_data1);
        EVLOG_info << "Transaction " << treq.transaction_id << " started";

        // Track local state (only used internally, not in device dump)
        m_transaction_active.store(true);
        m_transaction_id = treq.transaction_id;
        return {types::powermeter::TransactionRequestStatus::OK};
    } catch (const std::exception& e) {
        EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "get_signed_meter_value_error"};
    }
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    try {
        // Write 'E' command to end transaction (Table 4.35, register 328737)
        std::vector<uint16_t> command_data = {MODBUS_OCMF_COMMAND_END};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_COMMAND_ADDRESS, command_data);
        EVLOG_info << "Transaction " << transaction_id << " stopped";
        m_transaction_active.store(false);

        // check if the OCMF state is ready (Table 4.36, register 328742)
        uint16_t state = MODBUS_OCMF_STATE_NOT_READY;
        transport::DataVector state_data;
        int retries = 0;
        while (state != MODBUS_OCMF_STATE_READY) {
            state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
            state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});
            if (state == MODBUS_OCMF_STATE_CORRUPTED || state == MODBUS_OCMF_STATE_RUNNING) {
                return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR,
                        {},
                        {},
                        "get_signed_meter_value_error"};
            }
            if (state != MODBUS_OCMF_STATE_READY) {
                EVLOG_info << "OCMF state: " << state;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                retries++;
                if (retries > 10) {
                    return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR,
                            {},
                            {},
                            "get_signed_meter_value_error"};
                }
            }
        }

        // read the size of the OCMF file
        transport::DataVector size_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_SIZE_ADDRESS, 1);
        uint16_t size = modbus_utils::to_uint16(size_data, modbus_utils::ByteOffset{0});
        if (size == 0) {
            throw std::runtime_error("OCMF file size is 0");
        }

        // read the OCMF file
        transport::DataVector file_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_FILE_ADDRESS, size);
        std::string ocmf_data{file_data.begin(), file_data.end()};
        EVLOG_info << "OCMF file: " << ocmf_data;
        auto signed_meter_value = types::units_signed::SignedMeterValue{ocmf_data, "", "OCMF"};
        signed_meter_value.public_key.emplace(m_public_key_hex);

        // write 0 to the OCMF state to confirm the reading of the OCMF file
        std::vector<uint16_t> ocmf_confirmation_data = {MODBUS_OCMF_STATE_NOT_READY};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_STATE_ADDRESS, ocmf_confirmation_data);
        return types::powermeter::TransactionStopResponse{types::powermeter::TransactionRequestStatus::OK,
                                                          {}, // Empty start_signed_meter_value
                                                          signed_meter_value};
    } catch (const std::exception& e) {
        EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "get_signed_meter_value_error"};
    }
}

void powermeterImpl::read_powermeter_values() {
    // Read registers 300001-300082 (82 words = 0x50+2)
    // This single read includes:
    // - 300001-300052: Real-time values (52 words)
    // - 300053-300054: kWh (+) TOT (energy import) - INT32, 2 words
    // - Gap: 300055-300078 (Modbus will read but we ignore these bytes)
    // - 300079-300080: kWh (-) TOT (energy export) - INT32, 2 words
    // This optimization reduces Modbus requests from 3 to 2 (removing the separate totals read)
    transport::DataVector data =
        p_modbus_transport->fetch(MODBUS_REAL_TIME_VALUES_ADDRESS, MODBUS_REAL_TIME_VALUES_COUNT);

    types::powermeter::Powermeter powermeter{};
    powermeter.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    powermeter.meter_id = std::move(std::string(this->mod->info.id));

    // Voltage values (INT32, weight: Volt*10)
    // 300001 (0000h): V L1-N
    // 300003 (0002h): V L2-N
    // 300005 (0004h): V L3-N
    types::units::Voltage voltage_V;
    voltage_V.L1 =
        Factors::VOLTAGE * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::V_L1_N}));
    voltage_V.L2 =
        Factors::VOLTAGE * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::V_L2_N}));
    voltage_V.L3 =
        Factors::VOLTAGE * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::V_L3_N}));
    powermeter.voltage_V = voltage_V;

    // Current values (INT32, weight: Ampere*1000)
    // Values are already signed: positive = import, negative = export
    // 300013 (000Ch): A L1
    // 300015 (000Eh): A L2
    // 300017 (0010h): A L3
    types::units::Current current_A;
    current_A.L1 =
        Factors::CURRENT * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::A_L1}));
    current_A.L2 =
        Factors::CURRENT * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::A_L2}));
    current_A.L3 =
        Factors::CURRENT * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::A_L3}));
    powermeter.current_A = current_A;

    // Power values (INT32, weight: Watt*10)
    // Values are already signed: positive = import, negative = export
    // 300019 (0012h): W L1
    // 300021 (0014h): W L2
    // 300023 (0016h): W L3
    // 300041 (0028h): W sys
    types::units::Power power_W;
    power_W.L1 =
        Factors::POWER * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::W_L1}));
    power_W.L2 =
        Factors::POWER * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::W_L2}));
    power_W.L3 =
        Factors::POWER * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::W_L3}));
    power_W.total =
        Factors::POWER * static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::W_SYS}));
    powermeter.power_W = power_W;

    // Reactive power values (INT32, weight: var*10)
    // Values are already signed: positive = import, negative = export
    // 300031 (001Eh): var L1
    // 300033 (0020h): var L2
    // 300035 (0022h): var L3
    // 300045 (002Ch): var sys
    types::units::ReactivePower VAR;
    VAR.L1 = Factors::REACTIVE_POWER *
             static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::VAR_L1}));
    VAR.L2 = Factors::REACTIVE_POWER *
             static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::VAR_L2}));
    VAR.L3 = Factors::REACTIVE_POWER *
             static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::VAR_L3}));
    VAR.total = Factors::REACTIVE_POWER *
                static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::VAR_SYS}));
    powermeter.VAR = VAR;

    // Frequency (INT16, weight: Hz*10) - register 300052 (0033h)
    // Note: Frequency is also available at 300273 and 301341 as INT32 with different factors,
    // but we use 300052 (INT16) to keep the bulk read compact (300001-300055)
    types::units::Frequency frequency_Hz;
    frequency_Hz.L1 = Factors::FREQUENCY *
                      static_cast<float>(modbus_utils::to_int16(data, modbus_utils::ByteOffset{Offsets::FREQUENCY}));
    powermeter.frequency_Hz = frequency_Hz;

    // Phase sequence (INT16) - register 300051 (0032h)
    // Value -1 = L1-L3-L2 sequence, value 1 = L1-L2-L3 sequence
    int16_t phase_sequence = modbus_utils::to_int16(data, modbus_utils::ByteOffset{Offsets::PHASE_SEQUENCE});
    if (phase_sequence == -1) {
        powermeter.phase_seq_error = true; // L1-L3-L2 is considered an error (counter-clockwise)
    } else if (phase_sequence == 1) {
        powermeter.phase_seq_error = false; // L1-L2-L3 is correct (clockwise)
    }

    // Energy import: register 300053 (kWh (+) TOT) - INT32, 2 words
    // Byte offset in data: 104 (52*2, since 300053 is at offset 52 from 300001)
    // Note: energy_Wh_import is a required field, not optional
    powermeter.energy_Wh_import.total =
        Factors::ENERGY_KWH_TO_WH *
        static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::ENERGY_IMPORT}));

    // Energy export: register 300079 (kWh (-) TOT) - INT32, 2 words
    // Byte offset in data: 156 (78*2, since 300079 is at offset 78 from 300001)
    types::units::Energy energy_Wh_export;
    energy_Wh_export.total =
        Factors::ENERGY_KWH_TO_WH *
        static_cast<float>(modbus_utils::to_int32(data, modbus_utils::ByteOffset{Offsets::ENERGY_EXPORT}));
    powermeter.energy_Wh_export = energy_Wh_export;

    // Read internal temperature (INT16, weight: Temperature*10) - register 300776 (0307h) - 1 word
    // transport::DataVector temperature_data = p_modbus_transport->fetch(MODBUS_TEMPERATURE_ADDRESS, 1);
    // types::temperature::Temperature temperature;
    // temperature.temperature = Factors::TEMPERATURE *
    //                           static_cast<float>(modbus_utils::to_int16(temperature_data,
    //                           modbus_utils::ByteOffset{0}));
    // temperature.location = "Internal";
    // std::vector<types::temperature::Temperature> temperatures;
    // temperatures.push_back(temperature);
    // powermeter.temperatures = temperatures;

    this->publish_powermeter(powermeter);
}

void powermeterImpl::dump_device_state() {
    try {
        // 1. OCMF state
        transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
        uint16_t state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

        // 2. Charging status (register 328742 / 7045h)
        transport::DataVector charging_status_data = p_modbus_transport->fetch(MODBUS_OCMF_CHARGING_STATUS_ADDRESS, 1);
        uint16_t charging_status = modbus_utils::to_uint16(charging_status_data, modbus_utils::ByteOffset{0});

        // 3. Last transaction id (register 328723 / 7059h, CHAR[])
        // transport::DataVector last_tx_data = p_modbus_transport->fetch(MODBUS_OCMF_LAST_TRANSACTION_ID_ADDRESS,
        //                                                                MODBUS_OCMF_LAST_TRANSACTION_ID_WORD_COUNT);
        // auto null_pos = std::find(last_tx_data.begin(), last_tx_data.end(), 0);
        // std::string last_tx_id(last_tx_data.begin(), null_pos);

        // 4. Time synchronization status (register 328769 / 7060h)
        transport::DataVector time_sync_status_data =
            p_modbus_transport->fetch(MODBUS_OCMF_TIME_SYNC_STATUS_ADDRESS, 1);
        uint16_t time_sync_status = modbus_utils::to_uint16(time_sync_status_data, modbus_utils::ByteOffset{0});

        // 5. OCMF command (last written command value)
        transport::DataVector cmd_data = p_modbus_transport->fetch(MODBUS_OCMF_COMMAND_ADDRESS, 1);
        uint16_t raw_cmd = modbus_utils::to_uint16(cmd_data, modbus_utils::ByteOffset{0});
        char cmd_char = static_cast<char>((raw_cmd >> 8U) & 0xFFU);

        // 6. Transaction ID definition (OCMF transaction ID generation)
        transport::DataVector tx_def_data = p_modbus_transport->fetch(MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS, 1);
        uint16_t tx_def = modbus_utils::to_uint16(tx_def_data, modbus_utils::ByteOffset{0});

        EVLOG_info << "EM580 device state dump:";
        EVLOG_info << "  OCMF state: " << state;
        EVLOG_info << "  Charging status (device, raw): " << charging_status;
        // EVLOG_info << "  Last transaction id (device): " << last_tx_id;
        EVLOG_info << "  Time synchronization status (device, raw): " << time_sync_status;
        EVLOG_info << "  Last OCMF command (raw): 0x" << std::hex << raw_cmd << " ('" << cmd_char << "')";
        EVLOG_info << "  Transaction ID definition (OCMF): 0x" << std::hex << tx_def;
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to dump EM580 device state: " << e.what();
    }
}

bool powermeterImpl::is_transaction_active() const {
    return m_transaction_active.load();
}

void powermeterImpl::synchronize_time() {
    // Get current UTC time as seconds since Unix epoch
    auto now_utc = date::utc_clock::now();
    // Convert to system_clock for time_t conversion
    auto sys_now = std::chrono::system_clock::now();
    auto time_since_epoch = sys_now.time_since_epoch();
    int64_t seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    // Convert to UINT64 and split into 4 words
    // According to EM580 Modbus spec: for INT64, word order is LSW->MSW (little-endian word order)
    // So we write: [LSW, LSW+1, MSW-1, MSW] = [bits 0-15, bits 16-31, bits 32-47, bits 48-63]
    uint64_t timestamp = static_cast<uint64_t>(seconds_since_epoch);
    std::vector<uint16_t> data;
    data.push_back(static_cast<uint16_t>(timestamp & 0xFFFF));         // LSW: bits 0-15
    data.push_back(static_cast<uint16_t>((timestamp >> 16) & 0xFFFF)); // bits 16-31
    data.push_back(static_cast<uint16_t>((timestamp >> 32) & 0xFFFF)); // bits 32-47
    data.push_back(static_cast<uint16_t>((timestamp >> 48) & 0xFFFF)); // MSW: bits 48-63

    // Write UTC timestamp to register 328723 (4 words for INT64)
    p_modbus_transport->write_multiple_registers(MODBUS_UTC_TIMESTAMP_ADDRESS, data);

    EVLOG_info << "Time synchronized: " << Everest::Date::to_rfc3339(now_utc)
               << " (Unix timestamp: " << seconds_since_epoch << ")";
}

void powermeterImpl::set_timezone(int offset_minutes) {
    EVLOG_info << "Try to set the timezone ... ";

    // Convert to INT16 (signed 16-bit integer)
    // Timezone offset range: -1440 to +1440 minutes is validated by the manifest.
    int16_t offset_int16 = static_cast<int16_t>(offset_minutes);
    std::vector<uint16_t> data;
    data.push_back(static_cast<uint16_t>(offset_int16));
    p_modbus_transport->write_multiple_registers(MODBUS_TIMEZONE_OFFSET_ADDRESS, data);

    EVLOG_info << "Timezone set to: " << (offset_minutes >= 0 ? "+" : "") << offset_minutes << " minutes";
}

void powermeterImpl::time_sync_thread() {
    const auto sync_interval = std::chrono::hours(1);
    auto next_sync_time = std::chrono::steady_clock::now() + sync_interval;

    while (true) {
        std::this_thread::sleep_until(next_sync_time);

        if (!is_transaction_active()) {
            // No active transaction, perform time sync immediately
            try {
                synchronize_time();
                m_pending_time_sync = false;
            } catch (const std::exception& e) {
                EVLOG_error << "Time synchronization failed: " << e.what();
                // Mark as pending to retry when transaction ends
                m_pending_time_sync = true;
            }
        } else {
            // Transaction is active, mark sync as pending
            EVLOG_info << "Time synchronization deferred: charging session in progress";
            m_pending_time_sync = true;
        }

        // Schedule next sync attempt in 1 hour
        next_sync_time += sync_interval;
    }
}

void powermeterImpl::read_device_state() {
    // Read device state register (Table 4.30, Section 4.3.6)
    // Register 320499 (5012h): Device state (UINT16 bitfield)
    transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_DEVICE_STATE_ADDRESS, 1);
    uint16_t device_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

    // Check for error bits and raise VendorError if any are set
    std::vector<std::string> error_messages;

    // Voltage over maximum range errors
    if ((device_state & (1U << 0U)) != 0U) {
        error_messages.emplace_back("V1N over maximum range");
    }
    if ((device_state & (1U << 1U)) != 0U) {
        error_messages.emplace_back("V2N over maximum range");
    }
    if ((device_state & (1U << 2U)) != 0U) {
        error_messages.emplace_back("V3N over maximum range");
    }
    if ((device_state & (1U << 3U)) != 0U) {
        error_messages.emplace_back("V12 over maximum range");
    }
    if ((device_state & (1U << 4U)) != 0U) {
        error_messages.emplace_back("V23 over maximum range");
    }
    if ((device_state & (1U << 5U)) != 0U) {
        error_messages.emplace_back("V31 over maximum range");
    }

    // Current over maximum range errors
    if ((device_state & (1U << 6U)) != 0U) {
        error_messages.emplace_back("I1 over maximum range");
    }
    if ((device_state & (1U << 7U)) != 0U) {
        error_messages.emplace_back("I2 over maximum range");
    }
    if ((device_state & (1U << 8U)) != 0U) {
        error_messages.emplace_back("I3 over maximum range");
    }

    // Frequency outside validity range
    if ((device_state & (1U << 9U)) != 0U) {
        error_messages.emplace_back("Frequency outside validity range");
    }

    // Module internal fault errors
    if ((device_state & (1U << 12U)) != 0U) {
        error_messages.emplace_back("EVCS module internal fault");
    }
    if ((device_state & (1U << 13U)) != 0U) {
        error_messages.emplace_back("Measure module internal fault");
    }

    // If any error bits are set, raise VendorError
    if (!error_messages.empty()) {
        std::string error_description = "Device state errors detected: ";
        for (size_t i = 0; i < error_messages.size(); ++i) {
            if (i > 0) {
                error_description += ", ";
            }
            error_description += error_messages[i];
        }
        error_description += " (device state: 0x" + fmt::format("{:04X}", device_state) + ")";

        EVLOG_error << "Device state error: " << error_description;
        auto error = this->error_factory->create_error("powermeter/VendorError", "DeviceStateError", error_description);
        raise_error(error);
    } else {
        EVLOG_debug << "Device state OK (0x" << fmt::format("{:04X}", device_state) << ")";
    }
}

} // namespace module::main
