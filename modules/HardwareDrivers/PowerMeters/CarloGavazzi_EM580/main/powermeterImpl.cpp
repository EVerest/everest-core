// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <utility>

#include <date/date.h>
#include <fmt/core.h>
#include <utils/date.hpp>

#include "everest/logging.hpp"
#include "helper.hpp"
#include "powermeterImpl.hpp"

namespace {
using em580::registers::MODBUS_BASE_ADDRESS;
using em580::registers::MODBUS_DEVICE_STATE_ADDRESS;
using em580::registers::MODBUS_FIRMWARE_COMMUNICATION_MODULE_ADDRESS;
using em580::registers::MODBUS_FIRMWARE_MEASURE_MODULE_ADDRESS;
using em580::registers::MODBUS_OCMF_CHARGING_POINT_ID_START_ADDRESS;
using em580::registers::MODBUS_OCMF_CHARGING_POINT_ID_TYPE_ADDRESS;
using em580::registers::MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT;
using em580::registers::MODBUS_OCMF_CHARGING_STATUS_ADDRESS;
using em580::registers::MODBUS_OCMF_COMMAND_ADDRESS;
using em580::registers::MODBUS_OCMF_COMMAND_END;
using em580::registers::MODBUS_OCMF_COMMAND_START;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_DATA_START_ADDRESS;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_FLAGS_START_ADDRESS;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_LEVEL_ADDRESS;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_STATUS_ADDRESS;
using em580::registers::MODBUS_OCMF_IDENTIFICATION_TYPE_ADDRESS;
using em580::registers::MODBUS_OCMF_LAST_TRANSACTION_ID_ADDRESS;
using em580::registers::MODBUS_OCMF_LAST_TRANSACTION_ID_WORD_COUNT;
using em580::registers::MODBUS_OCMF_SESSION_MODALITY_ADDRESS;
using em580::registers::MODBUS_OCMF_SESSION_MODALITY_CHARGING_VEHICLE;
using em580::registers::MODBUS_OCMF_STATE_ADDRESS;
using em580::registers::MODBUS_OCMF_STATE_FILE_ADDRESS;
using em580::registers::MODBUS_OCMF_STATE_NOT_READY;
using em580::registers::MODBUS_OCMF_STATE_READY;
using em580::registers::MODBUS_OCMF_STATE_SIZE_ADDRESS;
using em580::registers::MODBUS_OCMF_TARIFF_TEXT_ADDRESS;
using em580::registers::MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT;
using em580::registers::MODBUS_OCMF_TIME_SYNC_STATUS_ADDRESS;
using em580::registers::MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS;
using em580::registers::MODBUS_PRODUCTION_YEAR_ADDRESS;
using em580::registers::MODBUS_PUBLIC_KEY_ADDRESS;
using em580::registers::MODBUS_PUBLIC_KEY_DER_ADDRESS;
using em580::registers::MODBUS_PUBLIC_KEY_DER_WORD_COUNT_256;
using em580::registers::MODBUS_PUBLIC_KEY_DER_WORD_COUNT_384;
using em580::registers::MODBUS_REAL_TIME_VALUES_ADDRESS;
using em580::registers::MODBUS_REAL_TIME_VALUES_COUNT;
using em580::registers::MODBUS_SERIAL_NUMBER_REGISTER_COUNT;
using em580::registers::MODBUS_SERIAL_NUMBER_START_ADDRESS;
using em580::registers::MODBUS_SIGNATURE_TYPE_ADDRESS;
using em580::registers::MODBUS_TEMPERATURE_ADDRESS;
using em580::registers::MODBUS_TIMEZONE_OFFSET_ADDRESS;
using em580::registers::MODBUS_UTC_TIMESTAMP_ADDRESS;
} // namespace

