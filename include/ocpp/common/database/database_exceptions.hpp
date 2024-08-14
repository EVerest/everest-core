// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <exception>
#include <string>

namespace ocpp::common {

/// \brief Base class for database-related exceptions
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(const std::string& message) : msg(message) {
    }
    virtual ~DatabaseException() noexcept {
    }

    virtual const char* what() const noexcept override {
        return msg.c_str();
    }

protected:
    std::string msg;
};

/// \brief Exception for database connection errors
class DatabaseConnectionException : public DatabaseException {
public:
    explicit DatabaseConnectionException(const std::string& message) : DatabaseException(message) {
    }
};

/// \brief Exception that is used if expected table entries are not found
class RequiredEntryNotFoundException : public DatabaseException {
public:
    explicit RequiredEntryNotFoundException(const std::string& message) : DatabaseException(message) {
    }
};

/// \brief Exception for errors during database migration
class DatabaseMigrationException : public DatabaseException {
public:
    explicit DatabaseMigrationException(const std::string& message) : DatabaseException(message) {
    }
};

/// \brief Exception for errors during query execution
class QueryExecutionException : public DatabaseException {
public:
    explicit QueryExecutionException(const std::string& message) : DatabaseException(message) {
    }
};

} // namespace ocpp::common