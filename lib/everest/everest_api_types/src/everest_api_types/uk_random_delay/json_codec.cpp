// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "uk_random_delay/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "uk_random_delay/API.hpp"
#include "uk_random_delay/codec.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::uk_random_delay {

void to_json(json& j, CountDown const& k) noexcept {
    j = json{
        {"countdown_s", k.countdown_s},
        {"current_limit_after_delay_A", k.current_limit_after_delay_A},
        {"current_limit_during_delay_A", k.current_limit_during_delay_A},
    };
    if (k.start_time) {
        j["start_time"] = k.start_time.value();
    }
}

void from_json(const json& j, CountDown& k) {
    k.countdown_s = j.at("countdown_s");
    k.current_limit_after_delay_A = j.at("current_limit_after_delay_A");
    k.current_limit_during_delay_A = j.at("current_limit_during_delay_A");

    if (j.contains("start_time")) {
        k.start_time.emplace(j.at("start_time"));
    }
}

} // namespace everest::lib::API::V1_0::types::uk_random_delay
