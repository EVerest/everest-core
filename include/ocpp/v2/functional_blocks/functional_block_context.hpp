// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/v2/types.hpp>

namespace ocpp {
class EvseSecurity;

namespace v2 {
class DeviceModel;
class ConnectivityManagerInterface;
class EvseManagerInterface;
class DatabaseHandlerInterface;
class ComponentStateManagerInterface;

/// \brief Context / requirements for the functional blocks.
///
/// All functional blocks will get this context. The references and pointers in this struct are used by all or most
/// functional blocks.
struct FunctionalBlockContext {
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    ConnectivityManagerInterface& connectivity_manager;
    EvseManagerInterface& evse_manager;
    DatabaseHandlerInterface& database_handler;
    EvseSecurity& evse_security;
    ComponentStateManagerInterface& component_state_manager;

    FunctionalBlockContext(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                           ConnectivityManagerInterface& connectivity_manager, EvseManagerInterface& evse_manager,
                           DatabaseHandlerInterface& database_handler, EvseSecurity& evse_security,
                           ComponentStateManagerInterface& component_state_manager) :
        message_dispatcher(message_dispatcher),
        device_model(device_model),
        connectivity_manager(connectivity_manager),
        evse_manager(evse_manager),
        database_handler(database_handler),
        evse_security(evse_security),
        component_state_manager(component_state_manager) {
    }
};
} // namespace v2
} // namespace ocpp
