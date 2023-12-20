// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../PowermeterBSM_WS36A.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <generated/types/serial_comm_hub_requests.hpp>
#include <string>
#include <utils/config.hpp>

namespace module {
namespace utils {
/// @brief The type of the modbus register.
enum class RegisterType {
    HOLDING = 3,
    INPUT = 4
};

/// @brief A concise defintion of the register data
struct Register {
    uint32_t start_register;
    uint32_t num_registers;
    RegisterType type = RegisterType::HOLDING;
};

} // namespace utils
} // namespace module

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int powermeter_device_id;
    int modbus_base_address;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<PowermeterBSM_WS36A>& mod, Conf& config) :
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
    const Everest::PtrContainer<PowermeterBSM_WS36A>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    using TransactionStartResponse = types::powermeter::TransactionStartResponse;
    using TransactionStopResponse = types::powermeter::TransactionStopResponse;
    using TransactionReq = types::powermeter::TransactionReq;

    TransactionStartResponse handle_start_transaction_impl(const TransactionReq& _value);
    TransactionStopResponse handle_stop_transaction_impl(const std::string& _transaction_id);

    /// @brief Read the register address and de-serializes it to "Output".
    /// The Output type must be deserializable.
    template <typename Output> Output read_register(const utils::Register& register_config) const;

    /// @brief Writes the "Input" to the register address.
    /// The Input must be serializable.
    template <typename Input> void write_register(const utils::Register& register_data, const Input& data) const;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
