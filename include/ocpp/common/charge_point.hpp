// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_CHARGE_POINT_HPP
#define OCPP_COMMON_CHARGE_POINT_HPP

#include <everest/timer.hpp>

#include <ocpp/common/database_handler.hpp>
#include <ocpp/common/message_queue.hpp>
#include <ocpp/common/pki_handler.hpp>
#include <ocpp/common/websocket/websocket.hpp>

namespace ocpp {

/// \brief Common base class for OCPP1.6 and OCPP2.0.1 chargepoints
class ChargePoint {

protected:
    std::unique_ptr<Websocket> websocket;
    std::shared_ptr<PkiHandler> pki_handler;
    std::shared_ptr<MessageLogging> logging;
    std::shared_ptr<DatabaseHandler> database_handler;

    ChargePointConnectionState connection_state;

    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;

public:
    ChargePoint();
    virtual ~ChargePoint(){};
};

} // namespace ocpp

#endif // OCPP_COMMON
