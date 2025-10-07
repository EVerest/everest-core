// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <fusion_charger/modbus/extensions/unsolicitated_registers.hpp>
#include <modbus-registers/registers.hpp>
#include <modbus-registers/registry.hpp>
#include <stdexcept>

#include <ostream>

namespace fusion_charger {
namespace modbus_driver {
namespace raw_registers {

using namespace modbus::registers::data_providers;
using namespace modbus::registers::complex_registers;
using namespace modbus::registers::converters;
using namespace modbus::registers::registry;
using namespace modbus_extensions;

class CommonDispenserRegisters : public ComplexRegisterSubregistry {
public:
    struct DataProviders {
        DataProvider<uint16_t>& manufacturer;
        DataProvider<uint16_t>& model;
        DataProvider<uint16_t>& protocol_version;
        DataProvider<uint16_t>& hardware_version;
        DataProviderString<48>& software_version;
    };

    CommonDispenserRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint16_t>(0x0000, data_providers.manufacturer,     ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x0001, data_providers.model,            ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x0002, data_providers.protocol_version, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x0004, data_providers.hardware_version, ConverterABCD::instance()));
    this->add(new StringRegister    <48>      (0x0013, data_providers.software_version, ConverterIdentity::instance()));
        // clang-format on
    }
};

class CommonPowerUnitRegisters : public ComplexRegisterSubregistry {
public:
    struct DataProviders {
        DataProvider<uint16_t>& manufacturer;
        DataProvider<uint16_t>& protocol_version;
        DataProvider<uint16_t>& hardware_version;
        DataProviderString<48>& software_version;
        DataProviderString<32>& esn_control_board;
    };

    CommonPowerUnitRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint16_t>(0x0100, data_providers.manufacturer, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x0101, data_providers.protocol_version, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x0102, data_providers.hardware_version, ConverterABCD::instance()));
    this->add(new StringRegister    <48>      (0x0103, data_providers.software_version, ConverterIdentity::instance()));
    this->add(new StringRegister    <32>      (0x011B, data_providers.esn_control_board, ConverterIdentity::instance()));
        // clang-format on
    };
};

class CollectedDispenserRegisters : public ComplexRegisterSubregistry {
public:
    struct DataProviders {
        DataProvider<uint16_t>& charging_connectors_count;
        DataProviderString<22>& esn_dispenser;
        DataProviderUnsolicitated<uint32_t>& time_sync; // todo: what should this name be? The docs have a
                                                        // really long name!
    };

    CollectedDispenserRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint16_t>(0x1015, data_providers.charging_connectors_count, ConverterABCD::instance()));
    this->add(new StringRegister    <22>      (0x1016, data_providers.esn_dispenser, ConverterIdentity::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x1024, data_providers.time_sync, ConverterABCD::instance()));
        // clang-format on
    }
};

enum class ConnectorOffset : uint16_t {
    CONNECTOR_1_OFFSET = 0x0000,
    CONNECTOR_2_OFFSET = 0x0C00,
    CONNECTOR_3_OFFSET = 0x0D00,
    CONNECTOR_4_OFFSET = 0x0E00,
};

static ConnectorOffset offset_from_connector_number(uint16_t connector_number) {
    switch (connector_number) {
    case 1:
        return ConnectorOffset::CONNECTOR_1_OFFSET;
    case 2:
        return ConnectorOffset::CONNECTOR_2_OFFSET;
    case 3:
        return ConnectorOffset::CONNECTOR_3_OFFSET;
    case 4:
        return ConnectorOffset::CONNECTOR_4_OFFSET;
    default:
        throw std::runtime_error("Invalid connector number");
    }
}

enum class ConnectorType : uint16_t {
    CCS1 = 0x0001,
    CCS2 = 0x0002,
    CHAdeMO = 0x0003,
    GB = 0x0004,
};

