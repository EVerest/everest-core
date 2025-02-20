// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/v2/connectivity_manager.hpp>
#include <ocpp/v2/device_model.hpp>

namespace ocpp {
namespace v2 {

class MessageDispatcher : public MessageDispatcherInterface<MessageType> {

public:
    MessageDispatcher(ocpp::MessageQueue<MessageType>& message_queue, DeviceModel& device_model,
                      std::atomic<RegistrationStatusEnum>& registration_status) :
        message_queue(message_queue), device_model(device_model), registration_status(registration_status){};
    void dispatch_call(const json& call, bool triggered = false) override;
    std::future<ocpp::EnhancedMessage<MessageType>> dispatch_call_async(const json& call, bool triggered) override;
    void dispatch_call_result(const json& call_result) override;
    void dispatch_call_error(const json& call_error) override;

private:
    ocpp::MessageQueue<MessageType>& message_queue;
    DeviceModel& device_model;
    std::atomic<RegistrationStatusEnum>& registration_status;
};

} // namespace v2
} // namespace ocpp
