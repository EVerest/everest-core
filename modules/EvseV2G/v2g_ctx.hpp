// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef V2G_CTX_H
#define V2G_CTX_H

#include "v2g.hpp"

#include <stdbool.h>

#define PHY_VALUE_MULT_MIN  -3
#define PHY_VALUE_MULT_MAX  3
#define PHY_VALUE_VALUE_MIN SHRT_MIN
#define PHY_VALUE_VALUE_MAX SHRT_MAX

static const char* selected_energy_transfer_mode_string[] = {
    "AC_single_phase_core", "AC_three_phase_core", "DC_core", "DC_extended", "DC_combo_core", "DC_unique",
};

struct v2g_context* v2g_ctx_create(ISO15118_chargerImplBase* p_chargerImplBase, evse_securityIntf* r_security);

/*!
 * \brief v2g_ctx_init_charging_session This funcion inits a charging session.
 * \param ctx is a pointer of type \c v2g_context. It holds the charging values.
 * \param is_connection_terminated must be set to \c true if the connection is terminated.
 */
void v2g_ctx_init_charging_session(struct v2g_context* const ctx, bool is_connection_terminated);

/*!
 * \brief init_physical_value This funcion inits a physicalValue struct.
 * \param physicalValue is the struct of the physical value.
 * \param unit is the unit of the physical value.
 */
void init_physical_value(struct iso1PhysicalValueType* const physicalValue, iso1unitSymbolType unit);

/*!
 * \brief populate_physical_value This function fills all elements of a \c iso1PhysicalValueType struct regarding the
 * parameter value and unit.
 * \param pv is pointer to the physical value struct
 * \param value is the physical value
 * \param unit is the unit of the physical value
 * \return Returns \c true if the convertion was succesfull, otherwise \c false.
 */
bool populate_physical_value(struct iso1PhysicalValueType* pv, long long int value, iso1unitSymbolType unit);

/*!
 * \brief populate_physical_value_float This function fills all elements of a \c iso1PhysicalValueType struct from a
 * json object.
 * \param pv is pointer to the physical value struct
 * \param value is the physical value
 * \param decimal_places is to determine the precision
 * \param unit is the unit of the physical value
 */
void populate_physical_value_float(struct iso1PhysicalValueType* pv, float value, uint8_t decimal_places,
                                   iso1unitSymbolType unit);

/*!
 * \brief setMinPhysicalValue This function sets the minimum value of ASrcPhyValue and ADstPhyValue in ADstPhyValue.
 * \param ADstPhyValue is the destination value, where the minimum value will be stored.
 * \param ASrcPhyValue is the source value, which will be compared with the ADstPhyValue value.
 * \param AIsUsed If AIsUsed is \c 0 ASrcPhyValue will be used to initialize ADstPhyValue and AIsUsed will be set to
 * \c 1. Can be set to \c NULL
 */
void setMinPhysicalValue(struct iso1PhysicalValueType* ADstPhyValue, const struct iso1PhysicalValueType* ASrcPhyValue,
                         unsigned int* AIsUsed);

/*!
 * \brief v2g_ctx_init_charging_state This function inits the charging state. This should be called afer a terminated
 * charging session.
 * \param ctx is a pointer of type \c v2g_context. It holds the charging values.
 * \param is_connection_terminated is set to \c true if the connection is terminated
 */
void v2g_ctx_init_charging_state(struct v2g_context* const ctx, bool is_connection_terminated);

/*!
 * \brief init_charging_values This function inits all charge-values (din/iso). This should be called after starting the
 * charging session.
 * \param ctx is a pointer of type \c v2g_context. It holds the charging values.
 */
void v2g_ctx_init_charging_values(struct v2g_context* const ctx);

/*!
 * \brief v2g_ctx_free
 * \param ctx
 */
void v2g_ctx_free(struct v2g_context* ctx);

/*!
 * \brief stop_timer This function stops a event timer. Note: mqtt_lock mutex must be unclocked before
 *  calling of this function.
 * \param event_timer is the event timer.
 * \param timer_name is the name of the event timer.
 */
