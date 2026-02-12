#ifndef GENERATED_usecases_cs_lpc_service_proto_EXT_H
#define GENERATED_usecases_cs_lpc_service_proto_EXT_H

#include "usecases/cs/lpc/service.grpc.pb.h"
#include "usecases/cs/lpc/messages.grpc-ext.pb.h"

namespace cs_lpc {

grpc::Status CallConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ConsumptionLimitRequest& request,
    cs_lpc::ConsumptionLimitResponse* response);

grpc::Status CallSetConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetConsumptionLimitRequest& request,
    cs_lpc::SetConsumptionLimitResponse* response);

grpc::Status CallPendingConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::PendingConsumptionLimitRequest& request,
    cs_lpc::PendingConsumptionLimitResponse* response);

grpc::Status CallApproveOrDenyConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ApproveOrDenyConsumptionLimitRequest& request,
    cs_lpc::ApproveOrDenyConsumptionLimitResponse* response);

grpc::Status CallFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::FailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::FailsafeConsumptionActivePowerLimitResponse* response);

grpc::Status CallSetFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse* response);

grpc::Status CallFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::FailsafeDurationMinimumRequest& request,
    cs_lpc::FailsafeDurationMinimumResponse* response);

grpc::Status CallSetFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetFailsafeDurationMinimumRequest& request,
    cs_lpc::SetFailsafeDurationMinimumResponse* response);

grpc::Status CallStartHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::StartHeartbeatRequest& request,
    cs_lpc::StartHeartbeatResponse* response);

grpc::Status CallStopHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::StopHeartbeatRequest& request,
    cs_lpc::StopHeartbeatResponse* response);

grpc::Status CallIsHeartbeatWithinDuration(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::IsHeartbeatWithinDurationRequest& request,
    cs_lpc::IsHeartbeatWithinDurationResponse* response);

grpc::Status CallConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ConsumptionNominalMaxRequest& request,
    cs_lpc::ConsumptionNominalMaxResponse* response);

grpc::Status CallSetConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetConsumptionNominalMaxRequest& request,
    cs_lpc::SetConsumptionNominalMaxResponse* response);

} // namespace cs_lpc

#endif // GENERATED_usecases_cs_lpc_service_proto_EXT_H
