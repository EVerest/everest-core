// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <arpa/inet.h>
#include <stdint.h>
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
    uint16_t manufacturer;
    uint16_t model;
    uint16_t protocol_version;
    uint16_t hardware_version;
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
    uint16_t port;
    uint16_t hmac_key[24]; // todo: MAKE THIS uint8!!!!!!!!!

    std::optional<tls_util::MutualTlsServerConfig> tls_config;
};

class PowerStackMock {
public:
    static PowerStackMock* from_config(PowerStackMockConfig config);
    ~PowerStackMock();

    void goose_receiver_thread_run();
    void stop();
    int client_socket();

    void start_modbus_event_loop();
    void stop_modbus_event_loop();

    std::vector<uint16_t> get_unsolicited_report_data(uint16_t start_address, uint16_t quantity);
    std::vector<uint16_t> read_registers(uint16_t start_address, uint16_t quantity);
    void write_registers(uint16_t start_address, const std::vector<uint16_t>& values);

    void set_psu_running_mode(PSURunningMode mode);
    void send_mac_address();
    void send_hmac_key(uint16_t local_connector_number);
    void send_max_rated_current_of_output_port(float current, uint16_t local_connector_number);
    void send_max_rated_voltage_of_output_port(float voltage, uint16_t local_connector_number);
    void send_rated_power_of_output_port(float power, uint16_t local_connector_number);

    std::optional<fusion_charger::goose::PowerRequirementRequest>
    get_last_power_requirement_request(uint16_t global_connector_number);
    uint32_t get_power_requirements_counter(uint16_t global_connector_number);
    std::optional<fusion_charger::goose::StopChargeRequest>
    get_last_stop_charge_request(uint16_t global_connector_number);
    uint32_t get_stop_charge_request_counter(uint16_t global_connector_number);
    fusion_charger::modbus_driver::raw_registers::ConnectionStatus
    get_connection_status(uint16_t local_connector_number);
    float get_maximum_rated_charge_current(uint16_t local_connector_number);
    DispenserInformation get_dispenser_information();
    std::string get_dispenser_esn();
    uint32_t get_utc_time();

    ConnectorCallbackResults get_connector_callback_values(uint16_t local_connector_number);

    void set_enable_answer_module_placeholder_allocation(bool enable);

private:
    PowerStackMockConfig config;
    goose_ethernet::EthernetInterface eth;

    std::unordered_map<uint16_t, fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Segment> pdu_registers;
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
    std::vector<uint16_t> read_and_check(uint16_t start_address, uint16_t quantity);

    std::atomic<bool> running = true;
    std::atomic<bool> answer_module_placeholder_allocation = true;
    std::thread goose_receiver_thread;

    std::map<uint16_t, std::atomic<fusion_charger::goose::PowerRequirementRequest>> last_power_requirement_requests;
    std::map<uint16_t, std::atomic<uint32_t>> power_requirement_request_counter = {};
    std::map<uint16_t, std::atomic<fusion_charger::goose::StopChargeRequest>> last_stop_charge_requests;
    std::map<uint16_t, std::atomic<uint32_t>> stop_charge_request_counter = {};

    void on_pdu(const modbus_server::pdu::GenericPDU& pdu);

    static int open_socket(uint16_t port);
    float registers_to_float(std::vector<uint16_t> registers);

    PowerStackMock(int client_socket, std::optional<std::tuple<SSL*, SSL_CTX*>> openssl_data,
                   std::shared_ptr<modbus_server::ModbusTransport> transport, PowerStackMockConfig config);
};
