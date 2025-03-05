#ifndef GENERATED_usecases_cs_lpc_service_proto_EXT_H
#define GENERATED_usecases_cs_lpc_service_proto_EXT_H

#include "usecases/cs/lpc/service.grpc.pb.h"
#include "usecases/cs/lpc/messages.grpc-ext.pb.h"

namespace cs_lpc {

bool CallConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ConsumptionLimitRequest& request,
    cs_lpc::ConsumptionLimitResponse* response);

bool CallSetConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetConsumptionLimitRequest& request,
    cs_lpc::SetConsumptionLimitResponse* response);

bool CallPendingConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::PendingConsumptionLimitRequest& request,
    cs_lpc::PendingConsumptionLimitResponse* response);

bool CallApproveOrDenyConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ApproveOrDenyConsumptionLimitRequest& request,
    cs_lpc::ApproveOrDenyConsumptionLimitResponse* response);

bool CallFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::FailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::FailsafeConsumptionActivePowerLimitResponse* response);

bool CallSetFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse* response);

bool CallFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::FailsafeDurationMinimumRequest& request,
    cs_lpc::FailsafeDurationMinimumResponse* response);

bool CallSetFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetFailsafeDurationMinimumRequest& request,
    cs_lpc::SetFailsafeDurationMinimumResponse* response);

bool CallStartHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::StartHeartbeatRequest& request,
    cs_lpc::StartHeartbeatResponse* response);

bool CallStopHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::StopHeartbeatRequest& request,
    cs_lpc::StopHeartbeatResponse* response);

bool CallIsHeartbeatWithinDuration(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::IsHeartbeatWithinDurationRequest& request,
    cs_lpc::IsHeartbeatWithinDurationResponse* response);

bool CallConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ConsumptionNominalMaxRequest& request,
    cs_lpc::ConsumptionNominalMaxResponse* response);

bool CallSetConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetConsumptionNominalMaxRequest& request,
    cs_lpc::SetConsumptionNominalMaxResponse* response);

} // namespace cs_lpc

#endif // GENERATED_usecases_cs_lpc_service_proto_EXT_H
