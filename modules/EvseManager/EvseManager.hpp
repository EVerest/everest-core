// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_MANAGER_HPP
#define EVSE_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/auth_token_provider/Implementation.hpp>
#include <generated/interfaces/energy/Implementation.hpp>
#include <generated/interfaces/evse_manager/Implementation.hpp>
#include <generated/interfaces/uk_random_delay/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ISO15118_charger/Interface.hpp>
#include <generated/interfaces/ac_rcd/Interface.hpp>
#include <generated/interfaces/connector_lock/Interface.hpp>
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/isolation_monitor/Interface.hpp>
#include <generated/interfaces/power_supply_DC/Interface.hpp>
#include <generated/interfaces/powermeter/Interface.hpp>
#include <generated/interfaces/slac/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <date/date.h>
#include <date/tz.h>
#include <future>
#include <iostream>
#include <optional>

#include "CarManufacturer.hpp"
#include "Charger.hpp"
#include "ErrorHandling.hpp"
#include "SessionLog.hpp"
#include "VarContainer.hpp"
#include "scoped_lock_timeout.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int connector_id;
    std::string evse_id;
    std::string evse_id_din;
    bool payment_enable_eim;
    bool payment_enable_contract;
    double ac_nominal_voltage;
    bool ev_receipt_required;
    bool session_logging;
    std::string session_logging_path;
    bool session_logging_xml;
    bool three_phases;
    bool has_ventilation;
    std::string country_code;
    double max_current_import_A;
    double max_current_export_A;
    std::string charge_mode;
    bool ac_hlc_enabled;
    bool ac_hlc_use_5percent;
    bool ac_enforce_hlc;
    bool ac_with_soc;
    int dc_isolation_voltage_V;
    bool dbg_hlc_auth_after_tstep;
    int hack_sleep_in_cable_check;
    int hack_sleep_in_cable_check_volkswagen;
    int cable_check_wait_number_of_imd_measurements;
    bool cable_check_enable_imd_self_test;
    bool hack_skoda_enyaq;
    int hack_present_current_offset;
    bool hack_pause_imd_during_precharge;
    bool hack_allow_bpt_with_iso2;
    bool hack_simplified_mode_limit_10A;
    bool autocharge_use_slac_instead_of_hlc;
    bool enable_autocharge;
    std::string logfile_suffix;
    double soft_over_current_tolerance_percent;
    double soft_over_current_measurement_noise_A;
    bool hack_fix_hlc_integer_current_requests;
    bool disable_authentication;
    bool sae_j2847_2_bpt_enabled;
    std::string sae_j2847_2_bpt_mode;
    bool request_zero_power_in_idle;
    bool external_ready_to_start_charging;
    bool uk_smartcharging_random_delay_enable;
    int uk_smartcharging_random_delay_max_duration;
    bool uk_smartcharging_random_delay_at_any_change;
};

