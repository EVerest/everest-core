// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v21/functional_blocks/der_control.hpp>

#include <ocpp/common/call_types.hpp>
#include <ocpp/common/utils.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

#include <everest/logging.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>

namespace ocpp::v21 {

using namespace v2;

namespace {

constexpr std::size_t MAX_CURVE_POINTS = 256;
constexpr float MAX_DURATION_SECONDS = 86400.0F * 365.0F;          // one year
constexpr std::int64_t MAX_SCHEDULE_HORIZON_SECONDS = 86400 * 365; // one year

/// \brief Cap on total persisted DER controls to bound DER_CONTROLS table growth.
/// A malicious or compromised CSMS could otherwise issue SetDERControl with ever-unique
/// controlId values (each carrying up to MAX_CURVE_POINTS curve points) and fill the DB.
constexpr std::size_t MAX_DER_CONTROLS = 1000;

bool duration_is_sane(std::optional<float> d) {
    if (!d.has_value()) {
        return true;
    }
    return std::isfinite(d.value()) && d.value() >= 0.0F && d.value() <= MAX_DURATION_SECONDS;
}

bool is_finite_curve(const DERCurve& curve) {
    if (!is_finite_opt(curve.responseTime) || !is_finite_opt(curve.duration)) {
        return false;
    }
    for (const auto& point : curve.curveData) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y)) {
            return false;
        }
    }
    if (curve.hysteresis.has_value()) {
        const auto& h = curve.hysteresis.value();
        if (!is_finite_opt(h.hysteresisHigh) || !is_finite_opt(h.hysteresisLow) || !is_finite_opt(h.hysteresisDelay) ||
            !is_finite_opt(h.hysteresisGradient)) {
            return false;
        }
    }
    if (curve.reactivePowerParams.has_value()) {
        const auto& r = curve.reactivePowerParams.value();
        if (!is_finite_opt(r.vRef) || !is_finite_opt(r.autonomousVRefTimeConstant)) {
            return false;
        }
    }
    if (curve.voltageParams.has_value()) {
        const auto& v = curve.voltageParams.value();
        if (!is_finite_opt(v.hv10MinMeanValue) || !is_finite_opt(v.hv10MinMeanTripDelay)) {
            return false;
        }
    }
    return true;
}

/// \brief True if every float field on every populated variant of \p req is finite.
/// Applied before persistence so a CSMS-supplied NaN/Inf can never round-trip through
/// CONTROL_JSON back into a ReportDERControl.
bool are_control_floats_finite(const SetDERControlRequest& req) {
    if (req.curve.has_value() && !is_finite_curve(req.curve.value())) {
        return false;
    }
    if (req.freqDroop.has_value()) {
        const auto& fd = req.freqDroop.value();
        if (!std::isfinite(fd.overFreq) || !std::isfinite(fd.underFreq) || !std::isfinite(fd.overDroop) ||
            !std::isfinite(fd.underDroop) || !std::isfinite(fd.responseTime) || !is_finite_opt(fd.duration)) {
            return false;
        }
    }
    if (req.fixedVar.has_value()) {
        const auto& fv = req.fixedVar.value();
        if (!std::isfinite(fv.setpoint) || !is_finite_opt(fv.duration)) {
            return false;
        }
    }
    if (req.fixedPFAbsorb.has_value()) {
        const auto& pf = req.fixedPFAbsorb.value();
        if (!std::isfinite(pf.displacement) || !is_finite_opt(pf.duration)) {
            return false;
        }
    }
    if (req.fixedPFInject.has_value()) {
        const auto& pf = req.fixedPFInject.value();
        if (!std::isfinite(pf.displacement) || !is_finite_opt(pf.duration)) {
            return false;
        }
    }
    if (req.limitMaxDischarge.has_value()) {
        const auto& lmd = req.limitMaxDischarge.value();
        if (!is_finite_opt(lmd.pctMaxDischargePower) || !is_finite_opt(lmd.duration)) {
            return false;
        }
        if (lmd.powerMonitoringMustTrip.has_value() && !is_finite_curve(lmd.powerMonitoringMustTrip.value())) {
            return false;
        }
    }
    if (req.enterService.has_value()) {
        const auto& es = req.enterService.value();
        if (!std::isfinite(es.highVoltage) || !std::isfinite(es.lowVoltage) || !std::isfinite(es.highFreq) ||
            !std::isfinite(es.lowFreq) || !is_finite_opt(es.delay) || !is_finite_opt(es.randomDelay) ||
            !is_finite_opt(es.rampRate)) {
            return false;
        }
    }
    if (req.gradient.has_value()) {
        const auto& g = req.gradient.value();
        if (!std::isfinite(g.gradient) || !std::isfinite(g.softGradient)) {
            return false;
        }
    }
    return true;
}

bool mode_list_contains(const std::string& list, const std::string& wanted) {
    const auto tokens = ocpp::split_string(list, ',', true);
    return std::any_of(tokens.begin(), tokens.end(), [&](const std::string& token) { return token == wanted; });
}

/// Count how many control-specific fields are populated in the request
int count_populated_control_fields(const SetDERControlRequest& req) {
    int count = 0;
    if (req.curve.has_value())
        count++;
    if (req.enterService.has_value())
        count++;
    if (req.fixedPFAbsorb.has_value())
        count++;
    if (req.fixedPFInject.has_value())
        count++;
    if (req.fixedVar.has_value())
        count++;
    if (req.freqDroop.has_value())
        count++;
    if (req.gradient.has_value())
        count++;
    if (req.limitMaxDischarge.has_value())
        count++;
    return count;
}

