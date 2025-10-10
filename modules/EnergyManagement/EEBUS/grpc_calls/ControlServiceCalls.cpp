#include <grpc_calls/ControlServiceCalls.hpp>

#include <utility>

#include "EEBUS.hpp"

namespace module {
namespace grpc_calls {

ControlServiceCalls::ControlServiceCalls(const std::shared_ptr<control_service::ControlService::Stub>& stub) :
    control_service_stub(stub) {
    this->entity_address = common_types::CreateEntityAddress(std::vector<uint32_t>{1});
}

void ControlServiceCalls::call_set_config(uint32_t port, std::string vendor_code, std::string device_brand,
                                          std::string device_model, std::string serial_number) {
    control_service::SetConfigRequest request;
    request = control_service::CreateSetConfigRequest(
        port, std::move(vendor_code), std::move(device_brand), std::move(device_model), std::move(serial_number),
        std::vector<control_service::DeviceCategory_Enum>{
            control_service::DeviceCategory_Enum::DeviceCategory_Enum_E_MOBILITY},
        control_service::DeviceType_Enum::DeviceType_Enum_CHARGING_STATION,
        std::vector<control_service::EntityType_Enum>{control_service::EntityType_Enum::EntityType_Enum_EVSE}, 4);
    control_service::EmptyResponse response;
    control_service::CallSetConfig(this->control_service_stub, request, &response);
}

void ControlServiceCalls::call_start_setup() {
    control_service::EmptyResponse response;
    control_service::CallStartSetup(this->control_service_stub, control_service::EmptyRequest(), &response);
}

void ControlServiceCalls::call_register_remote_ski(const std::string& remote_ski) {
    control_service::RegisterRemoteSkiRequest request;
    request = control_service::CreateRegisterRemoteSkiRequest(remote_ski);
    control_service::EmptyResponse response;
    control_service::CallRegisterRemoteSki(this->control_service_stub, request, &response);
}

void ControlServiceCalls::call_start_service() {
    control_service::EmptyResponse response;
    control_service::CallStartService(this->control_service_stub, control_service::EmptyRequest(), &response);
}

std::string ControlServiceCalls::call_add_use_case(control_service::UseCase* use_case) {
    control_service::AddUseCaseRequest request =
        control_service::CreateAddUseCaseRequest(&this->entity_address, use_case);
    control_service::AddUseCaseResponse response;
    control_service::CallAddUseCase(this->control_service_stub, request, &response);

    std::ignore = request.release_entity_address();
    std::ignore = request.release_use_case();

    return response.endpoint();
}

void ControlServiceCalls::subscribe_use_case_events(module::EEBUS* eebus_module, use_case_stubs stub,
                                                    control_service::UseCase* use_case) {
    if (std::holds_alternative<std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>>(stub)) {
        control_service::SubscribeUseCaseEventsRequest request =
            control_service::CreateSubscribeUseCaseEventsRequest(&this->entity_address, use_case);
        this->lpc_uc_reader = std::make_unique<UseCaseEventReader>(
            this->control_service_stub, std::get<std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>>(stub),
            request, eebus_module);
        std::ignore = request.release_entity_address();
        std::ignore = request.release_use_case();
    }
}

} // namespace grpc_calls
} // namespace module
