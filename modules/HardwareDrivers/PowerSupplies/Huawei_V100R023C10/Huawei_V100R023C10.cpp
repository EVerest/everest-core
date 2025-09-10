// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Huawei_V100R023C10.hpp"
#include "connector_1/power_supply_DCImpl.hpp"
#include "connector_2/power_supply_DCImpl.hpp"
#include "connector_3/power_supply_DCImpl.hpp"
#include "connector_4/power_supply_DCImpl.hpp"

namespace module {

static ConnectorBase* get_connector_impl(Huawei_V100R023C10* mod, uint8_t connector) {
    switch (connector) {
    case 0:
        return &(dynamic_cast<connector_1::power_supply_DCImpl*>(mod->p_connector_1.get()))->base;
        break;
    case 1:
        return &(dynamic_cast<connector_2::power_supply_DCImpl*>(mod->p_connector_2.get()))->base;
        break;
    case 2:
        return &(dynamic_cast<connector_3::power_supply_DCImpl*>(mod->p_connector_3.get()))->base;
        break;
    case 3:
        return &(dynamic_cast<connector_4::power_supply_DCImpl*>(mod->p_connector_4.get()))->base;
        break;
    default:
        throw std::runtime_error("Connector number out of bounds (expected 0-3): " + std::to_string(connector));

        break;
    }
}

static std::vector<ConnectorBase*> get_connector_bases(Huawei_V100R023C10* mod, uint8_t connectors_used) {
    std::vector<ConnectorBase*> connector_bases;
    for (uint8_t i = 0; i < connectors_used; i++) {
        connector_bases.push_back(get_connector_impl(mod, i));
    }
    return connector_bases;
}

void Huawei_V100R023C10::init() {
    this->communication_fault_raised = false;
    this->psu_not_running_raised = false;
    this->initial_hmac_acquired = false;

    number_of_connectors_used = this->r_board_support.size();
    if (number_of_connectors_used > 4) {
        throw std::runtime_error("Got more board support modules than connectors supported");
    }

    EVLOG_info << "Assuming number of connectors used = " << number_of_connectors_used
               << " (based on number of connected board support modules)";

    if (this->r_isolation_monitor.size() != number_of_connectors_used) {
        EVLOG_critical << "Number of IMDs does not match number of connectors used";
        throw std::runtime_error("Number of powermeters does not match number of connectors in use");
    }
    if (this->r_carside_powermeter.size() != 0 && this->r_carside_powermeter.size() != number_of_connectors_used) {
        EVLOG_critical << "Either use no carside powermeters or use the same "
                          "number of powermeters as connectors in use";
        throw std::runtime_error("Powermeter count is greater than zero but "
                                 "mismatches number of connector in use");
    }

    // Initialize all connectors. After that the config was loaded and we can
    // initialize the dispenser
    for (int i = 0; i < number_of_connectors_used; i++) {
        invoke_init(*implementations[i]);
    }

    DispenserConfig dispenser_config = {
        .psu_host = config.psu_ip,
        .psu_port = (uint16_t)config.psu_port,
        .eth_interface = config.ethernet_interface,
        // fixed
        .manufacturer = 0x02,
        .model = 0x80,
        .charging_connector_count = number_of_connectors_used,
        // end fixed

        .esn = config.esn,
        .secure_goose = config.secure_goose,
        .module_placeholder_allocation_timeout = std::chrono::seconds(config.module_placeholder_allocation_timeout_s),
    };

    if (config.tls_enabled) {
        dispenser_config.tls_config = tls_util::MutualTlsClientConfig{
            .ca_cert = config.psu_ca_cert,
            .client_cert = config.client_cert,
            .client_key = config.client_key,
        };
    }

    logs::LogIntf log = logs::LogIntf{
        .error = logs::LogFun([](const std::string& message) { EVLOG_error << message; }),
        .warning = logs::LogFun([](const std::string& message) { EVLOG_warning << message; }),
        .info = logs::LogFun([](const std::string& message) { EVLOG_info << message; }),
        .verbose = logs::LogFun([](const std::string& message) { EVLOG_debug << message; }),
    };

    std::vector<ConnectorConfig> connector_configs;
    for (auto& connector : get_connector_bases(this, number_of_connectors_used)) {
        connector_configs.push_back(connector->get_connector_config());
    }

    dispenser = std::make_unique<Dispenser>(dispenser_config, connector_configs, log);
}

void Huawei_V100R023C10::ready() {
    this->dispenser->start();

    for (int i = 0; i < number_of_connectors_used; i++) {
        invoke_ready(*implementations[i]);
    }

    for (;;) {
        if (this->dispenser->get_psu_running_mode() == PSURunningMode::RUNNING && !initial_hmac_acquired) {
            acquire_initial_hmac_keys_for_all_connectors();
            initial_hmac_acquired = true;
        }

        update_psu_not_running_error();
        update_communication_errors();
        update_vendor_errors();
        restart_dispenser_if_needed();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Huawei_V100R023C10::acquire_initial_hmac_keys_for_all_connectors() {
    std::vector<std::thread> threads;
    for (int i = 0; i < number_of_connectors_used; i++) {
        threads.push_back(std::thread([this, i] { get_connector_impl(this, i)->do_init_hmac_acquire(); }));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void Huawei_V100R023C10::update_communication_errors() {
    auto connector_bases = get_connector_bases(this, number_of_connectors_used);

    if (this->dispenser->get_psu_communication_state() != DispenserPsuCommunicationState::READY) {
        if (!psu_not_running_raised) {
            for (auto& connector : connector_bases) {
                connector->raise_communication_fault();
            }
            psu_not_running_raised = true;
        }
    } else {
        if (psu_not_running_raised) {
            for (auto& connector : connector_bases) {
                connector->clear_communication_fault();
            }
            psu_not_running_raised = false;
        }
    }
}

void Huawei_V100R023C10::update_psu_not_running_error() {
    auto connector_bases = get_connector_bases(this, number_of_connectors_used);

    if (this->dispenser->get_psu_running_mode() != PSURunningMode::RUNNING) {
        if (!communication_fault_raised) {
            for (auto& connector : connector_bases) {
                connector->raise_psu_not_running();
            }
            communication_fault_raised = true;
        }
    } else {
        if (communication_fault_raised) {
            for (auto& connector : connector_bases) {
                connector->clear_psu_not_running();
            }
            communication_fault_raised = false;
        }
    }
}

void Huawei_V100R023C10::restart_dispenser_if_needed() {
    if (this->dispenser->get_psu_communication_state() == DispenserPsuCommunicationState::FAILED) {
        EVLOG_info << "restarting communication (stopping first)";
        this->dispenser->stop();
        EVLOG_info << "starting communications again";
        this->dispenser->start();
    }
}

void Huawei_V100R023C10::update_vendor_errors() {
    auto connector_bases = get_connector_bases(this, number_of_connectors_used);
    auto new_error_set = this->dispenser->get_raised_errors();

    ErrorEventSet new_raised_errors;
    std::set_difference(new_error_set.begin(), new_error_set.end(), raised_errors.begin(), raised_errors.end(),
                        std::inserter(new_raised_errors, new_raised_errors.begin()));

    for (auto raised_error : new_raised_errors) {
        for (auto& connector : connector_bases) {
            connector->raise_psu_error(raised_error);
        }
    }

    ErrorEventSet new_cleared_errors;
    std::set_difference(raised_errors.begin(), raised_errors.end(), new_error_set.begin(), new_error_set.end(),
                        std::inserter(new_cleared_errors, new_cleared_errors.begin()));

    for (auto cleared_error : new_cleared_errors) {
        for (auto& connector : connector_bases) {
            connector->clear_psu_error(cleared_error);
        }
    }

    ErrorEventSet errors_intersection;
    std::set_intersection(raised_errors.begin(), raised_errors.end(), new_error_set.begin(), new_error_set.end(),
                          std::inserter(errors_intersection, errors_intersection.begin()));

    ErrorEventSet changed_errors;
    for (auto error : errors_intersection) {
        auto old_error = raised_errors.find(error);
        auto new_error = new_error_set.find(error);

        if (old_error->payload.raw != new_error->payload.raw) {
            for (auto& connector : connector_bases) {
                connector->clear_psu_error(*old_error);
                connector->raise_psu_error(*new_error);
            }
        }
    }

    raised_errors = new_error_set;
}

} // namespace module
