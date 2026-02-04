# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from dataclasses import dataclass
from datetime import datetime
from typing import Optional
import json


# === Unit types (from https://everest.github.io/nightly/reference/types/units.html) ===

@dataclass
class SignedMeterValue:
    """Representation of a signed meter value"""
    signed_meter_data: str
    signing_method: str
    encoding_method: str
    public_key: Optional[str] = None
    timestamp: Optional[datetime] = None

    @classmethod
    def from_dict(cls, data: dict) -> "SignedMeterValue":
        timestamp = None
        if data.get("timestamp"):
            timestamp = datetime.fromisoformat(data["timestamp"].replace("Z", "+00:00"))
        return cls(
            signed_meter_data=data["signed_meter_data"],
            signing_method=data["signing_method"],
            encoding_method=data["encoding_method"],
            public_key=data.get("public_key"),
            timestamp=timestamp,
        )

    def to_dict(self) -> dict:
        result = {
            "signed_meter_data": self.signed_meter_data,
            "signing_method": self.signing_method,
            "encoding_method": self.encoding_method,
        }
        if self.public_key is not None:
            result["public_key"] = self.public_key
        if self.timestamp is not None:
            result["timestamp"] = self.timestamp.isoformat()
        return result


@dataclass
class Current:
    """Current in Ampere"""
    DC: Optional[float] = None
    L1: Optional[float] = None
    L2: Optional[float] = None
    L3: Optional[float] = None
    N: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Current":
        return cls(
            DC=data.get("DC"),
            L1=data.get("L1"),
            L2=data.get("L2"),
            L3=data.get("L3"),
            N=data.get("N"),
        )

    def to_dict(self) -> dict:
        result = {}
        if self.DC is not None:
            result["DC"] = self.DC
        if self.L1 is not None:
            result["L1"] = self.L1
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        if self.N is not None:
            result["N"] = self.N
        return result


@dataclass
class Voltage:
    """Voltage in Volt"""
    DC: Optional[float] = None
    L1: Optional[float] = None
    L2: Optional[float] = None
    L3: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Voltage":
        return cls(
            DC=data.get("DC"),
            L1=data.get("L1"),
            L2=data.get("L2"),
            L3=data.get("L3"),
        )

    def to_dict(self) -> dict:
        result = {}
        if self.DC is not None:
            result["DC"] = self.DC
        if self.L1 is not None:
            result["L1"] = self.L1
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        return result


@dataclass
class Frequency:
    """AC only: Frequency in Hertz"""
    L1: float
    L2: Optional[float] = None
    L3: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Frequency":
        return cls(
            L1=data["L1"],
            L2=data.get("L2"),
            L3=data.get("L3"),
        )

    def to_dict(self) -> dict:
        result = {"L1": self.L1}
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        return result


@dataclass
class Power:
    """Instantaneous power in Watt."""
    total: float
    L1: Optional[float] = None
    L2: Optional[float] = None
    L3: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Power":
        return cls(
            total=data["total"],
            L1=data.get("L1"),
            L2=data.get("L2"),
            L3=data.get("L3"),
        )

    def to_dict(self) -> dict:
        result = {"total": self.total}
        if self.L1 is not None:
            result["L1"] = self.L1
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        return result


@dataclass
class Energy:
    """Energy in Wh."""
    total: float
    L1: Optional[float] = None
    L2: Optional[float] = None
    L3: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Energy":
        return cls(
            total=data["total"],
            L1=data.get("L1"),
            L2=data.get("L2"),
            L3=data.get("L3"),
        )

    def to_dict(self) -> dict:
        result = {"total": self.total}
        if self.L1 is not None:
            result["L1"] = self.L1
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        return result


