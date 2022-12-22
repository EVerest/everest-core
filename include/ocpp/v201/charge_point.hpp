// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <future>

#include <ocpp/common/charge_point.hpp>

#include <ocpp/v201/charge_point_configuration.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>

#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>

namespace ocpp {
namespace v201 {

/// \brief Class implements OCPP2.0.1 charging point
class ChargePoint : ocpp::ChargePoint {

private:
    std::unique_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::shared_ptr<ChargePointConfiguration> configuration;

    // general message handling
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);

    void init_websocket();

    void handle_message(const json& json_message, MessageType message_type);
    void message_callback(const std::string& message);

    // message requests

    // Provisioning
    void boot_notification_req();

    // message response handlers

    // Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);

public:
    ChargePoint(const json& config, const std::string& ocpp_main_path, const std::string& message_log_path);

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    bool start();
};

} // namespace v201
} // namespace ocpp
