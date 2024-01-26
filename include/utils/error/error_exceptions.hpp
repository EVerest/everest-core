// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_EXCEPTIONS_HPP
#define UTILS_ERROR_EXCEPTIONS_HPP

#include <everest/exceptions.hpp>
#include <string>

#include <utils/error.hpp>
namespace Everest {
namespace error {

class EverestNotValidErrorTypeError : public EverestBaseRuntimeError {
public:
    explicit EverestNotValidErrorTypeError(const ErrorType& error_type_) :
        EverestBaseRuntimeError("Error type " + error_type_ + " is not valid."){};
};

class EverestAlreadyExistsError : public EverestBaseLogicError {
public:
    using EverestBaseLogicError::EverestBaseLogicError;
};

class EverestNotAllowedError : public EverestBaseLogicError {
public:
    using EverestBaseLogicError::EverestBaseLogicError;
};

class EverestInterfaceMissingDeclartionError : public EverestBaseLogicError {
public:
    using EverestBaseLogicError::EverestBaseLogicError;
};

class EverestDirectoryNotFoundError : public EverestBaseRuntimeError {
public:
    using EverestBaseRuntimeError::EverestBaseRuntimeError;
};

class EverestParseError : public EverestBaseRuntimeError {
public:
    using EverestBaseRuntimeError::EverestBaseRuntimeError;
};

class EverestArgumentError : public EverestBaseLogicError {
public:
    using EverestBaseLogicError::EverestBaseLogicError;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_EXCEPTIONS_HPP