@dataclass
class ReactivePower:
    """Reactive power VAR"""
    total: float
    L1: Optional[float] = None
    L2: Optional[float] = None
    L3: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "ReactivePower":
        return cls(
            total=data["total"],
            L1=data.get("L1"),
            L2=data.get("L2"),
            L3=data.get("L3"),
        )

    def to_dict(self) -> dict:
        result = {"total": self.total}
        if self.L1 is not None:
            result["L1"] = self.L1
        if self.L2 is not None:
            result["L2"] = self.L2
        if self.L3 is not None:
            result["L3"] = self.L3
        return result


@dataclass
class Temperature:
    """Temperature sensor reading"""
    name: Optional[str] = None
    value: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Temperature":
        return cls(
            name=data.get("name"),
            value=data.get("value"),
        )

    def to_dict(self) -> dict:
        result = {}
        if self.name is not None:
            result["name"] = self.name
        if self.value is not None:
            result["value"] = self.value
        return result

# === Powermeter types (from https://everest.github.io/nightly/reference/types/powermeter.html) ===

@dataclass
class Powermeter:
    """Measured dataset (AC or DC)"""
    timestamp: datetime
    energy_Wh_import: Energy
    meter_id: Optional[str] = None
    phase_seq_error: Optional[bool] = None
    energy_Wh_export: Optional[Energy] = None
    power_W: Optional[Power] = None
    voltage_V: Optional[Voltage] = None
    VAR: Optional[ReactivePower] = None
    current_A: Optional[Current] = None
    frequency_Hz: Optional[Frequency] = None
    temperatures: Optional[list[Temperature]] = None

    @classmethod
    def from_dict(cls, data: dict) -> "Powermeter":
        timestamp = datetime.fromisoformat(data["timestamp"].replace("Z", "+00:00"))
        return cls(
            timestamp=timestamp,
            energy_Wh_import=Energy.from_dict(data["energy_Wh_import"]),
            meter_id=data.get("meter_id"),
            phase_seq_error=data.get("phase_seq_error"),
            energy_Wh_export=Energy.from_dict(data["energy_Wh_export"]) if data.get("energy_Wh_export") else None,
            power_W=Power.from_dict(data["power_W"]) if data.get("power_W") else None,
            voltage_V=Voltage.from_dict(data["voltage_V"]) if data.get("voltage_V") else None,
            VAR=ReactivePower.from_dict(data["VAR"]) if data.get("VAR") else None,
            current_A=Current.from_dict(data["current_A"]) if data.get("current_A") else None,
            frequency_Hz=Frequency.from_dict(data["frequency_Hz"]) if data.get("frequency_Hz") else None,
            temperatures=[Temperature.from_dict(t) for t in data["temperatures"]] if data.get("temperatures") else None,
        )

    @classmethod
    def from_json(cls, json_str: str) -> "Powermeter":
        return cls.from_dict(json.loads(json_str))

    def to_dict(self) -> dict:
        result = {
            "timestamp": self.timestamp.isoformat(),
            "energy_Wh_import": self.energy_Wh_import.to_dict(),
        }
        if self.meter_id is not None:
            result["meter_id"] = self.meter_id
        if self.phase_seq_error is not None:
            result["phase_seq_error"] = self.phase_seq_error
        if self.energy_Wh_export is not None:
            result["energy_Wh_export"] = self.energy_Wh_export.to_dict()
        if self.power_W is not None:
            result["power_W"] = self.power_W.to_dict()
        if self.voltage_V is not None:
            result["voltage_V"] = self.voltage_V.to_dict()
        if self.VAR is not None:
            result["VAR"] = self.VAR.to_dict()
        if self.current_A is not None:
            result["current_A"] = self.current_A.to_dict()
        if self.frequency_Hz is not None:
            result["frequency_Hz"] = self.frequency_Hz.to_dict()
        if self.temperatures is not None:
            result["temperatures"] = [t.to_dict() for t in self.temperatures]
        return result


# === Energy types (from https://everest.github.io/nightly/reference/types/energy.html) ===

