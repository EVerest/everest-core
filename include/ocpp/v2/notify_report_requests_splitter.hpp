// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_NOTIFY_REPORT_REQUESTS_SPLITTER_HPP
#define OCPP_NOTIFY_REPORT_REQUESTS_SPLITTER_HPP

#include "ocpp/common/call_types.hpp"
#include "ocpp/v2/messages/NotifyReport.hpp"
#include "ocpp/v2/types.hpp"

namespace ocpp {
namespace v2 {

/// \brief Utility class that is used to split NotifyReportRequest into several ones in case ReportData is too big.
class NotifyReportRequestsSplitter {

private:
    // cppcheck-suppress unusedStructMember
    static const std::string MESSAGE_TYPE; // NotifyReport
    const NotifyReportRequest& original_request;
    // cppcheck-suppress unusedStructMember
    size_t max_size;
    const std::function<MessageId()> message_id_generator_callback;
    json request_json_template; // json that is used  as template for request json
    // cppcheck-suppress unusedStructMember
    const size_t json_skeleton_size; // size of the json skeleton for a call json object which includes everything
                                     // except the requests' reportData and the messageId

public:
    NotifyReportRequestsSplitter(const NotifyReportRequest& originalRequest, size_t max_size,
                                 std::function<MessageId()>&& message_id_generator_callback);
    NotifyReportRequestsSplitter() = delete;

    /// \brief Splits the provided NotifyReportRequest into (potentially) several Call payloads
    /// \returns the json messages that serialize the resulting Call<NotifyReportRequest> objects
    std::vector<json> create_call_payloads();

private:
    size_t create_request_template_json_and_return_skeleton_size();

    // Create next call payload (with as many reportData items as possible)
    json create_next_payload(const int& seq_no, std::vector<ocpp::v2::ReportData>::const_iterator& report_data_iterator,
                             const std::vector<ocpp::v2::ReportData>::const_iterator& report_data_end,
                             const std::string& message_id);

    // Create next request payload (with as many reportData items as possible) to be contained in next call payload
    static json create_next_report_data_json(std::vector<ocpp::v2::ReportData>::const_iterator& report_data_iterator,
                                             const std::vector<ocpp::v2::ReportData>::const_iterator& report_data_end,
                                             const size_t& remaining_size);
};

} // namespace v2
} // namespace ocpp

#endif // OCPP_NOTIFY_REPORT_REQUESTS_SPLITTER_HPP
