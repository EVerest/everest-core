// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include "gmock/gmock.h"

#include <ocpp/common/message_dispatcher.hpp>

using namespace ocpp::v201;

class MockMessageDispatcher : public ocpp::MessageDispatcherInterface<MessageType> {
public:
    MOCK_METHOD(void, dispatch_call, (const json& call, bool triggered), (override));
    MOCK_METHOD(std::future<ocpp::EnhancedMessage<MessageType>>, dispatch_call_async,
                (const json& call, bool triggered), (override));
    MOCK_METHOD(void, dispatch_call_result, (const json& call_result), (override));
    MOCK_METHOD(void, dispatch_call_error, (const json& call_error), (override));
};
