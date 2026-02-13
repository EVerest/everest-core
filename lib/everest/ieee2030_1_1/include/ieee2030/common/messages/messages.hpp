// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest
#pragma once

// #include <linux/can.h>
#include <optional>
#include <ostream>
#include <stdint.h>
#include <vector>

namespace ieee2030 {

namespace defs {

static constexpr auto CHARGED_RATE_REFERENCE = 100;

enum class ProtocolNumber : uint8_t {
    VERSION_0_9_1 = 0,
    VERSION_1_X_X = 1,
    VERSION_2_0 = 2,
};

enum class EVStatusFault : uint8_t {
    FAULT_BATTERY_OVER_VOLTAGE,
    FAULT_BATTERY_UNDER_VOTLAGE,
    FAULT_BATTER_CURRENT_DEVIATION_ERROR,
    FAULT_HIGH_BATTERY_TEMPERATURE,
    FAULT_BATTERY_VOLTAGE_DEVIATION_ERROR,
    STATUS_CHARGING_ENABLED,
    STATUS_SHIFT_POSITION,
    STATUS_SYSTEM_FAULT,
    STATUS_VEHICLE_STATUS,
    STATUS_STOP_REQUEST,
};
enum class ChargerStatusError : uint8_t {
    CHARGER_STATUS,
    CHARGER_MALFUNCTION,
    CONNECTOR_LOCK,
    BATTERY_INCOMPATIBILITY,
    SYSTEM_MALFUNCTION,
    STOP_CONTROL,
};
} // namespace defs

namespace messages {

static constexpr auto EV_ID_100 = 0x100;
static constexpr auto EV_ID_101 = 0x101;
static constexpr auto EV_ID_102 = 0x102;

struct EV100 {
    EV100(){};
    EV100(const std::vector<uint8_t> raw);
    friend std::ostream& operator<<(std::ostream&, const EV100&);
    operator std::vector<uint8_t>();

    float max_battery_voltage;
    uint8_t charged_rate{defs::CHARGED_RATE_REFERENCE};
};

struct EV101 {
    EV101(){};
    EV101(const std::vector<uint8_t> raw);
    friend std::ostream& operator<<(std::ostream&, const EV101&);
    operator std::vector<uint8_t>();

    uint8_t max_charging_time_10s; // 0xFF -> use of time_1min
    uint8_t max_charging_time_1min;
    uint8_t estimated_charging_time_1min;
    std::optional<float> total_capacity; // 0.1kWh/bit
};

struct EV102 {
    EV102(){};
    EV102(const std::vector<uint8_t> raw);
    friend std::ostream& operator<<(std::ostream&, const EV102&);
    operator std::vector<uint8_t>();

    defs::ProtocolNumber protocol;
    float target_voltage;
    float target_current;

    bool fault_battery_over_voltage;
    bool fault_battery_under_voltage;
    bool fault_battery_current_deviation_error;
    bool fault_high_battery_temperature;
    bool fault_battery_voltage_deviation_error;

    bool status_charging_enabled;
    bool status_shift_position;
    bool status_system_fault;
    bool status_vehicle_status;
    bool status_stop_request;

    uint8_t soc; // 0% - 100%
};

namespace v1_2 {} // namespace v1_2

namespace v2_0 {} // namespace v2_0

static constexpr auto CHARGER_ID_108 = 0x108;
static constexpr auto CHARGER_ID_109 = 0x109;

struct Charger108 {
    Charger108(){};
    Charger108(const std::vector<uint8_t> raw);
    Charger108(bool welding_detection_, float voltage_, float current_, float threshold_voltage_) :
        identifier_welding_detection(welding_detection_),
        available_voltage(voltage_),
        available_current(current_),
        threshold_voltage(threshold_voltage_){};
    friend std::ostream& operator<<(std::ostream&, const Charger108&);
    operator std::vector<uint8_t>();

    uint8_t identifier_welding_detection; // 0: not supported, 1-255: supported
    float available_voltage;
    float available_current;
    float threshold_voltage;
};

struct Charger109 {
    Charger109(){};
    Charger109(const std::vector<uint8_t> raw);
    Charger109(defs::ProtocolNumber protocol_) : protocol(protocol_){};
    friend std::ostream& operator<<(std::ostream&, const Charger109&);
    operator std::vector<uint8_t>();

    defs::ProtocolNumber protocol;
    float present_voltage;
    float present_current;

    bool charger_status;
    bool charger_malfunction;
    bool connector_lock;
    bool battery_incompatibility;
    bool system_malfunction;
    bool stop_control{true};

    uint8_t reamining_time_10s; // 0xFF -> use of time_1min
    uint8_t reamining_time_1min;
};

namespace v1_2 {} // namespace v1_2

namespace v2_0 {} // namespace v2_0

} // namespace messages

} // namespace ieee2030
