// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost/json_codec.hpp"
#include "session_cost/API.hpp"
#include "session_cost/codec.hpp"

#include "auth/json_codec.hpp"
#include "money/json_codec.hpp"
#include "text_message/json_codec.hpp"

#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::session_cost {

using json = nlohmann::json;

void to_json(json& j, TariffMessage const& k) noexcept {
    j = json{
        {"messages", k.messages},
    };
    if (k.ocpp_transaction_id) {
        j["ocpp_transaction_id"] = k.ocpp_transaction_id.value();
    }
    if (k.identifier_id) {
        j["identifier_id"] = k.identifier_id.value();
    }
    if (k.identifier_type) {
        j["identifier_type"] = k.identifier_type.value();
    }
}
void from_json(const json& j, TariffMessage& k) {
    for (auto val : j.at("messages")) {
        k.messages.push_back(val);
    }
    if (j.contains("ocpp_transaction_id")) {
        k.ocpp_transaction_id.emplace(j.at("ocpp_transaction_id"));
    }
    if (j.contains("identifier_id")) {
        k.identifier_id.emplace(j.at("identifier_id"));
    }
    if (j.contains("identifier_type")) {
        k.identifier_type.emplace(j.at("identifier_type"));
    }
}

void to_json(json& j, IdlePrice const& k) noexcept {
    j = json{};
    if (k.hour_price) {
        j["hour_price"] = k.hour_price.value();
    }
    if (k.grace_minutes) {
        j["grace_minutes"] = k.grace_minutes.value();
    }
}
void from_json(const json& j, IdlePrice& k) {
    if (j.contains("grace_minutes")) {
        k.grace_minutes.emplace(j.at("grace_minutes"));
    }
    if (j.contains("hour_price")) {
        k.hour_price = j.at("hour_price");
    }
}

