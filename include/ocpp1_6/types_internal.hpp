// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_TYPES_INTERNAL_HPP
#define OCPP1_6_TYPES_INTERNAL_HPP

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>

namespace ocpp1_6 {
/// \brief Contains a CaseInsensitive string implementation that only allows printable ASCII characters
class CiString {
private:
    std::string data;
    size_t length;

public:
    /// \brief Creates a case insensitive string from the given \p data and maximum \p length
    CiString(const std::string& data, size_t length);

    /// \brief Creates a case insensitive string from the given maximum \p length
    CiString(size_t length);

    /// \brief A CiString without a maximum length is not allowed
    CiString() = delete;

    /// \brief Provides a std::string representation of the case insensitive string
    /// \returns a std::string
    std::string get() const;

    /// \brief Sets the content of the case insensitive string to the given \p data
    void set(const std::string& data);

    /// \brief Case insensitive compare for a case insensitive (Ci)String
    bool operator==(const char* c) const {
        return boost::iequals(this->data, c);
    }

    /// \brief Case insensitive compare for a case insensitive (Ci)String
    bool operator==(const CiString& s) {
        return boost::iequals(this->data, s.get());
    }

    /// \brief Case insensitive compare for a case insensitive (Ci)String
    bool operator!=(const char* c) const {
        return !(this->data == c);
    }

    /// \brief Case insensitive compare for a case insensitive (Ci)String
    bool operator!=(const CiString& s) {
        return !(this->data == s.get());
    }

    /// \brief Writes the given case insensitive string \p str to the given output stream \p os
    /// \returns an output stream with the case insensitive string written to
    friend std::ostream& operator<<(std::ostream& os, const CiString& str);

    /// \brief Conversion operator to turn a CiString into std::string
    operator std::string() {
        return this->data;
    }
};

/// \brief Contains a DateTime implementation that can parse and create RFC 3339 compatible strings
class DateTimeImpl {
private:
    std::chrono::time_point<std::chrono::system_clock> timepoint;

public:
    /// \brief Creates a new DateTimeImpl object with the current system time
    DateTimeImpl();

    /// \brief Creates a new DateTimeImpl object from the given \p timepoint
    DateTimeImpl(std::chrono::time_point<std::chrono::system_clock> timepoint);

    /// \brief Creates a new DateTimeImpl object from the given \p timepoint_str
    DateTimeImpl(const std::string& timepoint_str);

    /// \brief Converts this DateTimeImpl to a RFC 3339 compatible string
    /// \returns a RFC 3339 compatible string representation of the stored DateTime
    std::string to_rfc3339() const;

    /// \brief Converts this DateTimeImpl to a std::chrono::time_point
    /// \returns a std::chrono::time_point
    std::chrono::time_point<std::chrono::system_clock> to_time_point();

    /// \brief Writes the given DateTimeImpl \p dt to the given output stream \p os as a RFC 3339 compatible string
    /// \returns an output stream with the DateTimeImpl as a RFC 3339 compatible string written to
    friend std::ostream& operator<<(std::ostream& os, const DateTimeImpl& dt);
    operator std::string() {
        return this->to_rfc3339();
    }

    /// \brief Comparison operator> between two DateTimeImpl \p lhs and \p rhs
    friend bool operator>(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() > rhs.to_rfc3339();
    }

    /// \brief Comparison operator>= between two DateTimeImpl \p lhs and \p rhs
    friend bool operator>=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() >= rhs.to_rfc3339();
    }

    /// \brief Comparison operator< between two DateTimeImpl \p lhs and \p rhs
    friend bool operator<(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() < rhs.to_rfc3339();
    }

    /// \brief Comparison operator<= between two DateTimeImpl \p lhs and \p rhs
    friend bool operator<=(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() <= rhs.to_rfc3339();
    }

    /// \brief Comparison operator== between two DateTimeImpl \p lhs and \p rhs
    friend bool operator==(const DateTimeImpl& lhs, const DateTimeImpl& rhs) {
        return lhs.to_rfc3339() == rhs.to_rfc3339();
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_TYPES_INTERNAL_HPP
