// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef FRAMEWORK_EVEREST_RUNTIME_HPP
#define FRAMEWORK_EVEREST_RUNTIME_HPP

#include <filesystem>
#include <string>

#include <fmt/color.h>
#include <fmt/core.h>
#include <sys/prctl.h>

#include <framework/ModuleAdapter.hpp>
#include <utils/config/settings.hpp>
#include <utils/module_config.hpp>
#include <utils/yaml_loader.hpp>

#include <everest/compile_time_settings.hpp>
#include <everest/logging.hpp>

namespace boost::program_options {
class variables_map; // forward declaration
} // namespace boost::program_options

namespace Everest {

namespace fs = std::filesystem;

// FIXME (aw): should be everest wide or defined in liblog
const int DUMP_INDENT = 4;

// FIXME (aw): we should also define all other config keys and default
//             values here as string literals

inline constexpr auto EV_MODULE = "EV_MODULE";
inline constexpr auto EV_PREFIX = "EV_PREFIX";
inline constexpr auto EV_LOG_CONF_FILE = "EV_LOG_CONF_FILE";
inline constexpr auto EV_MQTT_EVEREST_PREFIX = "EV_MQTT_EVEREST_PREFIX";
inline constexpr auto EV_MQTT_EXTERNAL_PREFIX = "EV_MQTT_EXTERNAL_PREFIX";
inline constexpr auto EV_MQTT_BROKER_SOCKET_PATH = "EV_MQTT_BROKER_SOCKET_PATH";
inline constexpr auto EV_MQTT_BROKER_HOST = "EV_MQTT_BROKER_HOST";
inline constexpr auto EV_MQTT_BROKER_PORT = "EV_MQTT_BROKER_PORT";
inline constexpr auto EV_VALIDATE_SCHEMA = "EV_VALIDATE_SCHEMA";
inline constexpr auto VERSION_INFORMATION_FILE = "version_information.txt";

// FIXME (aw): this needs to be made available by
namespace defaults {

// defaults:
//   PREFIX: set by cmake
//   EVEREST_NAMESPACE: everest
//   EVEREST_INSTALL_LIBDIR: set by cmake
//   BIN_DIR: ${PREFIX}/bin
//   LIBEXEC_DIR: ${PREFIX}/libexec
//   LIB_DIR: ${PREFIX}/${EVEREST_INSTALL_LIBDIR}
//   SYSCONF_DIR: /etc, if ${PREFIX}==/usr, otherwise ${PREFIX}/etc
//   LOCALSTATE_DIR: /var, if ${PREFIX}==/usr, otherwise ${PREFIX}/var
//   DATAROOT_DIR: ${PREFIX}/share
//
//   modules_dir: ${LIBEXEC_DIR}${EVEREST_NAMESPACE}
//   types_dir: ${DATAROOT_DIR}${EVEREST_NAMESPACE}/types
//   interfaces_dir: ${DATAROOT_DIR}${EVEREST_NAMESPACE}/interfaces
//   schemas_dir: ${DATAROOT_DIR}${EVEREST_NAMESPACE}/schemas
//   configs_dir: ${SYSCONF_DIR}${EVEREST_NAMESPACE}
//
//   config_path: ${SYSCONF_DIR}${EVEREST_NAMESPACE}/default.yaml
//   logging_config_path: ${SYSCONF_DIR}${EVEREST_NAMESPACE}/default_logging.cfg

inline constexpr auto PREFIX = EVEREST_INSTALL_PREFIX;
inline constexpr auto NAMESPACE = EVEREST_NAMESPACE;

inline constexpr auto BIN_DIR = "bin";
inline constexpr auto LIB_DIR = EVEREST_INSTALL_LIBDIR;
inline constexpr auto LIBEXEC_DIR = "libexec";
inline constexpr auto SYSCONF_DIR = "etc";
inline constexpr auto LOCALSTATE_DIR = "var";
inline constexpr auto DATAROOT_DIR = "share";

inline constexpr auto MODULES_DIR = "modules";
inline constexpr auto TYPES_DIR = "types";
inline constexpr auto ERRORS_DIR = "errors";
inline constexpr auto INTERFACES_DIR = "interfaces";
inline constexpr auto SCHEMAS_DIR = "schemas";
inline constexpr auto CONFIG_NAME = "default.yaml";
inline constexpr auto LOGGING_CONFIG_NAME = "default_logging.cfg";

inline constexpr auto WWW_DIR = "www";

inline constexpr auto CONTROLLER_PORT = 8849;
inline constexpr auto CONTROLLER_RPC_TIMEOUT_MS = 2000;
inline constexpr auto MQTT_BROKER_SOCKET_PATH = "/tmp/mqtt_broker.sock";
inline constexpr auto MQTT_BROKER_HOST = "localhost";
inline constexpr auto MQTT_BROKER_PORT = 1883;
inline constexpr auto MQTT_EVEREST_PREFIX = "everest";
inline constexpr auto MQTT_EXTERNAL_PREFIX = "";
inline constexpr auto TELEMETRY_PREFIX = "everest-telemetry";
inline constexpr auto TELEMETRY_ENABLED = false;
inline constexpr auto VALIDATE_SCHEMA = false;
inline constexpr auto FORWARD_EXCEPTIONS = false;

} // namespace defaults

std::string parse_string_option(const boost::program_options::variables_map& vm, const char* option);

inline constexpr auto TERMINAL_STYLE_ERROR = fmt::emphasis::bold | fg(fmt::terminal_color::red);
inline constexpr auto TERMINAL_STYLE_OK = fmt::emphasis::bold | fg(fmt::terminal_color::green);
inline constexpr auto TERMINAL_STYLE_BLUE = fmt::emphasis::bold | fg(fmt::terminal_color::blue);

// NOTE: this function needs the be called with a pre-initialized ModuleInfo struct
void populate_module_info_path_from_runtime_settings(ModuleInfo&, const RuntimeSettings& rs);

/// \brief Callbacks that need to be registered for modules
struct ModuleCallbacks {
    std::function<void(ModuleAdapter module_adapter)> register_module_adapter;
    std::function<std::vector<cmd>(const RequirementInitialization& requirement_init)> everest_register;
    std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)> init;
    std::function<void()> ready;

    ModuleCallbacks() = default;

    ModuleCallbacks(
        const std::function<void(ModuleAdapter module_adapter)>& register_module_adapter,
        const std::function<std::vector<cmd>(const RequirementInitialization& requirement_init)>& everest_register,
        const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init,
        const std::function<void()>& ready);
};

///\brief Version information
struct VersionInformation {
    std::string project_name;    ///< CMake project name
    std::string project_version; ///< Human readable version number
    std::string git_version;     ///< Git version containing tag and branch information
};

class ModuleLoader {
private:
    std::unique_ptr<RuntimeSettings> runtime_settings;
    MQTTSettings mqtt_settings;
    std::shared_ptr<MQTTAbstraction> mqtt;
    std::string module_id;
    std::string original_process_name;
    std::string application_name;
    ModuleCallbacks callbacks;
    VersionInformation version_information;
    fs::path logging_config_file;
    bool should_exit = false;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): pass-through of argc and argv from main()
    bool parse_command_line(int argc, char* argv[]);

public:
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): pass-through of argc and argv from main()
    explicit ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks) :
        ModuleLoader(argc, argv, std::move(callbacks),
                     {"undefined project", "undefined version", "undefined git version"}){};
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays): pass-through of argc and argv from main()
    explicit ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks, VersionInformation version_information);

    int initialize();
};

} // namespace Everest

#endif // FRAMEWORK_EVEREST_RUNTIME_HPP