void to_json(json& j, CostCategory const& k) noexcept {
    switch (k) {
    case CostCategory::Energy:
        j = "Energy";
        return;
    case CostCategory::Time:
        j = "Time";
        return;
    case CostCategory::FlatFee:
        j = "FlatFee";
        return;
    case CostCategory::Other:
        j = "Other";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::session_cost::CostCategory";
}
void from_json(const json& j, CostCategory& k) {
    std::string s = j;
    if (s == "Energy") {
        k = CostCategory::Energy;
        return;
    }
    if (s == "Time") {
        k = CostCategory::Time;
        return;
    }
    if (s == "FlatFee") {
        k = CostCategory::FlatFee;
        return;
    }
    if (s == "Other") {
        k = CostCategory::Other;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::session_cost::CostCategory");
}

void to_json(json& j, ChargingPriceComponent const& k) noexcept {
    j = json{};
    if (k.category) {
        j["category"] = k.category.value();
    }
    if (k.price) {
        j["price"] = k.price.value();
    }
}
void from_json(const json& j, ChargingPriceComponent& k) {
    if (j.contains("category")) {
        k.category.emplace(j.at("category"));
    }
    if (j.contains("price")) {
        k.price.emplace(j.at("price"));
    }
}

void to_json(json& j, NextPeriodPrice const& k) noexcept {
    j = json{
        {"timestamp_from", k.timestamp_from},
        {"charging_price", k.charging_price},
    };
    if (k.idle_price) {
        j["idle_price"] = k.idle_price.value();
    }
}
void from_json(const json& j, NextPeriodPrice& k) {
    k.timestamp_from = j.at("timestamp_from");
    for (auto val : j.at("charging_price")) {
        k.charging_price.push_back(val);
    }
    if (j.contains("idle_price")) {
        k.idle_price.emplace(j.at("idle_price"));
    }
}

void to_json(json& j, SessionCostChunk const& k) noexcept {
    j = json{};
    if (k.timestamp_from) {
        j["timestamp_from"] = k.timestamp_from.value();
    }
    if (k.timestamp_to) {
        j["timestamp_to"] = k.timestamp_to.value();
    }
    if (k.metervalue_from) {
        j["metervalue_from"] = k.metervalue_from.value();
    }
    if (k.metervalue_to) {
        j["metervalue_to"] = k.metervalue_to.value();
    }
    if (k.cost) {
        j["cost"] = k.cost.value();
    }
    if (k.category) {
        j["category"] = k.category.value();
    }
}
void from_json(const json& j, SessionCostChunk& k) {
    if (j.contains("timestamp_from")) {
        k.timestamp_from.emplace(j.at("timestamp_from"));
    }
    if (j.contains("timestamp_to")) {
        k.timestamp_to.emplace(j.at("timestamp_to"));
    }
    if (j.contains("metervalue_from")) {
        k.metervalue_from.emplace(j.at("metervalue_from"));
    }
    if (j.contains("metervalue_to")) {
        k.metervalue_to.emplace(j.at("metervalue_to"));
    }
    if (j.contains("cost")) {
        k.cost.emplace(j.at("cost"));
    }
    if (j.contains("category")) {
        k.category.emplace(j.at("category"));
    }
}

void to_json(json& j, SessionStatus const& k) noexcept {
    switch (k) {
    case SessionStatus::Running:
        j = "Running";
        return;
    case SessionStatus::Idle:
        j = "Idle";
        return;
    case SessionStatus::Finished:
        j = "Finished";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::session_cost::SessionStatus";
}
void from_json(const json& j, SessionStatus& k) {
    std::string s = j;
    if (s == "Running") {
        k = SessionStatus::Running;
        return;
    }
    if (s == "Idle") {
        k = SessionStatus::Idle;
        return;
    }
    if (s == "Finished") {
        k = SessionStatus::Finished;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::session_cost::SessionStatus");
}

void to_json(json& j, SessionCost const& k) noexcept {
    j = json{
        {"session_id", k.session_id},
        {"currency", k.currency},
        {"status", k.status},
    };
    if (k.id_tag) {
        j["id_tag"] = k.id_tag.value();
    }
    if (k.cost_chunks) {
        if (j.size() == 0) {
            j = json{{"cost_chunks", json::array()}};
        } else {
            j["cost_chunks"] = json::array();
        }
        for (auto val : k.cost_chunks.value()) {
            j["cost_chunks"].push_back(val);
        }
    }
    if (k.idle_price) {
        j["idle_price"] = k.idle_price.value();
    }
    if (k.charging_price) {
        j["charging_price"] = k.charging_price.value();
    }
    if (k.next_period) {
        j["next_period"] = k.next_period.value();
    }
    if (k.message) {
        if (j.size() == 0) {
            j = json{{"message", json::array()}};
        } else {
            j["message"] = json::array();
        }
        for (auto val : k.message.value()) {
            j["message"].push_back(val);
        }
    }
    if (k.qr_code) {
        j["qr_code"] = k.qr_code.value();
    }
}
void from_json(const json& j, SessionCost& k) {
    using namespace auth;
    k.session_id = j.at("session_id");
    k.currency = j.at("currency");
    k.status = j.at("status");
    if (j.contains("id_tag")) {
        k.id_tag.emplace(j.at("id_tag"));
    }
    if (j.contains("cost_chunks")) {
        json arr = j.at("cost_chunks");
        std::vector<SessionCostChunk> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.cost_chunks.emplace(vec);
    }
    if (j.contains("idle_price")) {
        k.idle_price.emplace(j.at("idle_price"));
    }
    if (j.contains("charging_price")) {
        k.charging_price.emplace(j.at("charging_price"));
    }
    if (j.contains("next_period")) {
        k.next_period.emplace(j.at("next_period"));
    }
    if (j.contains("message")) {
        json arr = j.at("message");
        std::vector<text_message::MessageContent> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.message.emplace(vec);
    }
    if (j.contains("qr_code")) {
        k.qr_code.emplace(j.at("qr_code"));
    }
}

} // namespace everest::lib::API::V1_0::types::session_cost