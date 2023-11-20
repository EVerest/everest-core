// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <ocpp/v201/average_meter_values.hpp>

namespace ocpp {
namespace v201 {
AverageMeterValues::AverageMeterValues() {
}
void AverageMeterValues::clear_values() {
    std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    this->aligned_meter_values.clear();
    this->averaged_meter_values.sampledValue.clear();
}

void AverageMeterValues::set_values(const MeterValue& meter_value) {
    std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    // store all the meter values in the struct
    this->averaged_meter_values = meter_value;

    // avg all the possible measurerands
    for (auto& element : meter_value.sampledValue) {
        if (is_avg_meas(element)) {
            MeterValueCalc& temp =
                this->aligned_meter_values[{element.measurand.value(), element.phase, element.location}];
            temp.sum += element.value;
            temp.num_elements++;
        }
    }
}

MeterValue AverageMeterValues::retrieve_processed_values() {
    std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    this->average_meter_value();
    return this->averaged_meter_values;
}

void AverageMeterValues::average_meter_value() {
    for (auto& element : this->averaged_meter_values.sampledValue) {
        if (is_avg_meas(element)) {

            MeterValueCalc& temp =
                this->aligned_meter_values[{element.measurand.value(), element.phase, element.location}];

            element.value = temp.sum / temp.num_elements;
        }
    }
}
bool AverageMeterValues::is_avg_meas(const SampledValue& sample) {
    if (sample.measurand.has_value() and (sample.measurand == MeasurandEnum::Current_Import) or
        (sample.measurand == MeasurandEnum::Voltage) or (sample.measurand == MeasurandEnum::Power_Active_Import) or
        (sample.measurand == MeasurandEnum::Frequency)) {
        return true;
    } else {
        return false;
    }
}
} // namespace v201
} // namespace ocpp