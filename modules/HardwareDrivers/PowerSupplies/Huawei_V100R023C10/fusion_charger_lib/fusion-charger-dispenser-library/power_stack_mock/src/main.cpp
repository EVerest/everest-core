// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#define MOCK_REGULAR_ERRORS 0

#include <functional>

#include "dispenser.hpp"
#include "mqtt.hpp"
#include "power_stack_mock/power_stack_mock.hpp"
#include "socket_server.hpp"

using namespace fusion_charger::goose;

#if MOCK_REGULAR_ERRORS
bool has_error = false;
std::uint16_t error_value = 0;
#endif

static bool environment_variable_enabled(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    if (value == nullptr) {
        return false; // Environment variable not set
    }
    std::string value_str(value);
    return value_str == "1" || value_str == "true";
}

class MqttPowerRequestPublisher {
    constexpr static std::chrono::milliseconds PUBLISH_INTERVAL{1000};

public:
    MqttPowerRequestPublisher(std::shared_ptr<MqttClient> mqtt_client, const std::string& base_topic) :
        mqtt_client(mqtt_client), base_topic(base_topic) {
    }

    void publish(double voltage, double current, std::uint16_t global_connector_number) {
        const auto data =
            "{\"voltage\": " + std::to_string(voltage) + ", \"current\": " + std::to_string(current) + "}";

        {
            std::lock_guard<std::mutex> lock(last_publish_mutex);
            auto now = std::chrono::steady_clock::now();

            if (last_publish_data.find(global_connector_number) != last_publish_data.end()) {
                std::string last_data = last_publish_data[global_connector_number];
                auto deadline_at = publish_deadline[global_connector_number];

                if (last_data == data and now < deadline_at) {
                    return; // data is the same and deadline has not expired yet -> no
                            // need to publish
                }
            }

            publish_deadline[global_connector_number] = now + PUBLISH_INTERVAL;
            last_publish_data[global_connector_number] = data;
        }

        std::string topic = base_topic + std::to_string(global_connector_number) + "/power_request";

        mqtt_client->publish(topic, data);
    }

    void publish(const PowerRequirementRequest& req) {
        publish(req.voltage, req.current, req.charging_connector_no);
    }

    void publish(const StopChargeRequest& req) {
        publish(0.0, 0.0, req.charging_connector_no);
    }

private:
    std::shared_ptr<MqttClient> mqtt_client;
    std::string base_topic;

    std::unordered_map<std::uint8_t, std::chrono::steady_clock::time_point> publish_deadline;
    std::unordered_map<std::uint8_t, std::string> last_publish_data;
    std::mutex last_publish_mutex;
};

// Mock for a single dispenser that simulates a PowerStack device
class Mock {
private:
    std::uint16_t used_connectors;
    std::unique_ptr<PowerStackMock> mock;

    std::chrono::_V2::steady_clock::time_point periodic_update_deadline = std::chrono::steady_clock::now();

    int not_sending_capabilities_counter = 0; // used to test "capabilities not received" error

    double mock_total_historical_ac_input_energy = 0.0;

    /**
     * @brief generate voltage, current and total historic power values and send them to the dispenser
     */
    void generate_and_send_voltage_current_power(int seconds_since_last_call) {
        double used_power = 0.0; // power in W that is being drawn by the EV(s)

        for (int i = 1; i <= used_connectors; i++) {
            auto working_status = mock->get_working_status(i);
            if (not working_status.has_value() || working_status.value() != WorkingStatus::CHARGING) {
                continue;
            }

            auto global_connector_number = mock->get_global_connector_number_from_local(i);
            if (!global_connector_number.has_value()) {
                continue;
            }

            auto req_opt = mock->get_last_power_requirement_request(i);
            if (req_opt.has_value()) {
                auto req = req_opt.value();
                used_power += req.voltage * req.current;
            }
        }

        // add to total historical power
        mock_total_historical_ac_input_energy += (used_power * seconds_since_last_call) / 3600.0 / 1000.0; // kWh
        mock_total_historical_ac_input_energy += 0.00001; // add a bit more to simulate standby consumption

        double ac_base_voltage = 230.0;
        ac_base_voltage +=
            static_cast<double>((std::rand() % 256) / 10.0 - 12.8); // add noise between -12.8V and +12.7V

        double ac_base_current = used_power / ac_base_voltage / 3.0; // 3 phases

        mock->send_total_historical_ac_input_energy(mock_total_historical_ac_input_energy);
        mock->send_ac_input_voltages_currents(ac_base_voltage, ac_base_voltage, ac_base_voltage, ac_base_current,
                                              ac_base_current, ac_base_current);
    }

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

        if (not_sending_capabilities_counter > 1) {
            for (int i = 1; i <= used_connectors; i++) { // connector number starts at 1
                mock->send_max_rated_current_of_output_port(100.0, i);
                mock->send_min_rated_current_of_output_port(1.0, i);
                mock->send_max_rated_voltage_of_output_port(1000.0, i);
                mock->send_min_rated_voltage_of_output_port(100.0, i);
                mock->send_rated_power_of_output_port(60.0, i);

                mock->send_port_available(true, i);
            }
        } else {
            not_sending_capabilities_counter++;
        }

        generate_and_send_voltage_current_power(5); // called every 5 seconds, the approximation is good enough
    }

    std::array<bool, 4> car_plugged_in = {false, false, false, false};

    void send_goose_key_on_car_plugged_in(std::uint8_t local_connector_number) {
        auto offset = offset_from_connector_number(local_connector_number);

        auto raw = mock->get_unsolicited_report_data(0x110B + (std::uint16_t)offset, 1);

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

    void stop() {
        if (mock) {
            mock->stop_modbus_event_loop();
            mock.reset();
        }
    }
};

