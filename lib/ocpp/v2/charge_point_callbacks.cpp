#include <ocpp/v2/charge_point_callbacks.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>

namespace ocpp::v2 {

bool Callbacks::all_callbacks_valid(std::shared_ptr<DeviceModel> device_model) const {
    bool valid =
        this->is_reset_allowed_callback != nullptr and this->reset_callback != nullptr and
        this->stop_transaction_callback != nullptr and this->pause_charging_callback != nullptr and
        this->connector_effective_operative_status_changed_callback != nullptr and
        this->get_log_request_callback != nullptr and this->unlock_connector_callback != nullptr and
        this->remote_start_transaction_callback != nullptr and this->is_reservation_for_token_callback != nullptr and
        this->update_firmware_request_callback != nullptr and this->security_event_callback != nullptr and
        this->set_charging_profiles_callback != nullptr and
        (!this->variable_changed_callback.has_value() or this->variable_changed_callback.value() != nullptr) and
        (!this->validate_network_profile_callback.has_value() or
         this->validate_network_profile_callback.value() != nullptr) and
        (!this->configure_network_connection_profile_callback.has_value() or
         this->configure_network_connection_profile_callback.value() != nullptr) and
        (!this->time_sync_callback.has_value() or this->time_sync_callback.value() != nullptr) and
        (!this->boot_notification_callback.has_value() or this->boot_notification_callback.value() != nullptr) and
        (!this->ocpp_messages_callback.has_value() or this->ocpp_messages_callback.value() != nullptr) and
        (!this->cs_effective_operative_status_changed_callback.has_value() or
         this->cs_effective_operative_status_changed_callback.value() != nullptr) and
        (!this->evse_effective_operative_status_changed_callback.has_value() or
         this->evse_effective_operative_status_changed_callback.value() != nullptr) and
        (!this->get_customer_information_callback.has_value() or
         this->get_customer_information_callback.value() != nullptr) and
        (!this->clear_customer_information_callback.has_value() or
         this->clear_customer_information_callback.value() != nullptr) and
        (!this->all_connectors_unavailable_callback.has_value() or
         this->all_connectors_unavailable_callback.value() != nullptr) and
        (!this->data_transfer_callback.has_value() or this->data_transfer_callback.value() != nullptr) and
        (!this->transaction_event_callback.has_value() or this->transaction_event_callback.value() != nullptr) and
        (!this->transaction_event_response_callback.has_value() or
         this->transaction_event_response_callback.value() != nullptr);

    if (valid) {
        if (device_model->get_optional_value<bool>(ControllerComponentVariables::DisplayMessageCtrlrAvailable)
                .value_or(false)) {
            if ((!this->clear_display_message_callback.has_value() or
                 this->clear_display_message_callback.value() == nullptr) or
                (!this->get_display_message_callback.has_value() or
                 this->get_display_message_callback.value() == nullptr) or
                (!this->set_display_message_callback.has_value() or
                 this->set_display_message_callback.value() == nullptr)) {
                EVLOG_error << "Display message controller is set to 'Available' in device model, but callbacks are "
                               "not (all) implemented";
                valid = false;
            }
        }

        // If cost is available and enabled, the running cost callback must be enabled as well.
        if (device_model->get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableCost)
                .value_or(false)) {
            if (!this->set_running_cost_callback.has_value() or this->set_running_cost_callback.value() == nullptr) {
                EVLOG_error << "TariffAndCost controller 'Cost' is set to 'Available' in device model, "
                               "but callback is not implemented";
                valid = false;
            }
        }

        if (device_model->get_optional_value<bool>(ControllerComponentVariables::TariffCostCtrlrAvailableTariff)
                .value_or(false)) {
            if (!this->set_display_message_callback.has_value() or
                this->set_display_message_callback.value() == nullptr) {
                EVLOG_error << "TariffAndCost controller 'Tariff' is set to 'Available'. In this case, the "
                               "set_display_message_callback must be implemented to send the tariff, but it is not";
                valid = false;
            }
        }

        if (device_model->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrAvailable)
                .value_or(false)) {
            if (!this->reserve_now_callback.has_value() or this->reserve_now_callback == nullptr) {
                EVLOG_error << "Reservation is set to 'Available' and 'Enabled' in device model, but "
                               "reserve_now_callback is not implemented.";
                valid = false;
            }

            if (!this->cancel_reservation_callback.has_value() or this->cancel_reservation_callback == nullptr) {
                EVLOG_error
                    << "Reservation is set to 'Available' and 'Enabled' in device model, but cancel_reservation "
                       "callback is not implemented";
                valid = false;
            }
        }

        if (device_model->get_optional_value<bool>(ControllerComponentVariables::V2XChargingCtrlrAvailable)
                .value_or(false) and
            device_model->get_optional_value<bool>(ControllerComponentVariables::ISO15118CtrlrAvailable)
                .value_or(false)) {
            if (!this->update_allowed_energy_transfer_modes_callback.has_value() or
                this->update_allowed_energy_transfer_modes_callback == nullptr) {
                EVLOG_error << "V2XCharging and ISO15118 are both marked as 'Available', but "
                               "update_allowed_energy_transfer_modes_callback is not implemented";
                valid = false;
            }
        }
    }

    return valid;
}
} // namespace ocpp::v2
