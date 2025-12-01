// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/utilities/logging.hpp>
#include <iomanip>
namespace charge_bridge::utilities {

enum class color {
    error,
    success,
    warning,
    message,
    unit,
    standard,
    terminal,
};

std::ostream& operator<<(std::ostream& s, color c) {
    switch (c) {
    case color::error:
        s << "\033[31m";
        break;
    case color::success:
        s << "\033[32m";
        break;
    case color::warning:
        s << "\033[33m";
        break;
    case color::message:
        s << "\033[37m";
        break;
    case color::unit:
        s << "\033[1;37m";
        break;
    case color::terminal:
        s << "\033[m";
        break;
    case color::standard:
    default:
        s << "\033[39;49m";
    }
    return s;
}

std::ostream& print_error(std::string const& device, std::string const& unit, int status) {
    // clang-format off
    auto ctrl =
        status == 0 ? color::success :
        status == -1 ? color::warning:
        color::error;
    std::cout << "[ " << ctrl << std::setw(13) << std::left << unit << color::terminal << " ] "
              << color::unit << std::setw(20) << device << color::terminal << " ";
    if(status not_eq 0){
        if(status == -1){
            std::cout << color::standard << "WARNING ";
        }
        else{
            std::cout << color::standard << "ERROR ( " << status << " ) ";
        }
    }
    return std::cout << color::standard;
    // clang-format on
}

} // namespace charge_bridge::utilities
