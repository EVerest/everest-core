// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/utilities/symlink.hpp>

#include <fcntl.h>
#include <unistd.h>

namespace charge_bridge::utilities {

symlink::symlink() {
}

symlink::symlink(std::string const& src, std::string const& tar) {
    auto result = set_link(src, tar);
    if (not result) {
        std::string msg = "Cannot create symbolic link from '" + src + "' to '" + tar + "'";
        perror(msg.c_str());
    }
}

bool symlink::set_link(std::string const& src, std::string const& tar) {
    m_tar = tar;
    del_link();
    auto result = ::symlink(src.c_str(), tar.c_str()) == 0;
    if (result) {
        m_tar = tar;
    }
    return result;
}

bool symlink::del_link() {
    auto result = true;
    if (not m_tar.empty()) {
        auto code = ::unlink(m_tar.c_str());
        result = code == 0 or code == ENOENT;
        m_tar = "";
    }
    return result;
}

symlink::~symlink() {
    if (not m_tar.empty()) {
        auto result = del_link();
        if (not result) {
            std::string msg = "Cannot delete symbolic link '" + m_tar + "'";
            perror(msg.c_str());
        }
    }
}

} // namespace charge_bridge::utilities
