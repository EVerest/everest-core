// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "uk_random_delay/wrapper.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::uk_random_delay {

CountDown_Internal to_internal_api(CountDown_External const& val) {
    CountDown_Internal result;
    result.countdown_s = val.countdown_s;
    result.current_limit_after_delay_A = val.current_limit_after_delay_A;
    result.current_limit_during_delay_A = val.current_limit_during_delay_A;
    result.start_time = val.start_time;
    return result;
}

CountDown_External to_external_api(CountDown_Internal const& val) {
    CountDown_External result;
    result.countdown_s = val.countdown_s;
    result.current_limit_after_delay_A = val.current_limit_after_delay_A;
    result.current_limit_during_delay_A = val.current_limit_during_delay_A;
    result.start_time = val.start_time;
    return result;
}

} // namespace everest::lib::API::V1_0::types::uk_random_delay
