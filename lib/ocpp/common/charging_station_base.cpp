// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/make_shared.hpp>
#include <ocpp/common/charging_station_base.hpp>

namespace ocpp {
ChargingStationBase::ChargingStationBase(const std::shared_ptr<EvseSecurity> evse_security,
                                         const std::optional<SecurityConfiguration> security_configuration) :
    uuid_generator(boost::uuids::random_generator()) {

    if (evse_security != nullptr) {
        this->evse_security = evse_security;
    } else {
        if (!security_configuration.has_value()) {
            throw std::runtime_error("No implementation of EvseSecurity interface and no SecurityConfiguration "
                                     "provided to chargepoint constructor. One of options must be set");
        }
        this->evse_security = std::make_shared<EvseSecurityImpl>(security_configuration.value());
    }
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

std::string ChargingStationBase::uuid() {
    std::stringstream s;
    s << this->uuid_generator();
    return s.str();
}

} // namespace ocpp
