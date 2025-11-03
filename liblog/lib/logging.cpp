// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifdef LIBLOG_USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/attributes/current_process_name.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/formatter.hpp>
#include <boost/log/expressions/formatters/c_decorator.hpp>
#include <boost/log/expressions/formatters/format.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/settings_parser.hpp>
#include <fstream>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

// this will only be used while bootstrapping our logging (e.g. the logging settings aren't yet applied)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EVEREST_INTERNAL_LOG_AND_THROW(exception)                                                                      \
    do {                                                                                                               \
        BOOST_LOG_TRIVIAL(fatal) << (exception).what();                                                                \
        throw(exception);                                                                                              \
    } while (0);

#ifdef LIBLOG_USE_BOOST_FILESYSTEM
namespace fs = boost::filesystem;
#else
namespace fs = std::filesystem;
#endif
namespace logging = boost::log::BOOST_LOG_VERSION_NAMESPACE;
namespace attrs = logging::attributes;
namespace expr = logging::expressions;

namespace Everest {
namespace Logging {
namespace {
bool is_initialized = false;
}

std::array<std::string, 6> severity_strings = {
    "VERB", //
    "DEBG", //
    "INFO", //
    "WARN", //
    "ERRO", //
    "CRIT", //
};

std::array<std::string, 6> severity_strings_colors = {
    "", //
    "", //
    "", //
    "", //
    "", //
    "", //
};

std::string process_name_padding(const std::string& process_name) {
    const unsigned int process_name_padding_length = 15;
    std::string padded_process_name = process_name;
    if (process_name_padding_length > padded_process_name.size())
        padded_process_name.insert(padded_process_name.size(), process_name_padding_length, ' ');
    if (padded_process_name.size() > process_name_padding_length)
        padded_process_name = padded_process_name.substr(0, process_name_padding_length);
    return padded_process_name;
}

attrs::mutable_constant<std::string> current_process_name(process_name_padding(logging::aux::get_process_name()));

// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<<(std::ostream& strm, severity_level level) {
    if (static_cast<std::size_t>(level) < severity_strings.size()) {
        strm << severity_strings_colors.at(level) << severity_strings.at(level) << "\033[0m";
    } else {
        strm << static_cast<int>(level);
    }

    return strm;
}

// The operator parses the severity level from the stream
std::istream& operator>>(std::istream& strm, severity_level& level) {
    if (strm.good()) {
        std::string str;
        strm >> str;

        for (unsigned int i = 0; i < severity_strings.size(); ++i) {
            if (str == severity_strings.at(i)) {
                level = static_cast<severity_level>(i);
                return strm;
            }
        }

        strm.setstate(std::ios_base::failbit);
    }

    return strm;
}

/// Custom formatter for escaped messages.
///
/// Not really clear but just a wrapper around the c_decor formatter.
struct escaped_message_formatter {
    explicit escaped_message_formatter(logging::attribute_name const& name) :
        f_{expr::stream << expr::c_decor[expr::stream << expr::smessage]} {
    }
    void operator()(logging::record_view const& rec, logging::formatting_ostream& strm) const {
        f_(rec, strm);
    }

private:
    /// @brief The formatter itself.
    boost::log::formatter f_;
};

/// The factory for the EscMessage formatter.
struct escaped_message_formatter_factory : public logging::formatter_factory<char> {
    formatter_type create_formatter(logging::attribute_name const& attr_name, args_map const& args) {
        return formatter_type(escaped_message_formatter(attr_name));
    }
};

void init() {
    logging::core::get()->remove_all_sinks();
    logging::core::get()->set_logging_enabled(false);
}

void init(const std::string& logconf) {
    init(logconf, "");
}

void init(const std::string& logconf, std::string process_name) {
    BOOST_LOG_FUNCTION();

    if (is_initialized) {
        // this prevents us from registering the sinks multiple times which would lead to duplicate output
        logging::core::get()->remove_all_sinks();
    }

    // First thing - register the custom formatter for EscMessage
    logging::register_formatter_factory("EscapedMessage", boost::make_shared<escaped_message_formatter_factory>());

    // add useful attributes
    logging::add_common_attributes();

    std::string padded_process_name;

    if (!process_name.empty()) {
        padded_process_name = process_name_padding(process_name);
    }

    logging::core::get()->add_global_attribute("Process", current_process_name);
    if (!padded_process_name.empty()) {
        current_process_name.set(padded_process_name);
    }
    logging::core::get()->add_global_attribute("Scope", attrs::named_scope());

    // Before initializing the library from settings, we need to register any custom filter and formatter factories
    logging::register_simple_filter_factory<severity_level>("Severity");
    logging::register_simple_formatter_factory<severity_level, char>("Severity");

    // open logging.ini config file located at our base_dir and use it to configure boost::log logging (filters and
    // format)
    fs::path logging_path = fs::path(logconf);
    std::ifstream logging_config(logging_path.c_str());
    if (!logging_config.is_open()) {
        EVEREST_INTERNAL_LOG_AND_THROW(EverestConfigError(std::string("Could not open logging config file at ") +
                                                          std::string(fs::absolute(logging_path).c_str())));
    }

    auto settings = logging::parse_settings(logging_config);

    auto sink = settings["Sinks.Console"].get_section();

    severity_strings_colors[severity_level::verbose] =
        sink["SeverityStringColorTrace"].get<std::string>().get_value_or("");
    severity_strings_colors[severity_level::debug] =
        sink["SeverityStringColorDebug"].get<std::string>().get_value_or("");
    severity_strings_colors[severity_level::info] = sink["SeverityStringColorInfo"].get<std::string>().get_value_or("");
    severity_strings_colors[severity_level::warning] =
        sink["SeverityStringColorWarning"].get<std::string>().get_value_or("");
    severity_strings_colors[severity_level::error] =
        sink["SeverityStringColorError"].get<std::string>().get_value_or("");
    severity_strings_colors[severity_level::critical] =
        sink["SeverityStringColorCritical"].get<std::string>().get_value_or("");

    logging::init_from_settings(settings);
    logging::core::get()->set_logging_enabled(true);

    EVLOG_debug << "Logger " << (is_initialized ? "re" : "") << "initialized (using " << logconf << ")...";
    is_initialized = true;
}

void update_process_name(std::string process_name) {
    if (!process_name.empty()) {
        std::string padded_process_name;

        padded_process_name = process_name_padding(process_name);
        current_process_name.set(padded_process_name);
    }
}
} // namespace Logging
} // namespace Everest
