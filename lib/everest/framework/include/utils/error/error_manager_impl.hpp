// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_MANAGER_IMPL_HPP
#define UTILS_ERROR_MANAGER_IMPL_HPP

#include <list>
#include <memory>
#include <mutex>

#include <utils/error.hpp>

namespace Everest {
namespace error {

struct ErrorDatabase;
struct ErrorTypeMap;

class ErrorManagerImpl {
public:
    using PublishErrorFunc = std::function<void(const error::Error&)>;

    ErrorManagerImpl(std::shared_ptr<ErrorTypeMap> error_type_map, std::shared_ptr<ErrorDatabase> error_database,
                     std::list<ErrorType> allowed_error_types, PublishErrorFunc publish_raised_error,
                     PublishErrorFunc publish_cleared_error, const bool validate_error_types = true);

    ///
    /// \brief raise_error raises an error
    /// \param error The error to raise
    /// \details This function raises an error. The error is added to the error database if it has a valid error type
    /// and is allowed to be raised.
    ///
    void raise_error(const Error& error);

    ///
    /// \brief clear_error clears an error
    /// \param type The error type to clear
    /// \details This function clears an error if there are not multiples sub types active.
    ///
    std::list<ErrorPtr> clear_error(const ErrorType& type);

    ///
    /// \brief clear_error clears an error
    /// \param type The error type to clear
    /// \param sub_type The error sub type to clear
    /// \details This function clears a specific subtype of an error type.
    ///
    std::list<ErrorPtr> clear_error(const ErrorType& type, const ErrorSubType& sub_type);

    ///
    /// \brief clear_all_errors clears all errors
    /// \details This function clears all errors that are currently active.
    ///
    std::list<ErrorPtr> clear_all_errors();

    ///
    /// \brief clear_all_errors clears all errors of a specific ErrorType
    /// \param error_type The error type to clear
    /// \details This function clears all errors of a specific ErrorType that are currently active.
    ///
    std::list<ErrorPtr> clear_all_errors(const ErrorType& error_type);

private:
    bool can_be_raised(const ErrorType& type, const ErrorSubType& sub_type) const;
    bool can_be_cleared(const ErrorType& type, const ErrorSubType& sub_type) const;
    bool can_be_cleared(const ErrorType& type) const;

    std::shared_ptr<ErrorTypeMap> error_type_map;
    std::shared_ptr<ErrorDatabase> database;
    std::list<ErrorType> allowed_error_types;

    std::mutex mutex;

    PublishErrorFunc publish_raised_error;
    PublishErrorFunc publish_cleared_error;

    const bool validate_error_types;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_MANAGER_IMPL_HPP
