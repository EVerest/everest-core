#include <grpc_calls/ControlServiceCalls.hpp>

#include <utility>

#include "EEBUS.hpp"

namespace module {
namespace grpc_calls {

ControlServiceCalls::ControlServiceCalls(const std::shared_ptr<control_service::ControlService::Stub> stub) :
    stub(stub) {
    this->entity_address = common_types::CreateEntityAddress(std::vector<uint32_t>{1});
    this->use_case = control_service::CreateUseCase(
        control_service::UseCase_ActorType_Enum::UseCase_ActorType_Enum_ControllableSystem,
        control_service::UseCase_NameType_Enum::UseCase_NameType_Enum_limitationOfPowerConsumption);
}

void ControlServiceCalls::call_set_config() {
    control_service::SetConfigRequest request;
    // TODO: Make this configurable
    request = control_service::CreateSetConfigRequest(
        4715, "Demo", "Demo", "EVSE", "2345678901",
        std::vector<control_service::DeviceCategory_Enum>{
            control_service::DeviceCategory_Enum::DeviceCategory_Enum_E_MOBILITY},
        control_service::DeviceType_Enum::DeviceType_Enum_CHARGING_STATION,
        std::vector<control_service::EntityType_Enum>{control_service::EntityType_Enum::EntityType_Enum_EVSE}, 4);
    control_service::EmptyResponse response;
    control_service::CallSetConfig(this->stub, request, &response);
}

void ControlServiceCalls::call_start_setup() {
    control_service::EmptyResponse response;
    control_service::CallStartSetup(this->stub, control_service::EmptyRequest(), &response);
}

void ControlServiceCalls::call_register_remote_ski(const std::string& remote_ski) {
    control_service::RegisterRemoteSkiRequest request;
    request = control_service::CreateRegisterRemoteSkiRequest(remote_ski);
    control_service::EmptyResponse response;
    control_service::CallRegisterRemoteSki(this->stub, request, &response);
}

void ControlServiceCalls::call_start_service() {
    control_service::EmptyResponse response;
    control_service::CallStartService(this->stub, control_service::EmptyRequest(), &response);
}

std::string ControlServiceCalls::call_add_use_case() {
    control_service::AddUseCaseRequest request =
        control_service::CreateAddUseCaseRequest(&this->entity_address, &this->use_case);
    control_service::AddUseCaseResponse response;
    control_service::CallAddUseCase(this->stub, request, &response);

    std::ignore = request.release_entity_address();
    std::ignore = request.release_use_case();

    return response.endpoint();
}

void ControlServiceCalls::subscribe_use_case_events(
    module::EEBUS* eebus_module, const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> cs_lpc_stub) {
    control_service::SubscribeUseCaseEventsRequest request =
        control_service::CreateSubscribeUseCaseEventsRequest(&this->entity_address, &this->use_case);
    eebus_module->set_use_case_event_reader(
        std::make_unique<UseCaseEventReader>(this->stub, cs_lpc_stub, request, eebus_module));

    std::ignore = request.release_entity_address();
    std::ignore = request.release_use_case();
}

} // namespace grpc_calls
} // namespace module
