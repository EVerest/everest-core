// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Huawei_V100R023C10.hpp"
#include "connector_1/power_supply_DCImpl.hpp"
#include "connector_2/power_supply_DCImpl.hpp"
#include "connector_3/power_supply_DCImpl.hpp"
#include "connector_4/power_supply_DCImpl.hpp"

namespace module {

static ConnectorBase* get_connector_impl(Huawei_V100R023C10* mod, std::uint8_t connector) {
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

static std::vector<ConnectorBase*> get_connector_bases(Huawei_V100R023C10* mod, std::uint8_t connectors_used) {
    std::vector<ConnectorBase*> connector_bases;
    for (std::uint8_t i = 0; i < connectors_used; i++) {
        connector_bases.push_back(get_connector_impl(mod, i));
    }
    return connector_bases;
}

static std::string get_everest_error_for_dispenser_alarm(DispenserAlarms alarm) {
    switch (alarm) {
    case DispenserAlarms::DOOR_STATUS_ALARM:
        return "evse_board_support/EnclosureOpen";
    case DispenserAlarms::WATER_ALARM:
        return "evse_board_support/WaterIngressDetected";
    case DispenserAlarms::EPO_ALARM:
        return "evse_board_support/MREC8EmergencyStop";
    case DispenserAlarms::TILT_ALARM:
        return "evse_board_support/TiltDetected";
    }

    throw std::runtime_error("Unknown DispenserAlarm enum value");
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

    if (config.upstream_voltage_source == "IMD") {
        upstream_voltage_source = Huawei_V100R023C10::UpstreamVoltageSource::IMD;
    } else if (config.upstream_voltage_source == "OVM") {
        upstream_voltage_source = Huawei_V100R023C10::UpstreamVoltageSource::OVM;
    } else {
        EVLOG_AND_THROW(std::runtime_error("Invalid upstream voltage source: " + config.upstream_voltage_source));
    }

    bool imds_necessary = upstream_voltage_source == UpstreamVoltageSource::IMD;
    bool ovms_necessary = upstream_voltage_source == UpstreamVoltageSource::OVM ||
                          config.HACK_use_ovm_while_cable_check; // note that if the hack is enabled we also need OVMs

    if (this->r_carside_powermeter.size() != 0 and this->r_carside_powermeter.size() != number_of_connectors_used) {
        EVLOG_AND_THROW(std::runtime_error(
            "Either use no carside powermeters or use the same number of powermeters as connectors in use"));
    }
    if (imds_necessary and this->r_isolation_monitor.size() != number_of_connectors_used) {
        EVLOG_AND_THROW(
            std::runtime_error("IMDs are necessary but number of IMDs does not match number of connectors in use"));
    }
    if (ovms_necessary and this->r_over_voltage_monitor.size() != number_of_connectors_used) {
        EVLOG_AND_THROW(
            std::runtime_error("OVMs are necessary but number of OVMs does not match number of connectors in use"));
    }

    if (config.telemetry_topic_prefix.empty()) {
        this->telemetry_publisher = std::make_shared<fusion_charger::telemetry::TelemetryPublisherNull>();
    } else {
        this->telemetry_publisher = std::make_shared<TelemetryPublisherEverest>(
            [this](const std::string& topic, const nlohmann::json& data) {
                try {
                    mqtt.publish(topic, data.dump());
                } catch (std::exception& e) {
                    EVLOG_error << "Failed publishing telemetry data to MQTT topic " << topic << ": " << e.what();
                }
            },
            config.telemetry_topic_prefix);
    }

    // Initialize all connectors. After that the config was loaded and we can initialize the dispenser
    for (int i = 0; i < number_of_connectors_used; i++) {
        invoke_init(*implementations[i]);
    }

    DispenserConfig dispenser_config;
    dispenser_config.psu_host = config.psu_ip;
    dispenser_config.psu_port = (std::uint16_t)config.psu_port;
    dispenser_config.eth_interface = config.ethernet_interface;
    // fixed
    dispenser_config.manufacturer = 0x02;
    dispenser_config.model = 0x80;
    dispenser_config.charging_connector_count = number_of_connectors_used;
    // end fixed

    dispenser_config.esn = config.esn;
    dispenser_config.send_secure_goose = config.send_secure_goose;
    dispenser_config.allow_unsecured_goose = config.allow_insecure_goose;
    dispenser_config.verify_secure_goose_hmac = config.verify_secure_goose;
    dispenser_config.module_placeholder_allocation_timeout =
        std::chrono::seconds(config.module_placeholder_allocation_timeout_s);

    dispenser_config.telemetry_publisher = this->telemetry_publisher;

    if (config.tls_enabled) {
        tls_util::MutualTlsClientConfig mutual_tls_config;
        mutual_tls_config.ca_cert = config.psu_ca_cert;
        mutual_tls_config.client_cert = config.client_cert;
        mutual_tls_config.client_key = config.client_key;
        dispenser_config.tls_config = mutual_tls_config;
    }

    logs::LogIntf log{logs::LogFun([](const std::string& message) { EVLOG_error << message; }),
                      logs::LogFun([](const std::string& message) { EVLOG_warning << message; }),
                      logs::LogFun([](const std::string& message) { EVLOG_info << message; }),
                      logs::LogFun([](const std::string& message) { EVLOG_debug << message; }),
                      logs::LogFun([](const std::string& message) { EVLOG_verbose << message; })};

    std::vector<ConnectorConfig> connector_configs;
    for (auto& connector : get_connector_bases(this, number_of_connectors_used)) {
        connector_configs.push_back(connector->get_connector_config());
    }

    dispenser = std::make_unique<Dispenser>(dispenser_config, connector_configs, log);

    // Subscribe to BSP Dispenser Alarms
    for (int bsp_idx = 0; bsp_idx < number_of_connectors_used; bsp_idx++) {
        dispenser_alarms_per_bsp.push_back(std::set<DispenserAlarms>{});

        for (auto& alarm : get_all_dispenser_alarms()) {
            std::string everest_error = get_everest_error_for_dispenser_alarm(alarm);

            r_board_support[bsp_idx]->subscribe_error(
                everest_error,
                [this, bsp_idx, alarm, everest_error](const ::Everest::error::Error& e) {
                    // Error raised
                    auto& alarms = dispenser_alarms_per_bsp[bsp_idx];
                    if (alarms.find(alarm) == alarms.end()) {
                        alarms.insert(alarm);

                        EVLOG_info << "Raising dispenser alarm due to BSP error " << everest_error;
                        dispenser->set_dispenser_alarm(alarm, true);
                    }
                },
                [this, bsp_idx, alarm, everest_error](const ::Everest::error::Error& e) {
                    // Error cleared
                    auto& alarms = dispenser_alarms_per_bsp[bsp_idx];
                    if (alarms.find(alarm) != alarms.end()) {
                        alarms.erase(alarm);

                        // check if any other BSP raised this alarm
                        bool alarm_still_raised = false;
                        for (const auto& other_alarms : dispenser_alarms_per_bsp) {
                            if (other_alarms.find(alarm) != other_alarms.end()) {
                                alarm_still_raised = true;
                                break;
                            }
                        }
                        if (not alarm_still_raised) {
                            EVLOG_info << "Clearing dispenser alarm as all BSPs cleared error " << everest_error;
                            dispenser->set_dispenser_alarm(alarm, false);
                        }
                    }
                });
        }
    }
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
        // Clear the stored capabilities in all connectors so that the missing cababilities error is raised
        // until we get new capabilities
        for (auto& connector : get_connector_bases(this, number_of_connectors_used)) {
            connector->clear_stored_capabilities();
        }
        EVLOG_info << "Dispenser: restarting communication (stopping first)";
        this->dispenser->stop();
        EVLOG_info << "Dispenser: starting communications again";
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
