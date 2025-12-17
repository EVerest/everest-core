// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/session/iso.hpp>

#include <cassert>
#include <chrono>
#include <cstring>
#include <thread>

#include <endian.h>

#include <iso15118/d20/state/supported_app_protocol.hpp>

#include <iso15118/detail/helper.hpp>

namespace iso15118 {

static constexpr auto SESSION_IDLE_TIMEOUT_MS = 5000;

static void log_sdp_packet(const iso15118::io::SdpPacket& sdp) {
    static constexpr auto ESCAPED_BYTE_CHAR_COUNT = 4;
    auto payload_string_buffer = std::make_unique<char[]>(sdp.get_payload_length() * ESCAPED_BYTE_CHAR_COUNT + 1);
    for (std::size_t i = 0; i < sdp.get_payload_length(); ++i) {
        snprintf(payload_string_buffer.get() + i * ESCAPED_BYTE_CHAR_COUNT, ESCAPED_BYTE_CHAR_COUNT + 1, "\\x%02hx",
                 sdp.get_payload_buffer()[i]);
    }

    iso15118::logf_info("[SDP Packet in]: Header: %04hx, Payload: %s", sdp.get_payload_type(),
                        payload_string_buffer.get());
}

static void log_packet_from_car(const iso15118::io::SdpPacket& packet, session::SessionLogger& logger) {
    logger.exi(static_cast<uint16_t>(packet.get_payload_type()), packet.get_payload_buffer(),
               packet.get_payload_length(), session::logging::ExiMessageDirection::FROM_EV);
}

static std::unique_ptr<message_20::Variant> make_variant_from_packet(const iso15118::io::SdpPacket& packet) {
    return std::make_unique<message_20::Variant>(
        packet.get_payload_type(), io::StreamInputView{packet.get_payload_buffer(), packet.get_payload_length()});
}

void raise_invalid_packet_state(const io::SdpPacket& sdp_packet) {
    using PacketState = io::SdpPacket::State;

    auto error = std::string("Error while reading sdp packet: ");
    switch (sdp_packet.get_state()) {
    case PacketState::INVALID_HEADER:
        error += "invalid sdp packet header";
        break;
    case PacketState::PAYLOAD_TO_LONG:
        error += "packet too large for buffer";
        break;
    default:
        assert(false);
    }

    log_and_throw(error.c_str());
}

// NOTE (aw): this function return true, if it would block to read a complete packet
//            if it returns false, the packet is complete
bool read_single_sdp_packet(io::IConnection& connection, io::SdpPacket& sdp_packet) {
    // NOTE (aw): not happy with this function
    //            main problem is, that it combines too much logic of the sdp packet and io related stuff
    using PacketState = io::SdpPacket::State;

    assert(sdp_packet.get_state() == PacketState::EMPTY || sdp_packet.get_state() == PacketState::HEADER_READ);

    const auto first_try =
        connection.read(sdp_packet.get_current_buffer_pos(), sdp_packet.get_remaining_bytes_to_read());

    sdp_packet.update_read_bytes(first_try.bytes_read);

    if (first_try.would_block) {
        // need more data for at least the header
        return true;
    }

    if (sdp_packet.get_state() == PacketState::COMPLETE) {
        // done
        return false;
    }

    // packet not finished
    if (sdp_packet.get_state() != PacketState::HEADER_READ) {
        raise_invalid_packet_state(sdp_packet);
    }

    // header read successfully, try to read the rest
    const auto second_try =
        connection.read(sdp_packet.get_current_buffer_pos(), sdp_packet.get_remaining_bytes_to_read());

    sdp_packet.update_read_bytes(second_try.bytes_read);

    if (second_try.would_block) {
        // need more data for the rest of the packet!
        return true;
    }

    // assert finished packet
    if (sdp_packet.get_state() != PacketState::COMPLETE) {
        raise_invalid_packet_state(sdp_packet);
    }

    return false;
}

static size_t setup_response_header(uint8_t* buffer, iso15118::io::v2gtp::PayloadType payload_type, size_t size) {
    buffer[0] = iso15118::io::SDP_PROTOCOL_VERSION;
    buffer[1] = iso15118::io::SDP_INVERSE_PROTOCOL_VERSION;

    const uint16_t response_payload_type =
        htobe16(static_cast<std::underlying_type_t<iso15118::io::v2gtp::PayloadType>>(payload_type));

    std::memcpy(buffer + 2, &response_payload_type, sizeof(response_payload_type));

    const uint32_t tmp32 = htobe32(size);

    std::memcpy(buffer + 4, &tmp32, sizeof(tmp32));

    return size + iso15118::io::SdpPacket::V2GTP_HEADER_SIZE;
}

Session::Session(std::unique_ptr<io::IConnection> connection_, d20::SessionConfig session_config,
                 const session::feedback::Callbacks& callbacks, std::optional<d20::PauseContext>& pause_ctx) :
    connection(std::move(connection_)),
    log(this),
    ctx(callbacks, log, std::move(session_config), pause_ctx, active_control_event, message_exchange, timeouts),
    fsm(ctx.create_state<d20::state::SupportedAppProtocol>()) {

    next_session_event = offset_time_point_by_ms(get_current_time_point(), SESSION_IDLE_TIMEOUT_MS);
    connection->set_event_callback([this](io::ConnectionEvent event) { this->handle_connection_event(event); });
}

Session::~Session() = default;

void Session::push_control_event(const d20::ControlEvent& event) {
    control_event_queue.push(event);
}

TimePoint const& Session::poll() {
    const auto now = get_current_time_point();

    if (not state.connected) {
        // nothing happened so far, just return
        next_session_event = offset_time_point_by_ms(now, SESSION_IDLE_TIMEOUT_MS);
        return next_session_event;
    }

    // check for new data to read
    if (state.new_data) {
        const bool would_block = read_single_sdp_packet(*connection, packet);

        if (would_block) {
            state.new_data = false;
        }
    }

    // send all of our queued control events
    while ((active_control_event = control_event_queue.pop()) != std::nullopt) {

        if (const auto control_data = ctx.get_control_event<d20::DcTransferLimits>()) {
            ctx.session_config.dc_limits = *control_data;
        } else if (const auto control_data = ctx.get_control_event<d20::EnergyServices>()) {
            ctx.session_config.supported_energy_transfer_services = *control_data;
        } else if (const auto control_data = ctx.get_control_event<d20::SupportedVASs>()) {
            ctx.session_config.supported_vas_services = *control_data;
        } else if (const auto control_data = ctx.get_control_event<d20::AcTransferLimits>()) {
            ctx.session_config.ac_limits = *control_data;
        } else if (const auto control_data = ctx.get_control_event<d20::UpdateDynamicModeParameters>()) {
            ctx.cache_dynamic_mode_parameters.emplace(*control_data);
        } else if (const auto control_data = ctx.get_control_event<d20::AcTargetPower>()) {
            ctx.cache_ac_target_power.emplace(*control_data);
        } else if (const auto control_data = ctx.get_control_event<d20::AcPresentPower>()) {
            ctx.cache_ac_present_power.emplace(*control_data);
        }
        // Save some control events. It can happen that these events are sent before the corresponding state. They are
        // stored temporarily here.
        // TODO(sl): Construct ControlEventCache Struct

        [[maybe_unused]] const auto res = fsm.feed(d20::Event::CONTROL_MESSAGE);
        // FIXME (aw): check result!
    }

    const auto timeouts_reached = timeouts.check();

    if (timeouts_reached.has_value()) {
        const auto& reached = timeouts_reached.value();

        for (const auto& timeout : reached) {
            if (timeout == d20::TimeoutType::SEQUENCE) {
                logf_error("Sequence Timeout 40secs is reached. Stopping the session");
                ctx.session_stopped = true;
                break;
            } else {
                ctx.set_active_timeout(timeout);

                [[maybe_unused]] const auto res = fsm.feed(d20::Event::TIMEOUT);
                timeouts.reset_timeout(timeout);
            }
        }
    }

    // check for complete sdp packet
    if (packet.is_complete()) {
        // FIXME (aw): this event loop only acts on new packets, seems to be enough for now ...
        log_packet_from_car(packet, log);

        message_exchange.set_request(make_variant_from_packet(packet));

        packet = {}; // reset the packet

        const auto request_msg_type = ctx.peek_request_type();

        // There is no sequence timer before SupportedAppProtocol
        if (request_msg_type != message_20::Type::SupportedAppProtocolReq) {
            timeouts.stop_timeout(d20::TimeoutType::SEQUENCE);
        }

        ctx.feedback.v2g_message(request_msg_type);

        [[maybe_unused]] const auto res = fsm.feed(d20::Event::V2GTP_MESSAGE);
        // FIXME(sl): check result!
    }

    const auto [got_response, payload_size, payload_type, response_type] = message_exchange.check_and_clear_response();

    if (got_response) {
        const auto response_size = setup_response_header(response_buffer, payload_type, payload_size);
        connection->write(response_buffer, response_size);

        timeouts.start_timeout(d20::TimeoutType::SEQUENCE, d20::TIMEOUT_SEQUENCE);

        // FIXME (aw): this is hacky ...
        log.exi(static_cast<uint16_t>(payload_type), response_buffer + io::SdpPacket::V2GTP_HEADER_SIZE, payload_size,
                session::logging::ExiMessageDirection::TO_EV);

        ctx.feedback.v2g_message(response_type);
    }

    if (ctx.session_stopped or ctx.session_paused) {
        // TODO(SL): Does this also apply when a timeout is triggered? Or should the TCP/TLS connection be terminated
        // directly?
        // Wait for 5 seconds [V2G20-1643]
        std::this_thread::sleep_for(std::chrono::seconds(5));
        connection->close();

        const auto signal =
            (ctx.session_paused) ? session::feedback::Signal::DLINK_PAUSE : session::feedback::Signal::DLINK_TERMINATE;
        ctx.feedback.signal(signal);
    }

    // FIXME (aw): proper timeout handling!
    next_session_event = offset_time_point_by_ms(now, SESSION_IDLE_TIMEOUT_MS);
    return next_session_event;
}

void Session::handle_connection_event(io::ConnectionEvent event) {
    using Event = io::ConnectionEvent;
    switch (event) {
    case Event::ACCEPTED:
        assert(state.connected == false);
        state.connected = true;
        log("Accepted connection on port %d", connection->get_public_endpoint().port);
        return;

    case Event::NEW_DATA:
        assert(state.connected);
        state.new_data = true;
        return;

    case Event::OPEN:
        assert(state.connected);
        if (const auto new_vehicle_cert_hash = connection->get_vehicle_cert_hash()) {
            logf_info("Vehicle Cert is available");
            ctx.set_new_vehicle_cert_hash(new_vehicle_cert_hash);
        }
        // NOTE (aw): for now, we don't really need this information ...
        return;

    case Event::CLOSED:
        state.connected = false;
        logf_info("Connection is closed");
        return;
    }
}

void Session::close() {
    connection->close();
    ctx.feedback.signal(session::feedback::Signal::DLINK_TERMINATE);
    ctx.session_stopped = true;
}

} // namespace iso15118
