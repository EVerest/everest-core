// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <atomic>
#include <fusion_charger/goose/power_request.hpp>
#include <fusion_charger/goose/stop_charge_request.hpp>
#include <fusion_charger/modbus/extensions/unsolicitated_registry.hpp>
#include <fusion_charger/modbus/extensions/unsolicitated_report_server.hpp>
#include <fusion_charger/modbus/registers/connector.hpp>
#include <fusion_charger/modbus/registers/dispenser.hpp>
#include <fusion_charger/modbus/registers/error.hpp>
#include <fusion_charger/modbus/registers/power_unit.hpp>
#include <fusion_charger/modbus/registers/raw.hpp>
#include <goose-ethernet/driver.hpp>
#include <logs/logs.hpp>
#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-ssl/openssl_transport.hpp>
#include <set>
#include <thread>

#include "configuration.hpp"
#include "connector.hpp"
#include "connector_goose_sender.hpp"
#include "state.hpp"

using namespace fusion_charger::modbus_driver::raw_registers;
using namespace fusion_charger::modbus_driver;
using namespace fusion_charger::modbus_extensions;

typedef fusion_charger::modbus_driver::raw_registers::CollectedConnectorRegisters::ContactorStatus ContactorStatus;

typedef fusion_charger::modbus_driver::raw_registers::WorkingStatus WorkingStatus;
typedef fusion_charger::modbus_driver::raw_registers::ConnectionStatus ConnectionStatus;

typedef fusion_charger::goose::RequirementType PowerRequirementType;

typedef fusion_charger::goose::Mode PowerRequirementsMode;

typedef fusion_charger::goose::StopChargeRequest::Reason StopChargeReason;

typedef fusion_charger::modbus_driver::raw_registers::SettingPowerUnitRegisters::PSURunningMode PSURunningMode;

// add a custom comparator to the set which ignores the payload value
// This is necessary to avoid having several error events with the same
// category and subcategory
typedef std::set<ErrorEvent, ErrorEventComparator> ErrorEventSet;

class Dispenser {
private:
    std::vector<std::shared_ptr<Connector>> connectors;
    logs::LogIntf log;

    std::atomic<DispenserPsuCommunicationState> psu_communication_state = DispenserPsuCommunicationState::UNINITIALIZED;

    DispenserConfig dispenser_config;
    std::vector<ConnectorConfig> connector_configs;

    goose_ethernet::EthernetInterface eth_interface;

    std::atomic<StopChargeReason> stop_charge_reason = StopChargeReason::INSULATION_FAULT;

    std::optional<int> modbus_socket;
    std::optional<std::thread> modbus_event_loop_thread;
    std::optional<std::thread> modbus_unsolicitated_event_thread;
    std::optional<std::thread> goose_receiver_thread;

    std::shared_ptr<modbus_server::ModbusTransport> transport;

    std::optional<std::tuple<SSL*, SSL_CTX*>> openssl_data;
    std::shared_ptr<modbus_server::ModbusTCPProtocol> protocol;
    std::shared_ptr<modbus_server::PDUCorrelationLayer> pcl;
    std::optional<UnsolicitatedReportBasicServer> server;
    std::optional<UnsolicitatedRegistry> registry;

    std::optional<DispenserRegisters> dispenser_registers;
    std::optional<PowerUnitRegisters> psu_registers;
    std::optional<ErrorRegisters> error_registers;

    ErrorEventSet raised_errors = {};
    std::mutex raised_error_mutex;

    std::optional<SettingPowerUnitRegisters::PSURunningMode> psu_running_mode = std::nullopt;

    const int MAX_NUMBER_OF_CONNECTORS = 4;

    // true if the psu wrote its mac address via modbus
    bool psu_mac_received = false;
    // true if the psu wrote the connectors hmac key via modbus
    bool connector_hmac_received = false;

    void init();

    void update_psu_communication_state();

    const int do_connect(const char* ip, std::uint16_t port);
    const int connect_with_retry(const char* ip, std::uint16_t port, int retries);
    void modbus_event_loop_thread_run();
    void modbus_unsolicitated_event_thread_run();
    void goose_receiver_thread_run();
    bool psu_communication_is_ok();
    bool is_stop_requested();

public:
    Dispenser(DispenserConfig dispenser_config, std::vector<ConnectorConfig> connector_configs,
              logs::LogIntf log = logs::log_printf);
    ~Dispenser();

    /// @brief start threads that will run dispenser
    void start();

    /// @brief stop running threads that are running dispenser. May take some
    /// time.
    void stop();

    PSURunningMode get_psu_running_mode();
    // PsuOutputPortAvailability get_output_port_availability();
    DispenserPsuCommunicationState get_psu_communication_state();

    /// @brief Retrieves the new error events. Clears the internal set of error
    /// events since the last call to this function
    /// @return The ErrorEvents that occured since the last call to this function
    ErrorEventSet get_raised_errors();

    /// @brief Get the connector with the given local connector number.
    /// @param local_connector_number 1-4
    std::shared_ptr<Connector> get_connector(int local_connector_number) {
        if (local_connector_number == 0) {
            throw std::runtime_error("Connector number must be greater than 0");
        }
        if (local_connector_number > connectors.size()) {
            throw std::runtime_error("Connector number too high. Max local connector number: " +
                                     std::to_string(connectors.size()));
        }

        // Connector numbers start at 1
        return connectors[local_connector_number - 1];
    }
};
