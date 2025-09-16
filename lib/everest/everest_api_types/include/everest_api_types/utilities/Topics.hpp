// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <string>
#include <string_view>

namespace everest::lib::API {

class Topics {
public:
    Topics() = default;
    Topics(const std::string& target_module_id);

    void setTargetApiModuleID(const std::string& target_module_id, const std::string& api_type);

    std::string everest_to_extern(const std::string& var) const;
    std::string extern_to_everest(const std::string& var) const;
    std::string reply_to_everest(const std::string& reply) const;

    static const std::string api_base;
    static const std::string api_version;
    static const std::string api_out;
    static const std::string api_in;

private:
    std::string target_module_id;
    std::string api_type;
};

} // namespace everest::lib::API
