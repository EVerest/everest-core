// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
#include <ocpp/v2/types.hpp>
#include <ocpp/v2/utils.hpp>
#include <vector>
namespace ocpp {
namespace v2 {
class AverageMeterValues {

public:
    AverageMeterValues() = default;
    /// @brief Set the meter values into the local object for processing
    /// @param meter_value MeterValue
    void set_values(const MeterValue& meter_value);
    /// @brief retrive the processed values
    /// @return MeterValue type
    MeterValue retrieve_processed_values();
    /// @brief Manually clear the local object meter values
    void clear_values();

private:
    struct MeterValueCalc {
        double sum;
        int num_elements;
    };
    struct MeterValueMeasurands {
        MeasurandEnum measurand;
        std::optional<PhaseEnum> phase;       // measurand may or may not have a phase field
        std::optional<LocationEnum> location; // measurand may or may not have location field

        // Define a comparison operator for the struct
        bool operator<(const MeterValueMeasurands& other) const {
            // Using tie here to compare the two lexicographically instead of writing it all out
            return std::tie(measurand, location, phase) < std::tie(other.measurand, other.location, other.phase);
        }
    };

    MeterValue averaged_meter_values;
    std::mutex avg_meter_value_mutex;
    std::map<MeterValueMeasurands, MeterValueCalc> aligned_meter_values;
    void average_meter_value();
};
} // namespace v2

} // namespace ocpp