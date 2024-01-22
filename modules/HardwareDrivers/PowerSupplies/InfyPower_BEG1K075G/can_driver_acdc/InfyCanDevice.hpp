// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef INFY_CAN_DEVICE_HPP
#define INFY_CAN_DEVICE_HPP

#include "CanDevice.hpp"
#include <atomic>
#include <linux/can.h>
#include <mutex>
#include <sigslot/signal.hpp>
#include <thread>

class InfyCanDevice : public CanDevice {
public:
    InfyCanDevice();
    ~InfyCanDevice();

    enum class OutputMode {
        Parallel = 0xA0,
        Series = 0xA1,
        Automatic = 0xA2
    };

    // Commands

    bool switch_on_off(bool on);
    bool set_walkin_enabled(bool on);
    bool set_inverter_mode(bool inverter);
    bool set_voltage_current(float voltage, float current);
    bool set_output_mode(OutputMode mode);
    bool adjust_power_factor(float pf);
    bool set_generic_setting(uint8_t byte0, uint8_t byte1, uint32_t value);

    // Data out
    sigslot::signal<float, float, float> signalacdcTemperatures;
    sigslot::signal<float, float> signalVoltageCurrent;
    sigslot::signal<can_packet_acdc::PowerModuleStatus, can_packet_acdc::InverterStatus> signalModuleStatus;
    bool request_rx(const uint8_t destination_address, const std::vector<uint8_t>& payload);

    struct Telemetry {
        float ac_ab_line_voltage{0.};
        float ac_bc_line_voltage{0.};
        float ac_ca_line_voltage{0.};
        float ambient_temperature{0.};
        float dc_max_output_voltage{0.};
        float dc_min_output_voltage{0.};
        float dc_max_output_current{0.};
        float dc_rated_output_power{0.};
        float ac_phase_a_current{0.};
        float ac_phase_b_current{0.};
        float ac_phase_c_current{0.};
        float ac_phase_a_voltage{0.};
        float ac_phase_b_voltage{0.};
        float ac_phase_c_voltage{0.};
        float ac_frequency{0.};
        float ac_phase_a_active_power{0.};
        float ac_phase_b_active_power{0.};
        float ac_phase_c_active_power{0.};
        float ac_total_active_ower{0.};

        float ac_phase_a_reactive_power{0.};
        float ac_phase_b_reactive_power{0.};
        float ac_phase_c_reactive_power{0.};
        float ac_total_reactive_ower{0.};

        float ac_phase_a_apparent_power{0.};
        float ac_phase_b_apparent_power{0.};
        float ac_phase_c_apparent_power{0.};
        float ac_total_apparent_ower{0.};

        float dc_high_side_voltage{0.};
        float dc_high_side_current{0.};

        can_packet_acdc::SystemDCVoltage voltage;
        can_packet_acdc::SystemDCCurrent current;
        can_packet_acdc::PowerModuleStatus status;
    } telemetry;

    friend std::ostream& operator<<(std::ostream& out, const Telemetry& self);

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload);

private:
    std::atomic_bool exitTxThread;
    std::thread txThreadHandle;
    void txThread();

    bool tx(const uint8_t destination_address, const std::vector<uint8_t>& payload);

    std::mutex settingsMutex;
};

#endif // INFY_CAN_DEVICE_HPP
