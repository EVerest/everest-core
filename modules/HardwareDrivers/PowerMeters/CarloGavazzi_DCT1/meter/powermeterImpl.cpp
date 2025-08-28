// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <thread>

#include "base64.hpp"
#include "powermeterImpl.hpp"

const int MODBUS_BASE_ADDRESS = 300001;

const int MODBUS_SIGNATURE_TYPE_ADDRESS = 309472;
const int MODBUS_PUBLIC_KEY_ADDRESS = 309473;

const int MODBUS_SIGNED_MAP_ADDRESS = 302049;
const int MODBUS_SIGNED_MAP_SIGNATURE_ADDRESS = 302126;

const int MODBUS_REAL_TIME_VALUES_ADDRESS = 300001;

inline int32_t to_int32(const transport::DataVector& data, transport::DataVector::size_type offset) {
    return data[offset] << 8 | data[offset + 1] | data[offset + 2] << 24 | data[offset + 3] << 16;
}

inline uint16_t to_uint16(const transport::DataVector& data, transport::DataVector::size_type offset) {
    return data[offset] << 8 | data[offset + 1];
}

inline std::string to_hex_string(const transport::DataVector& data, transport::DataVector::size_type offset,
                                 transport::DataVector::size_type length) {
    std::stringstream ss;
    for (std::size_t index = 0; index < length; ++index)
        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << int(data[index + offset]);
    return ss.str();
}

inline std::string to_base64_string(const transport::DataVector& data, transport::DataVector::size_type offset,
                                    transport::DataVector::size_type length) {
    auto begin = std::begin(data) + offset;
    auto end = begin + length;
    return base64::encode_into<std::string>(begin, end);
}

namespace module {
namespace meter {

void powermeterImpl::init() {
    p_modbus_transport = move(std::make_unique<transport::SerialCommHubTransport>(
        *(mod->r_modbus.get()), config.powermeter_device_id, MODBUS_BASE_ADDRESS));
}

void powermeterImpl::read_signature_config() {
    // lock device mutex
    std::scoped_lock lock(m_device_mutex);

    enum SignatureType {
        SIGNATURE_256_BIT,
        SIGNATURE_384_BIT,
        SIGNATURE_NONE
    };

    auto read_signature_type = [this]() {
        transport::DataVector data = p_modbus_transport->fetch(MODBUS_SIGNATURE_TYPE_ADDRESS, 1);
        print_data(data);
        return static_cast<SignatureType>(to_uint16(data, 0));
    };

    auto read_public_key = [this](int lengthInBits) {
        const transport::DataVector data =
            p_modbus_transport->fetch(MODBUS_PUBLIC_KEY_ADDRESS, (lengthInBits >> 3) + 1);
        print_data(data);
        return to_hex_string(data, 0, data.size() - 1);
    };

    const SignatureType signature_type = read_signature_type();

    EVLOG_info << "Signature type: " << signature_type << "\n";

    switch (signature_type) {
    case SIGNATURE_256_BIT:
        this->m_public_key_length_in_bits = 256;
        break;
    case SIGNATURE_384_BIT:
        this->m_public_key_length_in_bits = 384;
        break;
    default:
        throw std::runtime_error("no signature keys are configured");
    }

    this->m_public_key_hex = read_public_key(this->m_public_key_length_in_bits);

    EVLOG_info << "Public key: " << this->m_public_key_hex.value() << "\n";
}

void powermeterImpl::ready() {
    read_signature_config();

    std::thread t([this] {
        while (true) {
            read_powermeter_values();
            sleep(1);
        }
    });
    t.detach();
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    try {
        // lock device mutex
        std::scoped_lock lock(m_device_mutex);

        this->m_start_signed_meter_value = read_signed_meter_values();

        return {types::powermeter::TransactionRequestStatus::OK};
    } catch (const std::exception& e) {
        EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "get_signed_meter_value_error"};
    }
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    try {
        // lock device mutex
        std::scoped_lock lock(m_device_mutex);

        auto signed_meter_value = read_signed_meter_values();

        return {types::powermeter::TransactionRequestStatus::OK, this->m_start_signed_meter_value, signed_meter_value};
    } catch (const std::exception& e) {
        EVLOG_error << __PRETTY_FUNCTION__ << " Error: " << e.what() << std::endl;
        return {types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR, {}, {}, "get_signed_meter_value_error"};
    }
}

types::units_signed::SignedMeterValue powermeterImpl::read_signed_meter_values() {
    int registers =
        MODBUS_SIGNED_MAP_SIGNATURE_ADDRESS - MODBUS_SIGNED_MAP_ADDRESS + (this->m_public_key_length_in_bits >> 3);
    transport::DataVector data = p_modbus_transport->fetch(MODBUS_SIGNED_MAP_ADDRESS, registers);
    auto signed_meter_value = types::units_signed::SignedMeterValue{
        /* signed_meter_data */ to_base64_string(data, 0, data.size()),
        /* signing_method */
        fmt::format("{0} bit ECDSA SHA {0}, using curve brainpoolP{0}r1", this->m_public_key_length_in_bits),
        /* encoding_method - ?????? */ "",
        /* public_key */ this->m_public_key_hex};
    return signed_meter_value;
}

void powermeterImpl::read_powermeter_values() {
    try {
        // lock device mutex
        std::scoped_lock lock(m_device_mutex);

        transport::DataVector data = p_modbus_transport->fetch(MODBUS_REAL_TIME_VALUES_ADDRESS, 6);

        float sign = 1.;
        if (not config.low_side_sensing) {
            sign = -1.;
        }

        types::powermeter::Powermeter powermeter{};
        powermeter.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
        powermeter.meter_id = std::move(std::string(this->mod->info.id));
        powermeter.voltage_V = std::move(types::units::Voltage{0.1f * (float)to_int32(data, 0)});
        powermeter.current_A = std::move(types::units::Current{sign * 0.001f * (float)to_int32(data, 4)});
        powermeter.power_W = std::move(types::units::Power{sign * 0.1f * (float)to_int32(data, 8)});

        // TODO:
        /*
            energy_Wh_export
            energy_Wh_import
            reactive_power_VAR
            frequency_Hz
        */
        this->publish_powermeter(powermeter);
    } catch (const std::exception& e) {
        // we catch std::exception& here since there may be other exceotions than std::runtime_error
        EVLOG_warning << __PRETTY_FUNCTION__ << " Exception caught: " << e.what() << std::endl;
    }
}

void powermeterImpl::print_data(const transport::DataVector& data) {
    std::stringstream ss;

    for (size_t i = 0; i < data.size(); i++) {
        if (i != 0)
            ss << ", ";
        ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << int(data[i]);
    }

    EVLOG_debug << "received modbus response: " << ss.str() << "\n";
}

} // namespace meter
} // namespace module
