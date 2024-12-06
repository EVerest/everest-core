// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_TYPE_MAP_HPP
#define UTILS_ERROR_TYPE_MAP_HPP

#include <filesystem>

#include <utils/error.hpp>

namespace Everest {
namespace error {

///
/// \brief A map of error types to their descriptions.
/// This class is used to load error types from a directory
/// and to get the description of an error type.
///
class ErrorTypeMap {
public:
    ///
    /// \brief Default constructor.
    /// Creates an empty map.
    ///
    ErrorTypeMap() = default;

    ///
    /// \brief Constructor that loads error types from a directory.
    /// \param error_types_dir The directory to load the error types from.
    ///
    explicit ErrorTypeMap(std::filesystem::path error_types_dir);

    ///
    /// \brief Loads error types from a directory.
    /// \param error_types_dir The directory to load the error types from.
    ///
    void load_error_types(std::filesystem::path error_types_dir);

    ///
    /// \brief Loads error types from a given map
    /// \param error_types_dir The map to load the error types from.
    ///
    void load_error_types_map(std::map<ErrorType, std::string> error_types_map);

    ///
    /// \brief Gets the description of an error type.
    /// \param error_type The error type to get the description of.
    /// \return The description of the error type.
    ///
    std::string get_description(const ErrorType& error_type) const;

    ///
    /// \brief Checks if an error type exists.
    /// \param error_type The error type to check.
    /// \return True if the error type exists, false otherwise.
    ///
    bool has(const ErrorType& error_type) const;

    ///
    /// \brief Returns the contained ErrorType map
    /// \return The error types map
    ///
    std::map<ErrorType, std::string> get_error_types();

private:
    std::map<ErrorType, std::string> error_types;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_TYPE_MAP_HPP
