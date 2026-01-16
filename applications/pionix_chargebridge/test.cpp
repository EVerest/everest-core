#include <chrono>
#include <csignal>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mdns/mdns.hpp>
#include <everest/io/mdns/mdns_client.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& s, everest::lib::io::mdns::mDNS_discovery const& obj) {
    s << obj.service_instance << "\n"
      << " Hostname: " << obj.hostname << "\n"
      << " IP:       " << obj.ip << "\n"
      << " Port:     " << obj.port << "\n"
      << " TXT: \n";
    for (auto const& [key, val] : obj.txt) {
        s << "   " << key << " = " << val << "\n";
    }
    return s;
}

std::atomic<bool> g_run_application(true);
void signal_handler(int signum) {
    std::cout << "\nSignal " << signum << " received. Initiating graceful shutdown." << std::endl;
    g_run_application = false;
}

everest::lib::io::mdns::mDNS_registry registry;

void set_rx_handler(everest::lib::io::mdns::mdns_client& client, everest::lib::io::event::fd_event_handler& handler) {
    handler.register_event_handler(&client);
    client.set_rx_handler([&](auto const& data, auto&) {
        auto discovery = everest::lib::io::mdns::parse_mdns_packet(data.buffer);
        if (discovery.has_value()) {
            if (registry.update(*discovery)) {
                std::cout << *discovery << std::endl;
            };
        }
    });
}

int main(int argc, char* argv[]) {
    using namespace std::chrono_literals;
    std::cout << "Starting ChargeBridge mDNS discovery" << std::endl;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGHUP, signal_handler);
    std::signal(SIGTERM, signal_handler);

    everest::lib::io::event::fd_event_handler handler;
    everest::lib::io::event::timer_fd timer;

    std::vector<std::unique_ptr<everest::lib::io::mdns::mdns_client>> clients;

    for (auto const& item : everest::lib::io::socket::get_all_interaces()) {
        clients.emplace_back(new everest::lib::io::mdns::mdns_client(item.name));
        set_rx_handler(*clients.back(), handler);
    }

    timer.set_timeout(1s);

    handler.register_event_handler(&timer, [&](auto) {
        for (auto& item : clients) {
            item->get_raw_handler()->query("_chargebridge._udp.local");
        }
    });

    handler.run(g_run_application);

    return 0;
}
