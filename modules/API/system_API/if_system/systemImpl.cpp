// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "systemImpl.hpp"
#include "basecamp/system/API.hpp"
#include "basecamp/system/codec.hpp"
#include "basecamp/system/json_codec.hpp"
#include "basecamp/system/wrapper.hpp"
#include "companion/asyncapi/AsyncApiRequestReply.hpp"
#include "everest/exceptions.hpp"
#include "everest/logging.hpp"
#include "framework/ModuleAdapter.hpp"
#include "generated/types/system.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace basecamp::companion;
namespace ns_types_ext = basecamp::API::V1_0::types::system;

namespace types {
namespace system {
void to_json(json& j, const ResetType& k) noexcept {
    j = reset_type_to_string(k);
}
void from_json(const json& j, ResetType& k) {
    k = string_to_reset_type(j);
}
} // namespace system
} // namespace types

namespace {
bool toExternalApi(bool value) {
    return value;
}
} // namespace

namespace module {
namespace if_system {

void systemImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void systemImpl::ready() {
}

template <class T, class ReqT>
auto systemImpl::generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic) {
    using namespace ns_types_ext;
    using ExtT = decltype(toExternalApi(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

types::system::UpdateFirmwareResponse
systemImpl::handle_update_firmware(types::system::FirmwareUpdateRequest& firmware_update_request) {
    static const types::system::UpdateFirmwareResponse default_response =
        types::system::UpdateFirmwareResponse::Rejected;
    return generic_request_reply(default_response, firmware_update_request, "update_firmware");
}

void systemImpl::handle_allow_firmware_installation() {
    auto topic = mod->get_topics().basecamp_to_extern("allow_firmware_installation");
    mod->mqtt.publish(topic, "");
}

types::system::UploadLogsResponse
systemImpl::handle_upload_logs(types::system::UploadLogsRequest& upload_logs_request) {
    static const types::system::UploadLogsResponse default_response =
        types::system::UploadLogsResponse{types::system::UploadLogsStatus::Rejected, {}};
    return generic_request_reply(default_response, upload_logs_request, "upload_logs");
}

bool systemImpl::handle_is_reset_allowed(types::system::ResetType& type) {
    static const bool default_response = false;
    return generic_request_reply(default_response, type, "is_reset_allowed");
}

void systemImpl::handle_reset(types::system::ResetType& type, bool& scheduled) {
    auto topic = mod->get_topics().basecamp_to_extern("reset");
    json args = ns_types_ext::ResetRequest{ns_types_ext::toExternalApi(type), scheduled};
    mod->mqtt.publish(topic, args.dump());
}

bool systemImpl::handle_set_system_time(std::string& timestamp) {
    static const bool default_response = false;
    return generic_request_reply(default_response, timestamp, "set_system_time");
}

types::system::BootReason systemImpl::handle_get_boot_reason() {
    static const types::system::BootReason default_respone = types::system::BootReason::Unknown;
    return generic_request_reply(default_respone, internal::empty_payload, "get_boot_reason");
}

} // namespace if_system
} // namespace module
