// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ac_rcdImpl.hpp"
#include "companion/asyncapi/AsyncApiRequestReply.hpp"

namespace {
bool toExternalApi(bool value) {
    return value;
}
} // namespace

namespace module {
namespace rcd {

using namespace basecamp::companion;

void ac_rcdImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void ac_rcdImpl::ready() {
}

template <class T, class ReqT>
auto ac_rcdImpl::generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic) {
    using namespace ns_types_ext;
    using ExtT = decltype(toExternalApi(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

void ac_rcdImpl::handle_self_test() {
    auto topic = mod->get_topics().basecamp_to_extern("self_test");
    mod->mqtt.publish(topic, "");
}

bool ac_rcdImpl::handle_reset() {
    static bool default_response = false;
    return generic_request_reply(default_response, internal::empty_payload, "reset");
}

} // namespace rcd
} // namespace module
