#include "errors.hpp"

#include <nlohmann/json.hpp>

std::tuple<bool, std::optional<ErrorDefinition>> parse_error(const std::string& payload) {
    const auto dict = nlohmann::json::parse(payload);

    const auto raise = dict.value("raise", false);

    const auto error_type = static_cast<std::string>(dict.at("error_type"));

    if (error_type == "DiodeFault") {
        return {raise, error_definitions::diode_fault};
    } else if (error_type == "BrownOut") {
        return {raise, error_definitions::brown_out};
    } else if (error_type == "EnergyManagment") {
        return {raise, error_definitions::energy_managment};
    }

    return {raise, std::nullopt};
}
