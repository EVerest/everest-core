// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>
#include <fmt/color.h>
#include <fmt/core.h>

#include <framework/everest.hpp>
#include <framework/runtime.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace Everest;

const auto PARENT_DIED_SIGNAL = SIGTERM;

class SubprocessHandle {
public:
    bool is_child() const {
        return this->pid == 0;
    }

    void send_error_and_exit(const std::string& message) {
        // FIXME (aw): howto do asserts?
        assert(pid == 0);

        write(fd, message.c_str(), std::min(message.size(), MAX_PIPE_MESSAGE_SIZE - 1));
        close(fd);
        _exit(EXIT_FAILURE);
    }

    // FIXME (aw): this function should be callable only once
    pid_t check_child_executed() {
        assert(pid != 0);

        std::string message(MAX_PIPE_MESSAGE_SIZE, 0);

        auto retval = read(fd, message.data(), MAX_PIPE_MESSAGE_SIZE);
        if (retval == -1) {
            throw std::runtime_error(fmt::format(
                "Failed to communicate via pipe with forked child process. Syscall to read() failed ({}), exiting",
                strerror(errno)));
        } else if (retval > 0) {
            throw std::runtime_error(fmt::format("Forked child process did not complete exec():\n{}", message.c_str()));
        }

        close(fd);
        return pid;
    }

private:
    const size_t MAX_PIPE_MESSAGE_SIZE = 1024;
    SubprocessHandle(int fd, pid_t pid) : fd(fd), pid(pid){};
    int fd{};
    pid_t pid{};

    friend SubprocessHandle create_subprocess(bool set_pdeathsig);
};

SubprocessHandle create_subprocess(bool set_pdeathsig = true) {
    int pipefd[2];

    if (pipe2(pipefd, O_CLOEXEC | O_DIRECT)) {
        throw std::runtime_error(fmt::format("Syscall pipe2() failed ({}), exiting", strerror(errno)));
    }

    const auto reading_end_fd = pipefd[0];
    const auto writing_end_fd = pipefd[1];

    const auto parent_pid = getpid();

    pid_t pid = fork();

    if (pid == -1) {
        throw std::runtime_error(fmt::format("Syscall fork() failed ({}), exiting", strerror(errno)));
    }

    if (pid == 0) {
        // close read end in child
        close(reading_end_fd);

        SubprocessHandle handle{writing_end_fd, pid};

        if (set_pdeathsig) {
            // FIXME (aw): how does the the forked process does cleanup when receiving PARENT_DIED_SIGNAL compared to
            //             _exit() before exec() has been called?
            if (prctl(PR_SET_PDEATHSIG, PARENT_DIED_SIGNAL)) {
                handle.send_error_and_exit(fmt::format("Syscall prctl() failed ({}), exiting", strerror(errno)));
            }

            if (getppid() != parent_pid) {
                // kill ourself, with the same handler as we would have
                // happened when the parent process died
                kill(getpid(), PARENT_DIED_SIGNAL);
            }
        }

        return handle;
    } else {
        close(writing_end_fd);
        return {reading_end_fd, pid};
    }
}

// Helper struct keeping information on how to start module
struct ModuleStartInfo {
    enum class Language
    {
        cpp,
        javascript
    };
    ModuleStartInfo(const std::string& name, const std::string& printable_name, Language lang,
                    const boost::filesystem::path& path) :
        name(name), printable_name(printable_name), language(lang), path(path) {
    }
    std::string name;
    std::string printable_name;
    Language language;
    boost::filesystem::path path;
};

static std::vector<char*> arguments_to_exec_argv(std::vector<std::string>& arguments) {
    std::vector<char*> argv_list(arguments.size() + 1);
    std::transform(arguments.begin(), arguments.end(), argv_list.begin(),
                   [](std::string& value) { return value.data(); });

    // add NULL for exec
    argv_list.back() = nullptr;
    return argv_list;
}

static SubprocessHandle exec_cpp_module(const ModuleStartInfo& module_info, const RuntimeSettings& rs) {

    const auto exec_binary = module_info.path.string().c_str();
    std::vector<std::string> arguments = {module_info.printable_name,
                                          "--main_dir",
                                          rs.main_dir.string(),
                                          rs.interfaces_dir.string(),
                                          "--log_conf",
                                          rs.logging_config.string(),
                                          "--conf",
                                          rs.config_file.string(),
                                          "--module",
                                          module_info.name};

    auto handle = create_subprocess();
    if (handle.is_child()) {
        auto argv_list = arguments_to_exec_argv(arguments);
        execv(exec_binary, argv_list.data());

        // exec failed
        handle.send_error_and_exit(fmt::format("Syscall to execv() with \"{} {}\" failed ({})", exec_binary,
                                               fmt::join(arguments.begin() + 1, arguments.end(), " "),
                                               strerror(errno)));
    }

    return handle;
}

