// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"
#include <iomanip>
#include <sstream>

namespace module {
namespace main {

static std::string nfcidToString(const std::vector<std::uint8_t>& nfcid) {
    std::ostringstream oss;

    for (std::uint16_t byte : nfcid) {
        oss << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << byte;
    }

    return oss.str();
}

void auth_token_providerImpl::detected_rfid_token_callback(
    const std::pair<std::string, std::vector<std::uint8_t>>& reply) {
    auto& [protocol, nfcid] = reply;

    const auto now = std::chrono::steady_clock::now();
    const auto debounce_interval = std::chrono::milliseconds(config.token_debounce_interval_ms);

    types::authorization::ProvidedIdToken provided_token;

    try {
        provided_token.id_token = {nfcidToString(nfcid), types::authorization::string_to_id_token_type(protocol)};
    } catch (const std::out_of_range& e) {
        return;
    }

    provided_token.authorization_type = types::authorization::AuthorizationType::RFID;

    if (now < last_rfid_submit + debounce_interval) {
        if (config.debug) {
            EVLOG_info << "Ignoring rfid/nfc token (debouncing): " << protocol << ": " << nfcidToString(nfcid);
        }
        return;
    }

    if (config.debug) {
        EVLOG_info << "Publishing new rfid/nfc token: " << protocol << ": " << nfcidToString(nfcid);
    }
    publish_provided_token(provided_token);
    last_rfid_submit = now;
}

void auth_token_providerImpl::error_log_callback(const std::string& message) {
    EVLOG_warning << "NxpNfcFrontend: " << message;
}

void auth_token_providerImpl::init() {
    stop = false;

    if (config.no_physical_nfc_reader) {
        return;
    }

    try {
        nxpNfcFrontend = std::make_unique<NxpNfcFrontendDataSource>();
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to initialize NxpNfcFrontend: " << e.what();
    }
    nxpNfcFrontend->setDetectionCallback(
        [this](auto&&... args) { this->detected_rfid_token_callback(std::forward<decltype(args)>(args)...); });
    nxpNfcFrontend->setErrorLogCallback(
        [this](auto&&... args) { this->error_log_callback(std::forward<decltype(args)>(args)...); });
}

void auth_token_providerImpl::ready() {
    if (nxpNfcFrontend) {
        nxpNfcFrontend->run();
    }
    while (not stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

} // namespace main
} // namespace module
