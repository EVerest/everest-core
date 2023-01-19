// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/charging_station_base.hpp>

namespace ocpp {
ChargingStationBase::ChargingStationBase() {
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

} // namespace ocpp
