// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "dispenser.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>

using namespace fusion_charger::modbus_driver::raw_registers;
using namespace fusion_charger::modbus_driver;
using namespace fusion_charger::modbus_extensions;

void Dispenser::modbus_unsolicitated_event_thread_run() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (psu_communication_is_ok()) {
        try {
            auto req = registry->unsolicitated_report();
            if (req.has_value()) {
                server->send_unsolicitated_report(req.value(), std::chrono::seconds(3));
            }
        } catch (modbus_server::transport_exceptions::ConnectionClosedException& e) {
            log.error << "Unsolicitated reporter noticed an closed connection; exiting...";
            break;
        } catch (std::runtime_error& e) {
            log.error << "Unsolicitated reporter thread error: " + std::string(e.what());
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (!is_stop_requested()) {
        psu_communication_state = DispenserPsuCommunicationState::FAILED;
    }

    log.debug << "Unsolicitated reporter thread exiting";
}

void Dispenser::goose_receiver_thread_run() {
    while (psu_communication_is_ok()) {
        auto p = eth_interface.receive_packet();
        if (!p.has_value()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto packet = p.value();

        // we are only interested in GOOSE packets and there a other packets
        if (packet.ethertype != goose::frame::GOOSE_ETHERTYPE) {
            continue;
        }

        // filter src, if the source mac is our own mac, we ignore the packet
        if (std::memcmp(packet.source, eth_interface.get_mac_address(), 6) == 0) {
            continue;
        }

        // Settings tag, because it is lost during transmission
        packet.eth_802_1q_tag = 0x8100A000;

        goose::frame::GoosePDU pdu;

        bool decoded = false;

        try {
            goose::frame::SecureGooseFrame frame(packet); // note: hmac is verified below (only if configured)
            pdu = frame.pdu;
            decoded = true;
            log.verbose << "Received secure goose frame";
        } catch (std::runtime_error& e) {
            log.verbose << "Could not parse goose frame as secure: " + std::string(e.what());
        }

        if (this->dispenser_config.allow_unsecured_goose && !decoded) {
            try {
                goose::frame::GooseFrame frame(packet);
                pdu = frame.pdu;
                decoded = true;
                log.verbose << "Received non-secure goose frame";
            } catch (std::runtime_error& e) {
                log.verbose << "Could not parse goose frame as non-secure: " + std::string(e.what());
            }
        }

        if (!decoded) {
            log.warning << "Received frame could not be decoded";
            continue;
        }

        if (strcmp(pdu.go_id, "CC/0$GO$PowerRequestReply") != 0) {
            log.info << "Received goose frame with weird go_id: " + std::string(pdu.go_id);
            continue;
        }

        fusion_charger::goose::PowerRequirementResponse response;
        response.from_pdu(pdu);

        bool corresponding_connector_found = false;
        for (auto& c : connectors) {
            if (c->connector_config.global_connector_number == response.charging_connector_no) {
                corresponding_connector_found = true;

                // verify hmac if configured
                if (dispenser_config.verify_secure_goose_hmac) {
                    try {
                        goose::frame::SecureGooseFrame secure_frame(packet, c->get_hmac_key());
                        log.verbose << "HMAC verified for secure goose frame";
                    } catch (std::exception& e) {
                        log.error << "Received secure goose frame, but HMAC verification "
                                     "failed: " +
                                         std::string(e.what());
                        continue;
                    }
                }

                c->on_module_placeholder_allocation_response(
                    response.result == fusion_charger::goose::PowerRequirementResponse::Result::SUCCESS);
            }
        }

        if (!corresponding_connector_found) {
            log.verbose << "Received module replacement goose frame but charging "
                           "connector no is wrong!";
        }
    }

    log.debug << "Goose receiver thread exiting";
}

void Dispenser::modbus_event_loop_thread_run() {
    auto timeout = std::chrono::steady_clock::now() + dispenser_config.modbus_timeout_ms;

    try {
        while (psu_communication_is_ok()) {
            bool data_received = pcl->poll();
            if (data_received) {
                timeout = std::chrono::steady_clock::now() + dispenser_config.modbus_timeout_ms;
            } else {
                if (timeout < std::chrono::steady_clock::now()) {
                    throw std::runtime_error("No Modbus data received for " +
                                             std::to_string(dispenser_config.modbus_timeout_ms.count()) + " ms");
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    } catch (modbus_server::transport_exceptions::ConnectionClosedException& e) {
        log.error << "Poll thread noticed an closed connection; exiting... Error: " + std::string(e.what());
    } catch (modbus_ssl::OpenSSLTransportException& e) {
        log.error << "Poll thread noticed an OpenSSL error; exiting... Error: " + std::string(e.what());
    } catch (std::runtime_error& e) {
        log.error << "Poll thread error: " + std::string(e.what());
    } catch (...) {
        log.error << "Poll thread error: unknown";
    }

    if (!is_stop_requested()) {
        psu_communication_state = DispenserPsuCommunicationState::FAILED;
    }

    log.debug << "Poll thread exiting";
}

Dispenser::Dispenser(DispenserConfig dispenser_config, std::vector<ConnectorConfig> connector_configs,
                     logs::LogIntf log) :
    log(log),
    dispenser_config(dispenser_config),
    connector_configs(connector_configs),
    eth_interface(dispenser_config.eth_interface.c_str()) {
    if (connector_configs.size() > MAX_NUMBER_OF_CONNECTORS) {
        throw std::runtime_error("Too many connectors: " + std::to_string(connector_configs.size()) +
                                 "Max: " + std::to_string(MAX_NUMBER_OF_CONNECTORS));
    }

    // number connectors from 1 to n
    for (size_t local_connector_number = 1; local_connector_number <= connector_configs.size();
         local_connector_number++) {
        std::shared_ptr<Connector> connector = std::make_shared<Connector>(
            connector_configs[local_connector_number - 1], local_connector_number, dispenser_config, log);
        connectors.push_back(connector);
    }
}

void Dispenser::start() {
    if (modbus_socket.has_value()) {
        stop();
    }

    psu_communication_state = DispenserPsuCommunicationState::INITIALIZING;

    modbus_event_loop_thread = std::thread([this]() {
        try {
            init();
            update_psu_communication_state();

            for (auto& c : connectors) {
                c->start();
            }

            modbus_event_loop_thread_run();
        } catch (std::runtime_error& e) {
            if (!is_stop_requested()) {
                log.error << "Error initializing: " + std::string(e.what());
                psu_communication_state = DispenserPsuCommunicationState::FAILED;
            }
        }
    });
}

void Dispenser::stop() {
    psu_communication_state = DispenserPsuCommunicationState::UNINITIALIZED;

    if (modbus_unsolicitated_event_thread.has_value()) {
        modbus_unsolicitated_event_thread->join();
        modbus_unsolicitated_event_thread = std::nullopt;
        log.verbose << "Modbus unsolicitated event thread joined";
    }

    if (modbus_event_loop_thread.has_value()) {
        modbus_event_loop_thread->join();
        modbus_event_loop_thread = std::nullopt;
        log.verbose << "Modbus event loop thread joined";
    }

    if (goose_receiver_thread.has_value()) {
        goose_receiver_thread->join();
        goose_receiver_thread = std::nullopt;
        log.verbose << "Goose receiver thread joined";
    }

    for (auto& c : connectors) {
        c->stop();
    }

    if (modbus_socket.has_value()) {
        if (close(modbus_socket.value()) != 0) {
            log.error << "Could not close modbus socket";
        };
        modbus_socket.reset();
        log.verbose << "Modbus socket closed";
    }

    if (this->openssl_data.has_value()) {
        tls_util::free_ssl(this->openssl_data.value());
        this->openssl_data.reset();
    }

    this->psu_running_mode.reset();

    this->server.reset();
    this->pcl.reset();
    this->protocol.reset();
    this->transport.reset();

    this->registry.reset();
    this->dispenser_registers.reset();
    this->psu_registers.reset();
    this->error_registers.reset();
}

const int Dispenser::do_connect(const char* ip, std::uint16_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        log.error << "Could not open modbus socket";
        throw std::runtime_error("Could not open modbus socket");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    log.info << "Connecting to " + std::string(ip) + ":" + std::to_string(port);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        log.error << "Could not connect to PSU";
        throw std::runtime_error("Could not connect to PSU");
    }
    log.info << "Connected to PSU via TCP";

    return sock;
}

const int Dispenser::connect_with_retry(const char* ip, std::uint16_t port, int retries) {
    for (int i = 0; i < retries; i++) {
        try {
            return do_connect(ip, port);
        } catch (std::runtime_error& e) {
            log.error << "Connection attempt " + std::to_string(i + 1) + " failed";
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 * std::pow(2, i))));
            if (i == retries - 1) {
                throw;
            }
        }
    }

    throw std::runtime_error("Unreachable code when connecting to PSU");
}

PSURunningMode Dispenser::get_psu_running_mode() {
    return psu_registers->psu_running_mode.get_value();
}

void Dispenser::init() {
    log.info << "Using host, port and interface: " + dispenser_config.psu_host + ":" +
                    std::to_string(dispenser_config.psu_port) + " % " + dispenser_config.eth_interface;

    modbus_socket = connect_with_retry(dispenser_config.psu_host.c_str(), dispenser_config.psu_port, 10);

    if (dispenser_config.tls_config.has_value()) {
        log.info << "Using TLS to connect to PSU";
        auto tls = dispenser_config.tls_config.value();
        try {
            auto openssl_data = tls_util::init_mutual_tls_client(modbus_socket.value(), tls);
            this->openssl_data = openssl_data;
            SSL* ssl = std::get<0>(openssl_data);

            transport = std::make_shared<modbus_ssl::OpenSSLTransport>(ssl);
        } catch (std::runtime_error& e) {
            log.error << "Could not connect to PSU using TLS: " + std::string(e.what());
            throw;
        }

    } else {
        log.info << "TLS not configured, using plain TCP to connect to PSU";
        transport = std::make_shared<modbus_server::ModbusSocketTransport>(modbus_socket.value());
    }

    protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport, 0, 0);
    pcl = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);
    server.emplace(pcl, log);

    error_registers.emplace();

    psu_registers.emplace();

    DispenserRegistersConfig dispenser_registers_config;
    dispenser_registers_config.manufacturer = dispenser_config.manufacturer;
    dispenser_registers_config.model = dispenser_config.model;
    dispenser_registers_config.protocol_version = dispenser_config.protocol_version;
    dispenser_registers_config.hardware_version = dispenser_config.hardware_version;
    dispenser_registers_config.software_version = dispenser_config.software_version;
    dispenser_registers_config.esn = dispenser_config.esn;
    dispenser_registers_config.connector_count = dispenser_config.charging_connector_count;

    dispenser_registers.emplace(dispenser_registers_config);

    // add received callbacks
    psu_registers->psu_mac.add_write_callback([this](const std::uint8_t* value) { this->psu_mac_received = true; });

    // Callbacks for common power unit registers
    psu_registers->manufacturer.add_write_callback([this](std::uint16_t value) {
        log.debug << "Dispenser   : PSU Manufacturer changed to " + std::to_string(value);
    });
    psu_registers->protocol_version.add_write_callback([this](std::uint16_t value) {
        log.debug << "Dispenser   : PSU Protocol version changed to " + std::to_string(value);
    });
    psu_registers->esn_control_board.add_write_callback(
        [this](const std::string& value) { log.debug << "Dispenser   : PSU ESN Control Board changed to " + value; });
    psu_registers->software_version.add_write_callback(
        [this](const std::string& value) { log.debug << "Dispenser   : PSU Software version changed to " + value; });
    psu_registers->hardware_version.add_write_callback(
        [this](std::uint16_t val) { log.debug << "Dispenser   : PSU HW version changed to " + std::to_string(val); });

    psu_registers->psu_running_mode.add_write_callback([this](SettingPowerUnitRegisters::PSURunningMode new_value) {
        if (psu_running_mode.has_value() and psu_running_mode.value() == new_value) {
            return; // no change
        }

        psu_running_mode = new_value;
        log.info << "Dispenser   : PSU Running mode changed to " +
                        SettingPowerUnitRegisters::psu_running_mode_to_string(new_value);
    });

    psu_registers->psu_mac.add_write_callback([this](const std::uint8_t* value) {
        char mac_str[18];
        sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", value[0], value[1], value[2], value[3], value[4], value[5]);
        log.debug << "Dispenser   : 🍔 PSU (Big) MAC changed to " + std::string(mac_str);

        auto mac = std::vector<std::uint8_t>(value, value + 6);

        for (auto& c : connectors) {
            c->on_psu_mac_change(mac);
        }

        update_psu_communication_state();
    });

    registry.emplace(UnsolicitatedRegistry());

    error_registers->add_to_registry(registry.value());
    error_registers->add_callback([this](ErrorEvent event) {
        {
            std::lock_guard<std::mutex> lock(this->raised_error_mutex);

            if (event.payload.is_error()) {
                if (raised_errors.find(event) != raised_errors.end()) {
                    raised_errors.erase(event);
                }
                raised_errors.insert(event);
            } else {
                if (raised_errors.find(event) != raised_errors.end()) {
                    raised_errors.erase(event);
                }
            }
        }
    });

    dispenser_registers->add_to_registry(registry.value());
    psu_registers->add_to_registry(registry.value());

    for (auto& c : connectors) {
        c->connector_registers.add_to_registry(registry.value());
    }

    registry->verify_overlap();

    server->set_read_holding_registers_request_cb([this](const modbus_server::pdu::ReadHoldingRegistersRequest& req) {
        auto data = registry->on_read(req.register_start, req.register_count);
        return modbus_server::pdu::ReadHoldingRegistersResponse(req, data);
    });
    server->set_write_multiple_registers_request_cb(
        [this](const modbus_server::pdu::WriteMultipleRegistersRequest& req) {
            registry->on_write(req.register_start, req.register_data);
            return modbus_server::pdu::WriteMultipleRegistersResponse(req);
        });
    server->set_write_single_register_request_cb([this](const modbus_server::pdu::WriteSingleRegisterRequest& req) {
        registry->on_write(req.register_address,
                           {(std::uint8_t)(req.register_value >> 8), (std::uint8_t)(req.register_value & 0xff)});
        return modbus_server::pdu::WriteSingleRegisterResponse(req);
    });

    modbus_unsolicitated_event_thread = std::thread([this]() { modbus_unsolicitated_event_thread_run(); });

    goose_receiver_thread = std::thread([this]() { goose_receiver_thread_run(); });
}

void Dispenser::update_psu_communication_state() {
    if (!psu_mac_received) {
        return;
    }
    // todo: do we have to check whether received mac is "valid"?

    psu_communication_state = DispenserPsuCommunicationState::READY;
}

DispenserPsuCommunicationState Dispenser::get_psu_communication_state() {
    return psu_communication_state.load();
}

ErrorEventSet Dispenser::get_raised_errors() {
    std::lock_guard<std::mutex> lock(raised_error_mutex);
    return raised_errors;
}

bool Dispenser::psu_communication_is_ok() {
    auto current_state = psu_communication_state.load();
    return current_state == DispenserPsuCommunicationState::INITIALIZING ||
           current_state == DispenserPsuCommunicationState::READY;
}

bool Dispenser::is_stop_requested() {
    return psu_communication_state == DispenserPsuCommunicationState::UNINITIALIZED;
}

Dispenser::~Dispenser() {
    stop();
}
