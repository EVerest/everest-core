// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef YETI_SIMULATOR_HPP
#define YETI_SIMULATOR_HPP

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
#include <generated/interfaces/evse_megawatt_charging/Implementation.hpp>
#include <generated/interfaces/powermeter/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "util/errors.hpp"
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
                  std::unique_ptr<evse_megawatt_chargingImplBase> p_mcs,
                  std::unique_ptr<ev_board_supportImplBase> p_ev_board_support, std::unique_ptr<ac_rcdImplBase> p_rcd,
                  std::unique_ptr<connector_lockImplBase> p_connector_lock, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        telemetry(telemetry),
        p_powermeter(std::move(p_powermeter)),
        p_board_support(std::move(p_board_support)),
        p_mcs(std::move(p_mcs)),
        p_ev_board_support(std::move(p_ev_board_support)),
        p_rcd(std::move(p_rcd)),
        p_connector_lock(std::move(p_connector_lock)),
        config(config){};

    Everest::MqttProvider& mqtt;
    Everest::TelemetryProvider& telemetry;
    const std::unique_ptr<powermeterImplBase> p_powermeter;
    const std::unique_ptr<evse_board_supportImplBase> p_board_support;
    const std::unique_ptr<evse_megawatt_chargingImplBase> p_mcs;
    const std::unique_ptr<ev_board_supportImplBase> p_ev_board_support;
    const std::unique_ptr<ac_rcdImplBase> p_rcd;
    const std::unique_ptr<connector_lockImplBase> p_connector_lock;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    std::unique_ptr<state::ModuleState> module_state;

    void reset_module_state() {
        module_state = std::make_unique<state::ModuleState>();
    }

    void pwm_on(const double dutycycle);
    void pwm_off();
    void pwm_f();
    void ce_on(const double dutycycle);
    void ce_off();
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
    void run_telemetry_slow() const;
    void run_telemetry_fast() const;
    [[noreturn]] void run_simulation(int sleep_time_ms);
    void simulation_step();
    void check_error_rcd();
    void read_from_car();
    void simulation_statemachine();
    void add_noise();
    void simulate_powermeter();
    void publish_ev_board_support() const;
    void publish_powermeter();
    void publish_telemetry();
    void publish_keepalive();
    void drawPower(int l1, int l2, int l3, int n) const;
    void powerOn();
    void powerOff();
    void reset_powermeter() const;
    [[nodiscard]] types::board_support_common::ProximityPilot read_pp_ampacity() const;

    void publish_event(state::State event);
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // YETI_SIMULATOR_HPP
