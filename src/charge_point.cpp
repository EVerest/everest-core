// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
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

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <everest/logging.hpp>

#include <ocpp1_6/types.hpp>

#include <ocpp1_6/charge_point.hpp>

#include <ocpp1_6/database_handler.hpp>

namespace po = boost::program_options;

ocpp1_6::ChargePoint* charge_point;
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

    // initialize logging as early as possible
    std::string logging_config = maindir + "/logging.ini";
    if (vm.count("logconf") != 0) {
        logging_config = vm["logconf"].as<std::string>();
    }
    Everest::Logging::init(logging_config, "charge_point");

    std::string conf = "config.json";
    if (vm.count("conf") != 0) {
        conf = vm["conf"].as<std::string>();
    }

    const auto configs_path = maindir;
    const auto database_path = "/tmp/ocpp";

    boost::filesystem::path config_path = boost::filesystem::path(maindir) / conf;
    std::ifstream ifs(config_path.c_str());
    std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    auto json_config = json::parse(config_file);
    json_config["Internal"]["LogMessagesFormat"][0] = "console_detailed";
    auto user_config_path = boost::filesystem::path("/tmp") / "user_config.json";

    if (boost::filesystem::exists(user_config_path)) {
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

    std::shared_ptr<ocpp1_6::ChargePointConfiguration> configuration =
        std::make_shared<ocpp1_6::ChargePointConfiguration>(json_config, configs_path, user_config_path.string());

    const boost::filesystem::path sql_init_path = boost::filesystem::path(configs_path) / "init.sql";
    charge_point = new ocpp1_6::ChargePoint(configuration, database_path, sql_init_path.string(), "/tmp");

    /************************************** START REGISTERING CALLBACKS /**************************************/

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
        [](const std::string& id_token, const std::vector<int32_t> &referenced_connectors, bool prevalidated) {
            std::cout << "Callback: "
                       << "Received token " << id_token << " for further validation" << std::endl;
        });

    charge_point->register_stop_transaction_callback([](int32_t connector, ocpp1_6::Reason reason) {
        std::cout << "Callback: "
                   << "Stopping transaction with  at connector#" << connector
                   << " with reason: " << ocpp1_6::conversions::reason_to_string(reason);
        return true;
    });

    charge_point->register_reserve_now_callback([](int32_t reservation_id, int32_t connector,
                                                   ocpp1_6::DateTime expiryDate, ocpp1_6::CiString20Type idTag,
                                                   boost::optional<ocpp1_6::CiString20Type> parent_id) {
        std::cout << "Callback: "
                   << "Reserving connector# " << connector << " for id token: " << idTag.get() << " until "
                   << expiryDate;
        return ocpp1_6::ReservationStatus::Accepted;
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

    charge_point->register_set_max_current_callback([](int32_t connector, double max_current) {
        std::cout << "Callback: "
                   << "Setting maximum current for connector#" << connector << "to: " << max_current;
        return true;
    });

    charge_point->register_upload_diagnostics_callback([](const ocpp1_6::GetDiagnosticsRequest& request) {
        std::cout << "Callback: "
                   << "Uploading Diagnostics file" << std::endl;
        ocpp1_6::GetLogResponse r;
        r.status = ocpp1_6::LogStatusEnumType::Accepted;
        return r;
    });

    charge_point->register_update_firmware_callback([](const ocpp1_6::UpdateFirmwareRequest msg) {
        std::cout << "Callback: "
                   << "Updating Firmware" << std::endl;
    });

    charge_point->register_signed_update_firmware_callback([](ocpp1_6::SignedUpdateFirmwareRequest msg) {
        std::cout << "Callback: "
                   << "Updating Signed Firmware" << std::endl;
        return ocpp1_6::UpdateFirmwareStatusEnumType::Accepted;
    });

    charge_point->register_upload_logs_callback([](ocpp1_6::GetLogRequest req) {
        std::cout << "Callback: "
                   << "Uploading Logs" << std::endl;
        ocpp1_6::GetLogResponse r;
        r.status = ocpp1_6::LogStatusEnumType::Accepted;
        return r;
    });

    charge_point->register_set_connection_timeout_callback([](int32_t connection_timeout) {
        std::cout << "Callback: "
                   << "Setting connection timeout" << std::endl;
    });

    charge_point->register_reset_callback([](const ocpp1_6::ResetType& reset_type) {
        std::cout << "Callback: "
                   << "Exectuting: " << ocpp1_6::conversions::reset_type_to_string(reset_type) << " reset" << std::endl;
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

    /************************************** STOP REGISTERING CALLBACKS /**************************************/

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

            if (command == "auth") {
                auto result = charge_point->authorize_id_token(ocpp1_6::CiString20Type(std::string("DEADBEEF")));
            } else if (command == "start_transaction") {
                if (!transaction_running) {
                    charge_point->on_session_started(1, "s355i0n-1d", "EVConnected");
                    const auto result =
                        charge_point->authorize_id_token(ocpp1_6::CiString20Type(std::string("DEADBEEF")));
                    if (result.status == ocpp1_6::AuthorizationStatus::Accepted) {
                        charge_point->on_transaction_started(1, "s355i0n-1d", "DEADBEEF", 0, boost::none,
                                                             ocpp1_6::DateTime(), boost::none);
                        charge_point->on_resume_charging(1);
                        transaction_running = true;
                    }
                }

            } else if (command == "stop_transaction") {
                if (transaction_running) {
                    charge_point->on_transaction_stopped(1, "s355i0n-1d", ocpp1_6::Reason::Local, ocpp1_6::DateTime(),
                                                         2500, boost::none, boost::none);
                    charge_point->on_session_stopped(1);
                    transaction_running = false;
                } else {
                    std::cout << "No transaction is currently running. Use command \"start_transaction\" first" << std::endl;
                }
            } else if (command == "end") {
                running = false;
            } else if (command == "help") {
                std::cout << "You can run the following commands:\n"
                           << "\t- start_transaction : Starts a transaction with token \"DEADBEEF\"\n"
                           << "\t- stop_transaction : Stops a transaction if currently running\n"
                           << "\t- auth : Authorizes the token \"DEADBEEF\"\n"
                           << "\t- end : Stop execution of the program\n" << std::endl;
            } else {
                std::cout << "Received unknown command" << std::endl;
            }
        }
    });

    t1.join();

    const auto db_file_name = "/tmp/ocpp/" + json_config.at("Internal").at("ChargePointId").get<std::string>() + ".db";
    std::filesystem::remove(db_file_name);

    std::cout << "Goodbye" << std::endl;

    return 0;
}
