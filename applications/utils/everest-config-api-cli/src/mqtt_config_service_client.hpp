// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include "i_config_service_client.hpp"

#include "everest_api_types/utilities/Topics.hpp"
#include <everest/io/event/event_fd.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>

#include <atomic>
#include <cstdint>
#include <future>
#include <memory>
#include <string>
#include <thread>

namespace everest::config_cli {

class MqttConfigServiceClient : public IConfigServiceClient {
public:
    MqttConfigServiceClient(const std::string& host, std::uint16_t port, const everest::lib::API::Topics& topics,
                            std::uint32_t reconnect_ms = 5000, bool verbose = false);
    ~MqttConfigServiceClient() override;

    std::optional<ListSlotIdsResult> list_all_slots() override;
    std::optional<GetActiveSlotIdResult> get_active_slot() override;
    std::optional<MarkActiveSlotResult> mark_active_slot(int slot_id) override;
    std::optional<DeleteSlotResult> delete_slot(int slot_id) override;
    std::optional<DuplicateSlotResult> duplicate_slot(int slot_id, const std::string& description) override;
    std::optional<LoadFromYamlResult> load_from_yaml(const std::string& raw_yaml, const std::string& description,
                                                     std::optional<int> slot_id) override;
    std::optional<GetConfigurationResult> get_configuration(int slot_id) override;
    std::optional<ConfigurationParameterUpdateRequestResult>
    set_config_parameters(const ConfigurationParameterUpdateRequest& request) override;
    void subscribe_to_updates(bool suppress_parameter_updates, ActiveSlotCallback active_cb,
                              ConfigUpdateCallback config_cb) override;

private:
    std::optional<std::string> perform_rpc_raw(const std::string& operation, const std::string& payload_json);

    template <typename ReqType, typename ResType>
    std::optional<ResType> perform_rpc(const std::string& operation, const ReqType& req);

    void setup_update_subscriptions();

    everest::lib::io::event::fd_event_handler event_handler_;
    everest::lib::io::mqtt::mqtt_client client_;

    everest::lib::API::Topics topics;
    std::string reply_topic_base_;

    std::atomic<bool> running_{true};
    everest::lib::io::event::event_fd cancel_event_;
    std::thread event_loop_thread_;

    std::promise<void> connected_promise_;
    std::shared_future<void> connected_future_{connected_promise_.get_future().share()};

    bool suppress_parameter_updates_{false};
    ActiveSlotCallback active_cb_;
    ConfigUpdateCallback config_cb_;
};

} // namespace everest::config_cli
