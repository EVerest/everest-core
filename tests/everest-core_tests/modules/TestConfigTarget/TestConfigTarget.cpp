// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TestConfigTarget.hpp"

namespace module {

void TestConfigTarget::init() {
    invoke_init(*p_main);
}

void TestConfigTarget::ready() {
    invoke_ready(*p_main);
}

TestConfigTarget::ConfigChangeResult TestConfigTarget::on_rw_param_changed(const std::string& value) {
    rw_config.rw_param = value;
    return ConfigChangeResult::Accepted();
}

TestConfigTarget::ConfigChangeResult TestConfigTarget::on_rw_reboot_param_changed(const std::string& value) {
    rw_config.rw_reboot_param = value;
    return ConfigChangeResult::AcceptedRebootRequired();
}

TestConfigTarget::ConfigChangeResult TestConfigTarget::on_rw_reject_param_changed(const std::string& /* value */) {
    return ConfigChangeResult::Rejected("test rejection reason");
}

} // namespace module
