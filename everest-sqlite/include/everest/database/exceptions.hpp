// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <exception>
#include <string>

namespace everest::db {

/// \brief Base class for database-related exceptions
class Exception : public std::exception {
public:
    explicit Exception(const std::string& message) : msg(message) {
    }
    virtual ~Exception() noexcept {
    }

    virtual const char* what() const noexcept override {
        return msg.c_str();
    }

protected:
    std::string msg;
};

/// \brief Exception for database connection errors
class ConnectionException : public Exception {
public:
    explicit ConnectionException(const std::string& message) : Exception(message) {
    }
};

/// \brief Exception that is used if expected table entries are not found
class RequiredEntryNotFoundException : public Exception {
public:
    explicit RequiredEntryNotFoundException(const std::string& message) : Exception(message) {
    }
};

/// \brief Exception for errors during database migration
class MigrationException : public Exception {
public:
    explicit MigrationException(const std::string& message) : Exception(message) {
    }
};

/// \brief Exception for errors during query execution
class QueryExecutionException : public Exception {
public:
    explicit QueryExecutionException(const std::string& message) : Exception(message) {
    }
};

} // namespace everest::db
