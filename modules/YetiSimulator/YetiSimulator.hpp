// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ac_rcd/Implementation.hpp>
#include <generated/interfaces/connector_lock/Implementation.hpp>
#include <generated/interfaces/ev_board_support/Implementation.hpp>
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#include <generated/interfaces/powermeter/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "util/error_handler.hpp"
#include "util/mqtt_handler.hpp"
#include "util/state.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int connector_id;
};

class YetiSimulator : public Everest::ModuleBase {
public:
    YetiSimulator() = delete;
    YetiSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, Everest::TelemetryProvider& telemetry,
                  std::unique_ptr<powermeterImplBase> p_powermeter,
                  std::unique_ptr<evse_board_supportImplBase> p_board_support,
                  std::unique_ptr<ev_board_supportImplBase> p_ev_board_support, std::unique_ptr<ac_rcdImplBase> p_rcd,
                  std::unique_ptr<connector_lockImplBase> p_connector_lock, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        telemetry(telemetry),
        p_powermeter(std::move(p_powermeter)),
        p_board_support(std::move(p_board_support)),
        p_ev_board_support(std::move(p_ev_board_support)),
        p_rcd(std::move(p_rcd)),
        p_connector_lock(std::move(p_connector_lock)),
        config(config){};

    Everest::MqttProvider& mqtt;
    Everest::TelemetryProvider& telemetry;
    const std::unique_ptr<powermeterImplBase> p_powermeter;
    const std::unique_ptr<evse_board_supportImplBase> p_board_support;
    const std::unique_ptr<ev_board_supportImplBase> p_ev_board_support;
    const std::unique_ptr<ac_rcdImplBase> p_rcd;
    const std::unique_ptr<connector_lockImplBase> p_connector_lock;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    void reset_module_state();
    Everest::MqttProvider& get_mqtt() const;
    state::ModuleState& get_module_state() const;
    const ModuleInfo& get_info() const;
    evse_board_supportImplBase& get_board_support() const;
    ev_board_supportImplBase& get_ev_board_support() const;
    powermeterImplBase& get_powermeter() const;
    ac_rcdImplBase& get_ac_rcd() const;
    connector_lockImplBase& get_connector_lock() const;
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend struct LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    void run_telemetry_slow() const;
    void run_telemetry_fast() const;
    void start_simulation(int sleep_time_ms);
    [[noreturn]] void run_simulation(int sleep_time_ms) const;
    void simulation_step() const;
    void check_error_rcd() const;
    void read_from_car() const;
    void simulation_statemachine() const;
    void add_noise() const;
    void simulate_powermeter() const;
    void publish_ev_board_support() const;
    void publish_powermeter() const;
    void publish_telemetry() const;
    void publish_keepalive() const;
    void drawPower(int l1, int l2, int l3, int n) const;
    static void clear_disconnect_errors(ErrorHandler& error_handler, const evse_board_supportImplBase& board_support);
    void powerOn(evse_board_supportImplBase& board_support) const;
    void powerOff(evse_board_supportImplBase& board_support) const;
    void reset_powermeter() const;
    [[nodiscard]] types::board_support_common::ProximityPilot read_pp_ampacity() const;

    std::unique_ptr<state::ModuleState> module_state;
    std::unique_ptr<MqttHandler> mqtt_handler;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
types::powermeter::Powermeter power_meter_external(const state::PowermeterData& powermeter_data);
double duty_cycle_to_amps(double dc);
bool is_voltage_in_range(const double voltage, double center);
void publish_event(evse_board_supportImplBase& board_support, state::State event);
static types::board_support_common::BspEvent event_to_enum(state::State event);
static std::string event_to_string(state::State state);
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module
