// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for the GSH01 powermeter application layer
*/
#ifndef APP_LAYER
#define APP_LAYER

#include "ld-ev.hpp"
#include <chrono>
#include <cstdint>
#include <everest/crc/crc.hpp>
#include <everest/logging.hpp>
#include <generated/types/powermeter.hpp>

namespace app_layer {

enum class CommandType : std::uint16_t {
    ///---------------Device Information Registers-------------------
    APP_MODE = 0x4102,            // get or set the mode of application
    TRANSPARENT_MODE = 0x4103,    // get the transparent mode meter
    SERVER_ID = 0x4110,           // get the server ID
    SERIAL_NR = 0x4111,           // get the serial number
    HW_VERSION = 0x4112,          // get the hardware version
    DEVICE_TYPE = 0x4113,         // get the device type
    APP_FW_VERSION = 0x4114,      // get the software version of application
    APP_FW_CHECKSUM = 0x4115,     // get the checksum (crc16) of application
    MT_FW_VERSION = 0x4116,       // get the software version of metering firmware
    MT_FW_CHECKSUM = 0x4117,      // get the crc16 from software integrity check of metering firmware
    BOOTL_VERSION = 0x4118,       // get the bootloader version
    APP_FW_HASH = 0x4119,         // get the application firmware hash(crc16)
    MEASUREMENT_MODE = 0x4120,    // get the measurement mode
    GET_NORMAL_VOLTAGE = 0x4121,  // get the normal voltage
    GET_NORMAL_CURRENT = 0x4122,  // get the normal current
    GET_MAX_CURRENT = 0x4123,     // get the maximum current
    REVERSE_MODE = 0x4124,        // get and set(?) the reverse mode
    CLEAR_METER_STATUS = 0x4125,  // set the clear meter status command
    INIT_METER = 0x4126,          // set the initialize meter command
    LINE_LOSS_IMPEDANCE = 0x4130, // get or set (in assembling mode) the line loss impedance
    LINE_LOSS_MEAS_MODE = 0x4131, // get the line loss energy measurement mode
    MT_MODE = 0x4133,             // get the meter operation mode
    TIME = 0x4135,                // get or set the device time
    STATUS_WORD = 0x4137,         // get the statusword
    APP_CONFIG_COMPLETE = 0x4211, // set the application configuration complete command
    METER_BUS_ADDR = 0x4212,      // get or set the meter bus address
    AS_CONFIG_COMPLETE = 0x4221,  // set the assmbling configuration complete command
    ///---------------Device Control Registers--------------------------
    GET_PUBKEY_STR16 = 0x0211,                        // get the public key in string format (base16)
    GET_PUBKEY_STR32 = 0x0212,                        // get the public key in string format (base32)
    GET_PUBKEY_CSTR16 = 0x0213,                       // get the public key compressed in string format (base16)
    GET_PUBKEY_CSTR32 = 0x0214,                       // get the public key compressed in string format (base32)
    START_TRANSACTION = 0x1501,                       // start transaction
    STOP_TRANSACTION = 0x1502,                        // stop transaction
    OCMF_STATS = 0x1510,                              // get the statistics of OCMF
    GET_OCMF = 0x1520,                                // get the OCMF
    GET_LAST_OCMF = 0x1521,                           // get last OCMF
    GET_OCMF_REVERSE = 0x1522,                        // get the OCMF reverse
    GET_PUBKEY_ASN1 = 0x1530,                         // get public key in ASN1 format
    CHARGE_POINT_ID = 0x1531,                         // get or set the charge point identification
    GET_TRANSACT_IMPORT_LINE_LOSS_ENERGY = 0x1540,    // get total transaction import line loss energy
    GET_TRANSACT_TOTAL_IMPORT_DEV_ENERGY = 0x1550,    // get total transaction import device energy
    GET_TRANSACT_TOTAL_IMPORT_MAINS_ENERGY = 0x1560,  // get total transaction import mains energy
    GET_TRANSACT_TOTAL_DURATION = 0x156A,             // get total transaction duration.
    GET_TOTAL_START_IMPORT_LINE_LOSS_ENERGY = 0x1570, // get total start import line loss energy
    GET_TOTAL_START_IMPORT_DEV_ENERGY = 0x1580,       // get total start import device energy
    GET_TOTAL_START_IMPORT_MAINS_ENERGY = 0x1590,     // get total start import mains energy
    GET_TOTAL_STOP_IMPORT_LINE_LOSS_ENERGY = 0x15A0,  // get total stop import line loss energy
    GET_TOTAL_STOP_IMPORT_DEV_ENERGY = 0x15B0,        // get total stop import device energy
    GET_TOTAL_STOP_IMPORT_MAINS_ENERGY = 0x15C0,      // get total stop import mains energy
    GET_LOG_STATS = 0x1710,                           // get the statistic of log
    GET_LOG_ENTRY = 0x1720,                           // get the log entry
    GET_LAST_LOG_ENTRY = 0x1721,                      // get last log entry
    GET_LOG_ENTRY_REVERSE = 0x1722,                   // get the log entry reverse
    ///----------------Instantaneous Registers------------------------
    GET_TOTAL_IMPORT_MAINS_ENERGY = 0x0110,     // get total import mains energy
    GET_CURRENT_L1 = 0x0131,                    // get current (phase L1)
    GET_VOLTAGE_L1 = 0x0132,                    // get voltage (phase L1)
    GET_TOTAL_IMPORT_MAINS_POWER = 0x0133,      // total import mains power
    GET_POS_DEV_VOLTAGE = 0x0135,               // get positive device voltage
    GET_NEG_DEV_VOLTAGE = 0x0136,               // get negative device voltage
    GET_TOTAL_VOLTAGE = 0x0137,                 // get total device voltage
    GET_IMPORT_DEV_POWER = 0x0138,              // get import device power
    GET_IMPORT_LINE_LOSS_POWER = 0x013A,        // get import line loss power
    GET_TOTAL_DEV_POWER = 0x013C,               // get total device power
    GET_TOTAL_IMPORT_LINE_LOSS_ENERGY = 0x0160, // get the total import line loss energy
    GET_TOTAL_IMPORT_DEV_ENERGY = 0x0170,       // get the total import device energy
    GET_SECOND_INDEX = 0x0180                   // get the second index
};

enum class CommandStatus : std::uint8_t {
    OK = 0,                        // no error
    GENERAL_ERROR = 1,             // internal process has an error
    OUT_OF_RANGE = 2,              // data is out of range
    SECURITY_ACCES_DENIED = 3,     // the current mode of the slave cannot start this command
    REJECTED = 4,                  // error of communication between application and metering board
    LOGICAL_ADDRESS_NOT_FOUND = 5, // command not implemented
    FORMAL_INVALID = 6,            // length in bytes is incorrect; e.g. received more data than expected
    NOT_AVAILABLE = 7,             // data not available
    IS_BUSY = 8,                   // slave cannot process command; already processing long operation
    PUBKEY_MISSING = 9             // public key is not stored in internal memory
};

enum class CommandResult : std::uint8_t {
    OK = 0,
    GENERAL_ERROR = 1,          // read ERROR register (read with GET_ERRORS[0xA004]) for more information
    OUT_OF_RANGE = 2,           //
    SECURITY_ACCESS_DENIED = 3, // application board is not in production mode
    REJECTED = 4,               // metering board is not responding
    FORMAT_INVALID = 6,         // payload is not empty
    NOT_AVAILABLE = 7,          // data could not be read
    BUSY = 8,                   // transaction ongoing, metering board unavailable
    PUBLIC_KEY_MISSING = 9,     //
    PROTOCOL_ERROR = 250,       // error on reception at host: protocol error (SLIP protocol)
    TIMEOUT = 251,              // no response during at least 1100 ms
    COMMUNICATION_FAILED = 254, // error on communication between PM and host device
    PENDING = 255               // special state for transaction commands
};

inline std::string command_result_to_string(CommandResult res) {
    switch (res) {
    case CommandResult::OK:
        return "OK";
    case CommandResult::GENERAL_ERROR:
        return "General Error";
    case CommandResult::OUT_OF_RANGE:
        return "Out of Range";
    case CommandResult::SECURITY_ACCESS_DENIED:
        return "Security Access Denied";
    case CommandResult::REJECTED:
        return "Rejected";
    case CommandResult::FORMAT_INVALID:
        return "Format Invalid";
    case CommandResult::NOT_AVAILABLE:
        return "Not Available";
    case CommandResult::BUSY:
        return "Busy";
    case CommandResult::PUBLIC_KEY_MISSING:
        return "Public Key Missing";
    case CommandResult::PROTOCOL_ERROR:
        return "Protocol Error";
    case CommandResult::TIMEOUT:
        return "Timeout";
    case CommandResult::COMMUNICATION_FAILED:
        return "Communication Failed";
    case CommandResult::PENDING:
        return "Pending";
    }
    throw std::out_of_range("No known string conversion for provided enum of type CommandResult");
}

enum class UserIdStatus : std::uint8_t {
    USER_NOT_ASSIGNED = 0x00,
    USER_ASSIGNED = 0x01
};

enum class UserIdType : std::uint8_t {
    NONE = 0,          // not available
    DENIED = 1,        // not retrievable (e.g. two-factor-auth)
    UNDEFINED = 2,     // type unknown / other
    ISO14443 = 10,     // UID of RFID card according to ISO14443 (4 or 7 bytes HEX)
    ISO15693 = 11,     // UID of RFID card according to ISO15693 (8 bytes HEX)
    EMAID = 20,        // Electro-Mobility-Account-ID according to ISO/IEC15118 (14 or 15 bytes string)
    EVCCID = 21,       // ID of an EV according to ISO/IEC15118 (max 6 bytes)
    EVCOID = 30,       // EV-Contract-ID according to DIN91286
    ISO7812 = 40,      // Identification-Card-Format according to ISO/IEC7812 (credit-/banking-cards, etc.)
    CAR_TXN_NR = 50,   // Card-Transaction-Number (CardTxNbr) for credit- or banking-cards
    CENTRAL = 60,      // centrally generated ID (no fixed format, e.g. UUID); OCPP 2.0
    CENTRAL_1 = 61,    // centrally generated ID (no fixed format, e.g. start-via-SMS); (up to) OCPP 1.6
    CENTRAL_2 = 62,    // centrally generated ID (no fixed format, e.g. start-by-operator); (up to) OCPP 1.6
    LOCAL = 70,        // locally generated ID (no fixed format, e.g. UUID); OCPP 2.0
    LOCAL_1 = 71,      // locally generated ID (no fixed format, e.g. generated by chargepoint); (up to) OCPP 1.6
    LOCAL_2 = 72,      // locally generated ID (no fixed format, other); (up to) OCPP 1.6
    PHONE_NUMBER = 80, // international phone number (leading '+' with country code)
    KEY_CODE = 90      // private user key (no fixed format)
};

enum class ErrorCategory : std::uint8_t {
    LAST = 0,
    LAST_CRITICAL = 1,
    FIRST = 2,
    FIRST_CRITICAL = 3
};

enum class ErrorSource : std::uint8_t {
    SYSTEM = 0,
    COMMUNICATION = 1
};

enum class ApplicationBoardMode : std::uint8_t {
    APPLICATION = 0,
    ASSEMBLY = 1
};

enum class LogType : std::uint8_t {
    NONE = 0,
    LINE_LOSS_MEASUREMENT_MODE = 1,
    IMPEDANCE_CHANGED = 2,
    OPERATION_MODE_CHANGED = 3,
    ASSEMBLY_CONFIG_CHANGED = 4,
    FATAL_ERROR_EVENT = 5,
    TIME_DELTA_TOO_BIG_EVENT = 6,
    CHARGE_POINT_ID_CHANGED = 7,
    EXTERNAL_DISPLAY_PAIRED = 8,
    EXTERNAL_DISPLAY_FAILURE = 9,
    CHARGE_DATA_OUT_OF_MEMORY = 10,
    LOG_DATA_OUT_OF_MEMORY = 11,
    FW_VERSION_CHANGED = 12,
    PULSE_LED_SOURCE_CHANGED = 13
};

inline std::string log_type_to_string(LogType log) {
    switch (log) {
    case LogType::NONE:
        return "None";
    case LogType::LINE_LOSS_MEASUREMENT_MODE:
        return "Line loss measurement mode";
    case LogType::IMPEDANCE_CHANGED:
        return "Impedance changed";
    case LogType::OPERATION_MODE_CHANGED:
        return "Operation mode changed";
    case LogType::ASSEMBLY_CONFIG_CHANGED:
        return "Assembly config changed";
    case LogType::FATAL_ERROR_EVENT:
        return "Fatal error event";
    case LogType::TIME_DELTA_TOO_BIG_EVENT:
        return "Time delta too big event";
    case LogType::CHARGE_POINT_ID_CHANGED:
        return "Charge point id changed";
    case LogType::EXTERNAL_DISPLAY_PAIRED:
        return "External display paired";
    case LogType::EXTERNAL_DISPLAY_FAILURE:
        return "External display failure";
    case LogType::CHARGE_DATA_OUT_OF_MEMORY:
        return "Charge data out of memory";
    case LogType::LOG_DATA_OUT_OF_MEMORY:
        return "Log data out of memory";
    case LogType::FW_VERSION_CHANGED:
        return "Fw version changed";
    case LogType::PULSE_LED_SOURCE_CHANGED:
        return "Pulse LED source changed";
    }
    throw std::out_of_range("No known string conversion for provided enum of type LogType");
}

class LogEntry {
public:
    app_layer::LogType type;
    std::uint32_t second_index{};
    std::uint32_t utc_time{};
    std::uint8_t utc_offset{};
    std::vector<std::uint8_t> old_value; // max. 10 elements
    std::vector<std::uint8_t> new_value; // max. 10 elements
    std::vector<std::uint8_t> server_id; // 10 elements
    std::vector<std::uint8_t> signature; // 64 elements
};

class StatusWord {
private:
    static std::map<std::uint64_t, std::string> bit_meaning;

public:
    static void print(std::uint64_t status) {
        for (const auto& [key, value] : bit_meaning) {
            if (status & key) {
                EVLOG_info << value;
            }
        }
    }
};

class Command {
public:
    app_layer::CommandType type;
    std::uint16_t length;
    app_layer::CommandStatus status;
    std::vector<std::uint8_t> data;
};

static constexpr std::uint16_t PM_GSH01_MAX_RX_LENGTH = 1500;
static constexpr std::uint16_t PM_GSH01_SERIAL_RX_INITIAL_TIMEOUT_MS = 1100;
static constexpr std::uint16_t PM_GSH01_SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

class AppLayer {

public:
    void create_command_start_transaction(app_layer::UserIdStatus user_id_status, app_layer::UserIdType user_id_type,
                                          std::string user_id_data, std::vector<std::uint8_t>& command_data);
    void create_command_stop_transaction(std::vector<std::uint8_t>& command_data);

