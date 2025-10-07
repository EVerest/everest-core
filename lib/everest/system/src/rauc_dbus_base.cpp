// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <everest/system/dbus_base.hpp>
#include <everest/system/rauc_dbus_base.hpp>
#include <everest/system/safe_system.hpp>

#include <string>
#include <string_view>

namespace {
using namespace everest::lib::system;
using namespace everest::lib::system::rauc_dbus;
using namespace everest::lib::system::rauc_dbus::rauc_messages;

sdbus::PendingAsyncCall get_property_async(RaucBase::property_cb handler, const std::unique_ptr<sdbus::IProxy>& proxy,
                                           const char* property, const char* interface) {
    sdbus::PendingAsyncCall result;
    try {
        result = dbus::getPropertyAsync(proxy, handler, property, interface);
    } catch (const sdbus::Error& e) {
        EVLOG_error << "getPropertyAsync '" << property << "': " << e.what();
    }
    return result;
}

sdbus::PendingAsyncCall call_method_async(RaucBase::method_cb handler, const std::unique_ptr<sdbus::IProxy>& proxy,
                                          const char* interface, const char* method, uint64_t timeout_us) {
    sdbus::PendingAsyncCall result;
    try {
        auto m = dbus::createMethodCall(proxy, interface, method);
        result = dbus::callMethodAsync(proxy, m, handler, timeout_us);
    } catch (const sdbus::Error& e) {
        EVLOG_error << "callMethodAsync '" << method << "': " << e.what();
    }
    return result;
}

template <typename T>
CmdResult get_property(T& result, const std::unique_ptr<sdbus::IProxy>& proxy, const char* property,
                       const char* interface) {
    CmdResult res{false, {}};
    try {
        result = proxy->getProperty(property).onInterface(interface).get<T>();
        res.success = true;
    } catch (const sdbus::Error& e) {
        EVLOG_error << "getProperty '" << property << "': " << e.what();
        res.error_msg = e.getMessage();
    }
    return res;
}

template <typename T>
CmdResult call_method(T& result, const std::unique_ptr<sdbus::IProxy>& proxy, sdbus::MethodCall& method,
                      const char* m_str, uint64_t timeout_us) {
    CmdResult res{false, {}};
    try {
        auto reply = proxy->callMethod(method, timeout_us);
        if (!reply.isEmpty()) {
            reply >> result;
            res.success = true;
        }
    } catch (const sdbus::Error& e) {
        EVLOG_error << "callMethod '" << m_str << "': " << e.what();
        res.error_msg = e.getMessage();
    }
    return res;
}

CmdResult call_method(const std::unique_ptr<sdbus::IProxy>& proxy, sdbus::MethodCall& method, const char* m_str,
                      uint64_t timeout_us) {
    CmdResult res{false, {}};
    try {
        proxy->callMethod(method, timeout_us);
        res.success = true;
    } catch (const sdbus::Error& e) {
        EVLOG_error << "callMethod '" << m_str << "': " << e.what();
        res.error_msg = e.getMessage();
    }
    return res;
}

} // namespace

