from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class SharedType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = ()
    INVERTER: _ClassVar[SharedType]
INVERTER: SharedType

class DeviceCategory(_message.Message):
    __slots__ = ()
    class Enum(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        UNKNOWN: _ClassVar[DeviceCategory.Enum]
        GRID_CONNECTION_HUB: _ClassVar[DeviceCategory.Enum]
        ENERGY_MANAGEMENT_SYSTEM: _ClassVar[DeviceCategory.Enum]
        E_MOBILITY: _ClassVar[DeviceCategory.Enum]
        HVAC: _ClassVar[DeviceCategory.Enum]
        INVERTER: _ClassVar[DeviceCategory.Enum]
        DOMESTIC_APPLIANCE: _ClassVar[DeviceCategory.Enum]
        METERING: _ClassVar[DeviceCategory.Enum]
    UNKNOWN: DeviceCategory.Enum
    GRID_CONNECTION_HUB: DeviceCategory.Enum
    ENERGY_MANAGEMENT_SYSTEM: DeviceCategory.Enum
    E_MOBILITY: DeviceCategory.Enum
    HVAC: DeviceCategory.Enum
    INVERTER: DeviceCategory.Enum
    DOMESTIC_APPLIANCE: DeviceCategory.Enum
    METERING: DeviceCategory.Enum
    def __init__(self) -> None: ...

class EntityType(_message.Message):
    __slots__ = ()
    class Enum(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        UNKNOWN: _ClassVar[EntityType.Enum]
        Battery: _ClassVar[EntityType.Enum]
        Compressor: _ClassVar[EntityType.Enum]
        DeviceInformation: _ClassVar[EntityType.Enum]
        DHWCircuit: _ClassVar[EntityType.Enum]
        DHWStorage: _ClassVar[EntityType.Enum]
        Dishwasher: _ClassVar[EntityType.Enum]
        Dryer: _ClassVar[EntityType.Enum]
        ElectricalImmersionHeater: _ClassVar[EntityType.Enum]
        Fan: _ClassVar[EntityType.Enum]
        GasHeatingAppliance: _ClassVar[EntityType.Enum]
        Generic: _ClassVar[EntityType.Enum]
        HeatingBufferStorage: _ClassVar[EntityType.Enum]
        HeatingCircuit: _ClassVar[EntityType.Enum]
        HeatingObject: _ClassVar[EntityType.Enum]
        HeatingZone: _ClassVar[EntityType.Enum]
        HeatPumpAppliance: _ClassVar[EntityType.Enum]
        HeatSinkCircuit: _ClassVar[EntityType.Enum]
        HeatSourceCircuit: _ClassVar[EntityType.Enum]
        HeatSourceUnit: _ClassVar[EntityType.Enum]
        HvacController: _ClassVar[EntityType.Enum]
        HvacRoom: _ClassVar[EntityType.Enum]
        InstantDHWHeater: _ClassVar[EntityType.Enum]
        Inverter: _ClassVar[EntityType.Enum]
        OilHeatingAppliance: _ClassVar[EntityType.Enum]
        Pump: _ClassVar[EntityType.Enum]
        RefrigerantCircuit: _ClassVar[EntityType.Enum]
        SmartEnergyAppliance: _ClassVar[EntityType.Enum]
        SolarDHWStorage: _ClassVar[EntityType.Enum]
        SolarThermalCircuit: _ClassVar[EntityType.Enum]
        SubMeterElectricity: _ClassVar[EntityType.Enum]
        TemperatureSensor: _ClassVar[EntityType.Enum]
        Washer: _ClassVar[EntityType.Enum]
        BatterySystem: _ClassVar[EntityType.Enum]
        ElectricityGenerationSystem: _ClassVar[EntityType.Enum]
        ElectricityStorageSystem: _ClassVar[EntityType.Enum]
        GridConnectionPointOfPremises: _ClassVar[EntityType.Enum]
        Household: _ClassVar[EntityType.Enum]
        PVSystem: _ClassVar[EntityType.Enum]
        EV: _ClassVar[EntityType.Enum]
        EVSE: _ClassVar[EntityType.Enum]
        ChargingOutlet: _ClassVar[EntityType.Enum]
        CEM: _ClassVar[EntityType.Enum]
        PV: _ClassVar[EntityType.Enum]
        PVESHybrid: _ClassVar[EntityType.Enum]
        ElectricalStorage: _ClassVar[EntityType.Enum]
        PVString: _ClassVar[EntityType.Enum]
        GridGuard: _ClassVar[EntityType.Enum]
        ControllableSystem: _ClassVar[EntityType.Enum]
    UNKNOWN: EntityType.Enum
    Battery: EntityType.Enum
    Compressor: EntityType.Enum
    DeviceInformation: EntityType.Enum
    DHWCircuit: EntityType.Enum
    DHWStorage: EntityType.Enum
    Dishwasher: EntityType.Enum
    Dryer: EntityType.Enum
    ElectricalImmersionHeater: EntityType.Enum
    Fan: EntityType.Enum
    GasHeatingAppliance: EntityType.Enum
    Generic: EntityType.Enum
    HeatingBufferStorage: EntityType.Enum
    HeatingCircuit: EntityType.Enum
    HeatingObject: EntityType.Enum
    HeatingZone: EntityType.Enum
    HeatPumpAppliance: EntityType.Enum
    HeatSinkCircuit: EntityType.Enum
    HeatSourceCircuit: EntityType.Enum
    HeatSourceUnit: EntityType.Enum
    HvacController: EntityType.Enum
    HvacRoom: EntityType.Enum
    InstantDHWHeater: EntityType.Enum
    Inverter: EntityType.Enum
    OilHeatingAppliance: EntityType.Enum
    Pump: EntityType.Enum
    RefrigerantCircuit: EntityType.Enum
    SmartEnergyAppliance: EntityType.Enum
    SolarDHWStorage: EntityType.Enum
    SolarThermalCircuit: EntityType.Enum
    SubMeterElectricity: EntityType.Enum
    TemperatureSensor: EntityType.Enum
    Washer: EntityType.Enum
    BatterySystem: EntityType.Enum
    ElectricityGenerationSystem: EntityType.Enum
    ElectricityStorageSystem: EntityType.Enum
    GridConnectionPointOfPremises: EntityType.Enum
    Household: EntityType.Enum
    PVSystem: EntityType.Enum
    EV: EntityType.Enum
    EVSE: EntityType.Enum
    ChargingOutlet: EntityType.Enum
    CEM: EntityType.Enum
    PV: EntityType.Enum
    PVESHybrid: EntityType.Enum
    ElectricalStorage: EntityType.Enum
    PVString: EntityType.Enum
    GridGuard: EntityType.Enum
    ControllableSystem: EntityType.Enum
    def __init__(self) -> None: ...

class DeviceType(_message.Message):
    __slots__ = ()
    class Enum(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        UNKNOWN: _ClassVar[DeviceType.Enum]
        DISHWASHER: _ClassVar[DeviceType.Enum]
        DRYER: _ClassVar[DeviceType.Enum]
        ENVIRONMENT_SENSOR: _ClassVar[DeviceType.Enum]
        GENERIC: _ClassVar[DeviceType.Enum]
        HEAT_GENERATION_SYSTEM: _ClassVar[DeviceType.Enum]
        HEAT_SINK_SYSTEM: _ClassVar[DeviceType.Enum]
        HEAT_STORAGE_SYSTEM: _ClassVar[DeviceType.Enum]
        HVAC_CONTROLLER: _ClassVar[DeviceType.Enum]
        SUBMETER: _ClassVar[DeviceType.Enum]
        WASHER: _ClassVar[DeviceType.Enum]
        ELECTRICITY_SUPPLY_SYSTEM: _ClassVar[DeviceType.Enum]
        ENERGY_MANAGEMENT_SYSTEM: _ClassVar[DeviceType.Enum]
        INVERTER: _ClassVar[DeviceType.Enum]
        CHARGING_STATION: _ClassVar[DeviceType.Enum]
    UNKNOWN: DeviceType.Enum
    DISHWASHER: DeviceType.Enum
    DRYER: DeviceType.Enum
    ENVIRONMENT_SENSOR: DeviceType.Enum
    GENERIC: DeviceType.Enum
    HEAT_GENERATION_SYSTEM: DeviceType.Enum
    HEAT_SINK_SYSTEM: DeviceType.Enum
    HEAT_STORAGE_SYSTEM: DeviceType.Enum
    HVAC_CONTROLLER: DeviceType.Enum
    SUBMETER: DeviceType.Enum
    WASHER: DeviceType.Enum
    ELECTRICITY_SUPPLY_SYSTEM: DeviceType.Enum
    ENERGY_MANAGEMENT_SYSTEM: DeviceType.Enum
    INVERTER: DeviceType.Enum
    CHARGING_STATION: DeviceType.Enum
    def __init__(self) -> None: ...

class UseCase(_message.Message):
    __slots__ = ("actor", "name")
    class ActorType(_message.Message):
        __slots__ = ()
        class Enum(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
            __slots__ = ()
            UNKNOWN: _ClassVar[UseCase.ActorType.Enum]
            Battery: _ClassVar[UseCase.ActorType.Enum]
            BatterySystem: _ClassVar[UseCase.ActorType.Enum]
            CEM: _ClassVar[UseCase.ActorType.Enum]
            ConfigurationAppliance: _ClassVar[UseCase.ActorType.Enum]
            Compressor: _ClassVar[UseCase.ActorType.Enum]
            ControllableSystem: _ClassVar[UseCase.ActorType.Enum]
            DHWCircuit: _ClassVar[UseCase.ActorType.Enum]
            EnergyBroker: _ClassVar[UseCase.ActorType.Enum]
            EnergyConsumer: _ClassVar[UseCase.ActorType.Enum]
            EnergyGuard: _ClassVar[UseCase.ActorType.Enum]
            EVSE: _ClassVar[UseCase.ActorType.Enum]
            EV: _ClassVar[UseCase.ActorType.Enum]
            GridConnectionPoint: _ClassVar[UseCase.ActorType.Enum]
            HeatPump: _ClassVar[UseCase.ActorType.Enum]
            HeatingCircuit: _ClassVar[UseCase.ActorType.Enum]
            HeatingZone: _ClassVar[UseCase.ActorType.Enum]
            HVACRoom: _ClassVar[UseCase.ActorType.Enum]
            Inverter: _ClassVar[UseCase.ActorType.Enum]
            MonitoredUnit: _ClassVar[UseCase.ActorType.Enum]
            MonitoringAppliance: _ClassVar[UseCase.ActorType.Enum]
            OutdoorTemperatureSensor: _ClassVar[UseCase.ActorType.Enum]
            PVString: _ClassVar[UseCase.ActorType.Enum]
            PVSystem: _ClassVar[UseCase.ActorType.Enum]
            SmartAppliance: _ClassVar[UseCase.ActorType.Enum]
            VisualizationAppliance: _ClassVar[UseCase.ActorType.Enum]
        UNKNOWN: UseCase.ActorType.Enum
        Battery: UseCase.ActorType.Enum
        BatterySystem: UseCase.ActorType.Enum
        CEM: UseCase.ActorType.Enum
        ConfigurationAppliance: UseCase.ActorType.Enum
        Compressor: UseCase.ActorType.Enum
        ControllableSystem: UseCase.ActorType.Enum
        DHWCircuit: UseCase.ActorType.Enum
        EnergyBroker: UseCase.ActorType.Enum
        EnergyConsumer: UseCase.ActorType.Enum
        EnergyGuard: UseCase.ActorType.Enum
        EVSE: UseCase.ActorType.Enum
        EV: UseCase.ActorType.Enum
        GridConnectionPoint: UseCase.ActorType.Enum
        HeatPump: UseCase.ActorType.Enum
        HeatingCircuit: UseCase.ActorType.Enum
        HeatingZone: UseCase.ActorType.Enum
        HVACRoom: UseCase.ActorType.Enum
        Inverter: UseCase.ActorType.Enum
        MonitoredUnit: UseCase.ActorType.Enum
        MonitoringAppliance: UseCase.ActorType.Enum
        OutdoorTemperatureSensor: UseCase.ActorType.Enum
        PVString: UseCase.ActorType.Enum
        PVSystem: UseCase.ActorType.Enum
        SmartAppliance: UseCase.ActorType.Enum
        VisualizationAppliance: UseCase.ActorType.Enum
        def __init__(self) -> None: ...
    class NameType(_message.Message):
        __slots__ = ()
        class Enum(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
            __slots__ = ()
            UNKNOWN: _ClassVar[UseCase.NameType.Enum]
            configurationOfDhwSystemFunction: _ClassVar[UseCase.NameType.Enum]
            configurationOfDhwTemperature: _ClassVar[UseCase.NameType.Enum]
            configurationOfRoomCoolingSystemFunction: _ClassVar[UseCase.NameType.Enum]
            configurationOfRoomCoolingTemperature: _ClassVar[UseCase.NameType.Enum]
            configurationOfRoomHeatingSystemFunction: _ClassVar[UseCase.NameType.Enum]
            configurationOfRoomHeatingTemperature: _ClassVar[UseCase.NameType.Enum]
            controlOfBattery: _ClassVar[UseCase.NameType.Enum]
            coordinatedEvCharging: _ClassVar[UseCase.NameType.Enum]
            evChargingSummary: _ClassVar[UseCase.NameType.Enum]
            evCommissioningAndConfiguration: _ClassVar[UseCase.NameType.Enum]
            evseCommissioningAndConfiguration: _ClassVar[UseCase.NameType.Enum]
            evStateOfCharge: _ClassVar[UseCase.NameType.Enum]
            flexibleLoad: _ClassVar[UseCase.NameType.Enum]
            flexibleStartForWhiteGoods: _ClassVar[UseCase.NameType.Enum]
            limitationOfPowerConsumption: _ClassVar[UseCase.NameType.Enum]
            limitationOfPowerProduction: _ClassVar[UseCase.NameType.Enum]
            incentiveTableBasedPowerConsumptionManagement: _ClassVar[UseCase.NameType.Enum]
            measurementOfElectricityDuringEvCharging: _ClassVar[UseCase.NameType.Enum]
            monitoringAndControlOfSmartGridReadyConditions: _ClassVar[UseCase.NameType.Enum]
            monitoringOfBattery: _ClassVar[UseCase.NameType.Enum]
            monitoringOfDhwSystemFunction: _ClassVar[UseCase.NameType.Enum]
            monitoringOfDhwTemperature: _ClassVar[UseCase.NameType.Enum]
            monitoringOfGridConnectionPoint: _ClassVar[UseCase.NameType.Enum]
            monitoringOfInverter: _ClassVar[UseCase.NameType.Enum]
            monitoringOfOutdoorTemperature: _ClassVar[UseCase.NameType.Enum]
            monitoringOfPowerConsumption: _ClassVar[UseCase.NameType.Enum]
            monitoringOfPvString: _ClassVar[UseCase.NameType.Enum]
            monitoringOfRoomCoolingSystemFunction: _ClassVar[UseCase.NameType.Enum]
            monitoringOfRoomHeatingSystemFunction: _ClassVar[UseCase.NameType.Enum]
            monitoringOfRoomTemperature: _ClassVar[UseCase.NameType.Enum]
            optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility: _ClassVar[UseCase.NameType.Enum]
            optimizationOfSelfConsumptionDuringEvCharging: _ClassVar[UseCase.NameType.Enum]
            overloadProtectionByEvChargingCurrentCurtailment: _ClassVar[UseCase.NameType.Enum]
            visualizationOfAggregatedBatteryData: _ClassVar[UseCase.NameType.Enum]
            visualizationOfAggregatedPhotovoltaicData: _ClassVar[UseCase.NameType.Enum]
            visualizationOfHeatingAreaName: _ClassVar[UseCase.NameType.Enum]
        UNKNOWN: UseCase.NameType.Enum
        configurationOfDhwSystemFunction: UseCase.NameType.Enum
        configurationOfDhwTemperature: UseCase.NameType.Enum
        configurationOfRoomCoolingSystemFunction: UseCase.NameType.Enum
        configurationOfRoomCoolingTemperature: UseCase.NameType.Enum
        configurationOfRoomHeatingSystemFunction: UseCase.NameType.Enum
        configurationOfRoomHeatingTemperature: UseCase.NameType.Enum
        controlOfBattery: UseCase.NameType.Enum
        coordinatedEvCharging: UseCase.NameType.Enum
        evChargingSummary: UseCase.NameType.Enum
        evCommissioningAndConfiguration: UseCase.NameType.Enum
        evseCommissioningAndConfiguration: UseCase.NameType.Enum
        evStateOfCharge: UseCase.NameType.Enum
        flexibleLoad: UseCase.NameType.Enum
        flexibleStartForWhiteGoods: UseCase.NameType.Enum
        limitationOfPowerConsumption: UseCase.NameType.Enum
        limitationOfPowerProduction: UseCase.NameType.Enum
        incentiveTableBasedPowerConsumptionManagement: UseCase.NameType.Enum
        measurementOfElectricityDuringEvCharging: UseCase.NameType.Enum
        monitoringAndControlOfSmartGridReadyConditions: UseCase.NameType.Enum
        monitoringOfBattery: UseCase.NameType.Enum
        monitoringOfDhwSystemFunction: UseCase.NameType.Enum
        monitoringOfDhwTemperature: UseCase.NameType.Enum
        monitoringOfGridConnectionPoint: UseCase.NameType.Enum
        monitoringOfInverter: UseCase.NameType.Enum
        monitoringOfOutdoorTemperature: UseCase.NameType.Enum
        monitoringOfPowerConsumption: UseCase.NameType.Enum
        monitoringOfPvString: UseCase.NameType.Enum
        monitoringOfRoomCoolingSystemFunction: UseCase.NameType.Enum
        monitoringOfRoomHeatingSystemFunction: UseCase.NameType.Enum
        monitoringOfRoomTemperature: UseCase.NameType.Enum
        optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility: UseCase.NameType.Enum
        optimizationOfSelfConsumptionDuringEvCharging: UseCase.NameType.Enum
        overloadProtectionByEvChargingCurrentCurtailment: UseCase.NameType.Enum
        visualizationOfAggregatedBatteryData: UseCase.NameType.Enum
        visualizationOfAggregatedPhotovoltaicData: UseCase.NameType.Enum
        visualizationOfHeatingAreaName: UseCase.NameType.Enum
        def __init__(self) -> None: ...
    ACTOR_FIELD_NUMBER: _ClassVar[int]
    NAME_FIELD_NUMBER: _ClassVar[int]
    actor: UseCase.ActorType.Enum
    name: UseCase.NameType.Enum
    def __init__(self, actor: _Optional[_Union[UseCase.ActorType.Enum, str]] = ..., name: _Optional[_Union[UseCase.NameType.Enum, str]] = ...) -> None: ...

class UseCaseEvent(_message.Message):
    __slots__ = ("use_case", "event")
    USE_CASE_FIELD_NUMBER: _ClassVar[int]
    EVENT_FIELD_NUMBER: _ClassVar[int]
    use_case: UseCase
    event: str
    def __init__(self, use_case: _Optional[_Union[UseCase, _Mapping]] = ..., event: _Optional[str] = ...) -> None: ...
