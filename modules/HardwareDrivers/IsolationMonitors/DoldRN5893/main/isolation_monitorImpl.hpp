// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_ISOLATION_MONITOR_IMPL_HPP
#define MAIN_ISOLATION_MONITOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/isolation_monitor/Implementation.hpp>

#include "../DoldRN5893.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "registers.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class isolation_monitorImpl : public isolation_monitorImplBase {
public:
    isolation_monitorImpl() = delete;
    isolation_monitorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<DoldRN5893>& mod, Conf& config) :
        isolation_monitorImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_start() override;
    virtual void handle_stop() override;
    virtual void handle_start_self_test(double& test_voltage_V) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<DoldRN5893>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    /**
     * @brief reads a number of input registers via modbus
     * @param first_register_address the address of the first register to read (protocol address)
     * @param register_quantity the number of registers to read
     * @return a vector of integers containing the register values, or std::nullopt in case of a communication error
     */
    std::optional<std::vector<int>> read_input_registers(uint16_t first_protocol_register_address,
                                                         uint16_t register_quantity);
    std::optional<std::vector<int>> read_holding_registers(uint16_t first_protocol_register_address,
                                                           uint16_t register_quantity);
    // writes a single holding register via modbus. Returns true on success
    bool write_holding_register(uint16_t protocol_address, uint16_t value);
    // writes multiple holding registers via modbus. Returns true on success
    bool write_holding_registers(uint16_t protocol_address, std::vector<uint16_t> values);

    // if true, the main loop publishes measurements
    std::atomic_bool publish_enabled = false;

    // true if a self test has been triggered and the device should do a self test.
    // stays true until the self test is finished or the timeout is reached
    std::atomic_bool self_test_triggered = false;
    // true if a self test has been triggered and the device has started the self test.
    // When triggering a self test, this stays false until the device switches to self test mode.
    std::atomic_bool self_test_running = false;
    // Deadline for the current self test. If the self test is not finished by this time, it is considered failed
    std::chrono::steady_clock::time_point self_test_deadline;

    // Raises a communication fault if not already raised
    void raise_communication_fault();

    // Upload the device configuration to the device. Returns true on success
    bool configure_device();

    // Reads the current isolation measurement and voltage from the device. Returns std::nullopt on error
    std::optional<types::isolation_monitor::IsolationMeasurement> read_isolation_measurement();

    // Read the two first input registers containing the device fault and device state
    std::optional<std::tuple<DeviceFault_30001, DeviceState_30002>> read_device_fault_and_state();

    // Updates the control word 1 register (address 40001) based on the current state and config.
    // Set start_self_test to true to start a self test.
    // Returns true on success
    bool update_control_word1(bool start_self_test = false);
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_ISOLATION_MONITOR_IMPL_HPP
