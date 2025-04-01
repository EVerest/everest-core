// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <optional>
#include <string>

#include <Enums.hpp>

// This contains types for all the JSON objects

namespace data {
    struct ConnectorInfoObj {
        int id;
        ConnectorTypeEnum type;
        std::optional<std::string> description;
      }

    struct EVSEInfoObj {
        std::string id;
        std::optional<std::string> description;

        bool bidi_charging;
    }
}