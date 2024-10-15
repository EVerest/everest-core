// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <mqtt.h>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

class MqttClient {
public:
    MqttClient(std::string mqtt_host, std::string mqtt_port);
    ~MqttClient();

    void publish(const std::string& topic, const std::string& message);

private:
    struct mqtt_client client;
    char sendbuf[500 * 1024];
    char recvbuf[1024];
    std::thread background_thread;
    bool stop_flag = false;

    struct PublishQueueEntry {
        std::string topic;
        std::string message;
    };
    std::mutex publish_queue_mutex;
    std::vector<PublishQueueEntry> publish_queue;

    static int open_socket(std::string host, std::string port);

    void background_thread_fn();
};