namespace everest::lib::system::rauc_dbus {

namespace rauc_messages {

sdbus::Message& operator<<(sdbus::Message& msg, const Progress& items) {
    return msg << sdbus::Struct{std::forward_as_tuple(items.percent, items.description, items.level)};
}

sdbus::Message& operator>>(sdbus::Message& msg, Progress& items) {
    sdbus::Struct s{std::forward_as_tuple(items.percent, items.description, items.level)};
    return msg >> s;
}

Operation string_to_operation(const std::string_view& s) {
    if (s == "idle") {
        return rauc_messages::Operation::Idle;
    }
    if (s == "installing") {
        return rauc_messages::Operation::Installing;
    }
    if (!s.empty()) {
        EVLOG_warning << "Unknown rauc operation: " << s;
    }
    return rauc_messages::Operation::Unknown;
}

} // namespace rauc_messages

// ----------------------------------------------------------------------------
// class RaucBase

RaucBase::RaucBase() {
    // Create proxy object, this will start an internal event loop thread
    proxy = dbus::createProxy(defaults::service_domain, defaults::object_path);
}

RaucBase::RaucBase(sdbus::dont_run_event_loop_thread_t) {
    // Create proxy object, using an external event loop
    proxy = dbus::createProxy(defaults::service_domain, defaults::object_path, sdbus::dont_run_event_loop_thread);
}

void RaucBase::configure(const std::string& verify_update_script_path) {
    this->verify_update_script_path = verify_update_script_path;
    try {
        dbus::initialise_handlers(proxy, [this]() { configure_handlers(); });
    } catch (const sdbus::Error& e) {
        EVLOG_error << "RaucBase::configure: " << e.getMessage();
    }
}

// Call on boot and pass a previous transaction that was not closed yet
bool RaucBase::check_previous_transaction(const CurrentState& current, const rauc_messages::UpdateTransaction& saved) {
    bool result{false};

    const std::string state = "boot=" + current.boot_slot + " primary=" + current.primary_slot;
    const std::string transaction = "transaction boot=" + saved.boot_slot + " primary=" + saved.primary_slot +
                                    " id=" + std::to_string(saved.request_id);
    if (decide_if_good(saved, current)) {
        EVLOG_info << "We have booted into the new updated slot (" << state << "), update was successful ("
                   << transaction << ") Marking good";
        // Mark this slot as good in RAUC
        result = true;
    } else {
        EVLOG_error << "We have booted into the old fall back slot (" << state << "), update was not successful ("
                    << transaction << ')';
    }

    return result;
}

sdbus::PendingAsyncCall RaucBase::get_boot_slot(property_cb handler) const {
    return get_property_async(handler, proxy, property::BootSlot, interface::Installer);
}

sdbus::PendingAsyncCall RaucBase::install_bundle(method_cb handler, const std::string& filename, uint64_t timeout_us) {
    sdbus::PendingAsyncCall result;
    try {
        auto method = dbus::createMethodCall(proxy, interface::Installer, method::InstallBundle);
        std::map<std::string, sdbus::Variant> parameters;
        method << filename << parameters;
        result = dbus::callMethodAsync(proxy, method, handler, timeout_us);
    } catch (const sdbus::Error& e) {
        EVLOG_error << "RaucBase::install_bundle: " << e.what();
    }
    return result;
}

sdbus::PendingAsyncCall RaucBase::mark(method_cb handler, const std::string& mark, const std::string& slot,
                                       uint64_t timeout_us) {
    sdbus::PendingAsyncCall result;
    try {
        auto method = dbus::createMethodCall(proxy, interface::Installer, method::Mark);
        method << mark << slot;
        result = dbus::callMethodAsync(proxy, method, handler, timeout_us);
    } catch (const sdbus::Error& e) {
        EVLOG_error << "RaucBase::mark: " << e.what();
    }
    return result;
}

RaucBase::slot_info_t RaucBase::convert(slots_t& slots) {
    slot_info_t result;
    for (const auto& [n, v] : slots) {
        std::map<std::string, std::string> nv_dict;
        for (const auto& [nn, vv] : v) {
            try {
                if (nn == "size") {
                    nv_dict[nn] = std::to_string(vv.get<std::uint64_t>());
                } else if (nn == "installed.count") {
                    nv_dict[nn] = std::to_string(vv.get<std::uint32_t>());
                } else {
                    nv_dict[nn] = vv.get<std::string>();
                }
            } catch (...) {
                nv_dict[nn] = {};
            }
        }
        result[n] = std::move(nv_dict);
    }
    return result;
}

rauc_messages::HealthCheckStatus RaucBase::check_system_health() {
    // checking the success or failure of an OTA update following reboot
    using namespace rauc_messages;
    if (verify_update_script_path.empty()) {
        return HealthCheckStatus::ScriptNotSet;
    }
    if (access(verify_update_script_path.c_str(), X_OK) != 0) {
        EVLOG_error << "Health check script '" << verify_update_script_path << "' not executable";
        return HealthCheckStatus::ScriptNotExecutable;
    }
    const auto res = everest::lib::system::safe_system(verify_update_script_path, {verify_update_script_path});
    switch (res.status) {
    case everest::lib::system::CMD_SETUP_FAILED:
        EVLOG_error << "Health check script '" << verify_update_script_path << "' setup failed, errno: " << res.code;
        return HealthCheckStatus::SetupFailed;
    case everest::lib::system::CMD_TERMINATED:
        EVLOG_error << "Health check script '" << verify_update_script_path << "' terminated by signal: " << res.code;
        return HealthCheckStatus::ScriptTerminatedBySignal;
    case everest::lib::system::CMD_SUCCESS:
        if (res.code == 0) {
            EVLOG_debug << "Health check script '" << verify_update_script_path << "' returned success";
            return HealthCheckStatus::Success;
        } else {
            EVLOG_error << "Health check script '" << verify_update_script_path
                        << "' returned error code: " << res.code;
            return HealthCheckStatus::ScriptExitedWithError;
        }
    }
    // Should not reach here
    return HealthCheckStatus::UnknownError;
}

sdbus::PendingAsyncCall RaucBase::get_operation(property_cb handler) const {
    return get_property_async(handler, proxy, property::Operation, interface::Installer);
}

sdbus::PendingAsyncCall RaucBase::get_last_error(property_cb handler) const {
    return get_property_async(handler, proxy, property::LastError, interface::Installer);
}

sdbus::PendingAsyncCall RaucBase::get_primary_slot(method_cb handler, uint64_t timeout_us) {
    return call_method_async(handler, proxy, interface::Installer, method::GetPrimary, timeout_us);
}

sdbus::PendingAsyncCall RaucBase::get_slot_status(method_cb handler, uint64_t timeout_us) {
    return call_method_async(handler, proxy, interface::Installer, method::GetSlotStatus, timeout_us);
}

sdbus::PendingAsyncCall RaucBase::get_progress(property_cb handler) const {
    return get_property_async(handler, proxy, property::Progress, interface::Installer);
}

bool RaucBase::decide_if_good(const rauc_messages::UpdateTransaction& saved, const CurrentState& current) {
    // check that the boot slot has changed and update script ran successfully or was not set
    if (saved.boot_slot == current.boot_slot) {
        return false;
    }
    if (current.system_health_rc != rauc_messages::HealthCheckStatus::Success and
        current.system_health_rc != rauc_messages::HealthCheckStatus::ScriptNotSet) {
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------------
// class RaucBaseSync

rauc_messages::UpdateTransaction RaucBaseSync::create_transaction(std::int32_t request_id, uint64_t timeout_us) {
    return {request_id, get_boot_slot(), get_primary_slot(timeout_us)};
}

bool RaucBaseSync::check_previous_transaction(const rauc_messages::UpdateTransaction& saved, uint64_t timeout_us) {
    const CurrentState current{check_system_health(), get_boot_slot(), get_primary_slot(timeout_us)};
    const auto res = RaucBase::check_previous_transaction(current, saved);
    if (res) {
        mark("good", "booted", timeout_us);
    }
    return res;
}

rauc_messages::CmdResult RaucBaseSync::install_bundle(const std::string& filename, uint64_t timeout_us) {
    auto method = dbus::createMethodCall(proxy, interface::Installer, method::InstallBundle);
    std::map<std::string, sdbus::Variant> parameters;
    method << filename << parameters;
    return call_method(proxy, method, method::InstallBundle, timeout_us);
}

void RaucBaseSync::mark(const std::string& mark, const std::string& slot, uint64_t timeout_us) {
    auto method = dbus::createMethodCall(proxy, interface::Installer, method::Mark);
    method << mark << slot;
    call_method(proxy, method, method::Mark, timeout_us);
}

std::string RaucBaseSync::get_boot_slot() const {
    std::string result;
    get_property(result, proxy, property::BootSlot, interface::Installer);
    return result;
}

std::string RaucBaseSync::get_primary_slot(uint64_t timeout_us) {
    std::string result;
    auto method = dbus::createMethodCall(proxy, interface::Installer, method::GetPrimary);
    call_method(result, proxy, method, method::GetPrimary, timeout_us);
    return result;
}

std::map<std::string, std::map<std::string, std::string>> RaucBaseSync::get_slot_status(uint64_t timeout_us) {
    // a(sa{sv})
    slots_t slots;
    auto method = dbus::createMethodCall(proxy, interface::Installer, method::GetSlotStatus);
    call_method(slots, proxy, method, method::GetSlotStatus, timeout_us);
    return convert(slots);
}

std::string RaucBaseSync::get_last_error() const {
    std::string result;
    get_property(result, proxy, property::LastError, interface::Installer);
    return result;
}

rauc_messages::Operation RaucBaseSync::get_operation() const {
    std::string result;
    get_property(result, proxy, property::Operation, interface::Installer);
    return rauc_messages::string_to_operation(result);
}

rauc_messages::Progress RaucBaseSync::get_progress() const {
    rauc_messages::Progress result{0, {}, 0};
    get_property(result, proxy, property::Progress, interface::Installer);
    return result;
}

// ----------------------------------------------------------------------------
// class RaucBaseAsync

bool RaucBaseAsync::install_bundle(const std::string& filename, uint64_t timeout_us) {
    auto handler = [this](sdbus::MethodReply reply, std::optional<sdbus::Error> error) {
        if (error) {
            cb_error(error_t::install_bundle, error->getMessage());
        } else {
            cb_install_bundle();
        }
    };
    active_request = RaucBase::install_bundle(handler, filename, timeout_us);
    return active_request.isPending();
}

bool RaucBaseAsync::mark(const std::string& mark, const std::string& slot, uint64_t timeout_us) {
    auto handler = [this](sdbus::MethodReply reply, std::optional<sdbus::Error> error) {
        if (error) {
            cb_error(error_t::mark, error->getMessage());
        } else {
            cb_mark();
        }
    };
    active_request = RaucBase::mark(handler, mark, slot, timeout_us);
    return active_request.isPending();
}

bool RaucBaseAsync::get_boot_slot() {
    auto handler = [this](std::optional<sdbus::Error> error, sdbus::Variant value) {
        if (error) {
            cb_error(error_t::boot_slot, error.value().getMessage());
        } else {
            try {
                cb_boot_slot(value.get<std::string>());
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::boot_slot, e_str);
            }
        }
    };
    active_request = RaucBase::get_boot_slot(handler);
    return active_request.isPending();
}

bool RaucBaseAsync::get_primary_slot(uint64_t timeout_us) {
    auto handler = [this](sdbus::MethodReply reply, std::optional<sdbus::Error> error) {
        if (error) {
            cb_error(error_t::primary_slot, error->getMessage());
        } else {
            try {
                std::string slot;
                reply >> slot;
                cb_primary_slot(slot);
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::primary_slot, e_str);
            }
        }
    };
    active_request = RaucBase::get_primary_slot(handler, timeout_us);
    return active_request.isPending();
}

bool RaucBaseAsync::get_slot_status(uint64_t timeout_us) {
    auto handler = [this](sdbus::MethodReply reply, std::optional<sdbus::Error> error) {
        if (error) {
            cb_error(error_t::slot_status, error->getMessage());
        } else {
            try {
                slots_t slots;
                reply >> slots;
                cb_slot_status(convert(slots));
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::slot_status, e_str);
            }
        }
    };
    active_request = RaucBase::get_slot_status(handler, timeout_us);
    return active_request.isPending();
}

