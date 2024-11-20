// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/common/constants.hpp>
#include <ocpp/v201/functional_blocks/data_transfer.hpp>

namespace ocpp {
namespace v201 {

void DataTransfer::handle_message(const EnhancedMessage<MessageType>& message) {

    if (message.messageType != MessageType::DataTransfer) {
        throw MessageTypeNotImplementedException(message.messageType);
    }

    Call<DataTransferRequest> call = message.message;
    const auto msg = call.msg;
    DataTransferResponse response;
    response.status = DataTransferStatusEnum::UnknownVendorId;

    if (this->data_transfer_callback.has_value()) {
        response = this->data_transfer_callback.value()(call.msg);
    } else {
        response.status = DataTransferStatusEnum::UnknownVendorId;
        EVLOG_warning << "Received a DataTransferRequest but no data transfer callback was registered";
    }

    ocpp::CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->message_dispatcher.dispatch_call_result(call_result);
}

std::optional<DataTransferResponse> DataTransfer::data_transfer_req(const CiString<255>& vendorId,
                                                                    const std::optional<CiString<50>>& messageId,
                                                                    const std::optional<json>& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data = data;

    return this->data_transfer_req(req);
}

std::optional<DataTransferResponse> DataTransfer::data_transfer_req(const DataTransferRequest& request) {
    DataTransferResponse response;
    response.status = DataTransferStatusEnum::Rejected;

    ocpp::Call<DataTransferRequest> call(request);
    auto data_transfer_future = this->message_dispatcher.dispatch_call_async(call);

    if (data_transfer_future.wait_for(this->response_timeout) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf future timed out";
        return std::nullopt;
    }

    auto enhanced_message = data_transfer_future.get();

    if (enhanced_message.offline) {
        return std::nullopt;
    }

    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        try {
            ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
            response = call_result.msg;
        } catch (const EnumConversionException& e) {
            EVLOG_error << "EnumConversionException during handling of message: " << e.what();
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
            this->message_dispatcher.dispatch_call_error(call_error);
            return std::nullopt;
        } catch (const json::exception& e) {
            EVLOG_error << "Unable to parse DataTransfer.conf from CSMS: " << enhanced_message.message;
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
            this->message_dispatcher.dispatch_call_error(call_error);
            return std::nullopt;
        }
    }

    return response;
}

}; // namespace v201
} // namespace ocpp
