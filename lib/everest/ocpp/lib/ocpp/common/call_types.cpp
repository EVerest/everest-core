// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/call_types.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ocpp {

MessageId create_message_id() {
    static boost::uuids::random_generator uuid_generator;
    const boost::uuids::uuid uuid = uuid_generator();
    std::stringstream s;
    s << uuid;
    return MessageId(s.str());
}

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