// Byte offsets for Modbus register 300001-300055 (physical addresses
// 0000h-0036h) Each INT32 register is 4 bytes, each INT16 register is 2 bytes
namespace Offsets {
// Voltage registers (INT32, 4 bytes each)
constexpr std::size_t V_L1_N = 0; // 300001 (0000h)
constexpr std::size_t V_L2_N = 4; // 300003 (0002h)
constexpr std::size_t V_L3_N = 8; // 300005 (0004h)

// Current registers (INT32, 4 bytes each)
constexpr std::size_t A_L1 = 24; // 300013 (000Ch)
constexpr std::size_t A_L2 = 28; // 300015 (000Eh)
constexpr std::size_t A_L3 = 32; // 300017 (0010h)

// Power registers (INT32, 4 bytes each)
constexpr std::size_t W_L1 = 36;  // 300019 (0012h)
constexpr std::size_t W_L2 = 40;  // 300021 (0014h)
constexpr std::size_t W_L3 = 44;  // 300023 (0016h)
constexpr std::size_t W_SYS = 80; // 300041 (0028h)

// Reactive power registers (INT32, 4 bytes each)
constexpr std::size_t VAR_L1 = 60;  // 300031 (001Eh)
constexpr std::size_t VAR_L2 = 64;  // 300033 (0020h)
constexpr std::size_t VAR_L3 = 68;  // 300035 (0022h)
constexpr std::size_t VAR_SYS = 88; // 300045 (002Ch)

// Phase sequence register (INT16, 2 bytes)
constexpr std::size_t PHASE_SEQUENCE = 100; // 300051 (0032h)

// Frequency register (INT16, 2 bytes)
constexpr std::size_t FREQUENCY = 102; // 300052 (0033h)

// Energy registers (INT32, 4 bytes each) - within extended read range
// (300001-300080)
constexpr std::size_t ENERGY_IMPORT = 104; // 300053 (0034h) - kWh (+) TOT, byte offset 104 (52*2)
constexpr std::size_t ENERGY_EXPORT = 156; // 300079 (004Eh) - kWh (-) TOT, byte offset 156 (78*2)
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

powermeterImpl::~powermeterImpl() {
    stop_requested_.store(true);
    stop_cv_.notify_all();
    if (live_measure_thread_.joinable()) {
        live_measure_thread_.join();
    }
    if (time_sync_thread_.joinable()) {
        time_sync_thread_.join();
    }
}

void powermeterImpl::init() {
    m_pending_closed_transaction = false;
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

    const transport::SerialCommHubTransport::RetryConfig retry_config{
        config.initial_connection_retry_count,
        config.initial_connection_retry_delay_ms,
        config.communication_retry_count,
        config.communication_retry_delay_ms,
    };

    const transport::SerialCommHubTransport::TransportConfig transport_config{
        config.powermeter_device_id,
        MODBUS_BASE_ADDRESS,
        retry_config,
    };

    p_modbus_transport = std::make_unique<transport::SerialCommHubTransport>(*mod->r_modbus, transport_config,
                                                                             error_handler, clear_error_handler);
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

    auto read_public_key_in_hex = [this](int lengthInBits) {
        const transport::DataVector data =
            p_modbus_transport->fetch(MODBUS_PUBLIC_KEY_ADDRESS, static_cast<std::uint16_t>((lengthInBits >> 3) + 1));
        // Table 4.18/4.19: last byte is unused and always 0x00; keep the 0x04
        // prefix, drop the unused byte.
        return modbus_utils::to_hex_string(data, modbus_utils::ByteOffset{0},
                                           modbus_utils::ByteLength{data.size() - 1});
    };

    auto read_public_key_der_in_hex = [this](std::uint16_t der_word_count) {
        // Table 4.20/4.21: mandatory to read whole block starting at 2600h.
        const transport::DataVector der_data = p_modbus_transport->fetch(MODBUS_PUBLIC_KEY_DER_ADDRESS, der_word_count);
        std::size_t der_len = der_data.size();
        if (der_data.size() >= 2 && der_data[0] == 0x30) {
            // DER header is: 0x30 <len> ...
            der_len = std::min<std::size_t>(der_data.size(), static_cast<std::size_t>(2 + der_data[1]));
        }
        return modbus_utils::to_hex_string(der_data, modbus_utils::ByteOffset{0}, modbus_utils::ByteLength{der_len});
    };

    const SignatureType signature_type = read_signature_type();
    std::string signature_type_string;
    std::uint16_t der_word_count = 0;

    switch (signature_type) {
    case SIGNATURE_256_BIT:
        this->m_public_key_length_in_bits = 256;
        signature_type_string = "256-bit";
        der_word_count = MODBUS_PUBLIC_KEY_DER_WORD_COUNT_256;
        break;
    case SIGNATURE_384_BIT:
        this->m_public_key_length_in_bits = 384;
        signature_type_string = "384-bit";
        der_word_count = MODBUS_PUBLIC_KEY_DER_WORD_COUNT_384;
        break;
    default:
        signature_type_string = "none";
        throw std::runtime_error("no signature keys are configured, device is not eichrecht compliant");
    }
    EVLOG_info << "Signature type detected: " << signature_type_string;

    this->m_public_key_hex = read_public_key_in_hex(this->m_public_key_length_in_bits);
    EVLOG_info << "Public key (raw, hex): " << this->m_public_key_hex;
    this->publish_public_key_ocmf(this->m_public_key_hex);

    const std::string der_hex = read_public_key_der_in_hex(der_word_count);
    EVLOG_info << "Public key (DER, hex): " << der_hex;
}

void powermeterImpl::read_firmware_versions() {
    EVLOG_info << "Read the firmware versions...";

    // Read measure module firmware version/revision (register 300771)
    transport::DataVector measure_fw_data = p_modbus_transport->fetch(MODBUS_FIRMWARE_MEASURE_MODULE_ADDRESS, 1);
    std::uint16_t measure_fw_value = modbus_utils::to_uint16(measure_fw_data, modbus_utils::ByteOffset{0});

    // Parse firmware version: MSB bits 0-3 = Minor, bits 4-7 = Major, LSB =
    // Revision
    std::uint8_t major = (measure_fw_value >> 8) & 0xF0;
    major = major >> 4; // Shift right to get actual major version (0-15)
    std::uint8_t minor = (measure_fw_value >> 8) & 0x0F;
    std::uint8_t revision = measure_fw_value & 0xFF;

    m_measure_module_firmware_version = fmt::format("{}.{}.{}", major, minor, revision);
    EVLOG_info << "Measure module firmware version: " << m_measure_module_firmware_version;

    // Read communication module firmware version/revision (register 300772)
    transport::DataVector comm_fw_data = p_modbus_transport->fetch(MODBUS_FIRMWARE_COMMUNICATION_MODULE_ADDRESS, 1);
    std::uint16_t comm_fw_value = modbus_utils::to_uint16(comm_fw_data, modbus_utils::ByteOffset{0});

    // Parse firmware version: MSB bits 0-3 = Minor, bits 4-7 = Major, LSB =
    // Revision
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
    // Modbus returns data in big-endian format: each UINT16 register is [MSB,
    // LSB] So for 7 registers, we get: [reg0_MSB, reg0_LSB, reg1_MSB, reg1_LSB,
    // ...] We assume the string contains only printable characters and null
    // terminator is correctly set or at the end
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
    std::uint16_t production_year = modbus_utils::to_uint16(year_data, modbus_utils::ByteOffset{0});

    // Combine serial number and production year with a dot separator
    m_serial_number = serial_str + "." + std::to_string(production_year);
    EVLOG_info << "Serial number: " << m_serial_number;
}

void powermeterImpl::read_transaction_state_and_id() {
    transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
    std::uint16_t ocmf_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});
    if (ocmf_state == MODBUS_OCMF_STATE_READY) {
        m_pending_closed_transaction = true;
        EVLOG_info << "Detected a closed transaction with data pending to be read";
    }
}

