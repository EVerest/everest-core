// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <vector>

#include <generated/types/iso15118.hpp>
#include <generated/types/iso15118_vas.hpp>

#include <iso15118/d20/ev_information.hpp>
#include <iso15118/message/service_detail.hpp>
#include <iso15118/message/type.hpp>

static constexpr auto NUMBER_OF_SETUP_STEPS = 5;

enum class SetupStep : std::uint8_t {
    SETUP,
    ENERGY_SERVICE,
    AUTH_SETUP,
    MAX_LIMITS,
    MIN_LIMITS,
};

template <typename T> constexpr auto to_underlying_value(T t) {
    return static_cast<std::underlying_type_t<T>>(t);
}

static_assert(NUMBER_OF_SETUP_STEPS == to_underlying_value(SetupStep::MIN_LIMITS) + 1,
              "NUMBER_OF_SETUP_STEPS should be in sync with the SetupStep enum definition");

constexpr types::iso15118::V2gMessageId convert_v2g_message_type(iso15118::message_20::Type type) {

    using Type = iso15118::message_20::Type;
    using Id = types::iso15118::V2gMessageId;

    switch (type) {
    case Type::None:
        return Id::UnknownMessage;
    case Type::SupportedAppProtocolReq:
        return Id::SupportedAppProtocolReq;
    case Type::SupportedAppProtocolRes:
        return Id::SupportedAppProtocolRes;
    case Type::SessionSetupReq:
        return Id::SessionSetupReq;
    case Type::SessionSetupRes:
        return Id::SessionSetupRes;
    case Type::AuthorizationSetupReq:
        return Id::AuthorizationSetupReq;
    case Type::AuthorizationSetupRes:
        return Id::AuthorizationSetupRes;
    case Type::AuthorizationReq:
        return Id::AuthorizationReq;
    case Type::AuthorizationRes:
        return Id::AuthorizationRes;
    case Type::ServiceDiscoveryReq:
        return Id::ServiceDiscoveryReq;
    case Type::ServiceDiscoveryRes:
        return Id::ServiceDiscoveryRes;
    case Type::ServiceDetailReq:
        return Id::ServiceDetailReq;
    case Type::ServiceDetailRes:
        return Id::ServiceDetailRes;
    case Type::ServiceSelectionReq:
        return Id::ServiceSelectionReq;
    case Type::ServiceSelectionRes:
        return Id::ServiceSelectionRes;
    case Type::DC_ChargeParameterDiscoveryReq:
        return Id::DcChargeParameterDiscoveryReq;
    case Type::DC_ChargeParameterDiscoveryRes:
        return Id::DcChargeParameterDiscoveryRes;
    case Type::ScheduleExchangeReq:
        return Id::ScheduleExchangeReq;
    case Type::ScheduleExchangeRes:
        return Id::ScheduleExchangeRes;
    case Type::DC_CableCheckReq:
        return Id::DcCableCheckReq;
    case Type::DC_CableCheckRes:
        return Id::DcCableCheckRes;
    case Type::DC_PreChargeReq:
        return Id::DcPreChargeReq;
    case Type::DC_PreChargeRes:
        return Id::DcPreChargeRes;
    case Type::PowerDeliveryReq:
        return Id::PowerDeliveryReq;
    case Type::PowerDeliveryRes:
        return Id::PowerDeliveryRes;
    case Type::DC_ChargeLoopReq:
        return Id::DcChargeLoopReq;
    case Type::DC_ChargeLoopRes:
        return Id::DcChargeLoopRes;
    case Type::DC_WeldingDetectionReq:
        return Id::DcWeldingDetectionReq;
    case Type::DC_WeldingDetectionRes:
        return Id::DcWeldingDetectionRes;
    case Type::SessionStopReq:
        return Id::SessionStopReq;
    case Type::SessionStopRes:
        return Id::SessionStopRes;
    case Type::AC_ChargeParameterDiscoveryReq:
        return Id::AcChargeParameterDiscoveryReq;
    case Type::AC_ChargeParameterDiscoveryRes:
        return Id::AcChargeParameterDiscoveryRes;
    case Type::AC_ChargeLoopReq:
        return Id::AcChargeLoopReq;
    case Type::AC_ChargeLoopRes:
        return Id::AcChargeLoopRes;
    }

    return Id::UnknownMessage;
}

namespace module::charger {

template <typename T, typename U> std::optional<T> convert_from_optional(const std::optional<U>& in);

types::iso15118::AppProtocol convert_app_protocol(const iso15118::message_20::SupportedAppProtocol& app_protocol);
types::iso15118::EvInformation convert_ev_info(const iso15118::d20::EVInformation& ev_info);

template <typename T> types::iso15118::DcChargeDynamicModeValues convert_dynamic_values(const T& in);

template <typename EVSE, typename EV>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params, const EVSE& evse_limits,
                                  const EV& ev_limits);
template <typename In>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params, const In& data);

std::vector<iso15118::message_20::datatypes::ParameterSet>
convert_parameter_set_list(const std::vector<types::iso15118_vas::ParameterSet>& parameter_set_list);

template <typename T> types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const T& mode);
template <typename T> types::iso15118::AcEvPresentPowerValues fill_ac_ev_present_power_values(const T& mode);

} // namespace module::charger
