// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/display_message/API.hpp>
#include <everest_api_types/money/API.hpp>
#include <everest_api_types/text_message/API.hpp>
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::session_cost {

struct TariffMessage {
    std::vector<text_message::MessageContent> messages;
    std::optional<std::string> ocpp_transaction_id;
    std::optional<std::string> identifier_id;
    std::optional<display_message::Identifier_type> identifier_type;
};

struct IdlePrice {
    std::optional<int32_t> grace_minutes;
    std::optional<money::Price> hour_price;
};

enum class CostCategory {
    Energy,
    Time,
    FlatFee,
    Other,
};

struct ChargingPriceComponent {
    std::optional<CostCategory> category;
    std::optional<money::Price> price;
};

struct NextPeriodPrice {
    std::string timestamp_from;
    std::vector<ChargingPriceComponent> charging_price;
    std::optional<IdlePrice> idle_price;
};

struct SessionCostChunk {
    std::optional<std::string> timestamp_from;
    std::optional<std::string> timestamp_to;
    std::optional<int32_t> metervalue_from;
    std::optional<int32_t> metervalue_to;
    std::optional<money::MoneyAmount> cost;
    std::optional<CostCategory> category;
};

enum class SessionStatus {
    Running,
    Idle,
    Finished,
};

struct SessionCost {
    std::string session_id;
    money::Currency currency;
    SessionStatus status;
    std::optional<auth::ProvidedIdToken> id_tag;
    std::optional<std::vector<SessionCostChunk>> cost_chunks;
    std::optional<IdlePrice> idle_price;
    std::optional<std::vector<ChargingPriceComponent>> charging_price;
    std::optional<NextPeriodPrice> next_period;
    std::optional<std::vector<text_message::MessageContent>> message;
    std::optional<std::string> qr_code;
};

} // namespace everest::lib::API::V1_0::types::session_cost
