// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/message_dispatcher.hpp>
#include <ocpp/v201/message_handler.hpp>
#include <ocpp/v201/messages/ClearDisplayMessage.hpp>
#include <ocpp/v201/messages/GetDisplayMessages.hpp>
#include <ocpp/v201/messages/SetDisplayMessage.hpp>

namespace ocpp::v201 {

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

typedef std::function<std::vector<ocpp::DisplayMessage>(const GetDisplayMessagesRequest& request)>
    GetDisplayMessageCallback;
typedef std::function<SetDisplayMessageResponse(const std::vector<DisplayMessage>& display_messages)>
    SetDisplayMessageCallback;
typedef std::function<ClearDisplayMessageResponse(const ClearDisplayMessageRequest& request)>
    ClearDisplayMessageCallback;

class DisplayMessageInterface : public MessageHandlerInterface {
public:
    virtual ~DisplayMessageInterface(){};
};

class DisplayMessageBlock : public DisplayMessageInterface {

public:
    DisplayMessageBlock(MessageDispatcherInterface<MessageType>& message_dispatcher, DeviceModel& device_model,
                        EvseManagerInterface& evse_manager, GetDisplayMessageCallback get_display_message_callback,
                        SetDisplayMessageCallback set_display_message_callback,
                        ClearDisplayMessageCallback clear_display_message_callback);
    virtual void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

private:
    MessageDispatcherInterface<MessageType>& message_dispatcher;
    DeviceModel& device_model;
    EvseManagerInterface& evse_manager;

    GetDisplayMessageCallback get_display_message_callback;
    SetDisplayMessageCallback set_display_message_callback;
    ClearDisplayMessageCallback clear_display_message_callback;

    void handle_get_display_message(Call<GetDisplayMessagesRequest> call);
    void handle_set_display_message(Call<SetDisplayMessageRequest> call);
    void handle_clear_display_message(Call<ClearDisplayMessageRequest> call);
};

} // namespace ocpp::v201