/// Get the priority from whichever control field is populated
int32_t get_priority_from_request(const SetDERControlRequest& req) {
    if (req.freqDroop.has_value())
        return req.freqDroop->priority;
    if (req.curve.has_value())
        return req.curve->priority;
    if (req.enterService.has_value())
        return req.enterService->priority;
    if (req.fixedPFAbsorb.has_value())
        return req.fixedPFAbsorb->priority;
    if (req.fixedPFInject.has_value())
        return req.fixedPFInject->priority;
    if (req.fixedVar.has_value())
        return req.fixedVar->priority;
    if (req.gradient.has_value())
        return req.gradient->priority;
    if (req.limitMaxDischarge.has_value())
        return req.limitMaxDischarge->priority;
    return 0;
}

/// Get optional startTime from whichever control field is populated
std::optional<std::string> get_start_time_from_request(const SetDERControlRequest& req) {
    if (req.freqDroop.has_value() && req.freqDroop->startTime.has_value())
        return req.freqDroop->startTime->to_rfc3339();
    if (req.curve.has_value() && req.curve->startTime.has_value())
        return req.curve->startTime->to_rfc3339();
    if (req.fixedPFAbsorb.has_value() && req.fixedPFAbsorb->startTime.has_value())
        return req.fixedPFAbsorb->startTime->to_rfc3339();
    if (req.fixedPFInject.has_value() && req.fixedPFInject->startTime.has_value())
        return req.fixedPFInject->startTime->to_rfc3339();
    if (req.fixedVar.has_value() && req.fixedVar->startTime.has_value())
        return req.fixedVar->startTime->to_rfc3339();
    if (req.limitMaxDischarge.has_value() && req.limitMaxDischarge->startTime.has_value())
        return req.limitMaxDischarge->startTime->to_rfc3339();
    // EnterService and Gradient don't have startTime
    return std::nullopt;
}

/// Get optional duration from whichever control field is populated
std::optional<float> get_duration_from_request(const SetDERControlRequest& req) {
    if (req.freqDroop.has_value() && req.freqDroop->duration.has_value())
        return req.freqDroop->duration;
    if (req.curve.has_value() && req.curve->duration.has_value())
        return req.curve->duration;
    if (req.fixedPFAbsorb.has_value() && req.fixedPFAbsorb->duration.has_value())
        return req.fixedPFAbsorb->duration;
    if (req.fixedPFInject.has_value() && req.fixedPFInject->duration.has_value())
        return req.fixedPFInject->duration;
    if (req.fixedVar.has_value() && req.fixedVar->duration.has_value())
        return req.fixedVar->duration;
    if (req.limitMaxDischarge.has_value() && req.limitMaxDischarge->duration.has_value())
        return req.limitMaxDischarge->duration;
    return std::nullopt;
}

/// Check if startTime or duration is set in any control field
bool has_start_time_or_duration(const SetDERControlRequest& req) {
    return get_start_time_from_request(req).has_value() || get_duration_from_request(req).has_value();
}

/// R04.FR.50-56: Validate yUnit for curve-based controlTypes.
///
/// Every enum case MUST return explicitly. The post-switch `return false` is a
/// safety net for a future DERControlEnum value added without updating this
/// function: fail-closed rather than silently accept.
bool validate_yunit(DERControlEnum control_type, const DERCurve& curve) {
    auto unit = curve.yUnit;
    switch (control_type) {
    // R04.FR.50: FreqWatt -> PctMaxW or PctWAvail
    case DERControlEnum::FreqWatt:
        return unit == DERUnitEnum::PctMaxW || unit == DERUnitEnum::PctWAvail;

    // R04.FR.51: Trip curves -> Not_Applicable
    case DERControlEnum::HFMustTrip:
    case DERControlEnum::HFMayTrip:
    case DERControlEnum::HVMustTrip:
    case DERControlEnum::HVMomCess:
    case DERControlEnum::HVMayTrip:
    case DERControlEnum::LFMustTrip:
    case DERControlEnum::LVMustTrip:
    case DERControlEnum::LVMomCess:
    case DERControlEnum::LVMayTrip:
    case DERControlEnum::PowerMonitoringMustTrip:
        return unit == DERUnitEnum::Not_Applicable;

    // R04.FR.52: VoltVar -> PctMaxVar or PctVarAvail
    case DERControlEnum::VoltVar:
        return unit == DERUnitEnum::PctMaxVar || unit == DERUnitEnum::PctVarAvail;

    // R04.FR.53: VoltWatt -> PctMaxW or PctWAvail
    case DERControlEnum::VoltWatt:
        return unit == DERUnitEnum::PctMaxW || unit == DERUnitEnum::PctWAvail;

    // R04.FR.54: WattPF -> Not_Applicable
    case DERControlEnum::WattPF:
        return unit == DERUnitEnum::Not_Applicable;

    // R04.FR.55: WattVar -> PctMaxVar or PctVarAvail
    case DERControlEnum::WattVar:
        return unit == DERUnitEnum::PctMaxVar || unit == DERUnitEnum::PctVarAvail;

    // Non-curve types, no yUnit validation needed
    case DERControlEnum::EnterService:
    case DERControlEnum::FreqDroop:
    case DERControlEnum::FixedPFAbsorb:
    case DERControlEnum::FixedPFInject:
    case DERControlEnum::FixedVar:
    case DERControlEnum::Gradients:
    case DERControlEnum::LimitMaxDischarge:
        return true;
    }
    return false;
}

} // anonymous namespace

DERControl::DERControl(const v2::FunctionalBlockContext& context) : context(context) {
    // Start periodic check for expired scheduled controls (every 30 seconds).
    // The lambda takes a sweep_stopping handle (which holds the monitor's
    // lock) for the duration of the callback so the destructor can drain
    // any in-flight run before member destruction begins.
    this->scheduled_control_timer.interval(
        [this]() {
            auto handle = this->sweep_stopping.handle();
            if (*handle) {
                return;
            }
            this->check_scheduled_controls();
        },
        std::chrono::seconds(30));
}

