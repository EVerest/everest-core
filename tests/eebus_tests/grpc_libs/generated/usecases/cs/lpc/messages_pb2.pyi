from common_types import types_pb2 as _types_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Mapping as _Mapping
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ConsumptionLimitRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class ConsumptionLimitResponse(_message.Message):
    __slots__ = ("load_limit",)
    LOAD_LIMIT_FIELD_NUMBER: _ClassVar[int]
    load_limit: _types_pb2.LoadLimit
    def __init__(self, load_limit: _Optional[_Union[_types_pb2.LoadLimit, _Mapping]] = ...) -> None: ...

class SetConsumptionLimitRequest(_message.Message):
    __slots__ = ("load_limit",)
    LOAD_LIMIT_FIELD_NUMBER: _ClassVar[int]
    load_limit: _types_pb2.LoadLimit
    def __init__(self, load_limit: _Optional[_Union[_types_pb2.LoadLimit, _Mapping]] = ...) -> None: ...

class SetConsumptionLimitResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class PendingConsumptionLimitRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class PendingConsumptionLimitResponse(_message.Message):
    __slots__ = ("load_limits",)
    class LoadLimitsEntry(_message.Message):
        __slots__ = ("key", "value")
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: int
        value: _types_pb2.LoadLimit
        def __init__(self, key: _Optional[int] = ..., value: _Optional[_Union[_types_pb2.LoadLimit, _Mapping]] = ...) -> None: ...
    LOAD_LIMITS_FIELD_NUMBER: _ClassVar[int]
    load_limits: _containers.MessageMap[int, _types_pb2.LoadLimit]
    def __init__(self, load_limits: _Optional[_Mapping[int, _types_pb2.LoadLimit]] = ...) -> None: ...

class ApproveOrDenyConsumptionLimitRequest(_message.Message):
    __slots__ = ("msg_counter", "approve", "reason")
    MSG_COUNTER_FIELD_NUMBER: _ClassVar[int]
    APPROVE_FIELD_NUMBER: _ClassVar[int]
    REASON_FIELD_NUMBER: _ClassVar[int]
    msg_counter: int
    approve: bool
    reason: str
    def __init__(self, msg_counter: _Optional[int] = ..., approve: bool = ..., reason: _Optional[str] = ...) -> None: ...

class ApproveOrDenyConsumptionLimitResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class FailsafeConsumptionActivePowerLimitRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class FailsafeConsumptionActivePowerLimitResponse(_message.Message):
    __slots__ = ("limit", "is_changeable")
    LIMIT_FIELD_NUMBER: _ClassVar[int]
    IS_CHANGEABLE_FIELD_NUMBER: _ClassVar[int]
    limit: float
    is_changeable: bool
    def __init__(self, limit: _Optional[float] = ..., is_changeable: bool = ...) -> None: ...

class SetFailsafeConsumptionActivePowerLimitRequest(_message.Message):
    __slots__ = ("value", "is_changeable")
    VALUE_FIELD_NUMBER: _ClassVar[int]
    IS_CHANGEABLE_FIELD_NUMBER: _ClassVar[int]
    value: float
    is_changeable: bool
    def __init__(self, value: _Optional[float] = ..., is_changeable: bool = ...) -> None: ...

class SetFailsafeConsumptionActivePowerLimitResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class FailsafeDurationMinimumRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class FailsafeDurationMinimumResponse(_message.Message):
    __slots__ = ("duration_nanoseconds", "is_changeable")
    DURATION_NANOSECONDS_FIELD_NUMBER: _ClassVar[int]
    IS_CHANGEABLE_FIELD_NUMBER: _ClassVar[int]
    duration_nanoseconds: int
    is_changeable: bool
    def __init__(self, duration_nanoseconds: _Optional[int] = ..., is_changeable: bool = ...) -> None: ...

class SetFailsafeDurationMinimumRequest(_message.Message):
    __slots__ = ("duration_nanoseconds", "is_changeable")
    DURATION_NANOSECONDS_FIELD_NUMBER: _ClassVar[int]
    IS_CHANGEABLE_FIELD_NUMBER: _ClassVar[int]
    duration_nanoseconds: int
    is_changeable: bool
    def __init__(self, duration_nanoseconds: _Optional[int] = ..., is_changeable: bool = ...) -> None: ...

class SetFailsafeDurationMinimumResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class StartHeartbeatRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class StartHeartbeatResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class StopHeartbeatRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class StopHeartbeatResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class IsHeartbeatWithinDurationRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class IsHeartbeatWithinDurationResponse(_message.Message):
    __slots__ = ("is_within_duration",)
    IS_WITHIN_DURATION_FIELD_NUMBER: _ClassVar[int]
    is_within_duration: bool
    def __init__(self, is_within_duration: bool = ...) -> None: ...

class ConsumptionNominalMaxRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class ConsumptionNominalMaxResponse(_message.Message):
    __slots__ = ("value",)
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: float
    def __init__(self, value: _Optional[float] = ...) -> None: ...

class SetConsumptionNominalMaxRequest(_message.Message):
    __slots__ = ("value",)
    VALUE_FIELD_NUMBER: _ClassVar[int]
    value: float
    def __init__(self, value: _Optional[float] = ...) -> None: ...

class SetConsumptionNominalMaxResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...
