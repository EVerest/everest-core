// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>

namespace modbus {
namespace registers {
namespace data_providers {

/**
 * @brief An untyped \c DataProvider for Registers. This is only a helper
 * class.
 *
 */
class DataProviderUntyped {
public:
    virtual ~DataProviderUntyped() {
    }
};

/**
 * @brief A typed DataProvider for Registers. This is a interface
 *
 * @tparam T the provided data type. String types are generally handled by the
 * sub-interface \c DataProviderString
 */
template <typename T> class DataProvider : public DataProviderUntyped {
public:
    /**
     * @brief to be called when the register is read from, may also be called if a
     * partial write is happening.
     *
     * @param value The raw value of the register, already converted to system
     * endianness (can be safely cast to T*)
     * @param len The length of the value, normally sizeof(T)
     */
    virtual void on_read(uint8_t* value, size_t len) = 0;
    /**
     * @brief to be called when the register is written to, always called with the
     * full data
     *
     * @param value The raw value of the register, in system endianness, to be
     * converted later by \c Converter
     * @param len The length of the value, normally sizeof(T)
     */
    virtual void on_write(uint8_t* value, size_t len) = 0;
};

/**
 * @brief A typed DataProvider for Registers, dedicated to storing strings.
 *
 * @tparam string_length The maximum length of the string (without null
 * terminator)
 */
template <size_t string_length> class DataProviderString : public DataProvider<const char*> {};

/**
 * @brief A holding implementation for \c DataProviderString that stores the
 * data internally without callbacks.
 *
 * @tparam string_length The maximum length of the string (without null
 * terminator)
 */
template <size_t string_length> class DataProviderStringHolding : public DataProviderString<string_length> {
private:
    static void strlcpy(char* dst, const char* src, size_t size) {
        strncpy(dst, src, size - 1);
        dst[size - 1] = 0;
    }

protected:
    char value[string_length + 1];
    std::vector<std::function<void(const char*)>> callbacks;

    void notify_callbacks() {
        for (auto& callback : callbacks) {
            callback(value);
        }
    }

public:
    DataProviderStringHolding() {
        memset(value, 0, string_length);
    }
    DataProviderStringHolding(const char* value) {
        memset(this->value, 0, string_length + 1);
        strlcpy((char*)this->value, (char*)value, string_length + 1);
    }

    /**
     * @brief Add a write callback to be called when the value is written to
     *
     * @param callback The callback to be called
     */
    void add_write_callback(std::function<void(const char*)> callback) {
        callbacks.push_back(callback);
    }

    void on_read(uint8_t* value, size_t len) override {
        memset(value, 0, len);
        //! note: we are using a strncpy here because we do not want null
        //! termination in the returned string
        strncpy((char*)value, (char*)this->value, std::min(string_length, len));
        // todo: more tests
    }

    void on_write(uint8_t* value, size_t len) override {
        strlcpy((char*)this->value, (char*)value, std::min(string_length + 1, len + 1));
        // todo: more tests

        notify_callbacks();
    }

    /**
     * @brief Get the current value
     *
     * @return const char* The current value. Note that this pointer is pointing
     * to private memory
     */
    const char* get_value() const {
        return value;
    }

    /**
     * @brief Update the current value. If given string is longer than the maximum
     * length, it will be truncated.
     *
     * @param value The new value. Has to live past the function call.
     */
    void update_value(const char* value) {
        memset(this->value, 0, string_length + 1);
        strlcpy(this->value, value, string_length + 1);
    }
};

// todo: imporive read callback stuff

/**
 * @brief A typed DataProvider for Registers, dedicated to storing arrays.
 *
 * @tparam array_size The size of the array
 */
template <size_t array_size> class DataProviderMemory : public DataProvider<const uint8_t*> {};

/**
 * @brief A holding implementation for \c DataProviderMemory that stores the
 * data internally without callbacks.
 *
 * @tparam array_size The size of the array
 */
template <size_t array_size> class DataProviderMemoryHolding : public DataProviderMemory<array_size> {
protected:
    uint8_t value[array_size];
    std::vector<std::function<void(uint8_t*)>> callbacks;
    std::vector<std::function<void()>> read_callbacks;

    void notify_callbacks() {
        for (auto& callback : callbacks) {
            callback(value);
        }
    }

    void notify_read_callbacks() {
        for (auto& callback : read_callbacks) {
            callback();
        }
    }

public:
    DataProviderMemoryHolding() {
        memset(value, 0, array_size);
    }
    DataProviderMemoryHolding(const uint8_t* value) {
        memcpy(this->value, value, array_size);
    }
    DataProviderMemoryHolding(const std::vector<uint8_t>& value) {
        memcpy(this->value, value.data(), std::min(array_size, value.size()));
    }

    /**
     * @brief Add a write callback to be called when the value is written to
     *
     * @param callback The callback to be called
     */
    void add_write_callback(std::function<void(uint8_t*)> callback) {
        callbacks.push_back(callback);
    }

    void add_read_callback(std::function<void()> callback) {
        read_callbacks.push_back(callback);
    }

    void on_read(uint8_t* value, size_t len) override {
        memcpy(value, this->value, std::min(array_size, len));

        notify_read_callbacks();
    }

    void on_write(uint8_t* value, size_t len) override {
        memcpy(this->value, value, std::min(array_size, len));

        notify_callbacks();
    }

    /**
     * @brief Get the current value
     *
     * @return uint8_t* The current value. Note that this pointer is pointing to
     * private memory
     */
    const uint8_t* get_value() {
        return value;
    }

    /**
     * @brief Get the size of the array
     *
     * @return size_t The size of the array
     */
    size_t get_size() {
        return array_size;
    }

    /**
     * @brief Update the current value
     *
     * @param value The new value. Has to live past the function call.
     */
    void update_value(const uint8_t* value) {
        memcpy(this->value, value, array_size);
    }
};

/**
 * @brief A holding implementation for \c DataProvider that stores Elementary
 * data types internally without callbacks.
 *
 * @tparam T The data type to be stored
 */
template <typename T> class DataProviderHolding : public DataProvider<T> {
    static_assert(std::is_pointer<T>::value == false);

protected:
    T value;
    std::vector<std::function<void(T)>> write_callbacks;
    std::vector<std::function<void()>> read_callbacks;

    void notify_write_callbacks() {
        for (auto& callback : write_callbacks) {
            callback(value);
        }
    }

    void notify_read_callbacks() {
        for (auto& callback : read_callbacks) {
            callback();
        }
    }

public:
    /**
     * @brief Construct a new \c DataProviderHolding object with an initial value
     *
     * @param value The initial value
     */
    DataProviderHolding(T value) : value(value) {
    }

    /**
     * @brief Add a write callback to be called when the value is written to
     *
     * @param callback The callback to be called
     */
    void add_write_callback(std::function<void(T)> callback) {
        write_callbacks.push_back(callback);
    }

    void add_read_callback(std::function<void()> callback) {
        read_callbacks.push_back(callback);
    }

    void on_read(uint8_t* val, size_t len) override {
        memcpy(val, &value, std::min(sizeof(T), len));

        notify_read_callbacks();
    }

    void on_write(uint8_t* val, size_t len) override {
        memcpy(&value, val, std::min(sizeof(T), len));

        notify_write_callbacks();
    }

    /**
     * @brief Get the current value
     *
     * @return T The current value
     */
    T get_value() const {
        return value;
    }

    /**
     * @brief Update the current value
     *
     * @param value The new value
     */
    void update_value(T value) {
        this->value = value;
    }
};

template <typename T> class DataProviderCallbacks : public DataProvider<T> {
protected:
    std::function<T()> read_callback;
    std::function<void(T)> write_callback;

public:
    DataProviderCallbacks(std::function<T()> read_callback, std::function<void(T)> write_callback) :
        read_callback(read_callback), write_callback(write_callback) {
    }

    void on_read(uint8_t* val, size_t len) override {
        T value = read_callback();
        memcpy(val, &value, std::min(sizeof(T), len));
    }

    void on_write(uint8_t* val, size_t len) override {
        T value;
        memcpy(&value, val, std::min(sizeof(T), len));
        write_callback(value);
    }
};

}; // namespace data_providers
}; // namespace registers
}; // namespace modbus
