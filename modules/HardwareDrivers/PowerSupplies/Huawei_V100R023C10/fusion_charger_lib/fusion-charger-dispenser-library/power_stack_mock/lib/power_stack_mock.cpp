// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "power_stack_mock/power_stack_mock.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <modbus-server/client.hpp>
#include <thread>
#include <vector>

#include "fusion_charger/goose/power_request.hpp"
#include "fusion_charger/modbus/registers/raw.hpp"
#include "goose/frame.hpp"
#include "modbus-ssl/openssl_transport.hpp"
#include "power_stack_mock/util.hpp"

using namespace user_acceptance_tests::test_utils;

using fusion_charger::modbus_driver::raw_registers::offset_from_connector_number;

bool DispenserInformation::operator==(const DispenserInformation& rhs) const {
    return manufacturer == rhs.manufacturer && model == rhs.model && protocol_version == rhs.protocol_version &&
           hardware_version == rhs.hardware_version && software_version == rhs.software_version;
}

bool DispenserInformation::operator!=(const DispenserInformation& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const DispenserInformation& info) {
    os << "DispenserInformation{" << std::endl;
    os << "  manufacturer: " << info.manufacturer << std::endl;
    os << "  model: " << info.model << std::endl;
    os << "  protocol_version: " << info.protocol_version << std::endl;
    os << "  hardware_version: " << info.hardware_version << std::endl;
    os << "  software_version: " << info.software_version << std::endl;
    os << "}";
    return os;
}

bool ConnectorCallbackResults::operator==(const ConnectorCallbackResults& rhs) const {
    return connector_upstream_voltage == rhs.connector_upstream_voltage && output_voltage == rhs.output_voltage &&
           output_current == rhs.output_current && contactor_status == rhs.contactor_status &&
           electronic_lock_status == rhs.electronic_lock_status;
}

bool ConnectorCallbackResults::operator!=(const ConnectorCallbackResults& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const ConnectorCallbackResults& results) {
    os << "ConnectorCallbackResults{" << std::endl;
    os << "connector_upstream_voltage: " << results.connector_upstream_voltage << std::endl;
    os << "output_voltage: " << results.output_voltage << std::endl;
    os << "output_current: " << results.output_current << std::endl;
    os << "contactor_status: " << (uint32_t)results.contactor_status << std::endl;
    os << "electronic_lock_status: " << (uint32_t)results.electronic_lock_status << std::endl;
    os << "}";

    return os;
}

PowerStackMock::PowerStackMock(int client_socket, std::optional<std::tuple<SSL*, SSL_CTX*>> openssl_data,
                               std::shared_ptr<modbus_server::ModbusTransport> transport, PowerStackMockConfig config) :
    client_sock(client_socket),
    openssl_data(openssl_data),
    transport(transport),
    protocol(std::make_shared<modbus_server::ModbusTCPProtocol>(transport)),
    pas(std::make_shared<modbus_server::PDUCorrelationLayer>(protocol)),
    client(pas),
    config(config),
    eth(goose_ethernet::EthernetInterface(config.eth.c_str())) {
    pas->set_on_pdu([this](const modbus_server::pdu::GenericPDU& pdu) {
        this->on_pdu(pdu);
        return std::nullopt;
    });

    goose_receiver_thread = std::thread([this] { goose_receiver_thread_run(); });
}

PowerStackMock::~PowerStackMock() {
    running = false;
    goose_receiver_thread.join();

    if (openssl_data) {
        tls_util::free_ssl(openssl_data.value());
    }

    close(client_sock);
}

PowerStackMock* PowerStackMock::from_config(PowerStackMockConfig config) {
    int client_socket = open_socket(config.port);

    if (config.tls_config.has_value()) {
        auto openssl_data = init_mutual_tls_server(client_socket, config.tls_config.value());
        SSL* ssl = std::get<0>(openssl_data);
        return new PowerStackMock(client_socket, openssl_data, std::make_shared<modbus_ssl::OpenSSLTransport>(ssl),
                                  config);
    }

    return new PowerStackMock(client_socket, std::nullopt,
                              std::make_shared<modbus_server::ModbusSocketTransport>(client_socket), config);
}

std::vector<uint16_t> PowerStackMock::read_and_check(uint16_t start_address, uint16_t quantity) {
    auto registers = client.read_holding_registers(start_address, quantity);
    if (registers.size() != quantity) {
        fail_printf("Holding register at 0x%X, reading %d registers returned vector "
                    "of length: %d",
                    start_address, quantity, registers.size());
    }

    return registers;
}

