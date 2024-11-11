// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef REMOTE_CHARGING_EVSE_HPP
#define REMOTE_CHARGING_EVSE_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/power_supply_DC/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string tap_device;
    std::string plc_device;
    std::string bridge_device;
    std::string base_topic_sub;
    std::string base_topic_pub;
};

class RemoteChargingEVSE : public Everest::ModuleBase {
public:
    RemoteChargingEVSE() = delete;
    RemoteChargingEVSE(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                       std::unique_ptr<emptyImplBase> p_main, std::unique_ptr<evse_board_supportIntf> r_bsp,
                       std::unique_ptr<power_supply_DCIntf> r_power_supply, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_bsp(std::move(r_bsp)),
        r_power_supply(std::move(r_power_supply)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<evse_board_supportIntf> r_bsp;
    const std::unique_ptr<power_supply_DCIntf> r_power_supply;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    int tap_fd{0};
    std::mutex mutex;
    std::string last_cp_state;
    double last_voltage{0.};

    std::string cp_state_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/cp_state";
    };
    std::string cp_pwm_duty_cycle_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/cp_pwm_dc";
    };
    std::string eth_ev_to_evse_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/eth_ev_to_evse";
    };
    std::string eth_evse_to_ev_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/eth_evse_to_ev_topic";
    };
    std::string nmk_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/nmk";
    };
    std::string dc_voltage_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/dc_voltage";
    };
    std::string ping_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/ping";
    };
    std::string pong_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/pong";
    };

    void hlc_log(const std::string& origin, const std::string& target, const std::string& msg) {
        nlohmann::json data;
        data["origin"] = origin;
        data["target"] = target;
        data["iso15118"] = false;
        data["msg"] = msg;

        std::string hlc_log_topic = "everest_api/" + this->info.id + "/var/hlc_log";
        mqtt.publish(hlc_log_topic, data.dump());
    };

    void session_info() {

        json info;
        info["state"] = "Preparing";
        info["datetime"] = Everest::Date::to_rfc3339(date::utc_clock::now());
        mqtt.publish("everest_api/" + this->info.id + "/var/session_info", info.dump());
        mqtt.publish("everest_api/evse_manager/var/session_info", info.dump());
        /*
        ### everest_api/evse_manager/var/session_info
        This variable is published every second and contains a json object with information relating to the current
        charging session in the following format:
        ```json
        {
            "charged_energy_wh": 0,
            "charging_duration_s": 84,
            "datetime": "2022-10-11T16:48:35.747Z",
            "discharged_energy_wh": 0,
            "latest_total_w": 0.0,
            "permanent_fault": false,
            "state": "Unplugged",
            "active_enable_disable_source": {
                "source": "Unspecified",
                "state": "Enable",
                "priority": 5000
            },
            "uk_random_delay": {
                "remaining_s": 34,
                "current_limit_after_delay_A": 16.0,
                "current_limit_during_delay_A": 0.0,
                "start_time": "2024-02-28T14:11:11.129Z"
            },
            "last_enable_disable_source": "Unspecified"
        }
        */
    };
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // REMOTE_CHARGING_EVSE_HPP
