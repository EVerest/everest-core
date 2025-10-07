#include "usecases/cs/lpc/service.grpc-ext.pb.h"

#include <mutex>
#include <condition_variable>

namespace cs_lpc {

bool CallConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ConsumptionLimitRequest& request,
    cs_lpc::ConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->ConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallSetConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetConsumptionLimitRequest& request,
    cs_lpc::SetConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->SetConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallPendingConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::PendingConsumptionLimitRequest& request,
    cs_lpc::PendingConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->PendingConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallApproveOrDenyConsumptionLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ApproveOrDenyConsumptionLimitRequest& request,
    cs_lpc::ApproveOrDenyConsumptionLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->ApproveOrDenyConsumptionLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::FailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::FailsafeConsumptionActivePowerLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->FailsafeConsumptionActivePowerLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallSetFailsafeConsumptionActivePowerLimit(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetFailsafeConsumptionActivePowerLimitRequest& request,
    cs_lpc::SetFailsafeConsumptionActivePowerLimitResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->SetFailsafeConsumptionActivePowerLimit(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::FailsafeDurationMinimumRequest& request,
    cs_lpc::FailsafeDurationMinimumResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->FailsafeDurationMinimum(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallSetFailsafeDurationMinimum(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetFailsafeDurationMinimumRequest& request,
    cs_lpc::SetFailsafeDurationMinimumResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->SetFailsafeDurationMinimum(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallStartHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::StartHeartbeatRequest& request,
    cs_lpc::StartHeartbeatResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->StartHeartbeat(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallStopHeartbeat(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::StopHeartbeatRequest& request,
    cs_lpc::StopHeartbeatResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->StopHeartbeat(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallIsHeartbeatWithinDuration(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::IsHeartbeatWithinDurationRequest& request,
    cs_lpc::IsHeartbeatWithinDurationResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->IsHeartbeatWithinDuration(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::ConsumptionNominalMaxRequest& request,
    cs_lpc::ConsumptionNominalMaxResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->ConsumptionNominalMax(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

bool CallSetConsumptionNominalMax(
    const std::shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub> stub,
    const cs_lpc::SetConsumptionNominalMaxRequest& request,
    cs_lpc::SetConsumptionNominalMaxResponse* response) {
  grpc::ClientContext context;
  bool result;
  std::mutex mu;
  std::condition_variable cv;
  bool done = false;
  stub->async()->SetConsumptionNominalMax(
    &context, &request, response,
    [&result, &mu, &cv, &done, response](grpc::Status status) {
      bool ret;      if (!status.ok()) {
        ret = false;
      } else {
        ret = true;
      }
      std::lock_guard<std::mutex> lock(mu);
      result = ret;
      done = true;
      cv.notify_one();
    });
  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [&done] { return done; });
  return result;
}

} // namespace cs_lpc

