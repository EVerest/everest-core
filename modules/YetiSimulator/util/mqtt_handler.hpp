// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "error_handler.hpp"
#include "generated/interfaces/ac_rcd/Implementation.hpp"
#include "generated/interfaces/connector_lock/Implementation.hpp"
#include "generated/interfaces/evse_board_support/Implementation.hpp"
#include <string>
namespace module {

class MqttHandler {
public:
    MqttHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                connector_lockImplBase* p_connector_lock);
    ;

    void handle_mqtt_payload(const std::string& payload);

private:
    ErrorHandler errorHandler;
};

} // namespace module