enum class WorkingStatus : uint16_t {
    STANDBY = 0,
    STANDBY_WITH_CONNECTOR_INSERTED = 1,
    CHARGING = 3,
    CHARGING_COMPLETE = 4,
    FAULT = 5,
    DISPENSER_UPGRADE = 7,
    CHARGING_STARTING = 8,
};

enum class ConnectionStatus : uint16_t {
    NOT_CONNECTED = 0,
    SEMI_CONNECTED = 1, // not compatible with ccs2
    FULL_CONNECTED = 2,
};

enum class PsuOutputPortAvailability : uint16_t {
    NOT_AVAILABLE = 0,
    AVAILABLE = 1,
};

std::string working_status_to_string(const WorkingStatus& status);

class CollectedConnectorRegisters : public ComplexRegisterSubregistry {
public:
    enum class ContactorStatus : uint16_t {
        OFF = 0,
        ON = 1,
    };
    enum class ElectronicLockStatus : uint16_t {
        UNLOCKED = 0,
        LOCKED = 1,
    };
    enum class ChargingEventConnector : uint16_t {
        START_TO_STOP = 0,
        STOP_TO_START = 1,
    };

    struct DataProviders {
        DataProvider<double>& total_energy_charged;
        DataProvider<ConnectorType>& connector_type;
        DataProvider<float>& maximum_rated_charge_current;
        DataProvider<float>& output_voltage;
        DataProvider<float>& output_current;
        DataProviderUnsolicitated<WorkingStatus>& working_status;
        DataProviderUnsolicitated<ConnectionStatus>& connection_status;
        DataProvider<uint16_t>& connector_no; // 1-12
        DataProvider<float>& contactor_upstream_voltage;
        DataProviderMemory<6>& mac_address;
        // 0 off, 1 on
        DataProviderUnsolicitated<ContactorStatus>& contactor_status;
        DataProviderUnsolicitated<ElectronicLockStatus>& electronic_lock_status;
        DataProviderUnsolicitated<ChargingEventConnector>& charging_event_connector; // todo ??
    };

