/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <memory>

#include "Timer/SimpleTimeout.hpp"

namespace SessionAccountant {

struct TimerFactory {
std::function<std::shared_ptr<Timeout::TimeoutBase>()> get_powermeter_timeout_timer;
std::function<std::shared_ptr<Timeout::TimeoutBase>()> get_generic_timer;
};

} // namespace SessionAccountant
