// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <sys/socket.h>

#include <cstdint>
#include <fusion_charger/goose/power_request.hpp>
#include <goose-ethernet/driver.hpp>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

#include "dispenser.hpp"
#include "fusion_charger/goose/stop_charge_request.hpp"
#include "fusion_charger/modbus/extensions/unsolicitated_report.hpp"
#include "fusion_charger/modbus/registers/raw.hpp"
#include "modbus-server/client.hpp"
#include "modbus-server/frames.hpp"
#include "modbus-server/pdu_correlation.hpp"
#include "modbus-server/transport.hpp"
#include "modbus-server/transport_protocol.hpp"
#include "tls_util.hpp"

typedef fusion_charger::modbus_driver::raw_registers::SettingPowerUnitRegisters::PSURunningMode PSURunningMode;

struct DispenserInformation {
    std::uint16_t manufacturer;
    std::uint16_t model;
    std::uint16_t protocol_version;
    std::uint16_t hardware_version;
    std::string software_version;

    bool operator==(const DispenserInformation& rhs) const;
    bool operator!=(const DispenserInformation& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const DispenserInformation& info);
};

struct ConnectorCallbackResults {
    float connector_upstream_voltage;
    float output_voltage;
    float output_current;
    ContactorStatus contactor_status;
    ElectronicLockStatus electronic_lock_status;

    bool operator==(const ConnectorCallbackResults& rhs) const;
    bool operator!=(const ConnectorCallbackResults& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const ConnectorCallbackResults& results);
};

struct PowerStackMockConfig {
    std::string eth;
    std::uint16_t port;
    std::uint8_t hmac_key[48];
    bool enable_hmac = true; // if true sign goose frames with hmac key
    bool verify_hmac = true; // if true verify received goose frames with hmac key

    std::optional<tls_util::MutualTlsServerConfig> tls_config;

    std::function<void(const fusion_charger::goose::PowerRequirementRequest&)> power_requirement_request_callback;
    std::function<void(const fusion_charger::goose::StopChargeRequest&)> stop_charge_request_callback;
};

class PowerStackMock {
public:
    static PowerStackMock* from_config(PowerStackMockConfig config);
    static PowerStackMock* from_config(PowerStackMockConfig config, int socket);
    ~PowerStackMock();

    void goose_receiver_thread_run();
    void stop();
    int client_socket();

    void start_modbus_event_loop();
    void stop_modbus_event_loop();

    std::vector<std::uint16_t> get_unsolicited_report_data(std::uint16_t start_address, std::uint16_t quantity);
    std::vector<std::uint16_t> read_registers(std::uint16_t start_address, std::uint16_t quantity);
    void write_registers(std::uint16_t start_address, const std::vector<std::uint16_t>& values);

    void set_psu_running_mode(PSURunningMode mode);
    void send_mac_address();
    void send_hmac_key(std::uint16_t local_connector_number);
    void send_max_rated_current_of_output_port(float current, std::uint16_t local_connector_number);
    void send_min_rated_current_of_output_port(float current, std::uint16_t local_connector_number);
    void send_max_rated_voltage_of_output_port(float voltage, std::uint16_t local_connector_number);
    void send_min_rated_voltage_of_output_port(float voltage, std::uint16_t local_connector_number);
    void send_rated_power_of_output_port(float power, std::uint16_t local_connector_number);
    void send_total_historical_ac_input_energy(double energy);
    void send_ac_input_voltages_currents(float voltage_a, float voltage_b, float voltage_c, float current_a,
                                         float current_b, float current_c);
    void send_port_available(bool available, std::uint16_t local_connector_number);

    std::optional<fusion_charger::goose::PowerRequirementRequest>
    get_last_power_requirement_request(std::uint16_t global_connector_number);
    std::uint32_t get_power_requirements_counter(std::uint16_t global_connector_number);
    std::optional<fusion_charger::goose::StopChargeRequest>
    get_last_stop_charge_request(std::uint16_t global_connector_number);
    std::uint32_t get_stop_charge_request_counter(std::uint16_t global_connector_number);
    fusion_charger::modbus_driver::raw_registers::ConnectionStatus
    get_connection_status(std::uint16_t local_connector_number);
    float get_maximum_rated_charge_current(std::uint16_t local_connector_number);
    std::optional<fusion_charger::modbus_driver::raw_registers::WorkingStatus>
    get_working_status(std::uint16_t local_connector_number);
    DispenserInformation get_dispenser_information();
    std::string get_dispenser_esn();
    std::uint32_t get_utc_time();

    ConnectorCallbackResults get_connector_callback_values(std::uint16_t local_connector_number);

    void set_enable_answer_module_placeholder_allocation(bool enable);

    /**
     * @brief get the global connector number from the local connector number (range 1-4)
     * @returns the global connector number or std::nullopt if not found / error reading
     */
    std::optional<int> get_global_connector_number_from_local(std::uint16_t local_connector_number);

private:
    PowerStackMockConfig config;
    goose_ethernet::EthernetInterface eth;

    std::unordered_map<std::uint16_t, fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Segment>
        pdu_registers;
    std::mutex pdu_registers_mutex;

    // Keep the order of the elements, as this determines the order of the
    // initialization

    int client_sock;
    std::optional<std::tuple<SSL*, SSL_CTX*>> openssl_data;
    std::shared_ptr<modbus_server::ModbusTransport> transport;
    std::shared_ptr<modbus_server::ModbusTCPProtocol> protocol;
    std::shared_ptr<modbus_server::PDUCorrelationLayer> pas;
    modbus_server::client::ModbusClient client;

    std::optional<std::thread> modbus_event_loop;
    std::vector<std::uint16_t> read_and_check(std::uint16_t start_address, std::uint16_t quantity);

    std::atomic<bool> running = true;
    std::atomic<bool> answer_module_placeholder_allocation = true;
    std::thread goose_receiver_thread;

    std::map<std::uint16_t, std::atomic<fusion_charger::goose::PowerRequirementRequest>>
        last_power_requirement_requests;
    std::map<std::uint16_t, std::atomic<std::uint32_t>> power_requirement_request_counter = {};
    std::map<std::uint16_t, std::atomic<fusion_charger::goose::StopChargeRequest>> last_stop_charge_requests;
    std::map<std::uint16_t, std::atomic<std::uint32_t>> stop_charge_request_counter = {};

    void on_pdu(const modbus_server::pdu::GenericPDU& pdu);

    static int open_socket(std::uint16_t port);
    float registers_to_float(std::vector<std::uint16_t> registers);

    PowerStackMock(int client_socket, std::optional<std::tuple<SSL*, SSL_CTX*>> openssl_data,
                   std::shared_ptr<modbus_server::ModbusTransport> transport, PowerStackMockConfig config);
};
