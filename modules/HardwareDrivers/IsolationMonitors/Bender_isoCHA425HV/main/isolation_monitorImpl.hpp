// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef MAIN_ISOLATION_MONITOR_IMPL_HPP
#define MAIN_ISOLATION_MONITOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/isolation_monitor/Implementation.hpp>

#include "../Bender_isoCHA425HV.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int imd_device_id;
    int r1_prealarm_kohm;
    int r2_alarm_kohm;
    bool undervoltage_alarm_enable;
    int undervoltage_alarm_threshold_V;
    bool overvoltage_alarm_enable;
    int overvoltage_alarm_threshold_V;
    bool alarm_memory_enable;
    bool relais_r1_mode;
    bool relais_r2_mode;
    int delay_startup_device;
    int delay_t_on_k1_k2;
    int delay_t_off_k1_k2;
    int automatic_selftest_setting;
    bool chademo_mode;
    bool selftest_enable_gridconnection;
    bool selftest_enable_at_start;
    bool always_publish_measurements;
};

class isolation_monitorImpl : public isolation_monitorImplBase {
public:
    isolation_monitorImpl() = delete;
    isolation_monitorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<Bender_isoCHA425HV>& mod,
                          Conf& config) :
        isolation_monitorImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_start() override;
    virtual void handle_stop() override;
    virtual void handle_start_self_test(double& test_voltage_V) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<Bender_isoCHA425HV>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    enum class ImdRegisters : uint16_t {
        RESISTANCE_R_F_OHM = 1000,
        VOLTAGE_U_N_V = 1008,
        VOLTAGE_U_L1E_V = 1016,
        VOLTAGE_U_L2E_V = 1020,
        RESISTANCE_R_FU_OHM = 1028,
        DEVICE_NAME = 9800,
        DEVICE_FIRMWARE_VERSION = 9820,
    };

    enum class AlarmType {
        NoAlarm,
        PreWarning,
        DeviceError,
        Warning,
        Alarm,
        Reserved,
    };

    std::string to_string(AlarmType a) {
        switch (a) {
        case AlarmType::NoAlarm:
            return "NoAlarm";
        case AlarmType::PreWarning:
            return "PreWarning";
        case AlarmType::DeviceError:
            return "DeviceError";
        case AlarmType::Warning:
            return "Warning";
        case AlarmType::Alarm:
            return "Alarm";
        case AlarmType::Reserved:
            return "Reserved";
        }
        return "Reserved";
    };

    enum class TestType {
        NoTest,
        InternalTest,
        ExternalTest,
    };

    std::string to_string(TestType a) {
        switch (a) {
        case TestType::NoTest:
            return "NoTest";
        case TestType::InternalTest:
            return "InternalTest";
        case TestType::ExternalTest:
            return "ExternalTest";
        }
        return "NoTest";
    };

    enum class ValidType {
        TrueValue,
        TrueValueIsSmaller,
        TrueValueIsBigger,
        Invalid,
    };

    std::string to_string(ValidType a) {
        switch (a) {
        case ValidType::TrueValue:
            return "TrueValue";
        case ValidType::TrueValueIsSmaller:
            return "TrueValueIsSmaller";
        case ValidType::TrueValueIsBigger:
            return "TrueValueIsBigger";
        case ValidType::Invalid:
            return "Invalid";
        }
        return "Invalid";
    };

    enum class UnitType {
        Invalid,
        NoUnit,
        Ohm,
        Ampere,
        Volt,
        Percent,
        Hertz,
        Baud,
        Farad,
        Henry,
        DegC,
        DegF,
        Seconds,
        Minutes,
        Hours,
        Days,
        Months,
    };

    std::string to_string(UnitType a) {
        switch (a) {
        case UnitType::Invalid:
            return "Invalid";
        case UnitType::NoUnit:
            return "NoUnit";
        case UnitType::Ohm:
            return "Ohm";
        case UnitType::Ampere:
            return "Ampere";
        case UnitType::Volt:
            return "Volt";
        case UnitType::Percent:
            return "Percent";
        case UnitType::Hertz:
            return "Hertz";
        case UnitType::Baud:
            return "Baud";
        case UnitType::Farad:
            return "Farad";
        case UnitType::Henry:
            return "Henry";
        case UnitType::DegC:
            return "DegC";
        case UnitType::DegF:
            return "DegF";
        case UnitType::Seconds:
            return "Seconds";
        case UnitType::Minutes:
            return "Minutes";
        case UnitType::Hours:
            return "Hours";
        case UnitType::Months:
            return "Months";
        }
        return "Invalid";
    };

    enum class ChannelDescription {
        Undefined,
        IsolationError,
        IsolationError_rF,
        Voltage,
        UnderVoltage,
        OverVoltage,
        Capacity,
        IsolationError_Zi,
        GridConnection,
        EarthConnection,
        DeviceErrorIsometer,
        DeviceError,
        OwnAddress,
    };

    std::string to_string(ChannelDescription a) {
        switch (a) {
        case ChannelDescription::Undefined:
            return "Undefined";
        case ChannelDescription::IsolationError:
            return "IsolationError";
        case ChannelDescription::IsolationError_rF:
            return "IsolationError_rF";
        case ChannelDescription::Voltage:
            return "Voltage";
        case ChannelDescription::UnderVoltage:
            return "UnderVoltage";
        case ChannelDescription::OverVoltage:
            return "OverVoltage";
        case ChannelDescription::Capacity:
            return "Capacity";
        case ChannelDescription::IsolationError_Zi:
            return "IsolationError_Zi";
        case ChannelDescription::GridConnection:
            return "GridConnection";
        case ChannelDescription::EarthConnection:
            return "EarthConnection";
        case ChannelDescription::DeviceErrorIsometer:
            return "DeviceErrorIsometer";
        case ChannelDescription::DeviceError:
            return "DeviceError";
        case ChannelDescription::OwnAddress:
            return "OwnAddress";
        }
        return "Undefined";
    };

    ChannelDescription to_channel_description(uint16_t raw) {
        switch (raw) {
        case 1:
            return ChannelDescription::IsolationError;
        case 71:
            return ChannelDescription::IsolationError_rF;
        case 76:
            return ChannelDescription::Voltage;
        case 77:
            return ChannelDescription::UnderVoltage;
        case 78:
            return ChannelDescription::OverVoltage;
        case 82:
            return ChannelDescription::Capacity;
        case 86:
            return ChannelDescription::IsolationError_Zi;
        case 101:
            return ChannelDescription::GridConnection;
        case 102:
            return ChannelDescription::EarthConnection;
        case 115:
            return ChannelDescription::DeviceErrorIsometer;
        case 129:
            return ChannelDescription::DeviceError;
        case 145:
            return ChannelDescription::OwnAddress;
        default:
            return ChannelDescription::Undefined;
        }
    };

    UnitType to_unit_type(uint8_t raw) {
        uint8_t t = raw & 0x1F;
        switch (t) {
        case 0:
            return UnitType::Invalid;
        case 1:
            return UnitType::NoUnit;
        case 2:
            return UnitType::Ohm;
        case 3:
            return UnitType::Ampere;
        case 4:
            return UnitType::Volt;
        case 5:
            return UnitType::Percent;
        case 6:
            return UnitType::Hertz;
        case 7:
            return UnitType::Baud;
        case 8:
            return UnitType::Farad;
        case 9:
            return UnitType::Henry;
        case 10:
            return UnitType::DegC;
        case 11:
            return UnitType::DegF;
        case 12:
            return UnitType::Seconds;
        case 13:
            return UnitType::Minutes;
        case 14:
            return UnitType::Hours;
        case 15:
            return UnitType::Days;
        case 16:
            return UnitType::Months;
        }
        return UnitType::Invalid;
    };

    ValidType to_valid_type(uint8_t raw) {
        uint8_t t = raw >> 6;
        if (t == 1) {
            return ValidType::TrueValueIsSmaller;
        } else if (t == 2) {
            return ValidType::TrueValueIsBigger;
        } else if (t == 3) {
            return ValidType::Invalid;
        } else {
            return ValidType::TrueValue;
        }
    };

    TestType to_test_type(uint8_t raw) {
        uint8_t t = raw >> 6;
        if (t == 1) {
            return TestType::InternalTest;
        } else if (t == 2) {
            return TestType::ExternalTest;
        } else {
            return TestType::NoTest;
        }
    };

    AlarmType to_alarm_type(uint8_t raw) {
        uint8_t t = raw & 0x07;

        if (t == 0x00) {
            return AlarmType::NoAlarm;
        } else if (t == 0x01) {
            return AlarmType::PreWarning;
        } else if (t == 0x02 and (raw & 0xC0) == 0x00) {
            return AlarmType::DeviceError;
        } else if (t == 0x04) {
            return AlarmType::Warning;
        } else if (t == 0x05) {
            return AlarmType::Alarm;
        } else {
            return AlarmType::Reserved;
        }
    };

    struct MeasurementValue {
        float value{0.};
        AlarmType alarm{AlarmType::NoAlarm};
        TestType test{TestType::NoTest};
        UnitType unit{UnitType::Invalid};
        ValidType valid{ValidType::Invalid};
        ChannelDescription description{ChannelDescription::Undefined};
    };

    MeasurementValue read_register(const ImdRegisters start_register);

    std::string to_string(MeasurementValue m) {
        return fmt::format(" {} [{}] [{}] [{}] [{}] [{}]", m.value, to_string(m.unit), to_string(m.valid),
                           to_string(m.description), to_string(m.alarm), to_string(m.test));
    };

    types::isolation_monitor::IsolationMeasurement isolation_measurement_data{};

    std::thread output_thread;
    std::atomic_bool enable_publishing{false};
    std::atomic_bool self_test_started{false};
    std::atomic_int self_test_timeout{0};

    TestType last_test{TestType::NoTest};
    AlarmType last_alarm{AlarmType::NoAlarm};

    void set_deviceFault(const std::string& message);
    void clear_deviceFault();

    void configure_device();
    void start_measurements();
    void stop_measurements();
    void start_self_test();
    bool send_to_imd(const uint16_t& command, const uint16_t& value);
    void read_imd_values();
    std::string read_device_name();
    std::string read_firmware_version();
    bool check_for_faster_cablecheck();
    bool enable_faster_cable_check_mode();
    bool disable_faster_cable_check_mode();
    bool faster_cable_check_supported{false};
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_ISOLATION_MONITOR_IMPL_HPP
