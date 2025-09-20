// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "text_message/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "text_message/API.hpp"
#include "text_message/codec.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::text_message {

void to_json(json& j, MessageFormat const& k) noexcept {
    switch (k) {
    case MessageFormat::ASCII:
        j = "ASCII";
        return;
    case MessageFormat::HTML:
        j = "HTML";
        return;
    case MessageFormat::URI:
        j = "URI";
        return;
    case MessageFormat::UTF8:
        j = "UTF8";
        return;
    case MessageFormat::QRCODE:
        j = "QRCODE";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::text_message::MessageFormat";
}

void from_json(const json& j, MessageFormat& k) {
    std::string s = j;
    if (s == "ASCII") {
        k = MessageFormat::ASCII;
        return;
    }
    if (s == "HTML") {
        k = MessageFormat::HTML;
        return;
    }
    if (s == "URI") {
        k = MessageFormat::URI;
        return;
    }
    if (s == "UTF8") {
        k = MessageFormat::UTF8;
        return;
    }
    if (s == "QRCODE") {
        k = MessageFormat::QRCODE;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::text_message::MessageFormat");
}

void to_json(json& j, MessageContent const& k) noexcept {
    j = json{
        {"content", k.content},
    };
    if (k.format) {
        j["format"] = k.format.value();
    }
    if (k.language) {
        j["language"] = k.language.value();
    }
}
void from_json(const json& j, MessageContent& k) {
    k.content = j.at("content");
    if (j.contains("format")) {
        k.format.emplace(j.at("format"));
    }
    if (j.contains("language")) {
        k.language.emplace(j.at("language"));
    }
}

} // namespace everest::lib::API::V1_0::types::text_message
