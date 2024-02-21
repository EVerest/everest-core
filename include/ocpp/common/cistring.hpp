// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_CISTRING_HPP
#define OCPP_COMMON_CISTRING_HPP

#include <nlohmann/json.hpp>

#include <ocpp/common/string.hpp>
#include <ocpp/common/utils.hpp>

using json = nlohmann::json;

namespace ocpp {

/// \brief Contains a CaseInsensitive string implementation that only allows printable ASCII characters
template <size_t L> class CiString : public String<L> {

public:
    /// \brief Creates a string from the given \p data
    CiString(const std::string& data) : String<L>(data) {
    }

    CiString(const char* data) : String<L>(data) {
    }

    CiString(const CiString<L>& data) : String<L>(data.get()) {
    }

    /// \brief Creates a string
    CiString() : String<L>() {
    }

    /// \brief CaseInsensitive string implementation only allows printable ASCII characters
    bool is_valid(const std::string& data) {
        for (const char& character : data) {
            // printable ASCII starts at code 0x20 (space) and ends with code 0x7e (tilde) and 0xa (\n)
            if ((character < 0x20 || character > 0x7e) && character != 0xa) {
                throw std::runtime_error("CiString can only contain printable ASCII characters");
            }
        }
        return true;
    }

    /// \brief Conversion operator to turn a String into std::string
    operator std::string() const {
        return this->get();
    }
};

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator==(const CiString<L>& lhs, const char* rhs) {
    return iequals(lhs.get(), rhs);
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator==(const CiString<L>& lhs, const CiString<L>& rhs) {
    return iequals(lhs.get(), rhs.get());
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator!=(const CiString<L>& lhs, const char* rhs) {
    return !(lhs.get() == rhs);
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator!=(const CiString<L>& lhs, const CiString<L>& rhs) {
    return !(lhs.get() == rhs.get());
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator<(const CiString<L>& lhs, const CiString<L>& rhs) {
    return lhs.get() < rhs.get();
}

/// \brief Writes the given string \p str to the given output stream \p os
/// \returns an output stream with the case insensitive string written to
template <size_t L> std::ostream& operator<<(std::ostream& os, const CiString<L>& str) {
    os << str.get();
    return os;
}

template <size_t L> void to_json(json& j, const CiString<L>& k) {
    j = json(k.get());
}

template <size_t L> void from_json(const json& j, CiString<L>& k) {
    k.set(j);
}

} // namespace ocpp

#endif
