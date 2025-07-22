// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "../data/DataStore.hpp"
#include "ld-ev.hpp"

namespace helpers {
    void handle_error_raised(data::DataStoreCharger& data, const Everest::error::Error& error);
    void handle_error_cleared(data::DataStoreCharger& data, const Everest::error::Error& error);
}
#endif // ERROR_HANDLER_HPP
