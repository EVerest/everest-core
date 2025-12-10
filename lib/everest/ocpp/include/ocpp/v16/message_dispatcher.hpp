// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>

namespace ocpp {
namespace v16 {

class MessageDispatcher : public MessageDispatcherInterface<MessageType> {

public:
    MessageDispatcher(ocpp::MessageQueue<MessageType>& message_queue, ChargePointConfiguration& configuration,
                      std::atomic<RegistrationStatus>& registration_status) :
        message_queue(message_queue), configuration(configuration), registration_status(registration_status){};
    void dispatch_call(const json& call, bool triggered = false) override;
    std::future<ocpp::EnhancedMessage<MessageType>> dispatch_call_async(const json& call, bool triggered) override;
    void dispatch_call_result(const json& call_result) override;
    void dispatch_call_error(const json& call_error) override;

private:
    ocpp::MessageQueue<MessageType>& message_queue;
    ChargePointConfiguration& configuration;
    std::atomic<RegistrationStatus>& registration_status;
};

} // namespace v16
} // namespace ocpp