static SubprocessHandle exec_javascript_module(const ModuleStartInfo& module_info, const RuntimeSettings& rs) {
    // instead of using setenv, using execvpe might be a better way for a controlled environment!

    auto node_modules_path = rs.main_dir / "everestjs" / "node_modules";
    setenv("NODE_PATH", node_modules_path.c_str(), 0);

    setenv("EV_MODULE", module_info.name.c_str(), 1);
    setenv("EV_MAIN_DIR", rs.main_dir.c_str(), 0);
    setenv("EV_SCHEMAS_DIR", rs.schemas_dir.c_str(), 0);
    setenv("EV_MODULES_DIR", rs.modules_dir.c_str(), 0);
    setenv("EV_INTERFACES_DIR", rs.interfaces_dir.c_str(), 0);
    setenv("EV_CONF_FILE", rs.config_file.c_str(), 0);
    setenv("EV_LOG_CONF_FILE", rs.logging_config.c_str(), 0);

    if (!rs.validate_schema) {
        setenv("EV_DONT_VALIDATE_SCHEMA", "", 0);
    }

    chdir(rs.main_dir.c_str());

    const auto node_binary = "node";

    std::vector<std::string> arguments = {
        "node",
        "--unhandled-rejections=strict",
        module_info.path.string(),
    };

    auto handle = create_subprocess();
    if (handle.is_child()) {
        auto argv_list = arguments_to_exec_argv(arguments);
        execvp(node_binary, argv_list.data());

        // exec failed
        handle.send_error_and_exit(fmt::format("Syscall to execv() with \"{} {}\" failed ({})", node_binary,
                                               fmt::join(arguments.begin() + 1, arguments.end(), " "),
                                               strerror(errno)));
    }
    return handle;
}

static const std::map<pid_t, std::string> start_modules(const std::vector<ModuleStartInfo>& modules,
                                                        const RuntimeSettings& rs) {
    std::map<pid_t, std::string> children;
    for (const auto& module : modules) {

        auto handle = [&module, &rs]() -> SubprocessHandle {
            // this if should never return!
            switch (module.language) {
            case ModuleStartInfo::Language::cpp:
                return exec_cpp_module(module, rs);
            case ModuleStartInfo::Language::javascript:
                return exec_javascript_module(module, rs);
            default:
                throw std::logic_error("Module language not in enum");
                break;
            }
        }();

        // we can only come here, if we're the parent!
        auto child_pid = handle.check_child_executed();

        EVLOG(debug) << fmt::format("Forked module {} with pid: {}", module.name, child_pid);
        children[child_pid] = module.name;
    }

    return children;
}