@dataclass
class NumberWithSource:
    """Simple number type with source information"""
    value: float
    source: str

    @classmethod
    def from_dict(cls, data: dict) -> "NumberWithSource":
        return cls(
            value=data["value"],
            source=data["source"],
        )

    def to_dict(self) -> dict:
        return {
            "value": self.value,
            "source": self.source,
        }


@dataclass
class IntegerWithSource:
    """Simple integer type with source information"""
    value: int
    source: str

    @classmethod
    def from_dict(cls, data: dict) -> "IntegerWithSource":
        return cls(
            value=data["value"],
            source=data["source"],
        )

    def to_dict(self) -> dict:
        return {
            "value": self.value,
            "source": self.source,
        }


@dataclass
class LimitsReq:
    """Energy flow limiting object request (Evses to EnergyManager)"""
    total_power_W: Optional[NumberWithSource] = None
    ac_max_current_A: Optional[NumberWithSource] = None
    ac_min_current_A: Optional[NumberWithSource] = None
    ac_max_phase_count: Optional[IntegerWithSource] = None
    ac_min_phase_count: Optional[IntegerWithSource] = None
    ac_supports_changing_phases_during_charging: Optional[bool] = None
    ac_number_of_active_phases: Optional[int] = None

    @classmethod
    def from_dict(cls, data: dict) -> "LimitsReq":
        return cls(
            total_power_W=NumberWithSource.from_dict(data["total_power_W"]) if data.get("total_power_W") else None,
            ac_max_current_A=NumberWithSource.from_dict(data["ac_max_current_A"]) if data.get("ac_max_current_A") else None,
            ac_min_current_A=NumberWithSource.from_dict(data["ac_min_current_A"]) if data.get("ac_min_current_A") else None,
            ac_max_phase_count=IntegerWithSource.from_dict(data["ac_max_phase_count"]) if data.get("ac_max_phase_count") else None,
            ac_min_phase_count=IntegerWithSource.from_dict(data["ac_min_phase_count"]) if data.get("ac_min_phase_count") else None,
            ac_supports_changing_phases_during_charging=data.get("ac_supports_changing_phases_during_charging"),
            ac_number_of_active_phases=data.get("ac_number_of_active_phases"),
        )

    def to_dict(self) -> dict:
        result = {}
        if self.total_power_W is not None:
            result["total_power_W"] = self.total_power_W.to_dict()
        if self.ac_max_current_A is not None:
            result["ac_max_current_A"] = self.ac_max_current_A.to_dict()
        if self.ac_min_current_A is not None:
            result["ac_min_current_A"] = self.ac_min_current_A.to_dict()
        if self.ac_max_phase_count is not None:
            result["ac_max_phase_count"] = self.ac_max_phase_count.to_dict()
        if self.ac_min_phase_count is not None:
            result["ac_min_phase_count"] = self.ac_min_phase_count.to_dict()
        if self.ac_supports_changing_phases_during_charging is not None:
            result["ac_supports_changing_phases_during_charging"] = self.ac_supports_changing_phases_during_charging
        if self.ac_number_of_active_phases is not None:
            result["ac_number_of_active_phases"] = self.ac_number_of_active_phases
        return result


@dataclass
class FrequencyWattPoint:
    """A point of a frequency-watt curve"""
    frequency_Hz: float
    total_power_W: float

    @classmethod
    def from_dict(cls, data: dict) -> "FrequencyWattPoint":
        return cls(
            frequency_Hz=data["frequency_Hz"],
            total_power_W=data["total_power_W"],
        )

    def to_dict(self) -> dict:
        return {
            "frequency_Hz": self.frequency_Hz,
            "total_power_W": self.total_power_W,
        }


