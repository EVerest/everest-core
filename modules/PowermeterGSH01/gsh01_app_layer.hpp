// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 This is an implementation for the AST powermeter application layer
*/
#ifndef AST_APP_LAYER
#define AST_APP_LAYER

#include <everest/logging.hpp>
#include <optional>
#include <chrono>
#include <stdint.h>
#include "ld-ev.hpp"
#include <generated/types/powermeter.hpp>

#include "crc16.hpp"

namespace ast_app_layer {

enum class CommandType : std::uint16_t {
    RESET_DC_METER = 0x4101,                            // reset the DC meter
    AB_MODE_SET = 0x4102,                               // get or set the mode of application board  
    AB_SERVER_ID = 0x4110,                              // get or set the server ID of application board
    AB_SERIAL_NR = 0x4111,                              // get or set the serial number
    AB_HW_VERSION = 0x4112,                             // get or set the hardware version
    AB_DEVICE_TYPE = 0x4113,                            // get or set the device type 
    AB_SW_VERSION = 0x4114,                             // get the software version of application board
    AB_FW_CHECKSUM = 0x4115,                            // get the checksum (crc16) of application board
    MB_SW_VERSION = 0x4116,                             // get the software version of metering board 
    MB_FW_CHECKSUM = 0x4117,                            // get the crc16 from software integrity check of metering board
    AB_FW_HASH = 0x4119,                                // get the firmware hash(crc16) of application board
    MEASUREMENT_MODE = 0x4120,                          // get the measurement mode
    GET_NORMAL_VOLTAGE = 0x4121,                        // get the normal voltage
    GET_NORMAL_CURRENT = 0x4122,                        // get the normal current
    GET_MAX_CURRENT = 0x4123,                           // get the maximum current
    LINE_LOSS_IMPEDANCE = 0x4130,                       // get or set the line loss impedance 
    LINE_LOSS_MEAS_MODE = 0x4131,                       // get the line loss energy measurement mode
    TIME = 0x4135,                                      // get or set the device time 
    TEMPERATURE = 0x4136,                               // get the temperature
    AB_STATUS = 0x4137,                                 // get the statusword of application board
    METER_BUS_ADDR = 0x4212,                            // get or set the meter bus address 
    START_TRANSACTION = 0x1501,                         // start transaction
    STOP_TRANSACTION = 0x1502,                          // stop transaction 
    OCMF_STATS = 0x1510,                                // get the statistics of OCMF 
    GET_OCMF = 0x1520,                                  // get the OCMF 
    GET_LAST_OCMF = 0x1521,                             // get last OCMF
    GET_OCMF_REVERSE = 0x1522,                          // get the OCMF reverse 
    GET_PUBKEY_ASN1 = 0x1530,                           // get public key in ASN1 format
    CHARGE_POINT_ID = 0x1531,                           // get or set the charge point identification 
    GET_PUBKEY_BIN = 0x1532,                            // get public key as binary string
    GET_TRANSACT_IMPORT_LINE_LOSS_ENERGY = 0x1540,      // get total transaction import line loss energy
    GET_TRANSACT_EXPORT_LINE_LOSS_ENERGY = 0x1541,      // get total transaction export line loss energy
    GET_TRANSACT_TOTAL_IMPORT_DEV_ENERGY = 0x1550,      // get total transaction import device energy 
    GET_TRANSACT_TOTAL_EXPORT_DEV_ENERGY = 0x1551,      // get total transaction export device energy 
    GET_TRANSACT_TOTAL_IMPORT_MAINS_ENERGY = 0x1560,    // get total transaction import mains energy
    GET_TRANSACT_TOTAL_EXPORT_MAINS_ENERGY = 0x1561,    // get total transaction export mains energy
    GET_TRANSACT_TOTAL_DURATION = 0x156A,               // get total transaction duration.
    GET_TOTAL_START_IMPORT_LINE_LOSS_ENERGY = 0x1570,   // get total start import line loss energy
    GET_TOTAL_START_EXPORT_LINE_LOSS_ENERGY = 0x1571,   // get total start export line loss energy
    GET_TOTAL_START_IMPORT_DEV_ENERGY = 0x1580,         // get total start import device energy 
    GET_TOTAL_START_EXPORT_DEV_ENERGY = 0x1581,         // get total start export device energy 
    GET_TOTAL_START_IMPORT_MAINS_ENERGY = 0x1590,       // get total start import mains energy
    GET_TOTAL_START_EXPORT_MAINS_ENERGY = 0x1591,       // get total start export mains energy
    GET_TOTAL_STOP_IMPORT_LINE_LOSS_ENERGY = 0x15A0,    // get total stop import line loss energy 
    GET_TOTAL_STOP_EXPORT_LINE_LOSS_ENERGY = 0x15A1,    // get total stop export line loss energy 
    GET_TOTAL_STOP_IMPORT_DEV_ENERGY = 0x15B0,          // get total stop import device energy
    GET_TOTAL_STOP_EXPORT_DEV_ENERGY = 0x15B1,          // get total stop export device energy
    GET_TOTAL_STOP_IMPORT_MAINS_ENERGY = 0x15C0,        // get total stop import mains energy 
    GET_TOTAL_STOP_EXPORT_MAINS_ENERGY = 0x15C1,        // get total stop export mains energy
    GET_LOG_STATS = 0x1710,                             // get the statistic of log
    GET_LOG_ENTRY = 0x1720,                             // get the log entry
    GET_LAST_LOG_ENTRY = 0x1721,                        // get last log entry 
    GET_LOG_ENTRY_REVERSE = 0x1722,                     // get the log entry reverse
    REGISTER_DISPLAY_PUBKEY = 0x1801,                   // register display public key
    REQUEST_CHALLENGE = 0x1810,                         // request challenge
    SET_SIGNED_CHALLENGE = 0x1811,                      // set signed challenge
    REQUEST_METER_PUBKEY = 0x1830,                      // request meter public key 
    GET_TOTAL_IMPORT_MAINS_ENERGY = 0x0110,             // get total import mains energy
    GET_TOTAL_EXPORT_MAINS_ENERGY = 0x0120,             // get total export mains energy
    GET_CURRENT_L1 = 0x0131,                            // get current (phase L1) 
    GET_VOLTAGE_L1 = 0x0132,                            // get voltage (phase L1) 
    GET_TOTAL_IMPORT_MAINS_POWER = 0x0133,              // total import mains power
    GET_TOTAL_EXPORT_MAINS_POWER = 0x0134,              // total export mains power
    GET_DEV_VOLTAGE_L1 = 0x0137,                        // get device voltage (phase L1)
    GET_IMPORT_DEV_POWER = 0x0138,                      // get import device power
    GET_EXPORT_DEV_POWER = 0x0139,                      // get export device power
    GET_IMPORT_LINE_LOSS_POWER = 0x013A,                // get import line loss power 
    GET_EXPORT_LINE_LOSS_POWER = 0x013B,                // get export line loss power 
    GET_TOTAL_DEV_POWER = 0x013C,                       // get total device power 
    GET_TOTAL_IMPORT_LINE_LOSS_ENERGY = 0x0160,         // get the total import line loss energy
    GET_TOTAL_EXPORT_LINE_LOSS_ENERGY = 0x0163,         // get the total export line loss energy
    GET_TOTAL_IMPORT_DEV_ENERGY = 0x0170,               // get the total import device energy 
    GET_TOTAL_EXPORT_DEV_ENERGY = 0x0173,               // get the total export device energy
    GET_SECOND_INDEX = 0x0180,                          // get the second index 
    GET_PUBKEY_STR16 = 0x0211,                          // get the public key in string format (base16) 
    GET_PUBKEY_STR32 = 0x0212,                          // get the public key in string format (base32) 
    GET_PUBKEY_CSTR16 = 0x0213,                         // get the public key compressed in string format (base16)
    GET_PUBKEY_CSTR32 = 0x0214,                         // get the public key compressed in string format (base32)
    REPEAT_DATA = 0xA000,                               // repeat data
    OCMF_INFO = 0xA001,                                 // get or set OCMF info 
    OCMF_CONFIG = 0xA002,                               // get or set the table of OCMF configuration field 
    GET_ERRORS = 0xA004,                                // get errors 
    AB_DMC = 0xA006,                                    // get or set the DMC application board 
    AB_PROD_DATE = 0xA00B,                              // get or set the production date of application board
    SET_REQUEST_CHALLENGE = 0xA00C                      // enable/disable the request challenge
};

enum class CommandStatus : std::uint8_t {
    OK                          = 0,    // no error
    GENERAL_ERROR               = 1,    // internal process has an error
    OUT_OF_RANGE                = 2,    // data is out of range
    SECURITY_ACCES_DENIED       = 3,    // the current mode of the slave cannot start this command
    REJECTED                    = 4,    // error of communication between application and metering board
    LOGICAL_ADDRESS_NOT_FOUND   = 5,    // command not implemented
    FORMAL_INVALID              = 6,    // length in bytes is incorrect; e.g. received more data than expected
    NOT_AVAILABLE               = 7,    // data not available
    IS_BUSY                     = 8,    // slave cannot process command; already processing long operation
    PUBKEY_MISSING              = 9     // public key is not stored in internal memory
};

enum class CommandResult : std::uint8_t {
    OK = 0,
    GENERAL_ERROR = 1,                  // read ERROR register (read with GET_ERRORS[0xA004]) for more information
    OUT_OF_RANGE = 2,                   //
    SECURITY_ACCESS_DENIED = 3,         // application board is not in production mode
    REJECTED = 4,                       // metering board is not responding
    FORMAT_INVALID = 6,                 // payload is not empty
    NOT_AVAILABLE = 7,                  // data could not be read
    BUSY = 8,                           // transaction ongoing, metering board unavailable
    PUBLIC_KEY_MISSING = 9,             //
    PROTOCOL_ERROR = 250,               // error on reception at host: protocol error (SLIP protocol)
    TIMEOUT = 251,                      // no response during at least 1100 ms
    COMMUNICATION_FAILED = 254,         // error on communication between PM and host device
    PENDING = 255                       // special state for transaction commands
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
    NONE         = 0,   // not available
    DENIED       = 1,   // not retrievable (e.g. two-factor-auth)
    UNDEFINED    = 2,   // type unknown / other
    ISO14443     = 10,  // UID of RFID card according to ISO14443 (4 or 7 bytes HEX)
    ISO15693     = 11,  // UID of RFID card according to ISO15693 (8 bytes HEX)
    EMAID        = 20,  // Electro-Mobility-Account-ID according to ISO/IEC15118 (14 or 15 bytes string)
    EVCCID       = 21,  // ID of an EV according to ISO/IEC15118 (max 6 bytes)
    EVCOID       = 30,  // EV-Contract-ID according to DIN91286
    ISO7812      = 40,  // Identification-Card-Format according to ISO/IEC7812 (credit-/banking-cards, etc.)
    CAR_TXN_NR   = 50,  // Card-Transaction-Number (CardTxNbr) for credit- or banking-cards
    CENTRAL      = 60,  // centrally generated ID (no fixed format, e.g. UUID); OCPP 2.0
    CENTRAL_1    = 61,  // centrally generated ID (no fixed format, e.g. start-via-SMS); (up to) OCPP 1.6
    CENTRAL_2    = 62,  // centrally generated ID (no fixed format, e.g. start-by-operator); (up to) OCPP 1.6
    LOCAL        = 70,  // locally generated ID (no fixed format, e.g. UUID); OCPP 2.0
    LOCAL_1      = 71,  // locally generated ID (no fixed format, e.g. generated by chargepoint); (up to) OCPP 1.6
    LOCAL_2      = 72,  // locally generated ID (no fixed format, other); (up to) OCPP 1.6
    PHONE_NUMBER = 80,  // international phone number (leading '+' with country code)
    KEY_CODE     = 90   // private user key (no fixed format)
};

inline ast_app_layer::UserIdType user_id_type_conversion_everest_to_ast(types::powermeter::UserIdType e) {
    switch (e) {
        case types::powermeter::UserIdType::None: return ast_app_layer::UserIdType::NONE;
        case types::powermeter::UserIdType::Denied: return ast_app_layer::UserIdType::DENIED;
        case types::powermeter::UserIdType::Undefined: return ast_app_layer::UserIdType::UNDEFINED;
        case types::powermeter::UserIdType::Iso14443: return ast_app_layer::UserIdType::ISO14443;
        case types::powermeter::UserIdType::Iso15693: return ast_app_layer::UserIdType::ISO15693;
        case types::powermeter::UserIdType::Emaid: return ast_app_layer::UserIdType::EMAID;
        case types::powermeter::UserIdType::Evccid: return ast_app_layer::UserIdType::EVCCID;
        case types::powermeter::UserIdType::Evcoid: return ast_app_layer::UserIdType::EVCOID;
        case types::powermeter::UserIdType::Iso7812: return ast_app_layer::UserIdType::ISO7812;
        case types::powermeter::UserIdType::Card_Tx_Nbr: return ast_app_layer::UserIdType::CAR_TXN_NR;
        case types::powermeter::UserIdType::Central: return ast_app_layer::UserIdType::CENTRAL;
        case types::powermeter::UserIdType::Central_1: return ast_app_layer::UserIdType::CENTRAL_1;
        case types::powermeter::UserIdType::Central_2: return ast_app_layer::UserIdType::CENTRAL_2;
        case types::powermeter::UserIdType::Local: return ast_app_layer::UserIdType::LOCAL;
        case types::powermeter::UserIdType::Local_1: return ast_app_layer::UserIdType::LOCAL_1;
        case types::powermeter::UserIdType::Local_2: return ast_app_layer::UserIdType::LOCAL_2;
        case types::powermeter::UserIdType::Phone_Number: return ast_app_layer::UserIdType::PHONE_NUMBER;
        case types::powermeter::UserIdType::Key_Code: return ast_app_layer::UserIdType::KEY_CODE;
    }

    throw std::out_of_range("No known AST type conversion for provided enum of types::powermeter::UserIdType");
}


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
    ASSEMBLY = 1,
    // PRODUCTION = 2
};

