// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/staging/helpers/helpers.hpp>

#include <unordered_map>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <fmt/format.h>

#include <generated/types/authorization.hpp>

namespace everest::staging::helpers {
std::string redact(const std::string& token) {
    auto hash = std::hash<std::string>{}(token);
    return fmt::format("[redacted] hash: {:X}", hash);
}

types::authorization::ProvidedIdToken redact(const types::authorization::ProvidedIdToken& token) {
    types::authorization::ProvidedIdToken redacted_token = token;
    redacted_token.id_token.value = redact(redacted_token.id_token.value);
    if (redacted_token.parent_id_token.has_value()) {
        auto& parent_id_token = redacted_token.parent_id_token.value();
        parent_id_token.value = redact(parent_id_token.value);
    }
    return redacted_token;
}

std::string get_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}
} // namespace everest::staging::helpers