@dataclass
class SetpointType:
    """Defines a setpoint target value for charging or discharging"""
    source: str
    priority: int
    ac_current_A: Optional[float] = None
    total_power_W: Optional[float] = None
    frequency_table: Optional[list[FrequencyWattPoint]] = None

    @classmethod
    def from_dict(cls, data: dict) -> "SetpointType":
        frequency_table = None
        if data.get("frequency_table"):
            frequency_table = [FrequencyWattPoint.from_dict(p) for p in data["frequency_table"]]
        return cls(
            source=data["source"],
            priority=data["priority"],
            ac_current_A=data.get("ac_current_A"),
            total_power_W=data.get("total_power_W"),
            frequency_table=frequency_table,
        )

    def to_dict(self) -> dict:
        result = {
            "source": self.source,
            "priority": self.priority,
        }
        if self.ac_current_A is not None:
            result["ac_current_A"] = self.ac_current_A
        if self.total_power_W is not None:
            result["total_power_W"] = self.total_power_W
        if self.frequency_table is not None:
            result["frequency_table"] = [p.to_dict() for p in self.frequency_table]
        return result


@dataclass
class ScheduleReqEntry:
    """One entry for the time series (request)"""
    timestamp: datetime
    limits_to_root: LimitsReq
    limits_to_leaves: LimitsReq
    conversion_efficiency: Optional[float] = None

    @classmethod
    def from_dict(cls, data: dict) -> "ScheduleReqEntry":
        timestamp = datetime.fromisoformat(data["timestamp"].replace("Z", "+00:00"))
        return cls(
            timestamp=timestamp,
            limits_to_root=LimitsReq.from_dict(data["limits_to_root"]),
            limits_to_leaves=LimitsReq.from_dict(data["limits_to_leaves"]),
            conversion_efficiency=data.get("conversion_efficiency"),
        )

    def to_dict(self) -> dict:
        result = {
            "timestamp": self.timestamp.isoformat(),
            "limits_to_root": self.limits_to_root.to_dict(),
            "limits_to_leaves": self.limits_to_leaves.to_dict(),
        }
        if self.conversion_efficiency is not None:
            result["conversion_efficiency"] = self.conversion_efficiency
        return result


@dataclass
class ScheduleSetpointEntry:
    """One entry for the time series (setpoint request)"""
    timestamp: datetime
    setpoint: Optional[SetpointType] = None

    @classmethod
    def from_dict(cls, data: dict) -> "ScheduleSetpointEntry":
        timestamp = datetime.fromisoformat(data["timestamp"].replace("Z", "+00:00"))
        return cls(
            timestamp=timestamp,
            setpoint=SetpointType.from_dict(data["setpoint"]) if data.get("setpoint") else None,
        )

    def to_dict(self) -> dict:
        result = {
            "timestamp": self.timestamp.isoformat(),
        }
        if self.setpoint is not None:
            result["setpoint"] = self.setpoint.to_dict()
        return result


@dataclass
class ExternalLimits:
    """External Limits data type"""
    schedule_import: list[ScheduleReqEntry]
    schedule_export: list[ScheduleReqEntry]
    schedule_setpoints: list[ScheduleSetpointEntry]

    @classmethod
    def from_dict(cls, data: dict) -> "ExternalLimits":
        return cls(
            schedule_import=[ScheduleReqEntry.from_dict(e) for e in data["schedule_import"]],
            schedule_export=[ScheduleReqEntry.from_dict(e) for e in data["schedule_export"]],
            schedule_setpoints=[ScheduleSetpointEntry.from_dict(e) for e in data["schedule_setpoints"]],
        )

    @classmethod
    def from_json(cls, json_str: str) -> "ExternalLimits":
        return cls.from_dict(json.loads(json_str))

    def to_dict(self) -> dict:
        return {
            "schedule_import": [e.to_dict() for e in self.schedule_import],
            "schedule_export": [e.to_dict() for e in self.schedule_export],
            "schedule_setpoints": [e.to_dict() for e in self.schedule_setpoints],
        }

    def to_json(self) -> str:
        return json.dumps(self.to_dict())