// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "v2gMessage.hpp"

#include <chrono>
#include <date/date.h>
#include <date/tz.h>

#include <fmt/core.h>

namespace module {

v2g_message::v2g_message() {
}

bool v2g_message::from_xml(const std::string& xml) {
    return doc.load_string(xml.c_str());
}

std::string v2g_message::to_xml() {
    std::stringstream ss;
    doc.save(ss);
    return ss.str();
}

void v2g_message::from_json(const std::string& json_str) {
    j = nlohmann::json::parse(json_str);
}

std::string v2g_message::to_json() {
    return j.dump(4);
}

} // namespace module