enum class LogType : std::uint8_t {
    NONE                        = 0,
    LINE_LOSS_MEASUREMENT_MODE  = 1,
    IMPEDANCE_CHANGED           = 2,
    OPERATION_MODE_CHANGED      = 3,
    ASSEMBLY_CONFIG_CHANGED     = 4,
    FATAL_ERROR_EVENT           = 5,
    TIME_DELTA_TOO_BIG_EVENT    = 6,
    CHARGE_POINT_ID_CHANGED     = 7,
    EXTERNAL_DISPLAY_PAIRED     = 8,
    EXTERNAL_DISPLAY_FAILURE    = 9,
    CHARGE_DATA_OUT_OF_MEMORY   = 10,
    LOG_DATA_OUT_OF_MEMORY      = 11,
    FW_VERSION_CHANGED          = 12,
    PULSE_LED_SOURCE_CHANGED    = 13
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
    ast_app_layer::LogType type;
    uint32_t second_index{};
    uint32_t utc_time{};
    uint8_t utc_offset{};
    std::vector<uint8_t> old_value;  // max. 10 elements
    std::vector<uint8_t> new_value;  // max. 10 elements
    std::vector<uint8_t> server_id;  // 10 elements
    std::vector<uint8_t> signature;  // 64 elements
};

class Command {
public:
    ast_app_layer::CommandType type;
    uint16_t length;
    ast_app_layer::CommandStatus status;
    std::vector<uint8_t> data;
};

