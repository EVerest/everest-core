// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <stdexcept>

namespace Everest {
struct BootException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class CmdError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class MessageParsingError : public CmdError {
public:
    using CmdError::CmdError;
};

class SchemaValidationError : public CmdError {
public:
    using CmdError::CmdError;
};

class HandlerException : public CmdError {
public:
    using CmdError::CmdError;
};

class CmdTimeout : public CmdError {
public:
    using CmdError::CmdError;
};

class Shutdown : public CmdError {
public:
    using CmdError::CmdError;
};

class NotReady : public CmdError {
public:
    using CmdError::CmdError;
};
} // namespace Everest
