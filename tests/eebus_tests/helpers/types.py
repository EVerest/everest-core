# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from dataclasses import dataclass, field
import datetime

@dataclass
class PricePerKWh:
    """
    EVerest type
    """
    timestamp: datetime.datetime
    value: float
    currency: str

@dataclass
class TotalPowerW:
    """
    EVerest type
    """
    source: str
    value: float

@dataclass
class LimitsReq:
    """
    EVerest type
    """
    total_power_W: TotalPowerW | None = None
    ac_max_current_A: float | None = None
    ac_min_current_A: float | None = None
    ac_max_phase_count: int | None = None
    ac_min_phase_count: int | None = None
    ac_supports_changing_phases_during_charging: bool | None = None
    ac_number_of_active_phases: int | None = None

@dataclass
class ScheduleReqEntry:
    """
    EVerest type

    timestamp won't be compared, since it is not possible to
    assume the exact timestamp that is used by the EEBUS module
    """
    timestamp: datetime.datetime = field(compare=False)
    limits_to_root: LimitsReq
    limits_to_leaves: LimitsReq
    conversion_efficiency: float | None = None
    prive_per_kwh: PricePerKWh | None = None

@dataclass
class ExternalLimits:
    """
    EVerest type
    """
    schedule_import: list[ScheduleReqEntry] | None = None
    schedule_export: list[ScheduleReqEntry] | None = None