static constexpr uint16_t PM_AST_MAX_RX_LENGTH = 1000;
static constexpr uint16_t PM_AST_SERIAL_RX_INITIAL_TIMEOUT_MS = 1100;
static constexpr uint16_t PM_AST_SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS = 100;

class AstAppLayer {

public:
    AstAppLayer() = default;
    ~AstAppLayer() = default;

    void create_command_start_transaction(ast_app_layer::UserIdStatus user_id_status,
                                          ast_app_layer::UserIdType user_id_type,
                                          std::string user_id_data,
                                          int8_t gmt_offset_quarter_hours,
                                          std::vector<uint8_t>& command_data);
    void create_command_stop_transaction(std::vector<uint8_t>& command_data);

    void create_command_get_time(std::vector<uint8_t>& command_data);
    void create_command_set_time(date::utc_clock::time_point timepoint,
                                 int8_t gmt_offset_quarters_of_an_hour,
                                 std::vector<uint8_t>& command_data);

    void create_command_get_voltage(std::vector<uint8_t>& command_data);
    void create_command_get_current(std::vector<uint8_t>& command_data);
    void create_command_get_import_power(std::vector<uint8_t>& command_data);
    void create_command_get_export_power(std::vector<uint8_t>& command_data);
    void create_command_get_total_power(std::vector<uint8_t>& command_data);