    void create_command_get_time(std::vector<std::uint8_t>& command_data);
    void create_command_set_time(date::utc_clock::time_point timepoint, std::int8_t gmt_offset_quarters_of_an_hour,
                                 std::vector<std::uint8_t>& command_data);
    void create_command_get_bus_address(std::vector<std::uint8_t>& command_data);
    void create_command_set_bus_address(std::uint8_t bus_address, std::vector<std::uint8_t>& command_data);

    void create_command_get_voltage(std::vector<std::uint8_t>& command_data);
    void create_command_get_current(std::vector<std::uint8_t>& command_data);
    void create_command_get_import_power(std::vector<std::uint8_t>& command_data);
    void create_command_get_total_power(std::vector<std::uint8_t>& command_data);

    void create_command_get_total_start_import_energy(std::vector<std::uint8_t>& command_data);
    void create_command_get_total_stop_import_energy(std::vector<std::uint8_t>& command_data);
    void create_command_get_total_dev_import_energy(std::vector<std::uint8_t>& command_data);

    void create_command_get_total_transaction_duration(std::vector<std::uint8_t>& command_data);

    void create_command_get_pubkey_str16(std::vector<std::uint8_t>& command_data);
    void create_command_get_pubkey_asn1(std::vector<std::uint8_t>& command_data);

