# Message Dispatching Class Diagram

```mermaid
classDiagram
class MessageDispatcherInterface {
    +dispatch_call(const json& call, bool triggered = false)
    +dispatch_call_async(const json& call, bool triggered = false): std::future~EnhancedMessage~T~~
    +dispatch_call_result(const json& call_result)
    +dispatch_call_error(const json& call_error)
}

class v16_MessageDispatcher {
    - MessageQueue& message_queue
    - ChargePointConfiguration& configuration
    - RegistrationStatus& registration_status
}

class v2_MessageDispatcher {
    - MessageQueue& message_queue
    - DeviceModel& device_model
    - ConnectivityManager& connectivity_manager
    - RegistrationStatusEnum& registration_status
}

class v2_MessageHandlerInterface {
    +handle_message(EnhancedMessage~v2_MessageType~ message)
}

class v16_MessageHandlerInterface {
    +handle_message(EnhancedMessage~v16_MessageType~ message)
}

class v2_DataTransferInterface {
    +data_transfer_req(request: DataTransferRequest): std::optional~DataTransferResponse~
    +handle_data_transfer_req(call: Call~DataTransferRequest~)
}

class v2_DataTransfer {
    -MessageDispatcherInterface &message_dispatcher
    -std::optional~function~ data_transfer_callback
}

class v2_ChargePoint {
    std::unique_ptr~MessageDispatcherInterface~ message_dispatcher
    std::unique_ptr~v2_DataTransferInterface~ data_transfer
}

class v16_ChargePoint {
    std::unique_ptr~MessageDispatcherInterface~ message_dispatcher
}

MessageDispatcherInterface <|-- v16_MessageDispatcher  
MessageDispatcherInterface <|-- v2_MessageDispatcher
v2_DataTransferInterface <|-- v2_DataTransfer
v2_MessageHandlerInterface <|-- v2_DataTransferInterface
MessageDispatcherInterface *-- v2_DataTransfer
MessageDispatcherInterface *-- v2_ChargePoint
v2_DataTransferInterface *-- v2_ChargePoint
MessageDispatcherInterface *-- v16_ChargePoint
```
