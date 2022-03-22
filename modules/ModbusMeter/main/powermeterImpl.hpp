// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include <generated/powermeter/Implementation.hpp>

#include "../ModbusMeter.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
#include <memory>
#include <vector>

#include <modbus/modbus_client.hpp>
#include <modbus/utils.hpp>
#include <sunspec/conversion.hpp>
#include <utils/thread.hpp>
#include <date/date.h>
#include <date/tz.h>
#include <chrono>

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string modbus_ip_address;
    int modbus_port;
    int power_unit_id;
    int power_in_register;
    int power_in_length;
    int power_out_register;
    int power_out_length;
    int energy_unit_id;
    int energy_in_register;
    int energy_in_length;
    int energy_out_register;
    int energy_out_length;
    int update_interval;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<ModbusMeter>& mod, Conf& config) :
        powermeterImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    void run_meter_loop();

    uint32_t read_power_in();
    uint32_t read_power_out();
    uint32_t read_energy_in();
    uint32_t read_energy_out();
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual std::string handle_get_signed_meter_value(std::string& auth_token) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<ModbusMeter>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    void init_modbus_client();
    Everest::Thread meter_loop_thread;

    std::unique_ptr<everest::connection::TCPConnection> tcp_conn;
    std::unique_ptr<everest::modbus::ModbusTCPClient> modbus_client;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWERMETER_IMPL_HPP
