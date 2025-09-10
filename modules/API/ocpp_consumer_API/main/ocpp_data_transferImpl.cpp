// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp_data_transferImpl.hpp"

#include <everest_api_types/ocpp/codec.hpp>
#include <everest_api_types/ocpp/json_codec.hpp>
#include <everest_api_types/ocpp/wrapper.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>

using namespace everest::lib::API;
namespace ns_types_ext = everest::lib::API::V1_0::types::ocpp;

namespace module {
namespace main {

void ocpp_data_transferImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void ocpp_data_transferImpl::ready() {
}

template <class T, class ReqT>
auto ocpp_data_transferImpl::generic_request_reply(T const& default_value, ReqT const& request,
                                                   std::string const& topic) {
    using namespace ns_types_ext;
    using ExtT = decltype(to_external_api(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

types::ocpp::DataTransferResponse
ocpp_data_transferImpl::handle_data_transfer(types::ocpp::DataTransferRequest& request) {
    types::ocpp::DataTransferResponse default_response{types::ocpp::DataTransferStatus::Offline, {}, {}};
    return generic_request_reply(default_response, request, "data_transfer_incoming");
}

} // namespace main
} // namespace module
