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

using namespace everest::lib::API::V1_0::types;
using namespace everest::lib::io::mqtt;
using namespace everest::lib::io::event;

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
    client_(reconnect_ms, "ev_cfg_api_cli_" + random_id()),
    topics(topics),
    reply_topic_base_("ev_cfg_api_cli_" + random_id() + "/reply/") {

    if (verbose) {
        client_.set_error_handler(
            [](int ec, const std::string& msg) { std::cerr << "MQTT error (" << ec << "): " << msg << "\n"; });
    }

    // client_.connect(...) below calls the backend asynchronously, so we
    // set up the callbacks now to not miss the connection event.
    // The callback will set the connected_promise_ which allows perform_rpc to wait for a successful
    // connection before trying to send requests.
    client_.set_callback_connect([this](auto&, auto rc, auto, auto const&) {
        if (rc == mosquitto_cpp::ResponseCode::Success) {
            connected_promise_.set_value();
        }
    });

    client_.connect(host, port, 60);

    event_handler_.register_event_handler(&client_);
    event_handler_.register_event_handler(&cancel_event_, [] {});

    event_loop_thread_ = std::thread([this] { event_handler_.run(running_); });
}

MqttConfigServiceClient::~MqttConfigServiceClient() {
    running_ = false;
    cancel_event_.notify();
    if (event_loop_thread_.joinable()) {
        event_loop_thread_.join();
    }
    client_.disconnect();
}

void MqttConfigServiceClient::setup_update_subscriptions() {
    if (active_cb_) {
        client_.subscribe(
            topics.nonmodule_to_extern("active_slot"),
            [this](mosquitto_cpp&, const mosquitto_cpp::message& msg) {
                auto notice = config_service::try_deserialize<config_service::ActiveSlotUpdateNotice>(msg.payload);
                if (notice)
                    active_cb_(*notice);
            },
            mosquitto_cpp::QoS::at_least_once);
    }
    if (!suppress_parameter_updates_ && config_cb_) {
        client_.subscribe(
            topics.nonmodule_to_extern("config_updates"),
            [this](mosquitto_cpp&, const mosquitto_cpp::message& msg) {
                auto notice =
                    config_service::try_deserialize<config_service::ConfigurationParameterUpdateNotice>(msg.payload);
                if (notice)
                    config_cb_(*notice);
            },
            mosquitto_cpp::QoS::at_least_once);
    }
}

std::optional<std::string> MqttConfigServiceClient::perform_rpc_raw(const std::string& operation,
                                                                    const std::string& payload_json) {
    if (connected_future_.wait_for(std::chrono::milliseconds(5000)) != std::future_status::ready) {
        std::cerr << "Timeout waiting for MQTT connection\n";
        return std::nullopt;
    }

    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();

    generic::RequestReply rr;
    rr.replyTo = reply_topic_base_;
    rr.payload = payload_json;
    const std::string wrapped = generic::serialize(rr);
    const std::string req_topic = topics.extern_to_nonmodule(operation);

    // Subscribe to the reply topic and publish the request in one action so the
    // subscription is guaranteed to be in place before the server can respond.
    // This is executed in the event loop to ensure thread safety with the MQTT client.
    event_handler_.add_action([this, promise, req_topic, wrapped]() {
        client_.subscribe(
            reply_topic_base_,
            [promise](mosquitto_cpp&, const mosquitto_cpp::message& msg) { promise->set_value(msg.payload); },
            mosquitto_cpp::QoS::at_least_once);
        client_.publish(req_topic, wrapped);
    });

    std::optional<std::string> result;
    if (future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready) {
        result = future.get();
    } else {
        std::cerr << "Timeout waiting for reply on " << reply_topic_base_ << " (requested on " << req_topic << ")\n";
    }

    event_handler_.add_action([this] { client_.unsubscribe(reply_topic_base_, PropertiesBase{}); });

    return result;
}

