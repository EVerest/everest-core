// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <date/date.h>
#include <date/tz.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sys/prctl.h>
#include <thread>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>

#include <ocpp/v16/charge_point.hpp>
#include <ocpp/v16/database_handler.hpp>

#include <ocpp/common/cistring.hpp>
#include <ocpp/common/string.hpp>
#include <ocpp/common/support_older_cpp_versions.hpp>

namespace po = boost::program_options;

ocpp::v16::ChargePoint* charge_point;
bool running = false;
std::mutex m;

int main(int argc, char* argv[]) {
    po::options_description desc("OCPP charge point");

    desc.add_options()("help,h", "produce help message");
    desc.add_options()("maindir", po::value<std::string>(), "set main dir in which the schemas folder resides");
    desc.add_options()("conf", po::value<std::string>(), "charge point config relative to maindir");
    desc.add_options()("logconf", po::value<std::string>(), "The path to a custom logging.ini");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") != 0) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string maindir = ".";
    if (vm.count("maindir") != 0) {
        maindir = vm["maindir"].as<std::string>();
    }

    const auto database_path = "/tmp/ocpp";
    const auto share_path = fs::path(maindir) / "share" / "everest" / "modules" / "OCPP";

    // initialize logging as early as possible
    auto logging_config = share_path / "logging.ini";
    if (vm.count("logconf") != 0) {
        logging_config = fs::path(vm["logconf"].as<std::string>());
    }
    Everest::Logging::init(logging_config.string(), "charge_point");

    std::string conf = "config.json";
    if (vm.count("conf") != 0) {
        conf = vm["conf"].as<std::string>();
    }

    fs::path config_path = share_path / conf;
    if (!fs::exists(config_path)) {
        EVLOG_error << "Could not find config at: " << config_path;
        return 1;
    }
    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    auto json_config = json::parse(config_file);
    json_config["Internal"]["LogMessagesFormat"][0] = "console_detailed";
    auto user_config_path = fs::path("/tmp") / "user_config.json";

    if (fs::exists(user_config_path)) {
        std::ifstream ifs(user_config_path.c_str());
        std::string user_config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        auto user_config = json::parse(user_config_file);
        json_config.merge_patch(user_config);
    } else {
        std::ofstream fs(user_config_path.c_str());
        auto user_config = json::object();
        fs << user_config << std::endl;
        fs.close();
    }

    const fs::path sql_init_path = share_path / "init.sql";

    // create the cso_path
    const fs::path cso_path = "/tmp/client/cso";
    if (!fs::exists(cso_path)) {
        fs::create_directories(cso_path);
    }

    const fs::path csms_path = "/tmp/client/csms";
    if (!fs::exists(csms_path)) {
        fs::create_directories(csms_path);
    }

    const fs::path ca_v2g_path = "/tmp/certs/ca/v2g/";
    if (!fs::exists(ca_v2g_path)) {
        fs::create_directories(ca_v2g_path);
    }

    const fs::path ca_mo_path = "/tmp/certs/ca/mo/";
    if (!fs::exists(ca_mo_path)) {
        fs::create_directories(ca_mo_path);
    }

    if (!fs::is_regular_file("/tmp/certs/ca/v2g/V2G_CA_BUNDLE.pem")) {
        fsstd::ofstream("/tmp/certs/ca/v2g/V2G_CA_BUNDLE.pem");
    }

    if (!fs::is_regular_file("/tmp/certs/ca/mo/MO_CA_BUNDLE.pem")) {
        fsstd::ofstream("/tmp/certs/ca/mo//MO_CA_BUNDLE.pem");
    }

    ocpp::SecurityConfiguration secConfig;
    secConfig.csms_ca_bundle = fs::path("/tmp/certs/ca/v2g/V2G_CA_BUNDLE.pem");
    secConfig.mf_ca_bundle = fs::path("/tmp/certs/ca/v2g/V2G_CA_BUNDLE.pem");
    secConfig.v2g_ca_bundle = fs::path("/tmp/certs/ca/v2g/V2G_CA_BUNDLE.pem");
    secConfig.mo_ca_bundle = fs::path("/tmp/certs/ca/mo/MO_CA_BUNDLE.pem");

    secConfig.csms_leaf_cert_directory = fs::path("/tmp/client/csms/");
    secConfig.csms_leaf_key_directory = fs::path("/tmp/client/csms/");
    secConfig.secc_leaf_cert_directory = fs::path("/tmp/client/cso/");
    secConfig.secc_leaf_key_directory = fs::path("/tmp/client/cso/");

    charge_point = new ocpp::v16::ChargePoint(json_config.dump(), share_path, user_config_path, database_path,
                                              sql_init_path, fs::path("/tmp"), nullptr, secConfig);

    /************************************** START REGISTERING CALLBACKS **************************************/

    charge_point->register_enable_evse_callback([](int32_t connector) {
        std::cout << "Callback: "
                  << "Enabling chargepoint at connector#" << connector;
        return true;
    });

    charge_point->register_disable_evse_callback([](int32_t connector) {
        std::cout << "Callback: "
                  << "Disabling chargepoint at connector#" << connector;
        return true;
    });

    charge_point->register_pause_charging_callback([](int32_t connector) {
        std::cout << "Callback: "
                  << "Pausing charging at connector#" << connector;
        return true;
    });

    charge_point->register_resume_charging_callback([](int32_t connector) {
        std::cout << "Callback: "
                  << "Resuming charging at connector#" << connector;
        return true;
    });

    charge_point->register_provide_token_callback(
        [](const std::string& id_token, const std::vector<int32_t>& referenced_connectors, bool prevalidated) {
            std::cout << "Callback: "
                      << "Received token " << id_token << " for further validation" << std::endl;
        });

    charge_point->register_stop_transaction_callback([](int32_t connector, ocpp::v16::Reason reason) {
        std::cout << "Callback: "
                  << "Stopping transaction with  at connector#" << connector
                  << " with reason: " << ocpp::v16::conversions::reason_to_string(reason);
        return true;
    });

    charge_point->register_reserve_now_callback([](int32_t reservation_id, int32_t connector, ocpp::DateTime expiryDate,
                                                   ocpp::CiString<20> idTag,
                                                   std::optional<ocpp::CiString<20>> parent_id) {
        std::cout << "Callback: "
                  << "Reserving connector# " << connector << " for id token: " << idTag.get() << " until "
                  << expiryDate;
        return ocpp::v16::ReservationStatus::Accepted;
    });

    charge_point->register_cancel_reservation_callback([](int32_t reservation_id) {
        std::cout << "Callback: "
                  << "Cancelling reservation with id: " << reservation_id;
        return true;
    });

    charge_point->register_unlock_connector_callback([](int32_t connector) {
        std::cout << "Callback: "
                  << "Unlock connector#" << connector;
        return true;
    });

    charge_point->register_upload_diagnostics_callback([](const ocpp::v16::GetDiagnosticsRequest& request) {
        std::cout << "Callback: "
                  << "Uploading Diagnostics file" << std::endl;
        ocpp::v16::GetLogResponse r;
        r.status = ocpp::v16::LogStatusEnumType::Accepted;
        return r;
    });

    charge_point->register_update_firmware_callback([](const ocpp::v16::UpdateFirmwareRequest msg) {
        std::cout << "Callback: "
                  << "Updating Firmware" << std::endl;
    });

    charge_point->register_signed_update_firmware_callback([](ocpp::v16::SignedUpdateFirmwareRequest msg) {
        std::cout << "Callback: "
                  << "Updating Signed Firmware" << std::endl;
        return ocpp::v16::UpdateFirmwareStatusEnumType::Accepted;
    });

    charge_point->register_upload_logs_callback([](ocpp::v16::GetLogRequest req) {
        std::cout << "Callback: "
                  << "Uploading Logs" << std::endl;
        ocpp::v16::GetLogResponse r;
        r.status = ocpp::v16::LogStatusEnumType::Accepted;
        return r;
    });

    charge_point->register_set_connection_timeout_callback([](int32_t connection_timeout) {
        std::cout << "Callback: "
                  << "Setting connection timeout" << std::endl;
    });

    charge_point->register_reset_callback([](const ocpp::v16::ResetType& reset_type) {
        std::cout << "Callback: "
                  << "Exectuting: " << ocpp::v16::conversions::reset_type_to_string(reset_type) << " reset"
                  << std::endl;
        return true;
    });

    charge_point->register_set_system_time_callback([](const std::string& system_time) {
        std::cout << "Callback: "
                  << "Setting system time to " << system_time;
    });

    charge_point->register_signal_set_charging_profiles_callback([]() {
        std::cout << "Callback: "
                  << "Setting charging profiles" << std::endl;
    });

    charge_point->register_transaction_updated_callback([](const int32_t connector, const int32_t transaction_id,
                                                           const std::string& session_id,
                                                           const IdTagInfo& id_tag_info) {
        std::cout << "Callback: Transaction updated at connector# " << connector
                  << " and transaction id: " << transaction_id << std::endl;
    });

    /************************************** STOP REGISTERING CALLBACKS **************************************/

    charge_point->start();

    {
        std::lock_guard<std::mutex> lk(m);
        running = true;
    }

    std::thread t1([&] {
        int32_t value = 0;
        bool transaction_running = false;
        while (!std::cin.fail() && running) {
            std::string command;
            std::cin >> command;
            std::cout << std::endl;

            std::string uuid;

            if (command == "auth") {
                auto result = charge_point->authorize_id_token(ocpp::CiString<20>(std::string("DEADBEEF")));
            } else if (command == "start_transaction") {
                if (!transaction_running) {
                    uuid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
                    charge_point->on_session_started(1, uuid, ocpp::SessionStartedReason::EVConnected, std::nullopt);
                    const auto result = charge_point->authorize_id_token(ocpp::CiString<20>(std::string("DEADBEEF")));
                    if (result.status == ocpp::v16::AuthorizationStatus::Accepted) {
                        charge_point->on_transaction_started(1, uuid, "DEADBEEF", 0, std::nullopt, ocpp::DateTime(),
                                                             std::nullopt);
                        charge_point->on_resume_charging(1);
                        transaction_running = true;
                    }
                }

            } else if (command == "stop_transaction") {
                if (transaction_running) {
                    charge_point->on_transaction_stopped(1, uuid, ocpp::v16::Reason::Local, ocpp::DateTime(), 2500,
                                                         std::nullopt, std::nullopt);
                    charge_point->on_session_stopped(1, uuid);
                    transaction_running = false;
                } else {
                    std::cout << "No transaction is currently running. Use command \"start_transaction\" first"
                              << std::endl;
                }
            } else if (command == "end") {
                running = false;
            } else if (command == "help") {
                std::cout << "You can run the following commands:\n"
                          << "\t- start_transaction : Starts a transaction with token \"DEADBEEF\"\n"
                          << "\t- stop_transaction : Stops a transaction if currently running\n"
                          << "\t- auth : Authorizes the token \"DEADBEEF\"\n"
                          << "\t- end : Stop execution of the program\n"
                          << std::endl;
            } else {
                std::cout << "Received unknown command" << std::endl;
            }
        }
    });

    t1.join();

    std::cout << "Goodbye" << std::endl;

    return 0;
}
