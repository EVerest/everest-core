// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "mqtt.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <mqtt.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <stdexcept>
#include <string>

MqttClient::MqttClient(std::string mqtt_host, std::string mqtt_port) {
    int sockfd = open_socket(mqtt_host, mqtt_port);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open MQTT socket\n");
        throw std::runtime_error("Failed to open MQTT socket");
    }
    MQTTErrors err = mqtt_init(&client, sockfd, (std::uint8_t*)sendbuf, sizeof(sendbuf), (std::uint8_t*)recvbuf,
                               sizeof(recvbuf), NULL);

    client.publish_response_callback_state = this;

    if (err != MQTT_OK) {
        fprintf(stderr, "Failed to initialize MQTT client: %s\n", mqtt_error_str(err));
        throw std::runtime_error("Failed to initialize MQTT client");
    }

    err = mqtt_connect(&client, NULL, NULL, NULL, 0, NULL, NULL, MQTT_CONNECT_CLEAN_SESSION, 400);
    if (err != MQTT_OK) {
        fprintf(stderr, "Failed to connect to MQTT broker: %s\n", mqtt_error_str(err));
        throw std::runtime_error("Failed to connect to MQTT broker");
    }

    background_thread = std::thread(&MqttClient::background_thread_fn, this);
}

MqttClient::~MqttClient() {
    stop_flag = true;

    if (background_thread.joinable()) {
        background_thread.join();
    }
}

void MqttClient::publish(const std::string& topic, const std::string& message) {
    std::lock_guard<std::mutex> lock(publish_queue_mutex);
    publish_queue.push_back({topic, message});
}

void MqttClient::background_thread_fn() {
    while (!stop_flag) {
        std::vector<PublishQueueEntry> queue_copy;

        {
            std::lock_guard<std::mutex> lock(publish_queue_mutex);
            queue_copy.swap(publish_queue);
        }

        for (const auto& entry : queue_copy) {
            MQTTErrors err = mqtt_publish(&client, entry.topic.c_str(), entry.message.data(), entry.message.size(),
                                          MQTT_PUBLISH_QOS_0);
            if (err != MQTT_OK) {
                fprintf(stderr, "Failed to publish message: %s\n", mqtt_error_str(err));
            }
        }

        mqtt_sync(&client);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int MqttClient::open_socket(std::string host, std::string port) {
    struct addrinfo hints = {0};

    hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
    int sockfd = -1;
    int rv;
    struct addrinfo *p, *servinfo;

    /* get address information */
    rv = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "Failed to open socket (getaddrinfo): %s\n", gai_strerror(rv));
        return -1;
    }

    /* open the first possible socket */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        /* connect to server */
        rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (rv == -1) {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    /* free servinfo */
    freeaddrinfo(servinfo);

    /* make non-blocking */
    if (sockfd != -1)
        fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

    /* return the new socket fd */
    return sockfd;
}
