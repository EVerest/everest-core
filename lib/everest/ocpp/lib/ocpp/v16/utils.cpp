// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/common/utils.hpp>
#include <ocpp/v16/utils.hpp>

namespace ocpp {
namespace v16 {
namespace utils {

size_t get_message_size(const ocpp::Call<StopTransactionRequest>& call) {
    return json(call).at(CALL_PAYLOAD).dump().length();
}

void drop_transaction_data(size_t max_message_size, ocpp::Call<StopTransactionRequest>& call) {
    auto& transaction_data = call.msg.transactionData.value();
    while (get_message_size(call) > max_message_size && transaction_data.size() > 2) {
        for (size_t i = 1; i < transaction_data.size() - 1; i = i + 2) {
            transaction_data.erase(transaction_data.begin() + i);
        }
    }
}

bool is_critical(const std::string& security_event) {
    if (security_event == ocpp::security_events::FIRMWARE_UPDATED) {
        return true;
    }
    if (security_event == ocpp::security_events::SETTINGSYSTEMTIME) {
        return true;
    }
    if (security_event == ocpp::security_events::STARTUP_OF_THE_DEVICE) {
        return true;
    }
    if (security_event == ocpp::security_events::RESET_OR_REBOOT) {
        return true;
    }
    if (security_event == ocpp::security_events::SECURITYLOGWASCLEARED) {
        return true;
    }
    if (security_event == ocpp::security_events::MEMORYEXHAUSTION) {
        return true;
    }
    if (security_event == ocpp::security_events::TAMPERDETECTIONACTIVATED) {
        return true;
    }

    return false;
}

} // namespace utils
} // namespace v16
} // namespace ocpp
