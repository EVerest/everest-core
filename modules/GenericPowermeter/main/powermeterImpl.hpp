// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../GenericPowermeter.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <optional>
#include <string>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string model;
    int powermeter_device_id;
    int modbus_base_address;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<GenericPowermeter>& mod, Conf& config) :
        powermeterImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::powermeter::TransactionStartResponse
    handle_start_transaction(types::powermeter::TransactionReq& value) override;
    virtual types::powermeter::TransactionStopResponse handle_stop_transaction(std::string& transaction_id) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<GenericPowermeter>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    enum PowermeterRegisters {
        // do not change order or index of these elements!
        ENERGY_WH_IMPORT_TOTAL,
        ENERGY_WH_IMPORT_L1,
        ENERGY_WH_IMPORT_L2,
        ENERGY_WH_IMPORT_L3,
        ENERGY_WH_EXPORT_TOTAL,
        ENERGY_WH_EXPORT_L1,
        ENERGY_WH_EXPORT_L2,
        ENERGY_WH_EXPORT_L3,
        POWER_W_TOTAL,
        POWER_W_L1,
        POWER_W_L2,
        POWER_W_L3,
        VOLTAGE_V_DC,
        VOLTAGE_V_L1,
        VOLTAGE_V_L2,
        VOLTAGE_V_L3,
        REACTIVE_POWER_VAR_TOTAL,
        REACTIVE_POWER_VAR_L1,
        REACTIVE_POWER_VAR_L2,
        REACTIVE_POWER_VAR_L3,
        CURRENT_A_DC,
        CURRENT_A_L1,
        CURRENT_A_L2,
        CURRENT_A_L3,
        FREQUENCY_HZ_L1,
        FREQUENCY_HZ_L2,
        FREQUENCY_HZ_L3,
        NUM_PM_REGISTERS
    };

    enum ModbusFunctionType {
        READ_HOLDING_REGISTER,
        READ_INPUT_REGISTER,
        REGISTER_TYPE_UNDEFINED
    };

    struct RegisterData {
        float multiplier;
        PowermeterRegisters type;
        uint16_t start_register;
        ModbusFunctionType start_register_function;
        uint16_t exponent_register;
        ModbusFunctionType exponent_register_function;
        uint16_t num_registers;
    };

    std::vector<RegisterData> pm_configuration;

    types::powermeter::Powermeter pm_last_values;

    std::thread output_thread;

    /// @brief Remember whether we already logged the meter's unavailability.
    bool meter_is_unavailable{false};

    void init_default_values();
    void init_register_assignments(const json& loaded_registers);
    bool assign_register_data(const json& registers, const PowermeterRegisters register_type,
                              const std::string& register_selector);
    void assign_register_sublevel_data(const json& registers, const PowermeterRegisters& register_type,
                                       const std::string& register_selector, const std::string& sublevel_selector,
                                       const uint8_t offset);
    powermeterImpl::ModbusFunctionType select_modbus_function(const uint8_t function_code);
    void read_powermeter_values();
    bool read_register(const RegisterData& register_config);
    // void process_response(const RegisterData& message_type,
    //                       const types::serial_comm_hub_requests::Result register_message,
    //                       const types::serial_comm_hub_requests::Result exponent_message);
    bool process_response(
        const RegisterData& register_data, const types::serial_comm_hub_requests::Result& register_message,
        std::optional<std::reference_wrapper<const types::serial_comm_hub_requests::Result>> exponent_message);
    float merge_register_values_into_element(const RegisterData& reg_data, const int16_t exponent,
                                             const types::serial_comm_hub_requests::Result& reg_message);
    void process_current_rate_message(const types::serial_comm_hub_requests::Result message);
    void output_error_with_content(const types::serial_comm_hub_requests::Result& response);
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
