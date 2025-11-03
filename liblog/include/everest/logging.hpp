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

enum severity_level
{
    debug,
    info,
    warning,
    error,
    critical,
};

void init(const std::string& logconf);
void init(const std::string& logconf, std::string process_name);
void update_process_name(std::string process_name);
} // namespace Logging

// clang-format off
#define EVLOG(severity)                                                                                                \
    BOOST_LOG_SEV(::global_logger::get(), ::Everest::Logging::severity)                                                \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("file", __FILE__)                                        \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("line", __LINE__)                                        \
        << boost::log::BOOST_LOG_VERSION_NAMESPACE::add_value("function", BOOST_CURRENT_FUNCTION)
// clang-format on

#define EVLOG_AND_THROW(ex)                                                                                            \
    do {                                                                                                               \
        try {                                                                                                          \
            BOOST_THROW_EXCEPTION(boost::enable_error_info(ex)                                                         \
                                  << boost::log::BOOST_LOG_VERSION_NAMESPACE::current_scope());                        \
        } catch (std::exception & e) {                                                                                 \
            EVLOG(error) << e.what();                                                                                  \
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
    boost::log::BOOST_LOG_VERSION_NAMESPACE::sources::severity_logger_mt<Everest::Logging::severity_level>);

#endif // LOGGING_HPP
