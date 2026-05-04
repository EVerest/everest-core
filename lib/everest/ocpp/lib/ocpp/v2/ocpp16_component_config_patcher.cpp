// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ocpp16_component_config_patcher.hpp>

#include <algorithm>
#include <array>
#include <functional>

#include <everest/logging.hpp>
#include <ocpp/v16/known_keys.hpp>

namespace ocpp::v2 {

namespace {

bool is_same_variable(const DeviceModelVariable& v1, const DeviceModelVariable& v2) {
    return ((v1.name == v2.name) && (v1.instance == v2.instance));
}

std::optional<std::reference_wrapper<DeviceModelVariable>>
get_variable_to_patch(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                      const Component& component, const Variable& variable) {

    DeviceModelVariable variable_to_patch;
    variable_to_patch.name = variable.name.get();
    if (variable.instance.has_value()) {
        variable_to_patch.instance = variable.instance->get();
    }

    ComponentKey component_key;
    component_key.name = component.name.get();
    if (component.instance.has_value()) {
        component_key.instance = component.instance.value().get();
    }

    auto component_it = component_configs.find(component_key);
    if (component_it == component_configs.end()) {
        EVLOG_warning << "Component " << component_key.name << " not found in component configs, cannot patch variable "
                      << variable.name.get();
        return std::nullopt;
    }

    auto& variables = component_it->second;
    auto variable_it =
        std::find_if(variables.begin(), variables.end(), [&variable_to_patch](const DeviceModelVariable& var) {
            return is_same_variable(var, variable_to_patch);
        });

    if (variable_it == variables.end()) {
        EVLOG_warning << "Variable " << variable.name.get() << " not found in component " << component_key.name
                      << ", cannot patch variable.";
        return std::nullopt;
    }

    return std::ref(*variable_it);
}

std::optional<std::reference_wrapper<DeviceModelVariable>>
get_variable_to_patch(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                      const ocpp::v16::keys::DeviceModel_CV& device_model_cv) {
    if (!device_model_cv.has_value()) {
        return std::nullopt;
    }
    return get_variable_to_patch(component_configs, device_model_cv->first, device_model_cv->second);
}

void patch_variable_value(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                          const ocpp::v16::keys::DeviceModel_CV device_model_cv, const std::string& value,
                          const std::optional<bool>& readonly_override = std::nullopt) {

    auto variable_to_patch = get_variable_to_patch(component_configs, device_model_cv);
    if (!variable_to_patch.has_value()) {
        return;
    }

    auto& dm_variable = variable_to_patch->get();
    auto attribute_it =
        std::find_if(dm_variable.attributes.begin(), dm_variable.attributes.end(), [](const DbVariableAttribute& attr) {
            return attr.variable_attribute.type.has_value() and
                   attr.variable_attribute.type.value() == AttributeEnum::Actual;
        });

    if (attribute_it == dm_variable.attributes.end()) {
        EVLOG_warning << "Attribute Actual not found in variable " << dm_variable.name << ", cannot patch variable.";
        return;
    }

    if (readonly_override.has_value()) {
        attribute_it->variable_attribute.mutability =
            readonly_override.value() ? MutabilityEnum::ReadOnly : MutabilityEnum::ReadWrite;
    }

    attribute_it->value_source = "OCPP16Config";
    attribute_it->variable_attribute.value = value;
}

void patch_variable_max_limit(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                              const ocpp::v16::keys::DeviceModel_CV& device_model_cv, const int32_t max_limit) {
    if (auto variable_to_patch = get_variable_to_patch(component_configs, device_model_cv)) {
        variable_to_patch->get().characteristics.maxLimit = static_cast<float>(max_limit);
    }
}

void patch_variable_values_list(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                                const ocpp::v16::keys::DeviceModel_CV& device_model_cv,
                                const std::string& values_list) {

    auto variable_to_patch = get_variable_to_patch(component_configs, device_model_cv);
    if (!variable_to_patch.has_value()) {
        return;
    }

    variable_to_patch->get().characteristics.valuesList = values_list;
}

void patch_optional_max_limit(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                              const std::string& ocpp16_key, const std::optional<int32_t>& max_limit) {
    if (!max_limit.has_value()) {
        return;
    }

    patch_variable_max_limit(component_configs, ocpp::v16::keys::convert_v2(ocpp16_key), max_limit.value());
}

void patch_max_limit(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                     const char* ocpp16_key, const int32_t max_limit) {
    patch_variable_max_limit(component_configs, ocpp::v16::keys::convert_v2(ocpp16_key), max_limit);
}

void patch_max_limit_from_key(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                              ocpp::v16::ChargePointConfiguration& ocpp16_config, const char* ocpp16_key) {
    try {
        const auto key_value = ocpp16_config.get(ocpp16_key);
        if (!key_value.has_value() || !key_value->value.has_value()) {
            return;
        }
        const auto max_limit = std::stoi(key_value->value.value().get());
        patch_max_limit(component_configs, ocpp16_key, max_limit);
    } catch (const std::exception& e) {
        EVLOG_warning << "Could not read or parse max-limit key " << ocpp16_key << ": " << e.what();
    }
}

void patch_meter_public_keys(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                             ocpp::v16::ChargePointConfiguration& ocpp16_config) {

    const auto meter_public_keys = ocpp16_config.getMeterPublicKeys();
    if (!meter_public_keys.has_value()) {
        return;
    }

    const auto meter_public_keys_csl = ocpp16_config.getMeterPublicKeysCsl();
    if (!meter_public_keys_csl.has_value()) {
        return;
    }

    patch_variable_value(component_configs, ocpp::v16::keys::convert_v2("MeterPublicKeys"),
                         meter_public_keys_csl.value());
}

void patch_default_price_text(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                              ocpp::v16::ChargePointConfiguration& ocpp16_config) {
    const auto default_price_text = ocpp16_config.getDefaultPriceText();
    if (!default_price_text.has_value()) {
        return;
    }

    patch_variable_value(component_configs, ocpp::v16::keys::convert_v2("DefaultPriceText"),
                         default_price_text.value().dump());
}

void patch_supported_measurands(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                                ocpp::v16::ChargePointConfiguration& ocpp16_config) {

    const auto supported_measurands = ocpp16_config.getSupportedMeasurands();

    const std::array<ocpp::v16::keys::DeviceModel_CV, 5> measurands_mappings = {
        std::make_pair(ocpp::v2::Component{"AlignedDataCtrlr"}, ocpp::v2::Variable{"Measurands"}),
        std::make_pair(ocpp::v2::Component{"AlignedDataCtrlr"}, ocpp::v2::Variable{"TxEndedMeasurands"}),
        std::make_pair(ocpp::v2::Component{"SampledDataCtrlr"}, ocpp::v2::Variable{"TxEndedMeasurands"}),
        std::make_pair(ocpp::v2::Component{"SampledDataCtrlr"}, ocpp::v2::Variable{"TxStartedMeasurands"}),
        std::make_pair(ocpp::v2::Component{"SampledDataCtrlr"}, ocpp::v2::Variable{"TxUpdatedMeasurands"}),
    };

    for (const auto& mapping : measurands_mappings) {
        patch_variable_values_list(component_configs, mapping, supported_measurands);
    }
}

} // namespace

void patch_component_config_with_ocpp16(std::map<ComponentKey, std::vector<DeviceModelVariable>>& component_configs,
                                        ocpp::v16::ChargePointConfiguration& ocpp16_config,
                                        const Ocpp16CustomConfigMappings& custom_config_mappings) {

    const auto config_keys = ocpp16_config.get_all_key_value();

    for (const auto& config_key : config_keys) {
        const auto key_enum = ocpp::v16::keys::convert(config_key.key.get());
        if (key_enum && ocpp::v16::keys::is_max_limit_key(*key_enum)) {
            continue;
        }

        auto mapping_opt = ocpp::v16::keys::convert_v2(config_key.key.get());

        if (!mapping_opt.has_value()) {
            const auto custom_mapping_it = custom_config_mappings.find(config_key.key.get());
            if (custom_mapping_it != custom_config_mappings.end()) {
                const auto& [comp, var] = custom_mapping_it->second;
                mapping_opt = std::make_pair(comp, var);
            }
        }

        if (!config_key.value.has_value()) {
            EVLOG_info << "OCPP 1.6 config key " << config_key.key.get()
                       << " has no value, skipping patching variable.";
            continue;
        }

        if (!mapping_opt.has_value()) {
            EVLOG_info << "No OCPP 2.x mapping found for OCPP 1.6 config key " << config_key.key.get()
                       << ", skipping patching variable.";
            continue;
        }

        try {
            patch_variable_value(component_configs, mapping_opt, config_key.value.value().get(), config_key.readonly);
        } catch (const std::exception& e) {
            EVLOG_error << "Failed to patch OCPP 1.6 config key " << config_key.key.get() << " with value "
                        << config_key.value.value().get() << ": " << e.what();
            throw;
        }
    }

    // Handling for special cases that cannot be covered by the generic mapping above:

    if (ocpp16_config.getAuthorizationKey().has_value()) {
        patch_variable_value(component_configs, ocpp::v16::keys::convert_v2("AuthorizationKey"),
                             ocpp16_config.getAuthorizationKey().value());
    }

    // MeterPublicKeys -> MeterPublicKey[0], MeterPublicKey[1], ...
    patch_meter_public_keys(component_configs, ocpp16_config);
    // DefaultPriceText,<lang> -> OCPP16LegacyCtrlr:DefaultPriceText as combined JSON object
    patch_default_price_text(component_configs, ocpp16_config);
    // MeterValuesAlignedDataMaxLength -> AlignedDataCtrlr:Measurands.maxLimit
    patch_optional_max_limit(component_configs, "MeterValuesAlignedDataMaxLength",
                             ocpp16_config.getMeterValuesAlignedDataMaxLength());
    // MeterValuesSampledDataMaxLength -> SampledDataCtrlr:TxUpdatedMeasurands.maxLimit
    patch_optional_max_limit(component_configs, "MeterValuesSampledDataMaxLength",
                             ocpp16_config.getMeterValuesSampledDataMaxLength());
    // StopTxnAlignedDataMaxLength -> AlignedDataCtrlr:TxEndedMeasurands.maxLimit
    patch_optional_max_limit(component_configs, "StopTxnAlignedDataMaxLength",
                             ocpp16_config.getStopTxnAlignedDataMaxLength());
    // StopTxnSampledDataMaxLength -> SampledDataCtrlr:TxEndedMeasurands.maxLimit
    patch_optional_max_limit(component_configs, "StopTxnSampledDataMaxLength",
                             ocpp16_config.getStopTxnSampledDataMaxLength());
    // LocalAuthListMaxLength -> LocalAuthListCtrlr:Entries.maxLimit
    patch_max_limit_from_key(component_configs, ocpp16_config, "LocalAuthListMaxLength");
    // MaxChargingProfilesInstalled -> SmartChargingCtrlr.ChargingProfileEntries.maxLimit
    patch_max_limit_from_key(component_configs, ocpp16_config, "MaxChargingProfilesInstalled");
    // CertificateStoreMaxLength -> SecurityCtrlr:CertificateEntries.maxLimit
    patch_optional_max_limit(component_configs, "CertificateStoreMaxLength",
                             ocpp16_config.getCertificateStoreMaxLength());
    // SupportedMeasurands -> *Measurands.characteristics.valuesList (patches for all aligned and sampled data
    // measurands variables)
    patch_supported_measurands(component_configs, ocpp16_config);

    // TODO: currently not patched because they are not configured as part of the JSON component configs but by the
    // EVerest device model:
    // - ConnectorPhaseRotation -> *:PhaseRotation (wildcard component - applies to EVSE instances)
    // - ConnectorEvseIds -> *EVSE:EvseId (wildcard EVSE)
}

} // namespace ocpp::v2