bool RaucBaseAsync::get_last_error() {
    auto handler = [this](std::optional<sdbus::Error> error, sdbus::Variant value) {
        if (error) {
            cb_error(error_t::last_error, error.value().getMessage());
        } else {
            try {
                cb_last_error(value.get<std::string>());
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::last_error, e_str);
            }
        }
    };
    active_request = RaucBase::get_last_error(handler);
    return active_request.isPending();
}

bool RaucBaseAsync::get_operation() {
    auto handler = [this](std::optional<sdbus::Error> error, sdbus::Variant value) {
        if (error) {
            cb_error(error_t::operation, error.value().getMessage());
        } else {
            try {
                cb_operation(string_to_operation(value.get<std::string>()));
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::operation, e_str);
            }
        }
    };
    active_request = RaucBase::get_operation(handler);
    return active_request.isPending();
}

bool RaucBaseAsync::get_progress() {
    auto handler = [this](std::optional<sdbus::Error> error, sdbus::Variant value) {
        if (error) {
            cb_error(error_t::progress, error.value().getMessage());
        } else {
            try {
                cb_progress(value.get<rauc_messages::Progress>());
            } catch (const sdbus::Error& e) {
                std::string e_str = "Exception: " + e.getMessage();
                cb_error(error_t::progress, e_str);
            }
        }
    };
    active_request = RaucBase::get_progress(handler);
    return active_request.isPending();
}

} // namespace everest::lib::system::rauc_dbus