DERControl::~DERControl() {
    // Set the stopping flag under the lock so any callback that subsequently
    // acquires a handle observes it and short-circuits.
    {
        auto handle = this->sweep_stopping.handle();
        *handle = true;
    }
    // Cancel pending waits on the boost asio timer. After this, the timer's
    // wrapper guarantees at most one more callback may complete before the
    // io thread becomes idle.
    this->scheduled_control_timer.stop();
    // Drain: re-acquire a handle to wait for any callback that started
    // before stop() was observed by the io thread. After this returns, no
    // callback is running and none can start, so member destruction (which
    // tears down the io thread via ~Timer) is safe even if the
    // FunctionalBlockContext's referenced DatabaseHandler has already been
    // freed by the owning ChargePoint.
    auto drain_handle = this->sweep_stopping.handle();
}

void DERControl::handle_message(const ocpp::EnhancedMessage<v2::MessageType>& message) {
    const auto& json_message = message.message;
    if (message.messageType == v2::MessageType::SetDERControl) {
        this->handle_set_der_control(json_message);
    } else if (message.messageType == v2::MessageType::GetDERControl) {
        this->handle_get_der_control(json_message);
    } else if (message.messageType == v2::MessageType::ClearDERControl) {
        this->handle_clear_der_control(json_message);
    } else {
        throw v2::MessageTypeNotImplementedException(message.messageType);
    }
}

bool DERControl::is_control_type_supported(v2::DERControlEnum control_type) const {
    const auto control_type_str = v2::conversions::dercontrol_enum_to_string(control_type);
    const auto& evse_manager = this->context.evse_manager;

    for (int32_t evse_id = 1; evse_id <= static_cast<int32_t>(evse_manager.get_number_of_evses()); evse_id++) {
        auto dc_modes_cv =
            DERComponentVariables::get_dc_component_variable(evse_id, DERComponentVariables::ModesSupported);
        auto dc_modes = this->context.device_model.get_optional_value<std::string>(dc_modes_cv);
        if (dc_modes.has_value() && mode_list_contains(dc_modes.value(), control_type_str)) {
            return true;
        }

        auto ac_modes_cv =
            DERComponentVariables::get_ac_component_variable(evse_id, DERComponentVariables::ModesSupported);
        auto ac_modes = this->context.device_model.get_optional_value<std::string>(ac_modes_cv);
        if (ac_modes.has_value() && mode_list_contains(ac_modes.value(), control_type_str)) {
            return true;
        }
    }
    return false;
}

bool DERControl::validate_control_fields(const SetDERControlRequest& req) const {
    // R04.FR.17: Reject if multiple control fields are set
    if (count_populated_control_fields(req) != 1) {
        return false;
    }

    // Defense-in-depth: bound priority, duration, and curveData size before we touch the DB.
    if (get_priority_from_request(req) < 0) {
        return false;
    }
    if (!duration_is_sane(get_duration_from_request(req))) {
        return false;
    }
    if (req.curve.has_value() && req.curve->curveData.size() > MAX_CURVE_POINTS) {
        return false;
    }

    // Reject non-finite (NaN / +/-Inf) float values on any control variant. Without this
    // a CSMS-supplied NaN survives into CONTROL_JSON, serializes as JSON null, and
    // is later echoed back on ReportDERControl.
    if (!are_control_floats_finite(req)) {
        return false;
    }

    // Reject startTime beyond a bounded schedule horizon. Otherwise a zombie scheduled
    // control with startTime in year 2999 would persist until explicit Clear.
    const auto start_time_str = get_start_time_from_request(req);
    if (start_time_str.has_value()) {
        try {
            const auto start_tp = ocpp::DateTime(start_time_str.value()).to_time_point();
            const auto horizon_tp =
                ocpp::DateTime().to_time_point() + std::chrono::seconds(MAX_SCHEDULE_HORIZON_SECONDS);
            if (start_tp > horizon_tp) {
                return false;
            }
        } catch (const std::exception&) {
            // Unparseable startTime is itself invalid.
            return false;
        }
    }

    // R04.FR.16: Validate that the correct field is set for the controlType
    switch (req.controlType) {
    case DERControlEnum::FixedPFAbsorb:
        return req.fixedPFAbsorb.has_value();
    case DERControlEnum::FixedPFInject:
        return req.fixedPFInject.has_value();
    case DERControlEnum::FixedVar:
        return req.fixedVar.has_value();
    case DERControlEnum::LimitMaxDischarge:
        // R04.FR.56: If powerMonitoringMustTrip curve is present, its yUnit must be Not_Applicable
        if (req.limitMaxDischarge.has_value() && req.limitMaxDischarge->powerMonitoringMustTrip.has_value()) {
            if (!validate_yunit(DERControlEnum::PowerMonitoringMustTrip,
                                req.limitMaxDischarge->powerMonitoringMustTrip.value())) {
                return false;
            }
        }
        return req.limitMaxDischarge.has_value();
    case DERControlEnum::FreqDroop:
        return req.freqDroop.has_value();
    case DERControlEnum::EnterService:
        return req.enterService.has_value();
    case DERControlEnum::Gradients:
        return req.gradient.has_value();
    // All curve-based types require the curve field + R04.FR.50-55 yUnit validation
    case DERControlEnum::FreqWatt:
    case DERControlEnum::HFMustTrip:
    case DERControlEnum::HFMayTrip:
    case DERControlEnum::HVMustTrip:
    case DERControlEnum::HVMomCess:
    case DERControlEnum::HVMayTrip:
    case DERControlEnum::LFMustTrip:
    case DERControlEnum::LVMustTrip:
    case DERControlEnum::LVMomCess:
    case DERControlEnum::LVMayTrip:
    case DERControlEnum::PowerMonitoringMustTrip:
    case DERControlEnum::VoltVar:
    case DERControlEnum::VoltWatt:
    case DERControlEnum::WattPF:
    case DERControlEnum::WattVar:
        return req.curve.has_value() && validate_yunit(req.controlType, req.curve.value());
    }
    return false;
}

