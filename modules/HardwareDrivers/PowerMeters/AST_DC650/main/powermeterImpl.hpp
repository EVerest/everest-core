// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../AST_DC650.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "ast_app_layer.hpp"
#include "diagnostics.hpp"
#include "serial_device.hpp"
#include "slip_protocol.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int powermeter_device_id;
    std::string serial_port;
    int baudrate;
    int parity;
    int rs485_direction_gpio;
    bool ignore_echo;
    int gmt_offset_quarter_hours;
    bool publish_device_data;
    bool publish_device_diagnostics;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<AST_DC650>& mod, Conf& config) :
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
    const Everest::PtrContainer<AST_DC650>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here

    enum class MessageStatus : std::uint8_t {
        NONE = 0,
        SENT = 1,
        RECEIVED = 2
    };

    MessageStatus start_transaction_msg_status{MessageStatus::NONE};
    ast_app_layer::CommandResult start_transact_result{};
    MessageStatus stop_transaction_msg_status{MessageStatus::NONE};
    ast_app_layer::CommandResult stop_transact_result{};
    MessageStatus get_transaction_values_msg_status{MessageStatus::NONE};

    serial_device::SerialDevice serial_device{};
    slip_protocol::SlipProtocol slip{};
    ast_app_layer::AstAppLayer app_layer{};

    types::powermeter::Powermeter pm_last_values;

    DeviceData device_data_obj{};
    DeviceDiagnostics device_diagnostics_obj{};
    Logging logging_obj{};
    ast_app_layer::ErrorCategory category_requested{};
    ast_app_layer::ErrorSource source_requested{};
    std::uint8_t error_diagnostics_target{0};
    std::string last_ocmf_str;

    void init_default_values();
    void read_powermeter_values();
    void set_device_time();
    void set_device_charge_point_id(ast_app_layer::UserIdType id_type, std::string charge_point_id);
    void read_device_data();
    void read_diagnostics_data();
    void publish_device_data_topic();
    void publish_device_diagnostics_topic();
    void publish_logging_topic();
    void get_device_public_key();
    void readRegisters();
    ast_app_layer::CommandResult process_response(const std::vector<std::uint8_t>& register_message);
    void request_device_type();
    void request_error_diagnostics(std::uint8_t addr);
    void error_diagnostics(std::uint8_t addr);
    ast_app_layer::CommandResult receive_response();
    std::string get_meter_ocmf();

    static constexpr auto TIMEOUT_2s{std::chrono::seconds(2)};
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
