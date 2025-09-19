// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "text_message/wrapper.hpp"
#include <optional>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace text_message;
template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    if (src) {
        return std::make_optional(to_internal_api(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    if (src) {
        return std::make_optional(to_external_api(src.value()));
    }
    return std::nullopt;
}
} // namespace

namespace text_message {

MessageFormat_Internal to_internal_api(MessageFormat_External const& val) {
    using SrcT = MessageFormat_External;
    using TarT = MessageFormat_Internal;
    switch (val) {
    case SrcT::ASCII:
        return TarT::ASCII;
    case SrcT::HTML:
        return TarT::HTML;
    case SrcT::URI:
        return TarT::URI;
    case SrcT::UTF8:
        return TarT::UTF8;
    case SrcT::QRCODE:
        return TarT::QRCODE;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::text_message::MessageFormat_External");
}

MessageFormat_External to_external_api(MessageFormat_Internal const& val) {
    using SrcT = MessageFormat_Internal;
    using TarT = MessageFormat_External;
    switch (val) {
    case SrcT::ASCII:
        return TarT::ASCII;
    case SrcT::HTML:
        return TarT::HTML;
    case SrcT::URI:
        return TarT::URI;
    case SrcT::UTF8:
        return TarT::UTF8;
    case SrcT::QRCODE:
        return TarT::QRCODE;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::text_message::MessageFormat_Internal");
}

MessageContent_Internal to_internal_api(MessageContent_External const& val) {
    MessageContent_Internal result;
    result.content = val.content;
    result.format = optToInternal(val.format);
    result.language = val.language;
    return result;
}

MessageContent_External to_external_api(MessageContent_Internal const& val) {
    MessageContent_External result;
    result.content = val.content;
    result.format = optToExternal(val.format);
    result.language = val.language;
    return result;
}

} // namespace text_message
} // namespace everest::lib::API::V1_0::types
