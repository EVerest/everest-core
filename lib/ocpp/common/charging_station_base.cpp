// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/make_shared.hpp>
#include <ocpp/common/charging_station_base.hpp>

namespace ocpp {
ChargingStationBase::ChargingStationBase() : uuid_generator(boost::uuids::random_generator()) {
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

std::optional<DateTime> ChargingStationBase::get_next_clock_aligned_meter_value_timestamp(const int32_t interval) {
    if (interval == 0) {
        return std::nullopt;
    }
    const auto seconds_in_a_day = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)).count();
    const auto now = date::utc_clock::now();
    const auto midnight = date::floor<date::days>(now) + std::chrono::seconds(date::get_tzdb().leap_seconds.size());
    const auto diff = now - midnight;
    const auto start =
        std::chrono::duration_cast<std::chrono::seconds>(diff / interval) * interval + std::chrono::seconds(interval);
    const auto clock_aligned_meter_values_time_point = DateTime(midnight + start);
    EVLOG_debug << "Sending clock aligned meter values every " << interval << " seconds, starting at "
                << ocpp::DateTime(clock_aligned_meter_values_time_point) << ". This amounts to "
                << seconds_in_a_day / interval << " samples per day.";
    return clock_aligned_meter_values_time_point;
}

std::string ChargingStationBase::uuid() {
    std::stringstream s;
    s << this->uuid_generator();
    return s.str();
}

} // namespace ocpp
