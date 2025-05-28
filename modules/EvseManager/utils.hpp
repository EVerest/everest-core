// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _EVSEMANAGER_UTILS_H_
#define _EVSEMANAGER_UTILS_H_

#include <everest/staging/helpers/helpers.hpp>
#include <generated/types/powermeter.hpp>

namespace module {
namespace utils {

enum class SessionIdType {
    UUID,
    UUID_BASE64,
    SHORT_BASE64
};

inline SessionIdType get_session_id_type_from_string(const std::string& type) {
    if (type == "UUID_BASE64") {
        return SessionIdType::UUID_BASE64;
    } else if (type == "SHORT_BASE64") {
        return SessionIdType::SHORT_BASE64;
    }
    // Default to UUID if the type is unknown
    return SessionIdType::UUID;
}

inline std::string generate_session_id(SessionIdType type = SessionIdType::UUID) {
    if (type == SessionIdType::SHORT_BASE64) {
        return everest::staging::helpers::get_base64_id();
    } else if (type == SessionIdType::UUID_BASE64) {
        return everest::staging::helpers::get_base64_uuid();
    }
    return everest::staging::helpers::get_uuid();
}

inline types::powermeter::OCMFIdentificationType
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

// Simple helper class to measure time in ms
class Stopwatch {
public:
    Stopwatch() {
        reset();
    }

    void reset() {
        start_time = std::chrono::steady_clock::now();
        phase_start_time = std::chrono::steady_clock::now();
        intitial_start_time = std::chrono::steady_clock::now();
        rep.clear();
        current_rep.clear();
    }

    int mark_phase(const std::string& s) {
        int ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - phase_start_time)
                .count();
        phase_start_time = start_time = std::chrono::steady_clock::now();
        current_rep = s;
        return ms;
    }

    int mark(const std::string& s) {
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time)
                     .count();
        start_time = std::chrono::steady_clock::now();
        current_rep += fmt::format(" | {}: {} ms", s, ms);
        return ms;
    }

    std::string report_phase() {
        int ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - phase_start_time)
                .count();
        current_rep += fmt::format(" | Total: {} ms", ms);
        rep.push_back(current_rep);
        return current_rep;
    }

    std::vector<std::string> report_all_phases() {
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                       intitial_start_time)
                     .count();
        rep.push_back(fmt::format("Total duration all phases: {} ms", ms));
        return rep;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time, intitial_start_time, phase_start_time;
    std::string current_rep;
    std::vector<std::string> rep;
};

} // namespace utils
} // namespace module

#endif