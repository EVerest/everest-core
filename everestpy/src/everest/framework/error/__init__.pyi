from enum import Enum
from typing import Callable, overload, Optional

class Severity(Enum):
    Low = "Low",
    Medium = "Medium",
    High = "High"

class Mapping:
    @overload
    def __init__(self, evse: int) -> None ...

    @overload
    def __init__(self, evse: int, connector: int) -> None ...

    @property
    def evse(self) -> int ...

    @property
    def connector(self) -> Optional[int] ...

class ImplementationIdentifier:
    def __init__(self, module_id: str, implementation_id: str, mapping: Optional[Mapping]) -> None: ...

    @property
    def module_id(self) -> str: ...

    @property
    def implementation_id(self) -> str: ...

    @property
    def mapping(self) -> Optional[Mapping]: ...

class UUID:
    @overload
    def __init__(self) -> None: ...

    @overload
    def __init__(self, uuid: str) -> None: ...

class State(Enum):
    Active = "Active",
    ClearedByModule = "ClearedByModule",
    ClearedByReboot = "ClearedByReboot"

class Error:
    def __init__(self) -> None: ...

    @property
    def type(self) -> str: ...

    @property
    def sub_type(self) -> str: ...

    @property
    def description(self) -> str: ...

    @property
    def message(self) -> str: ...

    @property
    def severity(self) -> Severity: ...

    @property
    def origin(self) -> ImplementationIdentifier: ...

    @property
    def timestamp(self) -> int: ...

    @property
    def uuid(self) -> UUID: ...

    @property
    def state(self) -> State: ...

ErrorCallback = Callable[[Error], None]

class ErrorStateCondition:
    def __init__(self, type: str, sub_type: str, active: bool) -> None: ...

    @property
    def type(self) -> str: ...

    @property
    def sub_type(self) -> str: ...

    @property
    def active(self) -> bool: ...

class ErrorStateMonitor:
    def is_error_active(self, type: str, sub_type: str) -> bool: ...

    @overload
    def is_condition_satisfied(self, condition: ErrorStateCondition) -> bool: ...

    @overload
    def is_condition_satisfied(self, condition: list[ErrorStateCondition]) -> bool: ...

class ErrorFactory:
    @overload
    def create_error(self) -> Error: ...

    @overload
    def create_error(self, type: str, sub_type: str, message: str) -> Error: ...

    @overload
    def create_error(self, type: str, sub_type: str, message: str, severity: Severity) -> Error: ...

    @overload
    def create_error(self, type: str, sub_type: str, message: str, state: State) -> Error: ...

    @overload
    def create_error(self, type: str, sub_type: str, message: str, severity: Severity, state: State) -> Error: ...
