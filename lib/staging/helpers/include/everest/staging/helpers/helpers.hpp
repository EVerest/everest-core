// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef EVEREST_STAGING_HELPERS_HPP
#define EVEREST_STAGING_HELPERS_HPP

#include <algorithm>
#include <cctype>
#include <string>

namespace types::authorization {
struct ProvidedIdToken;
struct IdToken;
} // namespace types::authorization

namespace everest::staging::helpers {

/// \brief Redacts a provided \p token by hashing it
/// \returns a hashed version of the provided token
std::string redact(const std::string& token);

types::authorization::ProvidedIdToken redact(const types::authorization::ProvidedIdToken& token);

/// \brief Compares two strings case-insensitively
/// \returns true if the strings are equal, false otherwise
bool is_equal_case_insensitive(const std::string& str1, const std::string& str2);

/// \brief Compares two IdTokens case-insensitively
/// \returns true if the IdTokens are equal, false otherwise
/// \note This function compares the value and type of the IdTokens
bool is_equal_case_insensitive(const types::authorization::IdToken& token1,
                               const types::authorization::IdToken& token2);

/// \brief Compares two ProvidedIdTokens case-insensitively
/// \returns true if the ProvidedIdTokens are equal, false otherwise
/// \note This function compares the id_token and type of the ProvidedIdTokens
bool is_equal_case_insensitive(const types::authorization::ProvidedIdToken& token1,
                               const types::authorization::ProvidedIdToken& token2);

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
