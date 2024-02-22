// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef POWERMETER_BSM_HPP
#define POWERMETER_BSM_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/powermeter/Implementation.hpp>
#include <generated/interfaces/sunspec_ac_meter/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/serial_communication_hub/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "lib/transport.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int power_unit_id;
    int sunspec_base_address;
    int update_interval;
    int watchdog_wakeup_interval;
    std::string serial_device;
    int baud;
    bool use_serial_comm_hub;
    std::string meter_id;
};

class PowermeterBSM : public Everest::ModuleBase {
public:
    PowermeterBSM() = delete;
    PowermeterBSM(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                  Everest::WatchdogSupervisor& watchdog_supervisor, std::unique_ptr<powermeterImplBase> p_main,
                  std::unique_ptr<sunspec_ac_meterImplBase> p_ac_meter,
                  std::vector<std::unique_ptr<serial_communication_hubIntf>> r_serial_com_0_connection, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        watchdog_supervisor(watchdog_supervisor),
        p_main(std::move(p_main)),
        p_ac_meter(std::move(p_ac_meter)),
        r_serial_com_0_connection(std::move(r_serial_com_0_connection)),
        config(config){};

    Everest::MqttProvider& mqtt;
    Everest::WatchdogSupervisor& watchdog_supervisor;
    const std::unique_ptr<powermeterImplBase> p_main;
    const std::unique_ptr<sunspec_ac_meterImplBase> p_ac_meter;
    const std::vector<std::unique_ptr<serial_communication_hubIntf>> r_serial_com_0_connection;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    transport::AbstractDataTransport::Spt get_data_transport();
    std::recursive_mutex& get_device_mutex() {
        return m_device_mutex;
    }
    std::chrono::seconds get_update_interval() const {
        return m_update_interval;
    }
    std::chrono::seconds get_watchdog_interval() const {
        return m_watchdog_interval;
    }
    void dump_config_to_log();
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here

    std::chrono::seconds m_module_start_timestamp;

    // stuff for transport
    protocol_related_types::ModbusUnitId m_unit_id{};
    std::string m_serial_device_name;
    protocol_related_types::ModbusRegisterAddress m_modbus_base_address{40000};

    // variable publish interval
    std::chrono::seconds m_update_interval;

    // interval for watchdog variable thread
    std::chrono::seconds m_watchdog_interval;

    // implementations have to lock this mutex before accessing the transport.
    std::recursive_mutex m_device_mutex;

    void read_config();

    transport::AbstractDataTransport::Spt m_transport_spt;
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
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // POWERMETER_BSM_HPP