void PowerStackMock::goose_receiver_thread_run() {
    while (running) {
        auto p = eth.receive_packet();
        if (!p.has_value()) {
            continue;
        }

        auto eth_mac = eth.get_mac_address();
        if (memcmp(p.value().destination, eth_mac, 6) != 0) {
            continue;
        }

        if (p.value().ethertype != goose::frame::GOOSE_ETHERTYPE) {
            continue;
        }

        auto packet = p.value();
        // Settings tag, because it is lost during transmission
        packet.eth_802_1q_tag = 0x8100A000;
        goose::frame::SecureGooseFrame frame;
        try {
            frame = goose::frame::SecureGooseFrame(packet);
        } catch (std::runtime_error& e) {
            fail_printf("Could not parse goose frame as secure: %s", e.what());
            continue;
        }

        goose::frame::GoosePDU pdu = frame.pdu;

        if (strcmp(pdu.go_id, "CC/0$GO$PowerRequest") == 0) {
            fusion_charger::goose::PowerRequirementRequest new_request;
            new_request.from_pdu(pdu);
            auto global_connector_number = new_request.charging_connector_no;

            if (power_requirement_request_counter.find(global_connector_number) ==
                power_requirement_request_counter.end()) {
                power_requirement_request_counter[global_connector_number] = 0;
            }
            power_requirement_request_counter[global_connector_number] += 1;

            if (new_request.requirement_type == fusion_charger::goose::RequirementType::ModulePlaceholderRequest &&
                answer_module_placeholder_allocation) {
                printf("Received module placeholder request; sending answer\n");
                // send a reply
                fusion_charger::goose::PowerRequirementResponse response;
                response.charging_connector_no = new_request.charging_connector_no;
                response.charging_sn = new_request.charging_sn;
                response.requirement_type = new_request.requirement_type;
                response.mode = new_request.mode;
                response.voltage = new_request.voltage;
                response.current = new_request.current;
                response.result = fusion_charger::goose::PowerRequirementResponse::Result::SUCCESS;

                goose::frame::GoosePDU response_pdu = response.to_pdu();
                goose::frame::SecureGooseFrame response_frame;
                memcpy(response_frame.destination_mac_address, frame.source_mac_address, 6);
                memcpy(response_frame.source_mac_address, eth.get_mac_address(), 6);
                response_frame.vlan_id = 0;
                response_frame.priority = 5;
                response_frame.appid[0] = 0x30;
                response_frame.appid[1] = 0x01;
                response_frame.pdu = response_pdu;
                std::vector<uint8_t> hmac_key(config.hmac_key, config.hmac_key + 24);

                auto frame = response_frame.serialize(hmac_key);
                eth.send_packet(frame);
            }

            last_power_requirement_requests[global_connector_number] = new_request;
        }

        if (strcmp(pdu.go_id, "CC/0$GO$ShutdownRequest") == 0) {
            fusion_charger::goose::StopChargeRequest new_request;
            new_request.from_pdu(pdu);
            auto global_connector_number = new_request.charging_connector_no;

            if (stop_charge_request_counter.find(global_connector_number) == stop_charge_request_counter.end()) {
                stop_charge_request_counter[global_connector_number] = 0;
            }
            stop_charge_request_counter[global_connector_number] += 1;

            last_stop_charge_requests[global_connector_number] = new_request;
        }
    }

    psu_printf("Exiting Goose Receiver Thread");
}

