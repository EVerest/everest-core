// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_token_validatorImpl.hpp"
#include "basecamp/auth/API.hpp"
#include "basecamp/auth/codec.hpp"
#include "basecamp/auth/json_codec.hpp"
#include "basecamp/auth/wrapper.hpp"
#include "companion/asyncapi/AsyncApiRequestReply.hpp"
#include "generated/types/authorization.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace basecamp::companion;
namespace ns_types_ext = basecamp::API::V1_0::types::auth;

namespace module {
namespace auth_token_validator {

void auth_token_validatorImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void auth_token_validatorImpl::ready() {
}

template <class T, class ReqT>
auto auth_token_validatorImpl::generic_request_reply(T const& default_value, ReqT const& request,
                                                     std::string const& topic) {
    using namespace ns_types_ext;
    using ExtT = decltype(toExternalApi(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {
    static const types::authorization::ValidationResult default_response{
        types::authorization::AuthorizationStatus::Invalid, {}, {}, {}, {}, {}, {}};
    return generic_request_reply(default_response, provided_token, "validate_token");
}

} // namespace auth_token_validator
} // namespace module
