// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__FRAMES_HPP
#define MODBUS_SERVER__FRAMES_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace modbus_server {
namespace pdu {

/**
 * @brief Generic Modbus PDU container, storing the function code and the raw
 * data. Can be decoded into a \c SpecificPDU
 */
struct GenericPDU {
    std::uint8_t function_code;
    std::vector<std::uint8_t> data;

    // Empty constructor
    GenericPDU();

    /**
     * @brief Construct a new GenericPDU using given data
     *
     * @param data_with_function_code The pdu data prepended by the uint8 function
     * code
     */
    GenericPDU(const std::vector<std::uint8_t>& data_with_function_code);

    /**
     * @brief Construct a new GenericPDU from a function code and the separate
     * payload data
     *
     * @param function_code the function code
     * @param data the payload data, not including the function code
     */
    GenericPDU(std::uint8_t function_code, const std::vector<std::uint8_t>& data);

    /**
     * @brief Convert the PDU to a raw data buffer, including the function code at
     * the beginning
     *
     * @return std::vector<std::uint8_t> the raw data buffer
     */
    std::vector<std::uint8_t> to_vector() const;

    /**
     * @brief Serialize the PDU to a human readable string. Useful for debugging
     *
     * @return std::string the string representation of the PDU
     */
    std::string to_string() const;
};

/**
 * @brief An exception thrown when a \c SpecificPDU cannot be encoded to a
 * \c GenericPDU
 *
 */
class EncodingError : public std::runtime_error {
public:
    EncodingError(const std::string& encode_class, const std::string& msg);
};

/**
 * @brief An exception thrown when a \c GenericPDU cannot be decoded to a
 * \c SpecificPDU . Also stores the original data that caused the error
 *
 */
class DecodingError : public std::runtime_error {
protected:
    GenericPDU original_data;

public:
    DecodingError(const GenericPDU& original_data, const std::string& decode_class, const std::string& msg);

    const GenericPDU& get_original_data() const;
};

/**
 * @brief An interface to convert a \c GenericPDU from and to a specific Modbus
 * PDU, e.g. an \c ReadHoldingRegistersRequest
 *
 */
class SpecificPDU {
public:
    virtual ~SpecificPDU() = default;

    /**
     * @brief Populate the \c SpecificPDU from a \c GenericPDU
     *
     * @param generic the \c GenericPDU to decode
     * @throw DecodingError if the \c GenericPDU cannot be decoded
     */
    virtual void from_generic(const GenericPDU& generic) = 0;

    /**
     * @brief Convert the \c SpecificPDU to a \c GenericPDU
     *
     * @return GenericPDU the \c GenericPDU representation of the \c SpecificPDU
     * @throw EncodingError if the \c SpecificPDU cannot be encoded (most likely
     * due to invalid data)
     */
    virtual GenericPDU to_generic() const = 0;
};

enum class PDUExceptionCode : std::uint8_t {
    ILLEGAL_FUNCTION = 0x01,
    ILLEGAL_DATA_ADDRESS = 0x02,
    ILLEGAL_DATA_VALUE = 0x03,
    SERVER_DEVICE_FAILURE = 0x04,
    ACKNOWLEDGE = 0x05,
    SERVER_DEVICE_BUSY = 0x06,
    MEMORY_PARITY_ERROR = 0x08,
    GATEWAY_PATH_UNAVAILABLE = 0x0A,
    GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,
};

std::string exception_code_to_string(PDUExceptionCode code);

/**
 * @brief A \c SpecificPDU representing an Exception frame with an original
 * function code and an exception code
 *
 */
class ErrorPDU : public SpecificPDU {
public:
    std::uint8_t function_code;
    std::uint8_t exception_code;

    ErrorPDU();
    ErrorPDU(std::uint8_t function_code, std::uint8_t exception_code);

    void from_generic(const pdu::GenericPDU& generic) override;
    GenericPDU to_generic() const override;
};

/**
 * @brief Exception representing an received \c ErrorPDU without a specific
 * function code. Encouraged use primarily for Clients.
 */
class PDUException : public std::exception {
protected:
    std::uint8_t exception_code;
    std::string message;

public:
    PDUException(const GenericPDU& pdu);
    PDUException(PDUExceptionCode exception_code);
    std::uint8_t get_exception_code() const;

    const char* what() const noexcept override;
};

} // namespace pdu

} // namespace modbus_server

#endif
