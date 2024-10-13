#pragma once

#include <optional>
#include <string>
#include <tuple>

#include <utils/error.hpp>

enum class ErrorTarget {
    BoardSupport,
    ConnectorLock,
    Rcd,
};

struct ErrorDefinition {
    const char* type;
    const char* sub_type;
    const char* message;
    Everest::error::Severity severity;
    ErrorTarget target;
};

std::tuple<bool, std::optional<ErrorDefinition>> parse_error(const std::string& payload);

template <typename T> void forward_error(T& target, const Everest::error::Error& error, bool raise) {
    if (raise) {
        target.raise_error(error);
    } else {
        target.clear_error(error.type);
    }
}

namespace error_definitions {
namespace {
constexpr auto create_default(const char* type, ErrorTarget target) {
    return ErrorDefinition{type, "", "Simulated fault event", Everest::error::Severity::High, target};
};

constexpr auto evse_bsp_error(const char* type) {
    return create_default(type, ErrorTarget::BoardSupport);
}
} // namespace

inline constexpr auto diode_fault = evse_bsp_error("evse_board_support/DiodeFault");
inline constexpr auto brown_out = evse_bsp_error("evse_board_support/BrownOut");
inline constexpr auto energy_managment = evse_bsp_error("evse_board_support/EnergyManagement");

} // namespace error_definitions