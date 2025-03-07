#include <filesystem>
#include <iostream>
#include <string>
#include <unistd.h>

#include <iso15118/config.hpp>
#include <iso15118/io/connection_ssl.hpp>
#include <iso15118/io/logging.hpp>
#include <iso15118/io/poll_manager.hpp>
#include <iso15118/io/time.hpp>

namespace {

static constexpr auto DEFAULT_PW{"123456"};
static constexpr auto POLL_MANAGER_TIMEOUT_MS = 50;
static constexpr auto STOP_TIME = 30;

const char* short_opts = "hi:";
std::string interface {};

void parse_options(int argc, char** argv) {
    int c{0};

    while ((c = getopt(argc, argv, short_opts)) != -1) {
        switch (c) {
            break;
        case 'i':
            interface = std::string(optarg);
            break;
        case 'h':
        case '?':
            std::cout << "Usage: " << argv[0] << " [-i]" << std::endl;
            std::cout << "       -i <interface name>" << std::endl;
            exit(1);
            break;
        default:
            exit(2);
        }
    }

    if (interface.empty()) {
        std::cerr << "Error: " << argv[0] << " requires -i <interface name>" << std::endl;
        exit(3);
    }
}

void handle_connection_event(iso15118::io::ConnectionEvent event) {
    using namespace iso15118;

    using Event = io::ConnectionEvent;
    switch (event) {
    case Event::ACCEPTED:
        std::cout << "Accepted connection" << std::endl;
        return;

    case Event::NEW_DATA:
        std::cout << "New Data" << std::endl;
        return;

    case Event::OPEN:
        std::cout << "Connection open" << std::endl;
        return;

    case Event::CLOSED:
        std::cout << "Connection is closed" << std::endl;
        return;
    }
}

} // namespace

int main(int argc, char** argv) {
    using namespace iso15118;

    parse_options(argc, argv);

    io::set_logging_callback([](LogLevel level, const std::string& message) {
        std::cout << "log(" << static_cast<int>(level) << "): " << message << std::endl;
    });

    auto poll_manager = io::PollManager();

    const auto interface_name = interface;

    const config::SSLConfig ssl{iso15118::config::CertificateBackend::EVEREST_LAYOUT,
                                {},
                                "pki/certs/client/cso/CPO_CERT_CHAIN.pem",
                                "pki/certs/client/cso/SECC_LEAF.key",
                                DEFAULT_PW,
                                "pki/certs/ca/v2g/V2G_ROOT_CA.pem",
                                "pki/certs/ca/oem/OEM_ROOT_CA.pem",
                                false,   // enable_ssl_logging
                                true,    // enable_tls_key_logging
                                false,   // enforce_tls_1_3
                                "/tmp"}; // tls_key_log_file_path

    auto connection = io::ConnectionSSL(poll_manager, interface_name, ssl);
    connection.set_event_callback([](io::ConnectionEvent event) { handle_connection_event(event); });

    auto next_event = get_current_time_point();
    const auto start_time_point = next_event;

    while (true) {
        const auto duration =
            std::chrono::duration_cast<std::chrono::duration<double>>(get_current_time_point() - start_time_point);
        if (duration.count() >= STOP_TIME) {
            break;
        }

        const auto poll_timeout_ms = get_timeout_ms_until(next_event, POLL_MANAGER_TIMEOUT_MS);
        poll_manager.poll(poll_timeout_ms);

        next_event = offset_time_point_by_ms(get_current_time_point(), POLL_MANAGER_TIMEOUT_MS);
    }

    connection.close();

    return 0;
}
