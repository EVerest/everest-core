// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/frames.hpp>

using namespace modbus_server::pdu;

GenericPDU::GenericPDU(const std::vector<uint8_t>& data_with_function_code) : data() {
    if (data_with_function_code.size() < 1) {
        throw EncodingError("GenericPDU", "Data size must be at least 1");
    }
    this->data.insert(this->data.end(), data_with_function_code.begin() + 1, data_with_function_code.end());
    function_code = data_with_function_code[0];
}

GenericPDU::GenericPDU() : function_code(0), data() {
}

GenericPDU::GenericPDU(uint8_t function_code, const std::vector<uint8_t>& data) :
    function_code(function_code), data(data) {
}

std::vector<uint8_t> GenericPDU::to_vector() const {
    std::vector<uint8_t> ret;
    ret.push_back(function_code);
    ret.insert(ret.end(), data.begin(), data.end());
    return ret;
}

std::string GenericPDU::to_string() const {
    std::string ret;

    ret += "GenericPDU(fn_code: " + std::to_string(function_code) + ", data: [";

    size_t data_size = data.size();
    for (int i = 0; i < data_size; i++) {
        ret += std::to_string(data[i]);
        if (i != data_size - 1) {
            ret += ", ";
        }
    }

    ret += "])";
    return ret;
}

void ErrorPDU::from_generic(const pdu::GenericPDU& generic) {
    if ((generic.function_code & 0x80) == 0) {
        throw DecodingError(generic, "ErrorPDU", "Not an error PDU");
    }

    if (generic.data.size() != 1) {
        throw DecodingError(generic, "ErrorPDU", "Invalid data size");
    }

    function_code = generic.function_code & 0x7f;
    exception_code = generic.data[0];
}

GenericPDU ErrorPDU::to_generic() const {
    return GenericPDU({(uint8_t)(function_code | 0x80), exception_code});
}

EncodingError::EncodingError(const std::string& encode_class, const std::string& msg) :
    std::runtime_error("Could not encode from " + encode_class + ": " + msg) {
}

DecodingError::DecodingError(const GenericPDU& original_data, const std::string& decode_class, const std::string& msg) :
    std::runtime_error("Could not decode to " + decode_class + ": " + msg), original_data(original_data) {
}
const GenericPDU& DecodingError::get_original_data() const {
    return original_data;
}

ErrorPDU::ErrorPDU() : function_code(0), exception_code(0) {
}

ErrorPDU::ErrorPDU(uint8_t function_code, uint8_t exception_code) :
    function_code(function_code & 0x7f), exception_code(exception_code) {
}

std::string modbus_server::pdu::exception_code_to_string(PDUExceptionCode code) {
    switch (code) {
    case PDUExceptionCode::ILLEGAL_FUNCTION:
        return "ILLEGAL_FUNCTION";
    case PDUExceptionCode::ILLEGAL_DATA_ADDRESS:
        return "ILLEGAL_DATA_ADDRESS";
    case PDUExceptionCode::ILLEGAL_DATA_VALUE:
        return "ILLEGAL_DATA_VALUE";
    case PDUExceptionCode::SERVER_DEVICE_FAILURE:
        return "SERVER_DEVICE_FAILURE";
    case PDUExceptionCode::ACKNOWLEDGE:
        return "ACKNOWLEDGE";
    case PDUExceptionCode::SERVER_DEVICE_BUSY:
        return "SERVER_DEVICE_BUSY";
    case PDUExceptionCode::MEMORY_PARITY_ERROR:
        return "MEMORY_PARITY_ERROR";
    case PDUExceptionCode::GATEWAY_PATH_UNAVAILABLE:
        return "GATEWAY_PATH_UNAVAILABLE";
    case PDUExceptionCode::GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND:
        return "GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND";
    default:
        return "Unknown (" + std::to_string((uint8_t)code) + ")";
    }
}

PDUException::PDUException(const GenericPDU& pdu) {
    if (pdu.data.size() != 1) {
        throw DecodingError(pdu, "PDUException", "Invalid data size");
    }
    this->exception_code = pdu.data[0];

    this->message = "PDUException: " + exception_code_to_string((PDUExceptionCode)this->exception_code);
}

PDUException::PDUException(PDUExceptionCode exception_code) :
    exception_code((uint8_t)exception_code), message("PDUException: " + exception_code_to_string(exception_code)) {
}

const char* PDUException::what() const noexcept {
    return message.c_str();
};

uint8_t PDUException::get_exception_code() const {
    return exception_code;
}
