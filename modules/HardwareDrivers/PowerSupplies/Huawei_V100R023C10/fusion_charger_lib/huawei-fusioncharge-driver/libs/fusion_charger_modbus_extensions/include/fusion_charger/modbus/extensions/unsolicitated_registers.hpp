// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <modbus-registers/registers.hpp>
#include <optional>

namespace fusion_charger::modbus_extensions {

class DataProviderExtUnsolicitated {
public:
    DataProviderExtUnsolicitated() {
    }

    virtual bool should_uncolicitated_report() = 0;
};

template <typename T>
class DataProviderUnsolicitated : public DataProviderExtUnsolicitated,
                                  public modbus::registers::data_providers::DataProvider<T> {
public:
    DataProviderUnsolicitated() : modbus::registers::data_providers::DataProvider<T>() {
    }
};

// example for holding register
template <typename T>
class DataProviderHoldingUnsolicitated : public modbus::registers::data_providers::DataProviderHolding<T>,
                                         public DataProviderUnsolicitated<T> {
public:
    DataProviderHoldingUnsolicitated(T value) : modbus::registers::data_providers::DataProviderHolding<T>(value) {
    }

    void on_read(uint8_t* val, size_t len) override {
        modbus::registers::data_providers::DataProviderHolding<T>::on_read(val, len);
    }

    void on_write(uint8_t* val, size_t len) override {
        modbus::registers::data_providers::DataProviderHolding<T>::on_write(val, len);
    }
};

// Event provider; can only be read; reports once after report() was called
template <typename T> class DataProviderUnsolicitatedEvent : public DataProviderHoldingUnsolicitated<T> {
protected:
    bool should_report;

public:
    DataProviderUnsolicitatedEvent(T initial) : DataProviderHoldingUnsolicitated<T>(initial), should_report(false) {
    }

    bool should_uncolicitated_report() override {
        if (this->should_report) {
            this->should_report = false;
            return true;
        }
        return false;
    }

    void report(T val) {
        this->update_value(val);
        this->should_report = true;
    }
};

template <typename T>
class DataProviderHoldingUnsolicitatedReportCallback : public DataProviderHoldingUnsolicitated<T> {
protected:
    std::function<bool()> unsolicitated_report;

public:
    DataProviderHoldingUnsolicitatedReportCallback(T initial, std::function<bool()> unsolicitated_report) :
        DataProviderHoldingUnsolicitated<T>(initial), unsolicitated_report(unsolicitated_report) {
    }

    bool should_uncolicitated_report() override {
        return unsolicitated_report();
    }
};

template <typename T>
class DataProviderCallbacksUnsolicitated : public modbus::registers::data_providers::DataProviderCallbacks<T>,
                                           public DataProviderUnsolicitated<T> {
    std::function<bool()> unsolicitated_report;

public:
    DataProviderCallbacksUnsolicitated(std::function<T()> read_cb, std::function<void(T)> write_cb,
                                       std::function<bool()> unsolicitated_report) :
        modbus::registers::data_providers::DataProviderCallbacks<T>(read_cb, write_cb),
        unsolicitated_report(unsolicitated_report) {
    }

    bool should_uncolicitated_report() override {
        return unsolicitated_report();
    }

    void on_read(uint8_t* val, size_t len) override {
        modbus::registers::data_providers::DataProviderCallbacks<T>::on_read(val, len);
    }

    void on_write(uint8_t* val, size_t len) override {
        modbus::registers::data_providers::DataProviderCallbacks<T>::on_write(val, len);
    }
};

std::optional<std::vector<uint8_t>>
unsolicitated_report_helper(modbus::registers::complex_registers::ComplexRegisterUntyped* reg);

}; // namespace fusion_charger::modbus_extensions
