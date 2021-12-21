// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_CENTRAL_SYSTEM_HPP
#define OCPP_CENTRAL_SYSTEM_HPP

#include <boost/asio/signal_set.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <iostream>
#include <map>
#include <set>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ocpp1_6/schemas.hpp>
#include <ocpp1_6/types.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

typedef server::message_ptr message_ptr;
typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connection_list;
typedef std::map<websocketpp::connection_hdl, ocpp1_6::ChargePointConnection,
                 std::owner_less<websocketpp::connection_hdl>>
    connection_map;
typedef std::map<std::string, websocketpp::connection_hdl> connection_map_rev;

class CentralSystem {
private:
    server srv;
    connection_list connections;
    connection_map connections_map;
    connection_map_rev connections_map_rev;
    std::map<std::string, json> messages;

    boost::uuids::random_generator uuid_generator;
    ocpp1_6::MessageId getMessageId();
    std::string getChargePointId(server::connection_ptr con);
    ocpp1_6::ChargePointRequest getChargePointRequest(server::connection_ptr con);

public:
    CentralSystem(const std::string& address, const std::string& port, ocpp1_6::Schemas schemas);

    bool on_validate(server* s, websocketpp::connection_hdl hdl);
    void on_open(server* s, websocketpp::connection_hdl hdl);
    void on_close(server* s, websocketpp::connection_hdl hdl);
    void on_http(server* s, websocketpp::connection_hdl hdl);
    void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg);
    void stop();
};
#endif // OCPP_CENTRAL_SYSTEM_HPP
