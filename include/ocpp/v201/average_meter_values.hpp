// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>
#include <vector>
namespace ocpp {
namespace v201 {
class AverageMeterValues {

public:
    AverageMeterValues();
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

            return measurand < other.measurand or location < other.location or phase < other.phase;
        }
    };

    MeterValue averaged_meter_values;
    std::mutex avg_meter_value_mutex;
    std::map<MeterValueMeasurands, MeterValueCalc> aligned_meter_values;
    bool is_avg_meas(const SampledValue& sample);
    void average_meter_value();
};
} // namespace v201

} // namespace ocpp