// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/device_model_base.hpp>
#include <ocpp/v2/device_model_interface.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelAbstract : public DeviceModelBase, public DeviceModelInterface {};

} // namespace v2
} // namespace ocpp
