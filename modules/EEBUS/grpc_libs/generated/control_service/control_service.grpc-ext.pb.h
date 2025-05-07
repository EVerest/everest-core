#ifndef GENERATED_control_service_control_service_proto_EXT_H
#define GENERATED_control_service_control_service_proto_EXT_H

#include "control_service/control_service.grpc.pb.h"
#include "control_service/messages.grpc-ext.pb.h"

namespace control_service {

bool CallStartService(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response);

bool CallStopService(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response);

bool CallSetConfig(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::SetConfigRequest& request,
    control_service::EmptyResponse* response);

bool CallStartSetup(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::EmptyRequest& request,
    control_service::EmptyResponse* response);

bool CallAddEntity(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::AddEntityRequest& request,
    control_service::EmptyResponse* response);

bool CallRemoveEntity(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::RemoveEntityRequest& request,
    control_service::EmptyResponse* response);

bool CallRegisterRemoteSki(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::RegisterRemoteSkiRequest& request,
    control_service::EmptyResponse* response);

bool CallAddUseCase(
    const std::shared_ptr<control_service::ControlService::Stub> stub,
    const control_service::AddUseCaseRequest& request,
    control_service::AddUseCaseResponse* response);

} // namespace control_service

#endif // GENERATED_control_service_control_service_proto_EXT_H
