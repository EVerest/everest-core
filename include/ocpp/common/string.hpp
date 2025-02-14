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

class StringConversionException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum class StringTooLarge {
    Throw,
    Truncate
};

/// \brief Contains a String impementation with a maximum length
template <size_t L> class String {
private:
    std::string data;
    static constexpr size_t length = L;

public:
    /// \brief Creates a string from the given \p data
    explicit String(const std::string& data, StringTooLarge to_large = StringTooLarge::Throw) {
        this->set(data, to_large);
    }

    explicit String(const char* data, StringTooLarge to_large = StringTooLarge::Throw) {
        this->set(data, to_large);
    }

    /// \brief Creates a string
    String() = default;

    /// \brief Provides a std::string representation of the string
    /// \returns a std::string
    std::string get() const {
        return data;
    }

    /// \brief Sets the content of the string to the given \p data
    void set(const std::string& data, StringTooLarge to_large = StringTooLarge::Throw) {
        std::string_view view = data;
        if (view.length() > this->length) {
            if (to_large == StringTooLarge::Throw) {
                throw StringConversionException("String length (" + std::to_string(view.length()) +
                                                ") exceeds permitted length (" + std::to_string(this->length) + ")");
            }
            // Truncate
            view = view.substr(0, length);
        }

        if (this->is_valid(view)) {
            this->data = view;
        } else {
            throw StringConversionException("String has invalid format");
        }
    }

    /// \brief Override this to check for a specific format
    bool is_valid(std::string_view data) {
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
