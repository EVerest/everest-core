from datetime import datetime

from pydantic import BaseModel, Extra


class UnitCurrent(BaseModel, extra=Extra.forbid):
    DC: float | None
    L1: float | None
    L2: float | None
    L3: float | None
    N: float | None


class UnitVoltage(BaseModel, extra=Extra.forbid):
    DC: float | None
    L1: float | None
    L2: float | None
    L3: float | None


class UnitFrequency(BaseModel, extra=Extra.forbid):
    L1: float | None
    L2: float | None
    L3: float | None


class UnitPower(BaseModel, extra=Extra.forbid):
    total: float
    L1: float | None
    L2: float | None
    L3: float | None


class UnitEnergy(BaseModel, extra=Extra.forbid):
    total: float
    L1: float | None
    L2: float | None
    L3: float | None


class Powermeter(BaseModel, extra=Extra.forbid):
    current_A: UnitCurrent
    energy_Wh_export: UnitEnergy
    energy_Wh_import: UnitEnergy
    meter_id: str
    power_W: UnitPower
    timestamp: datetime
    voltage_V: UnitVoltage