void stop_timer(struct event** event_timer, char const* const timer_name, struct v2g_context* ctx);

/*!
 * \brief publish_DC_EVMaximumLimits This function publishes the DC_EVMaximumLimits
 * \param ctx  is a pointer of type \c v2g_context
 * \param v2g_dc_ev_max_current_limit is the EV max current limit
 * \param v2g_dc_ev_max_current_limit_is_used is set to \c true if used, otherwise \c false
 * \param v2g_dc_ev_max_power_limit is the EV max power limit
 * \param v2g_dc_ev_max_power_limit_is_used is set to \c true if used, otherwise \c false
 * \param v2g_dc_ev_max_voltage_limit is the EV max voltage limit
 * \param v2g_dc_ev_max_voltage_limit_is_used is set to \c true if used, otherwise \c false
 */
void publish_DC_EVMaximumLimits(struct v2g_context* ctx, const float& v2g_dc_ev_max_current_limit,
                                const unsigned int& v2g_dc_ev_max_current_limit_is_used,
                                const float& v2g_dc_ev_max_power_limit,
                                const unsigned int& v2g_dc_ev_max_power_limit_is_used,
                                const float& v2g_dc_ev_max_voltage_limit,
                                const unsigned int& v2g_dc_ev_max_voltage_limit_is_used);

/*!
 * \brief publish_DC_EVMaximumLimits This function publishes the DC_EVTargetValues
 * \param ctx  is a pointer of type \c v2g_context
 * \param v2g_dc_ev_target_voltage is the EV target voltage
 * \param v2g_dc_ev_target_current is the EV target current
 */
void publish_DC_EVTargetVoltageCurrent(struct v2g_context* ctx, const float& v2g_dc_ev_target_voltage,
                                       const float& v2g_dc_ev_target_current);

/*!
 * \brief publish_DC_EVRemainingTime This function publishes the DC_EVRemainingTime
 * \param ctx is a pointer of type \c v2g_context
 * \param iso1_dc_ev_remaining_time_to_full_soc is the EV remaining time to full soc
 * \param iso1_dc_ev_remaining_time_to_full_soc_is_used is set to \c true if used, otherwise \c false
 * \param iso1_dc_ev_remaining_time_to_bulk_soc is the EV remaining time to bulk soc
 * \param iso1_dc_ev_remaining_time_to_bulk_soc_is_used is set to \c true if used, otherwise \c false
 */
void publish_DC_EVRemainingTime(struct v2g_context* ctx, const float& iso1_dc_ev_remaining_time_to_full_soc,
                                const unsigned int& iso1_dc_ev_remaining_time_to_full_soc_is_used,
                                const float& iso1_dc_ev_remaining_time_to_bulk_soc,
                                const unsigned int& iso1_dc_ev_remaining_time_to_bulk_soc_is_used);

/*!
 * \brief log_selected_energy_transfer_type This function logs the selected_energy_transfer_mode
 */
void log_selected_energy_transfer_type(int selected_energy_transfer_mode);

/*!
 * \brief add_service_to_service_list This function adds a service list item to the service list.
 * \param v2g_ctx is a pointer of type \c v2g_context
 * \param evse_service is service which shall be provided by the EVSE in the service list.
 * \param parameter_set_id is an array of optional service parameter-set-IDs
 * \param parameter_set_id_len is the array length of parameter_set_id
 * \return Returns \c true if it was successful, otherwise \c false.
 */
bool add_service_to_service_list(struct v2g_context* v2g_ctx, const struct iso1ServiceType& evse_service,
                                 const int16_t* parameter_set_id = NULL, uint8_t parameter_set_id_len = 0);

/*!
 * \brief configure_parameter_set This function configures the parameter-set structure of a specific service ID.
 * \param v2g_ctx is a pointer of type \c v2g_context
 * \param parameterSetId is the parameter-set-ID which belongs to the service ID.
 * \param serviceId is the service ID. Currently only service ID 2 ("Certificate") supported.
 */
void configure_parameter_set(struct iso1ServiceParameterListType* parameterSetList, int16_t parameterSetId,
                             uint16_t serviceId);

#endif /* V2G_CTX_H */
