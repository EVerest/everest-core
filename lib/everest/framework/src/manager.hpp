#pragma once

#include <boost/program_options/variables_map.hpp>
#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace Everest {
class ManagerConfig;
class MQTTAbstraction;
class StatusFifo;
struct ManagerSettings;
namespace system {
class SignalPolling;
}
} // namespace Everest
struct TypedHandler;

class ManagerAdminPanel;

struct ModuleShutdownInfo {
    std::string id;
    int wstatus;
};

/// \brief Runtime state of the manager event loop.
enum class ManagerState {
    Initializing,
    StartingModules,
    Running,
    RestartRequested,
    CrashShutdownInProgress,
    ShutdownRequested,
    ForceTerminating,
    ShutdownFinalizing,
    Idle,
    Exiting
};

/// \brief Logical cause of the current shutdown flow.
enum class ShutdownCause {
    None,
    Normal,
    Restart,
    Crash
};

// State transition map (high level):
// - Initializing -> StartingModules -> Running (after all required modules report ready)
// - StartingModules/Running + unexpected child exit -> ShutdownRequested -> CrashShutdownInProgress
// - Admin restart request -> RestartRequested (while modules drain)
// - ShutdownRequested/CrashShutdownInProgress/RestartRequested timeout -> ForceTerminating
// - Any shutdown-flow state + all modules drained -> ShutdownFinalizing
// - ShutdownFinalizing -> (restart/crash-recovery) StartingModules, or (normal finish) Idle/Exiting
// ---------------------------------------------------------
// Notes:
// - Shutdown cause intent is tracked separately via ShutdownCause to survive transient states
//   like ForceTerminating/ShutdownFinalizing.

class Manager {
public:
    /// \brief Construct manager with parsed CLI arguments.
    /// \param vm Parsed command line options used by manager startup/runtime.
    explicit Manager(const boost::program_options::variables_map& vm);

    /// \brief Start manager lifecycle and main event loop.
    /// \return Process exit code (EXIT_SUCCESS / EXIT_FAILURE).
    int run();

private:
    /// \brief Ready-subscription tracking for a single module.
    struct ModuleReadyInfo {
        bool ready{false};
        std::shared_ptr<TypedHandler> ready_token;
        std::shared_ptr<TypedHandler> get_config_token;
    };
    using ModulesReadyType = std::unordered_map<std::string, ModuleReadyInfo>;

    // Per-run dependencies passed through handlers to avoid long parameter lists
    // while keeping runtime data explicit (instead of hidden mutable members).
    /// \brief Aggregates runtime dependencies used across handlers for one run.
    struct RuntimeContext {
        std::shared_ptr<Everest::ManagerConfig>& config;
        Everest::MQTTAbstraction& mqtt_abstraction;
        const std::vector<std::string>& ignored_modules;
        const std::vector<std::string>& standalone_modules;
        const Everest::ManagerSettings& ms;
        Everest::StatusFifo& status_fifo;
        bool retain_topics;
    };

    /// \brief Outcome of one lifecycle state-advance evaluation.
    struct LifecycleAdvanceResult {
        enum class Status {
            NoTransition,
            TransitionApplied,
            ExitRequested
        };
        Status status{Status::NoTransition};
        std::optional<int> exit_code{};