    void create_command_get_ocmf_stats(std::vector<std::uint8_t>& command_data);
    void create_command_get_transaction_ocmf(std::uint32_t ocmf_id, std::vector<std::uint8_t>& command_data);
    void create_command_get_last_transaction_ocmf(std::vector<std::uint8_t>& command_data);

    void create_command_get_charge_point_id(std::vector<std::uint8_t>& command_data);
    void create_command_set_charge_point_id(app_layer::UserIdType id_type, std::string id_data,
                                            std::vector<std::uint8_t>& command_data);

    void create_command_get_log_stats(std::vector<std::uint8_t>& command_data);
    void create_command_get_log_entry(std::uint32_t log_entry_id, std::vector<std::uint8_t>& command_data);
    void create_command_get_last_log_entry(std::vector<std::uint8_t>& command_data);
    void create_command_get_log_entry_reverse(std::uint32_t log_entry_id, std::vector<std::uint8_t>& command_data);

    void create_command_get_application_mode(std::vector<std::uint8_t>& command_data);
    void create_command_set_application_mode(app_layer::ApplicationBoardMode mode,
                                             std::vector<std::uint8_t>& command_data);

    void create_command_get_line_loss_impedance(std::vector<std::uint8_t>& command_data);
    void create_command_set_line_loss_impedance(std::uint16_t ll_impedance, std::vector<std::uint8_t>& command_data);

