// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-server/server_exception.hpp>

using namespace modbus_server;

ModbusBasicServer::ModbusBasicServer(std::shared_ptr<PDUCorrelationLayerIntf> pas, logs::LogIntf log) :
    pas(pas), log(log) {
    pas->set_on_pdu(std::bind(&ModbusBasicServer::on_pdu_error_handled, this, std::placeholders::_1));
}

void ModbusBasicServer::set_read_holding_registers_request_cb(
    AlwaysRespondingPDUHandler<pdu::ReadHoldingRegistersRequest, pdu::ReadHoldingRegistersResponse> fn) {
    read_registers_request = fn;
}

void ModbusBasicServer::set_write_multiple_registers_request_cb(
    AlwaysRespondingPDUHandler<pdu::WriteMultipleRegistersRequest, pdu::WriteMultipleRegistersResponse> fn) {
    write_multiple_registers_request = fn;
}

void ModbusBasicServer::set_write_single_register_request_cb(
    AlwaysRespondingPDUHandler<pdu::WriteSingleRegisterRequest, pdu::WriteSingleRegisterResponse> fn) {
    write_single_register_request = fn;
}

std::optional<pdu::GenericPDU> ModbusBasicServer::on_pdu_error_handled(const pdu::GenericPDU& input) {
    //! don't forget to change doxygen comment if exception-pdu mapping changes
    try {
        return this->on_pdu(input);
    } catch (pdu::DecodingError e) {
        log.error << "Decoding error: " + std::string(e.what());
        log.verbose << "\tOriginal data: " + e.get_original_data().to_string();

        return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::ILLEGAL_DATA_VALUE).to_generic();
    } catch (pdu::EncodingError e) {
        log.error << "Encoding error: " + std::string(e.what());

        // fallthrough to SERVER_DEVICE_FAILURE
    } catch (ApplicationServerError e) {
        return e.to_pdu(input.function_code);
    } catch (std::exception e) {
        log.error << "Unknown exception: " + std::string(e.what());

        // fallthrough to SERVER_DEVICE_FAILURE
    }

    return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::SERVER_DEVICE_FAILURE).to_generic();
}

std::optional<pdu::GenericPDU> ModbusBasicServer::on_pdu(const pdu::GenericPDU& input) {
    switch (input.function_code) {
    case 0x03: {
        if (!read_registers_request.has_value()) {
            return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION)
                .to_generic();
        }

        pdu::ReadHoldingRegistersRequest request;
        request.from_generic(input);
        auto response = this->read_registers_request.value()(request);
        return response.to_generic();
    }

    case 0x10: {
        if (!write_multiple_registers_request.has_value()) {
            return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION)
                .to_generic();
        }

        pdu::WriteMultipleRegistersRequest request;
        request.from_generic(input);
        auto response = this->write_multiple_registers_request.value()(request);
        return response.to_generic();
    }

    case 0x06: {
        if (!write_single_register_request.has_value()) {
            return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION)
                .to_generic();
        }

        pdu::WriteSingleRegisterRequest request;
        request.from_generic(input);
        auto response = this->write_single_register_request.value()(request);
        return response.to_generic();
    }
    }

    log.verbose << "Server: Unknown function code: " + std::to_string(input.function_code);

    // todo: determine if we should just ignore the PDU or send this error
    return pdu::ErrorPDU(input.function_code, (std::uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION).to_generic();
}
