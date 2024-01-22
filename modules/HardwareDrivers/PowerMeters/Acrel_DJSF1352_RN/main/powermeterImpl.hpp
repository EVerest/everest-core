// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef MAIN_POWERMETER_IMPL_HPP
#define MAIN_POWERMETER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/powermeter/Implementation.hpp>

#include "../Acrel_DJSF1352_RN.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    int powermeter_device_id;
};

class powermeterImpl : public powermeterImplBase {
public:
    powermeterImpl() = delete;
    powermeterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<Acrel_DJSF1352_RN>& mod, Conf& config) :
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
    const Everest::PtrContainer<Acrel_DJSF1352_RN>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1

    enum PmRegisters {
        DC_VOLTAGE,
        DC_VOLT_DECIMAL_POINT,
        DC_CURRENT,
        DC_CURR_DECIMAL_POINT,
        BROKEN_WIRE_DETECT,
        INTERNAL_TEMP,
        RESERVED_1,
        RESERVED_2,
        POWER,
        POWER_DECIMAL_POINT,
        RESERVED_3,
        RESERVED_4,
        TOTAL_POS_ACT_ENERGY_HIGH,
        TOTAL_POS_ACT_ENERGY_LOW,
        TOTAL_REV_ACT_ENERGY_HIGH,
        TOTAL_REV_ACT_ENERGY_LOW,
        VOLTAGE_TRANSFORM_RATIO,
        PRIMARY_RATED_CURRENT,
        SWITCH_IO_STATUS,
        ALARM_STATUS
    };

    enum CurrRateRegisters {
        SHARP,
        PEAK,
        SHOULDER,
        OFF_PEAK
    };

    types::powermeter::Powermeter pm_last_values;

    std::thread output_thread;

    void init_default_values();
    void read_powermeter_values();
    void process_power_data_message(const types::serial_comm_hub_requests::Result message);
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
