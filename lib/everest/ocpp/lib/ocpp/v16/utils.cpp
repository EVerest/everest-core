// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/common/utils.hpp>
#include <ocpp/v16/utils.hpp>

namespace ocpp::v16::utils {

size_t get_message_size(const ocpp::Call<StopTransactionRequest>& call) {
    return json(call).at(CALL_PAYLOAD).dump().length();
}

void drop_transaction_data(size_t max_message_size, ocpp::Call<StopTransactionRequest>& call) {
    auto& transaction_data = call.msg.transactionData.value();
    while (get_message_size(call) > max_message_size && transaction_data.size() > 2) {
        // Drop every second message, keeping the first and last.
        // Iterate backwards so that erase() does not shift the indices
        // of elements we still need to remove.
        int last = static_cast<int>(transaction_data.size()) - 2;
        int start = (last % 2 == 0) ? last - 1 : last;
        for (int i = start; i >= 1; i -= 2) {
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

std::string to_csl(const std::vector<std::string>& vec) {
    std::string csl;
    for (const auto& i : vec) {
        csl += i;
        csl += ",";
    }
    if (!csl.empty()) {
        csl.pop_back();
    }
    return csl;
}

std::vector<std::string> from_csl(const std::string& csl) {
    std::vector<std::string> vec;
    auto start = csl.find_first_not_of(',');
    while (start != std::string::npos) {
        auto end = csl.find_first_of(',', start);
        if (end == std::string::npos) {
            vec.push_back(std::move(csl.substr(start)));
            start = std::string::npos;
        } else {
            vec.push_back(std::move(csl.substr(start, end - start)));
            start = csl.find_first_not_of(',', end);
        }
    }
    return vec;
}

std::vector<std::string> split_string(char separator, const std::string& csl) {
    std::vector<std::string> vec;
    std::size_t start{0};
    if (!csl.empty()) {
        while (start != std::string::npos) {
            auto end = csl.find_first_of(separator, start);
            if (end == std::string::npos) {
                vec.push_back(std::move(csl.substr(start)));
                start = std::string::npos;
            } else if (start == end) {
                vec.emplace_back();
                start++;
            } else {
                vec.push_back(std::move(csl.substr(start, end - start)));
                start = end + 1;
            }
        }
    }
    return vec;
}

void OrderedUniqueStringList::do_insert(std::string&& s) {
    if (const auto it = list.find(s); it == list.end()) {
        list.insert({std::move(s), count++});
    }
}
std::vector<std::string> OrderedUniqueStringList::get() const {
    std::vector<std::string> result(list.size());
    for (const auto& i : list) {
        result[i.second] = i.first;
    }
    return result;
}

} // namespace ocpp::v16::utils
