// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_KNOWN_MODEL_HPP
#define POWERMETER_BSM_KNOWN_MODEL_HPP

#include "BSMSnapshotModel.hpp"
#include "sunspec_models.hpp" // sunspec models

/**
 * The namespace known_model contains predefined, widly known sunpec data models.
 * This includes standard models (prefixed with "Sunspec_") and custom models,
 * (prefixed with something vendor related).
 */
namespace known_model {

// this describes where to get the model data: the offset to the sunspec base address and the model length.
struct AddressData {
    const protocol_related_types::SunspecDataModelAddress base_offset;
    const std::size_t model_size;
};

extern const AddressData Sunspec_Common;
extern const AddressData Sunspec_ACMeter;
extern const AddressData BSM_CurrentSnapshot;
extern const AddressData BSM_OCMF_CurrentSnapshot;

} // namespace known_model

#endif // POWERMETER_BSM_KNOWN_MODEL_HPP