void DERControl::handle_set_der_control(ocpp::Call<SetDERControlRequest> call) {
    const auto& request = call.msg;
    SetDERControlResponse response;
    const auto control_type_str = v2::conversions::dercontrol_enum_to_string(request.controlType);

    // R04.FR.01: Check if controlType is supported
    if (!this->is_control_type_supported(request.controlType)) {
        response.status = DERControlStatusEnum::NotSupported;
        ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.16-17: Validate control fields match controlType
    if (!this->validate_control_fields(request)) {
        response.status = DERControlStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "ControlFieldMismatch";
        ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.13: Default controls cannot have startTime or duration
    if (request.isDefault && has_start_time_or_duration(request)) {
        response.status = DERControlStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "DefaultWithStartTimeOrDuration";
        ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.15: EnterService and Gradients can only be default (not scheduled)
    if (!request.isDefault &&
        (request.controlType == DERControlEnum::EnterService || request.controlType == DERControlEnum::Gradients)) {
        response.status = DERControlStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "EnterServiceOrGradientsMustBeDefault";
        ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // Bound DER_CONTROLS table growth. A compromised CSMS could otherwise issue
    // SetDERControl with ever-unique controlIds and fill the DB. An UPDATE of an
    // existing row does not increase row count, so it is always allowed.
    if (this->context.database_handler.count_der_controls() >= MAX_DER_CONTROLS &&
        !this->context.database_handler.get_der_control(request.controlId.get()).has_value()) {
        EVLOG_warning << "DER control table at cap (" << MAX_DER_CONTROLS
                      << "), rejecting new controlId=" << request.controlId.get();
        response.status = DERControlStatusEnum::Rejected;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "DERControlLimitExceeded";
        ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    const int32_t priority = get_priority_from_request(request);
    const auto start_time = get_start_time_from_request(request);
    const auto duration = get_duration_from_request(request);

    std::vector<CiString<36>> superseded_ids;
    bool new_is_superseded = false;

    // Supersede branching follows R04.FR.06 vs R04.FR.07. FR.06 covers the case
    // where the existing control is "not yet active" and mandates an immediate
    // flip even when the new control's startTime is later than the existing
    // one's. FR.07 covers the case where the existing control is "already
    // active" and defers the flip until startTime of the new control. So the
    // discriminator is the EXISTING control's active state, not the new one's
    // startTime. The deferred_supersede_target slot below is populated only on
    // the FR.07 path.
    std::optional<std::string> deferred_supersede_target;
    // Pending immediate-supersede side effects. We collect into a local list
    // first and only apply them after the loop, because a later iteration may
    // discover that the new control is itself superseded by a higher-priority
    // existing row (R04.FR.08). In that case the new control will never run,
    // so it must not flag any other rows as superseded by it.
    std::vector<std::string> pending_immediate_supersedes;
    const auto now = ocpp::DateTime();
    const bool has_future_start =
        !request.isDefault && start_time.has_value() && ocpp::DateTime(start_time.value()) > now;
    // R04.FR.20: immediate-start only applies to non-default scheduled controls
    // whose startTime is at-or-before now. The self-superseded case (R04.FR.08)
    // is evaluated below during the supersede scan and cancels this flag.
    const bool start_time_has_arrived =
        !request.isDefault && start_time.has_value() && ocpp::DateTime(start_time.value()) <= now;

    // Everything that touches DER_CONTROLS rows on behalf of this SetDERControl runs
    // inside a single transaction so the CSMS-handler thread and the scheduled-check
    // timer thread can never see a half-mutated snapshot.
    auto transaction = this->context.database_handler.begin_transaction();

    // Query existing controls of the same type and isDefault
    auto existing_jsons = this->context.database_handler.get_der_controls_matching_criteria(
        std::optional<bool>(request.isDefault), std::optional<std::string>(control_type_str), std::nullopt);

    // R04.FR.02-08: Handle priority-based superseding for existing controls
    for (const auto& existing_json : existing_jsons) {
        try {
            auto existing = json::parse(existing_json);
            auto existing_id = existing.at("controlId").get<std::string>();
            auto existing_priority = existing.at("priority").get<int32_t>();
            // Strict at(...).get<bool>(): we control the writer so both flags are
            // always present and bool-typed. A missing or wrong-typed value is
            // a sign of data corruption — let it throw and be logged by the
            // outer per-row catch instead of silently defaulting to false and
            // misclassifying the row.
            auto existing_is_superseded = existing.at("isSuperseded").get<bool>();
            auto existing_is_default = existing.at("isDefault").get<bool>();

            if (existing_id == request.controlId.get()) {
                continue; // Same controlId is an update, not a supersede
            }
            if (existing_is_superseded) {
                continue; // Already out of play; can neither supersede nor be superseded
            }

            // Determine whether `existing` is currently active. Defaults are not
            // "scheduled" in the time-windowed sense, so they never qualify for
            // deferred supersede — a new default with strictly lower priority value
            // immediately replaces an existing default.
            bool existing_is_currently_active = false;
            if (!existing_is_default && existing.contains("startTime")) {
                ocpp::DateTime existing_start(existing.at("startTime").get<std::string>());
                if (existing_start.to_time_point() <= now.to_time_point()) {
                    // Strict type check before .get<float>(): a malformed persisted
                    // duration would otherwise throw and be swallowed by the outer
                    // per-row catch, silently skipping supersede analysis for this
                    // row. Missing or non-numeric duration is treated as indefinite
                    // (the row is still active until explicitly cleared).
                    const auto duration_it = existing.find("duration");
                    if (duration_it == existing.end() || !duration_it->is_number()) {
                        existing_is_currently_active = true;
                    } else {
                        const auto existing_duration = duration_it->get<float>();
                        if (std::isfinite(existing_duration)) {
                            const auto expiry = ocpp::DateTime(
                                existing_start.to_time_point() +
                                std::chrono::milliseconds(static_cast<int64_t>(existing_duration * 1000.0F)));
                            existing_is_currently_active = expiry.to_time_point() > now.to_time_point();
                        } else {
                            // Non-finite duration → indefinite.
                            existing_is_currently_active = true;
                        }
                    }
                }
            }

            // Lower priority value overrules a higher one (R04.FR.03, R04.FR.06).
            // Ties do not supersede, spec requires strictly lower value to overrule.
            if (priority < existing_priority) {
                if (existing_is_currently_active && has_future_start) {
                    // R04.FR.07: existing is already active; remain active until
                    // startTime of the new control, at which point the deferred
                    // flip fires. The response omits supersededIds (TC_R_105
                    // step 10); CSMS is told via NotifyDERStartStop later.
                    if (!deferred_supersede_target.has_value()) {
                        deferred_supersede_target = existing_id;
                    }
                } else {
                    // R04.FR.06: existing is not yet active (or, for default vs
                    // default, R04.FR.03 — existing is a default and never has a
                    // startTime). Flip is deferred until after the loop so it
                    // can be cancelled if a higher-priority existing row
                    // supersedes the new control (R04.FR.08).
                    pending_immediate_supersedes.emplace_back(existing_id);
                }
            } else if (priority > existing_priority) {
                // R04.FR.08: existing has strictly higher priority than new, the new
                // scheduled control goes in as already-superseded; response's
                // supersededIds contains the NEW controlId. Existing keeps running.
                new_is_superseded = true;
            }
        } catch (const json::exception& e) {
            EVLOG_warning << "Failed to parse existing DER control JSON: " << e.what();
        }
    }

    // R04.FR.08: if the new control is itself superseded by a higher-priority
    // existing row, it cannot supersede anything else. Drop both immediate and
    // deferred supersede side effects so we don't persist a zombie row whose
    // IS_SUPERSEDED=1 yet whose PENDING_SUPERSEDE_ID still points at another
    // row, and so we don't flag rows as superseded by a control that will
    // never actually take effect.
    if (new_is_superseded) {
        pending_immediate_supersedes.clear();
        deferred_supersede_target.reset();
    } else {
        for (const auto& existing_id : pending_immediate_supersedes) {
            this->context.database_handler.update_der_control_superseded(existing_id, true);
            superseded_ids.emplace_back(existing_id);
        }
    }

    // Serialize the full request as the control JSON for storage
    json control_json;
    control_json["controlId"] = request.controlId.get();
    control_json["controlType"] = control_type_str;
    control_json["isDefault"] = request.isDefault;
    control_json["priority"] = priority;
    control_json["isSuperseded"] = new_is_superseded;
    if (start_time.has_value()) {
        control_json["startTime"] = start_time.value();
    }
    if (duration.has_value()) {
        control_json["duration"] = duration.value();
    }
    // Store the original request payload
    control_json["request"] = json(request);

    // Persist to database
    this->context.database_handler.insert_or_update_der_control(request.controlId.get(), request.isDefault,
                                                                control_type_str, new_is_superseded, priority,
                                                                start_time, duration, control_json.dump());

    // PENDING_SUPERSEDE_ID is preserved by the UPSERT, so we have to be
    // explicit on every Set: either point it at the freshly computed deferred
    // target, or clear any stale value left by a previous version of this row.
    if (deferred_supersede_target.has_value()) {
        this->context.database_handler.set_der_control_pending_supersede(request.controlId.get(),
                                                                         deferred_supersede_target.value());
    } else {
        this->context.database_handler.clear_der_control_pending_supersede(request.controlId.get());
    }

    // R04.FR.20: if the immediate-start branch below will fire, flag the row
    // STARTED_NOTIFIED inside this same transaction. That way a crash between
    // commit and dispatch loses a start notification (recoverable via CSMS
    // retry) instead of leaving the row with STARTED_NOTIFIED=0 after CSMS has
    // already observed the notify, which would cause the 30 s sweep to
    // duplicate it.
    const bool will_notify_immediate_start = start_time_has_arrived && !new_is_superseded;
    if (will_notify_immediate_start) {
        this->context.database_handler.mark_der_control_started_notified(request.controlId.get());
    }

    transaction->commit();

    response.status = DERControlStatusEnum::Accepted;
    if (new_is_superseded) {
        // R04.FR.08: the new control is itself superseded, echo its own id.
        response.supersededIds = std::vector<CiString<36>>{request.controlId};
    } else if (!superseded_ids.empty()) {
        response.supersededIds = superseded_ids;
    }

    ocpp::CallResult<SetDERControlResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    // R04.FR.20: post-commit dispatch. STARTED_NOTIFIED has already been written
    // inside the transaction, so a crash here loses at most one start notify.
    if (will_notify_immediate_start) {
        std::optional<std::vector<CiString<36>>> superseded_opt;
        if (!superseded_ids.empty()) {
            superseded_opt = superseded_ids;
        }
        this->send_notify_start_stop(request.controlId, true, ocpp::DateTime(), superseded_opt);
    }
}

void DERControl::handle_get_der_control(ocpp::Call<GetDERControlRequest> call) {
    const auto& request = call.msg;
    GetDERControlResponse response;

    // R04.FR.36: If controlType specified and not supported → NotSupported
    if (request.controlType.has_value() && !this->is_control_type_supported(request.controlType.value())) {
        response.status = DERControlStatusEnum::NotSupported;
        ocpp::CallResult<GetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // Build filter criteria from request
    std::optional<std::string> control_type_filter;
    if (request.controlType.has_value()) {
        control_type_filter = v2::conversions::dercontrol_enum_to_string(request.controlType.value());
    }
    std::optional<std::string> control_id_filter;
    if (request.controlId.has_value()) {
        control_id_filter = request.controlId->get();
    }

    // Wrap the DB read in a transaction so the GET handler observes a
    // consistent snapshot even if the scheduled-check timer is running on
    // another thread. SQLite's default isolation already prevents readers
    // from seeing uncommitted writes, but an explicit transaction also
    // pins the snapshot for any future read added to this handler.
    std::vector<std::string> matching;
    {
        auto transaction = this->context.database_handler.begin_transaction();
        matching = this->context.database_handler.get_der_controls_matching_criteria(
            request.isDefault, control_type_filter, control_id_filter);
        transaction->commit();
    }

    // R04.FR.30: No matching controls → NotFound
    if (matching.empty()) {
        response.status = DERControlStatusEnum::NotFound;
        ocpp::CallResult<GetDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.33-35, R04.FR.37: Return Accepted and send ReportDERControl
    response.status = DERControlStatusEnum::Accepted;
    ocpp::CallResult<GetDERControlResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);

    // Send report with matching controls
    this->send_report(request.requestId, matching);
}

void DERControl::handle_clear_der_control(ocpp::Call<ClearDERControlRequest> call) {
    const auto& request = call.msg;
    ClearDERControlResponse response;

    // R04.FR.46: If controlId is specified, clear that specific control, but
    // R04.FR.42 requires the caller-supplied isDefault to match too. A controlId
    // that exists only under the opposite isDefault must return NotFound.
    if (request.controlId.has_value()) {
        bool deleted = this->context.database_handler.delete_der_control_by_id_and_default(request.controlId->get(),
                                                                                           request.isDefault);
        response.status = deleted ? DERControlStatusEnum::Accepted : DERControlStatusEnum::NotFound;
        ocpp::CallResult<ClearDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.43: If controlType specified and not supported (no controlId) → NotSupported
    if (request.controlType.has_value() && !this->is_control_type_supported(request.controlType.value())) {
        response.status = DERControlStatusEnum::NotSupported;
        ocpp::CallResult<ClearDERControlResponse> call_result(response, call.uniqueId);
        this->context.message_dispatcher.dispatch_call_result(call_result);
        return;
    }

    // R04.FR.44: No controlType, no controlId → clear all matching isDefault
    // R04.FR.45: controlType set, no controlId → clear by type and isDefault
    std::optional<std::string> control_type_filter;
    if (request.controlType.has_value()) {
        control_type_filter = v2::conversions::dercontrol_enum_to_string(request.controlType.value());
    }

    int deleted_count =
        this->context.database_handler.delete_der_controls_matching_criteria(request.isDefault, control_type_filter);

    // R04.FR.44 (no controlType, no controlId) is unconditionally Accepted, even
    // when there is nothing to delete. NotFound is reserved for FR.41
    // (controlType-with-no-match) and FR.42 (controlId-with-no-match).
    if (control_type_filter.has_value()) {
        response.status = (deleted_count > 0) ? DERControlStatusEnum::Accepted : DERControlStatusEnum::NotFound;
    } else {
        response.status = DERControlStatusEnum::Accepted;
    }

    ocpp::CallResult<ClearDERControlResponse> call_result(response, call.uniqueId);
    this->context.message_dispatcher.dispatch_call_result(call_result);
}

void DERControl::send_notify_start_stop(const CiString<36>& control_id, bool started, const ocpp::DateTime& timestamp,
                                        const std::optional<std::vector<CiString<36>>>& superseded_ids) {
    NotifyDERStartStopRequest req;
    req.controlId = control_id;
    req.started = started;
    req.timestamp = timestamp;
    req.supersededIds = superseded_ids;

    ocpp::Call<NotifyDERStartStopRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call, false);
}

void DERControl::check_scheduled_controls() {
    // Notifications are queued inside the transaction and flushed only after
    // commit. If the transaction throws or rolls back we discard the queue, so
    // CSMS never observes a notification whose DB change was rolled back. A
    // per-notification dispatcher throw after commit is swallowed; the DB state
    // is already durable so no subsequent sweep will re-emit (STARTED_NOTIFIED
    // flags and expiry-deletes are already persisted).
    //
    // Reboot replay: FR.20 / FR.22 specify `timestamp = current time`, i.e. the
    // moment the notification is dispatched, not the moment the underlying
    // startTime / expiry event occurred. A row whose startTime elapsed during
    // downtime fires `started=true` on the first post-boot sweep with
    // `timestamp = now`; a row whose startTime AND expiry both elapsed during
    // downtime fires `started=true` followed by `started=false` back-to-back
    // with identical timestamps (pair semantics preserved because this loop
    // runs before the expiry loop below and both queue into the same
    // post-commit dispatch vector). No grace window is applied, all overdue
    // transitions are replayed. Spec-literal; product decision recorded in
    // docs/reviews/ocpp-der-open-questions.md §3.
    std::vector<NotifyDERStartStopRequest> pending_notifications;

    // Outer try ensures DB-level failures never escape into the asio timer thread.
    try {
        // Scan + DB updates run as a single atomic pass so concurrent CSMS
        // SetDERControl / ClearDERControl calls can't see a half-swept state.
        auto transaction = this->context.database_handler.begin_transaction();

        auto now = ocpp::DateTime();

        // R04.FR.07: activate any deferred supersedes whose new-control startTime
        // has arrived. Flip the target to isSuperseded, clear the pending pointer,
        // and queue the start notification for post-commit dispatch. Spec FR.21
        // mandates only the started=true with supersededIds for the new control;
        // it does NOT require a paired started=false on the existing control,
        // which is a pure supersede event. The existing control's NotifyDERStartStop
        // (started=false) is reserved for FR.22 genuine startTime+duration expiry.
        auto activations = this->context.database_handler.get_der_control_pending_supersede_activations(now);
        for (const auto& activation : activations) {
            this->context.database_handler.update_der_control_superseded(activation.existing_id, true);
            this->context.database_handler.clear_der_control_pending_supersede(activation.new_id);
            // The activation emits start-notification for the new control; flag it so
            // the R04.FR.20/21 pass below doesn't double-fire.
            this->context.database_handler.mark_der_control_started_notified(activation.new_id);

            NotifyDERStartStopRequest start_req;
            start_req.controlId = CiString<36>(activation.new_id);
            start_req.started = true;
            start_req.timestamp = now;
            start_req.supersededIds = std::optional<std::vector<CiString<36>>>{
                std::vector<CiString<36>>{CiString<36>(activation.existing_id)}};
            pending_notifications.push_back(std::move(start_req));
        }

        // R04.FR.20/21: queue NotifyDERStartStop(started=true) the first time the
        // timer observes a scheduled control whose startTime has arrived but for
        // which no start-notification has been sent yet. Flag the row so later
        // sweeps don't re-emit.
        auto to_start = this->context.database_handler.get_der_controls_needing_start_notify(now);
        for (const auto& control_id : to_start) {
            this->context.database_handler.mark_der_control_started_notified(control_id);
            NotifyDERStartStopRequest start_req;
            start_req.controlId = CiString<36>(control_id);
            start_req.started = true;
            start_req.timestamp = now;
            pending_notifications.push_back(std::move(start_req));
        }

        auto controls = this->context.database_handler.get_der_controls_matching_criteria(std::optional<bool>(false),
                                                                                          std::nullopt, std::nullopt);

        for (const auto& control_json_str : controls) {
            try {
                auto control = json::parse(control_json_str);
                auto control_id = control.at("controlId").get<std::string>();
                bool is_superseded = control.at("isSuperseded").get<bool>();

                if (is_superseded) {
                    continue; // Skip superseded controls
                }

                if (!control.contains("startTime") || !control.contains("duration")) {
                    continue; // No expiry possible without both fields
                }

                auto start_str = control.at("startTime").get<std::string>();
                auto duration_seconds = control.at("duration").get<float>();
                if (!std::isfinite(duration_seconds) || duration_seconds < 0.0F) {
                    continue;
                }
                auto start_time = ocpp::DateTime(start_str);

                // Calculate expiry: startTime + duration. duration_cast avoids the
                // undefined-on-overflow float-to-int cast; duration was already bounded
                // at insert time but we stay defensive on the persisted value too.
                auto start_tp = start_time.to_time_point();
                auto expiry_tp = start_tp + std::chrono::duration_cast<std::chrono::seconds>(
                                                std::chrono::duration<double>(static_cast<double>(duration_seconds)));
                auto expiry = ocpp::DateTime(expiry_tp);

                // R04.FR.10: Delete controls past startTime + duration
                // R04.FR.22: Queue NotifyDERStartStop with started=false for post-commit dispatch.
                if (expiry <= now) {
                    NotifyDERStartStopRequest stop_req;
                    stop_req.controlId = CiString<36>(control_id);
                    stop_req.started = false;
                    stop_req.timestamp = now;
                    pending_notifications.push_back(std::move(stop_req));

                    this->context.database_handler.delete_der_control(control_id);
                    // CSMS-controlled controlId: keep at DEBUG so routine expirations
                    // don't bleed into production INFO logs alongside curve data.
                    EVLOG_debug << "DER control " << control_id << " expired, deleted from database";
                }
            } catch (const std::exception& e) {
                // Broadened from json::exception: an unparseable startTime throws
                // TimePointParseException, which is not a json::exception. A single bad
                // row must not abort the rest of the sweep.
                EVLOG_warning << "Skipping malformed DER control row during scheduled check: " << e.what();
            }
        }

        transaction->commit();
    } catch (const std::exception& e) {
        // Anything before commit rolls the transaction back via the unique_ptr
        // destructor. Discard queued notifications so CSMS state stays in sync
        // with the DB.
        EVLOG_error << "DER scheduled-control check aborted: " << e.what();
        return;
    }

    // Post-commit dispatch: DB state is durable, notifications can now be
    // observed by CSMS without risk of DB rollback. A dispatcher throw per
    // notification is swallowed so the timer thread isn't torn down; remaining
    // notifications are still attempted.
    for (const auto& notification : pending_notifications) {
        try {
            ocpp::Call<NotifyDERStartStopRequest> call(notification);
            this->context.message_dispatcher.dispatch_call(call, false);
        } catch (const std::exception& e) {
            EVLOG_error << "DER scheduled-control notify dispatch failed: " << e.what();
        }
    }
}

namespace {

/// Maximum number of controls per ReportDERControl message. If more controls are
/// present, the report is split into multiple messages using the tbc flag.
constexpr size_t MAX_CONTROLS_PER_REPORT_MESSAGE = 10;

/// Build a ReportDERControlRequest populated with the given controls.
/// Each control JSON is parsed and sorted into the appropriate *Get vector.
ReportDERControlRequest build_report(int32_t request_id, const std::vector<std::string>& control_jsons, bool tbc) {
    ReportDERControlRequest report;
    report.requestId = request_id;

    std::vector<DERCurveGet> curves;
    std::vector<EnterServiceGet> enter_services;
    std::vector<FixedPFGet> fixed_pf_absorbs;
    std::vector<FixedPFGet> fixed_pf_injects;
    std::vector<FixedVarGet> fixed_vars;
    std::vector<FreqDroopGet> freq_droops;
    std::vector<GradientGet> gradients;
    std::vector<LimitMaxDischargeGet> limit_max_discharges;

    for (const auto& control_json_str : control_jsons) {
        try {
            auto stored = json::parse(control_json_str);
            auto control_id = stored.at("controlId").get<std::string>();
            auto control_type_str = stored.at("controlType").get<std::string>();
            bool is_default = stored.at("isDefault").get<bool>();
            bool is_superseded = stored.at("isSuperseded").get<bool>();
            auto control_type = v2::conversions::string_to_dercontrol_enum(control_type_str);
            const auto& request_json = stored.at("request");

            // Route each control into the appropriate *Get vector based on type
            switch (control_type) {
            case DERControlEnum::FixedPFAbsorb: {
                FixedPFGet get;
                get.fixedPF = request_json.at("fixedPFAbsorb").get<FixedPF>();
                get.id = control_id;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                fixed_pf_absorbs.push_back(get);
                break;
            }
            case DERControlEnum::FixedPFInject: {
                FixedPFGet get;
                get.fixedPF = request_json.at("fixedPFInject").get<FixedPF>();
                get.id = control_id;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                fixed_pf_injects.push_back(get);
                break;
            }
            case DERControlEnum::FixedVar: {
                FixedVarGet get;
                get.fixedVar = request_json.at("fixedVar").get<FixedVar>();
                get.id = control_id;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                fixed_vars.push_back(get);
                break;
            }
            case DERControlEnum::FreqDroop: {
                FreqDroopGet get;
                get.freqDroop = request_json.at("freqDroop").get<FreqDroop>();
                get.id = control_id;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                freq_droops.push_back(get);
                break;
            }
            case DERControlEnum::EnterService: {
                EnterServiceGet get;
                get.enterService = request_json.at("enterService").get<EnterService>();
                get.id = control_id;
                enter_services.push_back(get);
                break;
            }
            case DERControlEnum::Gradients: {
                GradientGet get;
                get.gradient = request_json.at("gradient").get<Gradient>();
                get.id = control_id;
                gradients.push_back(get);
                break;
            }
            case DERControlEnum::LimitMaxDischarge: {
                LimitMaxDischargeGet get;
                get.limitMaxDischarge = request_json.at("limitMaxDischarge").get<LimitMaxDischarge>();
                get.id = control_id;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                limit_max_discharges.push_back(get);
                break;
            }
            // All curve-based types
            case DERControlEnum::FreqWatt:
            case DERControlEnum::HFMustTrip:
            case DERControlEnum::HFMayTrip:
            case DERControlEnum::HVMustTrip:
            case DERControlEnum::HVMomCess:
            case DERControlEnum::HVMayTrip:
            case DERControlEnum::LFMustTrip:
            case DERControlEnum::LVMustTrip:
            case DERControlEnum::LVMomCess:
            case DERControlEnum::LVMayTrip:
            case DERControlEnum::PowerMonitoringMustTrip:
            case DERControlEnum::VoltVar:
            case DERControlEnum::VoltWatt:
            case DERControlEnum::WattPF:
            case DERControlEnum::WattVar: {
                DERCurveGet get;
                get.curve = request_json.at("curve").get<DERCurve>();
                get.id = control_id;
                get.curveType = control_type;
                get.isDefault = is_default;
                get.isSuperseded = is_superseded;
                curves.push_back(get);
                break;
            }
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Failed to parse stored DER control for report: " << e.what();
        }
    }

    if (!curves.empty()) {
        report.curve = curves;
    }
    if (!enter_services.empty()) {
        report.enterService = enter_services;
    }
    if (!fixed_pf_absorbs.empty()) {
        report.fixedPFAbsorb = fixed_pf_absorbs;
    }
    if (!fixed_pf_injects.empty()) {
        report.fixedPFInject = fixed_pf_injects;
    }
    if (!fixed_vars.empty()) {
        report.fixedVar = fixed_vars;
    }
    if (!freq_droops.empty()) {
        report.freqDroop = freq_droops;
    }
    if (!gradients.empty()) {
        report.gradient = gradients;
    }
    if (!limit_max_discharges.empty()) {
        report.limitMaxDischarge = limit_max_discharges;
    }

    // R04.FR.32: set tbc=true for all but the last message in a multi-message report
    if (tbc) {
        report.tbc = true;
    }

    return report;
}

} // anonymous namespace

void DERControl::send_report(int32_t request_id, const std::vector<std::string>& control_jsons) {
    // R04.FR.32: Split into multiple messages if we have more than MAX controls
    const size_t total = control_jsons.size();
    if (total == 0) {
        return;
    }

    size_t index = 0;
    while (index < total) {
        size_t chunk_end = std::min(index + MAX_CONTROLS_PER_REPORT_MESSAGE, total);
        bool is_last = (chunk_end == total);

        std::vector<std::string> chunk(control_jsons.begin() + static_cast<long>(index),
                                       control_jsons.begin() + static_cast<long>(chunk_end));

        // R04.FR.31: requestId set in every message; R04.FR.32: tbc=true except for last
        auto report = build_report(request_id, chunk, !is_last);

        ocpp::Call<ReportDERControlRequest> report_call(report);
        this->context.message_dispatcher.dispatch_call(report_call, false);

        index = chunk_end;
    }
}

} // namespace ocpp::v21
