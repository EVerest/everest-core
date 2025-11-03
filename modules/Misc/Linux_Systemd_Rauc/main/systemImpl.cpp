// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "systemImpl.hpp"
#include "diagnostics_handler.hpp"

#include <fstream>

#include <everest/run_application/run_application.hpp>

using namespace everest::run_application;

namespace module {
namespace main {

const std::string CONSTANTS = "constants.env";
const std::string DIAGNOSTICS_UPLOADER = "diagnostics_uploader.sh";

namespace fs = std::filesystem;

// FIXME (aw): this function needs to be refactored into some kind of utility library
fs::path create_temp_file(const fs::path& dir, const std::string& prefix) {
    const std::string fn_template = (dir / prefix).string() + "XXXXXX" + std::string(1, '\0');
    std::vector<char> fn_template_buffer{fn_template.begin(), fn_template.end()};

    // mkstemp needs to have at least 6 XXXXXX at the end and it will replace these
    // with a valid file name
    auto fd = mkstemp(fn_template_buffer.data());

    if (fd == -1) {
        EVLOG_AND_THROW(Everest::EverestBaseRuntimeError("Failed to create temporary file at: " + fn_template));
    }

    // close the file descriptor
    close(fd);

    return fn_template_buffer.data();
}

void systemImpl::init() {
    this->scripts_path = mod->info.paths.libexec;
}

void systemImpl::ready() {
}

types::system::UpdateFirmwareResponse
systemImpl::handle_update_firmware(types::system::FirmwareUpdateRequest& firmware_update_request) {
    // FIXME: implement planned updates at a specific time
    // FIXME: we don't care about the certificate and signature provided as an argument for now.
    // RAUC will not use them anyhow and updates will be equally secure whether they are launched by OCPP secure update
    // mechanism or the old non-secure mechanism.
    if (mod->rauc.is_idle()) {
        EVLOG_info << "Installing bundle from URL: " << firmware_update_request.location;
        this->mod->install_firmware_bundle(firmware_update_request.location, firmware_update_request.request_id);
        return types::system::UpdateFirmwareResponse::Accepted;
    } else {
        return types::system::UpdateFirmwareResponse::Rejected;
    }
}

types::system::UploadLogsResponse
systemImpl::handle_upload_logs(types::system::UploadLogsRequest& upload_logs_request) {
    types::system::UploadLogsResponse response;

    if (this->log_upload_running) {
        response.upload_logs_status = types::system::UploadLogsStatus::AcceptedCanceled;
    } else {
        response.upload_logs_status = types::system::UploadLogsStatus::Accepted;
    }

    const auto date_time = Everest::Date::to_rfc3339(date::utc_clock::now());
    const auto diagnostics_file_path = create_temp_file(fs::temp_directory_path(), "diagnostics-" + date_time);
    const auto diagnostics_file_name = diagnostics_file_path.filename().string();

    response.upload_logs_status = types::system::UploadLogsStatus::Accepted;
    response.file_name = diagnostics_file_name;

    // populate file with available logs within the specified time window
    DiagnosticsHandler diag(mod->info.paths.libexec, mod->config.OCPPLogPath, mod->config.SessionLogPath);
    const auto create_result = diag.create_log(diagnostics_file_path.c_str(), upload_logs_request.oldest_timestamp,
                                               upload_logs_request.latest_timestamp);

    this->upload_logs_thread =
        std::thread([this, create_result, upload_logs_request, diagnostics_file_name, diagnostics_file_path]() {
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
            if (create_result == DiagnosticsHandler::log_result_t::error_file) {
                // problem creating the file - nothing to upload
                log_status.log_status = types::system::LogStatusEnum::UploadFailure;
                this->publish_log_status(log_status);
            } else {
                while (!uploaded && retries <= total_retries && !this->interrupt_log_upload) {
                    retries += 1;
                    log_status.request_id = upload_logs_request.request_id.value_or(-1);
                    run_application(
                        diagnostics_uploader.string(), args, [this, &log_status](const std::string& output_line) {
                            if (output_line == "Uploaded") {
                                log_status.log_status = types::system::string_to_log_status_enum(output_line);
                            } else if (output_line == "UploadFailure" || output_line == "PermissionDenied" ||
                                       output_line == "BadMessage" || output_line == "NotSupportedOperation") {
                                log_status.log_status = types::system::LogStatusEnum::UploadFailure;
                            } else {
                                log_status.log_status = types::system::LogStatusEnum::Uploading;
                            }
                            this->publish_log_status(log_status);
                            if (this->interrupt_log_upload) {
                                return CmdControl::Terminate;
                            }
                            return CmdControl::Continue;
                        });
                    if (this->interrupt_log_upload) {
                        EVLOG_info << "Uploading Logs was interrupted, terminating upload script, requestId: "
                                   << log_status.request_id;
                        // N01.FR.20
                        log_status.log_status = types::system::LogStatusEnum::AcceptedCanceled;
                        this->publish_log_status(log_status);
                    } else if (log_status.log_status != types::system::LogStatusEnum::Uploaded &&
                               retries <= total_retries) {
                        std::this_thread::sleep_for(std::chrono::seconds(retry_interval));
                    } else {
                        uploaded = true;
                    }
                }
            }
            this->log_upload_running = false;
            this->log_upload_cv.notify_one();
            EVLOG_info << "Log upload thread finished";
        });
    this->upload_logs_thread.detach();

    return response;
}

bool systemImpl::handle_is_reset_allowed(types::system::ResetType& type) {
    // Allow resets at any time for now
    return true;
}

void systemImpl::handle_reset(types::system::ResetType& type, bool& scheduled) {
    if (type == types::system::ResetType::Soft) {
        EVLOG_info << "Performing soft reset";
        // This will effectivly stop everest and make it restart via systemd
        exit(255);
    } else {
        EVLOG_info << "Performing hard reset";

        // this reboots the whole linux system
        system("/sbin/reboot");
    }
}

bool systemImpl::handle_set_system_time(std::string& timestamp) {
    // currently not supported, system runs on network time
    return true;
}

types::system::BootReason systemImpl::handle_get_boot_reason() {
    return types::system::BootReason::Unknown;
}

void systemImpl::handle_allow_firmware_installation() {
    EVLOG_info << "Received allow_firmware_installation command - allow firmware update to proceed with reboot.";
    this->mod->firmware_update_may_proceed_with_reboot_callback();
}

} // namespace main
} // namespace module
