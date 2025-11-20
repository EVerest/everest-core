// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <string>
#include <string_view>

namespace everest::lib::API {

class Topics {
public:
    Topics() = default;
    Topics(Topics const&) = default;
    Topics(Topics&&) = default;

    ~Topics() = default;

    Topics& operator=(Topics const&) = default;
    Topics& operator=(Topics&&) = default;

    void setup(std::string const& target_module_id, std::string const& api_type, unsigned int version);
    std::string everest_to_extern(const std::string& var) const;
    std::string extern_to_everest(const std::string& var) const;
    std::string reply_to_everest(const std::string& reply) const;

    static const std::string api_base;
    static const std::string api_out;
    static const std::string api_in;

private:
    std::string m_target_module_id;
    std::string m_api_type;
    std::string m_api_version;
};

} // namespace everest::lib::API
