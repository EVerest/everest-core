// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef YETI_DRIVER_HPP
#define YETI_DRIVER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ac_rcd/Implementation.hpp>
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#include <generated/interfaces/powermeter/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "yeti_comms/evSerial.h"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string serial_port;
    int baud_rate;
    std::string control_mode;
    int reset_gpio;
    int caps_min_current_A;
};

class YetiDriver : public Everest::ModuleBase {
public:
    YetiDriver() = delete;
    YetiDriver(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, Everest::TelemetryProvider& telemetry,
               std::unique_ptr<powermeterImplBase> p_powermeter,
               std::unique_ptr<evse_board_supportImplBase> p_board_support, std::unique_ptr<ac_rcdImplBase> p_rcd,
               Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        telemetry(telemetry),
        p_powermeter(std::move(p_powermeter)),
        p_board_support(std::move(p_board_support)),
        p_rcd(std::move(p_rcd)),
        config(config){};

    Everest::MqttProvider& mqtt;
    Everest::TelemetryProvider& telemetry;
    const std::unique_ptr<powermeterImplBase> p_powermeter;
    const std::unique_ptr<evse_board_supportImplBase> p_board_support;
    const std::unique_ptr<ac_rcdImplBase> p_rcd;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    void publish_external_telemetry_livedata(const std::string& topic, const Everest::TelemetryMap& data);
    evSerial serial;
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
    Everest::TelemetryMap telemetry_power_path_controller_version;
    Everest::TelemetryMap telemetry_power_path_controller;
    Everest::TelemetryMap telemetry_power_switch;
    Everest::TelemetryMap telemetry_rcd;
    std::mutex telemetry_mutex;
    Everest::Thread telemetryThreadHandle;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
Everest::json power_meter_data_to_json(const PowerMeter& p);
Everest::json debug_update_to_json(const DebugUpdate& d);
Everest::json state_update_to_json(const StateUpdate& s);
Everest::json keep_alive_lo_to_json(const KeepAliveLo& k);
std::string error_type_to_string(ErrorFlags s);
std::string state_to_string(const StateUpdate& s);
InterfaceControlMode str_to_control_mode(std::string data);
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // YETI_DRIVER_HPP
