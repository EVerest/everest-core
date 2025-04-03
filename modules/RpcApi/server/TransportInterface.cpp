// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "TransportInterface.hpp"

namespace server {

std::string TransportInterface::server_name() const {
    return m_server_name;
}

std::string TransportInterface::server_url() const {
    return m_server_url;
}

void TransportInterface::set_server_url(const std::string &server_url) {
    m_server_url = server_url;
}

}