void powermeterImpl::configure_device() {
    EVLOG_info << "Configure the device...";
    read_firmware_versions();
    read_serial_number();
    read_signature_config();
    // need a delay here because if the device comes from a power outage, the time
    // sync will fail
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Initial time synchronization
    synchronize_time();
    // Set timezone offset
    set_timezone(config.timezone_offset_minutes);
    // see if there is a pending closed transaction that needs to be read
    read_transaction_state_and_id();
    EVLOG_info << "Device configured";
}

void powermeterImpl::ready() {
    // Retry logic is now handled by SerialCommHubTransport
    live_measure_thread_ = std::thread([this] {
        std::atomic_bool device_not_configured = true;
        auto last_device_state_read = std::chrono::steady_clock::time_point{};
        while (!stop_requested_.load()) {
            const auto measurement_interval = std::chrono::milliseconds{config.live_measurement_interval_ms};
            const auto device_state_interval = std::chrono::milliseconds{config.device_state_read_interval_ms};
            try {
                if (device_not_configured.load()) {
                    configure_device();
                    device_not_configured = false;
                    last_device_state_read = std::chrono::steady_clock::time_point{}; // force state read
                }
                read_powermeter_values();
                const auto now = std::chrono::steady_clock::now();
                if (last_device_state_read == std::chrono::steady_clock::time_point{} ||
                    (now - last_device_state_read) >= device_state_interval) {
                    read_device_state();
                    last_device_state_read = now;
                }
            } catch (const std::exception& e) {
                EVLOG_error << "Failed to communicate with the device, try again in "
                            << config.communication_error_pause_delay_s << " seconds: " << e.what();
                device_not_configured = true;
                {
                    std::unique_lock<std::mutex> lock(stop_mutex_);
                    stop_cv_.wait_for(lock, std::chrono::seconds{config.communication_error_pause_delay_s},
                                      [this] { return stop_requested_.load(); });
                }
            }
            {
                std::unique_lock<std::mutex> lock(stop_mutex_);
                stop_cv_.wait_for(lock, measurement_interval, [this] { return stop_requested_.load(); });
            }
        }
    });

    // Start time synchronization thread
    time_sync_thread_ = std::thread([this]() { time_sync_thread(); });
}

