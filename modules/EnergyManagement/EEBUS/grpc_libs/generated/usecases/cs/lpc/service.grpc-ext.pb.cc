#include "usecases/cs/lpc/service.grpc-ext.pb.h"

#include <condition_variable>
#include <mutex>

namespace cs_lpc {

grpc::Status CallConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ConsumptionLimitRequest& request,
    cs_lpc::ConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->ConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallSetConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetConsumptionLimitRequest& request,
    cs_lpc::SetConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->SetConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallPendingConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::PendingConsumptionLimitRequest& request,
    cs_lpc::PendingConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->PendingConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallApproveOrDenyConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ApproveOrDenyConsumptionLimitRequest& request,
    cs_lpc::ApproveOrDenyConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->ApproveOrDenyConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::FailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::FailsafeConsumptionActivePowerLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->FailsafeConsumptionActivePowerLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallSetFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->SetFailsafeConsumptionActivePowerLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::FailsafeDurationMinimumRequest& request,
    cs_lpc::FailsafeDurationMinimumResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->FailsafeDurationMinimum(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallSetFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetFailsafeDurationMinimumRequest& request,
    cs_lpc::SetFailsafeDurationMinimumResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->SetFailsafeDurationMinimum(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallStartHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::StartHeartbeatRequest& request,
    cs_lpc::StartHeartbeatResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->StartHeartbeat(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallStopHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::StopHeartbeatRequest& request,
    cs_lpc::StopHeartbeatResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->StopHeartbeat(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallIsHeartbeatWithinDuration(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::IsHeartbeatWithinDurationRequest& request,
    cs_lpc::IsHeartbeatWithinDurationResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->IsHeartbeatWithinDuration(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::ConsumptionNominalMaxRequest& request,
    cs_lpc::ConsumptionNominalMaxResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->ConsumptionNominalMax(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

grpc::Status CallSetConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> &stub,
    cs_lpc::SetConsumptionNominalMaxRequest& request,
    cs_lpc::SetConsumptionNominalMaxResponse* response) {
  grpc::ClientContext context;
  grpc::Status result;
  bool done = false;
  std::mutex mu;
  std::condition_variable cv;
  stub->async()->SetConsumptionNominalMax(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      result = std::move(status);
      std::lock_guard<std::mutex> lock(mu);
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

} // namespace cs_lpc