// parse args and update config for mTLS
void init_tls(int argc, char* argv[], PowerStackMockConfig& config) {
    if (argc < 2) {
        return;
    }
    printf("Using mutual TLS\n");

    std::string tls_certificates_folder = argv[1];

    if (tls_certificates_folder.back() != '/') {
        tls_certificates_folder += "/";
    }

    config.tls_config = tls_util::MutualTlsServerConfig{
        tls_certificates_folder + "dispenser_ca.crt.pem",
        tls_certificates_folder + "psu.crt.pem",
        tls_certificates_folder + "psu.key.pem",
    };
}

std::mutex mocks_mutex;
std::atomic<int> active_mocks; // counts all active mocks
std::condition_variable mocks_cv;

std::vector<std::thread> mock_threads;

void on_socket(int socket, void* context) {
    PowerStackMockConfig* config = (PowerStackMockConfig*)context;

    printf("New client\n");

    std::lock_guard<std::mutex> lock(mocks_mutex);
    active_mocks++;

    auto mock = std::make_unique<Mock>(std::unique_ptr<PowerStackMock>(PowerStackMock::from_config(*config, socket)));

    auto thread = std::thread([mock = std::move(mock)]() {
        try {
            mock->run();
        } catch (const std::exception& e) {
        }

        mock->stop();

        std::lock_guard<std::mutex> lock(mocks_mutex);
        active_mocks--;
        mocks_cv.notify_all();
    });

    mock_threads.push_back(std::move(thread));

    mocks_cv.notify_all();
}

int main(int argc, char* argv[]) {
    PowerStackMockConfig config{
        "veth1",
        8502,
        {0x67, 0xe4, 0x26, 0x56, 0x0a, 0x70, 0xca, 0x4a, 0x83, 0x3c, 0x44, 0xb3, 0x12, 0x70, 0xca, 0x93,
         0x55, 0xd8, 0x7b, 0x02, 0x0f, 0x57, 0x8e, 0x1e, 0x9d, 0x19, 0x74, 0xc0, 0x2f, 0xa6, 0xf6, 0x80,
         0x4c, 0x2f, 0xcb, 0xdf, 0x73, 0x5e, 0x71, 0x1c, 0xec, 0x08, 0x5b, 0x93, 0x81, 0x47, 0x16, 0xad},
        true,
        true,
    };

    init_tls(argc, argv, config);

    // Disables securing outgoing GOOSE frames with HMAC (does not affect
    // receiving)
    if (environment_variable_enabled("FUSION_CHARGER_MOCK_DISABLE_SEND_HMAC")) {
        config.enable_hmac = false;
        printf("Sending HMAC disabled\n");
    }
    // Disables verifying HMAC of incoming GOOSE frames (does not affect sending)
    // If this is set to true, the mock will also allow unsecured GOOSE frames
    if (environment_variable_enabled("FUSION_CHARGER_MOCK_DISABLE_VERIFY_HMAC")) {
        config.verify_hmac = false;
        printf("Verifying HMAC disabled\n");
    }

    // Set the Ethernet interface to use
    if (std::getenv("FUSION_CHARGER_MOCK_ETH")) {
        config.eth = std::getenv("FUSION_CHARGER_MOCK_ETH");
        printf("Using Ethernet interface: %s\n", config.eth.c_str());
    } else {
        printf("Using default Ethernet interface: %s\n", config.eth.c_str());
    }

    printf("Waiting for connections on port %d\n", config.port);

    std::shared_ptr<MqttPowerRequestPublisher> mqtt_publisher;

    // If both environment variables are set, use them to create an MQTT client
    if (std::getenv("FUSION_CHARGER_MOCK_MQTT_HOST") && std::getenv("FUSION_CHARGER_MOCK_MQTT_PORT")) {
        std::string mqtt_host = std::getenv("FUSION_CHARGER_MOCK_MQTT_HOST");
        std::string mqtt_port = std::getenv("FUSION_CHARGER_MOCK_MQTT_PORT");
        std::string mqtt_base_topic = "fusion_charger_mock/";
        if (std::getenv("FUSION_CHARGER_MOCK_MQTT_BASE_TOPIC")) {
            mqtt_base_topic = std::getenv("FUSION_CHARGER_MOCK_MQTT_BASE_TOPIC");
            if (mqtt_base_topic.back() != '/') {
                mqtt_base_topic += "/";
            }
        }

        mqtt_publisher = std::make_shared<MqttPowerRequestPublisher>(std::make_shared<MqttClient>(mqtt_host, mqtt_port),
                                                                     mqtt_base_topic);
        printf("Using MQTT client with host: %s and port: %s\n", mqtt_host.c_str(), mqtt_port.c_str());
        printf("Using MQTT base topic: %s\n", mqtt_base_topic.c_str());
    }

    if (mqtt_publisher) {
        config.power_requirement_request_callback = [&mqtt_publisher](const PowerRequirementRequest& req) {
            mqtt_publisher->publish(req);
        };

        config.stop_charge_request_callback = [&mqtt_publisher](const StopChargeRequest& req) {
            mqtt_publisher->publish(req);
        };
    }

    SocketServer socket_server(config.port, (void*)&config, on_socket);

    for (;;) {
        std::unique_lock<std::mutex> lock(mocks_mutex);
        mocks_cv.wait(lock);
        if (active_mocks == 0) {
            printf("No dispensers connected anymore, exiting\n");
            for (auto& thread : mock_threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            return 0;
        }
    }

    return 1;
}
