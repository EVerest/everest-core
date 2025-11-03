// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <boost/current_function.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/throw_exception.hpp>
#include <exception>
#include <string>

namespace Everest {
namespace Logging {

enum severity_level {
    verbose,
    debug,
    info,
    warning,
    error,
    critical,
};

/// \brief Initialize a completely silenced logger
void init();

/// \brief Initialize logger using the config pointed to by \p logconf
void init(const std::string& logconf);

/// \brief Initialize logger using the config pointed to by \p logconf additionally setting the \p process_name
void init(const std::string& logconf, std::string process_name);

void update_process_name(std::string process_name);
std::string trace();
} // namespace Logging

// clang-format off
#define EVLOG_verbose                                                                                                  \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::verbose)                                                 \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("file", __FILE__)                                        \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("line", __LINE__)                                        \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)

#define EVLOG_debug                                                                                                    \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::debug)                                                   \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)

#define EVLOG_info                                                                                                     \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::info)

#define EVLOG_warning                                                                                                  \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::warning)                                                 \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)

#define EVLOG_error                                                                                                    \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::error)                                                   \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)

#define EVLOG_critical                                                                                                 \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::critical)                                                \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)
// clang-format on

#define EVLOG_AND_THROW(ex)                                                                                            \
    do {                                                                                                               \
        try {                                                                                                          \
            BOOST_THROW_EXCEPTION(boost::enable_error_info(ex)                                                         \
                                  << boost::log::BOOST_LOG_VERSION_NAMESPACE::current_scope());                        \
        } catch (std::exception & e) {                                                                                 \
            EVLOG_error << e.what();                                                                                   \
            throw;                                                                                                     \
        }                                                                                                              \
    } while (0)

#define EVTHROW(ex)                                                                                                    \
    do {                                                                                                               \
        BOOST_THROW_EXCEPTION(boost::enable_error_info(ex)                                                             \
                              << boost::log::BOOST_LOG_VERSION_NAMESPACE::current_scope());                            \
    } while (0)
} // namespace Everest

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(
    global_logger,
    boost::log::BOOST_LOG_VERSION_NAMESPACE::sources::severity_logger_mt<Everest::Logging::severity_level>)

#endif // LOGGING_HPP
