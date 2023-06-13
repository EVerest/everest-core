// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/call_types.hpp>

namespace ocpp {

bool operator<(const MessageId& lhs, const MessageId& rhs) {
    return lhs.get() < rhs.get();
}

void to_json(json& j, const MessageId& k) {
    j = json(k.get());
}

void from_json(const json& j, MessageId& k) {
    k.set(j);
}

} // namespace ocpp
