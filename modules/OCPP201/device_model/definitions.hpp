// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ctrlr_component_variables.hpp>

using ocpp::v2::EVSE;
using ocpp::v2::VariableCharacteristics;

namespace EvseDefinitions {

EVSE get_evse(const int32_t evse_id, const std::optional<int32_t>& connector_id = std::nullopt);

namespace Characteristics {
extern const VariableCharacteristics AllowReset;
extern const VariableCharacteristics AvailabilityState;
extern const VariableCharacteristics Available;
extern const VariableCharacteristics EvseId;
extern VariableCharacteristics EVSEPower;
extern const VariableCharacteristics SupplyPhases;
extern const VariableCharacteristics ISO15118EvseId;
} // namespace Characteristics
} // namespace EvseDefinitions

namespace ConnectorDefinitions {
namespace Characteristics {
extern const VariableCharacteristics AvailabilityState;
extern const VariableCharacteristics Available;
extern const VariableCharacteristics ConnectorType;
extern const VariableCharacteristics SupplyPhases;
} // namespace Characteristics
} // namespace ConnectorDefinitions
