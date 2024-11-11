// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "RemoteChargingEVSE.hpp"

#include "../common/slac_helper.hpp"
#include "../common/tap_device.hpp"

namespace module {

static std::optional<std::string> to_cp_state_str(const types::board_support_common::BspEvent event) {
    using CP = types::board_support_common::Event;
    switch (event.event) {
    case CP::A:
        return "A";
    case CP::B:
        return "B";
    case CP::C:
        return "C";
    case CP::D:
        return "D";
    case CP::E:
        return "E";
    case CP::F:
        return "F";
    }
    return {};
}

void RemoteChargingEVSE::init() {
    invoke_init(*p_main);

    // Bridge CP State
    r_bsp->subscribe_event([this](const types::board_support_common::BspEvent event) {
        const auto cp = to_cp_state_str(event);
        if (cp.has_value()) {
            {
                std::scoped_lock lock(mutex);
                last_cp_state = cp.value();
            }
            mqtt.publish(cp_state_topic(true), cp.value());
            if (event.event == types::board_support_common::Event::C) {
                // switch on relays whenever state C was detected
                r_bsp->call_allow_power_on({true, types::evse_board_support::Reason::FullPowerCharging});
                r_power_supply->call_setMode(types::power_supply_DC::Mode::Export,
                                             types::power_supply_DC::ChargingPhase::Other);
            } else {
                r_bsp->call_allow_power_on({false, types::evse_board_support::Reason::PowerOff});
                {
                    std::scoped_lock lock(mutex);
                    last_voltage = 0.;
                }
                r_power_supply->call_setMode(types::power_supply_DC::Mode::Off,
                                             types::power_supply_DC::ChargingPhase::Other);
                r_power_supply->call_setExportVoltageCurrent(0., 1.);
            }
        }
    });

    // Bridge CP PWM dutycycle
    mqtt.subscribe(cp_pwm_duty_cycle_topic(false), [this](const std::string& _pwm) {
        try {
            float pwm = std::stof(_pwm);
            if (pwm < 1.) {
                // r_bsp->call_pwm_F();
            } else if (pwm < 99.) {
                r_bsp->call_pwm_on(pwm);
            } else {
                r_bsp->call_pwm_off();
            }
        } catch (...) {
            // Just ignore if we cannot parse/forward the value
            EVLOG_info << "Cannot parse PWM value: " << _pwm;
        }
    });

    // Bridge DC voltage
    mqtt.subscribe(dc_voltage_topic(false), [this](const std::string& _voltage) {
        try {
            double v = std::stod(_voltage);
            {
                std::scoped_lock lock(mutex);
                last_voltage = v;
            }
            r_power_supply->call_setExportVoltageCurrent(v, 1);
        } catch (...) {
            // Just ignore if we cannot parse/forward the value
            EVLOG_info << "Cannot parse Voltage value: " << _voltage;
        }
    });

    // Bridge NMK (we do not get the SLAC_MATCH.CNF on this side without promiscuous mode)
    mqtt.subscribe(nmk_topic(false), [this](const std::string& match_cnf_msg) {
        auto msg = slac_helper::to_homeplug_msg(match_cnf_msg);
        auto set_key_req = slac_helper::set_nmk_key_from_match_cnf(msg);
        EVLOG_info << "Setting NMK key";
        hlc_log("HLC", "", "Setting NMK key");
        tap::send_eth_packet(tap_fd, set_key_req);
    });

    // ping pong
    mqtt.subscribe(pong_topic(false), [this](const std::string& _ping_time) {
        std::string ping_time = _ping_time.substr(0, _ping_time.size() - 1500);
        EVLOG_info << ping_time;
        auto start_time = Everest::Date::from_rfc3339(ping_time);
        auto latency =
            std::chrono::duration_cast<std::chrono::milliseconds>(date::utc_clock::now() - start_time).count();
        EVLOG_info << "Round-trip time: " << latency;
        hlc_log("SYS", "", fmt::format("Round-trip time: {}", latency));
    });

    tap_fd = tap::open_devices(config.tap_device);

    if (tap_fd <= 0) {
        EVLOG_error << "Could not open TAP device: " << config.tap_device;
    } else {
        mqtt.subscribe(eth_evse_to_ev_topic(false), [this](const std::string& packet) {
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

void RemoteChargingEVSE::ready() {
    invoke_ready(*p_main);

    std::thread interval([this]() {
        int i = 0;
        while (true) {
            {
                std::scoped_lock lock(mutex);
                mqtt.publish(cp_state_topic(true), last_cp_state);
            }
            // for (int i = 0; i < 2; i++) {
            const int ideal_size = 1500;
            std::string s = Everest::Date::to_rfc3339(date::utc_clock::now());
            s.append(1500, 'X');
            mqtt.publish(ping_topic(true), s);
            //}
            mqtt.publish("everest_api/" + this->info.id + "/var/datetime",
                         Everest::Date::to_rfc3339(date::utc_clock::now()));
            session_info();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
    interval.detach();

    // Run loop to receive all ethernet packets
    tap::run_receive_loop(tap_fd, [this](const std::string& packet) {
        // Is it a Homeplug AV packet?
        if (slac_helper::is_homeplug_protocol(packet)) {
            auto msg = slac_helper::to_homeplug_msg(packet);

            // The CM_ATTEN_PROFILE.IND is created by the PLC chip itself.
            // Since we now have 4 PLC chips instead of 2, they would be duplicated.
            // The real charger gets them from the PLC chip in the RemoteChargingEV,
            // so we do not need to forward them via MQTT.
            if (slac_helper::is_cm_atten_profile_ind(msg)) {
                EVLOG_info << "Ignoring CM_ATTEN_PROFILE.IND";
                return;
            }
        }

        // Forward via MQTT
        auto _packet = packet;
        _packet.append(1500, 'X');
        mqtt.publish(eth_ev_to_evse_topic(true), _packet);
    });
}

} // namespace module
