// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <array>
#include <ctype.h>
#include <optional>
#include <string>
#include <vector>

namespace iso15118::d2::msg {

namespace data_types {
using ServiceName = std::string;                      // MaxLength: 32
using ServiceScope = std::string;                     // MaxLength: 64
using PaymentOptionList = std::vector<PaymentOption>; // [1 - 2]

enum class ServiceCategory {
    EvCharging,
    Internet,
    ContractCertificate,
    OtherCustom
};

enum class EnergyTransferMode {
    AC_single_phase_core,
    AC_three_phase_core,
    DC_core,
    DC_extended,
    DC_combo_core,
    DC_unique
};
using SupportedEnergyTransferMode = std::vector<EnergyTransferMode>; // MaxLength: 6
constexpr auto SupportedEnergyTransferModeMaxLength = 6;

struct Service {
    ServiceID service_id;
    std::optional<ServiceName> service_name{std::nullopt};
    ServiceCategory service_category;
    std::optional<ServiceScope> service_scope{std::nullopt};
    bool FreeService;
};
using ServiceList = std::vector<Service>; // [1 - 8]
constexpr auto ServiceListMaxLength = 8;

struct ChargeService : Service {
    SupportedEnergyTransferMode supported_energy_transfer_mode;
};
} // namespace data_types

struct ServiceDiscoveryRequest {
    Header header;
    std::optional<data_types::ServiceScope> service_scope{std::nullopt};
    std::optional<data_types::ServiceCategory> service_category{std::nullopt};
};

struct ServiceDiscoveryResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::PaymentOptionList payment_option_list;
    data_types::ChargeService charge_service;
    std::optional<data_types::ServiceList> service_list{std::nullopt};
};

} // namespace iso15118::d2::msg
