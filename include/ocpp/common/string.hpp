// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_STRING_HPP
#define OCPP_COMMON_STRING_HPP

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ocpp {

/// \brief Contains a String impementation with a maximum length
template <size_t L> class String {
private:
    std::string data;
    size_t length;

public:
    /// \brief Creates a string from the given \p data
    String(const std::string& data) : length(L) {
        this->set(data);
    }

    String(const char* data) : length(L) {
        this->set(data);
    }

    /// \brief Creates a string
    String() : length(L) {
    }

    /// \brief Provides a std::string representation of the string
    /// \returns a std::string
    std::string get() const {
        return data;
    }

    /// \brief Sets the content of the string to the given \p data
    void set(const std::string& data) {
        if (data.length() <= this->length) {
            if (this->is_valid(data)) {
                this->data = data;
            } else {
                throw std::runtime_error("String has invalid format");
            }
        } else {
            throw std::runtime_error("String length (" + std::to_string(data.length()) +
                                     ") exceeds permitted length (" + std::to_string(this->length) + ")");
        }
    }

    /// \brief Override this to check for a specific format
    bool is_valid(const std::string& data) {
        (void)data; // not needed here
        return true;
    }

    /// \brief Conversion operator to turn a String into std::string
    operator std::string() {
        return this->get();
    }
};

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator==(const String<L>& lhs, const char* rhs) {
    return lhs.get() == rhs;
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator==(const String<L>& lhs, const String<L>& rhs) {
    return lhs.get() == rhs.get();
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator!=(const String<L>& lhs, const char* rhs) {
    return !(lhs.get() == rhs);
}

/// \brief Case insensitive compare for a case insensitive (Ci)String
template <size_t L> bool operator!=(const String<L>& lhs, const String<L>& rhs) {
    return !(lhs.get() == rhs.get());
}

/// \brief Writes the given string \p str to the given output stream \p os
/// \returns an output stream with the case insensitive string written to
template <size_t L> std::ostream& operator<<(std::ostream& os, const String<L>& str) {
    os << str.get();
    return os;
}

} // namespace ocpp

#endif
