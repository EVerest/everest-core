// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <generated/types/powermeter.hpp>

namespace module {
namespace utils {

std::string generate_session_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

types::powermeter::OCMFIdentificationType
convert_to_ocmf_identification_type(const types::authorization::IdTokenType& id_token_type) {
    switch (id_token_type) {
    case types::authorization::IdTokenType::Central:
        return types::powermeter::OCMFIdentificationType::CENTRAL;
    case types::authorization::IdTokenType::eMAID:
        return types::powermeter::OCMFIdentificationType::EMAID;
    case types::authorization::IdTokenType::ISO14443:
        return types::powermeter::OCMFIdentificationType::ISO14443;
    case types::authorization::IdTokenType::ISO15693:
        return types::powermeter::OCMFIdentificationType::ISO15693;
    case types::authorization::IdTokenType::KeyCode:
        return types::powermeter::OCMFIdentificationType::KEY_CODE;
    case types::authorization::IdTokenType::Local:
        return types::powermeter::OCMFIdentificationType::LOCAL;
    case types::authorization::IdTokenType::NoAuthorization:
        return types::powermeter::OCMFIdentificationType::NONE;
    case types::authorization::IdTokenType::MacAddress:
        return types::powermeter::OCMFIdentificationType::UNDEFINED;
    }
    return types::powermeter::OCMFIdentificationType::UNDEFINED;
}

} // namespace utils
} // namespace module
