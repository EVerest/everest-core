// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "config_writer.hpp"
#include "generated/types/ocpp.hpp"
#include "mapping_reader.hpp"

namespace module {

class EventHandler {
public:
    EventHandler(const std::string& config_mapping_path, const std::string& user_config_path);
    void handleEvent(const types::ocpp::EventData& event_data);

private:
    const std::optional<EverestModuleMapping> find_event_in_map_or_log_error(const std::string& event_name) const;
    const EverestModuleMapping find_event_in_map(const std::string& event_name) const;

    OcppToEverestModuleMapping event_map;
    ConfigWriter config_writer;
};

} // namespace module
