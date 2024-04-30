// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "power_supply_DCImpl.hpp"

#include <memory>

#include "can_broker.hpp"

#include <fmt/core.h>
#include <utils/formatter.hpp>

std::unique_ptr<CanBroker> can_broker;

namespace dpm1000 = can::protocol::dpm1000;

namespace module {
namespace main {

static void log_status_on_fail(const std::string& msg, CanBroker::AccessReturnType status) {
    using ReturnStatus = CanBroker::AccessReturnType;

    std::string reason;
    switch (status) {
    case ReturnStatus::FAILED:
        reason = "failed";
        break;
    case ReturnStatus::NOT_READY:
        reason = "not ready";
        break;
    case ReturnStatus::TIMEOUT:
        reason = "timeout";
    default:
        return;
    }

    EVLOG_info << msg << " reason: (" << reason << ")";
}

static std::string alarm_to_string(uint32_t alarm) {
    std::string alarmflags;

    if (alarm & (1 << (int)dpm1000::def::Alarm::FUSE_BURN_OUT))
        alarmflags += "[FUSE_BURN_OUT]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::PFC_DCDC_COMMUNICATION_ERROR))
        alarmflags += "[PFC_DCDC_COMMUNICATION_ERROR]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::UNBALANCED_BUS_VOLTAGE))
        alarmflags += "[UNBALANCED_BUS_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::BUS_OVER_VOLTAGE))
        alarmflags += "[BUS_OVER_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::BUS_ABNORMAL_VOLTAGE))
        alarmflags += "[BUS_ABNORMAL_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::PHASE_OVER_VOLTAGE))
        alarmflags += "[PHASE_OVER_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::ID_NUMBER_REPETITION))
        alarmflags += "[ID_NUMBER_REPETITION]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::BUS_UNDER_VOLTAGE))
        alarmflags += "[BUS_UNDER_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::PHASE_LOSS))
        alarmflags += "[PHASE_LOSS]";

    if (alarm & (1 << (int)dpm1000::def::Alarm::PHASE_UNDER_VOLTAGE))
        alarmflags += "[PHASE_UNDER_VOLTAGE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::CAN_COMMUNICATION_FAULT))
        alarmflags += "[CAN_COMMUNICATION_FAULT]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_UNEVEN_CURRENT_SHARING))
        alarmflags += "[DCDC_UNEVEN_CURRENT_SHARING]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::PFC_POWER_OFF))
        alarmflags += "[PFC_POWER_OFF]";

    if (alarm & (1 << (int)dpm1000::def::Alarm::FAN_FULL_SPEED))
        alarmflags += "[FAN_FULL_SPEED]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::POWER_LIMITING))
        alarmflags += "[POWER_LIMITING]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_POWER_OFF))
        alarmflags += "[DCDC_POWER_OFF]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::TEMPERATURE_POWER_LIMITING))
        alarmflags += "[TEMPERATURE_POWER_LIMITING]";

    if (alarm & (1 << (int)dpm1000::def::Alarm::AC_POWER_LIMITING))
        alarmflags += "[AC_POWER_LIMITING]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_EEPROM_FAULTS))
        alarmflags += "[DCDC_EEPROM_FAULTS]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::FAN_FAULTS))
        alarmflags += "[FAN_FAULTS]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_SHORT_CIRCUIT))
        alarmflags += "[DCDC_SHORT_CIRCUIT]";

    if (alarm & (1 << (int)dpm1000::def::Alarm::PFC_EEPROM_FAULTS))
        alarmflags += "[PFC_EEPROM_FAULTS]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_OVER_TEMPERATURE))
        alarmflags += "[DCDC_OVER_TEMPERATURE]";
    if (alarm & (1 << (int)dpm1000::def::Alarm::DCDC_OUTPUT_OVER_VOLTAGE))
        alarmflags += "[DCDC_OUTPUT_OVER_VOLTAGE]";

    return alarmflags;
}

void power_supply_DCImpl::init() {
    current = 0;
    voltage = 300;

    config_current_limit = mod->config.current_limit_A;
    config_voltage_limit = mod->config.voltage_limit_V;
    config_power_limit = mod->config.power_limit_W;

    if (!mod->config.discharge_gpio_chip.empty()) {
        discharge_gpio.open(mod->config.discharge_gpio_chip, mod->config.discharge_gpio_line,
                            !mod->config.discharge_gpio_polarity);
        discharge_gpio.set_output(false);
    }

    can_broker = std::make_unique<CanBroker>(mod->config.device, mod->config.device_address);

    // ensure the module is switched off
    can_broker->set_state(false);

    // Configure module for series or parallel mode
    // 0 is automatic switching mode
    float series_parallel_mode = 0.;

    if (mod->config.series_parallel_mode == "Series") {
        series_parallel_mode = 1050.;
        config_min_voltage_limit = 300.;
        parallel_mode = false;
    } else if (mod->config.series_parallel_mode == "Parallel") {
        series_parallel_mode = 520.;
        if (config_voltage_limit > 520) {
            config_voltage_limit = 520;
        }
        parallel_mode = true;
    }

    // WTF: This really uses a float to set one of the three modes automatic, series or parallel.
    auto status = can_broker->set_data(dpm1000::def::SetValueType::SERIES_PARALLEL_MODE, series_parallel_mode);
    log_status_on_fail("Set current limit failed", status);
}

void power_supply_DCImpl::ready() {
    types::power_supply_DC::Capabilities caps;
    caps.bidirectional = false;
    caps.max_export_current_A = config_current_limit;
    caps.max_export_voltage_V = config_voltage_limit;
    caps.min_export_current_A = 0;
    caps.min_export_voltage_V = config_min_voltage_limit;
    caps.max_export_power_W = config_power_limit;
    caps.current_regulation_tolerance_A = 0.5;
    caps.peak_current_ripple_A = 1;
    caps.conversion_efficiency_export = 0.95;

    publish_capabilities(caps);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // Send voltage, current and power limits

        auto status = can_broker->set_data(dpm1000::def::SetValueType::CURRENT_LIMIT, current);
        log_status_on_fail("Set current limit failed", status);

        status = can_broker->set_data(dpm1000::def::SetValueType::DEFAULT_CURRENT_LIMIT, 1.0);
        log_status_on_fail("Set default current limit failed", status);

        status = can_broker->set_data(dpm1000::def::SetValueType::VOLTAGE, voltage);
        log_status_on_fail("Set voltage failed", status);

        status = can_broker->set_data(dpm1000::def::SetValueType::POWER_LIMIT, 1.0);
        log_status_on_fail("Set current limit failed", status);

        // Read voltage and current
        float tmp;
        types::power_supply_DC::VoltageCurrent vc;

        status = can_broker->read_data(dpm1000::def::ReadValueType::VOLTAGE, tmp);
        log_status_on_fail("Read voltage failed", status);
        if (status != CanBroker::AccessReturnType::SUCCESS) {
            continue;
        }
        vc.voltage_V = tmp;

        status = can_broker->read_data(dpm1000::def::ReadValueType::CURRENT, tmp);
        log_status_on_fail("Read current failed", status);
        if (status != CanBroker::AccessReturnType::SUCCESS) {
            continue;
        }

        // Publish voltage and current var
        // Current scaling depends on series/parallel mode operation.
        vc.current_A = tmp;
        if (parallel_mode) {
            vc.current_A *= 2.;
        }
        publish_voltage_current(vc);

        // read alarm flags
        uint32_t alarm = 0;
        status = can_broker->read_data_int(dpm1000::def::ReadValueType::ALARM, alarm);
        log_status_on_fail("Read alarm failed", status);
        if (status == CanBroker::AccessReturnType::SUCCESS) {
            if (last_alarm_flags != alarm) {
                auto alarmflags = alarm_to_string(alarm);
                if (alarmflags != "") {
                    EVLOG_warning << "Alarm flags changed: " << alarmflags;
                } else {
                    EVLOG_info << "All Alarm flags cleared.";
                }
                last_alarm_flags = alarm;
            }
        }

        // Discharge output if it is higher then setpoint voltage.
        // Note that this has no timeout, so HW must be designed to sustain the worst case load (e.g. 1000V) continously
        if (vc.voltage_V > (voltage + 10)) {
            discharge_gpio.set(true);
        } else {
            discharge_gpio.set(false);
        }

        if (mod->config.debug_print_all_telemetry) {
            // read additional meta data
            float current_real_part = 0, current_limit = 0, dcdc_temperature = 0, ac_voltage = 0, voltage_limit = 0,
                  pfc0_voltage = 0, pfc1_voltage = 0, env_temperature = 0, ac_voltage_phase_a = 0,
                  ac_voltage_phase_b = 0, ac_voltage_phase_c = 0, pfc_temperature = 0, power_limit = 0;
            status = can_broker->read_data(dpm1000::def::ReadValueType::CURRENT_REAL_PART, current_real_part);
            log_status_on_fail("Read CURRENT_REAL_PART failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::CURRENT_LIMIT, current_limit);
            log_status_on_fail("Read CURRENT_LIMIT failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::DCDC_TEMPERATURE, dcdc_temperature);
            log_status_on_fail("Read DCDC_TEMPERATURE failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::AC_VOLTAGE, ac_voltage);
            log_status_on_fail("Read AC_VOLTAGE failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::VOLTAGE_LIMIT, voltage_limit);
            log_status_on_fail("Read VOLTAGE_LIMIT failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::PFC0_VOLTAGE, pfc0_voltage);
            log_status_on_fail("Read PFC0_VOLTAGE failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::PFC1_VOLTAGE, pfc1_voltage);
            log_status_on_fail("Read PFC1_VOLTAGE failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::ENV_TEMPERATURE, env_temperature);
            log_status_on_fail("Read ENV_TEMPERATURE failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::AC_VOLTAGE_PHASE_A, ac_voltage_phase_a);
            log_status_on_fail("Read AC_VOLTAGE_PHASE_A failed", status);
            status = can_broker->read_data(dpm1000::def::ReadValueType::AC_VOLTAGE_PHASE_B, ac_voltage_phase_b);
            log_status_on_fail("Read AC_VOLTAGE_PHASE_B failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::AC_VOLTAGE_PHASE_C, ac_voltage_phase_c);
            log_status_on_fail("Read AC_VOLTAGE_PHASE_C failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::PFC_TEMPERATURE, pfc_temperature);
            log_status_on_fail("Read PFC_TEMPERATURE failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::POWER_LIMIT, power_limit);
            log_status_on_fail("Read POWER_LIMIT failed", status);

            status = can_broker->read_data(dpm1000::def::ReadValueType::ENV_TEMPERATURE, env_temperature);
            log_status_on_fail("Read ENV_TEMPERATURE failed", status);

            EVLOG_info << fmt::format(
                "set_voltage {} set_current {} vc.current_A {} vc.voltage_V {} current_real_part {} current_limit {} "
                "dcdc_temperature {} ac_voltage {} "
                "voltage_limit "
                "{} pfc0_voltage {} pfc1_voltage {} env_temperature {} ac_voltage_phase_a {} ac_voltage_phase_b {} "
                "ac_voltage_phase_c {} pfc_temperature {} power_limit {}",
                voltage, current, vc.current_A, vc.voltage_V, current_real_part, current_limit, dcdc_temperature,
                ac_voltage, voltage_limit, pfc0_voltage, pfc1_voltage, env_temperature, ac_voltage_phase_a,
                ac_voltage_phase_b, ac_voltage_phase_c, pfc_temperature, power_limit);
        }
    }
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& value) {
    if (value == types::power_supply_DC::Mode::Export) {
        can_broker->set_state(true);
    } else {
        can_broker->set_state(false);
    }
}

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    if (voltage <= config_voltage_limit && voltage >= config_min_voltage_limit && current <= config_current_limit) {
        this->voltage = voltage;
        this->current = current / 100.;
    } else {
        EVLOG_error << fmt::format("Out of range voltage/current settings ignored: {}V / {}A", voltage, current);
    }
}

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    // power supply is uni directional only
}

} // namespace main
} // namespace module
