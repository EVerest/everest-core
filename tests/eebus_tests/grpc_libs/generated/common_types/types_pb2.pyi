from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from collections.abc import Iterable as _Iterable
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class LoadLimit(_message.Message):
    __slots__ = ("duration_nanoseconds", "is_changeable", "is_active", "value", "delete_duration")
    DURATION_NANOSECONDS_FIELD_NUMBER: _ClassVar[int]
    IS_CHANGEABLE_FIELD_NUMBER: _ClassVar[int]
    IS_ACTIVE_FIELD_NUMBER: _ClassVar[int]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    DELETE_DURATION_FIELD_NUMBER: _ClassVar[int]
    duration_nanoseconds: int
    is_changeable: bool
    is_active: bool
    value: float
    delete_duration: bool
    def __init__(self, duration_nanoseconds: _Optional[int] = ..., is_changeable: bool = ..., is_active: bool = ..., value: _Optional[float] = ..., delete_duration: bool = ...) -> None: ...

class EntityAddress(_message.Message):
    __slots__ = ("entity_address",)
    ENTITY_ADDRESS_FIELD_NUMBER: _ClassVar[int]
    entity_address: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, entity_address: _Optional[_Iterable[int]] = ...) -> None: ...
