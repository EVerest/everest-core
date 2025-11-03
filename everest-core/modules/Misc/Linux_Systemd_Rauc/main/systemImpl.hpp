// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SYSTEM_IMPL_HPP
#define MAIN_SYSTEM_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/system/Implementation.hpp>

#include "../Linux_Systemd_Rauc.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <filesystem>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class systemImpl : public systemImplBase {
public:
    systemImpl() = delete;
    systemImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<Linux_Systemd_Rauc>& mod, Conf& config) :
        systemImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::system::UpdateFirmwareResponse
    handle_update_firmware(types::system::FirmwareUpdateRequest& firmware_update_request) override;
    virtual void handle_allow_firmware_installation() override;
    virtual types::system::UploadLogsResponse
    handle_upload_logs(types::system::UploadLogsRequest& upload_logs_request) override;
    virtual bool handle_is_reset_allowed(types::system::ResetType& type) override;
    virtual void handle_reset(types::system::ResetType& type, bool& scheduled) override;
    virtual bool handle_set_system_time(std::string& timestamp) override;
    virtual types::system::BootReason handle_get_boot_reason() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<Linux_Systemd_Rauc>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    std::filesystem::path scripts_path;
    bool log_upload_running{false};
    std::atomic_bool interrupt_log_upload;
    std::thread upload_logs_thread;
    std::mutex log_upload_mutex;
    std::condition_variable log_upload_cv;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SYSTEM_IMPL_HPP
