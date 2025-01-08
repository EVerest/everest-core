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

} // namespace defaults

std::string parse_string_option(const boost::program_options::variables_map& vm, const char* option);

const auto TERMINAL_STYLE_ERROR = fmt::emphasis::bold | fg(fmt::terminal_color::red);
const auto TERMINAL_STYLE_OK = fmt::emphasis::bold | fg(fmt::terminal_color::green);
const auto TERMINAL_STYLE_BLUE = fmt::emphasis::bold | fg(fmt::terminal_color::blue);

/// \brief Runtime settings needed to successfully run modules
struct RuntimeSettings {
    fs::path prefix;      ///< Prefix for EVerest installation
    fs::path etc_dir;     ///< Directory that contains configs, certificates
    fs::path data_dir;    ///< Directory for general data, definitions for EVerest interfaces, types, errors an schemas
    fs::path modules_dir; ///< Directory that contains EVerest modules
    fs::path logging_config_file; ///< Path to the logging configuration file
    std::string telemetry_prefix; ///< MQTT prefix for telemetry
    bool telemetry_enabled;       ///< If telemetry is enabled
    bool validate_schema;         ///< If schema validation for all var publishes and cmd calls is enabled

    explicit RuntimeSettings(const fs::path& prefix, const fs::path& etc_dir, const fs::path& data_dir,
                             const fs::path& modules_dir, const fs::path& logging_config_file,
                             const std::string& telemetry_prefix, bool telemetry_enabled, bool validate_schema);

    explicit RuntimeSettings(const nlohmann::json& json);
};

/// \brief Settings needed by the manager to load and validate a config

struct ManagerSettings {
    fs::path configs_dir;          ///< Directory that contains EVerest configs
    fs::path schemas_dir;          ///< Directory that contains schemas for config, manifest, interfaces, etc.
    fs::path interfaces_dir;       ///< Directory that contains interface definitions
    fs::path types_dir;            ///< Directory that contains type definitions
    fs::path errors_dir;           ///< Directory that contains error definitions
    fs::path config_file;          ///< Path to the loaded config file
    fs::path www_dir;              ///< Directory that contains the everest-admin-panel
    int controller_port;           ///< Websocket port of the controller
    int controller_rpc_timeout_ms; ///< RPC timeout for controller commands

    std::string run_as_user; ///< Username under which EVerest should run

    std::string version_information; ///< Version information string reported on startup of the manager

    nlohmann::json config; ///< Parsed json of the config_file

    MQTTSettings mqtt_settings;                        ///< MQTT connection settings
    std::unique_ptr<RuntimeSettings> runtime_settings; ///< Runtime settings needed to successfully run modules

    ManagerSettings(const std::string& prefix, const std::string& config);

    const RuntimeSettings& get_runtime_settings() const;
};

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

    bool parse_command_line(int argc, char* argv[]);

public:
    explicit ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks) :
        ModuleLoader(argc, argv, std::move(callbacks),
                     {"undefined project", "undefined version", "undefined git version"}){};
    explicit ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks, VersionInformation version_information);

    int initialize();
};

} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN
template <> struct adl_serializer<Everest::RuntimeSettings> {
    static void to_json(nlohmann::json& j, const Everest::RuntimeSettings& r);

    static void from_json(const nlohmann::json& j, Everest::RuntimeSettings& r);
};
NLOHMANN_JSON_NAMESPACE_END

#endif // FRAMEWORK_EVEREST_RUNTIME_HPP
