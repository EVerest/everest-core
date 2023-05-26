// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/optional.hpp>
#include <map>

#include <ocpp/common/pki_handler.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/device_model_management_base.hpp>

namespace ocpp {
namespace v201 {

/// \brief Class for configuration and monitoring of OCPP components and variables
class DeviceModelManager : public DeviceModelManagerBase {

public:
    /// \brief Construct a new DeviceModelManager object
    /// \param config OCPP json config
    /// \param ocpp_main_path path where utility files for OCPP are read and written to
    DeviceModelManager(const json& config, const std::string& ocpp_main_path);

    /// \brief Set the variable specified by \p set_variable_data
    /// \param set_variable_data specifies the variable to be set
    /// \return SetVariableStatusEnum indicating the result of the operation
    SetVariableStatusEnum set_variable(const SetVariableData& set_variable_data);

    /// \brief Get the variable specified by \p get_variable_data
    /// \param get_variable_data specifies the variable to get
    /// \return std::pair<GetVariableStatusEnum, std::optional<CiString<2500>>> first item of the pair indicates the
    /// result of the operation and the second item optionally contains the value
    std::pair<GetVariableStatusEnum, std::optional<CiString<2500>>>
    get_variable(const GetVariableData& get_variable_data);

    /// \brief This function returns an std::vector<ReportData> based on the options specified by the arguments \p
    /// report_base, \p component_variables and \p component_criteria
    /// \param report_base optionally specifies the type of inventory data that should be included in the result
    /// \param component_variables optionally specifies std::vector<ComponentVariables> that should be included in the
    /// result
    /// \param component_criteria optionally specifies std::vector<ComponentCriterionEnum> that should be
    /// included in the result
    /// \return std::vector<ReportData>
    std::vector<ReportData>
    get_report_data(const std::optional<ReportBaseEnum>& report_base = std::nullopt,
                    const std::optional<std::vector<ComponentVariable>>& component_variables = std::nullopt,
                    const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria = std::nullopt);
};

} // namespace v201
} // namespace ocpp
