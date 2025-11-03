#ifndef YETI_DRIVER_HPP
#define YETI_DRIVER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 0.0.1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/powermeter/Implementation.hpp>
#include <generated/board_support_AC/Implementation.hpp>
#include <generated/yeti_extras/Implementation.hpp>
#include <generated/debug_json/Implementation.hpp>
#include <generated/debug_json/Implementation.hpp>
#include <generated/debug_json/Implementation.hpp>
#include <generated/debug_json/Implementation.hpp>
#include <generated/yeti_simulation_control/Implementation.hpp>


// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "yeti_comms/evSerial.h"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string serial_port;
    int baud_rate;
    std::string control_mode;
};

class YetiDriver : public Everest::ModuleBase {
public:
    YetiDriver() = delete;
    YetiDriver(
        Everest::MqttProvider& mqtt_provider,
        std::unique_ptr<powermeterImplBase> p_powermeter,
        std::unique_ptr<board_support_ACImplBase> p_board_support,
        std::unique_ptr<yeti_extrasImplBase> p_yeti_extras,
        std::unique_ptr<debug_jsonImplBase> p_debug_yeti,
        std::unique_ptr<debug_jsonImplBase> p_debug_powermeter,
        std::unique_ptr<debug_jsonImplBase> p_debug_state,
        std::unique_ptr<debug_jsonImplBase> p_debug_keepalive,
        std::unique_ptr<yeti_simulation_controlImplBase> p_yeti_simulation_control,
        Conf& config
    ) :
        mqtt(mqtt_provider),
        p_powermeter(std::move(p_powermeter)),
        p_board_support(std::move(p_board_support)),
        p_yeti_extras(std::move(p_yeti_extras)),
        p_debug_yeti(std::move(p_debug_yeti)),
        p_debug_powermeter(std::move(p_debug_powermeter)),
        p_debug_state(std::move(p_debug_state)),
        p_debug_keepalive(std::move(p_debug_keepalive)),
        p_yeti_simulation_control(std::move(p_yeti_simulation_control)),
        config(config)
    {};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<powermeterImplBase> p_powermeter;
    const std::unique_ptr<board_support_ACImplBase> p_board_support;
    const std::unique_ptr<yeti_extrasImplBase> p_yeti_extras;
    const std::unique_ptr<debug_jsonImplBase> p_debug_yeti;
    const std::unique_ptr<debug_jsonImplBase> p_debug_powermeter;
    const std::unique_ptr<debug_jsonImplBase> p_debug_state;
    const std::unique_ptr<debug_jsonImplBase> p_debug_keepalive;
    const std::unique_ptr<yeti_simulation_controlImplBase> p_yeti_simulation_control;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
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
