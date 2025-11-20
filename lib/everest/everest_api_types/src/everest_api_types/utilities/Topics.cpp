// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "utilities/Topics.hpp"
#include <iostream>
#include <sstream>

namespace everest::lib::API {

void Topics::setup(std::string const& target_module_id, std::string const& api_type, unsigned int version) {
    m_target_module_id = target_module_id;
    m_api_type = api_type;
    m_api_version = std::to_string(version);
}

std::string Topics::everest_to_extern(const std::string& var) const {
    std::stringstream topic;
    topic << api_base << "/" << m_api_version << "/" << m_api_type << "/" << m_target_module_id << "/" << api_out << "/"
          << var;
    return topic.str();
}

std::string Topics::extern_to_everest(const std::string& var) const {
    std::stringstream topic;
    topic << api_base << "/" << m_api_version << "/" << m_api_type << "/" << m_target_module_id << "/" << api_in << "/"
          << var;
    return topic.str();
}

std::string Topics::reply_to_everest(const std::string& reply) const {
    std::stringstream topic;
    topic << api_base << "/" << m_api_version << "/" << m_api_type << "/" << m_target_module_id << "/" << api_in
          << "/reply/" << reply;
    return topic.str();
}

const std::string Topics::api_base = "everest_api";
const std::string Topics::api_out = "e2m";
const std::string Topics::api_in = "m2e";

} // namespace everest::lib::API
