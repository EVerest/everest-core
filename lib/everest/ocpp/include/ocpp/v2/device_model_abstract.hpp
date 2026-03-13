// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/device_model_base.hpp>
#include <ocpp/v2/device_model_interface.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelAbstract : public DeviceModelBase, public DeviceModelInterface {
public:
    // Disambiguate typed getters: prefer DeviceModelBase versions which use
    // request_value_internal and provide EVLOG_critical logging on missing values.
    using DeviceModelBase::get_optional_value;
    using DeviceModelBase::get_value;
    using DeviceModelBase::request_value;
};

} // namespace v2
} // namespace ocpp