    CollectedConnectorRegisters(ConnectorOffset connector, const DataProviders& data_providers) {
        uint16_t offset = static_cast<uint16_t>(connector);
        // clang-format off
    this->add(new ElementaryRegister<double>           (0x1100 + offset, data_providers.total_energy_charged,           ConverterABCD::instance()));
    this->add(new ElementaryRegister<ConnectorType>    (0x1104 + offset, data_providers.connector_type,                 ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>            (0x1105 + offset, data_providers.maximum_rated_charge_current, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>            (0x1107 + offset, data_providers.output_voltage,                 ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>            (0x1109 + offset, data_providers.output_current,                 ConverterABCD::instance()));
    this->add(new ElementaryRegister<WorkingStatus>    (0x110B + offset, data_providers.working_status,                 ConverterABCD::instance()));
    this->add(new ElementaryRegister<ConnectionStatus> (0x110D + offset, data_providers.connection_status,              ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>         (0x110E + offset, data_providers.connector_no,                   ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>            (0x1113 + offset, data_providers.contactor_upstream_voltage,     ConverterABCD::instance()));
    this->add(new MemoryRegister    <6>                (0x114D + offset, data_providers.mac_address,                    ConverterIdentity::instance()));
    this->add(new ElementaryRegister<ContactorStatus>         (0x1154 + offset, data_providers.contactor_status,               ConverterABCD::instance()));
    this->add(new ElementaryRegister<ElectronicLockStatus>         (0x1156 + offset, data_providers.electronic_lock_status,         ConverterABCD::instance()));
    this->add(new ElementaryRegister<ChargingEventConnector>         (0x117E + offset, data_providers.charging_event_connector,       ConverterABCD::instance()));
        // clang-format on
    }
};

class SettingPowerUnitRegisters : public ComplexRegisterSubregistry {
public:
    enum class PSURunningMode : uint16_t {
        STARTING_UP = 0,
        RUNNING = 1,
        FAULTY = 2,
        SLEEPING = 3,
        UPGRADING = 4,
    };

    static std::string psu_running_mode_to_string(const PSURunningMode& mode);

    struct DataProviders {
        DataProvider<PSURunningMode>& psu_running_mode;
        DataProviderMemory<6>& psu_mac;
        DataProvider<float>& ac_input_voltage_a;
        DataProvider<float>& ac_input_voltage_b;
        DataProvider<float>& ac_input_voltage_c;
        DataProvider<float>& ac_input_current_a;
        DataProvider<float>& ac_input_current_b;
        DataProvider<float>& ac_input_current_c;
        DataProvider<double>& total_historic_input_energy;
    };

    SettingPowerUnitRegisters(const DataProviders& data_provider) {
        // clang-format off
    this->add(new ElementaryRegister<PSURunningMode>(0x2006, data_provider.psu_running_mode, ConverterABCD::instance()));
    this->add(new MemoryRegister    <6>             (0x2111, data_provider.psu_mac, ConverterIdentity::instance()));
    this->add(new ElementaryRegister<float>         (0x2007, data_provider.ac_input_voltage_a, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>         (0x2009, data_provider.ac_input_voltage_b, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>         (0x200B, data_provider.ac_input_voltage_c, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>         (0x200D, data_provider.ac_input_current_a, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>         (0x200F, data_provider.ac_input_current_b, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>         (0x2011, data_provider.ac_input_current_c, ConverterABCD::instance()));
    this->add(new ElementaryRegister<double>        (0x2013, data_provider.total_historic_input_energy, ConverterABCD::instance()));
        // clang-format on
    };
};

class SettingConnectorRegisters : public ComplexRegisterSubregistry {
public:
    struct DataProviders {
        DataProvider<float>& max_rated_psu_voltage;
        DataProvider<float>& max_rated_psu_current;
        DataProvider<float>& min_rated_psu_voltage;
        DataProvider<float>& min_rated_psu_current;
        DataProvider<float>& rated_output_power_connector; // kW
        DataProviderMemory<48>& hmac_key;
        DataProvider<float>& rated_output_power_psu; // kW
        DataProvider<PsuOutputPortAvailability>& psu_port_available;
    };

    SettingConnectorRegisters(ConnectorOffset connector, const DataProviders& data_providers) {
        uint16_t offset = static_cast<uint16_t>(connector);

        // clang-format off
    this->add(new ElementaryRegister<float>                 (offset + 0x2100, data_providers.max_rated_psu_voltage, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>                 (offset + 0x2102, data_providers.max_rated_psu_current, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>                 (offset + 0x2105, data_providers.min_rated_psu_voltage, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>                 (offset + 0x2107, data_providers.min_rated_psu_current, ConverterABCD::instance()));
    this->add(new ElementaryRegister<float>                 (offset + 0x2109, data_providers.rated_output_power_connector, ConverterABCD::instance()));
    this->add(new MemoryRegister    <48>                    (offset + 0x2115, data_providers.hmac_key, ConverterIdentity::instance()));
    this->add(new ElementaryRegister<float>                 (offset + 0x212D, data_providers.rated_output_power_psu, ConverterABCD::instance()));
    this->add(new ElementaryRegister<PsuOutputPortAvailability>(offset + 0x212F, data_providers.psu_port_available, ConverterABCD::instance()));
        // clang-format on
    }
};

struct AlarmDispenserRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProviderUnsolicitated<uint16_t>& door_status_alarm;
        DataProviderUnsolicitated<uint16_t>& water_alarm;
        DataProviderUnsolicitated<uint16_t>& epo_alarm;
        DataProviderUnsolicitated<uint16_t>& tilt_alarm;
    };

    AlarmDispenserRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint16_t>(0x3001, data_providers.door_status_alarm, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x3002, data_providers.water_alarm,       ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x3003, data_providers.epo_alarm,         ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(0x3004, data_providers.tilt_alarm,        ConverterABCD::instance()));
        // clang-format on
    }
};

struct AlarmConnectorRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProviderUnsolicitated<uint16_t>& dc_output_contact_fault;
        DataProviderUnsolicitated<uint16_t>& inverse_connection_dispenser_inlet_cable;
    };

    AlarmConnectorRegisters(ConnectorOffset connector, const DataProviders& data_providers) {
        uint16_t offset = static_cast<uint16_t>(connector);

        // clang-format off
    this->add(new ElementaryRegister<uint16_t>(offset + 0x3105, data_providers.dc_output_contact_fault,                  ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>(offset + 0x3115, data_providers.inverse_connection_dispenser_inlet_cable, ConverterABCD::instance()));
        // clang-format on
    }
};

enum class AlarmStatus : uint16_t {
    NORMAL = 0,
    ALARM = 1,
};

inline std::ostream& operator<<(std::ostream& os, const AlarmStatus& status) {
    switch (status) {
    case AlarmStatus::NORMAL:
        os << "NORMAL";
        break;
    case AlarmStatus::ALARM:
        os << "ALARM";
        break;
    default:
        os << "UNKNOWN";
        break;
    }
    return os;
}

struct AlarmPowerUnitRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<AlarmStatus>& high_voltage_door_status_sensor;
        DataProvider<AlarmStatus>& door_status_sensor;
        DataProvider<AlarmStatus>& water;
        DataProvider<AlarmStatus>& smoke;
        DataProvider<AlarmStatus>& epo;
    };

    AlarmPowerUnitRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<AlarmStatus>(0x4000, data_providers.high_voltage_door_status_sensor, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4001, data_providers.door_status_sensor, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4002, data_providers.water, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4003, data_providers.smoke, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4004, data_providers.epo, ConverterABCD::instance()));
        // clang-format on
    }
};

struct AlarmChargingPowerUnitRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<AlarmStatus>& unknown_system_type;
        DataProvider<AlarmStatus>& power_detection_exception;
        DataProvider<AlarmStatus>& syncrhonization_cable_status_fault_of_energy_routing_board;
        DataProvider<AlarmStatus>& soft_start_fault;
        DataProvider<AlarmStatus>& soft_start_module_communication_failure;
        DataProvider<AlarmStatus>& soft_start_module_overloaded;
        DataProvider<AlarmStatus>& soft_start_module_fault;
        DataProvider<AlarmStatus>& soft_start_module_overtemperature;
        DataProvider<AlarmStatus>& soft_start_module_undertemperature;
        DataProvider<AlarmStatus>& soft_start_module_disconnection_failure;
        DataProvider<AlarmStatus>& phase_sequence_abornmal_alarm;
        DataProvider<AlarmStatus>& power_distribution_module_communication_failure;
        DataProvider<AlarmStatus>& fault_of_insulation_resistance_to_ground;
        DataProvider<uint16_t>& modbus_tcp_certificate;
    };

    AlarmChargingPowerUnitRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<AlarmStatus>(0x4005, data_providers.unknown_system_type, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4006, data_providers.power_detection_exception, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4007, data_providers.syncrhonization_cable_status_fault_of_energy_routing_board, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4008, data_providers.soft_start_fault, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4009, data_providers.soft_start_module_communication_failure, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400A, data_providers.soft_start_module_overloaded, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400B, data_providers.soft_start_module_fault, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400C, data_providers.soft_start_module_overtemperature, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400D, data_providers.soft_start_module_undertemperature, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400E, data_providers.soft_start_module_disconnection_failure, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x400F, data_providers.phase_sequence_abornmal_alarm, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4010, data_providers.power_distribution_module_communication_failure, ConverterABCD::instance()));
    this->add(new ElementaryRegister<AlarmStatus>(0x4011, data_providers.fault_of_insulation_resistance_to_ground, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint16_t>   (0x4012, data_providers.modbus_tcp_certificate, ConverterABCD::instance()));
        // clang-format on
    }
};

struct AlarmAcBranchRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<uint32_t>& ac_branch_1;
        DataProvider<uint32_t>& ac_branch_2;
    };

    AlarmAcBranchRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint32_t>(0x4020, data_providers.ac_branch_1, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4022, data_providers.ac_branch_2, ConverterABCD::instance()));
        // clang-format on
    }
};

struct AlarmAcDcRectifierRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<uint32_t>& rectifier_1;
        DataProvider<uint32_t>& rectifier_2;
        DataProvider<uint32_t>& rectifier_3;
        DataProvider<uint32_t>& rectifier_4;
        DataProvider<uint32_t>& rectifier_5;
        DataProvider<uint32_t>& rectifier_6;
    };

    AlarmAcDcRectifierRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint32_t>(0x4040, data_providers.rectifier_1, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4042, data_providers.rectifier_2, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4044, data_providers.rectifier_3, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4046, data_providers.rectifier_4, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4048, data_providers.rectifier_5, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x404A, data_providers.rectifier_6, ConverterABCD::instance()));
        // clang-format on
    }
};

struct DcDcChargingModuleRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<uint32_t>& dc_dc_charging_module_1;
        DataProvider<uint32_t>& dc_dc_charging_module_2;
        DataProvider<uint32_t>& dc_dc_charging_module_3;
        DataProvider<uint32_t>& dc_dc_charging_module_4;
        DataProvider<uint32_t>& dc_dc_charging_module_5;
        DataProvider<uint32_t>& dc_dc_charging_module_6;
        DataProvider<uint32_t>& dc_dc_charging_module_7;
        DataProvider<uint32_t>& dc_dc_charging_module_8;
        DataProvider<uint32_t>& dc_dc_charging_module_9;
        DataProvider<uint32_t>& dc_dc_charging_module_10;
        DataProvider<uint32_t>& dc_dc_charging_module_11;
        DataProvider<uint32_t>& dc_dc_charging_module_12;
    };

    DcDcChargingModuleRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint32_t>(0x4070, data_providers.dc_dc_charging_module_1, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4072, data_providers.dc_dc_charging_module_2, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4074, data_providers.dc_dc_charging_module_3, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4076, data_providers.dc_dc_charging_module_4, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4078, data_providers.dc_dc_charging_module_5, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x407A, data_providers.dc_dc_charging_module_6, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x407C, data_providers.dc_dc_charging_module_7, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x407E, data_providers.dc_dc_charging_module_8, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4080, data_providers.dc_dc_charging_module_9, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4082, data_providers.dc_dc_charging_module_10, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4084, data_providers.dc_dc_charging_module_11, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x4086, data_providers.dc_dc_charging_module_12, ConverterABCD::instance()));
        // clang-format on
    }
};

