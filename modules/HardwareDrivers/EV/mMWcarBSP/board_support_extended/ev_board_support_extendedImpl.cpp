// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ev_board_support_extendedImpl.hpp"

namespace module {
namespace board_support_extended {

void ev_board_support_extendedImpl::init() {
    EVLOG_info << "mMWcar extended INIT";

    mod->serial.signal_keep_alive.connect([this](KeepAlive k) {
        types::ev_board_support_extended::KeepAliveData data;
        data.timestamp = k.time_stamp;                // will get converted to signed unfortunatly
        data.hw_revision_minor = k.hw_revision_minor; // will get converted to signed unfortunatly
        data.sw_version = k.sw_version_string;
        switch (k.hw_revision_major) {
        case ConfigHardwareRevisionMajor_HW_MAJOR_REV_A:
            data.hw_revision_major = types::ev_board_support_extended::Hw_revision_major::Rev_A;
            break;

        default:
            data.hw_revision_major = types::ev_board_support_extended::Hw_revision_major::UNKNOWN;
            break;
        }
        publish_keep_alive_event(data);
    });

    mod->serial.signal_ac_meas_instant.connect([this](ACMeasInstant d) {
        types::ev_board_support_extended::ACVoltageData data;
        data.Ua = d.Ua;
        data.Ub = d.Ub;
        data.Uc = d.Uc;
        publish_ac_voltage_event(data);
    });

    mod->serial.signal_ac_stats.connect([this](ACMeasStats d) {
        types::ev_board_support_extended::ACPhaseStatistics data;
        data.Ua.min = d.Ua.min;
        data.Ua.max = d.Ua.max;
        data.Ua.average = d.Ua.avg;

        data.Ub.min = d.Ub.min;
        data.Ub.max = d.Ub.max;
        data.Ub.average = d.Ub.avg;

        data.Uc.min = d.Uc.min;
        data.Uc.max = d.Uc.max;
        data.Uc.average = d.Uc.avg;

        publish_ac_statistics_event(data);
    });

    mod->serial.signal_dc_meas_instant.connect([this](DCMeasInstant d) { publish_dc_voltage_event(d.bus_voltage); });

    mod->serial.signal_edge_timing_statistics.connect([this](EdgeTimingMeasResult d) {
        types::ev_board_support_extended::EdgeTimingData data;
        data.rising.min = d.rising.duration_min_ns;
        data.rising.max = d.rising.duration_max_ns;
        data.rising.average = d.rising.duration_avg_ns;

        data.falling.min = d.falling.duration_min_ns;
        data.falling.max = d.falling.duration_max_ns;
        data.falling.average = d.falling.duration_avg_ns;

        data.num_periods = d.num_periods;

        // ignore if states are not A/B/C/D
        switch (d.cp_state) {
        case CpState_STATE_E:
        case CpState_STATE_F:
            return;
        }

        data.cp_state = mcu_cp_state_to_everest_cp_state(d.cp_state);

        publish_edge_timing_event(data);
    });
}

void ev_board_support_extendedImpl::ready() {
    EVLOG_info << "mMWcar extended READY";
}

bool ev_board_support_extendedImpl::handle_set_status_led(bool& state) {
    EVLOG_info << "Setting led state to " << (state ? "ON" : "OFF");
    return true;
}

void ev_board_support_extendedImpl::handle_reset(bool& soft_reset) {
    // EVLOG_info << "Resetting " << (soft_reset ? "SOFT" : "HARD");
    if (soft_reset) {
        if (!mod->serial.soft_reset())
            EVLOG_error << "Could not soft reset";
        else
            EVLOG_info << "Sent soft resetting packet to MCU.";
    } else {
        mod->serial.hard_reset(mod->config.reset_gpio_chip, mod->config.reset_gpio_line);
    }
}

void ev_board_support_extendedImpl::handle_get_ac_data_instant() {
    if (!mod->serial.get_ac_data_instant())
        EVLOG_error << "Could not send measurement request packet.";
    else
        EVLOG_info << "Sent AC instant measurement request packet.";
}

void ev_board_support_extendedImpl::handle_get_ac_statistics() {
    if (!mod->serial.get_ac_statistic())
        EVLOG_error << "Could not send measurement request packet.";
    else
        EVLOG_info << "Sent AC statistics measurement request packet.";
}

void ev_board_support_extendedImpl::handle_get_dc_data_instant() {
    if (!mod->serial.get_dc_data_instant())
        EVLOG_error << "Could not send measurement request packet.";
    else
        EVLOG_info << "Sent DC instant measurement request packet.";
}

void ev_board_support_extendedImpl::handle_set_cp_voltage(double& voltage) {
    if (!mod->serial.set_cp_voltage(voltage))
        EVLOG_error << "Could not send set_cp_voltage packet.";
    else
        EVLOG_info << fmt::format("Sent set_cp_voltage [{}V] packet.", voltage);
}

void ev_board_support_extendedImpl::handle_set_cp_load_en(bool& state) {
    if (!mod->serial.set_cp_load_en(state))
        EVLOG_error << "Could not send set_cp_load_en packet.";
    else
        EVLOG_info << fmt::format("Sent set_cp_load_en [{}] packet.", state ? "TRUE" : "FALSE");
}

void ev_board_support_extendedImpl::handle_set_cp_short_to_gnd_en(bool& state) {
    if (!mod->serial.set_cp_short_to_gnd_en(state))
        EVLOG_error << "Could not send set_cp_short_to_gnd_en packet.";
    else
        EVLOG_info << fmt::format("Sent set_cp_short_to_gnd_en [{}] packet.", state ? "TRUE" : "FALSE");
}

void ev_board_support_extendedImpl::handle_set_cp_diode_fault_en(bool& state) {
    if (!mod->serial.set_cp_diode_fault_en(state))
        EVLOG_error << "Could not send set_cp_diode_fault_en packet.";
    else
        EVLOG_info << fmt::format("Sent set_cp_diode_fault_en [{}] packet.", state ? "TRUE" : "FALSE");
}

void ev_board_support_extendedImpl::handle_trigger_edge_timing_measurement(int& num_periods, bool& force_start) {
    if (!mod->serial.trigger_edge_timing_measurement(num_periods, force_start))
        EVLOG_error << "Could not trigger edge timing measurement.";
    // else
    // EVLOG_info << fmt::format("Sent set_cp_diode_fault_en [{}] packet.", state ? "TRUE" : "FALSE");
}

} // namespace board_support_extended
} // namespace module
