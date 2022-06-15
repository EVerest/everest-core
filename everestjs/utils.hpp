// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_HPP
#define UTILS_HPP

#include <everest/logging.hpp>
#include <everest/metamacros.hpp>

namespace EverestJs {

// this is needed to get javascript stacktraces whenever possible while still logging boost error information
#define EVLOG_AND_RETHROW(...)                                                                                         \
    do {                                                                                                               \
        try {                                                                                                          \
            throw;                                                                                                     \
        } catch (Napi::Error & e) {                                                                                    \
            try {                                                                                                      \
                BOOST_THROW_EXCEPTION(boost::enable_error_info(e)                                                      \
                                      << boost::log::BOOST_LOG_VERSION_NAMESPACE::current_scope());                    \
            } catch (boost::exception & ex) {                                                                          \
                char const* const* f = boost::get_error_info<boost::throw_file>(ex);                                   \
                int const* l = boost::get_error_info<boost::throw_line>(ex);                                           \
                char const* const* fn = boost::get_error_info<boost::throw_function>(ex);                              \
                EVLOG_critical  << "Catched top level Napi::Error, forwarding to javascript..." << std::endl           \
                                << (f ? *f : "") << ":" << (l ? std::to_string(*l) : "") << " in Function "            \
                                << (fn ? *fn : "") << std::endl                                                        \
                                << boost::diagnostic_information(e, true) << boost::diagnostic_information(ex, false)  \
                                << std::endl                                                                           \
                                << "==============================" << std::endl                                       \
                                << std::endl;                                                                          \
            }                                                                                                          \
            throw; /* this will forward the exception back to javascript and enable js backtraces */                   \
        } catch (std::exception & e) {                                                                                 \
            try {                                                                                                      \
                BOOST_THROW_EXCEPTION(boost::enable_error_info(e)                                                      \
                                      << boost::log::BOOST_LOG_VERSION_NAMESPACE::current_scope());                    \
            } catch (boost::exception & ex) {                                                                          \
                char const* const* f = boost::get_error_info<boost::throw_file>(ex);                                   \
                int const* l = boost::get_error_info<boost::throw_line>(ex);                                           \
                char const* const* fn = boost::get_error_info<boost::throw_function>(ex);                              \
                EVLOG_critical  << "Catched top level exception, forwarding to javascript..." << std::endl             \
                                << (f ? *f : "") << ":" << (l ? std::to_string(*l) : "") << " in Function "            \
                                << (fn ? *fn : "") << std::endl                                                        \
                                << boost::diagnostic_information(e, true) << boost::diagnostic_information(ex, false)  \
                                << std::endl                                                                           \
                                << "==============================" << std::endl                                       \
                                << std::endl;                                                                          \
                /* this will forward the exception to javascript and enable js backtraces */                           \
                metamacro_if_eq(0, metamacro_argcount(__VA_ARGS__))(throw;)(EVTHROW(Napi::Error::New(                  \
                    metamacro_at(0, __VA_ARGS__), Napi::String::New(metamacro_at(0, __VA_ARGS__), e.what())));)        \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)
} // namespace EverestJs

#endif // UTILS_HPP
