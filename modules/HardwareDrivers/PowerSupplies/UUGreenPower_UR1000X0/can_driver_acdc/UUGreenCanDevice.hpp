// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef INFY_CAN_DEVICE_HPP
#define INFY_CAN_DEVICE_HPP

#include "CanDevice.hpp"
#include <atomic>
#include <linux/can.h>
#include <map>
#include <mutex>
#include <sigslot/signal.hpp>
#include <thread>

class UUGreenCanDevice : public CanDevice {
public:
    UUGreenCanDevice() {
        // spawn thread that requests some data periodically to keep the connection alive
        exit_tx_thread = false;
        tx_thread_handle = std::thread(&UUGreenCanDevice::tx_thread, this);
        exit_cmd_thread = false;
        cmd_thread_handle = std::thread(&UUGreenCanDevice::cmd_thread, this);
    };

    ~UUGreenCanDevice();

    void set_module_address(const std::vector<uint8_t>& _module_addresses) {
        module_addresses = _module_addresses;
    };

    // Commands

    bool switch_on(bool on);
    bool set_voltage_current(float voltage, float current);

    enum class VoltageMode : uint32_t {
        Automatic = 0,
        High = 1,
        Low = 2,
    };

    bool set_voltage_mode(VoltageMode high_mode) {
        hi_mode_config_setting = high_mode;
        return internal_update_voltage_mode();
    }

    bool set_voltage_mode(int high_mode) {
        return set_voltage_mode(from_int(high_mode));
    }

    int get_number_of_modules() {
        return module_addresses.size();
    }

    void request_module_info();

    // Data out
    sigslot::signal<float, float> signal_voltage_current;
    sigslot::signal<int, const std::string&> signal_serial_number;

    struct Telemetry {
        /*
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
        */

        float voltage{0.};
        float current{0.};
    };

    float total_current{0.};

    std::map<uint8_t, Telemetry> telemetries;

    friend std::ostream& operator<<(std::ostream& out, const Telemetry& self);

protected:
    virtual void rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload);

private:
    VoltageMode from_int(int mode) {
        VoltageMode m{VoltageMode::Automatic};
        if (mode == 1) {
            m = VoltageMode::High;
        } else if (mode == 2) {
            m = VoltageMode::Low;
        }
        return m;
    };

    std::atomic_bool exit_tx_thread;
    std::thread tx_thread_handle;
    void tx_thread();

    std::atomic_bool exit_cmd_thread;
    std::thread cmd_thread_handle;
    void cmd_thread();

    bool tx(const uint8_t module_address, const std::vector<uint8_t>& payload);

    std::vector<uint8_t> module_addresses{{UU::ADDR_BROADCAST}};

    bool switch_on_nolock(bool on);
    bool internal_update_voltage_mode();

    bool set_voltage_current_nolock(float voltage, float current);

    float set_point_voltage{0.};
    float set_point_current{0.};

    std::atomic<float> requested_set_point_voltage{0.};
    std::atomic<float> requested_set_point_current{0.};

    VoltageMode hi_mode_config_setting{VoltageMode::Automatic};
    VoltageMode hi_mode_commanded{VoltageMode::Low};
    VoltageMode hi_mode_last_commanded{VoltageMode::High};

    bool is_on{false};
    std::atomic_bool requested_on{false};
    bool last_requested_on{false};

    static constexpr float LO_MODE_MAX_VOLTAGE{500};
};

#endif // INFY_CAN_DEVICE_HPP
