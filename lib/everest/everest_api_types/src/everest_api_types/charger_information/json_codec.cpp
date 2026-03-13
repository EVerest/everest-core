// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "charger_information/json_codec.hpp"
#include "charger_information/API.hpp"
#include "charger_information/codec.hpp"

#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::charger_information {

void to_json(json& j, ChargerInformation const& k) noexcept {
    j = json{
        {"vendor", k.vendor},
        {"model", k.model},
    };

    if (k.chargepoint_serial) {
        j["chargepoint_serial"] = k.chargepoint_serial.value();
    }
    if (k.chargebox_serial) {
        j["chargebox_serial"] = k.chargebox_serial.value();
    }
    if (k.friendly_name) {
        j["friendly_name"] = k.friendly_name.value();
    }
    if (k.manufacturer) {
        j["manufacturer"] = k.manufacturer.value();
    }
    if (k.manufacturer_url) {
        j["manufacturer_url"] = k.manufacturer_url.value();
    }
    if (k.model_url) {
        j["model_url"] = k.model_url.value();
    }
    if (k.model_number) {
        j["model_number"] = k.model_number.value();
    }
    if (k.model_revision) {
        j["model_revision"] = k.model_revision.value();
    }
    if (k.board_revision) {
        j["board_revision"] = k.board_revision.value();
    }
    if (k.firmware_version) {
        j["firmware_version"] = k.firmware_version.value();
    }
}

void from_json(const json& j, ChargerInformation& k) {
    k.vendor = j.at("vendor");
    k.model = j.at("model");

    if (j.contains("chargepoint_serial")) {
        k.chargepoint_serial.emplace(j.at("chargepoint_serial"));
    }
    if (j.contains("chargebox_serial")) {
        k.chargebox_serial.emplace(j.at("chargebox_serial"));
    }
    if (j.contains("friendly_name")) {
        k.friendly_name.emplace(j.at("friendly_name"));
    }
    if (j.contains("manufacturer")) {
        k.manufacturer.emplace(j.at("manufacturer"));
    }
    if (j.contains("manufacturer_url")) {
        k.manufacturer_url.emplace(j.at("manufacturer_url"));
    }
    if (j.contains("model_url")) {
        k.model_url.emplace(j.at("model_url"));
    }
    if (j.contains("model_number")) {
        k.model_number.emplace(j.at("model_number"));
    }
    if (j.contains("model_revision")) {
        k.model_revision.emplace(j.at("model_revision"));
    }
    if (j.contains("board_revision")) {
        k.board_revision.emplace(j.at("board_revision"));
    }
    if (j.contains("firmware_version")) {
        k.firmware_version.emplace(j.at("firmware_version"));
    }
}

} // namespace everest::lib::API::V1_0::types::charger_information