void powermeterImpl::write_transaction_registers(const types::powermeter::TransactionReq& transaction_req) {
    // 1. Write OCMF Identification Status (register 328673, 7000h)
    // 0 = NOT_ASSIGNED (False), 1 = ASSIGNED (True)
    std::uint16_t identification_status_value =
        (transaction_req.identification_status == types::powermeter::OCMFUserIdentificationStatus::ASSIGNED) ? 1 : 0;
    std::vector<std::uint16_t> status_data = {identification_status_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_STATUS_ADDRESS, status_data);

    // 2. Write OCMF Identification Level (register 328674, 7001h) - optional
    std::uint16_t identification_level_value = 0; // Default: NONE
    if (transaction_req.identification_level.has_value()) {
        identification_level_value = ocmf::level_to_value(transaction_req.identification_level.value());
    }
    std::vector<std::uint16_t> level_data = {identification_level_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_LEVEL_ADDRESS, level_data);

    // 3. Write OCMF Identification Flags (registers 328675-328678, 7002h-7005h) -
    // up to 4 flags
    std::vector<std::uint16_t> flags_data(MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT, 0);
    for (size_t i = 0; i < transaction_req.identification_flags.size() && i < MODBUS_OCMF_IDENTIFICATION_FLAGS_COUNT;
         ++i) {
        flags_data[i] = ocmf::flag_to_value(transaction_req.identification_flags[i]);
    }
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_FLAGS_START_ADDRESS, flags_data);

    // 4. Write OCMF Identification Type (register 328679, 7006h)
    std::uint16_t identification_type_value = ocmf::type_to_value(transaction_req.identification_type);
    std::vector<std::uint16_t> type_data = {identification_type_value};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_TYPE_ADDRESS, type_data);

    // 5. Write OCMF Identification Data (registers 328680-328699, 7007h-701Ah) -
    // CHAR[40] = 20 words Format: identification_data + ',' + transaction_id Max
    // length: 40 characters - the transaction_id is 36 characters max
    std::string client_id_str = transaction_req.identification_data.value_or("");
    modbus_utils::log_truncation_warning_if_needed("OCMF Identification Data", client_id_str,
                                                   MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT);
    std::vector<std::uint16_t> id_data =
        modbus_utils::string_to_modbus_char_array(client_id_str, MODBUS_OCMF_IDENTIFICATION_DATA_WORD_COUNT);
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_IDENTIFICATION_DATA_START_ADDRESS, id_data);

    // 6. Write OCMF Charging point identifier type (register 328700, 701Bh)
    // 0 = EVSEID, 1 = CBIDC (default to EVSEID)
    std::uint16_t charging_point_id_type = 0; // EVSEID
    std::vector<std::uint16_t> id_type_data = {charging_point_id_type};
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_CHARGING_POINT_ID_TYPE_ADDRESS, id_type_data);

    // 7. Write OCMF Charging point identifier (registers 328701-328720,
    // 701Ch-702Fh) - CHAR[40] = 20 words (evse_id)
    modbus_utils::log_truncation_warning_if_needed("OCMF Charging Point Identifier (EVSE ID)", transaction_req.evse_id,
                                                   MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT);
    std::vector<std::uint16_t> evse_id_data =
        modbus_utils::string_to_modbus_char_array(transaction_req.evse_id, MODBUS_OCMF_CHARGING_POINT_ID_WORD_COUNT);
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_CHARGING_POINT_ID_START_ADDRESS, evse_id_data);

    // 8. Write tariff text (register 326881, 6900h) - CHAR[252] = 126 words
    // The device accepts partial writes as long as the string is 0-terminated.
    const std::string tt_marker = "<=>";
    const std::string tariff_text =
        transaction_req.tariff_text.value_or("") + tt_marker + transaction_req.transaction_id;
    modbus_utils::log_truncation_warning_if_needed("OCMF Tariff Text (TT)", tariff_text,
                                                   MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT);
    const std::vector<std::uint16_t> tariff_text_data =
        modbus_utils::string_to_modbus_char_array(tariff_text, MODBUS_OCMF_TARIFF_TEXT_WORD_COUNT);
    p_modbus_transport->write_multiple_registers(MODBUS_OCMF_TARIFF_TEXT_ADDRESS, tariff_text_data);
}