class EvseManager : public Everest::ModuleBase {
public:
    EvseManager() = delete;
    EvseManager(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, Everest::TelemetryProvider& telemetry,
                std::unique_ptr<evse_managerImplBase> p_evse, std::unique_ptr<energyImplBase> p_energy_grid,
                std::unique_ptr<auth_token_providerImplBase> p_token_provider,
                std::unique_ptr<uk_random_delayImplBase> p_random_delay, std::unique_ptr<evse_board_supportIntf> r_bsp,
                std::vector<std::unique_ptr<ac_rcdIntf>> r_ac_rcd,
                std::vector<std::unique_ptr<connector_lockIntf>> r_connector_lock,
                std::vector<std::unique_ptr<powermeterIntf>> r_powermeter_grid_side,
                std::vector<std::unique_ptr<powermeterIntf>> r_powermeter_car_side,
                std::vector<std::unique_ptr<slacIntf>> r_slac, std::vector<std::unique_ptr<ISO15118_chargerIntf>> r_hlc,
                std::vector<std::unique_ptr<isolation_monitorIntf>> r_imd,
                std::vector<std::unique_ptr<power_supply_DCIntf>> r_powersupply_DC, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        telemetry(telemetry),
        p_evse(std::move(p_evse)),
        p_energy_grid(std::move(p_energy_grid)),
        p_token_provider(std::move(p_token_provider)),
        p_random_delay(std::move(p_random_delay)),
        r_bsp(std::move(r_bsp)),
        r_ac_rcd(std::move(r_ac_rcd)),
        r_connector_lock(std::move(r_connector_lock)),
        r_powermeter_grid_side(std::move(r_powermeter_grid_side)),
        r_powermeter_car_side(std::move(r_powermeter_car_side)),
        r_slac(std::move(r_slac)),
        r_hlc(std::move(r_hlc)),
        r_imd(std::move(r_imd)),
        r_powersupply_DC(std::move(r_powersupply_DC)),
        config(config){};

    Everest::MqttProvider& mqtt;
    Everest::TelemetryProvider& telemetry;
    const std::unique_ptr<evse_managerImplBase> p_evse;
    const std::unique_ptr<energyImplBase> p_energy_grid;
    const std::unique_ptr<auth_token_providerImplBase> p_token_provider;
    const std::unique_ptr<uk_random_delayImplBase> p_random_delay;
    const std::unique_ptr<evse_board_supportIntf> r_bsp;
    const std::vector<std::unique_ptr<ac_rcdIntf>> r_ac_rcd;
    const std::vector<std::unique_ptr<connector_lockIntf>> r_connector_lock;
    const std::vector<std::unique_ptr<powermeterIntf>> r_powermeter_grid_side;
    const std::vector<std::unique_ptr<powermeterIntf>> r_powermeter_car_side;
    const std::vector<std::unique_ptr<slacIntf>> r_slac;
    const std::vector<std::unique_ptr<ISO15118_chargerIntf>> r_hlc;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> r_imd;
    const std::vector<std::unique_ptr<power_supply_DCIntf>> r_powersupply_DC;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<Charger> charger;
    sigslot::signal<int> signalNrOfPhasesAvailable;
    types::powermeter::Powermeter get_latest_powermeter_data_billing();
    types::evse_board_support::HardwareCapabilities get_hw_capabilities();
    bool updateLocalMaxCurrentLimit(float max_current); // deprecated
    bool updateLocalMaxWattLimit(float max_watt);       // deprecated
    bool updateLocalEnergyLimit(types::energy::ExternalLimits l);
    types::energy::ExternalLimits getLocalEnergyLimits();
    bool getLocalThreePhases();

    void cancel_reservation(bool signal_event);
    bool is_reserved();
    bool reserve(int32_t id);
    int32_t get_reservation_id();

    bool get_hlc_enabled();
    bool get_hlc_waiting_for_auth_pnc();
    sigslot::signal<types::evse_manager::SessionEvent> signalReservationEvent;

    void charger_was_authorized();

    const std::vector<std::unique_ptr<powermeterIntf>>& r_powermeter_billing();

    // FIXME: this will be removed with proper intergration of BPT on ISO-20
    // on DIN SPEC and -2 we claim a positive charging current on ISO protocol,
    // but the power supply switches to discharge if this flag is set.
    std::atomic_bool is_actually_exporting_to_grid{false};

    types::evse_manager::EVInfo get_ev_info();
    void apply_new_target_voltage_current();

    std::string selected_protocol = "Unknown";

    std::atomic_bool sae_bidi_active{false};

    void ready_to_start_charging();

    std::unique_ptr<IECStateMachine> bsp;
    std::unique_ptr<ErrorHandling> error_handling;

    std::atomic_bool random_delay_enabled{false};
    std::atomic_bool random_delay_running{false};
    std::chrono::time_point<std::chrono::steady_clock> random_delay_end_time;
    std::chrono::time_point<date::utc_clock> random_delay_start_time;
    std::atomic<std::chrono::seconds> random_delay_max_duration;
    std::atomic<std::chrono::time_point<std::chrono::steady_clock>> timepoint_ready_for_charging;

    types::power_supply_DC::Capabilities get_powersupply_capabilities() {
        std::scoped_lock lock(powersupply_capabilities_mutex);
        return powersupply_capabilities;
    }

    void update_powersupply_capabilities(types::power_supply_DC::Capabilities caps) {
        std::scoped_lock lock(powersupply_capabilities_mutex);
        powersupply_capabilities = caps;

        // Inform HLC layer about update of physical values
        types::iso15118_charger::SetupPhysicalValues setup_physical_values;
        setup_physical_values.dc_current_regulation_tolerance = powersupply_capabilities.current_regulation_tolerance_A;
        setup_physical_values.dc_peak_current_ripple = powersupply_capabilities.peak_current_ripple_A;
        setup_physical_values.dc_energy_to_be_delivered = 10000;
        r_hlc[0]->call_set_charging_parameters(setup_physical_values);

        types::iso15118_charger::DC_EVSEMinimumLimits evseMinLimits;
        evseMinLimits.minimum_current = powersupply_capabilities.min_export_current_A;
        evseMinLimits.minimum_voltage = powersupply_capabilities.min_export_voltage_V;
        evseMinLimits.minimum_power = evseMinLimits.minimum_current * evseMinLimits.minimum_voltage;
        r_hlc[0]->call_update_dc_minimum_limits(evseMinLimits);

        // HLC layer will also get new maximum current/voltage/watt limits etc, but those will need to run through
        // energy management first. Those limits will be applied in energy_grid implementation when requesting energy,
        // so it is enough to set the powersupply_capabilities here.
        // FIXME: this is not implemented yet: enforce_limits uses the enforced limits to tell HLC, but capabilities
        // limits are not yet included in request.

        // Inform charger about new max limits
        types::iso15118_charger::DC_EVSEMaximumLimits evseMaxLimits;
        evseMaxLimits.maximum_current = powersupply_capabilities.max_export_current_A;
        evseMaxLimits.maximum_power = powersupply_capabilities.max_export_power_W;
        evseMaxLimits.maximum_voltage = powersupply_capabilities.max_export_voltage_V;
        if (charger) {
            charger->inform_new_evse_max_hlc_limits(evseMaxLimits);
        }
    }
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
    std::mutex powersupply_capabilities_mutex;
    types::power_supply_DC::Capabilities powersupply_capabilities;

    Everest::timed_mutex_traceable power_mutex;
    types::powermeter::Powermeter latest_powermeter_data_billing;

    Everest::Thread energyThreadHandle;
    types::evse_board_support::HardwareCapabilities hw_capabilities;
    bool local_three_phases;
    types::energy::ExternalLimits local_energy_limits;
    const float EVSE_ABSOLUTE_MAX_CURRENT = 80.0;
    bool slac_enabled;

    std::atomic_bool contactor_open{true};

    Everest::timed_mutex_traceable hlc_mutex;

    bool hlc_enabled;

    bool hlc_waiting_for_auth_eim;
    bool hlc_waiting_for_auth_pnc;

    VarContainer<types::isolation_monitor::IsolationMeasurement> isolation_measurement;
    VarContainer<types::power_supply_DC::VoltageCurrent> powersupply_measurement;
    VarContainer<bool> selftest_result;

    double latest_target_voltage;
    double latest_target_current;

    types::authorization::ProvidedIdToken autocharge_token;

    void log_v2g_message(Object m);

    // Reservations
    bool reserved;
    int32_t reservation_id;
    Everest::timed_mutex_traceable reservation_mutex;

    void setup_AC_mode();
    void setup_fake_DC_mode();

    // special funtion to switch mode while session is active
    void switch_AC_mode();
    void switch_DC_mode();

    // DC handlers
    void cable_check();

    void powersupply_DC_on();
    std::atomic_bool powersupply_dc_is_on{false};
    bool powersupply_DC_set(double voltage, double current);
    void powersupply_DC_off();
    bool wait_powersupply_DC_voltage_reached(double target_voltage);
    bool wait_powersupply_DC_below_voltage(double target_voltage);

    bool cable_check_should_exit();

    // EV information
    Everest::timed_mutex_traceable ev_info_mutex;
    types::evse_manager::EVInfo ev_info;
    types::evse_manager::CarManufacturer car_manufacturer{types::evse_manager::CarManufacturer::Unknown};

    void imd_stop();
    void imd_start();
    Everest::Thread telemetryThreadHandle;

    void fail_cable_check();

    // setup sae j2847/2 v2h mode
    void setup_v2h_mode();

    bool check_isolation_resistance_in_range(double resistance);

    static constexpr auto CABLECHECK_CONTACTORS_CLOSE_TIMEOUT{std::chrono::seconds(5)};
    static constexpr double CABLECHECK_CURRENT_LIMIT{2};
    static constexpr double CABLECHECK_INSULATION_FAULT_RESISTANCE_OHM{100000.};
    static constexpr double CABLECHECK_SAFE_VOLTAGE{60.};
    static constexpr int CABLECHECK_SELFTEST_TIMEOUT{30};

    std::atomic_bool current_demand_active{false};
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVSE_MANAGER_HPP
