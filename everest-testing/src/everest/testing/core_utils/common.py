from enum import Enum


class Requirement:
    def __init__(self, module_id: str, implementation_id: str):
        self.module_id = module_id
        self.implementation_id = implementation_id


class OCPPVersion(str, Enum):
    ocpp16 = "ocpp1.6"
    ocpp201 = "ocpp2.0.1"
    ocpp21 = "ocpp2.1"
