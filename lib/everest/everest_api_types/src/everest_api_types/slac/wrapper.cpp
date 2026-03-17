// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "slac/wrapper.hpp"
#include "slac/codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::slac {

SlacState_Internal to_internal_api(SlacState_External const& val) {
    using SrcT = SlacState_External;
    using TarT = SlacState_Internal;
    switch (val) {
    case SrcT::UNMATCHED:
        return TarT::UNMATCHED;
    case SrcT::MATCHING:
        return TarT::MATCHING;
    case SrcT::MATCHED:
        return TarT::MATCHED;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::slac::SlacState_External");
}

SlacState_External to_external_api(SlacState_Internal const& val) {
    using SrcT = SlacState_Internal;
    using TarT = SlacState_External;
    switch (val) {
    case SrcT::UNMATCHED:
        return TarT::UNMATCHED;
    case SrcT::MATCHING:
        return TarT::MATCHING;
    case SrcT::MATCHED:
        return TarT::MATCHED;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::slac::SlacState_Internal");
}

} // namespace everest::lib::API::V1_0::types::slac