    void create_command_get_total_start_import_energy(std::vector<uint8_t>& command_data);
    void create_command_get_total_stop_import_energy(std::vector<uint8_t>& command_data);
    void create_command_get_total_start_export_energy(std::vector<uint8_t>& command_data);
    void create_command_get_total_stop_export_energy(std::vector<uint8_t>& command_data);
    void create_command_get_total_dev_import_energy(std::vector<uint8_t>& command_data);
    void create_command_get_total_dev_export_energy(std::vector<uint8_t>& command_data);

    void create_command_get_total_transaction_duration(std::vector<uint8_t>& command_data);

    void create_command_get_pubkey_str16(std::vector<uint8_t>& command_data);
    void create_command_get_pubkey_asn1(std::vector<uint8_t>& command_data);
    void create_command_get_meter_pubkey(std::vector<uint8_t>& command_data);

    void create_command_get_ocmf_stats(std::vector<uint8_t>& command_data);
    void create_command_get_transaction_ocmf(uint32_t ocmf_id,
                                             std::vector<uint8_t>& command_data);
    void create_command_get_last_transaction_ocmf(std::vector<uint8_t>& command_data);
    void create_command_get_ocmf_info(std::vector<uint8_t>& command_data);
    void create_command_get_ocmf_config(std::vector<uint8_t>& command_data);

