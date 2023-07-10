// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PacketSniffer.hpp"

#include <fmt/core.h>

namespace module {

const bool PROMISC_MODE = true;
const int PACKET_BUFFER_TIMEOUT_MS = 1000;
const int ALL_PACKETS_PROCESSED = -1;
const int WAIT_FOR_MS = 1;
const int BUFFERSIZE = 8192;

void PacketSniffer::init() {
    invoke_init(*p_main);

    capturing_stopped = false;
    already_started = false;

    p_handle = pcap_open_live(config.device.c_str(), BUFFERSIZE, PROMISC_MODE, PACKET_BUFFER_TIMEOUT_MS, errbuf);
    if (p_handle == nullptr) {
        EVLOG_AND_THROW(Everest::EverestConfigError(fmt::format("Could not open device {}", config.device)));
        return;
    }

    if (pcap_datalink(p_handle) != DLT_EN10MB) {
        EVLOG_AND_THROW(Everest::EverestConfigError(
            fmt::format("Device {} doesn't provide Ethernet headers - not supported", config.device)));
        return;
    }

    r_evse_manager->subscribe_session_event([this](types::evse_manager::SessionEvent session_event) {
        std::lock_guard<std::mutex> lock(this->capture_mutex);
        if (session_event.event == types::evse_manager::SessionEventEnum::SessionStarted) {
            if (!already_started) {
                already_started = true;
                capturing_stopped = false;
                if (session_event.session_started &&
                    session_event.session_started->logging_path) {
                    std::thread(&PacketSniffer::capture, this, session_event.session_started->logging_path.value(),
                                session_event.uuid)
                        .detach();
                }
            } else {
                EVLOG_warning << fmt::format("Capturing already started. Ignoring this SessionStarted event");
            }
        } else if (session_event.event == types::evse_manager::SessionEventEnum::SessionFinished) {
            capturing_stopped = true;
            already_started = false;
        }
    });
}

void PacketSniffer::ready() {
    invoke_ready(*p_main);
}

void PacketSniffer::capture(const std::string& logpath, const std::string& session_id) {

    const std::string fn = fmt::format("{}/ethernet-traffic.dump", logpath);

    if ((pdumpfile = pcap_dump_open(p_handle, fn.c_str())) == nullptr) {
        EVLOG_error << fmt::format("Error opening savefile {} for writing: {}", fn, pcap_geterr(p_handle));
        return;
    }

    while (!capturing_stopped) {
        if (pcap_dispatch(p_handle, ALL_PACKETS_PROCESSED, &pcap_dump, (u_char*)pdumpfile) <= PCAP_ERROR) {
            EVLOG_error << fmt::format("Error reading packets from interface: {}, error: {}", config.device,
                                       pcap_geterr(p_handle));
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_FOR_MS));
    }

    pcap_dump_close(pdumpfile);
}

} // namespace module
