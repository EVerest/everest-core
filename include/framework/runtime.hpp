#ifndef FRAMEWORK_EVEREST_RUNTIME_HPP
#define FRAMEWORK_EVEREST_RUNTIME_HPP

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <framework/ModuleAdapter.hpp>
#include <sys/prctl.h>

namespace Everest {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// FIXME (aw): should be everest wide or defined in liblog
const int DUMP_INDENT = 4;

// FIXME (aw): we should also define all other config keys and default
//             values here as string literals

const auto TERMINAL_STYLE_ERROR = fmt::emphasis::bold | fg(fmt::terminal_color::red);
const auto TERMINAL_STYLE_OK = fmt::emphasis::bold | fg(fmt::terminal_color::green);

struct RuntimeSettings {
    fs::path main_dir;
    fs::path main_binary;
    fs::path configs_dir;
    fs::path schemas_dir;
    fs::path modules_dir;
    fs::path interfaces_dir;
    fs::path types_dir;
    fs::path logging_config;
    fs::path config_file;
    bool validate_schema;

    explicit RuntimeSettings(const po::variables_map& vm);
};

struct ModuleCallbacks {
    std::function<void(ModuleAdapter module_adapter)> register_module_adapter;
    std::function<std::vector<cmd>(const json& connections)> everest_register;
    std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)> init;
    std::function<void()> ready;

    ModuleCallbacks(const std::function<void(ModuleAdapter module_adapter)>& register_module_adapter,
                    const std::function<std::vector<cmd>(const json& connections)>& everest_register,
                    const std::function<void(ModuleConfigs module_configs, const ModuleInfo& info)>& init,
                    const std::function<void()>& ready);
};

class ModuleLoader {
private:
    std::unique_ptr<RuntimeSettings> runtime_settings;
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
