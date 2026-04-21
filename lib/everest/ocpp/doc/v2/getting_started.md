# Getting Started with OCPP2.0.1

## Integrate this library with your Charging Station Implementation for OCPP2.0.1

OCPP is a protocol that affects, controls and monitors many areas of a charging station's operation.

If you want to integrate this library with your charging station implementation, you have to register a couple of **callbacks** and integrate **event handlers**. This is necessary for the library to interact with your charging station according to the requirements of OCPP.

Libocpp needs registered **callbacks** in order to execute control commands defined within OCPP (e.g ResetRequest or RemoteStartTransactionRequest)

The implementation must call **event handlers** of libocpp so that the library can track the state of the charging station and trigger OCPP messages accordingly (e.g. MeterValuesRequest , StatusNotificationRequest)

Your reference within libocpp to interact is a single instance to the class ocpp::v2::ChargePoint defined in `v2/charge_point.hpp` for OCPP 2.0.1.

## HLC (ISO 15118) Smart Charging integration

If the charging station supports ISO 15118 smart charging (K15 / K16),
the integration layer needs to wire three additional callbacks and
forward two events from the ISO 15118 stack back into libocpp. All
three callbacks are optional `std::optional<std::function<…>>`; leaving
them unset cleanly downgrades to non-HLC smart charging.

### Callbacks (libocpp → integration layer)

Defined in `v2/charge_point_callbacks.hpp`:

- **`notify_ev_charging_needs_response_callback(evse_id, NotifyEVChargingNeedsStatusEnum)`**
  Fires after libocpp has decided whether to accept the EV's
  `NotifyEVChargingNeedsRequest`. The integration wires this to a
  "schedule wait" signal on the ISO 15118 stack so the stack knows
  whether to hold `ChargeParameterDiscoveryRes` for the bundle or
  reply with a fallback.

- **`transfer_ev_charging_schedules_callback(evse_id, schedules, signature_value_b64, selected_charging_schedule_id)`**
  Fires after libocpp's composite builder has produced up to three
  `SAScheduleList` tuples (K15.FR.05 / FR.07). Carries the schedules,
  optional per-tuple ISO 15118-20 signature payloads, and an optional
  CSMS-preferred schedule id. The integration forwards this to the
  ISO 15118 stack which caches it for the next
  `ChargeParameterDiscoveryRes` (15118-2) or `ScheduleExchangeRes`
  (15118-20).

- **`trigger_schedule_renegotiation_callback(evse_id)`**
  Fires when a mid-session profile change (or the EV's own proposed
  schedule in `on_ev_selected_charging_schedule` violating K15.FR.09)
  requires the EV to re-enter `ChargeParameterDiscovery`. The
  integration drives the ISO 15118 stack to latch
  `EVSENotification=ReNegotiation` on the next wire response. libocpp
  invokes `transfer_ev_charging_schedules_callback` with the refreshed
  bundle immediately before firing this callback (K16.FR.03).

### Event handlers (integration layer → libocpp)

Defined in `v2/charge_point.hpp`:

- **`on_ev_charging_needs(NotifyEVChargingNeedsRequest)`**
  Call when the ISO 15118 stack receives `ChargeParameterDiscoveryReq`
  (15118-2) or the equivalent 15118-20 state. libocpp forwards the
  request to the CSMS and decides + builds the schedule bundle.

- **`on_ev_selected_charging_schedule(evse_id, sa_schedule_tuple_id, selected_charging_schedule_id, ev_charging_schedule)`**
  Call when the EV reports its selected tuple via `PowerDeliveryReq`
  (15118-2) or the 15118-20 equivalent. Runs the K15.FR.09 boundary
  check against the cached composite, emits
  `NotifyEVChargingSchedule` to the CSMS, and fires
  `trigger_schedule_renegotiation_callback` if the EV profile is out
  of bounds.

### Learn more

The decision logic, composite builder, trigger surface, and edge cases
are documented in
[Smart Charging Use Cases — K15 / K16 / composite](ocpp_201_smart_charging_in_depth.md#k15-iso-15118-smart-charging-initial-handoff).
