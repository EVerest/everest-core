// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "ErrorHandler.hpp"
#include <everest/logging.hpp>
#include <stdexcept>

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
namespace helpers {

void handle_error_raised(data::DataStoreCharger& data, const RPCDataTypes::ErrorObj& error) {
    try {
        data.chargererrors.add_error(error);
    } catch (const std::runtime_error& e) {
        EVLOG_warning << "Error while adding error to the data store: " << e.what();
    }

    // only set error present for EVSE specific errors. Index 0 is reserved for the charger itself
    if (!error.origin.evse_index.has_value() || error.origin.evse_index.value() == 0) {
        return;
    }
    auto tmp_evse_store = data.get_evse_store(error.origin.evse_index.value());
    if (tmp_evse_store != nullptr) {
        EVLOG_debug << "Setting error present for EVSE index: " << error.origin.evse_index.value();
        tmp_evse_store->evsestatus.set_error_present(true);
    } else {
        EVLOG_error << "Cannot set error present for EVSE index: " << error.origin.evse_index.value();
    }
}

void handle_error_cleared(data::DataStoreCharger& data, const RPCDataTypes::ErrorObj& error) {
    try {
        data.chargererrors.clear_error(error);
    } catch (const std::runtime_error& e) {
        EVLOG_warning << "Error while clearing error from the data store: " << e.what();
    }

    // only clear error present for EVSE specific errors. Index 0 is reserved for the charger itself
    if (!error.origin.evse_index.has_value() || error.origin.evse_index.value() == 0) {
        return;
    }

    auto tmp_charger_errors = data.chargererrors.get_data();
    if (tmp_charger_errors.has_value()) {
        for (const auto& charger_error : tmp_charger_errors.value()) {
            if (charger_error.origin.evse_index.has_value() &&
                charger_error.origin.evse_index.value() == error.origin.evse_index.value()) {
                return;
            }
        }
    }
    auto tmp_evse_store = data.get_evse_store(error.origin.evse_index.value());
    if (tmp_evse_store != nullptr) {
        tmp_evse_store->evsestatus.set_error_present(false);
    }
}

} // namespace helpers
