// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "systemImpl.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <vector>

#include <unistd.h>

#include <utils/date.hpp>

#include <boost/process.hpp>

namespace module {
namespace main {

const std::string CONSTANTS = "constants.env";
const std::string DIAGNOSTICS_UPLOADER = "diagnostics_uploader.sh";
const std::string FIRMWARE_UPDATER = "firmware_updater.sh";
const std::string SIGNED_FIRMWARE_DOWNLOADER = "signed_firmware_downloader.sh";
const std::string SIGNED_FIRMWARE_INSTALLER = "signed_firmware_installer.sh";

namespace fs = std::filesystem;

// FIXME (aw): this function needs to be refactored into some kind of utility library
fs::path create_temp_file(const fs::path& dir, const std::string& prefix) {
    const std::string fn_template = (dir / prefix).string() + "XXXXXX" + std::string(1, '\0');
    std::vector<char> fn_template_buffer{fn_template.begin(), fn_template.end()};

    // mkstemp needs to have at least 6 XXXXXX at the end and it will replace these
    // with a valid file name
    auto fd = mkstemp(fn_template_buffer.data());

    if (fd == -1) {
        EVLOG_error << "Failed to create temporary file at: " << fn_template;
        return {};
    }

    // close the file descriptor
    close(fd);

    return fn_template_buffer.data();
}

void systemImpl::init() {
    this->scripts_path = mod->info.paths.libexec;
    this->log_upload_running = false;
    this->firmware_download_running = false;
    this->firmware_installation_running = false;
    this->standard_firmware_update_running = false;
}

void systemImpl::ready() {
}

void systemImpl::standard_firmware_update(const types::system::FirmwareUpdateRequest& firmware_update_request) {

    this->standard_firmware_update_running = true;
    EVLOG_info << "Starting firmware update";
    // create temporary file
    const auto date_time = Everest::Date::to_rfc3339(date::utc_clock::now());

    const auto firmware_file_path = create_temp_file(fs::temp_directory_path(), "firmware-" + date_time);

    if (firmware_file_path.empty()) {
        EVLOG_error << "Firmware update ignored, cannot write temporary file.";
        publish_firmware_update_status({types::system::FirmwareUpdateStatusEnum::DownloadFailed});
        return;
    }

    const auto constants = this->scripts_path / CONSTANTS;

    this->update_firmware_thread = std::thread([this, firmware_update_request, firmware_file_path, constants]() {
        const auto firmware_updater = this->scripts_path / FIRMWARE_UPDATER;

        const std::vector<std::string> args = {constants.string(), firmware_update_request.location,
                                               firmware_file_path.string()};
        int32_t retries = 0;
        const auto total_retries = firmware_update_request.retries.value_or(this->mod->config.DefaultRetries);
        const auto retry_interval =
            firmware_update_request.retry_interval_s.value_or(this->mod->config.DefaultRetryInterval);

        auto firmware_status_enum = types::system::FirmwareUpdateStatusEnum::DownloadFailed;
        types::system::FirmwareUpdateStatus firmware_status;
        firmware_status.request_id = -1;
        firmware_status.firmware_update_status = firmware_status_enum;

        while (firmware_status.firmware_update_status == types::system::FirmwareUpdateStatusEnum::DownloadFailed &&
               retries <= total_retries) {
            boost::process::ipstream stream;
            boost::process::child cmd(firmware_updater.string(), boost::process::args(args),
                                      boost::process::std_out > stream);
            std::string temp;
            retries += 1;
            while (std::getline(stream, temp)) {
                firmware_status.firmware_update_status = types::system::string_to_firmware_update_status_enum(temp);
                this->publish_firmware_update_status(firmware_status);
            }
            if (firmware_status.firmware_update_status == types::system::FirmwareUpdateStatusEnum::DownloadFailed &&
                retries <= total_retries) {
                std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
            }
            cmd.wait();
        }
        this->standard_firmware_update_running = false;
    });
    this->update_firmware_thread.detach();
}

types::system::UpdateFirmwareResponse
systemImpl::handle_standard_firmware_update(const types::system::FirmwareUpdateRequest& firmware_update_request) {

    if (!this->standard_firmware_update_running) {
        if (firmware_update_request.retrieve_timestamp.has_value() &&
            Everest::Date::from_rfc3339(firmware_update_request.retrieve_timestamp.value()) > date::utc_clock::now()) {
            const auto retrieve_timestamp =
                Everest::Date::from_rfc3339(firmware_update_request.retrieve_timestamp.value());
            this->standard_update_firmware_timer.at(
                [this, retrieve_timestamp, firmware_update_request]() {
                    this->standard_firmware_update(firmware_update_request);
                },
                retrieve_timestamp);
            EVLOG_info << "Download for firmware scheduled for: " << Everest::Date::to_rfc3339(retrieve_timestamp);
        } else {
            // start download immediately
            this->update_firmware_thread = std::thread(
                [this, firmware_update_request]() { this->standard_firmware_update(firmware_update_request); });
            this->update_firmware_thread.detach();
        }
        return types::system::UpdateFirmwareResponse::Accepted;
    } else {
        EVLOG_info << "Not starting firmware update because firmware update process already running";
        return types::system::UpdateFirmwareResponse::Rejected;
    }
}

types::system::UpdateFirmwareResponse
systemImpl::handle_signed_fimware_update(const types::system::FirmwareUpdateRequest& firmware_update_request) {

    if (!firmware_update_request.signing_certificate.has_value()) {
        EVLOG_warning << "Signing certificate is missing in FirmwareUpdateRequest";
        return types::system::UpdateFirmwareResponse::Rejected;
    }
    if (!firmware_update_request.signature.has_value()) {
        EVLOG_warning << "Signature is missing in FirmwareUpdateRequest";
        return types::system::UpdateFirmwareResponse::Rejected;
    }

    EVLOG_info << "Executing signed firmware update download callback";

    if (firmware_update_request.retrieve_timestamp.has_value() &&
        Everest::Date::from_rfc3339(firmware_update_request.retrieve_timestamp.value()) > date::utc_clock::now()) {
        const auto retrieve_timestamp = Everest::Date::from_rfc3339(firmware_update_request.retrieve_timestamp.value());
        this->signed_firmware_update_download_timer.at(
            [this, retrieve_timestamp, firmware_update_request]() {
                this->download_signed_firmware(firmware_update_request);
            },
            retrieve_timestamp);
        EVLOG_info << "Download for firmware scheduled for: " << Everest::Date::to_rfc3339(retrieve_timestamp);
        types::system::FirmwareUpdateStatus firmware_update_status;
        firmware_update_status.request_id = firmware_update_request.request_id;
        firmware_update_status.firmware_update_status = types::system::FirmwareUpdateStatusEnum::DownloadScheduled;
        this->publish_firmware_update_status(firmware_update_status);
    } else {
        // start download immediately
        this->update_firmware_thread =
            std::thread([this, firmware_update_request]() { this->download_signed_firmware(firmware_update_request); });
        this->update_firmware_thread.detach();
    }

    if (this->firmware_download_running) {
        return types::system::UpdateFirmwareResponse::AcceptedCancelled;
    } else if (this->firmware_installation_running) {
        return types::system::UpdateFirmwareResponse::Rejected;
    } else {
        return types::system::UpdateFirmwareResponse::Accepted;
    }
}

void systemImpl::download_signed_firmware(const types::system::FirmwareUpdateRequest& firmware_update_request) {

    if (!firmware_update_request.signing_certificate.has_value()) {
        EVLOG_warning << "Signing certificate is missing in FirmwareUpdateRequest";
        this->publish_firmware_update_status(
            {types::system::FirmwareUpdateStatusEnum::DownloadFailed, firmware_update_request.request_id});
        return;
    }
    if (!firmware_update_request.signature.has_value()) {
        EVLOG_warning << "Signature is missing in FirmwareUpdateRequest";
        this->publish_firmware_update_status(
            {types::system::FirmwareUpdateStatusEnum::DownloadFailed, firmware_update_request.request_id});
        return;
    }

    if (this->firmware_download_running) {
        EVLOG_info
            << "Received Firmware update request and firmware update already running - cancelling firmware update";
        this->interrupt_firmware_download.exchange(true);
        EVLOG_info << "Waiting for other firmware download to finish...";
        std::unique_lock<std::mutex> lk(this->firmware_update_mutex);
        this->firmware_update_cv.wait(lk, [this]() { return !this->firmware_download_running; });
        EVLOG_info << "Previous Firmware download finished!";
    }

    std::lock_guard<std::mutex> lg(this->firmware_update_mutex);
    EVLOG_info << "Starting Firmware update";
    this->interrupt_firmware_download.exchange(false);
    this->firmware_download_running = true;

    // // create temporary file
    const auto date_time = Everest::Date::to_rfc3339(date::utc_clock::now());
    const auto firmware_file_path = create_temp_file(fs::temp_directory_path(), "signed_firmware-" + date_time);

    const auto firmware_downloader = this->scripts_path / SIGNED_FIRMWARE_DOWNLOADER;
    const auto constants = this->scripts_path / CONSTANTS;

    const std::vector<std::string> download_args = {
        constants.string(), firmware_update_request.location, firmware_file_path.string(),
        firmware_update_request.signature.value(), firmware_update_request.signing_certificate.value()};
    int32_t retries = 0;
    const auto total_retries = firmware_update_request.retries.value_or(this->mod->config.DefaultRetries);
    const auto retry_interval =
        firmware_update_request.retry_interval_s.value_or(this->mod->config.DefaultRetryInterval);

    auto firmware_status_enum = types::system::FirmwareUpdateStatusEnum::DownloadFailed;
    types::system::FirmwareUpdateStatus firmware_status;
    firmware_status.request_id = firmware_update_request.request_id;
    firmware_status.firmware_update_status = firmware_status_enum;

    while (firmware_status.firmware_update_status == types::system::FirmwareUpdateStatusEnum::DownloadFailed &&
           retries <= total_retries && !this->interrupt_firmware_download) {
        boost::process::ipstream download_stream;
        boost::process::child download_cmd(firmware_downloader.string(), boost::process::args(download_args),
                                           boost::process::std_out > download_stream);
        std::string temp;
        retries += 1;
        while (std::getline(download_stream, temp) && !this->interrupt_firmware_download) {
            firmware_status.firmware_update_status = types::system::string_to_firmware_update_status_enum(temp);
            this->publish_firmware_update_status(firmware_status);
        }
        if (this->interrupt_firmware_download) {
            EVLOG_info << "Updating firmware was interrupted, terminating firmware update script, requestId: "
                       << firmware_status.request_id;
            download_cmd.terminate();
        }
        if (firmware_status.firmware_update_status == types::system::FirmwareUpdateStatusEnum::DownloadFailed &&
            retries <= total_retries) {
            std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
        }
    }
    if (firmware_status.firmware_update_status == types::system::FirmwareUpdateStatusEnum::SignatureVerified) {
        this->initialize_firmware_installation(firmware_update_request, firmware_file_path);
    }

    this->firmware_download_running = false;
    this->firmware_update_cv.notify_one();
    EVLOG_info << "Firmware update thread finished";
}

void systemImpl::initialize_firmware_installation(const types::system::FirmwareUpdateRequest& firmware_update_request,
                                                  const fs::path& firmware_file_path) {
    if (firmware_update_request.install_timestamp.has_value() &&
        Everest::Date::from_rfc3339(firmware_update_request.install_timestamp.value()) > date::utc_clock::now()) {
        const auto install_timestamp = Everest::Date::from_rfc3339(firmware_update_request.install_timestamp.value());
        this->signed_firmware_update_install_timer.at(
            [this, firmware_update_request, firmware_file_path]() {
                this->install_signed_firmware(firmware_update_request, firmware_file_path);
            },
            install_timestamp);
        EVLOG_info << "Installation for firmware scheduled for: " << Everest::Date::to_rfc3339(install_timestamp);
        types::system::FirmwareUpdateStatus firmware_update_status;
        firmware_update_status.request_id = firmware_update_request.request_id;
        firmware_update_status.firmware_update_status = types::system::FirmwareUpdateStatusEnum::InstallScheduled;
        this->publish_firmware_update_status(firmware_update_status);
    } else {
        // start installation immediately
        this->update_firmware_thread = std::thread([this, firmware_update_request, firmware_file_path]() {
            this->install_signed_firmware(firmware_update_request, firmware_file_path);
        });
        this->update_firmware_thread.detach();
    }
}

void systemImpl::install_signed_firmware(const types::system::FirmwareUpdateRequest& firmware_update_request,
                                         const fs::path& firmware_file_path) {
    auto firmware_status_enum = types::system::FirmwareUpdateStatusEnum::Installing;
    types::system::FirmwareUpdateStatus firmware_status;
    firmware_status.request_id = firmware_update_request.request_id;
    firmware_status.firmware_update_status = firmware_status_enum;
    if (!this->firmware_installation_running) {
        this->firmware_installation_running = true;
        boost::process::ipstream install_stream;
        const auto firmware_installer = this->scripts_path / SIGNED_FIRMWARE_INSTALLER;
        const auto constants = this->scripts_path / CONSTANTS;
        const std::vector<std::string> install_args = {constants.string()};
        boost::process::child install_cmd(firmware_installer.string(), boost::process::args(install_args),
                                          boost::process::std_out > install_stream);
        std::string temp;
        while (std::getline(install_stream, temp)) {
            firmware_status.firmware_update_status = types::system::string_to_firmware_update_status_enum(temp);
            this->publish_firmware_update_status(firmware_status);
        }
    } else {
        firmware_status.firmware_update_status = types::system::FirmwareUpdateStatusEnum::InstallationFailed;
        this->publish_firmware_update_status(firmware_status);
    }
}

types::system::UpdateFirmwareResponse
systemImpl::handle_update_firmware(types::system::FirmwareUpdateRequest& firmware_update_request) {
    if (firmware_update_request.request_id == -1) {
        return this->handle_standard_firmware_update(firmware_update_request);
    } else {
        return this->handle_signed_fimware_update(firmware_update_request);
    }
};

void systemImpl::handle_allow_firmware_installation() {
    // TODO: implement me
}

types::system::UploadLogsResponse
systemImpl::handle_upload_logs(types::system::UploadLogsRequest& upload_logs_request) {

    types::system::UploadLogsResponse response;

    if (this->log_upload_running) {
        response.upload_logs_status = types::system::UploadLogsStatus::AcceptedCancelled;
    } else {
        response.upload_logs_status = types::system::UploadLogsStatus::Accepted;
    }

    const auto date_time = Everest::Date::to_rfc3339(date::utc_clock::now());
    // TODO(piet): consider start time and end time
    const auto diagnostics_file_path = create_temp_file(fs::temp_directory_path(), "diagnostics-" + date_time);
    const auto diagnostics_file_name = diagnostics_file_path.filename().string();

    response.upload_logs_status = types::system::UploadLogsStatus::Accepted;
    response.file_name = diagnostics_file_name;

    const auto fake_diagnostics_file = json({{"diagnostics", {{"key", "value"}}}});
    std::ofstream diagnostics_file(diagnostics_file_path.c_str());
    diagnostics_file << fake_diagnostics_file.dump();

    this->upload_logs_thread = std::thread([this, upload_logs_request, diagnostics_file_name, diagnostics_file_path]() {
        if (this->log_upload_running) {
            EVLOG_info << "Received Log upload request and log upload already running - cancelling current upload";
            this->interrupt_log_upload.exchange(true);
            EVLOG_info << "Waiting for other log upload to finish...";
            std::unique_lock<std::mutex> lk(this->log_upload_mutex);
            this->log_upload_cv.wait(lk, [this]() { return !this->log_upload_running; });
            EVLOG_info << "Previous Log upload finished!";
        }

        std::lock_guard<std::mutex> lg(this->log_upload_mutex);
        EVLOG_info << "Starting upload of log file";
        this->interrupt_log_upload.exchange(false);
        this->log_upload_running = true;
        const auto diagnostics_uploader = this->scripts_path / DIAGNOSTICS_UPLOADER;
        const auto constants = this->scripts_path / CONSTANTS;

        std::vector<std::string> args = {constants.string(), upload_logs_request.location, diagnostics_file_name,
                                         diagnostics_file_path.string()};
        bool uploaded = false;
        int32_t retries = 0;
        const auto total_retries = upload_logs_request.retries.value_or(this->mod->config.DefaultRetries);
        const auto retry_interval =
            upload_logs_request.retry_interval_s.value_or(this->mod->config.DefaultRetryInterval);

        types::system::LogStatus log_status;
        while (!uploaded && retries <= total_retries && !this->interrupt_log_upload) {

            boost::process::ipstream stream;
            boost::process::child cmd(diagnostics_uploader.string(), boost::process::args(args),
                                      boost::process::std_out > stream);
            std::string temp;
            retries += 1;
            log_status.request_id = upload_logs_request.request_id.value_or(-1);
            while (std::getline(stream, temp) && !this->interrupt_log_upload) {
                if (temp == "Uploaded") {
                    log_status.log_status = types::system::string_to_log_status_enum(temp);
                } else if (temp == "UploadFailure" || temp == "PermissionDenied" || temp == "BadMessage" ||
                           temp == "NotSupportedOperation") {
                    log_status.log_status = types::system::LogStatusEnum::UploadFailure;
                } else {
                    log_status.log_status = types::system::LogStatusEnum::Uploading;
                }
                this->publish_log_status(log_status);
            }
            if (this->interrupt_log_upload) {
                EVLOG_info << "Uploading Logs was interrupted, terminating upload script, requestId: "
                           << log_status.request_id;
                cmd.terminate();
            } else if (log_status.log_status != types::system::LogStatusEnum::Uploaded && retries <= total_retries) {
                std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
            } else {
                uploaded = true;
            }
            cmd.wait();
        }
        this->log_upload_running = false;
        this->log_upload_cv.notify_one();
        EVLOG_info << "Log upload thread finished";
    });
    this->upload_logs_thread.detach();

    return response;
};

bool systemImpl::handle_is_reset_allowed(types::system::ResetType& type) {
    // Right now we dont want to reject a reset ever
    return true;
}

void systemImpl::handle_reset(types::system::ResetType& type, bool& scheduled) {
    // let the actual work be done by a worker thread, which can also delay it
    // a little bit (if configured) to allow e.g. clean shutdown of communication
    // channels in parallel when this call returns
    std::thread([this, type, scheduled] {
        EVLOG_info << "Reset request received: " << type << ", " << (scheduled ? "" : "not ") << "scheduled";

        std::this_thread::sleep_for(std::chrono::seconds(this->mod->config.ResetDelay));

        if (type == types::system::ResetType::Soft) {
            EVLOG_info << "Performing soft reset now.";
            kill(getpid(), SIGINT);
        } else {
            EVLOG_info << "Performing hard reset now.";
            kill(getpid(), SIGINT); // FIXME(piet): Define appropriate behavior for hard reset
        }
    }).detach();
}

bool systemImpl::handle_set_system_time(std::string& timestamp) {
    // your code for cmd set_system_time goes here
    return true;
};

types::system::BootReason systemImpl::handle_get_boot_reason() {
    // FIXME(piet): Provide proper BootReason
    return types::system::BootReason::PowerUp;
}

} // namespace main
} // namespace module
