// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "error_history//json_codec.hpp"
#include "error_history/API.hpp"
#include "error_history/codec.hpp"
#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::error_history {

void to_json(json& j, State const& k) noexcept {
    switch (k) {
    case State::Active:
        j = "Active";
        return;
    case State::ClearedByModule:
        j = "ClearedByModule";
        return;
    case State::ClearedByReboot:
        j = "ClearedByReboot";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::error_history::State";
}

void from_json(const json& j, State& k) {
    std::string s = j;
    if (s == "Active") {
        k = State::Active;
        return;
    }
    if (s == "ClearedByModule") {
        k = State::ClearedByModule;
        return;
    }
    if (s == "ClearedByReboot") {
        k = State::ClearedByReboot;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::error_history::State");
}

void to_json(json& j, SeverityFilter const& k) noexcept {
    switch (k) {
    case SeverityFilter::HIGH_GE:
        j = "HIGH_GE";
        return;
    case SeverityFilter::MEDIUM_GE:
        j = "MEDIUM_GE";
        return;
    case SeverityFilter::LOW_GE:
        j = "LOW_GE";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::error_history::SeverityFilter";
}

void from_json(const json& j, SeverityFilter& k) {
    std::string s = j;
    if (s == "HIGH_GE") {
        k = SeverityFilter::HIGH_GE;
        return;
    }
    if (s == "MEDIUM_GE") {
        k = SeverityFilter::MEDIUM_GE;
        return;
    }
    if (s == "LOW_GE") {
        k = SeverityFilter::LOW_GE;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::error_history::SeverityFilter");
}

void to_json(json& j, Severity const& k) noexcept {
    switch (k) {
    case Severity::High:
        j = "High";
        return;
    case Severity::Medium:
        j = "Medium";
        return;
    case Severity::Low:
        j = "Low";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::error_history::Severity";
}

void from_json(const json& j, Severity& k) {
    std::string s = j;
    if (s == "High") {
        k = Severity::High;
        return;
    }
    if (s == "Medium") {
        k = Severity::Medium;
        return;
    }
    if (s == "Low") {
        k = Severity::Low;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::error_history::Severity");
}

void to_json(json& j, ImplementationIdentifier const& k) noexcept {
    j = json{
        {"module_id", k.module_id},
        {"implementation_id", k.implementation_id},
    };
}

void from_json(const json& j, ImplementationIdentifier& k) {
    k.module_id = j.at("module_id");
    k.implementation_id = j.at("implementation_id");
}

void to_json(json& j, TimeperiodFilter const& k) noexcept {
    j = json{
        {"timestamp_from", k.timestamp_from},
        {"timestamp_to", k.timestamp_to},
    };
}

void from_json(const json& j, TimeperiodFilter& k) {
    k.timestamp_from = j.at("timestamp_from");
    k.timestamp_to = j.at("timestamp_to");
}

void to_json(json& j, FilterArguments const& k) noexcept {
    j = json({});
    if (k.state_filter) {
        j["state_filter"] = k.state_filter.value();
    }
    if (k.origin_filter) {
        j["origin_filter"] = k.origin_filter.value();
    }
    if (k.type_filter) {
        j["type_filter"] = k.type_filter.value();
    }
    if (k.severity_filter) {
        j["severity_filter"] = k.severity_filter.value();
    }
    if (k.timeperiod_filter) {
        j["timeperiod_filter"] = k.timeperiod_filter.value();
    }
    if (k.handle_filter) {
        j["handle_filter"] = k.handle_filter.value();
    }
}

void from_json(const json& j, FilterArguments& k) {
    if (j.contains("state_filter")) {
        k.state_filter.emplace(j.at("state_filter"));
    }
    if (j.contains("origin_filter")) {
        k.origin_filter.emplace(j.at("origin_filter"));
    }
    if (j.contains("type_filter")) {
        k.type_filter.emplace(j.at("type_filter"));
    }
    if (j.contains("severity_filter")) {
        k.severity_filter.emplace(j.at("severity_filter"));
    }
    if (j.contains("timeperiod_filter")) {
        k.timeperiod_filter.emplace(j.at("timeperiod_filter"));
    }
    if (j.contains("handle_filter")) {
        k.handle_filter.emplace(j.at("handle_filter"));
    }
}

void to_json(json& j, ErrorObject const& k) noexcept {
    j = json{
        {"type", k.type},     {"description", k.description}, {"message", k.message}, {"severity", k.severity},
        {"origin", k.origin}, {"timestamp", k.timestamp},     {"uuid", k.uuid},       {"state", k.state},
    };
    if (k.sub_type) {
        j["sub_type"] = k.sub_type.value();
    }
}

void from_json(const json& j, ErrorObject& k) {
    k.type = j.at("type");
    k.description = j.at("description");
    k.message = j.at("message");
    k.severity = j.at("severity");
    k.origin = j.at("origin");
    k.timestamp = j.at("timestamp");
    k.uuid = j.at("uuid");
    k.state = j.at("state");

    if (j.contains("sub_type")) {
        k.sub_type.emplace(j.at("sub_type"));
    }
}

void to_json(json& j, ErrorList const& k) noexcept {
    j["errors"] = json::array();
    for (auto val : k.errors) {
        j["errors"].push_back(val);
    }
}

void from_json(const json& j, ErrorList& k) {
    k.errors.clear();
    for (auto val : j.at("errors")) {
        k.errors.push_back(val);
    }
}

} // namespace everest::lib::API::V1_0::types::error_history
