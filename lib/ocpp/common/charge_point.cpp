// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/charge_point.hpp>

namespace ocpp {
ChargePoint::ChargePoint() : connection_state(ChargePointConnectionState::Disconnected) {
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

} // namespace ocpp
