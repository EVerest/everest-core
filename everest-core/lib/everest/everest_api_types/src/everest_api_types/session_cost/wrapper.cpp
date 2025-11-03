// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost/wrapper.hpp"
#include "auth/wrapper.hpp"
#include "display_message/wrapper.hpp"
#include "money/wrapper.hpp"
#include "session_cost/API.hpp"
#include "text_message/wrapper.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace session_cost;
using namespace auth;
using namespace display_message;
using namespace money;
using namespace text_message;
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_internal_api(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}
} // namespace

namespace session_cost {

TariffMessage_Internal to_internal_api(TariffMessage_External const& val) {
    TariffMessage_Internal result;
    result.messages = vecToInternal(val.messages);
    result.ocpp_transaction_id = val.ocpp_transaction_id;
    result.identifier_id = val.identifier_id;
    result.identifier_type = optToInternal(val.identifier_type);
    return result;
}

TariffMessage_External to_external_api(TariffMessage_Internal const& val) {
    TariffMessage_External result;
    result.messages = vecToExternal(val.messages);
    result.ocpp_transaction_id = val.ocpp_transaction_id;
    result.identifier_id = val.identifier_id;
    result.identifier_type = optToExternal(val.identifier_type);
    return result;
}

IdlePrice_Internal to_internal_api(IdlePrice_External const& val) {
    IdlePrice_Internal result;
    result.grace_minutes = val.grace_minutes;
    result.hour_price = optToInternal(val.hour_price);
    return result;
}

IdlePrice_External to_external_api(IdlePrice_Internal const& val) {
    IdlePrice_External result;
    result.grace_minutes = val.grace_minutes;
    result.hour_price = optToExternal(val.hour_price);
    return result;
}

CostCategory_Internal to_internal_api(CostCategory_External const& val) {
    using SrcT = CostCategory_External;
    using TarT = CostCategory_Internal;
    switch (val) {
    case SrcT::Energy:
        return TarT::Energy;
    case SrcT::Time:
        return TarT::Time;
    case SrcT::FlatFee:
        return TarT::FlatFee;
    case SrcT::Other:
        return TarT::Other;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::session_cost::CostCategory_External");
}

CostCategory_External to_external_api(CostCategory_Internal const& val) {
    using SrcT = CostCategory_Internal;
    using TarT = CostCategory_External;
    switch (val) {
    case SrcT::Energy:
        return TarT::Energy;
    case SrcT::Time:
        return TarT::Time;
    case SrcT::FlatFee:
        return TarT::FlatFee;
    case SrcT::Other:
        return TarT::Other;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::session_cost::CostCategory_Internal");
}

ChargingPriceComponent_Internal to_internal_api(ChargingPriceComponent_External const& val) {
    ChargingPriceComponent_Internal result;
    result.category = optToInternal(val.category);
    result.price = optToInternal(val.price);
    return result;
}

ChargingPriceComponent_External to_external_api(ChargingPriceComponent_Internal const& val) {
    ChargingPriceComponent_External result;
    result.category = optToExternal(val.category);
    result.price = optToExternal(val.price);
    return result;
}

NextPeriodPrice_Internal to_internal_api(NextPeriodPrice_External const& val) {
    NextPeriodPrice_Internal result;
    result.timestamp_from = val.timestamp_from;
    result.charging_price = vecToInternal(val.charging_price);
    result.idle_price = optToInternal(val.idle_price);
    return result;
}

NextPeriodPrice_External to_external_api(NextPeriodPrice_Internal const& val) {
    NextPeriodPrice_External result;
    result.timestamp_from = val.timestamp_from;
    result.charging_price = vecToExternal(val.charging_price);
    result.idle_price = optToExternal(val.idle_price);
    return result;
}

SessionCostChunk_Internal to_internal_api(SessionCostChunk_External const& val) {
    SessionCostChunk_Internal result;
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    result.metervalue_from = val.metervalue_from;
    result.metervalue_to = val.metervalue_to;
    result.cost = optToInternal(val.cost);
    result.category = optToInternal(val.category);
    return result;
}

SessionCostChunk_External to_external_api(SessionCostChunk_Internal const& val) {
    SessionCostChunk_External result;
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    result.metervalue_from = val.metervalue_from;
    result.metervalue_to = val.metervalue_to;
    if (val.cost) {
        result.cost = money::to_external_api(val.cost.value());
    }
    result.category = optToExternal(val.category);
    return result;
}

SessionStatus_Internal to_internal_api(SessionStatus_External const& val) {
    using SrcT = SessionStatus_External;
    using TarT = SessionStatus_Internal;
    switch (val) {
    case SrcT::Running:
        return TarT::Running;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::Finished:
        return TarT::Finished;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::session_cost::SessionStatus_External");
}

SessionStatus_External to_external_api(SessionStatus_Internal const& val) {
    using SrcT = SessionStatus_Internal;
    using TarT = SessionStatus_External;
    switch (val) {
    case SrcT::Running:
        return TarT::Running;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::Finished:
        return TarT::Finished;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::session_cost::SessionStatus_Internal");
}

SessionCost_Internal to_internal_api(SessionCost_External const& val) {
    SessionCost_Internal result;
    result.session_id = val.session_id;
    result.currency = to_internal_api(val.currency);
    result.status = to_internal_api(val.status);
    result.id_tag = optToInternal(val.id_tag);
    if (val.cost_chunks) {
        result.cost_chunks = vecToInternal(val.cost_chunks.value());
    }
    result.idle_price = optToInternal(val.idle_price);
    if (val.charging_price) {
        result.charging_price = vecToInternal(val.charging_price.value());
    }
    result.next_period = optToInternal(val.next_period);
    if (val.message) {
        result.message = vecToInternal(val.message.value());
    }
    result.qr_code = val.qr_code;
    return result;
}

SessionCost_External to_external_api(SessionCost_Internal const& val) {
    SessionCost_External result;
    result.session_id = val.session_id;
    result.currency = money::to_external_api(val.currency);
    result.status = to_external_api(val.status);
    result.id_tag = optToExternal(val.id_tag);
    if (val.cost_chunks) {
        result.cost_chunks = vecToExternal(val.cost_chunks.value());
    }
    result.idle_price = optToExternal(val.idle_price);
    if (val.charging_price) {
        result.charging_price = vecToExternal(val.charging_price.value());
    }
    result.next_period = optToExternal(val.next_period);
    if (val.message) {
        result.message = vecToExternal(val.message.value());
    }
    result.qr_code = val.qr_code;
    return result;
}
} // namespace session_cost
} // namespace everest::lib::API::V1_0::types