struct CoolingSectionRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<uint32_t>& cooling_unit_1;
    };

    CoolingSectionRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint32_t>(0x40D0, data_providers.cooling_unit_1, ConverterABCD::instance()));
        // clang-format on
    }
};

struct PowerDistributionModuleRegisters : public ComplexRegisterSubregistry {
    struct DataProviders {
        DataProvider<uint32_t>& power_distribution_module_1;
        DataProvider<uint32_t>& power_distribution_module_2;
        DataProvider<uint32_t>& power_distribution_module_3;
        DataProvider<uint32_t>& power_distribution_module_4;
        DataProvider<uint32_t>& power_distribution_module_5;
    };

    PowerDistributionModuleRegisters(const DataProviders& data_providers) {
        // clang-format off
    this->add(new ElementaryRegister<uint32_t>(0x40E0, data_providers.power_distribution_module_1, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x40E2, data_providers.power_distribution_module_2, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x40E4, data_providers.power_distribution_module_3, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x40E6, data_providers.power_distribution_module_4, ConverterABCD::instance()));
    this->add(new ElementaryRegister<uint32_t>(0x40E8, data_providers.power_distribution_module_5, ConverterABCD::instance()));
        // clang-format on
    }
};

}; // namespace raw_registers
}; // namespace modbus_driver
}; // namespace fusion_charger
