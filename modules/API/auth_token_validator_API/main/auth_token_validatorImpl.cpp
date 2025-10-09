// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_token_validatorImpl.hpp"

#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/codec.hpp>
#include <everest_api_types/auth/json_codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>

#include <generated/types/authorization.hpp>

namespace API_types_ext = ev_API::V1_0::types::auth;

namespace module {
namespace main {

void auth_token_validatorImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void auth_token_validatorImpl::ready() {
}

template <class T, class ReqT>
auto auth_token_validatorImpl::generic_request_reply(T const& default_value, ReqT const& request,
                                                     std::string const& topic) {
    using namespace API_types_ext;
    using ExtT = decltype(to_external_api(std::declval<T>()));
    auto result = ev_API::request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {
    static const types::authorization::ValidationResult default_response{
        types::authorization::AuthorizationStatus::Invalid, {}, {}, {}, {}, {}, {}, {}};
    return generic_request_reply(default_response, API_types_ext::to_external_api(provided_token), "validate_token");
}

} // namespace main
} // namespace module
