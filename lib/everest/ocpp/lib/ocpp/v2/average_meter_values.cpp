// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <ocpp/v2/average_meter_values.hpp>

namespace ocpp {
namespace v2 {
namespace {
bool is_avg_meas(const SampledValue& sample) {
    if (sample.measurand.has_value() and (sample.measurand == MeasurandEnum::Current_Import) or
        (sample.measurand == MeasurandEnum::Voltage) or (sample.measurand == MeasurandEnum::Power_Active_Import) or
        (sample.measurand == MeasurandEnum::Frequency)) {
        return true;
    }
    return false;
}
} // namespace

void AverageMeterValues::clear_values() {
    const std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    this->aligned_meter_values.clear();
    this->averaged_meter_values.sampledValue.clear();
}

void AverageMeterValues::set_values(const MeterValue& meter_value) {
    const std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    // store all the meter values in the struct
    this->averaged_meter_values = meter_value;

    // avg all the possible measurerands
    for (auto& element : meter_value.sampledValue) {
        if (is_avg_meas(element)) {
            if (element.measurand.has_value()) {
                const MeterValueMeasurands key{element.measurand.value(), element.phase, element.location};
                this->aligned_meter_values.try_emplace(key,
                                                       MeterValueCalc{0.0, 0}); // If not exists yet, set to default

                MeterValueCalc& temp = this->aligned_meter_values.at(key);
                temp.sum += element.value;
                temp.num_elements++;
            }
        }
    }
}

MeterValue AverageMeterValues::retrieve_processed_values() {
    const std::lock_guard<std::mutex> lk(this->avg_meter_value_mutex);
    this->average_meter_value();
    return this->averaged_meter_values;
}

void AverageMeterValues::average_meter_value() {
    for (auto& element : this->averaged_meter_values.sampledValue) {
        if (is_avg_meas(element)) {
            if (element.measurand.has_value()) {
                const auto measurand = element.measurand.value();
                const MeterValueMeasurands key{measurand, element.phase, element.location};
                if (this->aligned_meter_values.find(key) == this->aligned_meter_values.end()) {
                    EVLOG_warning << "Measurand: " << measurand << " not present in map";
                } else {
                    const MeterValueCalc& temp = this->aligned_meter_values.at(key);
                    element.value = static_cast<float>(temp.sum / temp.num_elements);
                }
            }
        }
    }
}
} // namespace v2
} // namespace ocpp
