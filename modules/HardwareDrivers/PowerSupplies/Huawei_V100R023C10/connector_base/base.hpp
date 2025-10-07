// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../Huawei_V100R023C10.hpp"
#include <connector.hpp>
#include <generated/interfaces/power_supply_DC/Implementation.hpp>
#include <generated/interfaces/power_supply_DC/Types.hpp>

#include <atomic>
#include <thread>

namespace module {

struct EverestConnectorConfig {
    int global_connector_number;
    double max_export_current_A;
    double max_export_power_W;

    template <typename T> static EverestConnectorConfig from_everest(T in) {
        EverestConnectorConfig out;
        out.global_connector_number = in.global_connector_number;
        out.max_export_current_A = in.max_export_current_A;
        out.max_export_power_W = in.max_export_power_W;

        return out;
    }
};

class ConnectorBase {
public:
    /**
     * @brief Constructor
     *
     * @param connector    Connector number 0-3
     * @param ev_callbacks everest callbacks
     */
    ConnectorBase(uint8_t connector, power_supply_DCImplBase* impl);

    // Note that in init() the dispenser in the main class was not initialized yet
    void ev_init();
    void ev_ready();
    void ev_handle_setMode(types::power_supply_DC::Mode mode, types::power_supply_DC::ChargingPhase phase);
    void ev_handle_setExportVoltageCurrent(double voltage, double current);
    void ev_handle_setImportVoltageCurrent(double voltage, double current);

    void ev_set_config(EverestConnectorConfig config);
    void ev_set_mod(const Everest::PtrContainer<Huawei_V100R023C10>& mod);

    Connector* get_connector();

    ConnectorConfig get_connector_config();

    void raise_communication_fault();
    void clear_communication_fault();

    void raise_psu_not_running();
    void clear_psu_not_running();

    void raise_psu_error(ErrorEvent error);
    void clear_psu_error(ErrorEvent error);

    /**
     * @brief Does an car connect - disconnect cycle blockingly
     *
     */
    void do_init_hmac_acquire();

private:
    void raise_module_placeholder_allocation_failure();
    void clear_module_placeholder_allocation_failure();

    void worker_thread();
    std::thread worker_thread_handle;

    void update_module_placeholder_errors();

    void update_hack();
    void update_and_publish_capabilities();
    void init_capabilities();

    std::atomic<bool> module_placeholder_allocation_failure_raised;

    std::mutex connector_mutex;

    uint16_t connector_no; // 0-3
    power_supply_DCImplBase* impl;
    EverestConnectorConfig config;
    Everest::PtrContainer<Huawei_V100R023C10> mod;

    types::power_supply_DC::Capabilities caps;
    types::power_supply_DC::Mode last_mode;
    types::power_supply_DC::ChargingPhase last_phase;

    double export_voltage{0.};
    double export_current_limit{0.};

    struct {
        std::atomic<float> upstream_voltage;
        std::atomic<float> output_voltage;
        std::atomic<float> output_current;
        std::atomic<ContactorStatus> contactor_status;
    } external_provided_data;
};

}; // namespace module
