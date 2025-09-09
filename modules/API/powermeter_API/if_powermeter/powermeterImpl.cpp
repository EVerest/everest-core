// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include <everest_api_types/powermeter/API.hpp>
#include <everest_api_types/powermeter/codec.hpp>
#include <everest_api_types/powermeter/json_codec.hpp>
#include <everest_api_types/powermeter/wrapper.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>
#include "framework/ModuleAdapter.hpp"
#include "generated/types/powermeter.hpp"
#include "utils/types.hpp"
#include <optional>

namespace module {
namespace if_powermeter {

namespace ns_types_ext = everest::lib::API::V1_0::types::powermeter;
namespace ns_types_int = types::powermeter;
using status = ns_types_int::TransactionRequestStatus;

using namespace everest::lib::API;

void powermeterImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void powermeterImpl::ready() {
}

template <class T, class ReqT>
auto powermeterImpl::generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic) {
    using ExtT = decltype(ns_types_ext::to_external_api(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

ns_types_int::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    static const ns_types_int::TransactionStartResponse default_response{status::UNEXPECTED_ERROR, std::nullopt,
                                                                         std::nullopt, std::nullopt};
    return generic_request_reply(default_response, value, "start_transaction");
}

ns_types_int::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    static const ns_types_int::TransactionStopResponse default_response{status::UNEXPECTED_ERROR, std::nullopt,
                                                                        std::nullopt, std::nullopt};
    return generic_request_reply(default_response, transaction_id, "stop_transaction");
}

} // namespace if_powermeter
} // namespace module
