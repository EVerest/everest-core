// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SYSTEM_IMPL_HPP
#define MAIN_SYSTEM_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/system/Implementation.hpp>

#include "../System.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <filesystem>

#include <everest/timer.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class systemImpl : public systemImplBase {
public:
    systemImpl() = delete;
    systemImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<System>& mod, Conf& config) :
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
    const Everest::PtrContainer<System>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here

    std::filesystem::path scripts_path;

    std::atomic<bool> interrupt_firmware_download;
    std::atomic<bool> interrupt_log_upload;

    bool log_upload_running;
    bool standard_firmware_update_running;
    bool firmware_download_running;
    std::atomic<bool> firmware_installation_running;

    std::condition_variable log_upload_cv;
    std::condition_variable firmware_update_cv;

    std::mutex log_upload_mutex;
    std::mutex firmware_update_mutex;

    std::thread update_firmware_thread;
    std::thread upload_logs_thread;

    Everest::SteadyTimer standard_update_firmware_timer;
    Everest::SteadyTimer signed_firmware_update_download_timer;
    Everest::SteadyTimer signed_firmware_update_install_timer;

    std::string boot_reason_key;

    /**
     * @brief Executes a standard firmware update using the given \p firmware_update_request
     *
     * @param firmware_update_request
     */
    void standard_firmware_update(const types::system::FirmwareUpdateRequest& firmware_update_request);

    /**
     * @brief Handles the given \p firmware_update_request . If firmware update is already running, the request will be
     * rejected. If the download should not be started in the future it starts the download and installation of the
     * firmware immediately, otherwise this method sets a timer for the download accordingly.
     *
     * @param firmware_update_request
     * @return types::system::UpdateFirmwareResponse
     */
    types::system::UpdateFirmwareResponse
    handle_standard_firmware_update(const types::system::FirmwareUpdateRequest& firmware_update_request);

    /**
     * @brief Handles the given \p firmware_update_request. If the download should not be started in the future it
     * starts the download and installation of the firmware immediately, otherwise this method sets a timer for the
     * download accordingly.
     *
     * @param firmware_update_request
     */
    types::system::UpdateFirmwareResponse
    handle_signed_fimware_update(const types::system::FirmwareUpdateRequest& firmware_update_request);

    /**
     * @brief Handles the download of the firmware specified in the given \p firmware_update_request . If a download is
     * already running, this method will interrupt the download process and restart it.
     *
     * @param firmware_update_request
     */
    void download_signed_firmware(const types::system::FirmwareUpdateRequest& firmware_update_request);

    /**
     * @brief Initializes the firmware installation by starting it immediately or if specified in the \p
     * firmware_update_request it schedules it for the future.
     *
     * @param firmware_update_request
     * @param firmware_file_path
     */
    void initialize_firmware_installation(const types::system::FirmwareUpdateRequest& firmware_update_request,
                                          const std::filesystem::path& firmware_file_path);

    /**
     * @brief Executes the installation of the firmware specified in the given \p firmware_update_request .
     *
     * @param firmware_update_reqeust
     * @param firmware_file_path
     */
    void install_signed_firmware(const types::system::FirmwareUpdateRequest& firmware_update_reqeust,
                                 const std::filesystem::path& firmware_file_path);
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SYSTEM_IMPL_HPP
