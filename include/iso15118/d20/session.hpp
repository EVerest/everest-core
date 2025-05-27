// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <variant>
#include <vector>

#include <iso15118/io/sha_hash.hpp>
#include <iso15118/message/common_types.hpp>

namespace iso15118::d20 {

namespace dt = message_20::datatypes;

struct OfferedServices {

    std::vector<dt::Authorization> auth_services;
    std::vector<dt::ServiceCategory> energy_services;
    std::vector<dt::ServiceCategory> vas_services;

    std::map<uint8_t, dt::DcParameterList> dc_parameter_list;
    std::map<uint8_t, dt::DcBptParameterList> dc_bpt_parameter_list;
    std::map<uint8_t, dt::McsParameterList> mcs_parameter_list;
    std::map<uint8_t, dt::McsBptParameterList> mcs_bpt_parameter_list;
    std::map<uint8_t, dt::InternetParameterList> internet_parameter_list;
    std::map<uint8_t, dt::ParkingParameterList> parking_parameter_list;
};

struct SelectedServiceParameters {

    dt::ServiceCategory selected_energy_service;

    std::variant<dt::AcConnector, dt::DcConnector, dt::McsConnector> selected_connector;
    dt::ControlMode selected_control_mode;
    dt::MobilityNeedsMode selected_mobility_needs_mode;
    dt::Pricing selected_pricing;

    std::optional<dt::BptChannel> selected_bpt_channel;
    std::optional<dt::GeneratorMode> selected_generator_mode;

    SelectedServiceParameters() = default;
    SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::DcConnector dc_connector_,
                              dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_, dt::Pricing pricing_);
    SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::DcConnector dc_connector_,
                              dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_, dt::Pricing pricing_,
                              dt::BptChannel channel_, dt::GeneratorMode generator_);
    SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::McsConnector mcs_connector_,
                              dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_, dt::Pricing pricing_);
    SelectedServiceParameters(dt::ServiceCategory energy_service_, dt::McsConnector mcs_connector_,
                              dt::ControlMode control_mode_, dt::MobilityNeedsMode mobility_, dt::Pricing pricing_,
                              dt::BptChannel channel_, dt::GeneratorMode generator_);
};

struct SelectedVasParameter {
    std::vector<dt::ServiceCategory> vas_services;

    dt::Protocol internet_protocol;
    dt::Port internet_port;

    dt::IntendedService parking_intended_service;
    dt::ParkingStatus parking_status;
};

// TODO(SL): How to handle d2 pause? Move Struct to a seperate header file?
// TODO(SL): Missing handling scheduletuple in schedule mode [V2G20-1058]
struct PauseContext {
    io::sha512_hash_t vehicle_cert_session_id_hash{};
    std::array<uint8_t, 8> old_session_id{};
    SelectedServiceParameters selected_service_parameters{};
};

class Session {

    // todo(sl): move to a common defs file
    static constexpr auto ID_LENGTH = 8;

public:
    Session();
    Session(const PauseContext& pause_ctx);
    Session(SelectedServiceParameters);
    Session(OfferedServices);

    std::array<uint8_t, ID_LENGTH> get_id() const {
        return id;
    }

    bool find_parameter_set_id(const dt::ServiceCategory service, int16_t id);

    void selected_service_parameters(const dt::ServiceCategory service, const uint16_t id);

    auto get_selected_services() const& {
        return selected_services;
    }

    ~Session();

    OfferedServices offered_services;

    bool service_renegotiation_supported{false};

private:
    // NOTE (aw): could be const
    std::array<uint8_t, ID_LENGTH> id{};

    SelectedServiceParameters selected_services{};
    SelectedVasParameter selected_vas_services{};
};

} // namespace iso15118::d20