std::string powermeterImpl::read_ocmf_file() {
    transport::DataVector size_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_SIZE_ADDRESS, 1);
    std::uint16_t size = modbus_utils::to_uint16(size_data, modbus_utils::ByteOffset{0});
    if (size == 0) {
        throw std::runtime_error("OCMF file size is 0");
    }
    transport::DataVector file_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_FILE_ADDRESS, size);
    return std::string{file_data.begin(), file_data.end()};
}

void powermeterImpl::clear_transaction_states() {
    transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
    std::uint16_t ocmf_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

    if (ocmf_state == MODBUS_OCMF_STATE_READY) {
        EVLOG_info << "OCMF state before starting transaction: " << ocmf_state;
        EVLOG_info << "Cleanup necessary ...";
        read_ocmf_file();
        // write 0 to the OCMF state to confirm the reading of the OCMF file
        std::vector<std::uint16_t> ocmf_confirmation_data = {MODBUS_OCMF_STATE_NOT_READY};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_STATE_ADDRESS, ocmf_confirmation_data);
        EVLOG_info << "Cleanup done.";
    }
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& treq) {
    try {
        EVLOG_info << "Starting transaction with transaction id: " << treq.transaction_id
                   << " evse id: " << treq.evse_id << " identification status: " << treq.identification_status
                   << " identification type: "
                   << types::powermeter::ocmfidentification_type_to_string(treq.identification_type)
                   << " identification level: "
                   << types::powermeter::ocmfidentification_level_to_string(
                          treq.identification_level.value_or(types::powermeter::OCMFIdentificationLevel::NONE))
                   << " identification data: " << treq.identification_data.value_or("")
                   << " tariff text: " << treq.tariff_text.value_or("none");
        // Check OCMF state and ensure it's NOT_READY before starting a transaction
        // According to the Modbus document, the OCMF state must be NOT_READY (0) to
        // start a new transaction
        transport::DataVector state_data = p_modbus_transport->fetch(MODBUS_OCMF_STATE_ADDRESS, 1);
        std::uint16_t ocmf_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});
        EVLOG_info << "OCMF state before starting transaction: " << ocmf_state;

        if (ocmf_state != MODBUS_OCMF_STATE_NOT_READY) {
            EVLOG_warning << "Spurious transaction detected, clearing transaction states ...";
            clear_transaction_states();
            m_pending_closed_transaction = false;
            return {types::powermeter::TransactionRequestStatus::OK};
        }

        // Write transaction registers first
        EVLOG_info << "Write transaction registers...";
        write_transaction_registers(treq);

        EVLOG_info << "Write session modality ... to charging vehicle";
        std::vector<std::uint16_t> session_modality_data = {MODBUS_OCMF_SESSION_MODALITY_CHARGING_VEHICLE};
        p_modbus_transport->write_multiple_registers(MODBUS_OCMF_SESSION_MODALITY_ADDRESS, session_modality_data);

        // Write 'B' command to start transaction (Table 4.35, register 328737)
        std::vector<std::uint16_t> command_data1 = {MODBUS_OCMF_COMMAND_START};
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
    EVLOG_info << "Stopping transaction with transaction id: " << (transaction_id.empty() ? "empty" : transaction_id);
    // if the transaction id is empty, we need to clean up the transaction states
    // we do our best to clean up the transaction states
    if (transaction_id.empty()) {
        EVLOG_info << "Cleaning up the transaction request.";
        try {
            if (!m_pending_closed_transaction and m_transaction_active.load()) {
                std::vector<std::uint16_t> command_data = {MODBUS_OCMF_COMMAND_END};
                p_modbus_transport->write_multiple_registers(MODBUS_OCMF_COMMAND_ADDRESS, command_data);
                EVLOG_info << "Transaction " << transaction_id << " stopped";
            }
            m_pending_closed_transaction = false;
            clear_transaction_states();
        } catch (const std::exception& e) {
            EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
        }
        m_pending_closed_transaction = false;
        return {types::powermeter::TransactionRequestStatus::OK, {}, {}};
    }
    try {
        if (m_pending_closed_transaction) {
            // the received transaction id is different from the current transaction
            // id since there is a pending closed transaction, I assume a power loss
            // occurred we need to check if the transaction id is equal to the
            // transaction id from OCMF file
            EVLOG_info << "Power loss occurred, checking if the transaction id == "
                          "transaction id from OCMF file";
            std::string ocmf_file = read_ocmf_file();
            const auto ocmf_file_transaction_id_opt = ocmf::extract_transaction_id_from_ocmf_record(ocmf_file);
            if (!ocmf_file_transaction_id_opt.has_value()) {
                EVLOG_error << "Failed to extract transaction id from OCMF file TT field";
                return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR,
                        {},
                        {},
                        "Failed to extract transaction id from OCMF file"};
            }
            const std::string& ocmf_file_transaction_id = *ocmf_file_transaction_id_opt;
            EVLOG_info << "OCMF file transaction id: " << ocmf_file_transaction_id;
            if (ocmf_file_transaction_id != transaction_id) {
                return {
                    types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "Transaction id mismatch"};
            }
            EVLOG_info << "Transaction id matches, sending successful transaction "
                          "stop response with OCMF file";
            m_pending_closed_transaction = false;
            auto signed_meter_value = types::units_signed::SignedMeterValue{ocmf_file, "", "OCMF"};
            signed_meter_value.public_key.emplace(m_public_key_hex);
            ocmf::confirm_file_read(*p_modbus_transport);
            return types::powermeter::TransactionStopResponse{types::powermeter::TransactionRequestStatus::OK,
                                                              {}, // Empty start_signed_meter_value
                                                              signed_meter_value};
        } else if (m_transaction_id == transaction_id) {
            EVLOG_info << "Sending the end transaction command to the device";
            // Write 'E' command to end transaction (Table 4.35, register 328737)
            std::vector<std::uint16_t> command_data = {MODBUS_OCMF_COMMAND_END};
            p_modbus_transport->write_multiple_registers(MODBUS_OCMF_COMMAND_ADDRESS, command_data);
            EVLOG_info << "Transaction " << transaction_id << " stopped";
            m_transaction_active.store(false);

            // check if the OCMF state is ready (Table 4.36, register 328742)
            if (!ocmf::wait_for_ready(*p_modbus_transport)) {
                return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR,
                        {},
                        {},
                        "get_signed_meter_value_error"};
            }

            const std::string ocmf_data = read_ocmf_file();
            // EVLOG_info << "OCMF data: " << ocmf_data;
            auto signed_meter_value = types::units_signed::SignedMeterValue{ocmf_data, "", "OCMF"};
            signed_meter_value.public_key.emplace(m_public_key_hex);

            // write 0 to the OCMF state to confirm the reading of the OCMF file
            ocmf::confirm_file_read(*p_modbus_transport);
            m_pending_closed_transaction = false;
            return types::powermeter::TransactionStopResponse{types::powermeter::TransactionRequestStatus::OK,
                                                              {}, // Empty start_signed_meter_value
                                                              signed_meter_value};
        } else {
            EVLOG_error << "No open transaction or unknown transaction id: " << transaction_id;
            return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR,
                    {},
                    {},
                    "No open transaction or unknown transaction id"};
        }
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
    // This optimization reduces Modbus requests from 3 to 2 (removing the
    // separate totals read)
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
    // Note: Frequency is also available at 300273 and 301341 as INT32 with
    // different factors, but we use 300052 (INT16) to keep the bulk read compact
    // (300001-300055)
    types::units::Frequency frequency_Hz;
    frequency_Hz.L1 = Factors::FREQUENCY *
                      static_cast<float>(modbus_utils::to_int16(data, modbus_utils::ByteOffset{Offsets::FREQUENCY}));
    powermeter.frequency_Hz = frequency_Hz;

    // Phase sequence (INT16) - register 300051 (0032h)
    // Value -1 = L1-L3-L2 sequence, value 1 = L1-L2-L3 sequence
    std::int16_t phase_sequence = modbus_utils::to_int16(data, modbus_utils::ByteOffset{Offsets::PHASE_SEQUENCE});
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

    // Disable for now the temperature reading, since I can't read it in the above
    // block read Read internal temperature (INT16, weight: Temperature*10) -
    // register 300776 (0307h) - 1 word transport::DataVector temperature_data =
    // p_modbus_transport->fetch(MODBUS_TEMPERATURE_ADDRESS, 1);
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
        std::uint16_t state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

        // 2. Charging status (register 328742 / 7045h)
        transport::DataVector charging_status_data = p_modbus_transport->fetch(MODBUS_OCMF_CHARGING_STATUS_ADDRESS, 1);
        std::uint16_t charging_status = modbus_utils::to_uint16(charging_status_data, modbus_utils::ByteOffset{0});

        // 3. Last transaction id (register 328723 / 7059h, CHAR[])
        transport::DataVector last_tx_data = p_modbus_transport->fetch(MODBUS_OCMF_LAST_TRANSACTION_ID_ADDRESS,
                                                                       MODBUS_OCMF_LAST_TRANSACTION_ID_WORD_COUNT);
        auto null_pos = std::find(last_tx_data.begin(), last_tx_data.end(), 0);
        std::string last_tx_id(last_tx_data.begin(), null_pos);

        // 4. Time synchronization status (register 328769 / 7060h)
        transport::DataVector time_sync_status_data =
            p_modbus_transport->fetch(MODBUS_OCMF_TIME_SYNC_STATUS_ADDRESS, 1);
        std::uint16_t time_sync_status = modbus_utils::to_uint16(time_sync_status_data, modbus_utils::ByteOffset{0});

        // 5. OCMF command (last written command value)
        transport::DataVector cmd_data = p_modbus_transport->fetch(MODBUS_OCMF_COMMAND_ADDRESS, 1);
        std::uint16_t raw_cmd = modbus_utils::to_uint16(cmd_data, modbus_utils::ByteOffset{0});
        // ASCII code is stored in the low byte
        char cmd_char = static_cast<char>(raw_cmd & 0xFFU);

        // 6. Transaction ID definition (OCMF transaction ID generation)
        transport::DataVector tx_def_data = p_modbus_transport->fetch(MODBUS_OCMF_TRANSACTION_ID_GENERATION_ADDRESS, 1);
        std::uint16_t tx_def = modbus_utils::to_uint16(tx_def_data, modbus_utils::ByteOffset{0});

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
    std::int64_t seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    // Convert to UINT64 and split into 4 words
    // According to EM580 Modbus spec: for INT64, word order is LSW->MSW
    // (little-endian word order) So we write: [LSW, LSW+1, MSW-1, MSW] = [bits
    // 0-15, bits 16-31, bits 32-47, bits 48-63]
    std::uint64_t timestamp = static_cast<std::uint64_t>(seconds_since_epoch);
    std::vector<std::uint16_t> data;
    data.push_back(static_cast<std::uint16_t>(timestamp & 0xFFFF));         // LSW: bits 0-15
    data.push_back(static_cast<std::uint16_t>((timestamp >> 16) & 0xFFFF)); // bits 16-31
    data.push_back(static_cast<std::uint16_t>((timestamp >> 32) & 0xFFFF)); // bits 32-47
    data.push_back(static_cast<std::uint16_t>((timestamp >> 48) & 0xFFFF)); // MSW: bits 48-63

    // Write UTC timestamp to register 328723 (4 words for INT64)
    p_modbus_transport->write_multiple_registers(MODBUS_UTC_TIMESTAMP_ADDRESS, data);

    EVLOG_info << "Time synchronized: " << Everest::Date::to_rfc3339(now_utc)
               << " (Unix timestamp: " << seconds_since_epoch << ")";
}

