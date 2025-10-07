// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include <everest/system/dbus_base.hpp>

#include <cstdint>
#include <string>
#include <string_view>

namespace everest::lib::system::rauc_dbus {

namespace defaults {
constexpr const char* service_domain = "de.pengutronix.rauc";
constexpr const char* object_path = "/";
} // namespace defaults

namespace interface {
constexpr const char* Installer = "de.pengutronix.rauc.Installer";
constexpr const char* DBus_Properties = "org.freedesktop.DBus.Properties";
} // namespace interface

namespace property {
constexpr const char* Progress = "Progress";
constexpr const char* Operation = "Operation";
constexpr const char* LastError = "LastError";
constexpr const char* BootSlot = "BootSlot";
} // namespace property

namespace signal {
constexpr const char* PropertiesChanged = "PropertiesChanged";
constexpr const char* Completed = "Completed";
} // namespace signal

namespace method {
constexpr const char* InstallBundle = "InstallBundle";
constexpr const char* InspectBundle = "InspectBundle";
constexpr const char* Mark = "Mark";
constexpr const char* GetSlotStatus = "GetSlotStatus";
constexpr const char* GetPrimary = "GetPrimary";
} // namespace method

namespace rauc_messages {
struct Progress {
    int percent;
    std::string description;
    int level;
};

struct CmdResult {
    bool success;
    std::string error_msg;
};

enum class Operation {
    Unknown,
    Idle,
    Installing
};

struct UpdateTransaction {
    std::int32_t request_id;
    std::string boot_slot;
    std::string primary_slot;
};

enum class HealthCheckStatus {
    ScriptExitedWithError,
    ScriptNotExecutable,
    ScriptNotSet,
    ScriptTerminatedBySignal,
    SetupFailed,
    Success,
    UnknownError
};

Operation string_to_operation(const std::string_view& s);
// Stream operators are needed for type conversion in sdbus
sdbus::Message& operator<<(sdbus::Message& msg, const Progress& items);
sdbus::Message& operator>>(sdbus::Message& msg, Progress& items);

} // namespace rauc_messages

// ----------------------------------------------------------------------------
// Base class

class RaucBase {
public:
    using property_cb = dbus::async_property_callback;
    using method_cb = dbus::async_method_callback;
    using slot_info_t = std::map<std::string, std::map<std::string, std::string>>;

    struct CurrentState {
        rauc_messages::HealthCheckStatus system_health_rc;
        std::string boot_slot;
        std::string primary_slot;
    };

    RaucBase();
    RaucBase(sdbus::dont_run_event_loop_thread_t);

    void configure(const std::string& verify_update_script_path = "");
    bool check_previous_transaction(const CurrentState& current, const rauc_messages::UpdateTransaction& saved);

    // methods
    sdbus::PendingAsyncCall install_bundle(method_cb handler, const std::string& filename, uint64_t timeout_us);
    sdbus::PendingAsyncCall mark(method_cb handler, const std::string& mark, const std::string& slot,
                                 uint64_t timeout_us);

    // properties
    sdbus::PendingAsyncCall get_boot_slot(property_cb handler) const;

protected:
    std::unique_ptr<sdbus::IProxy> proxy;

    using slots_t = std::vector<sdbus::Struct<std::string, std::map<std::string, sdbus::Variant>>>;
    static slot_info_t convert(slots_t& slots);

    rauc_messages::HealthCheckStatus check_system_health();

    // methods
    sdbus::PendingAsyncCall get_primary_slot(method_cb handler, uint64_t timeout_us);
    sdbus::PendingAsyncCall get_slot_status(method_cb handler, uint64_t timeout_us);

    // properties
    sdbus::PendingAsyncCall get_last_error(property_cb handler) const;
    sdbus::PendingAsyncCall get_operation(property_cb handler) const;
    sdbus::PendingAsyncCall get_progress(property_cb handler) const;

    virtual bool decide_if_good(const rauc_messages::UpdateTransaction& saved, const CurrentState& current);
    virtual void configure_handlers() = 0;

private:
    std::string verify_update_script_path;
};

// ----------------------------------------------------------------------------
// Synchronous base class

class RaucBaseSync : public RaucBase {
public:
    using RaucBase::RaucBase;

    rauc_messages::UpdateTransaction create_transaction(std::int32_t request_id, uint64_t timeout_us);
    bool check_previous_transaction(const rauc_messages::UpdateTransaction& saved, uint64_t timeout_us);

    // methods
    rauc_messages::CmdResult install_bundle(const std::string& filename, uint64_t timeout_us);
    void mark(const std::string& mark, const std::string& slot, uint64_t timeout_us);

    // properties
    std::string get_boot_slot() const;

protected:
    // methods
    std::string get_primary_slot(uint64_t timeout_us);
    slot_info_t get_slot_status(uint64_t timeout_us);

    // properties
    std::string get_last_error() const;
    rauc_messages::Operation get_operation() const;
    rauc_messages::Progress get_progress() const;
};

// ----------------------------------------------------------------------------
// Asynchronous base class

class RaucBaseAsync : public RaucBase {
public:
    using RaucBase::RaucBase;

    // methods
    bool install_bundle(const std::string& filename, uint64_t timeout_us);
    bool mark(const std::string& mark, const std::string& slot, uint64_t timeout_us);

    // properties
    bool get_boot_slot();

protected:
    sdbus::PendingAsyncCall active_request;

    // methods
    bool get_primary_slot(uint64_t timeout_us);
    bool get_slot_status(uint64_t timeout_us);

    // properties
    bool get_last_error();
    bool get_operation();
    bool get_progress();

    // callbacks
    enum class error_t : std::uint8_t {
        install_bundle,
        mark,
        boot_slot,
        primary_slot,
        slot_status,
        last_error,
        operation,
        progress
    };

    virtual void cb_install_bundle() = 0;
    virtual void cb_mark() = 0;
    virtual void cb_boot_slot(const std::string_view& value) = 0;
    virtual void cb_primary_slot(const std::string_view& value) = 0;
    virtual void cb_slot_status(const slot_info_t& value) = 0;
    virtual void cb_last_error(const std::string_view& value) = 0;
    virtual void cb_operation(rauc_messages::Operation value) = 0;
    virtual void cb_progress(const rauc_messages::Progress& value) = 0;
    virtual void cb_error(error_t fn, const std::string_view& error) = 0;
};

} // namespace everest::lib::system::rauc_dbus

namespace sdbus {
template <>
struct signature_of<everest::lib::system::rauc_dbus::rauc_messages::Progress>
    : signature_of<Struct<int, std::string, int>> {};
} // namespace sdbus