int boot(const po::variables_map& vm) {
    bool check = (vm.count("check") != 0);
    RuntimeSettings rs(vm);

    ::Everest::Logging::init(rs.logging_config.string());

    EVLOG(debug) << fmt::format("main_dir was set to {}", rs.main_dir.string());
    EVLOG(debug) << fmt::format("main_binary was set to {}", rs.main_binary.string());

    // dump all manifests if requested and terminate afterwards
    if (vm.count("dumpmanifests")) {
        boost::filesystem::path dumpmanifests_path = boost::filesystem::path(vm["dumpmanifests"].as<std::string>());
        EVLOG(info) << fmt::format("Dumping all known validated manifests into '{}'", dumpmanifests_path.string());

        auto manifests = ::Everest::Config::load_all_manifests(rs.modules_dir.string(), rs.schemas_dir.string());

        for (const auto& module : manifests.items()) {
            std::string filename = module.key() + ".json";
            boost::filesystem::path module_output_path = dumpmanifests_path / filename;
            boost::filesystem::ofstream output_stream(module_output_path);

            output_stream << module.value().dump(DUMP_INDENT);
        }

        return EXIT_SUCCESS;
    }

    ::Everest::Config* config = nullptr;
    try {
        // FIXME (aw): we should also use boost::filesystem::path here as argument types
        config = new ::Everest::Config(rs.schemas_dir.string(), rs.config_file.string(), rs.modules_dir.string(),
                                       rs.interfaces_dir.string());
    } catch (::Everest::EverestInternalError& e) {
        EVLOG(error) << fmt::format("Failed to load and validate config!\n{}", boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    } catch (boost::exception& e) {
        EVLOG(error) << "Failed to load and validate config!";
        EVLOG(critical) << fmt::format("Caught top level boost::exception:\n{}",
                                       boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    } catch (std::exception& e) {
        EVLOG(error) << "Failed to load and validate config!";
        EVLOG(critical) << fmt::format("Caught top level std::exception:\n{}", boost::diagnostic_information(e, true));
        return EXIT_FAILURE;
    }

    // dump config if requested
    if (vm.count("dump")) {
        boost::filesystem::path dump_path = boost::filesystem::path(vm["dump"].as<std::string>());
        EVLOG(info) << fmt::format("Dumping validated config and manifests into '{}'", dump_path.string());

        boost::filesystem::path config_dump_path = dump_path / "config.json";

        boost::filesystem::ofstream output_config_stream(config_dump_path);

        output_config_stream << config->get_main_config().dump(DUMP_INDENT);

        auto manifests = config->get_manifests();

        for (const auto& module : manifests.items()) {
            std::string filename = module.key() + ".json";
            boost::filesystem::path module_output_path = dump_path / filename;
            boost::filesystem::ofstream output_stream(module_output_path);

            output_stream << module.value().dump(DUMP_INDENT);
        }
    }

    // only config check (and or config dumping) was requested, log check result and exit
    if (check) {
        EVLOG(info) << "Config is valid, terminating as requested";
        return EXIT_SUCCESS;
    }

    std::vector<std::string> standalone_modules;
    if (vm.count("standalone")) {
        standalone_modules = vm["standalone"].as<std::vector<std::string>>();
    }

    std::vector<std::string> ignored_modules;
    if (vm.count("ignore")) {
        ignored_modules = vm["ignore"].as<std::vector<std::string>>();
    }

    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_address = std::getenv("MQTT_SERVER_ADDRESS");
    if (mqtt_server_address == nullptr) {
        mqtt_server_address = "localhost";
    }

    // NOLINTNEXTLINE(concurrency-mt-unsafe): not problematic that this function is not threadsafe here
    const char* mqtt_server_port = std::getenv("MQTT_SERVER_PORT");
    if (mqtt_server_port == nullptr) {
        mqtt_server_port = "1883";
    }

    ::Everest::MQTTAbstraction& mqtt_abstraction =
        ::Everest::MQTTAbstraction::get_instance(mqtt_server_address, mqtt_server_port);
    mqtt_abstraction.connect();

    mqtt_abstraction.spawn_main_loop_thread();

    std::vector<ModuleStartInfo> modules_to_start;
    std::map<std::string, bool> modules_ready;
    std::mutex modules_ready_mutex;
    std::vector<Token> tokens;

    auto main_config = config->get_main_config();
    modules_to_start.reserve(main_config.size());

    for (const auto& module : main_config.items()) {
        std::string module_name = module.key();
        if (std::any_of(ignored_modules.begin(), ignored_modules.end(),
                        [module_name](const auto& element) { return element == module_name; })) {
            EVLOG(info) << fmt::format("Ignoring module: {}", module_name);
            continue;
        }
        std::string module_type = main_config[module_name]["module"];
        modules_ready[module_name] = false;

        Handler module_ready_handler = [module_name, &modules_ready, &modules_ready_mutex,
                                        &mqtt_abstraction](nlohmann::json json) {
            EVLOG(debug) << fmt::format("received module ready signal for module: {}({})", module_name, json.dump());
            std::unique_lock<std::mutex> lock(modules_ready_mutex);
            modules_ready[module_name] = json.get<bool>();
            for (const auto& mod : modules_ready) {
                std::string text_ready = fmt::format((mod.second) ? TERMINAL_STYLE_OK : TERMINAL_STYLE_ERROR, "ready");
                EVLOG(debug) << fmt::format("  {}: {}", mod.first, text_ready);
            }
            if (std::all_of(modules_ready.begin(), modules_ready.end(),
                            [](const auto& element) { return element.second; })) {
                EVLOG(info) << "all modules are ready";
                mqtt_abstraction.publish("everest/ready", nlohmann::json(true));
            }
            // FIXME (aw): this unlock shouldn't be necessary because lock will get destroyed here anyway
            lock.unlock();
        };

        std::string topic = fmt::format("{}/ready", config->mqtt_module_prefix(module_name));

        auto token =
            std::make_shared<TypedHandler>(HandlerType::ExternalMQTT, std::make_shared<Handler>(module_ready_handler));

        mqtt_abstraction.register_handler(topic, token, false, QOS::QOS2);
        tokens.push_back(token);

        if (std::any_of(standalone_modules.begin(), standalone_modules.end(),
                        [module_name](const auto& element) { return element == module_name; })) {
            EVLOG(info) << fmt::format("Not starting standalone module: {}", module_name);
            continue;
        }

        std::string binary_filename = fmt::format("{}", module_type);
        std::string javascript_library_filename = "index.js";
        boost::filesystem::path module_path = rs.modules_dir / module_type;
        const auto printable_module_name = config->printable_identifier(module_name);
        boost::filesystem::path binary_path = module_path / binary_filename;
        boost::filesystem::path javascript_library_path = module_path / javascript_library_filename;

        if (boost::filesystem::exists(binary_path)) {
            EVLOG(debug) << fmt::format("module: {} ({}) provided as binary", module_name, module_type);
            modules_to_start.emplace_back(module_name, printable_module_name, ModuleStartInfo::Language::cpp,
                                          binary_path);
        } else if (boost::filesystem::exists(javascript_library_path)) {
            EVLOG(debug) << fmt::format("module: {} ({}) provided as javascript library", module_name, module_type);
            modules_to_start.emplace_back(module_name, printable_module_name, ModuleStartInfo::Language::javascript,
                                          boost::filesystem::canonical(javascript_library_path));
        } else {
            EVLOG(error) << fmt::format(
                "module: {} ({}) cannot be loaded because no C++ or JavaScript library has been found", module_name,
                module_type);
            EVLOG(error) << fmt::format("  checked paths:");
            EVLOG(error) << fmt::format("    cpp: {}", binary_path.string());
            EVLOG(error) << fmt::format("    js:  {}", javascript_library_path.string());

            return EXIT_FAILURE;
        }
    }

    const auto children = start_modules(modules_to_start, rs);

    int status = 0;
    bool running = true;
    while (running) {
        pid_t child_pid = waitpid(-1, &status, 0);
        std::string child_name = "manager";
        if (child_pid != -1 && children.count(child_pid) != 0) {
            child_name = children.at(child_pid);
        }
        EVLOG(critical) << fmt::format(
            "Something happened to child: {} (pid: {}). Status: {}. Terminating all children.", child_name, child_pid,
            status);
        running = false;
        for (const auto& child : children) {
            if (child.first == -1) {
                EVLOG(error) << "Child with pid -1 in list of children, this cannot be correct and won't be "
                                "terminated.";
                continue;
            }
            int result = kill(child.first, SIGTERM);
            if (result != 0) {
                EVLOG(critical) << fmt::format("SIGTERM of child: {} (pid: {}) {}: {}. Escalating to SIGKILL",
                                               child.second, child.first, fmt::format(TERMINAL_STYLE_ERROR, "failed"),
                                               result);
                result = kill(child.first, SIGKILL);
                if (result != 0) {
                    EVLOG(critical) << fmt::format("SIGKILL of child: {} (pid: {}) {}: {}.", child.second, child.first,
                                                   fmt::format(TERMINAL_STYLE_ERROR, "failed"), result);
                } else {
                    EVLOG(info) << fmt::format("SIGKILL of child: {} (pid: {}) {}.", child.second, child.first,
                                               fmt::format(TERMINAL_STYLE_OK, "succeeded"));
                }
            } else {
                EVLOG(info) << fmt::format("SIGTERM of child: {} (pid: {}) {}.", child.second, child.first,
                                           fmt::format(TERMINAL_STYLE_OK, "succeeded"));
            }
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    po::options_description desc("EVerest manager");
    desc.add_options()("help,h", "produce help message");
    desc.add_options()("check", "Check and validate all config files and exit (0=success)");
    desc.add_options()("dump", po::value<std::string>(),
                       "Dump validated and augmented main config and all used module manifests into dir");
    desc.add_options()("dumpmanifests", po::value<std::string>(),
                       "Dump manifests of all modules into dir (even modules not used in config) and exit");
    desc.add_options()("main_dir", po::value<std::string>()->default_value("/usr/lib/everest"),
                       "set dir in which the main binaries reside");
    desc.add_options()("schemas_dir", po::value<std::string>(), "set dir in which the schemes folder resides");
    desc.add_options()("modules_dir", po::value<std::string>(), "set dir in which the modules reside ");
    desc.add_options()("interfaces_dir", po::value<std::string>(), "set dir in which the classes reside ");
    desc.add_options()("standalone,s", po::value<std::vector<std::string>>()->multitoken(),
                       "Module ID(s) to not automatically start child processes for (those must be started manually to "
                       "make the framework start!).");
    desc.add_options()("ignore", po::value<std::vector<std::string>>()->multitoken(),
                       "Module ID(s) to ignore: Do not automatically start child processes and do not require that "
                       "they are started.");
    desc.add_options()("dontvalidateschema", "Don't validate json schema on every message");
    desc.add_options()("log_conf", po::value<std::string>(), "The path to a custom logging.ini");
    desc.add_options()("conf", po::value<std::string>(), "The path to a custom config.json");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") != 0) {
            desc.print(std::cout);
            return EXIT_SUCCESS;
        }

        return boot(vm);

    } catch (const std::exception& e) {
        EVLOG(error) << "Main manager process exits because of caught exception:\n" << e.what();
        return EXIT_FAILURE;
    }
}