    void create_command_get_charge_point_id(std::vector<uint8_t>& command_data);
    void create_command_set_charge_point_id(ast_app_layer::UserIdType id_type,
                                            std::string id_data,
                                            std::vector<uint8_t>& command_data);

    void create_command_get_errors(ast_app_layer::ErrorCategory category,
                                   ast_app_layer::ErrorSource src,
                                   std::vector<uint8_t>& command_data);

    void create_command_get_log_stats(std::vector<uint8_t>& command_data);
    void create_command_get_log_entry(uint32_t log_entry_id,
                                      std::vector<uint8_t>& command_data);
    void create_command_get_last_log_entry(std::vector<uint8_t>& command_data);
    void create_command_get_log_entry_reverse(uint32_t log_entry_id,
                                              std::vector<uint8_t>& command_data);

    void create_command_get_application_board_mode(std::vector<uint8_t>& command_data);
    void create_command_set_application_board_mode(ast_app_layer::ApplicationBoardMode mode,
                                                   std::vector<uint8_t>& command_data);

    void create_command_get_hardware_version(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_server_id(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_serial_number(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_software_version(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_fw_checksum(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_fw_hash(std::vector<uint8_t>& command_data);
    void create_command_get_application_board_status(std::vector<uint8_t>& command_data);
    void create_command_get_metering_board_software_version(std::vector<uint8_t>& command_data);
    void create_command_get_metering_board_fw_checksum(std::vector<uint8_t>& command_data);
    
    void create_command_get_device_type(std::vector<uint8_t>& command_data);

    int8_t get_utc_offset_in_quarter_hours(const std::chrono::time_point<std::chrono::system_clock>& timepoint_system_clock);

private:
    std::vector<uint8_t> create_command(ast_app_layer::Command cmd);
    std::vector<uint8_t> create_simple_command(ast_app_layer::CommandType cmd_type);
};

} // namespace ast_app_layer
#endif // AST_APP_LAYER
