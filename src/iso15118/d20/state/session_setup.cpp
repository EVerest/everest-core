// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>
#include <iomanip>
#include <openssl/evp.h>
#include <sstream>

#include <iso15118/d20/state/authorization_setup.hpp>
#include <iso15118/d20/state/dc_charge_parameter_discovery.hpp>
#include <iso15118/d20/state/session_setup.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/session_setup.hpp>
#include <iso15118/detail/helper.hpp>
#include <iso15118/io/sha_hash.hpp>

namespace iso15118::d20::state {

namespace {

std::string session_id_to_string(const message_20::datatypes::SessionId& session_id) {
    std::stringstream ss;
    ss << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(session_id[0]);
    for (unsigned int i = 1; i < session_id.size(); ++i) {
        ss << ", 0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
           << (int)static_cast<int>(session_id[i]);
    }
    return ss.str();
}

bool session_is_zero(const message_20::datatypes::SessionId& session_id) {
    return std::all_of(session_id.begin(), session_id.end(), [](int i) { return i == 0; });
}

io::sha512_hash_t calculate_new_cert_session_id_hash(const io::sha512_hash_t& vehicle_cert_hash,
                                                     const message_20::datatypes::SessionId& session_id) {
    io::sha512_hash_t session_id_vehicle_hash{};
    std::array<std::uint8_t, 64 + 8> concatenated_session_id_vehicle{};

    std::copy(session_id.begin(), session_id.end(), concatenated_session_id_vehicle.begin());
    std::copy(vehicle_cert_hash.begin(), vehicle_cert_hash.end(),
              concatenated_session_id_vehicle.begin() + session_id.size());

    unsigned int digestlen{0};

    const auto result = EVP_Digest(concatenated_session_id_vehicle.data(), concatenated_session_id_vehicle.size(),
                                   session_id_vehicle_hash.data(), &digestlen, EVP_sha512(), nullptr);
    if (not result) {
        logf_error("X509_digest failed");
        return std::array<std::uint8_t, 64>{};
    }

    return session_id_vehicle_hash;
}
} // namespace

namespace dt = message_20::datatypes;

message_20::SessionSetupResponse handle_request([[maybe_unused]] const message_20::SessionSetupRequest& req,
                                                const d20::Session& session, const std::string& evse_id,
                                                bool new_session) {

    message_20::SessionSetupResponse res;
    setup_header(res.header, session);

    res.evseid = evse_id;

    if (new_session) {
        return response_with_code(res, dt::ResponseCode::OK_NewSessionEstablished);
    } else {
        return response_with_code(res, dt::ResponseCode::OK_OldSessionJoined);
    }
}

void SessionSetup::enter() {
    m_ctx.log.enter_state("SessionSetup");
}

Result SessionSetup::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::SessionSetupRequest>()) {

        logf_info("Received session setup with evccid: %s", req->evccid.c_str());
        m_ctx.feedback.evcc_id(req->evccid);

        bool new_session{false};

        const auto vehicle_cert_hash = m_ctx.get_new_vehicle_cert_hash();

        if (session_is_zero(req->header.session_id) or not vehicle_cert_hash.has_value() or
            not m_ctx.pause_ctx.has_value()) {
            m_ctx.session = Session();
            new_session = true;
        } else {
            const auto& pause_ctx = m_ctx.pause_ctx.value();
            const auto new_vehicle_cert_session_hash =
                calculate_new_cert_session_id_hash(vehicle_cert_hash.value(), req->header.session_id);

            if (pause_ctx.vehicle_cert_session_id_hash == new_vehicle_cert_session_hash) {
                logf_info("Old session resumed with session_id: %s",
                          session_id_to_string(req->header.session_id).c_str());
                m_ctx.session = Session(pause_ctx);
            } else {
                m_ctx.session = Session();
                new_session = true;
            }
        }

        if (new_session) {
            logf_info("New session created with session_id: %s", session_id_to_string(m_ctx.session.get_id()).c_str());
            if (vehicle_cert_hash) {
                auto& pause_ctx = m_ctx.pause_ctx.emplace();
                pause_ctx.vehicle_cert_session_id_hash =
                    calculate_new_cert_session_id_hash(vehicle_cert_hash.value(), m_ctx.session.get_id());
                pause_ctx.old_session_id = m_ctx.session.get_id();
            }
        }

        evse_id = m_ctx.session_config.evse_id;

        const auto res = handle_request(*req, m_ctx.session, evse_id, new_session);

        m_ctx.respond(res);

        if (not new_session) {
            const auto& pause_selected_energy_service = m_ctx.session.get_selected_services().selected_energy_service;
            if (pause_selected_energy_service == message_20::datatypes::ServiceCategory::AC or
                pause_selected_energy_service == message_20::datatypes::ServiceCategory::AC_BPT) {
                // TODO(sl): Missing AC charge parameter discovery state
                return {};
            } else if (pause_selected_energy_service == message_20::datatypes::ServiceCategory::DC or
                       pause_selected_energy_service == message_20::datatypes::ServiceCategory::DC_BPT) {
                return m_ctx.create_state<DC_ChargeParameterDiscovery>();
            }
            // TODO(sl): Error handling
            return {};
        }
        return m_ctx.create_state<AuthorizationSetup>();

    } else {
        m_ctx.log("expected SessionSetupReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
