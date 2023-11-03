// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef FRAMEWORK_EVEREST_RUNTIME_HPP
#define FRAMEWORK_EVEREST_RUNTIME_HPP

#include <filesystem>
#include <string>

#include <everest/logging.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <framework/ModuleAdapter.hpp>
#include <sys/prctl.h>

#include <utils/yaml_loader.hpp>

#include <everest/compile_time_settings.hpp>

namespace boost::program_options {
class variables_map; // forward declaration
}

namespace Everest {

namespace fs = std::filesystem;

// FIXME (aw): should be everest wide or defined in liblog
const int DUMP_INDENT = 4;

// FIXME (aw): we should also define all other config keys and default
//             values here as string literals

// FIXME (aw): this needs to be made available by
namespace defaults {

// defaults:
//   PREFIX: set by cmake
//   EVEREST_NAMESPACE: everest
//   BIN_DIR: ${PREFIX}/bin
//   LIBEXEC_DIR: ${PREFIX}/libexec
//   LIB_DIR: ${PREFIX}/lib
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
inline constexpr auto LIB_DIR = "lib";
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

struct BootException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeSettings {
    fs::path prefix;
    fs::path etc_dir;
    fs::path data_dir;
    fs::path configs_dir;
    fs::path schemas_dir;
    fs::path modules_dir;
    fs::path interfaces_dir;
    fs::path types_dir;
    fs::path errors_dir;
    fs::path logging_config_file;
    fs::path config_file;
    fs::path www_dir;
    int controller_port;
    int controller_rpc_timeout_ms;
    std::string mqtt_broker_host;
    int mqtt_broker_port;
    std::string mqtt_everest_prefix;
    std::string mqtt_external_prefix;
    std::string telemetry_prefix;
    bool telemetry_enabled;

    nlohmann::json config;

    bool validate_schema;

    explicit RuntimeSettings(const std::string& prefix, const std::string& config);
};

// NOTE: this function needs the be called with a pre-initialized ModuleInfo struct
void populate_module_info_path_from_runtime_settings(ModuleInfo&, std::shared_ptr<RuntimeSettings> rs);

struct ModuleCallbacks {
    std::function<void(ModuleAdapter module_adapter)> register_module_adapter;
    std::function<std::vector<cmd>(const json& connections)> everest_register;
    std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)> init;
    std::function<void()> ready;

    ModuleCallbacks() = default;

    ModuleCallbacks(const std::function<void(ModuleAdapter module_adapter)>& register_module_adapter,
                    const std::function<std::vector<cmd>(const json& connections)>& everest_register,
                    const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init,
                    const std::function<void()>& ready);
};

class ModuleLoader {
private:
    std::shared_ptr<RuntimeSettings> runtime_settings;
    std::string module_id;
    std::string original_process_name;
    ModuleCallbacks callbacks;

    bool parse_command_line(int argc, char* argv[]);

public:
    explicit ModuleLoader(int argc, char* argv[], ModuleCallbacks callbacks);

    int initialize();
};

} // namespace Everest

#endif // FRAMEWORK_EVEREST_RUNTIME_HPP
