// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "error_handler.hpp"

namespace module {

using Everest::error::Severity;

ErrorHandler::ErrorHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                           connector_lockImplBase* p_connector_lock) :
    p_board_support(p_board_support), p_rcd(p_rcd), p_connector_lock(p_connector_lock) {
}

Everest::error::Error create_error(const Everest::error::ErrorFactory& error_factory, const ErrorDefinition& def) {
    return error_factory.create_error(def.type, def.sub_type, def.message, def.severity);
}

template <typename HandleType>
void forward_error(HandleType& handle, const Everest::error::Error& error, const bool raise) {
    if (raise) {
        handle->raise_error(error);
    } else {
        handle->clear_error(error.type);
    }
}

} // namespace module