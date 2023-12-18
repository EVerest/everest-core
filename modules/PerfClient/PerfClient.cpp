// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PerfClient.hpp"

namespace module {

void PerfClient::init() {
    invoke_init(*p_main);

    r_server->subscribe_simple_var(
        [this](types::performance_testing::PerformanceTracing trace) { tracing_object = trace; });

    mqtt.subscribe("everest_external/performance_testing/timestamp",
                   [this](std::string data) { external_mqtt_object = data; });
}

void PerfClient::ready() {
    invoke_ready(*p_main);

    while (true) {
        if (config.test_commands) {
            test_commands();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (config.test_vars) {
            test_vars();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (config.test_external_mqtt) {
            test_external_mqtt();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        EVLOG_info << "------------------";
    }
}

void PerfClient::test_external_mqtt() {
    std::chrono::milliseconds total_time_to_client{0};
    std::chrono::milliseconds max_time_to_client{0};
    std::chrono::milliseconds min_time_to_client{1000000};

    auto start_vars = std::chrono::steady_clock::now();

    const int number_of_vars_to_receive = config.number_of_commands_to_call;

    r_server->call_start_publishing_external_mqtt(number_of_vars_to_receive, 250);

    for (int i = 0; i < number_of_vars_to_receive; i++) {
        std::string trace;
        if (external_mqtt_object.wait_for(trace, std::chrono::milliseconds(500))) {
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(date::utc_clock::now() -
                                                                             Everest::Date::from_rfc3339(trace));
            total_time_to_client += dur;
            if (dur > max_time_to_client) {
                max_time_to_client = dur;
            }
            if (dur < min_time_to_client) {
                min_time_to_client = dur;
            }
        } else {
            EVLOG_error << "Timeout when waiting for external mqtt!";
        }
    }

    auto total_time_receiving_vars =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_vars);

    auto avg_time_to_client = total_time_to_client / number_of_vars_to_receive;

    EVLOG_info << fmt::format("EXTERNAL MQTT: {}/{}/{}ms server-client", avg_time_to_client.count(),
                              max_time_to_client.count(), min_time_to_client.count());
}

void PerfClient::test_vars() {
    std::chrono::milliseconds total_time_to_client{0};
    std::chrono::milliseconds max_time_to_client{0};
    std::chrono::milliseconds min_time_to_client{1000000};
    auto start_vars = std::chrono::steady_clock::now();

    const int number_of_vars_to_receive = config.number_of_commands_to_call;

    r_server->call_start_publishing_var(number_of_vars_to_receive, 250);

    for (int i = 0; i < number_of_vars_to_receive; i++) {
        types::performance_testing::PerformanceTracing trace;
        if (tracing_object.wait_for(trace, std::chrono::milliseconds(500))) {
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                date::utc_clock::now() - Everest::Date::from_rfc3339(trace.timestamps[0].timestamp));

            total_time_to_client += dur;
            if (dur > max_time_to_client) {
                max_time_to_client = dur;
            }
            if (dur < min_time_to_client) {
                min_time_to_client = dur;
            }
        } else {
            EVLOG_error << "Timeout when waiting for var!";
        }
    }

    auto total_time_receiving_vars =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_vars);

    auto avg_time_to_client = total_time_to_client / number_of_vars_to_receive;

    EVLOG_info << fmt::format("VARS         : {}/{}/{}ms server-client", avg_time_to_client.count(),
                              max_time_to_client.count(), min_time_to_client.count());
}

void PerfClient::test_commands() {
    std::chrono::milliseconds total_time_to_server{0};
    std::chrono::milliseconds total_time_to_client{0};

    std::chrono::milliseconds max_time_to_server{0};
    std::chrono::milliseconds max_time_to_client{0};

    std::chrono::milliseconds min_time_to_server{100000};
    std::chrono::milliseconds min_time_to_client{100000};
    auto start_cmds = std::chrono::steady_clock::now();

    const int number_of_commands_to_call = config.number_of_commands_to_call;

    for (int i = 0; i < number_of_commands_to_call; i++) {
        auto ret = send_simple_command();

        total_time_to_server += ret[0];
        total_time_to_client += ret[1];

        if (ret[0] > max_time_to_server) {
            max_time_to_server = ret[0];
        }
        if (ret[0] < min_time_to_server) {
            min_time_to_server = ret[0];
        }
        if (ret[1] > max_time_to_client) {
            max_time_to_client = ret[0];
        }
        if (ret[1] < min_time_to_client) {
            min_time_to_client = ret[0];
        }
    }

    auto total_time_running_commands =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_cmds);
    auto avg_time_to_server = total_time_to_server / number_of_commands_to_call;
    auto avg_time_to_client = total_time_to_client / number_of_commands_to_call;

    EVLOG_info << fmt::format("CMDS         : {}ms    ({}+{}/{}+{}/{}+{}) client-server-client",
                              avg_time_to_server.count() + avg_time_to_client.count(), avg_time_to_server.count(),
                              avg_time_to_client.count(), max_time_to_server.count(), min_time_to_server.count(),
                              max_time_to_client.count(), min_time_to_client.count());
}

static std::string to_str(types::performance_testing::PerformanceTracing trace) {
    std::string ret;
    for (auto t : trace.timestamps) {
        ret += t.timestamp + " " + t.name + "\n";
    }
    return ret;
}

std::vector<std::chrono::milliseconds> PerfClient::send_simple_command() {
    types::performance_testing::PerformanceTimestamp t;
    t.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    t.name = "Command sent";
    types::performance_testing::PerformanceTracing tr;
    tr.timestamps.push_back(t);
    auto ret = r_server->call_simple_command(tr);
    // EVLOG_info << "Reply received: " << to_str(ret);

    auto duration_to_server =
        std::chrono::duration_cast<std::chrono::milliseconds>(Everest::Date::from_rfc3339(ret.timestamps[1].timestamp) -
                                                              Everest::Date::from_rfc3339(ret.timestamps[0].timestamp));
    auto duration_back_to_client = std::chrono::duration_cast<std::chrono::milliseconds>(
        date::utc_clock::now() - Everest::Date::from_rfc3339(ret.timestamps[1].timestamp));
    return {duration_to_server, duration_back_to_client};
}

} // namespace module
