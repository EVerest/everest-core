// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#define MOCK_REGULAR_ERRORS 0

#include "dispenser.hpp"
#include "power_stack_mock/power_stack_mock.hpp"

using namespace fusion_charger::goose;

PowerStackMockConfig config{
    .eth = "veth1",
    .port = 8502,
    .hmac_key = {0x67e4, 0x2656, 0x0a70, 0xca4a, 0x833c, 0x44b3, 0x1270, 0xca93, 0x55d8, 0x7b02, 0x0f57, 0x8e1e,
                 0x9d19, 0x74c0, 0x2fa6, 0xf680, 0x4c2f, 0xcbdf, 0x735e, 0x711c, 0xec08, 0x5b93, 0x8147, 0x16ad},
};

#if MOCK_REGULAR_ERRORS
bool has_error = false;
uint16_t error_value = 0;
#endif

class Mock {
private:
    uint16_t used_connectors;
    std::unique_ptr<PowerStackMock> mock;

    std::chrono::_V2::steady_clock::time_point periodic_update_deadline = std::chrono::steady_clock::now();

    void periodic_update() {
        auto now = std::chrono::steady_clock::now();
        if (now < periodic_update_deadline) {
            return;
        }
        periodic_update_deadline = now + std::chrono::seconds(5);

#if MOCK_REGULAR_ERRORS
        mock->write_registers(0x4000, {has_error ? 0x0001 : 0x0000});
        has_error = !has_error;

        error_value = (error_value + 1) % 3;
        mock->write_registers(0x40D0, {0, error_value});
#endif

        mock->set_psu_running_mode(PSURunningMode::RUNNING);
        mock->send_mac_address();
        for (int i = 1; i <= used_connectors; i++) { // connector number starts at 1
            mock->send_max_rated_current_of_output_port(100.0, i);
            mock->send_max_rated_voltage_of_output_port(1000.0, i);
            mock->send_rated_power_of_output_port(60.0, i);
        }
    }

    std::array<bool, 4> car_plugged_in = {false, false, false, false};

    void send_goose_key_on_car_plugged_in(uint8_t local_connector_number) {
        auto offset = offset_from_connector_number(local_connector_number);

        auto raw = mock->get_unsolicited_report_data(0x110B + (uint16_t)offset, 1);

        if (raw.size() == 0) {
            return;
        }

        auto working_status = (WorkingStatus)raw[0];
        if (!car_plugged_in[local_connector_number] &&
            working_status == WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED) {
            car_plugged_in[local_connector_number] = true;

            mock->send_hmac_key(local_connector_number);

            printf("Car plugged in\n");
        } else if (car_plugged_in[local_connector_number] && working_status == WorkingStatus::STANDBY) {
            car_plugged_in[local_connector_number] = false;
            printf("Car unplugged\n");
        }
    }

public:
    Mock(std::unique_ptr<PowerStackMock> mock) : mock(std::move(mock)) {
    }

    void run() {
        mock->start_modbus_event_loop();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        used_connectors = mock->read_registers(0x1015, 1)[0];

        printf("Using %d connectors\n", used_connectors);

        while (true) {
            periodic_update();

            for (int i = 1; i <= used_connectors; i++) {
                send_goose_key_on_car_plugged_in(i);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};

void init_tls(int argc, char* argv[]) {
    if (argc < 2) {
        return;
    }
    printf("Using mutual TLS\n");

    std::string tls_certificates_folder = argv[1];

    if (tls_certificates_folder.back() != '/') {
        tls_certificates_folder += "/";
    }

    config.tls_config = tls_util::MutualTlsServerConfig{
        .client_ca = tls_certificates_folder + "dispenser_ca.crt.pem",
        .server_cert = tls_certificates_folder + "psu.crt.pem",
        .server_key = tls_certificates_folder + "psu.key.pem",
    };
}

int main(int argc, char* argv[]) {
    init_tls(argc, argv);

    auto power_stack_mock = std::unique_ptr<PowerStackMock>(PowerStackMock::from_config(config));
    Mock mock_instance(std::move(power_stack_mock));

    mock_instance.run();

    return 1;
}
