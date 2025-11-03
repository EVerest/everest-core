// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fusion_charger/goose/power_request.hpp>
#include <fusion_charger/modbus/extensions/unsolicitated_registry.hpp>
#include <fusion_charger/modbus/extensions/unsolicitated_report_server.hpp>
#include <fusion_charger/modbus/registers/connector.hpp>
#include <fusion_charger/modbus/registers/dispenser.hpp>
#include <fusion_charger/modbus/registers/power_unit.hpp>
#include <fusion_charger/modbus/registers/raw.hpp>
#include <goose-ethernet/driver.hpp>
#include <modbus-server/modbus_basic_server.hpp>
#include <thread>

using namespace fusion_charger::modbus_driver::raw_registers;
using namespace fusion_charger::modbus_driver;
using namespace fusion_charger::modbus_extensions;

#define DEFAULT_IP        "192.168.11.1"
#define DEFAULT_PORT      502
#define DEFAULT_INTERFACE "eth0"

const int do_connect(const char* ip, uint16_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Could not open ");
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    printf("Connecting to %s:%d\n", ip, ntohs(addr.sin_port));
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Could not ");
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");

    return sock;
}

int main(int argc, char* argv[]) {
    const char* ip = DEFAULT_IP;
    uint16_t port = DEFAULT_PORT;
    const char* intf = DEFAULT_INTERFACE;

    if (argc != 4) {
        printf("Assuming default IP: %s and port %u with intf %s\n", DEFAULT_IP, DEFAULT_PORT, DEFAULT_INTERFACE);
        printf("To use another ip and intf use: %s <ip> <port> <intf>\n", argv[0]);
    } else {
        ip = argv[1];
        port = strtol(argv[2], nullptr, 10);
        intf = argv[3];
    }

    printf("Using IP, port and interface: %s:%u@%s\n", ip, port, intf);

    goose_ethernet::EthernetInterface eth(intf);
    int sock = do_connect(ip, port);

    auto transport = std::make_shared<modbus_server::ModbusSocketTransport>(sock);
    auto protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport);
    auto pcl = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);
    UnsolicitatedReportBasicServer server(pcl);

    PowerUnitRegisters psu_registers;
    DispenserRegisters dispenser_registers(DispenserRegistersConfig{
        .esn = "1234567890",
        .connector_count = 1,
    });
    ConnectorRegistersConfig connector_register_config = ConnectorRegistersConfig{
        .mac_address = {*eth.get_mac_address()}, // todo: fix!!
        .type = ConnectorType::CCS1,
        .global_connector_no = 1,
        .connector_number = 1,
        .max_rated_charge_current = 100.0,
        .rated_output_power_connector = 10000.0,
        .get_contactor_upstream_voltage = []() { return 0.0; },
        .get_output_voltage = []() { return 0.0; },
        .get_output_current = []() { return 0.0; },
    };
    ConnectorRegisters connector_registers(connector_register_config);
    // Callbacks for common power unit registers
    psu_registers.manufacturer.add_write_callback(
        [](uint16_t value) { printf("PSU Manufacturer changed to %d\n", value); });
    psu_registers.protocol_version.add_write_callback(
        [](uint16_t value) { printf("PSU Protocol version changed to %d\n", value); });
    psu_registers.esn_control_board.add_write_callback(
        [](const std::string& value) { printf("PSU ESN Control Board changed to %s\n", value.c_str()); });
    psu_registers.software_version.add_write_callback(
        [](const std::string& value) { printf("PSU Software version changed to %s\n", value.c_str()); });
    psu_registers.hardware_version.add_write_callback(
        [](uint16_t val) { printf("PSU HW version changed to %d\n", val); });

    psu_registers.psu_running_mode.add_write_callback([](SettingPowerUnitRegisters::PSURunningMode value) {
        printf("PSU Running mode changed to %s\n",
               SettingPowerUnitRegisters::psu_running_mode_to_string(value).c_str());
    });

    connector_registers.hmac_key.add_write_callback([](const uint8_t* value) {
        printf("🎉🎉 HMAC key changed\n");
        printf("🎉🎉 New key: ");
        for (int i = 0; i < 48; i++) {
            printf("%02x", value[i]);
        }
        printf("\n");
    });

    connector_registers.psu_port_available.add_write_callback(
        [](PsuOutputPortAvailability value) { printf("PSU port available changed to %d\n", (uint16_t)value); });

    connector_registers.rated_output_power_psu.add_write_callback(
        [](float value) { printf("Rated output power PSU changed to %f\n", value); });

    connector_registers.rated_output_power_connector.add_write_callback(
        [](float value) { printf("Rated output power connector changed to %f\n", value); });

    UnsolicitatedRegistry register_registry;

    dispenser_registers.add_to_registry(register_registry);
    psu_registers.add_to_registry(register_registry);
    connector_registers.add_to_registry(register_registry);

    register_registry.verify_overlap();

    // forward read and write
    server.set_read_holding_registers_request_cb(
        [&register_registry](const modbus_server::pdu::ReadHoldingRegistersRequest& req) {
            auto data = register_registry.on_read(req.register_start, req.register_count);
            return modbus_server::pdu::ReadHoldingRegistersResponse(req, data);
        });
    server.set_write_multiple_registers_request_cb(
        [&register_registry](const modbus_server::pdu::WriteMultipleRegistersRequest& req) {
            register_registry.on_write(req.register_start, req.register_data);
            return modbus_server::pdu::WriteMultipleRegistersResponse(req);
        });
    server.set_write_single_register_request_cb(
        [&register_registry](const modbus_server::pdu::WriteSingleRegisterRequest& req) {
            register_registry.on_write(req.register_address,
                                       {(uint8_t)(req.register_value >> 8), (uint8_t)(req.register_value & 0xff)});
            return modbus_server::pdu::WriteSingleRegisterResponse(req);
        });

    printf("Serving\n");

    bool closed = false;

    auto unsolicitated_reporter = std::thread([&server, &register_registry, &closed]() {
        printf("Unsolicitated reporter thread started\n");
        while (true) {
            try {
                std::this_thread::sleep_for(std::chrono::seconds(1));

                if (closed) {
                    printf("Unsolicitated reporter thread exiting\n");
                    return;
                }

                auto req = register_registry.unsolicitated_report();
                if (req.has_value()) {
                    server.send_unsolicitated_report(req.value(), std::chrono::seconds(3));
                }
            } catch (modbus_server::transport_exceptions::ConnectionClosedException& e) {
                printf("Unsolicitated reporter noticed an closed connection; "
                       "exiting...\n");
                closed = true;
                return;
            } catch (std::runtime_error& e) {
                printf("Unsolicitated reporter thread error: %s\n", e.what());
            }
        }
    });

    auto goose_thread = std::thread([&eth, &closed, &psu_registers, &connector_registers]() {
        uint16_t stNum = 1;
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (closed) {
                printf("Goose thread exiting\n");
                return;
            }

            auto mac = psu_registers.psu_mac.get_value();
            if (mac[0] == 0 && mac[1] == 1 && mac[2] == 0 && mac[3] == 0) {
                // first 4 bytes are 0 -> mac not set
                continue;
            }

            auto hmac = connector_registers.hmac_key.get_value();

            if (hmac[0] == 0 && hmac[1] == 0 && hmac[2] == 0 && hmac[3] == 0) {
                // first 4 bytes are 0 -> hmac not set
                continue;
            }

            fusion_charger::goose::PowerRequirementRequest report_pdu;
            report_pdu.charging_connector_no = 1;
            report_pdu.charging_sn = 0xffff;
            report_pdu.requirement_type = fusion_charger::goose::RequirementType::Charging;
            report_pdu.mode = fusion_charger::goose::Mode::ConstantCurrent;
            report_pdu.voltage = 400;
            report_pdu.current = 10;

            goose::frame::SecureGooseFrame frame;
            memcpy(frame.destination_mac_address, mac, 6);
            memcpy(frame.source_mac_address, eth.get_mac_address(), 6);
            frame.vlan_id = 0;
            frame.priority = 5;
            frame.appid[0] = 0;
            frame.appid[1] = 1;
            frame.pdu = report_pdu.to_pdu();
            frame.pdu.st_num = stNum++;

            eth.send_packet(frame.serialize(std::vector<uint8_t>(hmac, hmac + 48)));
            printf("🚀 Sent goose frame\n");
        }
    });

    auto poll_thread = std::thread([&pcl, &closed]() {
        try {
            while (true) {
                pcl->blocking_poll();
                if (closed) {
                    printf("Poll thread exiting\n");
                    return;
                }
            }
        } catch (modbus_server::transport_exceptions::ConnectionClosedException& e) {
            printf("Poll thread noticed an closed connection; "
                   "exiting...\n");
            closed = true;
        }
    });

    auto dummy_data_changer_thread = std::thread([&connector_registers]() {
        std::this_thread::sleep_for(std::chrono::seconds(20));

        connector_registers.connection_status.update_value(ConnectionStatus::FULL_CONNECTED);
        printf("👨‍💻 Changed connection status to full connected\n");

        connector_registers.working_status.update_value(WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED);
        printf("👨‍💻 Update working status to standby with inserted "
               "charger\n");
    });

    dummy_data_changer_thread.join();
    poll_thread.join();
    unsolicitated_reporter.join();
    goose_thread.join();

    printf("Exiting\n");
    close(sock);
}
