// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <modbus-registers/data_provider.hpp>
#include <string>

namespace fusion_charger::telemetry {

/**
 * @brief Base interface class for fusion charger telemetry managers which are responsible for publishing telemetry
 * datapoints
 */
class TelemetryPublisherBase {
public:
    virtual ~TelemetryPublisherBase() = default;
    /**
     * @brief Add a new subtopic for telemetry datapoints
     * @param subtopic The subtopic name
     */
    virtual void add_subtopic(const std::string& subtopic) = 0;
    /**
     * @brief Notify that a datapoint for a subtopic has changed (string value)
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param value The new value
     */
    virtual void datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                                   const std::string& value) = 0;
    /**
     * @brief Notify that a datapoint for a subtopic has changed (double value)
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param value The new value
     */
    virtual void datapoint_changed(const std::string& subtopic, const std::string& datapoint, double value) = 0;

    /**
     * @brief Notify that a datapoint for a subtopic has changed (bool value)
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param value The new value
     */
    virtual void datapoint_changed(const std::string& subtopic, const std::string& datapoint, bool value) = 0;

    /**
     * @brief Check if a datapoint exists for a subtopic
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @return true if the datapoint exists, false otherwise
     */
    virtual bool datapoint_exists(const std::string& subtopic, const std::string& datapoint) = 0;

    /**
     * @brief Initialize a datapoint for a subtopic with null
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     */
    virtual void initialize_datapoint(const std::string& subtopic, const std::string& datapoint) = 0;

    /**
     * @brief Initialize a datapoint for a subtopic with a value
     * @note Does nothing if the datapoint already exists
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param value The initial value
     */
    template <typename T>
    void initialize_datapoint(const std::string& subtopic, const std::string& datapoint, T value) {
        if (!datapoint_exists(subtopic, datapoint)) {
            datapoint_changed(subtopic, datapoint, value);
        }
    }

    /**
     * @brief Utility to add a callback to a holding complex register data provider that notifies the telemetry manager
     * on value changes
     * @tparam T The type of the data provider value
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param data_provider The data provider to add the callback to
     * @param conversion_func An optional conversion function to convert (e.g. to SI units) the value before sending it
     * to the telemetry manager
     */
    template <typename T>
    void
    register_complex_register_data_provider(const std::string& subtopic, const std::string& datapoint,
                                            modbus::registers::data_providers::DataProviderHolding<T>* data_provider,
                                            std::function<T(const T&)> conversion_func = nullptr) {
        initialize_datapoint(subtopic, datapoint);
        data_provider->add_write_callback([this, subtopic, datapoint, conversion_func](T value) {
            if (conversion_func) {
                value = conversion_func(value);
            }
            this->datapoint_changed(subtopic, datapoint, value);
        });
    }

    /**
     * @brief Utility to add a callback to a holding complex register data provider that notifies the telemetry manager
     * on value changes, converting the value to a string using the provided function
     * @tparam T The type of the data provider value
     * @param subtopic The subtopic name
     * @param datapoint The datapoint name
     * @param data_provider The data provider to add the callback to
     * @param to_string_func The function to convert the value to a string
     */
    template <typename T>
    void register_complex_register_data_provider_enum(
        const std::string& subtopic, const std::string& datapoint,
        modbus::registers::data_providers::DataProviderHolding<T>* data_provider,
        std::function<std::string(const T&)> to_string_func) {
        initialize_datapoint(subtopic, datapoint);
        data_provider->add_write_callback([this, subtopic, datapoint, to_string_func](T value) {
            this->datapoint_changed(subtopic, datapoint, to_string_func(value));
        });
    }
};

/**
 * @brief Null implementation of the TelemetryPublisherBase that does nothing
 */
class TelemetryPublisherNull : public TelemetryPublisherBase {
public:
    void add_subtopic(const std::string& subtopic) override {
    }
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint,
                           const std::string& value) override {
    }
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint, double value) override {
    }
    void datapoint_changed(const std::string& subtopic, const std::string& datapoint, bool value) override {
    }
    void initialize_datapoint(const std::string& subtopic, const std::string& datapoint) override {
    }
    bool datapoint_exists(const std::string& subtopic, const std::string& datapoint) override {
        return true;
    }
};

}; // namespace fusion_charger::telemetry
