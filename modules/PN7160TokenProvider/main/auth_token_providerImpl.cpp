// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"

std::string rfid_to_string(char const rfid[], size_t length) {
    std::stringstream ss;

    for (size_t i = 0; i < length; ++i) {
        ss << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << static_cast<uint16_t>(rfid[i]);
    }

    return ss.str();
};

namespace module {
namespace main {

void auth_token_providerImpl::init() {
    if (config.disable_nfc_rfid) {
        return;
    }

    try {
        this->nfc_handler = std::make_unique<NfcHandler>();
    } catch (const std::exception& e) {
        EVLOG_error << "Failed to initialize libnfc handler: " << e.what();
    }
}

void auth_token_providerImpl::new_rfid_token_callback(char* uid, size_t length, NfcHandler::Protocol protocol) {

    // debounce
    const auto now = std::chrono::steady_clock::now();
    const auto debounce_interval = std::chrono::milliseconds(config.token_debounce_interval_ms);

    if (now < this->last_rfid_submit + debounce_interval) {
        // nothing to do, just debounce
        return;
    }

    if (uid == nullptr || length == 0 || length > 32) {
        // NOTE (aw): invalid data?, can this even happen?
        // NOTE (aw): we should probably log here?
        return;
    }

    // convert token to string and publish ist
    types::authorization::ProvidedIdToken token;

    token.id_token.type = [](NfcHandler::Protocol const in) {
        switch (in) {
        case NfcHandler::Protocol::ISO_IEC_15693:
            return types::authorization::IdTokenType::ISO15693;
        case NfcHandler::Protocol::MIFARE:
            return types::authorization::IdTokenType::ISO14443;
        }

        // FIXME (aw): default would be unknown, what to do here?
        // choosing Local here, although this is not the proper way
        return types::authorization::IdTokenType::Local;
    }(protocol);
    token.id_token.value = rfid_to_string(uid, length);
    token.authorization_type = types::authorization::AuthorizationType::RFID;

    EVLOG_info << "Publishing new rfid/nfc token: " << token;
    this->publish_provided_token(token);

    this->last_rfid_submit = now;
}

void auth_token_providerImpl::ready() {
    if (this->nfc_handler) {
        this->nfc_handler->start(
            [this](auto&&... args) { this->new_rfid_token_callback(std::forward<decltype(args)>(args)...); });
    }
}

} // namespace main
} // namespace module