template <typename ReqType, typename ResType>
std::optional<ResType> MqttConfigServiceClient::perform_rpc(const std::string& operation, const ReqType& req) {
    auto raw = perform_rpc_raw(operation, config_service::serialize(req));
    if (!raw)
        return std::nullopt;
    auto res = config_service::try_deserialize<ResType>(*raw);
    if (!res)
        std::cerr << "Failed to deserialize reply for " << operation << "\n";
    return res;
}

std::optional<config_service::ListSlotIdsResult> MqttConfigServiceClient::list_all_slots() {
    auto raw = perform_rpc_raw("list_all_slots", "{}");
    if (!raw)
        return std::nullopt;
    return config_service::try_deserialize<config_service::ListSlotIdsResult>(*raw);
}

std::optional<config_service::GetActiveSlotIdResult> MqttConfigServiceClient::get_active_slot() {
    auto raw = perform_rpc_raw("get_active_slot", "{}");
    if (!raw)
        return std::nullopt;
    return config_service::try_deserialize<config_service::GetActiveSlotIdResult>(*raw);
}

std::optional<config_service::MarkActiveSlotResult> MqttConfigServiceClient::mark_active_slot(int slot_id) {
    config_service::MarkActiveSlotRequest req;
    req.slot_id = slot_id;
    return perform_rpc<config_service::MarkActiveSlotRequest, config_service::MarkActiveSlotResult>("mark_active_slot",
                                                                                                    req);
}

std::optional<config_service::DeleteSlotResult> MqttConfigServiceClient::delete_slot(int slot_id) {
    config_service::DeleteSlotRequest req;
    req.slot_id = slot_id;
    return perform_rpc<config_service::DeleteSlotRequest, config_service::DeleteSlotResult>("delete_slot", req);
}

std::optional<config_service::DuplicateSlotResult>
MqttConfigServiceClient::duplicate_slot(int slot_id, const std::string& description) {
    config_service::DuplicateSlotRequest req;
    req.slot_id = slot_id;
    req.new_description = description;
    return perform_rpc<config_service::DuplicateSlotRequest, config_service::DuplicateSlotResult>("duplicate_slot",
                                                                                                  req);
}

std::optional<config_service::LoadFromYamlResult>
MqttConfigServiceClient::load_from_yaml(const std::string& raw_yaml, const std::string& description,
                                        std::optional<int> slot_id) {
    config_service::LoadFromYamlRequest req;
    req.raw_yaml = raw_yaml;
    req.description = description;
    req.slot_id = slot_id;
    return perform_rpc<config_service::LoadFromYamlRequest, config_service::LoadFromYamlResult>("load_from_yaml", req);
}

std::optional<config_service::GetConfigurationResult> MqttConfigServiceClient::get_configuration(int slot_id) {
    config_service::GetConfigurationRequest req;
    req.slot_id = slot_id;
    return perform_rpc<config_service::GetConfigurationRequest, config_service::GetConfigurationResult>(
        "get_configuration", req);
}

std::optional<config_service::ConfigurationParameterUpdateRequestResult>
MqttConfigServiceClient::set_config_parameters(const config_service::ConfigurationParameterUpdateRequest& request) {
    return perform_rpc<config_service::ConfigurationParameterUpdateRequest,
                       config_service::ConfigurationParameterUpdateRequestResult>("set_config_parameters", request);
}

void MqttConfigServiceClient::subscribe_to_updates(bool suppress_parameter_updates, ActiveSlotCallback active_cb,
                                                   ConfigUpdateCallback config_cb) {
    suppress_parameter_updates_ = suppress_parameter_updates;
    active_cb_ = std::move(active_cb);
    config_cb_ = std::move(config_cb);
    if (connected_future_.wait_for(std::chrono::milliseconds(5000)) != std::future_status::ready) {
        std::cerr << "Timeout waiting for MQTT connection\n";
        return;
    }
    event_handler_.add_action([this] { setup_update_subscriptions(); });
}

} // namespace everest::config_cli
