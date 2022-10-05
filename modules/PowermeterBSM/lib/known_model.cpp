// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#include "known_model.hpp"

namespace known_model {

/*
 * Warning: only specify *addresses* relative to the sunspec base address,
 * the transport layer adds the relative addresses to the sunspec base address when querying the device.
 */

const AddressData Sunspec_Common{3_sma, calc_model_size_in_register(sunspec_model::Common::Model)};
const AddressData Sunspec_ACMeter{91_sma, calc_model_size_in_register(sunspec_model::ACMeter::Model)};

// TODO: this starts reading two registers after the "official beginning of the model, we dont need model id and payload
// length?
const AddressData BSM_CurrentSnapshot{524_sma, calc_model_size_in_register(bsm::SignedSnapshot::Model)};
// the turn on/turn off snapshots go here

const AddressData BSM_OCMF_CurrentSnapshot{1792_sma, calc_model_size_in_register(bsm::SignedOCMFSnapshot::Model)};
// the turn on/turn off snapshots go here

} // namespace known_model
