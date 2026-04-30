// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "mqtt_config_service_client.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <utility>

#include <everest/io/event/event_fd.hpp>
#include <everest_api_types/generic/codec.hpp>

namespace everest::config_cli {

namespace API_types = everest::lib::API::V1_0::types;
namespace API_generic = API_types::generic;
namespace API_types_cfg = API_types::configuration;
namespace IO_mqtt = everest::lib::io::mqtt;
namespace IO_event = everest::lib::io::event;

namespace {
std::string random_id(std::size_t len = 16) {
    static const char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<std::size_t> dist{0, sizeof(chars) - 2};
    std::string s;
    s.reserve(len);
    for (std::size_t i = 0; i < len; ++i)
        s += chars[dist(rng)];
    return s;
}
} // namespace

MqttConfigServiceClient::MqttConfigServiceClient(const std::string& host, std::uint16_t port,
                                                 const everest::lib::API::Topics& topics, std::uint32_t reconnect_ms,
                                                 bool verbose) :
    m_client(reconnect_ms, "ev_cfg_api_cli_" + random_id()),
    m_topics(topics),
    m_reply_topic_base("ev_cfg_api_cli_" + random_id() + "/reply/") {

    if (verbose) {
        m_client.set_error_handler(
            [](int ec, const std::string& msg) { std::cerr << "MQTT error (" << ec << "): " << msg << "\n"; });
    }

    // m_client.connect(...) below calls the backend asynchronously, so we
    // set up the callbacks now to not miss the connection event.
    // The callback will set the m_connected_promise which allows perform_rpc to wait for a successful
    // connection before trying to send requests.
    m_client.set_callback_connect([this](auto&, auto rc, auto, auto const&) {
        if (rc == IO_mqtt::mosquitto_cpp::ResponseCode::Success) {
            m_connected_promise.set_value();
        }
    });

    m_client.connect(host, port, 60);

    m_event_handler.register_event_handler(&m_client);
    m_event_handler.register_event_handler(&m_cancel_event, [] {});

    m_event_loop_thread = std::thread([this] { m_event_handler.run(m_running); });
}

MqttConfigServiceClient::~MqttConfigServiceClient() {
    m_running = false;
    m_cancel_event.notify();
    if (m_event_loop_thread.joinable()) {
        m_event_loop_thread.join();
    }
    m_client.disconnect();
}

void MqttConfigServiceClient::setup_update_subscriptions() {
    if (m_active_cb) {
        m_client.subscribe(
            m_topics.nonmodule_to_extern("active_slot"),
            [this](IO_mqtt::mosquitto_cpp&, const IO_mqtt::mosquitto_cpp::message& msg) {
                auto notice = API_types_cfg::try_deserialize<API_types_cfg::ActiveSlotUpdateNotice>(msg.payload);
                if (notice)
                    m_active_cb(*notice);
            },
            IO_mqtt::mosquitto_cpp::QoS::at_least_once);
    }
    if (!m_suppress_parameter_updates && m_config_cb) {
        m_client.subscribe(
            m_topics.nonmodule_to_extern("config_updates"),
            [this](IO_mqtt::mosquitto_cpp&, const IO_mqtt::mosquitto_cpp::message& msg) {
                auto notice =
                    API_types_cfg::try_deserialize<API_types_cfg::ConfigurationParameterUpdateNotice>(msg.payload);
                if (notice)
                    m_config_cb(*notice);
            },
            IO_mqtt::mosquitto_cpp::QoS::at_least_once);
    }
}

std::optional<std::string> MqttConfigServiceClient::perform_rpc_raw(const std::string& operation,
                                                                    const std::string& payload_json) {
    if (m_connected_future.wait_for(std::chrono::milliseconds(5000)) != std::future_status::ready) {
        std::cerr << "Timeout waiting for MQTT connection\n";
        return std::nullopt;
    }

    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();

    API_generic::RequestReply rr;
    rr.replyTo = m_reply_topic_base;
    rr.payload = payload_json;
    const std::string wrapped = API_generic::serialize(rr);
    const std::string req_topic = m_topics.extern_to_nonmodule(operation);

    // Subscribe to the reply topic and publish the request in one action so the
    // subscription is guaranteed to be in place before the server can respond.
    // This is executed in the event loop to ensure thread safety with the MQTT client.
    m_event_handler.add_action([this, promise, req_topic, wrapped]() {
        m_client.subscribe(
            m_reply_topic_base,
            [promise](IO_mqtt::mosquitto_cpp&, const IO_mqtt::mosquitto_cpp::message& msg) { promise->set_value(msg.payload); },
            IO_mqtt::mosquitto_cpp::QoS::at_least_once);
        m_client.publish(req_topic, wrapped);
    });

    std::optional<std::string> result;
    if (future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready) {
        result = future.get();
    } else {
        std::cerr << "Timeout waiting for reply on " << m_reply_topic_base << " (requested on " << req_topic << ")\n";
    }

    m_event_handler.add_action([this] { m_client.unsubscribe(m_reply_topic_base, IO_mqtt::PropertiesBase{}); });

    return result;
}

template <typename ReqType, typename ResType>
std::optional<ResType> MqttConfigServiceClient::perform_rpc(const std::string& operation, const ReqType& req) {
    auto raw = perform_rpc_raw(operation, API_types_cfg::serialize(req));
    if (!raw)
        return std::nullopt;
    auto res = API_types_cfg::try_deserialize<ResType>(*raw);
    if (!res)
        std::cerr << "Failed to deserialize reply for " << operation << "\n";
    return res;
}

std::optional<API_types_cfg::ListSlotIdsResult> MqttConfigServiceClient::list_all_slots() {
    auto raw = perform_rpc_raw("list_all_slots", "{}");
    if (!raw)
        return std::nullopt;
    return API_types_cfg::try_deserialize<API_types_cfg::ListSlotIdsResult>(*raw);
}

std::optional<API_types_cfg::GetActiveSlotIdResult> MqttConfigServiceClient::get_active_slot() {
    auto raw = perform_rpc_raw("get_active_slot", "{}");
    if (!raw)
        return std::nullopt;
    return API_types_cfg::try_deserialize<API_types_cfg::GetActiveSlotIdResult>(*raw);
}

std::optional<API_types_cfg::MarkActiveSlotResult> MqttConfigServiceClient::mark_active_slot(int slot_id) {
    API_types_cfg::MarkActiveSlotRequest req;
    req.slot_id = slot_id;
    return perform_rpc<API_types_cfg::MarkActiveSlotRequest, API_types_cfg::MarkActiveSlotResult>("mark_active_slot",
                                                                                                    req);
}

std::optional<API_types_cfg::DeleteSlotResult> MqttConfigServiceClient::delete_slot(int slot_id) {
    API_types_cfg::DeleteSlotRequest req;
    req.slot_id = slot_id;
    return perform_rpc<API_types_cfg::DeleteSlotRequest, API_types_cfg::DeleteSlotResult>("delete_slot", req);
}

std::optional<API_types_cfg::DuplicateSlotResult>
MqttConfigServiceClient::duplicate_slot(int slot_id, const std::string& description) {
    API_types_cfg::DuplicateSlotRequest req;
    req.slot_id = slot_id;
    req.new_description = description;
    return perform_rpc<API_types_cfg::DuplicateSlotRequest, API_types_cfg::DuplicateSlotResult>("duplicate_slot",
                                                                                                  req);
}

std::optional<API_types_cfg::LoadFromYamlResult>
MqttConfigServiceClient::load_from_yaml(const std::string& raw_yaml, const std::string& description,
                                        std::optional<int> slot_id) {
    API_types_cfg::LoadFromYamlRequest req;
    req.raw_yaml = raw_yaml;
    req.description = description;
    req.slot_id = slot_id;
    return perform_rpc<API_types_cfg::LoadFromYamlRequest, API_types_cfg::LoadFromYamlResult>("load_from_yaml", req);
}

std::optional<API_types_cfg::GetConfigurationResult> MqttConfigServiceClient::get_configuration(int slot_id) {
    API_types_cfg::GetConfigurationRequest req;
    req.slot_id = slot_id;
    return perform_rpc<API_types_cfg::GetConfigurationRequest, API_types_cfg::GetConfigurationResult>(
        "get_configuration", req);
}

std::optional<API_types_cfg::ConfigurationParameterUpdateRequestResult>
MqttConfigServiceClient::set_config_parameters(const API_types_cfg::ConfigurationParameterUpdateRequest& request) {
    return perform_rpc<API_types_cfg::ConfigurationParameterUpdateRequest,
                       API_types_cfg::ConfigurationParameterUpdateRequestResult>("set_config_parameters", request);
}

void MqttConfigServiceClient::subscribe_to_updates(bool suppress_parameter_updates, ActiveSlotCallback active_cb,
                                                   ConfigUpdateCallback config_cb) {
    m_suppress_parameter_updates = suppress_parameter_updates;
    m_active_cb = std::move(active_cb);
    m_config_cb = std::move(config_cb);
    if (m_connected_future.wait_for(std::chrono::milliseconds(5000)) != std::future_status::ready) {
        std::cerr << "Timeout waiting for MQTT connection\n";
        return;
    }
    m_event_handler.add_action([this] { setup_update_subscriptions(); });
}

} // namespace everest::config_cli
