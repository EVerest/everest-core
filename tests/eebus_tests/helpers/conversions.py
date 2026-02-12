# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from .types import LimitsReq, ScheduleReqEntry, ExternalLimits, TotalPowerW
import datetime

def convert_limits(limits_in: dict) -> LimitsReq:
    """
    Converts dict to LimitsReq
    """
    if not isinstance(limits_in, dict):
        raise TypeError("limits_in is not a dict")
    limits = LimitsReq()
    if "total_power_W" in limits_in:
        total_power_W_in = limits_in["total_power_W"]
        if not isinstance(total_power_W_in, dict):
            raise TypeError("total_power_W is not a dict")
        if not "source" in total_power_W_in or total_power_W_in["source"] is None:
            raise ValueError("total_power_W source is not provided")
        if not "value" in total_power_W_in or total_power_W_in["value"] is None:
            raise ValueError("total_power_W value is not provided")
        total_power_W = TotalPowerW(
            source=total_power_W_in["source"],
            value=float(total_power_W_in["value"])
        )
        limits.total_power_W = total_power_W
    if "ac_max_current_A" in limits_in:
        limits.ac_max_current_A = limits_in["ac_max_current_A"]
    if "ac_min_current_A" in limits_in:
        limits.ac_min_current_A = limits_in["ac_min_current_A"]
    if "ac_max_phase_count" in limits_in:
        limits.ac_max_phase_count = limits_in["ac_max_phase_count"]
    if "ac_min_phase_count" in limits_in:
        limits.ac_min_phase_count = limits_in["ac_min_phase_count"]
    if "ac_supports_changing_phases_during_charging" in limits_in:
        limits.ac_supports_changing_phases_during_charging = limits_in["ac_supports_changing_phases_during_charging"]
    if "ac_number_of_active_phases" in limits_in:
        limits.ac_number_of_active_phases = limits_in["ac_number_of_active_phases"]
    return limits

def convert_schedule_req_entry_list(schedule_in: list) -> list[ScheduleReqEntry]:
    """
    Converts list of dicts to list of ScheduleReqEntry
    """
    if not isinstance(schedule_in, list):
        raise TypeError("schedule_in is not a list")
    schedule = []
    for schedule_entry_in in schedule_in:
        schedule_entry_out = ScheduleReqEntry(
            timestamp=datetime.datetime.fromisoformat(schedule_entry_in["timestamp"]),
            limits_to_root=LimitsReq(),
            limits_to_leaves=LimitsReq(),
            conversion_efficiency=None,
            prive_per_kwh=None
        )
        if "limits_to_root" in schedule_entry_in and schedule_entry_in["limits_to_root"] is not None:
            schedule_entry_out.limits_to_root = convert_limits(schedule_entry_in["limits_to_root"])
        if "limits_to_leaves" in schedule_entry_in and schedule_entry_in["limits_to_leaves"] is not None:
            schedule_entry_out.limits_to_leaves = convert_limits(schedule_entry_in["limits_to_leaves"])
        if "conversion_efficiency" in schedule_entry_in and schedule_entry_in["conversion_efficiency"] is not None:
            raise NotImplementedError("conversion_efficiency is not implemented")
        if "prive_per_kwh" in schedule_entry_in and schedule_entry_in["prive_per_kwh"] is not None:
            raise NotImplementedError("prive_per_kwh is not implemented")
        schedule.append(schedule_entry_out)
    return schedule

def convert_external_limits(args: dict) -> ExternalLimits:
    """
    Converts dict to ExternalLimits
    """
    limits: ExternalLimits = ExternalLimits()
    if not isinstance(args, dict) or "value" not in args or args["value"] is None:
        raise ValueError("No value provided")
    if "schedule_import" in args["value"] and args["value"]["schedule_import"] is not None:
        limits.schedule_import = convert_schedule_req_entry_list(args["value"]["schedule_import"])
    if "schedule_export" in args["value"] and args["value"]["schedule_export"] is not None:
        limits.schedule_export = convert_schedule_req_entry_list(args["value"]["schedule_export"])
    return limits