    void create_command_get_server_id(std::vector<std::uint8_t>& command_data);
    void create_command_get_serial_number(std::vector<std::uint8_t>& command_data);
    void create_command_get_hardware_version(std::vector<std::uint8_t>& command_data);
    void create_command_get_device_type(std::vector<std::uint8_t>& command_data);
    void create_command_get_bootloader_version(std::vector<std::uint8_t>& command_data);
    void create_command_get_status_word(std::vector<std::uint8_t>& command_data);
    void create_command_get_application_fw_version(std::vector<std::uint8_t>& command_data);
    void create_command_get_application_fw_checksum(std::vector<std::uint8_t>& command_data);
    void create_command_get_application_fw_hash(std::vector<std::uint8_t>& command_data);
    void create_command_get_metering_fw_version(std::vector<std::uint8_t>& command_data);
    void create_command_get_metering_fw_checksum(std::vector<std::uint8_t>& command_data);
    void create_command_get_metering_mode(std::vector<std::uint8_t>& command_data);

    int8_t
    get_utc_offset_in_quarter_hours(const std::chrono::time_point<std::chrono::system_clock>& timepoint_system_clock);

private:
    std::vector<std::uint8_t> create_command(app_layer::Command cmd);
    std::vector<std::uint8_t> create_simple_command(app_layer::CommandType cmd_type);
};

} // namespace app_layer
#endif // APP_LAYER
