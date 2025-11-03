// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <memory>
#include <string>

#include <netinet/in.h>

namespace iso15118::io {

bool check_and_update_interface(std::string& interface_name);

bool get_first_sockaddr_in6_for_interface(const std::string& interface_name, sockaddr_in6& address);

std::unique_ptr<char[]> sockaddr_in6_to_name(const sockaddr_in6&);
} // namespace iso15118::io
