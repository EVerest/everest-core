// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EEBUS.hpp"

#include <string>

#include "helper.hpp"
#include "useCaseEventReader.hpp"

#include <everest/logging.hpp>

#include "control_service/control_service.grpc-ext.pb.h"
#include "usecases/cs/lpc/service.grpc-ext.pb.h"

#include "compile_time_settings.hpp"

namespace module {


void EEBUS::init() {
    std::string remote_ski = "fa2548488be47c89a342ea29c2fc9867df7cf700";
    std::string rpc_url = "localhost:50051";
    invoke_init(*p_main);

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(rpc_url, grpc::InsecureChannelCredentials());
    this->control_service_stub = control_service::ControlService::NewStub(channel);

    grpc::ClientContext* context = nullptr;
    std::vector<uint32_t> entity_addresses;
    entity_addresses.push_back(1);
    auto entity_address = common_types::CreateEntityAddress(
        entity_addresses
    );
    auto use_case = control_service::CreateUseCase(
        control_service::UseCase_ActorType_Enum::UseCase_ActorType_Enum_ControllableSystem,
        control_service::UseCase_NameType_Enum::UseCase_NameType_Enum_limitationOfPowerConsumption
    );

    control_service::CallSetConfig(
        this->control_service_stub,
        control_service::CreateSetConfigRequest(
            4715,
            "Demo",
            "Demo",
            "EVSE",
            "2345678901",
            std::vector<control_service::DeviceCategory_Enum>{
                control_service::DeviceCategory_Enum::DeviceCategory_Enum_E_MOBILITY
            },
            control_service::DeviceType_Enum::DeviceType_Enum_CHARGING_STATION,
            std::vector<control_service::EntityType_Enum>{
                control_service::EntityType_Enum::EntityType_Enum_EVSE
            },
            4
        ),
        new control_service::EmptyResponse()
    );

    control_service::CallStartService(
        this->control_service_stub,
        control_service::EmptyRequest(),
        new control_service::EmptyResponse()
    );

    control_service::CallRegisterRemoteSki(
        this->control_service_stub,
        control_service::CreateRegisterRemoteSkiRequest(
            remote_ski
        ),
        new control_service::EmptyResponse()
    );

    control_service::AddUseCaseResponse add_use_case_response;
    control_service::CallAddUseCase(
        this->control_service_stub,
        control_service::CreateAddUseCaseRequest(
            &entity_address,
            &use_case
        ),
        &add_use_case_response
    );

    std::shared_ptr<grpc::Channel> channel2 = grpc::CreateChannel(
        add_use_case_response.endpoint(),
        grpc::InsecureChannelCredentials()
    );
    this->cs_lpc_stub = cs_lpc::ControllableSystemLPCControl::NewStub(channel2);

    cs_lpc::CallSetConsumptionNominalMax(
        this->cs_lpc_stub,
        cs_lpc::CreateSetConsumptionNominalMaxRequest(
            32000
        ),
        new cs_lpc::SetConsumptionNominalMaxResponse()
    );

    common_types::LoadLimit load_limit = common_types::CreateLoadLimit(
        0,
        true,
        false,
        4200,
        false
    );
    cs_lpc::CallSetConsumptionLimit(
        this->cs_lpc_stub,
        cs_lpc::CreateSetConsumptionLimitRequest(
            &load_limit
        ),
        new cs_lpc::SetConsumptionLimitResponse()
    );

    cs_lpc::CallSetFailsafeConsumptionActivePowerLimit(
        this->cs_lpc_stub,
        cs_lpc::CreateSetFailsafeConsumptionActivePowerLimitRequest(
            4200,
            true
        ),
        new cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse()
    );

    cs_lpc::CallSetFailsafeDurationMinimum(
        this->cs_lpc_stub,
        cs_lpc::CreateSetFailsafeDurationMinimumRequest(
            (int64_t) 2 * 60 * 60 * 1000 * 1000 * 1000,
            true
        ),
        new cs_lpc::SetFailsafeDurationMinimumResponse()
    );

    this->reader = new UseCaseEventReader(
        this->control_service_stub,
        this->cs_lpc_stub,
        control_service::CreateSubscribeUseCaseEventsRequest(
            &entity_address,
            &use_case
        ),
        this
    );
}

void EEBUS::ready() {
    invoke_ready(*p_main);

    control_service::CallStartService(
        this->control_service_stub,
        control_service::EmptyRequest(),
        new control_service::EmptyResponse()
    );

    EVLOG_error << "eebus-grpc-api binary is here: " << EEBUS_GRPC_API_BINARY_PATH;
}

} // namespace module
