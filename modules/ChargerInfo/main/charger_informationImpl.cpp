// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "charger_informationImpl.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace module {
namespace main {

void charger_informationImpl::init() {
}

void charger_informationImpl::ready() {
    json info(handle_get_charger_information());

    EVLOG_debug << "ChargerInformation: " << info.dump();
}

std::string charger_informationImpl::load_fw_version_from_file(const std::string& fn) {
    std::ifstream fw_version_file(fn);
    std::string first_line;

    if (!fw_version_file) {
        return "";
    }

    if (!std::getline(fw_version_file, first_line)) {
        return "";
    }

    return first_line;
}

types::charger_information::ChargerInformation charger_informationImpl::handle_get_charger_information() {
    std::vector<std::string> keys = {"vendor",        "model",          "chargepoint_serial", "chargebox_serial",
                                     "friendly_name", "manufacturer",   "manufacturer_url",   "model_url",
                                     "model_number",  "model_revision", "board_revision",     "firmware_version"};
    json info = {};

    // in case we are chained, retrieve data from previous module
    if (not this->mod->r_charger_information.empty()) {
        info = this->mod->r_charger_information[0]->call_get_charger_information();
    }

    // iterate over all linked key-value stores and merge all items,
    // when a key exists in more than one kvs then the last one wins
    for (const auto& kvs : mod->r_kvs) {
        for (const auto k : keys) {
            if (kvs->call_exists(k)) {
                const auto v = kvs->call_load(k);
                info[k] = std::get<std::string>(v);
            }
        }
    }

    // finally check whether we should load the firmware version from a simple plain text file
    if (!mod->config.firmware_version_file.empty()) {
        info["firmware_version"] = load_fw_version_from_file(mod->config.firmware_version_file);
    }

    // generate fallback friendly_name: we use the chargepoint's serial,
    // because we assume that this one is printed on a device label and not the (internal)
    // chargebox' serial number (aka controller serial number) which is usually only important for
    // the manufacturer itself
    if (info.contains("vendor") and info.contains("model") and info.contains("chargepoint_serial") and
        not info.contains("friendly_name")) {
        info["friendly_name"] = info["vendor"].get<std::string>() + " " + info["model"].get<std::string>() + " [" +
                                info["chargepoint_serial"].get<std::string>() + "]";
    }

    EVLOG_debug << "ChargerInformation: " << info.dump();

    return info;
}

} // namespace main
} // namespace module
