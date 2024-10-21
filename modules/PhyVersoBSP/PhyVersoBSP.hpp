// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef PHY_VERSO_BSP_HPP
#define PHY_VERSO_BSP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ac_rcd/Implementation.hpp>
#include <generated/interfaces/connector_lock/Implementation.hpp>
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#include <generated/interfaces/generic_array/Implementation.hpp>
#include <generated/interfaces/led_driver/Implementation.hpp>
#include <generated/interfaces/phyverso_mcu_temperature/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "phyverso_gpio/evGpio.h"
#include "phyverso_mcu_comms/evSerial.h"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string serial_port;
    int baud_rate;
    int reset_gpio;
    int conn1_max_current_A_import;
    int conn1_min_current_A_import;
    int conn1_min_phase_count_import;
    int conn1_max_phase_count_import;
    int conn1_min_current_A_export;
    int conn1_max_current_A_export;
    int conn1_min_phase_count_export;
    int conn1_max_phase_count_export;
    bool conn1_has_socket;
    bool conn1_dc;
    int conn2_max_current_A_import;
    int conn2_min_current_A_import;
    int conn2_min_phase_count_import;
    int conn2_max_phase_count_import;
    int conn2_min_current_A_export;
    int conn2_max_current_A_export;
    int conn2_min_phase_count_export;
    int conn2_max_phase_count_export;
    bool conn2_has_socket;
    bool conn2_dc;
    int reset_gpio_bank;
    int reset_gpio_pin;
    int conn1_motor_lock_type;
    int conn2_motor_lock_type;
    bool conn1_gpio_stop_button_enabled;
    std::string conn1_gpio_stop_button_bank;
    int conn1_gpio_stop_button_pin;
    bool conn1_gpio_stop_button_invert;
    bool conn2_gpio_stop_button_enabled;
    std::string conn2_gpio_stop_button_bank;
    int conn2_gpio_stop_button_pin;
    bool conn2_gpio_stop_button_invert;
};

class PhyVersoBSP : public Everest::ModuleBase {
public:
    PhyVersoBSP() = delete;
    PhyVersoBSP(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, Everest::TelemetryProvider& telemetry,
                std::unique_ptr<evse_board_supportImplBase> p_connector_1,
                std::unique_ptr<evse_board_supportImplBase> p_connector_2, std::unique_ptr<ac_rcdImplBase> p_rcd_1,
                std::unique_ptr<ac_rcdImplBase> p_rcd_2, std::unique_ptr<connector_lockImplBase> p_connector_lock_1,
                std::unique_ptr<connector_lockImplBase> p_connector_lock_2,
                std::unique_ptr<phyverso_mcu_temperatureImplBase> p_phyverso_mcu_temperature,
                std::unique_ptr<generic_arrayImplBase> p_system_specific_data_1,
                std::unique_ptr<generic_arrayImplBase> p_system_specific_data_2,
                std::unique_ptr<led_driverImplBase> p_led_driver_1,
                std::unique_ptr<led_driverImplBase> p_led_driver_2,
                Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        telemetry(telemetry),
        p_connector_1(std::move(p_connector_1)),
        p_connector_2(std::move(p_connector_2)),
        p_rcd_1(std::move(p_rcd_1)),
        p_rcd_2(std::move(p_rcd_2)),
        p_connector_lock_1(std::move(p_connector_lock_1)),
        p_connector_lock_2(std::move(p_connector_lock_2)),
        p_phyverso_mcu_temperature(std::move(p_phyverso_mcu_temperature)),
        p_system_specific_data_1(std::move(p_system_specific_data_1)),
        p_system_specific_data_2(std::move(p_system_specific_data_2)),
        p_led_driver_1(std::move(p_led_driver_1)),
        p_led_driver_2(std::move(p_led_driver_2)),
        config(config),
        serial(verso_config),
        gpio(verso_config){};

    Everest::MqttProvider& mqtt;
    Everest::TelemetryProvider& telemetry;
    const std::unique_ptr<evse_board_supportImplBase> p_connector_1;
    const std::unique_ptr<evse_board_supportImplBase> p_connector_2;
    const std::unique_ptr<ac_rcdImplBase> p_rcd_1;
    const std::unique_ptr<ac_rcdImplBase> p_rcd_2;
    const std::unique_ptr<connector_lockImplBase> p_connector_lock_1;
    const std::unique_ptr<connector_lockImplBase> p_connector_lock_2;
    const std::unique_ptr<phyverso_mcu_temperatureImplBase> p_phyverso_mcu_temperature;
    const std::unique_ptr<generic_arrayImplBase> p_system_specific_data_1;
    const std::unique_ptr<generic_arrayImplBase> p_system_specific_data_2;
    const std::unique_ptr<led_driverImplBase> p_led_driver_1;
    const std::unique_ptr<led_driverImplBase> p_led_driver_2;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    evSerial serial;
    evConfig verso_config;
    evGpio gpio;
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
    void everest_config_to_verso_config();
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // PHY_VERSO_BSP_HPP
