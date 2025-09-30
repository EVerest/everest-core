// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
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
    const int MAIN_LOOP_INTERVAL_S = 1;

    /**
     * @brief reads a number of input registers via modbus
     * @note this raises a everest communication fault if unsuccessful
     * @param first_register_address the address of the first register to read (protocol address)
     * @param register_quantity the number of registers to read
     * @return a vector of integers containing the register values, or std::nullopt in case of a communication error
     */
    std::optional<std::vector<int>> read_input_registers(uint16_t first_protocol_register_address,
                                                         uint16_t register_quantity);
    /**
     * @brief reads a number of holding registers via modbus
     * @note this raises a everest communication fault if unsuccessful
     * @param first_register_address the address of the first register to read (protocol address)
     * @param register_quantity the number of registers to read
     * @return a vector of integers containing the register values, or std::nullopt in case of a communication error
     */
    std::optional<std::vector<int>> read_holding_registers(uint16_t first_protocol_register_address,
                                                           uint16_t register_quantity);

    /**
     * @brief writes a single holding register via modbus
     * @note this raises a everest communication fault if unsuccessful
     * @param protocol_address the address of the register to write (protocol address)
     * @param value the value to write
     * @return true on success, false on communication error
     */
    bool write_holding_register(uint16_t protocol_address, uint16_t value);
    /**
     * @brief writes multiple holding registers via modbus
     * @note this raises a everest communication fault if unsuccessful
     * @param protocol_address the address of the first register to write (protocol address)
     * @param values the values to write
     * @return true on success, false on communication error
     */
    bool write_holding_registers(uint16_t protocol_address, const std::vector<uint16_t>& values);

    // true if measurement should be published, set via the start/stop commands
    std::atomic_bool publish_enabled = false;

    // true if a self test has been triggered and the device should do a self test.
    // stays true until the self test is finished or the timeout is reached
    std::atomic_bool self_test_triggered = false;
    // true if a self test has been triggered and the device has started the self test.
    // When triggering a self test, this stays false until the device switches to self test mode.
    // It is reset when self_test_triggered is reset - after the self test is finished or the timeout is reached
    std::atomic_bool self_test_running = false;
    // Deadline for the current self test. If the self test is not finished by this time, it is considered failed.
    // Its value is only valid if self_test_triggered is true
    std::chrono::steady_clock::time_point self_test_deadline;

    /**
     * @brief raises a everest communication fault error if not already active
     * @note the fault is cleared in the main loop when communication is successful again
     */
    void raise_communication_fault();

    /**
     * @brief raises or clears a everest device fault based on the provided device fault register value
     * @param device_fault_register the current value of the device fault register
     * @note the error is raised if the value is not NoFailure and cleared if it is NoFailure
     */
    void raise_or_clear_device_fault(const DeviceFault_30001& device_fault_register);

    /**
     * @brief Configures the device according to the current config.
     * Only writes the registers if the current value differs from the desired value.
     * Also calls \c update_control_word1 to set the control word according to the current config and state.
     * @note this also raises a everest communication fault if unsuccessful
     * @return true on success, false on communication error
     */
    bool configure_device();

    /**
     * @brief Update the timeout registers based on the config
     * @note should be called once per main loop iteration
     * @return true on success, false on communication error
     */
    bool write_timeout_registers();

    /**
     * @brief reads the current isolation measurement and voltage from the device
     * @note this raises a everest communication fault if unsuccessful
     * @return the isolation measurement, or std::nullopt in case of a communication error
     */
    std::optional<types::isolation_monitor::IsolationMeasurement> read_isolation_measurement();

    /**
     * @brief reads the current device fault and state from the device (first two input registers)
     * @note this raises a everest communication fault if unsuccessful
     * @return a tuple of device fault and device state, or std::nullopt in case of a communication error
     */
    std::optional<std::tuple<DeviceFault_30001, DeviceState_30002>> read_device_fault_and_state();

    enum class ControlWord1Action {
        None,
        StartSelfTest,
        ResetFaults,
    };

    /**
     * @brief Updates the control word 1 register (address 40001) based on the current state and config.
     * Use action to trigger a specific action (self test or reset).
     * @param action the action to perform, defaults to None
     * @return true on success, false on communication error
     */
    bool update_control_word1(ControlWord1Action action = ControlWord1Action::None);
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_ISOLATION_MONITOR_IMPL_HPP
