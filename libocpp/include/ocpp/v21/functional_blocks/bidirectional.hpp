// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/message_handler.hpp>
#include <ocpp/v2/types.hpp>

#include <ocpp/v21/messages/NotifyAllowedEnergyTransfer.hpp>

namespace ocpp::v2 {

typedef std::function<bool(const std::vector<ocpp::v2::EnergyTransferModeEnum> allowed_energy_transfer_modes,
                           const CiString<36> transaction_id)>
    NotifyAllowedEnergyTransferCallback;

class BidirectionalInterface : public MessageHandlerInterface {
public:
    virtual ~BidirectionalInterface() {
    }
};

class Bidirectional : public BidirectionalInterface {
private: // Members
    const FunctionalBlockContext& context;

    std::optional<NotifyAllowedEnergyTransferCallback> notify_allowed_energy_transfer_callback;

public:
    explicit Bidirectional(const FunctionalBlockContext& context,
                           std::optional<NotifyAllowedEnergyTransferCallback> notify_allowed_energy_transfer_callback);
    ~Bidirectional();

    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;

private: // Functions
    void handle_notify_allowed_energy_transfer(Call<v21::NotifyAllowedEnergyTransferRequest> msg);
};

} // namespace ocpp::v2
