// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

namespace ocpp {
namespace v2 {
struct FunctionalBlockContext;
struct DataTransferRequest;
struct DataTransferResponse;

class DataTransferInterface : public MessageHandlerInterface {

public:
    virtual ~DataTransferInterface() = default;

    /// \brief Sends a DataTransfer.req message to the CSMS using the given parameters
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return DataTransferResponse containing the result from CSMS
    virtual std::optional<DataTransferResponse> data_transfer_req(const CiString<255>& vendorId,
                                                                  const std::optional<CiString<50>>& messageId,
                                                                  const std::optional<json>& data) = 0;

    /// \brief Sends a DataTransfer.req message to the CSMS using the given \p request
    /// \param request message shall be sent to the CSMS
    /// \return DataTransferResponse containing the result from CSMS. In case no response is received from the CSMS
    /// because the message timed out or the charging station is offline, std::nullopt is returned
    virtual std::optional<DataTransferResponse> data_transfer_req(const DataTransferRequest& request) = 0;
};

class DataTransfer : public DataTransferInterface {

private:
    const FunctionalBlockContext& context;
    std::optional<std::function<DataTransferResponse(const DataTransferRequest& request)>> data_transfer_callback;
    std::chrono::seconds response_timeout;

public:
    DataTransfer(const FunctionalBlockContext& functional_block_context,
                 const std::optional<std::function<DataTransferResponse(const DataTransferRequest& request)>>&
                     data_transfer_callback,
                 const std::chrono::seconds response_timeout) :
        context(functional_block_context),
        data_transfer_callback(data_transfer_callback),
        response_timeout(response_timeout){};

    void handle_message(const EnhancedMessage<MessageType>& message) override;

    std::optional<DataTransferResponse> data_transfer_req(const CiString<255>& vendorId,
                                                          const std::optional<CiString<50>>& messageId,
                                                          const std::optional<json>& data) override;

    std::optional<DataTransferResponse> data_transfer_req(const DataTransferRequest& request) override;
};

} // namespace v2
} // namespace ocpp
