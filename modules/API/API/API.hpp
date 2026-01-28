// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef METHODS_API_HPP
#define METHODS_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/charger_information/Interface.hpp>
#include <generated/interfaces/error_history/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/external_energy_limits/Interface.hpp>
#include <generated/interfaces/ocpp/Interface.hpp>
#include <generated/interfaces/uk_random_delay/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <sstream>

#include <date/date.h>
#include <date/tz.h>

#include "StartupMonitor.hpp"
#include "limit_decimal_places.hpp"

namespace module {

class LimitDecimalPlaces;

class SessionInfo {
public:
    SessionInfo();

    struct Error {
        std::string type;
        std::string description;
        std::string severity;
    };

    bool start_energy_export_wh_was_set{
        false}; ///< Indicate if start export energy value (optional) has been received or not
    bool end_energy_export_wh_was_set{
        false}; ///< Indicate if end export energy value (optional) has been received or not

    void reset();
    void update_state(const types::evse_manager::SessionEvent event);
    void set_start_energy_import_wh(int32_t start_energy_import_wh);
    void set_end_energy_import_wh(int32_t end_energy_import_wh);
    void set_latest_energy_import_wh(int32_t latest_energy_wh);
    void set_start_energy_export_wh(int32_t start_energy_export_wh);
    void set_end_energy_export_wh(int32_t end_energy_export_wh);
    void set_latest_energy_export_wh(int32_t latest_export_energy_wh);
    void set_latest_total_w(double latest_total_w);
    void set_uk_random_delay_remaining(const types::uk_random_delay::CountDown& c);
    void set_enable_disable_source(const std::string& active_source, const std::string& active_state,
                                   const int active_priority);
    void set_permanent_fault(bool f) {
        permanent_fault = f;
    }

    /// \brief Converts this struct into a serialized json object
    operator std::string();

private:
    std::mutex session_info_mutex;
    int32_t start_energy_import_wh; ///< Energy reading (import) at the beginning of this charging session in Wh
    int32_t end_energy_import_wh;   ///< Energy reading (import) at the end of this charging session in Wh
    int32_t start_energy_export_wh; ///< Energy reading (export) at the beginning of this charging session in Wh
    int32_t end_energy_export_wh;   ///< Energy reading (export) at the end of this charging session in Wh
    types::uk_random_delay::CountDown uk_random_delay_remaining; ///< Remaining time of a UK smart charging regs
                                                                 ///< delay. Set to 0 if no delay is active
    std::chrono::time_point<date::utc_clock> start_time_point;   ///< Start of the charging session
    std::chrono::time_point<date::utc_clock> end_time_point;     ///< End of the charging session
    double latest_total_w;                                       ///< Latest total power reading in W

    enum class State {
        Unknown,
        Unplugged,
        Disabled,
        Preparing,
        Reserved,
        AuthRequired,
        WaitingForEnergy,
        ChargingPausedEV,
        ChargingPausedEVSE,
        Charging,
        AuthTimeout,
        Finished,
        FinishedEVSE,
        FinishedEV
    } state;

    bool is_state_charging(const SessionInfo::State current_state);

    std::string state_to_string(State s);

    std::string active_enable_disable_source{"Unspecified"};
    std::string active_enable_disable_state{"Enabled"};
    int active_enable_disable_priority{0};
    bool permanent_fault{false};
};
} // namespace module
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string charger_information_file;
    int powermeter_energy_import_decimal_places;
    int powermeter_energy_export_decimal_places;
    int powermeter_power_decimal_places;
    int powermeter_voltage_decimal_places;
    int powermeter_VAR_decimal_places;
    int powermeter_current_decimal_places;
    int powermeter_frequency_decimal_places;
    int hw_caps_max_current_export_decimal_places;
    int hw_caps_max_current_import_decimal_places;
    int hw_caps_min_current_export_decimal_places;
    int hw_caps_min_current_import_decimal_places;
    int hw_caps_max_plug_temperature_C_decimal_places;
    int limits_max_current_decimal_places;
    int telemetry_evse_temperature_C_decimal_places;
    int telemetry_fan_rpm_decimal_places;
    int telemetry_supply_voltage_12V_decimal_places;
    int telemetry_supply_voltage_minus_12V_decimal_places;
    int telemetry_plug_temperature_C_decimal_places;
    double powermeter_energy_import_round_to;
    double powermeter_energy_export_round_to;
    double powermeter_power_round_to;
    double powermeter_voltage_round_to;
    double powermeter_VAR_round_to;
    double powermeter_current_round_to;
    double powermeter_frequency_round_to;
    double hw_caps_max_current_export_round_to;
    double hw_caps_max_current_import_round_to;
    double hw_caps_min_current_export_round_to;
    double hw_caps_min_current_import_round_to;
    double hw_caps_max_plug_temperature_C_round_to;
    double limits_max_current_round_to;
    double telemetry_evse_temperature_C_round_to;
    double telemetry_fan_rpm_round_to;
    double telemetry_supply_voltage_12V_round_to;
    double telemetry_supply_voltage_minus_12V_round_to;
    double telemetry_plug_temperature_C_round_to;
};

class API : public Everest::ModuleBase {
public:
    API() = delete;
    API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
        std::vector<std::unique_ptr<charger_informationIntf>> r_charger_information,
        std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager, std::vector<std::unique_ptr<ocppIntf>> r_ocpp,
        std::vector<std::unique_ptr<uk_random_delayIntf>> r_random_delay,
        std::vector<std::unique_ptr<error_historyIntf>> r_error_history,
        std::vector<std::unique_ptr<external_energy_limitsIntf>> r_evse_energy_sink, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_charger_information(std::move(r_charger_information)),
        r_evse_manager(std::move(r_evse_manager)),
        r_ocpp(std::move(r_ocpp)),
        r_random_delay(std::move(r_random_delay)),
        r_error_history(std::move(r_error_history)),
        r_evse_energy_sink(std::move(r_evse_energy_sink)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::vector<std::unique_ptr<charger_informationIntf>> r_charger_information;
    const std::vector<std::unique_ptr<evse_managerIntf>> r_evse_manager;
    const std::vector<std::unique_ptr<ocppIntf>> r_ocpp;
    const std::vector<std::unique_ptr<uk_random_delayIntf>> r_random_delay;
    const std::vector<std::unique_ptr<error_historyIntf>> r_error_history;
    const std::vector<std::unique_ptr<external_energy_limitsIntf>> r_evse_energy_sink;
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
    std::vector<std::thread> api_threads;
    bool running = true;

    StartupMonitor evse_manager_check;

    std::list<std::unique_ptr<SessionInfo>> info;
    std::list<std::string> hw_capabilities_str;
    std::string selected_protocol;
    json charger_information;
    std::unique_ptr<LimitDecimalPlaces> limit_decimal_places;

    std::mutex ocpp_data_mutex;
    json ocpp_charging_schedule;
    bool ocpp_charging_schedule_updated = false;
    std::string ocpp_connection_status = "unknown";

    const std::string api_base = "everest_api/";
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // METHODS_API_HPP
