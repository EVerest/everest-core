// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "display_message/wrapper.hpp"
#include "display_message/API.hpp"
#include "text_message/wrapper.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::display_message {

namespace {
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

MessagePriorityEnum_Internal to_internal_api(MessagePriorityEnum_External const& val) {
    using SrcT = MessagePriorityEnum_External;
    using TarT = MessagePriorityEnum_Internal;
    switch (val) {
    case SrcT::AlwaysFront:
        return TarT::AlwaysFront;
    case SrcT::InFront:
        return TarT::InFront;
    case SrcT::NormalCycle:
        return TarT::NormalCycle;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::MessagePriorityEnum_External");
}

MessagePriorityEnum_External to_external_api(MessagePriorityEnum_Internal const& val) {
    using SrcT = MessagePriorityEnum_Internal;
    using TarT = MessagePriorityEnum_External;
    switch (val) {
    case SrcT::AlwaysFront:
        return TarT::AlwaysFront;
    case SrcT::InFront:
        return TarT::InFront;
    case SrcT::NormalCycle:
        return TarT::NormalCycle;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::MessagePriorityEnum_Internal");
}

MessageStateEnum_Internal to_internal_api(MessageStateEnum_External const& val) {
    using SrcT = MessageStateEnum_External;
    using TarT = MessageStateEnum_Internal;
    switch (val) {
    case SrcT::Charging:
        return TarT::Charging;
    case SrcT::Faulted:
        return TarT::Faulted;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::Unavailable:
        return TarT::Unavailable;
    case SrcT::Suspending:
        return TarT::Suspending;
    case SrcT::Discharging:
        return TarT::Discharging;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::MessageStateEnum_External");
}

MessageStateEnum_External to_external_api(MessageStateEnum_Internal const& val) {
    using SrcT = MessageStateEnum_Internal;
    using TarT = MessageStateEnum_External;
    switch (val) {
    case SrcT::Charging:
        return TarT::Charging;
    case SrcT::Faulted:
        return TarT::Faulted;
    case SrcT::Idle:
        return TarT::Idle;
    case SrcT::Unavailable:
        return TarT::Unavailable;
    case SrcT::Suspending:
        return TarT::Suspending;
    case SrcT::Discharging:
        return TarT::Discharging;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::MessageStateEnum_Internal");
}

DisplayMessageStatusEnum_Internal to_internal_api(DisplayMessageStatusEnum_External const& val) {
    using SrcT = DisplayMessageStatusEnum_External;
    using TarT = DisplayMessageStatusEnum_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::NotSupportedMessageFormat:
        return TarT::NotSupportedMessageFormat;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::NotSupportedPriority:
        return TarT::NotSupportedPriority;
    case SrcT::NotSupportedState:
        return TarT::NotSupportedState;
    case SrcT::UnknownTransaction:
        return TarT::UnknownTransaction;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::DisplayMessageStatusEnum_External");
}

DisplayMessageStatusEnum_External to_external_api(DisplayMessageStatusEnum_Internal const& val) {
    using SrcT = DisplayMessageStatusEnum_Internal;
    using TarT = DisplayMessageStatusEnum_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::NotSupportedMessageFormat:
        return TarT::NotSupportedMessageFormat;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::NotSupportedPriority:
        return TarT::NotSupportedPriority;
    case SrcT::NotSupportedState:
        return TarT::NotSupportedState;
    case SrcT::UnknownTransaction:
        return TarT::UnknownTransaction;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::DisplayMessageStatusEnum_Internal");
}

ClearMessageResponseEnum_Internal to_internal_api(ClearMessageResponseEnum_External const& val) {
    using SrcT = ClearMessageResponseEnum_External;
    using TarT = ClearMessageResponseEnum_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::ClearMessageResponseEnum_External");
}

ClearMessageResponseEnum_External to_external_api(ClearMessageResponseEnum_Internal const& val) {
    using SrcT = ClearMessageResponseEnum_Internal;
    using TarT = ClearMessageResponseEnum_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::ClearMessageResponseEnum_Internal");
}

Identifier_type_Internal to_internal_api(Identifier_type_External const& val) {
    using SrcT = Identifier_type_External;
    using TarT = Identifier_type_Internal;
    switch (val) {
    case SrcT::IdToken:
        return TarT::IdToken;
    case SrcT::SessionId:
        return TarT::SessionId;
    case SrcT::TransactionId:
        return TarT::TransactionId;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::Identifier_type_External");
}

Identifier_type_External to_external_api(Identifier_type_Internal const& val) {
    using SrcT = Identifier_type_Internal;
    using TarT = Identifier_type_External;
    switch (val) {
    case SrcT::IdToken:
        return TarT::IdToken;
    case SrcT::SessionId:
        return TarT::SessionId;
    case SrcT::TransactionId:
        return TarT::TransactionId;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::display_message::Identifier_type_Internal");
}

DisplayMessage_Internal to_internal_api(DisplayMessage_External const& val) {
    DisplayMessage_Internal result;
    result.message = text_message::to_internal_api(val.message);
    result.id = val.id;
    result.priority = optToInternal(val.priority);
    result.state = optToInternal(val.state);
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    result.identifier_id = val.identifier_id;
    result.identifier_type = optToInternal(val.identifier_type);
    result.qr_code = val.qr_code;
    return result;
}

DisplayMessage_External to_external_api(DisplayMessage_Internal const& val) {
    DisplayMessage_External result;
    result.message = text_message::to_external_api(val.message);
    result.id = val.id;
    result.priority = optToExternal(val.priority);
    result.state = optToExternal(val.state);
    result.timestamp_from = val.timestamp_from;
    result.timestamp_to = val.timestamp_to;
    result.identifier_id = val.identifier_id;
    result.identifier_type = optToExternal(val.identifier_type);
    result.qr_code = val.qr_code;
    return result;
}

SetDisplayMessageResponse_Internal to_internal_api(SetDisplayMessageResponse_External const& val) {
    SetDisplayMessageResponse_Internal result;
    result.status = to_internal_api(val.status);
    result.status_info = val.status_info;
    return result;
}

SetDisplayMessageResponse_External to_external_api(SetDisplayMessageResponse_Internal const& val) {
    SetDisplayMessageResponse_External result;
    result.status = to_external_api(val.status);
    result.status_info = val.status_info;
    return result;
}

GetDisplayMessageRequest_Internal to_internal_api(GetDisplayMessageRequest_External const& val) {
    GetDisplayMessageRequest_Internal result;
    if (val.id) {
        result.id = val.id.value();
    }
    result.priority = optToInternal(val.priority);
    result.state = optToInternal(val.state);
    return result;
}

GetDisplayMessageRequest_External to_external_api(GetDisplayMessageRequest_Internal const& val) {
    GetDisplayMessageRequest_External result;
    if (val.id) {
        result.id = val.id.value();
    }
    result.priority = optToExternal(val.priority);
    result.state = optToExternal(val.state);
    return result;
}

GetDisplayMessageResponse_Internal to_internal_api(GetDisplayMessageResponse_External const& val) {
    GetDisplayMessageResponse_Internal result;
    result.status_info = val.status_info;
    if (val.messages) {
        result.messages = vecToInternal(val.messages.value());
    }
    return result;
}

GetDisplayMessageResponse_External to_external_api(GetDisplayMessageResponse_Internal const& val) {
    GetDisplayMessageResponse_External result;
    result.status_info = val.status_info;
    if (val.messages) {
        result.messages = vecToExternal(val.messages.value());
    }
    return result;
}

ClearDisplayMessageRequest_Internal to_internal_api(ClearDisplayMessageRequest_External const& val) {
    ClearDisplayMessageRequest_Internal result;
    result.id = val.id;
    return result;
}

ClearDisplayMessageRequest_External to_external_api(ClearDisplayMessageRequest_Internal const& val) {
    ClearDisplayMessageRequest_External result;
    result.id = val.id;
    return result;
}

ClearDisplayMessageResponse_Internal to_internal_api(ClearDisplayMessageResponse_External const& val) {
    ClearDisplayMessageResponse_Internal result;
    result.status = to_internal_api(val.status);
    result.status_info = val.status_info;
    return result;
}

ClearDisplayMessageResponse_External to_external_api(ClearDisplayMessageResponse_Internal const& val) {
    ClearDisplayMessageResponse_External result;
    result.status = to_external_api(val.status);
    result.status_info = val.status_info;
    return result;
}

} // namespace everest::lib::API::V1_0::types::display_message