        static LifecycleAdvanceResult noTransition() {
            return {Status::NoTransition, std::nullopt};
        }
        static LifecycleAdvanceResult transitionApplied() {
            return {Status::TransitionApplied, std::nullopt};
        }
        static LifecycleAdvanceResult exitRequested(int code) {
            return {Status::ExitRequested, code};
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup/helpers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// \brief Load and validate manager configuration from current boot source.
    /// \param ms Fully resolved manager settings for this run.
    /// \return Shared validated configuration object.
    std::shared_ptr<Everest::ManagerConfig> loadAndValidateConfig(const Everest::ManagerSettings& ms) const;

    /// \brief Create MQTT abstraction, connect, and spawn its main loop thread.
    /// \param ms Fully resolved manager settings for this run.
    /// \return Connected MQTT abstraction, or nullptr on connection failure.
    std::unique_ptr<Everest::MQTTAbstraction> createAndConnectMqtt(const Everest::ManagerSettings& ms) const;

    /// \brief Collect standalone module ids from config plus CLI overrides.
    /// \param config Validated manager configuration for this run.
    /// \return Module ids that manager should not spawn automatically.
    std::vector<std::string> collectStandaloneModules(const Everest::ManagerConfig& config) const;

    /// \brief Collect ignored module ids from CLI overrides.
    /// \return Module ids that manager should ignore entirely during startup.
    std::vector<std::string> collectIgnoredModules() const;

    /// \brief Publish interfaces/types/settings/manifests metadata on MQTT.
    /// \param ctx Runtime dependencies for the current run.
    void publishStartupMetadata(const RuntimeContext& ctx) const;

    /// \brief Unregister all module ready handlers and clear ready-tracking state.
    void unregisterModuleReadyHandlers(Everest::ManagerConfig& config, Everest::MQTTAbstraction& mqtt_abstraction);

    /// \brief Terminate remaining module processes (SIGTERM, then SIGKILL fallback).
    void shutdownModules(const std::map<pid_t, std::string>& modules, Everest::ManagerConfig& config,
                         Everest::MQTTAbstraction& mqtt_abstraction);

    /// \brief Convert ManagerState enum to a readable string for logs.
    const char* stateToString(ManagerState state) const;

    /// \brief Apply state transition with transition logging.
    void transitionTo(ManagerState new_state);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // State predicates
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// \brief True only when manager is fully running.
    bool areModulesStarted() const;
    /// \brief True when manager is in any shutdown-related state.
    bool isInShutdownFlowState() const;
    /// \brief True when restart has been requested.
    bool isRestartRequested() const;
    /// \brief True when crash-shutdown flow is active.
    bool isCrashInProgress() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // State/event handlers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// \brief Handle module startup: transition to StartingModules, register ready handlers, and spawn modules.
    /// \param ctx Runtime dependencies for the current run.
    /// \return Mapping of spawned child pid to module id.
    std::map<pid_t, std::string> handleStartModules(const RuntimeContext& ctx);

    /// \brief Advance lifecycle state when current phase is complete.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return Result containing transition/exit outcome for this evaluation step.
    LifecycleAdvanceResult advanceLifecycleStateIfReady(RuntimeContext& ctx, ManagerAdminPanel& admin_panel);

    /// \brief Complete shutdown finalization according to preserved restart/crash intent.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \param restart_requested Preserved restart intent for this finalization step.
    /// \param crash_in_progress Preserved crash-recovery intent for this finalization step.
    /// \return Exit code when manager should terminate, std::nullopt otherwise.
    std::optional<int> handleFinalizeShutdownTransition(RuntimeContext& ctx, ManagerAdminPanel& admin_panel,
                                                        bool restart_requested, bool crash_in_progress);

    /// \brief Reload config and initiate module restart sequence.
    void handleRestartModulesAfterShutdown(RuntimeContext& ctx);

    /// \brief Finalize normal shutdown and decide exit vs idle outcome.
    /// \param mqtt_abstraction Active MQTT abstraction instance.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return EXIT_SUCCESS/EXIT_FAILURE when manager exits, std::nullopt for idle mode.
    std::optional<int> handleFinishNormalShutdown(Everest::MQTTAbstraction& mqtt_abstraction,
                                                  ManagerAdminPanel& admin_panel);

    /// \brief Finalize crash-recovery shutdown path.
    void handleFinishCrashRecovery(Everest::MQTTAbstraction& mqtt_abstraction);

    /// \brief Start graceful shutdown and publish shutdown topic when required.
    /// \param module_exited_time Timestamp used as shutdown start reference.
    /// \param publish_when_sigint_received Whether to publish shutdown topic even after SIGINT.
    /// \param info_log Optional critical log message emitted before publish.
    /// \param mqtt_abstraction Active MQTT abstraction instance.
    /// \param ms Fully resolved manager settings for this run.
    void handleInitiateGracefulShutdown(const std::chrono::system_clock::time_point& module_exited_time,
                                        bool publish_when_sigint_received, const std::string* info_log,
                                        Everest::MQTTAbstraction& mqtt_abstraction, const Everest::ManagerSettings& ms);

    /// \brief Enforce shutdown timeout and force-terminate remaining modules.
    /// \param ctx Runtime dependencies for the current run.
    void handleShutdownTimeout(RuntimeContext& ctx);

    /// \brief Poll waitpid once and dispatch child exit handling.
    /// \param wstatus waitpid status output parameter.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return true when loop should short-circuit/continue after handling.
    bool handleWaitpidEvent(int& wstatus, RuntimeContext& ctx, ManagerAdminPanel& admin_panel);

    /// \brief Handle one child exit and update shutdown/restart state.
    /// \param pid Exited child process id.
    /// \param wstatus waitpid status for the exited child.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return true when loop should short-circuit/continue after handling.
    bool handleChildExit(pid_t pid, int wstatus, RuntimeContext& ctx, ManagerAdminPanel& admin_panel);

    /// \brief Poll controller IPC commands (restart/check-config).
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \param prefix_opt Prefix passed to config-check requests.
    /// \return Exit code when manager should terminate, std::nullopt otherwise.
    std::optional<int> handleControllerIpcPoll(RuntimeContext& ctx, ManagerAdminPanel& admin_panel,
                                               const std::string& prefix_opt);

    /// \brief Handle SIGINT/SIGTERM transition and optional immediate exit.
    /// \param signo Signal number received by polling.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return Exit code when manager should terminate, std::nullopt otherwise.
    std::optional<int> handleSignal(int signo, RuntimeContext& ctx, ManagerAdminPanel& admin_panel);

    /// \brief Poll signal fd and forward handled signals to handleSignal().
    /// \param signal_polling Signal polling abstraction used by manager loop.
    /// \param ctx Runtime dependencies for the current run.
    /// \param admin_panel Controller IPC/process integration helper.
    /// \return Exit code when manager should terminate, std::nullopt otherwise.
    std::optional<int> handleSignalPoll(Everest::system::SignalPolling& signal_polling, RuntimeContext& ctx,
                                        ManagerAdminPanel& admin_panel);

    const boost::program_options::variables_map& vm_;
    ManagerState state_{ManagerState::Idle};
    ShutdownCause shutdown_cause_{ShutdownCause::None};
    bool sigint_received_{false};
    std::uint8_t unexpected_module_exit_count_{0};
    std::optional<std::chrono::system_clock::time_point> shutdown_start_time_;
    std::map<pid_t, std::string> module_handles_;
    std::vector<ModuleShutdownInfo> shutdown_info_;
    ModulesReadyType modules_ready_; // guarded by modules_ready_mutex_
    std::mutex modules_ready_mutex_;
};