void PowerStackMock::start_modbus_event_loop() {
    if (modbus_event_loop.has_value()) {
        fail_printf("Modbus event loop already started");
        return;
    }

    modbus_event_loop = std::thread([this]() {
        psu_printf("Started: Modbus event loop");
        try {
            while (running) {
                bool poll = pas->poll();
                if (!poll) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        } catch (const std::exception& e) {
            fail_printf("Exception in event loop: %s", e.what());
        }

        running = false;
    });
}

void PowerStackMock::stop_modbus_event_loop() {
    if (!modbus_event_loop.has_value()) {
        return;
    }

    running = false;

    modbus_event_loop->join();
}

void PowerStackMock::stop() {
    stop_modbus_event_loop();
    close(client_sock);
}

void PowerStackMock::on_pdu(const modbus_server::pdu::GenericPDU& pdu) {
    fusion_charger::modbus_extensions::UnsolicitatedReportRequest req;
    req.from_generic(pdu);

    auto segments = req.devices[0].segments;

    std::lock_guard<std::mutex> guard(pdu_registers_mutex);

    for (auto& segment : segments) {
        pdu_registers.insert_or_assign(segment.registers_start, segment);
    }
}

std::vector<uint16_t> PowerStackMock::get_unsolicited_report_data(uint16_t start_address, uint16_t quantity) {
    std::lock_guard<std::mutex> guard(pdu_registers_mutex);

    if (pdu_registers.find(start_address) == pdu_registers.end()) {
        return {};
    }

    auto segment = pdu_registers.at(start_address);

    std::vector<uint16_t> registers;

    if (segment.registers_count != quantity) {
        throw std::runtime_error("Expected " + std::to_string(quantity) + " registers, got " +
                                 std::to_string(segment.registers_count));
    }

    for (int i = 0; i < quantity; i++) {
        // convert to big endian
        registers.push_back(segment.registers[i * 2] << 8 | segment.registers[i * 2 + 1]);
    }

    return registers;
}

std::vector<uint16_t> PowerStackMock::read_registers(uint16_t start_address, uint16_t quantity) {
    return client.read_holding_registers(start_address, quantity);
}

void PowerStackMock::write_registers(uint16_t start_address, const std::vector<uint16_t>& values) {
    client.write_multiple_registers(start_address, values);
}

void PowerStackMock::set_psu_running_mode(PSURunningMode mode) {
    client.write_single_register(0x2006, static_cast<uint16_t>(mode));
}

void PowerStackMock::send_mac_address() {
    auto address = eth.get_mac_address();

    auto mac = std::vector<uint16_t>();
    for (int i = 0; i < 6; i += 2) {
        mac.push_back(address[i] << 8 | address[i + 1]);
    }

    client.write_multiple_registers(0x2111, mac);
}

void PowerStackMock::send_hmac_key(uint16_t local_connector_number) {
    client.write_multiple_registers(0x2115 + (uint16_t)offset_from_connector_number(local_connector_number),
                                    std::vector(config.hmac_key, config.hmac_key + 24));
}

void PowerStackMock::send_max_rated_current_of_output_port(float current, uint16_t local_connector_number) {
    client.write_multiple_registers(0x2102 + (uint16_t)offset_from_connector_number(local_connector_number),
                                    float_to_uint16_vec(current));
}

void PowerStackMock::send_max_rated_voltage_of_output_port(float voltage, uint16_t local_connector_number) {
    client.write_multiple_registers(0x2100 + (uint16_t)offset_from_connector_number(local_connector_number),
                                    float_to_uint16_vec(voltage));
}

void PowerStackMock::send_rated_power_of_output_port(float power, uint16_t local_connector_number) {
    client.write_multiple_registers(0x212D + (uint16_t)offset_from_connector_number(local_connector_number),
                                    float_to_uint16_vec(power));
}

int PowerStackMock::open_socket(uint16_t port) {
    psu_printf("Waiting for modbus connection\n");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    bool is_true = true;
    // makes the socket-address reusable
    // if not set, socket may take some time to be cleaned up
    // making the application fail for a short period of time
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &is_true, sizeof(int));
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    int err = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        throw std::runtime_error("Failed to bind");
    }

    err = listen(sock, 1);
    if (err < 0) {
        throw std::runtime_error("Failed to listen");
    }

    printf("Accepting new connection\n");
    int client_sock = accept(sock, nullptr, nullptr);
    if (client_sock < 0) {
        fail_printf("Failed to accept with error: %d", errno);
        close(sock);

        throw std::runtime_error("Failed to accept with error: " + std::to_string(errno));
    }
    // close the server socket, but keep the connection socket open
    close(sock);

    return client_sock;
}

