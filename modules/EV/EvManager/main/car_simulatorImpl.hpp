// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_CAR_SIMULATOR_IMPL_HPP
#define MAIN_CAR_SIMULATOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/car_simulator/Implementation.hpp>

#include "../EvManager.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "car_simulation.hpp"
#include "command_registry.hpp"
#include <queue>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module::main {

struct Conf {};

class car_simulatorImpl : public car_simulatorImplBase {
public:
    car_simulatorImpl() = delete;
    car_simulatorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvManager>& mod, Conf& config) :
        car_simulatorImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_enable(bool& value) override;
    virtual void handle_execute_charging_session(std::string& value) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvManager>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1

    void run();
    void handle_modify_charging_session(std::string& value);
    bool check_can_execute();
    bool run_simulation_loop();
    void register_all_commands();
    void subscribe_to_variables_on_init();
    void setup_ev_parameters();
    void call_ev_board_support_functions();
    void subscribe_to_external_mqtt();
    void reset_car_simulation_defaults();
    void update_command_queue(std::string& value);
    void set_execution_active(bool value);
    void cancel_charging_session();

    std::unique_ptr<CommandRegistry> command_registry;

    std::mutex car_simulation_mutex;
    std::unique_ptr<CarSimulation> car_simulation;

    bool enabled{false};
    std::atomic<bool> execution_active{false};
    std::atomic<bool> cancel_charging_session_flag{false};
    std::atomic<bool> plugged_in{false};
    size_t loop_interval_ms{};

    std::queue<SimulationCommand> command_queue;

    std::string simulated_already_plugged_in_key;
    std::string simulated_plugged_in_command_key;
    std::string last_commands;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace module::main

#endif // MAIN_CAR_SIMULATOR_IMPL_HPP
