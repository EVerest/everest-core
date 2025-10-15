#include <grpc_calls/ControlServiceCalls.hpp>

#include <utility>

#include <control_service/messages.pb.h>
#include <everest/logging.hpp>

namespace module {
namespace grpc_calls {

constexpr int EEBUS_HEARTBEAT_TIMEOUT = 120; // 2 minutes

ControlServiceCalls::ControlServiceCalls(const std::shared_ptr<control_service::ControlService::Stub>& stub) :
    control_service_stub(stub) {
    this->entity_address = common_types::CreateEntityAddress(std::vector<uint32_t>{1});
}

void ControlServiceCalls::call_set_config(uint32_t port, std::string vendor_code, std::string device_brand,
                                          std::string device_model, std::string serial_number) {
    control_service::SetConfigRequest request = control_service::CreateSetConfigRequest(
        port, std::move(vendor_code), std::move(device_brand), std::move(device_model), std::move(serial_number),
        std::vector<control_service::DeviceCategory_Enum>{
            control_service::DeviceCategory_Enum::DeviceCategory_Enum_E_MOBILITY},
        control_service::DeviceType_Enum::DeviceType_Enum_CHARGING_STATION,
        std::vector<control_service::EntityType_Enum>{control_service::EntityType_Enum::EntityType_Enum_EVSE},
        EEBUS_HEARTBEAT_TIMEOUT);
    control_service::EmptyResponse response;
    auto status = control_service::CallSetConfig(this->control_service_stub, request, &response);
}

void ControlServiceCalls::call_start_setup() {
    control_service::EmptyResponse response;
    auto empty_req = control_service::CreateEmptyRequest();
    control_service::CallStartSetup(this->control_service_stub, empty_req, &response);
}

void ControlServiceCalls::call_register_remote_ski(const std::string& remote_ski) {
    control_service::RegisterRemoteSkiRequest request;
    request = control_service::CreateRegisterRemoteSkiRequest(remote_ski);
    control_service::EmptyResponse response;
    control_service::CallRegisterRemoteSki(this->control_service_stub, request, &response);
}

void ControlServiceCalls::call_start_service() {
    control_service::EmptyResponse response;
    auto empty_req = control_service::CreateEmptyRequest();
    control_service::CallStartService(this->control_service_stub, empty_req, &response);
}

std::string ControlServiceCalls::call_add_use_case(const control_service::UseCase& use_case) {
    control_service::AddUseCaseRequest request;
    if (use_case.name() == control_service::UseCase_NameType_Enum::UseCase_NameType_Enum_limitationOfPowerConsumption) {
        lpc_use_case = use_case;
        request = control_service::CreateAddUseCaseRequest(&this->entity_address, &lpc_use_case);
    } else {
        EVLOG_error << "Unknown use case, aborting: " << use_case.name();
        return "";
    }
    control_service::AddUseCaseResponse response;
    control_service::CallAddUseCase(this->control_service_stub, request, &response);

    std::ignore = request.release_entity_address();
    std::ignore = request.release_use_case();

    return response.endpoint();
}

void ControlServiceCalls::subscribe_use_case_events(eebus::EEBusCallbacks callbacks, use_case_stubs stub) {
    if (std::holds_alternative<std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>>(stub)) {
        control_service::SubscribeUseCaseEventsRequest request =
            control_service::CreateSubscribeUseCaseEventsRequest(&this->entity_address, &lpc_use_case);
        this->lpc_uc_reader = std::make_unique<LpcUseCaseEventReader>(
            this->control_service_stub, std::get<std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>>(stub),
            request, callbacks);
        std::ignore = request.release_entity_address();
        std::ignore = request.release_use_case();
    } else {
        EVLOG_error << "Unknown use case stub type";
    }
}

} // namespace grpc_calls
} // namespace module
