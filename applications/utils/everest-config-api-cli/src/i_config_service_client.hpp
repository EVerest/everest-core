// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <functional>
#include <optional>
#include <string>

#include <everest_api_types/config_service/codec.hpp>

namespace everest::config_cli {

using namespace everest::lib::API::V1_0::types::config_service;

class IConfigServiceClient {
public:
    virtual ~IConfigServiceClient() = default;

    virtual std::optional<ListSlotIdsResult> list_all_slots() = 0;
    virtual std::optional<GetActiveSlotIdResult> get_active_slot() = 0;
    virtual std::optional<MarkActiveSlotResult> mark_active_slot(int slot_id) = 0;
    virtual std::optional<DeleteSlotResult> delete_slot(int slot_id) = 0;
    virtual std::optional<DuplicateSlotResult> duplicate_slot(int slot_id, const std::string& description) = 0;
    virtual std::optional<LoadFromYamlResult>
    load_from_yaml(const std::string& raw_yaml, const std::string& description, std::optional<int> slot_id) = 0;
    virtual std::optional<GetConfigurationResult> get_configuration(int slot_id) = 0;
    virtual std::optional<ConfigurationParameterUpdateRequestResult>
    set_config_parameters(const ConfigurationParameterUpdateRequest& request) = 0;

    using ActiveSlotCallback = std::function<void(const ActiveSlotUpdateNotice&)>;
    using ConfigUpdateCallback = std::function<void(const ConfigurationParameterUpdateNotice&)>;

    virtual void subscribe_to_updates(bool suppress_parameter_updates, ActiveSlotCallback active_cb,
                                      ConfigUpdateCallback config_cb) = 0;
};

} // namespace everest::config_cli
