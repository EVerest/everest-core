// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/v2/notify_report_requests_splitter.hpp>

namespace ocpp {
namespace v2 {

const std::string NotifyReportRequestsSplitter::MESSAGE_TYPE =
    conversions::messagetype_to_string(MessageType::NotifyReport);

std::vector<json> NotifyReportRequestsSplitter::create_call_payloads() {

    // In case there is no report data, fallback to no-splitting call creation
    if (!original_request.reportData.has_value()) {
        return std::vector<json>{
            {MessageTypeId::CALL, message_id_generator_callback().get(), MESSAGE_TYPE, json(original_request)}};
    }

    // Loop along reportData and create payloads
    std::vector<json> payloads{};
    int seq_no = 0;

    auto report_data_iterator = original_request.reportData->begin();
    while (seq_no == 0 || report_data_iterator != original_request.reportData->end()) {
        payloads.emplace_back(create_next_payload(seq_no, report_data_iterator, original_request.reportData->end(),
                                                  message_id_generator_callback().get()));
        seq_no++;
    }

    if (seq_no > 1) {
        EVLOG_info << "Split NotifyReportRequest '" << original_request.requestId << "' into " << seq_no
                   << " messages.";
    }

    return payloads;
}

json NotifyReportRequestsSplitter::create_next_report_data_json(
    std::vector<ocpp::v2::ReportData>::const_iterator& report_data_iterator,
    const std::vector<ocpp::v2::ReportData>::const_iterator& report_data_end, const size_t& remaining_size) {

    if (report_data_iterator == report_data_end) {
        return json::array();
    }

    json report_data_json{*report_data_iterator};
    report_data_iterator++;
    if (report_data_iterator == report_data_end) {
        return report_data_json;
    }

    auto size = report_data_json.dump().size();

    for (; report_data_iterator != report_data_end; report_data_iterator++) {
        json current_json = *report_data_iterator;
        // new report data object will increase payload size by its dump + 1 (caused by the separating comma)
        auto additional_json_size = current_json.dump().size() + 1;
        if (size + additional_json_size <= remaining_size) {
            size += additional_json_size;
            report_data_json.emplace_back(std::move(current_json));
        } else {
            break;
        }
    }

    return report_data_json;
}

json NotifyReportRequestsSplitter::create_next_payload(
    const int& seq_no, std::vector<ocpp::v2::ReportData>::const_iterator& report_data_iterator,
    const std::vector<ocpp::v2::ReportData>::const_iterator& report_data_end, const std::string& message_id) {

    json call_base{MessageTypeId::CALL, message_id, MESSAGE_TYPE};

    size_t base_json_string_length = this->json_skeleton_size + message_id.size();
    size_t remaining_size = this->max_size >= base_json_string_length ? this->max_size - base_json_string_length : 0;

    auto request_json = request_json_template;
    request_json["reportData"] = create_next_report_data_json(report_data_iterator, report_data_end, remaining_size);
    request_json["tbc"] = report_data_iterator != report_data_end;
    request_json["seqNo"] = seq_no;

    call_base.emplace_back(request_json);

    return call_base;
}
NotifyReportRequestsSplitter::NotifyReportRequestsSplitter(const NotifyReportRequest& originalRequest, size_t max_size,
                                                           std::function<MessageId()>&& message_id_generator_callback) :
    original_request(originalRequest),
    max_size(max_size),
    message_id_generator_callback{std::move(message_id_generator_callback)},
    json_skeleton_size(create_request_template_json_and_return_skeleton_size()) {
}

size_t NotifyReportRequestsSplitter::create_request_template_json_and_return_skeleton_size() {

    NotifyReportRequest req{};
    req.requestId = original_request.requestId;
    req.generatedAt = original_request.generatedAt;
    req.tbc = false;
    this->request_json_template = req;

    // Skeleton json sizeof( [MessageTypeId::CALL, "", "NotifyReport", {<json of request without
    // reportData>,"reportData":}] )
    return json{MessageTypeId::CALL, "", MESSAGE_TYPE, request_json_template}.dump().size() +
           std::string{R"(,"reportData":)"}.size();
}

} // namespace v2
} // namespace ocpp
