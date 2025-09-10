// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef FUSION_CHARGER_MODBUS_EXTENSIONS__UNSOLICITATED_REPORT_SERVER_HPP
#define FUSION_CHARGER_MODBUS_EXTENSIONS__UNSOLICITATED_REPORT_SERVER_HPP

#include <modbus-server/modbus_basic_server.hpp>

#include "unsolicitated_report.hpp"

namespace fusion_charger::modbus_extensions {

class UnsolicitatedReportBasicServer : public modbus_server::ModbusBasicServer {
public:
    UnsolicitatedReportBasicServer(std::shared_ptr<modbus_server::PDUCorrelationLayerIntf> corr_layer,
                                   logs::LogIntf log = logs::log_printf) :
        modbus_server::ModbusBasicServer(corr_layer, log) {
    }

    // todo: check throws things
    /**
     * @brief Send an unsolicitated report to the server
     *
     * @param request
     * @param timeout
     * @returns std::nullopt if given request doesn't request a response
     * @returns UnsolicitatedReportResponse if given request requests a response
     * @throws modbus_server::pdu::EncodingError if given request can not be
     * encoded
     * @throws modbus_server::pdu::DecodingError if the response can not be
     * decoded
     * @throws modbus_server::ApplicationServerError if the client returns an
     * error
     */
    std::optional<UnsolicitatedReportResponse> send_unsolicitated_report(UnsolicitatedReportRequest& request,
                                                                         std::chrono::milliseconds timeout);
};

}; // namespace fusion_charger::modbus_extensions

#endif
