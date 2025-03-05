from control_service import types_pb2 as _types_pb2
from common_types import types_pb2 as _types_pb2_1
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class EmptyRequest(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class EmptyResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class SetConfigRequest(_message.Message):
    __slots__ = ("port", "vendor_code", "device_brand", "device_model", "serial_number", "device_categories", "device_type", "entity_types", "heartbeat_timeout_seconds")
    PORT_FIELD_NUMBER: _ClassVar[int]
    VENDOR_CODE_FIELD_NUMBER: _ClassVar[int]
    DEVICE_BRAND_FIELD_NUMBER: _ClassVar[int]
    DEVICE_MODEL_FIELD_NUMBER: _ClassVar[int]
    SERIAL_NUMBER_FIELD_NUMBER: _ClassVar[int]
    DEVICE_CATEGORIES_FIELD_NUMBER: _ClassVar[int]
    DEVICE_TYPE_FIELD_NUMBER: _ClassVar[int]
    ENTITY_TYPES_FIELD_NUMBER: _ClassVar[int]
    HEARTBEAT_TIMEOUT_SECONDS_FIELD_NUMBER: _ClassVar[int]
    port: int
    vendor_code: str
    device_brand: str
    device_model: str
    serial_number: str
    device_categories: _containers.RepeatedScalarFieldContainer[_types_pb2.DeviceCategory.Enum]
    device_type: _types_pb2.DeviceType.Enum
    entity_types: _containers.RepeatedScalarFieldContainer[_types_pb2.EntityType.Enum]
    heartbeat_timeout_seconds: int
    def __init__(self, port: _Optional[int] = ..., vendor_code: _Optional[str] = ..., device_brand: _Optional[str] = ..., device_model: _Optional[str] = ..., serial_number: _Optional[str] = ..., device_categories: _Optional[_Iterable[_Union[_types_pb2.DeviceCategory.Enum, str]]] = ..., device_type: _Optional[_Union[_types_pb2.DeviceType.Enum, str]] = ..., entity_types: _Optional[_Iterable[_Union[_types_pb2.EntityType.Enum, str]]] = ..., heartbeat_timeout_seconds: _Optional[int] = ...) -> None: ...

class AddEntityRequest(_message.Message):
    __slots__ = ("type", "address")
    TYPE_FIELD_NUMBER: _ClassVar[int]
    ADDRESS_FIELD_NUMBER: _ClassVar[int]
    type: _types_pb2.EntityType.Enum
    address: _types_pb2_1.EntityAddress
    def __init__(self, type: _Optional[_Union[_types_pb2.EntityType.Enum, str]] = ..., address: _Optional[_Union[_types_pb2_1.EntityAddress, _Mapping]] = ...) -> None: ...

class RemoveEntityRequest(_message.Message):
    __slots__ = ("address",)
    ADDRESS_FIELD_NUMBER: _ClassVar[int]
    address: _types_pb2_1.EntityAddress
    def __init__(self, address: _Optional[_Union[_types_pb2_1.EntityAddress, _Mapping]] = ...) -> None: ...

class AddUseCaseRequest(_message.Message):
    __slots__ = ("entity_address", "use_case")
    ENTITY_ADDRESS_FIELD_NUMBER: _ClassVar[int]
    USE_CASE_FIELD_NUMBER: _ClassVar[int]
    entity_address: _types_pb2_1.EntityAddress
    use_case: _types_pb2.UseCase
    def __init__(self, entity_address: _Optional[_Union[_types_pb2_1.EntityAddress, _Mapping]] = ..., use_case: _Optional[_Union[_types_pb2.UseCase, _Mapping]] = ...) -> None: ...

class AddUseCaseResponse(_message.Message):
    __slots__ = ("endpoint",)
    ENDPOINT_FIELD_NUMBER: _ClassVar[int]
    endpoint: str
    def __init__(self, endpoint: _Optional[str] = ...) -> None: ...

class RegisterRemoteSkiRequest(_message.Message):
    __slots__ = ("remote_ski",)
    REMOTE_SKI_FIELD_NUMBER: _ClassVar[int]
    remote_ski: str
    def __init__(self, remote_ski: _Optional[str] = ...) -> None: ...

class SubscribeUseCaseEventsRequest(_message.Message):
    __slots__ = ("entity_address", "use_case")
    ENTITY_ADDRESS_FIELD_NUMBER: _ClassVar[int]
    USE_CASE_FIELD_NUMBER: _ClassVar[int]
    entity_address: _types_pb2_1.EntityAddress
    use_case: _types_pb2.UseCase
    def __init__(self, entity_address: _Optional[_Union[_types_pb2_1.EntityAddress, _Mapping]] = ..., use_case: _Optional[_Union[_types_pb2.UseCase, _Mapping]] = ...) -> None: ...

class SubscribeUseCaseEventsResponse(_message.Message):
    __slots__ = ("remote_ski", "remote_entity_address", "use_case_event")
    REMOTE_SKI_FIELD_NUMBER: _ClassVar[int]
    REMOTE_ENTITY_ADDRESS_FIELD_NUMBER: _ClassVar[int]
    USE_CASE_EVENT_FIELD_NUMBER: _ClassVar[int]
    remote_ski: str
    remote_entity_address: _types_pb2_1.EntityAddress
    use_case_event: _types_pb2.UseCaseEvent
    def __init__(self, remote_ski: _Optional[str] = ..., remote_entity_address: _Optional[_Union[_types_pb2_1.EntityAddress, _Mapping]] = ..., use_case_event: _Optional[_Union[_types_pb2.UseCaseEvent, _Mapping]] = ...) -> None: ...