std::optional<fusion_charger::goose::PowerRequirementRequest>
PowerStackMock::get_last_power_requirement_request(uint16_t global_connector_number) {
    auto it = last_power_requirement_requests.find(global_connector_number);
    if (it != last_power_requirement_requests.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

uint32_t PowerStackMock::get_power_requirements_counter(uint16_t global_connector_number) {
    auto it = power_requirement_request_counter.find(global_connector_number);
    if (it != power_requirement_request_counter.end()) {
        return it->second;
    } else {
        return 0;
    }
}

std::optional<fusion_charger::goose::StopChargeRequest>
PowerStackMock::get_last_stop_charge_request(uint16_t global_connector_number) {
    auto it = last_stop_charge_requests.find(global_connector_number);
    if (it != last_stop_charge_requests.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

uint32_t PowerStackMock::get_stop_charge_request_counter(uint16_t global_connector_number) {
    auto it = stop_charge_request_counter.find(global_connector_number);
    if (it != stop_charge_request_counter.end()) {
        return it->second;
    } else {
        return 0;
    }
}

fusion_charger::modbus_driver::raw_registers::ConnectionStatus
PowerStackMock::get_connection_status(uint16_t local_connector_number) {
    return static_cast<fusion_charger::modbus_driver::raw_registers::ConnectionStatus>(
        client.read_holding_registers(0x110D + (uint16_t)offset_from_connector_number(local_connector_number), 1)[0]);
}
float PowerStackMock::get_maximum_rated_charge_current(uint16_t local_connector_number) {
    auto read_result =
        client.read_holding_registers(0x1105 + (uint16_t)offset_from_connector_number(local_connector_number), 2);

    return registers_to_float(read_result);
}

DispenserInformation PowerStackMock::get_dispenser_information() {
    auto read_result = client.read_holding_registers(0x0000, 3);
    auto hardware_version = client.read_holding_registers(0x0004, 1)[0];
    auto software_version_registers = client.read_holding_registers(0x0013, 24);
    std::vector<uint8_t> software_version_bytes;
    for (auto reg : software_version_registers) {
        software_version_bytes.push_back(reg >> 8);
        software_version_bytes.push_back(reg & 0xFF);
    }

    std::string software_version(software_version_bytes.begin(),
                                 std::find(software_version_bytes.begin(), software_version_bytes.end(), '\0'));

    return DispenserInformation{
        .manufacturer = read_result[0],
        .model = read_result[1],
        .protocol_version = read_result[2],
        .hardware_version = hardware_version,
        .software_version = software_version,
    };
}

std::string PowerStackMock::get_dispenser_esn() {
    auto registers = client.read_holding_registers(0x1016, 11);
    std::vector<uint8_t> bytes;
    for (auto reg : registers) {
        bytes.push_back(reg >> 8);
        bytes.push_back(reg & 0xFF);
    }

    std::string esn(bytes.begin(), std::find(bytes.begin(), bytes.end(), '\0'));

    return esn;
}

uint32_t PowerStackMock::get_utc_time() {
    auto registers = client.read_holding_registers(0x1024, 2);

    return (registers[0] << 16) | registers[1];
}

ConnectorCallbackResults PowerStackMock::get_connector_callback_values(uint16_t local_connector_number) {
    auto output_voltage = registers_to_float(
        client.read_holding_registers(0x1107 + (uint16_t)offset_from_connector_number(local_connector_number), 2));

    auto output_current = registers_to_float(
        client.read_holding_registers(0x1109 + (uint16_t)offset_from_connector_number(local_connector_number), 2));

    auto contactors_upstream_voltage = registers_to_float(
        client.read_holding_registers(0x1113 + (uint16_t)offset_from_connector_number(local_connector_number), 2));

    auto contactors_status = (ContactorStatus)client.read_holding_registers(
        0x1154 + (uint16_t)offset_from_connector_number(local_connector_number), 1)[0];

    auto electronic_lock_status = (ElectronicLockStatus)client.read_holding_registers(
        0x1156 + (uint16_t)offset_from_connector_number(local_connector_number), 1)[0];

    return ConnectorCallbackResults{
        .connector_upstream_voltage = contactors_upstream_voltage,
        .output_voltage = output_voltage,
        .output_current = output_current,
        .contactor_status = contactors_status,
        .electronic_lock_status = electronic_lock_status,
    };
}

float PowerStackMock::registers_to_float(std::vector<uint16_t> registers) {
    uint16_t high_byte = registers[0];
    uint16_t low_byte = registers[1];
    uint32_t combined_bytes = (static_cast<uint32_t>(high_byte) << 16) | low_byte;
    float result = *reinterpret_cast<float*>(&combined_bytes);
    return result;
}

int PowerStackMock::client_socket() {
    return client_sock;
}

void PowerStackMock::set_enable_answer_module_placeholder_allocation(bool enable) {
    answer_module_placeholder_allocation = enable;
}
