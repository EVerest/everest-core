// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "utilities/Topics.hpp"
#include <iostream>
#include <sstream>

namespace everest::lib::API {

Topics::Topics(const std::string& target_module_id_) : target_module_id(target_module_id_) {
}

void Topics::setTargetApiModuleID(const std::string& target_module_id_, const std::string& api_type_) {
    target_module_id = target_module_id_;
    api_type = api_type_;
}

std::string Topics::everest_to_extern(const std::string& var) const {
    std::stringstream topic;
    topic << api_base << "/" << api_version << "/" << api_type << "/" << target_module_id << "/" << api_out << "/"
          << var;
    return topic.str();
}

std::string Topics::extern_to_everest(const std::string& var) const {
    std::stringstream topic;
    topic << api_base << "/" << api_version << "/" << api_type << "/" << target_module_id << "/" << api_in << "/"
          << var;
    return topic.str();
}

std::string Topics::reply_to_everest(const std::string& reply) const {
    std::stringstream topic;
    topic << api_base << "/" << api_version << "/" << api_type << "/" << target_module_id << "/" << api_in << "/reply/"
          << reply;
    return topic.str();
}

std::string Topics::cloud_connector(const std::string_view& var) {
    std::stringstream topic;
    topic << cloud_base << "/" << var;
    return topic.str();
}

const std::string Topics::api_base = "everest_api";
const std::string Topics::api_version = "1.0";
const std::string Topics::api_out = "e2m";
const std::string Topics::api_in = "m2e";
const std::string Topics::cloud_base = "everest/cloud";

} // namespace everest::lib::API