void powermeterImpl::set_timezone(int offset_minutes) {
    EVLOG_info << "Try to set the timezone ... ";

    // Convert to INT16 (signed 16-bit integer)
    // Timezone offset range: -1440 to +1440 minutes is validated by the manifest.
    std::int16_t offset_int16 = static_cast<std::int16_t>(offset_minutes);
    std::vector<std::uint16_t> data;
    data.push_back(static_cast<std::uint16_t>(offset_int16));
    p_modbus_transport->write_multiple_registers(MODBUS_TIMEZONE_OFFSET_ADDRESS, data);

    EVLOG_info << "Timezone set to: " << (offset_minutes >= 0 ? "+" : "") << offset_minutes << " minutes";
}

void powermeterImpl::time_sync_thread() {
    const auto sync_interval = std::chrono::hours(1);
    auto next_sync_time = std::chrono::steady_clock::now() + sync_interval;

    while (!stop_requested_.load()) {
        {
            std::unique_lock<std::mutex> lock(stop_mutex_);
            stop_cv_.wait_until(lock, next_sync_time, [this] { return stop_requested_.load(); });
        }
        if (stop_requested_.load()) {
            break;
        }

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
    std::uint16_t device_state = modbus_utils::to_uint16(state_data, modbus_utils::ByteOffset{0});

    // Check for error bits and raise VendorError if any are set
    const auto error_messages = device_state_utils::decode_device_state_errors(device_state);

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
