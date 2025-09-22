// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "uk_random_delayImpl.hpp"

namespace module {
namespace random_delay {

void uk_random_delayImpl::init() {
}

void uk_random_delayImpl::ready() {
}

void uk_random_delayImpl::handle_enable() {
    mod->random_delay_running = false;
    mod->random_delay_enabled = true;
}

void uk_random_delayImpl::handle_disable() {
    mod->random_delay_running = false;
    mod->random_delay_enabled = false;
}

void uk_random_delayImpl::handle_cancel() {
    mod->random_delay_running = false;
}

void uk_random_delayImpl::handle_set_duration_s(int& value) {
    mod->random_delay_max_duration = std::chrono::seconds(value);
}

} // namespace random_delay

} // namespace module
