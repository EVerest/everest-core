// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/v2/messages/SetDisplayMessage.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
struct GetDisplayMessagesRequest;
struct ClearDisplayMessageResponse;
struct ClearDisplayMessageRequest;

///
/// \brief Convert message content from OCPP spec to DisplayMessageContent.
/// \param message_content  The struct to convert.
/// \return The converted struct.
///
DisplayMessageContent message_content_to_display_message_content(const MessageContent& message_content);
///
/// \brief Convert display message to MessageInfo from OCPP.
/// \param display_message  The struct to convert.
/// \return The converted struct.
///
std::optional<MessageInfo> display_message_to_message_info_type(const DisplayMessage& display_message);
///
/// \brief Convert message info from OCPP to DisplayMessage.
/// \param message_info The struct to convert.
/// \return The converted struct.
///
DisplayMessage message_info_to_display_message(const MessageInfo& message_info);

using GetDisplayMessageCallback =
    std::function<std::vector<ocpp::DisplayMessage>(const GetDisplayMessagesRequest& request)>;
using SetDisplayMessageCallback =
    std::function<SetDisplayMessageResponse(const std::vector<DisplayMessage>& display_messages)>;
using ClearDisplayMessageCallback =
    std::function<ClearDisplayMessageResponse(const ClearDisplayMessageRequest& request)>;

class DisplayMessageInterface : public MessageHandlerInterface {
public:
    ~DisplayMessageInterface() override = default;
};

class DisplayMessageBlock : public DisplayMessageInterface {

public:
    DisplayMessageBlock(const FunctionalBlockContext& functional_block_context,
                        GetDisplayMessageCallback get_display_message_callback,
                        SetDisplayMessageCallback set_display_message_callback,
                        ClearDisplayMessageCallback clear_display_message_callback);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

private:
    const FunctionalBlockContext& context;

    GetDisplayMessageCallback get_display_message_callback;
    SetDisplayMessageCallback set_display_message_callback;
    ClearDisplayMessageCallback clear_display_message_callback;

    void handle_get_display_message(Call<GetDisplayMessagesRequest> call);
    void handle_set_display_message(Call<SetDisplayMessageRequest> call);
    void handle_clear_display_message(Call<ClearDisplayMessageRequest> call);
};

} // namespace ocpp::v2
