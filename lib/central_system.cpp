// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <thread>

#include <ocpp1_6/BootNotification.hpp>
#include <ocpp1_6/GetConfiguration.hpp>

#include <central_system.hpp>

#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/iter_find.hpp>

CentralSystem::CentralSystem(const std::string& address, const std::string& port, ocpp1_6::Schemas schemas) :
    uuid_generator(boost::uuids::random_generator()) {

    try {
        srv.set_access_channels(websocketpp::log::alevel::none);
        srv.clear_access_channels(websocketpp::log::alevel::frame_payload);

        srv.init_asio();

        srv.set_validate_handler(bind(&CentralSystem::on_validate, this, &srv, ::_1));
        srv.set_open_handler(bind(&CentralSystem::on_open, this, &srv, ::_1));
        srv.set_close_handler(bind(&CentralSystem::on_close, this, &srv, ::_1));
        srv.set_http_handler(bind(&CentralSystem::on_http, this, &srv, ::_1));
        srv.set_message_handler(bind(&CentralSystem::on_message, this, &srv, ::_1, ::_2));

        srv.listen(address, port);

        srv.start_accept();

        boost::asio::signal_set signals(srv.get_io_service(), SIGINT, SIGTERM);
        signals.async_wait([&](const boost::system::error_code& /*error*/, int /*signal_number*/) { this->stop(); });
        srv.run();
    } catch (websocketpp::exception const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

std::string CentralSystem::getChargePointId(server::connection_ptr con) {
    auto url = con.get()->get_uri().get()->str();
    std::vector<std::string> components;
    iter_split(components, url, boost::algorithm::first_finder("/ocpp1.6/"));
    auto charge_point_id = components.back();
    return charge_point_id;
}

ocpp1_6::ChargePointRequest CentralSystem::getChargePointRequest(server::connection_ptr con) {
    auto url = con.get()->get_uri().get()->str();
    std::vector<std::string> components;
    iter_split(components, url, boost::algorithm::first_finder("/ocpp1.6/"));
    auto charge_point_id_and_request = components.back();
    std::vector<std::string> id_and_request_components;
    iter_split(id_and_request_components, charge_point_id_and_request, boost::algorithm::first_finder("/"));
    ocpp1_6::ChargePointRequest charge_point_request;
    charge_point_request.charge_point_id = id_and_request_components.front();
    charge_point_request.request = id_and_request_components.back();
    return charge_point_request;
}

ocpp1_6::MessageId CentralSystem::getMessageId() {
    std::stringstream s;
    s << this->uuid_generator();
    return ocpp1_6::MessageId(s.str());
}

void CentralSystem::stop() {
    std::cout << "Closing" << std::endl;
    this->srv.stop_listening();
    for (auto connection : this->connections) {
        this->srv.close(connection, websocketpp::close::status::normal, "");
    }
}

bool CentralSystem::on_validate(server* s, websocketpp::connection_hdl hdl) {
    auto con = s->get_con_from_hdl(hdl);
    std::cout << "validating... requested subprotocols:" << std::endl;
    auto subprotocols = con.get()->get_requested_subprotocols();
    bool ocpp1_6 = false;
    for (auto subprotocol : subprotocols) {
        std::cout << "  " << subprotocol << std::endl;
        if (subprotocol == "ocpp1.6") {
            ocpp1_6 = true;
        }
    }
    if (ocpp1_6) {
        con.get()->select_subprotocol("ocpp1.6");
    }

    // TODO(kai): here? we should also add a check to see if this charge point is allowed to connect

    return ocpp1_6;
}

void CentralSystem::on_open(server* s, websocketpp::connection_hdl hdl) {
    auto con = s->get_con_from_hdl(hdl);
    auto charge_point_id = getChargePointId(con);
    std::cout << "ON OPEN: "
              << "Charge point id: " << charge_point_id << std::endl
              << "subprotocol: " << con.get()->get_subprotocol() << std::endl;
    connections.insert(hdl);
    ocpp1_6::ChargePointConnection c;
    c.charge_point_id = charge_point_id;
    c.state = ocpp1_6::ChargePointConnectionState::Connected;
    connections_map[hdl] = c;
    connections_map_rev[charge_point_id] = hdl;
}

void CentralSystem::on_close(server* s, websocketpp::connection_hdl hdl) {
    auto charge_point_id = getChargePointId(s->get_con_from_hdl(hdl));

    std::cout << "ON Close: "
              << "Charge point id: " << charge_point_id << std::endl;
    connections.erase(hdl);
    connections_map[hdl].state = ocpp1_6::ChargePointConnectionState::Disconnected;
}

void CentralSystem::on_http(server* s, websocketpp::connection_hdl hdl) {
    server::connection_ptr con = s->get_con_from_hdl(hdl);

    auto req_type = this->getChargePointRequest(con);

    if (req_type.request == "GetConfiguration") {
        if (connections_map_rev.count(req_type.charge_point_id)) {
            std::cout << "found a connection to the charge point id!" << std::endl;
            server::connection_ptr charge_point_con =
                s->get_con_from_hdl(connections_map_rev[req_type.charge_point_id]);

            ocpp1_6::GetConfigurationRequest req;
            std::vector<ocpp1_6::CiString50Type> keys;
            keys.push_back(ocpp1_6::CiString50Type("HeartbeatInterval"));

            req.key.emplace(keys);

            ocpp1_6::Call call(req, this->getMessageId());

            this->messages[call.uniqueId] = json(call);

            charge_point_con->send(this->messages[call.uniqueId].dump(), websocketpp::frame::opcode::TEXT);
        }
    }

    std::string res = con->get_request_body();

    std::stringstream ss;
    ss << "got HTTP request with " << res.size() << " bytes of body data. id: " << req_type.charge_point_id
       << " request: " << req_type.request;

    con->set_body(ss.str());
    con->set_status(websocketpp::http::status_code::ok);
}

void CentralSystem::on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get() << " and message: " << msg->get_payload()
              << std::endl;

    auto message = msg->get_payload();

    try {
        try {
            json::array_t json_message = json::parse(message);
            std::cout << "json message: " << json_message << std::endl;
            if (json_message.at(2) == "BootNotification") {
                ocpp1_6::Call<ocpp1_6::BootNotificationRequest> call = json(json_message);

                std::cout << "Received BootNotification: " << call.msg << " sending response" << std::endl;
                // TODO: ocpp state machine and the rest of the messages!

                ocpp1_6::BootNotificationResponse res;
                res.currentTime = ocpp1_6::DateTime();
                res.interval = 300; // TODO
                res.status = ocpp1_6::RegistrationStatus::Accepted;

                ocpp1_6::CallResult call_result(res, call.uniqueId);
                s->send(hdl, json(call_result).dump(), websocketpp::frame::opcode::TEXT);
            }
        } catch (const std::exception& e) {
            std::cout << "json parse failed because: "
                      << "(" << e.what() << ")" << std::endl;
        }

    } catch (websocketpp::exception const& e) {
        std::cout << "failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}
