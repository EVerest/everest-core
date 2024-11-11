// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "RemoteChargingEV.hpp"

#include "../common/slac_helper.hpp"
#include "../common/tap_device.hpp"

namespace module {

static std::optional<types::ev_board_support::EvCpState> to_cp_state(const std::string cp) {
    using CP = types::ev_board_support::EvCpState;
    if (cp == "A") {
        return CP::A;
    } else if (cp == "B") {
        return CP::B;
    } else if (cp == "C") {
        return CP::C;
    } else if (cp == "D") {
        return CP::D;
    } else if (cp == "E") {
        return CP::E;
    } else {
        EVLOG_error << "Cannot convert " << cp;
        return {};
    }
}

void RemoteChargingEV::init() {
    invoke_init(*p_main);

    // Bridge CP State
    mqtt.subscribe(cp_state_topic(false), [this](const std::string& _cp) {
        auto cp = to_cp_state(_cp);
        if (cp.has_value()) {
            EVLOG_info << "Remote: set CP " << cp.value();
            r_bsp->call_set_cp_state(cp.value());
        }
    });

    // Bridge CP PWM dutycycle
    r_bsp->subscribe_bsp_measurement([this](types::board_support_common::BspMeasurement m) {
        {
            std::scoped_lock lock(mutex);
            last_duty_cycle = std::to_string(m.cp_pwm_duty_cycle);
            mqtt.publish(cp_pwm_duty_cycle_topic(true), last_duty_cycle);
        }
    });

    // Bridge DC voltage
    r_bsp_extended->subscribe_dc_voltage_event([this](double voltage) {
        {
            std::scoped_lock lock(mutex);
            last_voltage = voltage;
        }
        mqtt.publish(dc_voltage_topic(true), std::to_string(voltage));
    });

    // ping pong
    mqtt.subscribe(ping_topic(false),
                   [this](const std::string& ping_time) { mqtt.publish(pong_topic(true), ping_time); });

    // FIXME the EV side also needs to measure actual CP state as the real charger may set state F.
    // In this case the direction is inverted, i.e. charger sets the state and EV has to follow here
    // r_bsp->subscribe_bsp_event();

    tap_fd = tap::open_devices(config.tap_device);

    if (tap_fd <= 0) {
        EVLOG_error << "Could not open TAP device: " << config.tap_device;
    } else {
        mqtt.subscribe(eth_ev_to_evse_topic(false), [this](const std::string& packet) {
            auto _packet = packet.substr(0, packet.size() - 1500);
            tap::send_eth_packet(tap_fd, _packet);
        });

        // set up bridge
        system(fmt::format("ip link add name {} type bridge", config.bridge_device).c_str());
        system(fmt::format("ip link set dev {} up", config.bridge_device).c_str());
        system(fmt::format("ip link set dev {} master {}", config.plc_device, config.bridge_device).c_str());
        system(fmt::format("ip link set dev {} master {}", config.tap_device, config.bridge_device).c_str());
    }
}

void RemoteChargingEV::ready() {
    invoke_ready(*p_main);

    // Always allow to switch on the input relay (if any)
    r_bsp->call_allow_power_on(true);

    std::thread interval([this]() {
        while (true) {
            {
                std::scoped_lock lock(mutex);
                if (not last_duty_cycle.empty()) {
                    mqtt.publish(cp_pwm_duty_cycle_topic(true), last_duty_cycle);
                }
                mqtt.publish(dc_voltage_topic(true), last_voltage);
                for (int i = 0; i < 10; i++) {
                    mqtt.publish("everest_api/" + this->info.id + "/var/datetime",
                                 Everest::Date::to_rfc3339(date::utc_clock::now()));
                }
            }
            r_bsp_extended->call_get_dc_data_instant();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
    interval.detach();

    // Run loop to receive all ethernet packets
    tap::run_receive_loop(tap_fd, [this](const std::string& packet) {
        // Is it a Homeplug AV packet?
        if (slac_helper::is_homeplug_protocol(packet)) {
            auto msg = slac_helper::to_homeplug_msg(packet);

            // capture set NMK key packages and set local key
            if (slac_helper::is_cm_slac_match_cnf(msg)) {
                EVLOG_info << "Received CM_SLAC_MATCH.CNF with NMK";
                tap::send_eth_packet(tap_fd, slac_helper::set_nmk_key_from_match_cnf(msg));
                // send NMK to EVSE peer
                mqtt.publish(nmk_topic(true), packet);
            }
        }
        auto _packet = packet;
        _packet.append(1500, 'X');
        mqtt.publish(eth_evse_to_ev_topic(true), _packet);
    });
}

} // namespace module
