// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

namespace charge_bridge::utilities {
class symlink {
public:
    symlink(std::string const& src, std::string const& tar);
    symlink();
    bool set_link(std::string const& src, std::string const& tar);
    bool del_link();
    ~symlink();

private:
    std::string m_tar;
};
} // namespace charge_bridge::utilities
