// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <map>
#include <ocpp/v2/ocpp_types.hpp>
#include <utils/config_service.hpp>

/// \brief This class is used to map EVerest module configuration parameters to OCPP component variables
class VariableMapping {

public:
    /// \brief Constructor that loads the mapping from the given \p mapping_file and validates it against the schema in
    /// \p schema_file
    VariableMapping(const fs::path& mapping_file, const fs::path& schema_file);

    /// \brief EVerest modules are represented as OCPP component variables in the OCPP device model. The following
    /// functions adds a bi-directional mapping for the EVerest component variables and (mostly standardized) OCPP
    /// component variables.
    void add_cv_mapping(const ocpp::v2::ComponentVariable& everest_component_variable,
                        const ocpp::v2::ComponentVariable& ocpp_component_variable);

    /// \brief Gets a component variable from the given ȨVerest configuration parameter \p identifier
    /// \return An optional OCPP component variable if the mapping exists, otherwise std::nullopt
    std::optional<ocpp::v2::ComponentVariable>
    get_ocpp_cv(const everest::config::ConfigurationParameterIdentifier& identifier);

    /// \brief Gets a component variable from the given \p everest_component_variable
    /// \return An optional OCPP component variable if the mapping exists, otherwise std::nullopt
    std::optional<ocpp::v2::ComponentVariable>
    get_ocpp_cv(const ocpp::v2::ComponentVariable& everest_component_variable);

    /// \brief Gets the EVerest component variable for the given \p ocpp_component_variable
    std::optional<ocpp::v2::ComponentVariable>
    get_everest_cv(const ocpp::v2::ComponentVariable& ocpp_component_variable);

private:
    std::map<everest::config::ConfigurationParameterIdentifier, ocpp::v2::ComponentVariable>
        user_mapping; // Maps EVerest configuration parameters to OCPP component variables

    // EVerest modules are represented as OCPP component variables in the OCPP device model. The following maps are
    // bi-directional mappings to map between EVerest component variables and (mostly standardized) OCPP component
    // variables.
    std::map<ocpp::v2::ComponentVariable, ocpp::v2::ComponentVariable> everest_cv_to_ocpp_cv_mapping;
    std::map<ocpp::v2::ComponentVariable, ocpp::v2::ComponentVariable> ocpp_cv_to_everest_cv_mapping;
};