// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_STAGING_HELPERS_HPP
#define EVEREST_STAGING_HELPERS_HPP

#include <string>

namespace types::authorization {
struct ProvidedIdToken;
}

namespace everest::staging::helpers {

/// \brief Redacts a provided \p token by hashing it
/// \returns a hashed version of the provided token
std::string redact(const std::string& token);

types::authorization::ProvidedIdToken redact(const types::authorization::ProvidedIdToken& token);

/// \brief Provide a UUID
/// \returns a UUID string. This UUID is 36 characters long
std::string get_uuid();

/// \brief Provide a base64 encoded UUID
/// \returns a base64 encoded UUID string. This UUID is 22 characters long
std::string get_base64_uuid();

/// \brief Provide a base64 encoded ID
/// \returns a base64 encoded ID string. This ID is 16 characters long
std::string get_base64_id();

} // namespace everest::staging::helpers

#endif
