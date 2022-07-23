// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef V2G_MESSAGE_HPP
#define V2G_MESSAGE_HPP

#include <string>
#include <pugixml.hpp>

namespace module {
/*
 Simple V2G XML message helper
*/

class v2g_message {
public:
    v2g_message();
    bool from_xml(const std::string& xml);
    std::string to_xml();

private:
    pugi::xml_document doc;
};

} // namespace module

#endif // V2G_MESSAGE_HPP
