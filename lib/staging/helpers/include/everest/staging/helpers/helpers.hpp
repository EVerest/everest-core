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

} // namespace everest::staging::helpers

#endif
