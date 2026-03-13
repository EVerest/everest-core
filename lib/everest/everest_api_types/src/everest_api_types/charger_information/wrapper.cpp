// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "charger_information/wrapper.hpp"
#include "charger_information/API.hpp"

namespace everest::lib::API::V1_0::types::charger_information {

ChargerInformation_Internal to_internal_api(ChargerInformation_External const& val) {
    ChargerInformation_Internal result;
    result.vendor = val.vendor;
    result.model = val.model;
    result.chargepoint_serial = val.chargepoint_serial;
    result.chargebox_serial = val.chargebox_serial;
    result.friendly_name = val.friendly_name;
    result.manufacturer = val.manufacturer;
    result.manufacturer_url = val.manufacturer_url;
    result.model_url = val.model_url;
    result.model_number = val.model_number;
    result.model_revision = val.model_revision;
    result.board_revision = val.board_revision;
    result.firmware_version = val.firmware_version;
    return result;
}

ChargerInformation_External to_external_api(ChargerInformation_Internal const& val) {
    ChargerInformation_External result;
    result.vendor = val.vendor;
    result.model = val.model;
    result.chargepoint_serial = val.chargepoint_serial;
    result.chargebox_serial = val.chargebox_serial;
    result.friendly_name = val.friendly_name;
    result.manufacturer = val.manufacturer;
    result.manufacturer_url = val.manufacturer_url;
    result.model_url = val.model_url;
    result.model_number = val.model_number;
    result.model_revision = val.model_revision;
    result.board_revision = val.board_revision;
    result.firmware_version = val.firmware_version;
    return result;
}

} // namespace everest::lib::API::V1_0::types::charger_information
