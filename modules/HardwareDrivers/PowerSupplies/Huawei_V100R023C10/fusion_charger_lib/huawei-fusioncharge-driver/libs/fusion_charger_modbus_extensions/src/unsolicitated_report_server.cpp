// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/extensions/unsolicitated_report_server.hpp>
#include <modbus-server/server_exception.hpp>

using namespace fusion_charger::modbus_extensions;

std::optional<UnsolicitatedReportResponse>
UnsolicitatedReportBasicServer::send_unsolicitated_report(UnsolicitatedReportRequest& request,
                                                          std::chrono::milliseconds timeout) {
    request.defragment();

    if (!request.response_required) {
        this->pas->request_without_response(request.to_generic());
        return std::nullopt;
    }

    auto response_generic = this->pas->request_response(request.to_generic(), timeout);

    if (response_generic.function_code & 0x80) {
        throw modbus_server::pdu::PDUException(response_generic);
    }

    UnsolicitatedReportResponse response;
    response.from_generic(response_generic);

    return response;
}
