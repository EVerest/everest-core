// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include "context.hpp"

namespace ieee2030::charger::v20 {

class Context;

enum class Event {
    CAN_MESSAGE,
    HW_SIGNAL,
    EVENT,
    TIMEOUT,
};

enum class StateID {
    StateB,
    StateC
};

struct Result {
    constexpr Result() = default;
    Result(BasePointerType result_state) : unhandled(false), new_state(std::move(result_state)) {
    }

    bool unhandled{true};
    BasePointerType new_state{nullptr};
};

struct StateBase {
    using ContainerType = BasePointerType;
    using EventType = Event;

    StateBase(Context& ctx, StateID id) : m_ctx(ctx), m_id(id){};

    virtual ~StateBase() = default;

    StateID get_id() const {
        return m_id;
    }

    virtual void enter(){};
    virtual Result feed(Event) = 0;
    virtual void leave(){};

protected:
    Context& m_ctx;

private:
    StateID m_id;
};

} // namespace ieee2030::charger::v20
