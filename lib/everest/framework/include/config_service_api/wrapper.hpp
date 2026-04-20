// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <map>

#include <everest_api_types/config_service/API.hpp>

//TODO(CB): Are these pragmas required?
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "utils/config_service_interface.hpp"
#pragma GCC diagnostic pop

namespace everest::config::api::types {

using MarkActiveSlotResultEnum_Internal = ::Everest::config::SetActiveSlotStatus;
using MarkActiveSlotResultEnum_External = ::everest::lib::API::V1_0::types::config_service::MarkActiveSlotResultEnum;

MarkActiveSlotResultEnum_Internal to_internal_api(MarkActiveSlotResultEnum_External const& val);
MarkActiveSlotResultEnum_External to_external_api(MarkActiveSlotResultEnum_Internal const& val);

using DeleteSlotResultEnum_Internal = ::Everest::config::DeleteSlotStatus;
using DeleteSlotResultEnum_External = ::everest::lib::API::V1_0::types::config_service::DeleteSlotResultEnum;

DeleteSlotResultEnum_Internal to_internal_api(DeleteSlotResultEnum_External const& val);
DeleteSlotResultEnum_External to_external_api(DeleteSlotResultEnum_Internal const& val);

using ConfigurationParameterUpdateResultEnum_Internal = ::Everest::config::SetConfigParameterResult;
using ConfigurationParameterUpdateResultEnum_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateResultEnum;

ConfigurationParameterUpdateResultEnum_Internal
to_internal_api(ConfigurationParameterUpdateResultEnum_External const& val);
ConfigurationParameterUpdateResultEnum_External
to_external_api(ConfigurationParameterUpdateResultEnum_Internal const& val);

using ActiveSlotStatusEnum_Internal = ::Everest::config::ActiveSlotStatus;
using ActiveSlotStatusEnum_External = ::everest::lib::API::V1_0::types::config_service::ActiveSlotStatusEnum;

ActiveSlotStatusEnum_Internal to_internal_api(ActiveSlotStatusEnum_External const& val);
ActiveSlotStatusEnum_External to_external_api(ActiveSlotStatusEnum_Internal const& val);

using ConfigurationParameterDatatype_Internal = ::everest::config::Datatype;
using ConfigurationParameterDatatype_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterDatatype;

ConfigurationParameterDatatype_Internal to_internal_api(ConfigurationParameterDatatype_External const& val);
ConfigurationParameterDatatype_External to_external_api(ConfigurationParameterDatatype_Internal const& val);

using ConfigurationParameterMutability_Internal = ::everest::config::Mutability;
using ConfigurationParameterMutability_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterMutability;

ConfigurationParameterMutability_Internal to_internal_api(ConfigurationParameterMutability_External const& val);
ConfigurationParameterMutability_External to_external_api(ConfigurationParameterMutability_Internal const& val);

// using ConfigurationActivationPolicy_Internal = ::types::config_service::ConfigurationActivationPolicy;
// using ConfigurationActivationPolicy_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationActivationPolicy;

// ConfigurationActivationPolicy_Internal to_internal_api(ConfigurationActivationPolicy_External const& val);
// ConfigurationActivationPolicy_External to_external_api(ConfigurationActivationPolicy_Internal const& val);

using GetConfigurationStatusEnum_Internal = ::Everest::config::GetConfigurationStatus;
using GetConfigurationStatusEnum_External = ::everest::lib::API::V1_0::types::config_service::GetConfigurationStatusEnum;

GetConfigurationStatusEnum_Internal to_internal_api(GetConfigurationStatusEnum_External const& val);
GetConfigurationStatusEnum_External to_external_api(GetConfigurationStatusEnum_Internal const& val);

using ConfigMetadata_Internal = ::everest::config::SlotInfo;
using ConfigMetadata_External = ::everest::lib::API::V1_0::types::config_service::ConfigMetadata;

ConfigMetadata_Internal to_internal_api(ConfigMetadata_External const& val);
ConfigMetadata_External to_external_api(ConfigMetadata_Internal const& val);

using ListSlotIdsResult_Internal = ::std::vector<::everest::config::SlotInfo>;
using ListSlotIdsResult_External = ::everest::lib::API::V1_0::types::config_service::ListSlotIdsResult;

ListSlotIdsResult_Internal to_internal_api(ListSlotIdsResult_External const& val);
ListSlotIdsResult_External to_external_api(ListSlotIdsResult_Internal const& val);

using DuplicateSlotResult_Internal = ::everest::config::DuplicateSlotResult;
using DuplicateSlotResult_External = ::everest::lib::API::V1_0::types::config_service::DuplicateSlotResult;

DuplicateSlotResult_Internal to_internal_api(DuplicateSlotResult_External const& val);
DuplicateSlotResult_External to_external_api(DuplicateSlotResult_Internal const& val);

using LoadFromYamlResult_Internal = ::Everest::config::LoadFromYamlResult;
using LoadFromYamlResult_External = ::everest::lib::API::V1_0::types::config_service::LoadFromYamlResult;

LoadFromYamlResult_Internal to_internal_api(LoadFromYamlResult_External const& val);
LoadFromYamlResult_External to_external_api(LoadFromYamlResult_Internal const& val);

using ConfigurationParameterIdentifier_Internal = ::everest::config::ConfigurationParameterIdentifier;
using ConfigurationParameterIdentifier_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterIdentifier;

ConfigurationParameterIdentifier_Internal to_internal_api(ConfigurationParameterIdentifier_External const& val);
ConfigurationParameterIdentifier_External to_external_api(ConfigurationParameterIdentifier_Internal const& val);

using ConfigurationParameterUpdate_Internal = ::Everest::config::ConfigParameterUpdate;
using ConfigurationParameterUpdate_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdate;

ConfigurationParameterUpdate_Internal to_internal_api(ConfigurationParameterUpdate_External const& val);
ConfigurationParameterUpdate_External to_external_api(ConfigurationParameterUpdate_Internal const& val);

// using OriginOfChange_Internal = ::types::config_service::OriginOfChange;
// using OriginOfChange_External = ::everest::lib::API::V1_0::types::config_service::OriginOfChange;

// OriginOfChange_Internal to_internal_api(OriginOfChange_External const& val);
// OriginOfChange_External to_external_api(OriginOfChange_Internal const& val);

using ActiveSlotUpdateNotice_Internal = ::Everest::config::ActiveSlotUpdate;
using ActiveSlotUpdateNotice_External = ::everest::lib::API::V1_0::types::config_service::ActiveSlotUpdateNotice;

ActiveSlotUpdateNotice_Internal to_internal_api(ActiveSlotUpdateNotice_External const& val);
ActiveSlotUpdateNotice_External to_external_api(ActiveSlotUpdateNotice_Internal const& val);

using ConfigurationParameterUpdateResultRecord_Internal =
    ::Everest::config::ConfigParameterUpdateNotice;
using ConfigurationParameterUpdateResultRecord_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateResultRecord;

ConfigurationParameterUpdateResultRecord_Internal
to_internal_api(ConfigurationParameterUpdateResultRecord_External const& val);
ConfigurationParameterUpdateResultRecord_External
to_external_api(ConfigurationParameterUpdateResultRecord_Internal const& val);

using ConfigurationParameterUpdateNotice_Internal = ::Everest::config::ConfigurationUpdate;
using ConfigurationParameterUpdateNotice_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateNotice;

ConfigurationParameterUpdateNotice_Internal to_internal_api(ConfigurationParameterUpdateNotice_External const& val);
ConfigurationParameterUpdateNotice_External to_external_api(ConfigurationParameterUpdateNotice_Internal const& val);

using Mapping_Internal = Mapping;
using Mapping_External = ::everest::lib::API::V1_0::types::config_service::Mapping;

Mapping_Internal to_internal_api(Mapping_External const& val);
Mapping_External to_external_api(Mapping_Internal const& val);

using ModuleTierMappings_Internal = ModuleTierMappings;
using ModuleTierMappings_External = ::everest::lib::API::V1_0::types::config_service::ModuleTierMappings;

ModuleTierMappings_Internal to_internal_api(ModuleTierMappings_External const& val);
ModuleTierMappings_External to_external_api(ModuleTierMappings_Internal const& val);

using ReqFulfillment_Internal = Fulfillment;
using ReqFulfillment_External = ::everest::lib::API::V1_0::types::config_service::ReqFulfillment;

ReqFulfillment_Internal to_internal_api(ReqFulfillment_External const& val, const std::string& requirement_id);
ReqFulfillment_External to_external_api(ReqFulfillment_Internal const& val);

using ConfigurationParameterCharacteristics_Internal = ::everest::config::ConfigurationParameterCharacteristics;
using ConfigurationParameterCharacteristics_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameterCharacteristics;

ConfigurationParameterCharacteristics_Internal
to_internal_api(ConfigurationParameterCharacteristics_External const& val);
ConfigurationParameterCharacteristics_External
to_external_api(ConfigurationParameterCharacteristics_Internal const& val);

using ConfigurationParameter_Internal = ::everest::config::ConfigurationParameter;
using ConfigurationParameter_External = ::everest::lib::API::V1_0::types::config_service::ConfigurationParameter;

ConfigurationParameter_Internal to_internal_api(ConfigurationParameter_External const& val);
ConfigurationParameter_External to_external_api(ConfigurationParameter_Internal const& val);

using TelemetryConfig_Internal = TelemetryConfig;
using TelemetryConfig_External = ::everest::lib::API::V1_0::types::config_service::TelemetryConfig;

TelemetryConfig_Internal to_internal_api(TelemetryConfig_External const& val);
TelemetryConfig_External to_external_api(TelemetryConfig_Internal const& val);

using ModuleConfigAccess_Internal = ::everest::config::ModuleConfigAccess;
using ModuleConfigAccess_External = ::everest::lib::API::V1_0::types::config_service::ModuleConfigAccess;

ModuleConfigAccess_Internal to_internal_api(ModuleConfigAccess_External const& val);
ModuleConfigAccess_External to_external_api(ModuleConfigAccess_Internal const& val);

using ConfigAccess_Internal = ::everest::config::ConfigAccess;
using ConfigAccess_External = ::everest::lib::API::V1_0::types::config_service::ConfigAccess;

ConfigAccess_Internal to_internal_api(ConfigAccess_External const& val);
ConfigAccess_External to_external_api(ConfigAccess_Internal const& val);

using ConfigAccessControl_Internal = ::everest::config::Access;
using ConfigAccessControl_External = ::everest::lib::API::V1_0::types::config_service::ConfigAccessControl;

ConfigAccessControl_Internal to_internal_api(ConfigAccessControl_External const& val, std::string const& module_id);
ConfigAccessControl_External to_external_api(ConfigAccessControl_Internal const& val);

std::vector<Fulfillment> fulfillmentVecToInternal(const std::vector<ReqFulfillment_External>& fulfillments_external, const std::string& requirement_id);

using ModuleConfiguration_Internal = ::everest::config::ModuleConfig;
using ModuleConfiguration_External = ::everest::lib::API::V1_0::types::config_service::ModuleConfiguration;

ModuleConfiguration_Internal to_internal_api(ModuleConfiguration_External const& val);
ModuleConfiguration_External to_external_api(ModuleConfiguration_Internal const& val);

using GetConfigurationResult_Internal = ::Everest::config::GetConfigurationResult;
using GetConfigurationResult_External = ::everest::lib::API::V1_0::types::config_service::GetConfigurationResult;

GetConfigurationResult_Internal to_internal_api(GetConfigurationResult_External const& val);
GetConfigurationResult_External to_external_api(GetConfigurationResult_Internal const& val);

} // namespace everest::config::api::types
