// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/definitions.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>

using ocpp::CiString;
using ocpp::v2::DataEnum;

namespace EvseDefinitions {

EVSE get_evse(const int32_t evse_id, const std::optional<int32_t>& connector_id) {
    EVSE evse;
    evse.id = evse_id;
    evse.connectorId = connector_id;
    return evse;
}

namespace Characteristics {

const VariableCharacteristics AllowReset = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics AvailabilityState = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::OptionList;
    var.supportsMonitoring = false;
    var.valuesList = CiString<1000>("Available,Occupied,Reserved,Unavailable,Faulted");
    return var;
}();

const VariableCharacteristics Available = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics EvseId = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();

VariableCharacteristics EVSEPower = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::decimal;
    var.supportsMonitoring = false;
    var.unit = CiString<16>("W");
    var.maxLimit = 0.0f; // will be updated at runtime
    return var;
}();

const VariableCharacteristics SupplyPhases = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::integer;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics ISO15118EvseId = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    var.minLimit = 7.0f;
    var.maxLimit = 37.0f;
    return var;
}();
} // namespace Characteristics
} // namespace EvseDefinitions

namespace ConnectorDefinitions {
namespace Characteristics {

const VariableCharacteristics AvailabilityState = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::OptionList;
    var.supportsMonitoring = false;
    var.valuesList = CiString<1000>("Available,Occupied,Reserved,Unavailable,Faulted");
    return var;
}();

const VariableCharacteristics Available = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics ConnectorType = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics SupplyPhases = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::integer;
    var.supportsMonitoring = false;
    return var;
}();
} // namespace Characteristics
} // namespace ConnectorDefinitions

namespace V2XDefinitions {
namespace Characteristics {
const VariableCharacteristics Available = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics Enabled = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();

const VariableCharacteristics SupportedEnergyTransferModes = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::MemberList;
    var.supportsMonitoring = false;
    var.valuesList =
        "AC_single_phase,AC_two_phase,AC_three_phase,DC,AC_BPT,AC_BPT_DER,AC_DER,DC_BPT,DC_ACDP,DC_ACDP_BPT,WPT";
    return var;
}();

const VariableCharacteristics SupportedOperationModes = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::MemberList;
    var.supportsMonitoring = false;
    var.valuesList = "Idle,ChargingOnly,CentralSetpoint,ExternalSetpoint,ExternalLimits,CentralFrequency,"
                     "LocalFrequency,LocalLoadBalancing";
    return var;
}();

} // namespace Characteristics
} // namespace V2XDefinitions

namespace ISO15118Definitions {
namespace Characteristics {
const VariableCharacteristics Enabled = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics ServiceRenegotiationSupport = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics ProtocolSupported = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
} // namespace Characteristics
} // namespace ISO15118Definitions

namespace ConnectedEVDefinitions {
namespace Characteristics {
const VariableCharacteristics Available = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::boolean;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics VehicleId = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics ProtocolAgreed = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics VehicleCertificateLeaf = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics VehicleCertificateSubCa1 = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics VehicleCertificateSubCa2 = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics VehicleCertificateRoot = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
const VariableCharacteristics ProtocolSupportedByEV = [] {
    VariableCharacteristics var;
    var.dataType = DataEnum::string;
    var.supportsMonitoring = false;
    return var;
}();
} // namespace Characteristics
} // namespace ConnectedEVDefinitions
