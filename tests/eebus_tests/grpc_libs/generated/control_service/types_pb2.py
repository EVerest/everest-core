# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# NO CHECKED-IN PROTOBUF GENCODE
# source: control_service/types.proto
# Protobuf Python Version: 5.27.2
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    5,
    27,
    2,
    '',
    'control_service/types.proto'
)
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x1b\x63ontrol_service/types.proto\x12\x0f\x63ontrol_service\"\xab\x01\n\x0e\x44\x65viceCategory\"\x98\x01\n\x04\x45num\x12\x0b\n\x07UNKNOWN\x10\x00\x12\x17\n\x13GRID_CONNECTION_HUB\x10\x01\x12\x1c\n\x18\x45NERGY_MANAGEMENT_SYSTEM\x10\x02\x12\x0e\n\nE_MOBILITY\x10\x03\x12\x08\n\x04HVAC\x10\x04\x12\x0c\n\x08INVERTER\x10\x05\x12\x16\n\x12\x44OMESTIC_APPLIANCE\x10\x06\x12\x0c\n\x08METERING\x10\x07\"\xb2\x07\n\nEntityType\"\xa3\x07\n\x04\x45num\x12\x0b\n\x07UNKNOWN\x10\x00\x12\x0b\n\x07\x42\x61ttery\x10\x01\x12\x0e\n\nCompressor\x10\x02\x12\x15\n\x11\x44\x65viceInformation\x10\x03\x12\x0e\n\nDHWCircuit\x10\x04\x12\x0e\n\nDHWStorage\x10\x05\x12\x0e\n\nDishwasher\x10\x06\x12\t\n\x05\x44ryer\x10\x07\x12\x1d\n\x19\x45lectricalImmersionHeater\x10\x08\x12\x07\n\x03\x46\x61n\x10\t\x12\x17\n\x13GasHeatingAppliance\x10\n\x12\x0b\n\x07Generic\x10\x0b\x12\x18\n\x14HeatingBufferStorage\x10\x0c\x12\x12\n\x0eHeatingCircuit\x10\r\x12\x11\n\rHeatingObject\x10\x0e\x12\x0f\n\x0bHeatingZone\x10\x0f\x12\x15\n\x11HeatPumpAppliance\x10\x10\x12\x13\n\x0fHeatSinkCircuit\x10\x11\x12\x15\n\x11HeatSourceCircuit\x10\x12\x12\x12\n\x0eHeatSourceUnit\x10\x13\x12\x12\n\x0eHvacController\x10\x14\x12\x0c\n\x08HvacRoom\x10\x15\x12\x14\n\x10InstantDHWHeater\x10\x16\x12\x0c\n\x08Inverter\x10\x17\x12\x17\n\x13OilHeatingAppliance\x10\x18\x12\x08\n\x04Pump\x10\x19\x12\x16\n\x12RefrigerantCircuit\x10\x1a\x12\x18\n\x14SmartEnergyAppliance\x10\x1b\x12\x13\n\x0fSolarDHWStorage\x10\x1c\x12\x17\n\x13SolarThermalCircuit\x10\x1d\x12\x17\n\x13SubMeterElectricity\x10\x1e\x12\x15\n\x11TemperatureSensor\x10\x1f\x12\n\n\x06Washer\x10 \x12\x11\n\rBatterySystem\x10!\x12\x1f\n\x1b\x45lectricityGenerationSystem\x10\"\x12\x1c\n\x18\x45lectricityStorageSystem\x10#\x12!\n\x1dGridConnectionPointOfPremises\x10$\x12\r\n\tHousehold\x10%\x12\x0c\n\x08PVSystem\x10&\x12\x06\n\x02\x45V\x10\'\x12\x08\n\x04\x45VSE\x10(\x12\x12\n\x0e\x43hargingOutlet\x10)\x12\x07\n\x03\x43\x45M\x10*\x12\x06\n\x02PV\x10+\x12\x0e\n\nPVESHybrid\x10,\x12\x15\n\x11\x45lectricalStorage\x10-\x12\x0c\n\x08PVString\x10.\x12\r\n\tGridGuard\x10/\x12\x16\n\x12\x43ontrollableSystem\x10\x30\"\xbd\x02\n\nDeviceType\"\xae\x02\n\x04\x45num\x12\x0b\n\x07UNKNOWN\x10\x00\x12\x0e\n\nDISHWASHER\x10\x01\x12\t\n\x05\x44RYER\x10\x02\x12\x16\n\x12\x45NVIRONMENT_SENSOR\x10\x03\x12\x0b\n\x07GENERIC\x10\x04\x12\x1a\n\x16HEAT_GENERATION_SYSTEM\x10\x05\x12\x14\n\x10HEAT_SINK_SYSTEM\x10\x06\x12\x17\n\x13HEAT_STORAGE_SYSTEM\x10\x07\x12\x13\n\x0fHVAC_CONTROLLER\x10\x08\x12\x0c\n\x08SUBMETER\x10\t\x12\n\n\x06WASHER\x10\n\x12\x1d\n\x19\x45LECTRICITY_SUPPLY_SYSTEM\x10\x0b\x12\x1c\n\x18\x45NERGY_MANAGEMENT_SYSTEM\x10\x0c\x12\x0c\n\x08INVERTER\x10\r\x12\x14\n\x10\x43HARGING_STATION\x10\x0e\"\xca\x0f\n\x07UseCase\x12\x36\n\x05\x61\x63tor\x18\x01 \x01(\x0e\x32\'.control_service.UseCase.ActorType.Enum\x12\x34\n\x04name\x18\x02 \x01(\x0e\x32&.control_service.UseCase.NameType.Enum\x1a\xe5\x03\n\tActorType\"\xd7\x03\n\x04\x45num\x12\x0b\n\x07UNKNOWN\x10\x00\x12\x0b\n\x07\x42\x61ttery\x10\x01\x12\x11\n\rBatterySystem\x10\x02\x12\x07\n\x03\x43\x45M\x10\x03\x12\x1a\n\x16\x43onfigurationAppliance\x10\x04\x12\x0e\n\nCompressor\x10\x05\x12\x16\n\x12\x43ontrollableSystem\x10\x06\x12\x0e\n\nDHWCircuit\x10\x07\x12\x10\n\x0c\x45nergyBroker\x10\x08\x12\x12\n\x0e\x45nergyConsumer\x10\t\x12\x0f\n\x0b\x45nergyGuard\x10\n\x12\x08\n\x04\x45VSE\x10\x0b\x12\x06\n\x02\x45V\x10\x0c\x12\x17\n\x13GridConnectionPoint\x10\r\x12\x0c\n\x08HeatPump\x10\x0e\x12\x12\n\x0eHeatingCircuit\x10\x0f\x12\x0f\n\x0bHeatingZone\x10\x10\x12\x0c\n\x08HVACRoom\x10\x11\x12\x0c\n\x08Inverter\x10\x12\x12\x11\n\rMonitoredUnit\x10\x13\x12\x17\n\x13MonitoringAppliance\x10\x14\x12\x1c\n\x18OutdoorTemperatureSensor\x10\x15\x12\x0c\n\x08PVString\x10\x16\x12\x0c\n\x08PVSystem\x10\x17\x12\x12\n\x0eSmartAppliance\x10\x18\x12\x1a\n\x16VisualizationAppliance\x10\x19\x1a\xe8\n\n\x08NameType\"\xdb\n\n\x04\x45num\x12\x0b\n\x07UNKNOWN\x10\x00\x12$\n configurationOfDhwSystemFunction\x10\x01\x12!\n\x1d\x63onfigurationOfDhwTemperature\x10\x02\x12,\n(configurationOfRoomCoolingSystemFunction\x10\x03\x12)\n%configurationOfRoomCoolingTemperature\x10\x04\x12,\n(configurationOfRoomHeatingSystemFunction\x10\x05\x12)\n%configurationOfRoomHeatingTemperature\x10\x06\x12\x14\n\x10\x63ontrolOfBattery\x10\x07\x12\x19\n\x15\x63oordinatedEvCharging\x10\x08\x12\x15\n\x11\x65vChargingSummary\x10\t\x12#\n\x1f\x65vCommissioningAndConfiguration\x10\n\x12%\n!evseCommissioningAndConfiguration\x10\x0b\x12\x13\n\x0f\x65vStateOfCharge\x10\x0c\x12\x10\n\x0c\x66lexibleLoad\x10\r\x12\x1e\n\x1a\x66lexibleStartForWhiteGoods\x10\x0e\x12 \n\x1climitationOfPowerConsumption\x10\x0f\x12\x1f\n\x1blimitationOfPowerProduction\x10\x10\x12\x31\n-incentiveTableBasedPowerConsumptionManagement\x10\x11\x12,\n(measurementOfElectricityDuringEvCharging\x10\x12\x12\x32\n.monitoringAndControlOfSmartGridReadyConditions\x10\x13\x12\x17\n\x13monitoringOfBattery\x10\x14\x12!\n\x1dmonitoringOfDhwSystemFunction\x10\x15\x12\x1e\n\x1amonitoringOfDhwTemperature\x10\x16\x12#\n\x1fmonitoringOfGridConnectionPoint\x10\x17\x12\x18\n\x14monitoringOfInverter\x10\x18\x12\"\n\x1emonitoringOfOutdoorTemperature\x10\x19\x12 \n\x1cmonitoringOfPowerConsumption\x10\x1a\x12\x18\n\x14monitoringOfPvString\x10\x1b\x12)\n%monitoringOfRoomCoolingSystemFunction\x10\x1c\x12)\n%monitoringOfRoomHeatingSystemFunction\x10\x1d\x12\x1f\n\x1bmonitoringOfRoomTemperature\x10\x1e\x12@\n<optimizationOfSelfConsumptionByHeatPumpCompressorFlexibility\x10\x1f\x12\x31\n-optimizationOfSelfConsumptionDuringEvCharging\x10 \x12\x34\n0overloadProtectionByEvChargingCurrentCurtailment\x10!\x12(\n$visualizationOfAggregatedBatteryData\x10\"\x12-\n)visualizationOfAggregatedPhotovoltaicData\x10#\x12\"\n\x1evisualizationOfHeatingAreaName\x10$\"I\n\x0cUseCaseEvent\x12*\n\x08use_case\x18\x01 \x01(\x0b\x32\x18.control_service.UseCase\x12\r\n\x05\x65vent\x18\x02 \x01(\t*\x1a\n\nSharedType\x12\x0c\n\x08INVERTER\x10\x00\x42\x41Z?github.com/enbility/eebus-grpc-api/rpc_services/control_serviceb\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'control_service.types_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  _globals['DESCRIPTOR']._loaded_options = None
  _globals['DESCRIPTOR']._serialized_options = b'Z?github.com/enbility/eebus-grpc-api/rpc_services/control_service'
  _globals['_SHAREDTYPE']._serialized_start=3563
  _globals['_SHAREDTYPE']._serialized_end=3589
  _globals['_DEVICECATEGORY']._serialized_start=49
  _globals['_DEVICECATEGORY']._serialized_end=220
  _globals['_DEVICECATEGORY_ENUM']._serialized_start=68
  _globals['_DEVICECATEGORY_ENUM']._serialized_end=220
  _globals['_ENTITYTYPE']._serialized_start=223
  _globals['_ENTITYTYPE']._serialized_end=1169
  _globals['_ENTITYTYPE_ENUM']._serialized_start=238
  _globals['_ENTITYTYPE_ENUM']._serialized_end=1169
  _globals['_DEVICETYPE']._serialized_start=1172
  _globals['_DEVICETYPE']._serialized_end=1489
  _globals['_DEVICETYPE_ENUM']._serialized_start=1187
  _globals['_DEVICETYPE_ENUM']._serialized_end=1489
  _globals['_USECASE']._serialized_start=1492
  _globals['_USECASE']._serialized_end=3486
  _globals['_USECASE_ACTORTYPE']._serialized_start=1614
  _globals['_USECASE_ACTORTYPE']._serialized_end=2099
  _globals['_USECASE_ACTORTYPE_ENUM']._serialized_start=1628
  _globals['_USECASE_ACTORTYPE_ENUM']._serialized_end=2099
  _globals['_USECASE_NAMETYPE']._serialized_start=2102
  _globals['_USECASE_NAMETYPE']._serialized_end=3486
  _globals['_USECASE_NAMETYPE_ENUM']._serialized_start=2115
  _globals['_USECASE_NAMETYPE_ENUM']._serialized_end=3486
  _globals['_USECASEEVENT']._serialized_start=3488
  _globals['_USECASEEVENT']._serialized_end=3561
# @@protoc_insertion_point(module_scope)
