// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_CHARGE_POINT_HPP
#define OCPP_COMMON_CHARGE_POINT_HPP

#include <everest/timer.hpp>

#include <ocpp/common/message_queue.hpp>
#include <ocpp/common/pki_handler.hpp>
#include <ocpp/common/websocket/websocket.hpp>

namespace ocpp {

/// \brief Common base class for OCPP1.6 and OCPP2.0.1 charging stations
class ChargingStationBase {

protected:
    std::unique_ptr<Websocket> websocket;
    std::shared_ptr<PkiHandler> pki_handler;
    std::shared_ptr<MessageLogging> logging;

    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;

    boost::uuids::random_generator uuid_generator;

    /// \brief Identifies the next timestamp at which a clock aligned meter value should be send
    /// \param interval the configured AlignedDataInterval
    /// \return boost::optional<DateTime> If \param interval > 0 it returns the next timestamp at which a clock aligned
    /// meter value should be sent, else boost::none
    boost::optional<DateTime> get_next_clock_aligned_meter_value_timestamp(const int32_t interval);

    /// \brief Generates a uuid  
    /// \return uuid
    std::string uuid();


public:
    ChargingStationBase();
    virtual ~ChargingStationBase(){};
};

} // namespace ocpp

#endif // OCPP_COMMON
