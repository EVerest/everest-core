// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef LINUX_SYSTEMD_RAUC_HPP
#define LINUX_SYSTEMD_RAUC_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/system/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/kvs/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "rauc_dbus.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    double DefaultRetries;
    double DefaultRetryInterval;
    std::string OCPPLogPath;
    std::string SessionLogPath;
    std::string RebootCommand;
    std::string VerifyUpdateScriptPath;
};

class Linux_Systemd_Rauc : public Everest::ModuleBase {
public:
    Linux_Systemd_Rauc() = delete;
    Linux_Systemd_Rauc(const ModuleInfo& info, std::unique_ptr<systemImplBase> p_main, std::unique_ptr<kvsIntf> r_store,
                       Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_store(std::move(r_store)), config(config){};

    const std::unique_ptr<systemImplBase> p_main;
    const std::unique_ptr<kvsIntf> r_store;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    Rauc rauc;
    void firmware_update_may_proceed_with_reboot_callback();
    void install_firmware_bundle(const std::string& filename, int32_t request_id);
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    std::string store_path;

    std::recursive_mutex firmware_update_progress_mx;
    bool firmware_update_waiting_for_ocpp_unblocking =
        false; // Set to true in case of OTA update via OCPP; set to false when OCPP has signaled that installation may
               // proceed
    bool firmware_update_reboot_scheduled = false; // Set to true once a firmware update is installed but a restart has
                                                   // been blocked by firmware_update_waiting_for_ocpp_unblocking
    void reboot_after_firmware_update();
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // LINUX_SYSTEMD_RAUC_HPP
